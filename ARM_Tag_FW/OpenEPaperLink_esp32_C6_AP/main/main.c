// Ported to ESP32-C6 By ATC1441(ATCnetz.de) for OpenEPaperLink at ~08.2023

#include "main.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_ieee802154.h"
#include "esp_log.h"
#include "esp_phy_init.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "led.h"
#include "proto.h"
#include "radio.h"
#include "sdkconfig.h"
#include "second_uart.h"
#ifdef CONFIG_IDF_TARGET_ESP32C6
#include "soc/lp_uart_reg.h"
#endif
#include "soc/uart_struct.h"
#include "utils.h"
#include <esp_mac.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "logging.h"
#include "SubGigRadio.h"
#include "v2link.h"
#include "blockcache.h"


static const char *TAG = "MAIN";

const uint8_t channelList[6] = {11, 15, 20, 25, 26, 27};

#if CONFIG_OEPL_VERBOSE_DEBUG
const struct {
   uint8_t Type;
   const char *Name;
} gPktTypeLookupTbl[] = {
   {PKT_TAG_RETURN_DATA, "TAG_RETURN_DATA"},
   {PKT_TAG_RETURN_DATA_ACK, "TAG_RETURN_DATA_ACK"},
   {PKT_AVAIL_DATA_SHORTREQ, "AVAIL_DATA_SHORTREQ"},
   {PKT_AVAIL_DATA_REQ, "AVAIL_DATA_REQ"},
   {PKT_AVAIL_DATA_INFO, "AVAIL_DATA_INFO"},
   {PKT_BLOCK_PARTIAL_REQUEST, "BLOCK_PARTIAL_REQUEST"},
   {PKT_BLOCK_REQUEST_ACK, "BLOCK_REQUEST_ACK"},
   {PKT_BLOCK_REQUEST, "BLOCK_REQUEST"},
   {PKT_BLOCK_PART, "BLOCK_PART"},
   {PKT_XFER_COMPLETE, "XFER_COMPLETE"},
   {PKT_XFER_COMPLETE_ACK, "XFER_COMPLETE_ACK"},
   {PKT_CANCEL_XFER, "CANCEL_XFER"},
   {PKT_PING, "PING"},
   {PKT_PONG, "PONG"},
   {0,NULL} // End of table
};
#endif


#define DATATYPE_NOUPDATE 0
#define HW_TYPE           0xC6

#define MAX_PENDING_MACS      250
#define HOUSEKEEPING_INTERVAL 60UL

struct pendingData pendingDataArr[MAX_PENDING_MACS];

// VERSION GOES HERE!
uint16_t version = 0x0024;

#define RAW_PKT_PADDING 2

uint8_t radiotxbuffer[128];
uint8_t radiorxbuffer[128];

static uint32_t housekeepingTimer;

struct blockRequest requestedData = {0};                  // holds which data was requested by the tag

uint8_t  dstMac[8];                                       // target for the block transfer
uint16_t dstPan;                                          //

static uint32_t blockStartTimer  = 0;                     // reference that holds when the AP sends the next block
// Start a burst this long after the pleaseWaitMs the tag was given: the nRF52811 tag only starts its
// radio after pleaseWaitMs (the ZBS243 and TLSR tags listen before it).
#define BURST_MARGIN_MS     5
// If the block data from the S3 is not complete at that time, wait at most this long for it; after
// that send nothing (the tag asks again) rather than whatever is in the buffer.
#define BLOCK_DATA_SLACK_MS 100
// What the tag is told to wait: the block is in the cache, it is on its way for an earlier request,
// or it has to come from the S3 first. 0 or at least 10 (smaller values underflow in tag firmware).
#define PLEASE_WAIT_CACHED_MS   15
#define PLEASE_WAIT_COMING_MS   30
#define PLEASE_WAIT_DOWNLOAD_MS 140
uint32_t        nextBlockAttempt = 0;                     // when the radio last asked the S3 for a block
uint8_t         seq              = 0;                     // holds current sequence number for transmission
uint8_t         lastAckMac[8] = {0};

// these variables hold the current mac were talking to
#define CONCURRENT_REQUEST_DELAY 1200UL
uint32_t lastBlockRequest = 0;
uint8_t  lastBlockMac[8];
uint8_t  lastTagReturn[8];

// A block burst is sent one frame per loop pass, so serial frames from the S3 are handled between
// the parts (a burst takes up to 200 ms; the S3 expects an answer within 30 ms).
static uint8_t burstParts[BLOCK_MAX_PARTS];
static int     burstSlot   = -1;  // the cache slot the burst reads, pinned until it ends
static uint8_t burstLen    = 0;
static uint8_t burstPos    = 0;
static bool    burstActive = false;
static int16_t burstLateMs = 0;

// The S3 sends a HEARTBEAT every second. Without any frame for this long it is gone (rebooted, or
// reset its end of the line): go back to the ASCII protocol at 115200 baud, where it starts again.
#define V2_LINK_LOST_MS 5000

#define NO_SUBGHZ_CHANNEL  255
uint8_t curSubGhzChannel;
uint8_t curChannel = 25;
uint8_t curPower   = 10;

uint8_t curPendingData = 0;
uint8_t curNoUpdate    = 0;

void sendXferCompleteAck(uint8_t *dst, uint16_t pan);
void sendCancelXfer(uint8_t *dst, uint16_t pan);
void espNotifyAPInfo();

// tools
void addCRC(void *p, uint8_t len) {
    uint8_t total = 0;
    for (uint8_t c = 1; c < len; c++) {
        total += ((uint8_t *) p)[c];
    }
    ((uint8_t *) p)[0] = total;
}
bool checkCRC(void *p, uint8_t len) {
    uint8_t total = 0;
    for (uint8_t c = 1; c < len; c++) {
        total += ((uint8_t *) p)[c];
    }
    return ((uint8_t *) p)[0] == total;
}

uint8_t getPacketType(void *buffer) {
    struct MacFcs *fcs = buffer;
    if ((fcs->frameType == 1) && (fcs->destAddrType == 2) && (fcs->srcAddrType == 3) && (fcs->panIdCompressed == 0)) {
        // broadcast frame
        uint8_t type = ((uint8_t *) buffer)[sizeof(struct MacFrameBcast)];
        return type;
    } else if ((fcs->frameType == 1) && (fcs->destAddrType == 3) && (fcs->srcAddrType == 3) && (fcs->panIdCompressed == 1)) {
        // normal frame
        uint8_t type = ((uint8_t *) buffer)[sizeof(struct MacFrameNormal)];
        return type;
    }
    return 0;
}
uint8_t getBlockDataLength() {
    uint8_t partNo = 0;
    for (uint8_t c = 0; c < BLOCK_MAX_PARTS; c++) {
        if (requestedData.requestedParts[c / 8] & (1 << (c % 8))) {
            partNo++;
        }
    }
    return partNo;
}

