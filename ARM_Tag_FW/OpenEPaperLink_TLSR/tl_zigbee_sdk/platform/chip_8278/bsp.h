/********************************************************************************************************
 * @file    bsp.h
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

#ifndef BSP_H_
#define BSP_H_



/**
 *  Reg operations
 */
#define REG_BASE_ADDR			0x800000

#define REG_ADDR8(a)			(*(volatile unsigned char*) (REG_BASE_ADDR + (a)))
#define REG_ADDR16(a)			(*(volatile unsigned short*)(REG_BASE_ADDR + (a)))
#define REG_ADDR32(a)			(*(volatile unsigned long*) (REG_BASE_ADDR + (a)))

#define write_reg8(addr,v)		(*(volatile unsigned char*)  (REG_BASE_ADDR + (addr)) = (unsigned char)(v))
#define write_reg16(addr,v)		(*(volatile unsigned short*) (REG_BASE_ADDR + (addr)) = (unsigned short)(v))
#define write_reg32(addr,v)		(*(volatile unsigned long*)  (REG_BASE_ADDR + (addr)) = (v))

#define read_reg8(addr)			(*(volatile unsigned char*) (REG_BASE_ADDR + (addr)))
#define read_reg16(addr)		(*(volatile unsigned short*)(REG_BASE_ADDR + (addr)))
#define read_reg32(addr)		(*(volatile unsigned long*) (REG_BASE_ADDR + (addr)))

#define WRITE_REG8				write_reg8
#define WRITE_REG16				write_reg16
#define WRITE_REG32				write_reg32

#define READ_REG8				read_reg8
#define READ_REG16				read_reg16
#define READ_REG32				read_reg32

#define TCMD_UNDER_BOTH			0xc0
#define TCMD_UNDER_RD			0x80
#define TCMD_UNDER_WR			0x40

#define TCMD_MASK				0x3f

#define TCMD_WRITE				0x3
#define TCMD_WAIT				0x7
#define TCMD_WAREG				0x8


/**
 *  command table for special registers
 */
typedef struct TBLCMDSET {
	unsigned short	adr;
	unsigned char	dat;
	unsigned char	cmd;
} TBLCMDSET;

/**
 * @brief      This function performs a series of operations of writing digital or analog registers
 *             according to a command table
 * @param[in]  pt - pointer to a command table containing several writing commands
 * @param[in]  size  - number of commands in the table
 * @return     number of commands are carried out
 */

extern int LoadTblCmdSet(const TBLCMDSET * pt, int size);


/**
 * @brief      This function writes a byte data to analog register
 * @param[in]  addr - the address of the analog register needs to write
 * @param[in]  value  - the data will be written to the analog register
 * @param[in]  e - the end address of value
 * @param[in]  s - the start address of the value
 * @return     none
 */
extern void sub_wr_ana(unsigned int addr, unsigned char value, unsigned char e, unsigned char s);

/**
 * @brief      This function writes a byte data to a specified analog register
 * @param[in]  addr - the address of the analog register needs to write
 * @param[in]  value  - the data will be written to the analog register
 * @param[in]  e - the end address of value
 * @param[in]  s - the start address of the value
 * @return     none
 */
extern void sub_wr(unsigned int addr, unsigned char value, unsigned char e, unsigned char s);

/**
 * @brief      This function reboot mcu.
 * @return     none
 */
static inline void mcu_reset(void)
{
	write_reg8(0x06f,0x20);
}


#endif /* BSP_H_ */
