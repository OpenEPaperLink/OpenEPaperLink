/********************************************************************************************************
 * @file    register.h
 *
 * @brief   This is the header file for B87
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

#pragma once

#include "bit.h"
#include "bsp.h"

/********************************************************************************************
 *****|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|*****
 *****|								Digital  Register Table  						   |*****
 *****|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|*****
 ********************************************************************************************/

/*******************************      i2c registers: 0x00      ******************************/
#define reg_i2c_set				REG_ADDR32(0x00)
#define reg_i2c_speed			REG_ADDR8(0x00)
#define reg_i2c_id				REG_ADDR8(0x01)
enum{
	FLD_I2C_WRITE_READ_BIT  =  BIT(0),
	FLD_I2C_ID              =  BIT_RNG(1,7),
};

#define reg_i2c_status			REG_ADDR8(0x02)
enum{
	FLD_I2C_CMD_BUSY		= 	BIT(0),
	FLD_I2C_BUS_BUSY		= 	BIT(1),
	FLD_I2C_NAK				= 	BIT(2),
};

#define reg_i2c_mode			REG_ADDR8(0x03)
enum{
	FLD_I2C_ADDR_AUTO_ADD 		=	BIT(0),
	FLD_I2C_MASTER_EN			= 	BIT(1),      // 1: master, 0: slave
	FLD_I2C_SLAVE_MAPPING 		=	BIT(2),      // write i2c data to predefined memory address which set by other register
	FLD_I2C_HOLD_MASTER     	=   BIT(3),
};

#define reg_i2c_adr_dat			REG_ADDR16(0x04)
#define reg_i2c_dat_ctrl		REG_ADDR32(0x04)
#define reg_i2c_di_ctrl			REG_ADDR16(0x06)
#define reg_i2c_adr				REG_ADDR8(0x04)
#define reg_i2c_do				REG_ADDR8(0x05)
#define reg_i2c_di				REG_ADDR8(0x06)
#define reg_i2c_ctrl			REG_ADDR8(0x07)
enum{
	FLD_I2C_CMD_ID			= 		BIT(0),
	FLD_I2C_CMD_ADDR		= 		BIT(1),
	FLD_I2C_CMD_DO			= 		BIT(2),
	FLD_I2C_CMD_DI			= 		BIT(3),
	FLD_I2C_CMD_START		= 		BIT(4),
	FLD_I2C_CMD_STOP		= 		BIT(5),
	FLD_I2C_CMD_READ_ID		= 		BIT(6),
	FLD_I2C_CMD_ACK			= 		BIT(7),
};
/*******************************  i2c address map registers: 0xe0      ******************************/
#define reg_i2c_map_hadr		 	 REG_ADDR8(0xe0)

#define reg_i2c_slave_map_addrl      REG_ADDR8(0xe1)
#define reg_i2c_slave_map_addrm      REG_ADDR8(0xe2)
#define reg_i2c_slave_map_addrh      REG_ADDR8(0xe3)

#define reg_i2c_slave_irq_status 	 REG_ADDR8(0xe4)
#define reg_spi_slave_irq_status 	 REG_ADDR8(0xe4)
#define reg_i2c_map_host_status      REG_ADDR8(0xe4)
enum{
	FLD_HOST_CMD_IRQ  = 	BIT(0),  FLD_SLAVE_SPI_IRQ = FLD_HOST_CMD_IRQ,  //both host write & read trigger this status
	FLD_HOST_READ_IRQ = 	BIT(1),                                        //only host read trigger this status
};

#define reg_i2c_map_read0		     REG_ADDR8(0xe5)
#define reg_i2c_map_read1		     REG_ADDR8(0xe6)
#define reg_i2c_map_read2		     REG_ADDR8(0xe7)

/*******************************      spi registers: 0x08      ******************************/

#define reg_spi_data			REG_ADDR8(0x08)
#define reg_spi_ctrl			REG_ADDR8(0x09)
enum{
	FLD_SPI_CS 					= BIT(0),
	FLD_SPI_MASTER_MODE_EN 		= BIT(1),
	FLD_SPI_DATA_OUT_DIS 		= BIT(2),
	FLD_SPI_RD			    	= BIT(3),
	FLD_SPI_ADDR_AUTO_ADD  	 	= BIT(4),
	FLD_SPI_SHARE_MODE      	= BIT(5),
	FLD_SPI_BUSY         		= BIT(6),    // diff from doc,  bit 6 working
};
#define reg_spi_sp				REG_ADDR8(0x0a)
enum{
	FLD_MSPI_CLK = 		BIT_RNG(0,6),
	FLD_SPI_ENABLE =			BIT(7),
};

#define reg_spi_inv_clk			REG_ADDR8(0x0b)   //spi supports four modes
enum {
    FLD_SPI_MODE_WORK_MODE = BIT_RNG(0,1),
};

/*******************************      mspi registers: 0x0c      ******************************/

#define reg_mspi_data		REG_ADDR8(0x0c)
#define reg_mspi_ctrl		REG_ADDR8(0x0d)

enum{
	FLD_MSPI_CS 	= 		BIT(0),
	FLD_MSPI_SDO 	= 		BIT(1),
	FLD_MSPI_CONT 	= 		BIT(2),
	FLD_MSPI_RD 	= 		BIT(3),
	FLD_MSPI_BUSY 	= 		BIT(4),
};

#define reg_mspi_mode      		REG_ADDR8(0x0f)
enum
{
	FLD_MSPI_DUAL_DATA_MODE_EN  = BIT(0),
	FLD_MSPI_DUAL_ADDR_MODE_EN  = BIT(1),
	FLD_MSPI_CLK_DIV     	 	= BIT_RNG(2,7),
};

/*******************************      reset registers: 0x60      ******************************/

#define reg_rst0				REG_ADDR8(0x60)
enum{
	FLD_RST0_SPI = 				BIT(0),
	FLD_RST0_I2C = 				BIT(1),
	FLD_RST0_UART = 			BIT(2),
	FLD_RST0_USB = 				BIT(3),
	FLD_RST0_PWM = 				BIT(4),
	FLD_RST0_QDEC =				BIT(5),
	FLD_RST0_SWIRE = 			BIT(7),
};

#define reg_rst1				REG_ADDR8(0x61)
enum{
	FLD_RST1_ZB = 				BIT(0),
	FLD_RST1_SYS_TIMER = 		BIT(1),
	FLD_RST1_DMA =				BIT(2),
	FLD_RST1_ALGM = 			BIT(3),
	FLD_RST1_AES = 				BIT(4),
	FLD_RST1_ADC = 				BIT(5),
	FLD_RST1_ALG =				BIT(6),
};

#define reg_rst2				REG_ADDR8(0x62)
enum{
	FLD_RST2_AIF =				BIT(0),
	FLD_RST2_AUD =				BIT(1),
	FLD_RST2_DFIFO =			BIT(2),
	FLD_RST2_RISC =				BIT(4),
	FLD_RST2_MCIC = 			BIT(5),
	FLD_RST2_RISC1 =			BIT(6),
	FLD_RST2_MCIC1 = 			BIT(7),
};


#define reg_clk_en0				REG_ADDR8(0x63)
enum{
	FLD_CLK0_SPI_EN = 			BIT(0),
	FLD_CLK0_I2C_EN = 			BIT(1),
	FLD_CLK0_UART_EN = 		BIT(2),
	FLD_CLK0_USB_EN = 			BIT(3),
	FLD_CLK0_PWM_EN = 			BIT(4),
	FLD_CLK0_QDEC_EN = 			BIT(5),
	FLD_CLK0_SWIRE_EN = 		BIT(7),
};

#define reg_clk_en1				REG_ADDR8(0x64)
enum{
	FLD_CLK1_ZB_EN = 			BIT(0),
	FLD_CLK1_SYS_TIMER_EN = 	BIT(1),
	FLD_CLK1_DMA_EN = 			BIT(2),
	FLD_CLK1_ALGM_EN = 			BIT(3),
	FLD_CLK1_AES_EN = 			BIT(4),

};


#define reg_clk_en2				REG_ADDR8(0x65)
enum{
	FLD_CLK2_AIF_EN = 			BIT(0),
	FLD_CLK2_AUD_EN = 			BIT(1),
	FLD_CLK2_DFIFO_EN = 		BIT(2),
	FLD_CLK2_MC_EN = 			BIT(4),
	FLD_CLK2_MCIC_EN = 			BIT(5),
};


#define reg_clk_sel				REG_ADDR8(0x66)
enum{
	FLD_SCLK_DIV = 				BIT_RNG(0,4),
	FLD_SCLK_SEL =				BIT_RNG(5,6),
	FLD_SCLK_HS_SEL =			BIT(7),
};

