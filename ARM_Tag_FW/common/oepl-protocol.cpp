
// download-stuff
static PERSISTENTVAR struct blockRequest curBlock = {0};  // used by the block-requester, contains the next request that we'll send
static PERSISTENTVAR uint64_t curDispDataVer = 0;
static PERSISTENTVAR struct AvailDataInfo xferDataInfo = {0};  // last 'AvailDataInfo' we received from the AP
static PERSISTENTVAR bool requestPartialBlock = false;         // if we should ask the AP to get this block from the host or not
#define BLOCK_TRANSFER_ATTEMPTS 5

PERSISTENTVAR uint8_t xferImgSlot = 0xFF;
PERSISTENTVAR uint8_t curImgSlot = 0xFF;
static PERSISTENTVAR uint32_t curHighSlotId = 0;
static PERSISTENTVAR uint8_t nextImgSlot = 0;
static PERSISTENTVAR uint8_t imgSlots = 0;

// stuff we need to keep track of related to the network/AP
uint8_t APmac[8] = {0};
uint16_t APsrcPan = 0;
PERSISTENTVAR uint8_t mSelfMac[8] = {0};
static PERSISTENTVAR uint8_t seq = 0;
volatile uint8_t PERSISTENTVAR currentChannel = 0;

// buffer we use to prepare/read packets
static uint8_t inBuffer[128] = {0};
static uint8_t outBuffer[128] = {0};

