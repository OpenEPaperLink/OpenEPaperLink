/********************************************************************************************************
 * @file	flash_mid1360c8.h
 *
 * @brief	This is the header file for B85
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
#ifndef __MID1360C8_H__
#define __MID1360C8_H__

/*
 * @brief     MID = 0x1360c8 Flash include GD25LD40C.
 */


/**
 * @brief     define the section of the protected memory area which is read-only and unalterable.
 */
typedef enum{
	FLASH_LOCK_NONE_MID1360C8		=	0x00,
	FLASH_LOCK_LOW_504K_MID1360C8	=	0x04,	//000000h-07DFFFh
	FLASH_LOCK_LOW_496K_MID1360C8	=	0x08,	//000000h-07BFFFh
	FLASH_LOCK_LOW_480K_MID1360C8	=	0x0c,	//000000h-077FFFh
	FLASH_LOCK_LOW_448K_MID1360C8	=	0x10,	//000000h-06FFFFh
	FLASH_LOCK_LOW_384K_MID1360C8	=	0x14,	//000000h-05FFFFh
	FLASH_LOCK_LOW_256K_MID1360C8	=	0x18,	//000000h-03FFFFh
	FLASH_LOCK_ALL_512K_MID1360C8	=	0x1c,	//000000h-07FFFFh
}mid1360c8_lock_block_e;

/**
 * @brief     the range of bits to be modified when writing status.
 */
typedef enum{
	FLASH_WRITE_STATUS_BP_MID1360C8	=	0x1c,
}mid1360c8_write_status_bit_e;


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
unsigned char flash_read_status_mid1360c8(void);

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
void flash_write_status_mid1360c8(unsigned char data, mid1360c8_write_status_bit_e bit);

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
void flash_lock_mid1360c8(mid1360c8_lock_block_e data);

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
void flash_unlock_mid1360c8(void);

void flash_lock_all_mid1360c8(void);

#endif

