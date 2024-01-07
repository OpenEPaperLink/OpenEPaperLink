/********************************************************************************************************
 * @file    drv_pm.c
 *
 * @brief   This is the source file for drv_pm
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

#if defined(__PROJECT_TL_BOOT_LOADER__) || defined(__PROJECT_TL_SNIFFER__)
	#define RF_RECOVERY()
#else
	#define RF_RECOVERY()					do{ \
												ZB_RADIO_INIT();	\
											}while(0) 
#endif

#define PM_FC_SET_FLAG						0x5A
#define PM_FC_CLR_FLAG						0x00

#if defined(MCU_CORE_826x)
	#define PM_ANA_REG_FRAME_COUNTER_FLAG	DEEP_ANA_REG0
	#define PM_ANA_REG_FRAME_COUNTER		DEEP_ANA_REG4
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	#define PM_ANA_REG_FRAME_COUNTER_FLAG	DEEP_ANA_REG0
	#define PM_ANA_REG_FRAME_COUNTER		DEEP_ANA_REG6
#elif defined(MCU_CORE_B91)
	#define PM_ANA_REG_FRAME_COUNTER_FLAG	PM_ANA_REG_POWER_ON_CLR_BUF1
	#define PM_ANA_REG_FRAME_COUNTER		PM_ANA_REG_POWER_ON_CLR_BUF2
#endif

static u32 prevSleepTick = 0;


u32 drv_pm_sleepTime_get(void)
{
	u32 sleepTime = 0;
#if defined(MCU_CORE_826x) || defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	sleepTime = (pm_get_32k_tick() - prevSleepTick) / 32;
#elif defined(MCU_CORE_B91)
	sleepTime = (clock_get_32k_tick() - prevSleepTick) / 32;
#endif
	return sleepTime;
}

void drv_pm_deepSleep_frameCnt_set(u32 frameCounter)
{
#if defined(MCU_CORE_826x) || defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	analog_write(PM_ANA_REG_FRAME_COUNTER_FLAG, PM_FC_SET_FLAG);

	analog_write(PM_ANA_REG_FRAME_COUNTER,     frameCounter);
	analog_write(PM_ANA_REG_FRAME_COUNTER + 1, frameCounter >> 8);
	analog_write(PM_ANA_REG_FRAME_COUNTER + 2, frameCounter >> 16);
	analog_write(PM_ANA_REG_FRAME_COUNTER + 3, frameCounter >> 24);
#elif defined(MCU_CORE_B91)
	analog_write_reg8(PM_ANA_REG_FRAME_COUNTER_FLAG, PM_FC_SET_FLAG);

	analog_write_reg8(PM_ANA_REG_FRAME_COUNTER,     frameCounter);
	analog_write_reg8(PM_ANA_REG_FRAME_COUNTER + 1, frameCounter >> 8);
	analog_write_reg8(PM_ANA_REG_FRAME_COUNTER + 2, frameCounter >> 16);
	analog_write_reg8(PM_ANA_REG_FRAME_COUNTER + 3, frameCounter >> 24);
#endif
}

u32 drv_pm_deepSleep_frameCnt_get(void)
{
#if defined(MCU_CORE_826x) || defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	analog_write(PM_ANA_REG_FRAME_COUNTER_FLAG, PM_FC_CLR_FLAG);
	return ((analog_read(PM_ANA_REG_FRAME_COUNTER+3) << 24) | (analog_read(PM_ANA_REG_FRAME_COUNTER+2) << 16) | (analog_read(PM_ANA_REG_FRAME_COUNTER+1) << 8) | analog_read(PM_ANA_REG_FRAME_COUNTER) );
#elif defined(MCU_CORE_B91)
	analog_write_reg8(PM_ANA_REG_FRAME_COUNTER_FLAG, PM_FC_CLR_FLAG);
	return ((analog_read_reg8(PM_ANA_REG_FRAME_COUNTER+3) << 24) | (analog_read_reg8(PM_ANA_REG_FRAME_COUNTER+2) << 16) | (analog_read_reg8(PM_ANA_REG_FRAME_COUNTER+1) << 8) | analog_read_reg8(PM_ANA_REG_FRAME_COUNTER) );
#endif
}

bool drv_pm_deepSleep_flag_get(void)
{
#if defined(MCU_CORE_826x) || defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	return ((pm_get_mcu_status() == MCU_STATUS_DEEP_BACK) && (analog_read(PM_ANA_REG_FRAME_COUNTER_FLAG) == PM_FC_SET_FLAG));
#elif defined(MCU_CORE_B91)
	return ((g_pm_status_info.mcu_status == MCU_STATUS_DEEP_BACK) && (analog_read_reg8(PM_ANA_REG_FRAME_COUNTER_FLAG) == PM_FC_SET_FLAG));
#endif
}

void drv_pm_sleep(drv_pm_sleep_mode_e mode, drv_pm_wakeup_src_e src, u32 durationMs)
{
#if defined(MCU_CORE_826x)
	u8 sleep_mode = SUSPEND_MODE;
	WakeupSrc_TypeDef srcType = 0;

	if(mode == PM_SLEEP_MODE_SUSPEND){
		sleep_mode = SUSPEND_MODE;
	}else if(mode == PM_SLEEP_MODE_DEEPSLEEP){
		sleep_mode = DEEPSLEEP_MODE;
	}else{
		return;
	}

	if(src & PM_WAKEUP_SRC_PAD){
		srcType |= WAKEUP_SRC_PAD;
	}
	if(src & PM_WAKEUP_SRC_TIMER){
		srcType |= WAKEUP_SRC_TIMER;
	}

	prevSleepTick = pm_get_32k_tick();

	pm_sleep_wakeup(sleep_mode, srcType, clock_time() + durationMs*1000*S_TIMER_CLOCK_1US);

	drv_pm_wakeupTimeUpdate();
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	SleepMode_TypeDef sleep_mode = SUSPEND_MODE;
	SleepWakeupSrc_TypeDef srcType = 0;

	if(mode == PM_SLEEP_MODE_SUSPEND){
		sleep_mode = SUSPEND_MODE;
	}else if(mode == PM_SLEEP_MODE_DEEPSLEEP){
		sleep_mode = DEEPSLEEP_MODE;
	}else if(mode == PM_SLEEP_MODE_DEEP_WITH_RETENTION){
		sleep_mode = DEEPSLEEP_MODE_RET_SRAM_LOW32K;
	}else{
		return;
	}

	if(src & PM_WAKEUP_SRC_PAD){
		srcType |= PM_WAKEUP_PAD;
	}
	if(src & PM_WAKEUP_SRC_TIMER){
		srcType |= PM_WAKEUP_TIMER;
	}

	prevSleepTick = pm_get_32k_tick();

	cpu_sleep_wakeup(sleep_mode, srcType, clock_time() + durationMs*1000*S_TIMER_CLOCK_1US);

	drv_pm_wakeupTimeUpdate();

	RF_RECOVERY();
#elif defined(MCU_CORE_B91)
	pm_sleep_mode_e sleep_mode = SUSPEND_MODE;
	pm_sleep_wakeup_src_e srcType = 0;

	if(mode == PM_SLEEP_MODE_SUSPEND){
		sleep_mode = SUSPEND_MODE;
	}else if(mode == PM_SLEEP_MODE_DEEPSLEEP){
		sleep_mode = DEEPSLEEP_MODE;
	}else if(mode == PM_SLEEP_MODE_DEEP_WITH_RETENTION){
		sleep_mode = DEEPSLEEP_MODE_RET_SRAM_LOW64K;
	}else{
		return;
	}

	if(src & PM_WAKEUP_SRC_PAD){
		srcType |= PM_WAKEUP_PAD;
	}
	if(src & PM_WAKEUP_SRC_TIMER){
		srcType |= PM_WAKEUP_TIMER;
	}

	prevSleepTick = clock_get_32k_tick();

	pm_sleep_wakeup(sleep_mode, srcType, PM_TICK_STIMER_16M, clock_time() + durationMs*1000*S_TIMER_CLOCK_1US);

	drv_pm_wakeupTimeUpdate();

	RF_RECOVERY();
#endif
}

void drv_pm_longSleep(drv_pm_sleep_mode_e mode, drv_pm_wakeup_src_e src, u32 durationMs)
{
#if defined(MCU_CORE_826x)
	u8 sleep_mode = SUSPEND_MODE;
	WakeupSrc_TypeDef srcType = 0;

	if(mode == PM_SLEEP_MODE_SUSPEND){
		sleep_mode = SUSPEND_MODE;
	}else if(mode == PM_SLEEP_MODE_DEEPSLEEP){
		sleep_mode = DEEPSLEEP_MODE;
	}else{
		return;
	}

	if(src & PM_WAKEUP_SRC_PAD){
		srcType |= WAKEUP_SRC_PAD;
	}
	if(src & PM_WAKEUP_SRC_TIMER){
		srcType |= WAKEUP_SRC_TIMER;
	}

	prevSleepTick = pm_get_32k_tick();

	pm_long_sleep_wakeup(sleep_mode, srcType, durationMs * 32);

	drv_pm_wakeupTimeUpdate();
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	SleepMode_TypeDef sleep_mode = SUSPEND_MODE;
	SleepWakeupSrc_TypeDef srcType = 0;

	if(mode == PM_SLEEP_MODE_SUSPEND){
		sleep_mode = SUSPEND_MODE;
	}else if(mode == PM_SLEEP_MODE_DEEPSLEEP){
		sleep_mode = DEEPSLEEP_MODE;
	}else if(mode == PM_SLEEP_MODE_DEEP_WITH_RETENTION){
		sleep_mode = DEEPSLEEP_MODE_RET_SRAM_LOW32K;
	}else{
		return;
	}

	if(src & PM_WAKEUP_SRC_PAD){
		srcType |= PM_WAKEUP_PAD;
	}
	if(src & PM_WAKEUP_SRC_TIMER){
		srcType |= PM_WAKEUP_TIMER;
	}

	prevSleepTick = pm_get_32k_tick();

	pm_long_sleep_wakeup(sleep_mode, srcType, durationMs * 32);

	drv_pm_wakeupTimeUpdate();

	RF_RECOVERY();
#elif defined(MCU_CORE_B91)
	pm_sleep_mode_e sleep_mode = SUSPEND_MODE;
	pm_sleep_wakeup_src_e srcType = 0;

	if(mode == PM_SLEEP_MODE_SUSPEND){
		sleep_mode = SUSPEND_MODE;
	}else if(mode == PM_SLEEP_MODE_DEEPSLEEP){
		sleep_mode = DEEPSLEEP_MODE;
	}else if(mode == PM_SLEEP_MODE_DEEP_WITH_RETENTION){
		sleep_mode = DEEPSLEEP_MODE_RET_SRAM_LOW64K;
	}else{
		return;
	}

	if(src & PM_WAKEUP_SRC_PAD){
		srcType |= PM_WAKEUP_PAD;
	}
	if(src & PM_WAKEUP_SRC_TIMER){
		srcType |= PM_WAKEUP_TIMER;
	}

	prevSleepTick = clock_get_32k_tick();

	pm_sleep_wakeup(sleep_mode, srcType, PM_TICK_32K, durationMs * 32);

	drv_pm_wakeupTimeUpdate();

	RF_RECOVERY();
#endif
}


void pm_wakeup_pad_cfg(u32 pin, drv_pm_wakeup_level_e pol, int en)
{
#if defined(MCU_CORE_826x)
	PM_PadSet(pin, pol, en);
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	cpu_set_gpio_wakeup(pin, pol, en);
#elif defined(MCU_CORE_B91)
	pm_set_gpio_wakeup(pin, pol, en);
#endif
}

u8 drv_pm_wakeupPinValid(drv_pm_pinCfg_t *pinCfg, u32 pinNum)
{
	drv_pm_pinCfg_t *p = pinCfg;

	for(u32 i = 0; i < pinNum; i++){
		if((p->wakeupLevel == PM_WAKEUP_LEVEL_HIGH) && drv_gpio_read(p->wakeupPin)){
			return SUCCESS;
		}

		if((p->wakeupLevel == PM_WAKEUP_LEVEL_LOW) && !drv_gpio_read(p->wakeupPin)){
			return SUCCESS;
		}
		p++;
	}
	return FAILURE;
}

void drv_pm_wakeupPinLevelChange(drv_pm_pinCfg_t *pinCfg, u32 pinNum)
{
	drv_pm_pinCfg_t *p = pinCfg;
	for(u32 i = 0; i < pinNum; i++){
		if((p->wakeupLevel == PM_WAKEUP_LEVEL_HIGH) && drv_gpio_read(p->wakeupPin)){
			p->wakeupLevel = PM_WAKEUP_LEVEL_LOW;
		}

		if((p->wakeupLevel == PM_WAKEUP_LEVEL_LOW) && !drv_gpio_read(p->wakeupPin)){
			p->wakeupLevel = PM_WAKEUP_LEVEL_HIGH;
		}
		pm_wakeup_pad_cfg(p->wakeupPin, p->wakeupLevel, 1);
		p++;
	}
}

void drv_pm_wakeupPinConfig(drv_pm_pinCfg_t *pinCfg, u32 pinNum)
{
	drv_pm_pinCfg_t *p = pinCfg;

	for(u32 i = 0; i < pinNum; i++){
		pm_wakeup_pad_cfg(p->wakeupPin, p->wakeupLevel, 1);
		p++;
	}
}

void drv_pm_lowPowerEnter(void)
{
	drv_pm_wakeup_src_e wakeupSrc = PM_WAKEUP_SRC_PAD;
	u32 sleepTime = 0;
	bool longSleep = 0;

	u32 r = drv_disable_irq();

	ev_timer_event_t *timerEvt = ev_timer_nearestGet();
	if(timerEvt){
		wakeupSrc |= PM_WAKEUP_SRC_TIMER;
		sleepTime = timerEvt->timeout;
	}

#if defined(MCU_CORE_826x)
	drv_pm_sleep_mode_e sleepMode = (wakeupSrc & PM_WAKEUP_SRC_TIMER) ? PM_SLEEP_MODE_SUSPEND : PM_SLEEP_MODE_DEEPSLEEP;
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278) || defined(MCU_CORE_B91)
	drv_pm_sleep_mode_e sleepMode = (wakeupSrc & PM_WAKEUP_SRC_TIMER) ? PM_SLEEP_MODE_DEEP_WITH_RETENTION : PM_SLEEP_MODE_DEEPSLEEP;
#endif

	if(sleepTime){
		if(sleepTime > PM_NORMAL_SLEEP_MAX){
			longSleep = 1;
		}else{
			longSleep = 0;
		}
	}


	if(!longSleep){
		drv_pm_sleep(sleepMode, wakeupSrc, sleepTime);
	}else{
		drv_pm_longSleep(sleepMode, wakeupSrc, sleepTime);
	}

	drv_restore_irq(r);
}

void drv_pm_wakeupTimeUpdate(void)
{
	u32 sleepTime = drv_pm_sleepTime_get();
	if(sleepTime){
		ev_timer_update(sleepTime);
		ev_timer_setPrevSysTick(clock_time());
	}
}
