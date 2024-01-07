/********************************************************************************************************
 * @file    register.h
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

#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#include "bit.h"
#include "bsp.h"

/****************************************************
  secondary i2c regs struct: begin  addr : 0x00
 *****************************************************/
#define reg_i2c_set				REG_ADDR32(0x00)
#define reg_i2c_speed			REG_ADDR8(0x00)
#define reg_i2c_id				REG_ADDR8(0x01)
#define reg_i2c_status			REG_ADDR8(0x02)
enum{
	FLD_I2C_CMD_BUSY		= 	BIT(0),
	FLD_I2C_BUS_BUSY		= 	BIT(1),
	FLD_I2C_NAK				= 	BIT(2),
};

#define reg_i2c_mode			REG_ADDR8(0x03)
enum{
	FLD_I2C_ADDR_AUTO 		=	BIT(0),
	FLD_I2C_MODE_MASTER		= 	BIT(1),		// 1: master, 0: slave
	FLD_I2C_MEM_MAP 		=	BIT(2), 	// write i2c data to predefined memory address which set by other register
};

#define reg_i2c_adr_dat			REG_ADDR16(0x04)
#define reg_i2c_dat_ctrl		REG_ADDR32(0x04)
#define reg_i2c_di_ctrl			REG_ADDR16(0x06)
#define reg_i2c_adr				REG_ADDR8(0x04)
#define reg_i2c_do				REG_ADDR8(0x05)
#define reg_i2c_di				REG_ADDR8(0x06)
#define reg_i2c_ctrl			REG_ADDR8(0x07)
enum{
	FLD_I2C_CMD_ID		= 		BIT(0),
	FLD_I2C_CMD_ADR		= 		BIT(1),
	FLD_I2C_CMD_DO		= 		BIT(2),
	FLD_I2C_CMD_DI		= 		BIT(3),
	FLD_I2C_CMD_START	= 		BIT(4),
	FLD_I2C_CMD_STOP	= 		BIT(5),
	FLD_I2C_CMD_READ_ID	= 		BIT(6),
	FLD_I2C_CMD_NAK		= 		BIT(7),
};

/****************************************************
  secondary spi regs struct: begin  addr : 0x08
 *****************************************************/
#define reg_spi_data			REG_ADDR8(0x08)
#define reg_spi_ctrl			REG_ADDR8(0x09)
enum{
	FLD_SPI_CS = 				BIT(0),
	FLD_SPI_MASTER_MODE_EN = 	BIT(1),
	FLD_SPI_DATA_OUT_DIS = 		BIT(2),
	FLD_SPI_RD = 				BIT(3),
	FLD_SPI_ADDR_AUTO =			BIT(4),
	FLD_SPI_BUSY = 				BIT(6),		// diff from doc,  bit 6 working
};
#define reg_spi_sp				REG_ADDR8(0x0a)
enum{
	FLD_MASTER_SPI_CLK = 		BIT_RNG(0,6),
	FLD_SPI_ENABLE =			BIT(7),
	FLD_SPI_PAD_SEL = 			BIT(7),
};

#define reg_spi_inv_clk			REG_ADDR8(0x0b)

/****************************************************
 master spi regs struct: begin  addr : 0x0c
 *****************************************************/
#define reg_master_spi_data		REG_ADDR8(0x0c)
#define reg_master_spi_ctrl		REG_ADDR8(0x0d)

enum{
	FLD_MASTER_SPI_CS = 		BIT(0),
	FLD_MASTER_SPI_SDO = 		BIT(1),
	FLD_MASTER_SPI_CONT = 		BIT(2),
	FLD_MASTER_SPI_RD = 		BIT(3),
	FLD_MASTER_SPI_BUSY = 		BIT(4),
};

/****************************************************
 otp regs struct: begin  addr : 0x10
 *****************************************************/

#define reg_otp_addr_para		REG_ADDR16(0x10)
enum{
	FLD_OTP_PARA_ADDR = 		BIT_RNG(0,12),
	FLD_OTP_PARA_PTM = 			BIT_RNG(13,15),
};

#define reg_otp_ctrl			REG_ADDR8(0x12)
enum{
	FLD_OTP_CTRL_PCEN = 		BIT(0),
	FLD_OTP_FAST_CLK = 			BIT(1),
	FLD_OTP_OEN = 				BIT(2),
	FLD_OTP_CLK = 				BIT(3),
	FLD_OTP_PCEN_PWDN = 		BIT(4),
	FLD_OTP_WEN_PWDN = 			BIT(5),
	FLD_OTP_OEN_PWDN = 			BIT(6),
	FLD_OTP_CLK_PWDN = 			BIT(7),
};

#define reg_otp_byte_dat		REG_ADDR8(0x13)
#define reg_otp_dat				REG_ADDR32(0x14)
#define reg_otp_blk_code		REG_ADDR8(0x18)

#define reg_i2c_irq_status		REG_ADDR8(0x21)
#define reg_i2c_clr_status		REG_ADDR8(0x22)
enum{
	FLD_I2C_STATUS_WR = 		BIT(1),
	FLD_I2C_STATUS_RD = 		BIT(2),
};

/****************************************************
 adc regs struct: begin  addr : 0x28
 *****************************************************/

/////////// pga select, [3:0] L; [7:4] R ////////////////////
#define reg_adc_pga_sel_m		REG_ADDR8(0x28)
#define reg_adc_pga_sel_l		REG_ADDR8(0x29)
#define reg_adc_pga_sel_r		REG_ADDR8(0x2a)

#define reg_adc_ref				REG_ADDR8(0x2b)
enum{
	FLD_ADC_REF_M = 			BIT_RNG(0,1),
	FLD_ADC_REF_L = 			BIT_RNG(2,3),
	FLD_ADC_REF_R = 			BIT_RNG(4,5),
};

/////////// adc select [4:0] channel; [6:5] mode; [7] signed
#define reg_adc_chn_m_sel		REG_ADDR8(0x2c)
#define reg_adc_chn_l_sel		REG_ADDR8(0x2d)
#define reg_adc_chn_r_sel		REG_ADDR8(0x2e)
enum{
	FLD_ADC_CHN_SEL = 			BIT_RNG(0,4),
	FLD_ADC_DIFF_CHN_SEL = 		BIT_RNG(5,7),	// datasheet  12.1
};

enum{
	FLD_ADC_CHN_D0				= 0x01,
	FLD_ADC_CHN_D1				= 0x02,
	FLD_ADC_CHN_D2				= 0x03,
	FLD_ADC_CHN_D3				= 0x04,
	FLD_ADC_CHN_D4				= 0x05,
	FLD_ADC_CHN_D5				= 0x06,
	FLD_ADC_CHN_C2				= 0x07,
	FLD_ADC_CHN_C3				= 0x08,
	FLD_ADC_CHN_C4				= 0x09,
	FLD_ADC_CHN_C5				= 0x0a,
	FLD_ADC_CHN_C6				= 0x0b,
	FLD_ADC_CHN_C7				= 0x0c,

	FLD_ADC_CHN_PGA_R			= 0x0d,
	FLD_ADC_CHN_PGA_L			= 0x0e,

	FLD_ADC_CHN_TEMP_POS		= 0x0f,
	FLD_ADC_CHN_TEMP_NEG		= 0x10,
	FLD_ADC_CHN_VBUS			= 0x11,
	FLD_ADC_CHN_GND				= 0x12,

	FLD_ADC_SINGLE_IN			= 0x00,
	FLD_ADC_D5_REF				= 0x20,
	FLD_ADC_D7_REF				= 0x40,
	FLD_ADC_PGA_L_REF			= 0x60,
	FLD_ADC_DATA_SIGNED			= 0x80,

	FLD_ADC_PGA_C01				= 0x226d,
	FLD_ADC_PGA_C23				= 0x116d,
	FLD_ADC_PGA_C45				= 0x016d,

	FLD_ADC_PGA_R_C2			= 0x110d,
	FLD_ADC_PGA_R_C0			= 0x220d,
	FLD_ADC_PGA_L_C3			= 0x110e,
	FLD_ADC_PGA_L_C1			= 0x220e,

	FLD_ADC_PGA_OFF				= 0x0000,

};

#define reg_adc_ref				REG_ADDR8(0x2b)

#define reg_adc_res_lr			REG_ADDR8(0x2f)
#define reg_adc_res_m			REG_ADDR8(0x3c)
#define reg_adc_tsamp_lr		REG_ADDR8(0x3d)

enum{
	FLD_ADC_VREF_1P3V			= 0x15,
	FLD_ADC_12BITS				= 0x07,
	FLD_ADC_TSAMP_3CLOCK		= 0x00,
};


