/********************************************************************************************************
 * @file	soc.h
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
#pragma once
#include "bit.h"

/*Operation register, the highest bit must be set to 1 (|0x80000000),marked by minghai,confirmed by jianzhi.20210610.*/
#define FLASH_R_BASE_ADDR   		0x20000000
#define REG_RW_BASE_ADDR  			0x80000000
#define REG_ADDR8(a)				(*(volatile unsigned char*)(REG_RW_BASE_ADDR | (a)))
#define REG_ADDR16(a)				(*(volatile unsigned short*)(REG_RW_BASE_ADDR | (a)))
#define REG_ADDR32(a)				(*(volatile unsigned long*)(REG_RW_BASE_ADDR | (a)))

#define write_reg8(addr,v)			(*(volatile unsigned char*)(REG_RW_BASE_ADDR | (addr)) = (unsigned char)(v))
#define write_reg16(addr,v)			(*(volatile unsigned short*)(REG_RW_BASE_ADDR | (addr)) = (unsigned short)(v))
#define write_reg32(addr,v)			(*(volatile unsigned long*)(REG_RW_BASE_ADDR | (addr)) = (unsigned long)(v))

#define read_reg8(addr)				(*(volatile unsigned char*)(REG_RW_BASE_ADDR | (addr)))
#define read_reg16(addr)            (*(volatile unsigned short*)(REG_RW_BASE_ADDR | (addr)))
#define read_reg32(addr)            (*(volatile unsigned long*)(REG_RW_BASE_ADDR | (addr)))

#define write_sram8(addr,v)			(*(volatile unsigned char*)( (addr)) = (unsigned char)(v))
#define write_sram16(addr,v)		(*(volatile unsigned short*)( (addr)) = (unsigned short)(v))
#define write_sram32(addr,v)		(*(volatile unsigned long*)( (addr)) = (unsigned long)(v))

#define read_sram8(addr)			(*(volatile unsigned char*)((addr)))
#define read_sram16(addr)           (*(volatile unsigned short*)((addr)))
#define read_sram32(addr)           (*(volatile unsigned long*)((addr)))
#define TCMD_UNDER_BOTH				0xc0
#define TCMD_UNDER_RD				0x80
#define TCMD_UNDER_WR				0x40

#define TCMD_MASK					0x3f

#define TCMD_WRITE					0x3
#define TCMD_WAIT					0x7
#define TCMD_WAREG					0x8
//#if 1 //optimize
/*
 * IRAM area:0x00000~0x1FFFF BIT(19) is 0,BIT(16~0) 128K is address offset
 * DRAM area:0x80000~0x9FFFF BIT(19) is 1,BIT(16~0) 128K is address offset
 * ILM area:0xc0000000~0xc0020000 BIT(31~19) is 3,BIT(21) is 0, BIT(20~17) do not care  BIT(16~0) 128K is address offset 128K is address offset
 * DLM area:0xc0200000~0xc0220000 BIT(31~19) is 3,BIT(21) is 1, BIT(20~17) do not care  BIT(16~0) 128K is address offset 128K is address offset
 * BIT(19) is used to distinguish from IRAM to DRAM, BIT(21) is used to distinguish from ILM to DLM.
 * so we can write it as follow
 * #define  convert_ram_addr_cpu2bus  (((((addr))&0x80000)? ((addr)| 0xc0200000) : ((addr)|0xc0000000)))
 * BIT(20~17) are invalid address line ,IRAM address is less than 0x80000, (address-0x80000)must borrow from BIT(21)
 *   #define convert(addr) ((addr)-0x80000+0xc0200000)
 *  to simplify
 *  #define convert(addr) ((addr)+0xc0180000)
 * */
//#define convert_ram_addr_cpu2bus(addr)  ((unsigned int)(addr)+0xc0180000)
//#else  //no optimize
//#define  convert_ram_addr_cpu2bus  (((((unsigned int)(addr)) >=0x80000)?(((unsigned int)(addr))-0x80000+0xc0200000) : (((unsigned int)(addr)) + 0xc0000000)))
//#endif

// go further, if the source and destination  address is not in the sram(IRAM/DRAM)  interval, no address translation
#define convert_ram_addr_cpu2bus(addr)  (unsigned int)(addr) < 0xa0000 ? ((unsigned int)(addr)+0xc0180000): (unsigned int)(addr)

#define convert_ram_addr_bus2cpu(addr)  (((((unsigned int)(addr)) >=0xc0200000)?(((unsigned int)(addr)) + 0x80000-0xc0200000) : (((unsigned int)(addr)) - 0xc0000000)))


#define LM_BASE                        0x80000000

#define ILM_BASE                       (LM_BASE + 0x40000000)
#define DLM_BASE                       (LM_BASE + 0x40200000)
#define CPU_ILM_BASE                   (0x00000000)
#define CPU_DLM_BASE                   (0x00080000)

#define   SC_BASE_ADDR			        0x1401c0
/*******************************      reset registers: 1401e0      ******************************/
#define reg_rst						REG_ADDR32(0x1401e0)

#define reg_rst0					REG_ADDR8(0x1401e0)
enum{
	FLD_RST0_HSPI 			= 	BIT(0),
	FLD_RST0_I2C 			= 	BIT(1),
	FLD_RST0_UART0 			= 	BIT(2),
	FLD_RST0_USB 			= 	BIT(3),
	FLD_RST0_PWM 			= 	BIT(4),
	//RSVD
	FLD_RST0_UART1 			= 	BIT(6),
	FLD_RST0_SWIRE 			= 	BIT(7),
};


