/********************************************************************************************************
 * @file    drv_pwm.c
 *
 * @brief   This is the source file for drv_pwm
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

void drv_pwm_init(void){
#if defined(MCU_CORE_826x)
	pwm_Init(0);
#elif defined(MCU_CORE_B91)
	pwm_set_clk(0);
#endif
}

void drv_pwm_cfg(u8 pwmId, u16 cmp_tick, u16 cycle_tick){
#if defined(MCU_CORE_826x)
	pwm_Open(pwmId, NORMAL, 0, cmp_tick, cycle_tick, 0x2fff);
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	pwm_set_mode(pwmId, PWM_NORMAL_MODE);
	pwm_set_phase(pwmId, 0);   //no phase at pwm beginning
	pwm_set_cycle_and_duty(pwmId, cycle_tick, cmp_tick);
#elif defined(MCU_CORE_B91)
	pwm_set_tcmp(pwmId, cmp_tick);
	pwm_set_tmax(pwmId, cycle_tick);
#endif
}
