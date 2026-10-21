#include "tlsr_flasher.h"

#ifdef FLASHER_AP_SWS

#include <ArduinoJson.h>
#include <FS.h>
#include <HardwareSerial.h>

#include "espflasher.h"
#include "tag_db.h"
#include "tlsr_floader.h"
#include "serialap.h"
#include "storage.h"
#include "web.h"

#define SWS_BAUD 230400
#define FLOADER_ADDR 0x40000

#define CMD_VER 0x00
#define CMD_RBF 0x01
#define CMD_WBF 0x02
#define CMD_EFS 0x03
#define CMD_JDC 0x05

#define FLASH_SECTOR 4096
#define WRITE_CHUNK 1024

static void tlsrLog(const String &m) {
    wsSerial(m);
    printf("%s\r\n", m.c_str());
}

/* CRC-16, polynomial 0xA001, initial value 0xFFFF. Computed rather than
   tabulated - the table in the reference tool is 512 bytes for no gain. */
static uint16_t crc16(const uint8_t *d, size_t n) {
    uint16_t crc = 0xFFFF;
    while (n--) {
        crc ^= *d++;
        for (int i = 0; i < 8; i++) crc = (crc & 1) ? (uint16_t)((crc >> 1) ^ 0xA001) : (uint16_t)(crc >> 1);
    }
    return crc;
}

/* --- SWS, carried by the UART transmitter ---------------------------------
 *
 * One SWS byte becomes five UART bytes. The first byte of a frame starts from
 * 0xE8, the start flag; every following one from 0xEF. Each data bit clears
 * part of one of them.
 */
static void swsEncByte(uint8_t v, bool first, uint8_t *out) {
    out[0] = first ? 0xE8 : 0xEF;
    out[1] = out[2] = out[3] = out[4] = 0xEF;
    if (v & 0x80) out[0] &= 0x0F;
    if (v & 0x40) out[1] &= 0xE8;
    if (v & 0x20) out[1] &= 0x0F;
    if (v & 0x10) out[2] &= 0xE8;
    if (v & 0x08) out[2] &= 0x0F;
    if (v & 0x04) out[3] &= 0xE8;
    if (v & 0x02) out[3] &= 0x0F;
    if (v & 0x01) out[4] &= 0xE8;
}

static void swsEnd() {
    uint8_t enc[5];
    swsEncByte(0xFF, true, enc);
    Serial1.write(enc, 5);
}

static void swsWrite(uint32_t addr, const uint8_t *data, size_t n) {
    const uint8_t hdr[5] = {0x5A, (uint8_t)(addr >> 16), (uint8_t)(addr >> 8), (uint8_t)addr, 0x00};
    uint8_t enc[5];
    for (int i = 0; i < 5; i++) {
        swsEncByte(hdr[i], i == 0, enc);
        Serial1.write(enc, 5);
    }
    for (size_t i = 0; i < n; i++) {
        swsEncByte(data[i], false, enc);
        Serial1.write(enc, 5);
    }
    swsEnd();
    Serial1.flush();
}

static void swsWrite1(uint32_t addr, uint8_t v) { swsWrite(addr, &v, 1); }

/* --- the UART protocol of the loader -------------------------------------- */

static bool floaderCmd(const uint8_t *body, size_t bodyLen, uint8_t *reply, size_t replyLen) {
    static uint8_t frame[WRITE_CHUNK + 8];
    if (bodyLen + 2 > sizeof(frame)) return false;
    memcpy(frame, body, bodyLen);
    const uint16_t c = crc16(body, bodyLen);
    frame[bodyLen] = (uint8_t)c;
    frame[bodyLen + 1] = (uint8_t)(c >> 8);

    while (Serial1.available()) Serial1.read();
    Serial1.write(frame, bodyLen + 2);
    Serial1.flush();

    size_t got = 0;
    const uint32_t deadline = millis() + 2000;
    while (got < replyLen && (int32_t)(millis() - deadline) < 0) {
        if (Serial1.available())
            reply[got++] = (uint8_t)Serial1.read();
        else
            vTaskDelay(1);
    }
    if (got != replyLen) return false;
    return crc16(reply, replyLen - 2) == (uint16_t)(reply[replyLen - 2] | (reply[replyLen - 1] << 8));
}

static bool floaderVersion(uint16_t &chipId, uint8_t &ver) {
    const uint8_t req[4] = {CMD_VER, 0, 0, 0};
    uint8_t rep[6];
    if (!floaderCmd(req, 4, rep, 6) || rep[0] != CMD_VER) return false;
    ver = rep[1];
    chipId = (uint16_t)(rep[2] | (rep[3] << 8));
    return true;
}

static bool floaderJedec(uint32_t &jedec) {
    const uint8_t req[4] = {CMD_JDC, 0, 0, 0};
    uint8_t rep[6];
    if (!floaderCmd(req, 4, rep, 6) || rep[0] != CMD_JDC) return false;
    jedec = ((uint32_t)rep[1] << 16) | ((uint32_t)rep[2] << 8) | rep[3];
    return true;
}

/* The address goes out as low byte, then the remaining sixteen bits as a
   little endian word - that is what the loader expects. */