#define reg_i2s_step			REG_ADDR8(0x67)
enum{
	FLD_I2S_STEP = 				BIT_RNG(0,6),
	FLD_I2S_CLK_EN =			BIT(7),
};

#define reg_i2s_mod				REG_ADDR8(0x68)

#define reg_codec_clk_step			REG_ADDR8(0x6c)

#define reg_codec_clk_mod 			REG_ADDR8(0x6d)

#define reg_wakeup_en			REG_ADDR8(0x6e)
enum{
	FLD_WAKEUP_SRC_I2C = 		BIT(0),
	FLD_WAKEUP_SRC_SPI =		BIT(1),
	FLD_WAKEUP_SRC_USB =		BIT(2),
	FLD_WAKEUP_SRC_GPIO =		BIT(3),
	FLD_WAKEUP_SRC_I2C_SYN =	BIT(4),
	FLD_WAKEUP_SRC_GPIO_RM =	BIT(5),
	FLD_WAKEUP_SRC_USB_RESM =	BIT(6),
	FLD_WAKEUP_SRC_RST_SYS =	BIT(7),
};

#define reg_pwdn_ctrl			REG_ADDR8(0x6f)
enum
{
	FLD_PWDN_CTRL_REBOOT = BIT(5),
	FLD_PWDN_CTRL_SLEEP  = BIT(7),
};


#define reg_mcu_wakeup_mask		REG_ADDR32(0x78)

/*******************************      7816 registers: 0x7b      ******************************/

#define reg_7816_clk_div		REG_ADDR8(0x7b)

/*******************************      uart registers: 0x90      ******************************/

#define reg_uart_data_buf0		REG_ADDR8(0x90)
#define reg_uart_data_buf1		REG_ADDR8(0x91)
#define reg_uart_data_buf2		REG_ADDR8(0x92)
#define reg_uart_data_buf3		REG_ADDR8(0x93)

#define reg_uart_data_buf(i)    REG_ADDR8(0x90 + (i))  //i = 0~3
#define reg_uart_clk_div		REG_ADDR16(0x94)
enum{
	FLD_UART_CLK_DIV = 			BIT_RNG(0,14),
	FLD_UART_CLK_DIV_EN = 		BIT(15)
};

#define reg_uart_ctrl0			REG_ADDR8(0x96)
enum{
	FLD_UART_BPWC = 			BIT_RNG(0,3),
	FLD_UART_RX_DMA_EN = 		BIT(4),
	FLD_UART_TX_DMA_EN =		BIT(5),
	FLD_UART_RX_IRQ_EN = 		BIT(6),
	FLD_UART_TX_IRQ_EN =		BIT(7),
};

#define reg_uart_ctrl1         		REG_ADDR8(0x97)
enum {
    FLD_UART_CTRL1_CTS_SELECT	   = BIT(0),
    FLD_UART_CTRL1_CTS_EN 		   = BIT(1),
    FLD_UART_CTRL1_PARITY_EN 	   = BIT(2),
    FLD_UART_CTRL1_PARITY_POLARITY = BIT(3),       //1:odd parity   0:even parity
    FLD_UART_CTRL1_STOP_BIT 	   = BIT_RNG(4,5),
    FLD_UART_CTRL1_TTL 			   = BIT(6),
    FLD_UART_CTRL1_LOOPBACK 	   = BIT(7),
};

#define reg_uart_ctrl2			REG_ADDR16(0x98)
enum {
    FLD_UART_CTRL2_RTS_TRIG_LVL   	 = BIT_RNG(0,3),
    FLD_UART_CTRL2_RTS_PARITY 		 = BIT(4),
    FLD_UART_CTRL2_RTS_MANUAL_VAL 	 = BIT(5),
    FLD_UART_CTRL2_RTS_MANUAL_EN 	 = BIT(6),
    FLD_UART_CTRL2_RTS_EN 			 = BIT(7),
	FLD_UART_CTRL3_RX_IRQ_TRIG_LEVEL = BIT_RNG(8,11),
	FLD_UART_CTRL3_TX_IRQ_TRIG_LEVEL = BIT_RNG(12,15),
};


#define reg_uart_ctrl3        	REG_ADDR8(0x99)
enum {
	FLD_UART_RX_IRQ_TRIG_LEV = BIT_RNG(0,3),
	FLD_UART_TX_IRQ_TRIG_LEV = BIT_RNG(4,7),
};

#define reg_uart_rx_timeout0	REG_ADDR8(0x9a)
enum{
	FLD_UART_TIMEOUT_BW		 = 	BIT_RNG(0,7),
};

#define reg_uart_rx_timeout1    REG_ADDR8(0x9b)
enum{
	FLD_UART_TIMEOUT_MUL	 = 	BIT_RNG(0,1),
	FLD_UART_P7816_EN	 	 =  BIT(5),
	FLD_UART_MASK_TXDONE_IRQ =  BIT(6),
	FLD_UART_MASK_ERR_IRQ 	 =  BIT(7),
};


#define reg_uart_buf_cnt       REG_ADDR8(0x9c)

enum{
	FLD_UART_RX_BUF_CNT		=  BIT_RNG(0,3),
	FLD_UART_TX_BUF_CNT		=  BIT_RNG(4,7),
};

#define reg_uart_status0       REG_ADDR8(0x9d)
enum{
	FLD_UART_RBCNT 	     	=  BIT_RNG(0,2),
	FLD_UART_IRQ_FLAG    	=  BIT(3),
	FLD_UART_WBCNT 	     	=  BIT_RNG(4,6),
	FLD_UART_CLEAR_RX_FLAG 	=  BIT(6),
	FLD_UART_RX_ERR_FLAG 	=  BIT(7),
};

#define reg_uart_status1       REG_ADDR8(0x9e)
enum{
	FLD_UART_TX_DONE   	  =  BIT(0),
	FLD_UART_TX_BUF_IRQ   =  BIT(1),
	FLD_UART_RX_DONE   	  =  BIT(2),
	FLD_UART_RX_BUF_IRQ   =  BIT(3),
};


#define reg_uart_state       REG_ADDR8(0x9f)
enum{
	FLD_UART_TSTATE_I 	     =  BIT_RNG(0,2),
	FLD_UART_RSTATE_I	     =  BIT_RNG(4,7),
};


/*******************************      swire registers: 0xb0      ******************************/

#define reg_swire_data			REG_ADDR8(0xb0)
#define reg_swire_ctrl1			REG_ADDR8(0xb1)
enum{
	FLD_SWIRE_WR  = 			BIT(0),
	FLD_SWIRE_RD  = 			BIT(1),
	FLD_SWIRE_CMD =				BIT(2),
	FLD_SWIRE_USB_DET =			BIT(6),
	FLD_SWIRE_USB_EN =			BIT(7),
};

#define reg_swire_clk_div		REG_ADDR8(0xb2)

enum
{
	FLD_SWIRE_CLK_DIV = BIT_RNG(0,6),
};

#define reg_swire_id      		REG_ADDR8(0xb3)

enum
{
	FLD_SWIRE_ID_SLAVE_ID      = BIT_RNG(0,6),
	FLD_SWIRE_ID_SLAVE_FIFO_EN = BIT(7),
};

/*******************************      analog control registers: 0xb8      ******************************/

#define reg_ana_ctrl32			REG_ADDR32(0xb8)
#define reg_ana_addr_data		REG_ADDR16(0xb8)
#define reg_ana_addr			REG_ADDR8(0xb8)
#define reg_ana_data			REG_ADDR8(0xb9)
#define reg_ana_ctrl			REG_ADDR8(0xba)

enum{
	FLD_ANA_BUSY  = 			BIT(0),
	FLD_ANA_RSV	=				BIT(4),
	FLD_ANA_RW  = 				BIT(5),
	FLD_ANA_START  = 			BIT(6),
	FLD_ANA_CYC  = 				BIT(7),
};


/*******************************      usb registers: 0x100      ******************************/

#define reg_ctrl_ep_ptr			REG_ADDR8(0x100)
#define reg_ctrl_ep_dat			REG_ADDR8(0x101)
#define reg_ctrl_ep_ctrl		REG_ADDR8(0x102)

enum{
	FLD_EP_DAT_ACK  = 			BIT(0),
	FLD_EP_DAT_STALL =			BIT(1),
	FLD_EP_STA_ACK = 			BIT(2),
	FLD_EP_STA_STALL = 			BIT(3),
};

