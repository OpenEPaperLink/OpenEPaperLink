#include <stdint.h>
#include <stdlib.h>
#include "contiki.h"
#include "net/packetbuf.h"
#include "debug.h"
#include "lib/ringbuf.h"
#include "contiki-conf.h"
#include "dev/io-arch.h"
#include "net/netstack.h"
#include "proto.h"

static struct etimer et;
#define BUFSIZE 128
uint8_t cmd_buf[BUFSIZE];
static struct ringbuf rxbuf;

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

const uint8_t channelList[6] = {11, 15, 20, 25, 26, 27};

void radioTx1(uint8_t *buff)
{
    NETSTACK_RADIO.send((uint8_t *)&buff[1], buff[0] - 2);
}

void radioTx(uint8_t *buff)
{
    clock_delay_usec(500);
    NETSTACK_RADIO.send((uint8_t *)&buff[1], buff[0] - 2);
}
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
#define true 1
#define false 0
#define bool uint8_t
#define DATATYPE_NOUPDATE 0
#define HW_TYPE 0xFF

#define MAX_PENDING_MACS 6
#define HOUSEKEEPING_INTERVAL 60UL

struct pendingData pendingDataArr[MAX_PENDING_MACS];

// VERSION GOES HERE!
uint16_t version = 0x0017;

#define RAW_PKT_PADDING 2

uint8_t radiotxbuffer[128];
uint8_t radiorxbuffer[128];

uint8_t mSelfMac[8];

struct blockRequest requestedData = {0}; // holds which data was requested by the tag

uint8_t dstMac[8]; // target for the block transfer
uint16_t dstPan;   //

static uint32_t blockStartTimer = 0; // reference that holds when the AP sends the next block
// extern bool __idata serialBypassActive;                   // if the serial bypass is disabled, saves bytes straight to the block buffer
uint32_t nextBlockAttempt = 0;                   // reference time for when the AP can request a new block from the ESP32
uint8_t seq = 0;                                 // holds current sequence number for transmission
uint8_t blockbuffer[BLOCK_XFER_BUFFER_SIZE + 5]; // block transfer buffer
uint8_t lastAckMac[8] = {0};

uint8_t *blockXferBuffer = blockbuffer;

// these variables hold the current mac were talking to
#define CONCURRENT_REQUEST_DELAY (1200UL / 8)
uint32_t lastBlockRequest = 0;
uint8_t lastBlockMac[8];

uint8_t curChannel = 11;
uint8_t curPower = 10;

uint8_t curPendingData = 0;
uint8_t curNoUpdate = 0;

void sendXferCompleteAck(uint8_t *dst);
void sendCancelXfer(uint8_t *dst);
void espNotifyAPInfo();

// tools
void addCRC(void *p, uint8_t len)
{
    uint8_t total = 0;
    for (uint8_t c = 1; c < len; c++)
    {
        total += ((uint8_t *)p)[c];
    }
    ((uint8_t *)p)[0] = total;
}
bool checkCRC(void *p, uint8_t len)
{
    uint8_t total = 0;
    for (uint8_t c = 1; c < len; c++)
    {
        total += ((uint8_t *)p)[c];
    }
    return ((uint8_t *)p)[0] == total;
}

uint8_t getPacketType(void *buffer)
{
    struct MacFcs *fcs = buffer;
    if ((fcs->frameType == 1) && (fcs->destAddrType == 2) && (fcs->srcAddrType == 3) && (fcs->panIdCompressed == 0))
    {
        // broadcast frame
        uint8_t type = ((uint8_t *)buffer)[sizeof(struct MacFrameBcast)];
        return type;
    }
    else if ((fcs->frameType == 1) && (fcs->destAddrType == 3) && (fcs->srcAddrType == 3) && (fcs->panIdCompressed == 1))
    {
        // normal frame
        uint8_t type = ((uint8_t *)buffer)[sizeof(struct MacFrameNormal)];
        return type;
    }
    return 0;
}
uint8_t getBlockDataLength()
{
    uint8_t partNo = 0;
    for (uint8_t c = 0; c < BLOCK_MAX_PARTS; c++)
    {
        if (requestedData.requestedParts[c / 8] & (1 << (c % 8)))
        {
            partNo++;
        }
    }
    return partNo;
}

