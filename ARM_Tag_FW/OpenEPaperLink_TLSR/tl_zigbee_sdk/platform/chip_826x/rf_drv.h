/********************************************************************************************************
 * @file    rf_drv.h
 *
 * @brief   This is the header file for B86
 *
 * @author  Driver & Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *			All rights reserved.
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

#ifndef	RF_H
#define RF_H

#include "register.h"

/**
 *  @brief  Define crystal,Currently only supports RF_OSC_12M
 */
typedef enum {
	RF_OSC_16M = 0,
	RF_OSC_12M = 1
} RF_OscSelTypeDef;

/**
 *  @brief  Define RF mode
 */
typedef enum {
	RF_MODE_BLE_2M 			= 0,
	RF_MODE_BLE_1M 			= 1,
	RF_MODE_ZIGBEE_250K 	= 2,
	RF_MODE_PRIVATE_2M 		= 3,
	RF_MODE_STANDARD_BLE_1M = 4
} RF_ModeTypeDef;

/**
 *  @brief  Define RF state
 */
typedef enum {
  RF_MODE_TX 	= 0,
  RF_MODE_RX 	= 1,
  RF_MODE_AUTO 	= 2,
  RF_MODE_OFF 	= 3
} RF_StatusTypeDef;

/**
 *  @brief  Define RF Tx power level
 */
typedef enum {
	RF_POWER_7dBm		= 0,
	RF_POWER_5dBm		= 1,
	RF_POWER_m0P6dBm	= 2,
	RF_POWER_m4P3dBm	= 3,
	RF_POWER_m9P5dBm	= 4,
	RF_POWER_m13P6dBm	= 5,
	RF_POWER_m18P8dBm	= 6,
	RF_POWER_m23P3dBm	= 7,
	RF_POWER_m27P5dBm	= 8,
	RF_POWER_m30dBm		= 9,
	RF_POWER_m37dBm		= 10,
	RF_POWER_OFF		= 11,
} RF_PowerTypeDef;

extern RF_ModeTypeDef g_RFMode;

#define	TP0_THRESHOLD_H					0x4a
#define	TP0_THRESHOLD_L					0x36
#define	TP0_DEFAULT_VALUE				0x40

#define	TP1_THRESHOLD_H					0x43
#define	TP1_THRESHOLD_L					0x2f
#define	TP1_DEFAULT_VALUE				0x39

//ble
#define	RF_BLE_PACKET_LENGTH_OK(p)		(p[0] == (p[13]&0x3f)+17)
#define	RF_BLE_PACKET_CRC_OK(p)			((p[p[0]+3] & 0x51) == 0x40)
//zigbee
#define	ZIGBEE_MAX_PACKET_SIZE			127
#define	RF_PACKET_LENGTH_OK(p)			((p[0] == p[12]+13) && p[0] <= (ZIGBEE_MAX_PACKET_SIZE+13))
#define	RF_PACKET_CRC_OK(p)				((p[p[0]+3] & 0x51) == 0x10)
#define RF_GET_TIMESTAMP(p)    			(p[8] | (p[9]<<8) | (p[10]<<16) | (p[11]<<24))
#define RF_GET_PKT_RSSI(p)    			(p[4])
#define RF_GET_PKT_PAYLOAD_LEN(p)		(p[12])


#define	RFDMA_RX_DISABLE				(reg_dma_chn_en &= (~FLD_DMA_RF_RX))
#define	RFDMA_RX_ENABLE					(reg_dma_chn_en |= (FLD_DMA_RF_RX))

#define	RFDMA_TX_DISABLE				(reg_dma_chn_en &= (~FLD_DMA_RF_TX))
#define	RFDMA_TX_ENABLE					(reg_dma_chn_en |= (FLD_DMA_RF_TX))

#define RF_TxFinishClearFlag()		 	(reg_rf_irq_status = FLD_RF_IRQ_TX)
#define RF_RxFinishClearFlag()		 	(reg_rf_irq_status = FLD_RF_IRQ_RX)
#define RF_TxFinish()				 	(reg_rf_irq_status & FLD_RF_IRQ_TX)

