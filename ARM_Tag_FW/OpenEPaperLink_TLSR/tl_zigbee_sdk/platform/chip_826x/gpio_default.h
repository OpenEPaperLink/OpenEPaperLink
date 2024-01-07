/********************************************************************************************************
 * @file    gpio_default.h
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

#include "gpio.h"


#ifndef PA0_INPUT_ENABLE
#define PA0_INPUT_ENABLE	0
#endif
#ifndef PA1_INPUT_ENABLE
#define PA1_INPUT_ENABLE	0
#endif
#ifndef PA2_INPUT_ENABLE
#define PA2_INPUT_ENABLE	0
#endif
#ifndef PA3_INPUT_ENABLE
#define PA3_INPUT_ENABLE	0
#endif
#ifndef PA4_INPUT_ENABLE
#define PA4_INPUT_ENABLE	0
#endif
#ifndef PA5_INPUT_ENABLE
#define PA5_INPUT_ENABLE	0
#endif
#ifndef PA6_INPUT_ENABLE
#define PA6_INPUT_ENABLE	0
#endif
#ifndef PA7_INPUT_ENABLE
#define PA7_INPUT_ENABLE	0
#endif
#ifndef PA0_OUTPUT_ENABLE
#define PA0_OUTPUT_ENABLE	0
#endif
#ifndef PA1_OUTPUT_ENABLE
#define PA1_OUTPUT_ENABLE	0
#endif
#ifndef PA2_OUTPUT_ENABLE
#define PA2_OUTPUT_ENABLE	0
#endif
#ifndef PA3_OUTPUT_ENABLE
#define PA3_OUTPUT_ENABLE	0
#endif
#ifndef PA4_OUTPUT_ENABLE
#define PA4_OUTPUT_ENABLE	0
#endif
#ifndef PA5_OUTPUT_ENABLE
#define PA5_OUTPUT_ENABLE	0
#endif
#ifndef PA6_OUTPUT_ENABLE
#define PA6_OUTPUT_ENABLE	0
#endif
#ifndef PA7_OUTPUT_ENABLE
#define PA7_OUTPUT_ENABLE	0
#endif
#ifndef PA0_DATA_STRENGTH
#define PA0_DATA_STRENGTH	1
#endif
#ifndef PA1_DATA_STRENGTH
#define PA1_DATA_STRENGTH	1
#endif
#ifndef PA2_DATA_STRENGTH
#define PA2_DATA_STRENGTH	1
#endif
#ifndef PA3_DATA_STRENGTH
#define PA3_DATA_STRENGTH	1
#endif
#ifndef PA4_DATA_STRENGTH
#define PA4_DATA_STRENGTH	1
#endif
#ifndef PA5_DATA_STRENGTH
#define PA5_DATA_STRENGTH	1
#endif
#ifndef PA6_DATA_STRENGTH
#define PA6_DATA_STRENGTH	1
#endif
#ifndef PA7_DATA_STRENGTH
#define PA7_DATA_STRENGTH	1
#endif
#ifndef PA0_DATA_OUT
#define PA0_DATA_OUT	0
#endif
#ifndef PA1_DATA_OUT
#define PA1_DATA_OUT	0
#endif
#ifndef PA2_DATA_OUT
#define PA2_DATA_OUT	0
#endif
#ifndef PA3_DATA_OUT
#define PA3_DATA_OUT	0
#endif
#ifndef PA4_DATA_OUT
#define PA4_DATA_OUT	0
#endif
#ifndef PA5_DATA_OUT
#define PA5_DATA_OUT	0
#endif
#ifndef PA6_DATA_OUT
#define PA6_DATA_OUT	0
#endif
#ifndef PA7_DATA_OUT
#define PA7_DATA_OUT	0
#endif
#ifndef PA0_FUNC
#define PA0_FUNC	AS_GPIO
#endif
#ifndef PA1_FUNC
#define PA1_FUNC	AS_GPIO
#endif
#ifndef PA2_FUNC
#define PA2_FUNC	AS_GPIO
#endif
#ifndef PA3_FUNC
#define PA3_FUNC	AS_GPIO
#endif
#ifndef PA4_FUNC
#define PA4_FUNC	AS_GPIO
#endif
#ifndef PA5_FUNC
#define PA5_FUNC	AS_GPIO
#endif
#ifndef PA6_FUNC
#define PA6_FUNC	AS_GPIO
#endif
#ifndef PA7_FUNC
#define PA7_FUNC	AS_GPIO
#endif
#ifndef PULL_WAKEUP_SRC_PA0
#define PULL_WAKEUP_SRC_PA0	0
#endif
#ifndef PULL_WAKEUP_SRC_PA1
#define PULL_WAKEUP_SRC_PA1	0
#endif
#ifndef PULL_WAKEUP_SRC_PA2
#define PULL_WAKEUP_SRC_PA2	0
#endif
#ifndef PULL_WAKEUP_SRC_PA3
#define PULL_WAKEUP_SRC_PA3	0
#endif
#ifndef PULL_WAKEUP_SRC_PA4
#define PULL_WAKEUP_SRC_PA4	0
#endif
#ifndef PULL_WAKEUP_SRC_PA5
#define PULL_WAKEUP_SRC_PA5	0
#endif
#ifndef PULL_WAKEUP_SRC_PA6
#define PULL_WAKEUP_SRC_PA6	0
#endif
#ifndef PULL_WAKEUP_SRC_PA7
#define PULL_WAKEUP_SRC_PA7	0
#endif

//////////////////////////////////////////////////
#ifndef PB0_INPUT_ENABLE
#define PB0_INPUT_ENABLE	1 //SWS
#endif
#ifndef PB1_INPUT_ENABLE
#define PB1_INPUT_ENABLE	0
#endif
#ifndef PB2_INPUT_ENABLE
#define PB2_INPUT_ENABLE	0
#endif
#ifndef PB3_INPUT_ENABLE
#define PB3_INPUT_ENABLE	0
#endif
#ifndef PB4_INPUT_ENABLE
#define PB4_INPUT_ENABLE	0
#endif
#ifndef PB5_INPUT_ENABLE
#define PB5_INPUT_ENABLE	0
#endif
#ifndef PB6_INPUT_ENABLE
#define PB6_INPUT_ENABLE	0
#endif
#ifndef PB7_INPUT_ENABLE
#define PB7_INPUT_ENABLE	0
#endif
#ifndef PB0_OUTPUT_ENABLE
#define PB0_OUTPUT_ENABLE	0
#endif
#ifndef PB1_OUTPUT_ENABLE
#define PB1_OUTPUT_ENABLE	0
#endif
#ifndef PB2_OUTPUT_ENABLE
#define PB2_OUTPUT_ENABLE	0
#endif
#ifndef PB3_OUTPUT_ENABLE
#define PB3_OUTPUT_ENABLE	0
#endif
#ifndef PB4_OUTPUT_ENABLE
#define PB4_OUTPUT_ENABLE	0
#endif
#ifndef PB5_OUTPUT_ENABLE
#define PB5_OUTPUT_ENABLE	0
#endif
#ifndef PB6_OUTPUT_ENABLE
#define PB6_OUTPUT_ENABLE	0
#endif
#ifndef PB7_OUTPUT_ENABLE
#define PB7_OUTPUT_ENABLE	0
#endif
#ifndef PB0_DATA_STRENGTH
#define PB0_DATA_STRENGTH	1
#endif
#ifndef PB1_DATA_STRENGTH
#define PB1_DATA_STRENGTH	1
#endif
#ifndef PB2_DATA_STRENGTH
#define PB2_DATA_STRENGTH	1
#endif
#ifndef PB3_DATA_STRENGTH
#define PB3_DATA_STRENGTH	1
#endif
#ifndef PB4_DATA_STRENGTH
#define PB4_DATA_STRENGTH	1
#endif
#ifndef PB5_DATA_STRENGTH
#define PB5_DATA_STRENGTH	1
#endif
#ifndef PB6_DATA_STRENGTH
#define PB6_DATA_STRENGTH	1
#endif
#ifndef PB7_DATA_STRENGTH
#define PB7_DATA_STRENGTH	1
#endif
#ifndef PB0_DATA_OUT
#define PB0_DATA_OUT	0
#endif
#ifndef PB1_DATA_OUT
#define PB1_DATA_OUT	0
#endif
#ifndef PB2_DATA_OUT
#define PB2_DATA_OUT	0
#endif
#ifndef PB3_DATA_OUT
#define PB3_DATA_OUT	0
#endif
#ifndef PB4_DATA_OUT
#define PB4_DATA_OUT	0
#endif
#ifndef PB5_DATA_OUT
#define PB5_DATA_OUT	0
#endif
#ifndef PB6_DATA_OUT
#define PB6_DATA_OUT	0
#endif
#ifndef PB7_DATA_OUT
#define PB7_DATA_OUT	0
#endif
#ifndef PB0_FUNC
#define PB0_FUNC	AS_SWIRE
#endif
#ifndef PB1_FUNC
#define PB1_FUNC	AS_GPIO
#endif
#ifndef PB2_FUNC
#define PB2_FUNC	AS_GPIO
#endif
#ifndef PB3_FUNC
#define PB3_FUNC	AS_GPIO
#endif
#ifndef PB4_FUNC
#define PB4_FUNC	AS_GPIO
#endif
#ifndef PB5_FUNC
#define PB5_FUNC	AS_GPIO
#endif
#ifndef PB6_FUNC
#define PB6_FUNC	AS_GPIO
#endif
#ifndef PB7_FUNC
#define PB7_FUNC	AS_GPIO
#endif
#ifndef PULL_WAKEUP_SRC_PB0
#define PULL_WAKEUP_SRC_PB0		PM_PIN_PULLUP_1M //sws pullup
#endif
#ifndef PULL_WAKEUP_SRC_PB1
#define PULL_WAKEUP_SRC_PB1	0
#endif
#ifndef PULL_WAKEUP_SRC_PB2
#define PULL_WAKEUP_SRC_PB2	0
#endif
#ifndef PULL_WAKEUP_SRC_PB3
#define PULL_WAKEUP_SRC_PB3	0
#endif
#ifndef PULL_WAKEUP_SRC_PB4
#define PULL_WAKEUP_SRC_PB4	0
#endif
#ifndef PULL_WAKEUP_SRC_PB5
#define PULL_WAKEUP_SRC_PB5	0
#endif
#ifndef PULL_WAKEUP_SRC_PB6
#define PULL_WAKEUP_SRC_PB6	0
#endif
#ifndef PULL_WAKEUP_SRC_PB7
#define PULL_WAKEUP_SRC_PB7	0
#endif

//////////////////////////////////////////////////
#ifndef PC0_INPUT_ENABLE
#define PC0_INPUT_ENABLE	0
#endif
#ifndef PC1_INPUT_ENABLE
#define PC1_INPUT_ENABLE	0
#endif
#ifndef PC2_INPUT_ENABLE
#define PC2_INPUT_ENABLE	0
#endif
#ifndef PC3_INPUT_ENABLE
#define PC3_INPUT_ENABLE	0
#endif
#ifndef PC4_INPUT_ENABLE
#define PC4_INPUT_ENABLE	0
#endif
#ifndef PC5_INPUT_ENABLE
#define PC5_INPUT_ENABLE	0
#endif
#ifndef PC6_INPUT_ENABLE
#define PC6_INPUT_ENABLE	0
#endif
#ifndef PC7_INPUT_ENABLE
#define PC7_INPUT_ENABLE	0
#endif
#ifndef PC0_OUTPUT_ENABLE
#define PC0_OUTPUT_ENABLE	0
#endif
#ifndef PC1_OUTPUT_ENABLE
#define PC1_OUTPUT_ENABLE	0
#endif
#ifndef PC2_OUTPUT_ENABLE
#define PC2_OUTPUT_ENABLE	0
#endif
#ifndef PC3_OUTPUT_ENABLE
#define PC3_OUTPUT_ENABLE	0
#endif
#ifndef PC4_OUTPUT_ENABLE
#define PC4_OUTPUT_ENABLE	0
#endif
#ifndef PC5_OUTPUT_ENABLE
#define PC5_OUTPUT_ENABLE	0
#endif
#ifndef PC6_OUTPUT_ENABLE
#define PC6_OUTPUT_ENABLE	0
#endif
#ifndef PC7_OUTPUT_ENABLE
#define PC7_OUTPUT_ENABLE	0
#endif
#ifndef PC0_DATA_STRENGTH
#define PC0_DATA_STRENGTH	1
#endif
#ifndef PC1_DATA_STRENGTH
#define PC1_DATA_STRENGTH	1
#endif
#ifndef PC2_DATA_STRENGTH
#define PC2_DATA_STRENGTH	1
#endif
#ifndef PC3_DATA_STRENGTH
#define PC3_DATA_STRENGTH	1
#endif
#ifndef PC4_DATA_STRENGTH
#define PC4_DATA_STRENGTH	1
#endif
#ifndef PC5_DATA_STRENGTH
#define PC5_DATA_STRENGTH	1
#endif
#ifndef PC6_DATA_STRENGTH
#define PC6_DATA_STRENGTH	1
#endif
#ifndef PC7_DATA_STRENGTH
#define PC7_DATA_STRENGTH	1
#endif
#ifndef PC0_DATA_OUT
#define PC0_DATA_OUT	0
#endif
#ifndef PC1_DATA_OUT
#define PC1_DATA_OUT	0
#endif
#ifndef PC2_DATA_OUT
#define PC2_DATA_OUT	0
#endif
#ifndef PC3_DATA_OUT
#define PC3_DATA_OUT	0
#endif
#ifndef PC4_DATA_OUT
#define PC4_DATA_OUT	0
#endif
#ifndef PC5_DATA_OUT
#define PC5_DATA_OUT	0
#endif
#ifndef PC6_DATA_OUT
#define PC6_DATA_OUT	0
#endif
#ifndef PC7_DATA_OUT
#define PC7_DATA_OUT	0
#endif
#ifndef PC0_FUNC
#define PC0_FUNC	AS_GPIO
#endif
#ifndef PC1_FUNC
#define PC1_FUNC	AS_GPIO
#endif
#ifndef PC2_FUNC
#define PC2_FUNC	AS_GPIO
#endif
#ifndef PC3_FUNC
#define PC3_FUNC	AS_GPIO
#endif
#ifndef PC4_FUNC
#define PC4_FUNC	AS_GPIO
#endif
#ifndef PC5_FUNC
#define PC5_FUNC	AS_GPIO
#endif
#ifndef PC6_FUNC
#define PC6_FUNC	AS_GPIO
#endif
#ifndef PC7_FUNC
#define PC7_FUNC	AS_GPIO
#endif
#ifndef PULL_WAKEUP_SRC_PC0
#define PULL_WAKEUP_SRC_PC0	0
#endif
#ifndef PULL_WAKEUP_SRC_PC1
#define PULL_WAKEUP_SRC_PC1	0
#endif
#ifndef PULL_WAKEUP_SRC_PC2
#define PULL_WAKEUP_SRC_PC2	0
#endif
#ifndef PULL_WAKEUP_SRC_PC3
#define PULL_WAKEUP_SRC_PC3	0
#endif
#ifndef PULL_WAKEUP_SRC_PC4
#define PULL_WAKEUP_SRC_PC4	0
#endif
#ifndef PULL_WAKEUP_SRC_PC5
#define PULL_WAKEUP_SRC_PC5	0
#endif
#ifndef PULL_WAKEUP_SRC_PC6
#define PULL_WAKEUP_SRC_PC6	0
#endif
#ifndef PULL_WAKEUP_SRC_PC7
#define PULL_WAKEUP_SRC_PC7	0
#endif

//////////////////////////////////////////////////
#ifndef PD0_INPUT_ENABLE
#define PD0_INPUT_ENABLE	0
#endif
#ifndef PD1_INPUT_ENABLE
#define PD1_INPUT_ENABLE	0
#endif
#ifndef PD2_INPUT_ENABLE
#define PD2_INPUT_ENABLE	0
#endif
#ifndef PD3_INPUT_ENABLE
#define PD3_INPUT_ENABLE	0
#endif
#ifndef PD4_INPUT_ENABLE
#define PD4_INPUT_ENABLE	0
#endif
#ifndef PD5_INPUT_ENABLE
#define PD5_INPUT_ENABLE	0
#endif
#ifndef PD6_INPUT_ENABLE
#define PD6_INPUT_ENABLE	0
#endif
#ifndef PD7_INPUT_ENABLE
#define PD7_INPUT_ENABLE	0
#endif
#ifndef PD0_OUTPUT_ENABLE
#define PD0_OUTPUT_ENABLE	0
#endif
#ifndef PD1_OUTPUT_ENABLE
#define PD1_OUTPUT_ENABLE	0
#endif
#ifndef PD2_OUTPUT_ENABLE
#define PD2_OUTPUT_ENABLE	0
#endif
#ifndef PD3_OUTPUT_ENABLE
#define PD3_OUTPUT_ENABLE	0
#endif
#ifndef PD4_OUTPUT_ENABLE
#define PD4_OUTPUT_ENABLE	0
#endif
#ifndef PD5_OUTPUT_ENABLE
#define PD5_OUTPUT_ENABLE	0
#endif
#ifndef PD6_OUTPUT_ENABLE
#define PD6_OUTPUT_ENABLE	0
#endif
#ifndef PD7_OUTPUT_ENABLE
#define PD7_OUTPUT_ENABLE	0
#endif
#ifndef PD0_DATA_STRENGTH
#define PD0_DATA_STRENGTH	1
#endif
#ifndef PD1_DATA_STRENGTH
#define PD1_DATA_STRENGTH	1
#endif
#ifndef PD2_DATA_STRENGTH
#define PD2_DATA_STRENGTH	1
#endif
#ifndef PD3_DATA_STRENGTH
#define PD3_DATA_STRENGTH	1
#endif
#ifndef PD4_DATA_STRENGTH
#define PD4_DATA_STRENGTH	1
#endif
#ifndef PD5_DATA_STRENGTH
#define PD5_DATA_STRENGTH	1
#endif
#ifndef PD6_DATA_STRENGTH
#define PD6_DATA_STRENGTH	1
#endif
#ifndef PD7_DATA_STRENGTH
#define PD7_DATA_STRENGTH	1
#endif
#ifndef PD0_DATA_OUT
#define PD0_DATA_OUT	0
#endif
#ifndef PD1_DATA_OUT
#define PD1_DATA_OUT	0
#endif
#ifndef PD2_DATA_OUT
#define PD2_DATA_OUT	0
#endif
#ifndef PD3_DATA_OUT
#define PD3_DATA_OUT	0
#endif
#ifndef PD4_DATA_OUT
#define PD4_DATA_OUT	0
#endif
#ifndef PD5_DATA_OUT
#define PD5_DATA_OUT	0
#endif
#ifndef PD6_DATA_OUT
#define PD6_DATA_OUT	0
#endif
#ifndef PD7_DATA_OUT
#define PD7_DATA_OUT	0
#endif
#ifndef PD0_FUNC
#define PD0_FUNC	AS_GPIO
#endif
#ifndef PD1_FUNC
#define PD1_FUNC	AS_GPIO
#endif
#ifndef PD2_FUNC
#define PD2_FUNC	AS_GPIO
#endif
#ifndef PD3_FUNC
#define PD3_FUNC	AS_GPIO
#endif
#ifndef PD4_FUNC
#define PD4_FUNC	AS_GPIO
#endif
#ifndef PD5_FUNC
#define PD5_FUNC	AS_GPIO
#endif
#ifndef PD6_FUNC
#define PD6_FUNC	AS_GPIO
#endif
#ifndef PD7_FUNC
#define PD7_FUNC	AS_GPIO
#endif
#ifndef PULL_WAKEUP_SRC_PD0
#define PULL_WAKEUP_SRC_PD0	0
#endif
#ifndef PULL_WAKEUP_SRC_PD1
#define PULL_WAKEUP_SRC_PD1	0
#endif
#ifndef PULL_WAKEUP_SRC_PD2
#define PULL_WAKEUP_SRC_PD2	0
#endif
#ifndef PULL_WAKEUP_SRC_PD3
#define PULL_WAKEUP_SRC_PD3	0
#endif
#ifndef PULL_WAKEUP_SRC_PD4
#define PULL_WAKEUP_SRC_PD4	0
#endif
#ifndef PULL_WAKEUP_SRC_PD5
#define PULL_WAKEUP_SRC_PD5	0
#endif
#ifndef PULL_WAKEUP_SRC_PD6
#define PULL_WAKEUP_SRC_PD6	0
#endif
#ifndef PULL_WAKEUP_SRC_PD7
#define PULL_WAKEUP_SRC_PD7	0
#endif

//////////////////////////////////////////////////
#ifndef PE0_INPUT_ENABLE
#define PE0_INPUT_ENABLE	0
#endif
#ifndef PE1_INPUT_ENABLE
#define PE1_INPUT_ENABLE	0
#endif
#ifndef PE2_INPUT_ENABLE
#define PE2_INPUT_ENABLE	0 //USB DM
#endif
#ifndef PE3_INPUT_ENABLE
#define PE3_INPUT_ENABLE	0 //USB DP
#endif
#ifndef PE4_INPUT_ENABLE
#define PE4_INPUT_ENABLE	1 //MSPI
#endif
#ifndef PE5_INPUT_ENABLE
#define PE5_INPUT_ENABLE	1 //MSPI
#endif
#ifndef PE6_INPUT_ENABLE
#define PE6_INPUT_ENABLE	1 //MSPI
#endif
#ifndef PE7_INPUT_ENABLE
#define PE7_INPUT_ENABLE	1 //MSPI
#endif
#ifndef PE0_OUTPUT_ENABLE
#define PE0_OUTPUT_ENABLE	0
#endif
#ifndef PE1_OUTPUT_ENABLE
#define PE1_OUTPUT_ENABLE	0
#endif
#ifndef PE2_OUTPUT_ENABLE
#define PE2_OUTPUT_ENABLE	0
#endif
#ifndef PE3_OUTPUT_ENABLE
#define PE3_OUTPUT_ENABLE	0
#endif
#ifndef PE4_OUTPUT_ENABLE
#define PE4_OUTPUT_ENABLE	0
#endif
#ifndef PE5_OUTPUT_ENABLE
#define PE5_OUTPUT_ENABLE	0
#endif
#ifndef PE6_OUTPUT_ENABLE
#define PE6_OUTPUT_ENABLE	0
#endif
#ifndef PE7_OUTPUT_ENABLE
#define PE7_OUTPUT_ENABLE	0
#endif
#ifndef PE0_DATA_STRENGTH
#define PE0_DATA_STRENGTH	1
#endif
#ifndef PE1_DATA_STRENGTH
#define PE1_DATA_STRENGTH	1
#endif
#ifndef PE2_DATA_STRENGTH
#define PE2_DATA_STRENGTH	1
#endif
#ifndef PE3_DATA_STRENGTH
#define PE3_DATA_STRENGTH	1
#endif
#ifndef PE4_DATA_STRENGTH
#define PE4_DATA_STRENGTH	1
#endif
#ifndef PE5_DATA_STRENGTH
#define PE5_DATA_STRENGTH	1
#endif
#ifndef PE6_DATA_STRENGTH
#define PE6_DATA_STRENGTH	1
#endif
#ifndef PE7_DATA_STRENGTH
#define PE7_DATA_STRENGTH	1
#endif
#ifndef PE0_DATA_OUT
#define PE0_DATA_OUT	0
#endif
#ifndef PE1_DATA_OUT
#define PE1_DATA_OUT	0
#endif
#ifndef PE2_DATA_OUT
#define PE2_DATA_OUT	0
#endif
#ifndef PE3_DATA_OUT
#define PE3_DATA_OUT	0
#endif
#ifndef PE4_DATA_OUT
#define PE4_DATA_OUT	0
#endif
#ifndef PE5_DATA_OUT
#define PE5_DATA_OUT	0
#endif
#ifndef PE6_DATA_OUT
#define PE6_DATA_OUT	0
#endif
#ifndef PE7_DATA_OUT
#define PE7_DATA_OUT	0
#endif
#ifndef PE0_FUNC
#define PE0_FUNC	AS_GPIO
#endif
#ifndef PE1_FUNC
#define PE1_FUNC	AS_GPIO
#endif
#ifndef PE2_FUNC
#define PE2_FUNC	AS_USB
#endif
#ifndef PE3_FUNC
#define PE3_FUNC	AS_USB
#endif
#ifndef PE4_FUNC
#define PE4_FUNC	AS_MSPI	//MSPI
#endif
#ifndef PE5_FUNC
#define PE5_FUNC	AS_MSPI	//MSPI
#endif
#ifndef PE6_FUNC
#define PE6_FUNC	AS_MSPI	//MSPI
#endif
#ifndef PE7_FUNC
#define PE7_FUNC	AS_MSPI	//MSPI
#endif
#ifndef PULL_WAKEUP_SRC_PE0
#define PULL_WAKEUP_SRC_PE0	0
#endif
#ifndef PULL_WAKEUP_SRC_PE1
#define PULL_WAKEUP_SRC_PE1	0
#endif
#ifndef PULL_WAKEUP_SRC_PE2
#define PULL_WAKEUP_SRC_PE2	0
#endif
#ifndef PULL_WAKEUP_SRC_PE3
#define PULL_WAKEUP_SRC_PE3	0
#endif
#ifndef PULL_WAKEUP_SRC_PE4
#define PULL_WAKEUP_SRC_PE4	0
#endif
#ifndef PULL_WAKEUP_SRC_PE5
#define PULL_WAKEUP_SRC_PE5	0
#endif
#ifndef PULL_WAKEUP_SRC_PE6
#define PULL_WAKEUP_SRC_PE6	0
#endif
#ifndef PULL_WAKEUP_SRC_PE7
#define PULL_WAKEUP_SRC_PE7	0
#endif


static inline void gpio_analog_resistance_init(void)
{
	unsigned char areg = analog_read (0x0a) & 0x0f;

	analog_write (0x0a, areg | (PULL_WAKEUP_SRC_PA0<<4) |
						(PULL_WAKEUP_SRC_PA1<<6));

	analog_write (0x0b,  PULL_WAKEUP_SRC_PA2 |
						(PULL_WAKEUP_SRC_PA3<<2) |
						(PULL_WAKEUP_SRC_PA4<<4) |
						(PULL_WAKEUP_SRC_PA5<<6));

	analog_write (0x0c,  PULL_WAKEUP_SRC_PA6 |
						(PULL_WAKEUP_SRC_PA7<<2) |
						(PULL_WAKEUP_SRC_PB0<<4) |
						(PULL_WAKEUP_SRC_PB1<<6));

	analog_write (0x0d,  PULL_WAKEUP_SRC_PB2 |
						(PULL_WAKEUP_SRC_PB3<<2) |
						(PULL_WAKEUP_SRC_PB4<<4) |
						(PULL_WAKEUP_SRC_PB5<<6));

	analog_write (0x0e,  PULL_WAKEUP_SRC_PB6 |
						(PULL_WAKEUP_SRC_PB7<<2) |
						(PULL_WAKEUP_SRC_PC0<<4) |
						(PULL_WAKEUP_SRC_PC1<<6));

	analog_write (0x0f,  PULL_WAKEUP_SRC_PC2 |
						(PULL_WAKEUP_SRC_PC3<<2) |
						(PULL_WAKEUP_SRC_PC4<<4) |
						(PULL_WAKEUP_SRC_PC5<<6));

	analog_write (0x10,  PULL_WAKEUP_SRC_PC6 |
						(PULL_WAKEUP_SRC_PC7<<2) |
						(PULL_WAKEUP_SRC_PD0<<4) |
						(PULL_WAKEUP_SRC_PD1<<6));

	analog_write (0x11,  PULL_WAKEUP_SRC_PD2 |
						(PULL_WAKEUP_SRC_PD3<<2) |
						(PULL_WAKEUP_SRC_PD4<<4) |
						(PULL_WAKEUP_SRC_PD5<<6));

	analog_write (0x12,  PULL_WAKEUP_SRC_PD6 |
						(PULL_WAKEUP_SRC_PD7<<2) |
						(PULL_WAKEUP_SRC_PE0<<4) |
						(PULL_WAKEUP_SRC_PE1<<6));

	areg = analog_read (0x08) & 0x0f;
	analog_write (0x08,  areg | PULL_WAKEUP_SRC_PE2<<4 |
						(PULL_WAKEUP_SRC_PE3<<6) );
}

static inline void gpio_init(int anaRes_init_en)
{
	//return;
	reg_gpio_pa_setting1 =
		(PA0_INPUT_ENABLE<<8) 	| (PA1_INPUT_ENABLE<<9)	| (PA2_INPUT_ENABLE<<10)	| (PA3_INPUT_ENABLE<<11) |
		(PA4_INPUT_ENABLE<<12)	| (PA5_INPUT_ENABLE<<13)	| (PA6_INPUT_ENABLE<<14)	| (PA7_INPUT_ENABLE<<15) |
		((PA0_OUTPUT_ENABLE?0:1)<<16)	| ((PA1_OUTPUT_ENABLE?0:1)<<17) | ((PA2_OUTPUT_ENABLE?0:1)<<18)	| ((PA3_OUTPUT_ENABLE?0:1)<<19) |
		((PA4_OUTPUT_ENABLE?0:1)<<20)	| ((PA5_OUTPUT_ENABLE?0:1)<<21) | ((PA6_OUTPUT_ENABLE?0:1)<<22)	| ((PA7_OUTPUT_ENABLE?0:1)<<23) |
		(PA0_DATA_OUT<<24)	| (PA1_DATA_OUT<<25)	| (PA2_DATA_OUT<<26)	| (PA3_DATA_OUT<<27) |
		(PA4_DATA_OUT<<28)	| (PA5_DATA_OUT<<29)	| (PA6_DATA_OUT<<30)	| (PA7_DATA_OUT<<31) ;
	reg_gpio_pa_setting2 =
		(PA0_DATA_STRENGTH<<8)		| (PA1_DATA_STRENGTH<<9)| (PA2_DATA_STRENGTH<<10)	| (PA3_DATA_STRENGTH<<11) |
		(PA4_DATA_STRENGTH<<12)	| (PA5_DATA_STRENGTH<<13)	| (PA6_DATA_STRENGTH<<14)	| (PA7_DATA_STRENGTH<<15) |
		(PA0_FUNC==AS_GPIO ? BIT(16):0)	| (PA1_FUNC==AS_GPIO ? BIT(17):0)| (PA2_FUNC==AS_GPIO ? BIT(18):0)| (PA3_FUNC==AS_GPIO ? BIT(19):0) |
		(PA4_FUNC==AS_GPIO ? BIT(20):0)	| (PA5_FUNC==AS_GPIO ? BIT(21):0)| (PA6_FUNC==AS_GPIO ? BIT(22):0)| (PA7_FUNC==AS_GPIO ? BIT(23):0);

	reg_gpio_pb_setting1 =
		(PB0_INPUT_ENABLE<<8) 	| (PB1_INPUT_ENABLE<<9)	| (PB2_INPUT_ENABLE<<10)	| (PB3_INPUT_ENABLE<<11) |
		(PB4_INPUT_ENABLE<<12)	| (PB5_INPUT_ENABLE<<13)| (PB6_INPUT_ENABLE<<14)	| (PB7_INPUT_ENABLE<<15) |
		((PB0_OUTPUT_ENABLE?0:1)<<16)	| ((PB1_OUTPUT_ENABLE?0:1)<<17) | ((PB2_OUTPUT_ENABLE?0:1)<<18)	| ((PB3_OUTPUT_ENABLE?0:1)<<19) |
		((PB4_OUTPUT_ENABLE?0:1)<<20)	| ((PB5_OUTPUT_ENABLE?0:1)<<21) | ((PB6_OUTPUT_ENABLE?0:1)<<22)	| ((PB7_OUTPUT_ENABLE?0:1)<<23) |
		(PB0_DATA_OUT<<24)	| (PB1_DATA_OUT<<25)	| (PB2_DATA_OUT<<26)	| (PB3_DATA_OUT<<27) |
		(PB4_DATA_OUT<<28)	| (PB5_DATA_OUT<<29)	| (PB6_DATA_OUT<<30)	| (PB7_DATA_OUT<<31) ;
	reg_gpio_pb_setting2 =
		(PB0_DATA_STRENGTH<<8)	| (PB1_DATA_STRENGTH<<9)	| (PB2_DATA_STRENGTH<<10)	| (PB3_DATA_STRENGTH<<11) |
		(PB4_DATA_STRENGTH<<12)	| (PB5_DATA_STRENGTH<<13)	| (PB6_DATA_STRENGTH<<14)	| (PB7_DATA_STRENGTH<<15) |
		(PB0_FUNC==AS_GPIO ? BIT(16):0)	| (PB1_FUNC==AS_GPIO ? BIT(17):0)| (PB2_FUNC==AS_GPIO ? BIT(18):0)| (PB3_FUNC==AS_GPIO ? BIT(19):0) |
		(PB4_FUNC==AS_GPIO ? BIT(20):0)	| (PB5_FUNC==AS_GPIO ? BIT(21):0)| (PB6_FUNC==AS_GPIO ? BIT(22):0)| (PB7_FUNC==AS_GPIO ? BIT(23):0);

	reg_gpio_pc_setting1 =
		(PC0_INPUT_ENABLE<<8) 	| (PC1_INPUT_ENABLE<<9)	| (PC2_INPUT_ENABLE<<10)	| (PC3_INPUT_ENABLE<<11) |
		(PC4_INPUT_ENABLE<<12)	| (PC5_INPUT_ENABLE<<13)| (PC6_INPUT_ENABLE<<14)	| (PC7_INPUT_ENABLE<<15) |
		((PC0_OUTPUT_ENABLE?0:1)<<16)	| ((PC1_OUTPUT_ENABLE?0:1)<<17) | ((PC2_OUTPUT_ENABLE?0:1)<<18)	| ((PC3_OUTPUT_ENABLE?0:1)<<19) |
		((PC4_OUTPUT_ENABLE?0:1)<<20)	| ((PC5_OUTPUT_ENABLE?0:1)<<21) | ((PC6_OUTPUT_ENABLE?0:1)<<22)	| ((PC7_OUTPUT_ENABLE?0:1)<<23) |
		(PC0_DATA_OUT<<24)	| (PC1_DATA_OUT<<25)	| (PC2_DATA_OUT<<26)	| (PC3_DATA_OUT<<27) |
		(PC4_DATA_OUT<<28)	| (PC5_DATA_OUT<<29)	| (PC6_DATA_OUT<<30)	| (PC7_DATA_OUT<<31) ;
	reg_gpio_pc_setting2 =
		(PC0_DATA_STRENGTH<<8)	| (PC1_DATA_STRENGTH<<9)	| (PC2_DATA_STRENGTH<<10)	| (PC3_DATA_STRENGTH<<11) |
		(PC4_DATA_STRENGTH<<12)	| (PC5_DATA_STRENGTH<<13)	| (PC6_DATA_STRENGTH<<14)	| (PC7_DATA_STRENGTH<<15) |
		(PC0_FUNC==AS_GPIO ? BIT(16):0)	| (PC1_FUNC==AS_GPIO ? BIT(17):0)| (PC2_FUNC==AS_GPIO ? BIT(18):0)| (PC3_FUNC==AS_GPIO ? BIT(19):0) |
		(PC4_FUNC==AS_GPIO ? BIT(20):0)	| (PC5_FUNC==AS_GPIO ? BIT(21):0)| (PC6_FUNC==AS_GPIO ? BIT(22):0)| (PC7_FUNC==AS_GPIO ? BIT(23):0);

	reg_gpio_pd_setting1 =
		(PD0_INPUT_ENABLE<<8) 	| (PD1_INPUT_ENABLE<<9)	| (PD2_INPUT_ENABLE<<10)	| (PD3_INPUT_ENABLE<<11) |
		(PD4_INPUT_ENABLE<<12)	| (PD5_INPUT_ENABLE<<13)| (PD6_INPUT_ENABLE<<14)	| (PD7_INPUT_ENABLE<<15) |
		((PD0_OUTPUT_ENABLE?0:1)<<16)	| ((PD1_OUTPUT_ENABLE?0:1)<<17) | ((PD2_OUTPUT_ENABLE?0:1)<<18)	| ((PD3_OUTPUT_ENABLE?0:1)<<19) |
		((PD4_OUTPUT_ENABLE?0:1)<<20)	| ((PD5_OUTPUT_ENABLE?0:1)<<21) | ((PD6_OUTPUT_ENABLE?0:1)<<22)	| ((PD7_OUTPUT_ENABLE?0:1)<<23) |
		(PD0_DATA_OUT<<24)	| (PD1_DATA_OUT<<25)	| (PD2_DATA_OUT<<26)	| (PD3_DATA_OUT<<27) |
		(PD4_DATA_OUT<<28)	| (PD5_DATA_OUT<<29)	| (PD6_DATA_OUT<<30)	| (PD7_DATA_OUT<<31) ;
	reg_gpio_pd_setting2 =
		(PD0_DATA_STRENGTH<<8)	| (PD1_DATA_STRENGTH<<9)	| (PD2_DATA_STRENGTH<<10)	| (PD3_DATA_STRENGTH<<11) |
		(PD4_DATA_STRENGTH<<12)	| (PD5_DATA_STRENGTH<<13)	| (PD6_DATA_STRENGTH<<14)	| (PD7_DATA_STRENGTH<<15) |
		(PD0_FUNC==AS_GPIO ? BIT(16):0)	| (PD1_FUNC==AS_GPIO ? BIT(17):0)| (PD2_FUNC==AS_GPIO ? BIT(18):0)| (PD3_FUNC==AS_GPIO ? BIT(19):0) |
		(PD4_FUNC==AS_GPIO ? BIT(20):0)	| (PD5_FUNC==AS_GPIO ? BIT(21):0)| (PD6_FUNC==AS_GPIO ? BIT(22):0)| (PD7_FUNC==AS_GPIO ? BIT(23):0);

	reg_gpio_pe_setting1 =
		(PE0_INPUT_ENABLE<<8) 	| (PE1_INPUT_ENABLE<<9)	| (PE2_INPUT_ENABLE<<10)	| (PE3_INPUT_ENABLE<<11) |
		(PE4_INPUT_ENABLE<<12)	| (PE5_INPUT_ENABLE<<13)| (PE6_INPUT_ENABLE<<14)	| (PE7_INPUT_ENABLE<<15) |
		((PE0_OUTPUT_ENABLE?0:1)<<16)	| ((PE1_OUTPUT_ENABLE?0:1)<<17) | ((PE2_OUTPUT_ENABLE?0:1)<<18)	| ((PE3_OUTPUT_ENABLE?0:1)<<19) |
		((PE4_OUTPUT_ENABLE?0:1)<<20)	| ((PE5_OUTPUT_ENABLE?0:1)<<21) | ((PE6_OUTPUT_ENABLE?0:1)<<22)	| ((PE7_OUTPUT_ENABLE?0:1)<<23) |
		(PE0_DATA_OUT<<24)	| (PE1_DATA_OUT<<25)	| (PE2_DATA_OUT<<26)	| (PE3_DATA_OUT<<27) |
		(PE4_DATA_OUT<<28)	| (PE5_DATA_OUT<<29)	| (PE6_DATA_OUT<<30)	| (PE7_DATA_OUT<<31);
	reg_gpio_pe_setting2 =
		(PE0_DATA_STRENGTH<<8)	| (PE1_DATA_STRENGTH<<9)	| (PE2_DATA_STRENGTH<<10)	| (PE3_DATA_STRENGTH<<11) |
		(PE4_DATA_STRENGTH<<12)	| (PE5_DATA_STRENGTH<<13)	| (PE6_DATA_STRENGTH<<14)	| (PE7_DATA_STRENGTH<<15) |
		(PE0_FUNC==AS_GPIO ? BIT(16):0)	| (PE1_FUNC==AS_GPIO ? BIT(17):0)| (PE2_FUNC==AS_GPIO ? BIT(18):0)| (PE3_FUNC==AS_GPIO ? BIT(19):0) |
		(PE4_FUNC==AS_GPIO ? BIT(20):0)	| (PE5_FUNC==AS_GPIO ? BIT(21):0)| (PE6_FUNC==AS_GPIO ? BIT(22):0)| (PE7_FUNC==AS_GPIO ? BIT(23):0);


	if(anaRes_init_en){
		gpio_analog_resistance_init();
	}
}

