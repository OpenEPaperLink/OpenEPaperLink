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

struct pendingData {
    struct AvailDataInfo availdatainfo;
    uint8_t attemptsLeft;
    uint8_t targetMac[8];
    uint8_t includedThisBurst : 1;
} __packed;

struct burstMacData {
    uint16_t offset;
    uint8_t targetMac[8];
} __packed;

#define BLOCK_PART_DATA_SIZE 99
#define BLOCK_MAX_PARTS 42
#define BLOCK_DATA_SIZE 4096
#define BLOCK_XFER_BUFFER_SIZE BLOCK_DATA_SIZE + sizeof(struct blockData)
// #define BLOCK_XFER_BUFFER_SIZE 4096 + 4
#define BLOCK_REQ_PARTS_BYTES 6  // BLOCK_MAX_PARTS / 8 + 1
#define MAX_MACS_PER_SYNC 2
#define MAX_PENDING_MACS 10
#define SYNC_BURST_INTERVAL 30UL

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
    uint16_t blockSizeMs;
    uint16_t pleaseWaitMs;
    uint8_t cancelXfer;
} __packed;

struct espPendingData {
    uint8_t checksum;
    struct pendingData pending;
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

struct espJoinNetwork {
    uint8_t checksum;
    uint8_t src[8];
} __packed;


// used to transmit AP update information - flashing the firmware
struct espSaveUpdateBlock {
    uint8_t checksum;
    uint8_t blockId;
    uint16_t blockChecksum;
} __packed;

#define TIMER_TICKS_PER_MS 1333UL
uint16_t __xdata version = 0x0015;
#define RAW_PKT_PADDING 2

static uint8_t __xdata mRxBuf[COMMS_MAX_PACKET_SZ];

uint8_t __xdata radiotxbuffer[128];
uint8_t __xdata radiorxbuffer[128];

uint32_t __xdata burstLengthMs;  // stores how fast we were able to send a syncburst
uint8_t __xdata mSelfMac[8];

// serial stuff
uint8_t __xdata cmdbuffer[4];
uint8_t __xdata RXState = 0;
uint8_t __xdata serialbuffer[48];
uint8_t *__xdata serialbufferp;
uint8_t __xdata bytesRemain = 0;

static uint32_t __xdata burstIntervalTimer;
bool __xdata blockRequestInProgress = false;  // set if we get a CRC error, or should do a full request for other reasons

struct blockRequest __xdata requestedData = {0};
uint8_t __xdata dstMac[8];
uint16_t __xdata dstPan;
static uint32_t __xdata blockStartTimer = 0;

uint8_t seq = 0;  // holds current sequence number for transmission

uint8_t __xdata blockbuffer[BLOCK_XFER_BUFFER_SIZE];

#define SYNC_BURST_LENGTH 142  //(about 250ms)

void sendXferCompleteAck(uint8_t *dst);

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
uint8_t getBlockDataLength() {
    uint8_t partNo = 0;
    uint8_t rounds = 0;
    while (partNo < BLOCK_MAX_PARTS) {
        for (uint8_t c = 0; (c < BLOCK_MAX_PARTS) && (partNo < BLOCK_MAX_PARTS); c++) {
            if (requestedData.requestedParts[c / 8] & (1 << (c % 8))) {
                partNo++;
            }
        }
        rounds++;
        if (rounds == 4) {
            return partNo;
        }
    }
    return partNo;
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
uint16_t averageXmitDelay(uint16_t xfersize) {
    // returns the about maximum time the base should spend on the transfer; about 6 seconds (6000ms) for a 8000 byte transfer. This is * 3 / 4, pretty conservative
    // this includes a lot of retransmissions.
    return (xfersize * 3) / 5;
}

// serial update
void eraseUpdateBlock() {
    eepromErase(EEPROM_UPDATA_AREA_START, EEPROM_UPDATE_AREA_LEN / EEPROM_ERZ_SECTOR_SZ);
}
bool validateBlockData() {
    struct blockData *bd = (struct blockData *)blockbuffer;
    // pr("expected len = %04X, checksum=%04X\n", bd->size, bd->checksum);
    uint16_t t = 0;
    for (uint16_t c = 0; c < bd->size; c++) {
        t += bd->data[c];
    }
    return bd->checksum == t;
}
void saveBlock(uint8_t blockId) {
    if (!eepromWrite(EEPROM_UPDATA_AREA_START + (blockId * BLOCK_DATA_SIZE), blockbuffer + sizeof(struct blockData), BLOCK_DATA_SIZE))
        pr("EEPROM write failed\n");
}
void performUpdate() {
    eepromReadStart(EEPROM_UPDATA_AREA_START);
    selfUpdate();
}
uint16_t getBlockChecksum() {
    struct blockData *bd = (struct blockData *)blockbuffer;
    return bd->checksum;
}

// pendingdata slot stuff
int8_t findSlotForMac(const uint8_t *mac) {
    for (uint8_t __xdata c = 0; c < MAX_PENDING_MACS; c++) {
        if (u64_isEq((uint64_t __xdata *)mac, (uint64_t __xdata *)&(pendingDataArr[c].targetMac))) {  // this costs 1 sloc :(
            return c;
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

// processing serial data
#define ZBS_RX_WAIT_HEADER 0
#define ZBS_RX_WAIT_SDA 1
#define ZBS_RX_WAIT_UPDBLOCK 2
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
            if (strncmp(cmdbuffer, "VER?", 4) == 0) {
                pr("VER>%04X\n", version);
            }
            if (strncmp(cmdbuffer, "RDY?", 4) == 0) {
                pr("RDY>");
            }
            if (strncmp(cmdbuffer, "RSET", 4) == 0) {
                wdtDeviceReset();
            }
            if (strncmp(cmdbuffer, "ERAS", 4) == 0) {
                // erase update space
                eraseUpdateBlock();
                pr("EROK\n");
            }
            if (strncmp(cmdbuffer, "UPDA", 4) == 0) {
                // perform update!
                pr("OK>>\n");
                performUpdate();
            }
            if (strncmp(cmdbuffer, "SUBL", 4) == 0) {
                // save update block
                RXState = ZBS_RX_WAIT_UPDBLOCK;
                bytesRemain = sizeof(struct espSaveUpdateBlock);
                serialbufferp = serialbuffer;
                break;
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
                        pr("NOK>\n");
                    }
                } else {
                    pr("NOK>\n");
                }

                RXState = ZBS_RX_WAIT_HEADER;
            }
            break;
        case ZBS_RX_WAIT_UPDBLOCK:
            *serialbufferp = lastchar;
            serialbufferp++;
            bytesRemain--;
            if (bytesRemain == 0) {
                if (checkCRC(serialbuffer, sizeof(struct espSaveUpdateBlock))) {
                    if (validateBlockData()) {
                        const struct espSaveUpdateBlock *updb = (struct espSaveUpdateBlock *)serialbuffer;
                        if (updb->blockChecksum == getBlockChecksum()) {
                            saveBlock(updb->blockId);
                            pr("BLOK\n");
                        } else {
                            pr("BLFL> - block checksum doesn't match\n");
                        }
                    } else {
                        pr("BLFL> - block doesn't validate\n");
                    }
                } else {
                    // block failed download
                    pr("BLFL> - update block data checksum failed\n");
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
void espNotifyAvailDataReq(const struct AvailDataReq *adr) {
    uartTx('A');
    uartTx('D');
    uartTx('R');
    uartTx('>');
    for (uint8_t c = 0; c < sizeof(struct AvailDataReq); c++) {
        uartTx(((uint8_t *)adr)[c]);
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
void espNotifyJoinNetwork(const uint8_t *src) {
    struct espJoinNetwork ejn;
    xMemCopy8(&ejn.src, src);
    uartTx('T');
    uartTx('J');
    uartTx('N');
    uartTx('>');
    addCRC(&ejn, sizeof(ejn));
    for (uint8_t c = 0; c < sizeof(ejn); c++) {
        uartTx(((uint8_t *)ejn)[c]);
    }
}

// process data from tag
void processBlockRequest(const uint8_t *buffer, uint8_t forceBlockDownload) {
    struct MacFrameNormal *__xdata rxHeader = (struct MacFrameNormal *)buffer;
    struct blockRequest *__xdata blockReq = (struct blockRequest *)(buffer + sizeof(struct MacFrameNormal) + 1);
    if (!checkCRC(blockReq, sizeof(struct blockRequest))) return;
    // todo: actually do something with the block request
    // uint32_t __xdata curTimerValue = *t;
    bool __xdata requestDataDownload = false;
    // if ((blockReq->blockId != requestedData.blockId) || (blockReq->ver != requestedData.ver)) {
    if ((blockReq->blockId != requestedData.blockId) || (!u64_isEq((const uint64_t __xdata *)&blockReq->ver, (const uint64_t __xdata *)&requestedData.ver))) {
        // requested block isn't already in the buffer
        requestDataDownload = true;
    } else {
        // requested block is already in the buffer
        if (forceBlockDownload) {
            // force a download anyway; probably some error in the transfer between ESP32->AP-tag
            if (!blockRequestInProgress) {
                // block download from ESP32 not in progress
                blockRequestInProgress = true;
                requestDataDownload = true;
            } else {
                // block download from ESP32 requested, but already in progress. Maybe the transfer stalled for some reason; have the ESP32 send us some bytes.
                uartTx('R');
                uartTx('Q');
                uartTx('Q');
                uartTx('>');
            }
        }
    }

    memcpy(&requestedData, blockReq, sizeof(struct blockRequest));

    struct MacFrameNormal *txHeader = (struct MacFrameNormal *)(radiotxbuffer + 1);
    struct blockRequestAck *blockRequestAck = (struct blockRequestAck *)(radiotxbuffer + sizeof(struct MacFrameNormal) + 2);
    radiotxbuffer[0] = sizeof(struct MacFrameNormal) + 1 + sizeof(struct blockRequestAck) + RAW_PKT_PADDING;
    radiotxbuffer[sizeof(struct MacFrameNormal) + 1] = PKT_BLOCK_REQUEST_ACK;

    // TODO: get this data from somnewhere, dynamically. Depending on cache status we might need to return a longer or shorter wait period
    if (blockStartTimer == 0) {
        if (requestDataDownload) {
            // check if we need to download the first block; we need to give the ESP32 some additional time to cache the file
            if (blockReq->blockId == 0) {
                blockRequestAck->pleaseWaitMs = 200;
            } else {
                blockRequestAck->pleaseWaitMs = 100;
            }
            blockStartTimer = timerGet() + blockRequestAck->pleaseWaitMs * TIMER_TICKS_PER_MS;
        } else {
            blockRequestAck->pleaseWaitMs = 30;
            blockStartTimer = timerGet() + blockRequestAck->pleaseWaitMs * TIMER_TICKS_PER_MS;
        }
    } else {
        blockRequestAck->pleaseWaitMs = (blockStartTimer - timerGet()) / TIMER_TICKS_PER_MS;
        if (blockRequestAck->pleaseWaitMs < 30) {
            blockRequestAck->pleaseWaitMs = 30;
            blockStartTimer = timerGet() + blockRequestAck->pleaseWaitMs * TIMER_TICKS_PER_MS;
        }
    }
    blockRequestAck->blockSizeMs = 15 + 15 + (getBlockDataLength() * 245) / BLOCK_MAX_PARTS;
    blockRequestAck->cancelXfer = 0;
    // pr("s=%d\n", blockRequestAck->blockSizeMs);

    memcpy(txHeader->src, mSelfMac, 8);
    memcpy(txHeader->dst, rxHeader->src, 8);
    memcpy(dstMac, rxHeader->src, 8);
    dstPan = rxHeader->pan;

    txHeader->pan = rxHeader->pan;
    txHeader->fcs.frameType = 1;
    txHeader->fcs.panIdCompressed = 1;
    txHeader->fcs.destAddrType = 3;
    txHeader->fcs.srcAddrType = 3;
    txHeader->seq = seq++;

    addCRC((void *)blockRequestAck, sizeof(struct blockRequestAck));

    radioTx(radiotxbuffer);
    radioTx(radiotxbuffer);
    radioTx(radiotxbuffer);

    // pr("req blockreq: %02X%02X%02X%02X%02X%02X%02X%02X\n", ((uint8_t *)&(blockReq->ver))[0], ((uint8_t *)&(blockReq->ver))[1], ((uint8_t *)&(blockReq->ver))[2], ((uint8_t *)&(blockReq->ver))[3], ((uint8_t *)&(blockReq->ver))[4], ((uint8_t *)&(blockReq->ver))[5], ((uint8_t *)&(blockReq->ver))[6], ((uint8_t *)&(blockReq->ver))[7]);

    if (requestDataDownload) {
        //        espBlockRequest(blockReq);
        espBlockRequest(&requestedData);
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
    struct MacFrameNormal *rxHeader = (struct MacFrameNormal *)buffer;
    struct AvailDataReq *availDataReq = (struct AvailDataReq *)(buffer + sizeof(struct MacFrameNormal) + 1);

    if (!checkCRC(availDataReq, sizeof(struct AvailDataReq))) return;

    // prepare tx buffer to send a response
    memset(radiotxbuffer, 0, 120);
    struct MacFrameNormal *txHeader = (struct MacFrameNormal *)(radiotxbuffer + 1);
    struct AvailDataInfo *availDataInfo = (struct AvailDataInfo *)(radiotxbuffer + sizeof(struct MacFrameNormal) + 2);
    radiotxbuffer[0] = sizeof(struct MacFrameNormal) + 1 + sizeof(struct AvailDataInfo) + RAW_PKT_PADDING;
    radiotxbuffer[sizeof(struct MacFrameNormal) + 1] = PKT_AVAIL_DATA_INFO;

    // check to see if we were addressing this mac in this burst, and if yes, copy availdatainfo to the tx buffer
    bool haveData = false;
    for (uint8_t __xdata c = 0; c < MAX_PENDING_MACS; c++) {
        if (pendingDataArr[c].includedThisBurst == 1) {
            if (memcmp(pendingDataArr[c].targetMac, rxHeader->src, 8) == 0) {
                haveData = true;
                xMemCopyShort((void *__xdata)availDataInfo, &(pendingDataArr[c].availdatainfo), sizeof(struct AvailDataInfo));
                break;
            }
        }
    }
    if (!haveData) return;

    xMemCopy8(txHeader->src, mSelfMac);
    xMemCopy8(txHeader->dst, rxHeader->src);

    // memcpy(txHeader->src, mSelfMac, 8);
    // memcpy(txHeader->dst, rxHeader->src, 8);
    txHeader->pan = rxHeader->pan;
    txHeader->fcs.frameType = 1;
    txHeader->fcs.panIdCompressed = 1;
    txHeader->fcs.destAddrType = 3;
    txHeader->fcs.srcAddrType = 3;
    txHeader->seq = seq++;
    addCRC(availDataInfo, sizeof(struct AvailDataInfo));
    radioTx(radiotxbuffer);
    espNotifyAvailDataReq(availDataReq);
}
void processXferComplete(uint8_t *buffer) {
    struct MacFrameNormal *rxHeader = (struct MacFrameNormal *)buffer;
    sendXferCompleteAck(rxHeader->src);
    espNotifyXferComplete(rxHeader->src);
    uint8_t slot = findSlotForMac(rxHeader->src);
    pendingDataArr[slot].attemptsLeft = 0;
}

// send crap to the tag
void prepareMacForSyncBurst() {
    // mark all pending macs as 'not included'
    for (uint8_t __xdata c = 0; c < MAX_PENDING_MACS; c++) {
        pendingDataArr[c].includedThisBurst = 0;
    }

    memset(radiotxbuffer, 0, sizeof(struct MacFrameBcast) + 1 + 20 + 2);  // TODO, optimize

    struct burstMacData *__xdata macdata = (struct burstMacData * __xdata)(((uint8_t *)radiotxbuffer) + sizeof(struct MacFrameBcast) + 3);  // total len, pkt type sync, mac-count

    uint8_t __xdata count = 0;
    uint16_t __xdata currOffset = 500;
    while (count < MAX_MACS_PER_SYNC) {
        uint8_t __xdata tempmax = 0;
        int8_t __xdata maxid = -1;
        for (uint8_t __xdata c = 0; c < MAX_PENDING_MACS; c++) {
            // check if this mac is already included in the planned sync burst
            if (pendingDataArr[c].includedThisBurst == 0) {
                // check if this current amount of 'attemptsleft' is the current maximum
                if (pendingDataArr[c].attemptsLeft && (pendingDataArr[c].attemptsLeft > tempmax)) {
                    uint16_t timeoffset = SYNC_BURST_INTERVAL * 1000;
                    timeoffset -= 1000;
                    // check if the estimated transmission would fit in the remaining time
                    if (currOffset + averageXmitDelay(pendingDataArr[c].availdatainfo.dataSize) < timeoffset) {
                        tempmax = pendingDataArr[c].attemptsLeft;
                        maxid = c;
                    }
                }
            }
        }
        if (maxid == -1) {
            // didn't find any valid mac's to add to the syncburst
            return;
        } else {
            // found a pending-data info struct with the highest amount of attemptsLeft. Add this to the sync burst
            // make sure we don't add this pending data mac twice
            pendingDataArr[maxid].includedThisBurst = 1;
            pendingDataArr[maxid].attemptsLeft--;
            xMemCopyShort(macdata[count].targetMac, pendingDataArr[maxid].targetMac, 8);
            macdata[count].offset = currOffset;
            currOffset += averageXmitDelay(pendingDataArr[maxid].availdatainfo.dataSize);
            count++;
            *((uint8_t *)radiotxbuffer + sizeof(struct MacFrameBcast) + 2) = count;
        }
    }
}
void sendSyncBurst() {
    struct MacFrameBcast *txframe = (struct MacFrameBcast *)(radiotxbuffer + 1);
    memcpy(txframe->src, mSelfMac, 8);
    *((uint8_t *)txframe + sizeof(struct MacFrameBcast)) = PKT_SYNC_BURST;
    txframe->fcs.frameType = 1;
    txframe->fcs.secure = 0;
    txframe->fcs.framePending = 0;
    txframe->fcs.ackReqd = 0;
    txframe->fcs.panIdCompressed = 0;
    txframe->fcs.destAddrType = 2;
    txframe->fcs.frameVer = 0;
    txframe->fcs.srcAddrType = 3;
    txframe->seq = 0;
    txframe->dstPan = 0xFFFF;
    txframe->dstAddr = 0xFFFF;
    txframe->srcPan = 0x4447;

    radiotxbuffer[0] = sizeof(struct MacFrameBcast) + 1 + (MAX_MACS_PER_SYNC * sizeof(struct burstMacData)) + 2;
    pr("BST>\n");
    burstLengthMs = timerGet();
    for (uint16_t c = 0; c < SYNC_BURST_LENGTH; c++) {
        radioTx(radiotxbuffer);
        txframe->seq++;
    }
    burstLengthMs = (timerGet() - burstLengthMs) / 1333;
    // pr("atxc in %lu,\n", burstLengthMs);
}
void sendTimingReply(void *__xdata buf) {
    struct MacFrameBcast *rxframe = (struct MacFrameBcast *)buf;

    struct MacFrameNormal *frameHeader = (struct MacFrameNormal *)(radiotxbuffer + 1);
    struct timingResponse *response = (struct timingResponse *)(radiotxbuffer + sizeof(struct MacFrameNormal) + 2);
    radiotxbuffer[sizeof(struct MacFrameNormal) + 1] = PKT_TIMING_RESPONSE;
    radiotxbuffer[0] = sizeof(struct MacFrameNormal) + sizeof(struct timingResponse) + 1 + RAW_PKT_PADDING;
    memset(response, 0, sizeof(struct timingResponse) + sizeof(struct MacFrameNormal) + 1);
    memcpy(frameHeader->src, mSelfMac, 8);
    memcpy(frameHeader->dst, rxframe->src, 8);

    response->timerValue = timerGet();
    response->burstInterval = SYNC_BURST_INTERVAL * 1000;
    response->burstLength = SYNC_BURST_LENGTH;  // in packets; due to use of sequence field, limited to a 256-packet burst
    response->burstLengthMs = burstLengthMs;    // burst length in ms
    response->burstIntervalRemaining = (TIMER_TICKS_PER_SECOND * SYNC_BURST_INTERVAL) - (timerGet() - burstIntervalTimer);

    frameHeader->fcs.frameType = 1;
    frameHeader->fcs.panIdCompressed = 1;
    frameHeader->fcs.destAddrType = 3;
    frameHeader->fcs.srcAddrType = 3;
    frameHeader->seq = seq++;
    frameHeader->pan = rxframe->srcPan;
    addCRC(response, sizeof(struct timingResponse));
    radioTx(radiotxbuffer);
    espNotifyJoinNetwork(rxframe->src);
}
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
    uint8_t partNo = 0;
    uint8_t rounds = 0;
    while (partNo < BLOCK_MAX_PARTS) {
        for (uint8_t c = 0; (c < BLOCK_MAX_PARTS) && (partNo < BLOCK_MAX_PARTS); c++) {
            if (requestedData.requestedParts[c / 8] & (1 << (c % 8))) {
                sendPart(c);
                partNo++;
            }
        }
        rounds++;
        if (rounds == 4) {
            return;
        }
    }
    // TODO: not sure if we need this, probably not. Not sure why I added it in the first place
    commsRxUnencrypted(radiorxbuffer);
    commsRxUnencrypted(radiorxbuffer);
    commsRxUnencrypted(radiorxbuffer);
    commsRxUnencrypted(radiorxbuffer);
    commsRxUnencrypted(radiorxbuffer);
    commsRxUnencrypted(radiorxbuffer);
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
        // mSelfMac[c] = c;
    }

    if (!eepromInit()) {  // we'll need the eeprom here, init it.
        pr("failed to init eeprom\n");
        return;
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

    // send first burst, used to characterize the packet TX speed
    burstIntervalTimer = timerGet();  // + (TIMER_TICKS_PER_SECOND * SYNC_BURST_INTERVAL);
    sendSyncBurst();

    // really... if I do the call below, it'll cost me 8 bytes IRAM. Not the kind of 'optimization' I ever dreamed of doing
    // pr("MAC>%02X%02X%02X%02X%02X%02X%02X%02X\n", mSelfMac[0], mSelfMac[1], mSelfMac[2], mSelfMac[3], mSelfMac[4], mSelfMac[5], mSelfMac[6], mSelfMac[7]);
    pr("MAC>%02X%02X", mSelfMac[0], mSelfMac[1]);
    pr("%02X%02X", mSelfMac[2], mSelfMac[3]);
    pr("%02X%02X", mSelfMac[4], mSelfMac[5]);
    pr("%02X%02X\n", mSelfMac[6], mSelfMac[7]);

    pr("VER>%04X\n", version);
    while (1) {
        radioRxFlush();

        // spend about 30 seconds - 100ms in this while loop. The last 100ms are for preparing the sync burst
        while ((timerGet() - burstIntervalTimer) < ((TIMER_TICKS_PER_SECOND * SYNC_BURST_INTERVAL) - 100 * TIMER_TICKS_PER_MS)) {
            int8_t ret = commsRxUnencrypted(radiorxbuffer);
            if (ret > 1) {
                // received a packet, lets see what it is
                switch (getPacketType(radiorxbuffer)) {
                    case PKT_TIMING_REQ:
                        sendTimingReply(radiorxbuffer);
                        break;
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
                        //
                    default:
                        // pr("other packet...type = %02X\n", getPacketType(radiorxbuffer));
                        // dump(radiorxbuffer, 128);
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

        for (uint8_t __xdata c = 0; c < MAX_PENDING_MACS; c++) {
            if (pendingDataArr[c].attemptsLeft == 1) {
                espNotifyTimeOut();
                pendingDataArr[c].attemptsLeft = 0;
            }
        }
        prepareMacForSyncBurst();

        while ((timerGet() - burstIntervalTimer) < (TIMER_TICKS_PER_SECOND * SYNC_BURST_INTERVAL)) {
            // wait here for maximum burst-start accuracy
        }
        burstIntervalTimer = timerGet();
        sendSyncBurst();
    }
}
