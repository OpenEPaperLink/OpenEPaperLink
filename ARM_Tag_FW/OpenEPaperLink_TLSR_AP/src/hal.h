#pragma once

// Hardware interface of the AP logic (ap.c, v2link.c, blockcache.c). Implemented for the TLSR825x in
// tlsr/hal_tlsr.c and for the host tests in ../test/host_hal.c.

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// --- time ---------------------------------------------------------------------------------------
uint32_t halMillis(void);  // since boot, wraps after 49 days
// Microseconds, time base of halRxFrame.sfdUs; differences are modulo 2^28 (TLSR timer: 268 s)
uint32_t halMicros(void);
#define HAL_US_MASK 0x0FFFFFFFUL
void halDelayMs(uint32_t ms);
void halWatchdogFeed(void);
// As esp_reset_reason_t, like the C6 radio: 1 power-on, 2 external reset, 7 watchdog, 0 unknown
uint8_t halResetReason(void);
uint32_t halResetReasonRaw(void);  // the evidence it was decided from, for diagnostics

// --- stacks -------------------------------------------------------------------------------------
// Deepest use since boot and reserved size (tools/stackdepth.py) of the main and interrupt stacks, in
// bytes; the high-water functions scan memory, so for diagnostics only. 0 where not measured (host).
uint16_t halStackHigh(void);
uint16_t halIrqStackHigh(void);
uint16_t halStackSize(void);
uint16_t halIrqStackSize(void);

// --- the arena ----------------------------------------------------------------------------------
// SRAM for blockcache.c: base 8-aligned, size a multiple of 8, on the TLSR whatever RAM the build
// leaves (boot_8258_ap.link). Content at boot is undefined (0xFFFFFFFF on the TLSR and the host).
uint8_t *halArena(uint32_t *bytes);

// --- serial line to the ESP32-S3 ----------------------------------------------------------------
// Bytes arrive in an interrupt and wait in a ring buffer; halSerialRead() takes what is there.
size_t halSerialRead(uint8_t *buf, size_t max);
// ... but stops after the first 0x00, which ends every protocol v2 frame: one frame per main loop pass
size_t halSerialReadFrame(uint8_t *buf, size_t max);
void halSerialWrite(const uint8_t *buf, size_t len);  // blocks until the bytes are in the UART
// A frame written in place: up to HAL_SERIAL_TX_MAX bytes at halSerialTxBuf(), sent by
// halSerialTxCommit(n). No other serial writes in between; the buffer is not the caller's after.
#define HAL_SERIAL_TX_MAX 264  // one v2 frame on the wire: V2_MAX_FRAME_COBS + its delimiter
uint8_t *halSerialTxBuf(void);
void halSerialTxCommit(size_t len);
void halSerialSetBaud(uint32_t baud);                 // after the last byte has left; drops input
uint16_t halSerialOverflows(void);                    // bytes lost because the ring buffer was full
uint16_t halSerialRxHigh(void);                       // most bytes ever waiting in it, since boot
// TX by DMA (TLSR, at the v2 speed only); all 0 where TX is byte by byte
uint32_t halSerialTxDmaFrames(void);    // transfers sent by DMA
uint32_t halSerialTxDmaFails(void);     // transfers that did not finish in time: TX is byte by byte since
uint32_t halSerialTxDmaFailRegs(void);  // hardware state at the first failure (TLSR: 0xc20 0xc24 0x9c 0x9e)

// --- 802.15.4 radio -----------------------------------------------------------------------------
// Default delay from the end of a request to the reply's preamble ("reply timing" in ap.c), and the gap
// between block-burst frames, which the tag needs to copy and check each part.
#define RADIO_REPLY_AT_US 1220
#define RADIO_BURST_GAP_US 600
#define HAL_RADIO_MAX_FRAME 127

