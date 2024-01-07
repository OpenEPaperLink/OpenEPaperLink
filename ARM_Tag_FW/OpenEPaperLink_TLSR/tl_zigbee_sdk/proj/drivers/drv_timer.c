/********************************************************************************************************
 * @file    drv_timer.c
 *
 * @brief   This is the source file for drv_timer
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


#define TIMER_SAFE_BOUNDARY_IN_US(v)		(10 * v)
#define TIMER_OVERFLOW_VALUE        		(0xFFFFFFFE)


typedef struct{
    /* Expire time and callback parameters */
    ext_clk_t expireInfo;
    timerCb_t cb;
    void	  *arg;

    /* Flags for useful information */
    union{
        struct{
            u32 status:2;
            u32 reserved:30;
        }bf;
        u32 byteVal;
    }flags;
}hwTmr_info_t;

typedef struct{
    hwTmr_info_t timerInfo[TIMER_NUM];
}hwTmr_ctrl_t;


hwTmr_ctrl_t hwTmr_vars;


static void hwTimerInit(u8 tmrIdx, u8 mode)
{
#if defined(MCU_CORE_826x) || defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	if(tmrIdx < TIMER_IDX_3){
		timer_set_mode(tmrIdx, mode);
		timer_irq_enable(tmrIdx);
	}else{
		stimer_irq_enable();
	}
#elif defined(MCU_CORE_B91)
	if(tmrIdx < TIMER_IDX_3){
		timer_set_mode(tmrIdx, mode);
		if(tmrIdx == TIMER_IDX_0){
			plic_interrupt_enable(IRQ4_TIMER0);
		}else if(tmrIdx == TIMER_IDX_1){
			plic_interrupt_enable(IRQ3_TIMER1);
		}
	}else{
		plic_interrupt_enable(IRQ1_SYSTIMER);
	}
#endif
}

static void hwTimerSet(u8 tmrIdx, u32 tick)
{
	/* Set capture tick value */
	if(tmrIdx < TIMER_IDX_3){
		timer_set_init_tick(tmrIdx, 0);
		timer_set_cap_tick(tmrIdx, tick);
	}else{
		stimer_set_irq_capture(tick + clock_time());
	}
}

static void hwTimerStart(u8 tmrIdx)
{
	if(tmrIdx < TIMER_IDX_3){
		timer_start(tmrIdx);
	}else{
#if defined(MCU_CORE_826x) || defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
		stimer_set_irq_mask();
#elif defined(MCU_CORE_B91)
		stimer_set_irq_mask(FLD_SYSTEM_IRQ);
#endif
	}
}

static void hwTimerStop(u8 tmrIdx)
{
	if(tmrIdx < TIMER_IDX_3){
		timer_stop(tmrIdx);
	}else{
#if defined(MCU_CORE_826x) || defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
		stimer_clr_irq_mask();
#elif defined(MCU_CORE_B91)
		stimer_clr_irq_mask(FLD_SYSTEM_IRQ);
#endif
	}
}


static hw_timer_sts_t hwTmr_setAbs(u8 tmrIdx, ext_clk_t *absTimer, timerCb_t func, void *arg)
{
    hwTmr_info_t *pTimer = &hwTmr_vars.timerInfo[tmrIdx];

    /* Validation */
    if(TIMER_IDLE != pTimer->flags.bf.status){
        return HW_TIMER_IS_RUNNING;
    }

    u32 r = drv_disable_irq();

    /* Set expire information */
    pTimer->expireInfo.high = absTimer->high;
    pTimer->expireInfo.low  = absTimer->low;
    pTimer->cb = func;
    pTimer->arg = arg;

    pTimer->flags.bf.status = pTimer->expireInfo.high ? TIMER_WOF : TIMER_WTO;

    /* Safety Check - If time is already past, set timer as Expired */
    if(!pTimer->expireInfo.high && pTimer->expireInfo.low < TIMER_SAFE_BOUNDARY_IN_US(TIMER_TICK_1US_GET(tmrIdx))){
    	drv_restore_irq(r);
        memset(pTimer, 0, sizeof(hwTmr_info_t));
		if(func){
            func(arg);
		}
        return HW_TIMER_SUCC;
    }else{
    	hwTimerSet(tmrIdx, pTimer->expireInfo.high ? TIMER_OVERFLOW_VALUE : pTimer->expireInfo.low);
    	hwTimerStart(tmrIdx);
    }

    drv_restore_irq(r);

    return HW_TIMER_SUCC;
}

static void drv_hwTmr_irq_process(u8 tmrIdx)
{
    hwTmr_info_t *pTimer = &hwTmr_vars.timerInfo[tmrIdx];

    if(TIMER_WTO == pTimer->flags.bf.status){
        /* Expired, callback */
        if(pTimer->cb){
        	int t;

        	t = pTimer->cb(pTimer->arg);

        	if(t < 0){
        		hwTimerStop(tmrIdx);
        		memset(pTimer, 0, sizeof(hwTmr_info_t));
        	}else{
        		if(t != 0){
        			pTimer->expireInfo.low = t * TIMER_TICK_1US_GET(tmrIdx);
        		}
        		/* Set the capture tick again. */
        		hwTimerSet(tmrIdx, pTimer->expireInfo.high ? TIMER_OVERFLOW_VALUE : pTimer->expireInfo.low);
        	}
        }
    }else{
        if(--pTimer->expireInfo.high){
        	hwTimerSet(tmrIdx, TIMER_OVERFLOW_VALUE);
        }else{
        	hwTimerSet(tmrIdx, pTimer->expireInfo.low);

            pTimer->flags.bf.status = TIMER_WTO;
        }
    }
}

static void hwTimerInfoReset(u8 tmrIdx)
{
	memset((u8*)&hwTmr_vars.timerInfo[tmrIdx], 0, sizeof(hwTmr_info_t));
}

void drv_hwTmr_init(u8 tmrIdx, u8 mode)
{
	if(tmrIdx >= TIMER_NUM){
		return;
	}

	hwTimerInfoReset(tmrIdx);
	hwTimerInit(tmrIdx, mode);
}

void drv_hwTmr_cancel(u8 tmrIdx)
{
	if(tmrIdx >= TIMER_NUM){
		return;
	}

	u32 r = drv_disable_irq();

	hwTimerStop(tmrIdx);

	hwTmr_info_t *pTimer = &hwTmr_vars.timerInfo[tmrIdx];
	memset(pTimer, 0, sizeof(hwTmr_info_t));

	drv_restore_irq(r);
}

hw_timer_sts_t drv_hwTmr_set(u8 tmrIdx, u32 t_us, timerCb_t func, void *arg)
{
	if(tmrIdx >= TIMER_NUM){
		return HW_TIMER_INVALID;
	}

    ext_clk_t t;
    t.high = 0;
    t.low = t_us * TIMER_TICK_1US_GET(tmrIdx);

    return hwTmr_setAbs(tmrIdx, &t, func, arg);
}



void drv_timer_irq0_handler(void)
{
	drv_hwTmr_irq_process(TIMER_IDX_0);
}

void drv_timer_irq1_handler(void)
{
	drv_hwTmr_irq_process(TIMER_IDX_1);
}

void drv_timer_irq3_handler(void)
{
	drv_hwTmr_irq_process(TIMER_IDX_3);
}

