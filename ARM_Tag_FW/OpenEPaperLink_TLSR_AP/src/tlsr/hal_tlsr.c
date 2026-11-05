// hal.h for the TLSR8258 with the Telink driver library (tl_zigbee_sdk, libdrivers_8258.a).
//
// Pins and flash use match the closed-source binaries/TLSR/OEPL_TLSR_AP.bin: UART TX PB1, RX PA0,
// PC4 driven high, MAC from the flash UID, frequency offset calibration at flash 0x77000.

#include "tl_common.h"

#include "../hal.h"
#include "../../../../oepl-radio-v2.h"  // the frame sizes and the chunk window the UART ring is held to

// --- configuration ------------------------------------------------------------------------------

#define UART_TX_PIN GPIO_PB1
#define UART_RX_PIN GPIO_PA0
#define UART_BOOT_BAUD 115200

// Driven high from the start, as the closed-source firmware does (an enable line or a LED)
#define STATUS_PIN GPIO_PC4

#define CFG_FREQUENCY_OFFSET_ADDR 0x77000  // one byte, 0xFF = not calibrated
#define CFG_MAC_ADDRESS_ADDR 0x76000       // 8 bytes, used only when the flash has no UID

// The radio returns to RX from the TX interrupt, so the main loop runs while a frame is on the air. A
// reply is started by the radio's own state machine at its due tick (rf_start_stx, halRadioTxReply);
// a burst part is started by software (radioSend).

#define LOGICCHANNEL_TO_PHYSICAL(p) (((p)-10) * 5)
#define TICKS_PER_MS (sys_tick_per_us * 1000)

// --- time ---------------------------------------------------------------------------------------

static uint32_t msNow = 0;
static uint32_t msTick = 0;  // clock_time() at msNow

// The 16 MHz system timer wraps every 268 s; this is called far more often than that.
uint32_t halMillis(void) {
    // Skip the divide when under a millisecond has passed: it is a call into div_mod.S that masks
    // interrupts for some 50 cycles.
    const uint32_t ticks = (uint32_t)(clock_time() - msTick);
    if (ticks < TICKS_PER_MS) return msNow;
    const uint32_t elapsed = ticks / TICKS_PER_MS;
    msNow += elapsed;
    msTick += elapsed * TICKS_PER_MS;
    return msNow;
}

uint32_t halMicros(void) { return clock_time() / sys_tick_per_us; }

void halDelayMs(uint32_t ms) {
    while (ms--) {
        sleep_us(1000);
        drv_wd_clear();
    }
}

void halWatchdogFeed(void) { drv_wd_clear(); }

// --- reset reason -------------------------------------------------------------------------------
// Analog register 0x3b (DEEP_ANA_REG1) is cleared only by a power cycle, so a cookie there means we
// already ran on this power. reg_tmr_sta bit 3 says the watchdog fired; it is read first thing, before
// drv_wd_clear() clears it (a plain register read is safe before the clocks are set up). No cookie:
// power-on; cookie and watchdog bit: watchdog; cookie alone: reset pin (the S3) or SWS flasher.
#define RESET_COOKIE_REG 0x3b
#define RESET_COOKIE 0x5A

static uint8_t resetReason = 0;
static uint8_t resetTmrSta = 0;
static uint8_t resetCookieWas = 0;

static void resetWdStatusCapture(void) { resetTmrSta = reg_tmr_sta; }

static void resetReasonDecide(void) {
    resetCookieWas = analog_read(RESET_COOKIE_REG);
    if (resetCookieWas != RESET_COOKIE) {
        resetReason = 1;  // ESP_RST_POWERON
        analog_write(RESET_COOKIE_REG, RESET_COOKIE);
    } else {
        resetReason = (resetTmrSta & FLD_TMR_STA_WD) ? 7 : 2;  // ESP_RST_WDT : ESP_RST_EXT
    }
}

uint8_t halResetReason(void) { return resetReason; }
uint32_t halResetReasonRaw(void) { return ((uint32_t)resetCookieWas << 8) | resetTmrSta; }

// --- UART ---------------------------------------------------------------------------------------
// RX: an interrupt per byte (no DMA) empties the FIFO into a ring. TX: byte by byte at 115200 baud; at
// 2 Mbaud by DMA (channel 1), until a transfer fails to finish, which turns DMA off for good.
//
// The S3 keeps at most V2_CHUNK_WINDOW frames unacknowledged and the main loop takes one per pass
// (halSerialReadFrame), so the ring holds the window less one frame, rounded up to a power of two.
// More arriving (a slow pass, a stalled TX DMA) is counted in halSerialOverflows(), fails the frame's
// CRC and is retransmitted by protocol v2; holding the whole window would cost another 1 KB of arena.
#define URX_FRAME_WIRE (V2_MAX_FRAME_COBS + 1)                 // 264
#define URX_WINDOW_BYTES (V2_CHUNK_WINDOW * URX_FRAME_WIRE)    // 1056: the protocol's bound
#define URX_RING 1024  // a power of 2
static volatile uint8_t uRing[URX_RING];
// & (URX_RING - 1) is the wrap
typedef char uRingIsPowerOfTwo[((URX_RING & (URX_RING - 1)) == 0 && URX_RING >= 256) ? 1 : -1];
// ... and it is the smallest power of two that holds the window less the frame being handled
typedef char uRingHoldsTheWindowLessOne[(URX_RING >= URX_WINDOW_BYTES - URX_FRAME_WIRE &&
                                         URX_RING / 2 < URX_WINDOW_BYTES - URX_FRAME_WIRE) ? 1 : -1];
// One object, so the interrupt reaches all four through one base register (the TC32 has eight low
// registers)
static struct {
    volatile uint16_t head;       // written by the interrupt
    volatile uint16_t tail;       // written by the main loop
    volatile uint16_t overflows;  // bytes lost because the ring was full
    volatile uint8_t rxIdx;       // which of the four data registers comes next
} u;
static uint32_t uBaud = UART_BOOT_BAUD;
static uint32_t uUsPerByte = 10000000UL / UART_BOOT_BAUD;  // 10 bits per byte, at the current speed
static bool uTxDma = false;  // TX by DMA at the current speed

