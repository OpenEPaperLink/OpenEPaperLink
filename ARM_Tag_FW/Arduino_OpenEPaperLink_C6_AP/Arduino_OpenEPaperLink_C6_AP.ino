// Ported to ESP32-C6 Arduino By ATC1441(ATCnetz.de) for OpenEPaperLink at ~01.2024

#include "led.h"
#include "proto.h"
#include "radio.h"
#include "subGhz.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_ieee802154.h"
#include "esp_phy_init.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "second_uart.h"
#include "soc/lp_uart_reg.h"
#include "soc/uart_struct.h"
#include <esp_mac.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

static const char *TAG = "MAIN";

const uint8_t channelList[6] = { 11, 15, 20, 25, 26, 27 };

#define DATATYPE_NOUPDATE 0
#define HW_TYPE 0xC6

#define MAX_PENDING_MACS 250
#define HOUSEKEEPING_INTERVAL 60UL

struct pendingData pendingDataArr[MAX_PENDING_MACS];

// VERSION GOES HERE!
uint16_t version = 0x001A;

#define RAW_PKT_PADDING 2

uint8_t radiotxbuffer[135];
uint8_t radiorxbuffer[135];

static uint32_t housekeepingTimer;

struct blockRequest requestedData = { 0 };  // holds which data was requested by the tag

uint8_t dstMac[8];  // target for the block transfer
uint16_t dstPan;    //

static uint32_t blockStartTimer = 0;              // reference that holds when the AP sends the next block
uint32_t nextBlockAttempt = 0;                    // reference time for when the AP can request a new block from the ESP32
uint8_t seq = 0;                                  // holds current sequence number for transmission
uint8_t blockbuffer[BLOCK_XFER_BUFFER_SIZE + 5];  // block transfer buffer
uint8_t lastAckMac[8] = { 0 };

// these variables hold the current mac were talking to
#define CONCURRENT_REQUEST_DELAY 1200UL
uint32_t lastBlockRequest = 0;
uint8_t lastBlockMac[8];
uint8_t lastTagReturn[8];

uint8_t curChannel = 25;
uint8_t curPower = 10;

uint8_t curPendingData = 0;
uint8_t curNoUpdate = 0;

bool highspeedSerial = false;

void sendXferCompleteAck(uint8_t *dst, bool isSubGHz);
void sendCancelXfer(uint8_t *dst, bool isSubGHz);
void espNotifyAPInfo();

// tools
void addCRC(void *p, uint8_t len) {
  uint8_t total = 0;
  for (uint8_t c = 1; c < len; c++) {
    total += ((uint8_t *)p)[c];
  }
  ((uint8_t *)p)[0] = total;
}
bool checkCRC(void *p, uint8_t len) {
  uint8_t total = 0;
  for (uint8_t c = 1; c < len; c++) {
    total += ((uint8_t *)p)[c];
  }
  return ((uint8_t *)p)[0] == total;
}

