#define __packed
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "asmUtil.h"
#include "board.h"
#include "comms.h"
#include "cpu.h"
#include "eeprom.h"
#include "printf.h"
#include "proto.h"
#include "radio.h"
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
#define PKT_AVAIL_DATA_REQ_SHORT 0xE4
#define PKT_AVAIL_DATA_REQ 0xE5
#define PKT_AVAIL_DATA_INFO 0xE6
#define PKT_BLOCK_PARTIAL_REQUEST 0xE7
#define PKT_BLOCK_REQUEST_ACK 0xE9
#define PKT_BLOCK_REQUEST 0xE4
#define PKT_BLOCK_PART 0xE8
#define PKT_XFER_COMPLETE 0xEA
#define PKT_XFER_COMPLETE_ACK 0xEB
#define PKT_CANCEL_XFER 0xEC
#define PKT_PING 0xED
#define PKT_PONG 0xEE

struct AvailDataReq {
    uint8_t checksum;
    uint8_t lastPacketLQI;
    int8_t lastPacketRSSI;
    int8_t temperature;
    uint16_t batteryMv;
    uint8_t hwType;
    uint8_t wakeupReason;
    uint8_t capabilities;        // undefined, as of now
} __packed;


#define DATATYPE_NOUPDATE 0
#define DATATYPE_IMG 1
#define DATATYPE_IMGRAW 2
#define DATATYPE_UPDATE 3

struct AvailDataInfo {
    uint8_t checksum;
    uint64_t dataVer;              // MD5 of potential traffic
    uint32_t dataSize;              
    uint8_t dataType;          // allows for 16 different datatypes
    uint8_t dataTypeArgument;  // extra specification or instruction for the tag (LUT to be used for drawing image)
    uint16_t nextCheckIn;          // when should the tag check-in again? Measured in minutes
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

struct pendingData {
    struct AvailDataInfo availdatainfo;
    uint16_t attemptsLeft;
    uint8_t targetMac[8];
} __packed;

#define BLOCK_PART_DATA_SIZE 99
#define BLOCK_MAX_PARTS 42
#define BLOCK_DATA_SIZE 4096
#define BLOCK_XFER_BUFFER_SIZE BLOCK_DATA_SIZE + sizeof(struct blockData)
#define BLOCK_REQ_PARTS_BYTES 6  // BLOCK_MAX_PARTS / 8 + 1
#define MAX_PENDING_MACS 64
#define HOUSEKEEPING_INTERVAL 60UL

struct pendingData __xdata pendingDataArr[MAX_PENDING_MACS];

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
} __packed;

struct espBlockRequest {
    uint8_t checksum;
    uint64_t ver;
    uint8_t blockId;
} __packed;

struct espXferComplete {
    uint8_t checksum;
    uint8_t src[8];
} __packed;

struct espAvailDataReq {
    uint8_t checksum;
    uint8_t src[8];
    struct AvailDataReq adr;
} __packed;

#define TIMER_TICKS_PER_MS 1333UL
uint16_t __xdata version = 0x0006;
#define RAW_PKT_PADDING 2

static uint8_t __xdata mRxBuf[COMMS_MAX_PACKET_SZ];

uint8_t __xdata radiotxbuffer[128];
uint8_t __xdata radiorxbuffer[128];

uint8_t __xdata mSelfMac[8];

// serial stuff
uint8_t __xdata cmdbuffer[4];
uint8_t __xdata RXState = 0;
uint8_t __xdata serialbuffer[48];
uint8_t *__xdata serialbufferp;
uint8_t __xdata bytesRemain = 0;

static uint32_t __xdata housekeepingTimer;

struct blockRequest __xdata requestedData = {0};  // holds which data was requested by the tag

uint8_t __xdata dstMac[8];  // target for the block transfer
uint16_t __xdata dstPan;    //

static uint32_t __xdata blockStartTimer = 0;          // reference that holds when the AP sends the next block
extern bool __idata serialBypassActive;               // if the serial bypass is disabled, saves bytes straight to the block buffer
uint32_t __xdata nextBlockAttempt = 0;                // reference time for when the AP can request a new block from the ESP32
uint8_t seq = 0;                                      // holds current sequence number for transmission
uint8_t __xdata blockbuffer[BLOCK_XFER_BUFFER_SIZE];  // block transfer buffer
uint8_t lastAckMac[8] = {0};