static inline void rf_irq_enable(unsigned int msk)
{
    reg_rf_irq_mask |= msk;
}

static inline void rf_irq_disable(unsigned int msk)
{
    reg_rf_irq_mask &= (~msk);
}

static inline void RF_rx_buffer_reconfig(unsigned char *RF_RxAddr)
{
	reg_dma2_addr = (unsigned short)((unsigned int)RF_RxAddr);
}

/********************************************************************************
*	@brief	 This function is used during RF mode switch to reset Baseband.
*	@return	 	none
*/
extern void RF_BaseBandReset(void);

/********************************************************************************
*	@brief		This function should be invoked first,before the RF_TrxStateSet
*				is invoked
*
*	@param[in]	OscSel   enum variable of external crystal RF_OSC_16M/RF_OSC_12M
*	@param[in]	RF_Mode      enum variable of RF mode
*	@return		'1' :set success; '0':parameters set error
*/
extern int RF_Init(RF_OscSelTypeDef OscSel, RF_ModeTypeDef RF_Mode);

extern void RF_cal(unsigned long tpAddr, unsigned long capAddr);

/********************************************************************************
*	@brief	  	This function serves to switch to maxgain mode with better
*				Rx performance,	and will only take effect if its invoked after
*				the RF_Init.The RF_Init sets the mode as AGC mode by default.
*
*	@param[in]	length  	Optional range: 3~5
*							Note: The effect for 3-byte access_code is not good.
*
*	@return	 	none
*/
extern void RF_SetGainManualMax(void);
/********************************************************************************
*	@brief	  	This function serves to switch to AGC mode from maxgain mode,
*				and will only take effect if its invoked after the RF_Init.
*
*	@param[in]	length  	Optional range: 3~5
*							Note: The effect for 3-byte access_code is not good.
*
*	@return	 	none
*/
extern void RF_SetAgc(void);

/********************************************************************************
*	@brief		This function serves to switch RF mode. After this function is
*				invoked, the RF_TrxStateSet and RF_BaseBandReset should be
*				invoked, as shown below:
*				RF_ModeSet(RF_MODE_BLE_1M);
*		   		RF_TrxStateSet(RF_MODE_TX,20);
*		   		WaitUs(150);//wait pllclock
*		 		RF_BaseBandReset();
*
*	@param[in]	RF_Mode   Set working mode for RF module
*	@return	 	'0' :set success; '-1':parameters set error
*/
extern int RF_ModeSet(RF_ModeTypeDef RF_Mode);

/********************************************************************************
*	@brief	 This function is to update TP(two point),this value will affect
*			 RF performance
*			 it needs to use  before the function  "RF_TrxStateSet"
*
*	@param[in]	RF_Mode  	Set working mode for RF module.
*	@param[in]	RF_TPLow  	Tp value for lower frequency (2400Mhz)
*							range: 2M/250K : 0x40(+/-)10   1M :0x1d(+/-)10
*							If you set a value outside the range, you will be set to fail.
*	@param[in]	RF_TPHigh  	Tp value for higher frequency (2480Mhz)
*							range: 2M/250K : 0x39(+/-)10   1M :0x19(+/-)10
*							If you set a value outside the range, you will be set to fail.
*
*	@return	 	'0' :set success; '-1':set failed
*/
extern int RF_UpdateTpValue(RF_ModeTypeDef RF_Mode ,unsigned char RF_TPLow, unsigned char RF_TPHigh);

/********************************************************************************
*	@brief	 This function is to update cap value to affect RF performance
*			 it needs to use after the function  "RF_Init"
*
*	@param[in]	RF_Cap  	cap value : <4:0> is cap value(0x00 - 0x1f)
*
*	@return	 	none
*/
extern void RF_UpdateCapValue(unsigned char RF_Cap);

