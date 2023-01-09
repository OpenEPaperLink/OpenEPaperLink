#include "radio.h"

#include "asmUtil.h"
#include "board.h"
#include "cpu.h"
#include "printf.h"
#include "timer.h"

#define RX_BUFFER_SIZE (RADIO_MAX_PACKET_LEN + 1 /* len byte */ + 2 /* RSSI & LQI */)
#define RX_BUFFER_NUM 3

static volatile uint8_t __xdata mRxBufs[RX_BUFFER_NUM][RX_BUFFER_SIZE];
static volatile uint8_t __xdata mLastRSSI, mLastTxedSeq, mRxOn, mRxBufNextR, mRxBufNextW, mRxBufNumFree;
static volatile __bit mAckTimePassed, mGotAck;

// some things look like: https://www.ti.com/lit/ds/symlink/cc2430.pdf
// maybe a licensed and heavily modified version?

// maybe rx on and tx cal?
// see segmented_ota.<<code:00006D7B radioRxEnable>>

void RF_IRQ1(void) __interrupt(4) {
    uint8_t cause = RADIO_IRQ4_pending;
    static uint8_t __xdata lastRSSI;

    RADIO_IRQ4_pending = 0;

    if (cause & 0x40) {  // ack time happened

        // radio will report ACK if we (1) got an ack or (2) sent a packet that did not require it
        mAckTimePassed = true;
        mGotAck = !!(cause & 0x10);

    }
    if (cause & 0x20) {  // radio has RXed a packet into its internal buffer. vet it quickly and set up DMA

        uint8_t len = RADIO_GOTLEN;

        if (len < 3 || len >= 0x80 || RADIO_rxFirstByte + 1 != len || !mRxOn || !mRxBufNumFree) {  // detect invalid packets right away, or RX being off

            RADIO_command = RADIO_CMD_FLUSH_RX_FIFO;
        } else {
            uint8_t __xdata *buf = mRxBufs[mRxBufNextW];
            uint8_t bkp;

            buf[0] = len;

            bkp = CFGPAGE;
            CFGPAGE = 4;
            RADIO_RXLEN = len - 1;
            RADIO_RXPTRL = ((uint16_t)(buf + 1)) & 0xff;
            RADIO_RXPTRH = ((uint16_t)(buf + 1)) >> 8;
            TRIGGER |= 4;  // start rx dma

            TCON2 &= ~4;  // without this we trigger next irq too fast and get garbage

            CFGPAGE = bkp;
        }
    }
    /*
    if ((cause & 0x10) && !(RADIO_curRfState & 0x20)) {  // radio got a valid preamble and is RXing a packet. this is our chance to sample some RSSI
        uint8_t i;

        // we get here if radio is RXing a packet - tells us to capture some RSSI vals.
        // Seems there is an offset. Value is signed and offset by 56
        for (i = 0; i < 0x3c; i++)
            mLastRSSI = RADIO_currentRSSI;
    }
    */
}

void RF_IRQ2(void) __interrupt(5) {
    uint8_t bck = CFGPAGE;
    CFGPAGE = 4;

    if (TCON2 & 0x04) {  // RX dma over - time to check packet for valid CRC

        uint8_t __xdata *buf = mRxBufs[mRxBufNextW];
        uint8_t len;

        TCON2 &= ~0x04;
        RADIO_command = RADIO_CMD_FLUSH_RX_FIFO;

        // last byte we got DMAed to us has top bit as flags for "crc ok"

        len = buf[0];

        if (!(buf[len] & 0x80)) {
            // CRC failed on packet
        } else {
            buf[len - 1] = mLastRSSI - 56;

            if (++mRxBufNextW == RX_BUFFER_NUM)
                mRxBufNextW = 0;
            mRxBufNumFree--;
        }
    }
    if (TCON2 & 0x02) {  // TX DMA completed
        TCON2 &= ~2;

        // nothing to do here
    }
    if (TCON2 & 0x08) {  // radio init over
        TCON2 &= ~0x48;
        SETTINGS &= ~0x10;
    }
    CFGPAGE = bck;
}