// pendingdata slot stuff
// A tag accepts a reply only within a few ms of its request (2-9 ms depending on tag type and request,
// see the timing budget in docs/design/radio-chain-analysis.md). A request that waited longer in the
// queue (for example behind a block burst) is skipped: the reply would only cost airtime, and the tag
// has asked again already.
static uint32_t lateRequestsSkipped = 0;
static uint16_t statusLateSkipped   = 0;  // the same, since the last STATUS
static bool tooLateToAnswer(uint8_t type, int64_t ageUs) {
    switch (type) {
        case PKT_PING:
        case PKT_AVAIL_DATA_REQ:
        case PKT_AVAIL_DATA_SHORTREQ:
        case PKT_XFER_COMPLETE:
            return ageUs > 5000;
        case PKT_BLOCK_REQUEST:
        case PKT_BLOCK_PARTIAL_REQUEST:
            return ageUs > 7000;
        default:
            return false;
    }
}

// The tag inventory lock (V2_CFG_LOCK_MASK in SET_CONFIG). Locked, the radio answers only the tags on
// the allow-list the S3 keeps here (its tag database); learning, also a full check-in of a tag that
// just booted. Other tags get no reply, so they look for another AP, and the S3 hears nothing of them.
#define MAX_ALLOWED_MACS        1024
#define WAKEUP_REASON_FIRSTBOOT 0xFC
static uint64_t allowedMacs[MAX_ALLOWED_MACS];  // the 8 MAC bytes as received, compared as one value
static uint16_t allowedCount  = 0;
static uint8_t  lockMode      = V2_LOCK_OFF;
static uint32_t lockedIgnored = 0;  // frames from other tags, in the last minute

static int32_t findAllowed(uint64_t mac) {
    for (uint16_t c = 0; c < allowedCount; c++) {
        if (allowedMacs[c] == mac) return c;
    }
    return -1;
}
static bool allowAdd(const uint8_t *src) {
    uint64_t mac;
    memcpy(&mac, src, 8);
    if (findAllowed(mac) != -1) return true;
    if (allowedCount >= MAX_ALLOWED_MACS) return false;
    allowedMacs[allowedCount++] = mac;
    return true;
}
static void allowRemove(const uint8_t *src) {
    uint64_t mac;
    memcpy(&mac, src, 8);
    const int32_t c = findAllowed(mac);
    if (c != -1) allowedMacs[c] = allowedMacs[--allowedCount];
}
// May the radio answer this frame (len bytes, of packet type type)?
static bool tagAccepted(uint8_t type, const uint8_t *frame, int32_t len) {
    if (lockMode == V2_LOCK_OFF) return true;
    const struct MacFcs *fcs = (const struct MacFcs *) frame;
    const uint8_t       *src = (fcs->destAddrType == 2) ? ((const struct MacFrameBcast *) frame)->src : ((const struct MacFrameNormal *) frame)->src;
    uint64_t             mac;
    memcpy(&mac, src, 8);
    if (findAllowed(mac) != -1) return true;
    if (lockMode == V2_LOCK_LEARNING && type == PKT_AVAIL_DATA_REQ && (len == 28 || len == 40)) {
        const struct AvailDataReq *adr = (const struct AvailDataReq *) (frame + sizeof(struct MacFrameBcast) + 1);
        return adr->wakeupReason == WAKEUP_REASON_FIRSTBOOT;
    }
    return false;
}

int32_t findSlotForMac(const uint8_t *mac) {
    for (uint8_t c = 0; c < MAX_PENDING_MACS; c++) {
        if (memcmp(mac, ((uint8_t *) &(pendingDataArr[c].targetMac)), 8) == 0) {
            if (pendingDataArr[c].attemptsLeft != 0) {
                return c;
            }
        }
    }
    return -1;
}
int32_t findFreeSlot() {
    for (uint8_t c = 0; c < MAX_PENDING_MACS; c++) {
        if (pendingDataArr[c].attemptsLeft == 0) {
            return c;
        }
    }
    return -1;
}
int32_t findSlotForVer(const uint8_t *ver) {
    for (uint8_t c = 0; c < MAX_PENDING_MACS; c++) {
        if (memcmp(ver, ((uint8_t *) &(pendingDataArr[c].availdatainfo.dataVer)), 8) == 0) {
            if (pendingDataArr[c].attemptsLeft != 0) return c;
        }
    }
    return -1;
}
void deleteAllPendingDataForVer(const uint8_t *ver) {
    int32_t slot = -1;
    do {
        slot = findSlotForVer(ver);
        if (slot != -1) pendingDataArr[slot].attemptsLeft = 0;
    } while (slot != -1);
}
// Cached blocks of data that no pending entry offers any more
static void dropUnusedData(uint64_t ver) {
    if (ver != 0 && findSlotForVer((const uint8_t *) &ver) == -1) cacheDropVer(ver);
}
void deleteAllPendingDataForMac(const uint8_t *mac) {
    int32_t slot = -1;
    do {
        slot = findSlotForMac(mac);
        if (slot != -1) {
            pendingDataArr[slot].attemptsLeft = 0;
            dropUnusedData(pendingDataArr[slot].availdatainfo.dataVer);
        }
    } while (slot != -1);
}

void countSlots() {
    curPendingData = 0;
    curNoUpdate    = 0;
    for (uint8_t c = 0; c < MAX_PENDING_MACS; c++) {
        if (pendingDataArr[c].attemptsLeft != 0) {
            if (pendingDataArr[c].availdatainfo.dataType != 0) {
                curPendingData++;
            } else {
                curNoUpdate++;
            }
        }
    }
}

// capabilities, for CAP> and HELLO_REPLY
static uint16_t radioCaps() {
    uint16_t caps = V2_CAP_FRAMING | V2_CAP_BLOCK_CACHE | V2_CAP_TAG_RETURN_DATA | V2_CAP_ALLOW_LIST;
#ifdef CONFIG_OEPL_SUBGIG_SUPPORT
    if (curSubGhzChannel != NO_SUBGHZ_CHANNEL) caps |= V2_CAP_SUBGHZ;
#endif
    return caps;
}

