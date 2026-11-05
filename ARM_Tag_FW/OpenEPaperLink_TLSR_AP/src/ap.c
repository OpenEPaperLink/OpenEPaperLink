// OpenEPaperLink access point radio firmware for the TLSR825x, 802.15.4 (hw type 0xF8).
// The AP logic of the ESP32-C6/H2 radio firmware (ARM_Tag_FW/OpenEPaperLink_esp32_C6_AP/main/main.c,
// version 0x0024, ported there from the ZBS243 AP by ATC1441), on the HAL of hal.h.

#include "ap.h"

#include <string.h>

#include "blockcache.h"
#include "hal.h"
#include "proto.h"
#include "v2link.h"

// AP_DEBUG is the only compile-time option in the code (the makefile's others only affect the link).
// It adds instruments: per-ping, per-join and per-check-in lines, the idle-noise histogram, and LOG
// frames sent whether or not the S3 asked for them. Nothing behind it changes what goes on the air.
#ifndef AP_DEBUG
#define AP_DEBUG 0
#endif

static const uint8_t channelList[6] = {11, 15, 20, 25, 26, 27};

#define DATATYPE_NOUPDATE 0

// Set by HELLO: restart the loop timing marks (lp, in), so the link bring-up pass does not mask them.
static bool loopTimingRestart = false;

#define RAW_PKT_PADDING 2

// The frame being handled. rx.data is in the radio's receive slot, valid only until apLoop() releases
// it (halRadioRxDone); the rest of rx is a copy. Replies are built in the TX buffer (prepareNormalFrame).
static struct halRxFrame rx;
static uint8_t mSelfMac[8];

static uint32_t housekeepingTimer;

static struct blockRequest requestedData;  // holds which data was requested by the tag

static uint8_t dstMac[8];  // target for the block transfer
static uint16_t dstPan;

static uint32_t blockStartTimer = 0;  // when the AP sends the next block (0 = no burst due)
// When a burst starts, the same for every tag. A tag's firmware decides how it spends a wait, and that
// differs by chip and by version, so the radio does not try to know: it aims past the latest of them.
//
// A block in the cache goes out at once, with a wait of 0: every family then receives straight away,
// so the tag spends no time waiting, whatever its firmware. An nRF52811 before firmware 0x002D
// restarts its radio after a wait of 0, which is what the hold is for; an EFR32 restarts its receiver
// too, on a timer of 0 ms.
#define ZERO_WAIT_HOLD_MS 3  // that restart's 0.5-2 ms, rounded up; an estimate, not a measurement
// Any other wait: a tag sleeps or busy-waits through it, listening before its end or as it ends.
// An Opticon's radio is off 40 ms longer, so it can miss the first frames; the repeats and a
// re-request, then with a wait of 0, make up for those.
#define BURST_MARGIN_MS 5

// A block part on air: the MAC header, the packet type, the blockPart header and its data, plus the
// FCS the radio adds; 4 bytes of preamble, the SFD and the length byte come first, 32 us per byte.
#define BURST_FRAME_BYTES (sizeof(struct MacFrameNormal) + 1 + sizeof(struct blockPart) + BLOCK_PART_DATA_SIZE + 2)
// ... plus the TX settle before each frame (the driver's ZB_TX_WAIT_US)
#define RADIO_TX_SETTLE_US 120
#define BURST_FRAME_US ((((uint32_t)BURST_FRAME_BYTES + 6) * 32) + RADIO_TX_SETTLE_US)
// A tag listens for block parts for 315 ms (ZBS243) or 300 ms (nRF52811, TLSR8258); the burst fits in
// the shortest, with room for the tag's clock and for a frame still going out when it ends.
#define BURST_WINDOW_MS 260
#define TAG_BLOCK_WINDOW_MS 300  // that shortest window itself: what a further request round costs a tag
#define BURST_MAX_FRAMES ((BURST_WINDOW_MS * 1000UL) / (BURST_FRAME_US + RADIO_BURST_GAP_US))
#define BURST_SLOTS (BURST_MAX_FRAMES > BLOCK_MAX_PARTS ? BURST_MAX_FRAMES : BLOCK_MAX_PARTS)
// The longest frame 802.15.4 allows, on air: a tag's frame the receiver has started on is over by then
#define RADIO_MAX_FRAME_US ((HAL_RADIO_MAX_FRAME + 6) * 32UL)
// What a lost request attempt costs a tag: the reply window it listens through (5 ms for a check-in)
#define TAG_ATTEMPT_MS 5

// Copies of each requested part per burst, to start with; the tag's window is the only ceiling.
#define BURST_COPIES 2

// Repeats beyond one copy of each part cost other tags: while the radio transmits it is deaf to their
// requests. burstCopiesAdapt() weighs both sides in milliseconds of a tag's radio time:
//   harm     other tags' request attempts our burst frames went out over (TAG_ATTEMPT_MS each),
//            estimated from the frames heard while listening, plus requests heard too late to answer
//   benefit  re-requests after a burst, each a further TAG_BLOCK_WINDOW_MS with the tag's receiver on
// No signal changes nothing, so silence never grows the burst.
static uint8_t burstCopies = BURST_COPIES;

// The burst only grows while channel occupancy (percent) is below this.
#define BURST_BUSY_HOLD_PCT 25

static void burstCopiesAdapt(uint16_t harmAttempts, uint16_t rereqsThisWindow, uint8_t busyPct) {
    const uint32_t harmMs = (uint32_t)harmAttempts * TAG_ATTEMPT_MS;
    const uint32_t benefitMs = (uint32_t)rereqsThisWindow * TAG_BLOCK_WINDOW_MS;
    if (harmMs > benefitMs) {
        if (burstCopies > 1) burstCopies--;  // we are costing other tags more than the repeats save
    } else if (benefitMs > harmMs && burstCopies < BURST_MAX_FRAMES && busyPct < BURST_BUSY_HOLD_PCT) {
        // but not on a busy channel, where more frames would add to the losses
        burstCopies++;
    }
}
// --- reply timing -------------------------------------------------------------------------------
// A reply is scheduled, not sent: halRadioTxReply() has the radio start the preamble a fixed time
// after the end of the tag's request, however long the processing took. That time has to suit every
// family at once. The reply must be complete before the tightest window closes, and must not start
// before a ZBS243 listens, which is only once its 802.15.4 ack wait has passed, 1200-1350 us after
// its frame. The windows intersect at [1250, 2396] for a 1504 us AvailDataInfo and at [1250, 2258]
// for a 992 us PONG. RADIO_REPLY_AT_US (hal.h) is 1220: check-ins were answered equally well from 970
// to 2220. PONGs go later: scanning tags accepted them at 1350 and never below about 1200.
#define RADIO_REPLY_PONG_US 1700
typedef char pongFitsWindow[(RADIO_REPLY_PONG_US >= 1250 && RADIO_REPLY_PONG_US <= 2258) ? 1 : -1];
// If the block data from the S3 is not complete at that time, wait at most this long for it; after
// that send nothing (the tag asks again) rather than whatever is in the buffer.
#define BLOCK_DATA_SLACK_MS 100
// What the tag is told to wait: 0 when the block is in the cache and the radio is free, else what
// these say. 0 or at least 10 (smaller values underflow in tag firmware).
#define PLEASE_WAIT_QUEUED_MS 15  // the least, for a block in the cache behind another tag's burst
#define PLEASE_WAIT_COMING_MS 30  // it is on its way for an earlier request

// The wait told to a tag whose block has to come from the S3. It only ever rises: a long wait is
// cheap, since most tags sleep through it, while one too short costs the tag a wasted receive window
// and another request round.
#define DATA_WAIT_MAX_MS 550           // what the closed-source firmware asked for unconditionally
#define DATA_WAIT_START_MS 140         // the C6's constant
#define DATA_WAIT_MARGIN_MS 20         // over the slowest answer seen
static uint16_t dataWaitMs = DATA_WAIT_START_MS;

// A block a tag was waiting for took tookMs: raise the wait if that was slower than promised.
static void dataWaitArrived(uint32_t tookMs) {
    const uint32_t want = tookMs + DATA_WAIT_MARGIN_MS;
    if (want > dataWaitMs) dataWaitMs = want > DATA_WAIT_MAX_MS ? DATA_WAIT_MAX_MS : (uint16_t)want;
}

// The block was not there when the burst was due: back off by half again.
static void dataWaitMissed(void) {
    uint32_t want = (uint32_t)dataWaitMs + dataWaitMs / 2;
    if (want > DATA_WAIT_MAX_MS) want = DATA_WAIT_MAX_MS;
    dataWaitMs = (uint16_t)want;
}
static uint32_t nextBlockAttempt = 0;  // when the radio last asked the S3 for a block
static uint8_t seq = 0;                // sequence number for transmission
static uint8_t lastAckMac[8];

// A second tag's block request, acked with a wait that ends just after the burst in front of it
// (CANCEL_XFER would cost it a whole check-in cycle). One slot: a third tag is still turned away.
static struct {
    struct blockRequest req;  // as the tag sent it
    uint8_t mac[8];
    uint16_t pan;
    uint32_t startMs;   // when its burst is due; 0 = the slot is empty
    uint32_t askedMs;   // when its block was asked of the S3, 0 if it did not need to be
} queuedReq;
// The last tag return data reported to the S3 (see espNotifyTagReturnData), cleared once a minute
static uint8_t lastTagReturn[8];
static uint8_t lastTagReturnMac[8];

// A block burst is sent one frame per loop pass, so serial frames from the S3 are handled between
// the parts (a burst takes up to 260 ms; the S3 expects an answer within 30 ms).
static uint8_t burstParts[BURST_SLOTS];
static int burstSlot = -1;  // the cache slot the burst reads, pinned until it ends
static uint8_t burstLen = 0;
// The schedule of the last burst, so a re-request can be logged as the burst positions the tag missed
static uint64_t lastBurstVer = 0;
static uint8_t lastBurstBlock = 0xFF, lastBurstLen = 0;
static uint8_t lastBurstParts[BURST_SLOTS];
static void burstFinish(void);
static void burstStopFor(const uint8_t *mac);
static void burstDoneSeen(const uint8_t *mac, bool xfc);
static void queueBlockRequest(const uint8_t *src, uint16_t pan, const struct blockRequest *br);
static void promoteQueued(void);
static uint8_t burstPos = 0;
static bool burstActive = false;
static int16_t burstLateMs = 0;
// A running burst holds its frames until this time: its tag has just been re-acked with a wait of 0
// (ZERO_WAIT_HOLD_MS), or the next pass is not due yet. 0 = nothing held.
static uint32_t burstHoldUntil = 0;
static uint8_t burstMarginMs = BURST_MARGIN_MS;  // of the burst that is due or running: past its tag's wait

// The burst goes out in passes, every requested part once per pass. A tag can leave as soon as it has
// the block, so it gets time after the first pass to say so (burstDoneWaitMs), and if it does the rest
// is not sent. A tag that listens out its whole window loses at most the repeats this pushes past it.
static uint8_t burstPassLen = 0;    // frames per pass: the number of parts the tag asked for
static uint8_t burstPassFrom = 0;   // the burst position whose pass has been scheduled
static bool burstFinalBlock = false;  // the image's last block: the tag says XFC, not "next block"
static uint32_t burstWindowEndMs = 0;  // when the tag stops listening
// A part that falls due while the channel is in use waits: for a frame arriving, which would otherwise
// be cut off, and for energy the radio cannot decode, such as WiFi, which the part would be lost in. A
// clean channel costs nothing this way. A hold that a good frame came of is free. The rest comes out of
// this budget, per burst, and once that is gone the burst stops waiting, so a noise that never clears
// costs a tag at most this much of its receive window. A receive flag up for longer than any frame
// lasts is stuck, which it does get on hardware, and is not trusted again for the rest of the burst.
// The radio gives no RSSI reading while that flag is up, so the burst can't see the channel past it.
#define BURST_YIELD_BUDGET_US 30000UL
#define BURST_YIELD_FREE 0
#define BURST_YIELD_HOLDING 1  // since burstYieldSinceUs
#define BURST_YIELD_OFF 2
static uint32_t burstYieldSinceUs = 0;
static uint32_t burstYieldChargedUs = 0;  // of the budget, this burst
static uint32_t burstFlagUpSinceUs = 0;   // the receive flag has been up since, in a hold
static bool burstYieldToFlag = true;
static uint8_t burstYield = BURST_YIELD_FREE;
// This burst waits for energy too: not on a channel busy enough that waiting would not clear it
static bool burstYieldEnergy = false;

// How long a tag that leaves when complete has taken to say so, from the end of the first pass: [0] the
// next block request, [1] XFC after the last block; 0 = not seen yet, no wait. Every reading is the
// latency or longer, so the window's smallest reading is taken: at once when lower, at the end of the
// window when the window only saw higher ones.
static uint16_t burstDoneWaitMs[2];
static uint16_t burstDoneSeenMin[2] = {0xFFFF, 0xFFFF};
static uint32_t burstDoneFromMs = 0;  // the end of the first pass of the burst being watched; 0 = none

// The `air` line's counters, from one air line to the next rather than per window. Saturating.
static struct {
    uint16_t yields;         // burst parts held back because a frame was arriving
    uint16_t yieldFrames;    // ... after which a frame really was received: a hold that was a tag
    uint16_t yieldMs;        // ... the time of the holds no frame came of (BURST_YIELD_BUDGET_US)
    uint16_t others;         // frames from tags other than the one the bursts went to
    uint16_t burstMs;        // burst airtime
    uint16_t harm;           // the other tags' attempts that airtime is estimated to have gone over
} airAcc;
static uint16_t airAdd(uint16_t a, uint32_t b) { return a + b > 0xFFFF ? 0xFFFF : (uint16_t)(a + b); }
// A check on the receive flag that bursts yield to (halRadioRxArriving): the milliseconds it was seen
// up (fh, sampled once a millisecond while the radio listens) against what the frames received
// predict (fe: a frame holds it from receiver sync to its last byte, (len + 3) * 32 us, CRC failures
// included). A flag up for part of a frame reads fh low in proportion, a dead one 0, a stuck one far
// high, one that rises in the preamble about 130 us a frame high. Both are halved together at
// FLAG_HALVE_AT, which keeps them at four digits for the air line.
#define FLAG_HALVE_AT 9999
static uint16_t flagHigh;
static uint32_t flagFrameUs;  // the frames' expected flag time, us: fe is this / 1000
static uint32_t flagTick;     // the millisecond last looked at
static void flagSample(void) {
    const uint32_t now = halMillis();
    if (now == flagTick) return;
    flagTick = now;
    if (halRadioTxBusy()) return;  // our own frame is on the air or armed: the receiver hears nothing
    if (halRadioRxArriving() && ++flagHigh >= FLAG_HALVE_AT) {
        flagHigh /= 2;
        flagFrameUs /= 2;
    }
}
static void flagAddUs(uint32_t us) {
    flagFrameUs += us;
    if (flagFrameUs / 1000 >= FLAG_HALVE_AT) {
        flagHigh /= 2;
        flagFrameUs /= 2;
    }
}
static void flagFrame(uint8_t len) { flagAddUs(((uint32_t)len + 3) * 32); }
// CRC failures since the last call, each (length byte + 1) * 32 us of flag
static uint32_t flagCrcFails = 0, flagCrcFailBytes = 0;
static void flagCrcFrames(void) {
    const uint32_t fails = halRadioRxCrcFails(), bytes = halRadioRxCrcFailBytes();
    if (fails == flagCrcFails) return;
    flagAddUs((bytes - flagCrcFailBytes + (fails - flagCrcFails)) * 32);
    flagCrcFails = fails;
    flagCrcFailBytes = bytes;
}
static uint8_t burstYieldRx;  // halRadioRxFrames() when the current hold began (low byte)

// The S3 sends a HEARTBEAT every second. Without any frame for this long it is gone: go back to the
// ASCII protocol at 115200 baud, where it starts again.
#define V2_LINK_LOST_MS 5000
// Bytes without a valid frame: a rebooted S3 at 115200 baud. Back to ASCII at once, for its first probe.
#define V2_LINK_GARBLED_MS 1200
#define V2_LINK_GARBLED_BYTES 64
#define ASCII_BAUD 115200

static uint8_t curChannel = 11;
static int8_t curPower = 10;
static uint8_t curLed = 255;  // the activity LEDs flash at full brightness until the first SET_CONFIG

// The idle-noise histogram (see sampleNoise), defined here because the diag struct holds the buckets
#define NF_HIST_BUCKETS 8
#define NF_HIST_BASE_DBM (-110)
#define NF_HIST_SHIFT 3  // 8 dB a bucket, so the top bucket collects everything above -47 dBm