#define reg_adc_period_chn0		REG_ADDR16(0x30)
#define reg_adc_period_chn12	REG_ADDR8(0x32)

#define reg_adc_ctrl			REG_ADDR8(0x33)
enum{
	FLD_ADC_CHNL_AUTO_EN = 		BIT(0),
	FLD_ADC_CHNR_AUTO_EN =		BIT(1),
	FLD_ADC_AUD_DATAPATH_EN =	BIT(2),
	FLD_ADC_CHNM_AUTO_EN =		BIT(3),
	FLD_ADC_AUD_MODE =			BITS(4,5),		// 0: no audio; 1: mono; 2: stereo; 3: reserved
	FLD_ADC_DONE_SIGNAL =		BITS(6,7),		// 1: rising; 2: falling; else: counter
};

#define reg_adc_chn1_outp		REG_ADDR16(0x34)
enum{
	FLD_ADC_OUTPUT_CHN_VAL = 	BIT_RNG(0,14),
	FLD_ADC_CHN_MANU_START = 	BIT(15),
};
#define reg_adc_chn2_outp		REG_ADDR16(0x36)
#define reg_adc_dat_byp_outp	REG_ADDR16(0x38)

#define reg_adc_chn0_input		REG_ADDR16(0x3a)

#define reg_adc_samp_res		REG_ADDR16(0x3c)
enum{
	FLD_ADC_CHNM_SAMP_CYCLE = 	BIT_RNG(0,2),
	FLD_ADC_CHNM_SAMP_RESOL = 	BIT_RNG(3,5),
	FLD_ADC_CHNLR_SAMP_CYCLE =	BIT_RNG(8,10),	// reg: 0x3d
};

#define reg_i2c_mem_map			REG_ADDR16(0x3e)	// check with spec again // refer to FLD_I2C_MEM_MAP

/****************************************************
 sys regs struct: begin  addr : 0x60
 *****************************************************/
#define reg_rst0				REG_ADDR8(0x60)
#define reg_rst0_16				REG_ADDR16(0x60)
#define reg_rst1				REG_ADDR8(0x61)
#define reg_rst2				REG_ADDR8(0x62)
#define reg_rst_clk0			REG_ADDR32(0x60)
enum{
	FLD_RST_SPI = 				BIT(0),
	FLD_RST_I2C = 				BIT(1),
	FLD_RST_USB = 				BIT(2),
	FLD_RST_USB_PHY = 			BIT(3),
	FLD_RST_MCU = 				BIT(4),
	FLD_RST_MAC =				BIT(5),
	FLD_RST_AIF = 				BIT(6),
	FLD_RST_BB = 				BIT(7),
	FLD_RST_ZB =				BIT(7),
	FLD_RST_GPIO = 				BIT(8),
	FLD_RST_ALGM = 				BIT(9),
	FLD_RST_DMA =				BIT(10),
	FLD_RST_UART = 				BIT(11),
	FLD_RST_PWM = 				BIT(12),
	FLD_RST_AES = 				BIT(13),
	FLD_RST_SWR_M =				BIT(14),
	FLD_RST_SWR_S =				BIT(15),
	FLD_RST_SBC =				BIT(16),
	FLD_RST_AUD =				BIT(17),
	FLD_RST_DFIFO =				BIT(18),
	FLD_RST_ADC =				BIT(19),
	FLD_RST_SOFT_MCU =			BIT(20),
	FLD_RST_MCIC = 				BIT(21),
	FLD_RST_SOFT_MCIC =			BIT(22),
	FLD_RST_RSV =				BIT(23),
	FLD_CLK_SPI_EN =			BIT(24),
	FLD_CLK_I2C_EN =			BIT(25),
	FLD_CLK_USB_EN =			BIT(26),
	FLD_CLK_USB_PHY_EN =		BIT(27),
	FLD_CLK_MCU_EN =			BIT(28),
	FLD_CLK_MAC_EN =			BIT(29),
	FLD_CLK_ADC_EN =			BIT(30),	// ADC interface
	FLD_CLK_ZB_EN =				BIT(31),
};

#define reg_clk_en				REG_ADDR16(0x64)
#define reg_clk_en1				REG_ADDR8(0x64)
enum{
	FLD_CLK_GPIO_EN = 			BIT(0),
	FLD_CLK_ALGM_EN = 			BIT(1),
	FLD_CLK_DMA_EN = 			BIT(2),
	FLD_CLK_UART_EN = 			BIT(3),
	FLD_CLK_PWM_EN = 			BIT(4),
	FLD_CLK_AES_EN = 			BIT(5),
	FLD_CLK_32K_TIMER_EN =		BIT(6),
	FLD_CLK_SWIRE_EN = 			BIT(7),
	FLD_CLK_32K_QDEC_EN =		BIT(8),
	FLD_CLK_AUD_EN =			BIT(9),
	FLD_CLK_DIFIO_EN = 			BIT(10),
	FLD_CLK_KEYSCAN_EN =		BIT(11),
	FLD_CLK_MCIC_EN =			BIT(12),
	FLD_CLK_QDEC_EN =			BIT(13),
};
#define reg_clk_en2				REG_ADDR8(0x65)
enum{
	FLD_CLK2_SBC_EN =			BIT(0),
	FLD_CLK2_AUD_EN =			BIT(1),
	FLD_CLK2_DIFIO_EN = 		BIT(2),
	FLD_CLK2_I2S =				BIT_RNG(3,4),
	FLD_CLK2_C32K =				BIT_RNG(5,7),
};

#define reg_clk_sel				REG_ADDR8(0x66)
enum{
	FLD_CLK_SEL_DIV = 			BIT_RNG(0,4),
	FLD_CLK_SEL_SRC =			BIT_RNG(5,7),
};

#define reg_i2s_step			REG_ADDR8(0x67)
enum{
	FLD_I2S_STEP = 				BIT_RNG(0,6),
	FLD_I2S_CLK_EN =			BIT(7),
};

#define reg_i2s_mod				REG_ADDR8(0x68)

static inline void SET_SDM_CLOCK_MHZ(int f_mhz)	{
	reg_i2s_step = FLD_I2S_CLK_EN | f_mhz;
	reg_i2s_mod = 0xc0;
}

/****************************************************
	 ADC: 0x69
 *****************************************************/
#define reg_adc_step_l			REG_ADDR8(0x69)
#define reg_adc_mod_l			REG_ADDR8(0x6a)
#define reg_adc_mod				REG_ADDR16(0x6a)
enum{
	FLD_ADC_MOD = 				BIT_RNG(0,11),
	FLD_ADC_STEP_H = 			BIT_RNG(12,14),
	FLD_ADC_CLK_EN =			BIT(15),
};
#define reg_adc_clk_en			REG_ADDR8(0x6b)
#define reg_adc_mod_h			REG_ADDR8(0x6b)
enum{
	FLD_ADC_MOD_H = 			BIT_RNG(0,3),
	FLD_ADC_MOD_H_STEP =		BIT_RNG(4,6),
	FLD_ADC_MOD_H_CLK =			BIT(7),
};

#define reg_dmic_step			REG_ADDR8(0x6c)
enum{
	FLD_DMIC_STEP = 			BIT_RNG(0,6),
	FLD_DMIC_CLK_EN =			BIT(7),
};
#define reg_dmic_mod			REG_ADDR8(0x6d)


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
enum{
	FLD_PWDN_CTRL_REBOOT = 		BIT(5),
	FLD_PWDN_CTRL_SLEEP =		BIT(7),
};

#define reg_fhs_sel				REG_ADDR8(0x70)
enum{
	FLD_FHS_SELECT = 			BIT_RNG(0,1),
};
enum{
	FHS_SEL_192M_PLL = 0,
//	FHS_SEL_48M_PLL = 1,
	FHS_SEL_32M_OSC = 1,
//	FHS_SEL_16M_OSC = 3,
};

/****************************************************
  OTP  addr : 0x71
 *****************************************************/
#define reg_dcdc_clk			REG_ADDR8(0x71)

/****************************************************
 dev_id regs struct: begin  addr : 0x74
 *****************************************************/
#define reg_id_wr_en			REG_ADDR8(0x74)
#define reg_product_id			REG_ADDR32(0x7c)
#define reg_func_id				REG_ADDR8(0x7c)
enum{
	FLD_ID_USB_SYS_CLOCK = 		BIT(0),
	FLD_ID_MAC_CLOCK = 			BIT(1),
	FLD_ID_SBC_CLOCK = 			BIT(2),
	FLD_ID_RISC32_ENABLE = 		BIT(3),
	FLD_ID_I2S_ENABLE = 		BIT(4),
	FLD_ID_SDM_ENABLE = 		BIT(5),
	FLD_ID_RF_ENABLE = 			BIT(6),
	FLD_ID_ZB_ENABLE = 			BIT(7),
};

