#ifdef HAS_BLE_WRITER
// OpenDisplay BLE tags: session (authentication, AES-CCM envelope) and config read.
// Protocol notes: docs/design/opendisplay-ble.md
#include "od_session.h"

#include <FS.h>
#include <HTTPClient.h>

#include <algorithm>
#include <map>
#include <vector>

#include "BLEDevice.h"
#include "ble_filter.h"
#include "ble_writer.h"
#include "commstructs.h"
#include "esp_random.h"
#include "makeimage.h"
#include "miniz-oepl.h"
#include "newproto.h"
#include "mbedtls/aes.h"
#include "mbedtls/ccm.h"
#include "od_keys.h"
#include "storage.h"
#include "system.h"
#include "tag_db.h"
#include "udp.h"
#include "web.h"

#include <WiFi.h>

static BLEUUID odUUID("00002446-0000-1000-8000-00805F9B34FB");  // service and characteristic

#define OD_CMD_CONFIG_READ 0x0040
#define OD_CMD_FIRMWARE_VERSION 0x0043
#define OD_CMD_AUTHENTICATE 0x0050

#define OD_REPLY_TIMEOUT 3000
#define OD_CONNECT_TIMEOUT 45000  // connect, service, notify; then the link is dropped
#define OD_WRITE_TIMEOUT 10000
#define OD_RETRY_INTERVAL (5 * 60 * 1000)
#define OD_SEEN_RECENTLY 150  // seconds; the BLE scan runs every 60 s

enum OdState : uint8_t {
    OD_STATE_WAITING = 0,
    OD_STATE_NOT_ADDED,  // only listed; the AP doesn't connect until the user adds it
    OD_STATE_KEY_REJECTED,
    OD_STATE_KEY_NEEDED,  // added without a key, but the tag wants one
    OD_STATE_UNREACHABLE,
    OD_STATE_ERROR,
    OD_STATE_OK,
};
static const char* odStateNames[] = {"waiting", "not added", "key rejected", "key needed", "unreachable", "error", "ok"};

struct OdStatus {
    OdState state = OD_STATE_WAITING;
    uint32_t lastTry = 0;
    bool typeChecked = false;  // its type file was checked since boot
};

static std::map<uint64_t, OdConfig> odConfigs;
static std::map<uint64_t, OdStatus> odStatus;
static SemaphoreHandle_t odMutex = nullptr;

// Roaming: an AP alone on the network claims a tag right away. The AP list is only asked for
// at times, so odInit() asks for it once; other APs answer within seconds.
#define OD_APLIST_ANSWER_TIME (10 * 1000)
static uint32_t odApListAsked = 0;
extern UDPcomm udpsync;

struct OdMsg {
    uint16_t len;
    uint8_t data[260];
};
static QueueHandle_t odRxQueue = nullptr;

// one session at a time, owned by the BLE task
static BLEClient* odClient = nullptr;
static BLERemoteCharacteristic* odChar = nullptr;
static bool odEncrypted = false;
static uint8_t odSessionKey[16];
static uint8_t odSessionId[8];
static uint64_t odNonceCounter = 0;

static uint64_t odAddrKey(const uint8_t mac[8]) {
    uint64_t key = 0;
    for (int i = 5; i >= 0; i--) key = (key << 8) | mac[i];
    return key;
}

