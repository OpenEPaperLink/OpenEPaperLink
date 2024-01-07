/********************************************************************************************************
 * @file    flash.c
 *
 * @brief   This is the source file for B87
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
#include "spi_i.h"
#include "irq.h"
#include "timer.h"
#include "string.h"
#include "analog.h"

/*******************************************************************************************************************
 *												Primary interface
 ******************************************************************************************************************/

/**
 * @brief		This function to determine whether the flash is busy..
 * @return		1:Indicates that the flash is busy. 0:Indicates that the flash is free
 */
_attribute_ram_code_sec_ static inline int flash_is_busy(void){
	return mspi_read() & 0x01;				//  the busy bit, pls check flash spec
}

/**
 * @brief		This function serves to set flash write command.
 * @param[in]	cmd	- set command.
 * @return		none.
 */
_attribute_ram_code_sec_noinline_ static void flash_send_cmd(unsigned char cmd){
	mspi_high();
	sleep_us(1);
	mspi_low();
	mspi_write(cmd);
	mspi_wait();
}

/**
 * @brief		This function serves to send flash address.
 * @param[in]	addr	- the flash address.
 * @return		none.
 */
_attribute_ram_code_sec_noinline_ static void flash_send_addr(unsigned int addr){
	mspi_write((unsigned char)(addr>>16));
	mspi_wait();
	mspi_write((unsigned char)(addr>>8));
	mspi_wait();
	mspi_write((unsigned char)(addr));
	mspi_wait();
}

/**
 * @brief     This function serves to wait flash done.(make this a asynchorous version).
 * @return    none.
 */
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

/**
 * @brief 		This function is used to read data from flash or read the status of flash.
 * @param[in]   cmd			- the read command.
 * @param[in]   addr		- starting address.
 * @param[in]   addr_en		- whether need to send an address.
 * @param[in]   dummy_cnt	- the length(in byte) of dummy.
 * @param[out]  data		- the start address of the data buffer.
 * @param[in]   data_len	- the length(in byte) of content needs to read out.
 * @return 		none.
 */
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

/**
 * @brief 		This function is used to write data or status to flash.
 * @param[in]   cmd			- the write command.
 * @param[in]   addr		- starting address.
 * @param[in]   addr_en		- whether need to send an address.
 * @param[out]  data		- the start address of the data buffer.
 * @param[in]   data_len	- the length(in byte) of content needs to read out.
 * @return 		none.
 * @note		important:  "data" must not reside at flash, such as constant string.If that case, pls copy to memory first before write.
 */
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

/**
 * @brief 		This function serves to erase a sector.
 * @param[in]   addr	- the start address of the sector needs to erase.
 * @return 		none.
 * @note        Attention: Before calling the FLASH function, please check the power supply voltage of the chip.
 *              Only if the detected voltage is greater than the safe voltage value, the FLASH function can be called.
 *              Taking into account the factors such as power supply fluctuations, the safe voltage value needs to be greater
 *              than the minimum chip operating voltage. For the specific value, please make a reasonable setting according
 *              to the specific application and hardware circuit.
 *
 *              Risk description: When the chip power supply voltage is relatively low, due to the unstable power supply,
 *              there may be a risk of error in the operation of the flash (especially for the write and erase operations.
 *              If an abnormality occurs, the firmware and user data may be rewritten, resulting in the final Product failure)
 */
void flash_erase_sector(unsigned long addr)
{
	flash_mspi_write_ram(FLASH_SECT_ERASE_CMD, addr, 1, NULL, 0);
}

/**
 * @brief 		This function reads the content from a page to the buf.
 * @param[in]   addr	- the start address of the page.
 * @param[in]   len		- the length(in byte) of content needs to read out from the page.
 * @param[out]  buf		- the start address of the buffer.
 * @return 		none.
 * @note        Attention: Before calling the FLASH function, please check the power supply voltage of the chip.
 *              Only if the detected voltage is greater than the safe voltage value, the FLASH function can be called.
 *              Taking into account the factors such as power supply fluctuations, the safe voltage value needs to be greater
 *              than the minimum chip operating voltage. For the specific value, please make a reasonable setting according
 *              to the specific application and hardware circuit.
 *
 *              Risk description: When the chip power supply voltage is relatively low, due to the unstable power supply,
 *              there may be a risk of error in the operation of the flash (especially for the write and erase operations.
 *              If an abnormality occurs, the firmware and user data may be rewritten, resulting in the final Product failure)
 */
void flash_read_page(unsigned long addr, unsigned long len, unsigned char *buf)
{
	flash_mspi_read_ram(FLASH_READ_CMD, addr, 1, 0, buf, len);
}

