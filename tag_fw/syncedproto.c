#define __packed
#include "syncedproto.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "adc.h"
#include "asmUtil.h"
#include "board.h"
#include "chars.h"
#include "comms.h"
#include "cpu.h"
#include "drawing.h"
#include "eeprom.h"
#include "i2c.h"
#include "printf.h"
#include "proto.h"
#include "radio.h"
#include "screen.h"
#include "sleep.h"
#include "timer.h"
#include "wdt.h"

struct MacFrameFromMaster {
    struct MacFcs fcs;
    uint8_t seq;
    uint16_t pan;
    uint8_t dst[8];
    uint16_t from;
} __packed;

struct MacFrameNormal {
    struct MacFcs fcs;
    uint8_t seq;
    uint16_t pan;
    uint8_t dst[8];
    uint8_t src[8];
} __packed;

struct MacFrameBcast {
    struct MacFcs fcs;
    uint8_t seq;
    uint16_t dstPan;
    uint16_t dstAddr;
    uint16_t srcPan;
    uint8_t src[8];
} __packed;

#define PKT_TIMING_REQ 0xE0
#define PKT_TIMING_RESPONSE 0xE1
#define PKT_AVAIL_DATA_REQ 0xE5
#define PKT_AVAIL_DATA_INFO 0xE6
#define PKT_BLOCK_PARTIAL_REQUEST 0xE7
#define PKT_BLOCK_REQUEST_ACK 0xE9
#define PKT_BLOCK_REQUEST 0xE4
#define PKT_BLOCK_PART 0xE8
#define PKT_XFER_COMPLETE 0xEA
#define PKT_XFER_COMPLETE_ACK 0xEB
#define PKT_SYNC_BURST 0xEF

struct timingResponse {
    uint8_t checksum;
    uint32_t burstInterval;           // time between burst-start
    uint8_t burstLength;              // in packets; due to use of sequence field, limited to a 256-packet burst
    uint16_t burstLengthMs;           // burst length in ms
    uint32_t timerValue;              // current timer value (used to sync up other RC timers/oscillators)
    uint32_t burstIntervalRemaining;  // time until the next sync burst
    uint8_t dataAvailable;
} __packed;

struct AvailDataReq {
    uint8_t checksum;
    uint8_t lastPacketLQI;  // zero if not reported/not supported to be reported
    int8_t lastPacketRSSI;  // zero if not reported/not supported to be reported
    uint8_t temperature;    // zero if not reported/not supported to be reported. else, this minus CHECKIN_TEMP_OFFSET is temp in degrees C
    uint16_t batteryMv;
    uint8_t softVer;
    uint8_t hwType;
    uint8_t protoVer;
} __packed;

#define DATATYPE_NOUPDATE 0
#define DATATYPE_IMG 1
#define DATATYPE_IMGRAW 2
#define DATATYPE_UPDATE 3

struct AvailDataInfo {
    uint8_t checksum;
    uint64_t dataVer;
    uint32_t dataSize;
    uint8_t dataType;
} __packed;

struct blockPart {
    uint8_t checksum;
    uint8_t blockId;
    uint8_t blockPart;
    uint8_t data[];
} __packed;

struct blockData {
    uint16_t size;
    uint16_t checksum;
    uint8_t data[];
} __packed;

struct burstMacData {
    uint16_t offset;
    uint8_t targetMac[8];
} __packed;

#define BLOCK_PART_DATA_SIZE 99
#define BLOCK_MAX_PARTS 42
#define BLOCK_DATA_SIZE 4096
#define BLOCK_XFER_BUFFER_SIZE BLOCK_DATA_SIZE + sizeof(struct blockData)
#define BLOCK_REQ_PARTS_BYTES 6

struct blockRequest {
    uint8_t checksum;
    uint64_t ver;
    uint8_t blockId;
    uint8_t type;
    uint8_t requestedParts[BLOCK_REQ_PARTS_BYTES];
} __packed;

struct blockRequestAck {
    uint8_t checksum;
    uint16_t pleaseWaitMs;
    uint8_t cancelXfer;
} __packed;

#define TIMER_TICKS_PER_MS 1333UL
#define RX_WINDOW_SIZE 10UL  // ms
// #define DEBUGBLOCKS

// download-stuff
bool __xdata dataPending = true;
uint8_t __xdata blockXferBuffer[BLOCK_XFER_BUFFER_SIZE] = {0};
struct blockRequest __xdata curBlock = {0};
struct AvailDataInfo __xdata curDataInfo = {0};
uint16_t __xdata dataRemaining = 0;
bool __xdata curXferComplete = false;
bool __xdata requestPartialBlock = false;
uint8_t __xdata curImgSlot = 0;
uint32_t __xdata curHighSlotId = 0;
uint8_t __xdata nextImgSlot = 0;
uint8_t __xdata imgSlots = 0;

// stuff we need to keep track of because of the network
uint8_t __xdata APmac[8] = {0};
uint16_t __xdata APsrcPan = 0;
uint8_t __xdata mSelfMac[8] = {0};
uint8_t __xdata seq = 0;