static String odAddrHex(const uint8_t mac[8]) {
    char name[13];
    snprintf(name, sizeof(name), "%02X%02X%02X%02X%02X%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
    return String(name);
}

static void odSetState(const uint8_t mac[8], OdState state) {
    xSemaphoreTake(odMutex, portMAX_DELAY);
    OdStatus& st = odStatus[odAddrKey(mac)];
    st.state = state;
    st.lastTry = millis();
    xSemaphoreGive(odMutex);
}

// ---------- crypto ----------

static void aesEcb(const uint8_t key[16], const uint8_t in[16], uint8_t out[16]) {
    mbedtls_aes_context c;
    mbedtls_aes_init(&c);
    mbedtls_aes_setkey_enc(&c, key, 128);
    mbedtls_aes_crypt_ecb(&c, MBEDTLS_AES_ENCRYPT, in, out);
    mbedtls_aes_free(&c);
}

static void gfDouble(uint8_t b[16]) {  // multiply by x in GF(2^128)
    const uint8_t carry = b[0] & 0x80;
    for (int i = 0; i < 15; i++) b[i] = (b[i] << 1) | (b[i + 1] >> 7);
    b[15] <<= 1;
    if (carry) b[15] ^= 0x87;
}

// AES-128-CMAC (RFC 4493); the mbedTLS of Arduino-ESP32 2.x has no CMAC
static void aesCmac(const uint8_t key[16], const uint8_t* msg, size_t len, uint8_t out[16]) {
    uint8_t k1[16] = {0}, k2[16], x[16] = {0}, blk[16];
    aesEcb(key, k1, k1);
    gfDouble(k1);
    memcpy(k2, k1, 16);
    gfDouble(k2);
    size_t n = (len + 15) / 16;
    const bool complete = len && (len % 16 == 0);
    if (n == 0) n = 1;
    for (size_t i = 0; i < n; i++) {
        const size_t off = i * 16;
        if (i < n - 1) {
            for (int j = 0; j < 16; j++) blk[j] = x[j] ^ msg[off + j];
        } else {
            uint8_t last[16] = {0};
            const size_t rem = len - off;
            memcpy(last, msg + off, rem);
            if (complete) {
                for (int j = 0; j < 16; j++) last[j] ^= k1[j];
            } else {
                last[rem] = 0x80;
                for (int j = 0; j < 16; j++) last[j] ^= k2[j];
            }
            for (int j = 0; j < 16; j++) blk[j] = x[j] ^ last[j];
        }
        aesEcb(key, blk, x);
    }
    memcpy(out, x, 16);
}

static bool aesCmacSelfTest() {  // RFC 4493 examples 1 and 2
    const uint8_t k[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
    const uint8_t m[16] = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a};
    const uint8_t e0[16] = {0xbb, 0x1d, 0x69, 0x29, 0xe9, 0x59, 0x37, 0x28, 0x7f, 0xa3, 0x7d, 0x12, 0x9b, 0x75, 0x67, 0x46};
    const uint8_t e16[16] = {0x07, 0x0a, 0x16, 0xb4, 0x6b, 0x4d, 0x41, 0x44, 0xf7, 0x9b, 0xdd, 0x9d, 0xd0, 0x4a, 0x28, 0x7c};
    uint8_t o[16];
    aesCmac(k, nullptr, 0, o);
    const bool ok = !memcmp(o, e0, 16);
    aesCmac(k, m, 16, o);
    return ok && !memcmp(o, e16, 16);
}

// [op:2][nonce:16][ciphertext][tag:12]; nonce = session id + counter, plaintext = [len:1][payload]
static size_t odEncrypt(const uint8_t* frame, size_t len, uint8_t* out) {
    uint8_t nonce[16];
    memcpy(nonce, odSessionId, 8);
    for (int i = 0; i < 8; i++) nonce[8 + i] = (odNonceCounter >> (56 - 8 * i)) & 0xff;
    odNonceCounter++;
    uint8_t plain[256];
    const size_t plainLen = len - 2 + 1;
    plain[0] = len - 2;
    memcpy(plain + 1, frame + 2, len - 2);
    out[0] = frame[0];
    out[1] = frame[1];
    memcpy(out + 2, nonce, 16);
    mbedtls_ccm_context c;
    mbedtls_ccm_init(&c);
    mbedtls_ccm_setkey(&c, MBEDTLS_CIPHER_ID_AES, odSessionKey, 128);
    mbedtls_ccm_encrypt_and_tag(&c, plainLen, nonce + 3, 13, out, 2, plain, out + 18, out + 18 + plainLen, 12);
    mbedtls_ccm_free(&c);
    return 18 + plainLen + 12;
}

static bool odDecrypt(OdMsg& m) {
    if (m.len < 31) return false;
    const size_t ctLen = m.len - 18 - 12;
    uint8_t plain[256];
    mbedtls_ccm_context c;
    mbedtls_ccm_init(&c);
    mbedtls_ccm_setkey(&c, MBEDTLS_CIPHER_ID_AES, odSessionKey, 128);
    const int r = mbedtls_ccm_auth_decrypt(&c, ctLen, m.data + 2 + 3, 13, m.data, 2, m.data + 18, plain, m.data + 18 + ctLen, 12);
    mbedtls_ccm_free(&c);
    if (r) return false;
    size_t payloadLen = plain[0];
    if (payloadLen > ctLen - 1) payloadLen = ctLen - 1;
    memcpy(m.data + 2, plain + 1, payloadLen);
    m.len = payloadLen + 2;
    return true;
}

// ---------- session ----------

static void odNotify(BLERemoteCharacteristic*, uint8_t* data, size_t len, bool) {
    OdMsg m;
    m.len = len > sizeof(m.data) ? sizeof(m.data) : len;
    memcpy(m.data, data, m.len);
    xQueueSend(odRxQueue, &m, 0);
}

static void odSend(uint16_t op, const uint8_t* payload = nullptr, size_t len = 0) {
    uint8_t frame[260];
    frame[0] = op >> 8;
    frame[1] = op & 0xff;
    if (len) memcpy(frame + 2, payload, len);
    BLE_guardStart(*odClient->getPeerAddress().getNative(), OD_WRITE_TIMEOUT);
    if (odEncrypted) {
        uint8_t enc[300];
        odChar->writeValue(enc, odEncrypt(frame, len + 2, enc), true);
    } else {
        odChar->writeValue(frame, len + 2, true);
    }
    BLE_guardStop();
}

// Replies are [op:2][status/data]. Inside a session, short replies (acks, FE, FF) stay
// plaintext and replies of 31 bytes or more are encrypted.
static bool odReceive(OdMsg& m, uint32_t timeout = OD_REPLY_TIMEOUT) {
    if (xQueueReceive(odRxQueue, &m, pdMS_TO_TICKS(timeout)) != pdTRUE) return false;
    if (odEncrypted && m.len >= 31 && !odDecrypt(m)) {
        Serial.println("OD reply failed to decrypt");
        return false;
    }
    return true;
}

static void odDisconnect() {
    if (odClient != nullptr && odClient->isConnected()) odClient->disconnect();
    odChar = nullptr;
    odEncrypted = false;
}

static bool odConnectSession(const uint8_t mac[8]) {
    // The previous client has disconnected by now; BLEDevice::createClient() never reuses them
    if (odClient != nullptr) {
        delete odClient;
        odClient = nullptr;
    }
    xQueueReset(odRxQueue);
    odEncrypted = false;
    esp_bd_addr_t addr = {mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]};
    odClient = BLEDevice::createClient();
    if (!odClient->connect(BLEAddress(addr), BLE_ADDR_TYPE_PUBLIC)) return false;
    odClient->setMTU(255);
    BLERemoteService* service = odClient->getService(odUUID);
    if (service != nullptr) odChar = service->getCharacteristic(odUUID);
    // registerForNotify() on a link that just dropped waits for an event that never comes
    if (odChar == nullptr || !odChar->canNotify() || !odClient->isConnected()) {
        odDisconnect();
        return false;
    }
    odChar->registerForNotify(odNotify);
    return true;
}

static bool odConnect(const uint8_t mac[8]) {
    const uint8_t peer[] = {mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]};
    BLE_guardStart(peer, OD_CONNECT_TIMEOUT);
    const bool connected = odConnectSession(mac);
    BLE_guardStop();
    return connected;
}

// Two round trips; afterwards every request and long reply goes through the CCM envelope
static OdState odAuthenticate(const uint8_t master[16]) {
    OdMsg m;
    const uint8_t start = 0x00;
    odSend(OD_CMD_AUTHENTICATE, &start, 1);
    if (!odReceive(m) || m.len < 19 || m.data[2] != 0x00) return OD_STATE_ERROR;
    uint8_t serverNonce[16], clientNonce[16], devId[4] = {0, 0, 0, 1};
    memcpy(serverNonce, m.data + 3, 16);
    if (m.len >= 23) memcpy(devId, m.data + 19, 4);
    esp_fill_random(clientNonce, 16);

    uint8_t in[36], mac[16], step2[32];
    memcpy(in, serverNonce, 16);
    memcpy(in + 16, clientNonce, 16);
    memcpy(in + 32, devId, 4);
    aesCmac(master, in, 36, mac);
    memcpy(step2, clientNonce, 16);
    memcpy(step2 + 16, mac, 16);
    odSend(OD_CMD_AUTHENTICATE, step2, 32);
    if (!odReceive(m) || m.len < 3) return OD_STATE_ERROR;
    if (m.data[2] != 0x00 || m.len < 19) return OD_STATE_KEY_REJECTED;

    // session key = AES(master, 00..01 || CMAC(master, "OpenDisplay session" 00 devId cN sN 00 80)[0:8])
    uint8_t kdf[19 + 1 + 4 + 16 + 16 + 2];
    size_t p = 0;
    memcpy(kdf, "OpenDisplay session", 19);
    p = 19;
    kdf[p++] = 0x00;
    memcpy(kdf + p, devId, 4);
    p += 4;
    memcpy(kdf + p, clientNonce, 16);
    p += 16;
    memcpy(kdf + p, serverNonce, 16);
    p += 16;
    kdf[p++] = 0x00;
    kdf[p++] = 0x80;
    uint8_t derived[16], blk[16] = {0, 0, 0, 0, 0, 0, 0, 1};
    aesCmac(master, kdf, p, derived);
    memcpy(blk + 8, derived, 8);
    aesEcb(master, blk, odSessionKey);

    // the tag proves it has the key too: CMAC(session key, sN || cN || devId)
    uint8_t proof[16];
    aesCmac(odSessionKey, in, 36, proof);
    if (memcmp(proof, m.data + 3, 16)) return OD_STATE_KEY_REJECTED;

    uint8_t ids[32], sid[16];
    memcpy(ids, clientNonce, 16);
    memcpy(ids + 16, serverNonce, 16);
    aesCmac(odSessionKey, ids, 32, sid);
    memcpy(odSessionId, sid, 8);
    odNonceCounter = 0;
    odEncrypted = true;
    return OD_STATE_OK;
}

