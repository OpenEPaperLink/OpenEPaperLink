#define __packed

#include "syncedproto.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "asmUtil.h"
#include "comms.h"
#include "cpu.h"
#include "drawing.h"
#include "eeprom.h"
#include "i2cdevices.h"
#include "powermgt.h"
#include "printf.h"
#include "../oepl-definitions.h"
#include "../oepl-proto.h"
#include "radio.h"
#include "screen.h"
#include "settings.h"
#include "sleep.h"
#include "timer.h"
#include "userinterface.h"
#include "wdt.h"

// download-stuff
uint8_t __xdata blockXferBuffer[BLOCK_XFER_BUFFER_SIZE] = {0};
static struct blockRequest __xdata curBlock = {0};  // used by the block-requester, contains the next request that we'll send
static uint8_t __xdata curDispDataVer[8] = {0};
static struct AvailDataInfo __xdata xferDataInfo = {0};  // holds the AvailDataInfo during the transfer
static bool __xdata requestPartialBlock = false;         // if we should ask the AP to get this block from the host or not
#define BLOCK_TRANSFER_ATTEMPTS 5

static uint8_t xferImgSlot = 0xFF;          // holds current transfer slot in progress
uint8_t __xdata curImgSlot = 0xFF;          // currently shown image
static uint32_t __xdata curHighSlotId = 0;  // current highest ID, will be incremented before getting written to a new slot
static uint8_t __xdata nextImgSlot = 0;     // next slot in sequence for writing
static uint8_t __xdata imgSlots = 0;

// stuff we need to keep track of related to the network/AP
uint8_t __xdata APmac[8] = {0};
uint16_t __xdata APsrcPan = 0;
uint8_t __xdata mSelfMac[8] = {0};
static uint8_t __xdata seq = 0;
uint8_t __xdata currentChannel = 0;

// buffer we use to prepare/read packets
static uint8_t __xdata inBuffer[128] = {0};
static uint8_t __xdata outBuffer[128] = {0};

// other stuff we shouldn't have to put here...
static uint32_t __xdata markerValid = EEPROM_IMG_VALID;

extern void executeCommand(uint8_t cmd);  // this is defined in main.c

