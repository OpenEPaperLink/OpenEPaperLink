/********************************************************************************************************
 * @file    clock.h
 *
 * @brief   This is the header file for B85
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


#define 	_ASM_NOP_			asm("tnop")

/**
 * @brief  instruction delay.
 */
#define		CLOCK_DLY_1_CYC    _ASM_NOP_
#define		CLOCK_DLY_2_CYC    _ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_3_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_4_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_5_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_6_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_7_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_8_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_9_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_10_CYC   _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_

/**
 * @brief select RC as system clock
 */
#ifndef SYSCLK_RC_CLOCK_EN
#define SYSCLK_RC_CLOCK_EN			1
#endif

extern unsigned char system_clk_type;
extern unsigned char system_clk_mHz;

/**
 * @brief system clock type.
 */
typedef enum{
	SYS_CLK_12M_Crystal = 0x44,
	SYS_CLK_16M_Crystal = 0x43,
	SYS_CLK_24M_Crystal = 0x42,
	SYS_CLK_32M_Crystal = 0x60,
	SYS_CLK_48M_Crystal = 0x20,

	SYS_CLK_RC_THRES = 0x10,

	SYS_CLK_24M_RC 	 = 0x00,
	SYS_CLK_32M_RC 	 = 0x01,
	SYS_CLK_48M_RC 	 = 0x02,
}SYS_CLK_TypeDef;

/**
 * @brief 32K clock type.
 */

typedef enum{
	CLK_32K_RC   =0,
	CLK_32K_XTAL =1,
}CLK_32K_TypeDef;

/**
 * @brief       This function to select the system clock source.
 * @param[in]   SYS_CLK - the clock source of the system clock.
 * @return      none
 */
void clock_init(SYS_CLK_TypeDef SYS_CLK);

/**
 * @brief       This function to get the system clock source.
 * @param[in]   none
 * @return      system clock source
 */

static inline int clock_get_system_clk(void)
{
	return system_clk_type;
}

/**
 * @brief   This function serves to set 32k clock source.
 * @param   variable of 32k type.
 * @return  none.
 */
void clock_32k_init(CLK_32K_TypeDef src);

/**
 * @brief     This function performs to select 24M as the system clock source.
 * @param[in] none.
 * @return    none.
 */
void rc_24m_cal(void);

/**
 * @brief     This function performs to select 48M RC as the system clock source.
 * @param[in] none.
 * @return    none.
 */
void rc_48m_cal(void);

/**
 * @brief     This function performs to select 32K as the system clock source.
 * @param[in] none.
 * @return    none.
 */
void rc_32k_cal(void);

/**
 * @brief     This function performs to calibration the source clock.
 * @param[in] none.
 * @return    none.
 */
void doubler_calibration(void);

/**
 * @brief     This function performs to select 24M/2 RC as source of DMIC.
 * @param[in] source clock to provide DMIC.
 * @return    none.
 */
void dmic_prob_24M_rc(void);

/**
 * @brief   	This function serves to kick external crystal.
 * @param[in]   times - the frequency of being kicked by pwm.
 * @return  	none.
 */
void pwm_kick_32k_pad_times(unsigned int times);
