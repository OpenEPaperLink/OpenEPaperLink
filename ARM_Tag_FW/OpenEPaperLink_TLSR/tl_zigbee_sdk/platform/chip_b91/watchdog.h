/********************************************************************************************************
 * @file	watchdog.h
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
#ifndef WATCHDOG_H_
#define WATCHDOG_H_
#include "analog.h"
#include "gpio.h"

/**
 * @brief     start watchdog.
 * @return    none
 */
static inline void wd_start(void){

	BM_SET(reg_tmr_ctrl2, FLD_TMR_WD_EN);
}


/**
 * @brief     stop watchdog.
 * @return    none
 */
static inline void wd_stop(void){
	BM_CLR(reg_tmr_ctrl2, FLD_TMR_WD_EN);
}


/**
 * @brief     clear watchdog.
 * @return    none
 */
static inline void wd_clear(void)
{
	reg_tmr_sta = FLD_TMR_STA_WD|FLD_TMR_WD_CNT_CLR;

}

/**
 * @brief     clear watchdog timer tick cnt.
 * @return    none
 */
static inline void wd_clear_cnt(void)
{
	reg_tmr_sta = FLD_TMR_WD_CNT_CLR;

}

/**
 * @brief     This function set the watchdog trigger time.
 * 			  Because the lower 8bit of the wd timer register will always be 0, there will be an error ,
			  The time error = (0x00~0xff)/(APB clock frequency)
 * @param[in] period_ms - The watchdog trigger time. Unit is  millisecond
 * @return    none
 * @attention  The clock source of watchdog comes from pclk, when pclk changes it needs to be reconfigured.
 */
static inline void wd_set_interval_ms(unsigned int period_ms)
{
	static unsigned int tmp_period_ms = 0;
	tmp_period_ms=period_ms*sys_clk.pclk*1000;
	reg_wt_target=tmp_period_ms;
}

#endif
