/********************************************************************************************************
 * @file	stimer.h
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
/**	@page STIMER
 *
 *	Introduction
 *	===============
 *	TLSRB91 stimer use 16M clock count, have stimer irq.
 *
 *	API Reference
 *	===============
 *	Header File: uart.h
 */
#ifndef STIMER_H_
#define STIMER_H_
#include "compiler.h"
#include "reg_include/stimer_reg.h"

/**********************************************************************************************************************
 *                                         global constants                                                           *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *                                           global macro                                                             *
 *********************************************************************************************************************/
#ifndef SYS_TIMER_AUTO_MODE
#define SYS_TIMER_AUTO_MODE     			1
#endif
/**********************************************************************************************************************
 *                                         global data type                                                           *
 *********************************************************************************************************************/
/**********************************************************************************************************************
 *                                     global variable declaration                                                    *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *                                      global function prototype                                                     *
 *********************************************************************************************************************/
/**
 * @brief define system clock tick per us/ms/s.
 */
enum{
	SYSTEM_TIMER_TICK_1US 		= 16,
	SYSTEM_TIMER_TICK_1MS 		= 16000,
	SYSTEM_TIMER_TICK_1S 		= 16000000,

	SYSTEM_TIMER_TICK_625US  	= 10000,  //625*16
	SYSTEM_TIMER_TICK_1250US 	= 20000,  //1250*16
};


/**
 * @brief This function servers to set stimer irq mask.
 * @param[in]	mask - the irq mask.
 * @return  	none.
 */
static inline void stimer_set_irq_mask(stimer_irq_e mask)
{
	reg_system_irq_mask |= mask;
}

/**
 * @brief This function servers to clear stimer irq mask.
 * @param[in] 	mask - the irq mask.
 * @return  	none.
 */
static inline void stimer_clr_irq_mask(stimer_irq_e mask)
{
	reg_system_irq_mask &= (~mask);
}

/**
 * @brief This function servers to clear stimer irq status.
 * @param[in] 	status - the irq status.
 * @return  	none.
 */
static inline void stimer_clr_irq_status(stimer_irq_e status)
{
	reg_system_cal_irq = (status);
}

/**
 * @brief This function servers to get stimer irq status.
 * @param[in] 	status - the irq status.
 * @return      none.
 */
static inline unsigned char stimer_get_irq_status(stimer_irq_e status)
{
	return (reg_system_cal_irq & status);
}

/**
 * @brief This function servers to set tick irq capture.
 * @param[in] tick - the value of irq tick.
 * @return    none.
 */
static inline void stimer_set_irq_capture(unsigned int tick)
{
	reg_system_irq_level = (tick);
}

/**
 * @brief This function servers to set stimer tick.
 * @param[in] tick - the value of tick.
 * @return    none.
 */
static inline void stimer_set_tick(unsigned int tick)
{
	reg_system_tick = (tick);
}

/**
 * @brief This function servers to enable stimer.
 * @return  none.
 */
static inline void stimer_enable(void)
{
#if SYS_TIMER_AUTO_MODE
	reg_system_ctrl |= FLD_SYSTEM_TIMER_AUTO;
#else
	reg_system_ctrl |= FLD_SYSTEM_TIMER_EN;
#endif
}


/**
 * @brief This function servers to disable stimer.
 * @return  none.
 */
static inline void stimer_disable(void)
{
#if SYS_TIMER_AUTO_MODE
	reg_system_ctrl &= ~(FLD_SYSTEM_TIMER_AUTO);
#else
	reg_system_ctrl &= ~(FLD_SYSTEM_TIMER_EN);
#endif
}

/*
 * @brief     This function performs to get system timer tick.
 * @return    system timer tick value.
**/
static inline unsigned int stimer_get_tick(void)
{

	return reg_system_tick;
}

/**
 * @brief     This function serves to set timeout by us.
 * @param[in] ref  - reference tick of system timer .
 * @param[in] us   - count by us.
 * @return    true - timeout, false - not timeout
 */
static inline _Bool clock_time_exceed(unsigned int ref, unsigned int us)
{
	return ((unsigned int)(stimer_get_tick() - ref) > us * SYSTEM_TIMER_TICK_1US);
}
/**
 * @brief     This function performs to set delay time by us.
 * @param[in] microsec - need to delay.
 * @return    none
 */
_attribute_ram_code_sec_noinline_   void delay_us(unsigned int microsec);


/**
 * @brief     This function performs to set delay time by ms.
 * @param[in] millisec - need to delay.
 * @return    none
 */
_attribute_ram_code_sec_noinline_  void  delay_ms(unsigned int millisec);


#endif /* STIMER_H_ */
