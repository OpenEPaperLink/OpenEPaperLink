/********************************************************************************************************
 * @file	timer_reg.h
 *
 * @brief	This is the header file for B91
 *
 * @author	Driver Group
 * @date	2019
 *
 * @par     Copyright (c) 2019, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#ifndef TIMER_REG_H
#define TIMER_REG_H
#include "soc.h"

/*******************************      timer registers: 0x140140     ******************************/
#define reg_tmr_ctrl0			REG_ADDR8(0x140140)
#define reg_tmr_ctrl16			REG_ADDR16(0x140140)
#define reg_tmr_ctrl32			REG_ADDR32(0x140140)
enum{
	FLD_TMR0_MODE =				BIT_RNG(0,1),
	FLD_TMR0_EN =				BIT(2),
	FLD_TMR0_NOWRAP = 			BIT(3),
	FLD_TMR1_MODE =				BIT_RNG(4,5),
	FLD_TMR1_EN =				BIT(6),
	FLD_TMR1_NOWRAP  = 			BIT(7),
};

#define reg_tmr_ctrl2			REG_ADDR8(0x140142)

enum{
	FLD_TMR_WD_EN =				BIT(7),
};

#define reg_tmr_sta				REG_ADDR8(0x140143)
enum{
	FLD_TMR_STA_TMR0   =		BIT(0),
	FLD_TMR_STA_TMR1   =		BIT(1),
	FLD_TMR_STA_WD     =		BIT(2),
	FLD_TMR_WD_CNT_CLR =		BIT(3),
	FLD_TMR_SW_IRQ     =		BIT(7),
};

#define  reg_tmr0_capt          REG_ADDR32(0x140144)
#define  reg_tmr1_capt          REG_ADDR32(0x140148)
#define  reg_tmr_capt(i)		REG_ADDR32(0x140144 + ((i) << 2))

#define  reg_wt_target0         REG_ADDR8(0x14014c)// always is 0x00
#define  reg_wt_target1         REG_ADDR8(0x14014d)
#define  reg_wt_target2         REG_ADDR8(0x14014e)
#define  reg_wt_target3         REG_ADDR8(0x14014f)
#define  reg_wt_target          REG_ADDR32(0x14014c)


#define reg_tmr0_tick			REG_ADDR32(0X140150)
#define reg_tmr1_tick			REG_ADDR32(0X140154)
#define reg_tmr_tick(i)			REG_ADDR32(0X140150 + ((i) << 2))

#endif