// ---------- config ----------

// [len:2][version:1][packets][crc:2]; a packet is [instance][type][body of a fixed size per type].
// Wipes the encryption key in the security packet: the AP keeps keys in NVS only.
static bool odParseConfig(uint8_t* raw, size_t len, OdConfig& cfg, bool* keyWiped = nullptr) {
    static const struct {
        uint8_t type;
        uint16_t size;
    } sizes[] = {{0x01, 22}, {0x02, 22}, {0x04, 30}, {0x20, 46}, {0x21, 22}, {0x23, 30}, {0x24, 30}, {0x25, 30}, {0x26, 160}, {0x27, 64}, {0x28, 32}, {0x29, 32}, {0x2A, 32}, {0x2B, 32}, {0x2C, 288}};
    memset(&cfg, 0, sizeof(cfg));
    memset(cfg.buttonByteIndex, 0xFF, sizeof(cfg.buttonByteIndex));
    if (len < 5) return false;
    bool display = false;
    uint8_t* d = raw + 3;
    const size_t n = len - 5;
    size_t o = 0;
    while (o + 2 <= n) {
        const uint8_t type = d[o + 1];
        uint16_t size = 0;
        for (const auto& s : sizes)
            if (s.type == type) size = s.size;
        if (size == 0 || o + 2 + size > n) break;  // unknown type: its size is unknown too
        uint8_t* b = d + o + 2;
        if (type == 0x20 && !display) {  // first display only
            display = true;
            cfg.panelIc = b[2] | (b[3] << 8);
            cfg.width = b[4] | (b[5] << 8);
            cfg.height = b[6] | (b[7] << 8);
            cfg.widthMm = b[8] | (b[9] << 8);
            cfg.heightMm = b[10] | (b[11] << 8);
            cfg.legacyTagType = b[12] | (b[13] << 8);
            cfg.rotation = b[14];
            cfg.colorScheme = b[21];
            cfg.transmissionModes = b[22];
        } else if (type == 0x27) {
            cfg.encryption = b[0] != 0;
            for (int i = 1; i <= 16; i++) {
                if (b[i] && keyWiped) *keyWiped = true;
                b[i] = 0;
            }
        } else if (type == 0x25 && cfg.buttonBlocks < 4) {
            cfg.buttonByteIndex[cfg.buttonBlocks++] = b[15];
        } else if (type == 0x21) {
            cfg.hasLed = true;
        }
        o += 2 + size;
    }
    return display && cfg.width && cfg.height;
}

static String odConfigPath(const uint8_t mac[8]) {
    return "/odcfg/" + odAddrHex(mac) + ".bin";
}

static OdState odReadConfig(const uint8_t mac[8], OdConfig& cfg, uint16_t& fwVersion) {
    OdMsg m;
    fwVersion = 0;
    odSend(OD_CMD_FIRMWARE_VERSION);
    if (odReceive(m) && m.len >= 4 && m.data[1] == 0x43) fwVersion = (m.data[2] << 8) | m.data[3];

    static uint8_t raw[2048];
    size_t got = 0, total = 0;
    odSend(OD_CMD_CONFIG_READ);
    while (odReceive(m)) {
        if (m.len == 3 && m.data[1] == 0x40 && m.data[2] == 0xFE) return OD_STATE_KEY_NEEDED;  // auth required
        if (m.len < 4 || m.data[0] != 0x00 || m.data[1] != 0x40) return OD_STATE_ERROR;
        const uint16_t chunk = m.data[2] | (m.data[3] << 8);
        size_t off = 4;
        if (chunk == 0) {
            if (m.len < 6) return OD_STATE_ERROR;
            total = m.data[4] | (m.data[5] << 8);
            off = 6;
        }
        size_t n = m.len - off;
        if (got + n > sizeof(raw)) return OD_STATE_ERROR;
        memcpy(raw + got, m.data + off, n);
        got += n;
        if (total && got >= total) break;
    }
    if (!total || got < total || !odParseConfig(raw, total, cfg)) return OD_STATE_ERROR;

    xSemaphoreTake(fsMutex, portMAX_DELAY);
    contentFS->mkdir("/odcfg");
    File f = contentFS->open(odConfigPath(mac), "w");
    if (f) {
        f.write(raw, total);
        f.close();
    }
    xSemaphoreGive(fsMutex);
    return OD_STATE_OK;
}

static const char* odColorName(uint8_t scheme) {
    switch (scheme) {
        case 0: return "BW";
        case 1: return "BWR";
        case 2: return "BWY";
        case 3: return "BWRY";
        case 4: return "Spectra 6";
        case 5: return "gray 4";
        case 6: return "gray 16";
        case 7: return "7 color";
        default: return "unknown colors";
    }
}

static void odAssignType(tagRecord* taginfo, const OdConfig& cfg);

