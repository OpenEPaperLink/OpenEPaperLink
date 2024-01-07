/********************************************************************************************************
 * @file    clock.h
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

#include "register.h"


#ifdef __GNUC__
#define _ASM_NOP_				asm("tnop")
#else
#define _ASM_NOP_
#endif



enum{
	CLOCK_SEL_32M_RC 		= 0,
	CLOCK_SEL_HS_DIV 		= 1,
	CLOCK_SEL_16M_PAD 		= 2,
	CLOCK_SEL_32M_PAD 		= 3,
	CLOCK_SEL_SPI  	  		= 4,
	CLOCK_SEL_40M_INTERNAL 	= 5,
	CLOCK_SEL_32K_RC  		= 6,
};

typedef enum{
	SYS_CLK_16M_PLL,
	SYS_CLK_32M_PLL,
	SYS_CLK_32M_RC,
	SYS_CLK_16M_RC,
	SYS_CLK_8M_RC,
}SYS_CLK_TYPEDEF;


extern unsigned long CLOCK_SYS_CLOCK_1US;
extern unsigned long tickPerUs;



static inline unsigned long clock_time(void){
	return reg_system_tick;
}

// check if the current time is exceed span_us from ref time
static inline unsigned int clock_time_exceed(unsigned int ref, unsigned int span_us){
	return ((unsigned int)(clock_time() - ref) > span_us * CLOCK_SYS_CLOCK_1US);
}


void clock_init(SYS_CLK_TYPEDEF SYS_CLK);
void rc_32k_cal(void);
void sleep_us(unsigned int us);

#define WaitUs(t)			sleep_us(t)
#define WaitMs(t)			sleep_us((t)*1000)
#define sleep_ms(t)			sleep_us((t)*1000)