#define reg_ctrl_ep_irq_sta		REG_ADDR8(0x103)
enum{
	FLD_CTRL_EP_IRQ_TRANS  = 	BIT_RNG(0,3),
	FLD_CTRL_EP_IRQ_SETUP  =	BIT(4),
	FLD_CTRL_EP_IRQ_DATA  =		BIT(5),
	FLD_CTRL_EP_IRQ_STA  = 		BIT(6),
	FLD_CTRL_EP_IRQ_INTF  = 	BIT(7),
};

#define reg_ctrl_ep_irq_mode	REG_ADDR8(0x104)
enum{
	FLD_CTRL_EP_AUTO_ADDR = 	BIT(0),
	FLD_CTRL_EP_AUTO_CFG =		BIT(1),
	FLD_CTRL_EP_AUTO_INTF =		BIT(2),
	FLD_CTRL_EP_AUTO_STA =		BIT(3),
	FLD_CTRL_EP_AUTO_SYN =		BIT(4),
	FLD_CTRL_EP_AUTO_DESC =		BIT(5),
	FLD_CTRL_EP_AUTO_FEAT =		BIT(6),
	FLD_CTRL_EP_AUTO_STD =		BIT(7),
};

#define reg_usb_ctrl			REG_ADDR8(0x105)
enum{
	FLD_USB_CTRL_AUTO_CLK = 	BIT(0),
	FLD_USB_CTRL_LOW_SPD = 		BIT(1),
	FLD_USB_CTRL_LOW_JITT =		BIT(2),
	FLD_USB_CTRL_TST_MODE = 	BIT(3),
};

#define reg_usb_cyc_cali		REG_ADDR16(0x106)
#define reg_usb_mdev			REG_ADDR8(0x10a)
#define reg_usb_host_conn		REG_ADDR8(0x10b)
enum{
	FLD_USB_MDEV_SELF_PWR = 	BIT(0),
	FLD_USB_MDEV_SUSP_STA = 	BIT(1),
};

#define reg_usb_sups_cyc_cali	REG_ADDR8(0x10c)
#define reg_usb_intf_alt		REG_ADDR8(0x10d)
#define reg_usb_ep_en			REG_ADDR8(0x10e)
typedef enum{
	FLD_USB_EDP8_EN 		= 	BIT(0),	// printer
	FLD_USB_EDP1_EN 		= 	BIT(1),	// keyboard
	FLD_USB_EDP2_EN 		= 	BIT(2),	// mouse
	FLD_USB_EDP3_EN 		= 	BIT(3),
	FLD_USB_EDP4_EN 		= 	BIT(4),
	FLD_USB_EDP5_EN 		= 	BIT(5),	// printer
	FLD_USB_EDP6_EN 		= 	BIT(6),	// audio
	FLD_USB_EDP7_EN 		= 	BIT(7),	// audio
}usb_ep_en_e;
#define reg_usb_ep8123_ptr		REG_ADDR32(0x110)
#define reg_usb_ep8_ptr			REG_ADDR8(0x110)
#define reg_usb_ep1_ptr			REG_ADDR8(0x111)
#define reg_usb_ep2_ptr			REG_ADDR8(0x112)
#define reg_usb_ep3_ptr			REG_ADDR8(0x113)
#define reg_usb_ep4567_ptr		REG_ADDR32(0x114)
#define reg_usb_ep4_ptr			REG_ADDR8(0x114)
#define reg_usb_ep5_ptr			REG_ADDR8(0x115)
#define reg_usb_ep6_ptr			REG_ADDR8(0x116)
#define reg_usb_ep7_ptr			REG_ADDR8(0x117)
#define reg_usb_ep_ptr(i)		REG_ADDR8(0x110+((i) & 0x07))

#define reg_usb_ep8123_dat		REG_ADDR32(0x118)
#define reg_usb_ep8_dat			REG_ADDR8(0x118)
#define reg_usb_ep1_dat			REG_ADDR8(0x119)
#define reg_usb_ep2_dat			REG_ADDR8(0x11a)
#define reg_usb_ep3_dat			REG_ADDR8(0x11b)
#define reg_usb_ep4567_dat		REG_ADDR32(0x11c)
#define reg_usb_ep4_dat			REG_ADDR8(0x11c)
#define reg_usb_ep5_dat			REG_ADDR8(0x11d)
#define reg_usb_ep6_dat			REG_ADDR8(0x11e)
#define reg_usb_ep7_dat			REG_ADDR8(0x11f)
#define reg_usb_ep_dat(i)		REG_ADDR8(0x118+((i) & 0x07))
#define reg_usb_mic_dat0		REG_ADDR16(0x1800)
#define reg_usb_mic_dat1		REG_ADDR16(0x1802)

#define reg_usb_ep8_ctrl		REG_ADDR8(0x120)
#define reg_usb_ep1_ctrl		REG_ADDR8(0x121)
#define reg_usb_ep2_ctrl		REG_ADDR8(0x122)
#define reg_usb_ep3_ctrl		REG_ADDR8(0x123)
#define reg_usb_ep4_ctrl		REG_ADDR8(0x124)
#define reg_usb_ep5_ctrl		REG_ADDR8(0x125)
#define reg_usb_ep6_ctrl		REG_ADDR8(0x126)
#define reg_usb_ep7_ctrl		REG_ADDR8(0x127)
#define reg_usb_ep_ctrl(i)		REG_ADDR8(0x120+((i) & 0x07))

enum{
	FLD_USB_EP_BUSY = 			BIT(0),
	FLD_USB_EP_STALL =			BIT(1),
	FLD_USB_EP_DAT0 =			BIT(2),
	FLD_USB_EP_DAT1 =			BIT(3),
	FLD_USB_EP_MONO =			BIT(6),
	FLD_USB_EP_EOF_ISO =		BIT(7),
};

#define reg_usb_ep8123_buf_addr	REG_ADDR32(0x128)
#define reg_usb_ep8_buf_addr	REG_ADDR8(0x128)
#define reg_usb_ep1_buf_addr	REG_ADDR8(0x129)
#define reg_usb_ep2_buf_addr	REG_ADDR8(0x12a)
#define reg_usb_ep3_buf_addr	REG_ADDR8(0x12b)
#define reg_usb_ep4567_buf_addr	REG_ADDR32(0x12c)
#define reg_usb_ep4_buf_addr	REG_ADDR8(0x12c)
#define reg_usb_ep5_buf_addr	REG_ADDR8(0x12d)
#define reg_usb_ep6_buf_addr	REG_ADDR8(0x12e)
#define reg_usb_ep7_buf_addr	REG_ADDR8(0x12f)
#define reg_usb_ep_buf_addr(i)	REG_ADDR8(0x128+((i) & 0x07))

#define reg_usb_ram_ctrl		REG_ADDR8(0x130)
enum{
	FLD_USB_CEN_PWR_DN =		BIT(0),
	FLD_USB_CLK_PWR_DN =		BIT(1),
	FLD_USB_WEN_PWR_DN =		BIT(3),
	FLD_USB_CEN_FUNC =			BIT(4),
};

#define reg_usb_iso_mode		REG_ADDR8(0x138)
#define reg_usb_irq				REG_ADDR8(0x139)
#define reg_usb_mask			REG_ADDR8(0x13a)
enum{
	FLD_USB_EDP8_IRQ 		= 	BIT(0),
	FLD_USB_EDP1_IRQ 		= 	BIT(1),
	FLD_USB_EDP2_IRQ 		= 	BIT(2),
	FLD_USB_EDP3_IRQ 		= 	BIT(3),
	FLD_USB_EDP4_IRQ 		= 	BIT(4),
	FLD_USB_EDP5_IRQ 		= 	BIT(5),
	FLD_USB_EDP6_IRQ 		= 	BIT(6),
	FLD_USB_EDP7_IRQ 		= 	BIT(7),
};
#define reg_usb_ep8_send_max	REG_ADDR8(0x13b)
#define reg_usb_ep8_send_thre	REG_ADDR8(0x13c)
#define reg_usb_ep8_fifo_mode	REG_ADDR8(0x13d)
#define reg_usb_ep_max_size		REG_ADDR8(0x13e)

enum{
	FLD_USB_ENP8_FIFO_MODE =	BIT(0),
	FLD_USB_ENP8_FULL_FLAG =	BIT(1),
};

#define reg_rf_acc_len			REG_ADDR8(0x405)
enum{
	FLD_RF_ACC_LEN	    = 		BIT_RNG(0,2),
	FLD_RF_LR_MAN_EN	=       BIT(3),   //long range manual enable
	FLD_RF_LR_TX_SEL	=   	BIT(4),
	FLD_RF_BLE_LR		=   	BIT(5),
	FLD_RF_LR_ACC_TRIG	=   	BIT(7),
};