/**
 * @brief 		This function writes the buffer's content to the flash.
 * @param[in]   addr	- the start address of the area.
 * @param[in]   len		- the length(in byte) of content needs to write into the flash.
 * @param[in]   buf		- the start address of the content needs to write into.
 * @return 		none.
 * @note        the funciton support cross-page writing,which means the len of buf can bigger than 256.
 *
 *              Attention: Before calling the FLASH function, please check the power supply voltage of the chip.
 *              Only if the detected voltage is greater than the safe voltage value, the FLASH function can be called.
 *              Taking into account the factors such as power supply fluctuations, the safe voltage value needs to be greater
 *              than the minimum chip operating voltage. For the specific value, please make a reasonable setting according
 *              to the specific application and hardware circuit.
 *
 *              Risk description: When the chip power supply voltage is relatively low, due to the unstable power supply,
 *              there may be a risk of error in the operation of the flash (especially for the write and erase operations.
 *              If an abnormality occurs, the firmware and user data may be rewritten, resulting in the final Product failure)
 */
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

/**
 * @brief		This function reads the status of flash.
 * @param[in] 	cmd	- the cmd of read status.
 * @return 		the value of status.
 * @note        Attention: Before calling the FLASH function, please check the power supply voltage of the chip.
 *              Only if the detected voltage is greater than the safe voltage value, the FLASH function can be called.
 *              Taking into account the factors such as power supply fluctuations, the safe voltage value needs to be greater
 *              than the minimum chip operating voltage. For the specific value, please make a reasonable setting according
 *              to the specific application and hardware circuit.
 *
 *              Risk description: When the chip power supply voltage is relatively low, due to the unstable power supply,
 *              there may be a risk of error in the operation of the flash (especially for the write and erase operations.
 *              If an abnormality occurs, the firmware and user data may be rewritten, resulting in the final Product failure)
 */
unsigned char flash_read_status(unsigned char cmd)
{
	unsigned char status = 0;
	flash_mspi_read_ram(cmd, 0, 0, 0, &status, 1);
	return status;
}

/**
 * @brief 		This function write the status of flash.
 * @param[in]  	type	- the type of status.8 bit or 16 bit.
 * @param[in]  	data	- the value of status.
 * @return 		none.
 * @note        Attention: Before calling the FLASH function, please check the power supply voltage of the chip.
 *              Only if the detected voltage is greater than the safe voltage value, the FLASH function can be called.
 *              Taking into account the factors such as power supply fluctuations, the safe voltage value needs to be greater
 *              than the minimum chip operating voltage. For the specific value, please make a reasonable setting according
 *              to the specific application and hardware circuit.
 *
 *              Risk description: When the chip power supply voltage is relatively low, due to the unstable power supply,
 *              there may be a risk of error in the operation of the flash (especially for the write and erase operations.
 *              If an abnormality occurs, the firmware and user data may be rewritten, resulting in the final Product failure)
 */
void flash_write_status(flash_status_typedef_e type , unsigned short data)
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

/**
 * @brief	  	This function serves to read MID of flash(MAC id). Before reading UID of flash,
 * 				you must read MID of flash. and then you can look up the related table to select
 * 				the idcmd and read UID of flash.
 * @return    	MID of the flash(4 bytes).
 * @note        Attention: Before calling the FLASH function, please check the power supply voltage of the chip.
 *              Only if the detected voltage is greater than the safe voltage value, the FLASH function can be called.
 *              Taking into account the factors such as power supply fluctuations, the safe voltage value needs to be greater
 *              than the minimum chip operating voltage. For the specific value, please make a reasonable setting according
 *              to the specific application and hardware circuit.
 *
 *              Risk description: When the chip power supply voltage is relatively low, due to the unstable power supply,
 *              there may be a risk of error in the operation of the flash (especially for the write and erase operations.
 *              If an abnormality occurs, the firmware and user data may be rewritten, resulting in the final Product failure)
 */
unsigned int flash_read_mid(void)
{
	unsigned int flash_mid = 0;
	flash_mspi_read_ram(FLASH_GET_JEDEC_ID, 0, 0, 0, (unsigned char*)(&flash_mid), 3);
	return flash_mid;
}

/**
 * @brief	  	This function serves to read UID of flash.Before reading UID of flash, you must read MID of flash.
 * 				and then you can look up the related table to select the idcmd and read UID of flash.
 * @param[in] 	idcmd	- different flash vendor have different read-uid command. E.g: GD/PUYA:0x4B; XTX: 0x5A.
 * @param[in] 	buf		- store UID of flash.
 * @return    	none.
 * @note        Attention: Before calling the FLASH function, please check the power supply voltage of the chip.
 *              Only if the detected voltage is greater than the safe voltage value, the FLASH function can be called.
 *              Taking into account the factors such as power supply fluctuations, the safe voltage value needs to be greater
 *              than the minimum chip operating voltage. For the specific value, please make a reasonable setting according
 *              to the specific application and hardware circuit.
 *
 *              Risk description: When the chip power supply voltage is relatively low, due to the unstable power supply,
 *              there may be a risk of error in the operation of the flash (especially for the write and erase operations.
 *              If an abnormality occurs, the firmware and user data may be rewritten, resulting in the final Product failure)
 */
