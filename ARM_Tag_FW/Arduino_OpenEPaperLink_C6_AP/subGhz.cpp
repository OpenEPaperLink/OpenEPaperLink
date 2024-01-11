
// This code is heavily depending on Dmitrys wonderful Work !
// https://dmitry.gr/?r=05.Projects&proj=29.%20eInk%20Price%20Tags
// Ported and modified to fit the OpenEPaperLink Project by ATC1441 (ATCnetz.de) ~01.2024

#include "subGhz.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "proto.h"
#include <Arduino.h>
#include <SPI.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *TAG = "subGHz";

#define SUB_GHZ_CH_OFST 11
#define SUB_GHZ_NUM_CHANNELS 27

/*
	//we configure GDO_2 is for TX.has_fifo_space
	//we configure GDO_0 is for RX.has_data
  WIRING:
*/
#define sub_CS 4
#define sub_GD0 5
#define sub_GD2 6
#define sub_MISO 7
#define sub_CLK 0
#define sub_MOSI 1

#define CMD_SRES 0x30
#define CMD_SFSTXON 0x31
#define CMD_SXOFF 0x32
#define CMD_SCAL 0x33
#define CMD_SRX 0x34
#define CMD_STX 0x35
#define CMD_SIDLE 0x36
#define CMD_SWOR 0x38
#define CMD_SPWD 0x39
#define CMD_SFRX 0x3a
#define CMD_SFTX 0x3b
#define CMD_SWORRST 0x3c
#define CMD_SNOP 0x3d

#define REG_IOCFG2 0x00
#define REG_IOCFG1 0x01
#define REG_IOCFG0 0x02
#define REG_FIFOTHR 0x03
#define REG_SYNC1 0x04
#define REG_SYNC0 0x05
#define REG_PKTLEN 0x06
#define REG_PKTCTRL1 0x07
#define REG_PKTCTRL0 0x08
#define REG_ADDR 0x09
#define REG_CHANNR 0x0a
#define REG_FSCTRL1 0x0b
#define REG_FSCTRL0 0x0c
#define REG_FREQ2 0x0d
#define REG_FREQ1 0x0e
#define REG_FREQ0 0x0f
#define REG_MDMCFG4 0x10
#define REG_MDMCFG3 0x11
#define REG_MDMCFG2 0x12
#define REG_MDMCFG1 0x13
#define REG_MDMCFG0 0x14
#define REG_DEVIATN 0x15
#define REG_MCSM2 0x16
#define REG_MCSM1 0x17
#define REG_MCSM0 0x18
#define REG_FOCCFG 0x19
#define REG_BSCFG 0x1a
#define REG_AGCTRL2 0x1b
#define REG_AGCTRL1 0x1c
#define REG_AGCTRL0 0x1d
#define REG_WOREVT1 0x1e
#define REG_WOREVT0 0x1f
#define REG_WORCTRL 0x20
#define REG_FREND1 0x21
#define REG_FREND0 0x22
#define REG_FSCAL3 0x23
#define REG_FSCAL2 0x24
#define REG_FSCAL1 0x25
#define REG_FSCAL0 0x26
#define REG_RCCTRL1 0x27
#define REG_RCCTRL0 0x28
#define REG_FSTEST 0x29
#define REG_PTEST 0x2a
#define REG_AGCTEST 0x2b
#define REG_TEST2 0x2c
#define REG_TEST1 0x2d
#define REG_TEST0 0x2e

#define REG_PATABLE 0x3e
#define REG_FIFO 0x3f

#define REG_PARTNUM 0xf0
#define REG_VERSION 0xf1
#define REG_FREQEST 0xf2
#define REG_LQI 0xf3
#define REG_RSSI 0xf4
#define REG_MARCSTATE 0xf5
#define REG_WORTIME1 0xf6
#define REG_WORTIME0 0xf7
#define REG_PKTSTATUS 0xf8
#define REG_VCO_VC_DAC 0xf9
#define REG_TXBYTES 0xfa
#define REG_RXBYTES 0xfb
#define REG_RCCTRL1_STA 0xfc
#define REG_RCCTRL0_STA 0xfd



