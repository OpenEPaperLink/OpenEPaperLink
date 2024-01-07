/********************************************************************************************************
 * @file    drv_radio.h
 *
 * @brief   This is the header file for drv_radio
 *
 * @author  Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/

#pragma once

/*******************************************************************************************************
 * 									Radio interface for 8258
 ******************************************************************************************************/
/* radio module reset */
#define ZB_RADIO_RESET()

/* RF is busy */
#define RF_DMA_BUSY()									is_rf_receiving_pkt()

/* trx switch */
#define ZB_RADIO_TRX_SWITCH(mode, chn) 					rf_trx_state_set(mode, chn)

/* trx off auto mode */
#define ZB_RADIO_TRX_OFF_AUTO_MODE()					rf_set_tx_rx_off_auto_mode()

/* srx start*/
#define ZB_RADIO_SRX_START(tick)						rf_start_srx(tick)

/* set tx power */
#define ZB_RADIO_TX_POWER_SET(level)					rf_set_power_level_index(level)

/* set rx gain mode: max gain: only use auto gain mode */
#define ZB_RADIO_MODE_MAX_GAIN()

/* set rx gain mode: auto gain, only use auto gain mode */
#define ZB_RADIO_MODE_AUTO_GAIN()

/* get RSSI */
#define ZB_RADIO_RSSI_GET()								rf_rssi_get_154()

/* start sending a packet */
#define ZB_RADIO_TX_START(txBuf)						rf_tx_pkt(txBuf)

/* tx done */
#define ZB_RADIO_TX_DONE								rf_tx_finish()

/* clear tx status(interrupt status) */
#define ZB_RADIO_TX_DONE_CLR							rf_tx_finish_clear_flag()

/* rx done */
#define ZB_RADIO_RX_DONE								rf_rx_finish()

/* clear rx status(interrupt status) */
#define ZB_RADIO_RX_DONE_CLR							rf_rx_finish_clear_flag()

/* enable tx */
#define ZB_RADIO_TX_ENABLE								RFDMA_TX_ENABLE

/* disable tx */
#define ZB_RADIO_TX_DISABLE								RFDMA_TX_DISABLE

/* enable rx */
#define ZB_RADIO_RX_ENABLE								RFDMA_RX_ENABLE

/* disable rx */
#define ZB_RADIO_RX_DISABLE								RFDMA_RX_DISABLE

/* clear mask bit to disable tx irq */
#define ZB_RADIO_IRQ_MASK_CLR							irq_clr_mask(FLD_IRQ_ZB_RT_EN)

/* clear mask bit to disable tx irq */
#define ZB_RADIO_IRQ_MASK_SET							irq_set_mask(FLD_IRQ_ZB_RT_EN)

/* trx status get */
#define ZB_RADIO_TRX_STA_GET()							rf_trx_state_get()

/* rx buffer configure */
#define ZB_RADIO_RX_BUF_SET(addr)						rf_rx_buffer_reconfig(addr)

/* rx buffer clear */
#define ZB_RADIO_RX_BUF_CLEAR(p)						do{ \
															p[0] = 0;		\
															p[4] = 0;		\
														}while(0)

/* set rx mode, maxium receiver buffer size, enable Rx/Tx interrupt */
#define ZB_RADIO_TRX_CFG(size)							do{ \
															/* disable SRX timeout interrupt */\
															write_reg8(0xf03, read_reg8(0xf03) & 0xfb);	\
															rf_rx_cfg(size, 0); \
															dma_irq_disable(FLD_DMA_CHN_RF_RX | FLD_DMA_CHN_RF_TX); \
															rf_irq_disable(FLD_RF_IRQ_ALL);	\
															rf_irq_enable(FLD_RF_IRQ_RX | FLD_RF_IRQ_TX); \
															irq_set_mask(FLD_IRQ_ZB_RT_EN); \
														}while(0)

#define ZB_RADIO_DMA_HDR_BUILD(pBuf, len)				do{	\
															u8 *p = pBuf;	\
															p[0] = len + 1;	\
															p[1] = 0;		\
															p[2] = 0;		\
															p[3] = 0;		\
														}while(0)

/* tx packet header */
#define ZB_RADIO_TX_HDR_LEN								5

/* rx packet header */
#define ZB_RADIO_RX_HDR_LEN								5

/* delay after switch to Tx mode, and then start sending */
#define ZB_TX_WAIT_US									120

/* get real payload length */
#define ZB_RADIO_ACTUAL_PAYLOAD_LEN(p)					RF_ZIGBEE_PACKET_PAYLOAD_LENGTH_GET(p)

/* packet CRC check */
#define ZB_RADIO_CRC_OK(p)								RF_ZIGBEE_PACKET_CRC_OK(p)

/* packet length check */
#define ZB_RADIO_PACKET_LENGTH_OK(p)					RF_ZIGBEE_PACKET_LENGTH_OK(p)

/* enable timestamp */
#define ZB_TIMESTAMP_ENABLE								do{ \
															REG_ADDR8(0x430) |= BIT(1);\
														}while(0)

/* get time timestamp */
#define ZB_RADIO_TIMESTAMP_GET(p)						RF_ZIGBEE_PACKET_TIMESTAMP_GET(p)

#define ZB_RADION_PKT_RSSI_GET(p)						RF_ZIGBEE_PACKET_RSSI_GET(p)

/* tx power 0dBm*/
#define ZB_RADIO_TX_0DBM								RF_POWER_INDEX_P0p04dBm

/* radio initialization */
#define ZB_RADIO_INIT()									do{ \
															rf_drv_init(RF_MODE_ZIGBEE_250K); \
															rf_drv_cap(CFG_FREQUENCY_OFFSET); \
														}while(0)

/* sys timer initialization for mac-csma */
#define ZB_TIMER_INIT()									drv_hwTmr_init(TIMER_IDX_3, TIMER_MODE_SCLK)

#define ZB_RADIO_RSSI_TO_LQI(mode, rssi, lqi)			do{ \
															(void)mode;									\
															s16 minEd = -99;							\
															s16 maxEd = -15;  /* AGC */					\
															if(rssi > maxEd){rssi = maxEd;}				\
															if(rssi < minEd){rssi = minEd;}				\
															lqi = 255*(rssi - minEd)/(maxEd - minEd); 	\
														}while(0)

/* RSSI: [-99 ~ -15]
 *
 *  RSSI    LQI		COST
 *  -60		118      1		//excellent
 *  -68		94		 2		//good
 *  -76		69		 3		//ok
 *  -84		45		 5		//weak
 *  -99		0		 7
 */
#define ZB_LQI_TO_PATH_COST(lqi, path_cost) 			do{ \
															if(lqi > 118){path_cost = 1;}		\
															else if(lqi > 94){path_cost = 2;}	\
															else if(lqi > 69){path_cost = 3;}	\
															else if(lqi > 45){path_cost = 5;}	\
															else{path_cost = 7;}				\
														}while(0)