bool radioTx(const void __xdata *packetP)  // waits for tx end
{
    const uint8_t __xdata *packet = (const uint8_t __xdata *)packetP;
    uint16_t bkp, wait;
    __bit irqs;

    // this is how to do CCA. we do not bother because fuck it
    // this is how WE do CCA. 'Fuck it' still somewhat applies if we don't get a clear channel in a reasonable amount of time
    //  okay fuck it.
    /*
    for (uint8_t i = 0; i < 0x80; i++) {
            if (!(RADIO_curRfState & 1)) {
                    //pr("radio CCA fail\n");
                    timerDelay(TIMER_TICKS_PER_SECOND / 2000);
                    //return;
            }
    }
    */

    mAckTimePassed = false;
    mGotAck = false;
    mLastTxedSeq = packet[3];

    bkp = CFGPAGE;
    CFGPAGE = 4;

    irqs = IEN_EA;
    IEN_EA = 0;

    RADIO_TXLEN = packet[0] - 2;
    RADIO_TXPTRL = ((uint16_t)packet) & 0xff;
    RADIO_TXPTRH = ((uint16_t)packet) >> 8;
    RADIO_command = RADIO_CMD_LOAD_TX_FIFO;
    TRIGGER |= 2;  // start TX fifo DMA

    IEN_EA = irqs;

    CFGPAGE = bkp;

    //RADIO_unk_C8 = 0xff;  /// stock fw does this but seems unnecessary

    // wait for tx to start
    wait = 0;
	wait--;
    do {
        if (RADIO_curRfState & 0x80)
            break;
    } while (--wait);

    // wait for tx to end
    if (wait) {
        while (!mAckTimePassed)
            ;
			return true;
    } else {
		return false;
	}

    //RADIO_unk_C8 = 0x7f;  /// stock fw does this but seems unnecessary
}

void radioRxAckReset(void) {
    mGotAck = false;
}

int16_t radioRxAckGetLast(void) {
    if (mGotAck)
        return (uint16_t)mLastTxedSeq;
    else
        return -1;
}

void radioRxFilterCfg(const uint8_t __xdata *filterForLong, uint32_t myShortMac, uint16_t myPan) {
    uint16_t shortMac = (myShortMac == SHORT_MAC_UNUSED) ? 0xffff : myShortMac;
    uint8_t i;

    RADIO_PANID_Hi = myPan >> 8;
    RADIO_PANID_Lo = myPan;

    RADIO_ownShortAddress_Hi = shortMac >> 8;
    RADIO_ownShortAddress_Lo = shortMac;

    for (i = 0; i < 8; i++)
        ((volatile uint8_t __xdata *)&RADIO_ownMac_7)[i] = filterForLong[(uint8_t)((uint8_t)7 - (uint8_t)i)];
}

void radioRxEnable(__bit on, __bit autoAck) {
    if (!autoAck) {
        pr("auto ack forced for now\n");
        while (1)
            ;
    }
    mRxOn = on;
}

void radioSetTxPower(int8_t dBm) {
    if (dBm < -27)
        dBm = -27;
    else if (dBm > 8)
        dBm = 8;

    dBm += 27;

    RADIO_txPower = (uint8_t)(((uint8_t)dBm) + 3) / 5;
}

void radioSetChannel(uint8_t ch) {
    static const uint8_t perChannelSetting1[] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x22, 0x22, 0x22, 0x22, 0x33, 0x33, 0x33, 0x33, 0x33};
    static const uint8_t perChannelSetting2[] = {4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 2, 2, 2};

    if (ch < RADIO_FIRST_CHANNEL || ch >= RADIO_FIRST_CHANNEL + RADIO_NUM_CHANNELS)
        return;

    RADIO_channel = ch;  // configmed to be at least RX channel
    RADIO_command = RADIO_CMD_RECEIVE;
    RADIO_perChannelSetting1 = perChannelSetting1[ch - 11];
    RADIO_perChannelSetting2 = perChannelSetting2[ch - 11];
}

void radioRxFlush(void) {
    mRxBufNumFree = RX_BUFFER_NUM;
}