void flash_read_uid(unsigned char idcmd, unsigned char *buf)
{
	if(idcmd == FLASH_READ_UID_CMD_GD_PUYA_ZB_UT)	//< GD/PUYA/ZB/UT
	{
		flash_mspi_read_ram(idcmd, 0x00, 1, 1, buf, 16);
	}
	else if (idcmd == FLASH_XTX_READ_UID_CMD)		//< XTX
	{
		flash_mspi_read_ram(idcmd, 0x80, 1, 1, buf, 16);
	}
}

/*******************************************************************************************************************
 *												Secondary interface
 ******************************************************************************************************************/

/**
 * @brief		This function serves to read flash mid and uid,and check the correctness of mid and uid.
 * @param[out]	flash_mid	- Flash Manufacturer ID.
 * @param[out]	flash_uid	- Flash Unique ID.
 * @return		0: flash no uid or not a known flash model 	 1:the flash model is known and the uid is read.
 * @note        Attention: Before calling the FLASH function, please check the power supply voltage of the chip.
 *              Only if the detected voltage is greater than the safe voltage value, the FLASH function can be called.
 *              Taking into account the factors such as power supply fluctuations, the safe voltage value needs to be greater
 *              than the minimum chip operating voltage. For the specific value, please make a reasonable setting according
 *              to the specific application and hardware circuit.
 *
 *              Risk description: When the chip power supply voltage is relatively low, due to the unstable power supply,
 *              there may be a risk of error in the operation of the flash (especially for the write and erase operations.
 *              If an abnormality occurs, the firmware and user data may be rewritten, resulting in the final Product failure)
 */
int flash_read_mid_uid_with_check(unsigned int *flash_mid, unsigned char *flash_uid)
{
	unsigned char no_uid[16]={0x51,0x01,0x51,0x01,0x51,0x01,0x51,0x01,0x51,0x01,0x51,0x01,0x51,0x01,0x51,0x01};
	int i,f_cnt=0;
	*flash_mid  = flash_read_mid();

	/*
	 * If add flash type, need pay attention to the read uid cmd and the bir number of status register
	   Flash Type	CMD			MID		Company
	   GD25LD10C	0x4b(AN)0x1160C8	GD
	   GD25LD40C	0x4b	0x1360C8	GD
	   GD25LD80C	0x4b(AN)0x1460C8	GD
	   ZB25WD10A	0x4b	0x11325E	ZB
	   ZB25WD40B	0x4b	0x13325E	ZB
	   ZB25WD80B	0x4b	0x14325E	ZB
	   ZB25WD20A	0x4b	0x12325E	ZB	The actual capacity is 256K, but the nominal value is 128KB.
											The software cannot do capacity adaptation and requires special customer special processing.

	   The uid of the early ZB25WD40B (mid is 0x13325E) is 8 bytes. If you read 16 bytes of uid,
	   the next 8 bytes will be read as 0xff. Later, the uid of ZB25WD40B has been switched to 16 bytes.
	 */
	if((*flash_mid == 0x1160C8)||(*flash_mid == 0x1360C8)||(*flash_mid == 0x1460C8)||(*flash_mid == 0x11325E)||(*flash_mid == 0x12325E)||(*flash_mid == 0x13325E)||(*flash_mid == 0x14325E)){
		flash_read_uid(FLASH_READ_UID_CMD_GD_PUYA_ZB_UT, (unsigned char *)flash_uid);
	}else{
		return 0;
	}

	for(i=0;i<16;i++){
		if(flash_uid[i] == no_uid[i]){
			f_cnt++;
		}
	}

	if(f_cnt == 16){	//no uid flash
		return 0;
	}else{
		return 1;
	}
}

/**
 * @brief		This function serves to find whether it is zb flash.
 * @param[in]	none.
 * @return		1 - is zb flash;   0 - is not zb flash.
 */
unsigned char flash_is_zb(void)
{
	unsigned int flash_mid  = flash_read_mid();
	if((flash_mid == 0x13325E)||(flash_mid == 0x14325E)||(flash_mid == 0x11325E)||(flash_mid == 0x12325E))
	{
		return 1;
	}
	return 0;
}

/**
 * @brief		This function serves to calibration the flash voltage(VDD_F),if the flash has the calib_value,we will use it,either will
 * 				trim vdd_f to 1.95V(2b'111 the max) if the flash is zb.
 * @param[in]	vol - the voltage which you want to set.
 * @return		none.
 */
void flash_vdd_f_calib(void)
{
	unsigned short calib_value = flash_get_vdd_f_calib_value();
	if((0xffff == calib_value) || (0 != (calib_value & 0xf8f8)))
	{
		if(flash_is_zb())
		{
			analog_write(0x09, ((analog_read(0x09) & 0x8f) | (FLASH_VOLTAGE_1V95 << 4)));    //ldo mode flash ldo trim 1.95V
			analog_write(0x0c, ((analog_read(0x0c) & 0xf8) | FLASH_VOLTAGE_1V9));			 //dcdc mode flash ldo trim 1.90V
		}
	}
	else
	{
		analog_write(0x09, ((analog_read(0x09) & 0x8f)  | ((calib_value & 0xff00) >> 4) ));
		analog_write(0x0c, ((analog_read(0x0c) & 0xf8)  | (calib_value & 0xff)));
	}
}