// timing stuff
bool __xdata isSynced = false;  // are we synced to the network?
#define SYNC_CALIB_TIME 10000UL
#define MAX_NEXT_ATTEMPT_DELAY_MS 300000UL
#define MIN_NEXT_ATTEMPT_DELAY_MS 1000UL   // wait at least this time
uint32_t __xdata nextAttempt = 1000;       // delay before next attempt;
bool __xdata isCalibrated = false;         // is the RC oscillator calibrated against the AP?
uint16_t __xdata calib = SYNC_CALIB_TIME;  // RC oscillator calibration value
uint32_t __xdata APburstInterval = 0;
int8_t __xdata calibVector = 0;
uint16_t __xdata APburstLengthMs = 0;  // burst length in ms
uint8_t __xdata APburstLength = 0;

// buffer we use to prepare packets
// static uint8_t __xdata mRxBuf[130];
static uint8_t __xdata inBuffer[128] = {0};
static uint8_t __xdata outBuffer[128] = {0};

// tools
uint8_t __xdata getPacketType(void *__xdata buffer) {
    struct MacFcs *__xdata fcs = buffer;
    if ((fcs->frameType == 1) && (fcs->destAddrType == 2) && (fcs->srcAddrType == 3) && (fcs->panIdCompressed == 0)) {
        // broadcast frame
        uint8_t __xdata type = ((uint8_t *)buffer)[sizeof(struct MacFrameBcast)];
        return type;
    } else if ((fcs->frameType == 1) && (fcs->destAddrType == 3) && (fcs->srcAddrType == 3) && (fcs->panIdCompressed == 1)) {
        // normal frame
        uint8_t __xdata type = ((uint8_t *)buffer)[sizeof(struct MacFrameNormal)];
        return type;
    }
    return 0;
}
uint32_t __xdata getNextAttemptDelay() {
    nextAttempt *= 2;
    if (nextAttempt > MAX_NEXT_ATTEMPT_DELAY_MS) {
        nextAttempt = MAX_NEXT_ATTEMPT_DELAY_MS;
    }
    return nextAttempt;
}
void dump(uint8_t *__xdata a, uint16_t __xdata l) {
    pr("\n        ");
#define ROWS 16
    for (uint8_t c = 0; c < ROWS; c++) {
        pr(" %02X", c);
    }
    pr("\n--------");
    for (uint8_t c = 0; c < ROWS; c++) {
        pr("---");
    }
    for (uint16_t c = 0; c < l; c++) {
        if ((c % ROWS) == 0) {
            pr("\n0x%04X | ", c);
        }
        pr("%02X ", a[c]);
    }
    pr("\n--------");
    for (uint8_t c = 0; c < ROWS; c++) {
        pr("---");
    }
    pr("\n");
}
bool checkCRC(void *p, uint8_t len) {
    uint8_t total = 0;
    for (uint8_t c = 1; c < len; c++) {
        total += ((uint8_t *)p)[c];
    }
    // pr("CRC: rx %d, calc %d\n", ((uint8_t *)p)[0], total);
    return ((uint8_t *)p)[0] == total;
}
void addCRC(void *p, uint8_t len) {
    uint8_t total = 0;
    for (uint8_t c = 1; c < len; c++) {
        total += ((uint8_t *)p)[c];
    }
    ((uint8_t *)p)[0] = total;
}

// init/sleep
void initRadio() {
    radioInit();
    radioRxFilterCfg(mSelfMac, 0x10000, PROTO_PAN_ID);
    radioSetChannel(RADIO_FIRST_CHANNEL);
    radioSetTxPower(10);
}
void killRadio() {
    radioRxEnable(false, true);
    RADIO_IRQ4_pending = 0;
    UNK_C1 &= ~0x81;
    TCON &= ~0x20;
    uint8_t __xdata cfgPg = CFGPAGE;
    CFGPAGE = 4;
    RADIO_command = 0xCA;
    RADIO_command = 0xC5;
    CFGPAGE = cfgPg;
}
void initAfterWake() {
    clockingAndIntsInit();
    timerInit();
    // partialInit();
    boardInit();
    screenSleep();
    irqsOn();
    boardInitStage2();
    initRadio();
}
void doSleep(uint32_t __xdata t) {
    powerPortsDownForSleep();
    // sleepy
    sleepForMsec(t * SYNC_CALIB_TIME / (uint32_t)calib);
    initAfterWake();
}