// --- diagnostics: LOG lines every DIAG_INTERVAL_MS, when the S3 enabled LOG frames (an AP_DEBUG
// build sends them whether it asked or not)
#ifndef DIAG_INTERVAL_MS
#define DIAG_INTERVAL_MS 15000UL
#endif
#define REPEAT_WINDOW_MS 300  // a check-in of the same tag this soon after the last: it missed our reply
static struct {
    uint16_t checkins;     // AVAIL_DATA_INFO sent
    uint16_t offered;      // ... with data in it
    uint16_t repeats;      // check-ins repeated within REPEAT_WINDOW_MS
    uint32_t replyUsSum;   // start of the tag's request to the start of our reply
    uint32_t replyUsMax;
    uint16_t blockReqs;
    uint16_t bursts;
    uint16_t xfc;
    uint16_t pings;
    uint16_t zeroBursts;   // bursts from the cache, after a wait of 0
    uint16_t rereqs;       // partial requests for a block this radio had just burst: parts were lost
    uint16_t rereqParts;   // ... and how many parts they asked for again
    uint16_t ackResends;   // block request acks sent again mid-burst (the tag missed the first)
    uint16_t burstsCut;    // bursts stopped early because the tag had what it needed
    uint16_t silentZero;   // bursts whose tag went quiet after them, after a wait of 0 (burstAnswerSeen)
    uint16_t silentWaited;  // ... and after any other wait
    uint16_t othersHeard;  // frames from tags other than the one bursts went to
    uint32_t burstTxUs;    // time burst frames were on the air, when the radio heard nobody
    uint16_t cancelsBusy;  // CANCEL_XFER sent: more tags wanting a burst at once than the slot holds
    uint16_t queued;       // block requests acked to wait for another tag's burst, then served
    uint16_t cancelsNoData;  // ... and to a tag the S3 has nothing pending for
    uint16_t late;         // requests too old to answer, in THIS window: the burst loop's input
    uint16_t unhandled;    // frames of a known type dropped for their length, and unknown types
    uint32_t dataMsSum;    // the S3's answer time for a block a tag was already waiting for
    uint16_t dataMsMax;
    uint16_t dataCount;
    uint16_t dataLate;     // blocks that never arrived in time, so no burst went out
#if AP_DEBUG
    int16_t foMin, foMax;  // frequency offset of the tags' check-ins (11-bit signed)
    int32_t foSum;
    uint8_t fdcHiOr;       // the raw high byte ORed; Telink documents bits 7 and 3 of it as zero
#endif
    uint16_t nfSamples;    // idle-channel samples taken in this window
    uint16_t nfBusy;       // ... and how many of them found something on the channel
#if AP_DEBUG
    uint16_t nfHist[NF_HIST_BUCKETS];  // the shape of those samples (see sampleNoise)
#endif
} diag;
static uint8_t lastCheckinMac[8];
static uint32_t lastCheckinMs = 0;
static uint32_t diagTimer = 0;
static void diagAdd(char *buf, size_t *n, size_t max, const char *name, uint32_t value);
static void diagAddSigned(char *buf, size_t *n, size_t max, const char *name, int32_t value);
static void diagAddHex(char *buf, size_t *n, size_t max, const char *name, uint32_t value);
#if AP_DEBUG
static void logCheckinOffset(const uint8_t *mac);
#endif

// --- tools --------------------------------------------------------------------------------------

// unsigned, not uint8_t: GCC 4.5 would spend two instructions a byte truncating an 8-bit local
static void addCRC(void *p, uint8_t len) {
    unsigned total = 0;
    for (unsigned c = 1; c < len; c++) total += ((uint8_t *)p)[c];
    ((uint8_t *)p)[0] = (uint8_t)total;
}
static bool checkCRC(const void *p, uint8_t len) {
    unsigned total = 0;
    for (unsigned c = 1; c < len; c++) total += ((const uint8_t *)p)[c];
    return ((const uint8_t *)p)[0] == (uint8_t)total;
}

static void pr(const char *s) { halSerialWrite((const uint8_t *)s, strlen(s)); }

static char hexDigit(uint8_t v) { return (char)(v < 10 ? '0' + v : 'A' + v - 10); }

// "TOKhh..." with value as digits hex digits, as the ASCII protocol's printf("%0nX")
static void prHex(const char *token, uint32_t value, uint8_t digits) {
    char buf[16];
    uint8_t n = 0;
    while (*token && n < 8) buf[n++] = *token++;
    for (int8_t d = digits - 1; d >= 0; d--) buf[n++] = hexDigit((value >> (d * 4)) & 0x0F);
    halSerialWrite((const uint8_t *)buf, n);
}

// A short log line to the S3 (LOG frames, when the S3 asks for them): text and a number
static void logNum(uint8_t level, const char *text, uint32_t value) {
    char buf[V2_LOG_MAX];  // room for the longest text the LOG line takes, plus up to 10 digits
    size_t n = strlen(text);
    if (n > sizeof(buf) - 12) n = sizeof(buf) - 12;
    memcpy(buf, text, n);
    char digits[11];
    uint8_t d = 0;
    do {
        digits[d++] = (char)('0' + value % 10);
        value /= 10;
    } while (value && d < sizeof(digits));
    while (d) buf[n++] = digits[--d];
    buf[n] = 0;
    v2Log(level, buf);
}
#define LOG_WARN 2
#define LOG_INFO 3

static uint8_t getPacketType(const uint8_t *buffer) {
    const struct MacFcs *fcs = (const struct MacFcs *)buffer;
    if ((fcs->frameType == 1) && (fcs->destAddrType == 2) && (fcs->srcAddrType == 3) && (fcs->panIdCompressed == 0)) {
        return buffer[sizeof(struct MacFrameBcast)];  // broadcast frame
    } else if ((fcs->frameType == 1) && (fcs->destAddrType == 3) && (fcs->srcAddrType == 3) && (fcs->panIdCompressed == 1)) {
        return buffer[sizeof(struct MacFrameNormal)];  // normal frame
    }
    return 0;
}

static uint8_t getBlockDataLength(void) {
    uint8_t partNo = 0;
    for (uint8_t c = 0; c < BLOCK_MAX_PARTS; c++) {
        if (requestedData.requestedParts[c / 8] & (1 << (c % 8))) partNo++;
    }
    return partNo;
}

// A tag accepts a reply only within a few ms of its request (2-9 ms by tag type and request); a request
// that waited longer (behind a burst, say) is skipped.
static uint32_t lateRequestsSkipped = 0;
static uint16_t statusLateSkipped = 0;  // the same, since the last STATUS
// A PING may be answered after its window: tags do not flush their receive queue between attempts
// (detectAP() calls radioRxFlush() once), so a late PONG is read by the next attempt, and the nRF52811
// reads every PONG that way. A PING carries no hwType, so the limit is the widest window of any family.
// Opticon and the 88MZ100 keep a single frame, so for them the PONG still has to be prompt.
#define PING_AIRTIME_US 832           // 20-byte PSDU (MacFrameBcast + type + FCS) + 6 PHY bytes, 32 us/byte
#define PONG_AIRTIME_US 992           // 25-byte PSDU (MacFrameNormal + type + channel + FCS) + 6
#define PING_WINDOW_WIDEST_US 20000U  // EFR32xG22, the widest PING_REPLY_WINDOW of any 2.4 GHz family
#define PING_ANSWER_MAX_US (PING_AIRTIME_US + PING_WINDOW_WIDEST_US - PONG_AIRTIME_US)

// ageUs runs from the START of the tag's frame, so it includes that frame's airtime. The limits keep a
// reply inside the window of the tightest family that asks: for check-ins the 88MZ100 (4 ms, 3.0 ms
// after millis() truncation, three tries), which a 1504 us AvailDataInfo still reaches up to ageUs
// 1504 + 900 + 3000 - 1504 = 3900; for block requests the nRF52811 (6 ms). There is no too-early guard:
// the reply delay, RADIO_REPLY_AT_US, keeps a reply out of the tag's acknowledge wait.
static bool tooLateToAnswer(uint8_t type, uint32_t ageUs) {
    switch (type) {
        case PKT_PING:
            return ageUs > PING_ANSWER_MAX_US;
        case PKT_AVAIL_DATA_REQ:
        case PKT_AVAIL_DATA_SHORTREQ:
            return ageUs > 4000;
        case PKT_XFER_COMPLETE:
            return ageUs > 5000;
        case PKT_BLOCK_REQUEST:
        case PKT_BLOCK_PARTIAL_REQUEST:
            return ageUs > 6000;
        default:
            return false;
    }
}

// --- the tag store -------------------------------------------------------------------------------
// A record per tag the radio knows (allow-list flag, pending entry), a profile per group of
// tags that share the rest of their MAC, and an offer only for a tag with content waiting (most pending
// entries are an idle "check in again in n minutes"). The three live at the low end of the cache's
// arena, in whole granules; cacheListReserve is told their total, so the cache gives up room through
// its eviction policy, a pinned burst surviving it:
//
//   arena 0 | profiles | records | offers -->          free          <-- block |block| block |
//
// Profiles never move; records and offers move by memmove when a part in front of them changes its room
// (storeLayout). Indices survive a move and nothing holds a pointer into the store across a main-loop
// pass, so a tag is looked up once per frame (rxRec).

// --- how a MAC is stored
// On Solum tags mac[2..3] is the serial and mac[0] a check byte; mac[1] and mac[4..7] are shared within a
// family (0000032CA1B23E17, as the S3 prints it, is mac[0..7] 17 3e b2 a1 2c 03 00 00). So a record's key
// is mac[0], mac[2], mac[3] and the low byte of its profile index; the profile holds mac[1], mac[4..7].
// It is exact: a tag that differs from its family in anything gets a profile of its own, and past 256
// profiles the index's high byte is the record's profHi. `prof` on the cfg line shows
// how well the fixed positions fit: about one profile per family.

#define HOUSEKEEPING_INTERVAL_MS 60000UL

// A record: 12 bytes, 4-aligned, the key first so that the scan is one aligned load a tag.
struct tagRec {
    uint32_t key;         // mac[0] | mac[2] << 8 | mac[3] << 16 | (profile & 0xFF) << 24
    uint8_t flags;        // REC_*
    uint8_t profHi;       // profile >> 8: 0 until the fleet has more than 256 profiles
    uint16_t sleepUntil;  // the entry's nextCheckIn: a deadline on hkNow while REC_COUNTING, else the value
    uint16_t expires;     // the hkNow at which the entry lapses
    uint16_t offer;       // its content, in offers[], while REC_OFFER
} __attribute__((aligned(4)));
// What a group of tags shares. refs 0 is a free slot, taken again by the next new family.
struct tagProfile {
    uint32_t hi;     // mac[4..7]
    uint8_t b1;      // mac[1]
    uint8_t pad;
    uint16_t refs;   // records using it
} __attribute__((aligned(4)));
// Content offered to a tag: the AvailDataInfo fields an idle entry has as zero, and its record.
struct tagOffer {
    uint64_t dataVer;
    uint32_t dataSize;
    uint8_t dataType;
    uint8_t dataTypeArgument;
    uint16_t tag;  // the record it belongs to, so that freeing an offer can move another into its place
} __attribute__((aligned(4)));
V2_STATIC_ASSERT(sizeof(struct tagRec) == 12 && sizeof(struct tagProfile) == 8 && sizeof(struct tagOffer) == 16,
                 "the store's three strides");
// The reply to a check-in is one copy out of the offer, the checksum computed over it as it goes
// (addCRC) and nextCheckIn after it, so the offer's fields are the AvailDataInfo's, in its order.
#define OF_AT(f) (__builtin_offsetof(struct tagOffer, f) + 1 == __builtin_offsetof(struct AvailDataInfo, f))
#define OFFER_REPLY_BYTES __builtin_offsetof(struct tagOffer, tag)
V2_STATIC_ASSERT(__builtin_offsetof(struct AvailDataInfo, checksum) == 0 && OF_AT(dataVer) && OF_AT(dataSize) &&
                     OF_AT(dataType) && OF_AT(dataTypeArgument) &&
                     __builtin_offsetof(struct AvailDataInfo, nextCheckIn) == 1 + OFFER_REPLY_BYTES,
                 "an offer is an AvailDataInfo after its checksum, up to nextCheckIn");
#undef OF_AT

#define REC_LISTED 0x01    // on the allow-list
#define REC_PENDING 0x02   // has a pending entry: sleepUntil and expires are live
#define REC_OFFER 0x04     // ... with content, in offers[offer]
#define REC_COUNTING 0x08  // sleepUntil is a minute deadline still ahead
#define REC_STALE 0x20     // unowned at the last housekeeping: the next one deletes it
#define REC_AWAIT 0x40     // a burst went to this tag and it has not answered yet (burstAnswerSeen)
#define REC_AWAIT_ZERO 0x80  // ... and that burst came after a wait of 0
#define REC_OWNED (REC_LISTED | REC_PENDING)

// Room grows and shrinks in granules, so a new tag rarely moves anything; 16 * 12 and 4 * 8 bytes keep
// what follows 8-aligned, as the offers' dataVer wants.
#define REC_GRAN 16
#define PROF_GRAN 4
#define OFFER_GRAN 4
V2_STATIC_ASSERT(REC_GRAN * sizeof(struct tagRec) % 8 == 0 && PROF_GRAN * sizeof(struct tagProfile) % 8 == 0,
                 "the parts behind a granule stay 8-aligned");

static struct tagProfile *profs;  // the arena's base, never moved
static struct tagRec *recs;       // behind the profiles' room
static struct tagOffer *offers;   // behind the records' room
static uint16_t profN = 0, profCap = 0;  // slots up to the highest one in use, and room
static uint16_t recN = 0, recCap = 0;    // records, dense
static uint16_t offN = 0, offCap = 0;    // offers, dense
static uint16_t recListed = 0;   // records with REC_LISTED, which is what MAX_ALLOWED_MACS caps
static uint16_t recPending = 0;  // ... with REC_PENDING
// Listed tags the store can always take: the room above the cache's floor at 20 bytes a tag (a record
// and a profile of its own), capped at MAX_ALLOWED_MACS. Nothing else may eat into it (storeHeld).
static uint16_t listGuarantee = 0;
static uint16_t pendMax = 0;   // the most pending entries there may be (HELLO_REPLY, `pend` on cfg)
static uint16_t offerMax = 0;  // offers there is always room for, beside the list at its guarantee (`ofr`)
static uint16_t pendPeak = 0;  // the most there have been since boot (`pend` on the hw line)
// The store's clock, in housekeeping rounds; TTLs and "check in again in n minutes" are deadlines on
// it. They are compared for equality once a round, never ordered, so the 45-day wrap is harmless.
static uint16_t hkNow = 0;

static uint8_t lockMode = V2_LOCK_OFF;
static uint32_t lockedIgnored = 0;  // frames from other tags, in the last minute

static uint16_t gran(uint32_t n, uint16_t g) { return (uint16_t)((n + g - 1) / g * g); }
static uint32_t storeBytes(uint32_t pc, uint32_t rc, uint32_t oc) {
    return pc * sizeof(struct tagProfile) + rc * sizeof(struct tagRec) + oc * sizeof(struct tagOffer);
}
static uint16_t recProfile(const struct tagRec *r) { return (uint16_t)((r->key >> 24) | ((uint16_t)r->profHi << 8)); }

// What the store would need at these capacities, with dp profiles and dr records about to be added, if
// the list then grew to its guarantee. Everything but the list must keep this inside the arena: an
// allow-list refusal costs the S3's inventory lock, a refused offer is just retried at the next check-in.
static uint32_t storeHeld(uint16_t pc, uint16_t rc, uint16_t oc, uint8_t dp, uint8_t dr) {
    const uint32_t k = listGuarantee > recListed ? (uint32_t)(listGuarantee - recListed) : 0;
    const uint16_t p = gran(profN + dp + k, PROF_GRAN), r = gran(recN + dr + k, REC_GRAN);
    return storeBytes(p > pc ? p : pc, r > rc ? r : rc, oc);
}
// ... and whether that still fits: the test everything but the list passes before it adds anything.
static bool storeKeepsList(uint16_t pc, uint16_t rc, uint16_t oc, uint8_t dp, uint8_t dr) {
    return storeHeld(pc, rc, oc, dp, dr) <= cacheListBytesMax();
}