// these variables hold the current mac were talking to
#define CONCURRENT_REQUEST_DELAY 1200UL * TIMER_TICKS_PER_MS
uint32_t __xdata lastBlockRequest = 0;
uint8_t __xdata lastBlockMac[8];

void sendXferCompleteAck(uint8_t *dst);
void sendCancelXfer(uint8_t *dst);

// tools
void addCRC(void *p, uint8_t len) {
    uint8_t total = 0;
    for (uint8_t c = 1; c < len; c++) {
        total += ((uint8_t *)p)[c];
    }
    ((uint8_t *)p)[0] = total;
    // pr("%d",total);
}
bool checkCRC(void *p, uint8_t len) {
    uint8_t total = 0;
    for (uint8_t c = 1; c < len; c++) {
        total += ((uint8_t *)p)[c];
    }
    return ((uint8_t *)p)[0] == total;
}
void dump(uint8_t *__xdata a, uint16_t __xdata l) {
    pr("        ");
#define ROWS 16
    for (uint8_t c = 0; c < ROWS; c++) {
        pr(" %02X", c);
        timerDelay(1333);
    }
    pr("\n--------");
    for (uint8_t c = 0; c < ROWS; c++) {
        pr("---");
        timerDelay(1333);
    }
    for (uint16_t c = 0; c < l; c++) {
        timerDelay(1333);
        if ((c % ROWS) == 0) {
            pr("\n0x%04X | ", c);
        }
        pr("%02X ", a[c]);
    }
    pr("\n--------");
    for (uint8_t c = 0; c < ROWS; c++) {
        pr("---");
        timerDelay(1333);
    }
    pr("\n");
}
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
int8_t findSlotForMac(const uint8_t *mac) {
    for (uint8_t __xdata c = 0; c < MAX_PENDING_MACS; c++) {
        if (u64_isEq((uint64_t __xdata *)mac, (uint64_t __xdata *)&(pendingDataArr[c].targetMac))) {  // this costs 1 sloc :(
            if (pendingDataArr[c].attemptsLeft != 0) {
                return c;
            }
        }
    }
    return -1;
}
int8_t findFreeSlot() {
    for (uint8_t __xdata c = 0; c < MAX_PENDING_MACS; c++) {
        if (pendingDataArr[c].attemptsLeft == 0) {
            return c;
        }
    }
    return -1;
}
int8_t findSlotForVer(const uint8_t *ver) {
    for (uint8_t __xdata c = 0; c < MAX_PENDING_MACS; c++) {
        // if (u64_isEq((uint64_t __xdata *)ver, (uint64_t __xdata *)&(pendingDataArr[c].availdatainfo.dataVer))) {
        if (memcmp(ver, ((uint8_t __xdata *)&(pendingDataArr[c].availdatainfo.dataVer)), 8) == 0) {
            if (pendingDataArr[c].attemptsLeft != 0) return c;
        }
    }
    return -1;
}
void deleteAllPendingDataForVer(const uint8_t *ver) {
    int8_t slot = -1;
    do {
        slot = findSlotForVer(ver);
        if (slot != -1) pendingDataArr[slot].attemptsLeft = 0;
    } while (slot != -1);
}