int8_t radioRxDequeuePktGet(const void __xdata *__xdata *dstBufP, uint8_t __xdata *lqiP, int8_t __xdata *rssiP) {
    const uint8_t __xdata *buf = mRxBufs[mRxBufNextR];
    uint8_t lqi, len = buf[0];

    if (mRxBufNumFree == RX_BUFFER_NUM)
        return -1;

    lqi = (buf[len] & 0x7f);

    *lqiP = lqi;
    *rssiP = buf[len - 1];
    *dstBufP = buf + 1;

    return len - 2;
}

void radioRxDequeuedPktRelease(void) {
    if (++mRxBufNextR == RX_BUFFER_NUM)
        mRxBufNextR = 0;

    __critical {
        mRxBufNumFree++;
    }
}

void radioInit(void) {
    uint8_t bkp;

    mRxBufNextW = 0;
    mRxBufNextR = 0;
    mRxBufNumFree = RX_BUFFER_NUM;

    timerDelay(TIMER_TICKS_PER_SECOND / 1000);
    RADIO_unk_F0 |= 0x80;
    timerDelay(TIMER_TICKS_PER_SECOND / 1000);
    CFGPAGE = 0;
    RESET &= ~4;
    RESET |= 4;
    RESET &= ~4;
    timerDelay(TIMER_TICKS_PER_SECOND / 10000);

    RADIO_RadioPowerCtl |= 4;

    bkp = CFGPAGE;
    CFGPAGE = 4;
    SETTINGS |= 2;
    RADIO_INITSEQ0 = 2;
    RADIO_INITSEQ1 = 0xFA;
    RADIO_INITSEQ2 = 0xDD;
    SETTINGS |= 4;
    RADIO_INITSEQ3 = 1;
    RADIO_INITSEQ4 = 0xFA;
    RADIO_INITSEQ5 = 0xDD;
    IEN_RF2 = 1;
    CFGPAGE = bkp;

    RADIO_command = 0xC1;

    RADIO_unk_C1 = 0x02;
    RADIO_calibration_C2 = 0xf7;  // mdmctrl0L?
    RADIO_calibration_C3 = 0x05;
    RADIO_calibration_C4 = 0x35;
    RADIO_calibration_C5 = 0x24;
    RADIO_calibration_C6 = 0x33;
    RADIO_calibration_C7 = 0x70;
    RADIO_unk_CA = 0x58;
    RADIO_perChannelSetting2 = 0x02;
    RADIO_unk_CD = (RADIO_unk_CD & ~7) | (0x11 & 7);
    RADIO_txPower = 0;
    RADIO_calibration_CF = 0x30;
    RADIO_calibration_D0 = 0x00;
    RADIO_calibration_D1 = 0x49;
    RADIO_calibration_D2 = 0x06;
    RADIO_unk_D7 = 0x43;
    RADIO_unk_E2 = 0x08;  // setting bit 0x40 breaks rx entirely
    RADIO_unk_83 = 0xe5;  // maybe sync word?

    RADIO_unk_C8 = 0x7f;          // setting bit 0x80 breaks rx entirely
    RADIO_calibration_81 = 0xf0;  // removing bit 0x20 breask rx and tx
    RADIO_FLAGS |= 0x08;
    RADIO_unk_D8 = 0;
    RADIO_calibration_9D = 0x3f;  // firts untpouched calib val
    RADIO_calibration_A1 = 0x04;
    RADIO_calibration_94 = 0x7f;  // IOCFG0 ?
    RADIO_unk_CE = 0x55;
    RADIO_calibration_D3 = 0x30;
    RADIO_calibration_D4 = 0xcc;
    RADIO_calibration_D5 = 0xf6;
    RADIO_calibration_86 = 0x40;  // setting this to values like 0x20, 0x80, and 0x60 breaks auto-ack and maybe tx
    RADIO_calibration_95 = 0x08;
    RADIO_calibration_96 = 0xd3;
    RADIO_calibration_70 = 0x01;
    RADIO_calibration_71 = 0x40;

    RADIO_unk_A6 = 0x00;
    RADIO_command = 0xC8;
    RADIO_command = 0xC7;
    RADIO_command = 0xC6;
    RADIO_unk_AF = 0x60;  // int enable (we need 0x60)

    RADIO_unk_AF |= 0x10;

    IEN_RF1 = 1;

    RADIO_FLAGS |= 8;
}