static void odSession(const uint8_t mac[8]) {
    const String name = "OD" + odAddrHex(mac).substring(6);
    uint8_t key[16];
    const bool haveKey = odKeyGet(mac, key);
    if (!haveKey && !odKeyAdded(mac)) {
        odSetState(mac, OD_STATE_NOT_ADDED);
        return;
    }
    Serial.println(name + ": connecting");
    BLE_scanStop();
    if (!odConnect(mac)) {
        odDisconnect();
        odSetState(mac, OD_STATE_UNREACHABLE);
        wsLog(name + ": could not connect");
        return;
    }
    const OdState auth = haveKey ? odAuthenticate(key) : OD_STATE_OK;
    memset(key, 0, sizeof(key));
    if (auth != OD_STATE_OK) {
        odDisconnect();
        odSetState(mac, auth);
        wsErr(name + (auth == OD_STATE_KEY_REJECTED ? ": the tag rejected the key" : ": authentication failed"));
        return;
    }
    OdConfig cfg;
    uint16_t fwVersion;
    const OdState read = odReadConfig(mac, cfg, fwVersion);
    odDisconnect();
    if (read != OD_STATE_OK) {
        odSetState(mac, read);
        wsErr(name + (read == OD_STATE_KEY_NEEDED ? ": the tag has encryption on, it needs its key" : ": could not read the config"));
        return;
    }
    xSemaphoreTake(odMutex, portMAX_DELAY);
    odConfigs[odAddrKey(mac)] = cfg;
    xSemaphoreGive(odMutex);
    odSetState(mac, OD_STATE_OK);

    tagRecord* taginfo = tagRecord::findByMAC(mac);
    if (taginfo != nullptr) {
        if (fwVersion) taginfo->tagSoftwareVersion = fwVersion;
        odAssignType(taginfo, cfg);
    }
    wsLog(name + ": " + String(cfg.width) + "x" + String(cfg.height) + " " + odColorName(cfg.colorScheme) + ", panel " + String(cfg.panelIc) +
          ", firmware " + String(fwVersion >> 8) + "." + String(fwVersion & 0xff));
    wsSendTaginfo(mac, SYNC_TAGSTATUS);
}

// ---------- tag type ----------

// Color order = the tag's pixel codes. BWRY codes are 0 black, 1 white, 2 yellow, 3 red,
// except panel 0x1D, which has red and yellow the other way round.
static bool odPalette(const OdConfig& cfg, std::vector<const char*>& names, uint8_t& bpp) {
    switch (cfg.colorScheme) {
        case 0: names = {"black", "white"}; bpp = 1; return true;
        case 1: names = {"black", "white", "red"}; bpp = 2; return true;
        case 2: names = {"black", "white", "yellow"}; bpp = 2; return true;
        case 3:
            names = cfg.panelIc == 0x1D ? std::vector<const char*>{"black", "white", "red", "yellow"} : std::vector<const char*>{"black", "white", "yellow", "red"};
            bpp = BPP_PACKED_2BIT;
            return true;
        default: return false;
    }
}

static Color odColor(const char* name, bool perceptual) {
    if (!strcmp(name, "white")) return {255, 255, 255};
    if (!strcmp(name, "red")) return {255, 0, 0};
    if (!strcmp(name, "yellow")) return perceptual ? Color{200, 200, 0} : Color{255, 255, 0};
    return {0, 0, 0};
}

// Content layouts of the repo type of the same size (landscape width x height); generated from
// resources/tagtypes: the first type with its own templates, else the one most of them use
struct OdTemplateSize {
    uint16_t width, height;
    uint8_t hwType;
};
static const OdTemplateSize odTemplates[] = {
    {128, 128, 0xE4}, {152, 152, 0x00}, {160, 80, 0xE1}, {200, 144, 0x43}, {200, 152, 0x67}, {200, 200, 0x30}, {212, 104, 0x03}, {250, 128, 0x45},
    {250, 136, 0x69}, {256, 128, 0x54}, {264, 136, 0x59}, {264, 176, 0xD0}, {296, 128, 0x01}, {296, 144, 0x01}, {296, 152, 0x04}, {296, 160, 0x31},
    {296, 168, 0x01}, {300, 200, 0x47}, {320, 172, 0xE0}, {360, 184, 0x01}, {384, 168, 0x33}, {384, 184, 0x60}, {400, 168, 0x01}, {400, 300, 0x02},
    {480, 176, 0x68}, {480, 224, 0x53}, {480, 480, 0xE2}, {522, 152, 0x2F}, {600, 448, 0x35}, {640, 384, 0x05}, {640, 400, 0xC1}, {648, 480, 0x50},
    {720, 256, 0x44}, {792, 272, 0x41}, {800, 480, 0x36}, {960, 640, 0x37}, {960, 672, 0x2E}, {960, 768, 0x2D}, {1024, 576, 0x38}, {1360, 480, 0xE3}};

static uint8_t odTemplateFor(uint16_t width, uint16_t height) {
    const OdTemplateSize* best = &odTemplates[0];
    uint32_t bestDiff = UINT32_MAX;
    for (const auto& t : odTemplates) {
        const uint32_t diff = abs((int)t.width - width) + abs((int)t.height - height);
        if (diff < bestDiff) {
            best = &t;
            bestDiff = diff;
        }
    }
    return best->hwType;
}

static String odTypePath(uint16_t id) {
    char path[24];
    snprintf(path, sizeof(path), "/tagtypes/%04X.json", id);
    return String(path);
}

// Bump when the generated files change, so older ones are written again
#define OD_TYPE_GENERATION 3

enum OdTypeMatch { OD_TYPE_OTHER, OD_TYPE_OUTDATED, OD_TYPE_CURRENT };

// Does /tagtypes/<id>.json describe this display, and is it from this firmware's generation?
static OdTypeMatch odTypeMatches(uint16_t id, const OdConfig& cfg) {
    File f = contentFS->open(odTypePath(id), "r");
    if (!f) return OD_TYPE_OTHER;
    JsonDocument filter, doc;
    filter["opendisplay"] = true;
    const bool parsed = !deserializeJson(doc, f, DeserializationOption::Filter(filter));
    f.close();
    JsonObject od = doc["opendisplay"];
    if (!parsed || od["width"] != cfg.width || od["height"] != cfg.height || od["colors"] != cfg.colorScheme || od["panel"] != cfg.panelIc) return OD_TYPE_OTHER;
    return od["generation"] == OD_TYPE_GENERATION ? OD_TYPE_CURRENT : OD_TYPE_OUTDATED;
}

