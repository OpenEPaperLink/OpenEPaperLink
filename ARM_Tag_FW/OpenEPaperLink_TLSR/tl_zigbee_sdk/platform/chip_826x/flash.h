/********************************************************************************************************
 * @file    flash.h
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

#include "compiler.h"


#define PAGE_SIZE		256


enum{
	FLASH_WRITE_CMD						=	0x02,
	FLASH_READ_CMD						=	0x03,

	FLASH_SECT_ERASE_CMD				=	0x20,

	FLASH_READ_UID_CMD_GD_PUYA_ZB_UT	=	0x4B,
	FLASH_READ_UID_CMD_XTX				=	0x5A,

	FLASH_GET_JEDEC_ID					=	0x9F,

	FLASH_WRITE_STATUS_CMD_LOWBYTE		=	0x01,
	FLASH_WRITE_STATUS_CMD_HIGHBYTE		=	0x31,

	FLASH_READ_STATUS_CMD_LOWBYTE		=	0x05,
	FLASH_READ_STATUS_CMD_HIGHBYTE		=	0x35,

	FLASH_WRITE_DISABLE_CMD 			= 	0x04,
	FLASH_WRITE_ENABLE_CMD 				= 	0x06,
};

typedef enum{
	FLASH_TYPE_8BIT_STATUS   			= 0,
	FLASH_TYPE_16BIT_STATUS_ONE_CMD  	= 1,
	FLASH_TYPE_16BIT_STATUS_TWO_CMD  	= 2,
}flash_status_typedef_e;

void flash_erase_sector(unsigned long addr);
void flash_read_page(unsigned long addr, unsigned long len, unsigned char *buf);
void flash_write_page(unsigned long addr, unsigned long len, unsigned char *buf);
unsigned int flash_read_mid(void);
void flash_read_uid(unsigned char idcmd, unsigned char *buf);
int flash_read_mid_uid_with_check(unsigned int *flash_mid, unsigned char *flash_uid);