// timing
void sendTimingInfoReq() {
    struct MacFrameBcast __xdata *txframe = (struct MacFrameBcast *)outBuffer;
    memset(txframe, 0, sizeof(struct MacFrameBcast) + 2);
    memcpy(txframe->src, mSelfMac, 8);
    txframe->fcs.frameType = 1;
    txframe->fcs.ackReqd = 1;
    txframe->fcs.destAddrType = 2;
    txframe->fcs.srcAddrType = 3;
    txframe->seq = seq++;
    txframe->dstPan = 0xFFFF;
    txframe->dstAddr = 0xFFFF;
    txframe->srcPan = 0x4447;
    outBuffer[sizeof(struct MacFrameBcast)] = PKT_TIMING_REQ;
    commsTxUnencrypted(outBuffer, sizeof(struct MacFrameBcast) + 1);
}
const struct timingResponse *__xdata getTimingInfo() {
    radioRxFlush();
    uint32_t __xdata mTimerWaitStart;
    radioRxEnable(true, true);
    for (uint8_t c = 0; c < 5; c++) {
        sendTimingInfoReq();
        mTimerWaitStart = timerGet() + TIMER_TICKS_PER_MS * 4;
        while (timerGet() < mTimerWaitStart) {
            int8_t ret = commsRxUnencrypted(inBuffer);
            if (ret > 1) {
                // radioRxEnable(false, true);
                // radioRxFlush();
                if (getPacketType(inBuffer) == PKT_TIMING_RESPONSE) {
                    struct timingResponse __xdata *tresponse = (struct timingResponse *)(inBuffer + sizeof(struct MacFrameNormal) + 1);
                    if (!checkCRC(tresponse, sizeof(struct timingResponse))) {
                        pr("Invalid CRC in timing info!\n");
                        dump((uint8_t *)tresponse, 16);
                        return NULL;
                    } else {
                        APburstInterval = tresponse->burstInterval;  // time between burst-start
                        APburstLength = tresponse->burstLength;      // in packets; due to use of sequence field, limited to a 256-packet burst
                        APburstLengthMs = tresponse->burstLengthMs;
                        return tresponse;
                    }
                } else {
                    // pr("wrong packet type in getTimingInfo! %02X\n", getPacketType(inBuffer));
                    // return NULL;
                }
            }
        }
    }
    radioRxEnable(false, true);
    radioRxFlush();
    return NULL;
}
const struct timingResponse *__xdata getCalibration() {
    calib = SYNC_CALIB_TIME;
    const struct timingResponse *__xdata r = getTimingInfo();
    if (r == NULL) {
        return NULL;
    }
    uint32_t __xdata timerBegin = r->timerValue;
    doSleep(SYNC_CALIB_TIME);
    r = getTimingInfo();
    if (r != NULL) {
        timerBegin = r->timerValue - timerBegin;
        timerBegin *= 3UL;
        timerBegin /= 4000UL;
        calib = timerBegin;
    } else {
        return NULL;
    }
    return r;
}
uint32_t __xdata doResync() {
    // resync to the network; try to find out when the next sync burst is going to be
    const struct timingResponse *__xdata r = getTimingInfo();
    killRadio();
    uint32_t __xdata sleep;
    if (r == NULL) {
        // we didn't get anything from the AP
        sleep = getNextAttemptDelay();
        pr("NO_RESYNC, sleeping for %lu ms\n", sleep);
    } else {
        sleep = r->burstIntervalRemaining / TIMER_TICKS_PER_MS;
        pr("RESYNC, sleeping for %lu ms\n", sleep);
        isSynced = true;
        nextAttempt = MIN_NEXT_ATTEMPT_DELAY_MS;
    }
    sleep += APburstLengthMs / 2;
    return sleep;
}
struct MacFrameBcast *__xdata getSyncFrame() {
    uint32_t __xdata t;
    radioRxEnable(true, true);
    t = timerGet() + (TIMER_TICKS_PER_MS * RX_WINDOW_SIZE);
    do {
        int8_t __xdata ret = commsRxUnencrypted(inBuffer);
        if (ret > 1) {
            killRadio();
            return (struct MacFrameBcast *)inBuffer;
        }
    } while (timerGet() < t);
    return NULL;
}