#define reg_rst1					REG_ADDR8(0x1401e1)
enum{
	//RSVD
	FLD_RST1_SYS_STIMER 	= 	BIT(1),
	FLD_RST1_DMA 			=	BIT(2),
	FLD_RST1_ALGM 			= 	BIT(3),
	FLD_RST1_PKE 			= 	BIT(4),
	//RSVD
	FLD_RST1_PSPI 			=	BIT(6),
	FLD_RST1_SPISLV 		=	BIT(7),
};

#define reg_rst2					REG_ADDR8(0x1401e2)
enum{
	FLD_RST2_TIMER 			=	BIT(0),
	FLD_RST2_AUD 			=	BIT(1),
	FLD_RST2_TRNG 			=	BIT(2),
	//RSVD
	FLD_RST2_MCU 			=	BIT(4),
	FLD_RST2_LM 			= 	BIT(5),
	FLD_RST2_NPE 			=	BIT(6),
	//RSVD
};

#define reg_rst3					REG_ADDR8(0x1401e3)
enum{
	FLD_RST3_ZB 			=	BIT(0),
	FLD_RST3_MSTCLK 		=	BIT(1),
	FLD_RST3_LPCLK 			=	BIT(2),
	FLD_RST3_ZB_CRYPT 		=	BIT(3),
	FLD_RST3_MSPI 	    	=	BIT(4),
	FLD_RST3_CODEC  		=	BIT(5),
	FLD_RST3_SARADC			= 	BIT(6),
	FLD_RST3_ALG 			=	BIT(7),
};

#define reg_clk_en					REG_ADDR32(0x1401e4)

#define reg_clk_en0					REG_ADDR8(0x1401e4)
enum{
	FLD_CLK0_HSPI_EN 		= 	BIT(0),
	FLD_CLK0_I2C_EN 		= 	BIT(1),
	FLD_CLK0_UART0_EN 		= 	BIT(2),
	FLD_CLK0_USB_EN 		= 	BIT(3),
	FLD_CLK0_PWM_EN 		= 	BIT(4),
	FLD_CLK0_UART1_EN 		= 	BIT(6),
	FLD_CLK0_SWIRE_EN 		= 	BIT(7),
};

#define reg_clk_en1					REG_ADDR8(0x1401e5)
enum{
	FLD_CLK1_SYS_TIMER_EN 	= 	BIT(1),
	FLD_CLK1_ALGM_EN 		= 	BIT(3),
	FLD_CLK1_PKE_EN 		= 	BIT(4),
	FLD_CLK1_MACHINETIME_EN = 	BIT(5),
	FLD_CLK1_PSPI_EN 		= 	BIT(6),
	FLD_CLK1_SPISLV_EN 		= 	BIT(7),

};

#define reg_clk_en2					REG_ADDR8(0x1401e6)
enum{
	FLD_CLK2_TIMER_EN 		= 	BIT(0),
	FLD_CLK2_AUD_EN 		= 	BIT(1),
	FLD_CLK2_TRNG_EN 		= 	BIT(2),
	FLD_CLK2_MCU_EN 		= 	BIT(4),

	FLD_CLK2_NPE_EN 		= 	BIT(6),
	FLD_CLK2_EOC_EN 		= 	BIT(7),
};

#define reg_clk_en3					REG_ADDR8(0x1401e7)
enum{
	FLD_CLK3_ZB_PCLK_EN 	=	BIT(0),
	FLD_CLK3_ZB_MSTCLK_EN 	=	BIT(1),
	FLD_CLK3_ZB_LPCLK_EN 	=	BIT(2),
};

#define reg_clk_sel0				REG_ADDR8(0x1401e8)
enum{
	FLD_CLK_SCLK_DIV 		=	BIT_RNG(0,3),
	FLD_CLK_SCLK_SEL 		=	BIT_RNG(4,6),
	FLD_CLK_MSPI_CLK_SEL	=	BIT(7),
};

#define reg_clk_sel1				REG_ADDR8(0x1401e9)
enum{
	FLD_CLK_MSPI_DIV	 	=	BIT_RNG(4,7),
};

#define reg_i2s_step				REG_ADDR8(SC_BASE_ADDR+0x2a)
enum{
	FLD_I2S_STEP         	=	BIT_RNG(0,6),
	FLD_I2S_CLK_EN         	=	BIT(7),
};

#define reg_i2s_mod				   	REG_ADDR8(SC_BASE_ADDR+0x2b)


#define reg_dmic_step				REG_ADDR8(SC_BASE_ADDR+0x2c)
enum{
	FLD_DMIC_STEP         	=	BIT_RNG(0,6),
	FLD_DMIC_SEL        	=	BIT(7),
};

#define reg_dmic_mod				REG_ADDR8(SC_BASE_ADDR+0x2d)

#define reg_wakeup_en				REG_ADDR8(SC_BASE_ADDR+0x2e)
enum{
	FLD_USB_PWDN_I        	=	BIT(0),
	FLD_GPIO_WAKEUP_I       =	BIT(1),
	FLD_USB_RESUME        	=	BIT(2),
	FLD_STANDBY_EX        	=	BIT(3),
};

#define reg_pwdn_en					REG_ADDR8(SC_BASE_ADDR+0x2f)

#define reg_dmic_clk_set			REG_ADDR8(SC_BASE_ADDR+0x33)


#define reg_wakeup_status			0x64
typedef enum{
	FLD_WKUP_CMP			=	BIT(0),
	FLD_WKUP_TIMER			=	BIT(1),
	FLD_WKUP_DIG			=	BIT(2),
	FLD_WKUP_PAD			=	BIT(3),
	FLD_WKUP_MDEC			=	BIT(4),
	FLD_MDEC_RSVD			=	BIT_RNG(5,7),
}wakeup_status_e;


