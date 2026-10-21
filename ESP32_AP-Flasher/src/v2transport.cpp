// The framed S3 <-> radio protocol v2 (../../oepl-radio-v2.h): COBS frames with CRC16, sequence
// numbers, and an answer to every request. Chosen by bringAPOnline() when the radio announces it
// with CAP> in its NFO? answer. Messages from the radio are translated to the structs the old
// protocol used, so the rest of the AP code does not see the difference.

#include <Arduino.h>

#include <mutex>
#include <vector>

#include "../../oepl-radio-v2.h"
#include "commstructs.h"
#include "newproto.h"
#include "radiotransport.h"
#include "serialap.h"
#include "tag_db.h"
#include "web.h"

#define RESP_WAIT 0
#define RESP_DONE 1

// A request on the line, waiting for its response (filled in by the reader task)
struct v2Outstanding {
    volatile bool used;
    uint8_t seq;
    volatile uint8_t state;
    uint8_t type;
    uint8_t len;
    uint8_t data[48];
};

class V2Transport : public RadioTransport {
   public:
    const char* name() override { return "v2"; }
    uint16_t sendBlock(const void* data, uint16_t len, uint64_t ver, uint32_t offset, uint32_t totalSize) override;
    bool cacheBlock(const void* data, uint16_t len, uint64_t ver, uint32_t offset, uint32_t totalSize) override;
    bool sendDataAvail(struct pendingData* pending) override;
    bool cancelPending(struct pendingData* pending) override;
    bool setChannelPower(struct espSetChannelPower* scp) override;
    bool ping() override;
    bool requestInfo() override;
    bool enableFastLink() override { return true; }
    void onIdle() override;
    void onByte(uint8_t c) override;
    bool heartbeat() override { return true; }
    uint32_t pingIntervalMs() override { return V2_HEARTBEAT_MS; }
    uint8_t maxMissedPings() override { return 3; }
    void syncAllowList() override;

    bool start();

   private:
    // reception (reader task)
    uint8_t rxCobs[V2_MAX_FRAME_COBS];
    size_t rxLen = 0;
    bool rxDiscard = false;
    uint32_t lastByteMs = 0;
    uint8_t raw[V2_MAX_FRAME_RAW];
    int lastEventSeq = -1;  // the last event from the radio handled; a repeat is only acknowledged

    // requests (one task at a time, under txActive)
    uint8_t txSeq = 0;
    v2Outstanding out[V2_CHUNK_WINDOW];
    SemaphoreHandle_t writeMutex = nullptr;  // whole frames only: the reader writes ACKs too
    volatile uint32_t dataNeededMs = 0;      // when the last DATA_NEEDED came in

    // The radio's block cache. Every check-in of a tag with queued data repeats its SET_PENDING;
    // the data goes to the cache once (again after PRELOAD_AGAIN_MS: the radio may have evicted it).
    uint16_t cacheKb = 0;
    volatile int32_t cacheFreeKb = 0;  // from the last STATUS, minus what was queued for preloading since
    struct {
        uint64_t ver;
        uint32_t ms;
    } preloaded[16] = {};
    uint8_t preloadedNext = 0;
    static constexpr uint32_t PRELOAD_AGAIN_MS = 10 * 60 * 1000;
    bool wantPreload(const struct pendingData* pending);

    // last STATUS counters, to log only what changed
    uint16_t lastCrcErrors = 0, lastOverflows = 0, lastRetransmits = 0;

    // The radio's allow-list (V2_CAP_ALLOW_LIST): every MAC in the tag database. The lock bits go
    // in SET_CONFIG only while the list in the radio is complete.
    bool allowListReady = false;
    bool allowListFull = false;  // the radio has no room for all tags: the lock works on the S3 only
    bool allowRequest(uint8_t type, const uint8_t* macs, uint8_t count);
    bool sendAllowList();

    void writeFrame(uint8_t type, uint8_t seq, uint8_t flags, const void* payload, size_t len);
    v2Outstanding* sendRequest(uint8_t type, const void* payload, size_t len, uint8_t seq, bool retransmit);
    bool sendChunks(const void* data, uint16_t len, uint64_t ver, uint32_t offset, uint32_t totalSize);
    bool waitResponse(v2Outstanding* o, uint32_t timeoutMs);
    bool request(uint8_t type, const void* payload, size_t len, uint8_t* respType = nullptr, uint8_t* resp = nullptr, uint8_t* respLen = nullptr);
    void handleFrame(const uint8_t* frame, size_t frameLen);
    void handleEvent(uint8_t type, const uint8_t* payload, size_t len);
    void applyStatus(const uint8_t* payload, size_t len);
};