#define MAX_RX_PKTS 80

static volatile uint8_t mRxBufs[MAX_RX_PKTS][RADIO_MAX_PACKET_LEN + 1 /* length */ + 2 /* RSSI, LQI/STA */];
static volatile uint8_t mRxNextWrite, mRxNextRead, mRxNumFree, mRxNumGot;
static uint8_t mRxFilterLongMac[8];
static uint32_t mRxFilterShortMac;
static bool mRxEnabled, mAutoAck, mPromisc;
static uint16_t mRxFilterPan;
static volatile int16_t mLastAck;

struct MacHeaderGenericAddr {
  struct MacFcs fixed;
  uint8_t seq;
} __attribute__((packed));

struct MacHeaderShortAddr {
  struct MacFcs fixed;
  uint8_t seq;
  uint16_t pan;
  uint16_t shortDstAddr;
} __attribute__((packed));

struct MacHeaderLongAddr {
  struct MacFcs fixed;
  uint8_t seq;
  uint16_t pan;
  uint8_t longDstAddr[8];
} __attribute__((packed));


void tiRadioRxFilterCfg(const uint8_t *myMac, uint32_t myShortMac, uint16_t myPan, bool promisc) {
  mPromisc = promisc;
  mRxFilterShortMac = myShortMac;
  mRxFilterPan = myPan;
  memcpy(mRxFilterLongMac, myMac, sizeof(mRxFilterLongMac));
}

static bool tiRadioPrvSelect(void) {
  digitalWrite(sub_CS, LOW);
  SPI.beginTransaction(SPISettings(6000000, MSBFIRST, SPI_MODE0));
  asm volatile("nop \n nop \n nop \n");
  return true;
}

static void tiRadioPrvDeselect(void) {
  asm volatile("nop \n nop \n nop \n");
  SPI.endTransaction();
  digitalWrite(sub_CS, HIGH);
  asm volatile("nop \n nop \n nop \n");
}

static void read_multiple(uint8_t *dst, uint_fast8_t rxLen) {
  for (int i = 0; i < rxLen; i++) {
    dst[i] = SPI.transfer(0xff);
  }
}

static int_fast16_t tiRadioPrvStrobe(uint8_t cmd)  //negative on error
{
  if (!tiRadioPrvSelect())
    return -1;

  cmd = SPI.transfer(cmd);

  tiRadioPrvDeselect();

  return (uint_fast16_t)cmd;
}

static bool tiRadioPrvRegWrite(uint_fast8_t reg, uint_fast8_t val) {
  if (!tiRadioPrvSelect())
    return false;

  SPI.transfer(reg);
  SPI.transfer(val);

  tiRadioPrvDeselect();

  return true;
}

static int_fast16_t tiRadioPrvRegRead(uint_fast8_t reg) {
  uint8_t read_data = 0x00;

  if (!tiRadioPrvSelect())
    return -1;

  SPI.transfer(reg | 0x80);
  read_data = SPI.transfer(0xff);

  tiRadioPrvDeselect();

  return (uint_fast16_t)read_data;
}

static bool tiRadioPrvRegWriteLong(uint8_t reg, const uint8_t *valP, uint8_t len) {
  if (!tiRadioPrvSelect())
    return false;

  SPI.transfer(reg | 0x40);

  for (int i = 0; i < len; i++) {
    SPI.transfer(valP[i]);
  }

  tiRadioPrvDeselect();

  return true;
}

bool tiRadioRxEnable(bool on, bool autoAck) {
  bool ret = false;

  if (on) {

    mAutoAck = autoAck;
    if (mRxEnabled) {
      ret = true;
      goto out;
    }

    if (!tiRadioPrvStrobe(CMD_SRX))
      goto out;

    mRxEnabled = true;
  } else if (mRxEnabled) {

    if (!tiRadioPrvStrobe(CMD_SIDLE))
      goto out;

    mRxEnabled = false;
  }
  ret = true;

out:
  return ret;
}

