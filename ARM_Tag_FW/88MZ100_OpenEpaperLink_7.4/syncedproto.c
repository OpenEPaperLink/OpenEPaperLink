
#include "syncedproto.h"

#include "board.h"
#include "comms.h"
#include "drawing.h"
#include "eeprom.h"
#include "main.h"
#include "mz100_sleep.h"
#include "powermgt.h"
#include "proto.h"
#include "timer.h"
#include "util.h"
#include "zigbee.h"
#include "printf.h"

// download-stuff
uint8_t blockXferBuffer[BLOCK_XFER_BUFFER_SIZE] = {0};
__attribute__((section(".aonshadow"))) struct blockRequest curBlock = {0};      // used by the block-requester, contains the next request that we'll send
__attribute__((section(".aonshadow"))) struct AvailDataInfo curDataInfo = {0};  // last 'AvailDataInfo' we received from the AP
bool requestPartialBlock = false;                                               // if we should ask the AP to get this block from the host or not
#define BLOCK_TRANSFER_ATTEMPTS 10

__attribute__((section(".aonshadow"))) uint8_t prevImgSlot = 0xFF;
__attribute__((section(".aonshadow"))) uint8_t curImgSlot = 0xFF;
__attribute__((section(".aonshadow"))) static uint32_t curHighSlotId = 0;
__attribute__((section(".aonshadow"))) static uint8_t nextImgSlot = 0;
__attribute__((section(".aonshadow"))) static uint8_t imgSlots = 0;
__attribute__((section(".aonshadow"))) uint8_t drawWithLut = 0;

// stuff we need to keep track of related to the network/AP
uint8_t APmac[8] = {0};
uint16_t APsrcPan = 0;
__attribute__((section(".aonshadow"))) uint8_t mSelfMac[8] = {0};
__attribute__((section(".aonshadow"))) static uint8_t seq = 0;
__attribute__((section(".aonshadow"))) volatile uint8_t currentChannel = 0;

// buffer we use to prepare/read packets
static uint8_t inBuffer[128] = {0};
static uint8_t outBuffer[128] = {0};

// #define DEBUGBLOCKS 1

// tools
static uint8_t getPacketType(const void *buffer) {
    const struct MacFcs *fcs = buffer;
    if ((fcs->frameType == 1) && (fcs->destAddrType == 2) && (fcs->srcAddrType == 3) && (fcs->panIdCompressed == 0)) {
        // broadcast frame
        uint8_t type = ((uint8_t *)buffer)[sizeof(struct MacFrameBcast)];
        return type;
    } else if ((fcs->frameType == 1) && (fcs->destAddrType == 3) && (fcs->srcAddrType == 3) && (fcs->panIdCompressed == 1)) {
        // normal frame
        uint8_t type = ((uint8_t *)buffer)[sizeof(struct MacFrameNormal)];
        return type;
    }
    return 0;
}
static bool pktIsUnicast(const void *buffer) {
    const struct MacFcs *fcs = buffer;
    if ((fcs->frameType == 1) && (fcs->destAddrType == 2) && (fcs->srcAddrType == 3) && (fcs->panIdCompressed == 0)) {
        return false;
    } else if ((fcs->frameType == 1) && (fcs->destAddrType == 3) && (fcs->srcAddrType == 3) && (fcs->panIdCompressed == 1)) {
        // normal frame
        return true;
    }
    // unknown type...
    return false;
}
void dump(const uint8_t *a, const uint16_t l) {
    printf("\n        ");
#define ROWS 16
    for (uint8_t c = 0; c < ROWS; c++) {
        printf(" %02X", c);
    }
    printf("\n--------");
    for (uint8_t c = 0; c < ROWS; c++) {
        printf("---");
    }
    for (uint16_t c = 0; c < l; c++) {
        if ((c % ROWS) == 0) {
            printf("\n0x%04X | ", c);
        }
        printf("%02X ", a[c]);
    }
    printf("\n--------");
    for (uint8_t c = 0; c < ROWS; c++) {
        printf("---");
    }
    printf("\n");
}
static bool checkCRC(const void *p, const uint8_t len) {
    uint8_t total = 0;
    for (uint8_t c = 1; c < len; c++) {
        total += ((uint8_t *)p)[c];
    }
    // printf("CRC: rx %d, calc %d\n", ((uint8_t *)p)[0], total);
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
    struct MacFrameBcast *txframe = (struct MacFrameBcast *)(outBuffer + 1);
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
    uint32_t t;
    radioRxEnable(false);
    radioSetChannel(channel);
    radioRxFlush();
    radioRxEnable(true);
    for (uint8_t c = 1; c <= MAXIMUM_PING_ATTEMPTS; c++) {
        sendPing();
        t = timerGet() + (TIMER_TICKS_PER_MSEC * 5);
        while (timerGet() < t) {
            int8_t ret = commsRxUnenc(inBuffer);
            if (ret > 1) {
                if ((inBuffer[sizeof(struct MacFrameNormal) + 1] == channel) && (getPacketType(inBuffer) == PKT_PONG)) {
                    if (pktIsUnicast(inBuffer)) {
                        //        dump(inBuffer,32);
                        struct MacFrameNormal *f = (struct MacFrameNormal *)inBuffer;
                        memcpy(APmac, f->src, 8);
                        APsrcPan = f->pan;
                        return c;
                    }
                }
            }
            timerDelay(TIMER_TICKS_PER_MSEC * 2);
        }
    }
    return 0;
}