uint8_t getPacketType(void *buffer) {
  struct MacFcs *fcs = (MacFcs *)buffer;
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
  for (uint8_t c = 0; c < MAX_PENDING_MACS; c++) {
    if (memcmp(mac, ((uint8_t *) & (pendingDataArr[c].targetMac)), 8) == 0) {
      if (pendingDataArr[c].attemptsLeft != 0) {
        return c;
      }
    }
  }
  return -1;
}
int8_t findFreeSlot() {
  for (uint8_t c = 0; c < MAX_PENDING_MACS; c++) {
    if (pendingDataArr[c].attemptsLeft == 0) {
      return c;
    }
  }
  return -1;
}
int8_t findSlotForVer(const uint8_t *ver) {
  for (uint8_t c = 0; c < MAX_PENDING_MACS; c++) {
    if (memcmp(ver, ((uint8_t *) & (pendingDataArr[c].availdatainfo.dataVer)), 8) == 0) {
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
void deleteAllPendingDataForMac(const uint8_t *mac) {
  int8_t slot = -1;
  do {
    slot = findSlotForMac(mac);
    if (slot != -1) pendingDataArr[slot].attemptsLeft = 0;
  } while (slot != -1);
}

void countSlots() {
  curPendingData = 0;
  curNoUpdate = 0;
  for (uint8_t c = 0; c < MAX_PENDING_MACS; c++) {
    if (pendingDataArr[c].attemptsLeft != 0) {
      if (pendingDataArr[c].availdatainfo.dataType != 0) {
        curPendingData++;
      } else {
        curNoUpdate++;
      }
    }
  }
}

// processing serial data
#define ZBS_RX_WAIT_HEADER 0
#define ZBS_RX_WAIT_SDA 1     // send data avail
#define ZBS_RX_WAIT_CANCEL 2  // cancel traffic for mac
#define ZBS_RX_WAIT_SCP 3     // set channel power
#define ZBS_RX_WAIT_BLOCKDATA 4

bool isSame(uint8_t *in1, char *in2, int len) {
  bool flag = 1;
  for (int i = 0; i < len; i++) {
    if (in1[i] != in2[i]) flag = 0;
  }
  return flag;
}

int blockPosition = 0;
void processSerial(uint8_t lastchar) {
  static uint8_t cmdbuffer[4];
  static uint8_t RXState = 0;
  static uint8_t serialbuffer[48];
  static uint8_t *serialbufferp;
  static uint8_t bytesRemain = 0;
  static uint32_t lastSerial = 0;
  static uint32_t blockStartTime = 0;
  if ((RXState != ZBS_RX_WAIT_HEADER) && ((millis() - lastSerial) > 1000)) {
    RXState = ZBS_RX_WAIT_HEADER;
    ESP_LOGI(TAG, "UART Timeout");
  }
  lastSerial = millis();
  switch (RXState) {
    case ZBS_RX_WAIT_HEADER:
      // shift characters in
      for (uint8_t c = 0; c < 3; c++) {
        cmdbuffer[c] = cmdbuffer[c + 1];
      }
      cmdbuffer[3] = lastchar;

      if (isSame(cmdbuffer + 1, ">D>", 3)) {
        pr("ACK>");
        blockStartTime = millis();
        ESP_LOGI(TAG, "Starting BlkData, %lu ms after request", blockStartTime - nextBlockAttempt );
        blockPosition = 0;
        RXState = ZBS_RX_WAIT_BLOCKDATA;
      }

      if (isSame(cmdbuffer, "SDA>", 4)) {
        ESP_LOGI(TAG, "SDA In");
        RXState = ZBS_RX_WAIT_SDA;
        bytesRemain = sizeof(struct pendingData);
        serialbufferp = serialbuffer;
        break;
      }
      if (isSame(cmdbuffer, "CXD>", 4)) {
        ESP_LOGI(TAG, "CXD In");
        RXState = ZBS_RX_WAIT_CANCEL;
        bytesRemain = sizeof(struct pendingData);
        serialbufferp = serialbuffer;
        break;
      }
      if (isSame(cmdbuffer, "SCP>", 4)) {
        ESP_LOGI(TAG, "SCP In");
        RXState = ZBS_RX_WAIT_SCP;
        bytesRemain = sizeof(struct espSetChannelPower);
        serialbufferp = serialbuffer;
        break;
      }
      if (isSame(cmdbuffer, "NFO?", 4)) {
        pr("ACK>");
        ESP_LOGI(TAG, "NFO? In");
        espNotifyAPInfo();
        RXState = ZBS_RX_WAIT_HEADER;
      }
      if (isSame(cmdbuffer, "RDY?", 4)) {
        pr("ACK>");
        ESP_LOGI(TAG, "RDY? In");
        RXState = ZBS_RX_WAIT_HEADER;
      }
      if (isSame(cmdbuffer, "RSET", 4)) {
        pr("ACK>");
        ESP_LOGI(TAG, "RSET In");
        delay(100);
        // TODO RESET US HERE
        RXState = ZBS_RX_WAIT_HEADER;
      }
      if (isSame(cmdbuffer, "HSPD", 4)) {
        pr("ACK>");
        ESP_LOGI(TAG, "HSPD In, switching to 2000000");
        delay(100);
        uart_switch_speed(2000000);
        delay(100);
        highspeedSerial = true;
        pr("ACK>");
        RXState = ZBS_RX_WAIT_HEADER;
      }
      break;
    case ZBS_RX_WAIT_BLOCKDATA:
      blockbuffer[blockPosition++] = 0xAA ^ lastchar;
      if (blockPosition >= 4100) {
        ESP_LOGI(TAG, "Blockdata fully received in %lu ms, %lu ms after the request", millis() - blockStartTime, millis() - nextBlockAttempt);
        RXState = ZBS_RX_WAIT_HEADER;
      }
      break;

    case ZBS_RX_WAIT_SDA:
      *serialbufferp = lastchar;
      serialbufferp++;
      bytesRemain--;
      if (bytesRemain == 0) {
        if (checkCRC(serialbuffer, sizeof(struct pendingData))) {
          struct pendingData *pd = (struct pendingData *)serialbuffer;
          int8_t slot = findSlotForMac(pd->targetMac);
          if (slot == -1) slot = findFreeSlot();
          if (slot != -1) {
            memcpy(&(pendingDataArr[slot]), serialbuffer, sizeof(struct pendingData));
            pr("ACK>");
          } else {
            pr("NOQ>");
          }
        } else {
          pr("NOK>");
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
          deleteAllPendingDataForMac((uint8_t *)&pd->targetMac);
          pr("ACK>");
        } else {
          pr("NOK>");
        }

        RXState = ZBS_RX_WAIT_HEADER;
      }
      break;
    case ZBS_RX_WAIT_SCP:
      *serialbufferp = lastchar;
      serialbufferp++;
      bytesRemain--;
      if (bytesRemain == 0) {
        if (checkCRC(serialbuffer, sizeof(struct espSetChannelPower))) {
          struct espSetChannelPower *scp = (struct espSetChannelPower *)serialbuffer;
          for (uint8_t c = 0; c < sizeof(channelList); c++) {
            if (channelList[c] == scp->channel) goto SCPchannelFound;
          }
          goto SCPfailed;
SCPchannelFound:
          pr("ACK>");
          if (curChannel != scp->channel) {
            radioSetChannel(scp->channel);
            curChannel = scp->channel;
          }
          curPower = scp->power;
          radioSetTxPower(scp->power);
          ESP_LOGI(TAG, "Set channel: %d power: %d", curChannel, curPower);
        } else {
SCPfailed:
          pr("NOK>");
        }
        RXState = ZBS_RX_WAIT_HEADER;
      }
      break;
  }
}

// sending data to the ESP
void espBlockRequest(const struct blockRequest *br, uint8_t *src) {
  struct espBlockRequest *ebr = (struct espBlockRequest *)blockbuffer;
  uartTx('R');
  uartTx('Q');
  uartTx('B');
  uartTx('>');
  memcpy(&(ebr->ver), &(br->ver), 8);
  memcpy(&(ebr->src), src, 8);
  ebr->blockId = br->blockId;
  addCRC(ebr, sizeof(struct espBlockRequest));
  for (uint8_t c = 0; c < sizeof(struct espBlockRequest); c++) {
    uartTx(((uint8_t *)ebr)[c]);
  }
}
void espNotifyAvailDataReq(const struct AvailDataReq *adr, const uint8_t *src) {
  uartTx('A');
  uartTx('D');
  uartTx('R');
  uartTx('>');

  struct espAvailDataReq eadr = { 0 };
  memcpy((void *)eadr.src, (void *)src, 8);
  memcpy((void *)&eadr.adr, (void *)adr, sizeof(struct AvailDataReq));
  addCRC(&eadr, sizeof(struct espAvailDataReq));
  for (uint8_t c = 0; c < sizeof(struct espAvailDataReq); c++) {
    uartTx(((uint8_t *)&eadr)[c]);
  }
}
void espNotifyXferComplete(const uint8_t *src) {
  struct espXferComplete exfc;
  memcpy(&exfc.src, src, 8);
  uartTx('X');
  uartTx('F');
  uartTx('C');
  uartTx('>');
  addCRC(&exfc, sizeof(exfc));
  for (uint8_t c = 0; c < sizeof(exfc); c++) {
    uartTx(((uint8_t *)&exfc)[c]);
  }
}
void espNotifyTimeOut(const uint8_t *src) {
  struct espXferComplete exfc;
  memcpy(&exfc.src, src, 8);
  uartTx('X');
  uartTx('T');
  uartTx('O');
  uartTx('>');
  addCRC(&exfc, sizeof(exfc));
  for (uint8_t c = 0; c < sizeof(exfc); c++) {
    uartTx(((uint8_t *)&exfc)[c]);
  }
}
void espNotifyAPInfo() {
  pr("TYP>%02X", HW_TYPE);
  pr("VER>%04X", version);
  pr("MAC>%02X%02X", mSelfMac[0], mSelfMac[1]);
  pr("%02X%02X", mSelfMac[2], mSelfMac[3]);
  pr("%02X%02X", mSelfMac[4], mSelfMac[5]);
  pr("%02X%02X", mSelfMac[6], mSelfMac[7]);
  pr("ZCH>%02X", curChannel);
  pr("ZPW>%02X", curPower);
  countSlots();
  pr("PEN>%02X", curPendingData);
  pr("NOP>%02X", curNoUpdate);
}

void espNotifyTagReturnData(uint8_t *src, uint8_t len) {
  struct tagReturnData *trd = (struct tagReturnData *)(radiorxbuffer + sizeof(struct MacFrameBcast) + 1);  // oh how I'd love to pass this as an argument, but sdcc won't let me
  struct espTagReturnData *etrd = (struct espTagReturnData *)radiotxbuffer;

  if (memcmp((void *)&trd->dataVer, lastTagReturn, 8) == 0) {
    return;
  } else {
    memcpy(lastTagReturn, &trd->dataVer, 8);
  }

  memcpy(etrd->src, src, 8);
  etrd->len = len;
  memcpy(&etrd->returnData, trd, len);
  addCRC(etrd, len + 10);

  uartTx('T');
  uartTx('R');
  uartTx('D');
  uartTx('>');
  for (uint8_t c = 0; c < len + 10; c++) {
    uartTx(((uint8_t *)etrd)[c]);
  }
}

// process data from tag
void processBlockRequest(const uint8_t *buffer, uint8_t forceBlockDownload, bool isSubGHz) {
  struct MacFrameNormal *rxHeader = (struct MacFrameNormal *)buffer;
  struct blockRequest *blockReq = (struct blockRequest *)(buffer + sizeof(struct MacFrameNormal) + 1);
  if (!checkCRC(blockReq, sizeof(struct blockRequest))) return;

  // check if we're already talking to this mac
  if (memcmp(rxHeader->src, lastBlockMac, 8) == 0) {
    lastBlockRequest = millis();
  } else {
    // we weren't talking to this mac, see if there was a transfer in progress from another mac, recently
    if ((millis() - lastBlockRequest) > CONCURRENT_REQUEST_DELAY) {
      // mark this mac as the new current mac we're talking to
      memcpy((void *)lastBlockMac, (void *)rxHeader->src, 8);
      lastBlockRequest = millis();
    } else {
      // we're talking to another mac, let this mac know we can't accomodate another request right now
      pr("BUSY!\n");
      sendCancelXfer(rxHeader->src, isSubGHz);
      return;
    }
  }

  // check if we have data for this mac
  if (findSlotForMac(rxHeader->src) == -1) {
    // no data for this mac, politely tell it to fuck off
    sendCancelXfer(rxHeader->src, isSubGHz);
    return;
  }

  bool requestDataDownload = false;
  if ((blockReq->blockId != requestedData.blockId) || (blockReq->ver != requestedData.ver)) {
    // requested block isn't already in the buffer
    requestDataDownload = true;
  } else {
    // requested block is already in the buffer
    if (forceBlockDownload) {
      if ((millis() - nextBlockAttempt) > 380) {
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
      if (highspeedSerial == true) {
        blockRequestAck->pleaseWaitMs = 140;
      } else {
        blockRequestAck->pleaseWaitMs = 550;
      }
    } else {
      // block is already in buffer
      blockRequestAck->pleaseWaitMs = 30;
    }
  } else {
    blockRequestAck->pleaseWaitMs = 30;
  }
  blockStartTimer = millis() + blockRequestAck->pleaseWaitMs;

  memcpy(txHeader->src, mSelfMac, 8);
  memcpy(txHeader->dst, rxHeader->src, 8);

  txHeader->pan = rxHeader->pan;
  txHeader->fcs.frameType = 1;
  txHeader->fcs.panIdCompressed = 1;
  txHeader->fcs.destAddrType = 3;
  txHeader->fcs.srcAddrType = 3;
  txHeader->seq = seq++;

  addCRC((void *)blockRequestAck, sizeof(struct blockRequestAck));

  radioTx(radiotxbuffer, isSubGHz);

  // save the target for the blockdata
  memcpy(dstMac, rxHeader->src, 8);
  dstPan = rxHeader->pan;

  if (requestDataDownload) {
    blockPosition = 0;
    espBlockRequest(&requestedData, rxHeader->src);
    nextBlockAttempt = millis();
  }
}

void processAvailDataReq(uint8_t *buffer, bool isSubGHz) {
  struct MacFrameBcast *rxHeader = (struct MacFrameBcast *)buffer;
  struct AvailDataReq *availDataReq = (struct AvailDataReq *)(buffer + sizeof(struct MacFrameBcast) + 1);

  if (!checkCRC(availDataReq, sizeof(struct AvailDataReq))) return;

  // prepare tx buffer to send a response
  memset(radiotxbuffer, 0, sizeof(struct MacFrameNormal) + sizeof(struct AvailDataInfo) + 2);  // 120);
  struct MacFrameNormal *txHeader = (struct MacFrameNormal *)(radiotxbuffer + 1);
  struct AvailDataInfo *availDataInfo = (struct AvailDataInfo *)(radiotxbuffer + sizeof(struct MacFrameNormal) + 2);
  radiotxbuffer[0] = sizeof(struct MacFrameNormal) + 1 + sizeof(struct AvailDataInfo) + RAW_PKT_PADDING;
  radiotxbuffer[sizeof(struct MacFrameNormal) + 1] = PKT_AVAIL_DATA_INFO;

  // check to see if we have data available for this mac
  bool haveData = false;
  for (uint8_t c = 0; c < MAX_PENDING_MACS; c++) {
    if (pendingDataArr[c].attemptsLeft) {
      if (memcmp(pendingDataArr[c].targetMac, rxHeader->src, 8) == 0) {
        haveData = true;
        memcpy((void *)availDataInfo, &(pendingDataArr[c].availdatainfo), sizeof(struct AvailDataInfo));
        break;
      }
    }
  }

  // couldn't find data for this mac
  if (!haveData) availDataInfo->dataType = DATATYPE_NOUPDATE;

  memcpy(txHeader->src, mSelfMac, 8);
  memcpy(txHeader->dst, rxHeader->src, 8);
  txHeader->pan = rxHeader->dstPan;
  txHeader->fcs.frameType = 1;
  txHeader->fcs.panIdCompressed = 1;
  txHeader->fcs.destAddrType = 3;
  txHeader->fcs.srcAddrType = 3;
  txHeader->seq = seq++;
  addCRC(availDataInfo, sizeof(struct AvailDataInfo));
  radioTx(radiotxbuffer, isSubGHz);
  memset(lastAckMac, 0, 8);  // reset lastAckMac, so we can record if we've received exactly one ack packet
  espNotifyAvailDataReq(availDataReq, rxHeader->src);
}
void processXferComplete(uint8_t *buffer, bool isSubGHz) {
  struct MacFrameNormal *rxHeader = (struct MacFrameNormal *)buffer;
  sendXferCompleteAck(rxHeader->src, isSubGHz);
  if (memcmp(lastAckMac, rxHeader->src, 8) != 0) {
    memcpy((void *)lastAckMac, (void *)rxHeader->src, 8);
    espNotifyXferComplete(rxHeader->src);
    int8_t slot = findSlotForMac(rxHeader->src);
    if (slot != -1) pendingDataArr[slot].attemptsLeft = 0;
  }
}

void processTagReturnData(uint8_t *buffer, uint8_t len, bool isSubGHz) {
  struct MacFrameBcast *rxframe = (struct MacFrameBcast *)buffer;
  struct MacFrameNormal *frameHeader = (struct MacFrameNormal *)(radiotxbuffer + 1);

  if (!checkCRC((buffer + sizeof(struct MacFrameBcast) + 1), len - (sizeof(struct MacFrameBcast) + 1))) {
    return;
  }
  radiotxbuffer[sizeof(struct MacFrameNormal) + 1] = PKT_TAG_RETURN_DATA_ACK;
  radiotxbuffer[0] = sizeof(struct MacFrameNormal) + 1 + RAW_PKT_PADDING;
  memcpy(frameHeader->src, mSelfMac, 8);
  memcpy(frameHeader->dst, rxframe->src, 8);
  radiotxbuffer[1] = 0x41;  // fast way to set the appropriate bits
  radiotxbuffer[2] = 0xCC;  // normal frame
  frameHeader->seq = seq++;
  frameHeader->pan = rxframe->srcPan;
  radioTx(radiotxbuffer, isSubGHz);

  espNotifyTagReturnData(rxframe->src, len - (sizeof(struct MacFrameBcast) + 1));
}

// send block data to the tag
void sendPart(uint8_t partNo, bool isSubGHz) {
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
  radioTx(radiotxbuffer, isSubGHz);
}
void sendBlockData(bool isSubGHz) {
  if (getBlockDataLength() == 0) {
    pr("Invalid block request received, 0 parts..\n");
    requestedData.requestedParts[0] |= 0x01;
  }

  pr("Sending parts:");
  for (uint8_t c = 0; (c < BLOCK_MAX_PARTS); c++) {
    if (c % 10 == 0) pr(" ");
    if (requestedData.requestedParts[c / 8] & (1 << (c % 8))) {
      pr("X");
    } else {
      pr(".");
    }
  }
  pr("\n");

  uint8_t partNo = 0;
  while (partNo < BLOCK_MAX_PARTS) {
    for (uint8_t c = 0; (c < BLOCK_MAX_PARTS) && (partNo < BLOCK_MAX_PARTS); c++) {
      if (requestedData.requestedParts[c / 8] & (1 << (c % 8))) {
        sendPart(c, isSubGHz);
        partNo++;
      }
    }
  }
}
void sendXferCompleteAck(uint8_t *dst, bool isSubGHz) {
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
  radioTx(radiotxbuffer, isSubGHz);
}
void sendCancelXfer(uint8_t *dst, bool isSubGHz) {
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
  radioTx(radiotxbuffer, isSubGHz);
}
void sendPong(void *buf, bool isSubGHz) {
  struct MacFrameBcast *rxframe = (struct MacFrameBcast *)buf;
  struct MacFrameNormal *frameHeader = (struct MacFrameNormal *)(radiotxbuffer + 1);
  radiotxbuffer[sizeof(struct MacFrameNormal) + 1] = PKT_PONG;
  radiotxbuffer[sizeof(struct MacFrameNormal) + 2] = curChannel;
  radiotxbuffer[0] = sizeof(struct MacFrameNormal) + 1 + 1 + RAW_PKT_PADDING;
  memcpy(frameHeader->src, mSelfMac, 8);
  memcpy(frameHeader->dst, rxframe->src, 8);
  radiotxbuffer[1] = 0x41;  // fast way to set the appropriate bits
  radiotxbuffer[2] = 0xCC;  // normal frame
  frameHeader->seq = seq++;
  frameHeader->pan = rxframe->srcPan;
  radioTx(radiotxbuffer, isSubGHz);
}

extern uint8_t mSelfMac[8];
void setup() {
//Serial.begin(115200);
  init_led();
  init_second_uart();

  requestedData.blockId = 0xFF;
  // clear the array with pending information
  memset(pendingDataArr, 0, sizeof(pendingDataArr));

  radio_init(curChannel);
  radioSetTxPower(10);

  pr("RES>");
  pr("RDY>");
  ESP_LOGI(TAG, "C6 ready!");

  housekeepingTimer = millis();
}

bool isSubGhzRx = false;
void loop() {
  while ((millis() - housekeepingTimer) < ((1000 * HOUSEKEEPING_INTERVAL) - 100)) {
    int8_t ret = commsRxUnencrypted(radiorxbuffer, &isSubGhzRx);
    if (ret > 1) {
      if (0)
      {
        Serial.printf("RXed packet len %u :", ret);
        for (int t = 0; t < ret; t++) {
          Serial.printf(" %02x", radiorxbuffer[t]);
        }
        Serial.printf("\n");
      }

      if (isSubGhzRx) {
        ESP_LOGD(TAG, "RXed packet %s len %u", isSubGhzRx ? "subGhz" : "2.4Ghz", ret);
        ESP_LOGD(TAG, "First bytes: %02X %02X %02X %02X %02X", radiorxbuffer[0], radiorxbuffer[1], radiorxbuffer[2], radiorxbuffer[3], radiorxbuffer[4]);
      }

      led_flash(0);
      // received a packet, lets see what it is
      switch (getPacketType(radiorxbuffer)) {
        case PKT_AVAIL_DATA_REQ:
          if (ret == 28) {
            // old version of the AvailDataReq struct, set all the new fields to zero, so it will pass the CRC
            memset(radiorxbuffer + 1 + sizeof(struct MacFrameBcast) + sizeof(struct oldAvailDataReq), 0,
                   sizeof(struct AvailDataReq) - sizeof(struct oldAvailDataReq) + 2);
            processAvailDataReq(radiorxbuffer, isSubGhzRx);
          } else if (ret == 40) {
            // new version of the AvailDataReq struct
            processAvailDataReq(radiorxbuffer, isSubGhzRx);
          }
          break;
        case PKT_BLOCK_REQUEST:
          processBlockRequest(radiorxbuffer, 1, isSubGhzRx);
          break;
        case PKT_BLOCK_PARTIAL_REQUEST:
          processBlockRequest(radiorxbuffer, 0, isSubGhzRx);
          break;
        case PKT_XFER_COMPLETE:
          processXferComplete(radiorxbuffer, isSubGhzRx);
          break;
        case PKT_PING:
          sendPong(radiorxbuffer, isSubGhzRx);
          break;
        case PKT_AVAIL_DATA_SHORTREQ:
          // a short AvailDataReq is basically a very short (1 byte payload) packet that requires little preparation on the tx side, for optimal
          // battery use bytes of the struct are set 0, so it passes the checksum test, and the ESP32 can detect that no interesting payload is
          // sent
          if (ret == 18) {
            memset(radiorxbuffer + 1 + sizeof(struct MacFrameBcast), 0, sizeof(struct AvailDataReq) + 2);
            processAvailDataReq(radiorxbuffer, isSubGhzRx);
          }
          break;
        case PKT_TAG_RETURN_DATA:
          processTagReturnData(radiorxbuffer, ret, isSubGhzRx);
          break;
        default:
          ESP_LOGI(TAG, "t=%02X" , getPacketType(radiorxbuffer));
          break;
      }
    } else if (blockStartTimer == 0) {
      delay(10);
    }

    uint8_t curr_char;
    while (getRxCharSecond(&curr_char)) processSerial(curr_char);

    if (blockStartTimer) {
      if (millis() > blockStartTimer) {
        sendBlockData(isSubGhzRx);
        blockStartTimer = 0;
      }
    }
  }

  radio_housekeeping();
  memset(&lastTagReturn, 0, 8);
  for (uint8_t cCount = 0; cCount < MAX_PENDING_MACS; cCount++) {
    if (pendingDataArr[cCount].attemptsLeft == 1) {
      if (pendingDataArr[cCount].availdatainfo.dataType != DATATYPE_NOUPDATE) {
        espNotifyTimeOut(pendingDataArr[cCount].targetMac);
      }
      pendingDataArr[cCount].attemptsLeft = 0;
    } else if (pendingDataArr[cCount].attemptsLeft > 1) {
      pendingDataArr[cCount].attemptsLeft--;
      if (pendingDataArr[cCount].availdatainfo.nextCheckIn) pendingDataArr[cCount].availdatainfo.nextCheckIn--;
    }
  }
  housekeepingTimer = millis();
}
