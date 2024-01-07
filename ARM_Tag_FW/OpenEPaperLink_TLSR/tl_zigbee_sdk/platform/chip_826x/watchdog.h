/********************************************************************************************************
 * @file    watchdog.h
 *
 * @brief   This is the header file for B86
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

#include "register.h"

//  watchdog use timer 2
static inline void wd_set_interval_ms(unsigned int interval_ms)
{
	reg_tmr2_tick = 0;    //reset tick register
	reg_tmr_ctrl |= MASK_VAL(FLD_TMR_WD_CAPT, (interval_ms * tickPerUs * 1000 >> WATCHDOG_TIMEOUT_COEFF));
}

static inline void wd_start(void)
{
	SET_FLD(reg_tmr_ctrl, FLD_TMR_WD_EN);
}

static inline void wd_stop(void)
{
	CLR_FLD(reg_tmr_ctrl, FLD_TMR_WD_EN);
}

static inline void wd_clear(void)
{
	SET_FLD(reg_tmr_ctrl, FLD_CLR_WD);
}