#define reg_ver_id				REG_ADDR8(0x7d)
#define reg_prod_id				REG_ADDR16(0x7e)

#define reg_mcu_wakeup_mask		REG_ADDR32(0x78)

/****************************************************
 mac regs struct: begin  addr : 0xa0
 *****************************************************/
#define reg_mac_ctrl			REG_ADDR8(0x80)
enum{
	FLD_MAC_CTRL_TX_EN = 		BIT(0),
	FLD_MAC_CTRL_RX_EN =		BIT(1),
	FLD_MAC_CTRL_SPD_100M =		BIT(6),
	FLD_MAC_CTRL_PHY_RST =		BIT(7),
};

#define reg_mac_irq_sta			REG_ADDR8(0x81)
enum{
	FLD_MAC_STA_TX_DONE = 		BIT(0),
	FLD_MAC_STA_RX_DONE =		BIT(1),
	FLD_MAC_STA_TX_ERR =		BIT(2),
	FLD_MAC_STA_RX_ERR =		BIT(3),
	FLD_MAC_STA_RX_CRC =		BIT(4),
};

#define reg_mii_tx_data			REG_ADDR16(0x82)
#define reg_mii_rx_data			REG_ADDR16(0x84)
#define reg_mii_clk				REG_ADDR8(0x86)
enum{
	FLD_MII_PHY_ID  = 			BIT_RNG(0,4),
	FLD_MII_CLK_DIV =			BIT_RNG(5,7),
};
#define reg_mii_ctrl			REG_ADDR8(0x87)
enum{
	FLD_MII_INTERNAL_REG  = 	BIT_RNG(0,4),
	FLD_MII_BUSY =				BIT(5),
	FLD_MII_WR =				BIT(6),
	FLD_MII_PREAM_EN =			BIT(7),
};

/////////////  Uart  //////////////////////////////////
#define reg_uart_clk_div		REG_ADDR16(0x94)
enum{
	FLD_UART_CLK_DIV = 			BIT_RNG(0,14),
	FLD_UART_CLK_DIV_EN = 		BIT(15)
};

#define reg_uart_ctrl0			REG_ADDR16(0x96)
enum{
	FLD_UART_BPWC = 			BIT_RNG(0,3),
	FLD_UART_RX_DMA_EN = 		BIT(4),
	FLD_UART_TX_DMA_EN =		BIT(5),
	FLD_UART_RX_IRQ_EN = 		BIT(6),
	FLD_UART_TX_IRQ_EN =		BIT(7),
	FLD_UART_CTS_I_SELECT =		BIT(8),
	FLD_UART_CTS_EN = 			BIT(9),
	FLD_UART_PARITY_EN =		BIT(10),
};
#define reg_uart_ctrl1			REG_ADDR16(0x98)

#define reg_uart_rx_timeout		REG_ADDR16(0x9a)
enum{
	FLD_UART_TIMEOUT_BW = 		BIT_RNG(0,7),		//  timeout bit width
	FLD_UART_TIMEOUT_MUL = 		BIT_RNG(8,15),
};

/****************************************************
 swire regs struct: begin  addr : 0xb0
 *****************************************************/
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

//////  analog controls 0xb8 ///////
#define reg_ana_ctrl32			REG_ADDR32(0xb8)	// for performance, set addr and data at a time
#define reg_ana_addr_data		REG_ADDR16(0xb8)	// for performance, set addr and data at a time
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
/****************************************************
 audio regs struct: begin  addr : 0x100
 *****************************************************/
#define reg_ctrl_ep_ptr			REG_ADDR8(0x100)
#define reg_ctrl_ep_dat			REG_ADDR8(0x101)
#define reg_ctrl_ep_ctrl		REG_ADDR8(0x102)

// same for all endpoints
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
#define reg_usb_ep8_send_max	REG_ADDR8(0x13b)
#define reg_usb_ep8_send_thre	REG_ADDR8(0x13c)
#define reg_usb_ep8_fifo_mode	REG_ADDR8(0x13d)
#define reg_usb_ep_max_size		REG_ADDR8(0x13e)

enum{
	FLD_USB_ENP8_FIFO_MODE =	BIT(0),
	FLD_USB_ENP8_FULL_FLAG =	BIT(1),
};
/****************************************************
	RF : begin  addr : 0x4e8
 *****************************************************/
#define reg_rf_tx_mode1			REG_ADDR8(0x400)
#define reg_rf_tx_mode			REG_ADDR16(0x400)
enum{
	FLD_RF_TX_DMA_EN =			BIT(0),
	FLD_RF_TX_CRC_EN =			BIT(1),
	FLD_RF_TX_BANDWIDTH =		BIT_RNG(2,3),
	FLD_RF_TX_OUTPUT = 			BIT(4),
	FLD_RF_TX_TST_OUT =			BIT(5),
	FLD_RF_TX_TST_EN =			BIT(6),
	FLD_RF_TX_TST_MODE =		BIT(7),
	FLD_RF_TX_ZB_PN_EN =		BIT(8),
	FLD_RF_TX_ZB_FEC_EN =		BIT(9),
	FLD_RF_TX_ZB_INTL_EN =		BIT(10),	// interleaving
	FLD_RF_TX_1M2M_PN_EN =		BIT(11),
	FLD_RF_TX_1M2M_FEC_EN =		BIT(12),
	FLD_RF_TX_1M2M_INTL_EN =	BIT(13), 	// interleaving
};
#define reg_rf_tx_buf_sta		REG_ADDR32(0x41c)

#define reg_rf_rx_sense_thr		REG_ADDR8(0x422)
#define reg_rf_rx_auto			REG_ADDR8(0x426)
enum{
	FLD_RF_RX_IRR_GAIN =		BIT(0),
	FLD_RF_RX_IRR_PHASE =		BIT(1),
	FLD_RF_RX_DAC_I =			BIT(2),
	FLD_RF_RX_DAC_Q =			BIT(3),
	FLD_RF_RX_LNA_GAIN =		BIT(4),
	FLD_RF_RX_MIX2_GAIN =		BIT(5),
	FLD_RF_RX_PGA_GAIN =		BIT(6),
	FLD_RF_RX_CAL_EN =			BIT(7),
};
#define reg_rf_rx_sync			REG_ADDR8(0x427)
enum{
	FLD_RF_FREQ_COMP_EN =		BIT(0),
	FLD_RF_ADC_SYNC =			BIT(1),
	FLD_RF_ADC_INP_SIGNED =		BIT(2),
	FLD_RF_SWAP_ADC_IQ =		BIT(3),
	FLD_RF_NOTCH_FREQ_SEL =		BIT(4),
	FLD_RF_NOTCH_BAND_SEL = 	BIT(5),
	FLD_RF_NOTCH_EN = 			BIT(6),
	FLD_RF_DN_CONV_FREQ_SEL =	BIT(7),
};

#define reg_rf_rx_mode			REG_ADDR8(0x428)
enum{
	FLD_RF_RX_EN =				BIT(0),
	FLD_RF_RX_MODE_1M =			BIT(1),
	FLD_RF_RX_MODE_2M =			BIT(2),
	FLD_RF_RX_LOW_IF =			BIT(3),
	FLD_RF_RX_BYPASS_DCOC =		BIT(4),
	FLD_RF_RX_MAN_FINE_TUNE = 	BIT(5),
	FLD_RF_RX_SINGLE_CAL =		BIT(6),
	FLD_RF_RX_LOW_PASS_FILTER =	BIT(7),
};

#define reg_rf_rx_pilot			REG_ADDR8(0x42b)
enum{
	FLD_RF_PILOT_LEN =			BIT_RNG(0,3),
	FLD_RF_ZB_SFD_CHK =			BIT(4),
	FLD_RF_1M_SFD_CHK =			BIT(5),
	FLD_RF_2M_SFD_CHK = 		BIT(6),
	FLD_RF_ZB_OR_AUTO = 		BIT(7),
};

#define reg_rf_rx_chn_dc		REG_ADDR32(0x42c)
#define reg_rf_rx_q_chn_cal		REG_ADDR8(0x42f)
enum{
	FLD_RF_RX_DCQ_HIGH =		BIT_RNG(0,6),
	FLD_RF_RX_DCQ_CAL_START =	BIT(7),
};
#define reg_rf_rx_pel			REG_ADDR16(0x434)
#define reg_rf_rx_pel_gain		REG_ADDR32(0x434)
#define reg_rf_rx_rssi_offset	REG_ADDR8(0x439)