_attribute_ram_code_ static void uartIrq(void) {
    // Re-read the count: at 2 Mbaud (5 us a byte) the next byte often arrives before the interrupt
    // returns, and taking it here saves an entry. The guard bounds the loop.
    unsigned cnt;
    for (unsigned guard = 16; guard && (cnt = reg_uart_buf_cnt & FLD_UART_RX_BUF_CNT) != 0; guard--) {
        do {
            const uint8_t b = reg_uart_data_buf(u.rxIdx);
            u.rxIdx = (u.rxIdx + 1) & 3;
            const unsigned next = (u.head + 1) & (URX_RING - 1);
            if (next == u.tail) {
                u.overflows++;
            } else {
                uRing[u.head] = b;
                u.head = (uint16_t)next;
            }
        } while (--cnt);
    }
    if (reg_uart_status0 & FLD_UART_RX_ERR_FLAG) {
        // write-1-to-clear; the driver's uart_clear_parity_error() read-modify-writes, clearing other
        // bits too (fixed in Telink's B85 driver V1.6.0)
        reg_uart_status0 = FLD_UART_CLEAR_RX_FLAG;
        // after the clear the next byte is read from 0x90 again (Telink B85 uart.c)
        u.rxIdx = 0;
        u.overflows++;
    }
}

// Waits (bounded) until the byte-by-byte transmitter has sent what it has
static void uartTxByteDrain(uint32_t us) {
    const uint32_t start = clock_time();
    while (uart_tx_is_busy() && !clock_time_exceed(start, us)) {
    }
}

// keepRx: the ring buffer keeps what it has (switching TX mode); otherwise it is emptied (new speed)
static void uartSetup(uint32_t baud, bool keepRx);

static void uartTxBytes(const uint8_t *buf, size_t len) {
    for (size_t i = 0; i < len; i++) {
        uart_ndma_send_byte(buf[i]);
        uartTxByteDrain(1000);  // 87 us per byte at 115200 baud; never wait forever
    }
}

// UART TX by DMA, double-buffered: v2link.c encodes a frame straight into the buffer that is free
// (halSerialTxBuf) while the other is on the line, and a transfer starts only when the previous one is
// done. Each buffer is a 4-byte DMA length followed by one v2 frame on the wire (HAL_SERIAL_TX_MAX).
// The register sequence is Telink's later B85 uart_send_dma(), which also enables channel 1 in 0xc20
// (libdrivers_8258.a's uart_dma_send does not). Done means 0xc24 bit 1 clear, UART TX done (0x9e
// bit 0) and the bytes' line time passed.
#define UTX_CHUNK HAL_SERIAL_TX_MAX
typedef char uTxChunkKeepsAlignment[((4 + UTX_CHUNK) % 4 == 0) ? 1 : -1];
static uint8_t uTxBuf[2][4 + UTX_CHUNK] __attribute__((aligned(4)));
static uint8_t uTxIdx = 0;          // the buffer filled next
static bool uTxActive = false;      // a transfer from uTxBuf[uTxIdx ^ 1] is (or may be) in flight
static bool uTxDmaDead = false;     // a transfer did not finish: byte by byte for good
static uint32_t uTxStartTick = 0;
static uint32_t uTxMinUs = 0;       // the transfer's time on the line
static uint32_t uTxDmaFrames = 0;
static uint32_t uTxDmaFails = 0;
static uint32_t uTxDmaFailRegs = 0;  // 0xc20, 0xc24, 0x9c, 0x9e at the first failure

static bool uartTxDmaDone(void) {
    return !(reg_dma_tx_rdy0 & FLD_DMA_CHN_UART_TX) && !uart_tx_is_busy() &&
           clock_time_exceed(uTxStartTick, uTxMinUs);
}

// Stops the DMA and resends the unfinished transfer byte by byte, after a 0x00 that ends any partial
// frame (the S3 ignores empty frames)
static void uartTxDmaFail(void) {
    uTxDmaFails++;
    if (!uTxDmaFailRegs) {
        uTxDmaFailRegs = ((uint32_t)reg_dma_chn_en << 24) | ((uint32_t)reg_dma_tx_rdy0 << 16) |
                         ((uint32_t)reg_uart_buf_cnt << 8) | reg_uart_status1;
    }
    uTxDmaDead = true;
    uTxActive = false;
    uartSetup(uBaud, true);  // stops DMA channel 1 and leaves TX DMA mode
    const uint8_t *b = uTxBuf[uTxIdx ^ 1];
    static const uint8_t delimiter = 0x00;
    uartTxBytes(&delimiter, 1);
    uartTxBytes(b + 4, b[0] | ((size_t)b[1] << 8));
}

static void uartTxDmaPoll(void) {
    if (!uTxActive) return;
    if (uartTxDmaDone()) {
        uTxActive = false;
    } else if (clock_time_exceed(uTxStartTick, 2 * uTxMinUs + 5000)) {
        uartTxDmaFail();
    }
}

// Waits until the transfer in flight is done; false if it failed (DMA is off then)
static bool uartTxDmaWait(void) {
    while (uTxActive) {
        uartTxDmaPoll();
    }
    return !uTxDmaDead;
}

static void uartTxDmaStart(uint8_t *b, size_t n) {
    uTxMinUs = (uint32_t)n * uUsPerByte + 20;  // 10 bits per byte, plus 20 us
    const uint8_t r = irq_disable();  // 0xc20 is also changed by the radio interrupt
    reg_uart_state = BIT(7);          // uart_clr_tx_done(): TX done is set again when this one has left
    reg_dma_uart_tx_addr = (uint16_t)(uint32_t)b;
    reg_dma1_addrHi = 0x04;           // RAM at 0x84xxxx, as rf_tx_pkt() sets it for the RF TX channel
    reg_dma_uart_tx_size = 0xff;      // in 16-byte units: the most; the length is in the header
    reg_dma_chn_en |= FLD_DMA_CHN_UART_TX;
    reg_dma_tx_rdy0 |= FLD_DMA_CHN_UART_TX;
    uTxStartTick = clock_time();
    irq_restore(r);
    uTxActive = true;
    uTxDmaFrames++;
}

// The buffer filled next, which no transfer in flight reads
uint8_t *halSerialTxBuf(void) { return uTxBuf[uTxIdx] + 4; }

void halSerialTxCommit(size_t n) {
    uint8_t *b = uTxBuf[uTxIdx];
    if (n > UTX_CHUNK) n = UTX_CHUNK;  // the buffer's end; nothing past it was the caller's to write
    if (uTxDma) {
        b[0] = (uint8_t)n;  // DMA length in bytes, little endian, before the data
        b[1] = (uint8_t)(n >> 8);
        b[2] = 0;
        b[3] = 0;
        // On failure the OTHER buffer is resent (uartTxDmaFail) and this one goes byte by byte
        if (uartTxDmaWait()) {
            uartTxDmaStart(b, n);
            uTxIdx ^= 1;
            return;
        }
    }
    uartTxBytes(b + 4, n);
}

// For bytes that already exist elsewhere: copied into the frame buffer chunk by chunk
void halSerialWrite(const uint8_t *buf, size_t len) {
    while (len && uTxDma) {
        const size_t n = len > UTX_CHUNK ? UTX_CHUNK : len;
        memcpy(halSerialTxBuf(), buf, n);
        halSerialTxCommit(n);  // if DMA failed in it, uTxDma is false now: the rest goes byte by byte
        buf += n;
        len -= n;
    }
    if (len) uartTxBytes(buf, len);
}