#define reg_rf_timestamp		REG_ADDR32(0x450)


/*******************************      aes registers: 0x540      ******************************/

#define reg_aes_ctrl            REG_ADDR8(0x540)

enum {
    FLD_AES_CTRL_CODEC_TRIG = BIT(0),
    FLD_AES_CTRL_DATA_FEED = BIT(1),
    FLD_AES_CTRL_CODEC_FINISHED = BIT(2),
};

#define reg_aes_data            REG_ADDR32(0x548)
#define reg_aes_key(v)     		REG_ADDR8(0x550+v)


/*******************************      audio registers: 0x560      ******************************/

#define reg_audio_ctrl     REG_ADDR8(0x560)
enum{
	AUDIO_OUTPUT_OFF		   = 0,
	FLD_AUDIO_MONO_MODE        = BIT(0),
	FLD_AUDIO_I2S_PLAYER_EN    = BIT(1),
	FLD_AUDIO_SDM_PLAYER_EN    = BIT(2),
	FLD_AUDIO_ISO_PLAYER_EN	   = BIT(3),
	FLD_AUDIO_I2S_RECORDER_EN  = BIT(4),
	FLD_AUDIO_I2S_INTERFACE_EN = BIT(5),
	FLD_AUDIO_GRP_EN           = BIT(6),
	FLD_AUDIO_HPF_EN           = BIT(7),
};

#define	reg_pwm_ctrl	  REG_ADDR8(0x563)
enum{
	FLD_PWM_MULTIPLY2			= BIT(0),
	FLD_PWM_ENABLE				= BIT(1),
	FLD_LINER_INTERPOLATE_EN	= BIT(2),
	FLD_LEFT_SHAPING_EN 		= BIT(5),
	FLD_RIGTH_SHAPING_EN 		= BIT(6)
};

#define reg_ascl_tune     REG_ADDR32(0x564)

#define reg_pn1_left      REG_ADDR8(0x568)
enum{
	PN1_LEFT_CHN_BITS 	= BIT_RNG(0,4),
	PN2_LEFT_CHN_EN	  	= BIT(5),
	PN1_LEFT_CHN_EN   	= BIT(6),
};

#define reg_pn2_left      REG_ADDR8(0x569)
enum{
	PN2_LEFT_CHN_BITS 	= BIT_RNG(0,4),
	PN2_RIGHT_CHN_EN	= BIT(5),
	PN1_RIGHT_CHN_EN	= BIT(6),
};

#define reg_pn1_right      REG_ADDR8(0x56a)
enum{
	PN1_RIGHT_CHN_BITS 	= BIT_RNG(0,4),
	CLK2A_AUDIO_CLK_EN	= BIT(5),
	EXCHANGE_SDM_DATA_EN= BIT(6),
};

#define reg_pn2_right      REG_ADDR8(0x56b)
enum{
	PN2_RIGHT_CHN_BITS 		= BIT_RNG(0,4),
	SDM_LEFT_CHN_CONST_EN	= BIT(5),
	SDM_RIGHT_CHN_CONST_EN	= BIT(6),
};

/*******************************      gpio registers: 0x580      ******************************/

#define reg_gpio_pa_in			REG_ADDR8(0x580)
#define reg_gpio_pa_ie			REG_ADDR8(0x581)
#define reg_gpio_pa_oen			REG_ADDR8(0x582)
#define reg_gpio_pa_out			REG_ADDR8(0x583)
#define reg_gpio_pa_pol			REG_ADDR8(0x584)
#define reg_gpio_pa_ds			REG_ADDR8(0x585)
#define reg_gpio_pa_gpio		REG_ADDR8(0x586)
#define reg_gpio_pa_irq_en		REG_ADDR8(0x587)

#define reg_gpio_pb_in			REG_ADDR8(0x588)
#define reg_gpio_pb_ie			REG_ADDR8(0x589)
#define reg_gpio_pb_oen			REG_ADDR8(0x58a)
#define reg_gpio_pb_out			REG_ADDR8(0x58b)
#define reg_gpio_pb_pol			REG_ADDR8(0x58c)
#define reg_gpio_pb_ds			REG_ADDR8(0x58d)
#define reg_gpio_pb_gpio		REG_ADDR8(0x58e)
#define reg_gpio_pb_irq_en		REG_ADDR8(0x58f)

#define reg_gpio_pc_in			REG_ADDR8(0x590)
#define areg_gpio_pc_ie			0xc0
#define reg_gpio_pc_oen			REG_ADDR8(0x592)
#define reg_gpio_pc_out			REG_ADDR8(0x593)
#define reg_gpio_pc_pol			REG_ADDR8(0x594)
#define areg_gpio_pc_ds			0xc2
#define reg_gpio_pc_gpio		REG_ADDR8(0x596)
#define reg_gpio_pc_irq_en		REG_ADDR8(0x597)

#define reg_gpio_pd_in			REG_ADDR8(0x598)
#define reg_gpio_pd_ie			REG_ADDR8(0x599)
#define reg_gpio_pd_oen			REG_ADDR8(0x59a)
#define reg_gpio_pd_out			REG_ADDR8(0x59b)
#define reg_gpio_pd_pol			REG_ADDR8(0x59c)
#define reg_gpio_pd_ds			REG_ADDR8(0x59d)
#define reg_gpio_pd_gpio		REG_ADDR8(0x59e)
#define reg_gpio_pd_irq_en		REG_ADDR8(0x59f)

#define reg_gpio_pe_in			REG_ADDR8(0x5a0)
#define reg_gpio_pe_ie			REG_ADDR8(0x5a1)
#define reg_gpio_pe_oen			REG_ADDR8(0x5a2)
#define reg_gpio_pe_out			REG_ADDR8(0x5a3)
#define reg_gpio_pe_pol			REG_ADDR8(0x5a4)
#define reg_gpio_pe_ds			REG_ADDR8(0x5a5)
#define reg_gpio_pe_gpio		REG_ADDR8(0x5a6)
#define reg_gpio_pe_irq_en		REG_ADDR8(0x5a7)

#define reg_gpio_pa_setting1	REG_ADDR32(0x580)
#define reg_gpio_pa_setting2	REG_ADDR32(0x584)
#define reg_gpio_pb_setting1	REG_ADDR32(0x588)
#define reg_gpio_pb_setting2	REG_ADDR32(0x58c)
#define reg_gpio_pd_setting1	REG_ADDR32(0x598)
#define reg_gpio_pd_setting2	REG_ADDR32(0x59c)

#define reg_gpio_in(i)				REG_ADDR8(0x580+((i>>8)<<3))
#define reg_gpio_ie(i)				REG_ADDR8(0x581+((i>>8)<<3))
#define reg_gpio_oen(i)				REG_ADDR8(0x582+((i>>8)<<3))
#define reg_gpio_out(i)				REG_ADDR8(0x583+((i>>8)<<3))
#define reg_gpio_pol(i)				REG_ADDR8(0x584+((i>>8)<<3))
#define reg_gpio_ds(i)				REG_ADDR8(0x585+((i>>8)<<3))


#define reg_gpio_func(i)			REG_ADDR8(0x586+((i>>8)<<3))

#define reg_gpio_irq_wakeup_en(i)	REG_ADDR8(0x587+((i>>8)<<3))  // reg_irq_mask: FLD_IRQ_GPIO_EN

#define reg_gpio_irq_risc0_en(i)  	REG_ADDR8(0x5b8 + (i >> 8))	  // reg_irq_mask: FLD_IRQ_GPIO_RISC0_EN
#define reg_gpio_irq_risc1_en(i)  	REG_ADDR8(0x5c0 + (i >> 8))	  // reg_irq_mask: FLD_IRQ_GPIO_RISC1_EN
#define reg_gpio_irq_risc2_en(i)  	REG_ADDR8(0x5c8 + (i >> 8))

#define reg_mux_func_a1      		REG_ADDR8(0x5a8)
#define reg_mux_func_a2      		REG_ADDR8(0x5a9)
#define reg_mux_func_b1     		REG_ADDR8(0x5aa)
#define reg_mux_func_b2     		REG_ADDR8(0x5ab)
#define reg_mux_func_c1     		REG_ADDR8(0x5ac)
#define reg_mux_func_c2     		REG_ADDR8(0x5ad)
#define reg_mux_func_d1     		REG_ADDR8(0x5ae)
#define reg_mux_func_d2     		REG_ADDR8(0x5af)

