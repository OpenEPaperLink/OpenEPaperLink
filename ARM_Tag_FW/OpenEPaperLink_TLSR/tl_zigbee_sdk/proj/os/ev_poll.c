/********************************************************************************************************
 * @file    ev_poll.c
 *
 * @brief   This is the source file for ev_poll
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
#include "ev_poll.h"


ev_poll_t ev_poll[EV_POLL_MAX];


/**
 * @brief       Schedule a new priority task
 *
 * @param[in]   e   - Specified poll event ID
 * @param[in]   cb  - Pointer for the poll callback function
 *
 * @return      None
 */
void ev_on_poll(ev_poll_e e, ev_poll_callback_t cb)
{
	ev_poll[e].cb = cb;
	ev_poll[e].valid = 1;
}

/**
 * @brief       Enable the specified poll event
 *
 * @param[in]   e - Specified poll event ID
 *
 * @return      None
 */
void ev_enable_poll(ev_poll_e e)
{
	ev_poll[e].valid = 1;
}

/**
 * @brief       Schedule a LOW Medium task
 *
 * @param[in]   e - Specified poll event ID
 *
 * @return      None
 */
void ev_disable_poll(ev_poll_e e)
{
	ev_poll[e].valid = 0;
}

/**
 * @brief       Process poll events
 *
 * @param[in]   None
 *
 * @return      None
 */
void ev_poll_process(void){
    for(u8 i = 0; i < EV_POLL_MAX; i++){
        if(ev_poll[i].valid){
        	ev_poll[i].cb();
        }
    }
}