static uint8_t subGhzChannelForS3() {
#ifdef CONFIG_OEPL_SUBGIG_SUPPORT
    if (curSubGhzChannel != NO_SUBGHZ_CHANNEL) return curSubGhzChannel;
#endif
    return V2_NO_SUBGHZ_CHANNEL;
}

void espNotifyAPInfo() {
    pr("TYP>%02X", HW_TYPE);
    pr("VER>%04X", version);
    pr("MAC>%02X%02X", mSelfMac[0], mSelfMac[1]);
    pr("%02X%02X", mSelfMac[2], mSelfMac[3]);
    pr("%02X%02X", mSelfMac[4], mSelfMac[5]);
    pr("%02X%02X", mSelfMac[6], mSelfMac[7]);
    pr("ZCH>%02X", curChannel);
#ifdef CONFIG_OEPL_SUBGIG_SUPPORT
    pr("SCH>%03d",curSubGhzChannel);
#endif
    pr("ZPW>%02X", curPower);
    countSlots();
    pr("PEN>%02X", curPendingData);
    pr("NOP>%02X", curNoUpdate);
    pr("CAP>%04X", radioCaps());
}

bool isSame(uint8_t *in1, char *in2, int len) {
    bool flag = 1;
    for (int i = 0; i < len; i++) {
        if (in1[i] != in2[i]) flag = 0;
    }
    return flag;
}

// The ASCII protocol, before FRM1: only what the S3 needs to recognise this radio and switch it to
// v2 (oepl-radio-v2.h). Older S3 firmware, which needs the full ASCII protocol, is not supported.
// Returns true once the link switched to v2; the rest of the buffer then belongs to the old speed.
static bool processAscii(uint8_t lastchar) {
    static uint8_t cmdbuffer[4];
    for (uint8_t c = 0; c < 3; c++) {
        cmdbuffer[c] = cmdbuffer[c + 1];
    }
    cmdbuffer[3] = lastchar;

    if (isSame(cmdbuffer, "NFO?", 4)) {
        pr("ACK>");
        espNotifyAPInfo();
    }
    if (isSame(cmdbuffer, "RDY?", 4)) {
        pr("ACK>");
    }
    if (isSame(cmdbuffer, "FRM1", 4)) {
        memset(cmdbuffer, 0, sizeof(cmdbuffer));
        pr("ACK>");
        uart_wait_tx_done(1, pdMS_TO_TICKS(20));
        delay(V2_SWITCH_DELAY_MS);
        uart_switch_speed(V2_BAUD);
        uartFlushInput();
        v2Start();
        ESP_LOGW(TAG, "protocol v2");
        return true;
    }
    return false;
}

// --- requests from the S3 (v2) ------------------------------------------------------------------

static void nak(struct v2Reply *reply, uint8_t code) {
    reply->type    = V2_NAK;
    reply->len     = 1;
    reply->data[0] = code;
}

static void fillStatus(struct v2Reply *reply) {
    struct v2Status st = {0};
    st.uptimeMs        = getMillis();
    st.channel         = curChannel;
    st.subghzChannel   = subGhzChannelForS3();
    st.txPowerDbm      = (int8_t) curPower;
    st.lastResetReason = (uint8_t) esp_reset_reason();
    countSlots();
    st.pending             = curPendingData;
    st.pendingIdle         = curNoUpdate;
    st.cacheUsedKb         = cacheUsedKb();
    st.rxFrames            = radioRxFrames;
    st.txFrames            = radioTxFrames;
    st.lateRequestsSkipped = statusLateSkipped;
    st.serialCrcErrors     = v2CrcErrors;
    st.serialOverflows     = v2Overflows + (uint16_t) uartOverflows;
    st.retransmits         = v2Retransmits;
    st.eventsQueued        = v2EventsQueued();
    statusLateSkipped      = 0;
    reply->type            = V2_STATUS;
    reply->len             = sizeof(st);
    memcpy(reply->data, &st, sizeof(st));
}

static void fillHelloReply(struct v2Reply *reply) {
    struct v2HelloReply hr = {0};
    hr.protoMajor   = V2_PROTO_MAJOR;
    hr.protoMinor   = V2_PROTO_MINOR;
    hr.fwVersion    = version;
    hr.hwType       = HW_TYPE;
    memcpy(hr.mac, mSelfMac, 8);
    hr.caps         = radioCaps();
    hr.maxPayload   = V2_MAX_PAYLOAD;
    hr.cacheKb      = cacheKb();
    hr.pendingSlots = MAX_PENDING_MACS;
    reply->type     = V2_HELLO_REPLY;
    reply->len      = sizeof(hr);
    memcpy(reply->data, &hr, sizeof(hr));
}

static void setConfig(const struct v2SetConfig *sc, struct v2Reply *reply) {
    if (sc->channel != V2_CFG_UNCHANGED_U8) {
        bool valid = false;
        for (uint8_t c = 0; c < sizeof(channelList); c++) {
            if (channelList[c] == sc->channel) valid = true;
        }
        if (!valid) {
            nak(reply, V2_NAK_INVALID_PARAM);
            return;
        }
    }
#ifdef CONFIG_OEPL_SUBGIG_SUPPORT
    if (sc->subghzChannel != V2_CFG_UNCHANGED_U8 && curSubGhzChannel != NO_SUBGHZ_CHANNEL && curSubGhzChannel != sc->subghzChannel) {
        curSubGhzChannel = sc->subghzChannel;
        SubGig_radioSetChannel(sc->subghzChannel);
    }
#endif
    if (sc->channel != V2_CFG_UNCHANGED_U8 && curChannel != sc->channel) {
        radioSetChannel(sc->channel);
        curChannel = sc->channel;
    }
    led_set_brightness(sc->ledBrightness);
    if (sc->txPowerDbm != V2_CFG_UNCHANGED_POWER) {
        curPower = (uint8_t) sc->txPowerDbm;
        radioSetTxPower(curPower);
    }
    lockMode = (sc->flags & V2_CFG_LOCK_MASK) >> V2_CFG_LOCK_SHIFT;
    ESP_LOGI(TAG, "Set channel: %d power: %d lock: %d (%d tags allowed)", curChannel, curPower, lockMode, allowedCount);
}

