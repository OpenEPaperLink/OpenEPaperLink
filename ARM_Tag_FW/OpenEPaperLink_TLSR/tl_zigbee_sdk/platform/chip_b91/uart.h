/********************************************************************************************************
 * @file	uart.h
 *
 * @brief	This is the header file for B91
 *
 * @author	Driver Group
 * @date	2019
 *
 * @par     Copyright (c) 2019, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
/**	@page UART
 *
 *	Introduction
 *	===============
 *	B91 supports two uart: uart0~ uart1.
 *
 *	API Reference
 *	===============
 *	Header File: uart.h
 */
#ifndef     UART_H_
#define     UART_H_

#include <reg_include/register.h>
#include "gpio.h"
#include "dma.h"
#include "timer.h"

extern unsigned char uart_rx_byte_index[2];
extern unsigned char uart_tx_byte_index[2];

#define uart_rtx_pin_tx_trig(uart_num)  uart_clr_tx_done(uart_num)


/**********************************************************************************************************************
 *                                         global constants                                                           *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *                                           global macro                                                             *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *                                         global data type                                                           *
 *********************************************************************************************************************/
/**
 *  @brief  Define parity type
 */
typedef enum {
	UART_PARITY_NONE = 0,
	UART_PARITY_EVEN,
	UART_PARITY_ODD,
} uart_parity_e;

/**
 *  @brief  Define UART chn
 */
typedef enum {
	UART0 = 0,
	UART1,
}uart_num_e;

/**
 *  @brief  Define mul bits
 */
typedef enum{
	UART_BW_MUL1  = 0,
	UART_BW_MUL2  = 1,
	UART_BW_MUL3  = 2,
	UART_BW_MUL4  = 3,
} uart_timeout_mul_e;

/**
 *  @brief  Define the length of stop bit
 */
typedef enum {
	UART_STOP_BIT_ONE          = 0,
	UART_STOP_BIT_ONE_DOT_FIVE = BIT(4),
	UART_STOP_BIT_TWO          = BIT(5),
} uart_stop_bit_e;

/**
 *  @brief  Define UART RTS mode
 */
typedef enum {
    UART_RTS_MODE_AUTO = 0,
    UART_RTS_MODE_MANUAL,
} uart_rts_mode_e;

/**
 *  @brief  Define UART CTS pin : UART0(PA1 PB6 PD0), UART1(PC4 PD4 PE1)
 */
typedef enum{
	UART0_CTS_PA1 = GPIO_PA1,
	UART0_CTS_PB6 = GPIO_PB6,
	UART0_CTS_PD0 = GPIO_PD0,

	UART1_CTS_PC4 = GPIO_PC4,
	UART1_CTS_PD4 = GPIO_PD4,
	UART1_CTS_PE1 = GPIO_PE1,
}uart_cts_pin_e;

/**
 *  @brief  Define UART RTS pin : UART0(PA2 PB4 PD1), UART1(PC5 PD5 PE3)
 */
typedef enum{
	UART0_RTS_PA2 = GPIO_PA2,
	UART0_RTS_PB4 = GPIO_PB4,
	UART0_RTS_PD1 = GPIO_PD1,

	UART1_RTS_PC5 = GPIO_PC5,
	UART1_RTS_PD5 = GPIO_PD5,
	UART1_RTS_PE3 = GPIO_PE3,
}uart_rts_pin_e;

/**
 *  @brief  Define UART TX pin : UART0(PA3 PB2 PD2), UART1(PC6 PD6 PE0)
 */
typedef enum{
	UART0_TX_PA3 = GPIO_PA3,
	UART0_TX_PB2 = GPIO_PB2,
	UART0_TX_PD2 = GPIO_PD2,

	UART1_TX_PC6 = GPIO_PC6,
	UART1_TX_PD6 = GPIO_PD6,
	UART1_TX_PE0 = GPIO_PE0,
}uart_tx_pin_e;


/**
 *  @brief  Define UART RX pin : UART0(PA4 PB3 PD3), UART1(PC7 PD7 PE2)
 */
typedef enum{
	UART0_RX_PA4 = GPIO_PA4,
	UART0_RX_PB3 = GPIO_PB3,
	UART0_RX_PD3 = GPIO_PD3,

	UART1_RX_PC7 = GPIO_PC7,
	UART1_RX_PD7 = GPIO_PD7,
	UART1_RX_PE2 = GPIO_PE2,
}uart_rx_pin_e;

