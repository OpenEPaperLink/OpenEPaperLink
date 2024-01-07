/********************************************************************************************************
 * @file    bsp.h
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

#ifndef BSP_H
#define BSP_H


/****************************************************
 	 	 	 	 Reg operations
 *****************************************************/
#define REG_BASE_ADDR				0x800000

#define REG_ADDR8(a)				(*(volatile unsigned char  *)(REG_BASE_ADDR + (a)))
#define REG_ADDR16(a)				(*(volatile unsigned short *)(REG_BASE_ADDR + (a)))
#define REG_ADDR32(a)				(*(volatile unsigned long  *)(REG_BASE_ADDR + (a)))

#define WRITE_REG8(addr,v)          (*(volatile unsigned char  *)(REG_BASE_ADDR + (addr)) = (unsigned char)(v))
#define WRITE_REG16(addr,v)         (*(volatile unsigned short *)(REG_BASE_ADDR + (addr)) = (unsigned short)(v))
#define WRITE_REG32(addr,v)         (*(volatile unsigned long  *)(REG_BASE_ADDR + (addr)) = (v))

#define READ_REG8(addr)             (*(volatile unsigned char  *)(REG_BASE_ADDR + (addr)))
#define READ_REG16(addr)            (*(volatile unsigned short *)(REG_BASE_ADDR + (addr)))
#define READ_REG32(addr)            (*(volatile unsigned long  *)(REG_BASE_ADDR + (addr)))

#define write_reg8					WRITE_REG8
#define write_reg16					WRITE_REG16
#define write_reg32					WRITE_REG32

#define read_reg8					READ_REG8
#define read_reg16					READ_REG16
#define read_reg32					READ_REG32


#define TCMD_UNDER_RD				0x80
#define TCMD_UNDER_WR				0x40
#define TCMD_UNDER_BOTH				0xc0

#define TCMD_MASK					0x3f

#define TCMD_WRITE					0x3
#define TCMD_WAIT					0x7
#define TCMD_WAREG					0x8


typedef struct {
	unsigned char 	 rst0;
	unsigned char    rst1;
	unsigned char    rst2;
	unsigned char    clk0;
	unsigned char 	 clk1;
	unsigned char    clk2;
	unsigned char    clkctl;
} BSP_SysCtlTypeDef;


typedef enum {
    SYS_CLK_RC = 0,
    SYS_CLK_HS_DIV = 1,
} BSP_SysClkSelTypeDef;


typedef struct TBLCMDSET {
	unsigned short	adr;
	unsigned char	dat;
	unsigned char	cmd;
} TBLCMDSET;



static inline void mcu_reset(void){
	write_reg8(0x06f, 0x20);
}

int LoadTblCmdSet(const TBLCMDSET *pt, int size);

unsigned int rand(void);

unsigned char analog_read(unsigned char addr);
void analog_write(unsigned char addr, unsigned char v);
void analog_read_multi(unsigned char addr, unsigned char *v, int len);
void analog_write_multi(unsigned char addr, unsigned char *v, int len);

#define WriteAnalogReg(addr,v)	analog_write(addr,v)
#define ReadAnalogReg(addr)		analog_read(addr)

#endif /* BSP_H */