// pendingdata slot stuff
int8_t findSlotForMac(const uint8_t *mac)
{
    for (uint8_t c = 0; c < MAX_PENDING_MACS; c++)
    {
        // if (u64_isEq((uint64_t  *)mac, (uint64_t  *)&(pendingDataArr[c].targetMac))) {  // this costs 1 sloc :(
        if (memcmp(mac, ((uint8_t *)&(pendingDataArr[c].targetMac)), 8) == 0)
        {
            if (pendingDataArr[c].attemptsLeft != 0)
            {
                return c;
            }
        }
    }
    return -1;
}
int8_t findFreeSlot()
{
    for (uint8_t c = 0; c < MAX_PENDING_MACS; c++)
    {
        if (pendingDataArr[c].attemptsLeft == 0)
        {
            return c;
        }
    }
    return -1;
}
int8_t findSlotForVer(const uint8_t *ver)
{
    for (uint8_t c = 0; c < MAX_PENDING_MACS; c++)
    {
        // if (u64_isEq((uint64_t  *)ver, (uint64_t  *)&(pendingDataArr[c].availdatainfo.dataVer))) {
        if (memcmp(ver, ((uint8_t *)&(pendingDataArr[c].availdatainfo.dataVer)), 8) == 0)
        {
            if (pendingDataArr[c].attemptsLeft != 0)
                return c;
        }
    }
    return -1;
}
void deleteAllPendingDataForVer(const uint8_t *ver)
{
    int8_t slot = -1;
    do
    {
        slot = findSlotForVer(ver);
        if (slot != -1)
            pendingDataArr[slot].attemptsLeft = 0;
    } while (slot != -1);
}
void deleteAllPendingDataForMac(const uint8_t *mac)
{
    int8_t slot = -1;
    do
    {
        slot = findSlotForMac(mac);
        if (slot != -1)
            pendingDataArr[slot].attemptsLeft = 0;
    } while (slot != -1);
}

void countSlots()
{
    curPendingData = 0;
    curNoUpdate = 0;
    for (uint8_t c = 0; c < MAX_PENDING_MACS; c++)
    {
        if (pendingDataArr[c].attemptsLeft != 0)
        {
            if (pendingDataArr[c].availdatainfo.dataType != 0)
            {
                curPendingData++;
            }
            else
            {
                curNoUpdate++;
            }
        }
    }
}

// processing serial data
#define ZBS_RX_WAIT_HEADER 0
#define ZBS_RX_WAIT_SDA 1    // send data avail
#define ZBS_RX_WAIT_CANCEL 2 // cancel traffic for mac
#define ZBS_RX_WAIT_SCP 3    // set channel power

#define ZBS_RX_WAIT_BLOCKDATA 4