static void setPending(const struct v2SetPending *sp, struct v2Reply *reply) {
    if (sp->ttlMin == 0) {
        nak(reply, V2_NAK_INVALID_PARAM);
        return;
    }
    int32_t slot = findSlotForMac(sp->mac);
    if (slot == -1) slot = findFreeSlot();
    if (slot == -1) {
        nak(reply, V2_NAK_NO_SLOT);
        return;
    }
    struct pendingData *pd             = &pendingDataArr[slot];
    const uint64_t      oldVer         = pd->attemptsLeft ? pd->availdatainfo.dataVer : 0;
    memset(pd, 0, sizeof(struct pendingData));
    pd->availdatainfo.dataVer          = sp->dataVer;
    pd->availdatainfo.dataSize         = sp->dataSize;
    pd->availdatainfo.dataType         = sp->dataType;
    pd->availdatainfo.dataTypeArgument = sp->dataTypeArg;
    pd->availdatainfo.nextCheckIn      = sp->nextCheckin;
    pd->attemptsLeft                   = sp->ttlMin;
    memcpy(pd->targetMac, sp->mac, 8);
    if (oldVer != sp->dataVer) dropUnusedData(oldVer);
}

// Block data for the cache: what a tag asked for (DATA_NEEDED), or data the S3 sends in advance
static void dataChunk(const struct v2DataChunk *dc, size_t n, struct v2Reply *reply) {
    const bool awaited = blockStartTimer && dc->dataVer == requestedData.ver && dc->offset / V2_BLOCK_SIZE == requestedData.blockId &&
                         !cacheComplete(cacheFind(requestedData.ver, requestedData.blockId));
    const int slot = cacheChunk(dc->dataVer, dc->offset, dc->totalSize, dc->bytes, n);
    if (slot == CACHE_FULL) {
        nak(reply, V2_NAK_NO_CACHE_SPACE);
    } else if (slot == CACHE_BAD_CHUNK) {
        nak(reply, V2_NAK_INVALID_PARAM);
    } else if (awaited && cacheComplete(slot)) {
        ESP_LOGI(TAG, "block data complete, %lu ms after the request", getMillis() - nextBlockAttempt);
    }
}

void v2HandleRequest(uint8_t type, const uint8_t *payload, size_t len, struct v2Reply *reply) {
    switch (type) {
        case V2_HELLO:
            if (len < sizeof(struct v2Hello)) {
                nak(reply, V2_NAK_BAD_LENGTH);
            } else if (((const struct v2Hello *) payload)->protoMajor != V2_PROTO_MAJOR) {
                nak(reply, V2_NAK_INVALID_PARAM);
            } else {
                fillHelloReply(reply);
            }
            break;
        case V2_HEARTBEAT:
        case V2_GET_STATS:
            fillStatus(reply);
            break;
        case V2_SET_CONFIG:
            if (len < sizeof(struct v2SetConfig)) nak(reply, V2_NAK_BAD_LENGTH);
            else setConfig((const struct v2SetConfig *) payload, reply);
            break;
        case V2_SET_PENDING:
            if (len < sizeof(struct v2SetPending)) nak(reply, V2_NAK_BAD_LENGTH);
            else setPending((const struct v2SetPending *) payload, reply);
            break;
        case V2_CANCEL_PENDING:
            if (len < sizeof(struct v2CancelPending)) nak(reply, V2_NAK_BAD_LENGTH);
            else deleteAllPendingDataForMac(((const struct v2CancelPending *) payload)->mac);
            break;
        case V2_DATA_CHUNK:
            if (len <= sizeof(struct v2DataChunk) || len > sizeof(struct v2DataChunk) + V2_CHUNK_MAX) nak(reply, V2_NAK_BAD_LENGTH);
            else dataChunk((const struct v2DataChunk *) payload, len - sizeof(struct v2DataChunk), reply);
            break;
        case V2_DATA_DROP:
            if (len < sizeof(struct v2DataDrop)) nak(reply, V2_NAK_BAD_LENGTH);
            else cacheDropVer(((const struct v2DataDrop *) payload)->dataVer);
            break;
        case V2_ALLOW_CLEAR:
            allowedCount = 0;
            break;
        case V2_ALLOW_ADD:
        case V2_ALLOW_REMOVE:
            if (len == 0 || len % 8 || len / 8 > V2_ALLOW_MAX_MACS) {
                nak(reply, V2_NAK_BAD_LENGTH);
                break;
            }
            for (size_t c = 0; c < len; c += 8) {
                if (type == V2_ALLOW_REMOVE) {
                    allowRemove(payload + c);
                } else if (!allowAdd(payload + c)) {
                    nak(reply, V2_NAK_NO_SLOT);
                    break;
                }
            }
            break;
        default:
            nak(reply, V2_NAK_UNKNOWN_TYPE);
            break;
    }
}

// --- events to the S3 ---------------------------------------------------------------------------

// Asks the S3 for the block the tag requested; requestedData holds the request
static void requestBlockData(const uint8_t *src, uint16_t pleaseWaitMs) {
    struct v2DataNeeded dn;
    memcpy(dn.mac, src, 8);
    dn.dataVer = requestedData.ver;
    dn.offset  = (uint32_t) requestedData.blockId * V2_BLOCK_SIZE;
    dn.length  = V2_BLOCK_SIZE;
    int32_t slot = findSlotForMac(src);
    if (slot != -1) {
        const uint32_t size = pendingDataArr[slot].availdatainfo.dataSize;
        if (size > dn.offset && size - dn.offset < V2_BLOCK_SIZE) dn.length = size - dn.offset;
    }
    dn.pleaseWaitMs = pleaseWaitMs;
    v2QueueEvent(V2_DATA_NEEDED, &dn, sizeof(dn), true);
}

static void notifyXferEnd(uint8_t type, const uint8_t *src, uint64_t ver) {
    struct v2XferEnd xe;
    memcpy(xe.mac, src, 8);
    xe.dataVer = ver;
    v2QueueEvent(type, &xe, sizeof(xe), false);
}

