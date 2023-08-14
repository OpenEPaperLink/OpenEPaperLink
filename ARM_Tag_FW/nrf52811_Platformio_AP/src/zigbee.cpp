#include <Arduino.h>
#include "main.h"
#include "zigbee.h"
#include "utils.h"

uint8_t channelList[6] = {11, 15, 20, 25, 26, 27};

bool next_ack = 0;
int state = 0;

#define RADIO_MAX_PACKET_LEN (127)

uint8_t seq_nr = 0;
uint8_t get_seq_nr()
{
  return seq_nr++;
}

#define MAX_RX_PKTS 32

static volatile uint8_t mRxBufs[MAX_RX_PKTS][RADIO_MAX_PACKET_LEN + 1 /* length */ + 1 /* LQI */ + 1 /* RSSI */];
static volatile uint8_t mRxNextWrite, mRxNextRead, mRxNumFree, mRxNumGot;
static bool mRxEnabled = false;

void radioPrvDisable(void)
{
  NRF_RADIO->INTENCLR = 0xffffffff; // all ints off
  NRF_RADIO->EVENTS_DISABLED = 0;
  NRF_RADIO->TASKS_DISABLE = 1;
  while (!NRF_RADIO->EVENTS_DISABLED)
    ;
}

static void radioPrvDoTx(const void *pkt)
{
  radioPrvDisable();
  NRF_RADIO->PACKETPTR = (uintptr_t)pkt;
  NRF_RADIO->EVENTS_DISABLED = 0;
  NRF_RADIO->TASKS_TXEN = 1;
  while (!NRF_RADIO->EVENTS_DISABLED)
    ;
  NRF_RADIO->EVENTS_DISABLED = 0;
  NRF_RADIO->EVENTS_END = 0;
  (void)NRF_RADIO->EVENTS_END;
  NVIC_ClearPendingIRQ(RADIO_IRQn);
}

static void radioPrvQueueUpRx(void)
{
  if (mRxNumFree)
  {
    NRF_RADIO->PACKETPTR = (uintptr_t)mRxBufs[mRxNextWrite];
    NRF_RADIO->EVENTS_END = 0;
    (void)NRF_RADIO->EVENTS_END;
    NRF_RADIO->EVENTS_PAYLOAD = 0;
    (void)NRF_RADIO->EVENTS_PAYLOAD; // read to avoid re-triggering of irq
    NRF_RADIO->INTENSET = RADIO_INTENSET_PAYLOAD_Msk;
    NRF_RADIO->EVENTS_RXREADY = 0;
    NRF_RADIO->EVENTS_RSSIEND = 0;
    NRF_RADIO->TASKS_RXEN = 1;
  }
  else
    NRF_RADIO->INTENCLR = RADIO_INTENSET_PAYLOAD_Msk;
}

volatile uint8_t go_for_it = 0;
#ifdef __cplusplus
extern "C"
{
#endif
  void __attribute__((used)) RADIO_IRQHandler(void)
  {
    uint8_t *rxedPkt = (uint8_t *)mRxBufs[mRxNextWrite];
    int32_t len = (uint32_t)rxedPkt[0];
    bool crcOk;

    while (!NRF_RADIO->EVENTS_CRCOK && !NRF_RADIO->EVENTS_CRCERROR)
      ;

    crcOk = (NRF_RADIO->CRCSTATUS & RADIO_CRCSTATUS_CRCSTATUS_Msk) == RADIO_CRCSTATUS_CRCSTATUS_CRCOk;
    rxedPkt[len] = NRF_RADIO->RSSISAMPLE;

    mRxNumFree--;
    if (++mRxNextWrite == MAX_RX_PKTS)
      mRxNextWrite = 0;
    mRxNumGot++;

    NRF_RADIO->EVENTS_END = 0;
    (void)NRF_RADIO->EVENTS_END;
    NRF_RADIO->EVENTS_CRCOK = 0;
    NRF_RADIO->EVENTS_CRCERROR = 0;
    NRF_RADIO->EVENTS_PAYLOAD = 0;
    (void)NRF_RADIO->EVENTS_PAYLOAD; // read to avoid re-triggering of irq

    NVIC_ClearPendingIRQ(RADIO_IRQn);

    radioPrvQueueUpRx();
  }
#ifdef __cplusplus
}
#endif

static bool radioPrvRxEnable(void) // knows that radio is in disabled state
{
  radioPrvQueueUpRx();
  return true;
}

bool radioSetChannel(uint_fast8_t channel)
{
  if (channel < 11 || channel > 26)
    return false;
  NRF_RADIO->FREQUENCY = 5 * (channel - 10);
  return true;
}

