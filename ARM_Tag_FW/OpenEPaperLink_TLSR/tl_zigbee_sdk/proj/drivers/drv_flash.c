/********************************************************************************************************
 * @file    drv_flash.c
 *
 * @brief   This is the source file for drv_flash
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

#include "../tl_common.h"


void flash_write(u32 addr, u32 len, u8 *buf){
#if (MODULE_WATCHDOG_ENABLE)
	drv_wd_clear();
#endif

#if (VOLTAGE_DETECT_ENABLE)
	if(drv_get_adc_data() < BATTERY_SAFETY_THRESHOLD){return;}
#endif

	flash_write_page(addr, len, buf);
}

bool flash_writeWithCheck(u32 addr, u32 len, u8 *buf){
	s32 toalLen = (s32)len;
	s8 copyLen = 48;
	u8 pTemp[48];
	u8 wLen = 0;
	u32 sAddr = (u32)addr;
	u8 *pSrc = buf;

#if (MODULE_WATCHDOG_ENABLE)
	drv_wd_clear();
#endif

#if (VOLTAGE_DETECT_ENABLE)
	if(drv_get_adc_data() < BATTERY_SAFETY_THRESHOLD){
        return FALSE;
    }
#endif

	flash_write_page(addr, len, buf);

	while(toalLen > 0){
		wLen = (toalLen > copyLen) ? copyLen : toalLen;
		flash_read(sAddr, wLen, pTemp);
		if(memcmp(pTemp, pSrc, wLen)){
			return FALSE;
		}
		toalLen -= wLen;
		sAddr += wLen;
		pSrc += wLen;
	}
	return TRUE;
}


void flash_read(u32 addr, u32 len, u8 *buf){
	flash_read_page(addr, len, buf);
}

void flash_erase(u32 addr){
#if (MODULE_WATCHDOG_ENABLE)
	drv_wd_clear();
#endif

#if (VOLTAGE_DETECT_ENABLE)
	if(drv_get_adc_data() < BATTERY_SAFETY_THRESHOLD){return;}
#endif

	flash_erase_sector(addr);
}

#ifdef CFS_ENABLE
_attribute_ram_code_ void cfs_flash_write_page(u32 addr, u32 len, u8 *buf){
	u32 r = drv_disable_irq();
	// important:  buf must not reside at flash, such as constant string.  If that case, pls copy to memory first before write
	flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
	flash_send_cmd(FLASH_WRITE_CMD);
	flash_send_addr(addr);

	u32 i;
	for(i = 0; i < len; ++i){
		mspi_write(~(buf[i]));		/* write data */
		mspi_wait();
	}
	mspi_high();
	flash_wait_done();
	drv_restore_irq(r);
}

_attribute_ram_code_ void cfs_flash_read_page(u32 addr, u32 len, u8 *buf){
	u32 r = drv_disable_irq();
	flash_send_cmd(FLASH_READ_CMD);
	flash_send_addr(addr);

	mspi_write(0x00);		/* dummy,  to issue clock */
	mspi_wait();
	mspi_ctrl_write(0x0a);	/* auto mode */
	mspi_wait();
	/* get data */
	for(int i = 0; i < len; ++i){
		*buf++ = ~(mspi_get());
		mspi_wait();
	}
	mspi_high();
	drv_restore_irq(r);
}

void cfs_flash_op(u8 opmode, u32 addr, u32 len, u8 *buf){
	u32 re = addr%256;

	u32 pageReLen = (re)?(256 -re):256;

	u32 writeLen = 0;
	u32 remainLen = len;

	do{
		if(remainLen <= pageReLen){
			writeLen = remainLen;
			remainLen = 0;
		}else{
			remainLen -= pageReLen;
			writeLen = pageReLen;
			pageReLen = 256;
		}
		if(opmode){
			cfs_flash_write_page(addr,writeLen,buf);
		}else{
			cfs_flash_read_page(addr,writeLen,buf);
		}
		buf += writeLen;
		addr += writeLen;
	}while(remainLen);
}

void cfs_flash_write(u32 addr, u32 len, u8 *buf){
	cfs_flash_op(1, addr, len, buf);
}

void cfs_flash_read(u32 addr, u32 len, u8 *buf){
	cfs_flash_op(0, addr, len, buf);
}
#endif