#define reg_rf_rx_hdx			REG_ADDR8(0x43b)
enum{
	FLD_RX_HEADER_LEN =			BIT_RNG(0,3),
	FLD_RT_TICK_LO_SEL = 		BIT(4),
	FLD_RT_TICK_HI_SEL = 		BIT(5),
	FLD_RT_TICK_FRAME = 		BIT(6),
	FLD_PKT_LEN_OUTP_EN = 		BIT(7),
};

#define reg_rf_rx_gctl			REG_ADDR8(0x43c)
enum{
	FLD_RX_GCTL_CIC_SAT_LO_EN =	BIT(0),
	FLD_RX_GCTL_CIC_SAT_HI_EN = BIT(1),
	FLD_RX_GCTL_AUTO_PWR =		BIT(2),
	FLD_RX_GCTL_ADC_RST_VAL =	BIT(4),
	FLD_RX_GCTL_ADC_RST_EN =	BIT(5),
	FLD_RX_GCTL_PWR_CHG_DET_S =	BIT(6),
	FLD_RX_GCTL_PWR_CHG_DET_N = BIT(7),
};
#define reg_rf_rx_peak			REG_ADDR8(0x43d)
enum{
	FLD_RX_PEAK_DET_SRC_EN =	BIT_RNG(0,2),
	FLD_TX_PEAK_DET_EN =		BIT(3),
	FLD_PEAK_DET_NUM =			BIT_RNG(4,5),
	FLD_PEAK_MAX_CNT_PRD =		BIT_RNG(6,7),
};

#define reg_rf_rx_status		REG_ADDR8(0x443)
enum{
	FLD_RF_RX_STATE =			BIT_RNG(0,3),
	FLD_RF_RX_STA_RSV = 		BIT_RNG(4,5),
	FLD_RF_RX_INTR = 			BIT(6),
	FLD_RF_TX_INTR =			BIT(7),
};

#define reg_rf_irq_mask			REG_ADDR16(0xf1c)
#define reg_rf_irq_status		REG_ADDR16(0xf20)

enum{
	FLD_RF_IRQ_RX = 			BIT(0),
	FLD_RF_IRQ_TX =				BIT(1),
	FLD_RF_IRQ_RX_TIMEOUT =		BIT(2),
	FLD_RF_IRQ_CRC =			BIT(4),
	FLD_RF_IRQ_CMD_DONE  =		BIT(5),
	FLD_RF_IRQ_FSM_TIMEOUT  =	BIT(6),
	FLD_RF_IRQ_RETRY_HIT =		BIT(7),
	FLD_RF_IRQ_FIRST_TIMEOUT =	BIT(10),
	FLD_RF_IRQ_ALL =            0X1FFF,
};

// The value for FLD_RF_RX_STATE
enum{
	RF_RX_STA_IDLE = 0,
	RF_RX_STA_SET_GAIN = 1,
	RF_RX_STA_CIC_SETTLE = 2,
	RF_RX_STA_LPF_SETTLE = 3,
	RF_RX_STA_PE = 4,
	RF_RX_STA_SYN_START = 5,
	RF_RX_STA_GLOB_SYN = 6,
	RF_RX_STA_GLOB_LOCK = 7,
	RF_RX_STA_LOCAL_SYN = 8,
	RF_RX_STA_LOCAL_LOCK = 9,
	RF_RX_STA_ALIGN = 10,
	RF_RX_STA_ADJUST = 11,
	RF_RX_STA_DEMOD = 12,		// de modulation
	RF_RX_STA_FOOTER = 13,
};

#define reg_rx_rnd_mode			REG_ADDR8(0x447)
enum{
	FLD_RX_RND_SRC =			BIT(0),
	FLD_RX_RND_MANU_MODE =		BIT(1),
	FLD_RX_RND_AUTO_RD =		BIT(2),
	FLD_RX_RND_FREE_MODE =		BIT(3),
	FLD_RX_RND_CLK_DIV =		BIT_RNG(4,7),
};
#define reg_rnd_number			REG_ADDR16(0x448)

#define reg_bb_max_tick			REG_ADDR16(0x44c)
#define reg_rf_rtt				REG_ADDR32(0x454)
enum{
	FLD_RTT_CAL =				BIT_RNG(0,7),
	FLD_RTT_CYC1 =				BIT_RNG(8,15),
	FLD_RTT_LOCK =				BIT_RNG(16,23),
	FLD_RT_SD_DLY_40M =			BIT_RNG(24,27),
	FLD_RT_SD_DLY_BYPASS = 		BIT(28),
};

#define reg_rf_chn_rssi			REG_ADDR8(0x458)

#define reg_rf_rx_gain_agc(i)	REG_ADDR32(0x480+((i)<<2))

#define reg_rf_rx_dci			REG_ADDR8(0x4cb)	//  different from the document, why
#define reg_rf_rx_dcq			REG_ADDR8(0x4cf)	//  different from the document, why

#define reg_pll_rx_coarse_tune	REG_ADDR16(0x4d0)
#define reg_pll_rx_coarse_div	REG_ADDR8(0x4d2)
#define reg_pll_rx_fine_tune	REG_ADDR16(0x4d4)
#define reg_pll_rx_fine_div		REG_ADDR8(0x4d6)
#define reg_pll_tx_coarse_tune	REG_ADDR16(0x4d8)
#define reg_pll_tx_coarse_div	REG_ADDR8(0x4da)
#define reg_pll_tx_fine_tune	REG_ADDR16(0x4dc)
#define reg_pll_tx_fine_div		REG_ADDR8(0x4de)

#define reg_pll_rx_frac			REG_ADDR32(0x4e0)
#define reg_pll_tx_frac			REG_ADDR32(0x4e4)

#define reg_pll_tx_ctrl			REG_ADDR8(0x4e8)
#define reg_pll_ctrl16			REG_ADDR16(0x4e8)
#define reg_pll_ctrl			REG_ADDR32(0x4e8)
enum{
	FLD_PLL_TX_CYC0 =			BIT(0),
	FLD_PLL_TX_SOF =			BIT(1),
	FLD_PLL_TX_CYC1 =			BIT(2),
	FLD_PLL_TX_PRE_EN =			BIT(3),
	FLD_PLL_TX_VCO_EN =			BIT(4),
	FLD_PLL_TX_PWDN_DIV =		BIT(5),
	FLD_PLL_TX_MOD_EN =			BIT(6),
	FLD_PLL_TX_MOD_TRAN_EN =	BIT(7),
	FLD_PLL_RX_CYC0 =			BIT(8),
	FLD_PLL_RX_SOF = 			BIT(9),
	FLD_PLL_RX_CYC1 =			BIT(10),
	FLD_PLL_RX_PRES_EN = 		BIT(11),
	FLD_PLL_RX_VCO_EN =			BIT(12),
	FLD_PLL_RX_PWDN_DIV =		BIT(13),
	FLD_PLL_RX_PEAK_EN =		BIT(14),
	FLD_PLL_RX_TP_CYC = 		BIT(15),
	FLD_PLL_SD_RSTB =			BIT(16),
	FLD_PLL_SD_INTG_EN =		BIT(17),
	FLD_PLL_CP_TRI = 			BIT(18),
	FLD_PLL_PWDN_INTG1 = 		BIT(19),
	FLD_PLL_PWDN_INTG2 =		BIT(20),
	FLD_PLL_PWDN_INTG_DIV =		BIT(21),
	FLD_PLL_PEAK_DET_EN =		BIT(22),
	FLD_PLL_OPEN_LOOP_EN =		BIT(23),
	FLD_PLL_RX_TICK_EN =		BIT(24),
	FLD_PLL_TX_TICK_EN =		BIT(25),
	FLD_PLL_RX_ALWAYS_ON =		BIT(26),
	FLD_PLL_TX_ALWAYS_ON =		BIT(27),
	FLD_PLL_MANUAL_MODE_EN =	BIT(28),
	FLD_PLL_CAL_DONE_EN =		BIT(29),
	FLD_PLL_LOCK_EN =			BIT(30),
};
#define reg_pll_rx_ctrl			REG_ADDR8(0x4e9)
enum{
	FLD_PLL_RX2_CYC0 =			BIT(0),
	FLD_PLL_RX2_SOF = 			BIT(1),
	FLD_PLL_RX2_CYC1 =			BIT(2),
	FLD_PLL_RX2_PRES_EN = 		BIT(3),
	FLD_PLL_RX2_VCO_EN =		BIT(4),
	FLD_PLL_RX2_PD_DIV =		BIT(5),
	FLD_PLL_RX2_PEAK_EN =		BIT(6),
	FLD_PLL_RX2_TP_CYC = 		BIT(7),
};

