/********************************************************************************************************
 * @file    ev_timer.h
 *
 * @brief   This is the header file for ev_timer
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


/**
 *  @brief Definition for timer event
 */
#define TIMER_EVENT_NUM				(24)
#define TIMER_EVENT_ENOUGH_NUM		(TIMER_EVENT_NUM - 4)

/**
 *  @brief Type definition for timer callback function
 */
typedef int (*ev_timer_callback_t)(void *data);
 
/**
 *  @brief Type definition for timer event
 */
typedef struct ev_timer_event_t {
	struct ev_timer_event_t *next;  //!< Used internal

    ev_timer_callback_t     cb;     //!< Callback function when expire, this must be specified
    void *data;         			//!< Callback function arguments.
    u32 timeout;             		//!< In millisecond
    u32 period;      				//!< Used internal
    u32 curSysTick;					//!< Used internal, system tick when registering
	u8 resv[2];
	u8 isRunning;					//!< Used internal
    u8 used;						//!< Used internal
} ev_timer_event_t;

typedef struct ev_timer_event_pool_s {
	ev_timer_event_t evt[TIMER_EVENT_NUM];
    u32 used_num;
} ev_timer_event_pool_t;


bool ev_timer_enough(void);
void ev_timer_update(u32 updateTime);
void ev_timer_setPrevSysTick(u32 tick);

/**
 * @brief       EV timer pool initialization
 *
 * @param[in]   None
 *
 * @return      None
 */
void ev_timer_init(void);

/**
 * @brief       Process EV timer events
 *
 * @param[in]   None
 *
 * @return      None
 */
void ev_timer_process(void);

/**
 * @brief       Get the nearest EV timer events
 *
 * @param[in]   None
 *
 * @return      None
 */
ev_timer_event_t *ev_timer_nearestGet(void);

/**
  * @brief       Check whether a specified timer exist or not
  *
  * @param[in]   evt  - The specified timer event
  *
  * @return      True indicating the timer is already exist. <BR>
  *              False indicating the timer is not exist. <BR>
  */
bool ev_timer_exist(ev_timer_event_t *evt);

/**
  * @brief       Set a new timer
  *
  * @param[in]   evt  		- The timer event including the callback function
  * @param[in]   timeout 	- Timeout in millisecond
  *
  * @return      None
  */
void ev_on_timer(ev_timer_event_t *evt, u32 timeout);

/**
  * @brief       Cancel an existed timer
  *
  * @param[in]   e  - The existed timer event
  *
  * @return      None
  */
void ev_unon_timer(ev_timer_event_t *evt);

/**
  * @brief       push timer task to task list
  *
  * @param[in]   func - the callback of the timer event
  *
  * @param[in]   arg - the parameter to the callback
  *
  * @param		 cycle - the timer interval
  *
  * @return      the status
  */
ev_timer_event_t *ev_timer_taskPost(ev_timer_callback_t func, void *arg, u32 t_ms);
#define TL_ZB_TIMER_SCHEDULE(cb, arg, timeout) 	(ev_timer_taskPost((cb), (arg), (timeout)))

/**
  * @brief       cancel timer task from task list
  *
  * @param[in]   te - the pointer to the the timer event pointer
  *
  * @return      the status
  */
u8 ev_timer_taskCancel(ev_timer_event_t **evt);
#define TL_ZB_TIMER_CANCEL(evt)	(ev_timer_taskCancel(evt))