// processing serial data
#define ZBS_RX_WAIT_HEADER 0
#define ZBS_RX_WAIT_SDA 1
#define ZBS_RX_WAIT_CANCEL 2
void processSerial(uint8_t lastchar) {
    // uartTx(lastchar); echo
    switch (RXState) {
        case ZBS_RX_WAIT_HEADER:
            // shift characters in
            for (uint8_t c = 0; c < 3; c++) {
                cmdbuffer[c] = cmdbuffer[c + 1];
            }
            cmdbuffer[3] = lastchar;
            if (strncmp(cmdbuffer, "SDA>", 4) == 0) {
                RXState = ZBS_RX_WAIT_SDA;
                bytesRemain = sizeof(struct pendingData);
                serialbufferp = serialbuffer;
                break;
            }
            if (strncmp(cmdbuffer, "CXD>", 4) == 0) {
                RXState = ZBS_RX_WAIT_CANCEL;
                bytesRemain = sizeof(struct pendingData);
                serialbufferp = serialbuffer;
                break;
            }
            if (strncmp(cmdbuffer, "VER?", 4) == 0) {
                pr("VER>%04X\n", version);
            }
            if (strncmp(cmdbuffer, "RDY?", 4) == 0) {
                pr("RDY>");
            }
            if (strncmp(cmdbuffer, "RSET", 4) == 0) {
                wdtDeviceReset();
            }
            break;

        case ZBS_RX_WAIT_SDA:
            *serialbufferp = lastchar;
            serialbufferp++;
            bytesRemain--;
            if (bytesRemain == 0) {
                if (checkCRC(serialbuffer, sizeof(struct pendingData))) {
                    int8_t slot = findFreeSlot();
                    if (slot != -1) {
                        xMemCopyShort(&(pendingDataArr[slot]), serialbuffer, sizeof(struct pendingData));
                        pr("ACK>\n");
                    } else {
                        pr("NOQ>\n");
                    }
                } else {
                    pr("NOK>\n");
                }

                RXState = ZBS_RX_WAIT_HEADER;
            }
            break;
        case ZBS_RX_WAIT_CANCEL:
            *serialbufferp = lastchar;
            serialbufferp++;
            bytesRemain--;
            if (bytesRemain == 0) {
                if (checkCRC(serialbuffer, sizeof(struct pendingData))) {
                    struct pendingData *pd = (struct pendingData *)serialbuffer;
                    deleteAllPendingDataForVer((uint8_t *)&pd->availdatainfo.dataVer);
                    pr("ACK>\n");
                } else {
                    pr("NOK>\n");
                }

                RXState = ZBS_RX_WAIT_HEADER;
            }
            break;
    }
}

// sending data to the ESP
void espBlockRequest(const struct blockRequest *br) {
    struct espBlockRequest *__xdata ebr = (struct espBlockRequest *)blockbuffer;
    uartTx('R');
    uartTx('Q');
    uartTx('B');
    uartTx('>');
    // u64_copy(ebr->ver, br->ver);
    xMemCopy8(&(ebr->ver), &(br->ver));
    ebr->blockId = br->blockId;
    addCRC(ebr, sizeof(struct espBlockRequest));
    for (uint8_t c = 0; c < sizeof(struct espBlockRequest); c++) {
        uartTx(((uint8_t *)ebr)[c]);
    }
    // pr("req ebr ver: %02X%02X%02X%02X%02X%02X%02X%02X\n", ((uint8_t *)&(ebr->ver))[0], ((uint8_t *)&(ebr->ver))[1], ((uint8_t *)&(ebr->ver))[2], ((uint8_t *)&(ebr->ver))[3], ((uint8_t *)&(ebr->ver))[4], ((uint8_t *)&(ebr->ver))[5], ((uint8_t *)&(ebr->ver))[6], ((uint8_t *)&(ebr->ver))[7]);
    // pr("req br ver: %02X%02X%02X%02X%02X%02X%02X%02X\n", ((uint8_t *)&(br->ver))[0], ((uint8_t *)&(br->ver))[1], ((uint8_t *)&(br->ver))[2], ((uint8_t *)&(br->ver))[3], ((uint8_t *)&(br->ver))[4], ((uint8_t *)&(br->ver))[5], ((uint8_t *)&(br->ver))[6], ((uint8_t *)&(br->ver))[7]);
}
void espNotifyAvailDataReq(const struct AvailDataReq *adr, const uint8_t *src) {
    uartTx('A');
    uartTx('D');
    uartTx('R');
    uartTx('>');

    struct espAvailDataReq __xdata eadr = {0};
    xMemCopyShort((void *__xdata)eadr.src, (void *__xdata)src, 8);
    xMemCopyShort((void *__xdata) & eadr.adr, (void *__xdata)adr, sizeof(struct AvailDataReq));
    addCRC(&eadr, sizeof(struct espAvailDataReq));
    for (uint8_t c = 0; c < sizeof(struct espAvailDataReq); c++) {
        uartTx(((uint8_t *)eadr)[c]);
    }
}
void espNotifyXferComplete(const uint8_t *src) {
    struct espXferComplete exfc;
    xMemCopy8(&exfc.src, src);
    uartTx('X');
    uartTx('F');
    uartTx('C');
    uartTx('>');
    addCRC(&exfc, sizeof(exfc));
    for (uint8_t c = 0; c < sizeof(exfc); c++) {
        uartTx(((uint8_t *)exfc)[c]);
    }
}
void espNotifyTimeOut() {
}