#define reg_pll_ctrl_a			REG_ADDR8(0x4eb)
enum{
	FLD_PLL_A_RX_TICK_EN =		BIT(0),
	FLD_PLL_A_TX_TICK_EN =		BIT(1),
	FLD_PLL_A_RX_ALWAYS_ON =	BIT(2),
	FLD_PLL_A_TX_ALWAYS_ON =	BIT(3),
	FLD_PLL_A_MANUAL_MODE_EN =	BIT(4),
	FLD_PLL_A_CAL_DONE_EN =		BIT(5),
	FLD_PLL_A_LOCK_EN =			BIT(6),
};
// pll polarity
#define reg_pll_pol_ctrl		REG_ADDR16(0x4ec)
enum{
	FLD_PLL_POL_TX_PRE_EN =		BIT(0),
	FLD_PLL_POL_TX_VCO_EN =		BIT(1),
	FLD_PLL_POL_TX_PD_DIV =		BIT(2),
	FLD_PLL_POL_MOD_EN =		BIT(3),
	FLD_PLL_POL_MOD_TRAN_EN =	BIT(4),
	FLD_PLL_POL_RX_PRE_EN =		BIT(5),
	FLD_PLL_POL_RX_VCO_EN =		BIT(6),
	FLD_PLL_POL_RX_PD_DIV =		BIT(7),
	FLD_PLL_POL_SD_RSTB =		BIT(8),
	FLD_PLL_POL_SD_INTG_EN =	BIT(9),
	FLD_PLL_POL_CP_TRI =		BIT(10),
	FLD_PLL_POL_TX_SOF =		BIT(11),
	FLD_PLL_POL_RX_SOF =		BIT(12),
};

#define reg_rf_rx_cap			REG_ADDR16(0x4f0)
#define reg_rf_tx_cap			REG_ADDR16(0x4f0)

/****************************************************
 dma mac regs struct: begin  addr : 0x500
 *****************************************************/
#define reg_dma0_addr			REG_ADDR16(0x500)
#define reg_dma0_ctrl			REG_ADDR16(0x502)
#define reg_dma1_addr			REG_ADDR16(0x504)
#define reg_dma1_ctrl			REG_ADDR16(0x506)
#define reg_dma2_addr			REG_ADDR16(0x508)
#define reg_dma2_ctrl			REG_ADDR16(0x50a)
#define reg_dma3_addr			REG_ADDR16(0x50c)
#define reg_dma3_ctrl			REG_ADDR16(0x50e)
#define reg_dma4_addr			REG_ADDR16(0x510)
#define reg_dma4_ctrl			REG_ADDR16(0x512)
#define reg_dma5_addr			REG_ADDR16(0x514)
#define reg_dma5_ctrl			REG_ADDR16(0x516)

#define reg_dma_rx_rptr			REG_ADDR8(0x528)
#define reg_dma_rx_wptr			REG_ADDR8(0x529)

#define reg_dma_tx_rptr			REG_ADDR8(0x52a)
#define reg_dma_tx_wptr			REG_ADDR8(0x52b)
#define reg_dma_tx_fifo			REG_ADDR16(0x52c)
enum{
	FLD_DMA_BUF_SIZE =			BIT_RNG(0,7),
	FLD_DMA_WR_MEM =			BIT(8),
	FLD_DMA_PINGPONG_EN =		BIT(9),
	FLD_DMA_FIFO_EN =			BIT(10),
	FLD_DMA_AUTO_MODE =			BIT(11),
	FLD_DMA_BYTE_MODE =			BIT(12),
	FLD_DMA_FIFO8 	=			(BIT(15) | BIT(14) | FLD_DMA_WR_MEM | FLD_DMA_PINGPONG_EN),

	FLD_DMA_RPTR_CLR =			BIT(4),
	FLD_DMA_RPTR_NEXT =			BIT(5),
	FLD_DMA_RPTR_SET =			BIT(6),
};

#define reg_dma_chn_en			REG_ADDR8(0x520)
#define reg_dma_chn_irq_msk		REG_ADDR8(0x521)
#define reg_dma_tx_rdy0			REG_ADDR8(0x524)
#define reg_dma_tx_rdy1			REG_ADDR8(0x525)
#define reg_dma_rx_rdy0			REG_ADDR8(0x526)
#define reg_dma_irq_src			reg_dma_rx_rdy0
#define reg_dma_rx_rdy1			REG_ADDR8(0x527)
enum{
	FLD_DMA_UART_RX =			BIT(0),		//  not sure ???
	FLD_DMA_UART_TX =			BIT(1),
	FLD_DMA_RF_RX =				BIT(2),		//  not sure ???
	FLD_DMA_RF_TX =				BIT(3),
	FLD_DMA_CHN_UART_RX =		BIT(0),
	FLD_DMA_CHN_UART_TX =		BIT(1),
	FLD_DMA_CHN_RF_RX =			BIT(2),
	FLD_DMA_CHN_RF_TX =			BIT(3),
};


//  The default channel assignment
#define reg_dma_uart_rx_addr	reg_dma0_addr
#define reg_dma_uart_rx_ctrl	reg_dma0_ctrl
#define reg_dma_uart_tx_addr	reg_dma1_addr
#define reg_dma_uart_tx_ctrl	reg_dma1_ctrl

#define reg_dma_rf_rx_addr		reg_dma2_addr
#define reg_dma_rf_rx_ctrl		reg_dma2_ctrl
#define reg_dma_rf_tx_addr		reg_dma3_addr
#define reg_dma_rf_tx_ctrl		reg_dma3_ctrl

#define reg_aes_ctrl            REG_ADDR8(0x540)
#define reg_aes_data            REG_ADDR32(0x548)
#define reg_aes_key(key_id)     reg_aes_key##key_id
#define reg_aes_key0            REG_ADDR8(0x550)
#define reg_aes_key1            REG_ADDR8(0x551)
#define reg_aes_key2            REG_ADDR8(0x552)
#define reg_aes_key3            REG_ADDR8(0x553)
#define reg_aes_key4            REG_ADDR8(0x554)
#define reg_aes_key5            REG_ADDR8(0x555)
#define reg_aes_key6            REG_ADDR8(0x556)
#define reg_aes_key7            REG_ADDR8(0x557)
#define reg_aes_key8            REG_ADDR8(0x558)
#define reg_aes_key9            REG_ADDR8(0x559)
#define reg_aes_key10            REG_ADDR8(0x55a)
#define reg_aes_key11            REG_ADDR8(0x55b)
#define reg_aes_key12            REG_ADDR8(0x55c)
#define reg_aes_key13            REG_ADDR8(0x55d)
#define reg_aes_key14            REG_ADDR8(0x55e)
#define reg_aes_key15            REG_ADDR8(0x55f)
/****************************************************
 audio regs struct: begin  addr : 0x560
 *****************************************************/
#define reg_aud_ctrl			REG_ADDR8(0x560)
enum{
	FLD_AUD_ENABLE	 =			BIT(0),
	FLD_AUD_SDM_PLAY_EN = 		BIT(1),
	FLD_AUD_SHAPPING_EN =		BIT(2),
};

#define reg_aud_vol_ctrl		REG_ADDR8(0x561)
#define reg_aud_pn1				REG_ADDR8(0x562)
#define reg_aud_pn2				REG_ADDR8(0x563)

#define reg_ascl_step			REG_ADDR16(0x564)
#define reg_aud_base_adr		REG_ADDR16(0x568)
#define reg_aud_buff_size		REG_ADDR8(0x56a)
#define reg_aud_rptr			REG_ADDR16(0x56c)
#define reg_sdm_ptr				reg_aud_rptr

#define	AUD_SDM_STEP(i,o)		(0x8000*(i/100)/(o/100))

static inline unsigned short get_sdm_rd_ptr (void) {
	return reg_aud_rptr << 0;
}

enum {
	FLD_AUD_PN_1BIT		= 0x104050,
	FLD_AUD_PN_4BITS	= 0x0c404c,
};

#define reg_aud_const			REG_ADDR32(0x56)


#define reg_aud_filter			REG_ADDR16(0xb80)
enum {
	FLD_AUD_FLT_IBFT	=	BIT(0),
	FLD_AUD_FLT_AUTO	=	BIT(1),
	FLD_AUD_FLT_FIFO0	=	BIT(2),
	FLD_AUD_FLT_FIFO1	=	BIT(3),
	FLD_AUD_FLT_BYPASS0	=	BIT(5),
	FLD_AUD_FLT_BYPASS1	=	BIT(6),
	FLD_AUD_FLT_BYPASS		=   0x60,
	FLD_AUD_FLT_I0			=   0x47,
	FLD_AUD_FLT_I1			=   0x2b,
	FLD_AUD_FLT_I0I1		=   0x0f,
	FLD_AUD_FLT_CFGHIGH		= 	0x1000,
};
#define reg_aud_sram			REG_ADDR8(0xb81)



