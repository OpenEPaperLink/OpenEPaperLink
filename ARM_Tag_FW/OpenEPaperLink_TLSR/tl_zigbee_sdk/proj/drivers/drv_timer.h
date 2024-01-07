/********************************************************************************************************
 * @file    drv_timer.h
 *
 * @brief   This is the header file for drv_timer
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
	#define S_TIMER_CLOCK_1US				sysTimerPerUs
	#define H_TIMER_CLOCK_1US				(CLOCK_SYS_CLOCK_HZ / 1000000)
#elif defined(MCU_CORE_B91)
	#define S_TIMER_CLOCK_1US				sysTimerPerUs
	#define H_TIMER_CLOCK_1US				(sys_clk.pclk)

	#define clock_time()					stimer_get_tick()
	#define WaitUs(us)						delay_us(us)
	#define WaitMs(ms)						delay_ms(ms)
#endif

#define TIMER_TICK_1US_GET(idx)				((idx < TIMER_IDX_3) ? H_TIMER_CLOCK_1US : S_TIMER_CLOCK_1US)
 
/**
 * hardware_timer_index Hardware Timer Index
 */
#define TIMER_IDX_0             			0 //!< Timer0
#define TIMER_IDX_1             			1 //!< Timer1
#define TIMER_IDX_2             			2 //!< Timer2, for Watch dog.
#define TIMER_IDX_3							3 //!< SYS Timer, for MAC-CSMA.
#define TIMER_NUM               			4 //!< Total number of timer


/**
 * hardware_timer_mode Hardware Timer Mode
 */
#define TIMER_MODE_SCLK         			0 //!< Timer running in the system clock mode, it will free run from 0 to 0xffffffff
#define TIMER_MODE_GPIO         			1
#define TIMER_MODE_WIDTH_GPIO   			2
#define TIMER_MODE_TICK_MODE    			3

/**
 * hardware_timer_state Hardware Timer State
 */
#define TIMER_IDLE              			0 //!< Indicating the timer is not running
#define TIMER_WOF               			1 //!< Waiting for overflow
#define TIMER_WTO               			2 //!< Waiting for time out


/**
 *  @brief  Definition for 64 bit timer unit
 */
typedef struct{
    u32 low;
    u32 high;
}ext_clk_t;

/**
 *  @brief  Status of Hardware Timer
 */
typedef enum hw_timer_sts_e{
	HW_TIMER_SUCC 		= 0,
    HW_TIMER_IS_RUNNING = 1,
	HW_TIMER_INVALID,
}hw_timer_sts_t;

/**
 *  @brief  Definition for Timer callback function type
 */
typedef int (*timerCb_t)(void *p);


 /**
  * @brief       Initialize the specified hardware timer
  *
  * @param[in]   tmrIdx - Index of timer @ref hardware_timer_index
  * @param[in]   mode   - Specify the timer running mode @ref hardware_timer_mode
  *
  * @return      None
  */
void drv_hwTmr_init(u8 tmrIdx, u8 mode);

 /**
  * @brief       Set a new timer through relative time value
  *
  * @param[in]   tmrIdx   - Index of timer @ref hardware_timer_index
  * @param[in]   interval - The time interval from current time. The unit is microsecond
  * @param[in]   func     - Pointer to the callback function to be invoked when the timer expiry
  * @param[in]   arg      - Argument of callback function
  *
  * @return      Status
  */
hw_timer_sts_t drv_hwTmr_set(u8 tmrIdx, u32 t_us, timerCb_t func, void *arg);

/**
  * @brief       Cancel an existed timer
  *
  * @param[in]   tmrIdx  - Index of timer @ref hardware_timer_index
  *
  * @return      None
  */
void drv_hwTmr_cancel(u8 tmrIdx);

/**
  * @brief       Interrupt handler of Timer
  *
  * @param       None
  *
  * @return      None
  */
void drv_timer_irq0_handler(void);
void drv_timer_irq1_handler(void);
void drv_timer_irq3_handler(void);