// data xfer stuff
void sendAvailDataReq() {
    memset(outBuffer, 0, sizeof(struct MacFrameNormal) + sizeof(struct AvailDataReq) + 2 + 4);
    struct MacFrameNormal *__xdata f = (struct MacFrameNormal *)(outBuffer + 1);
    struct AvailDataReq *__xdata availreq = (struct AvailDataReq *)(outBuffer + 2 + sizeof(struct MacFrameNormal));
    outBuffer[0] = sizeof(struct MacFrameNormal) + sizeof(struct AvailDataReq) + 2 + 2;
    outBuffer[sizeof(struct MacFrameNormal) + 1] = PKT_AVAIL_DATA_REQ;
    memcpy(f->src, mSelfMac, 8);
    memcpy(f->dst, APmac, 8);
    f->fcs.frameType = 1;
    f->fcs.secure = 0;
    f->fcs.framePending = 0;
    f->fcs.ackReqd = 0;
    f->fcs.panIdCompressed = 1;
    f->fcs.destAddrType = 3;
    f->fcs.frameVer = 0;
    f->fcs.srcAddrType = 3;
    f->seq = seq++;
    f->pan = APsrcPan;
    // TODO: send some meaningful data
    availreq->softVer = 1;
    addCRC(availreq, sizeof(struct AvailDataReq));
    commsTxNoCpy(outBuffer);
}
struct AvailDataInfo *__xdata getAvailDataInfo() {
    uint32_t __xdata t;
    for (uint8_t c = 0; c < 15; c++) {
        sendAvailDataReq();
        t = timerGet() + (TIMER_TICKS_PER_MS * 10UL);
        while (timerGet() < t) {
            int8_t __xdata ret = commsRxUnencrypted(inBuffer);
            if (ret > 1) {
                if (getPacketType(inBuffer) == PKT_AVAIL_DATA_INFO) {
                    if (checkCRC(inBuffer + sizeof(struct MacFrameNormal) + 1, sizeof(struct AvailDataInfo))) {
                        return (struct AvailDataInfo *)(inBuffer + sizeof(struct MacFrameNormal) + 1);
                    }
                }
            }
        }
    }
    return NULL;
}
void processBlockPart(struct blockPart *bp) {
    uint16_t __xdata start = bp->blockPart * BLOCK_PART_DATA_SIZE;
    uint16_t __xdata size = BLOCK_PART_DATA_SIZE;
    // validate if it's okay to copy data
    if (bp->blockId != curBlock.blockId) {
        // pr("got a packet for block %02X\n", bp->blockId);
        return;
    }
    if (start >= (sizeof(blockXferBuffer) - 1)) return;
    if (bp->blockPart > BLOCK_MAX_PARTS) return;
    if ((start + size) > sizeof(blockXferBuffer)) {
        size = sizeof(blockXferBuffer) - start;
    }
    if (checkCRC(bp, sizeof(struct blockPart) + BLOCK_PART_DATA_SIZE)) {
        //  copy block data to buffer
        xMemCopy((void *)(blockXferBuffer + start), (const void *)bp->data, size);
        // we don't need this block anymore, set bit to 0 so we don't request it again
        curBlock.requestedParts[bp->blockPart / 8] &= ~(1 << (bp->blockPart % 8));
    }
}
bool blockRxLoop(uint32_t timeout) {
    uint32_t __xdata t;
    bool success = false;
    radioRxEnable(true, true);
    t = timerGet() + (TIMER_TICKS_PER_MS * (timeout + 20));
    while (timerGet() < t) {
        int8_t __xdata ret = commsRxUnencrypted(inBuffer);
        if (ret > 1) {
            if (getPacketType(inBuffer) == PKT_BLOCK_PART) {
                success = true;
                struct blockPart *bp = (struct blockPart *)(inBuffer + sizeof(struct MacFrameNormal) + 1);
                processBlockPart(bp);
            }
        }
    }
    radioRxEnable(false, true);
    radioRxFlush();
    return success;
}
struct blockRequestAck *__xdata continueToRX() {
    struct blockRequestAck *ack = (struct blockRequestAck *)(inBuffer + sizeof(struct MacFrameNormal) + 1);
    ack->pleaseWaitMs = 0;
    ack->cancelXfer = 0;
    return ack;
}
void sendBlockRequest() {
    memset(outBuffer, 0, sizeof(struct MacFrameNormal) + sizeof(struct blockRequest) + 2 + 2);
    struct MacFrameNormal *__xdata f = (struct MacFrameNormal *)(outBuffer + 1);
    struct blockRequest *__xdata blockreq = (struct blockRequest *)(outBuffer + 2 + sizeof(struct MacFrameNormal));
    outBuffer[0] = sizeof(struct MacFrameNormal) + sizeof(struct blockRequest) + 2 + 2;
    if (requestPartialBlock) {;
        outBuffer[sizeof(struct MacFrameNormal) + 1] = PKT_BLOCK_PARTIAL_REQUEST;
    } else {
        outBuffer[sizeof(struct MacFrameNormal) + 1] = PKT_BLOCK_REQUEST;
    }
    memcpy(f->src, mSelfMac, 8);
    memcpy(f->dst, APmac, 8);
    f->fcs.frameType = 1;
    f->fcs.secure = 0;
    f->fcs.framePending = 0;
    f->fcs.ackReqd = 0;
    f->fcs.panIdCompressed = 1;
    f->fcs.destAddrType = 3;
    f->fcs.frameVer = 0;
    f->fcs.srcAddrType = 3;
    f->seq = seq++;
    f->pan = APsrcPan;
    memcpy(blockreq, &curBlock, sizeof(struct blockRequest));
    // pr("req ver: %02X%02X%02X%02X%02X%02X%02X%02X\n", ((uint8_t*)&blockreq->ver)[0],((uint8_t*)&blockreq->ver)[1],((uint8_t*)&blockreq->ver)[2],((uint8_t*)&blockreq->ver)[3],((uint8_t*)&blockreq->ver)[4],((uint8_t*)&blockreq->ver)[5],((uint8_t*)&blockreq->ver)[6],((uint8_t*)&blockreq->ver)[7]);
    addCRC(blockreq, sizeof(struct blockRequest));
    commsTxNoCpy(outBuffer);
}
struct blockRequestAck *__xdata performBlockRequest() {
    uint32_t __xdata t;
    radioRxEnable(true, true);
    radioRxFlush();
    for (uint8_t c = 0; c < 30; c++) {
        sendBlockRequest();
        t = timerGet() + (TIMER_TICKS_PER_MS * (7UL + c / 10));
        do {
            int8_t __xdata ret = commsRxUnencrypted(inBuffer);
            if (ret > 1) {
                switch (getPacketType(inBuffer)) {
                    case PKT_BLOCK_REQUEST_ACK:
                        if (checkCRC((inBuffer + sizeof(struct MacFrameNormal) + 1), sizeof(struct blockRequestAck)))
                            return (struct blockRequestAck *)(inBuffer + sizeof(struct MacFrameNormal) + 1);
                        break;
                    case PKT_BLOCK_PART:
                        // block already started while we were waiting for a get block reply
                        //pr("!");
                        //processBlockPart((struct blockPart *)(inBuffer + sizeof(struct MacFrameNormal) + 1));
                        return continueToRX();
                        break;
                    default:
                        pr("pkt w/type %02X\n", getPacketType(inBuffer));
                        break;
                }
            }

        } while (timerGet() < t);
    }
    return continueToRX();
    //return NULL;
}
void sendXferCompletePacket() {
    memset(outBuffer, 0, sizeof(struct MacFrameNormal) + 2 + 4);
    struct MacFrameNormal *__xdata f = (struct MacFrameNormal *)(outBuffer + 1);
    outBuffer[0] = sizeof(struct MacFrameNormal) + 2 + 2;
    outBuffer[sizeof(struct MacFrameNormal) + 1] = PKT_XFER_COMPLETE;
    memcpy(f->src, mSelfMac, 8);
    memcpy(f->dst, APmac, 8);
    f->fcs.frameType = 1;
    f->fcs.secure = 0;
    f->fcs.framePending = 0;
    f->fcs.ackReqd = 0;
    f->fcs.panIdCompressed = 1;
    f->fcs.destAddrType = 3;
    f->fcs.frameVer = 0;
    f->fcs.srcAddrType = 3;
    f->pan = APsrcPan;
    f->seq = seq++;
    commsTxNoCpy(outBuffer);
}
void sendXferComplete() {
    radioRxEnable(true, true);

    for (uint8_t c = 0; c < 4; c++) {
        sendXferCompletePacket();
        uint32_t __xdata start = timerGet();
        while ((timerGet() - start) < (TIMER_TICKS_PER_MS * 6UL)) {
            int8_t __xdata ret = commsRxUnencrypted(inBuffer);
            if (ret > 1) {
                if (getPacketType(inBuffer) == PKT_XFER_COMPLETE_ACK) {
                    pr("XFC ACK\n");
                    return;
                }
            }
        }
    }
    pr("XFC NACK!\n");
    return;
}