/****************************************************
 gpio regs struct: begin  0x580
 *****************************************************/

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
#define reg_gpio_pc_ie			REG_ADDR8(0x591)
#define reg_gpio_pc_oen			REG_ADDR8(0x592)
#define reg_gpio_pc_out			REG_ADDR8(0x593)
#define reg_gpio_pc_pol			REG_ADDR8(0x594)
#define reg_gpio_pc_ds			REG_ADDR8(0x595)
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

#define reg_gpio_pf_in			REG_ADDR8(0x5a8)
#define reg_gpio_pf_ie			REG_ADDR8(0x5a9)
#define reg_gpio_pf_oen			REG_ADDR8(0x5aa)
#define reg_gpio_pf_out			REG_ADDR8(0x5ab)
#define reg_gpio_pf_pol			REG_ADDR8(0x5ac)
#define reg_gpio_pf_ds			REG_ADDR8(0x5ad)
#define reg_gpio_pf_gpio		REG_ADDR8(0x5ae)
#define reg_gpio_pf_irq_en		REG_ADDR8(0x5af)

#define reg_gpio_pa_setting1	REG_ADDR32(0x580)
#define reg_gpio_pa_setting2	REG_ADDR32(0x584)
#define reg_gpio_pb_setting1	REG_ADDR32(0x588)
#define reg_gpio_pb_setting2	REG_ADDR32(0x58c)
#define reg_gpio_pc_setting1	REG_ADDR32(0x590)
#define reg_gpio_pc_setting2	REG_ADDR32(0x594)
#define reg_gpio_pd_setting1	REG_ADDR32(0x598)
#define reg_gpio_pd_setting2	REG_ADDR32(0x59c)
#define reg_gpio_pe_setting1	REG_ADDR32(0x5a0)
#define reg_gpio_pe_setting2	REG_ADDR32(0x5a4)
#define reg_gpio_pf_setting1	REG_ADDR32(0x5a8)
#define reg_gpio_pf_setting2	REG_ADDR32(0x5ac)

#define reg_gpio_ctrl			REG_ADDR32(0x5a4)
enum{
	GPIO_WAKEUP_EN = BIT(0),
	GPIO_IRQ_EN	   = BIT(1),
	I2S_SLAVE_EN   = BIT(2),
	RMII_REFCLK_OUTPUT_EN = BIT(3),
};

#define reg_gpio_config_func REG_ADDR32(0x5b0)
#define reg_gpio_config_func0 REG_ADDR8(0x5b0)
enum{
	FLD_I2S_REFCLK_DMIC	=	BIT(0),
	FLD_I2S_BCK_BB_PEAK	=	BIT(1),
	FLD_I2S_BCK_PWM1	=	BIT(2),
	FLD_I2S_LCK_UART_RX	=	BIT(3),
	FLD_I2S_LCK_PWM2	=	BIT(4),
	FLD_I2S_DO_UART_TX	=	BIT(5),
	FLD_I2S_DO_PWM3		=	BIT(6),
	FLD_I2S_DI_DMIC		=	BIT(7),
};
#define reg_gpio_config_func1 REG_ADDR8(0x5b1)
enum{
	FLD_RP_TX_CYC1		=	BIT(0),
	FLD_RN_BB_RSSI		=	BIT(1),
	FLD_GP6_BB_SS2		=	BIT(2),
	FLD_GP7_RXADC_CLK	=	BIT(3),
	FLD_RP_T0			=	BIT(4),
	FLD_RN_T1			=	BIT(5),
	FLD_GP6_TE			=	BIT(6),
	FLD_GP7_MDC			=	BIT(7),
};
#define reg_gpio_config_func2 REG_ADDR8(0x5b2)
enum{
	FLD_GP8_RXADC_DAT	=	BIT(0),
	FLD_GP9_BB_SS1		=	BIT(1),
	FLD_GP10_BBSS0		=	BIT(2),
	FLD_SWS_BB_GAIN4	=	BIT(3),
	FLD_DMIC_CK_BBCLK_BB	=	BIT(4),
	FLD_DMIC_CK_REFCLK	=	BIT(5),
	FLD_UART_TX			=	BIT(6),
	FLD_UART_RX			=	BIT(7),
};
#define reg_gpio_config_func3 REG_ADDR8(0x5b3)
enum{
	FLD_CN_BB_GAIN3		=	BIT(0),
	FLD_CK_BB_GAIN2		=	BIT(1),
	FLD_DO_BB_GAIN1		=	BIT(2),
	FLD_DI_BB_GAIN0		=	BIT(3),
//	FLD_I2S_LCK_PWM2	=	BIT(4),//NOT SURE
	FLD_I2S_DO_RXDV		=	BIT(5),
	FLD_I2S_DI_RXER		=	BIT(6),
	FLD_I2S_DI_TXSD		=	BIT(7),
};
#define reg_gpio_config_func4 REG_ADDR8(0x5b4)
enum{
	FLD_DMIC_CK_RX_CLK	=	BIT(0),
	FLD_I2S_DI_RX_DAT	=	BIT(1),
};

#define reg_gpio_wakeup_irq		REG_ADDR8(0x5b5)
enum{
	FLD_GPIO_WAKEUP_EN		=	BIT(2),
	FLD_GPIO_INTERRUPT_EN	=	BIT(3),
};
/****************************************************
 timer regs struct: begin  0x620
 *****************************************************/
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
#define WATCHDOG_TIMEOUT_COEFF	18		//  check register definition, 0x622

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

/****************************************************
 interrupt regs struct: begin  0x640
 *****************************************************/
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
	FLD_IRQ_HOST_CMD_EN =		BIT(7),

	FLD_IRQ_EP0_SETUP_EN =		BIT(8),
	FLD_IRQ_EP0_DAT_EN =		BIT(9),
	FLD_IRQ_EP0_STA_EN =		BIT(10),
	FLD_IRQ_SET_INTF_EN =		BIT(11),
	FLD_IRQ_EP_DATA_EN =		BIT(12),
	FLD_IRQ_IRQ4_EN =			BIT(12),
	FLD_IRQ_ZB_RT_EN =			BIT(13),
	FLD_IRQ_SW_EN =				BIT(14),
	FLD_IRQ_AN_EN =				BIT(15),

	FLD_IRQ_USB_250US_EN =		BIT(16),
	FLD_IRQ_USB_RST_EN =		BIT(17),
	FLD_IRQ_GPIO_EN =			BIT(18),
	FLD_IRQ_PM_EN =				BIT(19),
	FLD_IRQ_SYSTEM_TIMER =		BIT(20),
	FLD_IRQ_GPIO_RISC0_EN =		BIT(21),
	FLD_IRQ_GPIO_RISC1_EN =		BIT(22),
	FLD_IRQ_GPIO_RISC2_EN = 	BIT(23),

	FLD_IRQ_EN =				BIT_RNG(24,31),
};
#define reg_irq_en				REG_ADDR8(0x643)

#define reg_system_tick			REG_ADDR32(0x740)
#define reg_system_tick_irq		REG_ADDR32(0x744)
#define reg_system_wakeup_tick	REG_ADDR32(0x748)
#define reg_system_tick_mode	REG_ADDR8(0x74c)
#define reg_system_tick_ctrl	REG_ADDR8(0x74f)

enum {
	FLD_SYSTEM_TICK_START	=		BIT(0),
	FLD_SYSTEM_TICK_STOP	=		BIT(1),
	FLD_SYSTEM_TICK_RUNNING	=		BIT(1),

	FLD_SYSTEM_TICK_IRQ_EN  = 		BIT(1),
};

/****************************************************
 PWM regs define:  begin  0x780
 *****************************************************/
#define reg_pwm_enable			REG_ADDR8(0x780)
#define reg_pwm_clk				REG_ADDR8(0x781)
#define reg_pwm_mode			REG_ADDR8(0x782)
#define reg_pwm_invert			REG_ADDR8(0x783)
#define reg_pwm_n_invert		REG_ADDR8(0x784)
#define reg_pwm_pol				REG_ADDR8(0x785)

#define reg_pwm_phase(i)		REG_ADDR16(0x788 + (i << 1))
#define reg_pwm_cycle(i)		REG_ADDR32(0x794 + (i << 2))
#define reg_pwm_cmp(i)			REG_ADDR16(0x794 + (i << 2))
enum{
	FLD_PWM_CMP  = 				BIT_RNG(0,15),
	FLD_PWM_MAX  = 				BIT_RNG(16,31),
};