void espNotifyTagReturnData(uint8_t *src, uint8_t len) {
    struct tagReturnData *trd = (struct tagReturnData *)(radiorxbuffer + sizeof(struct MacFrameBcast) + 1);

    if (memcmp((void *) & trd->dataVer, lastTagReturn, 8) == 0) {
        return;
    } else {
        memcpy(lastTagReturn, &trd->dataVer, 8);
    }

    if (len > sizeof(struct tagReturnData)) len = sizeof(struct tagReturnData);
    uint8_t buf[sizeof(struct v2TagReturnData) + sizeof(struct tagReturnData)];
    struct v2TagReturnData *e = (struct v2TagReturnData *) buf;
    memcpy(e->mac, src, 8);
    e->timestampUs = (uint32_t) lastRxSfdUs;
    e->rssi        = lastRxRssi;
    e->lqi         = lastRxLqi;
    memcpy(e->data, trd, len);
    v2QueueEvent(V2_TAG_RETURN_DATA, buf, sizeof(struct v2TagReturnData) + len, false);
}

// process data from tag
void processBlockRequest(const uint8_t *buffer) {
    struct MacFrameNormal *rxHeader = (struct MacFrameNormal *) buffer;
    struct blockRequest   *blockReq = (struct blockRequest *) (buffer + sizeof(struct MacFrameNormal) + 1);
    if (!checkCRC(blockReq, sizeof(struct blockRequest))) return;

    // the tag asks again: it has given up on the burst that is still being sent
    if (burstActive && memcmp(rxHeader->src, dstMac, 8) == 0) {
        burstActive = false;
        cachePin(-1);
    }

    // check if we're already talking to this mac
    if (memcmp(rxHeader->src, lastBlockMac, 8) == 0) {
        lastBlockRequest = getMillis();
    } else {
        // we weren't talking to this mac, see if there was a transfer in progress from another mac, recently
        if ((getMillis() - lastBlockRequest) > CONCURRENT_REQUEST_DELAY) {
            // mark this mac as the new current mac we're talking to
            memcpy((void *) lastBlockMac, (void *) rxHeader->src, 8);
            lastBlockRequest = getMillis();
        } else {
            // we're talking to another mac, let this mac know we can't accomodate another request right now
            ESP_LOGI(TAG, "busy with another tag, cancel");
            sendCancelXfer(rxHeader->src, rxHeader->pan);
            return;
        }
    }

    // check if we have data for this mac
    if (findSlotForMac(rxHeader->src) == -1) {
        // no data for this mac, politely tell it to fuck off
        sendCancelXfer(rxHeader->src, rxHeader->pan);
        return;
    }

    // The block is in the cache, on its way for an earlier request of this block, or has to come from
    // the S3. A cached block is checked chunk by chunk (CRC16 on the serial line), so a full request
    // for it no longer forces a new download.
    const bool sameBlock = blockReq->blockId == requestedData.blockId && blockReq->ver == requestedData.ver;
    const bool cached    = cacheComplete(cacheFind(blockReq->ver, blockReq->blockId));
    const bool coming    = !cached && sameBlock && (getMillis() - nextBlockAttempt) <= 300;

    // copy blockrequest into requested data
    memcpy(&requestedData, blockReq, sizeof(struct blockRequest));

    struct MacFrameNormal  *txHeader                 = (struct MacFrameNormal *) (radiotxbuffer + 1);
    struct blockRequestAck *blockRequestAck          = (struct blockRequestAck *) (radiotxbuffer + sizeof(struct MacFrameNormal) + 2);
    radiotxbuffer[0]                                 = sizeof(struct MacFrameNormal) + 1 + sizeof(struct blockRequestAck) + RAW_PKT_PADDING;
    radiotxbuffer[sizeof(struct MacFrameNormal) + 1] = PKT_BLOCK_REQUEST_ACK;

    blockRequestAck->pleaseWaitMs = cached ? PLEASE_WAIT_CACHED_MS : coming ? PLEASE_WAIT_COMING_MS : PLEASE_WAIT_DOWNLOAD_MS;
    blockStartTimer = getMillis() + blockRequestAck->pleaseWaitMs + BURST_MARGIN_MS;

    memcpy(txHeader->src, mSelfMac, 8);
    memcpy(txHeader->dst, rxHeader->src, 8);

    txHeader->pan                 = rxHeader->pan;
    txHeader->fcs.frameType       = 1;
    txHeader->fcs.panIdCompressed = 1;
    txHeader->fcs.destAddrType    = 3;
    txHeader->fcs.srcAddrType     = 3;
    txHeader->seq                 = seq++;

    addCRC((void *) blockRequestAck, sizeof(struct blockRequestAck));

    radioTx(radiotxbuffer);

    // save the target for the blockdata
    memcpy(dstMac, rxHeader->src, 8);
    dstPan = rxHeader->pan;

    if (!cached && !coming) {
        requestBlockData(rxHeader->src, blockRequestAck->pleaseWaitMs);
        nextBlockAttempt = getMillis();
    }
}