// The content list of the template type, without what is for OEPL tag firmware only (firmware
// update, set tag config, image preload, set tag mac). Fetches the template type from the repo
// when the AP doesn't have it yet, as the web UI would.
static bool odTemplateContent(uint8_t templateType, JsonArray contentids) {
    char path[20];
    snprintf(path, sizeof(path), "/tagtypes/%02X.json", templateType);
    if (!contentFS->exists(path)) {
        HTTPClient http;
        http.begin("https://raw.githubusercontent.com/" + config.repo + "/master/resources/tagtypes/" + String(path + 10));
        http.setTimeout(5000);
        if (http.GET() == 200) {
            LockedFile f(path, "w");
            if (f) {
                http.writeToStream(&f);
                f.close();
            }
        }
        http.end();
        forgetHwType(templateType);
    }
    File f = contentFS->open(path, "r");
    if (!f) return false;
    JsonDocument filter, doc;
    filter["contentids"] = true;
    const bool parsed = !deserializeJson(doc, f, DeserializationOption::Filter(filter));
    f.close();
    if (!parsed || !doc["contentids"].is<JsonArray>()) return false;
    for (JsonVariant id : doc["contentids"].as<JsonArray>()) {
        const int content = id.as<int>();
        if (content != 5 && content != 18 && content != 23 && content != 28) contentids.add(content);
    }
    return contentids.size() > 0;
}

// The AP's own type for a display without legacy_tag_type: one per distinct display, 0x0101 and up
static uint16_t odGeneratedType(const OdConfig& cfg) {
    std::vector<const char*> names;
    uint8_t bpp;
    if (!odPalette(cfg, names, bpp)) return 0;
    uint16_t writeId = 0;
    for (uint16_t id = HWTYPE_OPENDISPLAY_UNKNOWN + 1; id <= 0x01FF; id++) {
        const OdTypeMatch match = odTypeMatches(id, cfg);
        if (match == OD_TYPE_CURRENT) return id;
        if (match == OD_TYPE_OUTDATED) {  // same display, older file: write it again under its id
            writeId = id;
            break;
        }
        if (!writeId && !contentFS->exists(odTypePath(id))) writeId = id;
    }
    if (!writeId) return 0;

    // content in landscape; rotatebuffer turns it onto a portrait panel
    const bool portrait = cfg.width < cfg.height;
    const uint16_t width = portrait ? cfg.height : cfg.width;
    const uint16_t height = portrait ? cfg.width : cfg.height;
    JsonDocument doc;
    doc["version"] = 1;
    char name[64];
    const float inch = sqrtf((float)cfg.widthMm * cfg.widthMm + (float)cfg.heightMm * cfg.heightMm) / 25.4f;
    snprintf(name, sizeof(name), "OpenDisplay %.1f\" %ux%u %s", inch, width, height, odColorName(cfg.colorScheme));
    doc["name"] = name;
    doc["width"] = width;
    doc["height"] = height;
    doc["rotatebuffer"] = portrait ? 1 : 0;  // 1, not 3: checked on a 168x384 panel
    doc["bpp"] = bpp;
    JsonObject colortable = doc["colortable"].to<JsonObject>();
    JsonObject perceptual = doc["perceptual"].to<JsonObject>();
    for (const char* c : names) {
        const Color col = odColor(c, false), per = odColor(c, true);
        JsonArray a = colortable[c].to<JsonArray>();
        a.add(col.r), a.add(col.g), a.add(col.b);
        JsonArray b = perceptual[c].to<JsonArray>();
        b.add(per.r), b.add(per.g), b.add(per.b);
    }
    if (cfg.colorScheme == 2) doc["highlight_color"] = 3;  // yellow
    doc["shortlut"] = 0;
    const uint8_t templateType = odTemplateFor(width, height);
    doc["usetemplate"] = templateType;
    const bool haveContent = odTemplateContent(templateType, doc["contentids"].to<JsonArray>());
    if (!haveContent) doc.remove("contentids");  // the web UI then takes the template type's list
    if (cfg.hasLed) doc["options"].to<JsonArray>().add("led");
    JsonObject od = doc["opendisplay"].to<JsonObject>();
    od["width"] = cfg.width;
    od["height"] = cfg.height;
    od["colors"] = cfg.colorScheme;
    od["panel"] = cfg.panelIc;
    od["modes"] = cfg.transmissionModes;
    if (haveContent) od["generation"] = OD_TYPE_GENERATION;  // else written again after the next boot

    xSemaphoreTake(fsMutex, portMAX_DELAY);
    File f = contentFS->open(odTypePath(writeId), "w");
    const bool written = f;
    if (f) {
        serializeJsonPretty(doc, f);
        f.close();
    }
    xSemaphoreGive(fsMutex);
    if (!written) return 0;
    forgetHwType(writeId);
    wsLog("Tag type " + odTypePath(writeId).substring(10, 14) + " written: " + name);
    return writeId;
}

// Give an OpenDisplay tag with a known config its type: legacy_tag_type, else the AP's own
static void odAssignType(tagRecord* taginfo, const OdConfig& cfg) {
    uint16_t id = 0;
    if (cfg.legacyTagType > 0 && cfg.legacyTagType <= 0xFF) {
        id = cfg.legacyTagType;
    } else {
        id = taginfo->hwType;
        // keep the type it has as long as its file still describes this display
        if (id <= HWTYPE_OPENDISPLAY_UNKNOWN || odTypeMatches(id, cfg) != OD_TYPE_CURRENT) id = odGeneratedType(cfg);
    }
    if (id && id != taginfo->hwType) {
        taginfo->hwType = id;
        wsSendTaginfo(taginfo->mac, SYNC_TAGSTATUS);
    }
}

// ---------- upload ----------

#define OD_CMD_DIRECT_WRITE_START 0x0070
#define OD_CMD_DIRECT_WRITE_DATA 0x0071
#define OD_CMD_DIRECT_WRITE_END 0x0072
#define OD_REPLY_REFRESHED 0x73
#define OD_REPLY_REFRESH_TIMEOUT 0x74

// The rendered buffer has its pixels back to back; the tag wants every row to start on a byte.
// Two-plane schemes (BWR/BWY) get an empty second plane when the image has no red or yellow.
static uint8_t* odWireImage(const OdConfig& cfg, const uint8_t* data, size_t len, size_t& outLen) {
    std::vector<const char*> names;
    uint8_t bpp;
    if (!odPalette(cfg, names, bpp)) return nullptr;
    const uint8_t bits = bpp == BPP_PACKED_2BIT ? 2 : 1;
    const uint8_t planes = bpp == 2 ? 2 : 1;
    const size_t packedPlane = ((size_t)cfg.width * cfg.height * bits + 7) / 8;
    const size_t rowBytes = ((size_t)cfg.width * bits + 7) / 8;
    if (len != packedPlane && len != packedPlane * planes) return nullptr;
    outLen = rowBytes * cfg.height * planes;
    uint8_t* out = (uint8_t*)ps_calloc(outLen, 1);
    if (!out) return nullptr;
    for (uint8_t p = 0; p < planes; p++) {
        if ((p + 1) * packedPlane > len) break;  // no second plane: stays 0
        const uint8_t* src = data + p * packedPlane;
        uint8_t* dst = out + p * rowBytes * cfg.height;
        if (rowBytes * 8 == (size_t)cfg.width * bits) {
            memcpy(dst, src, packedPlane);
            continue;
        }
        for (size_t y = 0; y < cfg.height; y++)
            for (size_t x = 0; x < (size_t)cfg.width * bits; x++) {
                const size_t s = y * cfg.width * bits + x;
                if (src[s / 8] & (0x80 >> (s % 8))) dst[y * rowBytes + x / 8] |= 0x80 >> (x % 8);
            }
    }
    return out;
}

