/********************************************************************************************************
 * @file	lpc.h
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
#pragma once

#include "analog.h"

/**
 * define input IO.
 */
typedef enum{
	LPC_INPUT_PB1 = 1,
	LPC_INPUT_PB2 = 2,
	LPC_INPUT_PB3 = 3,
	LPC_INPUT_PB4 = 4,
	LPC_INPUT_PB5 = 5,
	LPC_INPUT_PB6 = 6,
	LPC_INPUT_PB7 = 7,
}lpc_input_channel_e;

/**
 * define work mode.
 */
typedef enum{
	LPC_NORMAL = 0,
	LPC_LOWPOWER,
}lpc_mode_e;

/**
 * define Reference voltage.
 */
typedef enum{
	LPC_REF_974MV  = 1,
	LPC_REF_923MV  = 2,
	LPC_REF_872MV  = 3,
	LPC_REF_820MV  = 4,
	LPC_REF_PB0    = 5,
	LPC_REF_PB3    = 6,
}lpc_reference_e;

/**
 * define scale.
 */
typedef enum{
	LPC_SCALING_PER25  = 0,
	LPC_SCALING_PER50  = 1,
	LPC_SCALING_PER75  = 2,
	LPC_SCALING_PER100 = 3,
}lpc_scaling_e;

/**
 * @brief		This function servers to powers down low power comparator.
 * @return		none.
 */
static inline void lpc_power_down(void)
{
	analog_write_reg8(0x07,(analog_read_reg8(0x07))|0x08);
}

/**
 * @brief		This function servers to power on low power comparator.
 * @return		none.
 */
static inline void lpc_power_on(void)
{
	analog_write_reg8(0x06,analog_read_reg8(0x06) & 0xfd);
}

/**
 * @brief		This function selects input channel for low power comparator.
 * @param[in]	pin		- selected input channel.Input derived from external PortB(PB<1>~PB<7>).
 * @return		none.
 */
static inline void lpc_set_input_chn(lpc_input_channel_e pin)
{
	analog_write_reg8(0x0d,(analog_read_reg8(0x0d) & 0xf8) | pin);
}

/**
 * @brief		This function serves to set scaling_coefficient for low power comparator.
 * @param[in]	divider	- selected scaling coefficient.(%25,%50,%75,%100)
 * @return		none.
 */
static inline void lpc_set_scaling_coeff(lpc_scaling_e divider)
{
	analog_write_reg8(0x0b,(analog_read_reg8(0x0b)&0xcf)|(divider<<4));
}

/**
 * @brief		This function serves to get the comparison results.if Vin>Vref 0x88[6]=0,else 0x88[6]=1.
 * @return		comparison results.
 */
static inline unsigned char lpc_get_result(void)
{
	return ((analog_read_reg8(0x88)&0x40)>>6);
}

/**
 * @brief		This function selects input reference voltage for low power comparator.
 * @param[in]	mode	- lower power comparator working mode includes normal mode and low power mode.
 * @param[in]	ref		- selected input reference voltage.
 * @return		none.
 */
void lpc_set_input_ref(lpc_mode_e mode, lpc_reference_e ref);