// process data from tag
void processBlockRequest(const uint8_t *buffer, uint8_t forceBlockDownload) {
    struct MacFrameNormal *__xdata rxHeader = (struct MacFrameNormal *)buffer;
    struct blockRequest *__xdata blockReq = (struct blockRequest *)(buffer + sizeof(struct MacFrameNormal) + 1);
    if (!checkCRC(blockReq, sizeof(struct blockRequest))) return;

    // check if we're already talking to this mac
    if (memcmp(rxHeader->src, lastBlockMac, 8) == 0) {
        lastBlockRequest = timerGet();
    } else {
        // we weren't talking to this mac, see if there was a transfer in progress from another mac, recently
        if ((timerGet() - lastBlockRequest) > CONCURRENT_REQUEST_DELAY) {
            // mark this mac as the new current mac we're talking to
            xMemCopyShort((void *__xdata)lastBlockMac, (void *__xdata)rxHeader->src, 8);
            lastBlockRequest = timerGet();
            // memcpy(lastBlockRequest, rxHeader->src, 8);
        } else {
            // we're talking to another mac, let this mac know we can't accomodate another request right now
            pr("not accepting traffic from this tag\n");
            sendCancelXfer(rxHeader->src);
            return;
        }
    }

    // check if we have data for this mac
    if (findSlotForMac(rxHeader->src) == -1) {
        // no data for this mac, politely tell it to fuck off
        sendCancelXfer(rxHeader->src);
        return;
    }

    bool __xdata requestDataDownload = false;
    if ((blockReq->blockId != requestedData.blockId) || (!u64_isEq((const uint64_t __xdata *)&blockReq->ver, (const uint64_t __xdata *)&requestedData.ver))) {
        // requested block isn't already in the buffer
        requestDataDownload = true;
    } else {
        // requested block is already in the buffer
        if (forceBlockDownload) {
            if ((timerGet() - nextBlockAttempt) > (380 * TIMER_TICKS_PER_MS)) {
                requestDataDownload = true;
                pr("FORCED\n");
            } else {
                pr("IGNORED\n");
            }
        }
    }

    // copy blockrequest into requested data
    memcpy(&requestedData, blockReq, sizeof(struct blockRequest));

    struct MacFrameNormal *txHeader = (struct MacFrameNormal *)(radiotxbuffer + 1);
    struct blockRequestAck *blockRequestAck = (struct blockRequestAck *)(radiotxbuffer + sizeof(struct MacFrameNormal) + 2);
    radiotxbuffer[0] = sizeof(struct MacFrameNormal) + 1 + sizeof(struct blockRequestAck) + RAW_PKT_PADDING;
    radiotxbuffer[sizeof(struct MacFrameNormal) + 1] = PKT_BLOCK_REQUEST_ACK;

    if (blockStartTimer == 0) {
        if (requestDataDownload) {
            // check if we need to download the first block; we need to give the ESP32 some additional time to cache the file
            if (blockReq->blockId == 0) {
                blockRequestAck->pleaseWaitMs = 200;
            } else {
                blockRequestAck->pleaseWaitMs = 100;
            }
        } else {
            // block is already in buffer
            blockRequestAck->pleaseWaitMs = 50;
        }
    } else {
        blockRequestAck->pleaseWaitMs = 50;
    }
    blockStartTimer = timerGet() + blockRequestAck->pleaseWaitMs * TIMER_TICKS_PER_MS;

    memcpy(txHeader->src, mSelfMac, 8);
    memcpy(txHeader->dst, rxHeader->src, 8);

    txHeader->pan = rxHeader->pan;
    txHeader->fcs.frameType = 1;
    txHeader->fcs.panIdCompressed = 1;
    txHeader->fcs.destAddrType = 3;
    txHeader->fcs.srcAddrType = 3;
    txHeader->seq = seq++;

    addCRC((void *)blockRequestAck, sizeof(struct blockRequestAck));

    radioTx(radiotxbuffer);

    // save the target for the blockdata
    memcpy(dstMac, rxHeader->src, 8);
    dstPan = rxHeader->pan;

    if (requestDataDownload) {
        serialBypassActive = false;
        espBlockRequest(&requestedData);
        nextBlockAttempt = timerGet();
    }

    /*
        pr("Req: %d [", blockReq->blockId);
        for (uint8_t c = 0; c < BLOCK_MAX_PARTS; c++) {
            if ((c != 0) && (c % 8 == 0)) pr("][");
            if (blockReq->requestedParts[c / 8] & (1 << (c % 8))) {
                pr("R");
            } else {
                pr(".");
            }
        }
        pr("]\n");
        */
}
void processAvailDataReq(uint8_t *buffer) {
    struct MacFrameBcast *rxHeader = (struct MacFrameBcast *)buffer;
    struct AvailDataReq *availDataReq = (struct AvailDataReq *)(buffer + sizeof(struct MacFrameBcast) + 1);

    if (!checkCRC(availDataReq, sizeof(struct AvailDataReq)))
        return;

    // prepare tx buffer to send a response
    memset(radiotxbuffer, 0, sizeof(struct MacFrameNormal) + sizeof(struct AvailDataInfo) + 2);  // 120);
    struct MacFrameNormal *txHeader = (struct MacFrameNormal *)(radiotxbuffer + 1);
    struct AvailDataInfo *availDataInfo = (struct AvailDataInfo *)(radiotxbuffer + sizeof(struct MacFrameNormal) + 2);
    radiotxbuffer[0] = sizeof(struct MacFrameNormal) + 1 + sizeof(struct AvailDataInfo) + RAW_PKT_PADDING;
    radiotxbuffer[sizeof(struct MacFrameNormal) + 1] = PKT_AVAIL_DATA_INFO;

    // check to see if we have data available for this mac
    bool haveData = false;
    for (uint8_t __xdata c = 0; c < MAX_PENDING_MACS; c++) {
        if (pendingDataArr[c].attemptsLeft) {
            if (memcmp(pendingDataArr[c].targetMac, rxHeader->src, 8) == 0) {
                haveData = true;
                xMemCopyShort((void *__xdata)availDataInfo, &(pendingDataArr[c].availdatainfo), sizeof(struct AvailDataInfo));
                break;
            }
        }
    }

    // couldn't find data for this mac
    if (!haveData) availDataInfo->dataType = DATATYPE_NOUPDATE;

    xMemCopy8(txHeader->src, mSelfMac);
    xMemCopy8(txHeader->dst, rxHeader->src);
    txHeader->pan = rxHeader->dstPan;
    txHeader->fcs.frameType = 1;
    txHeader->fcs.panIdCompressed = 1;
    txHeader->fcs.destAddrType = 3;
    txHeader->fcs.srcAddrType = 3;
    txHeader->seq = seq++;
    addCRC(availDataInfo, sizeof(struct AvailDataInfo));
    radioTx(radiotxbuffer);
    memset(lastAckMac, 0, 8);  // reset lastAckMac, so we can record if we've received exactly one ack packet
    espNotifyAvailDataReq(availDataReq, rxHeader->src);
}
void processXferComplete(uint8_t *buffer) {
    struct MacFrameNormal *rxHeader = (struct MacFrameNormal *)buffer;
    sendXferCompleteAck(rxHeader->src);
    if (memcmp(lastAckMac, rxHeader->src, 8) != 0) {
        xMemCopyShort((void *__xdata)lastAckMac, (void *__xdata)rxHeader->src, 8);
        espNotifyXferComplete(rxHeader->src);
        int8_t slot = findSlotForMac(rxHeader->src);
        if (slot != -1) pendingDataArr[slot].attemptsLeft = 0;
    }
}