/**
 *  @brief  Define UART IRQ MASK.The enumeration variable is just a index, and actually needs to be operated registers behind.
 */
typedef enum{
	UART_RX_IRQ_MASK  = BIT(0),//reg_uart_ctrl0(uart_num)       BIT(6)
	UART_TX_IRQ_MASK  = BIT(1),//reg_uart_ctrl0(uart_num)       BIT(7)
	UART_RXDONE_MASK  = BIT(2),//reg_uart_rx_timeout1(uart_num) BIT(2)
	UART_TXDONE_MASK  = BIT(3),//reg_uart_rx_timeout1(uart_num) BIT(6)
	UART_ERR_IRQ_MASK = BIT(4),//reg_uart_rx_timeout1(uart_num) BIT(7)
}uart_irq_mask_e;

/**
 *  @brief  Define UART IRQ BIT STATUS FOR GET
 */
typedef enum{
	UART_RX_ERR				= BIT(7),
	UART_TXDONE				= BIT(0),
	UART_TXBUF_IRQ_STATUS 	= BIT(1),
	UART_RXDONE				= BIT(2),
	UART_RXBUF_IRQ_STATUS 	= BIT(3),
}uart_irq_status_get_e;

/**
 *  @brief  Define UART IRQ BIT STATUS FOR CLR
 */
typedef enum{
	UART_CLR_RX				= BIT(6),
	UART_CLR_TX				= BIT(7),
}uart_irq_status_clr_e;

/**********************************************************************************************************************
 *                                     global variable declaration                                                    *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *                                      global function prototype                                                     *
 *********************************************************************************************************************/
/**
 * @brief     This function serves to get the rxfifo cnt.
 * @param[in] uart_num - UART0/UART1.
 * @return    none
 */
static inline unsigned char uart_get_rxfifo_num(uart_num_e uart_num)
{
	return reg_uart_buf_cnt(uart_num)&FLD_UART_RX_BUF_CNT ;
}

/**
 * @brief     This function serves to get the txfifo cnt.
 * @param[in] uart_num - UART0/UART1.
 * @return    none
 */
static inline unsigned char uart_get_txfifo_num(uart_num_e uart_num)
{
	return (reg_uart_buf_cnt(uart_num)&FLD_UART_TX_BUF_CNT )>>4;
}

/**
 * @brief     This function resets the UART module.
 * @param[in] uart_num - UART0 or UART1.
 * @return    none
 */
static inline void uart_reset(uart_num_e uart_num)
{
	/*
	  In B91, tx_done is 1 by default, after uart reset(write 0, then write 1) write 0,tx_done will be restored to its default value,
	  if tx mask is turned on in advance, it will enter interrupt,in the interrupt, there is the action of clearing tx_done, but after the clear, immediately becomes 1,
	  out of the interrupt, and immediately in the interrupt, and so on loop, resulting in the feeling that the program did not go down.
	 */
	unsigned char tx_mask_flag=0;
	if(reg_uart_rx_timeout1(uart_num)|FLD_UART_MASK_TXDONE)
	{
		tx_mask_flag=1;
		reg_uart_rx_timeout1(uart_num)&=~FLD_UART_MASK_TXDONE;
	}
	reg_rst0 &= (~((uart_num)?FLD_RST0_UART1:FLD_RST0_UART0));
	reg_rst0 |= ((uart_num)?FLD_RST0_UART1:FLD_RST0_UART0);
	if(tx_mask_flag==1){
		reg_uart_rx_timeout1(uart_num)|=FLD_UART_MASK_TXDONE;
	}
}

/**
 * @brief     This function enable the clock of UART module.
 * @param[in] uart_num - UART0/UART1.
 * @return    none
 */
static inline void uart_clk_en(uart_num_e uart_num)
{
	reg_clk_en0 |= ((uart_num)?FLD_CLK0_UART1_EN:FLD_CLK0_UART0_EN);
}