// zlib with the 512-byte window the tag inflates with
static uint8_t* odCompress(const uint8_t* data, size_t len, size_t& outLen) {
    Miniz::tdefl_compressor* comp = (Miniz::tdefl_compressor*)ps_malloc(sizeof(Miniz::tdefl_compressor));
    outLen = len + 1024;
    uint8_t* out = (uint8_t*)ps_malloc(outLen);
    bool ok = comp && out && Miniz::tdefl_initOEPL(comp, NULL, NULL, Miniz::TDEFL_WRITE_ZLIB_HEADER | 1500, 512) == Miniz::TDEFL_STATUS_OKAY;
    if (ok) {
        size_t inLen = len;
        ok = Miniz::tdefl_compressOEPL(comp, data, &inLen, out, &outLen, Miniz::TDEFL_FINISH) == Miniz::TDEFL_STATUS_DONE;
    }
    free(comp);
    if (!ok) {
        free(out);
        return nullptr;
    }
    return out;
}

static bool odAck(uint8_t op, uint32_t timeout = OD_REPLY_TIMEOUT) {
    OdMsg m;
    if (!odReceive(m, timeout)) return false;
    return m.len >= 2 && m.data[0] == 0x00 && m.data[1] == op && (m.len < 3 || m.data[2] == 0x00);
}

// START/DATA/END, an ack per frame, then the tag reports the refresh
static bool odSendImage(const uint8_t* image, size_t imageLen, const uint8_t* zlib, size_t zlibLen) {
    const size_t chunk = odEncrypted ? 154 : 230;
    const uint8_t* data = zlib ? zlib : image;
    const size_t len = zlib ? zlibLen : imageLen;
    size_t sent = 0;
    if (zlib) {
        uint8_t start[256];
        for (int i = 0; i < 4; i++) start[i] = (imageLen >> (8 * i)) & 0xff;
        sent = std::min(len, chunk - 4);
        memcpy(start + 4, data, sent);
        odSend(OD_CMD_DIRECT_WRITE_START, start, 4 + sent);
    } else {
        odSend(OD_CMD_DIRECT_WRITE_START);
    }
    if (!odAck(0x70, 10000)) return false;
    while (sent < len) {
        const size_t n = std::min(chunk, len - sent);
        odSend(OD_CMD_DIRECT_WRITE_DATA, data + sent, n);
        if (!odAck(0x71, 5000)) return false;
        sent += n;
    }
    const uint8_t fullRefresh = 0;
    odSend(OD_CMD_DIRECT_WRITE_END, &fullRefresh, 1);
    OdMsg m;
    while (odReceive(m, 60000)) {
        if (m.len >= 2 && m.data[0] == 0x00 && m.data[1] == OD_REPLY_REFRESHED) return true;
        if (m.len >= 2 && m.data[0] == 0x00 && m.data[1] == OD_REPLY_REFRESH_TIMEOUT) {
            Serial.println("OD: refresh timeout reported by the tag");
            return true;  // the image arrived; the panel took too long
        }
        if (m.len >= 3 && m.data[1] == 0x72 && m.data[2] != 0x00) return false;
    }
    return false;
}

// ---------- commands ----------

#define OD_CMD_REBOOT 0x000F
#define OD_CMD_DEEP_SLEEP 0x0053
#define OD_CMD_LED_ACTIVATE 0x0073
#define OD_DEEPSLEEP_CHECKIN 3216153600  // "in deep sleep" until the tag is seen again

// OEPL commands that have an OpenDisplay equivalent; the rest is dropped
static bool odCommand(const uint8_t mac[8], const AvailDataInfo& adi) {
    const String name = "OD" + odAddrHex(mac).substring(6);
    uint16_t op = 0;
    uint8_t payload[13];
    size_t len = 0;
    switch (adi.dataTypeArgument) {
        case CMD_DO_REBOOT:
            op = OD_CMD_REBOOT;
            break;
        case CMD_DO_DEEPSLEEP:
            op = OD_CMD_DEEP_SLEEP;
            break;
        case CMD_DO_LEDFLASH:
            // the 12-byte flash pattern has the same layout in both; OEPL keeps it in dataVer + dataSize
            op = OD_CMD_LED_ACTIVATE;
            payload[0] = 0;  // first LED
            memcpy(payload + 1, &adi.dataVer, 8);
            memcpy(payload + 9, &adi.dataSize, 4);
            len = 13;
            break;
        default:
            wsLog(name + ": command " + String(adi.dataTypeArgument) + " has no OpenDisplay equivalent, dropped");
            return true;
    }

    uint8_t key[16];
    const bool haveKey = odKeyGet(mac, key);
    bool ok = odConnect(mac);
    if (ok && haveKey) ok = odAuthenticate(key) == OD_STATE_OK;
    memset(key, 0, sizeof(key));
    if (ok) {
        odSend(op, payload, len);
        // reboot and deep sleep drop the link instead of answering; having sent them is enough
        if (op == OD_CMD_LED_ACTIVATE) ok = odAck(0x73);
    }
    odDisconnect();
    if (!ok) return false;
    wsLog(name + (op == OD_CMD_REBOOT ? ": reboot sent" : op == OD_CMD_DEEP_SLEEP ? ": deep sleep sent" : ": LED flash sent"));
    if (op == OD_CMD_DEEP_SLEEP) {
        tagRecord* taginfo = tagRecord::findByMAC(mac);
        if (taginfo != nullptr) {
            taginfo->expectedNextCheckin = OD_DEEPSLEEP_CHECKIN;
            wsSendTaginfo(mac, SYNC_TAGSTATUS);
        }
    }
    return true;
}

// ---------- buttons ----------