// The three parts at new capacities. Whatever moves, moves the way that does not write over the
// other: the offers first when they go up, the records first when the offers come down.
static void storeLayout(uint16_t pc, uint16_t rc, uint16_t oc) {
    struct tagRec *const r = (struct tagRec *)(void *)((uint8_t *)profs + pc * sizeof(struct tagProfile));
    struct tagOffer *const o = (struct tagOffer *)(void *)((uint8_t *)r + rc * sizeof(struct tagRec));
    if ((uint8_t *)o > (uint8_t *)offers) {
        memmove(o, offers, offN * sizeof(struct tagOffer));
        memmove(r, recs, recN * sizeof(struct tagRec));
    } else {
        memmove(r, recs, recN * sizeof(struct tagRec));
        memmove(o, offers, offN * sizeof(struct tagOffer));
    }
    recs = r;
    offers = o;
    profCap = pc;
    recCap = rc;
    offCap = oc;
}

// Who is asking for room, which decides what it may take:
#define FOR_LIST 0   // ALLOW_ADD: anything the arena has, down to the cache's floor
#define FOR_OTHER 1  // anything else - a pending entry, a profile: the same, less what the
                     // list is promised

static bool storeGrow(uint16_t pc, uint16_t rc, uint16_t oc, uint8_t who, uint8_t dp, uint8_t dr) {
    const uint32_t want = storeBytes(pc, rc, oc);
    if (want > cacheListBytesMax()) return false;
    if (who == FOR_OTHER && !storeKeepsList(pc, rc, oc, dp, dr)) return false;
    if (!cacheListReserve((uint16_t)want)) return false;  // which evicts blocks, through the policy
    storeLayout(pc, rc, oc);
    return true;
}

// And back: each part's room down to its use, rounded up to its granule, and the cache takes back
// what that frees. After the list shrinks, and once a minute from housekeeping.
static void storeTrim(void) {
    while (profN && !profs[profN - 1].refs) profN--;
    const uint16_t pc = gran(profN, PROF_GRAN), rc = gran(recN, REC_GRAN), oc = gran(offN, OFFER_GRAN);
    if (pc == profCap && rc == recCap && oc == offCap) return;
    storeLayout(pc, rc, oc);
    cacheListReserve((uint16_t)storeBytes(pc, rc, oc));
}

// The next live profile at or after `from` with these shared bytes.
static int32_t profFind(uint32_t hi, uint8_t b1, uint16_t from) {
    for (uint32_t p = from; p < profN; p++) {
        if (profs[p].hi == hi && profs[p].b1 == b1 && profs[p].refs) return (int32_t)p;
    }
    return -1;
}

// A profile for a tag with these shared bytes: its family's, or a slot of its own. A new slot is
// returned with refs 0 - the caller counts the tag in once it has its record, and a slot it never
// counts stays free.
static int32_t profFor(uint32_t hi, uint8_t b1, uint8_t who) {
    int32_t p = profFind(hi, b1, 0);
    if (p >= 0) return p;
    for (uint32_t c = 0; c < profN && p < 0; c++) {
        if (!profs[c].refs) p = (int32_t)c;
    }
    if (p < 0) {
        if (profN == profCap && !storeGrow((uint16_t)(profCap + PROF_GRAN), recCap, offCap, who, 1, 0)) return -1;
        if (who != FOR_LIST && !storeKeepsList(profCap, recCap, offCap, 1, 0)) return -1;
        p = profN++;
    }
    profs[p].hi = hi;
    profs[p].b1 = b1;
    profs[p].refs = 0;
    return p;
}

// On the check-in path, where a tag not in the store costs a full scan: one aligned load and compare
// per record. profHi is tested only when the key matched.
static int32_t recScan(uint32_t key, uint8_t profHi) {
    const struct tagRec *r = recs;
    const struct tagRec *const end = recs + recN;
    for (; r != end; r++) {
        if (r->key == key && r->profHi == profHi) return (int32_t)(r - recs);
    }
    return -1;
}

// The record for this MAC, or -1: the records are scanned once for each matching profile, and not at
// all for a MAC whose family has none.
static int32_t recFind(const uint8_t *mac) {
    uint32_t hi;
    memcpy(&hi, mac + 4, 4);
    const uint32_t key24 = mac[0] | ((uint32_t)mac[2] << 8) | ((uint32_t)mac[3] << 16);
    for (int32_t p = profFind(hi, mac[1], 0); p >= 0; p = profFind(hi, mac[1], (uint16_t)(p + 1))) {
        const int32_t i = recScan(key24 | ((uint32_t)p << 24), (uint8_t)(p >> 8));
        if (i >= 0) return i;
    }
    return -1;
}

// A record's MAC, which only the timeout report needs: the S3 is told which tag it was.
static void recMac(const struct tagRec *r, uint8_t *mac) {
    const struct tagProfile *pf = &profs[recProfile(r)];
    mac[0] = (uint8_t)r->key;
    mac[1] = pf->b1;
    mac[2] = (uint8_t)(r->key >> 8);
    mac[3] = (uint8_t)(r->key >> 16);
    memcpy(mac + 4, &pf->hi, 4);
}

// The record a pending entry's content belongs to points at it, and it back at the record, so that
// either can be moved into a hole by swap-with-last and the one pointer that names it fixed.
static int32_t offerNew(uint16_t rec) {
    if (offN == offCap && !storeGrow(profCap, recCap, (uint16_t)(offCap + OFFER_GRAN), FOR_OTHER, 0, 0)) return -1;
    offers[offN].tag = rec;
    return offN++;
}
static void offerFree(uint16_t j) {
    offN--;
    if (j != offN) {
        offers[j] = offers[offN];
        recs[offers[j].tag].offer = j;
    }
}

// A record nothing owns any more goes; the last record takes its place.
static void recDelete(uint16_t i) {
    profs[recProfile(&recs[i])].refs--;
    recN--;
    if (i != recN) {
        recs[i] = recs[recN];
        if (recs[i].flags & REC_OFFER) offers[recs[i].offer].tag = i;
    }
}

// A slot for a new record. Records nothing owns - a tag whose pending entry lapsed, not listed - are
// the first given up, before the store grows into the cache.
static int32_t recSlot(uint8_t who) {
    if (recN < recCap) {
        if (who != FOR_LIST && !storeKeepsList(profCap, recCap, offCap, 0, 1)) return -1;
        return recN++;
    }
    for (uint32_t c = 0; c < recN; c++) {
        if (!(recs[c].flags & REC_OWNED)) {
            profs[recProfile(&recs[c])].refs--;
            return (int32_t)c;
        }
    }
    if (!storeGrow(profCap, (uint16_t)(recCap + REC_GRAN), offCap, who, 0, 1)) return -1;
    return recN++;
}

// A record for a MAC the store does not hold yet, owning nothing: the caller sets what it is for.
static int32_t recAdd(const uint8_t *mac, uint8_t who) {
    uint32_t hi;
    memcpy(&hi, mac + 4, 4);
    const int32_t p = profFor(hi, mac[1], who);
    if (p < 0) return -1;
    const int32_t i = recSlot(who);
    if (i < 0) return -1;  // the profile, if it was a new one, stays free
    struct tagRec *r = &recs[i];
    r->key = mac[0] | ((uint32_t)mac[2] << 8) | ((uint32_t)mac[3] << 16) | ((uint32_t)p << 24);
    r->profHi = (uint8_t)(p >> 8);
    r->flags = 0;
    r->sleepUntil = 0;
    r->expires = 0;
    r->offer = 0;
    profs[p].refs++;
    return i;
}

// --- the tag inventory lock (V2_CFG_LOCK_MASK in SET_CONFIG)
// Locked, the radio answers only tags on the S3's allow-list; learning, also a full check-in of a tag
// that just booted. Other tags get no reply and look for another AP. The list is the REC_LISTED bit,
// capped at MAX_ALLOWED_MACS (blockcache.h); `tags` on the cfg line is what the store guarantees.
#define WAKEUP_REASON_FIRSTBOOT 0xFC

// False makes the S3 give up the radio-side inventory lock until the link restarts, so it happens only
// at MAX_ALLOWED_MACS or with the arena full and the cache at its floor; short of that a block is evicted.
static bool allowAdd(const uint8_t *mac) {
    int32_t i = recFind(mac);
    if (i >= 0 && (recs[i].flags & REC_LISTED)) return true;
    if (recListed >= MAX_ALLOWED_MACS) return false;
    if (i < 0 && (i = recAdd(mac, FOR_LIST)) < 0) return false;
    recs[i].flags = (uint8_t)((recs[i].flags | REC_LISTED) & ~REC_STALE);
    recListed++;
    return true;
}

// The S3 has deleted the tag. A record left owning nothing goes at once, so its room - and the
// cache block it may have cost - comes back with the next storeTrim.
static void allowRemove(const uint8_t *mac) {
    const int32_t i = recFind(mac);
    if (i < 0 || !(recs[i].flags & REC_LISTED)) return;
    recs[i].flags &= (uint8_t)~REC_LISTED;
    recListed--;
    if (!(recs[i].flags & REC_OWNED)) recDelete((uint16_t)i);
}

// The S3 sends this before the whole database, after every HELLO. A tag with something pending keeps
// its record for the ALLOW_ADD that follows; the rest go.
static void allowClear(void) {
    for (uint32_t c = 0; c < recN;) {
        recs[c].flags &= (uint8_t)~REC_LISTED;
        if (!(recs[c].flags & REC_OWNED)) {
            recDelete((uint16_t)c);  // the last record is in c now, and is looked at next
        } else {
            c++;
        }
    }
    recListed = 0;
    storeTrim();
}

// May the radio answer this frame (len bytes, of packet type type, from the tag with record rec)?
static bool tagAccepted(uint8_t type, const uint8_t *frame, uint8_t len, int32_t rec) {
    if (lockMode == V2_LOCK_OFF) return true;
    if (rec >= 0 && (recs[rec].flags & REC_LISTED)) return true;
    if (lockMode == V2_LOCK_LEARNING && type == PKT_AVAIL_DATA_REQ && (len == 28 || len == 40)) {
        const struct AvailDataReq *adr = (const struct AvailDataReq *)(frame + sizeof(struct MacFrameBcast) + 1);
        return adr->wakeupReason == WAKEUP_REASON_FIRSTBOOT;
    }
    return false;
}

// --- pending entries
// One per tag: the S3 keeps its own per-tag queue and announces one item at a time (checkQueue in
// ESP32_AP-Flasher/src/newproto.cpp), and SET_PENDING replaces the entry. Nearly every tag holds an idle
// entry (prepareIdleReq: all content fields zero), which is just the record's two deadlines; only an
// entry with content takes an offer.

// The nextCheckIn the tag is told now. Minutes count down from SET_PENDING, one a housekeeping round,
// and stop at 0; a value with bit 15 set is seconds and is told as it was given.
static uint16_t recNextCheckIn(const struct tagRec *r) {
    return (r->flags & REC_COUNTING) ? (uint16_t)(r->sleepUntil - hkNow) : r->sleepUntil;
}

// The entry ends: its offer is freed and the record no longer has one. Returns the version it
// offered (0 for an idle entry), whose cached blocks the caller drops if nothing else offers them.
static uint64_t pendEnd(uint16_t i) {
    struct tagRec *r = &recs[i];
    uint64_t ver = 0;
    if (r->flags & REC_OFFER) {
        ver = offers[r->offer].dataVer;
        offerFree(r->offer);
    }
    if (r->flags & REC_PENDING) recPending--;
    r->flags &= (uint8_t)~(REC_PENDING | REC_OFFER | REC_COUNTING | REC_STALE);
    return ver;
}

static bool verStillOffered(uint64_t ver) {
    for (uint32_t j = 0; j < offN; j++) {
        if (offers[j].dataVer == ver) return true;
    }
    return false;
}
// Drops cached blocks of a version no pending entry offers any more; chunks still arriving for it are
// marked unoffered by verStillOffered (dataChunk), so the cache evicts them first.
static void dropUnusedData(uint64_t ver) {
    if (ver != 0 && !verStillOffered(ver)) cacheDropVer(ver);
}

static uint16_t curPendingData = 0;
static uint16_t curNoUpdate = 0;
static void countSlots(void) {
    curPendingData = 0;
    for (uint32_t j = 0; j < offN; j++) {
        if (offers[j].dataType != DATATYPE_NOUPDATE) curPendingData++;
    }
    curNoUpdate = (uint16_t)(recPending - curPendingData);
}

// The record of the tag whose frame is being handled, or -1, found once in handleRadioFrame. Only valid
// for that frame: the store may move and renumber between main-loop passes.
static int32_t rxRec;

// The store at boot: empty, with no room; the arena is the cache's until the first record.
static void storeInit(void) {
    profs = (struct tagProfile *)(void *)cacheListWords();
    recs = (struct tagRec *)(void *)profs;
    offers = (struct tagOffer *)(void *)profs;
    profN = profCap = recN = recCap = offN = offCap = 0;
    recListed = recPending = 0;
    hkNow = 0;
    const uint32_t room = cacheListBytesMax();
    // The guarantee: the most listed tags at 20 bytes each, beside one granule of offers
    uint32_t k = MAX_ALLOWED_MACS;
    while (k && storeBytes(gran(k + 1, PROF_GRAN), gran(k + 1, REC_GRAN), OFFER_GRAN) > room) k--;
    listGuarantee = (uint16_t)k;
    // Pending entries: a record each, beside one granule of profiles (reached when every pending tag is
    // listed, as the S3 does)
    const uint32_t one = storeBytes(PROF_GRAN, 0, 0);
    pendMax = (uint16_t)(room > one ? (room - one) / sizeof(struct tagRec) / REC_GRAN * REC_GRAN : 0);
    // Offers there is always room for, beside the list at its guarantee
    const uint32_t list = storeBytes(gran(k + 1, PROF_GRAN), gran(k + 1, REC_GRAN), 0);
    offerMax = (uint16_t)(room > list ? (room - list) / sizeof(struct tagOffer) / OFFER_GRAN * OFFER_GRAN : 0);
}

// capabilities, for CAP> and HELLO_REPLY
static uint16_t radioCaps(void) { return V2_CAP_FRAMING | V2_CAP_BLOCK_CACHE | V2_CAP_TAG_RETURN_DATA | V2_CAP_ALLOW_LIST | V2_CAP_LOG; }

// --- the ASCII protocol, before FRM1 ------------------------------------------------------------
// Only what the S3 needs to recognise this radio and switch it to v2. Older S3 firmware, which needs
// the full ASCII protocol, is not supported (the same as with the C6/H2 from 0x0023).

static void espNotifyAPInfo(void) {
    prHex("TYP>", AP_HW_TYPE, 2);
    prHex("VER>", AP_FW_VERSION, 4);
    prHex("MAC>", ((uint32_t)mSelfMac[0] << 8) | mSelfMac[1], 4);
    prHex("", ((uint32_t)mSelfMac[2] << 8) | mSelfMac[3], 4);
    prHex("", ((uint32_t)mSelfMac[4] << 8) | mSelfMac[5], 4);
    prHex("", ((uint32_t)mSelfMac[6] << 8) | mSelfMac[7], 4);
    prHex("ZCH>", curChannel, 2);
    prHex("ZPW>", (uint8_t)curPower, 2);
    countSlots();
    prHex("PEN>", curPendingData > 0xFF ? 0xFF : curPendingData, 2);
    prHex("NOP>", curNoUpdate > 0xFF ? 0xFF : curNoUpdate, 2);
    prHex("CAP>", radioCaps(), 4);
}

// Returns true once the link switched to v2; the rest of the buffer then belongs to the old speed.
static bool processAscii(uint8_t lastchar) {
    static uint8_t cmdbuffer[4];
    for (uint8_t c = 0; c < 3; c++) cmdbuffer[c] = cmdbuffer[c + 1];
    cmdbuffer[3] = lastchar;

    if (memcmp(cmdbuffer, "NFO?", 4) == 0) {
        pr("ACK>");
        espNotifyAPInfo();
    }
    if (memcmp(cmdbuffer, "RDY?", 4) == 0) {
        pr("ACK>");
    }
    if (memcmp(cmdbuffer, "FRM1", 4) == 0) {
        memset(cmdbuffer, 0, sizeof(cmdbuffer));
        pr("ACK>");
        halDelayMs(V2_SWITCH_DELAY_MS);  // halSerialSetBaud() lets the ACK> leave first
        halSerialSetBaud(V2_BAUD);
        v2Start();
        return true;
    }
    return false;
}

// --- requests from the S3 (v2) ------------------------------------------------------------------

static void nak(struct v2Reply *reply, uint8_t code) {
    reply->type = V2_NAK;
    reply->len = 1;
    reply->data[0] = code;
}