// `data` points into the receive slot, readable and writable until halRadioRxDone() and not after; it
// is 1 mod 4, so proto.h's frame structs are packed and aligned(1). The other fields are copies. The
// 32-bit fields come first to stay word-aligned under -fpack-struct.
struct halRxFrame {
    uint32_t sfdUs;      // radio time of the start of the frame, in microseconds (wraps)
    uint32_t ageUs;      // how long ago that was, when halRadioRx() returned it
    uint8_t *data;       // the MAC frame, in its receive slot: valid until halRadioRxDone()
    // pkt_fdc[10:0], the offset between the sender's carrier and ours as the demodulator measured it,
    // sign-extended from 11 bits. The unit and the sign convention are undocumented: see hal_tlsr.c.
    int16_t freqOffset;
    uint8_t len;         // MAC frame length without the FCS
    int8_t rssi;         // dBm
    uint8_t lqi;         // 0..255
    // The raw high byte of that field: bits 2..0 are pkt_fdc[10:8], bits 6..4 rx_packet_chn_efuse[2:0],
    // bits 7 and 3 documented as zero, so logging it checks the layout is read right.
    uint8_t fdcHigh;
} __attribute__((aligned(4)));

void halRadioInit(uint8_t channel);
// The next frame with a good CRC, held until halRadioRxDone(); a frame still held is released first
bool halRadioRx(struct halRxFrame *frame);
void halRadioRxDone(void);  // the frame halRadioRx() returned goes back to the radio: frame->data is gone
// The TX buffer, once the last frame has left: packet[0] = MAC frame length + 2 (FCS), packet[1..] the
// frame (HAL_RADIO_MAX_FRAME at most). After the send it may be read, not written, until the next call.
uint8_t *halRadioTxFrame(void);
// Sends the buffer as a burst frame RADIO_BURST_GAP_US after the last one. With yieldToRx, returns
// false without sending if a frame is arriving or waiting; the next halRadioTxFrame() discards it.
bool halRadioTx(bool yieldToRx);
bool halRadioTxBusy(void);
// The receiver is in the middle of a frame (TLSR: 0x448 bit 5), the test halRadioTx() yields to
bool halRadioRxArriving(void);
// Sends the buffer as a reply to the frame halRadioRx() returned last, its preamble starting delayUs
// after the end of that frame whatever the processing took ("reply timing" in ap.c)
void halRadioTxReply(uint32_t delayUs);
// When the frame last sent leaves the antenna (halMicros() time base; in the future for a reply)
uint32_t halRadioTxEndUs(void);
uint32_t halRadioTakeReplyGapMaxUs(void);  // largest request-end-to-reply gap since the last call
uint32_t halRadioTsFallbacks(void);        // frames timed by the interrupt (timestamp implausible)
bool halRadioIdleRssi(int8_t *dbm);        // the channel's RSSI now, if the radio is idle in RX
int8_t halRadioTxPowerApplied(void);       // dBm really sent at, or -128 while the driver default is used
void halRadioSetChannel(uint8_t channel);  // 11..26
// TX power for the next frame, at the nearest step the radio has; applied once the air is free
void halRadioSetTxPower(int8_t dbm);
// Crystal load-capacitance trim, 0..63 (0xFF: none). Moves our carrier; can decide if scanning tags hear us
void halRadioSetXtalCap(uint8_t cap);
uint8_t halRadioGetXtalCap(void);
// The crystal setting register in full and the calibration byte in flash
uint32_t halRadioXtalRegs(void);
void halRadioGetMac(uint8_t mac[8]);
uint32_t halRadioRxFrames(void);
uint32_t halRadioTxFrames(void);
uint32_t halRadioRxDropped(void);   // good frames lost because the receive queue was full
uint32_t halRadioRxBusyDrops(void); // ... and lost because the next frame was already arriving
uint32_t halRadioRxCrcFails(void);     // frames received that failed their CRC or length check
uint32_t halRadioRxCrcFailBytes(void); // ... the sum of their length bytes, where the length checked out
uint8_t halRadioRxHigh(void);       // most frames ever waiting in the receive queue, since boot
uint32_t halRadioTxPowerRegs(void); // the PA registers as they really are (0 where not applicable)
uint16_t halRadioTxSettleUs(void);  // the radio's TX settle time, which a scheduled reply allows for
// TLSR 0xf04, 0xf16, 0xf03: the registers that time a scheduled frame (0 on the host)
uint32_t halRadioTxDelayRegs(void);
uint32_t halRadioTxTimeouts(void); // frames whose end of transmission was never signalled
uint32_t halRadioTxLateArms(void); // replies that could not keep their schedule and went at once

// --- activity LED (optional) --------------------------------------------------------------------
void halLedSetBrightness(uint8_t brightness);  // 0 = off
void halLedFlash(void);                         // a short blink on radio activity
