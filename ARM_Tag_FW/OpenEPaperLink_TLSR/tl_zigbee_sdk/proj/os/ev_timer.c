/********************************************************************************************************
 * @file    ev_timer.c
 *
 * @brief   This is the source file for ev_timer
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
#include "ev_timer.h"

typedef struct{
	ev_timer_event_t *timer_head;		//timer events is sorted, use single linked list
	ev_timer_event_t *timer_nearest;	//the nearest fired timer

	ev_timer_event_pool_t timerEventPool;
}ev_timer_ctrl_t;

u8 g_ev_timer_maxNum = TIMER_EVENT_NUM;
ev_timer_ctrl_t ev_timer;

static u32 prevSysTick = 0;
static u32 remSysTick = 0;

void ev_timer_init(void)
{
	memset((u8 *)&ev_timer, 0, sizeof(ev_timer));
}

void ev_timer_setPrevSysTick(u32 tick)
{
	prevSysTick = tick;
}

ev_timer_event_t *ev_timer_nearestGet(void)
{
	return ev_timer.timer_nearest;
}

ev_timer_event_t *ev_timer_freeGet(void)
{
	if(ev_timer.timerEventPool.used_num >= g_ev_timer_maxNum){
		return NULL;
	}

	for(u8 i = 0; i < g_ev_timer_maxNum; i++){
		if(ev_timer.timerEventPool.evt[i].used == 0){
			ev_timer.timerEventPool.evt[i].used = 1;
			ev_timer.timerEventPool.used_num++;
			return &ev_timer.timerEventPool.evt[i];
		}
	}

	return NULL;
}

void ev_timer_poolDelUpdate(ev_timer_event_t *evt)
{
	evt->isRunning = 0;
	evt->curSysTick = 0;

	if(((u32)evt >= (u32)&ev_timer.timerEventPool.evt[0]) &&
	   ((u32)evt <= (u32)&ev_timer.timerEventPool.evt[g_ev_timer_maxNum - 1]) &&
	   (evt->used)){
		evt->used = 0;
		ev_timer.timerEventPool.used_num--;
	}
}

void ev_timer_nearestUpdate(void)
{
	ev_timer_event_t *timerEvt = ev_timer.timer_head;

	ev_timer.timer_nearest = ev_timer.timer_head;

	while(timerEvt){
		if(timerEvt->timeout < ev_timer.timer_nearest->timeout){
			ev_timer.timer_nearest = timerEvt;
		}
		timerEvt = timerEvt->next;
	}
}

bool ev_timer_enough(void)
{
	if(ev_timer.timerEventPool.used_num < TIMER_EVENT_ENOUGH_NUM){
		return TRUE;
	}
	return FALSE;
}

bool ev_timer_exist(ev_timer_event_t *evt)
{
	ev_timer_event_t *timerEvt = ev_timer.timer_head;

    while(timerEvt){
        if(timerEvt == evt){
            return TRUE;
        }
        timerEvt = timerEvt->next;
    }
    return FALSE;
}

void ev_on_timer(ev_timer_event_t *evt, u32 timeout)
{
	if(!evt){
		ZB_EXCEPTION_POST(SYS_EXCEPTTION_COMMON_TIMER_EVEVT);
		return;
	}

	evt->period = timeout;

	u32 r = drv_disable_irq();

	ev_timer_event_t *out = NULL;

	LIST_EXIST(ev_timer.timer_head, evt, out);
	if(out){
		out->timeout = evt->period;
	}else{
		if(evt->isRunning == 0){
			evt->curSysTick = clock_time();
		}
		evt->timeout = evt->period;
		LIST_ADD(ev_timer.timer_head, evt);
	}

	ev_timer_nearestUpdate();

	drv_restore_irq(r);
}

void ev_unon_timer(ev_timer_event_t *evt)
{
	if(!evt){
		ZB_EXCEPTION_POST(SYS_EXCEPTTION_COMMON_TIMER_EVEVT);
		return;
	}

	ev_timer_event_t *out = NULL;
	LIST_EXIST(ev_timer.timer_head, evt, out);
	if(!out){
		return;
	}

	u32 r = drv_disable_irq();

	ev_timer_poolDelUpdate(evt);

	LIST_DELETE(ev_timer.timer_head, evt);

	ev_timer_nearestUpdate();

	drv_restore_irq(r);
}

ev_timer_event_t *ev_timer_add(ev_timer_callback_t func, void *arg, u32 timeout)
{
	ev_timer_event_t *timerEvt = ev_timer_freeGet();
	if(!timerEvt){
		ZB_EXCEPTION_POST(SYS_EXCEPTTION_COMMON_TIMER_EVEVT);
		return NULL;
	}

	timerEvt->cb = func;
	timerEvt->data = arg;

	ev_on_timer(timerEvt, timeout);

	return timerEvt;
}

ev_timer_event_t *ev_timer_taskPost(ev_timer_callback_t func, void *arg, u32 t_ms)
{
	ev_timer_event_t *timerEvt = NULL;

	u32 r = drv_disable_irq();

	timerEvt = ev_timer_add(func, arg, t_ms);

	drv_restore_irq(r);

	return timerEvt;
}

u8 ev_timer_taskCancel(ev_timer_event_t **evt)
{
	u8 ret = NO_TIMER_AVAIL;
	ev_timer_event_t *timerEvt = *evt;

	if(!timerEvt || !timerEvt->used){
		return ret;
	}

	ev_unon_timer(timerEvt);

	*evt = NULL;

	return SUCCESS;
}

void ev_timer_update(u32 updateTime)
{
	u32 updateTimeMs = 0;
	u32 curSysTick = clock_time();

	if(updateTime == 0){
		return;
	}

	u32 r = drv_disable_irq();

	ev_timer_event_t *timerEvt = ev_timer.timer_head;

	while(timerEvt){
		if(timerEvt->isRunning){
			/* Elapsed time, including time that may be blocked. */
			updateTimeMs = updateTime;
		}else{
			/* Just calculate the elapsed time from the time of registration to the present. */
			updateTimeMs = (curSysTick - timerEvt->curSysTick) / (S_TIMER_CLOCK_1US * 1000);

			timerEvt->isRunning = 1;
		}

		if(timerEvt->timeout > updateTimeMs){
			timerEvt->timeout -= updateTimeMs;
		}else{
			timerEvt->timeout = 0;
		}

		timerEvt = timerEvt->next;
	}

	drv_restore_irq(r);
}