// send block data to the tag
void sendPart(uint8_t partNo) {
    struct MacFrameNormal *frameHeader = (struct MacFrameNormal *)(radiotxbuffer + 1);
    struct blockPart *blockPart = (struct blockPart *)(radiotxbuffer + sizeof(struct MacFrameNormal) + 2);
    memset(radiotxbuffer + 1, 0, sizeof(struct blockPart) + sizeof(struct MacFrameNormal));
    radiotxbuffer[sizeof(struct MacFrameNormal) + 1] = PKT_BLOCK_PART;
    radiotxbuffer[0] = sizeof(struct MacFrameNormal) + sizeof(struct blockPart) + BLOCK_PART_DATA_SIZE + 1 + RAW_PKT_PADDING;
    memcpy(frameHeader->src, mSelfMac, 8);
    memcpy(frameHeader->dst, dstMac, 8);
    blockPart->blockId = requestedData.blockId;
    blockPart->blockPart = partNo;
    memcpy(&(blockPart->data), blockbuffer + (partNo * BLOCK_PART_DATA_SIZE), BLOCK_PART_DATA_SIZE);
    addCRC(blockPart, sizeof(struct blockPart) + BLOCK_PART_DATA_SIZE);
    frameHeader->fcs.frameType = 1;
    frameHeader->fcs.panIdCompressed = 1;
    frameHeader->fcs.destAddrType = 3;
    frameHeader->fcs.srcAddrType = 3;
    frameHeader->seq = seq++;
    frameHeader->pan = dstPan;
    radioTx(radiotxbuffer);
}
void sendBlockData() {
    if(getBlockDataLength()==0){
        pr("Invalid block request received, 0 parts..\n");
        requestedData.requestedParts[0] |= 0x01;
    }
    uint8_t partNo = 0;
    while (partNo < BLOCK_MAX_PARTS) {
        for (uint8_t c = 0; (c < BLOCK_MAX_PARTS) && (partNo < BLOCK_MAX_PARTS); c++) {
            if (requestedData.requestedParts[c / 8] & (1 << (c % 8))) {
                sendPart(c);
                partNo++;
            }
        }
    }
}
void sendXferCompleteAck(uint8_t *dst) {
    struct MacFrameNormal *frameHeader = (struct MacFrameNormal *)(radiotxbuffer + 1);
    memset(radiotxbuffer + 1, 0, sizeof(struct blockPart) + sizeof(struct MacFrameNormal));
    radiotxbuffer[sizeof(struct MacFrameNormal) + 1] = PKT_XFER_COMPLETE_ACK;
    radiotxbuffer[0] = sizeof(struct MacFrameNormal) + 1 + RAW_PKT_PADDING;
    memcpy(frameHeader->src, mSelfMac, 8);
    memcpy(frameHeader->dst, dst, 8);
    frameHeader->fcs.frameType = 1;
    frameHeader->fcs.panIdCompressed = 1;
    frameHeader->fcs.destAddrType = 3;
    frameHeader->fcs.srcAddrType = 3;
    frameHeader->seq = seq++;
    frameHeader->pan = dstPan;
    radioTx(radiotxbuffer);
}
void sendCancelXfer(uint8_t *dst) {
    struct MacFrameNormal *frameHeader = (struct MacFrameNormal *)(radiotxbuffer + 1);
    memset(radiotxbuffer + 1, 0, sizeof(struct blockPart) + sizeof(struct MacFrameNormal));
    radiotxbuffer[sizeof(struct MacFrameNormal) + 1] = PKT_CANCEL_XFER;
    radiotxbuffer[0] = sizeof(struct MacFrameNormal) + 1 + RAW_PKT_PADDING;
    memcpy(frameHeader->src, mSelfMac, 8);
    memcpy(frameHeader->dst, dst, 8);
    frameHeader->fcs.frameType = 1;
    frameHeader->fcs.panIdCompressed = 1;
    frameHeader->fcs.destAddrType = 3;
    frameHeader->fcs.srcAddrType = 3;
    frameHeader->seq = seq++;
    frameHeader->pan = dstPan;
    radioTx(radiotxbuffer);
}
void sendPong(void *__xdata buf) {
    struct MacFrameBcast *rxframe = (struct MacFrameBcast *)buf;
    struct MacFrameNormal *frameHeader = (struct MacFrameNormal *)(radiotxbuffer + 1);
    memset(radiotxbuffer, 0, sizeof(struct MacFrameNormal) + 2);

    radiotxbuffer[sizeof(struct MacFrameNormal) + 1] = PKT_PONG;
    radiotxbuffer[0] = sizeof(struct MacFrameNormal) + 1 + RAW_PKT_PADDING;
    memcpy(frameHeader->src, mSelfMac, 8);
    memcpy(frameHeader->dst, rxframe->src, 8);

    frameHeader->fcs.frameType = 1;
    frameHeader->fcs.panIdCompressed = 1;
    frameHeader->fcs.destAddrType = 3;
    frameHeader->fcs.srcAddrType = 3;
    frameHeader->seq = seq++;
    frameHeader->pan = rxframe->srcPan;

    radioTx(radiotxbuffer);
}

