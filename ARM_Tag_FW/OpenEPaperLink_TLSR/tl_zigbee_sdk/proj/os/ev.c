/********************************************************************************************************
 * @file    ev.c
 *
 * @brief   This is the source file for ev
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
#include "ev.h"

sys_exception_cb_t g_sysExceptCallbak = NULL;


volatile u16 T_evtExcept[4] = {0};
u8 sys_exceptionPost(u16 line, u8 evt)
{
	T_evtExcept[0] = line;
	T_evtExcept[1] = evt;

	/* TODO: some information stored in NV */
	if(g_sysExceptCallbak){
		g_sysExceptCallbak();
	}

	return 0;
}

void sys_stackStatusCheck(void)
{
	extern u32 _end_bss_;
	u8 *stackEnd = (u8*)&_end_bss_;
	u8 stackOverflown = 0;
	for(s32 i = 0; i < 4; i++){
		if(stackEnd[i] != 0xff){
			/* stack overflown */
			stackOverflown = 1;
			break;
		}
	}

	if(stackOverflown){
		ZB_EXCEPTION_POST(SYS_EXCEPTTION_COMMON_STACK_OVERFLOWN);
	}
}

void sys_exceptHandlerRegister(sys_exception_cb_t cb)
{
	g_sysExceptCallbak = cb;
}

void ev_main(void)
{
	ev_timer_process();
    ev_poll_process();

	//sys_stackStatusCheck();
}

