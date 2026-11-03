// hal.h on a PC, for the tests in test_ap.py: a virtual clock, the serial line and the radio as
// buffers the test fills and empties through the sim* functions (ctypes).

#include <string.h>

#if defined(__SANITIZE_ADDRESS__)
#include <sanitizer/asan_interface.h>
#else
#define ASAN_POISON_MEMORY_REGION(a, n) ((void)(a), (void)(n))
#define ASAN_UNPOISON_MEMORY_REGION(a, n) ((void)(a), (void)(n))
#endif

#include "../src/hal.h"

static uint32_t nowMs = 0;
static uint32_t baud = 115200;

#define SER_MAX 65536
static uint8_t serIn[SER_MAX], serOut[SER_MAX];
static size_t serInHead = 0, serInTail = 0, serOutLen = 0;

// The receive side as the TLSR has it (tlsr/hal_tlsr.c, "who owns which slot"): RX_SLOTS slots, the
// armed one at rxHead, the frame out with the AP at rxTail, a frame's bytes at slot + 5 - so at
// 1 mod 4, where UBSan's alignment check sees any parser that assumes more. A frame arriving when the
// slot after the armed one is the held one is dropped, as the interrupt does.
//
// A slot the AP does not own is poisoned for AddressSanitizer: an empty or released one, and the
// held one from halRadioRxDone() on. So a read of a frame's bytes after its release - the bug reading
// in place makes possible - is an ASan report at the line that does it, not a wrong value that a test
// may or may not notice. A slot becomes readable again only when a frame is "DMA'd" into it.
#define RX_SLOTS 4
#define RX_SLOT_SIZE 144
static uint8_t rxSlots[RX_SLOTS][RX_SLOT_SIZE] __attribute__((aligned(8)));
static uint32_t rxSfdUs[RX_SLOTS];  // what the hardware has in the slot's trailer
static int8_t rxRssi[RX_SLOTS];
static int rxHead = 0, rxTail = 0;
static bool rxHeld = false;
static uint32_t rxDrops = 0;
static uint8_t rxHigh = 0;
// Frames that arrive while the AP is in the middle of answering one: delivered from inside the next
// halRadioTxReply(), so the frame the reply answers is still held when they land.
#define RX_LATER 8
static uint8_t rxLater[RX_LATER][HAL_RADIO_MAX_FRAME];
static uint8_t rxLaterLen[RX_LATER];
static int rxLaterCount = 0;

// The transmit side: one buffer, which the AP builds its frame in, as on the TLSR. What the radio
// sends is recorded at the send; it is compared with the buffer again at the next halRadioTxFrame()
// (and whenever the test looks), so a write into a frame on its way out - which on the TLSR would go
// out on the air in the middle of that frame - is counted, and so is a send of a buffer that was not
// taken for it. On the TLSR the buffer still holds the previous frame when it is handed out; here it
// is filled with 0xA5, so a frame that relies on anything it did not write itself is caught too.
static uint8_t txBuf[1 + HAL_RADIO_MAX_FRAME];
static uint8_t txShadow[sizeof(txBuf)];
static bool txOnAir = false, txTaken = false;
static uint32_t txCorruptions = 0;
static uint8_t txLog[256][130];  // each: [0] = length of the MAC frame, then the frame
static int txCount = 0;
static uint32_t txBusyUntil = 0;
// End of the frame on air, recorded when it was sent rather than derived from the current time -
// otherwise it always reads as "just now" and anything waiting for the air to clear waits for ever
static uint32_t txEndUs = 0;
static int txAirtime = 0;  // simulate the airtime of each frame (ms) when set
static uint8_t channel = 0;
// -128 until a power has been asked for, as on the TLSR, where that means the PA registers still
// hold whatever the chip reset left in them. The simulated radio applies a power at once; the real
// one waits for the air to clear first (see halRadioSetTxPower in hal.h).
static int8_t powerDbm = -128;
static uint32_t rxFramesCount = 0, txFramesCount = 0;
static uint8_t ledBrightness = 255;

// --- driven by the test --------------------------------------------------------------------------
void simSetTime(uint32_t ms) { nowMs = ms; }
uint32_t simTime(void) { return nowMs; }
uint32_t simBaud(void) { return baud; }
uint8_t simChannel(void) { return channel; }
int8_t simPower(void) { return powerDbm; }
uint8_t simLed(void) { return ledBrightness; }
void simSetAirtime(int ms) { txAirtime = ms; }

