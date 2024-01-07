/********************************************************************************************************
 * @file	flash_mid166085.h
 *
 * @brief	This is the header file for B91
 *
 * @author	Driver Group
 * @date	2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#ifndef __MID166085_H__
#define __MID166085_H__

/*
 * @brief     MID = 0x1660c8 Flash include P25Q32SH.
 */


/**
 * @brief     define the section of the protected memory area which is read-only and unalterable.
 */
typedef enum{
	//---------WPS=0, CMP bit = 0-----------------//
	FLASH_LOCK_NONE_MID166085			=	0x0000,  //000000h-000000h  //0x0020 0x407c...
	FLASH_LOCK_UP_64K_MID166085	        =   0x0004,  //3F0000h-3FFFFFh
	FLASH_LOCK_UP_128K_MID166085        =   0x0008,  //3E0000h-3FFFFFh
	FLASH_LOCK_UP_256K_MID166085        =   0x000c,  //3C0000h-3FFFFFh
	FLASH_LOCK_UP_512K_MID166085        =   0x0010,  //380000h-3FFFFFh
	FLASH_LOCK_UP_1M_MID166085          =   0x0014,  //300000h-3FFFFFh
	FLASH_LOCK_UP_2M_MID166085          =   0x0018,  //200000h-3FFFFFh  //0x4038
	FLASH_LOCK_LOW_64K_MID166085        =   0x0024,  //000000h-00FFFFh
	FLASH_LOCK_LOW_128K_MID166085       =   0x0028,  //000000h-01FFFFh
	FLASH_LOCK_LOW_256K_MID166085       =   0x002c,  //000000h-03FFFFh
	FLASH_LOCK_LOW_512K_MID166085       =   0x0030,  //000000h-07FFFFh
	FLASH_LOCK_LOW_1M_MID166085         =   0x0034,  //000000h-0FFFFFh
	FLASH_LOCK_LOW_2M_MID166085         =   0x0038,  //000000h-1FFFFFh  //0x007c 0x4018...
	FLASH_LOCK_UP_4K_MID166085          =   0x0044,  //3FF000h-3FFFFFh
	FLASH_LOCK_UP_8K_MID166085          =   0x0048,  //3FE000h-3FFFFFh
	FLASH_LOCK_UP_16K_MID166085         =   0x004c,  //2FC000h-3FFFFFh
	FLASH_LOCK_UP_32K_MID166085         =   0x0058,  //3F8000h-3FFFFFh  //0x0050 0x0054
	FLASH_LOCK_LOW_4K_MID166085         =   0x0064,  //000000h-000FFFh
	FLASH_LOCK_LOW_8K_MID166085         =   0x0068,  //000000h-001FFFh
	FLASH_LOCK_LOW_16K_MID166085        =   0x006c,  //000000h-003FFFh
	FLASH_LOCK_LOW_32K_MID166085        =   0x0078,  //000000h-007FFFh  //0x0070 0x0074
	//----------------WPS=0, CMP bit = 1---------------//
	FLASH_LOCK_LOW_4032K_MID166085      =   0x4004,  //000000h-3EFFFFh
	FLASH_LOCK_LOW_3968K_MID166085      =   0x4008,  //000000h-3DFFFFh
	FLASH_LOCK_LOW_3840K_MID166085      =   0x400c,  //000000h-3BFFFFh
	FLASH_LOCK_LOW_3584K_MID166085      =   0x4010,  //000000h-37FFFFh
	FLASH_LOCK_LOW_3M_MID166085         =   0x4014,  //000000h-2FFFFFh
	FLASH_LOCK_UP_4032K_MID166085       =   0x4024,  //010000h-3FFFFFh
	FLASH_LOCK_UP_3968K_MID166085       =   0x4028,  //020000h-3FFFFFh
	FLASH_LOCK_UP_3840K_MID166085       =   0x402c,  //040000h-3FFFFFh
	FLASH_LOCK_UP_3584K_MID166085       =   0x4030,  //080000h-3FFFFFh
	FLASH_LOCK_UP_3M_MID166085          =   0x4034,  //100000h-3FFFFFh
	FLASH_LOCK_LOW_4092K_MID166085      =   0x4044,  //000000h-3FEFFFh
	FLASH_LOCK_LOW_4088K_MID166085      =   0x4048,  //000000h-3FDFFFh
	FLASH_LOCK_LOW_4080K_MID166085      =   0x404c,  //000000h-3FBFFFh
	FLASH_LOCK_LOW_4064K_MID166085      =   0x4058,  //000000h-3F7FFFh  //0x4050 0x4054
	FLASH_LOCK_UP_4092K_MID166085       =   0x4064,  //001000h-3FFFFFh
	FLASH_LOCK_UP_4088K_MID166085       =   0x4068,  //002000h-3FFFFFh
	FLASH_LOCK_UP_4080K_MID166085       =   0x406c,  //004000h-3FFFFFh
	FLASH_LOCK_UP_4064K_MID166085       =   0x4078,  //008000h-3FFFFFh  //0x4070 0x4074
	FLASH_LOCK_ALL_4M_MID166085         =   0x4000,  //000000h-3FFFFFh  //0x4000 0x4040 0x4020 0x4060...
}mid166085_lock_block_e;