#define reg_gpio_wakeup_irq  		REG_ADDR8(0x5b5)
enum{
    FLD_GPIO_CORE_WAKEUP_EN  	= BIT(2),
    FLD_GPIO_CORE_INTERRUPT_EN 	= BIT(3),
};

#define reg_pin_i2c_spi_out_en	    REG_ADDR8(0x5b6)
enum{
	FLD_PIN_PAGROUP_SPI_EN =	BIT_RNG(4,5),
	FLD_PIN_PBGROUP_SPI_EN =	BIT(6),
	FLD_PIN_PDGROUP_SPI_EN =	BIT(7),
};


#define reg_pin_i2c_spi_en			REG_ADDR8(0x5b7)  //poweron default  0xff
enum{
	FLD_PIN_PA3_SPI_EN =	BIT(0),
	FLD_PIN_PA4_SPI_EN =	BIT(1),
	FLD_PIN_PB6_SPI_EN =	BIT(2),
	FLD_PIN_PD7_SPI_EN =	BIT(3),
	FLD_PIN_PA3_I2C_EN =	BIT(4),
	FLD_PIN_PA4_I2C_EN =	BIT(5),
	FLD_PIN_PB6_I2C_EN =	BIT(6),
	FLD_PIN_PD7_I2C_EN =	BIT(7),
};


/*******************************      timer registers: 0x620      ******************************/

#define reg_tmr_ctrl			REG_ADDR32(0x620)
#define reg_tmr_ctrl16			REG_ADDR16(0x620)
#define reg_tmr_ctrl8			REG_ADDR8(0x620)
enum{
	FLD_TMR0_EN =				BIT(0),
	FLD_TMR0_MODE =				BIT_RNG(1,2),
	FLD_TMR1_EN = 				BIT(3),
	FLD_TMR1_MODE =				BIT_RNG(4,5),
	FLD_TMR2_EN =				BIT(6),
	FLD_TMR2_MODE = 			BIT_RNG(7,8),
	FLD_TMR_WD_CAPT = 			BIT_RNG(9,22),
	FLD_TMR_WD_EN =				BIT(23),
	FLD_TMR0_STA =				BIT(24),
	FLD_TMR1_STA =				BIT(25),
	FLD_TMR2_STA =				BIT(26),
	FLD_CLR_WD =				BIT(27),
};

#define reg_tmr_sta				REG_ADDR8(0x623)
enum{
	FLD_TMR_STA_TMR0 =			BIT(0),
	FLD_TMR_STA_TMR1 =			BIT(1),
	FLD_TMR_STA_TMR2 =			BIT(2),
	FLD_TMR_STA_WD =			BIT(3),
};

#define reg_tmr0_capt			REG_ADDR32(0x624)
#define reg_tmr1_capt			REG_ADDR32(0x628)
#define reg_tmr2_capt			REG_ADDR32(0x62c)
#define reg_tmr_capt(i)			REG_ADDR32(0x624 + ((i) << 2))
#define reg_tmr0_tick			REG_ADDR32(0x630)
#define reg_tmr1_tick			REG_ADDR32(0x634)
#define reg_tmr2_tick			REG_ADDR32(0x638)
#define reg_tmr_tick(i)			REG_ADDR32(0x630 + ((i) << 2))

/*******************************      irq registers: 0x640      ******************************/

#define reg_irq_mask			REG_ADDR32(0x640)
#define reg_irq_pri				REG_ADDR32(0x644)
#define reg_irq_src				REG_ADDR32(0x648)
#define reg_irq_src3			REG_ADDR8(0x64a)
enum{
	FLD_IRQ_TMR0_EN =			BIT(0),
	FLD_IRQ_TMR1_EN =			BIT(1),
	FLD_IRQ_TMR2_EN =			BIT(2),
	FLD_IRQ_USB_PWDN_EN =		BIT(3),
	FLD_IRQ_DMA_EN =			BIT(4),
	FLD_IRQ_DAM_FIFO_EN =		BIT(5),
	FLD_IRQ_UART_EN 	=		BIT(6),
	FLD_IRQ_MIX_CMD_EN =		BIT(7), FLD_IRQ_HOST_CMD_EN  =	BIT(7),//< MIX = I2C/QDEC/SPI

	FLD_IRQ_EP0_SETUP_EN =		BIT(8),
	FLD_IRQ_EP0_DAT_EN =		BIT(9),
	FLD_IRQ_EP0_STA_EN =		BIT(10),
	FLD_IRQ_SET_INTF_EN =		BIT(11),
	FLD_IRQ_EP_DATA_EN =		BIT(12),  FLD_IRQ_IRQ4_EN = BIT(12),
	FLD_IRQ_ZB_RT_EN =			BIT(13),
	FLD_IRQ_SW_PWM_EN =			BIT(14),  //irq_software | irq_pwm
	FLD_IRQ_PKE_EN =			BIT(15),//	RSVD 		=			BIT(15),

	FLD_IRQ_USB_250US_EN =		BIT(16),
	FLD_IRQ_USB_RST_EN =		BIT(17),
	FLD_IRQ_GPIO_EN =			BIT(18),
	FLD_IRQ_PM_EN =				BIT(19),
	FLD_IRQ_SYSTEM_TIMER =		BIT(20),
	FLD_IRQ_GPIO_RISC0_EN =		BIT(21),
	FLD_IRQ_GPIO_RISC1_EN =		BIT(22),
	//	RSVD		  	  = 	BIT(23),

	FLD_IRQ_EN =				BIT_RNG(24,31),
	FLD_IRQ_ALL           =     0XFFFFFFFF,
};
#define reg_irq_en				REG_ADDR8(0x643)




/*******************************      system timer registers: 0x740      ******************************/

#define reg_system_tick				REG_ADDR32(0x740)
#define reg_system_tick_irq_level	REG_ADDR32(0x744)
#define reg_system_irq_mask			REG_ADDR8(0x748)
#define reg_system_cal_irq			REG_ADDR8(0x749)
#define reg_system_ctrl				REG_ADDR8(0x74a)
enum{
	FLD_SYSTEM_32K_CAL_MODE 	= 	BIT_RNG(4,7),
	FLD_SYSTEM_32K_CAL_EN 		= 	BIT(3),
	FLD_SYSTEM_TIMER_AUTO_EN 	= 	BIT(2),
	FLD_SYSTEM_TIMER_MANUAL_EN 	= 	BIT(1),
	FLD_SYSTEM_32K_WR_EN 		= 	BIT(0),
};

#define reg_system_status			REG_ADDR8(0x74b)
enum{
	FLD_SYSTEM_32K_TIMER_BUSY 			=   BIT(6),
	FLD_SYSTEM_32K_TIMER_UPDATE_RD 		=   BIT(5),			FLD_SYSTEM_32K_TIMER_CLEAR_RD		=   BIT(5),
	FLD_SYSTEM_STATE_MACHINE_STATUS		=   BIT_RNG(3,4),  	FLD_SYSTEM_32K_TIMER_SYCN_TRIG		=   BIT(3),
	FLD_SYSTEM_CMD_SET_TRIG 			=   BIT(2),
	FLD_SYSTEM_CMD_SYCN_TRIG 			=   BIT(1),
	FLD_SYSTEM_TIMER_STATUS 			=   BIT(0),
};

#define reg_system_32k_tick_wt  	REG_ADDR32(0x74c)
#define reg_system_32k_tick_rd  	REG_ADDR32(0x750)
#define reg_system_32k_tick_cal  	REG_ADDR32(0x754)

/*******************************      pwm registers: 0x780      ******************************/

#define reg_pwm_enable			REG_ADDR8(0x780)
#define reg_pwm0_enable			REG_ADDR8(0x781)
#define reg_pwm_clk				REG_ADDR8(0x782)

#define reg_pwm0_mode			REG_ADDR8(0x783)


#define reg_pwm_invert			REG_ADDR8(0x784)
#define reg_pwm_n_invert		REG_ADDR8(0x785)
#define reg_pwm_pol				REG_ADDR8(0x786)

#define reg_pwm_cycle(i)		REG_ADDR32(0x794 + (i << 2))   //<15:0>: TCMP 0~5  <31:16>: TMAX 0~5
#define reg_pwm_cmp(i)			REG_ADDR16(0x794 + (i << 2))   //TCMP 0~5
#define reg_pwm_max(i)			REG_ADDR16(0x796 + (i << 2))   //TMAX 0~5
enum{
	FLD_PWM_CMP  = 				BIT_RNG(0,15),
	FLD_PWM_MAX  = 				BIT_RNG(16,31),
};

#define reg_pwm0_pulse_num		REG_ADDR16(0x7ac)

