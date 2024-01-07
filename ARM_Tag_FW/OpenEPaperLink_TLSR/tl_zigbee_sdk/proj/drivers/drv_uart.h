/********************************************************************************************************
 * @file    drv_uart.h
 *
 * @brief   This is the header file for drv_uart
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


#if defined(MCU_CORE_826x) || defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	#define UART_CLOCK_SOURCE			CLOCK_SYS_CLOCK_HZ
#elif defined(MCU_CORE_B91)
	/* PCLK provides clock source for UART module. */
	#define UART_CLOCK_SOURCE			(sys_clk.pclk * 1000 * 1000)
#endif

enum{
	UART_STA_IDLE,
	UART_STA_TX_DOING,
	UART_STA_TX_DONE,
	UART_STA_RX_DONE,
};

typedef void (*uart_irq_callback)(void);
typedef u8 (*uart_data_send)(u8 *data);

typedef struct{
	uart_irq_callback recvCb;
	uart_data_send send;
	volatile u8 status;
}drv_uart_t;


/****************************************************************************************
 *	@brief	uart tx/rx pin set
 *
 *	@param  txPin		TX pin
 *	@param  rxPin		RX pin
 *
 *	@return	none
 */
void drv_uart_pin_set(u32 txPin, u32 rxPin);

/****************************************************************************************
 *	@brief	init UART receive callback function
 *
 *	@param  baudRate		baud rate
 *	@param  rxBuf			the buffer for data receiving
 *	@param  rxBufLen		the length of the uart RX buffer, must be a multiple of 4
 *	@param	uart_recvCb		UART receive callback function.
 *
 *	@return	none
 */
void drv_uart_init(u32 baudRate, u8 *rxBuf, u16 rxBufLen, uart_irq_callback uartRecvCb);

/****************************************************************************************
 *	@brief	uart Rx ISR
 *
 */
void drv_uart_rx_irq_handler(void);

/****************************************************************************************
 *	@brief	uart Tx ISR
 *
 */
void drv_uart_tx_irq_handler(void);

/****************************************************************************************
 *	@brief	uart data send function
 *
 *	@param	data	start address of the data
 *	@param	len		length of data
 *
 *	@return	1: finished, 0: failed
 */
u8 drv_uart_tx_start(u8 *data, u32 len);

/****************************************************************************************
 *	@brief	check if exception occurs
 *
 */
void drv_uart_exceptionProcess(void);