/**
 * @brief      This function initializes the UART module.
 * @param[in]  uart_num    - UART0 or UART1.
 * @param[in]  div         - uart clock divider.
 * @param[in]  bwpc        - bitwidth, should be set to larger than 2.
 * @param[in]  parity      - selected parity type for UART interface.
 * @param[in]  stop_bit    - selected length of stop bit for UART interface.
 * @return     none
 * @note 	   sys_clk      baudrate   g_uart_div         g_bwpc
 *
 *  	       16Mhz        9600          118   			 13
 *                          19200         118     			  6
 *          	            115200          9       		 13
 *
 * 	           24Mhz        9600          249       		  9
 *           	 	    	19200		  124                 9
 *          	 	    	115200         12    			 15
 *
 *   	       32Mhz        9600          235       		 13
 *          	 	        19200		  235                 6
 *           	 	 	    115200         17    			 13
 *
 *   	       48Mhz        9600          499       		  9
 *          	 	 	    19200		  249                 9
 *           	 	 	    115200         25    			 15
*/
extern void uart_init(uart_num_e uart_num,unsigned short div, unsigned char bwpc, uart_parity_e parity, uart_stop_bit_e stop_bit);

/***********************************************************
 * @brief  		This function serves to calculate the best bwpc(bit width) .i.e reg0x96.
 * @param[in]	baudrate - baut rate of UART.
 * @param[in]	sysclk   - system clock.
 * @param[out]	div      - uart clock divider.
 * @param[out]	bwpc     - bitwidth, should be set to larger than 2.
 * @return 		none
 * @note        BaudRate*(div+1)*(bwpc+1) = system clock.
 *  		    simplify the expression: div*bwpc =  constant(z).
 * 		        bwpc range from 3 to 15.so loop and get the minimum one decimal point.
 */
void uart_cal_div_and_bwpc(unsigned int baudrate, unsigned int sysclk, unsigned short* div, unsigned char *bwpc);

/**
 * @brief  		This function serves to set r_rxtimeout. this setting is transfer one bytes need cycles base on uart_clk.
 * 				For example, if transfer one bytes (1start bit+8bits data+1 priority bit+2stop bits) total 12 bits,
 * 				this register setting should be (bpwc+1)*12.
 * @param[in]	uart_num - UART0 or UART1.
 * @param[in]	bwpc     - bitwidth, should be set to larger than 2.
 * @param[in]	bit_cnt  - bit number.
 * @param[in]	mul	     - mul.
 * @return 		none
 */
void uart_set_rx_timeout(uart_num_e uart_num,unsigned char bwpc, unsigned char bit_cnt, uart_timeout_mul_e mul);

/**
 * @brief     This function serves to config the number level setting the irq bit of status register.
 * @param[in] uart_num - UART0 or UART1.
 * @param[in] rx_level - receive level value. ie 0x140089[0,3].
 * @return    none
 */
static inline void uart_rx_irq_trig_level(uart_num_e uart_num,unsigned char rx_level)
{
	reg_uart_ctrl3(uart_num) = (reg_uart_ctrl3(uart_num) & (~FLD_UART_RX_IRQ_TRIQ_LEV)) | (rx_level & 0x0f);
}

/**
 * @brief     This function serves to config the number level setting the irq bit of status register.
 * @param[in] uart_num - UART0 or UART1.
 * @param[in] tx_level - transmit level value.ie 0x140089[4,7].
 * @return    none
 */
static inline void uart_tx_irq_trig_level(uart_num_e uart_num,unsigned char tx_level)
{
	reg_uart_ctrl3(uart_num) = (reg_uart_ctrl3(uart_num) & (~FLD_UART_TX_IRQ_TRIQ_LEV)) | (tx_level << 4);
}

/**
 * @brief     This function serves to send data by byte with not DMA method.
 * @param[in] uart_num - UART0 or UART1.
 * @param[in] tx_data  - the data to be send.
 * @return    none
 */
void uart_send_byte(uart_num_e uart_num,unsigned char tx_data);

/**
 * @brief     This function serves to receive uart data by byte with not DMA method.
 * @param[in] uart_num - UART0 or UART1.
 * @return    none
 */
unsigned char uart_read_byte(uart_num_e uart_num);
/**
 * @brief     This function serves to judge if the transmission of uart is done.
 * @param[in] uart_num - UART0 or UART1.
 * @return    return the tx status
 * -          0:tx is done     1:tx isn't done
 */
unsigned char uart_tx_is_busy(uart_num_e uart_num);
/**
 * @brief     This function serves to send uart data by halfword with not DMA method.
 * @param[in] uart_num - UART0 or UART1.
 * @param[in] data  - the data to be send.
 * @return    none
 */