/**
 * @brief     The starting address of the Security Registers.
 */
typedef enum{
	FLASH_OTP_0x001000_1024B_MID166085	=	0x001000,	//001000h-0013FFh
	FLASH_OTP_0x002000_1024B_MID166085	=	0x002000,	//002000h-0023FFh
	FLASH_OTP_0x003000_1024B_MID166085	=	0x003000,	//003000h-0033FFh
}mid166085_otp_block_e;

/**
 * @brief     the lock area of the Security Registers.
 */
typedef enum{
	FLASH_LOCK_OTP_0x001000_1024B_MID166085	=	0x0800,	//001000h-0013FFh
	FLASH_LOCK_OTP_0x002000_1024B_MID166085	=	0x1000,	//002000h-0023FFh
	FLASH_LOCK_OTP_0x003000_1024B_MID166085	=	0x2000,	//003000h-0033FFh
}mid166085_lock_otp_e;

/**
 * @brief     the range of bits to be modified when writing status.
 */
typedef enum{
	FLASH_WRITE_STATUS_BP_MID166085		=	0x407c,
	FLASH_WRITE_STATUS_QE_MID166085		=	0x0200,
	FLASH_WRITE_STATUS_OTP_MID166085	=	0x3800,
	
	/*Related to flash hardware protection.When using this function, you need to pay attention. 
	 *If the #WP pin of the flash is grounded and the hardware protection is set at this time, 
	 *the status register of the flash will be locked and irreversible.*/
	FLASH_WRITE_STATUS_SRP_MID166085	= 	0x0180,
}mid166085_write_status_bit_e;

/**
 * @brief     the range of bits to be modified when writing configure.
 */
typedef enum{
	FLASH_WRITE_CONFIGURE_WPS_MID166085		=	0x04,
}mid166085_write_config_bit_e;
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
unsigned short flash_read_status_mid166085(void);
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
void flash_write_status_mid166085(unsigned short data, mid166085_write_status_bit_e bit);

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
void flash_lock_mid166085(mid166085_lock_block_e data);

void flash_lock_all_mid166085(void);

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
void flash_unlock_mid166085(void);

/**
 * @brief 		This function serves to read data from the Security Registers of the flash.
 * @param[in]   addr	- the start address of the Security Registers.
 *						the address of the  Security Registers #1 0x001000-0x0013ff
 *						the address of the  Security Registers #2 0x002000-0x0023ff
 *						the address of the  Security Registers #3 0x003000-0x0033ff
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
void flash_read_otp_mid166085(unsigned long addr, unsigned long len, unsigned char* buf);

/**
 * @brief 		This function serves to write data to the Security Registers of the flash you choose.
 * @param[in]   addr	- the start address of the Security Registers.
 *						the address of the  Security Registers #1 0x001000-0x0013ff
 *						the address of the  Security Registers #2 0x002000-0x0023ff
 *						the address of the  Security Registers #3 0x003000-0x0033ff
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
void flash_write_otp_mid166085(unsigned long addr, unsigned long len, unsigned char *buf);

/**
 * @brief 		This function serves to erase the data of the Security Registers that you choose.
 * 				You can erase 1024-byte one time.
 * @param[in]   addr	- the address that you want to erase.
 *						the address of the  Security Registers #1 0x001000-0x0013ff
 *						the address of the  Security Registers #2 0x002000-0x0023ff
 *						the address of the  Security Registers #3 0x003000-0x0033ff
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
void flash_erase_otp_mid166085(mid166085_otp_block_e addr);

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
void flash_lock_otp_mid166085(mid166085_lock_otp_e data);

/**
 * @brief 		This function write the configure of flash.
 * @param[in]  	data	- the value of configure.
 * @param[in]  	bit		- the range of bits to be modified when writing configure.
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
void flash_write_config_mid166085(unsigned char data, mid166085_write_config_bit_e bit);
#endif