// main loop
void main(void) {
    clockingAndIntsInit();
    timerInit();
    boardInit();
    P0FUNC = 0b11001111;  // enable uart tx/rx and SPI bus functions
    irqsOn();
    boardInitStage2();
    requestedData.blockId = 0xFF;
    if (!boardGetOwnMac(mSelfMac)) {
        pr("failed to get MAC. Aborting\n");
        while (1)
            ;
    }
    for (uint8_t c = 0; c < 8; c++) {
        mSelfMac[c] = c;
    }

    // clear the array with pending information
    memset(pendingDataArr, 0, sizeof(pendingDataArr));

    radioInit();
    radioRxFilterCfg(mSelfMac, 0x10000, PROTO_PAN_ID);

    // init the "random" number generation unit
    rndSeed(mSelfMac[0] ^ (uint8_t)timerGetLowBits(), mSelfMac[1]);
    // wdtSetResetVal(0xFD0DCF);
    // wdtOn();
    radioSetChannel(RADIO_FIRST_CHANNEL);
    radioSetTxPower(10);
    radioRxEnable(true, true);

    // uint8_t __xdata fromMac[8];
    pr("RDY>\n");

    housekeepingTimer = timerGet();

    // really... if I do the call below, it'll cost me 8 bytes IRAM. Not the kind of 'optimization' I ever dreamed of doing
    // pr("MAC>%02X%02X%02X%02X%02X%02X%02X%02X\n", mSelfMac[0], mSelfMac[1], mSelfMac[2], mSelfMac[3], mSelfMac[4], mSelfMac[5], mSelfMac[6], mSelfMac[7]);
    pr("MAC>%02X%02X", mSelfMac[0], mSelfMac[1]);
    pr("%02X%02X", mSelfMac[2], mSelfMac[3]);
    pr("%02X%02X", mSelfMac[4], mSelfMac[5]);
    pr("%02X%02X\n", mSelfMac[6], mSelfMac[7]);

    pr("VER>%04X\n", version);
    while (1) {
        while ((timerGet() - housekeepingTimer) < ((TIMER_TICKS_PER_SECOND * HOUSEKEEPING_INTERVAL) - 100 * TIMER_TICKS_PER_MS)) {
            int8_t ret = commsRxUnencrypted(radiorxbuffer);
            if (ret > 1) {
                // received a packet, lets see what it is
                switch (getPacketType(radiorxbuffer)) {
                    case PKT_AVAIL_DATA_REQ:
                        processAvailDataReq(radiorxbuffer);
                        break;
                    case PKT_BLOCK_REQUEST:
                        processBlockRequest(radiorxbuffer, 1);
                        break;
                    case PKT_BLOCK_PARTIAL_REQUEST:
                        processBlockRequest(radiorxbuffer, 0);
                        break;
                    case PKT_XFER_COMPLETE:
                        processXferComplete(radiorxbuffer);
                        break;
                    case PKT_PING:
                        sendPong(radiorxbuffer);
                        break;
                    default:
                        pr("t=%02X\n", getPacketType(radiorxbuffer));
                        break;
                }
            }
            while (uartBytesAvail()) {
                processSerial(uartRx());
            }
            if (blockStartTimer) {
                // BUG: uint32 overflowing; this will break every once in a while. Don't know how to fix this other than ugly global variables
                if (timerGet() > blockStartTimer) {
                    sendBlockData();
                    blockStartTimer = 0;
                }
            }
        }
        pr("housekeeping...");

        for (uint8_t __xdata c = 0; c < MAX_PENDING_MACS; c++) {
            if (pendingDataArr[c].attemptsLeft == 1) {
                espNotifyTimeOut();
                pendingDataArr[c].attemptsLeft = 0;
            } else if (pendingDataArr[c].attemptsLeft > 1) {
                pendingDataArr[c].attemptsLeft--;
                if (pendingDataArr[c].availdatainfo.nextCheckIn) pendingDataArr[c].availdatainfo.nextCheckIn--;
            }
        }
        housekeepingTimer = timerGet();
    }
}