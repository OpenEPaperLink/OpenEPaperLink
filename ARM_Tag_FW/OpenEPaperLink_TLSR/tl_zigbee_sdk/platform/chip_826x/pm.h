/********************************************************************************************************
 * @file    pm.h
 *
 * @brief   This is the header file for B86
 *
 * @author  Driver & Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *			All rights reserved.
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

#include "bit.h"
#include "gpio.h"


//power on reset
#define DEEP_ANA_REG0				0x3A//initial value =0x00
#define DEEP_ANA_REG1				0x3B//initial value =0x00

//watch dog reset
#define DEEP_ANA_REG4				0x34//initial value =0x00
#define DEEP_ANA_REG5				0x35//initial value =0x00
#define DEEP_ANA_REG6				0x36//initial value =0x00
#define DEEP_ANA_REG7				0x37//initial value =0x00
#define DEEP_ANA_REG8				0x38//initial value =0x00
#define DEEP_ANA_REG9				0x39//initial value =0xff


#define SYS_NEED_REINIT_EXT32K		BIT(0)
#define SYS_DEEP_SLEEP_FLAG			BIT(1)

//ana3b system used, user can not use
#define SYS_DEEP_ANA_REG 			DEEP_ANA_REG1


enum {
	// WAKEUP_SRC_ANA 0 -- 2  not supported
	 PM_WAKEUP_CORE  = BIT(5),
	 PM_WAKEUP_TIMER = BIT(6),
	 PM_WAKEUP_COMP  = BIT(7),
	 PM_WAKEUP_PAD   = BIT(8),
};

enum{
	// WAKEUP_SRC_ANA 0 -- 2  not supported
	 WAKEUP_PC3_GRP0 = BIT(0),
	 WAKEUP_PC4_GRP0 = BIT(1),
	 WAKEUP_PC5_GRP0 = BIT(2),
	 WAKEUP_PD0_GRP0 = BIT(3),
	 WAKEUP_PD1_GRP1 = BIT(4),
	 WAKEUP_PD2_GRP1 = BIT(5),
	 WAKEUP_PD3_GRP1 = BIT(6),
	 WAKEUP_PD4_GRP1 = BIT(7),
	 WAKEUP_PD5_GRP2 = BIT(8),
	 WAKEUP_PD6_GRP2 = BIT(9),
	 WAKEUP_PD7_GRP2 = BIT(10),
	 WAKEUP_PA0_GRP2 = BIT(11),
	 WAKEUP_PA1_GRP3 = BIT(12),
	 WAKEUP_PA2_GRP3 = BIT(13),
	 WAKEUP_PA3_GRP3 = BIT(14),
	 WAKEUP_PA4_GRP3 = BIT(15),
	 WAKEUP_PA7_GRP4 = BIT(16),
	 WAKEUP_PC6_GRP4 = BIT(17),
	 WAKEUP_PC7_GRP4 = BIT(18),
	 WAKEUP_PE0_GRP4 = BIT(19),
	 WAKEUP_PE1_GRP5 = BIT(20),
	 WAKEUP_PE2_GRP5 = BIT(21),
	 WAKEUP_PA5_GRP5 = BIT(22),
	 WAKEUP_PA6_GRP5 = BIT(23),
};
/*wakeup-level*/
enum{
	WAKEUP_GRP0_POS_EDG = 0,
	WAKEUP_GRP1_POS_EDG = 0,
	WAKEUP_GRP2_POS_EDG = 0,
	WAKEUP_GRP3_POS_EDG = 0,
	WAKEUP_GRP4_POS_EDG = 0,
	WAKEUP_GRP5_POS_EDG = 0,

	WAKEUP_GRP0_NEG_EDG = BIT(0),
	WAKEUP_GRP1_NEG_EDG = BIT(1),
	WAKEUP_GRP2_NEG_EDG = BIT(2),
	WAKEUP_GRP3_NEG_EDG = BIT(3),
	WAKEUP_GRP4_NEG_EDG = BIT(4),
	WAKEUP_GRP5_NEG_EDG = BIT(5),
};

typedef enum {
	 WAKEUP_SRC_PAD        = BIT(4),
	 WAKEUP_SRC_DIG_GPIO   = BIT(5) | 0x0800,
     WAKEUP_SRC_DIG_USB    = BIT(5) | 0x0400,
	 WAKEUP_SRC_TIMER      = BIT(6),
	 WAKEUP_SRC_COMP       = BIT(7),
}WakeupSrc_TypeDef;

enum {
	LOWPWR_SUSPEND		= 0,
	LOWPWR_DEEPSLEEP  	= 1,
};

enum {
	SUSPEND_MODE	 	= 0,
	DEEPSLEEP_MODE  	= 1,
};

enum {
	MCU_STATUS_BOOT	 		= 0,
	MCU_STATUS_DEEP_BACK  	= 1,
};

extern unsigned char pm_mcu_status;

static inline unsigned char pm_get_mcu_status(void)
{
	return pm_mcu_status;
}

extern void PM_PadSet(GPIO_PinTypeDef pin, int pol, int en);
extern void PM_GPIOSet(GPIO_PinTypeDef pin, int pol, int en);
extern int pm_sleep_wakeup(int DeepSleep, int WakeupSrc, unsigned long WakeupTick);
extern int pm_long_sleep_wakeup(int DeepSleep, int WakeupSrc, unsigned long WakeupTick);
extern void cpu_wakeup_init(void);
extern void sleep_start(void);
extern void suspend_start(void);
extern unsigned int pm_get_32k_tick(void);