// tools
static uint8_t __xdata getPacketType(const void *__xdata buffer) {
    const struct MacFcs *__xdata fcs = buffer;
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
static bool pktIsUnicast(const void *__xdata buffer) {
    const struct MacFcs *__xdata fcs = buffer;
    if ((fcs->frameType == 1) && (fcs->destAddrType == 2) && (fcs->srcAddrType == 3) && (fcs->panIdCompressed == 0)) {
        return false;
    } else if ((fcs->frameType == 1) && (fcs->destAddrType == 3) && (fcs->srcAddrType == 3) && (fcs->panIdCompressed == 1)) {
        // normal frame
        return true;
    }
    // unknown type...
    return false;
}
void dump(const uint8_t *__xdata a, const uint16_t __xdata l) {
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
static bool checkCRC(const void *p, const uint8_t len) {
    uint8_t total = 0;
    for (uint8_t c = 1; c < len; c++) {
        total += ((uint8_t *)p)[c];
    }
    // pr("CRC: rx %d, calc %d\n", ((uint8_t *)p)[0], total);
    return ((uint8_t *)p)[0] == total;
}
static void addCRC(void *p, const uint8_t len) {
    uint8_t total = 0;
    for (uint8_t c = 1; c < len; c++) {
        total += ((uint8_t *)p)[c];
    }
    ((uint8_t *)p)[0] = total;
}

// radio stuff
static void sendPing() {
    struct MacFrameBcast __xdata *txframe = (struct MacFrameBcast *)(outBuffer + 1);
    memset(outBuffer, 0, sizeof(struct MacFrameBcast) + 2 + 4);
    outBuffer[0] = sizeof(struct MacFrameBcast) + 1 + 2;
    outBuffer[sizeof(struct MacFrameBcast) + 1] = PKT_PING;
    memcpy(txframe->src, mSelfMac, 8);
    txframe->fcs.frameType = 1;
    txframe->fcs.ackReqd = 1;
    txframe->fcs.destAddrType = 2;
    txframe->fcs.srcAddrType = 3;
    txframe->seq = seq++;
    txframe->dstPan = PROTO_PAN_ID;
    txframe->dstAddr = 0xFFFF;
    txframe->srcPan = PROTO_PAN_ID;
    commsTxNoCpy(outBuffer);
}
uint8_t detectAP(const uint8_t channel) {
    uint32_t __xdata t;
    radioRxEnable(false, true);
    radioSetChannel(channel);
    radioRxFlush();
    radioRxEnable(true, true);
    for (uint8_t c = 1; c <= MAXIMUM_PING_ATTEMPTS; c++) {
        sendPing();
        t = timerGet() + (TIMER_TICKS_PER_MS * PING_REPLY_WINDOW);
        while (timerGet() < t) {
            int8_t __xdata ret = commsRxUnencrypted(inBuffer);
            if (ret > 1) {
                //                dump(inBuffer+sizeof(struct MacFrameNormal),32);
                if ((inBuffer[sizeof(struct MacFrameNormal) + 1] == channel) && (getPacketType(inBuffer) == PKT_PONG)) {
                    if (pktIsUnicast(inBuffer)) {
                        struct MacFrameNormal *__xdata f = (struct MacFrameNormal *)inBuffer;
                        memcpy(APmac, f->src, 8);
                        APsrcPan = f->pan;
                        return c;
                    }
                }
            }
        }
    }
    return 0;
}

// data xfer stuff
static void sendShortAvailDataReq() {
    struct MacFrameBcast __xdata *txframe = (struct MacFrameBcast *)(outBuffer + 1);
    outBuffer[0] = sizeof(struct MacFrameBcast) + 1 + 2;
    outBuffer[sizeof(struct MacFrameBcast) + 1] = PKT_AVAIL_DATA_SHORTREQ;
    memcpy(txframe->src, mSelfMac, 8);
    outBuffer[1] = 0x21;
    outBuffer[2] = 0xC8;  // quickly set txframe fcs structure for broadcast packet
    txframe->seq = seq++;
    txframe->dstPan = PROTO_PAN_ID;
    txframe->dstAddr = 0xFFFF;
    txframe->srcPan = PROTO_PAN_ID;
    commsTxNoCpy(outBuffer);
}
static void sendAvailDataReq() {
    struct MacFrameBcast __xdata *txframe = (struct MacFrameBcast *)(outBuffer + 1);
    memset(outBuffer, 0, sizeof(struct MacFrameBcast) + sizeof(struct AvailDataReq) + 2 + 4);
    struct AvailDataReq *__xdata availreq = (struct AvailDataReq *)(outBuffer + 2 + sizeof(struct MacFrameBcast));
    outBuffer[0] = sizeof(struct MacFrameBcast) + sizeof(struct AvailDataReq) + 2 + 2;
    outBuffer[sizeof(struct MacFrameBcast) + 1] = PKT_AVAIL_DATA_REQ;
    memcpy(txframe->src, mSelfMac, 8);
    txframe->fcs.frameType = 1;
    txframe->fcs.ackReqd = 1;
    txframe->fcs.destAddrType = 2;
    txframe->fcs.srcAddrType = 3;
    txframe->seq = seq++;
    txframe->dstPan = PROTO_PAN_ID;
    txframe->dstAddr = 0xFFFF;
    txframe->srcPan = PROTO_PAN_ID;
    // TODO: send some (more) meaningful data
    availreq->hwType = HW_TYPE;
    availreq->wakeupReason = wakeUpReason;
    availreq->lastPacketRSSI = mLastRSSI;
    availreq->lastPacketLQI = mLastLqi;
    availreq->temperature = temperature;
    availreq->batteryMv = batteryVoltage;
    availreq->capabilities = capabilities;
    availreq->tagSoftwareVersion = fwVersion;
    availreq->currentChannel = currentChannel;
    availreq->customMode = tagSettings.customMode;
    addCRC(availreq, sizeof(struct AvailDataReq));
    commsTxNoCpy(outBuffer);
}
struct AvailDataInfo *__xdata getAvailDataInfo() {
    radioRxEnable(true, true);
    uint32_t __xdata t;
    for (uint8_t c = 0; c < DATA_REQ_MAX_ATTEMPTS; c++) {
        sendAvailDataReq();
        t = timerGet() + (TIMER_TICKS_PER_MS * DATA_REQ_RX_WINDOW_SIZE);
        while (timerGet() < t) {
            int8_t __xdata ret = commsRxUnencrypted(inBuffer);
            if (ret > 1) {
                if (getPacketType(inBuffer) == PKT_AVAIL_DATA_INFO) {
                    if (checkCRC(inBuffer + sizeof(struct MacFrameNormal) + 1, sizeof(struct AvailDataInfo))) {
                        struct MacFrameNormal *__xdata f = (struct MacFrameNormal *)inBuffer;
                        memcpy(APmac, f->src, 8);
                        APsrcPan = f->pan;
                        dataReqLastAttempt = c;
                        return (struct AvailDataInfo *)(inBuffer + sizeof(struct MacFrameNormal) + 1);
                    }
                }
            }
        }
    }
    dataReqLastAttempt = DATA_REQ_MAX_ATTEMPTS;
    return NULL;
}
struct AvailDataInfo *__xdata getShortAvailDataInfo() {
    radioRxEnable(true, true);
    uint32_t __xdata t;
    for (uint8_t c = 0; c < DATA_REQ_MAX_ATTEMPTS; c++) {
        sendShortAvailDataReq();
        // sendAvailDataReq();
        t = timerGet() + (TIMER_TICKS_PER_MS * DATA_REQ_RX_WINDOW_SIZE);
        while (timerGet() < t) {
            int8_t __xdata ret = commsRxUnencrypted(inBuffer);
            if (ret > 1) {
                if (getPacketType(inBuffer) == PKT_AVAIL_DATA_INFO) {
                    if (checkCRC(inBuffer + sizeof(struct MacFrameNormal) + 1, sizeof(struct AvailDataInfo))) {
                        struct MacFrameNormal *__xdata f = (struct MacFrameNormal *)inBuffer;
                        memcpy(APmac, f->src, 8);
                        APsrcPan = f->pan;
                        dataReqLastAttempt = c;
                        return (struct AvailDataInfo *)(inBuffer + sizeof(struct MacFrameNormal) + 1);
                    }
                }
            }
        }
    }
    dataReqLastAttempt = DATA_REQ_MAX_ATTEMPTS;
    return NULL;
}

#ifdef ENABLE_RETURN_DATA
static void sendTagReturnDataPacket(const uint8_t *data, uint8_t len, uint8_t type) {
    struct MacFrameBcast __xdata *txframe = (struct MacFrameBcast *)(outBuffer + 1);
    memset(outBuffer, 0, sizeof(struct MacFrameBcast) + sizeof(struct AvailDataReq) + 2 + 4);
    struct tagReturnData *__xdata trd = (struct tagReturnData *)(outBuffer + 2 + sizeof(struct MacFrameBcast));
    outBuffer[0] = sizeof(struct MacFrameBcast) + 11 + len + 1 + 2;
    outBuffer[sizeof(struct MacFrameBcast) + 1] = PKT_TAG_RETURN_DATA;
    memcpy(txframe->src, mSelfMac, 8);
    txframe->fcs.frameType = 1;
    txframe->fcs.ackReqd = 1;
    txframe->fcs.destAddrType = 2;
    txframe->fcs.srcAddrType = 3;
    txframe->seq = seq++;
    txframe->dstPan = PROTO_PAN_ID;
    txframe->dstAddr = 0xFFFF;
    txframe->srcPan = PROTO_PAN_ID;
    memcpy(trd->data, data, len);
    trd->dataType = type;
    memcpy(&trd->dataVer, mSelfMac, 8);
    memcpy(&trd->dataVer, data, 4);
    addCRC(trd, 11 + len);
    commsTxNoCpy(outBuffer);
}

bool sendTagReturnData(uint8_t __xdata *data, uint8_t len, uint8_t type) {
    radioRxEnable(true, true);
    uint32_t __xdata t;
    for (uint8_t c = 0; c < MAX_RETURN_DATA_ATTEMPTS; c++) {
        sendTagReturnDataPacket(data, len, type);
        t = timerGet() + (TIMER_TICKS_PER_MS * DATA_REQ_RX_WINDOW_SIZE);
        while (timerGet() < t) {
            int8_t __xdata ret = commsRxUnencrypted(inBuffer);
            if (ret > 1) {
                if ((getPacketType(inBuffer) == PKT_TAG_RETURN_DATA_ACK) && (pktIsUnicast(inBuffer))) {
                    return true;
                }
            }
        }
    }
    return false;
}
#endif

static bool processBlockPart(const struct blockPart *bp) {
    uint16_t __xdata start = bp->blockPart * BLOCK_PART_DATA_SIZE;
    uint16_t __xdata size = BLOCK_PART_DATA_SIZE;
    // validate if it's okay to copy data
    if (bp->blockId != curBlock.blockId) {
        // pr("got a packet for block %02X\n", bp->blockId);
        return false;
    }
    if (start >= (sizeof(blockXferBuffer) - 1)) return false;
    if (bp->blockPart > BLOCK_MAX_PARTS) return false;
    if ((start + size) > sizeof(blockXferBuffer)) {
        size = sizeof(blockXferBuffer) - start;
    }
    if (checkCRC(bp, sizeof(struct blockPart) + BLOCK_PART_DATA_SIZE)) {
        //  copy block data to buffer
        xMemCopy((void *)(blockXferBuffer + start), (const void *)bp->data, size);
        // we don't need this block anymore, set bit to 0 so we don't request it again
        curBlock.requestedParts[bp->blockPart / 8] &= ~(1 << (bp->blockPart % 8));
        return true;
    } else {
        return false;
    }
}
static bool blockRxLoop(const uint32_t timeout) {
    uint32_t __xdata t;
    bool success = false;
    radioRxEnable(true, true);
    t = timerGet() + (TIMER_TICKS_PER_MS * (timeout + 20));
    while (timerGet() < t) {
        int8_t __xdata ret = commsRxUnencrypted(inBuffer);
        if (ret > 1) {
            if (getPacketType(inBuffer) == PKT_BLOCK_PART) {
                struct blockPart *bp = (struct blockPart *)(inBuffer + sizeof(struct MacFrameNormal) + 1);
                success = processBlockPart(bp);
            }
        }
    }
    radioRxEnable(false, true);
    radioRxFlush();
    return success;
}
static struct blockRequestAck *__xdata continueToRX() {
    struct blockRequestAck *ack = (struct blockRequestAck *)(inBuffer + sizeof(struct MacFrameNormal) + 1);
    ack->pleaseWaitMs = 0;
    return ack;
}
static void sendBlockRequest() {
    memset(outBuffer, 0, sizeof(struct MacFrameNormal) + sizeof(struct blockRequest) + 2 + 2);
    struct MacFrameNormal *__xdata f = (struct MacFrameNormal *)(outBuffer + 1);
    struct blockRequest *__xdata blockreq = (struct blockRequest *)(outBuffer + 2 + sizeof(struct MacFrameNormal));
    outBuffer[0] = sizeof(struct MacFrameNormal) + sizeof(struct blockRequest) + 2 + 2;
    if (requestPartialBlock) {
        ;
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
static struct blockRequestAck *__xdata performBlockRequest() {
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
                        // pr("!");
                        // processBlockPart((struct blockPart *)(inBuffer + sizeof(struct MacFrameNormal) + 1));
                        return continueToRX();
                        break;
                    case PKT_CANCEL_XFER:
                        return NULL;
                    default:
                        pr("pkt w/type %02X\n", getPacketType(inBuffer));
                        break;
                }
            }

        } while (timerGet() < t);
    }
    return continueToRX();
    // return NULL;
}
static void sendXferCompletePacket() {
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
static void sendXferComplete() {
    radioRxEnable(true, true);

    for (uint8_t c = 0; c < 16; c++) {
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
static bool validateBlockData() {
    struct blockData *bd = (struct blockData *)blockXferBuffer;
    // pr("expected len = %04X, checksum=%04X\n", bd->size, bd->checksum);
    uint16_t t = 0;
    for (uint16_t c = 0; c < bd->size; c++) {
        t += bd->data[c];
    }
    return bd->checksum == t;
}

// EEprom related stuff
static uint32_t getAddressForSlot(const uint8_t s) {
    return EEPROM_IMG_START + (EEPROM_IMG_EACH * s);
}
static void getNumSlots() {
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
static uint8_t findSlotVer(const uint8_t *ver) {
    // return 0xFF;  // remove me! This forces the tag to re-download each and every upload without checking if it's already in the eeprom somewhere
    for (uint8_t c = 0; c < imgSlots; c++) {
        struct EepromImageHeader __xdata *eih = (struct EepromImageHeader __xdata *)blockXferBuffer;
        eepromRead(getAddressForSlot(c), eih, sizeof(struct EepromImageHeader));
        if (xMemEqual4(&eih->validMarker, &markerValid)) {
            if (xMemEqual(&eih->version, (void *)ver, 8)) {
                return c;
            }
        }
    }
    return 0xFF;
}

// making this reentrant saves one byte of idata...
uint8_t __xdata findSlotDataTypeArg(uint8_t arg) __reentrant {
    arg &= (0xF8);  // unmatch with the 'preload' bit and LUT bits
    for (uint8_t c = 0; c < imgSlots; c++) {
        struct EepromImageHeader __xdata *eih = (struct EepromImageHeader __xdata *)blockXferBuffer;
        eepromRead(getAddressForSlot(c), eih, sizeof(struct EepromImageHeader));
        if (xMemEqual4(&eih->validMarker, &markerValid)) {
            if ((eih->argument & 0xF8) == arg) {
                return c;
            }
        }
    }
    return 0xFF;
}
uint8_t getEepromImageDataArgument(const uint8_t slot) {
    struct EepromImageHeader __xdata *eih = (struct EepromImageHeader __xdata *)blockXferBuffer;
    eepromRead(getAddressForSlot(slot), eih, sizeof(struct EepromImageHeader));
    return eih->argument;
}
uint8_t __xdata findNextSlideshowImage(uint8_t start) __reentrant {
    struct EepromImageHeader __xdata *eih = (struct EepromImageHeader __xdata *)blockXferBuffer;
    uint8_t c = start;
    while (1) {
        c++;
        if (c > imgSlots) c = 0;
        if (c == start) return c;
        eepromRead(getAddressForSlot(c), eih, sizeof(struct EepromImageHeader));
        if (xMemEqual4(&eih->validMarker, &markerValid)) {
            if ((eih->argument & 0xF8) == (CUSTOM_IMAGE_SLIDESHOW << 3)) {
                return c;
            }
        }
    }
}

static void eraseUpdateBlock() {
    eepromErase(EEPROM_UPDATA_AREA_START, EEPROM_UPDATE_AREA_LEN / EEPROM_ERZ_SECTOR_SZ);
}
static void eraseImageBlock(const uint8_t c) {
    eepromErase(getAddressForSlot(c), EEPROM_IMG_EACH / EEPROM_ERZ_SECTOR_SZ);
}
static void saveUpdateBlockData(uint8_t blockId) {
    if (!eepromWrite(EEPROM_UPDATA_AREA_START + (blockId * BLOCK_DATA_SIZE), blockXferBuffer + sizeof(struct blockData), BLOCK_DATA_SIZE))
        pr("EEPROM write failed\n");
}
static void saveImgBlockData(const uint8_t imgSlot, const uint8_t blockId) {
    uint16_t length = EEPROM_IMG_EACH - (sizeof(struct EepromImageHeader) + (blockId * BLOCK_DATA_SIZE));
    if (length > 4096) length = 4096;

    if (!eepromWrite(getAddressForSlot(imgSlot) + sizeof(struct EepromImageHeader) + (blockId * BLOCK_DATA_SIZE), blockXferBuffer + sizeof(struct blockData), length))
        pr("EEPROM write failed\n");
}
void eraseImageBlocks() {
    for (uint8_t c = 0; c < imgSlots; c++) {
        eraseImageBlock(c);
    }
}
void drawImageFromEeprom(const uint8_t imgSlot, uint8_t lut) {
    drawImageAtAddress(getAddressForSlot(imgSlot), lut);
}
static uint32_t getHighSlotId() {
    uint32_t temp = 0;
    for (uint8_t __xdata c = 0; c < imgSlots; c++) {
        struct EepromImageHeader __xdata *eih = (struct EepromImageHeader __xdata *)blockXferBuffer;
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

// data transfer stuff
static uint8_t __xdata partsThisBlock = 0;
static uint8_t __xdata blockAttempts = 0;  // these CAN be local to the function, but for some reason, they won't survive sleep?
                                           // they get overwritten with  7F 32 44 20 00 00 00 00 11, I don't know why.

static bool getDataBlock(const uint16_t blockSize) {
    blockAttempts = BLOCK_TRANSFER_ATTEMPTS;
    if (blockSize == BLOCK_DATA_SIZE) {
        partsThisBlock = BLOCK_MAX_PARTS;
        memset(curBlock.requestedParts, 0xFF, BLOCK_REQ_PARTS_BYTES);
    } else {
        partsThisBlock = (sizeof(struct blockData) + blockSize) / BLOCK_PART_DATA_SIZE;
        if (blockSize % BLOCK_PART_DATA_SIZE) partsThisBlock++;
        memset(curBlock.requestedParts, 0x00, BLOCK_REQ_PARTS_BYTES);
        for (uint8_t c = 0; c < partsThisBlock; c++) {
            curBlock.requestedParts[c / 8] |= (1 << (c % 8));
        }
    }

    requestPartialBlock = false;  // this forces the AP to request the block data from the host

    while (blockAttempts--) {
#ifndef DEBUGBLOCKS
        pr("REQ %d ", curBlock.blockId);
#else
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
        powerUp(INIT_RADIO);
        struct blockRequestAck *__xdata ack = performBlockRequest();

        if (ack == NULL) {
            pr("Cancelled request\n");
            return false;
        }
        if (ack->pleaseWaitMs) {  // SLEEP - until the AP is ready with the data
            if (ack->pleaseWaitMs < 35) {
                timerDelay(ack->pleaseWaitMs * TIMER_TICKS_PER_MS);
            } else {
                doSleep(ack->pleaseWaitMs - 10);
                powerUp(INIT_UART | INIT_RADIO);
                radioRxEnable(true, true);
            }
        } else {
            // immediately start with the reception of the block data
        }
        blockRxLoop(270);  // BLOCK RX LOOP - receive a block, until the timeout has passed
        powerDown(INIT_RADIO);

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
#ifndef DEBUGBLOCKS
            pr("- COMPLETE\n");
#endif
            if (validateBlockData()) {
                // block download complete, validated
                return true;
            } else {
                for (uint8_t c = 0; c < partsThisBlock; c++) {
                    curBlock.requestedParts[c / 8] |= (1 << (c % 8));
                }
                requestPartialBlock = false;
                pr("blk failed validation!\n");
            }
        } else {
#ifndef DEBUGBLOCKS
            pr("- INCOMPLETE\n");
#endif
            // block incomplete, re-request a partial block
            requestPartialBlock = true;
        }
    }
    pr("failed getting block\n");
    return false;
}

uint16_t __xdata dataRequestSize = 0;
static bool downloadFWUpdate(const struct AvailDataInfo *__xdata avail) {
    // check if we already started the transfer of this information & haven't completed it
    if (xMemEqual((const void *__xdata) & avail->dataVer, (const void *__xdata) & xferDataInfo.dataVer, 8) && xferDataInfo.dataSize) {
        // looks like we did. We'll carry on where we left off.
    } else {
        // start, or restart the transfer from 0. Copy data from the AvailDataInfo struct, and the struct intself. This forces a new transfer
        curBlock.blockId = 0;
        xMemCopy8(&(curBlock.ver), &(avail->dataVer));
        curBlock.type = avail->dataType;
        xMemCopyShort(&xferDataInfo, (void *)avail, sizeof(struct AvailDataInfo));
        eraseUpdateBlock();
    }

    while (xferDataInfo.dataSize) {
        wdt10s();
        if (xferDataInfo.dataSize > BLOCK_DATA_SIZE) {
            // more than one block remaining
            dataRequestSize = BLOCK_DATA_SIZE;
        } else {
            // only one block remains
            dataRequestSize = xferDataInfo.dataSize;
        }
        if (getDataBlock(dataRequestSize)) {
            // succesfully downloaded datablock, save to eeprom
            powerUp(INIT_EEPROM);
            saveUpdateBlockData(curBlock.blockId);
            powerDown(INIT_EEPROM);
            curBlock.blockId++;
            xferDataInfo.dataSize -= dataRequestSize;
        } else {
            // failed to get the block we wanted, we'll stop for now, maybe resume later
            return false;
        }
    }
    // no more data, download complete
    return true;
}

uint16_t __xdata imageSize = 0;
static bool downloadImageDataToEEPROM(const struct AvailDataInfo *__xdata avail) {
    // check if we already started the transfer of this information & haven't completed it
    if (xMemEqual((const void *__xdata) & avail->dataVer, (const void *__xdata) & xferDataInfo.dataVer, 8) &&
        (xferDataInfo.dataTypeArgument == avail->dataTypeArgument) &&
        xferDataInfo.dataSize) {
        // looks like we did. We'll carry on where we left off.
        pr("restarting image download");
    } else {
        // new transfer
        powerUp(INIT_EEPROM);
        // go to the next image slot
        uint8_t startingSlot = nextImgSlot;
        while (1) {
            nextImgSlot++;
            if (nextImgSlot >= imgSlots) nextImgSlot = 0;
            if (nextImgSlot == startingSlot) {
                powerDown(INIT_EEPROM);
                pr("No slots available. Too many images in the slideshow?\n");
                return true;
            }
            struct EepromImageHeader __xdata *eih = (struct EepromImageHeader __xdata *)blockXferBuffer;
            eepromRead(getAddressForSlot(nextImgSlot), eih, sizeof(struct EepromImageHeader));
            // check if the marker is indeed valid
            if (xMemEqual4(&eih->validMarker, &markerValid)) {
                struct imageDataTypeArgStruct *eepromDataArgument = (struct imageDataTypeArgStruct *)&(eih->argument);
                // normal type, we can overwrite this
                if (eepromDataArgument->specialType == 0x00) break;
            } else {
                // bullshit marker, so safe to overwrite
                break;
            }
        }

        xferImgSlot = nextImgSlot;

        uint8_t __xdata attempt = 5;
        while (attempt--) {
            if (eepromErase(getAddressForSlot(xferImgSlot), EEPROM_IMG_EACH / EEPROM_ERZ_SECTOR_SZ)) goto eraseSuccess;
        }
    eepromFail:
        powerDown(INIT_RADIO);
        powerUp(INIT_EPD);
        showNoEEPROM();
        powerDown(INIT_EEPROM | INIT_EPD);
        doSleep(-1);
        wdtDeviceReset();
    eraseSuccess:
        powerDown(INIT_EEPROM);
        pr("new download, writing to slot %d\n", xferImgSlot);

        // start, or restart the transfer. Copy data from the AvailDataInfo struct, and the struct intself. This forces a new transfer
        curBlock.blockId = 0;
        xMemCopy8(&(curBlock.ver), &(avail->dataVer));
        curBlock.type = avail->dataType;
        xMemCopyShort(&xferDataInfo, (void *)avail, sizeof(struct AvailDataInfo));
        imageSize = xferDataInfo.dataSize;
    }

    while (xferDataInfo.dataSize) {
        wdt10s();
        if (xferDataInfo.dataSize > BLOCK_DATA_SIZE) {
            // more than one block remaining
            dataRequestSize = BLOCK_DATA_SIZE;
        } else {
            // only one block remains
            dataRequestSize = xferDataInfo.dataSize;
        }
        if (getDataBlock(dataRequestSize)) {
            // succesfully downloaded datablock, save to eeprom
            powerUp(INIT_EEPROM);
#ifdef DEBUGBLOCKS
            pr("Saving block %d to slot %d\n", curBlock.blockId, xferImgSlot);
#endif
            saveImgBlockData(xferImgSlot, curBlock.blockId);
            powerDown(INIT_EEPROM);
            curBlock.blockId++;
            xferDataInfo.dataSize -= dataRequestSize;
        } else {
            // failed to get the block we wanted, we'll stop for now, probably resume later
            return false;
        }
    }
    // no more data, download complete

    // borrow the blockXferBuffer temporarily
    struct EepromImageHeader __xdata *eih = (struct EepromImageHeader __xdata *)blockXferBuffer;
    xMemCopy8(&eih->version, &xferDataInfo.dataVer);
    eih->validMarker = EEPROM_IMG_VALID;
    eih->id = ++curHighSlotId;
    eih->size = imageSize;
    eih->dataType = xferDataInfo.dataType;
    eih->argument = xferDataInfo.dataTypeArgument;

#ifdef DEBUGBLOCKS
    pr("Now writing datatype 0x%02X to slot %d\n", xferDataInfo.dataType, xferImgSlot);
#endif
    powerUp(INIT_EEPROM);
    eepromWrite(getAddressForSlot(xferImgSlot), eih, sizeof(struct EepromImageHeader));
    powerDown(INIT_EEPROM);

    return true;
}

inline bool processImageDataAvail(struct AvailDataInfo *__xdata avail) {
    struct imageDataTypeArgStruct arg;
    *((uint8_t *)arg) = avail->dataTypeArgument;

    if (arg.preloadImage) {
        pr("Preloading image with type 0x%02X from arg 0x%02X\n", arg.specialType, avail->dataTypeArgument);
        powerUp(INIT_EEPROM);
        switch (arg.specialType) {
            // check if a slot with this argument is already set; if so, erase. Only one of each arg type should exist
            default: {
                uint8_t slot = findSlotDataTypeArg(avail->dataTypeArgument);
                if (slot != 0xFF) {
                    eepromErase(getAddressForSlot(slot), EEPROM_IMG_EACH / EEPROM_ERZ_SECTOR_SZ);
                }
            } break;
            // regular image preload, there can be multiple of this type in the EEPROM
            case CUSTOM_IMAGE_NOCUSTOM: {
                // check if a version of this already exists
                uint8_t slot = findSlotVer(&(avail->dataVer));
                if (slot != 0xFF) {
                    powerUp(INIT_RADIO);
                    sendXferComplete();
                    powerDown(INIT_RADIO);
                    return true;
                }
            } break;
            case CUSTOM_IMAGE_SLIDESHOW:
                break;
        }
        powerDown(INIT_EEPROM);

        pr("downloading preload image...\n");
        if (downloadImageDataToEEPROM(avail)) {
            // sets xferImgSlot to the right slot
            pr("preload complete!\n");
            powerUp(INIT_RADIO);
            sendXferComplete();
            powerDown(INIT_RADIO);
            return true;
        } else {
            return false;
        }

    } else {
        // check if we're currently displaying this data payload
        if (xMemEqual((const void *__xdata) & avail->dataVer, (const void *__xdata)curDispDataVer, 8)) {
            // currently displayed, not doing anything except for sending an XFC

            pr("currently shown image, send xfc\n");
            powerUp(INIT_RADIO);
            sendXferComplete();
            powerDown(INIT_RADIO);
            return true;

        } else {
            // currently not displayed

            // try to find the data in the SPI EEPROM
            powerUp(INIT_EEPROM);
            uint8_t findImgSlot = findSlotVer(&(avail->dataVer));
            powerDown(INIT_EEPROM);

            // Is this image already in a slot somewhere
            if (findImgSlot != 0xFF) {
                // found a (complete)valid image slot for this version
                powerUp(INIT_RADIO);
                sendXferComplete();
                powerDown(INIT_RADIO);

                // mark as completed and draw from EEPROM
                xMemCopyShort(&xferDataInfo, (void *)avail, sizeof(struct AvailDataInfo));
                xferDataInfo.dataSize = 0;  // mark as transfer not pending

                wdt60s();
                curImgSlot = findImgSlot;
                powerUp(INIT_EPD | INIT_EEPROM);
                drawImageFromEeprom(findImgSlot, arg.lut);
                powerDown(INIT_EPD | INIT_EEPROM);

            } else {
                // not found in cache, prepare to download
                pr("downloading image...\n");
                if (downloadImageDataToEEPROM(avail)) {
                    // sets xferImgSlot to the right slot
                    pr("download complete!\n");
                    powerUp(INIT_RADIO);
                    sendXferComplete();
                    powerDown(INIT_RADIO);

                    // not preload, draw now
                    wdt60s();
                    curImgSlot = xferImgSlot;
                    powerUp(INIT_EPD | INIT_EEPROM);
                    drawImageFromEeprom(xferImgSlot, arg.lut);
                    powerDown(INIT_EPD | INIT_EEPROM);

                } else {
                    return false;
                }
            }

            // keep track on what is currently displayed
            xMemCopy8(curDispDataVer, xferDataInfo.dataVer);
            return true;
        }
    }
}

bool processAvailDataInfo(struct AvailDataInfo *__xdata avail) {
    switch (avail->dataType) {
        case DATATYPE_IMG_BMP:
        case DATATYPE_IMG_DIFF:
        case DATATYPE_IMG_RAW_1BPP:
        case DATATYPE_IMG_RAW_2BPP:
            return processImageDataAvail(avail);
            break;
        case DATATYPE_FW_UPDATE:
            powerUp(INIT_EEPROM);
            if (downloadFWUpdate(avail)) {
                pr("firmware download complete, doing update.\n");

                powerUp(INIT_EPD);
                showApplyUpdate();

                powerUp(INIT_RADIO);
                sendXferComplete();
                powerDown(INIT_RADIO);

                powerUp(INIT_EEPROM);
                wdt60s();
                eepromReadStart(EEPROM_UPDATA_AREA_START);
                selfUpdate();
            } else {
                return false;
            }
            break;
        case DATATYPE_NFC_URL_DIRECT:
        case DATATYPE_NFC_RAW_CONTENT:
            // Handle data for the NFC IC (if we have it)

            // check if we actually have the capability to do NFC
            if (!(capabilities & CAPABILITY_HAS_NFC)) {
                // looks like we don't. mark as complete and then bail!
                powerUp(INIT_RADIO);
                sendXferComplete();
                powerDown(INIT_RADIO);
                return true;
            }

            pr("NFC URL received\n");
            /*
            if (curDataInfo.dataSize == 0 && xMemEqual((const void *__xdata) & avail->dataVer, (const void *__xdata) & curDataInfo.dataVer, 8)) {
                // we've already downloaded this NFC data, disregard and send XFC
                pr("this was the same as the last transfer, disregard\n");
                powerUp(INIT_RADIO);
                sendXferComplete();
                powerDown(INIT_RADIO);
                return true;
            }
            curBlock.blockId = 0;
            xMemCopy8(&(curBlock.ver), &(avail->dataVer));
            curBlock.type = avail->dataType;
            xMemCopyShort(&curDataInfo, (void *)avail, sizeof(struct AvailDataInfo));
            uint16_t __xdata nfcsize = avail->dataSize;
            wdt10s();
            if (getDataBlock(avail->dataSize)) {
                curDataInfo.dataSize = 0;  // mark as transfer not pending
                powerUp(INIT_I2C);
                if (avail->dataType == DATATYPE_NFC_URL_DIRECT) {
                    // only one URL (handle NDEF records on the tag)
                    loadURLtoNTag();
                } else {
                    // raw NFC data upload to the NFC IC
                    loadRawNTag(nfcsize);
                }
                timerDelay(13330);
                powerDown(INIT_I2C);
                powerUp(INIT_RADIO);
                sendXferComplete();
                powerDown(INIT_RADIO);
                return true;
            }
            return false;
            break;
            */
        case DATATYPE_TAG_CONFIG_DATA:
            if (xferDataInfo.dataSize == 0 && xMemEqual((const void *__xdata) & avail->dataVer, (const void *__xdata) & xferDataInfo.dataVer, 8)) {
                pr("this was the same as the last transfer, disregard\n");
                powerUp(INIT_RADIO);
                sendXferComplete();
                powerDown(INIT_RADIO);
                return true;
            }
            curBlock.blockId = 0;
            xMemCopy8(&(curBlock.ver), &(avail->dataVer));
            curBlock.type = avail->dataType;
            xMemCopyShort(&xferDataInfo, (void *)avail, sizeof(struct AvailDataInfo));
            wdt10s();
            if (getDataBlock(avail->dataSize)) {
                xferDataInfo.dataSize = 0;  // mark as transfer not pending
                loadSettingsFromBuffer(sizeof(struct blockData) + blockXferBuffer);
                powerUp(INIT_RADIO);
                sendXferComplete();
                powerDown(INIT_RADIO);
                return true;
            }
            return false;
            break;
        case DATATYPE_COMMAND_DATA:
            pr("CMD received\n");
            powerUp(INIT_RADIO);
            sendXferComplete();
            powerDown(INIT_RADIO);
            executeCommand(avail->dataTypeArgument);
            return true;
            break;
        case DATATYPE_CUSTOM_LUT_OTA:
            // Handle data for the NFC IC (if we have it)

            // check if we actually have the capability to do OTA Luts
            if (!(capabilities & CAPABILITY_SUPPORTS_CUSTOM_LUTS)) {
                // looks like we don't. mark as complete and then bail!
                powerUp(INIT_RADIO);
                sendXferComplete();
                powerDown(INIT_RADIO);
                return true;
            }
#ifdef EPD_SSD1619
            pr("OTA LUT received\n");
            if (xferDataInfo.dataSize == 0 && xMemEqual((const void *__xdata) & avail->dataVer, (const void *__xdata) & xferDataInfo.dataVer, 8)) {
                pr("this was the same as the last transfer, disregard\n");
                powerUp(INIT_RADIO);
                sendXferComplete();
                powerDown(INIT_RADIO);
                return true;
            }
            curBlock.blockId = 0;
            xMemCopy8(&(curBlock.ver), &(avail->dataVer));
            curBlock.type = avail->dataType;
            xMemCopyShort(&xferDataInfo, (void *)avail, sizeof(struct AvailDataInfo));
            wdt10s();
            if (getDataBlock(avail->dataSize)) {
                xferDataInfo.dataSize = 0;  // mark as transfer not pending
                memcpy(customLUT, sizeof(struct blockData) + blockXferBuffer, 6 + (dispLutSize * 10));
                powerUp(INIT_RADIO);
                sendXferComplete();
                powerDown(INIT_RADIO);
                return true;
            }
#endif
            return false;
            break;
    }
    return true;
}

void initializeProto() {
    powerUp(INIT_EEPROM);
    getNumSlots();
    curHighSlotId = getHighSlotId();
    powerDown(INIT_EEPROM);
}