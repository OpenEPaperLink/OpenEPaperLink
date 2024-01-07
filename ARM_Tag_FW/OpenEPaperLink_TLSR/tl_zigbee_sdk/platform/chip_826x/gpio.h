/********************************************************************************************************
 * @file    gpio.h
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
#include "bit.h"



#define AS_GPIO		0
#define AS_MSPI		1
#define AS_SWIRE	2
#define AS_UART		3
#define AS_PWM		4
#define AS_I2C		5
#define AS_SPI		6
#define AS_ETH_MAC	7
#define AS_I2S		8
#define AS_SDM		9
#define AS_DMIC		10
#define AS_USB		11
#define AS_SWS		12
#define AS_SWM		13
#define AS_TEST		14

typedef enum{
	    GPIO_PA0 = 0x000 | BIT(0),	GPIO_PWM0A0=GPIO_PA0,	GPIO_DMIC_DI=GPIO_PA0,
		GPIO_PA1 = 0x000 | BIT(1), 							GPIO_DMIC_CK=GPIO_PA1,
		GPIO_PA2 = 0x000 | BIT(2),	GPIO_PWM0NA2=GPIO_PA2,	GPIO_DO=GPIO_PA2,
		GPIO_PA3 = 0x000 | BIT(3),	GPIO_PWM1A3=GPIO_PA3,	GPIO_DI=GPIO_PA3,
		GPIO_PA4 = 0x000 | BIT(4), 	GPIO_PWM1NA4=GPIO_PA4,	GPIO_CK=GPIO_PA4,
		GPIO_PA5 = 0x000 | BIT(5), 	GPIO_PWM2NA5=GPIO_PA5,	GPIO_CN=GPIO_PA5,
		GPIO_PA6 = 0x000 | BIT(6), 							GPIO_UTXA6=GPIO_PA6,
		GPIO_PA7 = 0x000 | BIT(7), 							GPIO_URXA7=GPIO_PA7, GPIO_SWM=GPIO_PA7,

		GPIO_PB0 = 0x100 | BIT(0),	GPIO_PWM2B0=GPIO_PB0,	GPIO_SWS=GPIO_PB0,
		GPIO_PB1 = 0x100 | BIT(1),	GPIO_PWM2NB0=GPIO_PB1,
		GPIO_PB2 = 0x100 | BIT(2), 	GPIO_PWM3B2=GPIO_PB2,	GPIO_UTXB2=GPIO_PB2,
		GPIO_PB3 = 0x100 | BIT(3),	GPIO_PWM3NB3=GPIO_PB3,	GPIO_URXB3=GPIO_PB3,
		GPIO_PB4 = 0x100 | BIT(4),	GPIO_PWM4B4=GPIO_PB4,
		GPIO_PB5 = 0x100 | BIT(5),	GPIO_PWM4NB5=GPIO_PB5,
		GPIO_PB6 = 0x100 | BIT(6),	GPIO_PWM5B6=GPIO_PB6,
		GPIO_PB7 = 0x100 | BIT(7),	GPIO_PWM5NB7=GPIO_PB7,

		GPIO_PC0 = 0x200 | BIT(0), GPIO_PWM0C0=GPIO_PC0,
		GPIO_PC1 = 0x200 | BIT(1), GPIO_PWM1C1=GPIO_PC1,
		GPIO_PC2 = 0x200 | BIT(2), GPIO_PWM2C2=GPIO_PC2,	GPIO_UTXC2=GPIO_PC2,
		GPIO_PC3 = 0x200 | BIT(3), GPIO_PWM3C3=GPIO_PC3, 	GPIO_URXC3=GPIO_PC3,
		GPIO_PC4 = 0x200 | BIT(4), GPIO_PWM4C4=GPIO_PC4,	GPIO_URTSC4=GPIO_PC4,
		GPIO_PC5 = 0x200 | BIT(5), GPIO_PWM4C5=GPIO_PC5,	GPIO_UCTSC5=GPIO_PC5,
		GPIO_PC6 = 0x200 | BIT(6),
		GPIO_PC7 = 0x200 | BIT(7),

		GPIO_PD0 = 0x300 | BIT(0),
		GPIO_PD1 = 0x300 | BIT(1),
		GPIO_PD2 = 0x300 | BIT(2),
		GPIO_PD3 = 0x300 | BIT(3),
		GPIO_PD4 = 0x300 | BIT(4),
		GPIO_PD5 = 0x300 | BIT(5), GPIO_PWM0D5=GPIO_PD5,
		GPIO_PD6 = 0x300 | BIT(6), GPIO_PWM1D6=GPIO_PD6,
		GPIO_PD7 = 0x300 | BIT(7), GPIO_PWM2D7=GPIO_PD7,

		GPIO_PE0 = 0x400 | BIT(0), GPIO_PWM0E0=GPIO_PE0,	GPIO_SDMP=GPIO_PE0,
		GPIO_PE1 = 0x400 | BIT(1), GPIO_PWM1E1=GPIO_PE1,	GPIO_SDMN=GPIO_PE1,
		GPIO_PE2 = 0x400 | BIT(2), 							GPIO_DM=GPIO_PE2,
		GPIO_PE3 = 0x400 | BIT(3), 							GPIO_DP=GPIO_PE3,
		GPIO_PE4 = 0x400 | BIT(4), 							GPIO_MSDO=GPIO_PE4,
		GPIO_PE5 = 0x400 | BIT(5), 							GPIO_MCLK=GPIO_PE5,
		GPIO_PE6 = 0x400 | BIT(6), 							GPIO_MSCN=GPIO_PE6,
		GPIO_PE7 = 0x400 | BIT(7), 							GPIO_MSDI=GPIO_PE7,

		GPIO_PF0 = 0x500 | BIT(0),
		GPIO_PF1 = 0x500 | BIT(1),

		GPIO_MAX_COUNT = 56,
}GPIO_PinTypeDef;

/**
 *  @brief  Define pull up or down types
 */
