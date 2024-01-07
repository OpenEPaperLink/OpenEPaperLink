/********************************************************************************************************
 * @file	uart_reg.h
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
#ifndef UART_REG_H
#define UART_REG_H
#include "soc.h"

/*******************************      uart0 registers: 0x80140080   *******************************/
/*******************************      uart1 registers: 0x801400c0      ******************************/
#define reg_uart_data_buf_adr(i)  (0x80140080+(i)*0x40)  //uart(i)

#define reg_uart_data_buf(i,j)    REG_ADDR8(reg_uart_data_buf_adr(i)+(j)) //uart(i)_buf(j)
#define reg_uart_data_hword_buf(i,j)  REG_ADDR16(reg_uart_data_buf_adr(i)+(j)*2)

#define reg_uart_data_word_buf(i)  REG_ADDR32(reg_uart_data_buf_adr(i)) //uart(i)

#define reg_uart_clk_div(i)		REG_ADDR16(0x140084+(i)*0x40)

enum{
	FLD_UART_CLK_DIV = 			BIT_RNG(0,14),
	FLD_UART_CLK_DIV_EN = 		BIT(15)
};

#define reg_uart_ctrl0(i)			REG_ADDR8(0x140086+(i)*0x40)

enum{
	FLD_UART_BPWC_O = 			BIT_RNG(0,3),
	FLD_UART_MASK_RX_IRQ = 		BIT(6),
	FLD_UART_MASK_TX_IRQ =		BIT(7)
};

#define reg_uart_ctrl1(i)         		REG_ADDR8(0x140087+(i)*0x40)

enum {
    FLD_UART_TX_CTS_POLARITY = 	BIT(0),
    FLD_UART_TX_CTS_ENABLE = 	BIT(1),
    FLD_UART_PARITY_ENABLE = 	BIT(2),
    FLD_UART_PARITY_POLARITY = 	BIT(3),       //1:odd parity   0:even parity
    FLD_UART_STOP_SEL = 		BIT_RNG(4,5),
    FLD_UART_TTL_ENABLE = 		BIT(6),
    FLD_UART_LOOPBACK_O = 		BIT(7),
};


#define reg_uart_ctrl2(i)			REG_ADDR16(0x140088+(i)*0x40)

enum {
    FLD_UART_RTS_TRIQ_LEV   	= BIT_RNG(0,3),
    FLD_UART_RTS_POLARITY 		= BIT(4),
    FLD_UART_RTS_MANUAL_V 	 	= BIT(5),
    FLD_UART_RTS_MANUAL_M 		= BIT(6),
    FLD_UART_RTS_EN 			= BIT(7),
};

#define reg_uart_ctrl3(i)        	REG_ADDR8(0x140089+(i)*0x40)

enum {
	FLD_UART_RX_IRQ_TRIQ_LEV 	= BIT_RNG(0,3),
	FLD_UART_TX_IRQ_TRIQ_LEV 	= BIT_RNG(4,7),
};
////////////////////////////////////////////////////
#define reg_uart_rx_timeout0(i)	REG_ADDR8(0x14008a+(i)*0x40)

enum{
	FLD_UART_TIMEOUT_BW		 = 	BIT_RNG(0,7),
};

#define reg_uart_rx_timeout1(i)  REG_ADDR8(0x14008b+0x40*(i))

enum{
	FLD_UART_TIMEOUT_MUL	 = 	BIT_RNG(0,1),
	FLD_UART_MARK_RXDONE	 =  BIT(2),
	//rsvd BIT(4)
	FLD_UART_P7816_EN	 	 =  BIT(5),
	FLD_UART_MASK_TXDONE	 =  BIT(6),
	FLD_UART_MASK_ERR_IRQ 	 =  BIT(7),
};



#define reg_uart_buf_cnt(i)       REG_ADDR8(0x14008c+(i)*0x40)
enum{
	FLD_UART_RX_BUF_CNT		=  BIT_RNG(0,3),
	FLD_UART_TX_BUF_CNT		=  BIT_RNG(4,7),
};

#define reg_uart_status1(i)			REG_ADDR8(0x14008d+((i)*0x40))
enum{
	FLD_UART_RBCNT 	     	=  BIT_RNG(0,2),
	FLD_UART_IRQ_O    		=  BIT(3),
	FLD_UART_WBCNT 	     	=  BIT_RNG(4,6), //R
	FLD_UART_CLEAR_RX 		=  BIT(6),		 //Write 1 clear RX
	FLD_UART_RX_ERR	 		=  BIT(7),		//R
	FLD_UART_CLEAR_TX	 	=  BIT(7),		//Write 1 clear TX
};


#define reg_uart_status2(i)       REG_ADDR8((0x14008e) +(0x40*(i)))
enum{
	FLD_UART_TX_DONE   	  =  BIT(0),//only for dma default 1.
	FLD_UART_TX_BUF_IRQ   =  BIT(1),
	FLD_UART_RX_DONE   	  =  BIT(2),
	FLD_UART_RX_BUF_IRQ   =  BIT(3),
};

//state machine use for IC debug
#define reg_uart_state(i)       REG_ADDR8(0x14008f+0x40*(i))
enum{
	FLD_UART_TSTATE_i   	=  BIT_RNG(0,2),//only for dma default 1.
	FLD_UART_RSTATE_i   	=  BIT_RNG(4,7),
};

/*******************************      7816 registers: 0x1401f0     ******************************/
#define reg_7816_clk_div		REG_ADDR8(0x1401f0)
enum{
	FLD_7816_CLK_DIV = 			BIT_RNG(4,7),
};
#endif
