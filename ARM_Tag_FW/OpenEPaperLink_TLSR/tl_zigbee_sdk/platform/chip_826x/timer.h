/********************************************************************************************************
 * @file    timer.h
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

typedef enum{
	TIMER0		=0,
	TIMER1		=1,
	TIMER2		=2,
}TIMER_TypeDef;

typedef enum{
	TIMER_MODE_SYSCLK		=0,
	TIMER_MODE_GPIO_TRIGGER	=1,//not used
	TIMER_MODE_GPIO_WIDTH	=2,//not used
	TIMER_MODE_TICK			=3,//not used
}TIMER_ModeTypeDef;

static inline void timer_set_init_tick(TIMER_TypeDef type, unsigned int init_tick)
{
	reg_tmr_tick(type) = init_tick;
}

static inline void timer_set_cap_tick(TIMER_TypeDef type, unsigned int cap_tick)
{
	reg_tmr_capt(type) = cap_tick;
}

static inline void timer_irq_enable(TIMER_TypeDef type)
{
	reg_irq_mask |= (1 << type);
}

static inline void stimer_set_irq_capture(unsigned long cap_tick)
{
	reg_system_tick_irq = cap_tick;
}

static inline void stimer_irq_enable(void)
{
	reg_system_tick_mode |= FLD_SYSTEM_TICK_IRQ_EN;
}

static inline void stimer_set_irq_mask(void)
{
	reg_irq_mask |= FLD_IRQ_SYSTEM_TIMER;
}

static inline void stimer_clr_irq_mask(void)
{
	reg_irq_mask &= ~(unsigned long)FLD_IRQ_SYSTEM_TIMER;
}

void timer_set_mode(TIMER_TypeDef type, TIMER_ModeTypeDef mode);

void timer_start(TIMER_TypeDef type);

void timer_stop(TIMER_TypeDef type);