// Button bytes in the advertisement are id:3 | press count:4 | pressed:1; a new press count
// is a press. Which bytes are buttons is in the config. Runs in the BLE host task.
uint8_t odButtonEvent(const uint8_t mac[8], const uint8_t dynamic[11]) {
    static std::map<uint64_t, uint8_t> lastSeen;
    OdConfig cfg;
    if (!odGetConfig(mac, cfg)) return 0;
    uint8_t reason = 0;
    for (uint8_t i = 0; i < cfg.buttonBlocks; i++) {
        const uint8_t index = cfg.buttonByteIndex[i];
        if (index > 10) continue;
        const uint8_t b = dynamic[index];
        const uint64_t key = (odAddrKey(mac) << 4) | index;
        auto it = lastSeen.find(key);
        if (it != lastSeen.end() && ((it->second ^ b) & 0x78)) {
            const uint8_t id = b & 0x07;
            reason = id == 0 ? WAKEUP_REASON_BUTTON1 : id == 1 ? WAKEUP_REASON_BUTTON2 : WAKEUP_REASON_BUTTON3;
            Serial.printf("OD button %u pressed\r\n", id);
        }
        lastSeen[key] = b;
    }
    return reason;
}

// ---------- public ----------

void odImageParams(const uint8_t mac[8], imgParam& imageParams) {
    OdConfig cfg;
    std::vector<const char*> names;
    uint8_t bpp;
    if (!odGetConfig(mac, cfg) || !odPalette(cfg, names, bpp)) return;
    imageParams.bpp = bpp;
    imageParams.hwdata.bpp = bpp;
    imageParams.hwdata.colortable.clear();
    for (const char* c : names) imageParams.hwdata.colortable.push_back(odColor(c, false));
    imageParams.zlib = 0;
    imageParams.g5 = 0;
}

bool odUpload(const uint8_t mac[8]) {
    const String name = "OD" + odAddrHex(mac).substring(6);
    OdConfig cfg;
    PendingItem* queueItem = getQueueItem(mac, 0);
    if (queueItem == nullptr) return false;
    if (queueItem->pendingdata.availdatainfo.dataType == DATATYPE_COMMAND_DATA) return odCommand(mac, queueItem->pendingdata.availdatainfo);
    if (!odGetConfig(mac, cfg) || !loadQueueItemData(queueItem)) return false;
    size_t imageLen = 0, zlibLen = 0;
    uint8_t* image = odWireImage(cfg, queueItem->data, queueItem->len, imageLen);
    if (image == nullptr) {
        wsErr(name + ": image doesn't fit the display (" + String(queueItem->len) + " bytes)");
        return true;  // would never fit, drop it
    }
    uint8_t* zlib = (cfg.transmissionModes & 0x03) ? odCompress(image, imageLen, zlibLen) : nullptr;

    uint8_t key[16];
    const bool haveKey = odKeyGet(mac, key);
    bool ok = odConnect(mac);
    if (ok && haveKey) ok = odAuthenticate(key) == OD_STATE_OK;
    memset(key, 0, sizeof(key));
    const uint32_t t = millis();
    if (ok) ok = odSendImage(image, imageLen, zlib, zlibLen);
    odDisconnect();
    if (ok) wsLog(name + ": image sent (" + String(zlib ? zlibLen : imageLen) + " bytes) and shown, " + String((millis() - t) / 1000) + " s");
    free(image);
    free(zlib);
    return ok;
}


void odInit() {
    odMutex = xSemaphoreCreateMutex();
    odRxQueue = xQueueCreate(16, sizeof(OdMsg));
    odApListAsked = millis();
    udpsync.getAPList();
    if (!aesCmacSelfTest()) Serial.println("OD: AES-CMAC self-test FAILED");

    File dir = contentFS->open("/odcfg");
    if (!dir) return;
    File file = dir.openNextFile();
    static uint8_t raw[2048];
    while (file) {
        const String base = String(file.name()).substring(0, 12);
        uint8_t mac[8] = {0};
        OdConfig cfg;
        const size_t len = file.read(raw, sizeof(raw));
        file.close();
        bool keyWiped = false;
        if (sscanf(base.c_str(), "%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX", &mac[5], &mac[4], &mac[3], &mac[2], &mac[1], &mac[0]) == 6 &&
            odParseConfig(raw, len, cfg, &keyWiped)) {
            odConfigs[odAddrKey(mac)] = cfg;
            odStatus[odAddrKey(mac)].state = OD_STATE_OK;
            if (keyWiped) {  // written by an earlier build that kept the key in the file
                File f = contentFS->open(odConfigPath(mac), "w");
                if (f) f.write(raw, len);
                f.close();
            }
        }
        file = dir.openNextFile();
    }
}

void odService() {
    static uint32_t lastService = 0;
    if (odMutex == nullptr || millis() - lastService < 10000) return;
    lastService = millis();

    time_t now;
    time(&now);
    uint8_t mac[8];
    bool found = false;
    for (tagRecord* taginfo : tagDBSnapshot()) {
        // also when another AP has it now: this AP needs the config to take the tag over
        if (bleFamily(taginfo->mac, taginfo->hwType) != BLE_FAMILY_OPENDISPLAY) continue;
        if (now - taginfo->lastseen > OD_SEEN_RECENTLY) continue;  // don't wait for a tag that isn't there
        xSemaphoreTake(odMutex, portMAX_DELAY);
        const uint64_t key = odAddrKey(taginfo->mac);
        const bool haveConfig = odConfigs.count(key) > 0;
        const OdStatus st = odStatus[key];
        xSemaphoreGive(odMutex);
        if (haveConfig) {
            // its type: once after a boot (an older firmware may have written its file), and
            // when the tag type file was cleaned up
            if (!st.typeChecked || taginfo->hwType == HWTYPE_OPENDISPLAY_UNKNOWN ||
                (taginfo->hwType > HWTYPE_OPENDISPLAY_UNKNOWN && !contentFS->exists(odTypePath(taginfo->hwType)))) {
                OdConfig cfg;
                if (odGetConfig(taginfo->mac, cfg)) odAssignType(taginfo, cfg);
                xSemaphoreTake(odMutex, portMAX_DELAY);
                odStatus[key].typeChecked = true;
                xSemaphoreGive(odMutex);
            }
            continue;
        }
        // these wait until the user changes the key
        if (st.state == OD_STATE_KEY_REJECTED || st.state == OD_STATE_KEY_NEEDED) continue;
        if (st.lastTry && millis() - st.lastTry < OD_RETRY_INTERVAL) continue;
        // only tags the user added (with or without a key): the AP leaves the others alone
        if (!odKeyAdded(taginfo->mac)) {
            if (st.state != OD_STATE_NOT_ADDED) odSetState(taginfo->mac, OD_STATE_NOT_ADDED);
            continue;
        }
        memcpy(mac, taginfo->mac, 8);
        found = true;
        break;
    }
    if (found) odSession(mac);
}