#define reg_pwm_irq_mask		REG_ADDR8(0x7b0)
#define reg_pwm_irq_sta			REG_ADDR8(0x7b1)
enum{
	FLD_IRQ_PWM0_PNUM =					BIT(0),
	FLD_IRQ_PWM0_IR_DMA_FIFO_DONE  =	BIT(1),
	FLD_IRQ_PWM0_FRAME =				BIT(2),
	FLD_IRQ_PWM1_FRAME =				BIT(3),
	FLD_IRQ_PWM2_FRAME =				BIT(4),
	FLD_IRQ_PWM3_FRAME =				BIT(5),
	FLD_IRQ_PWM4_FRAME 	=				BIT(6),
	FLD_IRQ_PWM5_FRAME =				BIT(7),
};



#define reg_pwm0_fifo_mode_irq_mask		REG_ADDR8(0x7b2)

enum{
	FLD_PWM0_IRQ_IR_FIFO_EN  	 = BIT(0),
};

#define reg_pwm0_fifo_mode_irq_sta		REG_ADDR8(0x7b3)

enum{
	FLD_PWM0_IRQ_IR_FIFO_CNT 	 = BIT(0),
};



#define reg_pwm_tcmp0_shadow		REG_ADDR16(0x7c4)   //<15:0>: TCMP 0~5  <31:16>: TMAX 0~5
#define reg_pwm_tmax0_shadow		REG_ADDR16(0x7c6)   //TCMP 0~5

#define reg_pwm_ir_fifo_dat(i)			REG_ADDR16(0x7c8+i*2)
#define reg_pwm_ir_fifo_irq_trig_level	REG_ADDR8(0x7cc)

#define reg_pwm_ir_fifo_data_status		REG_ADDR8(0x7cd)
enum{
	FLD_PWM0_IR_FIFO_DATA_NUM 	=		BIT_RNG(0,3),
	FLD_PWM0_IR_FIFO_EMPTY 		=		BIT(4),
	FLD_PWM0_IR_FIFO_FULL 		=		BIT(5),
};

#define reg_pwm_ir_clr_fifo_data		REG_ADDR8(0x7ce)

enum{
	FLD_PWM0_IR_FIFO_CLR_DATA 	=		BIT(0),
};

/*******************************      fifo registers: 0x800      ******************************/

#define reg_fifo0_data			REG_ADDR32(0x800)
#define reg_fifo1_data			REG_ADDR32(0x900)
#define reg_fifo2_data			REG_ADDR32(0xa00)

/*******************************      dfifo registers: 0xb00      ******************************/

#define reg_dfifo0_addr			REG_ADDR16(0xb00)
#define reg_dfifo0_size			REG_ADDR8(0xb02)
#define reg_dfifo0_addHi		REG_ADDR8(0xb03)  //default 0x04, no need set

#define reg_dfifo1_addr			REG_ADDR16(0xb04)
#define reg_dfifo1_size			REG_ADDR8(0xb06)
#define reg_dfifo1_addHi		REG_ADDR8(0xb07)  //default 0x04, no need set

//misc channel only use dfifo2
#define reg_dfifo2_addr			REG_ADDR16(0xb08)
#define reg_dfifo2_size			REG_ADDR8(0xb0a)
#define reg_dfifo2_addHi		REG_ADDR8(0xb0b)  //default 0x04, no need set

#define reg_dfifo_audio_addr		reg_dfifo0_addr
#define reg_dfifo_audio_size		reg_dfifo0_size

#define reg_dfifo_misc_chn_addr		reg_dfifo2_addr
#define reg_dfifo_misc_chn_size		reg_dfifo2_size


#define reg_dfifo0_l_level		REG_ADDR8(0xb0c)  //dfifo0  low int threshold(wptr - rptr)
#define reg_dfifo0_h_level		REG_ADDR8(0xb0d)  //dfifo0 high int threshold(wptr - rptr)
#define reg_dfifo1_h_level		REG_ADDR8(0xb0e)  //dfifo1 high int threshold(wptr - rptr)
#define reg_dfifo2_h_level		REG_ADDR8(0xb0f)  //dfifo2 high int threshold(wptr - rptr)


#define	reg_dfifo_mode	REG_ADDR8(0xb10)
enum{
	FLD_AUD_DFIFO0_IN 		= BIT(0),
	FLD_AUD_DFIFO1_IN 		= BIT(1),
	FLD_AUD_DFIFO2_IN 		= BIT(2),
	FLD_AUD_DFIFO0_OUT 		= BIT(3),
	FLD_AUD_DFIFO0_L_INT	= BIT(4),
	FLD_AUD_DFIFO0_H_INT	= BIT(5),
	FLD_AUD_DFIFO1_H_INT	= BIT(6),
	FLD_AUD_DFIFO2_H_INT	= BIT(7),
};

#define	reg_dfifo_ain			REG_ADDR8(0xb11)
enum{
	FLD_AUD_SAMPLE_TIME_CONFIG        = BIT_RNG(0,1),
	FLD_AUD_FIFO0_INPUT_SELECT     	  = BIT_RNG(2,3),
	FLD_AUD_FIFO1_INPUT_SELECT     	  = BIT_RNG(4,5),
	FLD_AUD_MIC_LEFT_CHN_SELECT       = BIT(6),
	FLD_AUD_MIC_RIGHT_CHN_SELECT      = BIT(7),
};

enum{  //core_b11<3:2>  audio input select
	AUDIO_FIFO0_INPUT_SELECT_USB 	        = 0x00,
	AUDIO_FIFO0_INPUT_SELECT_I2S 	        = 0x01,
	AUDIO_FIFO0_INPUT_SELECT_16BIT 	        = 0x02,
	AUDIO_FIFO0_INPUT_SELECT_20BIT 	        = 0x03,
};

enum{  //core_b11<5:4>  audio input select
	AUDIO_FIFO1_INPUT_SELECT_USB 	        = 0x00,
	AUDIO_FIFO1_INPUT_SELECT_I2S 	        = 0x01,
	AUDIO_FIFO1_INPUT_SELECT_16BIT 	        = 0x02,
	AUDIO_FIFO1_INPUT_SELECT_20BIT 	        = 0x03,
};


#define	reg_mic_ctrl			REG_ADDR8(0xb12)
enum{
	FLD_AUD_MIC_VOL_CONTROL           = BIT_RNG(0,5),
	FLD_AUD_MIC_MONO_EN               = BIT(6),
	FLD_AUD_AMIC_DMIC_SELECT          = BIT(7),
};

enum{
	MIC_VOL_CONTROL_m48DB = 		0x00,
	MIC_VOL_CONTROL_m42DB = 		0x04,
	MIC_VOL_CONTROL_m36DB = 		0x08,
	MIC_VOL_CONTROL_m30DB = 		0x0c,
	MIC_VOL_CONTROL_m24DB = 		0x10,
	MIC_VOL_CONTROL_m18DB = 		0x14,
	MIC_VOL_CONTROL_m16DB = 		0x15,
	MIC_VOL_CONTROL_m12DB = 		0x18,
	MIC_VOL_CONTROL_m6DB  = 	   	0x1c,
	MIC_VOL_CONTROL_0DB   = 	    0x20,
	MIC_VOL_CONTROL_6DB   =  		0x24,
	MIC_VOL_CONTROL_12DB  = 		0x28,
	MIC_VOL_CONTROL_18DB  = 		0x2c,
	MIC_VOL_CONTROL_24DB  = 		0x30,
	MIC_VOL_CONTROL_30DB  = 		0x34,
	MIC_VOL_CONTROL_36DB  = 		0x38,
	MIC_VOL_CONTROL_42DB  =     	0x3c,
};


#define reg_set_filter_para     REG_ADDR8(0xb80)

#define reg_dfifo_dec_ratio	    REG_ADDR8(0xb8a)

#define reg_codec_dec_en	    REG_ADDR8(0xb8b)
enum{
	FLD_AUD_CODEC_DEC_EN	=   BIT(0),
};

#define reg_dfifo_irq_status	REG_ADDR8(0xb13)
enum{
	FLD_AUD_DFIFO0_L_IRQ	= BIT(4),
	FLD_AUD_DFIFO0_H_IRQ	= BIT(5),
	FLD_AUD_DFIFO1_L_IRQ	= BIT(6),
	FLD_AUD_DFIFO1_H_IRQ	= BIT(7),
};
#define reg_dfifo0_rptr			REG_ADDR16(0xb14)
#define reg_dfifo0_wptr			REG_ADDR16(0xb16)

#define reg_dfifo1_rptr			REG_ADDR16(0xb18)
#define reg_dfifo1_wptr			REG_ADDR16(0xb1a)