void halSerialSetBaud(uint32_t baud) {
    uartTxDmaWait();
    uartTxByteDrain(2000);
    uartSetup(baud, false);
}

static void uartTxPoll(void) { uartTxDmaPoll(); }
uint32_t halSerialTxDmaFrames(void) { return uTxDmaFrames; }
uint32_t halSerialTxDmaFails(void) { return uTxDmaFails; }
uint32_t halSerialTxDmaFailRegs(void) { return uTxDmaFailRegs; }

static void uartSetup(uint32_t baud, bool keepRx) {
    const uint8_t r = irq_disable();
    gpio_set_func(UART_TX_PIN, AS_GPIO);
    gpio_set_output_en(UART_TX_PIN, 1);
    gpio_write(UART_TX_PIN, 1);  // idle level while the UART takes over
    gpio_set_func(UART_RX_PIN, AS_GPIO);
    gpio_set_input_en(UART_RX_PIN, 1);
    gpio_set_output_en(UART_RX_PIN, 0);

    // TX by DMA only at the v2 speed, never for the ASCII handshake, never after a failure
    uTxDma = baud > UART_BOOT_BAUD && !uTxDmaDead;

    // DMA channels 0 (UART RX, unused) and 1 (UART TX): no interrupts, channel 1 enabled only for TX
    // DMA. Not dma_chn_irq_enable(.., 0): it also disables the channels in 0xc20.
    if (reg_dma_tx_rdy0 & FLD_DMA_CHN_UART_TX) reg_dma_tx_rdy0 &= (uint8_t)~FLD_DMA_CHN_UART_TX;  // stuck
    reg_dma_chn_irq_msk &= (uint8_t)~(FLD_DMA_CHN_UART_RX | FLD_DMA_CHN_UART_TX);
    reg_dma_irq_status = FLD_DMA_CHN_UART_RX | FLD_DMA_CHN_UART_TX;
    reg_dma_chn_en &= (uint8_t)~(FLD_DMA_CHN_UART_RX | FLD_DMA_CHN_UART_TX);
    if (uTxDma) {
        reg_dma_uart_tx_mode = 0;  // read from memory, no ping-pong / FIFO / auto mode (reset value)
        reg_dma_chn_en |= FLD_DMA_CHN_UART_TX;
    }

    uart_gpio_set(UART_TX_PB1, UART_RX_PA0);
    uart_reset();
    // exact dividers: baud = 48 MHz / ((div + 1) * (bwpc + 1))
    if (baud == 2000000) {
        uart_init(1, 11, PARITY_NONE, STOP_BIT_ONE);  // 48 MHz / (2 * 12), 12 samples per bit
    } else if (baud == 115200) {
        uart_init(25, 15, PARITY_NONE, STOP_BIT_ONE);  // the SDK's table: 115384 baud, +0.16 %
    } else {
        uart_init_baudrate(baud, CLOCK_SYS_CLOCK_HZ, PARITY_NONE, STOP_BIT_ONE);
    }
    uart_dma_enable(0, uTxDma);  // RX byte by byte (interrupt); TX by DMA or byte by byte
    uBaud = baud;
    uUsPerByte = 10000000UL / baud;  // exact at 2 Mbaud (5 us), the only speed DMA is used at
    uart_ndma_irq_triglevel(1, 0);  // an interrupt for every received byte
    uart_irq_enable(1, 0);
    uart_ndma_clear_tx_index();
    u.rxIdx = 0;  // uart_reset() cleared the hardware pointers
    if (!keepRx) {
        u.head = 0;
        u.tail = 0;
    }
    irq_set_mask(FLD_IRQ_UART_EN);
    irq_restore(r);
}

// Peak ring fill since boot, sampled when the main loop drains it (where the fill peaks)
static uint16_t uRxHigh = 0;
uint16_t halSerialRxHigh(void) { return uRxHigh; }

static size_t serialRead(uint8_t *buf, size_t max, bool toDelimiter) {
    size_t n = 0;
    uint16_t tail = u.tail;
    const uint16_t head = u.head;
    const uint16_t fill = (uint16_t)((head - tail) & (URX_RING - 1));
    if (fill > uRxHigh) uRxHigh = fill;
    while (tail != head && n < max) {
        const uint8_t b = uRing[tail];
        buf[n++] = b;
        tail = (tail + 1) & (URX_RING - 1);
        if (toDelimiter && b == 0x00) break;
    }
    u.tail = tail;
    return n;
}
size_t halSerialRead(uint8_t *buf, size_t max) { return serialRead(buf, max, false); }
size_t halSerialReadFrame(uint8_t *buf, size_t max) { return serialRead(buf, max, true); }

uint16_t halSerialOverflows(void) { return u.overflows; }

// --- radio --------------------------------------------------------------------------------------
// Received frames go by DMA into a ring of slots and are read in place. The receive interrupt only
// checks the frame and arms the next slot, so it does not hold up the UART (4-byte FIFO, 20 us).
//
// Who owns which slot:
//
//   rxHead           armed: the DMA writes into it whenever a frame arrives. The interrupt alone
//                    moves it, and only onto a slot that is not rxTail.
//   rxTail           the frame the main loop is reading, from halRadioRx() until halRadioRxDone().
//                    The main loop alone moves it, and only on that release.
//   tail+1..head-1   frames waiting their turn; nobody writes them.
//
// rxTail is one byte, so the release needs no lock: an interrupt that sees the old value drops a
// frame it could have kept, never the reverse. Four slots is plenty: on hardware no more than one
// frame has been seen waiting (halRadioRxHigh), and a loss would show in rxDropped.
#define RX_SLOTS 4
typedef char halRxSlotsPow2[(RX_SLOTS >= 2 && (RX_SLOTS & (RX_SLOTS - 1)) == 0) ? 1 : -1];  // ring % is a mask
// A slot as the DMA fills it: 4-byte length, PHR, MAC frame with FCS, then 4 bytes timestamp, 2
// frequency offset, 1 RSSI, 1 status; rounded up to the 16-byte units rf_rx_cfg() takes. The DMA stops
// at that size, so a corrupt length cannot run past the slot.
#define RX_SLOT_SIZE (((4 + 1 + HAL_RADIO_MAX_FRAME + 2 + 8) + 15) / 16 * 16)
static uint8_t rxSlots[RX_SLOTS][RX_SLOT_SIZE] __attribute__((aligned(4)));
typedef char halRxSlotFits[(RX_SLOT_SIZE >= 4 + 1 + HAL_RADIO_MAX_FRAME + 2 + 8 && RX_SLOT_SIZE % 16 == 0) ? 1 : -1];