typedef enum {
	PM_PIN_UP_DOWN_FLOAT    = 0,
	PM_PIN_PULLUP_1M     	= 1,
	PM_PIN_PULLUP_10K		= 2,
	PM_PIN_PULLDOWN_100K  	= 3,
}GPIO_PullTypeDef;

#define reg_gpio_in(i)				REG_ADDR8(0x580+((i>>8)<<3))
#define reg_gpio_ie(i)				REG_ADDR8(0x581+((i>>8)<<3))
#define reg_gpio_oen(i)				REG_ADDR8(0x582+((i>>8)<<3))
#define reg_gpio_out(i)				REG_ADDR8(0x583+((i>>8)<<3))
#define reg_gpio_pol(i)				REG_ADDR8(0x584+((i>>8)<<3))
#define reg_gpio_ds(i)				REG_ADDR8(0x585+((i>>8)<<3))
#define reg_gpio_gpio_func(i)		REG_ADDR8(0x586+((i>>8)<<3))
#define reg_gpio_irq_en0(i)			REG_ADDR8(0x587+((i>>8)<<3))	//Corresponding to reg_irq_mask, FLD_IRQ_GPIO_EN in reg_irq_src
#define reg_gpio_irq_en(i)			REG_ADDR8(0x5c8+(i>>8))			//Correspond to FLD_IRQ_GPIO_RISC2_EN in reg_irq_mask, reg_irq_src, in order to be consistent with 5320 and 5328, use FLD_IRQ_GPIO_RISC2_EN

#define reg_gpio_irq_src(i)			REG_ADDR8(0x5e0+(i>>8))

#define reg_gpio_irq_risc0_en(i)    REG_ADDR8(0x5b8 + (i >> 8))	  // reg_irq_mask: FLD_IRQ_GPIO_RISC0_EN
#define reg_gpio_irq_risc1_en(i)    REG_ADDR8(0x5c0 + (i >> 8))	  // reg_irq_mask: FLD_IRQ_GPIO_RISC1_EN
#define reg_gpio_irq_risc2_en(i)    REG_ADDR8(0x5c8 + (i >> 8))   // reg_irq_mask: FLD_IRQ_GPIO_RISC2_EN

#define reg_gpio_wakeup_irq  REG_ADDR8(0x5b5)
enum{
    FLD_GPIO_CORE_WAKEUP_EN  = BIT(2),
    FLD_GPIO_CORE_INTERRUPT_EN = BIT(3),
};

static inline void gpio_core_wakeup_enable_all(int en)
{
    if (en) {
        BM_SET(reg_gpio_wakeup_irq, FLD_GPIO_CORE_WAKEUP_EN);
    }
    else {
        BM_CLR(reg_gpio_wakeup_irq, FLD_GPIO_CORE_WAKEUP_EN);
    }
}


static inline int gpio_is_pe_pin(GPIO_PinTypeDef pin){
	return (pin >> 8) == 0x04;			// PE
}

static inline int gpio_is_output_en(GPIO_PinTypeDef pin){
	return !BM_IS_SET(reg_gpio_oen(pin), pin & 0xff);
}

static inline int gpio_is_input_en(GPIO_PinTypeDef pin){
	return BM_IS_SET(reg_gpio_ie(pin), pin & 0xff);
}

static inline void gpio_set_output_en(GPIO_PinTypeDef pin, unsigned int value){
	unsigned char	bit = pin & 0xff;
	if(!value){
		BM_SET(reg_gpio_oen(pin), bit);
	}else{
		BM_CLR(reg_gpio_oen(pin), bit);
	}
}

static inline void gpio_set_input_en(GPIO_PinTypeDef pin, unsigned int value){
	unsigned char	bit = pin & 0xff;
	if(value){
		BM_SET(reg_gpio_ie(pin), bit);
	}else{
		BM_CLR(reg_gpio_ie(pin), bit);
	}
}

static inline void gpio_set_data_strength(GPIO_PinTypeDef pin, unsigned int value){
	unsigned char	bit = pin & 0xff;
	if(value){
		BM_SET(reg_gpio_ds(pin), bit);
	}else{
		BM_CLR(reg_gpio_ds(pin), bit);
	}
}