void simSerialIn(const uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        serIn[serInHead] = data[i];
        serInHead = (serInHead + 1) % SER_MAX;
    }
}
size_t simSerialOut(uint8_t *buf, size_t max) {
    size_t n = serOutLen < max ? serOutLen : max;
    memcpy(buf, serOut, n);
    memmove(serOut, serOut + n, serOutLen - n);
    serOutLen -= n;
    return n;
}
// A frame received, ageUs after its start: into the armed slot, and kept if the ring has room. 1 if it
// was kept, 0 if it was dropped (halRadioRxDropped).
int simRadioIn(const uint8_t *frame, uint8_t len, int8_t rssi, uint32_t ageUs) {
    if (len > HAL_RADIO_MAX_FRAME) return 0;
    uint8_t *p = rxSlots[rxHead];
    ASAN_UNPOISON_MEMORY_REGION(p, RX_SLOT_SIZE);
    memset(p, 0, RX_SLOT_SIZE);
    p[4] = (uint8_t)(len + 2);  // the PHR, with the FCS
    memcpy(p + 5, frame, len);
    rxSfdUs[rxHead] = (nowMs * 1000 - ageUs) & HAL_US_MASK;
    rxRssi[rxHead] = rssi;
    const int next = (rxHead + 1) % RX_SLOTS;
    if (next == rxTail) {
        rxDrops++;  // as radioRxIrq(): moving on would arm the slot the AP may be reading
        ASAN_POISON_MEMORY_REGION(p, RX_SLOT_SIZE);
        return 0;
    }
    rxHead = next;
    return 1;
}
void simRadioInDuringReply(const uint8_t *frame, uint8_t len) {
    if (rxLaterCount == RX_LATER || len > HAL_RADIO_MAX_FRAME) return;
    memcpy(rxLater[rxLaterCount], frame, len);
    rxLaterLen[rxLaterCount++] = len;
}
uint32_t simRadioDrops(void) { return rxDrops; }
static void txCheck(void) {
    if (txOnAir && memcmp(txBuf, txShadow, (size_t)txShadow[0] - 1) != 0) {
        txCorruptions++;
        txOnAir = false;  // counted once
    }
}
uint32_t simTxCorruptions(void) {
    txCheck();
    return txCorruptions;
}
int simRadioOut(uint8_t *buf) {  // the oldest frame sent, as [len, frame...]; 0 if none
    txCheck();
    if (txCount == 0) return 0;
    memcpy(buf, txLog[0], txLog[0][0] + 1);
    memmove(&txLog[0], &txLog[1], (size_t)(txCount - 1) * sizeof(txLog[0]));
    txCount--;
    return 1;
}

// --- hal.h ---------------------------------------------------------------------------------------
uint32_t halMillis(void) { return nowMs; }
uint32_t halMicros(void) { return (nowMs * 1000) & HAL_US_MASK; }
void halDelayMs(uint32_t ms) { nowMs += ms; }
void halWatchdogFeed(void) {}
uint8_t halResetReason(void) { return 0; }
uint32_t halResetReasonRaw(void) { return 0; }
// A host stack says nothing about the radio's, so these read 0 unless a test sets what a radio would
// report (simSetStacks), which is how the warning past three quarters of a stack is driven.
static uint16_t stkHigh, irqHigh, stkSize, irqSize;
void simSetStacks(uint16_t high, uint16_t irq, uint16_t size, uint16_t irqSz) {
    stkHigh = high;
    irqHigh = irq;
    stkSize = size;
    irqSize = irqSz;
}
uint16_t halStackHigh(void) { return stkHigh; }
uint16_t halIrqStackHigh(void) { return irqHigh; }
uint16_t halStackSize(void) { return stkSize; }
uint16_t halIrqStackSize(void) { return irqSize; }

// The cache arena, which on the TLSR is whatever the link leaves (boot_8258_ap.link). Here a test
// build chooses it (HOST_ARENA_BYTES, test/Makefile); the default is what the firmware's own default
// build had when this was written (_cache_arena_size_ of the release build, 54984 since blocks are
// stored at their own length), so a build that sets nothing has the firmware's geometry - the same
// blocks and the same derived maxima - and does not follow it by itself.
// Filled with 0xFF before its first use, as cstartup's stack fill leaves it on the TLSR, so that
// nothing comes to depend on the zeroes a static array would have started with.
#ifndef HOST_ARENA_BYTES
#define HOST_ARENA_BYTES 54984
#endif
static uint8_t hostArena[HOST_ARENA_BYTES] __attribute__((aligned(8)));
uint8_t *halArena(uint32_t *bytes) {
    static bool filled = false;
    if (!filled) {
        memset(hostArena, 0xFF, sizeof(hostArena));
        filled = true;
    }
    *bytes = sizeof(hostArena);
    return hostArena;
}