bool radioRxEnable(bool on)
{
  if (!on == !mRxEnabled) // is there a state change?
    return true;
  mRxEnabled = on;
  if (on)
    radioPrvRxEnable();
  else
    radioPrvDisable();
  return true;
}

void radioRxFlush(void)
{
  NVIC_DisableIRQ(RADIO_IRQn);
  mRxNextWrite = 0;
  mRxNextRead = 0;
  mRxNumFree = MAX_RX_PKTS;
  mRxNumGot = 0;
  NVIC_EnableIRQ(RADIO_IRQn);
}

bool radioInit(void)
{
  uint32_t i;
  // reset it
  for (i = 0; i < 1000; i++)
  {
    NRF_RADIO->POWER = 0;
    (void)NRF_RADIO->POWER;
  }
  NRF_RADIO->POWER = 1;
  radioPrvDisable();
  startHFCLK();
  // configure the CRC subunit
  NRF_RADIO->CRCCNF = (RADIO_CRCCNF_SKIPADDR_Ieee802154 << RADIO_CRCCNF_SKIPADDR_Pos) | (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos);
  NRF_RADIO->CRCPOLY = 0x11021;
  NRF_RADIO->CRCINIT = 0;
  // IEE802.15.4 radio settings
  NRF_RADIO->SFD = 0xA7 << RADIO_SFD_SFD_Pos;
  NRF_RADIO->PCNF0 = (8 << RADIO_PCNF0_LFLEN_Pos) | (RADIO_PCNF0_PLEN_32bitZero << RADIO_PCNF0_PLEN_Pos) | RADIO_PCNF0_CRCINC_Msk;
  NRF_RADIO->PCNF1 = ((RADIO_MAX_PACKET_LEN + 1 /* len is part of txed val */) << RADIO_PCNF1_MAXLEN_Pos);
  NRF_RADIO->MODECNF0 = (RADIO_MODECNF0_DTX_Center << RADIO_MODECNF0_DTX_Pos) | (RADIO_MODECNF0_RU_Fast << RADIO_MODECNF0_RU_Pos);
  NRF_RADIO->TIFS = 192;
  // general radio things
  NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Pos4dBm << RADIO_TXPOWER_TXPOWER_Pos;
  NRF_RADIO->MODE = RADIO_MODE_MODE_Ieee802154_250Kbit << RADIO_MODE_MODE_Pos;
  // shortcuts
  NRF_RADIO->SHORTS = RADIO_SHORTS_PHYEND_DISABLE_Msk | RADIO_SHORTS_READY_START_Msk | RADIO_SHORTS_ADDRESS_RSSISTART_Msk;

  mRxEnabled = false;
  radioRxFlush();

  NVIC_SetPriority(RADIO_IRQn, 1);
  NVIC_ClearPendingIRQ(RADIO_IRQn);
  NVIC_EnableIRQ(RADIO_IRQn);

  return radioSetChannel(11);
}

int32_t radioRxDequeuePkt(void *dstBuf, uint32_t maxLen, int8_t *rssiP, uint8_t *lqiP)
{
  uint32_t len, copyLen = maxLen;

  if (!mRxNumGot)
    return -1;

  len = mRxBufs[mRxNextRead][0];
  if (len >= sizeof(uint16_t)) // remove CRC
    len -= sizeof(uint16_t);

  if (copyLen > len)
    copyLen = len;
  memcpy(dstBuf, (const void *)(mRxBufs[mRxNextRead] + 1), copyLen);
  if (lqiP)
  {

    uint32_t lqi = mRxBufs[mRxNextRead][len + 1];
    lqi *= 4;
    if (lqi > 255)
      lqi = 255;
    *lqiP = lqi;
  }

  if (rssiP)
    *rssiP = -(int8_t)mRxBufs[mRxNextRead][len + 2];

  if (++mRxNextRead == MAX_RX_PKTS)
    mRxNextRead = 0;

  NVIC_DisableIRQ(RADIO_IRQn);
  mRxNumFree++;
  mRxNumGot--;
  NVIC_EnableIRQ(RADIO_IRQn);
  if (mRxEnabled && !(NRF_RADIO->INTENSET & RADIO_INTENSET_PAYLOAD_Msk))
    radioPrvQueueUpRx();
  return len;
}

bool radioTxLL(const void *pkt)
{
  radioPrvDoTx(pkt);
  if (mRxEnabled)
    return radioPrvRxEnable();
  else
    return true;
}