static void putAddr(uint8_t *p, uint32_t off) {
    p[0] = (uint8_t)(off & 0xff);
    p[1] = (uint8_t)((off >> 8) & 0xff);
    p[2] = (uint8_t)((off >> 16) & 0xff);
}

static bool floaderEraseSector(uint32_t off) {
    uint8_t req[4] = {CMD_EFS, 0, 0, 0};
    putAddr(req + 1, off & ~(uint32_t)(FLASH_SECTOR - 1));
    uint8_t rep[6];
    return floaderCmd(req, 4, rep, 6) && rep[0] == CMD_EFS;
}

static bool floaderWrite(uint32_t off, const uint8_t *data, size_t n) {
    static uint8_t req[WRITE_CHUNK + 4];
    req[0] = CMD_WBF;
    putAddr(req + 1, off);
    memcpy(req + 4, data, n);
    uint8_t rep[6];
    return floaderCmd(req, n + 4, rep, 6) && rep[0] == CMD_WBF;
}

static bool floaderRead(uint32_t off, uint8_t *out, size_t n) {
    uint8_t req[6] = {CMD_RBF, 0, 0, 0, 0, 0};
    putAddr(req + 1, off);
    req[4] = (uint8_t)n;
    req[5] = (uint8_t)(n >> 8);
    static uint8_t rep[WRITE_CHUNK + 8];
    if (n + 6 > sizeof(rep)) return false;
    if (!floaderCmd(req, 6, rep, n + 6) || rep[0] != CMD_RBF) return false;
    memcpy(out, rep + 4, n);
    return true;
}

/* --- bringing the loader up ----------------------------------------------- */

static void swsPortForLoad() {
    Serial1.end();
    Serial1.begin(SWS_BAUD, SERIAL_8N1, -1, config.flashPinSws);
}

static void swsPortForModule() {
    Serial1.end();
    Serial1.begin(SWS_BAUD, SERIAL_8N1, config.flashPinRxd, config.flashPinTxd);
}

/* Reset the module and hold its CPU stopped. The window after a reset is
   short, so the stop command is repeated for the whole of it. */
static void tlsrActivate(uint32_t activateMs) {
    pinMode(config.flashPinReset, OUTPUT);
    digitalWrite(config.flashPinReset, LOW);
    delay(50);
    digitalWrite(config.flashPinReset, HIGH);
    const uint32_t t0 = millis();
    while (millis() - t0 < activateMs) swsWrite1(0x0602, 0x05);
}

static bool tlsrLoadFloader() {
    /* The loader used to be read from the filesystem. It is a build time
       constant that has to match this code, so it lives in tlsr_floader.h
       now - one less file that can go missing or fall out of step. */
    swsPortForLoad();
    tlsrActivate(300);

    uint32_t div = 32000000UL / SWS_BAUD;
    if (div > 127) div = 127;
    swsEnd();
    swsWrite1(0x00b2, (uint8_t)div);

    uint32_t addr = FLOADER_ADDR;
    uint32_t total = 0;
    while (total < TLSR_FLOADER_LEN) {
        size_t n = TLSR_FLOADER_LEN - total;
        if (n > 256) n = 256;
        swsWrite(addr, TLSR_FLOADER + total, n);
        addr += n;
        total += n;
        vTaskDelay(1);
    }

    swsWrite1(0x0602, 0x88);
    delay(100);

    swsPortForModule();
    delay(100);

    uint16_t chipId = 0;
    uint8_t ver = 0;
    for (int attempt = 0; attempt < 5; attempt++) {
        if (floaderVersion(chipId, ver)) {
            tlsrLog("TLSR: loader " + String(ver >> 4) + "." + String(ver & 0x0f) +
                    ", chip id " + String(chipId, HEX) + ", " + String(total) + " bytes loaded");
            return chipId != 0;
        }
        delay(100);
    }
    tlsrLog("TLSR: loader did not answer");
    return false;
}

/* bringAPOnline() refuses while the state says flashing */
static void tlsrResumeAP() {
    setAPstate(false, AP_STATE_OFFLINE);
    if (!bringAPOnline()) tlsrLog("TLSR: the radio doesn't respond. Try the update again.");
}