void processAvailDataReq(uint8_t *buffer, uint8_t kind) {
    struct MacFrameBcast *rxHeader     = (struct MacFrameBcast *) buffer;
    struct AvailDataReq  *availDataReq = (struct AvailDataReq *) (buffer + sizeof(struct MacFrameBcast) + 1);

    if (!checkCRC(availDataReq, sizeof(struct AvailDataReq))) return;

    // prepare tx buffer to send a response
    memset(radiotxbuffer, 0, sizeof(struct MacFrameNormal) + sizeof(struct AvailDataInfo) + 2);  // 120);
    struct MacFrameNormal *txHeader                  = (struct MacFrameNormal *) (radiotxbuffer + 1);
    struct AvailDataInfo  *availDataInfo             = (struct AvailDataInfo *) (radiotxbuffer + sizeof(struct MacFrameNormal) + 2);
    radiotxbuffer[0]                                 = sizeof(struct MacFrameNormal) + 1 + sizeof(struct AvailDataInfo) + RAW_PKT_PADDING;
    radiotxbuffer[sizeof(struct MacFrameNormal) + 1] = PKT_AVAIL_DATA_INFO;

    // check to see if we have data available for this mac
    bool haveData = false;
    for (uint8_t c = 0; c < MAX_PENDING_MACS; c++) {
        if (pendingDataArr[c].attemptsLeft) {
            if (memcmp(pendingDataArr[c].targetMac, rxHeader->src, 8) == 0) {
                haveData = true;
                memcpy((void *) availDataInfo, &(pendingDataArr[c].availdatainfo), sizeof(struct AvailDataInfo));
                break;
            }
        }
    }

    // couldn't find data for this mac
    if (!haveData) availDataInfo->dataType = DATATYPE_NOUPDATE;

    memcpy(txHeader->src, mSelfMac, 8);
    memcpy(txHeader->dst, rxHeader->src, 8);
    txHeader->pan                 = rxHeader->dstPan;
    txHeader->fcs.frameType       = 1;
    txHeader->fcs.panIdCompressed = 1;
    txHeader->fcs.destAddrType    = 3;
    txHeader->fcs.srcAddrType     = 3;
    txHeader->seq                 = seq++;
    addCRC(availDataInfo, sizeof(struct AvailDataInfo));
    radioTx(radiotxbuffer);
    memset(lastAckMac, 0, 8);  // reset lastAckMac, so we can record if we've received exactly one ack packet

    struct v2Checkin ci = {0};
    memcpy(ci.mac, rxHeader->src, 8);
    ci.timestampUs  = (uint32_t) lastRxSfdUs;
    ci.rssi         = lastRxRssi;
    ci.lqi          = lastRxLqi;
    ci.radio        = (rxHeader->srcPan == PROTO_PAN_ID_SUBGHZ) ? V2_RADIO_SUBGHZ : V2_RADIO_24GHZ;
    ci.channel      = (ci.radio == V2_RADIO_SUBGHZ) ? subGhzChannelForS3() : curChannel;
    ci.pan          = rxHeader->dstPan;
    ci.kind         = kind;
    ci.answeredVer  = availDataInfo->dataVer;
    ci.answeredType = availDataInfo->dataType;
    memcpy(ci.adr, availDataReq, sizeof(struct AvailDataReq));
    v2QueueEvent(V2_CHECKIN, &ci, sizeof(ci), false);
}
void processXferComplete(uint8_t *buffer) {
    struct MacFrameNormal *rxHeader = (struct MacFrameNormal *) buffer;
    sendXferCompleteAck(rxHeader->src, rxHeader->pan);
    if (memcmp(lastAckMac, rxHeader->src, 8) != 0) {
        memcpy((void *) lastAckMac, (void *) rxHeader->src, 8);
        int32_t slot = findSlotForMac(rxHeader->src);
        notifyXferEnd(V2_XFER_COMPLETE, rxHeader->src, slot != -1 ? pendingDataArr[slot].availdatainfo.dataVer : 0);
        if (slot != -1) {
            pendingDataArr[slot].attemptsLeft = 0;
            dropUnusedData(pendingDataArr[slot].availdatainfo.dataVer);
        }
    }
}

void processTagReturnData(uint8_t *buffer, uint8_t len) {
    struct MacFrameBcast *rxframe = (struct MacFrameBcast *)buffer;
    struct MacFrameNormal *frameHeader = (struct MacFrameNormal *)(radiotxbuffer + 1);

    if (!checkCRC((buffer + sizeof(struct MacFrameBcast) + 1), len - (sizeof(struct MacFrameBcast) + 1))) {
        return;
    }
    radiotxbuffer[sizeof(struct MacFrameNormal) + 1] = PKT_TAG_RETURN_DATA_ACK;
    radiotxbuffer[0] = sizeof(struct MacFrameNormal) + 1 + RAW_PKT_PADDING;
    memcpy(frameHeader->src, mSelfMac, 8);
    memcpy(frameHeader->dst, rxframe->src, 8);
    radiotxbuffer[1] = 0x41; // fast way to set the appropriate bits
    radiotxbuffer[2] = 0xCC; // normal frame
    frameHeader->seq = seq++;
    frameHeader->pan = rxframe->srcPan;
    radioTx(radiotxbuffer);

    espNotifyTagReturnData(rxframe->src, len - (sizeof(struct MacFrameBcast) + 1));
}

// send block data to the tag
void sendPart(uint8_t partNo) {
    struct MacFrameNormal *frameHeader = (struct MacFrameNormal *) (radiotxbuffer + 1);
    struct blockPart      *blockPart   = (struct blockPart *) (radiotxbuffer + sizeof(struct MacFrameNormal) + 2);
    memset(radiotxbuffer + 1, 0, sizeof(struct blockPart) + sizeof(struct MacFrameNormal));
    radiotxbuffer[sizeof(struct MacFrameNormal) + 1] = PKT_BLOCK_PART;
    radiotxbuffer[0]                                 = sizeof(struct MacFrameNormal) + sizeof(struct blockPart) + BLOCK_PART_DATA_SIZE + 1 + RAW_PKT_PADDING;
    memcpy(frameHeader->src, mSelfMac, 8);
    memcpy(frameHeader->dst, dstMac, 8);
    blockPart->blockId   = requestedData.blockId;
    blockPart->blockPart = partNo;
    memcpy(&(blockPart->data), cacheBuffer(burstSlot) + (partNo * BLOCK_PART_DATA_SIZE), BLOCK_PART_DATA_SIZE);
    addCRC(blockPart, sizeof(struct blockPart) + BLOCK_PART_DATA_SIZE);
    frameHeader->fcs.frameType       = 1;
    frameHeader->fcs.panIdCompressed = 1;
    frameHeader->fcs.destAddrType    = 3;
    frameHeader->fcs.srcAddrType     = 3;
    frameHeader->seq                 = seq++;
    frameHeader->pan                 = dstPan;
    radioTx(radiotxbuffer);
}
static void startBurst(int slot, int32_t lateMs) {
    if (getBlockDataLength() == 0) {
        ESP_LOGW(TAG, "Invalid block request received, 0 parts");
        requestedData.requestedParts[0] |= 0x01;
    }

    // Each requested part once, then repeated until twice the number of requested parts (at most 42
    // frames). Every burst used to be filled up to 42 frames: for a partial request of one part that
    // kept the radio busy for 200 ms after the tag had its data, while other tags waited.
    uint8_t requested = getBlockDataLength();
    uint8_t frames = (requested * 2 > BLOCK_MAX_PARTS) ? BLOCK_MAX_PARTS : requested * 2;
    if (frames < requested) frames = requested;
    if (dstPan == PROTO_PAN_ID_SUBGHZ) {
        // Don't repeat for subgig, it requests what it can handle with its limited RAM
        frames = requested;
    }
    burstLen = 0;
    while (burstLen < frames) {
        for (uint8_t c = 0; (c < BLOCK_MAX_PARTS) && (burstLen < frames); c++) {
            if (requestedData.requestedParts[c / 8] & (1 << (c % 8))) {
                burstParts[burstLen++] = c;
            }
        }
    }
    burstPos    = 0;
    burstLateMs = (int16_t) (lateMs + BURST_MARGIN_MS);  // against the promised pleaseWaitMs
    burstSlot   = slot;
    cachePin(slot);
    cacheTouch(slot);
    burstActive = true;
}

