#include "tl_common.h"
#include "main.h"
#include "zigbee.h"
#include <stdint.h>
#include <stdbool.h>

RAM _attribute_aligned_(4) volatile uint8_t rf_tx_buf[ZB_RADIO_TX_HDR_LEN + 250];
RAM _attribute_aligned_(4) volatile uint8_t rf_rx_buf[ZB_RADIO_TX_HDR_LEN + 250];

#define LOGICCHANNEL_TO_PHYSICAL(p) (((p)-10) * 5)
void rf_setChannel(uint8_t chn)
{
	u32 r = drv_disable_irq();
	uint8_t phySta = ZB_RADIO_TRX_STA_GET();
	ZB_RADIO_TRX_SWITCH(phySta, LOGICCHANNEL_TO_PHYSICAL(chn));
	drv_restore_irq(r);
}

RAM volatile uint8_t tx_done = 0;
void zigbee_tx(uint8_t *buf, uint8_t len)
{
	ZB_RADIO_DMA_HDR_BUILD(rf_tx_buf, len);
	rf_tx_buf[4] = len + 2;
	memcpy(rf_tx_buf + 5, buf, len);
	WaitUs(ZB_TX_WAIT_US);
	ZB_RADIO_RX_DONE_CLR;
	tx_done = 0;
	ZB_RADIO_TX_START(rf_tx_buf);
	while (!tx_done)
		;
}

#define ZB_MAX_BUFFER 50

RAM volatile uint8_t rxed = 0;
RAM volatile uint8_t bbb[ZB_MAX_BUFFER][250];
RAM volatile uint8_t len = 0;
RAM volatile uint8_t rssi = 0;
RAM volatile uint8_t current_buff = 0;
RAM volatile uint8_t current_buff_worked = 0;

uint8_t getRxBuffer(uint8_t *bufferout, uint8_t *rssiout)
{
	if (current_buff != current_buff_worked)
	{
		uint8_t len_rx = bbb[current_buff_worked][0];
		*rssiout = bbb[current_buff_worked][1];
		memcpy(bufferout, (uint8_t *)&bbb[current_buff_worked][2], len_rx);
		current_buff_worked++;
		current_buff_worked %= ZB_MAX_BUFFER;
		return len_rx;
	}
	return 0;
}

void init_zigbee(uint_fast8_t channel)
{
	current_buff_worked = current_buff;
	rf_set_power_level_index(RF_POWER_INDEX_P3p01dBm);
	rf_drv_init(RF_MODE_ZIGBEE_250K);
	rf_drv_cap(CFG_FREQUENCY_OFFSET);
	drv_hwTmr_init(TIMER_IDX_3, TIMER_MODE_SCLK);
	ZB_RADIO_IRQ_MASK_SET;
	rf_setChannel(channel);
}

void zigbee_off()
{
	rf_set_tx_rx_off();
	current_buff_worked = current_buff;
}

void zigbee_rx_start()
{
	rf_rx_buffer_set(rf_rx_buf, 250, 0);
	rf_set_tx_rx_off();
	rf_set_rxmode();
}

 __attribute__((optimize("-Os"))) void rf_tx_irq_handler(void)
{
	// printf("TX IRQ\r\n");
	ZB_RADIO_TX_DONE_CLR;
	tx_done = 1;
}

 __attribute__((optimize("-Os"))) void rf_rx_irq_handler(void)
{
	if (RF_DMA_BUSY())
	{
		return;
	}
	ZB_RADIO_RX_DISABLE;
	if ((!ZB_RADIO_CRC_OK(rf_rx_buf)) || (!ZB_RADIO_PACKET_LENGTH_OK(rf_rx_buf)))
	{
		ZB_RADIO_RX_BUF_CLEAR(rf_rx_buf);
		ZB_RADIO_RX_ENABLE;
		return;
	}

	bbb[current_buff][0] = rf_rx_buf[4] - 2;
	bbb[current_buff][1] = (read_reg8(0x449));
	memcpy((uint8_t *)&bbb[current_buff][2], (uint8_t *)&rf_rx_buf[5], bbb[current_buff][0]);
	current_buff++;
	current_buff %= ZB_MAX_BUFFER;
	ZB_RADIO_RX_BUF_CLEAR(rf_rx_buf);
	ZB_RADIO_RX_ENABLE;
}

uint8_t channelList[6] = {11, 15, 20, 25, 26, 27};

bool next_ack = 0;
int state = 0;

#define RADIO_MAX_PACKET_LEN (250)

uint8_t seq_nr = 0;
uint8_t get_seq_nr()
{
	return seq_nr++;
}

#define MAX_RX_PKTS 8

volatile uint8_t mRxBufs[MAX_RX_PKTS][RADIO_MAX_PACKET_LEN + 1 /* length */ + 1 /* LQI */ + 1 /* RSSI */];
volatile uint8_t mRxNextWrite, mRxNextRead, mRxNumFree, mRxNumGot;
bool mRxEnabled = false;

RAM uint8_t our_ch = 11;
bool radioSetChannel(uint_fast8_t channel)
{
	our_ch = channel;
	init_zigbee(channel);
	return true;
}

bool radioRxEnable(bool on)
{
	if (on)
		zigbee_rx_start();
	return true;
}

void radioRxFlush(void)
{
	current_buff_worked = current_buff;
}

bool radioInit(void)
{
	return 1;
}

int32_t radioRxDequeuePkt(uint8_t *dstBuf, uint32_t maxLen, int8_t *rssiP, uint8_t *lqiP)
{
	u8 curr_rx = getRxBuffer(dstBuf, lqiP);
	*rssiP = lqiP;

	/*if (curr_rx)
	{

		printf("RX: ");
		for (int i = 0; i < curr_rx; i++)
		{
			printf(" %02X", dstBuf[i]);
		}
		printf("\r\n");
	}*/
	return curr_rx;
}

bool radioTxLL(uint8_t *pkt)
{
	init_zigbee(our_ch);
	zigbee_tx((uint8_t *)&pkt[1], pkt[0] - 2);
	zigbee_rx_start();
	return true;
}