bool odGetConfig(const uint8_t mac[8], OdConfig& cfg) {
    if (odMutex == nullptr) return false;
    xSemaphoreTake(odMutex, portMAX_DELAY);
    auto it = odConfigs.find(odAddrKey(mac));
    const bool found = it != odConfigs.end();
    if (found) cfg = it->second;
    xSemaphoreGive(odMutex);
    return found;
}

void odKeyChanged(const uint8_t mac[8]) {
    if (odMutex == nullptr) return;
    xSemaphoreTake(odMutex, portMAX_DELAY);
    odStatus.erase(odAddrKey(mac));
    xSemaphoreGive(odMutex);
}

// ---------- roaming ----------
//
// A tag added on several APs belongs to one of them, the owner, which talks to it and
// forwards its check-ins. Another AP takes it over when it sees the tag and has heard
// nothing from an owner for a while; it doesn't need the old owner's consent, which may be
// gone. One rule for every AP:
// - you stay owner while you see the tag yourself; after OD_ROAM_WAIT without, you aren't;
// - not owner and you see the tag: wait OD_ROAM_WAIT. A check-in forwarded by another AP in
//   that time means it has the tag: start waiting again. Silence all the way: you are owner.
// A live owner forwards a check-in every minute, so a newcomer or an AP coming back never
// takes over from it. After a boot the wait is shorter: one forward interval is enough to
// hear an owner. Two APs that claim at the same moment: the lowest IP address keeps it.

#define OD_ROAM_WAIT (5 * 60 * 1000)
#define OD_ROAM_WAIT_AFTER_BOOT (90 * 1000)
#define OD_ROAM_TIE_WINDOW (2 * 60 * 1000)

struct OdRoam {
    bool owner = false;
    uint32_t lastLocal = 0;   // last time this AP saw the tag
    uint32_t lastRemote = 0;  // last check-in forwarded by another AP
    uint32_t claimStart = 0;  // waiting since, 0 = not waiting
    uint32_t claimedAt = 0;
};
static std::map<uint64_t, OdRoam> odRoam;

bool odLocalCheckin(const uint8_t mac[8]) {
    if (odMutex == nullptr) return true;
    const uint32_t now = millis();
    xSemaphoreTake(odMutex, portMAX_DELAY);
    OdRoam& r = odRoam[odAddrKey(mac)];
    if (r.owner && now - r.lastLocal > OD_ROAM_WAIT) r.owner = false;  // lost it for too long
    r.lastLocal = now;
    if (!r.owner) {
        if (r.claimStart == 0) r.claimStart = now;
        uint32_t wait = r.lastRemote == 0 && r.claimedAt == 0 ? OD_ROAM_WAIT_AFTER_BOOT : OD_ROAM_WAIT;
        // no other AP answered the AP-list request at start: nobody to wait for
        if (udpsync.peerCount() == 0 && now - odApListAsked > OD_APLIST_ANSWER_TIME) wait = 0;
        if (now - r.claimStart >= wait) {
            r.owner = true;
            r.claimedAt = now;
            r.claimStart = 0;
            Serial.printf("OD %s: this AP has the tag now\r\n", odAddrHex(mac).c_str());
        }
    }
    const bool owner = r.owner;
    xSemaphoreGive(odMutex);
    return owner;
}

bool odRemoteCheckin(const uint8_t mac[8], IPAddress from) {
    if (odMutex == nullptr) return true;
    const uint32_t now = millis();
    xSemaphoreTake(odMutex, portMAX_DELAY);
    OdRoam& r = odRoam[odAddrKey(mac)];
    r.lastRemote = now;
    if (r.owner && now - r.lastLocal > OD_ROAM_WAIT) r.owner = false;
    bool accept = !r.owner;
    // both claimed at the same moment: the lowest address keeps it
    if (r.owner && now - r.claimedAt < OD_ROAM_TIE_WINDOW && (uint32_t)from < (uint32_t)WiFi.localIP()) {
        r.owner = false;
        accept = true;
    }
    if (accept) r.claimStart = now;  // another AP has it: wait again from here
    xSemaphoreGive(odMutex);
    return accept;
}

bool odIsOwner(const uint8_t mac[8]) {
    if (odMutex == nullptr) return false;
    xSemaphoreTake(odMutex, portMAX_DELAY);
    auto it = odRoam.find(odAddrKey(mac));
    const bool owner = it != odRoam.end() && it->second.owner && millis() - it->second.lastLocal <= OD_ROAM_WAIT;
    xSemaphoreGive(odMutex);
    return owner;
}

void odForget(const uint8_t mac[8]) {
    if (odMutex == nullptr) return;
    xSemaphoreTake(odMutex, portMAX_DELAY);
    odStatus.erase(odAddrKey(mac));
    odConfigs.erase(odAddrKey(mac));
    xSemaphoreGive(odMutex);
    xSemaphoreTake(fsMutex, portMAX_DELAY);
    contentFS->remove(odConfigPath(mac));
    xSemaphoreGive(fsMutex);
    tagRecord* taginfo = tagRecord::findByMAC(mac);
    // its type too, unless another AP has the tag now: then that AP's type stands
    if (taginfo != nullptr && !taginfo->isExternal) taginfo->hwType = HWTYPE_OPENDISPLAY_UNKNOWN;
    odRoam.erase(odAddrKey(mac));
}

void odDescribe(const uint8_t mac[8], JsonObject od) {
    if (odMutex == nullptr) return;
    xSemaphoreTake(odMutex, portMAX_DELAY);
    const uint64_t key = odAddrKey(mac);
    auto st = odStatus.find(key);
    od["state"] = odStateNames[st == odStatus.end() ? OD_STATE_WAITING : st->second.state];
    auto it = odConfigs.find(key);
    if (it != odConfigs.end()) {
        const OdConfig& cfg = it->second;
        od["width"] = cfg.width;
        od["height"] = cfg.height;
        od["colors"] = odColorName(cfg.colorScheme);
        od["panel"] = cfg.panelIc;
        od["legacy"] = cfg.legacyTagType;
        od["encryption"] = cfg.encryption;
    }
    xSemaphoreGive(odMutex);
}
#endif