/********************************************************************************
*	@brief	  	This function is to select Tx power level. Parameter type is enum.
*
*	@param[in]	RF_TxPowerLevel   select Tx power level
*
*	@return	 	none
*/
extern void RF_PowerLevelSet(RF_PowerTypeDef RF_TxPowerLevel);

/********************************************************************************
*	@brief	  	This function is to set Rf channel and state(rx/tx/auto),After
*				this function is invoked and state is rx or tx,a 100us~150us
*				delay is needed to lock PLL. Then its ready to carry out packet
*				Rx/Tx operations.
*
*	@param[in]	RF_Status   set current status for RF module(tx/rx/auto),
*						Note: When the RF_Status is set as auto mode, the functions
*						RF_StartStx, RF_StartStxToRx and RF_StartSrxToTx
*						can be used.
*	@param[in]	RF_Status Unit is MHz.
*						The frequency is set as (2400+ channel) MHz.(0~100)
*						Note: It should be set as the same value for Rx and Tx
*						terminal.
*	@return	 	'0' :set success; '-1':parameters set error
*/
extern int RF_TrxStateSet(RF_StatusTypeDef RF_Status, signed char RF_Channel);

/********************************************************************************
*   @brief      This function is to get the current state of the RF transceiver
*
*   @param[in]  None
*   @return     RF_StatusTypeDef
*/
extern RF_StatusTypeDef RF_TrxStateGet(void);

/********************************************************************************
*	@brief	  	This function is to send packet
*
*	@param[in]	RF_TxAddr   Tx packet address in RAM. Should be 4-byte aligned.
*					  		 The format for Tx packet in RAM is shown as below:
*					 		(length indicates data length, data indicates data
*					 		 to be transmitted.)
*	BLE (1M, 2M) mode					Zigbee (250K) mode, Private (2M) mode
*	addr			length+2				addr				Length+1
*	addr+1				0					addr+1					0
*	addr+2				0					addr+2					0
*	addr+3				0					addr+3					0
*	addr+4				0					addr+4				Length+2
*	addr+5			length					addr+5				data(0)
*	addr+6			data(0)					addr+6				data(1)
*	addr+7			data(1)					addr+7				data(2)
*												
*	addr+5+(length)	data(length-1)			addr+4+(length)		data(length-1)
*
*	@return	 	none
*/
extern void RF_TxPkt(unsigned char *RF_TxAddr);

/********************************************************************************
*	@brief	  	This function is to set rx buffer
*
*	@param[out]	RF_RxAddr  	Pointer for Rx buffer in RAM(Generally its starting
*							address of an array.Should be 4-byte aligned)
*	@param[in]	Size   		Rx buffer size (Its an integral multiple of 16)
*	@param[in]	PingpongEn 	Enable/Disable Ping-Pong buffer 1Enable 0Disable
*							Note:
*							When PingpongEn is set as 0, received RF data will
*							be stored in RAM pointed by  RF_RxAddr.
*							When PingpongEn is set as 1, received RF data will
*							be stored in buffer0 and buffer1 successively.
*							The RAM size reserved for received RF data should be
*							double of Size.
*
*	@return	 	none
*/
extern void RF_RxBufferSet(unsigned char *RF_RxAddr, int Size, unsigned char PingpongEn);

extern void RF_rx_cfg(int Size, unsigned char PingpongEn);

/********************************************************************************
*	@brief	  	This function serves to start STX mode of auto_mode.
*				This mode will end after a packet is transmitted.
*
*	@param[in]	RF_TxAddr   Tx packet address in RAM. Should be 4-byte aligned.
*	@param[in]	RF_StartTick  Tick value of system timer. It determines when to
*						  	  start STX mode and send packet.
*	@return	 	none
*/
extern void RF_StartStx(unsigned char *RF_TxAddr, unsigned int RF_StartTick);