static bool radioPrvMacsEqual(const uint8_t *macA, const uint8_t *macB) {
  const uint32_t *a = (const uint32_t *)(const char *)macA;
  const uint32_t *b = (const uint32_t *)(const char *)macB;
  return a[0] == b[0] && a[1] == b[1];
}

static uint_fast8_t tiRadioPrvGetState(void) {
  uint_fast8_t state;

  do {
    state = tiRadioPrvRegRead(REG_MARCSTATE);
  } while (tiRadioPrvRegRead(REG_MARCSTATE) != state);

  return state;
}

static void tiRadioPrvPacketRx(void) {
  uint8_t *rxedPkt = (uint8_t *)mRxBufs[mRxNextWrite];
  const struct MacHeaderShortAddr *withShortDst = (const struct MacHeaderShortAddr *)(rxedPkt + 1);
  const struct MacHeaderGenericAddr *generic = (const struct MacHeaderGenericAddr *)(rxedPkt + 1);
  const struct MacHeaderLongAddr *withLongDst = (const struct MacHeaderLongAddr *)(rxedPkt + 1);
  bool crcOk, acceptPacket, sendAck = false;
  int32_t t, lenNoCrc, lenNoMacFixed;
  uint32_t nWaitCycles = 10000;
  uint_fast8_t spiLen, now;

  t = tiRadioPrvRegRead(REG_FIFO);
  if (t < 0)
    goto fail;

  if (!mRxNumFree)
    goto fail;

  spiLen = t;
  if (spiLen > RADIO_MAX_PACKET_LEN)
    goto fail;

  t = 0;
  rxedPkt[t++] = lenNoCrc = spiLen;
  now = 31;     //we just read one so 31 left for sure in the FIFO
  spiLen += 2;  //we expect 2 more bytes

  while (spiLen) {

    uint8_t reg;

    if (!tiRadioPrvSelect()) {
      tiRadioPrvDeselect();
      goto fail;
    }

    reg = 0xc0 | REG_FIFO;  //burst read
    reg = SPI.transfer(reg);
    now = reg & 0x0f;

    if (now > spiLen)
      now = spiLen;

    if (!now && !--nWaitCycles) {
      tiRadioPrvDeselect();
      ESP_LOGE(TAG, " !!! RX timeout !!! ");
      goto fail;
    }

    read_multiple(rxedPkt + t, now);
    t += now;
    spiLen -= now;

    tiRadioPrvDeselect();
  }

  rxedPkt++;  //skip len;
  crcOk = !!(rxedPkt[lenNoCrc + 1] & 0x80);

  lenNoMacFixed = lenNoCrc - sizeof(struct MacFcs) - sizeof(uint8_t);

  if (mPromisc)
    acceptPacket = true;
  //otherwise, we need a valid crc
  else if (!crcOk) {
    acceptPacket = false;
  }
  //packet should be big enough to contain a header
  else if (lenNoMacFixed < 0)
    acceptPacket = false;
  else switch (generic->fixed.frameType) {

      case FRAME_TYPE_ACK:
        mLastAck = (uint16_t)generic->seq;
        acceptPacket = false;  //no need to save it as a packet
        break;

      case FRAME_TYPE_DATA:  //we are not the coordinator, so we demand to see our address as destination...

        switch (generic->fixed.destAddrType) {
          case ADDR_MODE_SHORT:
            acceptPacket = (withShortDst->pan == 0xffff && withShortDst->shortDstAddr == 0xffff) || (withShortDst->pan == mRxFilterPan && (withShortDst->shortDstAddr == 0xffff || ((uint32_t)withShortDst->shortDstAddr) == mRxFilterShortMac));
            break;

          case ADDR_MODE_LONG:
            acceptPacket = withLongDst->pan == mRxFilterPan && radioPrvMacsEqual(withLongDst->longDstAddr, mRxFilterLongMac);
            break;

          default:
            acceptPacket = false;
            break;
        }
        sendAck = generic->fixed.ackReqd && mAutoAck;
        break;

      default:
        //no riff-raff please
        acceptPacket = false;
        break;
    }

  if (acceptPacket) {  //other checks here too plz

    if (sendAck) {

      static struct {
        uint8_t len;
        struct MacFcs mac;
        uint8_t seq;
      } __attribute__((packed)) ack = {
        .len = sizeof(ack) + sizeof(uint16_t) - sizeof(ack.len),
        .mac = {
          .frameType = FRAME_TYPE_ACK,
        },
      };

      ack.seq = generic->seq;

      tiRadioTxLL(&ack);
    }

    mRxNumFree--;
    if (++mRxNextWrite == MAX_RX_PKTS)
      mRxNextWrite = 0;
    mRxNumGot++;
  }

out:
  if (mRxEnabled && !sendAck) {  //if ack is being TXed, TX irq will restart rx later
    uint32_t maxWait = 100000;
    uint8_t state;

    (void)tiRadioPrvStrobe(CMD_SRX);

    do {
      state = tiRadioPrvGetState();

      if (!--maxWait) {
        //ESP_LOGE(TAG, "too long wait for rx state. state is %d", state);
        break;
      }

    } while (state != 13 && state != 14 && state != 15);
  }
  return;

fail:
  (void)tiRadioPrvStrobe(CMD_SFRX);
  goto out;
}