#define reg_dfifo2_rptr			REG_ADDR16(0xb1c)
#define reg_dfifo2_wptr			REG_ADDR16(0xb1e)


#define reg_audio_wptr			reg_dfifo0_wptr

#define reg_dfifo0_num			REG_ADDR16(0xb20)
#define reg_dfifo1_num			REG_ADDR16(0xb24)
#define reg_dfifo2_num			REG_ADDR16(0xb28)

#define reg_dfifo_manual_mode		REG_ADDR8(0xb2c)
enum{
	FLD_DFIFO_MANUAL_MODE_EN	= BIT(0),
};

#define reg_dfifo0_man_dat		REG_ADDR32(0xb30)


/*******************************      dma registers: 0xc00      ******************************/

//uart rx
#define reg_dma0_addr			REG_ADDR16(0xc00)
#define reg_dma0_size			REG_ADDR8(0xc02)
#define reg_dma0_mode			REG_ADDR8(0xc03)
enum{
	FLD_DMA_WR_MEM =			BIT(0),
	FLD_DMA_PINGPONG_EN =		BIT(1),
	FLD_DMA_FIFO_EN =			BIT(2),
	FLD_DMA_AUTO_MODE =			BIT(3),
	FLD_DMA_READ_MODE =			BIT(4),
	FLD_DMA_BYTE_MODE =			BIT(5)
};


//uart tx
#define reg_dma1_addr			REG_ADDR16(0xc04)
#define reg_dma1_size			REG_ADDR8(0xc06)
#define reg_dma1_mode			REG_ADDR8(0xc07)

//rf rx dma
#define reg_dma2_addr			REG_ADDR16(0xc08)
#define reg_dma2_size			REG_ADDR8(0xc0a)
#define reg_dma2_mode			REG_ADDR8(0xc0b)

//rf tx dma
#define reg_dma3_addr			REG_ADDR16(0xc0c)
#define reg_dma3_size			REG_ADDR8(0xc0e)
#define reg_dma3_mode			REG_ADDR8(0xc0f)

#define reg_dma4_addr			REG_ADDR16(0xc10)
#define reg_dma4_size			REG_ADDR8(0xc12)
#define reg_dma4_mode			REG_ADDR8(0xc13)

#define reg_dma5_addr			REG_ADDR16(0xc14)
#define reg_dma5_size			REG_ADDR8(0xc16)
#define reg_dma5_mode			REG_ADDR8(0xc17)

//pwm tx dma
#define reg_dma7_addr			REG_ADDR16(0xc18)
#define reg_dma7_size			REG_ADDR8(0xc1a)
#define reg_dma7_mode			REG_ADDR8(0xc1b)


#define reg_dma_t_addr			REG_ADDR16(0xc1c)
#define reg_dma_t_size			REG_ADDR8(0xc1e)

#define reg_dma_size(v)			REG_ADDR8(0xc02+4*v)

//  The default channel assignment
#define reg_dma_uart_rx_addr	reg_dma0_addr
#define reg_dma_uart_rx_size	reg_dma0_size
#define reg_dma_uart_rx_mode	reg_dma0_mode

#define reg_dma_uart_tx_addr	reg_dma1_addr
#define reg_dma_uart_tx_size	reg_dma1_size
#define reg_dma_uart_tx_mode	reg_dma1_mode

#define reg_dma_rf_rx_addr		reg_dma2_addr
#define reg_dma_rf_rx_size		reg_dma2_size
#define reg_dma_rf_rx_mode		reg_dma2_mode

#define reg_dma_rf_tx_addr		reg_dma3_addr
#define reg_dma_rf_tx_size		reg_dma3_size
#define reg_dma_rf_tx_mode		reg_dma3_mode

#define reg_dma_pwm_addr		reg_dma7_addr
#define reg_dma_pwm_size		reg_dma7_size
#define reg_dma_pwm_mode		reg_dma7_mode



#define reg_dma_chn_en			REG_ADDR8(0xc20)
#define reg_dma_chn_irq_msk		REG_ADDR8(0xc21)
#define reg_dma_tx_rdy0			REG_ADDR8(0xc24)
#define reg_dma_tx_rdy1			REG_ADDR8(0xc25)
#define reg_dma_rx_rdy0			REG_ADDR8(0xc26)
#define reg_dma_rx_rdy1			REG_ADDR8(0xc27)

#define reg_dma_irq_status		reg_dma_rx_rdy0

enum{
	FLD_DMA_CHN0 =	BIT(0),		FLD_DMA_CHN_UART_RX =	BIT(0),
	FLD_DMA_CHN1 =	BIT(1),		FLD_DMA_CHN_UART_TX =	BIT(1),
	FLD_DMA_CHN2 =	BIT(2),		FLD_DMA_CHN_RF_RX =		BIT(2),
	FLD_DMA_CHN3 =	BIT(3),		FLD_DMA_CHN_RF_TX =		BIT(3),
	FLD_DMA_CHN4 =	BIT(4),		FLD_DMA_CHN_AES_OUT =  BIT(4),
	FLD_DMA_CHN5 =	BIT(5),     FLD_DMA_CHN_AES_IN =  BIT(5),
	FLD_DMA_CHN7 =	BIT(7),		FLD_DMA_CHN_PWM  	 =	BIT(7),
};

typedef enum {
    FLD_DMA_IRQ_UART_RX  = BIT(0),
    FLD_DMA_IRQ_UART_TX  = BIT(1),
    FLD_DMA_IRQ_RF_RX    = BIT(2),
    FLD_DMA_IRQ_RF_TX    = BIT(3),
    FLD_DMA_IRQ_AES_DECO = BIT(4),
    FLD_DMA_IRQ_AES_CODE = BIT(5),
    FLD_DMA_IRQ_PWM		 = BIT(7),
    FLD_DMA_IRQ_ALL      = 0xff,
} IRQ_DMAIrqTypeDef;


#define reg_dma_rx_rptr			REG_ADDR8(0xc28)
#define reg_dma_rx_wptr			REG_ADDR8(0xc29)

#define reg_dma_tx_rptr			REG_ADDR8(0xc2a)
#define reg_dma_tx_wptr			REG_ADDR8(0xc2b)
#define reg_dma_tx_fifo			REG_ADDR16(0xc2c)
enum{
	FLD_DMA_RPTR_CLR =			BIT(4),
	FLD_DMA_RPTR_NEXT =			BIT(5),
	FLD_DMA_RPTR_SET =			BIT(6),
};

#define reg_dma0_addrHi			REG_ADDR8(0xc40)
#define reg_dma1_addrHi			REG_ADDR8(0xc41)
#define reg_dma2_addrHi			REG_ADDR8(0xc42)
#define reg_dma3_addrHi			REG_ADDR8(0xc43)
#define reg_dma4_addrHi			REG_ADDR8(0xc44)
#define reg_dma5_addrHi			REG_ADDR8(0xc45)
#define reg_dma_ta_addrHi		REG_ADDR8(0xc46)
#define reg_dma_a3_addrHi		REG_ADDR8(0xc47)
#define reg_dma7_addrHi			REG_ADDR8(0xc48)

/*******************************      linklayer registers: 0xf00      ******************************/

#define reg_rf_ll_ctrl_0		REG_ADDR8(0xf02)

#define reg_rf_ll_ctrl_1		REG_ADDR8(0xf03)
enum{
	FLD_RF_FSM_TIMEOUT_EN 		=   BIT(0),
	FLD_RF_RX_FIRST_TIMEOUT_EN	=   BIT(1),
	FLD_RF_RX_TIMEOUT_EN		=   BIT(2),
	FLD_RF_CRC_2_EN 			=   BIT(3),

	//BLE mode
	FLD_RF_BRX_SN_INIT	 		=   BIT(4),
	FLD_RF_BRX_NESN_INIT	 	=   BIT(5),
	FLD_RF_BTX_SN_INIT	 		=   BIT(6),
	FLD_RF_BTX_NESN_INIT	 	=   BIT(7),
};

#define  	FSM_TIMEOUT_ENABLE 	 			( reg_rf_ll_ctrl_1 |= FLD_RF_FSM_TIMEOUT_EN )
#define  	FSM_TIMEOUT_DISABLE				( reg_rf_ll_ctrl_1 &= (~FLD_RF_FSM_TIMEOUT_EN) )
#define		FSM_RX_FIRST_TIMEOUT_ENABLE		( reg_rf_ll_ctrl_1 |= FLD_RF_RX_FIRST_TIMEOUT_EN )
#define 	FSM_RX_FIRST_TIMEOUT_DISABLE	( reg_rf_ll_ctrl_1 &= (~FLD_RF_RX_FIRST_TIMEOUT_EN) )