const uint8_t epd_bitmap_ant[] = {
    0xff, 0xbf, 0xfd, 0xdf, 0xfe, 0xdf, 0xee, 0xdf, 0xee, 0xdf, 0xed, 0xdf, 0xd7, 0xbf, 0xd7, 0xff,
    0xd7, 0xff, 0xbb, 0xff, 0xbb, 0xbb, 0xbb, 0xd7, 0xbb, 0xef, 0x7d, 0xd7, 0x7d, 0xbb, 0x01, 0xff};
void copyImage() {
    uint8_t x_begin = 17;
    uint16_t y_begin = 0;
    uint8_t y_size = 16;
    const uint8_t *p = epd_bitmap_ant;
    pr("begin copy image to epd buffer");
    for (uint16_t y = 0; y < y_size; y++) {
        // moveToXY(x_begin, y_begin + y, false);
        pr("d=%02X,", *p);
        // screenByteRawTx(*p);
        p++;
        // screenByteRawTx(*p);
        p++;
    }
}
void drawPartial() {
    screenSleep();
    screenTxStart(true);
    pr("sending bytes\n");
    for (uint8_t iteration = 0; iteration < SCREEN_DATA_PASSES; iteration++) {
        for (uint16_t y = 0; y < SCREEN_HEIGHT; y++) {
            for (uint8_t x = 0; x < SCREEN_WIDTH / 8; x++) {
                if (iteration == 0) {
                    //  screenByteRawTx(0xFF);
                } else {
                    // screenByteRawTx(0x00);
                }
            }
        }
        screenEndPass();
        pr("pass complete\n");
    }
    copyImage();
    screenTxEnd();
}

// EEprom related stuff
uint32_t getAddressForSlot(uint8_t s) {
    return EEPROM_IMG_START + (EEPROM_IMG_EACH * s);
}
void getNumSlots() {
    uint32_t eeSize = eepromGetSize();
    uint16_t nSlots = mathPrvDiv32x16(eeSize - EEPROM_IMG_START, EEPROM_IMG_EACH >> 8) >> 8;
    if (eeSize < EEPROM_IMG_START || !nSlots) {
        pr("eeprom is too small\n");
        while (1)
            ;
    } else if (nSlots >> 8) {
        pr("eeprom is too big, some will be unused\n");
        imgSlots = 254;
    } else
        imgSlots = nSlots;
}
uint8_t findSlot(uint8_t *__xdata ver) {
    // return 0xFF;  // remove me! This forces the tag to re-download each and every upload without checking if it's already in the eeprom somewhere
    uint32_t __xdata markerValid = EEPROM_IMG_VALID;
    for (uint8_t __xdata c = 0; c < imgSlots; c++) {
        struct EepromImageHeader __xdata *eih = (struct EepromImageHeader __xdata *)mScreenRow;
        eepromRead(getAddressForSlot(c), eih, sizeof(struct EepromImageHeader));
        if (xMemEqual4(&eih->validMarker, &markerValid)) {
            if (xMemEqual(&eih->version, (void *)ver, 8)) {
                return c;
            }
        }
    }
    return 0xFF;
}
void eraseUpdateBlock() {
    eepromErase(EEPROM_UPDATA_AREA_START, EEPROM_UPDATE_AREA_LEN / EEPROM_ERZ_SECTOR_SZ);
}
void saveUpdateBlockData(uint8_t blockId) {
    if (!eepromWrite(EEPROM_UPDATA_AREA_START + (blockId * BLOCK_DATA_SIZE), blockXferBuffer + sizeof(struct blockData), BLOCK_DATA_SIZE))
        pr("EEPROM write failed\n");
}