void tiRadioRxAckReset(void) {
  mLastAck = -1;
}

int16_t tiRadioRxAckGetLast(void) {
  return mLastAck;
}

int32_t tiRadioRxDequeuePkt(void *dstBuf, uint32_t maxLen, int8_t *rssiP, uint8_t *lqiP) {
  uint32_t len, copyLen = maxLen;

  if (!mRxNumGot)
    return -1;

  len = mRxBufs[mRxNextRead][0];

  if (copyLen > len)
    copyLen = len;
  memcpy(dstBuf, (const void *)(mRxBufs[mRxNextRead] + 1), copyLen);

  if (lqiP) {

    uint32_t lqi = 296 - ((mRxBufs[mRxNextRead][len + 2] & 0x7f) * 2);

    //LQI: lower is better. 48 is very good (no lower value seen), 127 is very bad (max value possible)
    //we want LQI 0..255 so we scale as SATURATE(296 - 2 * val)

    if (lqi > 255)
      lqi = 255;

    *lqiP = lqi;
  }

  if (rssiP)
    *rssiP = ((int8_t)mRxBufs[mRxNextRead][len + 1]) / 2 - 77;

  if (++mRxNextRead == MAX_RX_PKTS)
    mRxNextRead = 0;

  noInterrupts();
  mRxNumFree++;
  mRxNumGot--;
  interrupts();

  //ti radio never stalls RX state machine so nothing to do even if we just freed up a buffer
  return len;
}

void data_input_interrupt(void) {
  while (digitalRead(sub_GD0))  //while there is data (packets)
    tiRadioPrvPacketRx();
}

static void tiRadioPrvIfInit(void) {
  //configure pins
  pinMode(sub_CS, OUTPUT);
  pinMode(sub_GD0, INPUT_PULLUP);
  pinMode(sub_GD2, INPUT_PULLUP);
  SPI.begin(sub_CLK, sub_MISO, sub_MOSI);

  tiRadioPrvDeselect();

  mRxNumFree = MAX_RX_PKTS;
}