/********************************************************************************
*	@brief	  	This function serves to start Srx mode of auto_mode. In this mode,
*				RF module stays in Rx status until a packet is received or it fails to receive packet when timeout expires.
*				Timeout duration is set by the parameter "RF_RxTimeoutUs".
*				The address to store received data is set by the function RF_RxBufferSet.
*
*	@param[in]	tick   		  	 Tick value of system timer. It determines when to start Srx mode.
*	@param[in]	timeout_us   	 Unit is us. It indicates timeout duration in Rx status.Max value: 0xffffff (16777215)
*
*	@return	 	none
*/
extern void RF_StartSrx(unsigned int RF_StartTick, unsigned int RF_RxTimeoutUs);

/********************************************************************************
*	@brief	  	This function serves to start StxToRx mode of auto_mode.
*				In this mode, a packet is sent first,RF module waits for 10us,
*				stays in Rx status until data is received or timeout expires,
*				then exits this mode.Timeout duration is set by the parameter
*				RF_RxTimeoutUs.The address to store received data is set by the
*				function RF_RxBufferSet.
*
*	@param[in]	RF_TxAddr  Tx packet address in RAM. Should be 4-byte aligned.
*	@param[in]	RF_StartTick   	Tick value of system timer. It determines when
*								to start StxToRx mode and send packet.
*	@param[in]	RF_RxTimeoutUs  Unit is us. It indicates timeout duration in
*							 	Rx status.Max value: 0xfff (4095)
*
*	@return	 	none
*/
extern void RF_StartStxToRx(unsigned char *RF_TxAddr, unsigned int RF_StartTick, unsigned short RF_RxTimeoutUs);

/********************************************************************************
*	@brief	  	This function serves to start SrxToTx mode of auto_mode.
*				In this mode,RF module stays in Rx status until a packet is
*				received or it fails to receive packetwhen timeout expires.
*				If a packet is received within the timeout duration, RF module
*				will wait for 10us,send a packet, and then exit this mode.
*				If it fails to receive packet when timeout expires, RF module
*				will directly exit this mode.Timeout duration is set by the
*				parameter "RF_RxTimeoutUs".	The address to store received data is set
*				by the function RF_RxBufferSet.
*
*	@param[in]	RF_TxAddr 	Tx packet address in RAM. Should be 4-byte aligned.
*	@param[in]	RF_StartTick   Tick value of system timer. It determines when to
*								start SrxToTx mode.
*	@param[in]	RF_RxTimeoutUs  Unit is us. It indicates timeout duration in Rx status.
*								Max value: 0xffffff (16777215)
*
*	@return	 	none
*/
extern void RF_StartSrxToTx(unsigned char* RF_TxAddr  ,unsigned int RF_StartTick,unsigned int RF_RxTimeoutUs);

/********************************************************************************
*	@brief	  	This function is turn off tx and rx auto mode
*
*	@return	 	none
*/
extern void RF_SetTxRxOffAutoMode(void);

/********************************************************************************
*	@brief	  	This function is turn off tx and rx
*
*	@return	 	none
*/
extern void RF_SetTxRxOff(void);

/********************************************************************************
*	@brief	  	This function is to obtain the RSSI value of the current channel
*				Before using it, it is necessary to set the "RF_MODE_RX" by "RF_TrxStateSet"
*
*
*	@return	 	RSSI value :  unit is dbm
*/
extern signed char RF_GetRssi(void);

extern void RF_reset(void);

/********************************************************************************
*	@brief	  	This function is to get rx buffer result
*
*	@return	 	0:There are received RF data in buffer 0.
*				1:There are received RF data in buffer 1.
*				2:No RF data is received.
*/
extern unsigned char RF_RxBufferRequest(void);

/********************************************************************************
*	@brief	  	This function serves to clear the flag bit for Rx buffer.
*				After RF data are received, flag bits will be generated for
*				buffer 0 and buffer 1.By reading corresponding flag bit, it can
*				check whether RF Rx event occurs.After the received data
*				are processed, its needed to clear flag bits for buffer 0
*				and buffer 1,so as to avoid regarding the old data as new by mistake.

*	@param[in]	RF_RxBufferIdx   0:buffer0  1:buffer1
*
*	@return	 	none
*/
extern void RF_RxBufferClearFlag(unsigned char RF_RxBufferIdx);


#endif