static size_t serialRead(uint8_t *buf, size_t max, bool toDelimiter) {
    size_t n = 0;
    while (serInTail != serInHead && n < max) {
        const uint8_t b = serIn[serInTail];
        buf[n++] = b;
        serInTail = (serInTail + 1) % SER_MAX;
        if (toDelimiter && b == 0x00) break;
    }
    return n;
}
size_t halSerialRead(uint8_t *buf, size_t max) { return serialRead(buf, max, false); }
size_t halSerialReadFrame(uint8_t *buf, size_t max) { return serialRead(buf, max, true); }
void halSerialWrite(const uint8_t *buf, size_t len) {
    if (serOutLen + len > SER_MAX) return;
    memcpy(serOut + serOutLen, buf, len);
    serOutLen += len;
}
// The frame buffer the firmware encodes into, as on the TLSR one frame of exactly HAL_SERIAL_TX_MAX:
// a global, so AddressSanitizer reports a byte written past its end. Refilled with a non-zero pattern
// each time it is handed out, so an encoder that leaves a byte unwritten - a code byte it meant to
// come back to - sends that pattern and fails the test's COBS decode or CRC, rather than happening to
// read the zero a previous frame left there.
static uint8_t txFrame[HAL_SERIAL_TX_MAX];
static uint32_t txFrames = 0;
uint32_t simSerialTxFrames(void) { return txFrames; }  // frames sent through halSerialTxCommit()
uint8_t *halSerialTxBuf(void) {
    memset(txFrame, 0xA5, sizeof(txFrame));
    return txFrame;
}
void halSerialTxCommit(size_t len) {
    if (len > sizeof(txFrame)) len = sizeof(txFrame);
    txFrames++;
    halSerialWrite(txFrame, len);
}
void halSerialSetBaud(uint32_t b) {
    baud = b;
    serInTail = serInHead;  // input at the old speed is lost
}
uint16_t halSerialOverflows(void) { return 0; }
uint16_t halSerialRxHigh(void) { return 0; }
uint32_t halSerialTxDmaFrames(void) { return 0; }
uint32_t halSerialTxDmaFails(void) { return 0; }
uint32_t halSerialTxDmaFailRegs(void) { return 0; }