bool tiRadioSetChannel(uint_fast8_t channel) {
  channel -= SUB_GHZ_CH_OFST;
  if (channel >= SUB_GHZ_NUM_CHANNELS) {
    ESP_LOGI(TAG, "CH not set: %i", channel);
    return false;
  }
  ESP_LOGI(TAG, "CH: %i", channel);

  delayMicroseconds(1000);
  return tiRadioPrvRegWrite(REG_CHANNR, channel * 3);
}

bool tiRadioTxLL(const void *pkt) {
  const uint8_t *data = (const uint8_t *)pkt;
  uint32_t len = 1 + *data, now;
  bool ret = false;

  ESP_LOGD(TAG, "TX packet len %u", len);
  ESP_LOGD(TAG, "First bytes: %02X %02X %02X %02X %02X", data[1], data[2], data[3], data[4], data[5]);

  if (tiRadioPrvStrobe(CMD_SIDLE) < 0)
    goto out;

  if (tiRadioPrvStrobe(CMD_SFTX) < 0)
    goto out;

  now = (len > 64 ? 64 : len);
  if (!tiRadioPrvRegWriteLong(REG_FIFO, data, now))
    goto out;
  len -= now;
  data += now;

  if (tiRadioPrvStrobe(CMD_STX) < 0)
    goto out;

  while (len) {

    now = 8;
    if (now > len)
      now = len;

    while (digitalRead(sub_GD2))
      ;  //wait till there is space

    if (!tiRadioPrvRegWriteLong(REG_FIFO, data, now))
      goto out;

    data += now;
    len -= now;
  }

  while (tiRadioPrvGetState() != 1)
    ;

  if (!tiRadioPrvStrobe(mRxEnabled ? CMD_SRX : CMD_SIDLE))
    goto out;

  ret = true;

out:
  return ret;
}


bool interrupt_attached = false;
bool init_subGhz(void) {
  uint8_t regsCfg[] = {
    /* 0x00 */ 0x02,    0x2e,    0x01,    0x07,    0xd3,    0x91,    0x7f,    0x04,
    /* 0x08 */ 0x45,    0x22,    0x00,    0x0e,    0x00,    0x22,    0xbb,    0x13,
    /* 0x10 */ 0x1d,    0x3b,    0x13,    0x43,    0xa4,    0x65,    0x07,    0x30,
    /* 0x18 */ 0x1d,    0x1e,    0x1c,    0xc7,    0x00,    0xb0,    0x87,    0x6b,
    /* 0x20 */ 0xfb,    0xb6,    0x10,    0xea,    0x2a,    0x00,    0x1f,    0x41,
    /* 0x28 */ 0x00,
  };

  uint8_t paTab[] = { 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0 };

  if (interrupt_attached)
    detachInterrupt(sub_GD0);

  tiRadioPrvIfInit();

  if (tiRadioPrvRegRead(REG_PARTNUM) != 0x00) {
    ESP_LOGE(TAG, "partnum is wrong");
    return false;
  }

  delayMicroseconds(300);

  if (tiRadioPrvStrobe(CMD_SRES) < 0) {
    ESP_LOGE(TAG, "res reply");
    return false;
  }

  delayMicroseconds(300);

  if (tiRadioPrvStrobe(CMD_SIDLE) != 0x0f) {
    ESP_LOGE(TAG, "idle reply");
    return false;
  }

  delayMicroseconds(300);

  if (!tiRadioPrvRegWriteLong(0, regsCfg, sizeof(regsCfg))) {
    ESP_LOGE(TAG, "config issue");
    return false;
  }

  delayMicroseconds(300);

  if (!tiRadioPrvRegWriteLong(REG_PATABLE, paTab, sizeof(paTab))) {
    ESP_LOGE(TAG, "PAtable issue");
    return false;
  }

  attachInterrupt(sub_GD0, data_input_interrupt, RISING);
  interrupt_attached = true;

  ESP_LOGI(TAG, "Sub-GHz radio inited");

  tiRadioRxEnable(true, false);
  delayMicroseconds(1000);
  ESP_LOGI(TAG, "Sub-GHz rx is on");
  return true;
}