// Every slot is 4-aligned, so the MAC frame at p + 5 is always at 1 mod 4 (see halRxFrame in hal.h).
// GCC 4.5 has no _Static_assert, hence the negative array sizes.
typedef char halRxFrameDataResidue[(RX_SLOT_SIZE % 4 == 0) ? 1 : -1];

// The SDK's ZB_RADIO_RSSI_TO_LQI, 255 * (rssi + 99) / 84 for rssi in -99..-15, as a table: a divide
// is a call into div_mod.S with interrupts masked.
static const uint8_t rssiToLqi[85] = {
    0,   3,   6,   9,   12,  15,  18,  21,  24,  27,  30,  33,  36,  39,  42,  45,  48,
    51,  54,  57,  60,  63,  66,  69,  72,  75,  78,  81,  85,  88,  91,  94,  97,  100,
    103, 106, 109, 112, 115, 118, 121, 124, 127, 130, 133, 136, 139, 142, 145, 148, 151,
    154, 157, 160, 163, 166, 170, 173, 176, 179, 182, 185, 188, 191, 194, 197, 200, 203,
    206, 209, 212, 215, 218, 221, 224, 227, 230, 233, 236, 239, 242, 245, 248, 251, 255,
};
static volatile uint32_t rxEndTick[RX_SLOTS];  // clock_time() in the receive interrupt
static volatile uint8_t rxHead = 0;            // the slot the DMA writes into
static volatile uint8_t rxTail = 0;            // the next slot to read, or the one being read; tail..head-1 hold frames
static bool rxHeld = false;                    // the frame in rxTail is out with the main loop
static volatile uint32_t rxDropped = 0;
static volatile uint32_t rxBusyDrops = 0;  // the DMA was already writing the next frame: it is lost
// Frames failing the CRC or length check, and the sum of the length bytes of those with a valid length
static volatile uint32_t rxCrcFails = 0;
static volatile uint32_t rxCrcFailBytes = 0;
static uint32_t rxFrames = 0;
static uint32_t txFrames = 0;
static uint32_t txTimeouts = 0;  // a frame was still "on the air" after 10 ms: no TX interrupt
static uint32_t lastRxEndTick = 0;  // end of the frame halRadioRx() returned last
static uint32_t txEndTick = 0;      // when the frame last handed to the radio leaves the antenna
static uint32_t tsFallbacks = 0;
static uint32_t replyGapMaxUs = 0;  // largest gap between a request's end and our reply's TX start
static uint32_t txLateArms = 0;  // replies whose due tick had passed when armed: sent at once

static uint8_t txBuf[ZB_RADIO_TX_HDR_LEN + 128] __attribute__((aligned(4)));
static volatile uint8_t txBusy = 0;
static volatile uint8_t txStx = 0;  // the frame on the air was started by rf_start_stx (auto mode)
static uint8_t radioChannel = 11;
static int8_t radioPowerDbm = 0;   // what was last asked for
static int8_t radioPowerSet = -128;  // what the PA registers hold; -128 = never written by us
static void radioApplyTxPower(void);
// TX settle time in us (REG_ADDR16(0xf04), 150 by default). rf_start_stx(addr, tick) begins settling
// at tick, so the preamble leaves at tick + this. Not ZB_TX_WAIT_US, the manual path's software wait.
static uint16_t txSettleUs = 150;

_attribute_ram_code_ static void radioRxIrq(void) {
    uint8_t *p = rxSlots[rxHead];
    if (RF_DMA_BUSY()) {
        rxBusyDrops++;
        return;
    }
    ZB_RADIO_RX_DISABLE;
    if (!ZB_RADIO_CRC_OK(p) || !ZB_RADIO_PACKET_LENGTH_OK(p)) {
        rxCrcFails++;
        if (ZB_RADIO_PACKET_LENGTH_OK(p)) rxCrcFailBytes += p[4];
        ZB_RADIO_RX_BUF_CLEAR(p);
        ZB_RADIO_RX_ENABLE;
        return;
    }
    const uint8_t next = (rxHead + 1) % RX_SLOTS;
    if (next == rxTail) {
        rxDropped++;  // the main loop is behind: this frame is lost, the buffer is used again
        ZB_RADIO_RX_BUF_CLEAR(p);
        ZB_RADIO_RX_ENABLE;
        return;
    }
    rxEndTick[rxHead] = clock_time();
    rxHead = next;
    uint8_t *n = rxSlots[next];
    ZB_RADIO_RX_BUF_CLEAR(n);
    ZB_RADIO_RX_BUF_SET(n);
    ZB_RADIO_RX_ENABLE;
}

// Crystal load-capacitance trim: FLD_XO_CDAC_ANA, the low six bits of analog register 0x8a, moves our
// TX carrier and RX centre together. rf_drv_cap() applies the flash 0x77000 byte the same way when it
// is programmed; a trim set here is reapplied after it on every radio init.

static uint8_t xtalCap = 0xFF;  // 0xFF until read from the hardware or set

uint8_t halRadioGetXtalCap(void) {
    if (xtalCap == 0xFF) xtalCap = analog_read(areg_xo_setting) & FLD_XO_CDAC_ANA;
    return xtalCap;
}

void halRadioSetXtalCap(uint8_t cap) {
    if (cap > FLD_XO_CDAC_ANA) cap = FLD_XO_CDAC_ANA;
    xtalCap = cap;
    analog_write(areg_xo_setting, (analog_read(areg_xo_setting) & (uint8_t)~FLD_XO_CDAC_ANA) | cap);
}

// Register 0x8a in full (with FLD_XO_CAP_OFF_ANA set the capacitor bank is out of circuit and the DAC
// does nothing) and the calibration byte from flash
uint32_t halRadioXtalRegs(void) {
    uint8_t calib = 0xFF;
    flash_read_page(CFG_FREQUENCY_OFFSET_ADDR, 1, &calib);
    return ((uint32_t)calib << 8) | analog_read(areg_xo_setting);
}

static void radioInitHw(void) {
    rf_drv_init(RF_MODE_ZIGBEE_250K);
    rf_drv_cap(CFG_FREQUENCY_OFFSET_ADDR);
    if (xtalCap != 0xFF) halRadioSetXtalCap(xtalCap);
    const uint16_t settle = REG_ADDR16(0xf04);
    if (settle >= 50 && settle <= 1000) txSettleUs = settle;  // trust it only if it looks sane
    ZB_TIMESTAMP_ENABLE;  // RX timestamp in each packet (REG 0x430 bit 1), as both Telink 802.15.4 MACs
    radioPowerSet = -128;  // rf_drv_init() has been over the registers: set the power again
    radioApplyTxPower();
}