// Sends the next part of the burst once the radio is free
static void burstStep() {
    if (!burstActive || radioTxBusy()) return;
    sendPart(burstParts[burstPos++]);
    if (burstPos < burstLen) return;
    burstActive = false;
    cachePin(-1);
    struct v2BlockServed bs;
    memcpy(bs.mac, dstMac, 8);
    bs.dataVer        = requestedData.ver;
    bs.blockId        = requestedData.blockId;
    bs.partsRequested = getBlockDataLength();
    bs.framesSent     = burstLen;
    bs.lateMs         = burstLateMs;
    v2QueueEvent(V2_BLOCK_SERVED, &bs, sizeof(bs), false);
}
void sendXferCompleteAck(uint8_t *dst, uint16_t pan) {
    struct MacFrameNormal *frameHeader = (struct MacFrameNormal *) (radiotxbuffer + 1);
    memset(radiotxbuffer + 1, 0, sizeof(struct blockPart) + sizeof(struct MacFrameNormal));
    radiotxbuffer[sizeof(struct MacFrameNormal) + 1] = PKT_XFER_COMPLETE_ACK;
    radiotxbuffer[0]                                 = sizeof(struct MacFrameNormal) + 1 + RAW_PKT_PADDING;
    memcpy(frameHeader->src, mSelfMac, 8);
    memcpy(frameHeader->dst, dst, 8);
    frameHeader->fcs.frameType       = 1;
    frameHeader->fcs.panIdCompressed = 1;
    frameHeader->fcs.destAddrType    = 3;
    frameHeader->fcs.srcAddrType     = 3;
    frameHeader->seq                 = seq++;
    frameHeader->pan                 = pan;  // the PAN (and so the radio) the tag used
    radioTx(radiotxbuffer);
}
void sendCancelXfer(uint8_t *dst, uint16_t pan) {
    struct MacFrameNormal *frameHeader = (struct MacFrameNormal *) (radiotxbuffer + 1);
    memset(radiotxbuffer + 1, 0, sizeof(struct blockPart) + sizeof(struct MacFrameNormal));
    radiotxbuffer[sizeof(struct MacFrameNormal) + 1] = PKT_CANCEL_XFER;
    radiotxbuffer[0]                                 = sizeof(struct MacFrameNormal) + 1 + RAW_PKT_PADDING;
    memcpy(frameHeader->src, mSelfMac, 8);
    memcpy(frameHeader->dst, dst, 8);
    frameHeader->fcs.frameType       = 1;
    frameHeader->fcs.panIdCompressed = 1;
    frameHeader->fcs.destAddrType    = 3;
    frameHeader->fcs.srcAddrType     = 3;
    frameHeader->seq                 = seq++;
    frameHeader->pan                 = pan;  // the PAN (and so the radio) the tag used
    radioTx(radiotxbuffer);
}
void sendPong(void *buf) {
    struct MacFrameBcast  *rxframe                   = (struct MacFrameBcast *) buf;
    struct MacFrameNormal *frameHeader               = (struct MacFrameNormal *) (radiotxbuffer + 1);
    radiotxbuffer[sizeof(struct MacFrameNormal) + 1] = PKT_PONG;
#ifdef CONFIG_OEPL_SUBGIG_SUPPORT
    if(rxframe->srcPan == PROTO_PAN_ID_SUBGHZ) {
       radiotxbuffer[sizeof(struct MacFrameNormal) + 2] = curSubGhzChannel;
    }
    else 
#endif
    radiotxbuffer[sizeof(struct MacFrameNormal) + 2] = curChannel;
    radiotxbuffer[0]                                 = sizeof(struct MacFrameNormal) + 1 + 1 + RAW_PKT_PADDING;
    memcpy(frameHeader->src, mSelfMac, 8);
    memcpy(frameHeader->dst, rxframe->src, 8);
    radiotxbuffer[1] = 0x41;  // fast way to set the appropriate bits
    radiotxbuffer[2] = 0xCC;  // normal frame
    frameHeader->seq = seq++;
    frameHeader->pan = rxframe->srcPan;
    radioTx(radiotxbuffer);
}