// tools
static uint8_t getPacketType(const void *buffer) {
    const struct MacFcs *fcs = (MacFcs *)buffer;
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
    const struct MacFcs *fcs = (MacFcs *)buffer;
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
        t = millis() + PING_REPLY_WINDOW;
        while (millis() < t) {
            int8_t ret = HAL_PacketRX(inBuffer);
            if (ret > 1) {
                //                dump(inBuffer+sizeof(struct MacFrameNormal),32);
                if ((inBuffer[sizeof(struct MacFrameNormal) + 1] == channel) && (getPacketType(inBuffer) == PKT_PONG)) {
                    if (pktIsUnicast(inBuffer)) {
                        struct MacFrameNormal *f = (struct MacFrameNormal *)inBuffer;
                        memcpy(APmac, f->src, 8);
                        APsrcPan = f->pan;
                        return c;
                    }
                }
            }
            HAL_msDelay(10);
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
    availreq->hwType = tag.OEPLtype;
    availreq->wakeupReason = wakeUpReason;
    availreq->lastPacketRSSI = mLastRSSI;
    availreq->lastPacketLQI = mLastLqi;
    availreq->temperature = temperature;
    availreq->batteryMv = batteryVoltage;
    availreq->capabilities = capabilities;
    availreq->currentChannel = currentChannel;
    availreq->tagSoftwareVersion = FW_VERSION;
    addCRC(availreq, sizeof(struct AvailDataReq));
    commsTxNoCpy(outBuffer);
}
struct AvailDataInfo *getAvailDataInfo() {
    radioRxEnable(true);
    uint32_t t;
    for (uint8_t c = 0; c < DATA_REQ_MAX_ATTEMPTS; c++) {
        sendAvailDataReq();
        t = millis() + DATA_REQ_RX_WINDOW_SIZE;
        while (millis() < t) {
            int8_t ret = HAL_PacketRX(inBuffer);
            if (ret > 1) {
                if (getPacketType(inBuffer) == PKT_AVAIL_DATA_INFO) {
                    if (checkCRC(inBuffer + sizeof(struct MacFrameNormal) + 1, sizeof(struct AvailDataInfo))) {
                        struct MacFrameNormal *f = (struct MacFrameNormal *)inBuffer;
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
struct AvailDataInfo *getShortAvailDataInfo() {
    radioRxEnable(true);
    uint32_t t;
    for (uint8_t c = 0; c < DATA_REQ_MAX_ATTEMPTS; c++) {
        sendShortAvailDataReq();
        t = millis() + DATA_REQ_RX_WINDOW_SIZE;
        while (millis() < t) {
            int8_t ret = HAL_PacketRX(inBuffer);
            if (ret > 1) {
                if (getPacketType(inBuffer) == PKT_AVAIL_DATA_INFO) {
                    if (checkCRC(inBuffer + sizeof(struct MacFrameNormal) + 1, sizeof(struct AvailDataInfo))) {
                        struct MacFrameNormal *f = (struct MacFrameNormal *)inBuffer;
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
static bool processBlockPart(const struct blockPart *bp, uint8_t *blockbuffer) {
    uint16_t start = bp->blockPart * BLOCK_PART_DATA_SIZE;
    uint16_t size = BLOCK_PART_DATA_SIZE;
    // validate if it's okay to copy data
    if (bp->blockId != curBlock.blockId) {
        return false;
    }
    if (start >= BLOCK_XFER_BUFFER_SIZE - 1)
        return false;
    if (bp->blockPart > BLOCK_MAX_PARTS)
        return false;
    if ((start + size) > BLOCK_XFER_BUFFER_SIZE) {
        size = BLOCK_XFER_BUFFER_SIZE - start;
    }
    if (checkCRC(bp, sizeof(struct blockPart) + BLOCK_PART_DATA_SIZE)) {
        //  copy block data to buffer
        memcpy((void *)(blockbuffer + start), (const void *)bp->data, size);
        // we don't need this block anymore, set bit to 0 so we don't request it again
        curBlock.requestedParts[bp->blockPart / 8] &= ~(1 << (bp->blockPart % 8));
        return true;
    } else {
        return false;
    }
}
static bool blockRxLoop(const uint32_t timeout, uint8_t *blockbuffer) {
    uint32_t t;
    bool success = false;
    radioRxEnable(true);
    t = millis() + (timeout + 20);

    bool blockComplete = false;

    while (millis() < t) {
        int8_t ret = HAL_PacketRX(inBuffer);
        if (ret > 1) {
            if ((getPacketType(inBuffer) == PKT_BLOCK_PART) && pktIsUnicast(inBuffer)) {
                struct blockPart *bp = (struct blockPart *)(inBuffer + sizeof(struct MacFrameNormal) + 1);
                success = processBlockPart(bp, blockbuffer);
            }
            blockComplete = true;
            for (uint8_t c = 0; c < BLOCK_MAX_PARTS; c++) {
                if (curBlock.requestedParts[c / 8] & (1 << (c % 8)))
                    blockComplete = false;
            }
            if (blockComplete) {
                radioRxEnable(false);
                radioRxFlush();
            }
        }
    }
    if (blockComplete == false) {
        radioRxEnable(false);
        radioRxFlush();
    }
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
    for (uint8_t c = 0; c < 10; c++) {
        sendBlockRequest();
        t = millis() + 6UL;
        do {
            int8_t ret = HAL_PacketRX(inBuffer);
            if (ret > 1) {
                switch (getPacketType(inBuffer)) {
                    case PKT_BLOCK_REQUEST_ACK:
                        if (checkCRC((inBuffer + sizeof(struct MacFrameNormal) + 1), sizeof(struct blockRequestAck))) {
                            return (struct blockRequestAck *)(inBuffer + sizeof(struct MacFrameNormal) + 1);
                        }
                        break;
                    case PKT_BLOCK_PART:
                        return continueToRX();
                        break;
                    case PKT_CANCEL_XFER:
                        return NULL;
                    default:
#ifdef DEBUG_PROTO
                        printf("PROTO: pkt w/type %02X\n", getPacketType(inBuffer));
#endif
                        break;
                }
            }

        } while (millis() < t);
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
        uint32_t start = millis();
        while ((millis() - start) < (6UL)) {
            int8_t ret = HAL_PacketRX(inBuffer);
            if (ret > 1) {
                if (getPacketType(inBuffer) == PKT_XFER_COMPLETE_ACK) {
#ifdef DEBUG_PROTO
                    printf("PROTO: XFC ACK\n");
#endif
                    return;
                }
            }
        }
    }
#ifdef DEBUG_PROTO
    printf("PROTO: XFC NACK!\n");
#endif
    return;
}
static bool validateBlockData(uint8_t *blockbuffer) {
    struct blockData *bd = (struct blockData *)blockbuffer;
#ifdef DEBUG_PROTO
    printf("PROTO: expected len = %d, checksum=%d\n", bd->size, bd->checksum);
#endif
    if (bd->size > BLOCK_XFER_BUFFER_SIZE - sizeof(blockData)) {
        printf("PROTO: Impossible data size; size = %d\n", bd->size);
        return false;
    }
    uint16_t t = 0;
    for (uint16_t c = 0; c < bd->size; c++) {
        t += bd->data[c];
    }
#ifdef DEBUG_PROTO
    printf("PROTO: calculated checksum = %04X, %d\n", t, t);
    if (t != bd->checksum) printf("PROTO: Checksum failed!\n");
#endif

#ifdef DEBUG_DONTVALIDATEPROTO
    return true;
#else
    return bd->checksum == t;
#endif
}

// EEprom related stuff
static uint32_t getAddressForSlot(const uint8_t s) {
    return EEPROM_IMG_START + (tag.imageSize * s);
}
static void getNumSlots() {
    uint32_t eeSize = eepromGetSize();
#ifdef DEBUG_PROTO
    printf("PROTO: eeSize = %lu, image = %lu\n", eeSize, tag.imageSize);
#endif
    uint16_t nSlots = (eeSize - EEPROM_SETTINGS_SIZE) / (tag.imageSize >> 8) >> 8;
    if (!nSlots) {
        printf("PROTO: eeprom is too small\n");
        while (1)
            ;
    } else if (nSlots >> 8) {
        printf("PROTO: eeprom is too big, some will be unused\n");
        imgSlots = 254;
    } else
        imgSlots = nSlots;
#ifdef DEBUG_PROTO
    printf("PROTO: EEPROM reported size = %lu, %d slots\n", eeSize, imgSlots);
#endif
}
static uint8_t findSlotVer(uint64_t ver) {
#ifdef DEBUGBLOCKS
    return 0xFF;  // remove me! This forces the tag to re-download each and every upload without checking if it's already in the eeprom somewhere
#endif
    uint32_t markerValid = EEPROM_IMG_VALID;
    for (uint8_t c = 0; c < imgSlots; c++) {
        struct EepromImageHeader eih;
        eepromRead(getAddressForSlot(c), &eih, sizeof(struct EepromImageHeader));
        if (!memcmp(&eih.validMarker, &markerValid, 4)) {
            if (eih.version == ver) {
                return c;
            }
        }
    }
    return 0xFF;
}

uint8_t findSlotDataTypeArg(uint8_t arg) {
    arg &= (0xF8);  // unmatch with the 'preload' bit and LUT bits
    for (uint8_t c = 0; c < imgSlots; c++) {
        struct EepromImageHeader eih;
        eepromRead(getAddressForSlot(c), &eih, sizeof(struct EepromImageHeader));
        if (eih.validMarker == EEPROM_IMG_VALID) {
            if ((eih.argument & 0xF8) == arg) {
                return c;
            }
        }
    }
    return 0xFF;
}
uint8_t getEepromImageDataArgument(const uint8_t slot) {
    struct EepromImageHeader eih;
    eepromRead(getAddressForSlot(slot), &eih, sizeof(struct EepromImageHeader));
    return eih.argument;
}
uint8_t findNextSlideshowImage(uint8_t start) {
    struct EepromImageHeader eih;
    ;
    uint8_t c = start;
    while (1) {
        c++;
        if (c > imgSlots) c = 0;
        if (c == start) return c;
        eepromRead(getAddressForSlot(c), &eih, sizeof(struct EepromImageHeader));
        if (eih.validMarker == EEPROM_IMG_VALID) {
            if ((eih.argument & 0xF8) == (CUSTOM_IMAGE_SLIDESHOW << 3)) {
                return c;
            }
        }
    }
}

static void eraseUpdateBlock(uint32_t fwsize) {
#ifdef FWNRF
    eepromErase(FW_LOC, (FW_METADATA_LOC + EEPROM_ERZ_SECTOR_SZ) / EEPROM_ERZ_SECTOR_SZ);
#endif
#ifdef FW88MZ100
    eepromErase(FW_LOC, flashRoundUp(fwsize) / EEPROM_ERZ_SECTOR_SZ);
#endif
}
static void eraseImageBlock(const uint8_t c) {
    eepromErase(getAddressForSlot(c), tag.imageSize / EEPROM_ERZ_SECTOR_SZ);
}
static void saveUpdateBlockData(uint8_t blockId, uint8_t *blockbuffer) {
    if (!eepromWrite(FW_LOC + (blockId * BLOCK_DATA_SIZE), blockbuffer + sizeof(struct blockData), BLOCK_DATA_SIZE))
        printf("PROTO: EEPROM write failed\n");
}

static void saveImgBlockData(const uint8_t imgSlot, const uint8_t blockId, uint8_t *blockbuffer) {
    uint32_t length = tag.imageSize - (sizeof(struct EepromImageHeader) + (blockId * BLOCK_DATA_SIZE));
    if (length > 4096)
        length = 4096;

    if (!eepromWrite(getAddressForSlot(imgSlot) + sizeof(struct EepromImageHeader) + (blockId * BLOCK_DATA_SIZE), blockbuffer + sizeof(struct blockData), length))
        printf("PROTO: EEPROM write failed\n");
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
    uint32_t markerValid = EEPROM_IMG_VALID;
    for (uint8_t c = 0; c < imgSlots; c++) {
        struct EepromImageHeader eih;
        eepromRead(getAddressForSlot(c), &eih, sizeof(struct EepromImageHeader));
        if (!memcmp(&eih.validMarker, &markerValid, 4)) {
            if (temp < eih.id) {
                temp = eih.id;
                nextImgSlot = c;
            }
        }
    }
#ifdef DEBUG_PROTO
    printf("PROTO: found high id=%lu in slot %d\n", temp, nextImgSlot);
#endif
    return temp;
}

static uint8_t *getDataBlock(const uint16_t blockSize) {
    uint8_t partsThisBlock = 0;
    uint8_t blockAttempts = 0;

    uint8_t *blockbuffer = (uint8_t *)malloc(BLOCK_XFER_BUFFER_SIZE);
    if (!blockbuffer) {
        printf("PROTO: failed to allocate block buffer\n");
        return nullptr;
    }

    blockAttempts = BLOCK_TRANSFER_ATTEMPTS;
    if (blockSize == BLOCK_DATA_SIZE) {
        partsThisBlock = BLOCK_MAX_PARTS;
        memset(curBlock.requestedParts, 0xFF, BLOCK_REQ_PARTS_BYTES);
    } else {
        partsThisBlock = (sizeof(struct blockData) + blockSize) / BLOCK_PART_DATA_SIZE;
        if ((sizeof(struct blockData) + blockSize) % BLOCK_PART_DATA_SIZE)
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
#ifdef DEBUG_PROTO
            printf("PROTO: Cancelled request\n");
#endif
            free(blockbuffer);
            return nullptr;
        }

#ifdef FWNRF
        doSleep(ack->pleaseWaitMs);
        powerUp(INIT_UART | INIT_RADIO);
        radioRxEnable(true);
#endif
#ifdef FW88MZ100
        if (ack->pleaseWaitMs) {  // SLEEP - until the AP is ready with the data
            timerDelay(TIMER_TICKS_PER_MSEC * ack->pleaseWaitMs);
        } else {
            // immediately start with the reception of the block data
        }
#endif

        blockRxLoop(BLOCKSIZE_MS, blockbuffer);  // BLOCK RX LOOP - receive a block, until the timeout has passed
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
        doSleep(30);

        if (blockComplete) {
#ifndef DEBUGBLOCKS
            printf("- COMPLETE\n");
#endif
            if (validateBlockData(blockbuffer)) {
                // printf("- Validated\n");
                //  block download complete, validated
                return blockbuffer;
            } else {
                for (uint8_t c = 0; c < partsThisBlock; c++) {
                    curBlock.requestedParts[c / 8] |= (1 << (c % 8));
                }
                requestPartialBlock = false;
                printf("PROTO: blk failed validation!\n");
            }
        } else {
#ifndef DEBUGBLOCKS
            printf("- INCOMPLETE\n");
#endif
            // block incomplete, re-request a partial block
            requestPartialBlock = true;
        }
    }
    printf("PROTO: failed getting block\n");
    free(blockbuffer);
    return nullptr;
}

static uint32_t downloadFWUpdate(const struct AvailDataInfo *avail) {
    static uint32_t PERSISTENTVAR curXferSize = 0;

    // check if we already started the transfer of this information & haven't completed it
    if (!memcmp((const void *)&avail->dataVer, (const void *)&xferDataInfo.dataVer, 8) && xferDataInfo.dataSize) {
        // looks like we did. We'll carry on where we left off.
    } else {
        // start, or restart the transfer from 0. Copy data from the AvailDataInfo struct, and the struct intself. This forces a new transfer
        curBlock.blockId = 0;
        memcpy(&(curBlock.ver), &(avail->dataVer), 8);
        curBlock.type = avail->dataType;
        memcpy(&xferDataInfo, (void *)avail, sizeof(struct AvailDataInfo));
        curXferSize = avail->dataSize;
        eraseUpdateBlock(curXferSize);
    }

    while (xferDataInfo.dataSize) {
        uint16_t dataRequestSize = 0;
        wdt10s();
        if (xferDataInfo.dataSize > BLOCK_DATA_SIZE) {
            // more than one block remaining
            dataRequestSize = BLOCK_DATA_SIZE;
        } else {
            // only one block remains
            dataRequestSize = xferDataInfo.dataSize;
        }
        if (uint8_t *blockbuffer = getDataBlock(dataRequestSize)) {
            // succesfully downloaded datablock, save to eeprom
            powerUp(INIT_EEPROM);
            saveUpdateBlockData(curBlock.blockId, blockbuffer);
            powerDown(INIT_EEPROM);
            free(blockbuffer);
            curBlock.blockId++;
            xferDataInfo.dataSize -= dataRequestSize;
        } else {
            // failed to get the block we wanted, we'll stop for now, maybe resume later
            return 0;
        }
    }

    // no more data, download complete
    powerUp(INIT_EEPROM);
    if (validateEepromMD5(xferDataInfo.dataVer, FW_LOC, curXferSize)) {
        // md5 matches
        powerDown(INIT_EEPROM);
        return curXferSize;
    } else {
        // md5 does not match, invalidate current transfer result, forcing a restart of the transfer
        memset((void *)&xferDataInfo, 0, sizeof(struct AvailDataInfo));
        powerDown(INIT_EEPROM);
        return 0;
    }
}

static bool downloadImageDataToEEPROM(const struct AvailDataInfo *avail) {
    static uint32_t PERSISTENTVAR curXferSize = 0;

    powerUp(INIT_EEPROM);
    // check if we already started the transfer of this information & haven't completed it
    if (!memcmp((const void *)&avail->dataVer, (const void *)&xferDataInfo.dataVer, 8) && xferDataInfo.dataSize) {
        // looks like we did. We'll carry on where we left off.
#ifdef DEBUG_PROTO
        printf("PROTO: restarting image download");
#endif
        // curImgSlot = nextImgSlot; // hmmm
    } else {
        // new transfer
        uint8_t startingSlot = nextImgSlot;
        while (1) {
            nextImgSlot++;
            if (nextImgSlot >= imgSlots) nextImgSlot = 0;
            if (nextImgSlot == startingSlot) {
                // looped
                powerDown(INIT_EEPROM);
                printf("PROTO: no slot available...\n");
                return true;
            }
            struct EepromImageHeader eih;
            eepromRead(getAddressForSlot(nextImgSlot), &eih, sizeof(struct EepromImageHeader));
            // checked if the marker is valid
            if (eih.validMarker == EEPROM_IMG_VALID) {
                struct imageDataTypeArgStruct *eepromDataArgument = (struct imageDataTypeArgStruct *)&(eih.argument);
                // if this is a normal type, we can replace it
                if (eepromDataArgument->specialType == 0x00) break;
            } else {
                // invalid header, so safe to overwrite anyway
                break;
            }
        }

        xferImgSlot = nextImgSlot;

        uint8_t attempt = 5;
        while (attempt--) {
            if (eepromErase(getAddressForSlot(xferImgSlot), tag.imageSize / EEPROM_ERZ_SECTOR_SZ)) goto eraseSuccess;
        }
    eepromFail:
        powerDown(INIT_RADIO);
#ifdef FWNRF
        powerUp(INIT_EPD);
        showNoEEPROM();
        powerDown(INIT_EEPROM | INIT_EPD);
        doSleep(-1);
#endif
        NVIC_SystemReset();
    eraseSuccess:
#ifdef DEBUG_PROTO
        printf("PROTO: new download, writing to slot %d\n", xferImgSlot);
#endif
        // start, or restart the transfer. Copy data from the AvailDataInfo struct, and the struct intself. This forces a new transfer
        curBlock.blockId = 0;
        memcpy(&(curBlock.ver), &(avail->dataVer), 8);
        curBlock.type = avail->dataType;
        memcpy(&xferDataInfo, (void *)avail, sizeof(struct AvailDataInfo));
        curXferSize = xferDataInfo.dataSize;
    }

    while (xferDataInfo.dataSize) {
        uint16_t dataRequestSize = 0;
        wdt10s();
        if (xferDataInfo.dataSize > BLOCK_DATA_SIZE) {
            // more than one block remaining
            dataRequestSize = BLOCK_DATA_SIZE;
        } else {
            // only one block remains
            dataRequestSize = xferDataInfo.dataSize;
        }
        if (uint8_t *blockbuffer = getDataBlock(dataRequestSize)) {
            // succesfully downloaded datablock, save to eeprom
            powerUp(INIT_EEPROM);
#ifdef DEBUGBLOCKS
            printf("PROTO: Saving block %d to slot %d\n", curBlock.blockId, xferImgSlot);
#endif
            saveImgBlockData(xferImgSlot, curBlock.blockId, blockbuffer);
            powerDown(INIT_EEPROM);
            free(blockbuffer);
            curBlock.blockId++;
            xferDataInfo.dataSize -= dataRequestSize;
        } else {
            // failed to get the block we wanted, we'll stop for now, probably resume later
            return false;
        }
    }
    // no more data, download complete

    struct EepromImageHeader eih;
    memcpy(&eih.version, &xferDataInfo.dataVer, 8);
    eih.validMarker = EEPROM_IMG_VALID;
    eih.id = ++curHighSlotId;
    eih.size = curXferSize;
    eih.dataType = xferDataInfo.dataType;
    eih.argument = xferDataInfo.dataTypeArgument;

#ifdef DEBUGBLOCKS
    printf("PROTO: Now writing datatype 0x%02X to slot %d\n", xferDataInfo.dataType, xferImgSlot);
#endif

    powerUp(INIT_EEPROM);
    if (validateEepromMD5(xferDataInfo.dataVer, getAddressForSlot(xferImgSlot) + sizeof(struct EepromImageHeader), curXferSize)) {
        // md5 matches
        eepromWrite(getAddressForSlot(xferImgSlot), &eih, sizeof(struct EepromImageHeader));
        powerDown(INIT_EEPROM);

    } else {
        // md5 does not match, invalidate current transfer result, forcing a restart of the transfer
        memset((void *)&xferDataInfo, 0, sizeof(struct AvailDataInfo));
        powerDown(INIT_EEPROM);
        return false;
    }
    return true;
}

bool processImageDataAvail(struct AvailDataInfo *avail) {
    struct imageDataTypeArgStruct arg = *((struct imageDataTypeArgStruct *)avail->dataTypeArgument);

    // check if the size sent can be contained in the image slot
    if (avail->dataSize > tag.imageSize) {
        printf("PROTO: Unable to save image, it's too big!\n");
        powerUp(INIT_RADIO);
        sendXferComplete();
        powerDown(INIT_RADIO);
        return false;
    }

    if (arg.preloadImage) {
#ifdef DEBUG_PROTO
        printf("PROTO: Preloading image with type 0x%02X from arg 0x%02X\n", arg.specialType, avail->dataTypeArgument);
#endif
        powerUp(INIT_EEPROM);
        switch (arg.specialType) {
            // check if a slot with this argument is already set; if so, erase. Only one of each arg type should exist
            default: {
                uint8_t slot = findSlotDataTypeArg(avail->dataTypeArgument);
                if (slot != 0xFF) {
                    eepromErase(getAddressForSlot(slot), tag.imageSize / EEPROM_ERZ_SECTOR_SZ);
                }
            } break;
            // regular image preload, there can be multiple of this type in the EEPROM
            case CUSTOM_IMAGE_NOCUSTOM: {
                // check if a version of this already exists
                uint8_t slot = findSlotVer(avail->dataVer);
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
#ifdef DEBUG_PROTO
        printf("PROTO: downloading preload image...\n");
#endif
        if (downloadImageDataToEEPROM(avail)) {
            // sets xferImgSlot to the right slot
#ifdef DEBUG_PROTO
            printf("PROTO: preload complete!\n");
#endif
            powerUp(INIT_RADIO);
            sendXferComplete();
            powerDown(INIT_RADIO);
            return true;
        } else {
            return false;
        }

    } else {
        // check if we're currently displaying this data payload
        if (avail->dataVer == curDispDataVer) {
            // currently displayed, not doing anything except for sending an XFC
#ifdef DEBUG_PROTO
            printf("PROTO: currently shown image, send xfc\n");
#endif
            powerUp(INIT_RADIO);
            sendXferComplete();
            powerDown(INIT_RADIO);
            return true;

        } else {
            // currently not displayed

            // try to find the data in the SPI EEPROM
            powerUp(INIT_EEPROM);
            uint8_t findImgSlot = findSlotVer(avail->dataVer);
            powerDown(INIT_EEPROM);

            // Is this image already in a slot somewhere
            if (findImgSlot != 0xFF) {
                // found a (complete)valid image slot for this version
                powerUp(INIT_RADIO);
                sendXferComplete();
                powerDown(INIT_RADIO);

                // mark as completed and draw from EEPROM
                memcpy(&xferDataInfo, (void *)avail, sizeof(struct AvailDataInfo));
                xferDataInfo.dataSize = 0;  // mark as transfer not pending

                wdt60s();
                curImgSlot = findImgSlot;
                powerUp(INIT_EPD | INIT_EEPROM);
                drawImageFromEeprom(findImgSlot, arg.lut);
                powerDown(INIT_EPD | INIT_EEPROM);

            } else {
                // not found in cache, prepare to download
#ifdef DEBUG_PROTO
                printf("PROTO: downloading image...\n");
#endif
                if (downloadImageDataToEEPROM(avail)) {
                    // sets xferImgSlot to the right slot
#ifdef DEBUG_PROTO
                    printf("PROTO: download complete!\n");
#endif
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
            curDispDataVer = xferDataInfo.dataVer;
            return true;
        }
    }
}

bool processAvailDataInfo(struct AvailDataInfo *avail) {
#ifdef DEBUG_PROTO
    printf("PROTO: New dataAvail with type 0x%02X\n", avail->dataType);
#endif
    switch (avail->dataType) {
        case DATATYPE_IMG_RAW_1BPP:
        case DATATYPE_IMG_RAW_2BPP:
        case DATATYPE_IMG_ZLIB:
            processImageDataAvail(avail);
            break;
        case DATATYPE_FW_UPDATE:
            powerUp(INIT_EEPROM);
            if (uint32_t fwsize = downloadFWUpdate(avail)) {
#ifdef DEBUG_PROTO
                printf("PROTO: firmware download complete, doing update.\n");
#endif
                powerUp(INIT_EPD);
                // showApplyUpdate();

                powerUp(INIT_RADIO);
                sendXferComplete();
                powerDown(INIT_RADIO);

                powerUp(INIT_EEPROM);
                wdt60s();
#ifdef FWNRF
                saveUpdateMetadata(fwsize);
#endif

#ifdef FW88MZ100
                applyUpdate(fwsize);
#endif
                NVIC_SystemReset();
            } else {
                return false;
            }
            break;
        case DATATYPE_NFC_URL_DIRECT:
        case DATATYPE_NFC_RAW_CONTENT: {
            // Handle data for the NFC IC (if we have it)

            // check if we actually have the capability to do NFC
            if (!(capabilities & CAPABILITY_HAS_NFC)) {
                // looks like we don't. mark as complete and then bail!
                powerUp(INIT_RADIO);
                sendXferComplete();
                powerDown(INIT_RADIO);
                return true;
            }
#ifdef DEBUG_PROTO
            printf("PROTO: NFC URL received\n");
#endif
            if (xferDataInfo.dataSize == 0 && !memcmp((const void *)&avail->dataVer, (const void *)&xferDataInfo.dataVer, 8)) {
                // we've already downloaded this NFC data, disregard and send XFC
#ifdef DEBUG_PROTO
                printf("PROTO: this was the same as the last transfer, disregard\n");
#endif
                powerUp(INIT_RADIO);
                sendXferComplete();
                powerDown(INIT_RADIO);
                return true;
            }
            curBlock.blockId = 0;
            memcpy(&(curBlock.ver), &(avail->dataVer), 8);
            curBlock.type = avail->dataType;
            memcpy(&xferDataInfo, (void *)avail, sizeof(struct AvailDataInfo));
            uint16_t nfcsize = avail->dataSize;
            wdt10s();
            if (uint8_t *blockbuffer = getDataBlock(avail->dataSize)) {
                xferDataInfo.dataSize = 0;  // mark as transfer not pending
                powerUp(INIT_I2C);
                if (avail->dataType == DATATYPE_NFC_URL_DIRECT) {
                    // only one URL (handle NDEF records on the tag)
                    // loadURLtoNTag();
                } else {
                    // raw NFC data upload to the NFC IC
                    // loadRawNTag(nfcsize);
                }
                delay(13330);
                free(blockbuffer);
                powerDown(INIT_I2C);
                powerUp(INIT_RADIO);
                sendXferComplete();
                powerDown(INIT_RADIO);
                return true;
            }
            return false;
            break;
        }
        case DATATYPE_COMMAND_DATA:
            memcpy(&xferDataInfo, (void *)avail, sizeof(struct AvailDataInfo));
#ifdef LEDSENABLED
            if (avail->dataTypeArgument == 4) {
#ifdef DEBUG_PROTO
                printf("PROTO: LED CMD");
#endif
                setled(xferDataInfo.dataVer, xferDataInfo.dataSize);
            }
#endif
            powerUp(INIT_RADIO);
            sendXferComplete();
            powerDown(INIT_RADIO);
            if (avail->dataTypeArgument != 4) {
                executeCommand(xferDataInfo.dataTypeArgument);
            }
            return true;
            break;
        case DATATYPE_TAG_CONFIG_DATA:
            if (xferDataInfo.dataSize == 0 && memcmp((const void *)&avail->dataVer, (const void *)&xferDataInfo.dataVer, 8) == 0) {
#ifdef DEBUG_PROTO
                printf("PROTO: this was the same as the last transfer, disregard\n");
#endif
                powerUp(INIT_RADIO);
                sendXferComplete();
                powerDown(INIT_RADIO);
                return true;
            }
            curBlock.blockId = 0;
            memcpy(&(curBlock.ver), &(avail->dataVer), sizeof(curBlock.ver));
            curBlock.type = avail->dataType;
            memcpy(&xferDataInfo, (void *)avail, sizeof(struct AvailDataInfo));
            wdt10s();
            if (uint8_t *blockbuffer = getDataBlock(avail->dataSize)) {
                xferDataInfo.dataSize = 0;  // mark as transfer not pending
                powerUp(INIT_EEPROM);
                loadSettingsFromBuffer(sizeof(struct blockData) + blockbuffer);
                free(blockbuffer);
                powerDown(INIT_EEPROM);
                powerUp(INIT_RADIO);
                sendXferComplete();
                powerDown(INIT_RADIO);
                return true;
            }
            return false;
            break;
    }
    return true;
}

void initializeProto() {
    getNumSlots();
    curHighSlotId = getHighSlotId();
}