_attribute_ram_code_ static void radioTxIrq(void) {
    ZB_RADIO_TX_DONE_CLR;
    if (txStx) {
        // back from the state machine (auto mode) to manual RX
        rf_set_tx_rx_off_auto_mode();
        rf_set_tx_rx_off();
        txStx = 0;
    }
    ZB_RADIO_TRX_SWITCH(RF_MODE_RX, LOGICCHANNEL_TO_PHYSICAL(radioChannel));
    txBusy = 0;
}

void halRadioInit(uint8_t channel) {
    radioChannel = channel;
    radioInitHw();
    // as rf_reset() in ieee802154/mac/mac_phy.c of Telink's 802.15.4 SDK
    // (telink-semi/telink_802154_sdk on GitHub)
    const uint8_t r = irq_disable();
    ZB_RADIO_TRX_SWITCH(RF_MODE_TX, LOGICCHANNEL_TO_PHYSICAL(radioChannel));
    ZB_RADIO_RX_BUF_CLEAR(rxSlots[rxHead]);
    rf_rx_buffer_set(rxSlots[rxHead], RX_SLOT_SIZE, 0);
    ZB_RADIO_TRX_CFG(RX_SLOT_SIZE);  // RX size, RF interrupts RX and TX only, ZB_RT interrupt on
    ZB_RADIO_RX_ENABLE;
    ZB_RADIO_TX_ENABLE;
    ZB_RADIO_TRX_SWITCH(RF_MODE_RX, LOGICCHANNEL_TO_PHYSICAL(radioChannel));
    irq_restore(r);
}

static uint8_t rxHigh = 0;
uint8_t halRadioRxHigh(void) { return rxHigh; }

// The frame in rxTail goes back to the radio. Until this store the interrupt will not arm the slot
// (see "who owns which slot" above); after it, the next frame may be DMA'd into it at any time.
void halRadioRxDone(void) {
    if (!rxHeld) return;
    rxHeld = false;
    rxTail = (rxTail + 1) % RX_SLOTS;
}

bool halRadioRx(struct halRxFrame *f) {
    halRadioRxDone();  // a frame still held from the last call
    // the frames waiting when the main loop comes to read one, the one just released not counted
    const uint8_t waiting = (uint8_t)((rxHead + RX_SLOTS - rxTail) % RX_SLOTS);
    if (waiting > rxHigh) rxHigh = waiting;
    while (rxTail != rxHead) {
        const uint8_t slot = rxTail;
        uint8_t *p = rxSlots[slot];
        const uint8_t phr = p[4];  // MAC frame length with the FCS
        bool ok = phr >= 2 && phr - 2 <= HAL_RADIO_MAX_FRAME;
        if (ok) {
            f->len = phr - 2;
            f->data = p + 5;  // read in place: the slot stays out of the ring until halRadioRxDone()
            int16_t rssi = (int16_t)RF_ZIGBEE_PACKET_RSSI_GET(p) - 110;
            f->rssi = (int8_t)rssi;
            int16_t ed = rssi;  // the macro's AGC clamp; rssi itself is reported unclamped
            if (ed > -15) ed = -15;
            if (ed < -99) ed = -99;
            f->lqi = rssiToLqi[ed + 99];
            // After the timestamp (Telink's B91 handbook, 802.15.4 receive packet): p[phr+9] is
            // pkt_fdc[7:0], p[phr+10] is {0, rx_packet_chn_efuse[2:0], 0, pkt_fdc[10:8]}, then RSSI
            // and status, where the 8258 driver's macros also put them. The demodulator's frequency
            // offset is pkt_fdc[10:0]; its unit and sign are undocumented. Two's complement is
            // assumed (observed values are small, not near 1024), but no negative value has been seen.
            const uint16_t fdc = (uint16_t)(p[phr + 9] | ((p[phr + 10] & 0x07) << 8));
            f->freqOffset = (int16_t)(fdc << 5) >> 5;  // sign-extend the 11 bits
            f->fdcHigh = p[phr + 10];
            // When the timestamp is captured is undocumented; it is taken as the start of the PHR, so
            // the frame ends (PHR + 1) bytes later and the preamble began 5 bytes earlier. (A capture
            // after the frame would send every frame to the fallback; within the header the error is
            // at most 192 us.) Both branches put the preamble (PHR + 6) bytes before the frame end.
            const uint32_t usTicks = 32 * sys_tick_per_us;
            const uint32_t ts = RF_ZIGBEE_PACKET_TIMESTAMP_GET(p);
            const uint32_t tsEnd = ts + (uint32_t)(phr + 1) * usTicks;
            const uint32_t irqLagTicks = rxEndTick[slot] - tsEnd;  // the interrupt comes after the end
            uint32_t endTick, sfdTick;
            if (irqLagTicks < 3000 * sys_tick_per_us) {
                endTick = tsEnd;
                sfdTick = ts - 5 * usTicks;  // the start of the preamble: 4 bytes + SFD before
            } else {
                endTick = rxEndTick[slot];
                sfdTick = endTick - (uint32_t)(phr + 6) * usTicks;
                tsFallbacks++;
            }
            lastRxEndTick = endTick;
            f->sfdUs = sfdTick / sys_tick_per_us;
            f->ageUs = (uint32_t)(clock_time() - sfdTick) / sys_tick_per_us;
            rxFrames++;
            // the trailer has been read: the caller may now write into the slot
            rxHeld = true;
            return true;
        }
        rxTail = (slot + 1) % RX_SLOTS;  // a length nothing can be: the slot goes straight back
    }
    return false;
}

static void radioSend(void);

// Time on air for a psdu-byte PHY payload: preamble (4), SFD and length byte first, 32 us per byte
static inline uint32_t airtimeUs(uint8_t psdu) { return ((uint32_t)psdu + 6) * 32; }

static void radioWaitTxFree(void);

// One TX buffer. Frames are built in txBuf itself, at the PHR (packet[0] of hal.h); the DMA header in
// front is written at the send. From the moment a frame is handed to the radio (manual TX, or armed
// by rf_start_stx) until the TX interrupt clears txBusy, the radio reads txBuf, so it is handed out
// only once txBusy is clear, or after radioWaitTxFree() has given up and stopped the radio. A burst
// part never waits here (burstStep() checks halRadioTxBusy() first). A reply waits only when our own
// frame started after its request ended (a burst part, ~4.3 ms, or an earlier reply); by then its
// due tick has passed anyway (txLateArms), except for a PONG's longer lead.
#define TX_PACKET (txBuf + ZB_RADIO_TX_HDR_LEN - 1)  // packet[0], the PHR
typedef char halTxFrameFits[(sizeof(txBuf) - (ZB_RADIO_TX_HDR_LEN - 1) >= 1 + HAL_RADIO_MAX_FRAME) ? 1 : -1];