void uart_send_hword(uart_num_e uart_num, unsigned short data);

/**
 * @brief     This function serves to send uart data by word with not DMA method.
 * @param[in] uart_num - UART0 or UART1.
 * @param[in] data - the data to be send.
 * @return    none
 */
void uart_send_word(uart_num_e uart_num, unsigned int data);


/**
 * @brief     This function sets the RTS pin's level manually.
 * @param[in] uart_num - UART0 or UART1.
 * @param[in] polarity - set the output of RTS pin(only for manual mode).
 * @return    none
 */
void uart_set_rts_level(uart_num_e uart_num, unsigned char polarity);

/**
 *	@brief		This function serves to set pin for UART cts function .
 *	@param[in]  cts_pin -To set cts pin.
 *	@return		none
 */
void uart_set_cts_pin(uart_cts_pin_e cts_pin);

/**
 *	@brief		This function serves to set pin for UART rts function .
 *	@param[in]  rts_pin - To set rts pin.
 *	@return		none
 */
void uart_set_rts_pin(uart_rts_pin_e rts_pin);

/**
* @brief      This function serves to select pin for UART module.
* @param[in]  tx_pin   - the pin to send data.
* @param[in]  rx_pin   - the pin to receive data.
* @return     none
*/
void uart_set_pin(uart_tx_pin_e tx_pin,uart_rx_pin_e rx_pin);

/**
* @brief      This function serves to set rx pin for UART module,
*             this pin can be used as either tx or rx. this pin is only used as tx when there is a sending action, but it is used as an rx at all times.
* @param[in]  rx_pin  - the rx pin need to set.
* @return     none
*/
void uart_set_rtx_pin(uart_rx_pin_e rx_pin);


/**
 * @brief     	This function serves to send data by DMA, this function tell the DMA to get data from the RAM and start.
 * @param[in]  	uart_num - UART0 or UART1.
 * @param[in] 	addr     - pointer to the buffer containing data need to send.
 * @param[in] 	len      - DMA transmission length.The maximum transmission length of DMA is 0xFFFFFC bytes, so dont'n over this length.
 * @return      1  dma start send.
 *              0  the length is error.
 */
unsigned char uart_send_dma(uart_num_e uart_num, unsigned char * addr, unsigned int len );

/**
* @brief     This function serves to send data with not DMA method.
* @param[in] uart_num - UART0 or UART1.
* @param[in] addr     - pointer to the buffer containing data need to send.
* @param[in] len      - NDMA transmission length.
* @return    1
*/
unsigned char uart_send(uart_num_e uart_num, unsigned char * addr, unsigned char len );

/**
 * @brief     	This function serves to receive data function by DMA, this  function tell the DMA to get data from the uart data fifo.
 * @param[in]  	uart_num - UART0 or UART1.
 * @param[in] 	addr     - pointer to the buffer  receive data.
 * @param[in]   rev_size - the receive length of DMA,The maximum transmission length of DMA is 0xFFFFFC bytes, so dont'n over this length.
 * @note        1. rev_size must be larger than the data you received actually.
 *              2. the data length can be arbitrary if less than rev_size.
 * @return    	none
 */
extern void uart_receive_dma(uart_num_e uart_num, unsigned char * addr,unsigned int rev_size);

/**
 * @brief     This function serves to get the length of the data that dma received.
 * @param[in] uart_num - UART0 or UART1.
 * @param[in] chn      - dma channel.
 * @return    data length.
 */
extern unsigned int uart_get_dma_rev_data_len(uart_num_e uart_num,dma_chn_e chn);

/**
  * @brief     This function serves to set uart tx_dma channel and config dma tx default.
  * @param[in] uart_num - UART0 or UART1.
  * @param[in] chn      - dma channel.
  * @return    none
  * @note      In the case that the DMA transfer is not completed(bit 0 of reg_dma_ctr0(chn): 1-the transmission has not been completed,0-the transmission is completed), re-calling the DMA-related functions may cause problems.
  *            If you must do this, you must perform the following sequence:
  *            1. dma_chn_dis(uart_dma_tx_chn[uart_num]) 2.uart_reset() 3.uart_send_dma()
  */
extern void uart_set_tx_dma_config(uart_num_e uart_num, dma_chn_e chn);

