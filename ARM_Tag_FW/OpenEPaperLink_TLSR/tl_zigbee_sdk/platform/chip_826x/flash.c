/********************************************************************************************************
 * @file    flash.c
 *
 * @brief   This is the source file for B86
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

#include "flash.h"
#include "register.h"
#include "clock.h"
#include "irq.h"
#include "string.h"

static inline void mspi_wait(void){
	while(reg_master_spi_ctrl & FLD_MASTER_SPI_BUSY);
}

static inline void mspi_high(void){
	reg_master_spi_ctrl = FLD_MASTER_SPI_CS;
}

static inline void mspi_low(void){
	reg_master_spi_ctrl = 0;
}

static inline unsigned char mspi_get(void){
	return reg_master_spi_data;
}

static inline void mspi_write(unsigned char c){
	reg_master_spi_data = c;
}

static inline void mspi_ctrl_write(unsigned char c){
	reg_master_spi_ctrl = c;
}

static inline unsigned char mspi_read(void){
	mspi_write(0);		// dummy, issue clock
	mspi_wait();
	return mspi_get();
}

_attribute_ram_code_sec_ static inline int flash_is_busy(void)
{
	return mspi_read() & 0x01;				//  the busy bit, pls check flash spec
}

_attribute_ram_code_sec_noinline_ static void flash_send_cmd(unsigned char cmd)
{
	mspi_high();
	sleep_us(1);
	mspi_low();
	mspi_write(cmd);
	mspi_wait();
}

_attribute_ram_code_sec_noinline_ static void flash_send_addr(unsigned long addr)
{
	mspi_write((unsigned char)(addr>>16));
	mspi_wait();
	mspi_write((unsigned char)(addr>>8));
	mspi_wait();
	mspi_write((unsigned char)(addr));
	mspi_wait();
}

//  make this a asynchorous version
_attribute_ram_code_sec_noinline_ static void flash_wait_done(void)
{
	sleep_us(100);
	flash_send_cmd(FLASH_READ_STATUS_CMD_LOWBYTE);

	int i;
	for(i = 0; i < 10000000; ++i){
		if(!flash_is_busy()){
			break;
		}
	}
	mspi_high();
}

_attribute_ram_code_sec_noinline_ void flash_mspi_read_ram(unsigned char cmd, unsigned long addr, unsigned char addr_en, unsigned char dummy_cnt, unsigned char *data, unsigned long data_len)
{
	unsigned char r = irq_disable();

	flash_send_cmd(cmd);
	if(addr_en)
	{
		flash_send_addr(addr);
	}
	for(int i = 0; i < dummy_cnt; ++i)
	{
		mspi_write(0x00);		/* dummy */
		mspi_wait();
	}
	mspi_write(0x00);			/* to issue clock */
	mspi_wait();
	mspi_ctrl_write(0x0a);		/* auto mode */
	mspi_wait();
	for(int i = 0; i < data_len; ++i)
	{
		*data++ = mspi_get();
		mspi_wait();
	}
	mspi_high();

	irq_restore(r);
}

_attribute_ram_code_sec_noinline_ void flash_mspi_write_ram(unsigned char cmd, unsigned long addr, unsigned char addr_en, unsigned char *data, unsigned long data_len)
{
	unsigned char r = irq_disable();

	flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
	flash_send_cmd(cmd);
	if(addr_en)
	{
		flash_send_addr(addr);
	}
	for(int i = 0; i < data_len; ++i)
	{
		mspi_write(data[i]);
		mspi_wait();
	}
	mspi_high();
	flash_wait_done();

	irq_restore(r);
}

void flash_erase_sector(unsigned long addr)
{
	flash_mspi_write_ram(FLASH_SECT_ERASE_CMD, addr, 1, NULL, 0);
}

void flash_read_page(unsigned long addr, unsigned long len, unsigned char *buf)
{
	flash_mspi_read_ram(FLASH_READ_CMD, addr, 1, 0, buf, len);
}

void flash_write_page(unsigned long addr, unsigned long len, unsigned char *buf)
{
	unsigned int ns = PAGE_SIZE - (addr&(PAGE_SIZE - 1));
	int nw = 0;

	do{
		nw = len > ns ? ns :len;
		flash_mspi_write_ram(FLASH_WRITE_CMD, addr, 1, buf, nw);
		ns = PAGE_SIZE;
		addr += nw;
		buf += nw;
		len -= nw;
	}while(len > 0);
}

unsigned char flash_read_status(unsigned char cmd)
{
	unsigned char status = 0;
	flash_mspi_read_ram(cmd, 0, 0, 0, &status, 1);
	return status;
}

void flash_write_status(flash_status_typedef_e type, unsigned short data)
{
	unsigned char buf[2];

	buf[0] = data;
	buf[1] = data>>8;
	if(type == FLASH_TYPE_8BIT_STATUS){
		flash_mspi_write_ram(FLASH_WRITE_STATUS_CMD_LOWBYTE, 0, 0, buf, 1);
	}else if(type == FLASH_TYPE_16BIT_STATUS_ONE_CMD){
		flash_mspi_write_ram(FLASH_WRITE_STATUS_CMD_LOWBYTE, 0, 0, buf, 2);
	}
}

unsigned int flash_read_mid(void)
{
	unsigned int flash_mid = 0;
	flash_mspi_read_ram(FLASH_GET_JEDEC_ID, 0, 0, 0, (unsigned char*)(&flash_mid), 3);
	return flash_mid;
}

void flash_read_uid(unsigned char idcmd, unsigned char *buf)
{
	if(idcmd == FLASH_READ_UID_CMD_GD_PUYA_ZB_UT)	//< GD/PUYA/ZB/UT
	{
		flash_mspi_read_ram(idcmd, 0x00, 1, 1, buf, 16);
	}
	else if (idcmd == FLASH_READ_UID_CMD_XTX)		//< XTX
	{
		flash_mspi_read_ram(idcmd, 0x80, 1, 1, buf, 16);
	}
}

int flash_read_mid_uid_with_check(unsigned int *flash_mid, unsigned char *flash_uid)
{
	unsigned char no_uid[16]={0x51,0x01,0x51,0x01,0x51,0x01,0x51,0x01,0x51,0x01,0x51,0x01,0x51,0x01,0x51,0x01};
	int i,f_cnt=0;
	*flash_mid = flash_read_mid();

	/*
	 * If add flash type, need pay attention to the read uid cmd and the bir number of status register
	   Flash Type	CMD			MID		Company
	   MD25D40C		0x4b	0x134051	GD
	   MD25D40D		0x4b	0x134051	GD
	   GD25D10B		0x4b	0x1140C8	GD
	   GD25D10C		0x4b	0x1140C8	GD
	   ZB25WD40B	0x4b	0x13325E	ZB
	 */

	if((*flash_mid == 0x134051)||(*flash_mid == 0x1140C8)||(*flash_mid == 0x13325E)){
		flash_read_uid(FLASH_READ_UID_CMD_GD_PUYA_ZB_UT, (unsigned char *)flash_uid);
	}else{
		return 0;
	}

	for(i = 0; i < 16; i++){
		if(flash_uid[i] == no_uid[i]){
			f_cnt++;
		}
	}

	if(f_cnt == 16){ //no uid flash
		return 0;
	}else{
		return 1;
	}
}