uint8_t *halRadioTxFrame(void) {
    radioWaitTxFree();
    return TX_PACKET;
}

// A burst part. The gap is counted from the end of the frame sent last, so a call after a yield does
// not wait it twice; a tag's frame (768 us or more) cannot complete within the 600 us gap, so the
// wait delays no reply. With yieldToRx, a frame being received (is_rf_receiving_pkt(), 0x448 bit 5,
// the test Telink's 802.15.4 MAC uses) or waiting in the ring takes precedence: sending would cut it
// off, or push its reply ~4.3 ms past its due tick. The slot being read (rxHeld) is not waiting.
// yieldToEnergy reads the channel at the end of the gap, past the time the receiver reads high after
// we transmit (NF_TX_RECOVER_US in ap.c), and catches what the radio cannot decode, such as WiFi. It
// gets no reading while the receive flag is up (halRadioIdleRssi), so a stuck flag hides the channel.
bool halRadioTx(bool yieldToRx, bool yieldToEnergy) {
    const uint32_t gapEnd = txEndTick + RADIO_BURST_GAP_US * sys_tick_per_us;
    const int32_t left = (int32_t)(gapEnd - clock_time());
    if (left > 0 && left <= (int32_t)(RADIO_BURST_GAP_US * sys_tick_per_us)) WaitUs((uint32_t)left / sys_tick_per_us);
    if (yieldToRx) {
        const int queued = (int)((rxHead + RX_SLOTS - rxTail) % RX_SLOTS) - (rxHeld ? 1 : 0);
        if (queued > 0 || is_rf_receiving_pkt()) return false;
    }
    int8_t rssi;
    if (yieldToEnergy && halRadioIdleRssi(&rssi) && rssi > RADIO_BUSY_DBM) return false;
    radioSend();
    return true;
}

uint32_t halRadioTxEndUs(void) { return txEndTick / sys_tick_per_us; }

static void radioTxHeader(void);

// A scheduled reply. rf_start_stx(buf, tick) hands the frame to the radio's state machine, which
// starts the TX settle at `tick` and has the preamble on the air txSettleUs later; the TX interrupt
// (radioTxIrq) then puts the radio back into manual RX. So the reply leaves delayUs after the end of
// the request however long ap.c took to build it, as long as it is armed before its tick. Armed
// after it (a burst part or an earlier reply was on the air), it goes at once and is counted in
// txLateArms; on hardware such replies have gone out without a TX timeout.
void halRadioTxReply(uint32_t delayUs) {
    if (!delayUs) delayUs = RADIO_REPLY_AT_US;
    radioWaitTxFree();  // returns at once: halRadioTxFrame() waited already
    radioApplyTxPower();  // the air is free now, so the PA registers may be written
    radioTxHeader();
    // aim txSettleUs early, so the preamble leaves delayUs after the end of the request
    const uint32_t leadUs = delayUs > txSettleUs ? delayUs - txSettleUs : 0;
    uint32_t due = lastRxEndTick + leadUs * sys_tick_per_us;
    const uint32_t now = clock_time();
    if ((int32_t)(due - now) > (int32_t)((delayUs + 200) * sys_tick_per_us)) due = now;  // implausible
    const uint32_t start = (int32_t)(due - now) > 0 ? due : now;
    if (start != due) txLateArms++;
    // what the diagnostics report is the gap to the frame actually going on air
    const uint32_t gapUs = (start - lastRxEndTick) / sys_tick_per_us + txSettleUs;
    if (gapUs > replyGapMaxUs) replyGapMaxUs = gapUs;
    txEndTick = start + (txSettleUs + airtimeUs(txBuf[4])) * sys_tick_per_us;
    txFrames++;
    const uint8_t r = irq_disable();
    rf_set_tx_rx_off();  // leave manual RX; the state machine takes over
    txStx = 1;
    txBusy = 1;
    ZB_RADIO_TX_DONE_CLR;
    ZB_RADIO_RX_DONE_CLR;
    // `start`, never a tick in the past: the state machine would wait for the timer to wrap (268 s)
    rf_start_stx(txBuf, start);
    irq_restore(r);
}

uint32_t halRadioTakeReplyGapMaxUs(void) {
    const uint32_t v = replyGapMaxUs;
    replyGapMaxUs = 0;
    return v;
}
bool halRadioIdleRssi(int8_t *dbm) {
    if (txBusy || RF_DMA_BUSY()) return false;
    *dbm = rf_rssi_get_154();
    return true;
}

int8_t halRadioTxPowerApplied(void) { return radioPowerSet; }
// The PA registers (rf_drv_init() does not set them): 0x1227 bits 4..0 and 0x1226 bit 7 are the PA
// code, 0x1225 bit 6 picks VBAT or VANT
uint32_t halRadioTxPowerRegs(void) {
    return ((uint32_t)read_reg8(0x1225) << 16) | ((uint32_t)read_reg8(0x1226) << 8) | read_reg8(0x1227);
}
uint16_t halRadioTxSettleUs(void) { return txSettleUs; }
// Registers that decide when a scheduled frame leaves, at chip reset values (rf_drv_init() does not
// write the 0xf00 block): 0xf04 TX settle; 0xf16 FLD_RF_TX_EN_DLY_EN (bit 0) and FLD_RF_R_TX_EN_DLY
// (bits 7..4), a TX-enable delay that rf_start_stx leaves alone; 0xf03 state machine timeout enables.
uint32_t halRadioTxDelayRegs(void) {
    return ((uint32_t)REG_ADDR16(0xf04) << 16) | ((uint32_t)read_reg8(0xf16) << 8) | read_reg8(0xf03);
}
uint32_t halRadioTsFallbacks(void) { return tsFallbacks; }
uint32_t halRadioTxLateArms(void) { return txLateArms; }

// Waits for a frame still on the air (at most 10 ms); a lost TX interrupt is counted and the radio
// brought back to RX
static void radioWaitTxFree(void) {
    const uint32_t start = clock_time();
    while (txBusy && !clock_time_exceed(start, 10000)) {
    }
    if (txBusy) {
        txTimeouts++;
        const uint8_t r = irq_disable();
        if (txStx) {
            rf_set_tx_rx_off_auto_mode();
            rf_set_tx_rx_off();
            txStx = 0;
        }
        ZB_RADIO_TRX_SWITCH(RF_MODE_RX, LOGICCHANNEL_TO_PHYSICAL(radioChannel));
        txBusy = 0;
        irq_restore(r);
    }
}

// The DMA's length field in front of the frame ap.c built at TX_PACKET: the MAC frame plus one
static void radioTxHeader(void) {
    const uint8_t len = TX_PACKET[0] - 2;  // the MAC frame without the FCS
    ZB_RADIO_DMA_HDR_BUILD(txBuf, len);
}