int blockPosition = 0;
void processSerial(uint8_t lastchar)
{
    static uint8_t cmdbuffer[4];
    static uint8_t RXState = 0;
    static uint8_t serialbuffer[48];
    static uint8_t *serialbufferp;
    static uint8_t bytesRemain = 0;

    static uint32_t lastSerial = 0;
    if ((clock_time() - lastSerial) > (800 / 8))
    {
        RXState = ZBS_RX_WAIT_HEADER;
        lastSerial = clock_time();
    }
    else
    {
        lastSerial = clock_time();
    }
    // uartTx(lastchar); echo
    switch (RXState)
    {
    case ZBS_RX_WAIT_HEADER:
        // shift characters in
        for (uint8_t c = 0; c < 3; c++)
        {
            cmdbuffer[c] = cmdbuffer[c + 1];
        }
        cmdbuffer[3] = lastchar;

        if (strncmp(cmdbuffer + 1, ">D>", 3) == 0)
        {
            putstring("ACK>\n");
            blockPosition = 0;
            RXState = ZBS_RX_WAIT_BLOCKDATA;
        }

        if (strncmp(cmdbuffer, "SDA>", 4) == 0)
        {
            RXState = ZBS_RX_WAIT_SDA;
            bytesRemain = sizeof(struct pendingData);
            serialbufferp = serialbuffer;
            break;
        }
        if (strncmp(cmdbuffer, "CXD>", 4) == 0)
        {
            RXState = ZBS_RX_WAIT_CANCEL;
            bytesRemain = sizeof(struct pendingData);
            serialbufferp = serialbuffer;
            break;
        }
        if (strncmp(cmdbuffer, "SCP>", 4) == 0)
        {
            RXState = ZBS_RX_WAIT_SCP;
            bytesRemain = sizeof(struct espSetChannelPower);
            serialbufferp = serialbuffer;
            break;
        }
        if (strncmp(cmdbuffer, "NFO?", 4) == 0)
        {
            putstring("ACK>");
            espNotifyAPInfo();
        }
        if (strncmp(cmdbuffer, "RDY?", 4) == 0)
        {
            putstring("ACK>");
        }
        if (strncmp(cmdbuffer, "RSET", 4) == 0)
        {
            putstring("ACK>");
            // timerDelay(100);
            /*CFGPAGE = 4;
            WDTCONF = 0x80;
            WDTENA = 1;
            WDTRSTVALH = 0xff;
            WDTRSTVALM = 0xff;
            WDTRSTVALL = 0xff;
            while(1);*/
        }
        break;
    case ZBS_RX_WAIT_BLOCKDATA:
        blockbuffer[blockPosition++] = 0xAA ^ lastchar;
        if (blockPosition >= 4100)
        {
            putstring("ACK>");
            RXState = ZBS_RX_WAIT_HEADER;
        }
        break;

    case ZBS_RX_WAIT_SDA:
        *serialbufferp = lastchar;
        serialbufferp++;
        bytesRemain--;
        if (bytesRemain == 0)
        {
            if (checkCRC(serialbuffer, sizeof(struct pendingData)))
            {
                struct pendingData *pd = (struct pendingData *)serialbuffer;
                int8_t slot = findSlotForMac(pd->targetMac);
                if (slot == -1)
                    slot = findFreeSlot();
                if (slot != -1)
                {
                    memcpy(&(pendingDataArr[slot]), serialbuffer, sizeof(struct pendingData));
                    putstring("ACK>\n");
                }
                else
                {
                    putstring("NOQ>\n");
                }
            }
            else
            {
                putstring("NOK>\n");
            }

            RXState = ZBS_RX_WAIT_HEADER;
        }
        break;
    case ZBS_RX_WAIT_CANCEL:
        *serialbufferp = lastchar;
        serialbufferp++;
        bytesRemain--;
        if (bytesRemain == 0)
        {
            if (checkCRC(serialbuffer, sizeof(struct pendingData)))
            {
                struct pendingData *pd = (struct pendingData *)serialbuffer;
                // deleteAllPendingDataForVer((uint8_t *)&pd->availdatainfo.dataVer);
                deleteAllPendingDataForMac((uint8_t *)&pd->targetMac);
                putstring("ACK>\n");
            }
            else
            {
                putstring("NOK>\n");
            }

            RXState = ZBS_RX_WAIT_HEADER;
        }
        break;
    case ZBS_RX_WAIT_SCP:
        *serialbufferp = lastchar;
        serialbufferp++;
        bytesRemain--;
        if (bytesRemain == 0)
        {
            if (checkCRC(serialbuffer, sizeof(struct espSetChannelPower)))
            {
                struct espSetChannelPower *scp = (struct espSetChannelPower *)serialbuffer;
                for (uint8_t c = 0; c < sizeof(channelList); c++)
                {
                    if (channelList[c] == scp->channel)
                        goto SCPchannelFound;
                }
                goto SCPfailed;
            SCPchannelFound:
                curChannel = scp->channel;
                curPower = scp->power;
                NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, scp->channel);
                // NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, scp->power);
                putstring("ACK>\n");
            }
            else
            {
            SCPfailed:
                putstring("NOK>\n");
            }
            RXState = ZBS_RX_WAIT_HEADER;
        }
        break;
    }
}

