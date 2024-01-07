/********************************************************************************************************
 * @file    drv_pm.h
 *
 * @brief   This is the header file for drv_pm
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




typedef enum{
	PM_SLEEP_MODE_SUSPEND,
	PM_SLEEP_MODE_DEEPSLEEP,
	PM_SLEEP_MODE_DEEP_WITH_RETENTION,	//826x not support

	PM_SLEEP_MODE_MCU_STALL,
}drv_pm_sleep_mode_e;

typedef enum{
	PM_WAKEUP_SRC_PAD 	= BIT(0),
	PM_WAKEUP_SRC_TIMER = BIT(1),
}drv_pm_wakeup_src_e;

typedef enum{
	PM_WAKEUP_LEVEL_LOW,
	PM_WAKEUP_LEVEL_HIGH,
}drv_pm_wakeup_level_e;

typedef struct{
	u32 wakeupPin;
	drv_pm_wakeup_level_e wakeupLevel;
}drv_pm_pinCfg_t;

#if defined(MCU_CORE_826x)
	#define PM_CLOCK_INIT()					do{ rc_32k_cal(); }while(0)

	#define PM_NORMAL_SLEEP_MAX				(100 * 1000)//100s, (0xC0000000 / 32)
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	#define PM_CLOCK_INIT()					do{ \
												/* Initialize 32K for timer wakeup. */	\
												clock_32k_init(CLK_32K_RC);				\
												rc_32k_cal();							\
												pm_select_internal_32k_rc();			\
											}while(0)

	#define PM_NORMAL_SLEEP_MAX				(230 * 1000)//230s, (0xE0000000 / 16)
#elif defined(MCU_CORE_B91)
	/* 24M RC is inaccurate, and it is greatly affected by temperature, so real-time calibration is required
	 * The 24M RC needs to be calibrated before the pm_sleep_wakeup function,
	 * because this clock will be used to kick 24m xtal start after wake up.
	 * The more accurate this time, the faster the crystal will start. Calibration cycle depends on usage
	 */
	#define PM_CLOCK_INIT()					do{ \
												clock_cal_24m_rc();						\
												/* Initialize 32K for timer wakeup. */	\
												clock_32k_init(CLK_32K_RC);				\
												clock_cal_32k_rc();/*6.68ms*/			\
											}while(0)

	#define PM_NORMAL_SLEEP_MAX				(230 * 1000)//230s, (0xE0000000 / 16)
#endif


void drv_pm_deepSleep_frameCnt_set(u32 frameCounter);
u32 drv_pm_deepSleep_frameCnt_get(void);
bool drv_pm_deepSleep_flag_get(void);

void pm_wakeup_pad_cfg(u32 pin, drv_pm_wakeup_level_e pol, int en);
void drv_pm_wakeupPinConfig(drv_pm_pinCfg_t *pinCfg, u32 pinNum);
u8 drv_pm_wakeupPinValid(drv_pm_pinCfg_t *pinCfg, u32 pinNum);

void drv_pm_sleep(drv_pm_sleep_mode_e mode, drv_pm_wakeup_src_e src, u32 durationMs);
void drv_pm_longSleep(drv_pm_sleep_mode_e mode, drv_pm_wakeup_src_e src, u32 durationMs);

void drv_pm_lowPowerEnter(void);
void drv_pm_wakeupTimeUpdate(void);

void drv_pm_wakeupPinLevelChange(drv_pm_pinCfg_t *pinCfg, u32 pinNum);