void saveImgBlockData(uint8_t blockId) {
    uint16_t length = EEPROM_IMG_EACH - (sizeof(struct EepromImageHeader) + (blockId * BLOCK_DATA_SIZE));
    if (length > 4096) length = 4096;

    if (!eepromWrite(getAddressForSlot(curImgSlot) + sizeof(struct EepromImageHeader) + (blockId * BLOCK_DATA_SIZE), blockXferBuffer + sizeof(struct blockData), length))
        pr("EEPROM write failed\n");
}
void drawImageFromEeprom() {
    // enable WDT, to make sure de tag resets if it's for some reason unable to draw the image
    wdtSetResetVal(0xFFFFFFFF - 0x38C340);
    wdtOn();
    drawImageAtAddress(getAddressForSlot(curImgSlot));
    //   adcSampleBattery();
    initRadio();
}
uint32_t getHighSlotId() {
    uint32_t temp = 0;
    uint32_t __xdata markerValid = EEPROM_IMG_VALID;
    for (uint8_t __xdata c = 0; c < imgSlots; c++) {
        struct EepromImageHeader __xdata *eih = (struct EepromImageHeader __xdata *)mScreenRow;
        eepromRead(getAddressForSlot(c), eih, sizeof(struct EepromImageHeader));
        if (xMemEqual4(&eih->validMarker, &markerValid)) {
            if (temp < eih->id) {
                temp = eih->id;
                nextImgSlot = c;
            }
        }
    }
    pr("found high id=%lu in slot %d\n", temp, nextImgSlot);
    return temp;
}

bool validateBlockData() {
    struct blockData *bd = (struct blockData *)blockXferBuffer;
    // pr("expected len = %04X, checksum=%04X\n", bd->size, bd->checksum);
    uint16_t t = 0;
    for (uint16_t c = 0; c < bd->size; c++) {
        t += bd->data[c];
    }
    return bd->checksum == t;
}