void halRadioInit(uint8_t ch) {
    channel = ch;
    rxHead = rxTail = 0;
    rxHeld = false;
    ASAN_POISON_MEMORY_REGION(rxSlots, sizeof(rxSlots));  // no frame in any slot yet
}
void halRadioRxDone(void) {
    if (!rxHeld) return;
    rxHeld = false;
    ASAN_POISON_MEMORY_REGION(rxSlots[rxTail], RX_SLOT_SIZE);  // the AP's last access to it was before here
    rxTail = (rxTail + 1) % RX_SLOTS;
}
bool halRadioRx(struct halRxFrame *frame) {
    halRadioRxDone();
    const uint8_t waiting = (uint8_t)((rxHead + RX_SLOTS - rxTail) % RX_SLOTS);
    if (waiting > rxHigh) rxHigh = waiting;
    if (rxTail == rxHead) return false;
    uint8_t *p = rxSlots[rxTail];
    frame->len = (uint8_t)(p[4] - 2);
    frame->data = p + 5;
    frame->rssi = rxRssi[rxTail];
    frame->lqi = 100;
    frame->freqOffset = 0;
    frame->fdcHigh = 0x40;  // rx_packet_chn_efuse 4, pkt_fdc[10:8] zero, as the hardware reports it
    frame->sfdUs = rxSfdUs[rxTail];
    frame->ageUs = (nowMs * 1000 - rxSfdUs[rxTail]) & HAL_US_MASK;  // a frame ages while it waits
    rxHeld = true;
    rxFramesCount++;
    return true;
}
uint8_t *halRadioTxFrame(void) {
    txCheck();  // nothing wrote into the frame that went out last
    if (txAirtime && (int32_t)(txBusyUntil - nowMs) > 0) nowMs = txBusyUntil;  // waits for it to leave
    txOnAir = false;
    txTaken = true;
    memset(txBuf, 0xA5, sizeof(txBuf));
    return txBuf;
}
static void txSend(void) {
    // a buffer not taken for this frame, or a frame sent while the last one is still on the air
    if (!txTaken || (txAirtime && (int32_t)(txBusyUntil - nowMs) > 0)) txCorruptions++;
    txTaken = false;
    const uint8_t len = (uint8_t)(txBuf[0] - 2);
    if (txCount < 256) {
        txLog[txCount][0] = len;
        memcpy(&txLog[txCount][1], txBuf + 1, len);
        txCount++;
    }
    memcpy(txShadow, txBuf, (size_t)len + 1);
    txOnAir = true;
    txFramesCount++;
    if (txAirtime) txBusyUntil = nowMs + txAirtime;
    txEndUs = (nowMs * 1000 + (uint32_t)txAirtime * 1000) & HAL_US_MASK;
}
// A frame the receiver has synchronised on and not finished: what the TLSR reads from 0x448. The
// test sets it to stand for a tag transmitting into a burst's gap.
static int rxArriving = 0;
void simSetRxArriving(int on) { rxArriving = on; }
bool halRadioRxArriving(void) { return rxArriving != 0; }
bool halRadioTx(bool yieldToRx) {
    const int queued = (rxHead + RX_SLOTS - rxTail) % RX_SLOTS - (rxHeld ? 1 : 0);
    if (yieldToRx && (rxArriving || queued > 0)) return false;
    txSend();
    return true;
}
void halRadioTxReply(uint32_t delayUs) {
    (void)delayUs;  // the simulated radio sends at once; the delay is hardware timing
    // Frames that completed while the AP was working out this reply, before the radio left RX for
    // it: they land in the ring around the request it answers, which the AP is still holding - the
    // moment at which, on the TLSR, the request's slot must not be armed.
    const int n = rxLaterCount;
    rxLaterCount = 0;
    for (int i = 0; i < n; i++) simRadioIn(rxLater[i], rxLaterLen[i], -60, 1000);
    txSend();
}
uint32_t halRadioTxEndUs(void) { return txEndUs; }
uint32_t halRadioTakeReplyGapMaxUs(void) { return 0; }
// What the receiver reads on an idle channel. Settable so a test can feed a known distribution
// through sampleNoise() and check the histogram against it: with one fixed value there is only ever
// one bucket, which exercises neither the bucket arithmetic nor the rule that a bucket holding any
// sample at all must not read zero.
static int8_t idleRssi = -95;
void simSetIdleRssi(int dbm) { idleRssi = (int8_t)dbm; }
bool halRadioIdleRssi(int8_t *dbm) {
    *dbm = idleRssi;
    return true;
}
int8_t halRadioTxPowerApplied(void) { return powerDbm; }
uint32_t halRadioTsFallbacks(void) { return 0; }
bool halRadioTxBusy(void) { return txAirtime && (int32_t)(txBusyUntil - nowMs) > 0; }
void halRadioSetChannel(uint8_t ch) { channel = ch; }
void halRadioSetTxPower(int8_t dbm) { powerDbm = dbm; }
static uint8_t xtalCap = 24;
void halRadioSetXtalCap(uint8_t cap) { xtalCap = cap; }
uint8_t halRadioGetXtalCap(void) { return xtalCap; }
uint32_t halRadioXtalRegs(void) { return 0xFF00u | xtalCap; }
void halRadioGetMac(uint8_t mac[8]) {
    static const uint8_t m[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    memcpy(mac, m, 8);
}
uint32_t halRadioRxFrames(void) { return rxFramesCount; }
uint32_t halRadioTxFrames(void) { return txFramesCount; }
uint32_t halRadioRxDropped(void) { return rxDrops; }
uint8_t halRadioRxHigh(void) { return rxHigh; }
uint32_t halRadioRxBusyDrops(void) { return 0; }
// A frame that failed its CRC: counted, never handed to the AP, as the TLSR's receive interrupt does
static uint32_t crcFails = 0, crcFailBytes = 0;
void simRadioInBadCrc(uint8_t len) {
    crcFails++;
    crcFailBytes += (uint32_t)len + 2;  // the length byte counts the FCS
}
uint32_t halRadioRxCrcFails(void) { return crcFails; }
uint32_t halRadioRxCrcFailBytes(void) { return crcFailBytes; }
uint32_t halRadioTxPowerRegs(void) { return 0; }
uint16_t halRadioTxSettleUs(void) { return 0; }
uint32_t halRadioTxDelayRegs(void) { return 0; }
uint32_t halRadioTxTimeouts(void) { return 0; }
uint32_t halRadioTxLateArms(void) { return 0; }
void halLedSetBrightness(uint8_t b) { ledBrightness = b; }
void halLedFlash(void) {}
