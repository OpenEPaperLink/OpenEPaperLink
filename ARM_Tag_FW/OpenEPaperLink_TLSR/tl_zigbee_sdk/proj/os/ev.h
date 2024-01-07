/********************************************************************************************************
 * @file    ev.h
 *
 * @brief   This is the header file for ev
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


enum {
	SYS_EXCEPTTION_COMMON_MEM_ACCESS = 0,
	SYS_EXCEPTTION_COMMON_TIMER_EVEVT,
	SYS_EXCEPTTION_COMMON_BUFFER_OVERFLOWN,
	SYS_EXCEPTTION_COMMON_STACK_OVERFLOWN,
	SYS_EXCEPTTION_COMMON_PARAM_ERROR,
	SYS_EXCEPTTION_COMMON_TASK_POST,

	SYS_EXCEPTTION_ZB_BUFFER_OVERFLOWN = 0x10,
	SYS_EXCEPTTION_ZB_BUFFER_EXCEPTION_FREE_OVERFLON,
	SYS_EXCEPTTION_ZB_BUFFER_EXCEPTION_FREE_MULIT,
	SYS_EXCEPTTION_ZB_BUFFER_EXCEPTION,
	SYS_EXCEPTTION_ZB_TIMER_TASK,
	SYS_EXCEPTTION_ZB_TASK_POST,

	SYS_EXCEPTTION_ZB_MAC_TX_IRQ = 0x20,
	SYS_EXCEPTTION_ZB_MAC_TX_TIMER,
	SYS_EXCEPTTION_ZB_MAC_CSMA,
	SYS_EXCEPTTION_ZB_MAC_TRX_TASK,

	SYS_EXCEPTTION_ZB_NWK_ADDR_IDX = 0x30,
	SYS_EXCEPTTION_ZB_NWK_BRC_INFO,
	SYS_EXCEPTTION_ZB_NWK_GET_ENTRY,
	SYS_EXCEPTTION_ZB_NWK_NEIGHBOR_TABLE,
	SYS_EXCEPTTION_ZB_NWK_ROUTE_TABLE,
	SYS_EXCEPTTION_ZB_NWK_LINK_STA_MEM_ALLOC_FAIL,

	SYS_EXCEPTTION_ZB_SS_KEY_INDEX = 0x40,

	SYS_EXCEPTTION_ZB_APS_PARAM = 0x50,
	SYS_EXCEPTTION_ZB_APS_FRAGMENT_TRANS,
	SYS_EXCEPTTION_ZB_APS_FRAGMENT_RCV,

	SYS_EXCEPTTION_ZB_ZCL_ENTRY = 0x60,

	SYS_EXCEPTTION_EV_BUFFER_OVERFLOWN = 0x70,
	SYS_EXCEPTTION_EV_BUFFER_EXCEPTION_FREE_OVERFLON,
	SYS_EXCEPTTION_EV_BUFFER_EXCEPTION_FREE_MULIT,
	SYS_EXCEPTTION_EV_TASK_POST,

	SYS_EXCEPTTION_NV_CHECK_TABLE_FULL = 0x80,
};

typedef void (*sys_exception_cb_t)(void);

void sys_exceptHandlerRegister(sys_exception_cb_t cb);

u8 sys_exceptionPost(u16 line, u8 evt);
#define ZB_EXCEPTION_POST(evt)  sys_exceptionPost(__LINE__, evt)

//will be called in every main loop
void ev_main(void);