static void radioSend(void) {
    radioWaitTxFree();  // returns at once: halRadioTxFrame() waited already
    radioApplyTxPower();  // the air is free now, so the PA registers may be written
    radioTxHeader();
    txFrames++;
    txBusy = 1;
    const uint8_t r = irq_disable();
    ZB_RADIO_TRX_SWITCH(RF_MODE_TX, LOGICCHANNEL_TO_PHYSICAL(radioChannel));
    irq_restore(r);
    txEndTick = clock_time() + (ZB_TX_WAIT_US + airtimeUs(txBuf[4])) * sys_tick_per_us;
    WaitUs(ZB_TX_WAIT_US);  // the synthesizer settles
    // as rf802154_tx() in Telink's 802.15.4 MAC
    ZB_RADIO_TX_DONE_CLR;
    ZB_RADIO_RX_DONE_CLR;
    ZB_RADIO_TX_START(txBuf);
}

bool halRadioTxBusy(void) { return txBusy != 0; }
bool halRadioRxArriving(void) { return !txBusy && is_rf_receiving_pkt(); }

void halRadioSetChannel(uint8_t channel) {
    const uint32_t start = clock_time();
    while (txBusy && !clock_time_exceed(start, 10000)) {
    }
    radioChannel = channel;
    const uint8_t r = irq_disable();
    ZB_RADIO_TRX_SWITCH(RF_MODE_RX, LOGICCHANNEL_TO_PHYSICAL(radioChannel));
    irq_restore(r);
}

// TX power steps of the TLSR8258 (rf_drv.h), highest first, in 0.01 dBm
static const struct {
    int16_t centiDbm;
    uint8_t index;
} powerTable[] = {
    {1046, RF_POWER_INDEX_P10p46dBm}, {1001, RF_POWER_INDEX_P10p01dBm}, {897, RF_POWER_INDEX_P8p97dBm},
    {813, RF_POWER_INDEX_P8p13dBm},   {702, RF_POWER_INDEX_P7p02dBm},   {614, RF_POWER_INDEX_P6p14dBm},
    {513, RF_POWER_INDEX_P5p13dBm},   {394, RF_POWER_INDEX_P3p94dBm},   {301, RF_POWER_INDEX_P3p01dBm},
    {199, RF_POWER_INDEX_P1p99dBm},   {90, RF_POWER_INDEX_P0p90dBm},    {4, RF_POWER_INDEX_P0p04dBm},
    {-97, RF_POWER_INDEX_N0p97dBm},   {-189, RF_POWER_INDEX_N1p89dBm},  {-303, RF_POWER_INDEX_N3p03dBm},
    {-426, RF_POWER_INDEX_N4p26dBm},  {-503, RF_POWER_INDEX_N5p03dBm},  {-667, RF_POWER_INDEX_N6p67dBm},
    {-865, RF_POWER_INDEX_N8p65dBm},  {-1140, RF_POWER_INDEX_N11p4dBm}, {-1588, RF_POWER_INDEX_N15p88dBm},
    {-2518, RF_POWER_INDEX_N25p18dBm},
};

// Applied at once when the air is free, else at the next send: rewriting the PA registers during a
// frame changes the power of its tail.
void halRadioSetTxPower(int8_t dbm) {
    radioPowerDbm = dbm;
    if (!txBusy) radioApplyTxPower();
}

// The highest step at or below the request + 0.5 dB ("10 dBm" is 10.01, "11" is 10.46). Cheap enough
// (three register writes) to call before every frame.
static void radioApplyTxPower(void) {
    if (radioPowerDbm == radioPowerSet) return;
    radioPowerSet = radioPowerDbm;
    const int16_t want = (int16_t)radioPowerDbm * 100 + 50;
    uint8_t i = 0;
    while (i < sizeof(powerTable) / sizeof(powerTable[0]) - 1 && powerTable[i].centiDbm > want) i++;
    rf_set_power_level_index((RF_PowerIndexTypeDef)powerTable[i].index);
}

void halRadioGetMac(uint8_t mac[8]) {
    // the two halves of the flash UID combined, as the closed-source firmware does, so the MAC is kept
    unsigned int mid = 0;
    uint8_t uid[16];
    memset(uid, 0, sizeof(uid));
    if (flash_read_mid_uid_with_check(&mid, uid)) {
        for (uint8_t i = 0; i < 8; i++) mac[i] = uid[i] ^ uid[i + 8];
        return;
    }
    flash_read_page(CFG_MAC_ADDRESS_ADDR, 8, mac);
    bool blank = true;
    for (uint8_t i = 0; i < 8; i++) {
        if (mac[i] != 0xFF) blank = false;
    }
    if (blank) {
        const uint32_t r1 = clock_time() ^ ((uint32_t)analog_read(0x8a) << 16);
        for (uint8_t i = 0; i < 8; i++) mac[i] = (uint8_t)(r1 >> ((i & 3) * 8)) ^ (uint8_t)(i * 0x3D);
    }
}

uint32_t halRadioRxFrames(void) { return rxFrames; }
uint32_t halRadioTxFrames(void) { return txFrames; }
uint32_t halRadioRxDropped(void) { return rxDropped; }
uint32_t halRadioRxBusyDrops(void) { return rxBusyDrops; }
uint32_t halRadioRxCrcFails(void) { return rxCrcFails; }
uint32_t halRadioRxCrcFailBytes(void) { return rxCrcFailBytes; }
uint32_t halRadioTxTimeouts(void) { return txTimeouts; }

// --- LED ----------------------------------------------------------------------------------------
// No activity LED is known on the TLSR AP modules; define ACTIVITY_LED_PIN to have one.

#ifdef ACTIVITY_LED_PIN
static uint8_t ledBrightness = 255;  // flash at full brightness until the first SET_CONFIG
static uint32_t ledOnTick = 0;
static bool ledOn = false;
void halLedSetBrightness(uint8_t brightness) { ledBrightness = brightness; }
void halLedFlash(void) {
    if (!ledBrightness) return;
    gpio_write(ACTIVITY_LED_PIN, 1);
    ledOn = true;
    ledOnTick = clock_time();
}
static void ledPoll(void) {
    if (ledOn && clock_time_exceed(ledOnTick, 30000)) {
        gpio_write(ACTIVITY_LED_PIN, 0);
        ledOn = false;
    }
}
#else
void halLedSetBrightness(uint8_t brightness) { (void)brightness; }
void halLedFlash(void) {}
static void ledPoll(void) {}
#endif

// --- interrupts ---------------------------------------------------------------------------------
// Replaces platform/services/b85m/irq_handler.c of the SDK: the UART first (4 bytes of FIFO). Nothing
// here re-enables interrupts, so handlers never nest (tools/stackdepth.py relies on this).