static inline void gpio_en_interrupt(GPIO_PinTypeDef pin, unsigned int en){  // reg_irq_mask: FLD_IRQ_GPIO_EN
	unsigned char	bit = pin & 0xff;
	if(en){
		BM_SET(reg_gpio_irq_en0(pin), bit);
	}
	else{
		BM_CLR(reg_gpio_irq_en0(pin), bit);
	}
}

static inline void gpio_set_interrupt(GPIO_PinTypeDef pin, unsigned int falling){  //pol_rising = 0,pol_falling = 1
	unsigned char	bit = pin & 0xff;
	reg_gpio_wakeup_irq |= FLD_GPIO_CORE_INTERRUPT_EN;
	if(falling){
		BM_SET(reg_gpio_pol(pin), bit);
	}else{
		BM_CLR(reg_gpio_pol(pin), bit);
	}

	reg_irq_src = FLD_IRQ_GPIO_EN;
	reg_irq_mask |= FLD_IRQ_GPIO_EN;
}

static inline void gpio_en_interrupt_risc0(GPIO_PinTypeDef pin, unsigned int en){  // reg_irq_mask: FLD_IRQ_GPIO_RISC0_EN
	unsigned char	bit = pin & 0xff;
	if(en){
		BM_SET(reg_gpio_irq_risc0_en(pin), bit);
	}
	else{
		BM_CLR(reg_gpio_irq_risc0_en(pin), bit);
	}
}

static inline void gpio_set_interrupt_risc0(GPIO_PinTypeDef pin, unsigned int falling){
	unsigned char	bit = pin & 0xff;
	if(falling){
		BM_SET(reg_gpio_pol(pin), bit);
	}else{
		BM_CLR(reg_gpio_pol(pin), bit);
	}

	reg_irq_src = FLD_IRQ_GPIO_RISC0_EN;
	reg_irq_mask |= FLD_IRQ_GPIO_RISC0_EN;
}

static inline void gpio_en_interrupt_risc1(GPIO_PinTypeDef pin, unsigned int en){  // reg_irq_mask: FLD_IRQ_GPIO_RISC1_EN
	unsigned char	bit = pin & 0xff;
	if(en){
		BM_SET(reg_gpio_irq_risc1_en(pin), bit);
	}
	else{
		BM_CLR(reg_gpio_irq_risc1_en(pin), bit);
	}
}

static inline void gpio_set_interrupt_risc1(GPIO_PinTypeDef pin, unsigned int falling){
	unsigned char	bit = pin & 0xff;
	if(falling){
		BM_SET(reg_gpio_pol(pin), bit);
	}else{
		BM_CLR(reg_gpio_pol(pin), bit);
	}

	reg_irq_src = FLD_IRQ_GPIO_RISC1_EN;
	reg_irq_mask |= FLD_IRQ_GPIO_RISC1_EN;
}

static inline void gpio_set_interrupt_pol(GPIO_PinTypeDef pin, unsigned int falling){
	unsigned char	bit = pin & 0xff;
	if(falling){
		BM_SET(reg_gpio_pol(pin), bit);
	}else{
		BM_CLR(reg_gpio_pol(pin), bit);
	}
}

static inline void gpio_clr_interrupt(GPIO_PinTypeDef pin){
	unsigned char	bit = pin & 0xff;
	BM_CLR(reg_gpio_irq_en0(pin), bit);
}

static inline void gpio_write(GPIO_PinTypeDef pin, unsigned int value){
	unsigned char	bit = pin & 0xff;
	if(value){
		BM_SET(reg_gpio_out(pin), bit);
	}else{
		BM_CLR(reg_gpio_out(pin), bit);
	}
}

static inline void gpio_toggle(GPIO_PinTypeDef pin) {
	reg_gpio_out(pin) ^= (pin & 0xFF);
}

static inline unsigned int gpio_irq_src_read(GPIO_PinTypeDef pin){
	return BM_IS_SET(reg_gpio_irq_src(pin), pin & 0xff);
}

static inline unsigned int gpio_read(GPIO_PinTypeDef pin){
	return BM_IS_SET(reg_gpio_in(pin), pin & 0xff);
}


static inline void gpio_read_all(unsigned char *p){
	p[0] = REG_ADDR8(0x580);
	p[1] = REG_ADDR8(0x588);
	p[2] = REG_ADDR8(0x590);
	p[3] = REG_ADDR8(0x598);
	p[4] = REG_ADDR8(0x5a0);
}

static inline void gpio_enable_wakeup_pin(GPIO_PinTypeDef pin, int en){
	//return;
	unsigned char bit = pin & 0xff;
    if (en) {
        BM_SET(reg_gpio_irq_en0(pin), bit);
    }
    else {
        BM_CLR(reg_gpio_irq_en0(pin), bit);
    }
}


static inline void gpio_set_func(GPIO_PinTypeDef pin, unsigned int func){
	unsigned char	bit = pin & 0xff;
	if(func == AS_GPIO){
		BM_SET(reg_gpio_gpio_func(pin), bit);
		return;
	}else{
		BM_CLR(reg_gpio_gpio_func(pin), bit);
	}
}



extern void gpio_setup_up_down_resistor(GPIO_PinTypeDef gpio, GPIO_PullTypeDef up_down);