static void fillStatus(struct v2Reply *reply) {
    struct v2Status st;
    memset(&st, 0, sizeof(st));
    st.uptimeMs = halMillis();
    st.channel = curChannel;
    st.subghzChannel = V2_NO_SUBGHZ_CHANNEL;
    st.txPowerDbm = curPower;
    st.lastResetReason = halResetReason();
    countSlots();
    st.pending = curPendingData;
    st.pendingIdle = curNoUpdate;
    st.cacheUsedKb = cacheUsedKb();
    st.rxFrames = halRadioRxFrames();
    st.txFrames = halRadioTxFrames();
    st.lateRequestsSkipped = statusLateSkipped;
    st.serialCrcErrors = v2CrcErrors;
    st.serialOverflows = v2Overflows + halSerialOverflows();
    st.retransmits = v2Retransmits;
    st.eventsQueued = v2EventsQueued();
    statusLateSkipped = 0;
    reply->type = V2_STATUS;
    reply->len = sizeof(st);
    memcpy(reply->data, &st, sizeof(st));
}

static void fillHelloReply(struct v2Reply *reply) {
    struct v2HelloReply hr;
    memset(&hr, 0, sizeof(hr));
    hr.protoMajor = V2_PROTO_MAJOR;
    hr.protoMinor = V2_PROTO_MINOR;
    hr.fwVersion = AP_FW_VERSION;
    hr.hwType = AP_HW_TYPE;
    memcpy(hr.mac, mSelfMac, 8);
    hr.caps = radioCaps();
    hr.maxPayload = V2_MAX_PAYLOAD;
    hr.cacheKb = cacheKb();
    hr.pendingSlots = pendMax;
    reply->type = V2_HELLO_REPLY;
    reply->len = sizeof(hr);
    memcpy(reply->data, &hr, sizeof(hr));
}

static void setConfig(const struct v2SetConfig *sc, struct v2Reply *reply) {
    if (sc->channel != V2_CFG_UNCHANGED_U8) {
        bool valid = false;
        for (uint8_t c = 0; c < sizeof(channelList); c++) {
            if (channelList[c] == sc->channel) valid = true;
        }
        // 802.15.4 at 2.4 GHz has channels 11..26. channelList carries 27 as the C6/H2 list does, but
        // here it would program about 2485 MHz, where no tag transmits.
        if (sc->channel < 11 || sc->channel > 26) valid = false;
        if (!valid) {
            nak(reply, V2_NAK_INVALID_PARAM);
            return;
        }
    }
    bool changed = false;
    // no sub-GHz radio: subghzChannel is ignored
    if (sc->channel != V2_CFG_UNCHANGED_U8 && curChannel != sc->channel) {
        halRadioSetChannel(sc->channel);
        curChannel = sc->channel;
        changed = true;
    }
    if (sc->ledBrightness != curLed) {
        halLedSetBrightness(sc->ledBrightness);
        curLed = sc->ledBrightness;
        changed = true;
    }
    if (sc->txPowerDbm != V2_CFG_UNCHANGED_POWER && sc->txPowerDbm != curPower) {
        // Applied before the next frame, at the nearest step at or below the request (halRadioSetTxPower);
        // STATUS reports what was asked for, halRadioTxPowerApplied() the step the PA holds.
        curPower = sc->txPowerDbm;
        halRadioSetTxPower(curPower);
        changed = true;
    }
    const uint8_t lock = (sc->flags & V2_CFG_LOCK_MASK) >> V2_CFG_LOCK_SHIFT;
    if (lock != lockMode) {
        lockMode = lock;
        changed = true;
    }
    v2SetLogEnabled((sc->flags & V2_CFG_LOG_FRAMES) != 0);
    // Logged only on a change (the S3 sends SET_CONFIG after every HELLO)
    if (!changed) return;
    char buf[V2_LOG_MAX];
    size_t n = 0;
    diagAdd(buf, &n, sizeof(buf), "cfg ch ", curChannel);
    diagAddSigned(buf, &n, sizeof(buf), "pwr ", curPower);
    diagAdd(buf, &n, sizeof(buf), "lock ", lockMode);
    diagAdd(buf, &n, sizeof(buf), "led ", curLed);
    // The arena's maxima, worked out at boot: `ckb` the KB of block data the cache can hold (cacheKb),
    // `tags` listGuarantee, `pend` pendMax, `ofr` offerMax; and `prof` the profiles in use.
    uint16_t live = 0;
    for (uint32_t p = 0; p < profN; p++) {
        if (profs[p].refs) live++;
    }
    diagAdd(buf, &n, sizeof(buf), "ckb ", cacheKb());
    diagAdd(buf, &n, sizeof(buf), "tags ", listGuarantee);
    diagAdd(buf, &n, sizeof(buf), "pend ", pendMax);
    diagAdd(buf, &n, sizeof(buf), "ofr ", offerMax);
    diagAdd(buf, &n, sizeof(buf), "prof ", live);
    buf[n] = 0;
    v2Log(LOG_INFO, buf);
}

// Replaces the tag's entry. An offer that finds no room is NO_SLOT, which the S3 retries at the tag's
// next check-in; the old entry is then left as it was.
static void setPending(const struct v2SetPending *sp, struct v2Reply *reply) {
    if (sp->ttlMin == 0) {
        nak(reply, V2_NAK_INVALID_PARAM);
        return;
    }
    const bool idle = !sp->dataVer && !sp->dataSize && !sp->dataType && !sp->dataTypeArg;
    int32_t i = recFind(sp->mac);
    const bool made = i < 0;
    if (made && (i = recAdd(sp->mac, FOR_OTHER)) < 0) {
        nak(reply, V2_NAK_NO_SLOT);
        return;
    }
    uint64_t oldVer = 0;
    if (recs[i].flags & REC_OFFER) {
        oldVer = offers[recs[i].offer].dataVer;
        if (idle) {
            offerFree(recs[i].offer);
            recs[i].flags &= (uint8_t)~REC_OFFER;
        }
    } else if (!idle) {
        const int32_t j = offerNew((uint16_t)i);  // may move the records, never renumber them
        if (j < 0) {
            if (made) recDelete((uint16_t)i);
            nak(reply, V2_NAK_NO_SLOT);
            return;
        }
        recs[i].offer = (uint16_t)j;
        recs[i].flags |= REC_OFFER;
    }
    if (!idle) {
        struct tagOffer *o = &offers[recs[i].offer];
        o->dataVer = sp->dataVer;
        o->dataSize = sp->dataSize;
        o->dataType = sp->dataType;
        o->dataTypeArgument = sp->dataTypeArg;
    }
    struct tagRec *r = &recs[i];
    if (!(r->flags & REC_PENDING)) {
        r->flags |= REC_PENDING;
        if (++recPending > pendPeak) pendPeak = recPending;
    }
    r->flags &= (uint8_t)~(REC_COUNTING | REC_STALE);
    r->expires = (uint16_t)(hkNow + sp->ttlMin);
    const uint16_t nc = sp->nextCheckin;
    if (nc && !(nc & 0x8000)) {
        r->sleepUntil = (uint16_t)(hkNow + nc);
        r->flags |= REC_COUNTING;
    } else {
        r->sleepUntil = nc;
    }
    if (oldVer != sp->dataVer) dropUnusedData(oldVer);
}

// The S3 has withdrawn the tag's entry; no XFER_TIMEOUT. A record left owning nothing goes.
static void cancelPending(const uint8_t *mac) {
    const int32_t i = recFind(mac);
    if (i < 0 || !(recs[i].flags & REC_PENDING)) return;
    dropUnusedData(pendEnd((uint16_t)i));
    if (!(recs[i].flags & REC_OWNED)) recDelete((uint16_t)i);
}

// Block data for the cache: what a tag asked for (DATA_NEEDED), or data the S3 sends in advance
static void dataChunk(const struct v2DataChunk *dc, size_t n, struct v2Reply *reply) {
    const int slot = cacheChunk(dc->dataVer, dc->offset, dc->totalSize, dc->bytes, n, verStillOffered(dc->dataVer));
    if (slot == CACHE_FULL) {
        nak(reply, V2_NAK_NO_CACHE_SPACE);
    } else if (slot == CACHE_BAD_CHUNK) {
        nak(reply, V2_NAK_INVALID_PARAM);
    } else if (blockStartTimer && cacheComplete(slot) && dc->dataVer == requestedData.ver &&
               dc->offset / V2_BLOCK_SIZE == requestedData.blockId) {
        // A block a tag is waiting for has just arrived: this is the time the ack's wait has to cover.
        const uint32_t ms = halMillis() - nextBlockAttempt;
        diag.dataMsSum += ms;
        if (ms > diag.dataMsMax) diag.dataMsMax = (uint16_t)(ms > 0xFFFF ? 0xFFFF : ms);
        diag.dataCount++;
        dataWaitArrived(ms);
    }
}