_attribute_ram_code_ void irq_handler(void) {
    const uint32_t src = irq_get_src();
    if (src & FLD_IRQ_UART_EN) {
        reg_irq_src = FLD_IRQ_UART_EN;
        uartIrq();
    }

    const uint16_t srcRf = rf_irq_src_get();
    if (srcRf & FLD_RF_IRQ_TX) {
        rf_irq_clr_src(FLD_RF_IRQ_TX);
        radioTxIrq();
    }
    if (srcRf & FLD_RF_IRQ_RX) {
        rf_irq_clr_src(FLD_RF_IRQ_RX);
        radioRxIrq();
    }
    if (srcRf & (FLD_RF_IRQ_RX_TIMEOUT | FLD_RF_IRQ_FIRST_TIMEOUT)) {
        rf_irq_clr_src(FLD_RF_IRQ_RX_TIMEOUT | FLD_RF_IRQ_FIRST_TIMEOUT);
    }

    // Unused sources the SDK enables, cleared with one masked write per register (both registers are
    // write-1-to-clear, with the timers at bits 0 and 1 of each). Kept small: .ram_code past a 256-byte
    // boundary costs 256 bytes of SRAM (boot_8258_ap.link, _ramcode_tail_).
    const uint32_t unused = src & (FLD_IRQ_SYSTEM_TIMER | FLD_IRQ_TMR0_EN | FLD_IRQ_TMR1_EN);
    if (unused) {
        reg_irq_src = unused;
        reg_tmr_sta = (uint8_t)(unused & (FLD_TMR_STA_TMR0 | FLD_TMR_STA_TMR1));
    }
}
typedef char halTimerBitsMatch[((uint32_t)FLD_IRQ_TMR0_EN == FLD_TMR_STA_TMR0 && (uint32_t)FLD_IRQ_TMR1_EN == FLD_TMR_STA_TMR1) ? 1 : -1];

// --- stack high-water ---------------------------------------------------------------------------
// Both stacks are sized at 4/3 of a static bound (tools/stackdepth.py; boot_8258_ap.link); these
// measure the real use, counting words still 0xFFFFFFFF as unused. The main stack is filled by
// cstartup_8258.S (FLL_STK_EN) from the end of .bss up; the cache arena below it is written from
// boot, so the scan starts at _cache_arena_end_, the stack's floor. The interrupt stack is in .bss and
// zeroed, so irqStackFill() fills it before drv_enable_irq(); interrupts do not nest, so it is all
// idle while main() runs and the scan is safe.
#define STACK_FILL 0xFFFFFFFFUL
extern uint32_t _cache_arena_end_[], _stack_end_[];  // boot_8258_ap.link and cstartup_8258.S
extern uint32_t _irq_stk_lo_[], _irq_stk_hi_[];     // boot_8258_ap.link, around irq_stk

static void irqStackFill(void) {
    for (volatile uint32_t *p = _irq_stk_lo_; p < _irq_stk_hi_; p++) *p = STACK_FILL;
}

// Bytes from the first word that is not the fill, scanning up from the far end, to the top
static uint16_t stackHigh(const uint32_t *lo, const uint32_t *hi) {
    const volatile uint32_t *p = lo;
    while (p < hi && *p == STACK_FILL) p++;
    return (uint16_t)((uintptr_t)hi - (uintptr_t)p);
}

uint16_t halStackHigh(void) { return stackHigh(_cache_arena_end_, _stack_end_); }
uint16_t halIrqStackHigh(void) { return stackHigh(_irq_stk_lo_, _irq_stk_hi_); }
uint16_t halStackSize(void) { return (uint16_t)((uintptr_t)_stack_end_ - (uintptr_t)_cache_arena_end_); }
uint16_t halIrqStackSize(void) { return (uint16_t)((uintptr_t)_irq_stk_hi_ - (uintptr_t)_irq_stk_lo_); }

// --- the arena ----------------------------------------------------------------------------------
// Placed by the link from the end of everything else in RAM, 8-aligned, to the main stack's floor
// (boot_8258_ap.link); the cache derives its maxima from its size (blockcache.c, cacheLayout).
extern uint8_t _cache_arena_start_[];
uint8_t *halArena(uint32_t *bytes) {
    *bytes = (uint32_t)((uintptr_t)_cache_arena_end_ - (uintptr_t)_cache_arena_start_);
    return _cache_arena_start_;
}

// --- start --------------------------------------------------------------------------------------

void halBoardInit(void) {
    resetWdStatusCapture();  // before drv_wd_clear() clears the watchdog bit
    irqStackFill();          // before the first interrupt

    // the SDK's platform init: clocks, calibration, flash voltage, radio driver
    drv_platform_init();
    resetReasonDecide();

    gpio_set_func(STATUS_PIN, AS_GPIO);
    gpio_write(STATUS_PIN, 1);
    gpio_set_output_en(STATUS_PIN, 1);
    gpio_set_input_en(STATUS_PIN, 0);
    gpio_setup_up_down_resistor(STATUS_PIN, PM_PIN_PULLDOWN_100K);
#ifdef ACTIVITY_LED_PIN
    gpio_set_func(ACTIVITY_LED_PIN, AS_GPIO);
    gpio_write(ACTIVITY_LED_PIN, 0);
    gpio_set_output_en(ACTIVITY_LED_PIN, 1);
    gpio_set_input_en(ACTIVITY_LED_PIN, 0);
#endif

    msTick = clock_time();
    uartSetup(UART_BOOT_BAUD, false);
    drv_enable_irq();

    drv_wd_setInterval(10000);
    drv_wd_start();
    drv_wd_clear();
}

void halBoardPoll(void) {
    ledPoll();
    uartTxPoll();
}

// --- the SDK's software timer list, which this firmware does not have -----------------------------
// proj/os/ev_timer.c is left out of the build: its 684-byte `ev_timer` pool is never used, and as a
// COMMON symbol --gc-sections cannot drop it. drv_platform_init() still reaches the two functions
// below (via drv_pm_wakeupTimeUpdate), so they are empty stubs; the real ones do nothing on a list
// that was never initialised. `tc32-elf-nm` shows nothing that adds a timer or sleeps (ev_timer_add,
// ev_timer_taskPost, drv_pm_sleep, cpu_sleep_wakeup) in the image. ev_timer_nearestGet and
// ev_timer_process are left undefined on purpose, so code that needs the timer list fails to link.
// Adding an SDK timer or a sleep mode means putting ev_timer.c back in the makefile.
void ev_timer_update(unsigned int updateTime) { (void)updateTime; }
void ev_timer_setPrevSysTick(unsigned int tick) { (void)tick; }
