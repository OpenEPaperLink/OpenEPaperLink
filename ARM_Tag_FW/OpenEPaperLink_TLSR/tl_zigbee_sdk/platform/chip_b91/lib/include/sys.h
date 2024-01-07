/********************************************************************************************************
 * @file	sys.h
 *
 * @brief	This is the header file for B91
 *
 * @author	Driver Group
 * @date	2019
 *
 * @par     Copyright (c) 2019, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
/**	@page SYS
 *
 *	Introduction
 *	===============
 *	Clock init and system timer delay.
 *
 *	API Reference
 *	===============
 *	Header File: sys.h
 */

#ifndef SYS_H_
#define SYS_H_
#include "reg_include/stimer_reg.h"


/**********************************************************************************************************************
 *                                         global constants                                                           *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *                                           global macro                                                             *
 *********************************************************************************************************************/
/*
 * brief instruction delay
 */

#define	_ASM_NOP_					__asm__("nop")

#define	CLOCK_DLY_1_CYC				_ASM_NOP_
#define	CLOCK_DLY_2_CYC				_ASM_NOP_;_ASM_NOP_
#define	CLOCK_DLY_3_CYC				_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define	CLOCK_DLY_4_CYC				_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define	CLOCK_DLY_5_CYC				_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define	CLOCK_DLY_6_CYC				_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define	CLOCK_DLY_7_CYC				_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define	CLOCK_DLY_8_CYC				_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define	CLOCK_DLY_9_CYC				_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define	CLOCK_DLY_10_CYC			_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_


/**********************************************************************************************************************
 *                                         global data type                                                           *
 *********************************************************************************************************************/

/**
 * @brief 	Power type for different application
 */
typedef enum{
	LDO_1P4_LDO_1P8 	= 0x00,	/**< 1.4V-LDO & 1.8V-LDO mode */
	DCDC_1P4_LDO_1P8	= 0x01,	/**< 1.4V-DCDC & 1.8V-LDO mode */
	DCDC_1P4_DCDC_1P8	= 0x03,	/**< 1.4V-DCDC & 1.8V-DCDC mode */
}power_mode_e;

/**
 * @brief 	The maximum voltage that the chip can withstand is 3.6V.
 * 			When the vbat power supply voltage is lower than 3.6V, it is configured as VBAT_MAX_VALUE_LESS_THAN_3V6 mode,
 * 			bypass is turned on, and the vbat voltage directly supplies power to the chip.
 * 			The voltage of the GPIO pin is the voltage of vbat.
 * 			When the vbat power supply voltage may be higher than 3.6V, it is configured as VBAT_MAX_VALUE_GREATER_THAN_3V6 mode,
 * 			the bypass is closed, and the vbat voltage passes through an LDO to supply power to the chip.
 * 			The voltage of the GPIO pin (V_ioh) is the voltage after Vbat passes through the LDO (V_ldo),
 * 			and the maximum value is about 3.3V floating 10% (V_ldoh).
 * 			When Vbat > V_ldoh, V_ioh = V_ldo = V_ldoh. When Vbat < V_ldoh, V_ioh = V_ldo = Vbat.
 */
typedef enum{
	VBAT_MAX_VALUE_GREATER_THAN_3V6	= 0x00,		/*VBAT may be greater than 3.6V. */
	VBAT_MAX_VALUE_LESS_THAN_3V6	= BIT(3),	/*VBAT must be below 3.6V. */
}vbat_type_e;

/**
 * @brief command table for special registers
 */
typedef struct tbl_cmd_set_t {
	unsigned int  	adr;
	unsigned char	dat;
	unsigned char	cmd;
} tbl_cmd_set_t;


/**********************************************************************************************************************
 *                                     global variable declaration                                                    *
 *********************************************************************************************************************/

extern unsigned int g_chip_version;

/**********************************************************************************************************************
 *                                      global function prototype                                                     *
 *********************************************************************************************************************/
/**
 * @brief      This function reboot mcu.
 * @return     none
 */
static inline void sys_reboot(void)
{
	reg_pwdn_en = 0x20;
}
/**
 * @brief   	This function serves to initialize system.
 * @param[in]	power_mode - power mode(LDO/DCDC/LDO_DCDC)
 * @param[in]	vbat_v		- vbat voltage type: 0 vbat may be greater than 3.6V,  1 vbat must be below 3.6V.
 * @return  	none
 */
void sys_init(power_mode_e power_mode, vbat_type_e vbat_v);

/**
 * @brief      This function performs a series of operations of writing digital or analog registers
 *             according to a command table
 * @param[in]  pt    - pointer to a command table containing several writing commands
 * @param[in]  size  - number of commands in the table
 * @return     number of commands are carried out
 */

int write_reg_table(const tbl_cmd_set_t * pt, int size);
/**
 * @brief     this function servers to get calibration value from EFUSE.
 * 			  Only the two-point calibration gain and offset of GPIO sampling are saved in the efuse of B91.
 * @param[out]gain - gpio_calib_value.
 * @param[out]offset - gpio_calib_value_offset.
 * @return    1 means there is a calibration value in efuse, and 0 means there is no calibration value in efuse.
 */
unsigned char efuse_get_adc_calib_value(unsigned short* gain, signed char* offset);

#endif
