/********************************************************************************************************
 * @file    timer.h
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

#ifndef TIMER_H_
#define TIMER_H_

#include "register.h"
#include "gpio.h"

/**
 * @brief   Type of Timer
 */
typedef enum{
	TIMER0		=0,
	TIMER1		=1,
	TIMER2		=2,
}TIMER_TypeDef;

/**
 * @brief   Mode of Timer
 */
typedef enum{
	TIMER_MODE_SYSCLK		=0,
	TIMER_MODE_GPIO_TRIGGER	=1,
	TIMER_MODE_GPIO_WIDTH	=2,
	TIMER_MODE_TICK			=3,
}TIMER_ModeTypeDef;

#define	    sys_tick_per_us   				16
#define		CLOCK_SYS_CLOCK_1250US			(1250 * sys_tick_per_us)

/**
 * @brief   system Timer : 16Mhz, Constant
 */
enum{
	CLOCK_16M_SYS_TIMER_CLK_1S  = 16*1000*1000,
	CLOCK_16M_SYS_TIMER_CLK_1MS = 16*1000,
	CLOCK_16M_SYS_TIMER_CLK_1US = 16,
};

/**
 * @brief     This function performs to gets system timer0 address.
 * @param[in] none.
 * @return    timer0 address.
 */
static inline unsigned long clock_time(void)
{
	return reg_system_tick;
}

/**
 * @brief     This function performs to calculation exceed us of the timer.
 * @param[in] ref - Variable of reference timer address.
 * @param[in] span_us - Variable of span us.
 * @return    the exceed.
 */
static inline unsigned int clock_time_exceed(unsigned int ref, unsigned int us)
{
	return ((unsigned int)(clock_time() - ref) > us * 16);
}

/*
 * @brief     This function set to initial tick for timer.
 * @param[in] type - the specified timer.
 * @param[in] init_tick - initial tick value.
 * @return    none
 */
static inline void timer_set_init_tick(TIMER_TypeDef type, unsigned int init_tick)
{
	reg_tmr_tick(type) = init_tick;
}

/*
 * @brief     This function set to capture tick for timer.
 * @param[in] type - the specified timer.
 * @param[in] cap_tick - cap tick value.
 * @return    none
 */
static inline void timer_set_cap_tick(TIMER_TypeDef type, unsigned int cap_tick)
{
	reg_tmr_capt(type) = cap_tick;
}

/*
 * @brief     This function to enable the timer interrupt.
 * @param[in] type - the specified timer.
 * @return    none
 */
static inline void timer_irq_enable(TIMER_TypeDef type)
{
	reg_irq_mask |= (1 << type);
}

/*
 * @brief     This function to enable the system timer interrupt.
 * @param[in] type - the specified timer.
 * @return    none
 */
static inline void stimer_irq_enable(void)
{
	reg_system_tick_mode |= FLD_SYSTEM_TICK_IRQ_EN;
}

/*
 * @brief     This function set capture tick for system timer.
 * @param[in] cap_tick - cap tick value.
 * @return    none
 */
static inline void stimer_set_irq_capture(unsigned long cap_tick)
{
	reg_system_tick_irq = cap_tick;
}

/**
 * @brief This function servers to set system timer irq mask.
 * @param[in]	none.
 * @return  	none.
 */
static inline void stimer_set_irq_mask(void)
{
	reg_irq_mask |= FLD_IRQ_SYSTEM_TIMER;
}

/**
 * @brief This function servers to clear system timer irq mask.
 * @param[in] 	none.
 * @return  	none.
 */
static inline void stimer_clr_irq_mask(void)
{
	reg_irq_mask &= ~(unsigned long)FLD_IRQ_SYSTEM_TIMER;
}

/**
 * @brief     set timer mode.
 * @param[in] type - the specified timer.
 * @param[in] mode - select mode for timer.
 * @return    none
 */
void timer_set_mode(TIMER_TypeDef type, TIMER_ModeTypeDef mode);

/**
 * @brief     initiate GPIO for gpio trigger and gpio width mode of timer.
 * @param[in] type - the specified timer.
 * @param[in] pin - select pin for timer.
 * @param[in] pol - select polarity for gpio trigger and gpio width
 * @return    none
 */
void timer_gpio_init(TIMER_TypeDef type, GPIO_PinTypeDef pin, GPIO_PolTypeDef pol);

/**
 * @brief     the specifed timer start working.
 * @param[in] type - select the timer to start.
 * @return    none
 */
void timer_start(TIMER_TypeDef type);

/**
 * @brief     the specifed timer stop working.
 * @param[in] type - select the timer to stop.
 * @return    none
 */
void timer_stop(TIMER_TypeDef type);

/**
 * @brief     This function performs to set sleep us.
 * @param[in] us - microseconds need to delay.
 * @return    none
 */
void sleep_us(unsigned long us);

#define ClockTime			clock_time
#define WaitUs				sleep_us
#define WaitMs(t)			sleep_us((t)*1000)
#define sleep_ms(t)			sleep_us((t)*1000)
#endif /* TIMER_H_ */