#define reg_rf_rx_timeout		REG_ADDR16(0xf0a)

#define reg_rf_ll_ctrl_2		REG_ADDR8(0xf15)

#define reg_rf_ll_ctrl_3		REG_ADDR8(0xf16)
enum{
	FLD_RF_TX_EN_DLY_EN = 		BIT(0),
	FLD_RF_PLL_RST_EN	=       BIT(1),
	FLD_RF_CMD_SCHEDULE_EN	=   BIT(2),
	FLD_RF_R_TX_EN_DLY =		BIT_RNG(4,7),
};


#define reg_rf_irq_mask			REG_ADDR16(0xf1c)
#define reg_rf_irq_status		REG_ADDR16(0xf20)
#define reg_rf_fsm_timeout		REG_ADDR32(0xf2c)

#define		CLEAR_ALL_RFIRQ_STATUS   ( reg_rf_irq_status = 0xffff )

enum{
	FLD_RF_IRQ_RX = 			BIT(0),
	FLD_RF_IRQ_TX =				BIT(1),
	FLD_RF_IRQ_RX_TIMEOUT =		BIT(2),
	FLD_RF_IRQ_RX_CRC_2 =		BIT(4),
	FLD_RF_IRQ_CMD_DONE  =		BIT(5),
	FLD_RF_IRQ_FSM_TIMEOUT  =	BIT(6),
	FLD_RF_IRQ_RETRY_HIT =		BIT(7),
	FLD_RF_IRQ_TX_DS =          BIT(8),
    FLD_RF_IRQ_RX_DR =          BIT(9),
	FLD_RF_IRQ_FIRST_TIMEOUT =	BIT(10),
	FLD_RF_IRQ_INVALID_PID =    BIT(11),
	FLD_RF_IRQ_STX_TIMEOUT =    BIT(12),
	FLD_RF_IRQ_ALL =            0X1FFF,
};
/*******************************      aura registers: 0x1200      ******************************/
#define REG_AURA_BASE            0x1200
#define reg_rf_tx_flex_mi_0      REG_ADDR8(REG_AURA_BASE+0x1c)
enum
{
	FLD_RF_TX_MI_FRACTION     = BIT_RNG(0,5),
	FLD_RF_TX_MI_INTEGER_1    = BIT_RNG(6,7),
};
#define reg_rf_tx_flex_mi_1     REG_ADDR8(REG_AURA_BASE+0x1d)
enum
{
	FLD_RF_TX_MI_INTEGER_2   = BIT(0),
};

#define reg_rf_rx_flex_mi_0      REG_ADDR8(REG_AURA_BASE+0x1e)
enum
{
	FLD_RF_RX_MI_FRACTION     = BIT_RNG(0,5),
	FLD_RF_RX_MI_INTEGER_1    = BIT_RNG(6,7),
};
#define reg_rf_rx_flex_mi_1     REG_ADDR8(REG_AURA_BASE+0x1f)
enum
{
	FLD_RF_RX_MI_INTEGER_2   = BIT(0),
};

#define		reg_rf_mode_cfg_tx1_0		REG_ADDR8(REG_AURA_BASE+0x22)
enum
{
	FLD_RF_BLE_MODE_TX			      =	BIT(0),
	FLD_RF_VCO_TRIM_KVM			      = BIT_RNG(1,3),
	FLD_RF_HPMC_EXP_DIFF_COUNT_L	  = BIT_RNG(4,7),
};
/*******************************      pke registers: 0x2000      ******************************/
#define reg_pke_ctrl             REG_ADDR32(0x2000)
enum{
	FLD_PKE_CTRL_START = 		 BIT(0),
	FLD_PKE_CTRL_STOP = 		 BIT(16),
};

#define reg_pke_conf             REG_ADDR32(0x2004)
enum{
	FLD_PKE_CONF_IRQ_EN = 		 BIT(8),
	FLD_PKE_CONF_PARTIAL_RADIX = BIT_RNG(16,23),
	FLD_PKE_CONF_BASE_RADIX	=    BIT_RNG(24,26),
};

#define reg_pke_mc_ptr           REG_ADDR32(0x2010)

#define reg_pke_stat             REG_ADDR32(0x2020)
enum{
	FLD_PKE_STAT_DONE = 		 BIT(0),
};

#define reg_pke_rt_code          REG_ADDR32(0x2024)
enum{
	FLD_PKE_RT_CODE_STOP_LOG =	 BIT_RNG(0,3),
};

#define reg_pke_exe_conf         REG_ADDR32(0x2050)
enum{
	FLD_PKE_EXE_CONF_IAFF_R0 = 	 BIT(0),
	FLD_PKE_EXE_CONF_IMON_R0 = 	 BIT(1),
	FLD_PKE_EXE_CONF_IAFF_R1 = 	 BIT(2),
	FLD_PKE_EXE_CONF_IMON_R1 = 	 BIT(3),
	FLD_PKE_EXE_CONF_OAFF = 	 BIT(4),
	FLD_PKE_EXE_CONF_OMON = 	 BIT(5),
	FLD_PKE_EXE_CONF_ME_SCA_EN = BIT_RNG(8,9),
};

/********************************************************************************************
 *****|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|*****
 *****|								Aanlog  Register Table  						   |*****
 *****|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|*****
 ********************************************************************************************/

/*******************************      analog registers(3v3): 0x00      ***************************/

#define areg_06_pll_bg			        0x06
enum{
	FLD_PLL_BG_POWER_DOWN = BIT(4),      //    	1: Power down;     0: Power up
};

#define areg_0e_pa0_pa3_pull			0x0E
#define areg_0f_pa4_pa7_pull			0x0F
#define areg_10_pb0_pb3_pull			0x10
#define areg_11_pb4_pb7_pull			0x11
#define areg_12_pc0_pc3_pull			0x12
#define areg_13_pc4_pc7_pull			0x13
#define areg_14_pd0_pd3_pull			0x14
#define areg_15_pd4_pd7_pull			0x15


/*******************************      mdec register      ***************************/

#define mdec_rst_addr                   0x16
enum{
	FLD_SELE_PA0 = 				BIT(0),
	FLD_SELE_PB7 = 				BIT(1),
	FLD_SELE_PC4 = 			    BIT(2),
	FLD_SELE_PD0 = 				BIT(3),
	FLD_RST_MDEC =              BIT(4),
	FLD_CLS_MDEC = 				BIT_RNG(0,4),
};

#define mdec_match_addr                 0x17
enum{
	FLD_CLS_MATCH_ADDR = 		BIT_RNG(0,3),
};


/*******************************      analog registers(1v8): 0x80      ***************************/
#define areg_auto_pd_ctrl_0				0x2B
enum{
	FLD_AUTO_PD_PL_ALL			  = BIT(0),
	FLD_AUTO_PD_PL_DCDC 		  = BIT(1),

};

#define areg_auto_pd_ctrl_1				0x2C
enum{
	FLD_AUTO_PD_RC_32K			  = BIT(0),
	FLD_AUTO_PD_XTAL_32K 		  = BIT(1),
	FLD_AUTO_PD_XTAL_24M		  = BIT(2),
	FLD_AUTO_PD_LC_COMP		  	  = BIT(3),
	FLD_AUTO_PD_MISC			  = BIT(4),
	FLD_AUTO_PD_XTAL_QUICK_RST 	  = BIT(5),
	FLD_AUTO_PD_OTP			      = BIT(6),
    // <7> rsvd
};



#define areg_clk_setting				0x82
enum{
	FLD_DCCC_DOUBLER_POWER_DOWN	  = BIT(3),
	FLD_CLK_48M_TO_RX_EN 		  = BIT(4),
	FLD_CLK_48M_TO_DIG_EN 		  = BIT(5),
	FLD_CLK_24M_TO_SAR_EN 		  = BIT(6),
	FLD_CLK_48M_TO_CAL_DIG_MAN_EN = BIT(7),
};

#define areg_xo_setting				0x8a
enum{
	FLD_XO_CDAC_ANA	  	= BIT_RNG(0,5),
	FLD_XO_MODE_ANA 	= BIT(6),
	FLD_XO_CAP_OFF_ANA  = BIT(7),
};

/*******************************        analog registers(PGA)        ***************************/
#define codec_ana_cfg2				   0xe6
#define codec_ana_cfg3                 0xe7
#define codec_ana_cfg4                 0xe8

#define pga_audio_enable               0x34


#if defined(__cplusplus)
}
#endif