static V2Transport v2Transport;
RadioTransport* v2RadioTransport() { return &v2Transport; }

// Tag database changes for the radio's allow-list, queued from any task (under the tag database
// lock) and sent by syncAllowList() from APTask. Only while a radio with an allow-list is linked;
// after each HELLO the whole database goes to the radio.
struct allowChange {
    uint8_t mac[8];
    bool added;
};
static std::mutex allowMutex;
static std::vector<allowChange> allowChanges;
static volatile bool allowListInUse = false;
static bool allowResend = false;  // send the whole list: after HELLO, or when the queue overflowed
#define MAX_ALLOW_CHANGES 256

void radioAllowListChanged(const uint8_t mac[8], bool added) {
    if (!allowListInUse) return;
    std::lock_guard<std::mutex> lock(allowMutex);
    if (allowResend) return;
    if (allowChanges.size() >= MAX_ALLOW_CHANGES) {
        allowChanges.clear();
        allowResend = true;
        return;
    }
    allowChange c;
    memcpy(c.mac, mac, 8);
    c.added = added;
    allowChanges.push_back(c);
}

static bool txStart() {
    while (!xSemaphoreTake(txActive, portTICK_PERIOD_MS)) {
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    return true;
}
static void txEnd() { xSemaphoreGive(txActive); }

void V2Transport::writeFrame(uint8_t type, uint8_t seq, uint8_t flags, const void* payload, size_t len) {
    uint8_t wire[V2_MAX_FRAME_COBS + 1];
    const size_t n = v2BuildFrame(type, seq, flags, payload, len, wire);
    if (!n) return;
    xSemaphoreTake(writeMutex, portMAX_DELAY);
    AP_SERIAL_PORT.write(wire, n);
    xSemaphoreGive(writeMutex);
}

v2Outstanding* V2Transport::sendRequest(uint8_t type, const void* payload, size_t len, uint8_t seq, bool retransmit) {
    v2Outstanding* o = nullptr;
    for (auto& slot : out) {
        if (slot.used && slot.seq == seq) o = &slot;
    }
    for (auto& slot : out) {
        if (o == nullptr && !slot.used) o = &slot;
    }
    if (o == nullptr) return nullptr;
    o->seq = seq;
    o->state = RESP_WAIT;
    o->used = true;  // before the frame goes out, so the reader can't miss a quick answer
    writeFrame(type, seq, retransmit ? V2_FLAG_RETRANSMIT : 0, payload, len);
    return o;
}

bool V2Transport::waitResponse(v2Outstanding* o, uint32_t timeoutMs) {
    const uint32_t start = millis();
    while (o->state == RESP_WAIT && millis() - start < timeoutMs) vTaskDelay(1);
    return o->state == RESP_DONE;
}

// Stop-and-wait with retransmissions; true on ACK or a typed reply, false on NAK or no answer
bool V2Transport::request(uint8_t type, const void* payload, size_t len, uint8_t* respType, uint8_t* resp, uint8_t* respLen) {
    const uint8_t seq = txSeq++;
    for (uint8_t attempt = 0; attempt <= V2_RETRIES; attempt++) {
        v2Outstanding* o = sendRequest(type, payload, len, seq, attempt > 0);
        if (o == nullptr) return false;
        if (waitResponse(o, V2_ACK_TIMEOUT_MS)) {
            o->used = false;
            lastAPActivity = millis();
            if (respType) *respType = o->type;
            if (resp && respLen) {
                memcpy(resp, o->data, o->len);
                *respLen = o->len;
            }
            if (o->type == V2_NAK) {
                Serial.printf("v2: request %02X refused, code %d\r\n", type, o->len ? o->data[0] : 0);
                return false;
            }
            return true;
        }
    }
    for (auto& slot : out) slot.used = false;
    Serial.printf("v2: no answer to request %02X\r\n", type);
    return false;
}

// HELLO at the new baud rate; true when the radio answered with a matching protocol version
bool V2Transport::start() {
    if (writeMutex == nullptr) writeMutex = xSemaphoreCreateMutex();
    rxLen = 0;
    rxDiscard = false;
    lastEventSeq = -1;
    txSeq = 0;
    for (auto& slot : out) slot.used = false;
    for (auto& p : preloaded) p = {0, 0};

    allowListInUse = false;
    allowListReady = false;
    allowListFull = false;

    struct v2Hello hello = {V2_PROTO_MAJOR, V2_PROTO_MINOR, V2_CAP_FRAMING | V2_CAP_SUBGHZ | V2_CAP_TAG_RETURN_DATA | V2_CAP_ALLOW_LIST};
    uint8_t respType = 0, resp[48], respLen = 0;
    txStart();
    bool ok = false;
    // three tries of 200 ms: the radio may still be switching its baud rate
    for (uint8_t attempt = 0; attempt < 3 && !ok; attempt++) {
        const uint8_t seq = txSeq++;
        v2Outstanding* o = sendRequest(V2_HELLO, &hello, sizeof(hello), seq, false);
        if (o && waitResponse(o, 200)) {
            o->used = false;
            respType = o->type;
            respLen = o->len;
            memcpy(resp, o->data, o->len);
            ok = (respType == V2_HELLO_REPLY && respLen >= sizeof(struct v2HelloReply));
        }
        for (auto& slot : out) slot.used = false;
    }
    txEnd();
    if (!ok) return false;

    struct v2HelloReply hr;
    memcpy(&hr, resp, sizeof(hr));
    if (hr.protoMajor != V2_PROTO_MAJOR) {
        Serial.printf("v2: radio speaks protocol %d.%d\r\n", hr.protoMajor, hr.protoMinor);
        return false;
    }
    apInfo.type = hr.hwType;
    apInfo.version = hr.fwVersion;
    memcpy(apInfo.mac, hr.mac, 8);
    apInfo.caps = hr.caps;
    cacheKb = (hr.caps & V2_CAP_BLOCK_CACHE) ? hr.cacheKb : 0;
    cacheFreeKb = cacheKb;
    if (hr.caps & V2_CAP_ALLOW_LIST) {
        std::lock_guard<std::mutex> lock(allowMutex);
        allowChanges.clear();
        allowResend = true;
        allowListInUse = true;
    }
    lastAPActivity = millis();
    Serial.printf("v2: protocol %d.%d, radio %02X version %04X, caps %04X, %d pending slots, %d KB block cache\r\n", hr.protoMajor, hr.protoMinor, hr.hwType, hr.fwVersion, hr.caps, hr.pendingSlots, cacheKb);
    return requestInfo();
}

// A block as DATA_CHUNKs, up to V2_CHUNK_WINDOW unanswered at a time
bool V2Transport::sendChunks(const void* data, uint16_t len, uint64_t ver, uint32_t offset, uint32_t totalSize) {
    const uint8_t* bytes = (const uint8_t*)data;
    const uint16_t chunks = (len + V2_CHUNK_MAX - 1) / V2_CHUNK_MAX;
    uint8_t payload[sizeof(struct v2DataChunk) + V2_CHUNK_MAX];
    struct v2DataChunk* dc = (struct v2DataChunk*)payload;
    dc->dataVer = ver;
    dc->totalSize = totalSize;

    struct inFlight {
        uint16_t chunk;
        uint8_t seq;
        uint8_t tries;
        uint32_t sentMs;
        v2Outstanding* o;
    } flight[V2_CHUNK_WINDOW];
    uint8_t inFlightCount = 0;
    uint16_t next = 0, done = 0;
    bool failed = false;

    auto send = [&](inFlight& f, bool retransmit) {
        const uint16_t n = (f.chunk == chunks - 1) ? len - f.chunk * V2_CHUNK_MAX : V2_CHUNK_MAX;
        dc->offset = offset + f.chunk * V2_CHUNK_MAX;
        memcpy(dc->bytes, bytes + f.chunk * V2_CHUNK_MAX, n);
        f.o = sendRequest(V2_DATA_CHUNK, payload, sizeof(struct v2DataChunk) + n, f.seq, retransmit);
        f.sentMs = millis();
    };

    txStart();
    for (auto& slot : out) slot.used = false;
    while (done < chunks && !failed) {
        while (inFlightCount < V2_CHUNK_WINDOW && next < chunks) {
            inFlight& f = flight[inFlightCount++];
            f.chunk = next++;
            f.seq = txSeq++;
            f.tries = 0;
            send(f, false);
            if (f.o == nullptr) {
                inFlightCount--;
                failed = true;
            }
        }
        vTaskDelay(1);
        for (uint8_t i = 0; i < inFlightCount && !failed;) {
            inFlight& f = flight[i];
            if (f.o->state == RESP_DONE) {
                if (f.o->type == V2_NAK) {
                    Serial.printf("v2: chunk at %u refused, code %d\r\n", offset + f.chunk * V2_CHUNK_MAX, f.o->len ? f.o->data[0] : 0);
                    failed = true;
                    break;
                }
                f.o->used = false;
                done++;
                flight[i] = flight[--inFlightCount];
                continue;
            }
            if (millis() - f.sentMs >= V2_ACK_TIMEOUT_MS) {
                if (++f.tries > V2_RETRIES) {
                    failed = true;
                    break;
                }
                send(f, true);
            }
            i++;
        }
    }
    for (auto& slot : out) slot.used = false;
    txEnd();
    if (failed) {
        Serial.printf("v2: block at %u not sent\r\n", offset);
        return false;
    }
    lastAPActivity = millis();
    return true;
}

// The block a tag asked for (DATA_NEEDED); the radio starts the burst once it has all of it. Returns
// the block checksum (as the old protocol did), 0 on failure.
uint16_t V2Transport::sendBlock(const void* data, uint16_t len, uint64_t ver, uint32_t offset, uint32_t totalSize) {
    const uint32_t t = millis();
    if (!sendChunks(data, len, ver, offset, totalSize)) return 0;
    const uint8_t* bytes = (const uint8_t*)data;
    const uint16_t chunks = (len + V2_CHUNK_MAX - 1) / V2_CHUNK_MAX;
    uint16_t checksum = 0;
    for (uint16_t c = 0; c < len; c++) checksum += bytes[c];
    Serial.printf("v2: block at %u, %u chunks in %u ms, %u ms after the request\r\n", offset, chunks, millis() - t, millis() - dataNeededMs);
    return checksum;
}

bool V2Transport::cacheBlock(const void* data, uint16_t len, uint64_t ver, uint32_t offset, uint32_t totalSize) {
    return cacheKb && sendChunks(data, len, ver, offset, totalSize);
}

// Send this item to the radio's cache? Only data, only once, and only what fits in the free space
bool V2Transport::wantPreload(const struct pendingData* pending) {
    const struct AvailDataInfo& adi = pending->availdatainfo;
    if (cacheKb == 0 || adi.dataType == DATATYPE_NOUPDATE || adi.dataSize == 0) return false;
    for (const auto& p : preloaded) {
        if (p.ver == adi.dataVer && p.ms && millis() - p.ms < PRELOAD_AGAIN_MS) return false;
    }
    const int32_t needKb = (adi.dataSize + V2_BLOCK_SIZE - 1) / V2_BLOCK_SIZE * (V2_BLOCK_SIZE / 1024);
    if (needKb > cacheFreeKb) return false;
    cacheFreeKb -= needKb;
    preloaded[preloadedNext] = {adi.dataVer, millis() | 1};
    preloadedNext = (preloadedNext + 1) % (sizeof(preloaded) / sizeof(preloaded[0]));
    return true;
}

bool V2Transport::sendDataAvail(struct pendingData* pending) {
    struct v2SetPending sp;
    memcpy(sp.mac, pending->targetMac, 8);
    sp.dataVer = pending->availdatainfo.dataVer;
    sp.dataSize = pending->availdatainfo.dataSize;
    sp.dataType = pending->availdatainfo.dataType;
    sp.dataTypeArg = pending->availdatainfo.dataTypeArgument;
    sp.nextCheckin = pending->availdatainfo.nextCheckIn;
    sp.ttlMin = pending->attemptsLeft ? pending->attemptsLeft : 1;
    const bool preload = wantPreload(pending);
    sp.flags = preload ? V2_PENDING_DATA_FOLLOWS : 0;
    txStart();
    // NAK NO_SLOT: the radio's table is full; the next check-in of the tag tries again
    const bool ok = request(V2_SET_PENDING, &sp, sizeof(sp));
    txEnd();
    if (ok && preload) {
        // by rxCmdProcessor, which also answers DATA_NEEDED: this may be called from any task
        struct espBlockRequest* br = (struct espBlockRequest*)calloc(sizeof(struct espBlockRequest) + 8, 1);
        if (br != nullptr) {
            memcpy(br->src, sp.mac, 8);
            br->ver = sp.dataVer;
            addRXQueue((uint8_t*)br, sizeof(struct espBlockRequest), RX_CMD_PRELOAD);
        }
    }
    return ok;
}

bool V2Transport::cancelPending(struct pendingData* pending) {
    struct v2CancelPending cp;
    memcpy(cp.mac, pending->targetMac, 8);
    txStart();
    const bool ok = request(V2_CANCEL_PENDING, &cp, sizeof(cp));
    txEnd();
    return ok;
}

bool V2Transport::setChannelPower(struct espSetChannelPower* scp) {
    struct v2SetConfig sc;
    sc.channel = scp->channel ? scp->channel : V2_CFG_UNCHANGED_U8;
#ifdef HAS_SUBGHZ
    sc.subghzChannel = scp->subghzchannel;
#else
    sc.subghzChannel = V2_CFG_UNCHANGED_U8;
#endif
    sc.txPowerDbm = (int8_t)scp->power;
    sc.flags = allowListReady ? (config.lock << V2_CFG_LOCK_SHIFT) & V2_CFG_LOCK_MASK : 0;
    sc.ledBrightness = config.led;  // the radio's activity LEDs follow the AP's LED setting
    txStart();
    const bool ok = request(V2_SET_CONFIG, &sc, sizeof(sc));
    txEnd();
    return ok;
}

// ALLOW_ADD or ALLOW_REMOVE for up to V2_ALLOW_MAX_MACS MACs, or ALLOW_CLEAR (count 0); true when
// the radio confirmed it. Sets allowListFull when the radio had no room.
bool V2Transport::allowRequest(uint8_t type, const uint8_t* macs, uint8_t count) {
    uint8_t respType = 0, resp[48], respLen = 0;
    txStart();
    const bool ok = request(type, macs, count * 8, &respType, resp, &respLen);
    txEnd();
    if (!ok && respType == V2_NAK && respLen && resp[0] == V2_NAK_NO_SLOT) allowListFull = true;
    return ok;
}

// The whole tag database to the radio's allow-list
bool V2Transport::sendAllowList() {
    std::vector<uint8_t> macs;
    {
        TagDBLock lock;
        macs.reserve(tagDB.size() * 8);
        for (const tagRecord* tag : tagDB) {
            if (tag->version == 0) macs.insert(macs.end(), tag->mac, tag->mac + 8);
        }
    }
    if (!allowRequest(V2_ALLOW_CLEAR, nullptr, 0)) return false;
    const size_t count = macs.size() / 8;
    for (size_t c = 0; c < count; c += V2_ALLOW_MAX_MACS) {
        const uint8_t n = (count - c < V2_ALLOW_MAX_MACS) ? count - c : V2_ALLOW_MAX_MACS;
        if (!allowRequest(V2_ALLOW_ADD, macs.data() + c * 8, n)) return false;
    }
    Serial.printf("v2: allow-list of %u tags sent to the radio\r\n", count);
    return true;
}

void V2Transport::syncAllowList() {
    static uint32_t retryMs = 0;
    if (!allowListInUse || (retryMs && millis() - retryMs < 1000)) return;
    retryMs = 0;
    std::vector<allowChange> changes;
    bool resend;
    {
        std::lock_guard<std::mutex> lock(allowMutex);
        changes.swap(allowChanges);
        resend = allowResend;
        allowResend = false;
    }
    if (!resend && changes.empty()) return;

    bool ok = true;
    if (resend) {
        if (allowListReady) {
            // no lock while the list is incomplete, or known tags would go unanswered
            allowListReady = false;
            sendChannelPower(&curChannel);
        }
        ok = sendAllowList();
    } else {
        // consecutive changes of the same kind in one request
        for (size_t c = 0; c < changes.size() && ok;) {
            uint8_t macs[V2_ALLOW_MAX_MACS * 8];
            uint8_t n = 0;
            const bool added = changes[c].added;
            while (c < changes.size() && changes[c].added == added && n < V2_ALLOW_MAX_MACS) memcpy(macs + 8 * n++, changes[c++].mac, 8);
            ok = allowRequest(added ? V2_ALLOW_ADD : V2_ALLOW_REMOVE, macs, n);
        }
    }

    if (allowListFull) {
        allowListInUse = false;
        allowListReady = false;
        wsErr("The radio has no room for all tags: the tag inventory lock works on the AP only");
        sendChannelPower(&curChannel);
        return;
    }
    if (!ok) {
        // the whole list again, after a second; a lost link is brought up again by APTask
        std::lock_guard<std::mutex> lock(allowMutex);
        allowChanges.clear();
        allowResend = true;
        retryMs = millis() | 1;
        return;
    }
    if (!allowListReady) {
        allowListReady = true;
        if (config.lock) sendChannelPower(&curChannel);
    }
}

void V2Transport::applyStatus(const uint8_t* payload, size_t len) {
    if (len < sizeof(struct v2Status)) return;
    struct v2Status st;
    memcpy(&st, payload, sizeof(st));
    apInfo.channel = st.channel;
    apInfo.power = (uint8_t)st.txPowerDbm;
    apInfo.pendingBuffer = st.pending > 255 ? 255 : st.pending;
    apInfo.nop = st.pendingIdle > 255 ? 255 : st.pendingIdle;
    cacheFreeKb = (int32_t)cacheKb - st.cacheUsedKb;
#ifdef HAS_SUBGHZ
    apInfo.hasSubGhz = st.subghzChannel != V2_NO_SUBGHZ_CHANNEL;
    apInfo.SubGhzChannel = apInfo.hasSubGhz ? st.subghzChannel : 0;
#endif
    if (st.serialCrcErrors != lastCrcErrors || st.serialOverflows != lastOverflows || st.retransmits != lastRetransmits) {
        Serial.printf("v2 radio: %u CRC errors, %u overflows, %u retransmits\r\n", st.serialCrcErrors, st.serialOverflows, st.retransmits);
        lastCrcErrors = st.serialCrcErrors;
        lastOverflows = st.serialOverflows;
        lastRetransmits = st.retransmits;
    }
}

bool V2Transport::ping() {
    uint8_t respType = 0, resp[48], respLen = 0;
    txStart();
    const bool ok = request(V2_HEARTBEAT, nullptr, 0, &respType, resp, &respLen);
    txEnd();
    if (ok && respType == V2_STATUS) applyStatus(resp, respLen);
    return ok && respType == V2_STATUS;
}

bool V2Transport::requestInfo() {
    uint8_t respType = 0, resp[48], respLen = 0;
    txStart();
    const bool ok = request(V2_GET_STATS, nullptr, 0, &respType, resp, &respLen);
    txEnd();
    if (ok && respType == V2_STATUS) applyStatus(resp, respLen);
    return ok && respType == V2_STATUS;
}

void V2Transport::onIdle() {
    // a frame whose end never came: drop it rather than glue it to the next one
    if (rxLen && !AP_SERIAL_PORT.available() && millis() - lastByteMs > 20) {
        rxLen = 0;
        rxDiscard = false;
    }
}

void V2Transport::onByte(uint8_t c) {
    lastByteMs = millis();
    if (c != 0x00) {
        if (rxLen < sizeof(rxCobs)) {
            rxCobs[rxLen++] = c;
        } else {
            rxDiscard = true;
        }
        return;
    }
    if (!rxDiscard && rxLen) {
        const int n = v2CobsDecode(rxCobs, rxLen, raw, sizeof(raw));
        if (n >= 5 && v2CheckFrame(raw, n)) {
            handleFrame(raw, n);
        } else {
            Serial.print("v2: frame with bad CRC dropped\r\n");
        }
    }
    rxLen = 0;
    rxDiscard = false;
}

void V2Transport::handleFrame(const uint8_t* frame, size_t frameLen) {
    const struct v2Header* h = (const struct v2Header*)frame;
    const uint8_t* payload = frame + sizeof(struct v2Header);
    const size_t len = frameLen - sizeof(struct v2Header) - 2;
    lastAPActivity = millis();

    if (h->flags & V2_FLAG_RESPONSE) {
        for (auto& slot : out) {
            if (slot.used && slot.seq == h->seq && slot.state == RESP_WAIT) {
                slot.type = h->type;
                slot.len = len > sizeof(slot.data) ? sizeof(slot.data) : len;
                memcpy(slot.data, payload, slot.len);
                __sync_synchronize();  // the data before the state, for the task on the other core
                slot.state = RESP_DONE;
            }
        }
        return;
    }

    // an event: acknowledge at once (the radio sends the next one only then), handle it once
    const bool known = h->type >= V2_CHECKIN && h->type <= V2_LOG;
    if (!known) {
        const uint8_t code = V2_NAK_UNKNOWN_TYPE;
        writeFrame(V2_NAK, h->seq, V2_FLAG_RESPONSE, &code, 1);
        return;
    }
    if (h->type != V2_LOG) writeFrame(V2_ACK, h->seq, V2_FLAG_RESPONSE, nullptr, 0);
    if ((int)h->seq == lastEventSeq) return;  // a repeat: our ACK got lost
    lastEventSeq = h->seq;
    handleEvent(h->type, payload, len);
}

void V2Transport::handleEvent(uint8_t type, const uint8_t* payload, size_t len) {
    switch (type) {
        case V2_CHECKIN: {
            if (len < sizeof(struct v2Checkin)) return;
            const struct v2Checkin* ci = (const struct v2Checkin*)payload;
            struct espAvailDataReq* eadr = (struct espAvailDataReq*)calloc(sizeof(struct espAvailDataReq) + 8, 1);
            if (eadr == nullptr) return;
            memcpy(eadr->src, ci->mac, 8);
            memcpy(&eadr->adr, ci->adr, sizeof(struct AvailDataReq));
            addCRC(eadr, sizeof(struct espAvailDataReq));
            addRXQueue((uint8_t*)eadr, sizeof(struct espAvailDataReq), RX_CMD_ADR);
            break;
        }
        case V2_DATA_NEEDED: {
            if (len < sizeof(struct v2DataNeeded)) return;
            const struct v2DataNeeded* dn = (const struct v2DataNeeded*)payload;
            struct espBlockRequest* br = (struct espBlockRequest*)calloc(sizeof(struct espBlockRequest) + 8, 1);
            if (br == nullptr) return;
            dataNeededMs = millis();
            br->ver = dn->dataVer;
            br->blockId = dn->offset / V2_BLOCK_SIZE;
            memcpy(br->src, dn->mac, 8);
            addCRC(br, sizeof(struct espBlockRequest));
            addRXQueue((uint8_t*)br, sizeof(struct espBlockRequest), RX_CMD_RQB);
            break;
        }
        case V2_XFER_COMPLETE:
        case V2_XFER_TIMEOUT: {
            if (len < sizeof(struct v2XferEnd)) return;
            const struct v2XferEnd* xe = (const struct v2XferEnd*)payload;
            struct espXferComplete* xfc = (struct espXferComplete*)calloc(sizeof(struct espXferComplete) + 8, 1);
            if (xfc == nullptr) return;
            memcpy(xfc->src, xe->mac, 8);
            addCRC(xfc, sizeof(struct espXferComplete));
            addRXQueue((uint8_t*)xfc, sizeof(struct espXferComplete), type == V2_XFER_COMPLETE ? RX_CMD_XFC : RX_CMD_XTO);
            break;
        }
        case V2_TAG_RETURN_DATA: {
            if (len <= sizeof(struct v2TagReturnData)) return;
            const struct v2TagReturnData* td = (const struct v2TagReturnData*)payload;
            size_t n = len - sizeof(struct v2TagReturnData);
            if (n > sizeof(struct tagReturnData)) n = sizeof(struct tagReturnData);
            struct espTagReturnData* etrd = (struct espTagReturnData*)calloc(sizeof(struct espTagReturnData) + 8, 1);
            if (etrd == nullptr) return;
            memcpy(etrd->src, td->mac, 8);
            etrd->len = n;
            memcpy(&etrd->returnData, td->data, n);
            addCRC(etrd, n + 10);
            addRXQueue((uint8_t*)etrd, n + 10, RX_CMD_TRD);
            break;
        }
        case V2_BLOCK_SERVED: {
            if (len < sizeof(struct v2BlockServed)) return;
            struct v2BlockServed* bs = (struct v2BlockServed*)malloc(sizeof(struct v2BlockServed));
            if (bs == nullptr) return;
            memcpy(bs, payload, sizeof(struct v2BlockServed));
            addRXQueue((uint8_t*)bs, sizeof(struct v2BlockServed), RX_CMD_BLS);
            break;
        }
        case V2_LOG:
            if (len > 1) {
                Serial.print("radio: ");
                Serial.write(payload + 1, len - 1);
                Serial.print("\r\n");
            }
            break;
    }
}

bool v2StartLink() {
    // the reader hands bytes to radioTransport, so switch first; the HELLO_REPLY must reach v2
    radioTransport = &v2Transport;
    if (v2Transport.start()) return true;
    radioTransport = legacyRadioTransport();
    return false;
}