/**
 * @brief     This function serves to set uart rx_dma channel and config dma rx default.
 * @param[in] uart_num - UART0 or UART1.
 * @param[in] chn      - dma channel.
 * @return    none
 * @note      In the case that the DMA transfer is not completed(bit 0 of reg_dma_ctr0(chn): 1-the transmission has not been completed,0-the transmission is completed), re-calling the DMA-related functions may cause problems.
 *            If you must do this, you must perform the following sequence:
 *            1. dma_chn_dis(uart_dma_rx_chn[uart_num]) 2.uart_reset() 3.uart_receive_dma()
 */
extern void uart_set_rx_dma_config(uart_num_e uart_num, dma_chn_e chn);

/**
 * @brief     This function serves to config the irq of uart tx and rx.
 * @param[in] uart_num - UART0 or UART1.
 * @param[in] mask     - uart irq mask.
 * @return    none
 */
static inline void uart_set_irq_mask(uart_num_e uart_num,uart_irq_mask_e mask)
{
	if((mask & UART_RX_IRQ_MASK) || (mask & UART_TX_IRQ_MASK))
	{
		reg_uart_ctrl0(uart_num) |= (((mask & UART_RX_IRQ_MASK)? 1:0) << 6) | (((mask & UART_TX_IRQ_MASK)? 1:0 )<< 7);
	}

	if((mask & UART_RXDONE_MASK) || (mask & UART_TXDONE_MASK) || (mask & UART_ERR_IRQ_MASK))
	{
		reg_uart_rx_timeout1(uart_num) |= (((mask & UART_RXDONE_MASK)? 1:0) << 2) | (((mask & UART_TXDONE_MASK) ? 1:0 )<< 6) | (((mask & UART_ERR_IRQ_MASK) ? 1:0 )<< 7);
	}
}

/**
 * @brief     This function serves to clear the irq of uart tx and rx.
 * @param[in] uart_num - UART0 or UART1.
 * @param[in] mask     - uart irq mask.
 * @return    none
 */
static inline void uart_clr_irq_mask(uart_num_e uart_num,uart_irq_mask_e mask)
{
	if((mask & UART_RX_IRQ_MASK) || (mask & UART_TX_IRQ_MASK))
	{
		reg_uart_ctrl0(uart_num) &= ~((((mask & UART_RX_IRQ_MASK)? 1:0)<< 6) | (((mask & UART_TX_IRQ_MASK)? 1:0) << 7));
	}
	if((mask & UART_RXDONE_MASK) || (mask & UART_TXDONE_MASK) || (mask & UART_ERR_IRQ_MASK)){
		reg_uart_rx_timeout1(uart_num) &= ~((((mask & UART_RXDONE_MASK)? 1:0) << 2) | (((mask & UART_TXDONE_MASK)? 1:0)<< 6) | (((mask & UART_ERR_IRQ_MASK)? 1:0) << 7));
	}
}


/**
 * @brief     This function serves to get the irq status of uart tx and rx.
 * @param[in] uart_num - UART0 or UART1.
 * @param[in] status   - uart irq status.
 * @return    irq status
 */
static inline unsigned int  uart_get_irq_status(uart_num_e uart_num,uart_irq_status_get_e status)
{
	if(status == UART_RX_ERR){
		return (reg_uart_status1(uart_num) & (status));
	}
	else
	{
		return (reg_uart_status2(uart_num) & (status));
	}
}


/**
 * @brief     This function serves to clear the irq status of uart tx and rx.
 * @param[in] uart_num - UART0 or UART1.
 * @param[in] status - uart irq status.
 * @return    none
 */
static inline void uart_clr_irq_status(uart_num_e uart_num,uart_irq_status_clr_e status)
{
	reg_uart_status1(uart_num) |= (status);
}

/**
 * @brief     This function serves to set uart rts enable.
 * @param[in] uart_num - UART0 or UART1.
 * @return    none
 */
static inline void uart_set_rts_en(uart_num_e uart_num)
{
	reg_uart_ctrl2(uart_num) |= FLD_UART_RTS_EN; //enable RTS function
}

/**
 * @brief     This function serves to set uart rts disable.
 * @param[in] uart_num - UART0 or UART1.
 * @return    none
 */
static inline void uart_set_rts_dis(uart_num_e uart_num)
{
	reg_uart_ctrl2(uart_num) &= (~FLD_UART_RTS_EN); //disable RTS function
}