#define DEBUGBLOCKS
// Main download function
void doDataDownload() {
    // this is the main function for the download process

    if (!eepromInit()) {  // we'll need the eeprom here, init it.
        pr("failed to init eeprom\n");
        return;
    }

    // GET AVAIL DATA INFO - enable the radio and get data
    radioRxEnable(true, true);
    struct AvailDataInfo *__xdata avail = getAvailDataInfo();
    if (avail == NULL) {  // didn't receive a reply to get info about the data, we'll resync and try again later
#ifdef DEBUGBLOCKS
        pr("didn't receive getavaildatainfo");
#endif
        return;
    }

    // did receive available data info (avail struct)
    uint8_t __xdata partsThisBlock = 0;
    bool __xdata lastBlock = false;

    switch (avail->dataType) {
        case DATATYPE_IMG:
        case DATATYPE_IMGRAW:
            // check if this download is currently displayed or active
            if (curXferComplete && xMemEqual((const void *__xdata) & avail->dataVer, (const void *__xdata) & curDataInfo.dataVer, 8)) {
                // we've downloaded this already, we're guessing it's already displayed
                pr("old ver, already downloaded!\n");
                sendXferComplete();
                return;
            } else {
                // check if we've seen this version before
                curImgSlot = findSlot(&(avail->dataVer));
                if (curImgSlot != 0xFF) {
                    // found a (complete)valid image slot for this version
                    sendXferComplete();
                    killRadio();

                    pr("already seen, drawing from eeprom slot %d\n", curImgSlot);

                    // mark as completed and draw from EEPROM
                    curXferComplete = true;
                    xMemCopyShort(&curDataInfo, (void *)avail, sizeof(struct AvailDataInfo));

                    drawImageFromEeprom();
                    return;
                } else {
                    // not found in cache, prepare to download
                    // go to the next image slot
                    nextImgSlot++;
                    if (nextImgSlot >= imgSlots) nextImgSlot = 0;
                    curImgSlot = nextImgSlot;

                    eepromErase(getAddressForSlot(curImgSlot), EEPROM_IMG_EACH / EEPROM_ERZ_SECTOR_SZ);
                    pr("new download, writing to slot %d\n", curImgSlot);
                }
            }
            break;
        case DATATYPE_UPDATE:
            pr("received firmware!\n");
            eepromErase(EEPROM_UPDATA_AREA_START, EEPROM_UPDATE_AREA_LEN / EEPROM_ERZ_SECTOR_SZ);
            break;
    }

    // prepare for download
    curXferComplete = false;
    curBlock.blockId = 0;
    xMemCopy8(&(curBlock.ver), &(avail->dataVer));
    curBlock.type = avail->dataType;
    xMemCopyShort(&curDataInfo, (void *)avail, sizeof(struct AvailDataInfo));
    dataRemaining = curDataInfo.dataSize;  // this was + 2, and I can't remember why. It works fine without it, so I don't know....

    // set requested parts - check if the transfer is contained in this block
    if (dataRemaining > BLOCK_DATA_SIZE) {
        // full block, not last
        lastBlock = false;
        partsThisBlock = BLOCK_MAX_PARTS;
        memset(curBlock.requestedParts, 0xFF, BLOCK_REQ_PARTS_BYTES);
    } else {
        // final block, probably partial
        lastBlock = true;
        partsThisBlock = dataRemaining / BLOCK_PART_DATA_SIZE;
        if (dataRemaining % BLOCK_PART_DATA_SIZE) partsThisBlock++;
        memset(curBlock.requestedParts, 0x00, BLOCK_REQ_PARTS_BYTES);
        for (uint8_t c = 0; c < partsThisBlock; c++) {
            curBlock.requestedParts[c / 8] |= (1 << (c % 8));
        }
    }

    // do transfer!
    uint8_t __xdata blockRequestAttempt = 0;
    while (!curXferComplete) {
        // this while loop loops until the transfer has been completed, or we get tired for other reasons
        blockRequestAttempt = 0;
    startdownload:;
#ifdef DEBUGBLOCKS
        pr("REQ %d[", curBlock.blockId);
        for (uint8_t c = 0; c < BLOCK_MAX_PARTS; c++) {
            if ((c != 0) && (c % 8 == 0)) pr("][");
            if (curBlock.requestedParts[c / 8] & (1 << (c % 8))) {
                pr("R");
            } else {
                pr("_");
            }
        }
        pr("]\n");
#endif

        //timerDelay(TIMER_TICKS_PER_MS*100);

        // DO BLOCK REQUEST - request a block, get an ack with timing info (hopefully)
        struct blockRequestAck *__xdata ack = performBlockRequest();
        if (ack == NULL) {
            pr("no reply on gblockrequest\n");
            // didn't get an ack :( we'll probably try again later
            return;
        } else if (ack->cancelXfer == 1) {
            // we were asked to cancel the transfer by the AP
            pr("transfer cancelled by AP\n");
            return;
        } else {
            // got an ack
        }

        // SLEEP - until the AP is ready with the data
        if (ack->pleaseWaitMs) {
            ack->pleaseWaitMs -= 10;
            if (ack->pleaseWaitMs < 35) {
                timerDelay(ack->pleaseWaitMs * TIMER_TICKS_PER_MS);
            } else {
                doSleep(ack->pleaseWaitMs - 30);
                radioRxEnable(true, true);
            }
        } else {
            // immediately start with the reception of the block data
        }

        // BLOCK RX LOOP - receive a block, until the timeout has passed
        if (!blockRxLoop(440)) { // was 340
            // didn't receive packets
            blockRequestAttempt++;
            if (blockRequestAttempt > 5) {
                pr("bailing on download, 0 blockparts rx'd\n");
                return;
            } else {
                //goto startdownload;
            }
        } else {
            // successfull block RX loop
            blockRequestAttempt = 0;
        }

#ifdef DEBUGBLOCKS
        pr("RX  %d[", curBlock.blockId);
        for (uint8_t c = 0; c < BLOCK_MAX_PARTS; c++) {
            if ((c != 0) && (c % 8 == 0)) pr("][");
            if (curBlock.requestedParts[c / 8] & (1 << (c % 8))) {
                pr(".");
            } else {
                pr("R");
            }
        }
        pr("]\n");
#endif

        // check if we got all the parts we needed, e.g: has the block been completed?
        bool blockComplete = true;
        for (uint8_t c = 0; c < partsThisBlock; c++) {
            if (curBlock.requestedParts[c / 8] & (1 << (c % 8))) blockComplete = false;
        }

        if (blockComplete) {
            if (validateBlockData()) {
                // checked and found okay
                requestPartialBlock = false;
                switch (curBlock.type) {
                    case DATATYPE_IMG:
                    case DATATYPE_IMGRAW:
                        saveImgBlockData(curBlock.blockId);
                        break;
                    case DATATYPE_UPDATE:
                        saveUpdateBlockData(curBlock.blockId);
                        break;
                }
            } else {
                // block checked, but failed validation. Mark all parts for this block as 'request'
                for (uint8_t c = 0; c < partsThisBlock; c++) {
                    curBlock.requestedParts[c / 8] |= (1 << (c % 8));
                }
                blockComplete = false;
                requestPartialBlock = false;
                pr("block failed validation!\n");
            }
        } else {
            // block incomplete, re-request a partial block
            requestPartialBlock = true;
        }

        if (blockComplete) {
            if (!lastBlock) {
                // Not the last block! check what the next block is going to be
                curBlock.blockId++;
                dataRemaining -= BLOCK_DATA_SIZE;
                if (dataRemaining > BLOCK_DATA_SIZE) {
                    // full block-size
                    partsThisBlock = BLOCK_MAX_PARTS;
                    memset(curBlock.requestedParts, 0xFF, BLOCK_REQ_PARTS_BYTES);
                } else {
                    // final block, probably partial
                    partsThisBlock = dataRemaining / BLOCK_PART_DATA_SIZE;
                    if (dataRemaining % BLOCK_PART_DATA_SIZE) partsThisBlock++;
                    memset(curBlock.requestedParts, 0x00, BLOCK_REQ_PARTS_BYTES);
                    for (uint8_t c = 0; c < partsThisBlock; c++) {
                        curBlock.requestedParts[c / 8] |= (1 << (c % 8));
                    }
                    lastBlock = true;
                }
            } else {
                // this was the last block. What should we do next?
                switch (curBlock.type) {
                    case DATATYPE_IMG:
                    case DATATYPE_IMGRAW:;
                        // transfer complete. Save data info and mark data in image slot as 'valid'
                        struct EepromImageHeader __xdata *eih = (struct EepromImageHeader __xdata *)mScreenRow;
                        xMemCopy8(&eih->version, &curDataInfo.dataVer);
                        eih->size = curDataInfo.dataSize;
                        eih->validMarker = EEPROM_IMG_VALID;
                        eih->id = ++curHighSlotId;
                        eepromWrite(getAddressForSlot(curImgSlot), eih, sizeof(struct EepromImageHeader));
                        // pr("transfer complete!");
                        curXferComplete = true;
                        sendXferComplete();
                        killRadio();
                        drawImageFromEeprom();
                        curDataInfo.dataVer = 0xAA;
                        break;
                    case DATATYPE_UPDATE:
                        pr("firmware download complete, doing update.\n");
                        curXferComplete = true;
                        sendXferComplete();
                        killRadio();
                        eepromReadStart(EEPROM_UPDATA_AREA_START);
                        //wdtDeviceReset();
                        selfUpdate();

                        break;
                }
            }
        } else {
            // incomplete block, wrap around and get the rest of the block...
        }
    }  // end download while loop
}