// sending data to the ESP
void espBlockRequest(const struct blockRequest *br, uint8_t *src)
{
    struct espBlockRequest *ebr = (struct espBlockRequest *)blockbuffer;
    putchar('R');
    putchar('Q');
    putchar('B');
    putchar('>');
    memcpy(&(ebr->ver), &(br->ver), 8);
    memcpy(&(ebr->src), src, 8);
    ebr->blockId = br->blockId;
    addCRC(ebr, sizeof(struct espBlockRequest));
    for (uint8_t c = 0; c < sizeof(struct espBlockRequest); c++)
    {
        putchar(((uint8_t *)ebr)[c]);
    }
}
void espNotifyAvailDataReq(const struct AvailDataReq *adr, const uint8_t *src)
{
    putchar('A');
    putchar('D');
    putchar('R');
    putchar('>');

    struct espAvailDataReq eadr = {0};
    memcpy((void *)eadr.src, (void *)src, 8);
    memcpy((void *)&eadr.adr, (void *)adr, sizeof(struct AvailDataReq));
    addCRC(&eadr, sizeof(struct espAvailDataReq));
    for (uint8_t c = 0; c < sizeof(struct espAvailDataReq); c++)
    {
        putchar(((uint8_t *)eadr)[c]);
    }
}
void espNotifyXferComplete(const uint8_t *src)
{
    struct espXferComplete exfc;
    memcpy(&exfc.src, src, 8);
    putchar('X');
    putchar('F');
    putchar('C');
    putchar('>');
    addCRC(&exfc, sizeof(exfc));
    for (uint8_t c = 0; c < sizeof(exfc); c++)
    {
        putchar(((uint8_t *)exfc)[c]);
    }
}
void espNotifyTimeOut(const uint8_t *src)
{
    struct espXferComplete exfc;
    memcpy(&exfc.src, src, 8);
    putchar('X');
    putchar('T');
    putchar('O');
    putchar('>');
    addCRC(&exfc, sizeof(exfc));
    for (uint8_t c = 0; c < sizeof(exfc); c++)
    {
        putchar(((uint8_t *)exfc)[c]);
    }
}
void espNotifyAPInfo()
{
    putstring("TYP>");
    puthex(HW_TYPE);
    putchar('\n');

    putstring("VER>");
    puthex(version >> 8);
    puthex(version & 0xFF);
    putchar('\n');

    putstring("MAC>");
    puthex(mSelfMac[0]);
    puthex(mSelfMac[1]);
    puthex(mSelfMac[2]);
    puthex(mSelfMac[3]);
    puthex(mSelfMac[4]);
    puthex(mSelfMac[5]);
    puthex(mSelfMac[6]);
    puthex(mSelfMac[7]);
    putchar('\n');

    putstring("ZCH>");
    puthex(curChannel);
    putchar('\n');
    putstring("ZPW>");
    puthex(curPower);
    putchar('\n');

    countSlots();

    putstring("PEN>");
    puthex(curPendingData);
    putchar('\n');

    putstring("NOP>");
    puthex(curNoUpdate);
    putchar('\n');
}