/**
 * @brief     This function serves to set uart cts enable.
 * @param[in] uart_num - UART0 or UART1.
 * @return    none
 */
static inline void uart_set_cts_en(uart_num_e uart_num)
{
	reg_uart_ctrl1(uart_num) |= FLD_UART_TX_CTS_ENABLE; //enable CTS function
}

/**
 * @brief     This function serves to set uart cts disable.
 * @param[in] uart_num - UART0 or UART1.
 * @return    none
 */
static inline void uart_set_cts_dis(uart_num_e uart_num)
{
	reg_uart_ctrl1(uart_num) &= (~FLD_UART_TX_CTS_ENABLE); //disable CTS function
}

/**
  * @brief     This function serves to configure UART hardware flow. Configure CTS.
  * @param[in] uart_num   - UART0 or UART1.
  * @param[in] cts_pin    - RTS pin select.
  * @param[in] cts_parity - when CTS's input equals to select, tx will be stopped.
  * @return    none
  */
extern void uart_cts_config(uart_num_e uart_num,uart_cts_pin_e cts_pin,unsigned char cts_parity);

/**
 * @brief     This function serves to configure UART hardware flow. Configure RTS.
 * @param[in] uart_num     - UART0 or UART1.
 * @param[in] rts_pin      - RTS pin select.
 * @param[in] rts_parity   - whether invert the output of RTS pin(only for auto mode).
 * @param[in] auto_mode_en - set the mode of RTS(auto or manual).
 * @return    none
 */
extern void uart_rts_config(uart_num_e uart_num,uart_rts_pin_e rts_pin,unsigned char rts_parity,unsigned char auto_mode_en);

/**
 * @brief     This function serves to set uart rts trig lexel in auto mode.
 * @param[in] uart_num - UART0 or UART1.
 * @param[in] level    - threshold of trig RTS pin's level toggle(only for auto mode).
 * @return    none
 */
static inline void uart_rts_trig_level_auto_mode(uart_num_e uart_num,unsigned char level)
{
    reg_uart_ctrl2(uart_num) &= (~FLD_UART_RTS_TRIQ_LEV);
    reg_uart_ctrl2(uart_num) |= (level & FLD_UART_RTS_TRIQ_LEV);
}

/**
 * @brief     This function serves to set uart rts auto mode.
 * @param[in] uart_num - UART0 or UART1.
 * @return    none
 */
static inline void uart_rts_auto_mode(uart_num_e uart_num)
{
	reg_uart_ctrl2(uart_num) &= (~FLD_UART_RTS_MANUAL_M);
}

/**
 * @brief     This function serves to set uart rts manual mode.
 * @param[in] uart_num - UART0 or UART1.
 * @return    none
 */
static inline void uart_rts_manual_mode(uart_num_e uart_num)
{
	reg_uart_ctrl2(uart_num) |= (FLD_UART_RTS_MANUAL_M);
}


/**
 * @brief     This function is used to set the 'uart_rx_byte_index' to 0.
 *			  after wakeup from power-saving mode or reset uart, you must call this function before receiving the data.
 * @param[in] uart_num
 * @return    none.
 */
static inline void uart_clr_rx_index(uart_num_e uart_num)
{
	uart_rx_byte_index[uart_num]=0;
}

/**
 * @brief     This function is used to set the 'uart_tx_byte_index' to 0.
 *			  after wakeup from power-saving mode or reset uart, you must call this function before sending the data.
 * @param[in] uart_num
 * @return    none.
 */
static inline void uart_clr_tx_index(uart_num_e uart_num)
{
	uart_tx_byte_index[uart_num]=0;
}

/**
 * @brief     This function is used to clr uart tx-done,which means set tx-done to 0
 * @param[in] uart_num
 * @return    none.
 */
static inline void uart_clr_tx_done(uart_num_e uart_num)
{
	reg_uart_state(uart_num) |=BIT(7);
}

/**
 * @brief      	This function is used to enable the rtx function of .
 * @param[in]  	chn - UART0 or UART1.
 * @return     	none.
 */
static inline void uart_rtx_en(uart_num_e chn)
{
	reg_uart_rx_timeout1(chn)|=FLD_UART_P7816_EN;
}

#endif	/* UART_H_ */