void app_main(void) {
    radioWakeTask = xTaskGetCurrentTaskHandle();
    esp_event_loop_create_default();
    
    init_nvs();
    init_led();
    init_second_uart();

    requestedData.blockId = 0xFF;
    // clear the array with pending information
    memset(pendingDataArr, 0, sizeof(pendingDataArr));

    radio_init(curChannel);
#ifdef CONFIG_OEPL_SUBGIG_SUPPORT
    if(SubGig_radio_init(curSubGhzChannel)) {
    // Ether we don't have a cc1101 or it's not working
       curSubGhzChannel = NO_SUBGHZ_CHANNEL; 
       ESP_LOGI(TAG,"CC1101 NOT detected.");
    }
    else {
       ESP_LOGI(TAG,"CC1101 detected.");
    }
#endif
    radioSetTxPower(10);
    cacheInit();  // last: takes what is left of the heap

    pr("RES>");
    pr("RDY>");
#ifdef CONFIG_IDF_TARGET_ESP32C6
    ESP_LOGI(TAG, "C6 ready!");
#else
    ESP_LOGI(TAG, "H2 ready!");
#endif

    housekeepingTimer = getMillis();
    while (1) {
        while ((getMillis() - housekeepingTimer) < ((1000 * HOUSEKEEPING_INTERVAL) - 100)) {
            int32_t ret = commsRxUnencrypted(radiorxbuffer);
            if (ret > 1) {
                led_flash(0);

                uint8_t PktType = getPacketType(radiorxbuffer);
#if CONFIG_OEPL_VERBOSE_DEBUG
                LOGV_RAW("Received %d byte ",ret);
                for(uint8_t i = 0; gPktTypeLookupTbl[i].Name != NULL; i++) {
                   if(gPktTypeLookupTbl[i].Type == PktType) {
                      LOGV_RAW("%s",gPktTypeLookupTbl[i].Name);
                      break;
                   }
                   if(gPktTypeLookupTbl[i].Name == NULL) {
                      LOGV_RAW("undefined (0x%02x)",PktType);
                   }
                }
                LOGV_RAW(" packet:\n");
                LOGV_HEX(radiorxbuffer,ret);
#endif
                if (tooLateToAnswer(PktType, lastRxAgeUs)) {
                    lateRequestsSkipped++;
                    statusLateSkipped++;
                    PktType = 0;  // falls through to default below without a reply
                }
                if (PktType && !tagAccepted(PktType, radiorxbuffer, ret)) {
                    lockedIgnored++;
                    PktType = 0;
                }
                // received a packet, lets see what it is
                switch (PktType) {
                    case PKT_AVAIL_DATA_REQ:
                        if (ret == 28) {
                            // old version of the AvailDataReq struct, set all the new fields to zero, so it will pass the CRC
                            memset(radiorxbuffer + 1 + sizeof(struct MacFrameBcast) + sizeof(struct oldAvailDataReq), 0,
                                   sizeof(struct AvailDataReq) - sizeof(struct oldAvailDataReq) + 2);
                            processAvailDataReq(radiorxbuffer, V2_CHECKIN_FULL);
                        } else if (ret == 40) {
                            // new version of the AvailDataReq struct
                            processAvailDataReq(radiorxbuffer, V2_CHECKIN_FULL);
                        }
                        break;
                    case PKT_BLOCK_REQUEST:
                        processBlockRequest(radiorxbuffer);
                        break;
                    case PKT_BLOCK_PARTIAL_REQUEST:
                        processBlockRequest(radiorxbuffer);
                        break;
                    case PKT_XFER_COMPLETE:
                        processXferComplete(radiorxbuffer);
                        break;
                    case PKT_PING:
                        sendPong(radiorxbuffer);
                        break;
                    case PKT_AVAIL_DATA_SHORTREQ:
                        // a short AvailDataReq is basically a very short (1 byte payload) packet that requires little preparation on the tx side, for optimal
                        // battery use bytes of the struct are set 0, so it passes the checksum test, and the ESP32 can detect that no interesting payload is
                        // sent
                        if (ret == 18) {
                            memset(radiorxbuffer + 1 + sizeof(struct MacFrameBcast), 0, sizeof(struct AvailDataReq) + 2);
                            processAvailDataReq(radiorxbuffer, V2_CHECKIN_SHORT);
                        }
                        break;
                    case PKT_TAG_RETURN_DATA:
                        processTagReturnData(radiorxbuffer, ret);
                        break;
                    case 0:
                        break;
                    default:
                        ESP_LOGI(TAG, "t=%02X" , getPacketType(radiorxbuffer));
                        break;
                }
            }

            uint8_t serialBuf[256];
            int     serialLen;
            while ((serialLen = uartRead(serialBuf, sizeof(serialBuf))) > 0) {
                if (v2Active()) {
                    v2RxBytes(serialBuf, serialLen);
                } else {
                    for (int i = 0; i < serialLen; i++) {
                        if (processAscii(serialBuf[i])) break;
                    }
                }
            }
            if (v2Active() && v2MsSinceLastFrame() > V2_LINK_LOST_MS) {
                ESP_LOGW(TAG, "no frames from the S3, back to the ASCII protocol");
                v2Stop();
                uart_switch_speed(115200);
                uartFlushInput();
            }

            if (blockStartTimer) {
                int32_t late = (int32_t)(getMillis() - blockStartTimer);
                if (late >= 0) {
                    const int slot = cacheFind(requestedData.ver, requestedData.blockId);
                    if (cacheComplete(slot)) {
                        startBurst(slot, late);
                        blockStartTimer = 0;
                    } else if (late > BLOCK_DATA_SLACK_MS) {
                        ESP_LOGW(TAG, "block data not complete in time, no burst");
                        blockStartTimer = 0;
                    }
                }
            }
            burstStep();
            v2Poll();

            if (ret <= 1) {
                // Nothing from the radio this round: sleep until a radio frame, serial data or the
                // end of a transmission arrives (all notify this task), or until a timer is due, at
                // most 100 ms (sub-GHz reception is still polled).
                uint32_t waitMs = 100;
                if (blockStartTimer) {
                    int32_t left = (int32_t)(blockStartTimer - getMillis()) + 1;
                    if (left <= 0) left = (int32_t)(blockStartTimer + BLOCK_DATA_SLACK_MS - getMillis()) + 1;  // waiting for data
                    waitMs = left > 0 ? (uint32_t)left : 0;
                    if (waitMs > 100) waitMs = 100;
                }
                const uint32_t v2Ms = v2MsUntilWork();
                if (v2Ms < waitMs) waitMs = v2Ms;
                if (burstActive) waitMs = radioTxBusy() ? 5 : 0;  // the TX-done interrupt wakes us
                if (waitMs) ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(waitMs));
            }
        }

        if (lateRequestsSkipped) {
            ESP_LOGW(TAG, "%lu requests skipped in the last minute: too late to answer", lateRequestsSkipped);
            lateRequestsSkipped = 0;
        }
        if (lockedIgnored) {
            ESP_LOGW(TAG, "%lu frames ignored in the last minute: tag not on the allow-list", lockedIgnored);
            lockedIgnored = 0;
        }
        memset(&lastTagReturn, 0, 8);
        for (uint8_t cCount = 0; cCount < MAX_PENDING_MACS; cCount++) {
            if (pendingDataArr[cCount].attemptsLeft == 1) {
                if (pendingDataArr[cCount].availdatainfo.dataType != DATATYPE_NOUPDATE) {
                    notifyXferEnd(V2_XFER_TIMEOUT, pendingDataArr[cCount].targetMac, pendingDataArr[cCount].availdatainfo.dataVer);
                }
                pendingDataArr[cCount].attemptsLeft = 0;
                dropUnusedData(pendingDataArr[cCount].availdatainfo.dataVer);
            } else if (pendingDataArr[cCount].attemptsLeft > 1) {
                pendingDataArr[cCount].attemptsLeft--;
                // Minutes count down; a value with bit 15 set is seconds and stays as it is.
                // Counting 0x8005 down reaches 0x8000 (the tag sleeps "forever") and 0x7FFF (22 days).
                uint16_t nextCheckIn = pendingDataArr[cCount].availdatainfo.nextCheckIn;
                if (nextCheckIn && !(nextCheckIn & 0x8000)) pendingDataArr[cCount].availdatainfo.nextCheckIn--;
            }
        }
        housekeepingTimer = getMillis();
    }
}