void v2HandleRequest(uint8_t type, const uint8_t *payload, size_t len, struct v2Reply *reply) {
    switch (type) {
        case V2_HELLO:
            if (len < sizeof(struct v2Hello)) {
                nak(reply, V2_NAK_BAD_LENGTH);
            } else if (((const struct v2Hello *)payload)->protoMajor != V2_PROTO_MAJOR) {
                nak(reply, V2_NAK_INVALID_PARAM);
            } else {
                fillHelloReply(reply);
                loopTimingRestart = true;
            }
            break;
        case V2_HEARTBEAT:
        case V2_GET_STATS:
            fillStatus(reply);
            break;
        case V2_SET_CONFIG:
            if (len < sizeof(struct v2SetConfig)) nak(reply, V2_NAK_BAD_LENGTH);
            else setConfig((const struct v2SetConfig *)payload, reply);
            break;
        case V2_SET_PENDING:
            if (len < sizeof(struct v2SetPending)) nak(reply, V2_NAK_BAD_LENGTH);
            else setPending((const struct v2SetPending *)payload, reply);
            break;
        case V2_CANCEL_PENDING:
            if (len < sizeof(struct v2CancelPending)) nak(reply, V2_NAK_BAD_LENGTH);
            else cancelPending(((const struct v2CancelPending *)payload)->mac);
            break;
        case V2_DATA_CHUNK:
            if (len <= sizeof(struct v2DataChunk) || len > sizeof(struct v2DataChunk) + V2_CHUNK_MAX) nak(reply, V2_NAK_BAD_LENGTH);
            else dataChunk((const struct v2DataChunk *)payload, len - sizeof(struct v2DataChunk), reply);
            break;
        case V2_DATA_DROP:
            if (len < sizeof(struct v2DataDrop)) nak(reply, V2_NAK_BAD_LENGTH);
            else cacheDropVer(((const struct v2DataDrop *)payload)->dataVer);
            break;
        case V2_ALLOW_CLEAR:
            allowClear();
            break;
        case V2_ALLOW_REMOVE:
            if (len == 0 || len % 8 || len / 8 > V2_ALLOW_MAX_MACS) {
                nak(reply, V2_NAK_BAD_LENGTH);
                break;
            }
            for (size_t c = 0; c < len; c += 8) allowRemove(payload + c);
            storeTrim();  // once for the request, not once a MAC
            break;
        case V2_ALLOW_ADD:
            if (len == 0 || len % 8 || len / 8 > V2_ALLOW_MAX_MACS) {
                nak(reply, V2_NAK_BAD_LENGTH);
                break;
            }
            for (size_t c = 0; c < len; c += 8) {
                if (!allowAdd(payload + c)) {
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

// Every frame goes out at the power the S3 configured. Not lowered per tag: block parts sent at a
// lower power cost a whole request round when lost.

// --- frames to the tags -------------------------------------------------------------------------

// Where a normal frame's payload starts in the packet: the length byte, the header, the packet type
#define NORMAL_PAYLOAD_AT (1 + sizeof(struct MacFrameNormal) + 1)

// The header of a frame to one tag, built in the radio's TX buffer; returns where the payload goes.
// Taking the buffer waits for our last frame to leave (halRadioTxFrame), so callers do their lookups
// first. From here to the send nothing else may transmit; after it the frame may be read, not written.
static uint8_t *prepareNormalFrame(const uint8_t *dst, uint16_t pan, uint8_t pktType, uint8_t payloadLen) {
    uint8_t *const packet = halRadioTxFrame();
    struct MacFrameNormal *txHeader = (struct MacFrameNormal *)(packet + 1);
    memset(packet, 0, NORMAL_PAYLOAD_AT);
    packet[0] = sizeof(struct MacFrameNormal) + 1 + payloadLen + RAW_PKT_PADDING;
    memcpy(txHeader->src, mSelfMac, 8);
    memcpy(txHeader->dst, dst, 8);
    txHeader->fcs.frameType = 1;
    txHeader->fcs.panIdCompressed = 1;
    txHeader->fcs.destAddrType = 3;
    txHeader->fcs.srcAddrType = 3;
    txHeader->seq = seq++;
    txHeader->pan = pan;  // the PAN (and so the radio) the tag used
    packet[NORMAL_PAYLOAD_AT - 1] = pktType;
    return packet + NORMAL_PAYLOAD_AT;
}

static void sendXferCompleteAck(const uint8_t *dst, uint16_t pan) {
    prepareNormalFrame(dst, pan, PKT_XFER_COMPLETE_ACK, 0);
    halRadioTxReply(RADIO_REPLY_AT_US);
}

static void sendCancelXfer(const uint8_t *dst, uint16_t pan) {
    prepareNormalFrame(dst, pan, PKT_CANCEL_XFER, 0);
    halRadioTxReply(RADIO_REPLY_AT_US);
}

#if AP_DEBUG
// The last PONG sent to a scanning tag, for the join line at its first check-in
static uint8_t pongMac[8];
static uint16_t pongAtUs = 0;
static uint32_t pongAtMs = 0;
static uint8_t pongCount = 0;  // pongs sent to this tag in this scan
#endif

static void sendPong(const uint8_t *buf) {
    const struct MacFrameBcast *rxframe = (const struct MacFrameBcast *)buf;
    uint8_t *payload = prepareNormalFrame(rxframe->src, rxframe->srcPan, PKT_PONG, 1);
    payload[0] = curChannel;
    const uint16_t delayUs = RADIO_REPLY_PONG_US;
    halRadioTxReply(delayUs);
    // Logging only after the reply is armed (see processBlockRequest); larm then includes this reply.
#if AP_DEBUG
    // a tag keeps pinging until it accepts a PONG, so this counts the attempts it needed
    if (memcmp(pongMac, rxframe->src, 8) != 0 || (uint32_t)(halMillis() - pongAtMs) > 60000) {
        memcpy(pongMac, rxframe->src, 8);
        pongCount = 0;
    }
    if (pongCount < 255) pongCount++;
    pongAtUs = delayUs;
    pongAtMs = halMillis();
#endif
#if AP_DEBUG
    // Rate limited: a scan is 20 pings in 40 ms and the log queue holds six lines.
    static uint32_t lastPingLogMs = 0;
    if ((uint32_t)(halMillis() - lastPingLogMs) > 2000) {
        lastPingLogMs = halMillis();
        char pb[V2_LOG_MAX];
        size_t pn = 0;
        diagAddHex(pb, &pn, sizeof(pb), "ping ",
                   ((uint32_t)rxframe->src[3] << 24) | ((uint32_t)rxframe->src[2] << 16) |
                       ((uint32_t)rxframe->src[1] << 8) | rxframe->src[0]);
        diagAdd(pb, &pn, sizeof(pb), "ch ", curChannel);
        diagAdd(pb, &pn, sizeof(pb), "age ", rx.ageUs);
        diagAdd(pb, &pn, sizeof(pb), "len ", payload[-(int)NORMAL_PAYLOAD_AT]);  // packet[0], only read
        diagAdd(pb, &pn, sizeof(pb), "larm ", halRadioTxLateArms());
        diagAddSigned(pb, &pn, sizeof(pb), "rs ", rx.rssi);
        diagAdd(pb, &pn, sizeof(pb), "at ", delayUs);
        pb[pn] = 0;
        v2Log(LOG_INFO, pb);
    }
#endif
}

// --- events to the S3 ---------------------------------------------------------------------------

// Asks the S3 for the block the tag requested; requestedData holds the request
static void requestBlockData(const uint8_t *src, int32_t rec, const struct blockRequest *br, uint16_t pleaseWaitMs) {
    struct v2DataNeeded dn;
    memcpy(dn.mac, src, 8);
    dn.dataVer = br->ver;
    dn.offset = (uint32_t)br->blockId * V2_BLOCK_SIZE;
    dn.length = V2_BLOCK_SIZE;
    if (rec >= 0 && (recs[rec].flags & REC_OFFER)) {
        const uint32_t size = offers[recs[rec].offer].dataSize;
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

static void espNotifyTagReturnData(const uint8_t *src, const struct tagReturnData *trd, uint8_t len) {
    // A tag that missed our ack sends the same report again. Match the MAC too: tags running the same
    // image carry the same data version.
    if (memcmp(&trd->dataVer, lastTagReturn, 8) == 0 && memcmp(src, lastTagReturnMac, 8) == 0) return;
    memcpy(lastTagReturn, &trd->dataVer, 8);
    memcpy(lastTagReturnMac, src, 8);

    if (len > sizeof(struct tagReturnData)) len = sizeof(struct tagReturnData);
    uint8_t buf[sizeof(struct v2TagReturnData) + sizeof(struct tagReturnData)];
    struct v2TagReturnData *e = (struct v2TagReturnData *)buf;
    memcpy(e->mac, src, 8);
    e->timestampUs = rx.sfdUs;
    e->rssi = rx.rssi;
    e->lqi = rx.lqi;
    memcpy(e->data, trd, len);
    v2QueueEvent(V2_TAG_RETURN_DATA, buf, sizeof(struct v2TagReturnData) + len, false);
}

// --- frames from the tags -----------------------------------------------------------------------

// "blk 0 parts 18 of 42: 0 1 2 ..." (the first parts asked for), to see which parts a tag misses
static void logBlockRequest(const struct blockRequest *br) {
    char buf[96];
    size_t n = 0;
    uint8_t count = 0;
    for (uint8_t c = 0; c < BLOCK_MAX_PARTS; c++) {
        if (br->requestedParts[c / 8] & (1 << (c % 8))) count++;
    }
    diagAdd(buf, &n, sizeof(buf), "blk ", br->blockId);
    diagAdd(buf, &n, sizeof(buf), "parts ", count);
    for (uint8_t c = 0; c < BLOCK_MAX_PARTS && n < sizeof(buf) - 5; c++) {
        if (br->requestedParts[c / 8] & (1 << (c % 8))) diagAdd(buf, &n, sizeof(buf), "", c);
    }
    buf[n] = 0;
    v2Log(LOG_INFO, buf);
}

// "rereq blk 0 n 2 pos 0 18 2 20": a tag asks again for parts of the block it was just sent, and
// where in that burst those parts went out
static void logReRequest(const struct blockRequest *br) {
    char buf[V2_LOG_MAX];
    size_t n = 0;
    uint8_t count = 0;
    for (uint8_t c = 0; c < BLOCK_MAX_PARTS; c++) {
        if (br->requestedParts[c / 8] & (1 << (c % 8))) count++;
    }
    diag.rereqs++;
    diag.rereqParts += count;
    diagAdd(buf, &n, sizeof(buf), "rereq blk ", br->blockId);
    diagAdd(buf, &n, sizeof(buf), "n ", count);
    diagAdd(buf, &n, sizeof(buf), "of ", lastBurstLen);
    if (n + 4 >= sizeof(buf)) return;
    memcpy(buf + n, "pos ", 4);
    n += 4;
    for (uint8_t p = 0; p < lastBurstLen && n < sizeof(buf) - 5; p++) {
        const uint8_t part = lastBurstParts[p];
        if (br->requestedParts[part / 8] & (1 << (part % 8))) diagAdd(buf, &n, sizeof(buf), "", p);
    }
    buf[n] = 0;
    v2Log(LOG_INFO, buf);
}

// A block request: ack it with the wait that fits where the block is, schedule the burst, and ask the
// S3 for the block when the cache has not got it. Every path logs only after its ack is armed: each
// diagAdd is a divide with interrupts masked (div_mod.S), and a 27-part list takes about 320 us,
// while a scheduled reply fires on its own tick regardless.
static void processBlockRequest(const uint8_t *buffer) {
    const struct MacFrameNormal *rxHeader = (const struct MacFrameNormal *)buffer;
    const struct blockRequest *blockReq = (const struct blockRequest *)(buffer + sizeof(struct MacFrameNormal) + 1);
    if (!checkCRC(blockReq, sizeof(struct blockRequest))) return;
    diag.blockReqs++;
    // the next block of the same image: the tag has the one it was sent
    if (blockReq->ver == requestedData.ver && blockReq->blockId != requestedData.blockId) burstDoneSeen(rxHeader->src, false);
    // Before any reason to turn the tag away: a tag told to wait comes back for this block, so the
    // cache keeps it.
    cacheRequested(blockReq->ver, blockReq->blockId);

    // A tag asking again for exactly what the running burst sends missed the ack. Ack again with
    // pleaseWaitMs 0 and keep the burst running, holding its frames for ZERO_WAIT_HOLD_MS. Not a
    // non-zero wait: it would have to be at least 10 ms, which many tags busy-wait with the receiver on.
    if (burstActive && memcmp(rxHeader->src, dstMac, 8) == 0) {
        if (blockReq->blockId == requestedData.blockId && blockReq->ver == requestedData.ver &&
            memcmp(blockReq->requestedParts, requestedData.requestedParts, BLOCK_REQ_PARTS_BYTES) == 0) {
            struct blockRequestAck *again = (struct blockRequestAck *)prepareNormalFrame(
                rxHeader->src, rxHeader->pan, PKT_BLOCK_REQUEST_ACK, sizeof(struct blockRequestAck));
            again->pleaseWaitMs = 0;  // "carry on receiving", the tag's continueToRX case
            addCRC(again, sizeof(struct blockRequestAck));
            halRadioTxReply(RADIO_REPLY_AT_US);
            diag.ackResends++;
            uint32_t ackInUs = (halRadioTxEndUs() - halMicros()) & HAL_US_MASK;
            if (ackInUs > 20000) ackInUs = 0;  // the ack has left already, or the clocks disagree
            burstHoldUntil = halMillis() + (ackInUs + 999) / 1000 + ZERO_WAIT_HOLD_MS;
            if (burstHoldUntil == 0) burstHoldUntil = 1;  // 0 means "nothing held"
            logBlockRequest(blockReq);
            return;
        }
        // something else: another block, or the parts it is still missing after its window closed
        burstFinish();
    }
    const bool isReRequest = lastBurstLen && blockReq->blockId == lastBurstBlock && blockReq->ver == lastBurstVer;

    // no data for this mac
    if (rxRec < 0 || !(recs[rxRec].flags & REC_PENDING)) {
        diag.cancelsNoData++;
        if (queuedReq.startMs && memcmp(rxHeader->src, queuedReq.mac, 8) == 0) queuedReq.startMs = 0;
        sendCancelXfer(rxHeader->src, rxHeader->pan);
        logBlockRequest(blockReq);
        if (isReRequest) logReRequest(blockReq);
        return;
    }

    // Another tag's burst is due or on the air: wait in the slot (replacing this tag's own entry), or be
    // turned away if another tag holds it.
    const bool forOther = memcmp(rxHeader->src, dstMac, 8) != 0;
    if (forOther && (burstActive || blockStartTimer)) {
        if (queuedReq.startMs && memcmp(rxHeader->src, queuedReq.mac, 8) != 0) {
            diag.cancelsBusy++;
            sendCancelXfer(rxHeader->src, rxHeader->pan);
        } else {
            queueBlockRequest(rxHeader->src, rxHeader->pan, blockReq);
        }
        logBlockRequest(blockReq);
        if (isReRequest) logReRequest(blockReq);
        return;
    }

    // The block is in the cache, on its way for an earlier request, or has to come from the S3
    const bool sameBlock = blockReq->blockId == requestedData.blockId && blockReq->ver == requestedData.ver;
    const bool cached = cacheComplete(cacheFind(blockReq->ver, blockReq->blockId));
    const bool coming = !cached && sameBlock && (halMillis() - nextBlockAttempt) <= 300;

    memcpy(&requestedData, blockReq, sizeof(struct blockRequest));

    struct blockRequestAck *blockRequestAck =
        (struct blockRequestAck *)prepareNormalFrame(rxHeader->src, rxHeader->pan, PKT_BLOCK_REQUEST_ACK, sizeof(struct blockRequestAck));
    blockRequestAck->pleaseWaitMs = cached ? 0 : coming ? PLEASE_WAIT_COMING_MS : dataWaitMs;
    burstMarginMs = cached ? ZERO_WAIT_HOLD_MS : BURST_MARGIN_MS;
    if (cached) diag.zeroBursts++;
    const uint16_t pleaseWaitMs = blockRequestAck->pleaseWaitMs;
    addCRC(blockRequestAck, sizeof(struct blockRequestAck));
    halRadioTxReply(RADIO_REPLY_AT_US);
    logBlockRequest(blockReq);
    if (isReRequest) logReRequest(blockReq);

    // The tag counts its wait from the end of this ack, about 2 ms from now
    uint32_t ackInUs = (halRadioTxEndUs() - halMicros()) & HAL_US_MASK;
    if (ackInUs > 20000) ackInUs = 0;  // the ack has left already, or the clocks disagree
    blockStartTimer = halMillis() + (ackInUs + 999) / 1000 + pleaseWaitMs + burstMarginMs;
    if (blockStartTimer == 0) blockStartTimer = 1;  // 0 means "no burst due"

    // save the target for the block data
    memcpy(dstMac, rxHeader->src, 8);
    dstPan = rxHeader->pan;

    if (!cached && !coming) {
        requestBlockData(rxHeader->src, rxRec, blockReq, pleaseWaitMs);
        nextBlockAttempt = halMillis();
    }
}

static void processAvailDataReq(const uint8_t *buffer, uint8_t kind) {
    const struct MacFrameBcast *rxHeader = (const struct MacFrameBcast *)buffer;
    const struct AvailDataReq *availDataReq = (const struct AvailDataReq *)(buffer + sizeof(struct MacFrameBcast) + 1);

    if (!checkCRC(availDataReq, sizeof(struct AvailDataReq))) return;
#if AP_DEBUG
    // The first check-in after a scan, which can come tens of seconds after the accepted PONG. Once per
    // joining tag, so not rate limited.
    if (pongAtUs && memcmp(rxHeader->src, pongMac, 8) == 0 && (uint32_t)(halMillis() - pongAtMs) < 120000) {
        char jb[V2_LOG_MAX];
        size_t jn = 0;
        diagAddHex(jb, &jn, sizeof(jb), "joined ",
                   ((uint32_t)pongMac[3] << 24) | ((uint32_t)pongMac[2] << 16) | ((uint32_t)pongMac[1] << 8) | pongMac[0]);
        diagAdd(jb, &jn, sizeof(jb), "after ", pongCount);
        // `last`: the offset of the accepted PONG; `after`: PONGs the scan took (1 when it is right)
        diagAdd(jb, &jn, sizeof(jb), "last ", pongAtUs);
        diagAddSigned(jb, &jn, sizeof(jb), "rs ", rx.rssi);
        jb[jn] = 0;
        v2Log(LOG_INFO, jb);
        pongAtUs = 0;  // one line per join, not one per check-in that follows it
    }
#endif
    burstStopFor(rxHeader->src);  // a tag that checks in again is not waiting for block parts
    // Nothing pending, or an idle entry, leaves the fields zero, DATATYPE_NOUPDATE
    struct AvailDataInfo *availDataInfo =
        (struct AvailDataInfo *)prepareNormalFrame(rxHeader->src, rxHeader->dstPan, PKT_AVAIL_DATA_INFO, sizeof(struct AvailDataInfo));
    memset(availDataInfo, 0, sizeof(struct AvailDataInfo));
    if (rxRec >= 0 && (recs[rxRec].flags & REC_PENDING)) {
        const struct tagRec *r = &recs[rxRec];
        if (r->flags & REC_OFFER) memcpy(&availDataInfo->dataVer, &offers[r->offer], OFFER_REPLY_BYTES);
        availDataInfo->nextCheckIn = recNextCheckIn(r);
    }
    addCRC(availDataInfo, sizeof(struct AvailDataInfo));
    // from the request's start to the reply armed
    const uint32_t replyUs = (halMicros() - rx.sfdUs) & HAL_US_MASK;
    halRadioTxReply(RADIO_REPLY_AT_US);
    memset(lastAckMac, 0, 8);  // so we can record if we've received exactly one ack packet

    diag.checkins++;
#if AP_DEBUG
    if (diag.checkins == 1 || rx.freqOffset < diag.foMin) diag.foMin = rx.freqOffset;
    if (diag.checkins == 1 || rx.freqOffset > diag.foMax) diag.foMax = rx.freqOffset;
    diag.fdcHiOr |= rx.fdcHigh;
    diag.foSum += rx.freqOffset;
    logCheckinOffset(rxHeader->src);
#endif
    if (availDataInfo->dataType != DATATYPE_NOUPDATE) diag.offered++;
    diag.replyUsSum += replyUs;
    if (replyUs > diag.replyUsMax) diag.replyUsMax = replyUs;
    // `rep` counts a repeat only when the previous check-in of all was the same tag's.
    if (memcmp(lastCheckinMac, rxHeader->src, 8) == 0 && halMillis() - lastCheckinMs < REPEAT_WINDOW_MS) {
        diag.repeats++;
    }
    memcpy(lastCheckinMac, rxHeader->src, 8);
    lastCheckinMs = halMillis();

    struct v2Checkin ci;
    memset(&ci, 0, sizeof(ci));
    memcpy(ci.mac, rxHeader->src, 8);
    ci.timestampUs = rx.sfdUs;
    ci.rssi = rx.rssi;
    ci.lqi = rx.lqi;
    ci.radio = V2_RADIO_24GHZ;
    ci.channel = curChannel;
    ci.pan = rxHeader->dstPan;
    ci.kind = kind;
    ci.answeredVer = availDataInfo->dataVer;
    ci.answeredType = availDataInfo->dataType;
    memcpy(ci.adr, availDataReq, sizeof(struct AvailDataReq));
    v2QueueEvent(V2_CHECKIN, &ci, sizeof(ci), false);
}

static void processXferComplete(const uint8_t *buffer) {
    const struct MacFrameNormal *rxHeader = (const struct MacFrameNormal *)buffer;
    burstDoneSeen(rxHeader->src, true);
    burstStopFor(rxHeader->src);  // it has the data; the rest of the burst would go to nobody
    sendXferCompleteAck(rxHeader->src, rxHeader->pan);
    diag.xfc++;
    if (memcmp(lastAckMac, rxHeader->src, 8) != 0) {
        memcpy(lastAckMac, rxHeader->src, 8);
        // The record stays until housekeeping, for the idle SET_PENDING that usually follows
        const bool pending = rxRec >= 0 && (recs[rxRec].flags & REC_PENDING);
        const uint64_t ver = pending && (recs[rxRec].flags & REC_OFFER) ? offers[recs[rxRec].offer].dataVer : 0;
        notifyXferEnd(V2_XFER_COMPLETE, rxHeader->src, ver);
        if (pending) dropUnusedData(pendEnd((uint16_t)rxRec));
    }
}

static void processTagReturnData(const uint8_t *buffer, uint8_t len) {
    const struct MacFrameBcast *rxframe = (const struct MacFrameBcast *)buffer;
    const uint8_t hdr = sizeof(struct MacFrameBcast) + 1;
    if (len <= hdr || !checkCRC(buffer + hdr, len - hdr)) return;
    prepareNormalFrame(rxframe->src, rxframe->srcPan, PKT_TAG_RETURN_DATA_ACK, 0);
    halRadioTxReply(RADIO_REPLY_AT_US);
    espNotifyTagReturnData(rxframe->src, (const struct tagReturnData *)(buffer + hdr), len - hdr);
}

// --- block bursts -------------------------------------------------------------------------------
// The life of a burst: processBlockRequest() acks the request with a pleaseWaitMs and sets
// blockStartTimer to just after that wait (ZERO_WAIT_HOLD_MS, BURST_MARGIN_MS). At that moment apLoop() calls startBurst(), if the block is complete, which lays the frames out in passes
// (every requested part once per pass). burstStep() then sends one frame per main-loop pass, waiting
// between passes (burstPassDueMs) and for frames that are arriving, until the frames are out, the
// window has closed, or the tag has moved on (burstStopFor). burstFinish() reports BLOCK_SERVED and
// hands the radio to a tag waiting its turn (promoteQueued).

// Sends one part, unless the receiver is busy with a frame (yieldRx), which the main loop then answers
// first, or the channel is in use (yieldEnergy). The sequence number is only taken when the frame
// really goes.
static bool sendPart(uint8_t partNo, bool yieldRx, bool yieldEnergy) {
    const uint8_t keepSeq = seq;
    struct blockPart *blockPart =
        (struct blockPart *)prepareNormalFrame(dstMac, dstPan, PKT_BLOCK_PART, sizeof(struct blockPart) + BLOCK_PART_DATA_SIZE);
    blockPart->blockId = requestedData.blockId;
    blockPart->blockPart = partNo;
    cacheCopyPart(burstSlot, partNo, blockPart->data);
    addCRC(blockPart, sizeof(struct blockPart) + BLOCK_PART_DATA_SIZE);
    if (halRadioTx(yieldRx, yieldEnergy)) {
        diag.burstTxUs += BURST_FRAME_US;
        return true;
    }
    seq = keepSeq;
    return false;
}

static uint8_t channelBusyPct(void);

static void startBurst(int slot, int32_t lateMs) {
    if (getBlockDataLength() == 0) requestedData.requestedParts[0] |= 0x01;  // invalid request, 0 parts

    // the parts the tag asked for, in order
    uint8_t parts[BLOCK_MAX_PARTS];
    uint8_t n = 0;
    for (uint8_t c = 0; c < BLOCK_MAX_PARTS; c++) {
        if (requestedData.requestedParts[c / 8] & (1 << (c % 8))) parts[n++] = c;
    }

    // Every requested part at least once, then up to burstCopies of each, as many as fit in what is left
    // of the tag's window (less when the block data arrived late)
    uint32_t budgetMs = BURST_WINDOW_MS;
    if (lateMs > 0) budgetMs = (uint32_t)lateMs < BURST_WINDOW_MS ? BURST_WINDOW_MS - (uint32_t)lateMs : 0;
    uint16_t frames = (uint16_t)((budgetMs * 1000UL) / (BURST_FRAME_US + RADIO_BURST_GAP_US));
    if (frames > (uint16_t)n * burstCopies) frames = (uint16_t)n * burstCopies;
    if (frames < n) frames = n;  // every part goes out at least once, late or not

    // A partial repeat pass covers the first parts, the ones a tag still settling in tends to miss. For
    // an even part count, every other pass swaps adjacent parts (1 0 3 2 ...), so the copies of a part
    // are an odd distance apart and a tag that misses every second frame still gets each part.
    burstLen = 0;
    for (uint8_t pass = 0; burstLen < frames; pass++) {
        const bool swap = (n % 2 == 0) && (pass % 2);
        for (uint8_t i = 0; i < n && burstLen < frames; i++) {
            burstParts[burstLen++] = parts[swap ? (i ^ 1) : i];
        }
    }
    burstPos = 0;
    burstHoldUntil = 0;
    burstLateMs = (int16_t)(lateMs + burstMarginMs);  // after the wait the tag was told
    burstPassLen = n;
    burstPassFrom = 0;
    burstWindowEndMs = halMillis() + budgetMs;
    // A pass later than the request, so the record is looked for again: the store may have moved.
    const int32_t rec = recFind(dstMac);
    const struct tagOffer *o = rec >= 0 && (recs[rec].flags & REC_OFFER) ? &offers[recs[rec].offer] : NULL;
    burstFinalBlock = o && o->dataVer == requestedData.ver && o->dataSize &&
                      requestedData.blockId == (o->dataSize - 1) / BLOCK_DATA_SIZE;
    burstDoneFromMs = 0;
    burstYield = BURST_YIELD_FREE;
    burstYieldChargedUs = 0;
    burstYieldToFlag = true;
    burstYieldEnergy = channelBusyPct() < BURST_BUSY_HOLD_PCT;
    burstSlot = slot;
    cachePin(slot);
    cacheServed(slot);
    burstActive = true;
}

static uint16_t burstAwaiting = 0;  // records with REC_AWAIT set: burstAnswerSeen's lookup is skipped while 0

// Ends the burst, reporting the frames that really went out (burstPos) and keeping its schedule for
// logReRequest.
static void burstFinish(void) {
    const uint8_t sent = burstPos;
    burstActive = false;
    burstHoldUntil = 0;
    cachePin(-1);
    diag.bursts++;
    lastBurstVer = requestedData.ver;
    lastBurstBlock = requestedData.blockId;
    lastBurstLen = sent;
    memcpy(lastBurstParts, burstParts, sent);
    airAcc.yieldMs = airAdd(airAcc.yieldMs, (burstYieldChargedUs + 500) / 1000);
    struct v2BlockServed bs;
    memcpy(bs.mac, dstMac, 8);
    bs.dataVer = requestedData.ver;
    bs.blockId = requestedData.blockId;
    bs.partsRequested = getBlockDataLength();
    bs.framesSent = sent;
    bs.lateMs = burstLateMs;
    v2QueueEvent(V2_BLOCK_SERVED, &bs, sizeof(bs), false);
    // What the tag sends next says whether it got the block (burstAnswerSeen). Before promoteQueued,
    // which changes burstMarginMs to the next burst's.
    const int32_t r = recFind(dstMac);
    if (r >= 0) {
        if (!(recs[r].flags & REC_AWAIT)) burstAwaiting++;
        recs[r].flags = (uint8_t)((recs[r].flags & ~REC_AWAIT_ZERO) | REC_AWAIT |
                                  (burstMarginMs == ZERO_WAIT_HOLD_MS ? REC_AWAIT_ZERO : 0));
    }
    promoteQueued();  // the radio is free: a tag waiting its turn has it now
}

// Whether a tag answered the burst it was sent, from its next frame: a block request or XFC is an
// answer; a check-in or ping means it gave up on the transfer. Counted apart for bursts after a wait of
// 0 and after any other wait.
static void burstAnswerSeen(const uint8_t *src, uint8_t pktType) {
    if (!burstAwaiting) return;
    bool answered;
    switch (pktType) {
        case PKT_XFER_COMPLETE:
        case PKT_BLOCK_REQUEST:
        case PKT_BLOCK_PARTIAL_REQUEST:
            answered = true;
            break;
        case PKT_AVAIL_DATA_REQ:
        case PKT_AVAIL_DATA_SHORTREQ:
        case PKT_PING:
            answered = false;
            break;
        default:
            return;  // tag return data and the like say nothing about the burst
    }
    const int32_t r = recFind(src);  // again: handling the frame may have moved the store
    if (r < 0 || !(recs[r].flags & REC_AWAIT)) return;
    if (!answered) {
        if (recs[r].flags & REC_AWAIT_ZERO) diag.silentZero++;
        else diag.silentWaited++;
    }
    recs[r].flags &= (uint8_t)~(REC_AWAIT | REC_AWAIT_ZERO);
    burstAwaiting--;
}

// The tag a burst is for has moved on (XFC, a check-in, another block): the rest of the burst would
// reach nobody.
static void burstStopFor(const uint8_t *mac) {
    if (!burstActive || memcmp(mac, dstMac, 8) != 0) return;
    diag.burstsCut++;
    burstFinish();
}

// Start timing burstDoneWaitMs from the end of the frame now on the air
static void burstDoneWatchFrom(void) {
    if (burstDoneFromMs) return;
    uint32_t inUs = (halRadioTxEndUs() - halMicros()) & HAL_US_MASK;
    if (inUs > 20000) inUs = 0;  // the frame has left already, or the clocks disagree
    burstDoneFromMs = halMillis() + (inUs + 500) / 1000;
    if (!burstDoneFromMs) burstDoneFromMs = 1;
}

// When the pass starting at the current position may begin: the first at once, the others
// burstDoneWaitMs after the last.
static uint32_t burstPassDueMs(uint32_t now) {
    const uint8_t pass = burstPos / burstPassLen;
    if (pass == 0) return now;
    if (pass == 1) burstDoneWatchFrom();
    // No wait when not one repeat would fit after it (a ZBS243 sends XFC 0.5-1.5 s after the last
    // block, so in practice only the next-block wait pauses a burst)
    const uint32_t wait = burstDoneWaitMs[burstFinalBlock];
    const uint32_t slotMs = (BURST_FRAME_US + RADIO_BURST_GAP_US + 999) / 1000;
    if ((int32_t)(now + wait + slotMs - burstWindowEndMs) > 0) return now;
    return now + wait;
}

// Sends the next part once the radio is free, the hold is over (burstHoldUntil), its pass is due, and
// the receiver is not in the middle of somebody's frame.
static void burstStep(void) {
    if (!burstActive || halRadioTxBusy()) return;
    const uint32_t now = halMillis();
    if (burstPos % burstPassLen == 0 && burstPos != burstPassFrom) {
        burstPassFrom = burstPos;
        const uint32_t due = burstPassDueMs(now);
        if (!burstHoldUntil || (int32_t)(due - burstHoldUntil) > 0) burstHoldUntil = due ? due : 1;
    }
    if (burstHoldUntil) {
        if ((int32_t)(now - burstHoldUntil) < 0) return;
        burstHoldUntil = 0;
    }
    // every part at least once; repeats only inside the tag's window
    if (burstPos >= burstPassLen && (int32_t)(now - burstWindowEndMs) >= 0) {
        burstDoneWatchFrom();
        burstFinish();
        return;
    }
    // Give way to a frame or to something else on the channel, within the budget
    if (burstYield == BURST_YIELD_HOLDING) {
        const uint8_t rx = (uint8_t)halRadioRxFrames();
        const uint32_t nowUs = halMicros();
        if (rx != burstYieldRx) {
            // a good frame came of it (one that failed its CRC still held us): nothing to charge
            airAcc.yieldFrames = airAdd(airAcc.yieldFrames, 1);
            burstYieldRx = rx;
            burstYieldSinceUs = nowUs;
            burstFlagUpSinceUs = nowUs;
        } else if (burstYieldChargedUs + ((nowUs - burstYieldSinceUs) & HAL_US_MASK) > BURST_YIELD_BUDGET_US) {
            burstYieldChargedUs = BURST_YIELD_BUDGET_US;
            burstYield = BURST_YIELD_OFF;
        }
        if (!halRadioRxArriving()) burstFlagUpSinceUs = nowUs;
        else if (((nowUs - burstFlagUpSinceUs) & HAL_US_MASK) > RADIO_MAX_FRAME_US) burstYieldToFlag = false;
    }
    const bool yieldOn = burstYield != BURST_YIELD_OFF;
    if (!sendPart(burstParts[burstPos], yieldOn && burstYieldToFlag, yieldOn && burstYieldEnergy)) {
        if (burstYield == BURST_YIELD_FREE) {
            burstYield = BURST_YIELD_HOLDING;
            burstYieldSinceUs = halMicros();
            burstFlagUpSinceUs = burstYieldSinceUs;
            burstYieldRx = (uint8_t)halRadioRxFrames();
            if (airAcc.yields != 0xFFFF) airAcc.yields++;
        }
        return;
    }
    if (burstYield == BURST_YIELD_HOLDING) {
        // the hold is over: was it a frame?
        if ((uint8_t)halRadioRxFrames() != burstYieldRx) airAcc.yieldFrames = airAdd(airAcc.yieldFrames, 1);
        else burstYieldChargedUs += (halMicros() - burstYieldSinceUs) & HAL_US_MASK;
        burstYield = BURST_YIELD_FREE;
    }
    burstPos++;
    if (burstPos < burstLen) return;
    burstDoneWatchFrom();
    burstFinish();
}

// The tag a burst went to says it is done with the block (next block request, or XFC after the last):
// one more reading for burstDoneWaitMs. Only the signal that fits the block counts.
static void burstDoneSeen(const uint8_t *mac, bool xfc) {
    if (!burstDoneFromMs || memcmp(mac, dstMac, 8) != 0) return;
    const uint32_t ms = halMillis() - burstDoneFromMs;
    burstDoneFromMs = 0;
    if (xfc != burstFinalBlock) return;
    const uint8_t k = burstFinalBlock;
    const uint16_t m = ms > 0xFFFE ? 0xFFFE : (ms ? (uint16_t)ms : 1);
    if (m < burstDoneSeenMin[k]) burstDoneSeenMin[k] = m;
    if (!burstDoneWaitMs[k] || m < burstDoneWaitMs[k]) burstDoneWaitMs[k] = m;
}

// Once per reporting window: if every reading was longer than the wait, the smallest becomes the wait
static void burstDoneWaitRoll(void) {
    for (uint8_t k = 0; k < 2; k++) {
        if (burstDoneSeenMin[k] != 0xFFFF && burstDoneSeenMin[k] > burstDoneWaitMs[k]) burstDoneWaitMs[k] = burstDoneSeenMin[k];
        burstDoneSeenMin[k] = 0xFFFF;
    }
}

// --- a second tag waiting its turn -------------------------------------------------------------

#define BURST_SLOT_US (BURST_FRAME_US + RADIO_BURST_GAP_US)

// When the radio is free again: the planned end of the burst on the air or due, never past its window
static uint32_t radioBusyUntilMs(void) {
    const uint32_t now = halMillis();
    if (burstActive) {
        uint32_t end = now + ((uint32_t)(burstLen - burstPos) * BURST_SLOT_US + 999) / 1000;
        if (burstPos < burstPassLen && burstPassLen < burstLen) end += burstDoneWaitMs[burstFinalBlock];
        if (burstHoldUntil && (int32_t)(burstHoldUntil - now) > 0) end += burstHoldUntil - now;
        return (int32_t)(end - burstWindowEndMs) > 0 ? burstWindowEndMs : end;
    }
    const uint32_t windowEnd = blockStartTimer + BURST_WINDOW_MS;
    const uint8_t n = getBlockDataLength() ? getBlockDataLength() : 1;
    uint32_t frames = (uint32_t)n * burstCopies;
    if (frames > BURST_MAX_FRAMES) frames = BURST_MAX_FRAMES;
    if (frames < n) frames = n;
    uint32_t end = blockStartTimer + (frames * BURST_SLOT_US + 999) / 1000;
    if (frames > n) end += burstDoneWaitMs[0];
    return (int32_t)(end - windowEnd) > 0 ? windowEnd : end;
}

// Acks a tag whose burst has to wait for another one, with a wait that has it start listening as the
// radio is free, and puts it in the slot. Its block is asked of the S3 now.
static void queueBlockRequest(const uint8_t *src, uint16_t pan, const struct blockRequest *br) {
    const bool cached = cacheComplete(cacheFind(br->ver, br->blockId));
    const bool again = queuedReq.startMs && memcmp(src, queuedReq.mac, 8) == 0 && queuedReq.req.ver == br->ver &&
                       queuedReq.req.blockId == br->blockId;
    const bool coming = !cached && ((br->ver == requestedData.ver && br->blockId == requestedData.blockId) ||
                                    (again && queuedReq.askedMs));
    uint32_t pw = cached ? PLEASE_WAIT_QUEUED_MS : coming ? PLEASE_WAIT_COMING_MS : dataWaitMs;
    // Lengthen the wait so that its burst starts after the one in front of it; the ack ends about 3 ms
    // from now.
    const int32_t need = (int32_t)(radioBusyUntilMs() - (halMillis() + 3));
    if ((int32_t)pw + BURST_MARGIN_MS < need) pw = (uint32_t)(need - BURST_MARGIN_MS);
    if (pw > 0xFFFF) pw = 0xFFFF;
    struct blockRequestAck *ack =
        (struct blockRequestAck *)prepareNormalFrame(src, pan, PKT_BLOCK_REQUEST_ACK, sizeof(struct blockRequestAck));
    ack->pleaseWaitMs = (uint16_t)pw;
    addCRC(ack, sizeof(struct blockRequestAck));
    halRadioTxReply(RADIO_REPLY_AT_US);
    uint32_t ackInUs = (halRadioTxEndUs() - halMicros()) & HAL_US_MASK;
    if (ackInUs > 20000) ackInUs = 0;
    const uint32_t askedMs = again ? queuedReq.askedMs : 0;
    memcpy(&queuedReq.req, br, sizeof(struct blockRequest));
    memcpy(queuedReq.mac, src, 8);
    queuedReq.pan = pan;
    queuedReq.askedMs = askedMs;
    queuedReq.startMs = halMillis() + (ackInUs + 999) / 1000 + pw + BURST_MARGIN_MS;
    if (!queuedReq.startMs) queuedReq.startMs = 1;
    if (!cached && !coming) {
        requestBlockData(src, recFind(src), br, (uint16_t)pw);  // the waiting tag's own record, for its offer
        queuedReq.askedMs = halMillis() ? halMillis() : 1;
    }
    diag.queued++;
}

// The radio is free: the waiting tag's burst becomes the one that is due, at the time its tag was told
static void promoteQueued(void) {
    if (!queuedReq.startMs || burstActive || blockStartTimer) return;
    memcpy(&requestedData, &queuedReq.req, sizeof(struct blockRequest));
    memcpy(dstMac, queuedReq.mac, 8);
    dstPan = queuedReq.pan;
    burstMarginMs = BURST_MARGIN_MS;
    blockStartTimer = queuedReq.startMs;
    if (queuedReq.askedMs) nextBlockAttempt = queuedReq.askedMs;
    queuedReq.startMs = 0;
}

// --- main loop ----------------------------------------------------------------------------------

// The frame is read, and the short check-in forms padded with zeroes, in place in the receive slot: it
// is ours until apLoop() releases it, and has room for the padding (127 bytes plus a trailer already
// decoded into rx). Anything needed after that is copied out.
static void handleRadioFrame(void) {
    uint8_t *buf = rx.data;
    const uint8_t len = rx.len;
    halLedFlash();
    uint8_t pktType = getPacketType(buf);
    if (tooLateToAnswer(pktType, rx.ageUs)) {
        lateRequestsSkipped++;
        statusLateSkipped++;
        diag.late++;
        return;
    }
    const struct MacFcs *fcs = (const struct MacFcs *)buf;
    const uint8_t *src = fcs->destAddrType == 2 ? ((const struct MacFrameBcast *)buf)->src : ((const struct MacFrameNormal *)buf)->src;
    rxRec = pktType ? recFind(src) : -1;
    if (pktType && !tagAccepted(pktType, buf, len, rxRec)) {
        lockedIgnored++;
        return;
    }
    // othersHeard (below) is counted after the frame is handled: an answered block request makes its
    // sender the one the bursts are for
    switch (pktType) {
        case PKT_AVAIL_DATA_REQ:
            // each case accepts only its payload's exact lengths; anything else counts as unhandled
            if (len != 28 && len != 40) diag.unhandled++;
            if (len == 28) {
                // old version of the AvailDataReq struct: zero the new fields so it passes the CRC
                memset(buf + 1 + sizeof(struct MacFrameBcast) + sizeof(struct oldAvailDataReq), 0,
                       sizeof(struct AvailDataReq) - sizeof(struct oldAvailDataReq) + 2);
                processAvailDataReq(buf, V2_CHECKIN_FULL);
            } else if (len == 40) {
                processAvailDataReq(buf, V2_CHECKIN_FULL);
            }
            break;
        case PKT_BLOCK_REQUEST:
        case PKT_BLOCK_PARTIAL_REQUEST:
            if (len >= sizeof(struct MacFrameNormal) + 1 + sizeof(struct blockRequest)) processBlockRequest(buf);
            else diag.unhandled++;
            break;
        case PKT_XFER_COMPLETE:
            if (len >= sizeof(struct MacFrameNormal) + 1) processXferComplete(buf);
            else diag.unhandled++;
            break;
        case PKT_PING:
            if (len >= sizeof(struct MacFrameBcast) + 1) {
                sendPong(buf);
                diag.pings++;
            } else {
                diag.unhandled++;
            }
            break;
        case PKT_AVAIL_DATA_SHORTREQ:
            // a short AvailDataReq, one byte of payload: its fields are zeroed so it passes the checksum
            // and the S3 can tell no payload came with it
            if (len == 18) {
                memset(buf + 1 + sizeof(struct MacFrameBcast), 0, sizeof(struct AvailDataReq) + 2);
                processAvailDataReq(buf, V2_CHECKIN_SHORT);
            } else {
                diag.unhandled++;
            }
            break;
        case PKT_TAG_RETURN_DATA:
            processTagReturnData(buf, len);
            break;
        default:
            // another installation's traffic, an unknown protocol, or noise that passed the CRC
            diag.unhandled++;
            break;
    }
    if (pktType && memcmp(src, dstMac, 8) != 0) diag.othersHeard++;
    burstAnswerSeen(src, pktType);  // after the frame is handled: a burst it cut short is marked by then
}

// "name value " into buf at *n
static void diagAdd(char *buf, size_t *n, size_t max, const char *name, uint32_t value) {
    char digits[11];
    uint8_t d = 0;
    do {
        digits[d++] = (char)('0' + value % 10);
        value /= 10;
    } while (value && d < sizeof(digits));
    const size_t nameLen = strlen(name);
    if (*n + nameLen + d + 2 >= max) return;
    memcpy(buf + *n, name, nameLen);
    *n += nameLen;
    while (d) buf[(*n)++] = digits[--d];
    buf[(*n)++] = ' ';
}

static void diagAddSigned(char *buf, size_t *n, size_t max, const char *name, int32_t value) {
    if (value < 0) {
        const size_t nameLen = strlen(name);
        if (*n + nameLen + 2 >= max) return;
        memcpy(buf + *n, name, nameLen);
        *n += nameLen;
        buf[(*n)++] = '-';
        diagAdd(buf, n, max, "", (uint32_t)(-value));
    } else {
        diagAdd(buf, n, max, name, (uint32_t)value);
    }
}

// How busy the channel is: the share of idle-channel RSSI samples above RADIO_BUSY_DBM. (Not a mean, a
// margin above the lowest reading, or a percentile: the distribution is bimodal, and the receiver's own
// floor reads about -109 dBm.) Our own frames are excluded: halRadioIdleRssi() refuses while the radio
// is busy, and the receiver reads high for NF_TX_RECOVER_US after we transmit.
#define NF_TX_RECOVER_US 400

// AP_DEBUG builds also keep the distribution, 16-bit counts per window (a bucket can reach 1500 at one
// sample every 10 ms), with no division per sample.

static uint32_t noiseTimer = 0;
static void sampleNoise(void) {
    if (halMillis() - noiseTimer < 10) return;
    noiseTimer = halMillis();
    // our own frame just left, or is still due to leave
    const uint32_t sinceTx = (halMicros() - halRadioTxEndUs()) & HAL_US_MASK;
    if (sinceTx < NF_TX_RECOVER_US || sinceTx > (HAL_US_MASK >> 1)) return;
    int8_t rssi;
    if (!halRadioIdleRssi(&rssi)) return;
#if AP_DEBUG
    // The low clamp comes before the shift, so nothing depends on how a negative value shifts
    int16_t above = (int16_t)rssi - (NF_HIST_BASE_DBM);
    if (above < 0) above = 0;
    uint8_t bucket = (uint8_t)(above >> NF_HIST_SHIFT);
    if (bucket >= NF_HIST_BUCKETS) bucket = NF_HIST_BUCKETS - 1;
    diag.nfHist[bucket]++;
#endif
    diag.nfSamples++;
    // Not derived from nfHist: it decides the burst size, and RADIO_BUSY_DBM is not a bucket edge
    if (rssi > RADIO_BUSY_DBM) diag.nfBusy++;
}

// Percent of samples that found something on the channel; 0 when nothing was sampled
static uint8_t channelBusyPct(void) {
    if (!diag.nfSamples) return 0;
    return (uint8_t)(((uint32_t)diag.nfBusy * 100) / diag.nfSamples);
}

// "name hhhhhhhh " into buf at *n
static void diagAddHex(char *buf, size_t *n, size_t max, const char *name, uint32_t value) {
    const size_t nameLen = strlen(name);
    if (*n + nameLen + 8 + 2 >= max) return;
    memcpy(buf + *n, name, nameLen);
    *n += nameLen;
    for (int8_t i = 7; i >= 0; i--) buf[(*n)++] = hexDigit((uint8_t)((value >> (4 * i)) & 0xF));
    buf[(*n)++] = ' ';
}

// --- crystal trim -------------------------------------------------------------------------------
// A decoded frequency offset is that tag's crystal error plus ours. AP_DEBUG builds log one line per
// check-in (not per frame, which would weight the tag being served) with the cap code in force. An
// instrument only: rf_drv_cap() already applies the factory calibration, the field may not be a plain
// 11-bit offset (see hal_tlsr.c), and the cap decides whether a scanning tag can hear us at all.
#if AP_DEBUG

// "name hh " into buf at *n: a byte in two hex digits, where diagAddHex would spend eight
static void diagAddHexByte(char *buf, size_t *n, size_t max, const char *name, uint8_t value) {
    const size_t nameLen = strlen(name);
    if (*n + nameLen + 2 + 2 >= max) return;
    memcpy(buf + *n, name, nameLen);
    *n += nameLen;
    buf[(*n)++] = hexDigit((uint8_t)(value >> 4));
    buf[(*n)++] = hexDigit((uint8_t)(value & 0xF));
    buf[(*n)++] = ' ';
}

static void logCheckinOffset(const uint8_t *mac) {
    char buf[V2_LOG_MAX];
    size_t n = 0;
    // the low four bytes, as the S3 prints them (the frame's address is little-endian)
    diagAddHex(buf, &n, sizeof(buf), "fo ",
               ((uint32_t)mac[3] << 24) | ((uint32_t)mac[2] << 16) | ((uint32_t)mac[1] << 8) | mac[0]);
    diagAdd(buf, &n, sizeof(buf), "cap ", halRadioGetXtalCap());
    diagAddSigned(buf, &n, sizeof(buf), "o ", rx.freqOffset);
    diagAddSigned(buf, &n, sizeof(buf), "rs ", rx.rssi);
    // raw, to show the bits Telink documents as zero
    diagAddHexByte(buf, &n, sizeof(buf), "hi ", rx.fdcHigh);
    buf[n] = 0;
    v2Log(LOG_INFO, buf);
}
#endif  // AP_DEBUG

// What the radio booted into, sent once from the first diagnostics window: PA registers, TX settle,
// reset reason and evidence, crystal setting. One line, about 86 characters at its widest.
static bool bootFactsLogged = false;
// The main loop's longest pass-to-pass gap and where the last pass started (see apLoop), and the
// high-water line's last value (see diagReport).
static uint32_t loopLastUs = 0, loopGapMaxUs = 0;
static bool loopStarted = false;
// ... and which part of a pass the time goes to: the longest time each part of apLoop has taken since
// boot, in us (capped at 65535):
//   0 between passes (halBoardPoll and the loop itself)  1 a radio frame  2 the S3's requests
//   3 starting a burst  4 the burst's next frame  5 events to the S3  6 diagnostics and housekeeping
#define LOOP_PARTS 7
static uint16_t loopPartMaxUs[LOOP_PARTS];
static uint32_t loopMark;
static void loopPart(uint8_t part) {
    const uint32_t now = halMicros();
    uint32_t us = (now - loopMark) & HAL_US_MASK;
    if (us > 0xFFFF) us = 0xFFFF;
    if (us > loopPartMaxUs[part]) loopPartMaxUs[part] = (uint16_t)us;
    loopMark = now;
}
// The part holding the record, as reported on the hw line (`in`)
static uint8_t loopWorstPart(void) {
    uint8_t w = 0;
    for (uint8_t i = 1; i < LOOP_PARTS; i++) {
        if (loopPartMaxUs[i] > loopPartMaxUs[w]) w = i;
    }
    return w;
}
static uint32_t hwLastSig = 0;
// ... and the stack line's, whose signature is never 0, so the first report after the boot facts sends
// it. stkTurn: the stack line was due last time and gave way to the hw line.
static uint32_t stkLastSig = 0;
static bool stkTurn = false;
// Each stack is 4/3 of the deepest path tools/stackdepth.py finds, so a mark past three quarters means
// the analysis was wrong; the stack line then goes out once as a warning.
static bool stkWarned = false;
static bool stkPast(uint16_t high, uint16_t size) { return size && (uint32_t)high * 4 > (uint32_t)size * 3; }
static void logBootFacts(void) {
    if (bootFactsLogged) return;
    bootFactsLogged = true;
    char buf[V2_LOG_MAX];
    size_t n = 0;
    diagAddHex(buf, &n, sizeof(buf), "boot pa ", halRadioTxPowerRegs());
    diagAdd(buf, &n, sizeof(buf), "settle ", halRadioTxSettleUs());
    // ssss dd tt: 0xf04 (settle, us), 0xf16 (bit 0 and bits 7..4 are the transmit-enable delay,
    // never set by anything and not in our timing model) and 0xf03. See halRadioTxDelayRegs().
    diagAddHex(buf, &n, sizeof(buf), "lld ", halRadioTxDelayRegs());
    diagAddSigned(buf, &n, sizeof(buf), "pwr ", halRadioTxPowerApplied());
    diagAdd(buf, &n, sizeof(buf), "rst ", halResetReason());
    diagAddHex(buf, &n, sizeof(buf), "rstev ", halResetReasonRaw());
    diagAddHex(buf, &n, sizeof(buf), "xtal ", halRadioXtalRegs());
    buf[n] = 0;
    v2Log(LOG_INFO, buf);
}

// Activity since the last report decides whether one is sent; otherwise one every DIAG_QUIET_MS
#define DIAG_QUIET_MS 300000UL
static uint32_t diagLastRx = 0, diagLastFails = 0, diagLastSentMs = 0;

// Short LOG lines: diagAdd() silently skips a field that does not fit a V2_LOG_MAX payload, so keep each
// line's worst plausible width under 98 characters, and add a line rather than a field when one is full.
static void diagReport(void) {
    const bool activity = diag.checkins || diag.blockReqs || diag.bursts || diag.xfc || diag.pings || diag.rereqs ||
                          diag.ackResends || diag.cancelsBusy || diag.queued || diag.cancelsNoData ||
                          halRadioRxFrames() != diagLastRx || halSerialTxDmaFails() != diagLastFails;
    if (!activity && halMillis() - diagLastSentMs < DIAG_QUIET_MS) {
        memset(&diag, 0, sizeof(diag));
        halRadioTakeReplyGapMaxUs();
        return;
    }
    diagLastSentMs = halMillis();
    diagLastRx = halRadioRxFrames();
    diagLastFails = halSerialTxDmaFails();
    // Harm (see burstCopies): other tags' frames heard per millisecond of listening, times the
    // milliseconds spent on bursts, plus requests heard too late. Adapted before the lines go out.
    const uint32_t burstTxMs = diag.burstTxUs / 1000;
    uint32_t harm = diag.late;
    if (burstTxMs && burstTxMs < DIAG_INTERVAL_MS) harm += (uint32_t)diag.othersHeard * burstTxMs / (DIAG_INTERVAL_MS - burstTxMs);
    if (harm > 0xFFFF) harm = 0xFFFF;
    burstCopiesAdapt((uint16_t)harm, diag.rereqs, channelBusyPct());
    burstDoneWaitRoll();
    airAcc.others = airAdd(airAcc.others, diag.othersHeard);
    airAcc.burstMs = airAdd(airAcc.burstMs, burstTxMs);
    airAcc.harm = airAdd(airAcc.harm, harm);
    char buf[V2_LOG_MAX];
    size_t n = 0;
    // check-ins: answered, offering data, repeated (the tag missed our reply), reply times
    diagAdd(buf, &n, sizeof(buf), "ci ", diag.checkins);
    diagAdd(buf, &n, sizeof(buf), "data ", diag.offered);
    diagAdd(buf, &n, sizeof(buf), "rep ", diag.repeats);
    diagAdd(buf, &n, sizeof(buf), "us ", diag.checkins ? diag.replyUsSum / diag.checkins : 0);
    diagAdd(buf, &n, sizeof(buf), "max ", diag.replyUsMax);
    diagAdd(buf, &n, sizeof(buf), "gap ", halRadioTakeReplyGapMaxUs());
    diagAdd(buf, &n, sizeof(buf), "late ", lateRequestsSkipped);
    // tags that went quiet after their burst (burstAnswerSeen), by its timing
    diagAdd(buf, &n, sizeof(buf), "silz ", diag.silentZero);
    diagAdd(buf, &n, sizeof(buf), "silw ", diag.silentWaited);
    buf[n] = 0;
    v2Log(LOG_INFO, buf);
    // bursts on air: what the tags asked for and what this radio put out
    n = 0;
    diagAdd(buf, &n, sizeof(buf), "xfer brq ", diag.blockReqs);
    diagAdd(buf, &n, sizeof(buf), "bst ", diag.bursts);
    diagAdd(buf, &n, sizeof(buf), "zbst ", diag.zeroBursts);
    diagAdd(buf, &n, sizeof(buf), "xfc ", diag.xfc);
    diagAdd(buf, &n, sizeof(buf), "rereq ", diag.rereqs);
    diagAdd(buf, &n, sizeof(buf), "rparts ", diag.rereqParts);
    diagAdd(buf, &n, sizeof(buf), "reack ", diag.ackResends);
    diagAdd(buf, &n, sizeof(buf), "cut ", diag.burstsCut);
    diagAdd(buf, &n, sizeof(buf), "cpy ", burstCopies);
    diagAdd(buf, &n, sizeof(buf), "png ", diag.pings);
    buf[n] = 0;
    v2Log(LOG_INFO, buf);
    // where the block data comes from: cancels by reason, the cache, and the S3's answer times. cevu
    // counts evicted blocks never served to a tag; rising with dlate or dl, it says two transfers are
    // evicting each other's blocks. `cb`/`cu`: blocks and KB the cache holds now (cf. `ckb` on cfg).
    n = 0;
    diagAdd(buf, &n, sizeof(buf), "blk cxlb ", diag.cancelsBusy);
    diagAdd(buf, &n, sizeof(buf), "bq ", diag.queued);
    diagAdd(buf, &n, sizeof(buf), "cxln ", diag.cancelsNoData);
    diagAdd(buf, &n, sizeof(buf), "cev ", cacheEvictions());
    diagAdd(buf, &n, sizeof(buf), "cevu ", cacheEvictionsUnserved());
    // 97 of 98 characters at every field's widest: no room for more fields here
    diagAdd(buf, &n, sizeof(buf), "cb ", cacheBlocks());
    diagAdd(buf, &n, sizeof(buf), "cu ", (cacheHeldBytes() + 1023) / 1024);
    diagAdd(buf, &n, sizeof(buf), "dl ", diag.dataCount ? diag.dataMsSum / diag.dataCount : 0);
    diagAdd(buf, &n, sizeof(buf), "dlx ", diag.dataMsMax);
    diagAdd(buf, &n, sizeof(buf), "dlate ", diag.dataLate);
    diagAdd(buf, &n, sizeof(buf), "dw ", dataWaitMs);
    buf[n] = 0;
    v2Log(LOG_INFO, buf);
    // radio and serial line
    n = 0;
    diagAdd(buf, &n, sizeof(buf), "rf rx ", halRadioRxFrames());
    // frames that failed their CRC, since boot like rx: how many of what the radio hears is corrupt
    diagAdd(buf, &n, sizeof(buf), "crc ", halRadioRxCrcFails());
    diagAdd(buf, &n, sizeof(buf), "tx ", halRadioTxFrames());
    diagAdd(buf, &n, sizeof(buf), "txto ", halRadioTxTimeouts());
    diagAdd(buf, &n, sizeof(buf), "drop ", halRadioRxDropped());
    diagAdd(buf, &n, sizeof(buf), "bdrop ", halRadioRxBusyDrops());
    diagAdd(buf, &n, sizeof(buf), "unh ", diag.unhandled);
    diagAdd(buf, &n, sizeof(buf), "tsfb ", halRadioTsFallbacks());
    diagAdd(buf, &n, sizeof(buf), "busy ", channelBusyPct());
#if AP_DEBUG
    // The idle-channel distribution as eight hex digits, lowest bucket first (-110..-103 dBm), in
    // sixteenths of the window's samples; a bucket with any sample reads at least 1.
    if (diag.nfSamples) {
        if (n + 4 + NF_HIST_BUCKETS < sizeof(buf)) {
            memcpy(buf + n, "nf ", 3);
            n += 3;
            for (uint8_t i = 0; i < NF_HIST_BUCKETS; i++) {
                uint32_t s16 = ((uint32_t)diag.nfHist[i] * 16) / diag.nfSamples;
                if (!s16 && diag.nfHist[i]) s16 = 1;
                if (s16 > 15) s16 = 15;
                buf[n++] = hexDigit((uint8_t)s16);
            }
            buf[n++] = ' ';
        }
    }
#endif
    // UART TX by DMA: transfers, and failures (then byte by byte for good) with the registers seen
    diagAdd(buf, &n, sizeof(buf), "udma ", halSerialTxDmaFrames());
    diagAdd(buf, &n, sizeof(buf), "udmaf ", halSerialTxDmaFails());
    if (halSerialTxDmaFails()) diagAddHex(buf, &n, sizeof(buf), "udmar ", halSerialTxDmaFailRegs());
    buf[n] = 0;
    v2Log(LOG_INFO, buf);

    // At most one more line per report, so the LOG queue never overflows (V2_OUTQ_BYTES in v2link.c):
    // the boot facts once, then the hw or stack line when one of their marks has risen (taking turns),
    // else the air line after bursts (its counters keep until it goes).
    //
    // hw: high-water marks since boot of everything whose size is a choice:
    //   urx   bytes waiting in the UART receive ring when the main loop drained it (URX_RING)
    //   lp    longest time from one main-loop pass to the next since the link came up, us
    //   in    the part of a pass that has taken longest (see loopPart), and that time in us
    //   rq    the slowest S3 request since boot, received to answered: its type and the time in us
    //   rxq   frames waiting in the radio's receive slots (RX_SLOTS)
    //   evq   events queued for the S3, and `obq` bytes of the ring they share with LOG lines
    //         (V2_OUTQ_BYTES in v2link.c)
    //   logq  LOG lines queued in that ring, and `lgd` lines it had no room for
    //   pend  the most pending entries there have been (of pendMax, on the cfg line)
    //
    // stk: `stk <main> irq <interrupt>`, bytes in use at the deepest point since boot (halStackHigh
    // scans the fill), and only when not 0: `evd`, events the ring had no room for, and `larm`, replies
    // that could not keep their schedule. The hw line is already at its 98-character limit.
    {
        const uint32_t sig = halSerialRxHigh() + loopGapMaxUs + halRadioRxHigh() + v2EventsHigh() + v2OutHigh() +
                             v2LogHigh() + v2LogsDropped() + pendPeak + loopPartMaxUs[loopWorstPart()] + v2ReqMaxUs();
        const uint16_t stk = halStackHigh(), irq = halIrqStackHigh();
        // All four only rise, so the sum moves when any does; + 1 so it is never the 0 it starts at
        const uint32_t stkSig = (uint32_t)stk + irq + v2EventsDropped + halRadioTxLateArms() + 1;
        const bool hwDue = bootFactsLogged && sig != hwLastSig;
        const bool stkDue = bootFactsLogged && stkSig != stkLastSig;
        const bool otherDue = !bootFactsLogged || hwDue || stkDue;
        const bool airDue = airAcc.burstMs || airAcc.yields || airAcc.yieldMs;
        if (airDue && !otherDue) {
            // since the last air line: parts held while the channel was in use (yld), holds that ended
            // in a frame (ylf), ms of the others (yms); fh/fe since boot (see flagSample); other tags'
            // frames (oth), burst ms (btx), estimated harm (hrm); burstDoneWaitMs (dn, dx). 98 characters
            // at most.
            n = 0;
            diagAdd(buf, &n, sizeof(buf), "air yld ", airAcc.yields);
            diagAdd(buf, &n, sizeof(buf), "ylf ", airAcc.yieldFrames);
            diagAdd(buf, &n, sizeof(buf), "yms ", airAcc.yieldMs);
            diagAdd(buf, &n, sizeof(buf), "fh ", flagHigh);
            diagAdd(buf, &n, sizeof(buf), "fe ", flagFrameUs / 1000);
            diagAdd(buf, &n, sizeof(buf), "oth ", airAcc.others);
            diagAdd(buf, &n, sizeof(buf), "btx ", airAcc.burstMs);
            diagAdd(buf, &n, sizeof(buf), "hrm ", airAcc.harm);
            diagAdd(buf, &n, sizeof(buf), "dn ", burstDoneWaitMs[0]);
            diagAdd(buf, &n, sizeof(buf), "dx ", burstDoneWaitMs[1]);
            buf[n] = 0;
            v2Log(LOG_INFO, buf);
            memset(&airAcc, 0, sizeof(airAcc));
        } else if (hwDue && !(stkDue && stkTurn)) {
            hwLastSig = sig;
            stkTurn = stkDue;
            n = 0;
            diagAdd(buf, &n, sizeof(buf), "hw urx ", halSerialRxHigh());
            diagAdd(buf, &n, sizeof(buf), "lp ", loopGapMaxUs);
            diagAdd(buf, &n, sizeof(buf), "in ", loopWorstPart());   // which part (see loopPart)
            diagAdd(buf, &n, sizeof(buf), "", loopPartMaxUs[loopWorstPart()]);
            diagAdd(buf, &n, sizeof(buf), "rq ", v2ReqMaxType());   // the slowest S3 request
            diagAdd(buf, &n, sizeof(buf), "", v2ReqMaxUs());
            diagAdd(buf, &n, sizeof(buf), "rxq ", halRadioRxHigh());
            diagAdd(buf, &n, sizeof(buf), "evq ", v2EventsHigh());
            diagAdd(buf, &n, sizeof(buf), "obq ", v2OutHigh());
            diagAdd(buf, &n, sizeof(buf), "logq ", v2LogHigh());
            diagAdd(buf, &n, sizeof(buf), "lgd ", v2LogsDropped());
            diagAdd(buf, &n, sizeof(buf), "pend ", pendPeak);
            buf[n] = 0;
            v2Log(LOG_INFO, buf);
        } else if (stkDue) {
            stkLastSig = stkSig;
            stkTurn = false;
            n = 0;
            diagAdd(buf, &n, sizeof(buf), "stk ", stk);
            diagAdd(buf, &n, sizeof(buf), "irq ", irq);
            // the two counts that should stay at 0, when they have not (see above)
            if (v2EventsDropped) diagAdd(buf, &n, sizeof(buf), "evd ", v2EventsDropped);
            if (halRadioTxLateArms()) diagAdd(buf, &n, sizeof(buf), "larm ", halRadioTxLateArms());
            const uint16_t stkSize = halStackSize(), irqSize = halIrqStackSize();
            const bool warn = !stkWarned && (stkPast(stk, stkSize) || stkPast(irq, irqSize));
            if (warn) {
                // the same line in the same slot, with the sizes and why
                static const char why[] = "past 3/4: deeper than the static bound";
                stkWarned = true;
                diagAdd(buf, &n, sizeof(buf), "of ", stkSize);
                diagAdd(buf, &n, sizeof(buf), "", irqSize);
                // fits at the stacks' widest; checked anyway, for evd and larm beside them
                if (n + sizeof(why) <= sizeof(buf)) {
                    memcpy(buf + n, why, sizeof(why) - 1);
                    n += sizeof(why) - 1;
                }
            }
            buf[n] = 0;
            v2Log(warn ? LOG_WARN : LOG_INFO, buf);
        } else {
            logBootFacts();  // last, so the counters keep the first slots of the LOG queue
        }
    }

    memset(&diag, 0, sizeof(diag));
}

static void housekeeping(void) {
    if (lateRequestsSkipped) {
        logNum(LOG_WARN, "requests skipped in the last minute, too late to answer: ", lateRequestsSkipped);
        lateRequestsSkipped = 0;
    }
    if (lockedIgnored) {
        logNum(LOG_WARN, "frames ignored in the last minute, tag not on the allow-list: ", lockedIgnored);
        lockedIgnored = 0;
    }
    memset(lastTagReturn, 0, 8);
    memset(lastTagReturnMac, 0, 8);
    // one round of the store's clock: a write only where something lapses
    hkNow++;
    for (uint32_t c = 0; c < recN;) {
        struct tagRec *r = &recs[c];
        if (r->flags & REC_PENDING) {
            if (r->expires == hkNow) {
                if ((r->flags & REC_OFFER) && offers[r->offer].dataType != DATATYPE_NOUPDATE) {
                    uint8_t mac[8];
                    recMac(r, mac);
                    notifyXferEnd(V2_XFER_TIMEOUT, mac, offers[r->offer].dataVer);
                }
                dropUnusedData(pendEnd((uint16_t)c));  // owning nothing now; unless listed, later rounds age it out
            } else if ((r->flags & REC_COUNTING) && r->sleepUntil == hkNow) {
                r->flags &= (uint8_t)~REC_COUNTING;  // "check in again in 0 minutes" from here on
                r->sleepUntil = 0;
            }
        } else if (!(r->flags & REC_LISTED)) {
            // Owning nothing: kept one round, for a SET_PENDING that is on its way, then given up.
            if (r->flags & REC_STALE) {
                recDelete((uint16_t)c);  // the last record is in c now, and is looked at next
                continue;
            }
            r->flags |= REC_STALE;
        }
        c++;
    }
    storeTrim();  // and the room that frees goes back to the cache
}

void apInit(void) {
    memset(&requestedData, 0, sizeof(requestedData));
    requestedData.blockId = 0xFF;
    storeInit();
    memset(lastAckMac, 0, sizeof(lastAckMac));
    memset(lastTagReturn, 0, sizeof(lastTagReturn));
    memset(lastTagReturnMac, 0, sizeof(lastTagReturnMac));
    // cacheInit gives the whole arena to the cache, so the tag store that shares it has to be empty
    // here (storeInit above) or the two would disagree about where the boundary is.
    lockMode = V2_LOCK_OFF;
    cacheInit();
    cacheListReserve(0);

    halRadioInit(curChannel);
    halRadioSetTxPower(curPower);
    halRadioGetMac(mSelfMac);

    pr("RES>");
    pr("RDY>");
    // What the radio booted into is reported from the first diagnostics window, not from here: the
    // serial line is still the ASCII protocol, and v2Log() would drop the line.
    housekeepingTimer = halMillis();
}

uint32_t apLoop(void) {
    // From one pass to the next is what the UART receive ring has to cover: main() calls this in a
    // loop and never waits, so the gap is this pass, the board's poll, and whatever ran in between.
    const uint32_t passUs = halMicros();
    if (loopStarted) {
        const uint32_t gap = (passUs - loopLastUs) & HAL_US_MASK;
        if (gap > loopGapMaxUs) loopGapMaxUs = gap;
        loopPart(0);  // from the end of the last pass's work to here
    }
    loopLastUs = passUs;
    loopMark = passUs;
    loopStarted = true;
    halWatchdogFeed();
    const bool gotFrame = halRadioRx(&rx);
    if (gotFrame) {
        flagFrame(rx.len);
        handleRadioFrame();
        // At once, not at the next halRadioRx(): the rest of the pass can be several milliseconds of
        // serial work, and a slot held through it is one fewer for the frames arriving meanwhile.
        halRadioRxDone();
    }
    loopPart(1);

    uint8_t serialBuf[64];
    size_t serialLen;
    if (v2Active()) {
        // One frame of the S3's per pass, then back to the radio: a preload stream never empties the
        // ring. The S3 has at most V2_CHUNK_WINDOW chunks in flight, and the ring is sized for that.
        while ((serialLen = halSerialReadFrame(serialBuf, sizeof(serialBuf))) > 0) {
            v2RxBytes(serialBuf, serialLen);
            if (serialBuf[serialLen - 1] == 0x00) break;  // a delimiter: that was one frame
        }
    } else {
        while ((serialLen = halSerialRead(serialBuf, sizeof(serialBuf))) > 0) {
            for (size_t i = 0; i < serialLen; i++) {
                if (processAscii(serialBuf[i])) break;
            }
        }
    }
    if (v2Active() && (v2MsSinceLastFrame() > V2_LINK_LOST_MS ||
                       (v2MsSinceLastFrame() > V2_LINK_GARBLED_MS && v2BytesSinceFrame() > V2_LINK_GARBLED_BYTES))) {
        v2Stop();
        halSerialSetBaud(ASCII_BAUD);
    }
    loopPart(2);

    if (blockStartTimer) {
        const int32_t late = (int32_t)(halMillis() - blockStartTimer);
        if (late >= 0) {
            const int slot = cacheFind(requestedData.ver, requestedData.blockId);
            if (cacheComplete(slot)) {
                startBurst(slot, late);
                blockStartTimer = 0;
            } else if (late > BLOCK_DATA_SLACK_MS) {
                diag.dataLate++;
                dataWaitMissed();
                logNum(LOG_WARN, "block data not complete in time, no burst for block ", requestedData.blockId);
                blockStartTimer = 0;
                promoteQueued();
            }
        }
    }
    loopPart(3);
    burstStep();
    loopPart(4);
    v2Poll();
    loopPart(5);

    sampleNoise();
    flagSample();
    flagCrcFrames();
    if (halMillis() - diagTimer >= DIAG_INTERVAL_MS) {
        diagTimer = halMillis();
        diagReport();
    }
    if (halMillis() - housekeepingTimer >= HOUSEKEEPING_INTERVAL_MS) {
        housekeeping();
        housekeepingTimer = halMillis();
    }
    loopPart(6);
    if (loopTimingRestart) {
        loopTimingRestart = false;
        loopGapMaxUs = 0;
        memset(loopPartMaxUs, 0, sizeof(loopPartMaxUs));
    }

    if (gotFrame || burstActive) return 0;
    // Nothing from the radio this pass: the caller may wait for a frame, serial data, or a timer
    uint32_t waitMs = 100;
    if (blockStartTimer) {
        int32_t left = (int32_t)(blockStartTimer - halMillis()) + 1;
        if (left <= 0) left = (int32_t)(blockStartTimer + BLOCK_DATA_SLACK_MS - halMillis()) + 1;  // waiting for data
        waitMs = left > 0 ? (uint32_t)left : 0;
        if (waitMs > 100) waitMs = 100;
    }
    const uint32_t v2Ms = v2MsUntilWork();
    if (v2Ms < waitMs) waitMs = v2Ms;
    return waitMs;
}