bool tlsrFlashFile(const String &path) {
    if (contentFS == nullptr) {
        printf("TLSR: filesystem not mounted yet\r\n");
        return false;
    }
    fs::File f = contentFS->open(path, "r");
    if (!f || f.size() == 0) {
        tlsrLog("TLSR: " + path + " missing or empty");
        if (f) f.close();
        return false;
    }
    const uint32_t total = f.size();

    /* Stop talking to the module first: the access point task keeps a UART
       conversation running with the very chip being flashed, and it owns the
       same port this needs. */
    const bool wasRunning = apReaderRunning();
    if (wasRunning) {
        tlsrLog("TLSR: pausing access point communication");
        setAPstate(false, AP_STATE_FLASHING);
        stopApReader();
    }

    bool ok = false;
    for (int bringUp = 0; bringUp < 3 && !ok; bringUp++) ok = tlsrLoadFloader();
    if (!ok) {
        f.close();
        if (wasRunning) tlsrResumeAP();
        return false;
    }

    uint32_t jedec = 0;
    if (floaderJedec(jedec)) tlsrLog("TLSR: flash id " + String(jedec, HEX));

    static uint8_t buf[WRITE_CHUNK];
    static uint8_t chk[WRITE_CHUNK];
    uint32_t addr = 0;
    int8_t decade = -1;
    while (addr < total && ok) {
        if ((addr % FLASH_SECTOR) == 0 && !floaderEraseSector(addr)) {
            tlsrLog("TLSR: erase failed at 0x" + String(addr, HEX));
            ok = false;
            break;
        }
        const size_t n = f.read(buf, sizeof(buf));
        if (n == 0) break;

        /* Three tries on the block, and if those do not take, load the loader
           again and try once more. A block that keeps failing usually means the
           link glitched rather than the flash refusing - reloading puts both
           ends back into a known state, and nothing is lost by it because the
           image is written sector by sector anyway. */
        bool good = false;
        for (int recover = 0; recover < 3 && !good; recover++) {
            if (recover > 0) {
                tlsrLog("TLSR: reloading loader after trouble at 0x" + String(addr, HEX));
                /* No erase here: that would wipe the whole 4 KiB sector and
                   with it the blocks already written into it. Rewriting the
                   same data over a partially written page is fine - a flash
                   write only ever clears bits, and the bits are the same. */
                if (!tlsrLoadFloader()) break;
            }
            for (int attempt = 0; attempt < 3 && !good; attempt++) {
                if (!floaderWrite(addr, buf, n)) continue;
                if (!floaderRead(addr, chk, n)) continue;
                good = (memcmp(buf, chk, n) == 0);
            }
        }
        if (!good) {
            tlsrLog("TLSR: failed at 0x" + String(addr, HEX));
            ok = false;
            break;
        }
        addr += n;
        const int8_t d = (int8_t)(addr * 10 / total);
        if (d != decade) {
            decade = d;
            tlsrLog("TLSR: " + String(d * 10) + "%");
        }
        vTaskDelay(1);
    }
    f.close();
    if (!ok) {
        if (wasRunning) tlsrResumeAP();
        return false;
    }

    tlsrLog("TLSR: written and verified");
    /* Reset out of the loader so the freshly written firmware starts. */
    pinMode(config.flashPinReset, OUTPUT);
    digitalWrite(config.flashPinReset, LOW);
    delay(50);
    digitalWrite(config.flashPinReset, HIGH);

    if (wasRunning) {
        tlsrLog("TLSR: bringing the access point back online");
        tlsrResumeAP();
    }
    return true;
}

bool tlsrFlashFromUrl(const String &dirUrl, const String &apType) {
    String local = "/TLSR_AP_FW.bin";
    String jsonName = "/firmware_TLSR.json";
    String jsonUrl = dirUrl + "/firmware_TLSR.json";

    if (!downloadAndWriteBinary(jsonName, jsonUrl.c_str())) return false;

    JsonDocument doc;
    fs::File jf = contentFS->open(jsonName, "r");
    DeserializationError err = deserializeJson(doc, jf);
    jf.close();
    contentFS->remove(jsonName);
    if (err) {
        tlsrLog("TLSR: cannot read firmware_TLSR.json");
        return false;
    }

    /* The same directory holds both builds - the GFSK one (C7) and the
       802.15.4/Zigbee one (C8). They are different AP types, so the caller
       says which one it wants. */
    String file;
    for (JsonObject e : doc.as<JsonArray>()) {
        if (e["aptype"] == apType) {
            file = e["filename"].as<String>();
            /* Say which of the two was picked and which build it is. The
               manifest has carried a name and a version all along and nothing
               ever printed them, so the log could not tell the two radios
               apart. */
            String nm = e["name"].as<String>();
            String ver = e["version"].as<String>();
            tlsrLog("TLSR: " + (nm.length() ? nm : file) + " (type " + apType +
                    (ver.length() ? ", version " + ver : "") + ")");
            break;
        }
    }
    if (file == "") {
        tlsrLog("TLSR: no " + apType + " image listed in firmware_TLSR.json");
        return false;
    }

    String binUrl = dirUrl + "/" + file;
    tlsrLog("TLSR: fetching " + file);
    if (!downloadAndWriteBinary(local, binUrl.c_str())) return false;
    return tlsrFlashFile(local);
}

/* Flashing takes a while and must not block the web server, so it runs in a
   task of its own. */
struct tlsrJob {
    String path;
    String url;
    String apType;
};

static void tlsrFlashTask(void *arg) {
    tlsrJob *j = (tlsrJob *)arg;
    if (j->url.length())
        tlsrFlashFromUrl(j->url, j->apType);
    else
        tlsrFlashFile(j->path);
    delete j;
    vTaskDelete(NULL);
}

void tlsrFlashStart(const String &path, const String &url, const String &apType) {
    tlsrJob *j = new tlsrJob{path, url, apType};
    xTaskCreate(tlsrFlashTask, "tlsrFlash", 8192, j, 5, NULL);
}

#endif