// data xfer stuff
static void sendShortAvailDataReq() {
    struct MacFrameBcast *txframe = (struct MacFrameBcast *)(outBuffer + 1);
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
    struct MacFrameBcast *txframe = (struct MacFrameBcast *)(outBuffer + 1);
    memset(outBuffer, 0, sizeof(struct MacFrameBcast) + sizeof(struct AvailDataReq) + 2 + 4);
    struct AvailDataReq *availreq = (struct AvailDataReq *)(outBuffer + 2 + sizeof(struct MacFrameBcast));
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
    availreq->hwType = 5;
    availreq->wakeupReason = wakeUpReason;
    availreq->lastPacketRSSI = mLastRSSI;
    availreq->lastPacketLQI = mLastLqi;
    availreq->temperature = temperature;
    availreq->batteryMv = batteryVoltage;
    availreq->capabilities = capabilities;
    addCRC(availreq, sizeof(struct AvailDataReq));
    commsTxNoCpy(outBuffer);
}
struct AvailDataInfo *getAvailDataInfo() {
    radioRxEnable(true);
    uint32_t t;
    for (uint8_t c = 0; c < DATA_REQ_MAX_ATTEMPTS; c++) {
        sendAvailDataReq();
        t = timerGet() + (TIMER_TICKS_PER_MSEC * (DATA_REQ_RX_WINDOW_SIZE + 2));
        while (timerGet() < t) {
            int8_t ret = commsRxUnenc(inBuffer);
            if (ret > 1) {
                if (getPacketType(inBuffer) == PKT_AVAIL_DATA_INFO) {
                    if (checkCRC(inBuffer + sizeof(struct MacFrameNormal) + 1, sizeof(struct AvailDataInfo))) {
                        struct MacFrameNormal *f = (struct MacFrameNormal *)inBuffer;
                        memcpy(APmac, f->src, 8);
                        APsrcPan = f->pan;
                        dataReqLastAttempt = c;
                        printf("%d", dataReqLastAttempt);
                        return (struct AvailDataInfo *)(inBuffer + sizeof(struct MacFrameNormal) + 1);
                    }
                }
            }
        }
    }
    dataReqLastAttempt = DATA_REQ_MAX_ATTEMPTS;
    return NULL;
}
struct AvailDataInfo *getShortAvailDataInfo() {
    radioRxEnable(true);
    uint32_t t;
    for (uint8_t c = 0; c < DATA_REQ_MAX_ATTEMPTS; c++) {
        sendShortAvailDataReq();
        // sendAvailDataReq();
        t = timerGet() + (TIMER_TICKS_PER_MSEC * 25);
        while (timerGet() < t) {
            int8_t ret = commsRxUnenc(inBuffer);
            if (ret > 1) {
                if (getPacketType(inBuffer) == PKT_AVAIL_DATA_INFO) {
                    if (checkCRC(inBuffer + sizeof(struct MacFrameNormal) + 1, sizeof(struct AvailDataInfo))) {
                        struct MacFrameNormal *f = (struct MacFrameNormal *)inBuffer;
                        memcpy(APmac, f->src, 8);
                        APsrcPan = f->pan;
                        dataReqLastAttempt = c;
                        printf("%d", dataReqLastAttempt);
                        return (struct AvailDataInfo *)(inBuffer + sizeof(struct MacFrameNormal) + 1);
                    }
                }
            }
        }
    }
    dataReqLastAttempt = DATA_REQ_MAX_ATTEMPTS;
    return NULL;
}
static bool processBlockPart(const struct blockPart *bp) {
    uint16_t start = bp->blockPart * BLOCK_PART_DATA_SIZE;
    uint16_t size = BLOCK_PART_DATA_SIZE;
    // validate if it's okay to copy data
    if (bp->blockId != curBlock.blockId) {
        // printf("got a packet for block %02X\n", bp->blockId);
        return false;
    }
    if (start >= (sizeof(blockXferBuffer) - 1))
        return false;
    if (bp->blockPart > BLOCK_MAX_PARTS)
        return false;
    if ((start + size) > sizeof(blockXferBuffer)) {
        size = sizeof(blockXferBuffer) - start;
    }
    if (checkCRC(bp, sizeof(struct blockPart) + BLOCK_PART_DATA_SIZE)) {
        //  copy block data to buffer
        memcpy((void *)(blockXferBuffer + start), (const void *)bp->data, size);
        // we don't need this block anymore, set bit to 0 so we don't request it again
        curBlock.requestedParts[bp->blockPart / 8] &= ~(1 << (bp->blockPart % 8));
        return true;
    } else {
        return false;
    }
}
static bool blockRxLoop(const uint32_t timeout) {
    uint32_t t;
    bool success = false;
    radioRxEnable(true);
    t = timerGet() + (TIMER_TICKS_PER_MSEC * (timeout + 20));
    while (timerGet() < t) {
        int8_t ret = commsRxUnenc(inBuffer);
        if (ret > 1) {
            if (getPacketType(inBuffer) == PKT_BLOCK_PART) {
                struct blockPart *bp = (struct blockPart *)(inBuffer + sizeof(struct MacFrameNormal) + 1);
                success = processBlockPart(bp);
            }
        }
    }
    radioRxEnable(false);
    radioRxFlush();
    return success;
}
static struct blockRequestAck *continueToRX() {
    struct blockRequestAck *ack = (struct blockRequestAck *)(inBuffer + sizeof(struct MacFrameNormal) + 1);
    ack->pleaseWaitMs = 0;
    return ack;
}
static void sendBlockRequest() {
    memset(outBuffer, 0, sizeof(struct MacFrameNormal) + sizeof(struct blockRequest) + 2 + 2);
    struct MacFrameNormal *f = (struct MacFrameNormal *)(outBuffer + 1);
    struct blockRequest *blockreq = (struct blockRequest *)(outBuffer + 2 + sizeof(struct MacFrameNormal));
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
    // printf("req ver: %02X%02X%02X%02X%02X%02X%02X%02X\n", ((uint8_t*)&blockreq->ver)[0],((uint8_t*)&blockreq->ver)[1],((uint8_t*)&blockreq->ver)[2],((uint8_t*)&blockreq->ver)[3],((uint8_t*)&blockreq->ver)[4],((uint8_t*)&blockreq->ver)[5],((uint8_t*)&blockreq->ver)[6],((uint8_t*)&blockreq->ver)[7]);
    addCRC(blockreq, sizeof(struct blockRequest));
    commsTxNoCpy(outBuffer);
}
static struct blockRequestAck *performBlockRequest() {
    uint32_t t;
    radioRxEnable(true);
    radioRxFlush();
    for (uint8_t c = 0; c < 30; c++) {
        sendBlockRequest();
        t = timerGet() + (TIMER_TICKS_PER_MSEC * (7UL + c / 10));
        do {
            int8_t ret = commsRxUnenc(inBuffer);
            if (ret > 1) {
                switch (getPacketType(inBuffer)) {
                    case PKT_BLOCK_REQUEST_ACK:
                        if (checkCRC((inBuffer + sizeof(struct MacFrameNormal) + 1), sizeof(struct blockRequestAck)))
                            return (struct blockRequestAck *)(inBuffer + sizeof(struct MacFrameNormal) + 1);
                        break;
                    case PKT_BLOCK_PART:
                        // block already started while we were waiting for a get block reply
                        // printf("!");
                        // processBlockPart((struct blockPart *)(inBuffer + sizeof(struct MacFrameNormal) + 1));
                        return continueToRX();
                        break;
                    case PKT_CANCEL_XFER:
                        return NULL;
                    default:
                        printf("pkt w/type %02X\n", getPacketType(inBuffer));
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
    struct MacFrameNormal *f = (struct MacFrameNormal *)(outBuffer + 1);
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
    radioRxEnable(true);

    for (uint8_t c = 0; c < 16; c++) {
        sendXferCompletePacket();
        uint32_t start = timerGet();
        while ((timerGet() - start) < (TIMER_TICKS_PER_MSEC * 6UL)) {
            int8_t ret = commsRxUnenc(inBuffer);
            if (ret > 1) {
                if (getPacketType(inBuffer) == PKT_XFER_COMPLETE_ACK) {
                    printf("XFC ACK\n");
                    return;
                }
            }
        }
    }
    printf("XFC NACK!\n");
    return;
}
static bool validateBlockData() {
    struct blockData *bd = (struct blockData *)blockXferBuffer;
    // printf("expected len = %04X, checksum=%04X\n", bd->size, bd->checksum);
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
    printf("Checking slots\n");
    imgSlots = 1;

    /*uint32_t eeSize = eepromGetSize();
    uint16_t nSlots = (eeSize - EEPROM_IMG_START) / (EEPROM_IMG_EACH >> 8) >> 8;
    if (eeSize < EEPROM_IMG_START || !nSlots)
    {
        printf("eeprom is too small\n");
        while (1)
            ;
    }
    else if (nSlots >> 8)
    {
        printf("eeprom is too big, some will be unused\n");
        imgSlots = 254;
    }
    else
        imgSlots = nSlots;*/
    printf("Got %i nr of slots\n", imgSlots);
}
static uint8_t findSlot(const uint8_t *ver) {
    // return 0xFF;  // remove me! This forces the tag to re-download each and every upload without checking if it's already in the eeprom somewhere
    uint32_t markerValid = EEPROM_IMG_VALID;
    for (uint8_t c = 0; c < imgSlots; c++) {
        struct EepromImageHeader *eih = (struct EepromImageHeader *)blockXferBuffer;
        eepromRead(getAddressForSlot(c), eih, sizeof(struct EepromImageHeader));
        if (!memcmp(&eih->validMarker, &markerValid, 4)) {
            if (!memcmp(&eih->version, (void *)ver, 8)) {
                return c;
            }
        }
    }
    return 0xFF;
}
static void eraseUpdateBlock() {
    eepromErase(EEPROM_UPDATA_AREA_START, (uint16_t)EEPROM_UPDATE_AREA_LEN);
}
static void saveUpdateBlockData(uint8_t blockId) {
    printf("EEPROM writing UpdateBlock %i\n", blockId);
    if (!eepromWrite(EEPROM_UPDATA_AREA_START + (blockId * BLOCK_DATA_SIZE), blockXferBuffer + sizeof(struct blockData), BLOCK_DATA_SIZE))
        printf("EEPROM write failed\n");
}
static void saveImgBlockData(const uint8_t imgSlot, const uint8_t blockId) {
    printf("EEPROM writing Slot: %i ImageBlock %i\n", imgSlot, blockId);
    uint32_t length = EEPROM_IMG_EACH - (sizeof(struct EepromImageHeader) + (blockId * BLOCK_DATA_SIZE));
    if (length > 4096)
        length = 4096;

    if (!eepromWrite(getAddressForSlot(imgSlot) + sizeof(struct EepromImageHeader) + (blockId * BLOCK_DATA_SIZE), blockXferBuffer + sizeof(struct blockData), length))
        printf("EEPROM write failed\n");
}
void drawImageFromEeprom(const uint8_t imgSlot) {
    drawImageAtAddress(getAddressForSlot(imgSlot), drawWithLut);
    drawWithLut = 0;  // default back to the regular ol' stock/OTP LUT
}
static uint32_t getHighSlotId() {
    uint32_t temp = 0;
    /*uint32_t markerValid = EEPROM_IMG_VALID;
    for (uint8_t c = 0; c < imgSlots; c++)
    {
        struct EepromImageHeader *eih = (struct EepromImageHeader *)blockXferBuffer;
        eepromRead(getAddressForSlot(c), eih, sizeof(struct EepromImageHeader));
        if (!memcmp(&eih->validMarker, &markerValid, 4))
        {
            if (temp < eih->id)
            {
                temp = eih->id;
                nextImgSlot = c;
            }
        }
    }*/
    printf("found high id=%lu in slot %d\n", temp, nextImgSlot);
    return temp;
}

static uint8_t partsThisBlock = 0;
static uint8_t blockAttempts = 0;  // these CAN be local to the function, but for some reason, they won't survive sleep?
                                   // they get overwritten with  7F 32 44 20 00 00 00 00 11, I don't know why.

static bool getDataBlock(const uint16_t blockSize) {
    blockAttempts = BLOCK_TRANSFER_ATTEMPTS;
    if (blockSize == BLOCK_DATA_SIZE) {
        partsThisBlock = BLOCK_MAX_PARTS;
        memset(curBlock.requestedParts, 0xFF, BLOCK_REQ_PARTS_BYTES);
    } else {
        partsThisBlock = (sizeof(struct blockData) + blockSize) / BLOCK_PART_DATA_SIZE;
        if (blockSize % BLOCK_PART_DATA_SIZE)
            partsThisBlock++;
        memset(curBlock.requestedParts, 0x00, BLOCK_REQ_PARTS_BYTES);
        for (uint8_t c = 0; c < partsThisBlock; c++) {
            curBlock.requestedParts[c / 8] |= (1 << (c % 8));
        }
    }

    requestPartialBlock = false;  // this forces the AP to request the block data from the host

    while (blockAttempts--) {
#ifndef DEBUGBLOCKS
        printf("REQ %d ", curBlock.blockId);
#else
        printf("REQ %d[", curBlock.blockId);
        for (uint8_t c = 0; c < BLOCK_MAX_PARTS; c++) {
            if ((c != 0) && (c % 8 == 0))
                printf("][");
            if (curBlock.requestedParts[c / 8] & (1 << (c % 8))) {
                printf("R");
            } else {
                printf("_");
            }
        }
        printf("]\n");
#endif
        powerUp(INIT_RADIO);
        struct blockRequestAck *ack = performBlockRequest();

        if (ack == NULL) {
            printf("Cancelled request\n");
            return false;
        }
        if (ack->pleaseWaitMs) {  // SLEEP - until the AP is ready with the data
            timerDelay(TIMER_TICKS_PER_MSEC * ack->pleaseWaitMs);
        } else {
            // immediately start with the reception of the block data
        }
        blockRxLoop(270);  // BLOCK RX LOOP - receive a block, until the timeout has passed
        powerDown(INIT_RADIO);

#ifdef DEBUGBLOCKS
        printf("RX  %d[", curBlock.blockId);
        for (uint8_t c = 0; c < BLOCK_MAX_PARTS; c++) {
            if ((c != 0) && (c % 8 == 0))
                printf("][");
            if (curBlock.requestedParts[c / 8] & (1 << (c % 8))) {
                printf(".");
            } else {
                printf("R");
            }
        }
        printf("]\n");
#endif
        // check if we got all the parts we needed, e.g: has the block been completed?
        bool blockComplete = true;
        for (uint8_t c = 0; c < partsThisBlock; c++) {
            if (curBlock.requestedParts[c / 8] & (1 << (c % 8)))
                blockComplete = false;
        }

        if (blockComplete) {
#ifndef DEBUGBLOCKS
            printf("- COMPLETE\n");
#endif
            if (validateBlockData()) {
                // block download complete, validated
                return true;
            } else {
                for (uint8_t c = 0; c < partsThisBlock; c++) {
                    curBlock.requestedParts[c / 8] |= (1 << (c % 8));
                }
                requestPartialBlock = false;
                printf("blk failed validation!\n");
            }
        } else {
#ifndef DEBUGBLOCKS
            printf("- INCOMPLETE\n");
#endif
            // block incomplete, re-request a partial block
            requestPartialBlock = true;
        }
    }
    printf("failed getting block\n");
    return false;
}
uint16_t dataRequestSize = 0;
static bool downloadFWUpdate(const struct AvailDataInfo *avail) {
    // check if we already started the transfer of this information & haven't completed it
    if (!memcmp((const void *)&avail->dataVer, (const void *)&curDataInfo.dataVer, 8) && curDataInfo.dataSize) {
        // looks like we did. We'll carry on where we left off.
    } else {
        // start, or restart the transfer from 0. Copy data from the AvailDataInfo struct, and the struct intself. This forces a new transfer
        curBlock.blockId = 0;
        memcpy(&(curBlock.ver), &(avail->dataVer), 8);
        curBlock.type = avail->dataType;
        memcpy(&curDataInfo, (void *)avail, sizeof(struct AvailDataInfo));
        eraseUpdateBlock();
    }

    while (curDataInfo.dataSize) {
        wdt10s();
        if (curDataInfo.dataSize > BLOCK_DATA_SIZE) {
            // more than one block remaining
            dataRequestSize = BLOCK_DATA_SIZE;
        } else {
            // only one block remains
            dataRequestSize = curDataInfo.dataSize;
        }
        if (getDataBlock(dataRequestSize)) {
            // succesfully downloaded datablock, save to eeprom
            powerUp(INIT_EEPROM);
            saveUpdateBlockData(curBlock.blockId);
            powerDown(INIT_EEPROM);
            curBlock.blockId++;
            curDataInfo.dataSize -= dataRequestSize;
        } else {
            // failed to get the block we wanted, we'll stop for now, maybe resume later
            return false;
        }
    }
    // no more data, download complete
    return true;
}

uint16_t imageSize = 0;
static bool downloadImageDataToEEPROM(const struct AvailDataInfo *avail) {
    // check if we already started the transfer of this information & haven't completed it
    if (!memcmp((const void *)&avail->dataVer, (const void *)&curDataInfo.dataVer, 8) && curDataInfo.dataSize) {
        // looks like we did. We'll carry on where we left off.
        printf("restarting image download");
        curImgSlot = nextImgSlot;
    } else {
        // go to the next image slot
        nextImgSlot++;
        if (nextImgSlot >= imgSlots)
            nextImgSlot = 0;
        curImgSlot = nextImgSlot;
        printf("Saving to image slot %d\n", curImgSlot);
        drawWithLut = avail->dataTypeArgument;
        powerUp(INIT_EEPROM);
        uint8_t attempt = 5;
        while (attempt--) {
            if (eepromErase(getAddressForSlot(curImgSlot), (uint16_t)EEPROM_IMG_EACH))
                goto eraseSuccess;
        }
        NVIC_SystemReset();
    eraseSuccess:
        printf("new download, writing to slot %d\n", curImgSlot);

        // start, or restart the transfer. Copy data from the AvailDataInfo struct, and the struct intself. This forces a new transfer
        curBlock.blockId = 0;
        memcpy(&(curBlock.ver), &(avail->dataVer), 8);
        curBlock.type = avail->dataType;

        memcpy(&curDataInfo, (void *)avail, sizeof(struct AvailDataInfo));

        imageSize = curDataInfo.dataSize;
    }

    while (curDataInfo.dataSize) {
        wdt10s();
        if (curDataInfo.dataSize > BLOCK_DATA_SIZE) {
            // more than one block remaining
            dataRequestSize = BLOCK_DATA_SIZE;
        } else {
            // only one block remains
            dataRequestSize = curDataInfo.dataSize;
        }
        if (getDataBlock(dataRequestSize)) {
            // succesfully downloaded datablock, save to eeprom
            powerUp(INIT_EEPROM);
#ifdef DEBUGBLOCKS
            printf("Saving block %d to slot %d\n", curBlock.blockId, curImgSlot);
#endif
            saveImgBlockData(curImgSlot, curBlock.blockId);
            powerDown(INIT_EEPROM);
            curBlock.blockId++;
            curDataInfo.dataSize -= dataRequestSize;
        } else {
            // failed to get the block we wanted, we'll stop for now, probably resume later
            return false;
        }
    }
    // no more data, download complete

    // borrow the blockXferBuffer temporarily
    struct EepromImageHeader *eih = (struct EepromImageHeader *)blockXferBuffer;
    memcpy(&eih->version, &curDataInfo.dataVer, 8);
    eih->validMarker = EEPROM_IMG_VALID;
    eih->id = ++curHighSlotId;
    eih->size = imageSize;
    eih->dataType = curDataInfo.dataType;

#ifdef DEBUGBLOCKS
    printf("Now writing datatype 0x%02X to slot %d\n", curDataInfo.dataType, curImgSlot);
#endif
    powerUp(INIT_EEPROM);
    eepromWrite(getAddressForSlot(curImgSlot), eih, sizeof(struct EepromImageHeader));
    powerDown(INIT_EEPROM);

    return true;
}

bool processAvailDataInfo(struct AvailDataInfo *avail) {
    switch (avail->dataType) {
        case DATATYPE_IMG_BMP:
        case DATATYPE_IMG_DIFF:
        case DATATYPE_IMG_RAW_1BPP:
        case DATATYPE_IMG_RAW_2BPP:
            // check if this download is currently displayed or active
            if (curDataInfo.dataSize == 0 && !memcmp((const void *)&avail->dataVer, (const void *)&curDataInfo.dataVer, 8)) {
                // we've downloaded this already, we're guessing it's already displayed
                printf("currently shown image, send xfc\n");
                powerUp(INIT_RADIO);
                sendXferComplete();
                powerDown(INIT_RADIO);
                return true;
            }

            // check if we've seen this version before
            powerUp(INIT_EEPROM);
            curImgSlot = findSlot((uint8_t *)&(avail->dataVer));
            powerDown(INIT_EEPROM);
            if (curImgSlot != 0xFF) {
                // found a (complete)valid image slot for this version
                powerUp(INIT_RADIO);
                sendXferComplete();
                powerDown(INIT_RADIO);

                printf("already seen, drawing from eeprom slot %d\n", curImgSlot);

                // mark as completed and draw from EEPROM
                memcpy(&curDataInfo, (void *)avail, sizeof(struct AvailDataInfo));
                curDataInfo.dataSize = 0;  // mark as transfer not pending

                drawWithLut = avail->dataTypeArgument;
                wdt60s();
                powerUp(INIT_EPD | INIT_EEPROM);
                drawImageFromEeprom(curImgSlot);
                powerDown(INIT_EPD | INIT_EEPROM);
                return true;
            } else {
                // not found in cache, prepare to download
                printf("downloading to imgslot\n");
                drawWithLut = avail->dataTypeArgument;
                powerUp(INIT_EEPROM);
                if (downloadImageDataToEEPROM(avail)) {
                    printf("download complete!\n");
                    powerUp(INIT_RADIO);
                    sendXferComplete();
                    powerDown(INIT_RADIO);

                    wdt60s();
                    powerUp(INIT_EPD | INIT_EEPROM);
                    drawImageFromEeprom(curImgSlot);
                    powerDown(INIT_EPD | INIT_EEPROM);
                    return true;
                } else {
                    powerDown(INIT_EEPROM);
                    return false;
                }
            }
            break;
        case DATATYPE_FW_UPDATE:
            powerUp(INIT_EEPROM);
            if (downloadFWUpdate(avail)) {
                printf("firmware download complete, doing update.\n");

                powerUp(INIT_EPD);
                // uiPrvFullscreenMsg("Updating", NULL, NULL);

                powerUp(INIT_RADIO);
                sendXferComplete();
                powerDown(INIT_RADIO);

                powerUp(INIT_EEPROM);
                wdt60s();
                prvApplyUpdateIfNeeded();
            } else {
                return false;
            }
            break;
        case DATATYPE_NFC_URL_DIRECT:
        case DATATYPE_NFC_RAW_CONTENT: {
            return false;
            break;
        }
        case DATATYPE_CUSTOM_LUT_OTA:
            return false;
            break;
    }
    return true;
}

void initializeProto() {
    getNumSlots();
    curHighSlotId = getHighSlotId();
}