// process data from tag
void processBlockRequest(const uint8_t *buffer, uint8_t forceBlockDownload)
{
    struct MacFrameNormal *rxHeader = (struct MacFrameNormal *)buffer;
    struct blockRequest *blockReq = (struct blockRequest *)(buffer + sizeof(struct MacFrameNormal) + 1);
    if (!checkCRC(blockReq, sizeof(struct blockRequest)))
        return;

    // check if we're already talking to this mac
    if (memcmp(rxHeader->src, lastBlockMac, 8) == 0)
    {
        lastBlockRequest = clock_time();
    }
    else
    {
        // we weren't talking to this mac, see if there was a transfer in progress from another mac, recently
        if ((clock_time() - lastBlockRequest) > CONCURRENT_REQUEST_DELAY)
        {
            // mark this mac as the new current mac we're talking to
            memcpy((void *)lastBlockMac, (void *)rxHeader->src, 8);
            lastBlockRequest = clock_time();
        }
        else
        {
            // we're talking to another mac, let this mac know we can't accomodate another request right now
            putstring("BUSY!\n");
            sendCancelXfer(rxHeader->src);
            return;
        }
    }

    // check if we have data for this mac
    if (findSlotForMac(rxHeader->src) == -1)
    {
        // no data for this mac, politely tell it to fuck off
        sendCancelXfer(rxHeader->src);
        return;
    }

    bool requestDataDownload = false;
    if ((blockReq->blockId != requestedData.blockId) || (!memcmp((const uint64_t *)&blockReq->ver, (const uint64_t *)&requestedData.ver, 8)))
    {
        // requested block isn't already in the buffer
        requestDataDownload = true;
    }
    else
    {
        // requested block is already in the buffer
        if (forceBlockDownload)
        {
            if ((clock_time() - nextBlockAttempt) > (400 / 8))
            {
                requestDataDownload = true;
                putstring("FORCED\n");
            }
            else
            {
                putstring("IGNORED\n");
            }
        }
    }

    // copy blockrequest into requested data
    memcpy(&requestedData, blockReq, sizeof(struct blockRequest));

    struct MacFrameNormal *txHeader = (struct MacFrameNormal *)(radiotxbuffer + 1);
    struct blockRequestAck *blockRequestAck = (struct blockRequestAck *)(radiotxbuffer + sizeof(struct MacFrameNormal) + 2);
    radiotxbuffer[0] = sizeof(struct MacFrameNormal) + 1 + sizeof(struct blockRequestAck) + RAW_PKT_PADDING;
    radiotxbuffer[sizeof(struct MacFrameNormal) + 1] = PKT_BLOCK_REQUEST_ACK;

    if (blockStartTimer == 0)
    {
        if (requestDataDownload)
        {
            // check if we need to download the first block; we need to give the ESP32 some additional time to cache the file
            if (blockReq->blockId == 0)
            {
                blockRequestAck->pleaseWaitMs = 600;
            }
            else
            {
                blockRequestAck->pleaseWaitMs = 600;
            }
        }
        else
        {
            // block is already in buffer
            blockRequestAck->pleaseWaitMs = 600;
        }
    }
    else
    {
        blockRequestAck->pleaseWaitMs = 600;
    }
    blockStartTimer = clock_time() + (blockRequestAck->pleaseWaitMs / 8);

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

    if (requestDataDownload)
    {
        // serialBypassActive = false;
        espBlockRequest(&requestedData, rxHeader->src);
        nextBlockAttempt = clock_time();
    }
}
void processAvailDataReq(uint8_t *buffer)
{
    struct MacFrameBcast *rxHeader = (struct MacFrameBcast *)buffer;
    struct AvailDataReq *availDataReq = (struct AvailDataReq *)(buffer + sizeof(struct MacFrameBcast) + 1);

    if (!checkCRC(availDataReq, sizeof(struct AvailDataReq)))
        return;

    // prepare tx buffer to send a response
    memset(radiotxbuffer, 0, sizeof(struct MacFrameNormal) + sizeof(struct AvailDataInfo) + 2); // 120);
    struct MacFrameNormal *txHeader = (struct MacFrameNormal *)(radiotxbuffer + 1);
    struct AvailDataInfo *availDataInfo = (struct AvailDataInfo *)(radiotxbuffer + sizeof(struct MacFrameNormal) + 2);
    radiotxbuffer[0] = sizeof(struct MacFrameNormal) + 1 + sizeof(struct AvailDataInfo) + RAW_PKT_PADDING;
    radiotxbuffer[sizeof(struct MacFrameNormal) + 1] = PKT_AVAIL_DATA_INFO;

    // check to see if we have data available for this mac
    bool haveData = false;
    for (uint8_t c = 0; c < MAX_PENDING_MACS; c++)
    {
        if (pendingDataArr[c].attemptsLeft)
        {
            if (memcmp(pendingDataArr[c].targetMac, rxHeader->src, 8) == 0)
            {
                haveData = true;
                memcpy((void *)availDataInfo, &(pendingDataArr[c].availdatainfo), sizeof(struct AvailDataInfo));
                break;
            }
        }
    }

    // couldn't find data for this mac
    if (!haveData)
        availDataInfo->dataType = DATATYPE_NOUPDATE;

    memcpy(txHeader->src, mSelfMac, 8);
    memcpy(txHeader->dst, rxHeader->src, 8);
    txHeader->pan = rxHeader->dstPan;
    txHeader->fcs.frameType = 1;
    txHeader->fcs.panIdCompressed = 1;
    txHeader->fcs.destAddrType = 3;
    txHeader->fcs.srcAddrType = 3;
    txHeader->seq = seq++;
    addCRC(availDataInfo, sizeof(struct AvailDataInfo));
    radioTx(radiotxbuffer);
    memset(lastAckMac, 0, 8); // reset lastAckMac, so we can record if we've received exactly one ack packet
    espNotifyAvailDataReq(availDataReq, rxHeader->src);
}
void processXferComplete(uint8_t *buffer)
{
    struct MacFrameNormal *rxHeader = (struct MacFrameNormal *)buffer;
    sendXferCompleteAck(rxHeader->src);
    if (memcmp(lastAckMac, rxHeader->src, 8) != 0)
    {
        memcpy((void *)lastAckMac, (void *)rxHeader->src, 8);
        espNotifyXferComplete(rxHeader->src);
        int8_t slot = findSlotForMac(rxHeader->src);
        if (slot != -1)
            pendingDataArr[slot].attemptsLeft = 0;
    }
}

