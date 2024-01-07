/********************************************************************************************************
 * @file    ev_poll.h
 *
 * @brief   This is the header file for ev_poll
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
 *  @brief Definition for poll callback function 
 */
typedef void (*ev_poll_callback_t)(void);
 
/**
 *  @brief Definition for each poll item 
 */
typedef struct ev_poll_s {
    ev_poll_callback_t  cb;                 //!< Callback of the poll task
    u32                 valid;              //!< Indicate the poll task is used or not
} ev_poll_t; 


extern ev_poll_t ev_poll[];

/**
 * @brief       Process poll events
 *
 * @param[in]   None
 *
 * @return      None
 */
void ev_poll_process(void);

/**
 * @brief       Schedule a new priority task
 *
 * @param[in]   e   - Specified poll event ID
 * @param[in]   cb  - Pointer for the poll callback function
 *
 * @return      None
 */
void ev_on_poll(ev_poll_e e, ev_poll_callback_t cb);

/**
 * @brief       Enable the specified poll event
 *
 * @param[in]   e - Specified poll event ID
 *
 * @return      None
 */
void ev_enable_poll(ev_poll_e e);

/**
 * @brief       Schedule a LOW Medium task
 *
 * @param[in]   e - Specified poll event ID
 *
 * @return      None
 */
void ev_disable_poll(ev_poll_e e);

 /**
  * @brief       Schedule a Poll Task
  *
  * @param[in]   e    - Specified poll event ID
  * @param[in]   task - Pointer for the task callback function
  *
  * @return      None
  */
#define EV_SCHEDULE_POLL(e, task) (ev_on_poll(e, task))

