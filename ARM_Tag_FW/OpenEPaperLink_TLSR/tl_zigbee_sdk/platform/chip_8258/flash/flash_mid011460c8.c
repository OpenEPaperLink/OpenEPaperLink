/********************************************************************************************************
 * @file	flash_mid011460c8.c
 *
 * @brief	This is the source file for B85
 *
 * @author	Driver Group
 * @date	2018
 *
 * @par     Copyright (c) 2018, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#include "flash_type.h"


/**
 * @brief 		This function reads the status of flash.
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
unsigned short flash_read_status_mid011460c8(void)
{
	unsigned char status_low = flash_read_status(FLASH_READ_STATUS_CMD_LOWBYTE);
	unsigned char status_high = flash_read_status(FLASH_READ_STATUS_CMD_HIGHBYTE);
	return (status_low | (status_high << 8));
}

/**
 * @brief 		This function write the status of flash.
 * @param[in]  	data	- the value of status.
 * @param[in]  	bit		- the range of bits to be modified when writing status.
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
void flash_write_status_mid011460c8(unsigned short data, mid011460c8_write_status_bit_e bit)
{
	unsigned short status = flash_read_status_mid011460c8();
	data |= (status & ~(bit));
	flash_write_status(FLASH_TYPE_16BIT_STATUS_ONE_CMD, data);
}

/**
 * @brief 		This function serves to set the protection area of the flash.
 * @param[in]   data	- refer to the protection area definition in the .h file.
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
void flash_lock_mid011460c8(mid011460c8_lock_block_e data)
{
	flash_write_status_mid011460c8(data, FLASH_WRITE_STATUS_BP_MID011460C8);
}

void flash_lock_all_mid011460c8(void)
{
	flash_write_status_mid011460c8(FLASH_LOCK_ALL_1M_MID011460C8, FLASH_WRITE_STATUS_BP_MID011460C8);
}
/**
 * @brief 		This function serves to flash release protection.
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
void flash_unlock_mid011460c8(void)
{
	flash_write_status_mid011460c8(FLASH_LOCK_NONE_MID011460C8, FLASH_WRITE_STATUS_BP_MID011460C8);
}

/**
 * @brief 		This function serves to read data from the Security Registers of the flash.
 * @param[in]   addr	- the start address of the Security Registers.
 *						the address of the  Security Registers #1 0x001000-0x0011ff
 *						the address of the  Security Registers #2 0x002000-0x0021ff
 *						the address of the  Security Registers #3 0x003000-0x0031ff
 * @param[in]   len		- the length of the content to be read.
 * @param[out]  buf		- the starting address of the content to be read.
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
void flash_read_otp_mid011460c8(unsigned long addr, unsigned long len, unsigned char* buf)
{
	flash_read_otp(addr, len, buf);
}

/**
 * @brief 		This function serves to write data to the Security Registers of the flash you choose.
 * @param[in]   addr	- the start address of the Security Registers.
 *						the address of the  Security Registers #1 0x001000-0x0011ff
 *						the address of the  Security Registers #2 0x002000-0x0021ff
 *						the address of the  Security Registers #3 0x003000-0x0031ff
 * @param[in]   len		- the length of content to be written.
 * @param[in]   buf		- the starting address of the content to be written.
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
void flash_write_otp_mid011460c8(unsigned long addr, unsigned long len, unsigned char *buf)
{
	flash_write_otp(addr, len, buf);
}

/**
 * @brief 		This function serves to erase the data of the Security Registers that you choose.
 * 				You can erase 512-byte one time.
 * @param[in]   addr	- the address that you want to erase.
 *						the address of the  Security Registers #1 0x001000-0x0011ff
 *						the address of the  Security Registers #2 0x002000-0x0021ff
 *						the address of the  Security Registers #3 0x003000-0x0031ff
 * @return 		none.
 * @note		Even you choose the middle area of the Security Registers,it will erase the whole area.
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
void flash_erase_otp_mid011460c8(mid011460c8_otp_block_e addr)
{
	flash_erase_otp(addr);
}

/**
 * @brief 		This function serves to provide the write protect control to the Security Registers.
 * @param[in]   data	- the lock area of the Security Registers.
 * @return 		none.
 * @note		once they are set to 1, the Security Registers will become read-only permanently,
 * 				you can't write or erase the area anymore.
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
void flash_lock_otp_mid011460c8(mid011460c8_lock_otp_e data)
{
	flash_write_status_mid011460c8(data, FLASH_WRITE_STATUS_OTP_MID011460C8);
}

