/********************************************************************************************************
 * @file	stimer_reg.h
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
#ifndef STIMER_REG_H_
#define STIMER_REG_H_

#include "soc.h"
/*******************************      sys clock registers: 0x140200       ******************************/
#define STIMER_BASE_ADDR			   	0x140200
#define reg_system_tick         		REG_ADDR32(STIMER_BASE_ADDR)

#define reg_system_irq_level         	REG_ADDR32(STIMER_BASE_ADDR+0x4)

#define reg_system_irq_mask				REG_ADDR8(STIMER_BASE_ADDR+0x8)
enum{
	FLD_SYSTEM_IRQ_MASK 	= 	BIT_RNG(0,2),
	FLD_SYSTEM_TRIG_PAST_EN = 	BIT(3),
};

#define reg_system_cal_irq		REG_ADDR8(STIMER_BASE_ADDR+0x9)

typedef enum{
	FLD_SYSTEM_IRQ  		= 	BIT(0),
	FLD_SYSTEM_32K_IRQ  	= 	BIT(1),
}stimer_irq_e;

#define reg_system_ctrl		    REG_ADDR8(STIMER_BASE_ADDR+0xa)
enum{
	FLD_SYSTEM_32K_WR_EN 		= 	BIT(0),
	FLD_SYSTEM_TIMER_EN 	    = 	BIT(1),
	FLD_SYSTEM_TIMER_AUTO 	    = 	BIT(2),
	FLD_SYSTEM_32K_TRACK_EN 	= 	BIT(3),
	FLD_SYSTEM_32K_CAL_MODE 	= 	BIT_RNG(4,7),

};

#define reg_system_st		    REG_ADDR8(STIMER_BASE_ADDR+0xb)

enum{

	FLD_SYSTEM_CMD_STOP 			=   BIT(1),
	FLD_SYSTEM_CMD_SYNC		        =   BIT(3),
	FLD_SYSTEM_CLK_32K		        =   BIT(4),
	FLD_SYSTEM_CLR_RD_DONE			=   BIT(5),
	FLD_SYSTEM_RD_BUSY			    =   BIT(6),
	FLD_SYSTEM_CMD_SET_DLY_DONE	    =   BIT(7),

};

#define reg_system_timer_set_32k         	REG_ADDR32(STIMER_BASE_ADDR+0xc)

#define reg_system_timer_read_32k         	REG_ADDR32(STIMER_BASE_ADDR+0x10)

#define reg_system_cal_latch_32k         	REG_ADDR32(STIMER_BASE_ADDR+0x14)

#define reg_system_up_32k					REG_ADDR32(STIMER_BASE_ADDR+0x18)
enum{

	FLD_SYSTEM_UPDATE_UPON_32K 			=   BIT(0),
	FLD_SYSTEM_RUN_UPON_NXT_32K		    =   BIT(1),
};

#endif /* STIMER_REG_H_ */