#define reg_pwm_pulse_num(i)	REG_ADDR16(0x7ac + (i << 1))	// i == 0, 1
#define reg_pwm_irq_mask		REG_ADDR8(0x7b0)
#define reg_pwm_irq_sta			REG_ADDR8(0x7b1)

#define reg_pwm_pnum0			REG_ADDR16(0x80c)
#define reg_pwm_pnum1			REG_ADDR16(0x80e)

//////////////////////////////////////////////////////////////
// DFIFO
//////////////////////////////////////////////////////////////

#define reg_fifo0_data			REG_ADDR32(0x800)
#define reg_fifo1_data			REG_ADDR32(0x900)
#define reg_fifo2_data			REG_ADDR32(0xa00)

/****************************************************
 dfifo regs define:  begin  0xb00
 *****************************************************/
#define reg_dfifo0_buf			REG_ADDR32(0xb00)
#define reg_dfifo1_buf			REG_ADDR32(0xb04)
#define reg_dfifo2_buf			REG_ADDR32(0xb08)
enum{
	FLD_DFIFO_BUF_ADDR =		BIT_RNG(0,15),
	FLD_DFIFO_BUF_SIZE =		BIT_RNG(16,23),
};

#define reg_dfifo0_addr			REG_ADDR16(0xb00)
#define reg_dfifo0_size			REG_ADDR8(0xb02)

#define reg_dfifo_ana_in		REG_ADDR8(0xb03)
enum{
	FLD_DFIFO_MIC0_RISING_EDGE = BIT(0),
	FLD_DFIFO_MIC_ADC_IN 	= BIT(1),

	FLD_DFIFO_AUD_INPUT_MONO =	BIT(4) | BIT(5),
//	FLD_DFIFO_AUD_INPUT_BYPASS = BIT(5),
};
enum{
	REG_AUD_INPUT_SEL_USB = 0,
	REG_AUD_INPUT_SEL_I2S = 1,
	REG_AUD_INPUT_SEL_ADC = 2,
	REG_AUD_INPUT_SEL_DMIC = 3,
};

#define reg_dfifo_scale			REG_ADDR8(0xb04)
enum{
	FLD_DFIFO2_DEC_CIC =		BIT_RNG(0,3),
	FLD_DFIFO0_DEC_SCALE =		BIT_RNG(4,7),
};

#define reg_aud_hpf_alc			REG_ADDR8(0xb05)
enum {
	FLD_AUD_IN_HPF_SFT	=	BIT_RNG(0,3),
	FLD_AUD_IN_HPF_BYPASS	=	BIT(4),
	FLD_AUD_IN_ALC_BYPASS	=	BIT(5),
	FLD_AUD_IN_USB_SET		=  0x3b,
};

#define reg_aud_alc_vol			REG_ADDR8(0xb06)

#define reg_audio_wr_ptr		REG_ADDR16(0xb10)
//#define reg_mic_ptr				reg_audio_wr_ptr

static inline unsigned short get_mic_wr_ptr (void) {
	return reg_audio_wr_ptr << 0;
}


/////////////////////////////////////////////////////////////////////////////
#define rega_xtal_ctrl			0x01
enum {
	FLDA_XTAL_CS =				BIT_RNG(0,4),
	FLDA_LDO_TRIM_3V = 			BIT_RNG(5,6),
	FLDA_XTAL_COM_REF = 		BIT(7),
};

enum{
	ANALOG_LDO_VOL_1P6 = 0, 	// 1.6v
	ANALOG_LDO_VOL_1P8 = 2,		// 1.8v
};

#define rega_vol_ldo_ctrl		0x02
enum {
	FLDA_VOL_LDO_RESV = 		BIT_RNG(0,2),
	FLDA_VOL_LDO_OUTPUT = 		BIT_RNG(3,4),
	FLDA_VOL_BAND_GAP = 		BIT_RNG(5,7),
};

#define rega_pwdn_setting1		0x05
enum {
	FLDA_32K_RC_PWDN = 			BIT(0),
	FLDA_32K_XTAL_PWDN =		BIT(1),
	FLDA_32M_RC_PWDN = 			BIT(2),
	FLDA_32M_XTAL_PWDN = 		BIT(3),
	FLDA_LDO_PWDN = 			BIT(4),
	FLDA_BGIREF_3V_PWDN = 		BIT(5),
	FLDA_COMP_PWDN = 			BIT(6),
	FLDA_TEMPSEN_PWDN = 		BIT(7),
};

#define rega_pwdn_ldo			0x06
enum {
	FLDA_RX_LO_LDO_PWDN = 		BIT(0),
	FLDA_BG_I_EXT_PWDN =		BIT(1),
	FLDA_RX_ANA_LDO_PWDN = 		BIT(2),
	FLDA_RX_RF_LDO_PWDN = 		BIT(3),
	FLDA_PLL_BG_PWDN = 			BIT(4),
	FLDA_PLL_DS_LDO_PWDN = 		BIT(5),
	FLDA_PLL_LDO_PWDN = 		BIT(6),
	FLDA_PLL_LDO2_PWDN = 		BIT(7),
};

#define rega_ldo_setting1		0x07
enum {
	FLDA_RX_ANA_LDO_O_CTRL = 	BIT_RNG(0,1),
	FLDA_RX_RF_LDO_O_CTRL = 	BIT_RNG(2,3),
	FLDA_PLL_LDO_B2 = 			BIT_RNG(4,5),
	FLDA_PLL_LDO_DS = 			BIT_RNG(6,7),
};

#define rega_ldo_setting2		0x0c
enum {
	FLDA_RX_LOLDO_OUTCTRL = 	BIT_RNG(0,1),
	FLDA_LDO_ANA_LDOTRIM = 		BIT(2),
	FLDA_XTAL_LDO_TRIM = 		BIT(3),
	FLDA_PULLDN_LOLDO =			BIT(4),
	FLDA_PULLDN_DIGLDO = 		BIT(5),	// digit  LDO
	FLDA_LDO_SETTING2_RSV =		BIT_RNG(6,7),
};

#define rega_32k_tm_thre0		0x10
#define rega_32k_tm_thre1		0x11
#define rega_32k_tm_thre2		0x12
#define rega_32k_tm_thre3		0x13
enum {
	FLDA_32K_THR3_H = 			BIT_RNG(0,1),		// conflict with rega_32k_tm_val3 in document !!!
	FLDA_32K_CONTINUE_MODE =	BIT(2),				// 0: one shot,  1: continuing
	FLDA_32K_TIMER_EN =			BIT(3),
	FLDA_DELAY_AFTER_WAKEUP =	BIT_RNG(4,7),
};
#define rega_delay_after_wakeup	0x14

#define rega_wakeup_pin_l		0x15
#define rega_wakeup_pin_h		0x16
#define rega_pm_ctrl			0x17
enum {
	FLDA_PM_WAKEUP_FROM_DIG = 	BIT(0),				// voltage
	FLDA_PM_WAKEUP_32K_SEL = 	BIT(1),				// conflict with the document !!!
	FLDA_PM_RESV0 = 			BIT_RNG(2,3),
	FLDA_PM_WAKEUP_POL = 		BIT(4),
	FLDA_PM_RESV1 =				BIT(5),
	FLDA_PM_PWDN_LDO = 			BIT(6),
	FLDA_PM_PWDN_EN = 			BIT(7),
};

#define rega_pwdn_setting2		0x18
enum {
	FLDA_PWDN_OSC_32K_EN = 		BIT(0),
#if(MCU_CORE_TYPE == MCU_CORE_5320)
	FLDA_PWDN_PAD_32K_EN =		BIT(1),
#else
	FLDA_RESET_32K_TIMER =		BIT(1),
#endif
	FLDA_PWDN_TX = 				BIT(2),
	FLDA_PWDN_RX = 				BIT(3),
	FLDA_PWDN_SAR_ADC = 		BIT(4),
	FLDA_PWDN_LOW_LEAK_LDO = 	BIT(5),
	FLDA_PWDN_REG456_EN = 		BIT(6),
	FLDA_PWDN_ISO_EN = 			BIT(7),
};

#define rega_32k_tm_val0		0x20
#define rega_32k_tm_val1		0x21
#define rega_32k_tm_val2		0x22
#define rega_32k_tm_val3		0x23
enum{
	FLDA_32K_CNT_CONTINUOUS = BIT(3),
	FLDA_32K_RST_TIMER = BIT(4),
	FLDA_32K_TRIGGER_TIMER = BIT(7),
};

