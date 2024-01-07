/********************************************************************************************************
 * @file    drv_flash.h
 *
 * @brief   This is the header file for drv_flash
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


typedef void (*drv_flash_write)(unsigned long addr, unsigned long len, unsigned char *buf);
typedef void (*drv_flash_read)(unsigned long addr, unsigned long len, unsigned char *buf);
typedef void (*drv_flash_erase)(unsigned long addr);

typedef struct{
	drv_flash_write write;
	drv_flash_read read;
	drv_flash_erase erase;
}drv_flash_t;

void flash_write(u32 addr, u32 len, u8 *buf);
bool flash_writeWithCheck(u32 addr, u32 len, u8 *buf);
void flash_read(u32 addr, u32 len, u8 *buf);
void flash_erase(u32 addr);