void ev_timer_executeCB(void)
{
	ev_timer_event_t *timerEvt = ev_timer.timer_head;
	ev_timer_event_t *prev_head = timerEvt;

	while(timerEvt){
		if(timerEvt->timeout == 0){
			s32 t = timerEvt->cb(timerEvt->data);

			if(t < 0){
				ev_unon_timer(timerEvt);
			}else if(t == 0){
				timerEvt->timeout = timerEvt->period;
			}else{
				timerEvt->period = (u32)t;
				timerEvt->timeout = timerEvt->period;
			}

			if(prev_head != ev_timer.timer_head){
				timerEvt = ev_timer.timer_head;
				prev_head = timerEvt;
			}else{
				timerEvt = timerEvt->next;
			}
		}else{
			timerEvt = timerEvt->next;
		}
	}

	ev_timer_nearestUpdate();
}

void ev_timer_process(void)
{
	u32 updateTime = 0;
	u32 sysTicks = 0;
	u32 currSysTick = clock_time();

	if(currSysTick != prevSysTick){
		sysTicks = (u32)(currSysTick - prevSysTick);

		/* store current ticks. */
		prevSysTick = currSysTick;

		updateTime = sysTicks / (S_TIMER_CLOCK_1US * 1000);
		remSysTick += sysTicks % (S_TIMER_CLOCK_1US * 1000);

		updateTime += remSysTick / (S_TIMER_CLOCK_1US * 1000);
		remSysTick = remSysTick % (S_TIMER_CLOCK_1US * 1000);

		/* more than 1 ms. */
		if(updateTime){
			ev_timer_update(updateTime);
			ev_timer_executeCB();
		}
	}
}