enum{
	FLDA_WKUP_POL_PAD0 = BIT(0),
	FLDA_WKUP_POL_PAD1 = BIT(1),
	FLDA_WKUP_POL_PAD2 = BIT(2),
	FLDA_WKUP_POL_PAD3 = BIT(3),
	FLDA_WKUP_POL_PAD4 = BIT(4),
	FLDA_WKUP_POL_PAD5 = BIT(5),
};

// from 0x2c, 0x2d, 0x2e, 0x2f, 0x30, fro customer use
#define rega_pkt_seq_no			0x2d
#define rega_deepsleep_rf_chn	0x2e
//#define rega_deepsleep_flag		0x2f

#define rega_xtal_csel			0x81
enum {
	FLDA_XTAL_FREQ_OFF = 		BIT_RNG(0, 4),
	FLDA_XTAL_PWDN = 			BIT(5),
	FLDA_XTAL_VOLT = 			BIT_RNG(6,7),
};

#define rega_bp_pll				0x84
enum {
	FLDA_BP_PLL_CP_PWDN = 		BIT(0),
	FLDA_BP_PLL_3ST_VCO_PWDN = 	BIT(1),
	FLDA_BP_PLL_4ST_VCO_PWDN = 	BIT(2),
	FLDA_BP_PLL_VCO_SEL = 		BIT(3),
	FLDA_BP_PLL_R_DIV = 		BIT_RNG(6,7),
};

#define rega_aud_ctrl			0x86
enum {
	FLDA_AUD_PWDN_LEFT = 		BIT(0),
	FLDA_AUD_PWDN_RIGHT = 		BIT(1),
	FLDA_AUD_MUTE_RIGHT = 		BIT(2),
	FLDA_AUD_MUTE_LEFT = 		BIT(3),
	FLDA_AUD_PRE_GAIN_RIGHT = 	BIT_RNG(4,5),
	FLDA_AUD_PRE_GAIN_LEFT = 	BIT_RNG(6,7),
};

#define rega_aud_ctrl2			0x87
enum {
	FLDA_AUD_PST_GAIN_RIGHT = 	BIT_RNG(0,3),
	FLDA_AUD_PST_GAIN_LEFT = 	BIT_RNG(4,7),
};

#define rega_dcdc_ctrl			0x88
enum {
	FLDA_DCDC_RESV = 			BIT(0),
	FLDA_RNS_PWDN = 			BIT(1),
	FLDA_DCDC_RESETB = 			BIT(2),
	FLDA_DCDC_6P75_PWDN = 		BIT(3),
	FLDA_DCDC_IN_2P8_PS = 		BIT(4),
	FLDA_DCDC_OUT_2P8_SEL = 	BIT(5),
	FLDA_DCDC_OUT_6P6_SEL = 	BIT(6),
	FLDA_DCDC_2P8_PWDN = 		BIT(7),
};

#define rega_dig_ldo_sel_atb	0x89
enum {
	FLDA_LDO_SEL_ATB = 			BIT_RNG(0,2),
	FLDA_PWDN_CLK_BB = 			BIT(3),
	FLDA_PWDN_CLK_RF = 			BIT(4),
	FLDA_PWDN_BUF_RCCAL = 		BIT(5),
	FLDA_PWDN_BUF_DIG = 		BIT(6),
};
#define rega_adc_setting1		0x8a
#define rega_adc_setting2		0x8b
enum{
	FLDA_RX_ADC_COMPLEX_EN = 	BIT(0),
	FLDA_RX_ADC_CTUNE = 		BIT_RNG(1,4),
	FLDA_RX_ADC_DLY_CTRL =		BIT_RNG(5,6),
	FLDA_RX_ADC_PWDN = 			BIT(7),
};

#define rega_rx_lna				0x91
enum{
	FLDA_RX_LNA_OUT_TUNE = 		BIT_RNG(0,2),
	FLDA_RX_LNA_BIAS =			BIT_RNG(3,5),
	FLDA_RX_LNA_ON = 			BIT(6),
	FLDA_PA_TX_EXT_C1_EN = 		BIT(7),
};

#define rega_rx_pga				0x94
enum{
	FLDA_RX_PGA_IQ_SWAP_EN = 	BIT(0),
	FLDA_RX_PGA_BW_TUNE = 		BIT(1),
	FLDA_RX_PGA_CAP_CTRL = 		BIT_RNG(2,5),
	FLDA_RX_PGA_COMP_EN = 		BIT(6),
	FLDA_RX_PGA_PWDN = 			BIT(7),
};
#define rega_rx_test			0x99
enum{
	FLDA_RF_PEAK_DET_DLY1_EN = 	BIT(0),
	FLDA_RF_PEAK_DET_DLY2_EN =	BIT(1),
	FLDA_RF_PEAK_DET_DLY3_EN = 	BIT(2),
	FLDA_TEST_PGA_OUT_EN = 		BIT(3),
	FLDA_TEST_PGA_IN_EN = 		BIT(4),
	FLDA_TEST_IQ_MIXER_EN = 	BIT(5),
	FLDA_RF_PEAK_DET1_EN = 		BIT(6),
	FLDA_RF_PEAK_DET2_EN = 		BIT(7),
};

#define rega_tx_pa_ctrl			0x9e
enum{
	FLDA_PA_TX_BIAS = 			BIT_RNG(0,4),
	FLDA_PA_TX_UNIT = 			BIT_RNG(3,5),
};
#define rega_tx_pa_ctrl2		0x9f

#define rega_pfd				0xa5
enum{
	FLDA_PFD_MUX_L = 			BIT_RNG(0,2),
	FLDA_PFD_NP_INTG = 			BIT(3),
	FLDA_INTG_PFD_S0 = 			BIT(4),
	FLDA_INTG_PFD_S1 = 			BIT(5),
	FLDA_REFBUF_PWDN = 			BIT(6),
	FLDA_REG_DC_MOD0 = 			BIT(7),
};

#define rega_intg_n0			0xa0
enum{
	FLDA_DPLL_EN = 				BIT(0),
	FLDA_IDIV2_UP_RX = 			BIT(1),
	FLDA_IDIV2_UP_TX =			BIT(2),
	FLDA_RX_INTG_N0 = 			BIT_RNG(3,7),
};

#define rega_intg_n1			0xa1
enum{
	FLDA_RX_INTG_N1 = 			BIT_RNG(0,4),
	FLDA_RX_IB1 = 				BIT_RNG(5,6),
	FLDA_LPF_PWDN = 			BIT(7),
};

#define rega_intg_rdiv			0xa2
enum{
	FLDA_INTG_RDIV = 			BIT_RNG(0,5),
	FLDA_MASH2_EN = 			BIT(6),
	FLDA_NDIV_EN = 				BIT(7),
};

#define rega_reg_dc_mod1		0xa6
#define rega_reg_dc_mod2		0xa7
#define rega_vco_cal_comp		0xa8
enum{
	FLDA_RF_TEST_PWDN = 		BIT(0),
	FLDA_RX_IB = 				BIT_RNG(1,2),
	FLDA_VCO_CAL_COMP = 		BIT_RNG(3,7),
};

#define rega_vco_setting		0xa9
enum{
	FLDA_TX_IB = 				BIT_RNG(0,1),
	FLDA_VCO_RXI = 				BIT_RNG(2,4),
	FLDA_VCO_TXI = 				BIT_RNG(5,7),
};

#define rega_intg_rxcap			0xaa
enum{
	FLDA_PWDN_INTG_PLL = 		BIT(0),
	FLDA_INTG_RXCAP = 			BIT_RNG(1,7),
};

#define rega_intg_txcap			0xab
#define rega_pll_dpll			0xac
#define rega_pll_dpll_setting	0xad
enum{
	FLDA_PLL_DPLL = 			BIT_RNG(0,1),
	FLDA_PLL_DPLL_ADJ = 		BIT_RNG(2,4),
	FLDA_PLL_DPLL_REF = 		BIT_RNG(5,6),
	FLDA_TXCALDIV2BUF_PWDN = 	BIT(7),

};

#define rega_pll_dpll_wait		0xae
enum{
	FLDA_PLL_DPLL_WAIT = 		BIT_RNG(0,5),
	FLDA_RX_MAN_EN =			BIT(6),
	FLDA_TX_MAN_EN =			BIT(7),
};

///////////////////// PM register /////////////////////////
#define		rega_wakeup_en_val0		0x41
#define		rega_wakeup_en_val1		0x42
#define		rega_wakeup_en_val2		0x43
#define		raga_gpio_wkup_pol		0x44

#define		raga_pga_gain0			0x86
#define		raga_pga_gain1			0x87




#if defined(__cplusplus)
}
#endif

