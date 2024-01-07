/********************************************************************************************************
 * @file    drv_pwm.h
 *
 * @brief   This is the header file for drv_pwm
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

#if defined(MCU_CORE_826x) || defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	#define PWM_CLOCK_SOURCE			CLOCK_SYS_CLOCK_HZ
#elif defined(MCU_CORE_B91)
	#define PWM_CLOCK_SOURCE			(sys_clk.pclk * 1000 * 1000)
#endif


#if defined(MCU_CORE_826x)
	#define drv_pwm_start(pwmId)		pwm_Start(pwmId)
	#define drv_pwm_stop(pwmId)			pwm_Stop(pwmId)
	#define drv_pwm_invert(pwmId)		pwm_Invert(pwmId)
	#define drv_pwm_n_invert(pwmId)		pwm_INVInvert(pwmId)
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	#define drv_pwm_start(pwmId)		pwm_start(pwmId)
	#define drv_pwm_stop(pwmId)			pwm_stop(pwmId)
	#define drv_pwm_invert(pwmId)		pwm_revert(pwmId)
	#define drv_pwm_n_invert(pwmId)		pwm_n_revert(pwmId)
#elif defined(MCU_CORE_B91)
	#define drv_pwm_start(pwmId)		(pwmId == PWM0_ID) ? (pwm_start(FLD_PWM0_EN)) : (pwm_start(BIT(pwmId)))
	#define drv_pwm_stop(pwmId)			(pwmId == PWM0_ID) ? (pwm_stop(FLD_PWM0_EN)) : (pwm_stop(BIT(pwmId)))
	#define drv_pwm_invert(pwmId)		pwm_invert_en(pwmId)
	#define drv_pwm_n_invert(pwmId)		pwm_n_invert_en(pwmId)
#endif

void drv_pwm_init(void);

void drv_pwm_cfg(u8 pwmId, u16 cmp_tick, u16 cycle_tick);