// main loop;
void mainProtocolLoop(void) {
    clockingAndIntsInit();
    timerInit();
    boardInit();

    if (!boardGetOwnMac(mSelfMac)) {
        pr("failed to get MAC. Aborting\n");
        while (1)
            ;
    } else {
        /*
        for (uint8_t c = 0; c < 8; c++) {
            mSelfMac[c] = c + 5;
        }
        */
        // really... if I do the call below, it'll cost me 8 bytes IRAM. Not the kind of 'optimization' I ever dreamed of doing
        // pr("MAC>%02X%02X%02X%02X%02X%02X%02X%02X\n", mSelfMac[0], mSelfMac[1], mSelfMac[2], mSelfMac[3], mSelfMac[4], mSelfMac[5], mSelfMac[6], mSelfMac[7]);
        pr("MAC>%02X%02X", mSelfMac[0], mSelfMac[1]);
        pr("%02X%02X", mSelfMac[2], mSelfMac[3]);
        pr("%02X%02X", mSelfMac[4], mSelfMac[5]);
        pr("%02X%02X\n", mSelfMac[6], mSelfMac[7]);
    }

    irqsOn();
    boardInitStage2();
    // i2ctest();

    pr("BOOTED> (new version!)\n\n");
    isSynced = false;
    isCalibrated = false;

    if (!eepromInit()) {
        pr("failed to init eeprom\n");
        drawFullscreenMsg((const __xdata char *)"eeprom failed");
        while (1)
            ;
    } else {
        getNumSlots();
        curHighSlotId = getHighSlotId();
    }

    screenSleep();
    eepromDeepPowerDown();
    initRadio();
    // drawPartial();
    // i2ctest();

    while (1) {
        uint8_t frameseq = 0;
        dataPending = false;

        // If synced to a network, try to see if we can receive a sync frame;
        if (isSynced) {
            struct MacFrameBcast *__xdata f = getSyncFrame();
            struct burstMacData *__xdata macdata = (struct burstMacData * __xdata)(((uint8_t *)f) + sizeof(struct MacFrameBcast) + 2);
            if (f != NULL) {
                frameseq = f->seq + 1;
                memcpy(APmac, f->src, 8);
                APsrcPan = f->srcPan;
                uint8_t totalmacs = *(((uint8_t *)f) + sizeof(struct MacFrameBcast) + 1);
                for (uint8_t c = 0; c < totalmacs; c++) {
                    if (memcmp(mSelfMac, macdata[c].targetMac, 8) == 0) {
                        // we matched our MAC, prepare to start a download
                        // save AP mac and srcPan in order to send a AvailDataReq packet later on
                        dataPending = true;
                        // sleep until we can start our transaction with the AP
                        doSleep(macdata[c].offset);
                        break;
                    }
                }
            }
        }

        uint32_t __xdata sleep = 0;
        if (frameseq != 0) {
            // received a frame;
            // math here can be optimized heavily!
            int16_t __xdata packet_offset = (frameseq - (APburstLength / 2)) * APburstLengthMs;
            packet_offset /= APburstLength;
            calib += (packet_offset / 12);
            if (dataPending) {
                pr("OK f=%d DL!\n", frameseq);
                doDataDownload();
                eepromDeepPowerDown();
                // wdtDeviceReset();
                isSynced = false;
                radioRxEnable(true, true);  // probably not needed
                sleep = doResync();
            } else {
                sleep = (uint32_t)(APburstInterval - packet_offset);
                pr("OK f=%d\n", frameseq, sleep);
            }

        } else if (!isCalibrated) {
            // uncalibrated; try to get two packages in order to calibrate
            pr("\nDoing calibration... ");
            const struct timingResponse *__xdata r;
            r = getCalibration();
            if (r != NULL) {
                // calibration data received
                pr("OK!\n");
                sleep = r->burstIntervalRemaining / TIMER_TICKS_PER_MS;
                sleep += (APburstLengthMs / 2);
                isSynced = true;
                isCalibrated = true;
                // dataPending = r->dataAvailable;
            } else {
                // calibration failed...
                sleep = getNextAttemptDelay();
                pr("FAILED!\n");
            }
        } else {
            // we didn't receive anything... Ask for the next burst position
            if (isSynced) {
                // we were previously synced, we'll wait until we're 1 second away from (estimated) burst time
                isSynced = false;
                sleep = 2 * APburstInterval - 1000;
                pr("NO JOY! sleeping for %lums\n", sleep);
            } else {
                // check when the next burst location is
                sleep = doResync();
            }
        }
        doSleep(sleep);
    }
}