// send block data to the tag
void sendPart(uint8_t partNo)
{
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
    radioTx1(radiotxbuffer);
}
void sendBlockData()
{
    if (getBlockDataLength() == 0)
    {
        putstring("Invalid block request received, 0 parts..\n");
        requestedData.requestedParts[0] |= 0x01;
    }
    uint8_t partNo = 0;
    while (partNo < BLOCK_MAX_PARTS)
    {
        for (uint8_t c = 0; (c < BLOCK_MAX_PARTS) && (partNo < BLOCK_MAX_PARTS); c++)
        {
            if (requestedData.requestedParts[c / 8] & (1 << (c % 8)))
            {
                sendPart(c);
                partNo++;
            }
        }
    }
}
void sendXferCompleteAck(uint8_t *dst)
{
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
void sendCancelXfer(uint8_t *dst)
{
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
void sendPong(void *buf)
{
    struct MacFrameBcast *rxframe = (struct MacFrameBcast *)buf;
    struct MacFrameNormal *frameHeader = (struct MacFrameNormal *)(radiotxbuffer + 1);
    radiotxbuffer[sizeof(struct MacFrameNormal) + 1] = PKT_PONG;
    radiotxbuffer[sizeof(struct MacFrameNormal) + 2] = curChannel;
    radiotxbuffer[0] = sizeof(struct MacFrameNormal) + 1 + 1 + RAW_PKT_PADDING;
    memcpy(frameHeader->src, mSelfMac, 8);
    memcpy(frameHeader->dst, rxframe->src, 8);
    radiotxbuffer[1] = 0x41; // fast way to set the appropriate bits
    radiotxbuffer[2] = 0xCC; // normal frame
    frameHeader->seq = seq++;
    frameHeader->pan = rxframe->srcPan;
    radioTx(radiotxbuffer);
}
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

static void process_incoming_data(void)
{
    int c = 0;
    uint8_t byte_in;
    c = ringbuf_get(&rxbuf);
    while (c != -1)
    {
        byte_in = (uint8_t)c;
        processSerial(byte_in);
        c = ringbuf_get(&rxbuf);
    }
}

PROCESS(cc2531_usb_demo_process, "cc2531 USB Demo process");
PROCESS(cc2531_RF, "cc2531 RF");
AUTOSTART_PROCESSES(&cc2531_usb_demo_process, &cc2531_RF);

static int char_in(unsigned char c)
{
    ringbuf_put(&rxbuf, c);
    process_poll(&cc2531_usb_demo_process);
    return 1;
}

PROCESS_THREAD(cc2531_RF, ev, data)
{
    PROCESS_BEGIN();
    uint16_t short_addr;
    short_addr = mSelfMac[7];
    short_addr |= mSelfMac[6] << 8;

    NETSTACK_RADIO.off();
    NETSTACK_RADIO.set_value(RADIO_PARAM_PAN_ID, PROTO_PAN_ID);
    NETSTACK_RADIO.set_value(RADIO_PARAM_16BIT_ADDR, short_addr);
    NETSTACK_RADIO.set_object(RADIO_PARAM_64BIT_ADDR, mSelfMac, 8);

    NETSTACK_RADIO.set_value(RADIO_PARAM_RX_MODE, 0); // RADIO_RX_MODE_ADDRESS_FILTER);
    NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, 11);
    NETSTACK_RADIO.on();

    while (1)
    {
        if (NETSTACK_RADIO.pending_packet())
        {
            int len = NETSTACK_RADIO.read(radiorxbuffer, 128);
            if (len)
            {
                /*putstring("RX: ");
                for(int i = 0;i<len;i++){
                    puthex(radiorxbuffer[i]);
                }
                putchar('\n');*/
                // received a packet, lets see what it is
                switch (getPacketType(radiorxbuffer))
                {
                case PKT_AVAIL_DATA_REQ:
                    if (len == 28)
                    {
                        // old version of the AvailDataReq struct, set all the new fields to zero, so it will pass the CRC
                        processAvailDataReq(radiorxbuffer);
                        memset(radiorxbuffer + 1 + sizeof(struct MacFrameBcast) + sizeof(struct oldAvailDataReq), 0, sizeof(struct AvailDataReq) - sizeof(struct oldAvailDataReq) + 2);
                    }
                    else if (len == 40)
                    {
                        // new version of the AvailDataReq struct
                        processAvailDataReq(radiorxbuffer);
                    }
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
                case PKT_AVAIL_DATA_SHORTREQ:
                    // a short AvailDataReq is basically a very short (1 byte payload) packet that requires little preparation on the tx side, for optimal battery use
                    // bytes of the struct are set 0, so it passes the checksum test, and the ESP32 can detect that no interesting payload is sent
                    if (len == 18)
                    {
                        memset(radiorxbuffer + 1 + sizeof(struct MacFrameBcast), 0, sizeof(struct AvailDataReq) + 2);
                        processAvailDataReq(radiorxbuffer);
                    }
                    break;
                default:
                    putstring("t=");
                    puthex(getPacketType(radiorxbuffer));
                    putchar('\n');
                    break;
                }
            }
        }
        if (blockStartTimer)
        {
            // BUG: uint32 overflowing; this will break every once in a while. Don't know how to fix this other than ugly global variables
            if (clock_time() > blockStartTimer)
            {
                sendBlockData();
                blockStartTimer = 0;
            }
        }
        PROCESS_PAUSE();
    }
    PROCESS_END();
}

PROCESS_THREAD(cc2531_usb_demo_process, ev, data)
{
    PROCESS_BEGIN();
    ringbuf_init(&rxbuf, cmd_buf, sizeof(cmd_buf));
    io_arch_set_input(&char_in);
    etimer_set(&et, CLOCK_SECOND * HOUSEKEEPING_INTERVAL);

    unsigned char *macp = &X_IEEE_ADDR;
    requestedData.blockId = 0xFF;
    for (uint8_t c = 0; c < 8; c++)
    {
        mSelfMac[c] = *macp;
        macp++;
    }
    memset(pendingDataArr, 0, sizeof(pendingDataArr));

    putstring("RES>\n");
    putstring("RDY>\n");

    while (1)
    {
        PROCESS_YIELD();
        if (ev == PROCESS_EVENT_POLL)
        {
            process_incoming_data();
        }
        else if (ev == PROCESS_EVENT_TIMER)
        {
            putstring("Housekeepin\r\n");
            for (uint8_t cCount = 0; cCount < MAX_PENDING_MACS; cCount++)
            {
                if (pendingDataArr[cCount].attemptsLeft == 1)
                {
                    if (pendingDataArr[cCount].availdatainfo.dataType != DATATYPE_NOUPDATE)
                    {
                        espNotifyTimeOut(pendingDataArr[cCount].targetMac);
                    }
                    pendingDataArr[cCount].attemptsLeft = 0;
                }
                else if (pendingDataArr[cCount].attemptsLeft > 1)
                {
                    pendingDataArr[cCount].attemptsLeft--;
                    if (pendingDataArr[cCount].availdatainfo.nextCheckIn)
                        pendingDataArr[cCount].availdatainfo.nextCheckIn--;
                }
            }
        }
    }
    PROCESS_END();
}
