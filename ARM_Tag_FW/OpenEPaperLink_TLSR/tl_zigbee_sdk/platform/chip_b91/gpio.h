/********************************************************************************************************
 * @file	gpio.h
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
/**	@page GPIO
 *
 *	Introduction
 *	===============
 *	B91 contain two six group gpio(A~F), total 44 gpio pin.
 *
 *	API Reference
 *	===============
 *	Header File: gpio.h
 */
#ifndef DRIVERS_GPIO_H_
#define DRIVERS_GPIO_H_


#include "lib/include/plic.h"
#include "analog.h"
#include "reg_include/gpio_reg.h"
/**********************************************************************************************************************
 *                                         global constants                                                           *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *                                           global macro                                                             *
 *********************************************************************************************************************/


/**********************************************************************************************************************
 *                                         global data type                                                           *
 *********************************************************************************************************************/
/**
 *  @brief  Define GPIO types
 */
typedef enum{
		GPIO_GROUPA    = 0x000,
		GPIO_GROUPB    = 0x100,
		GPIO_GROUPC    = 0x200,
		GPIO_GROUPD    = 0x300,
		GPIO_GROUPE    = 0x400,
		GPIO_GROUPF    = 0x500,
		GPIO_ALL       = 0x600,
	    GPIO_PA0 = GPIO_GROUPA | BIT(0),
		GPIO_PA1 = GPIO_GROUPA | BIT(1),
		GPIO_PA2 = GPIO_GROUPA | BIT(2),
		GPIO_PA3 = GPIO_GROUPA | BIT(3),
		GPIO_PA4 = GPIO_GROUPA | BIT(4),
		GPIO_PA5 = GPIO_GROUPA | BIT(5),GPIO_DM=GPIO_PA5,
		GPIO_PA6 = GPIO_GROUPA | BIT(6),GPIO_DP=GPIO_PA6,
		GPIO_PA7 = GPIO_GROUPA | BIT(7),GPIO_SWS=GPIO_PA7,
		GPIOA_ALL = GPIO_GROUPA | 0x00ff,

		GPIO_PB0 = GPIO_GROUPB | BIT(0),
		GPIO_PB1 = GPIO_GROUPB | BIT(1),
		GPIO_PB2 = GPIO_GROUPB | BIT(2),
		GPIO_PB3 = GPIO_GROUPB | BIT(3),
		GPIO_PB4 = GPIO_GROUPB | BIT(4),
		GPIO_PB5 = GPIO_GROUPB | BIT(5),
		GPIO_PB6 = GPIO_GROUPB | BIT(6),
		GPIO_PB7 = GPIO_GROUPB | BIT(7),

		GPIO_PC0 = GPIO_GROUPC | BIT(0),GPIO_SWM=GPIO_PC0,
		GPIO_PC1 = GPIO_GROUPC | BIT(1),
		GPIO_PC2 = GPIO_GROUPC | BIT(2),
		GPIO_PC3 = GPIO_GROUPC | BIT(3),
		GPIO_PC4 = GPIO_GROUPC | BIT(4),
		GPIO_PC5 = GPIO_GROUPC | BIT(5),
		GPIO_PC6 = GPIO_GROUPC | BIT(6),
		GPIO_PC7 = GPIO_GROUPC | BIT(7),
		GPIOC_ALL = GPIO_GROUPC | 0x00ff,

		GPIO_PD0 = GPIO_GROUPD | BIT(0),
		GPIO_PD1 = GPIO_GROUPD | BIT(1),
		GPIO_PD2 = GPIO_GROUPD | BIT(2),
		GPIO_PD3 = GPIO_GROUPD | BIT(3),
		GPIO_PD4 = GPIO_GROUPD | BIT(4),
		GPIO_PD5 = GPIO_GROUPD | BIT(5),
		GPIO_PD6 = GPIO_GROUPD | BIT(6),
		GPIO_PD7 = GPIO_GROUPD | BIT(7),

		GPIO_PE0 = GPIO_GROUPE | BIT(0),
		GPIO_PE1 = GPIO_GROUPE | BIT(1),
		GPIO_PE2 = GPIO_GROUPE | BIT(2),
		GPIO_PE3 = GPIO_GROUPE | BIT(3),
		GPIO_PE4 = GPIO_GROUPE | BIT(4),
		GPIO_PE5 = GPIO_GROUPE | BIT(5),
		GPIO_PE6 = GPIO_GROUPE | BIT(6),
		GPIO_PE7 = GPIO_GROUPE | BIT(7),
		GPIOE_ALL = GPIO_GROUPE | 0x00ff,

		GPIO_PF0 = GPIO_GROUPF | BIT(0),
		GPIO_PF1 = GPIO_GROUPF | BIT(1),
		GPIO_PF2 = GPIO_GROUPF | BIT(2),
		GPIO_PF3 = GPIO_GROUPF | BIT(3),
		GPIO_PF4 = GPIO_GROUPF | BIT(4),
		GPIO_PF5 = GPIO_GROUPF | BIT(5),
}gpio_pin_e;

/**
 *  @brief  Define GPIO mux func
 */
typedef enum{
	    AS_GPIO,
		AS_MSPI,

		AS_SWS,
		AS_SWM,

		AS_USB_DP,
		AS_USB_DM,

		AS_TDI,
		AS_TDO,
		AS_TMS,
		AS_TCK,



}gpio_fuc_e;



/**
 *  @brief  Define rising/falling types
 */
typedef enum{
	POL_RISING   = 0,
	POL_FALLING  = 1,
}gpio_pol_e;


/**
 *  @brief  Define interrupt types
 */
typedef enum{
	 INTR_RISING_EDGE=0,
	 INTR_FALLING_EDGE ,
	 INTR_HIGH_LEVEL,
	 INTR_LOW_LEVEL,
} gpio_irq_trigger_type_e;

/**
 *  @brief  Define pull up or down types
 */
typedef enum {
	GPIO_PIN_UP_DOWN_FLOAT    = 0,
	GPIO_PIN_PULLUP_1M     	= 1,
	GPIO_PIN_PULLDOWN_100K  	= 2,
	GPIO_PIN_PULLUP_10K 		= 3,
}gpio_pull_type_e;


typedef enum{
	GPIO_IRQ_MASK_GPIO       =          BIT(0),
	GPIO_IRQ_MASK_GPIO2RISC0 = 			BIT(1),
	GPIO_IRQ_MASK_GPIO2RISC1 = 			BIT(2),
}gpio_irq_mask_e;

/**
 * @brief      This function servers to enable gpio function.
 * @param[in]  pin - the selected pin.
 * @return     none.
 */
static inline void gpio_function_en(gpio_pin_e pin)
{
	unsigned char	bit = pin & 0xff;
	BM_SET(reg_gpio_func(pin), bit);
}


/**
 * @brief      This function servers to disable gpio function.
 * @param[in]  pin - the selected pin.
 * @return     none.
 */
static inline void gpio_function_dis(gpio_pin_e pin)
{
	unsigned char	bit = pin & 0xff;
	BM_CLR(reg_gpio_func(pin), bit);
}



/**
 * @brief     This function set the pin's output high level.
 * @param[in] pin - the pin needs to set its output level.
 * @return    none.
 */
static inline void gpio_set_high_level(gpio_pin_e pin)
{
	unsigned char	bit = pin & 0xff;
	BM_SET(reg_gpio_out(pin), bit);

}


/**
 * @brief     This function set the pin's output low level.
 * @param[in] pin - the pin needs to set its output level.
 * @return    none.
 */
static inline void gpio_set_low_level(gpio_pin_e pin)
{
	unsigned char	bit = pin & 0xff;
	BM_CLR(reg_gpio_out(pin), bit);

}

/**
 * @brief     This function set the pin's output level.
 * @param[in] pin - the pin needs to set its output level
 * @param[in] value - value of the output level(1: high 0: low)
 * @return    none
 */
static inline void gpio_set_level(gpio_pin_e pin, unsigned char value)
{
	if(value)
	{
		gpio_set_high_level(pin);
	}
	else
	{
		gpio_set_low_level(pin);
	}
}

/**
 * @brief     This function read the pin's input/output level.
 * @param[in] pin - the pin needs to read its level.
 * @return    1: the pin's level is high.
 * 			  0: the pin's level is low.
 */
static inline _Bool gpio_get_level(gpio_pin_e pin)
{
	return BM_IS_SET(reg_gpio_in(pin), pin & 0xff);
}


/**
 * @brief      This function read all the pins' input level.
 * @param[out] p - the buffer used to store all the pins' input level
 * @return     none
 */
static inline void gpio_get_level_all(unsigned char *p)
{
	p[0] = reg_gpio_pa_in;
	p[1] = reg_gpio_pb_in;
	p[2] = reg_gpio_pc_in;
	p[3] = reg_gpio_pd_in;
	p[4] = reg_gpio_pe_in;
}



/**
 * @brief     This function set the pin toggle.
 * @param[in] pin - the pin needs to toggle.
 * @return    none.
 */
static inline void gpio_toggle(gpio_pin_e pin)
{
	reg_gpio_out(pin) ^= (pin & 0xFF);
}



/**
 * @brief      This function enable the output function of a pin.
 * @param[in]  pin - the pin needs to set the output function.
 * @return     none.
 */
static inline void gpio_output_en(gpio_pin_e pin)
{
	unsigned char	bit = pin & 0xff;
	BM_CLR(reg_gpio_oen(pin), bit);
}

/**
 * @brief      This function disable the output function of a pin.
 * @param[in]  pin - the pin needs to set the output function.
 * @return     none.
 */
static inline void gpio_output_dis(gpio_pin_e pin)
{
	unsigned char	bit = pin & 0xff;
	BM_SET(reg_gpio_oen(pin), bit);
}

/**
 * @brief      This function enable set output function of a pin.
 * @param[in]  pin - the pin needs to set the output function (1: enable,0: disable)
 * @return     none
 */
static inline void gpio_set_output(gpio_pin_e pin, unsigned char value)
{
	if(value)
	{
		gpio_output_en(pin);
	}
	else
	{
		gpio_output_dis(pin);
	}

}
/**
 * @brief      This function determines whether the output function of a pin is enabled.
 * @param[in]  pin - the pin needs to determine whether its output function is enabled.
 * @return     1: the pin's output function is enabled.
 *             0: the pin's output function is disabled.
 */
static inline _Bool  gpio_is_output_en(gpio_pin_e pin)
{
	return !BM_IS_SET(reg_gpio_oen(pin), pin & 0xff);
}


/**
 * @brief     This function determines whether the input function of a pin is enabled.
 * @param[in] pin - the pin needs to determine whether its input function is enabled(not include group_pc).
 * @return    1: the pin's input function is enabled.
 *            0: the pin's input function is disabled.
 */
static inline _Bool gpio_is_input_en(gpio_pin_e pin)
{
	return BM_IS_SET(reg_gpio_ie(pin), pin & 0xff);
}

/**
 * @brief      This function serves to enable gpio irq function.
 * @param[in]  pin  - the pin needs to enable its IRQ.
 * @return     none.
 */
static inline void gpio_irq_en(gpio_pin_e pin)
{
	BM_SET(reg_gpio_irq_en(pin), pin & 0xff);
}

/**
 * @brief      This function serves to disable gpio irq function.
 * @param[in]  pin  - the pin needs to disable its IRQ.
 * @return     none.
 */
static inline void gpio_irq_dis(gpio_pin_e pin)
{
	BM_CLR(reg_gpio_irq_en(pin), pin & 0xff);
}

/**
 * @brief      This function serves to enable gpio risc0 irq function.
 * @param[in]  pin  - the pin needs to enable its IRQ.
 * @return     none.
 */
static inline void gpio_gpio2risc0_irq_en(gpio_pin_e pin)
{
	BM_SET(reg_gpio_irq_risc0_en(pin), pin & 0xff);
}
/**
 * @brief      This function serves to disable gpio risc0 irq function.
 * @param[in]  pin  - the pin needs to disable its IRQ.
 * @return     none.
 */
static inline void gpio_gpio2risc0_irq_dis(gpio_pin_e pin)
{
	BM_CLR(reg_gpio_irq_risc0_en(pin), pin & 0xff);
}
/**
 * @brief      This function serves to enable gpio risc1 irq function.
 * @param[in]  pin  - the pin needs to enable its IRQ.
 * @return     none.
 */
static inline void gpio_gpio2risc1_irq_en(gpio_pin_e pin)
{
	BM_SET(reg_gpio_irq_risc1_en(pin), pin & 0xff);
}

/**
 * @brief      This function serves to disable gpio risc1 irq function.
 * @param[in]  pin  - the pin needs to disable its IRQ.
 * @return     none.
 */
static inline void gpio_gpio2risc1_irq_dis(gpio_pin_e pin)
{
	BM_CLR(reg_gpio_irq_risc1_en(pin), pin & 0xff);
}
/**
 * @brief      This function serves to clr gpio irq status.
 * @param[in]  status  - the pin needs to disable its IRQ.
 * @return     none.
 */
static inline void gpio_clr_irq_status(gpio_irq_status_e status)
{
	reg_gpio_irq_clr=status;
}

/**
 * @brief      This function serves to enable gpio_irq/gpio_risc0_irq/gpio_risc1_irq mask function.
 * @param[in]  mask  - to select interrupt type.
 * @return     none.
 */
static inline void gpio_set_irq_mask(gpio_irq_mask_e mask)
{
	BM_SET(reg_gpio_irq_risc_mask, mask);
}

/**
 * @brief      This function serves to disable gpio_irq/gpio_risc0_irq/gpio_risc1_irq mask function.
 *             if disable gpio interrupt,choose disable gpio mask , use interface gpio_clr_irq_mask instead of gpio_irq_dis/gpio_gpio2risc0_irq_dis/gpio_gpio2risc1_irq_dis.
 * @return     none.
 */
static inline void gpio_clr_irq_mask(gpio_irq_mask_e mask)
{
	BM_CLR(reg_gpio_irq_risc_mask, mask);
}

/**
 * @brief      This function set the pin's driving strength at strong.
 * @param[in]  pin - the pin needs to set the driving strength.
 * @return     none.
 */
void gpio_ds_en(gpio_pin_e pin);


/**
 * @brief      This function set the pin's driving strength.
 * @param[in]  pin - the pin needs to set the driving strength at poor.
 * @return     none.
 */
 void gpio_ds_dis(gpio_pin_e pin);




void gpio_set_irq(gpio_pin_e pin, gpio_irq_trigger_type_e trigger_type);

/**
 * @brief     This function set a pin's IRQ_RISC0.
 * @param[in] pin 			- the pin needs to enable its IRQ.
 * @param[in] trigger_type  - gpio interrupt type 0  rising edge 1 falling edge 2 high level 3 low level
 * @return    none.
 */
void gpio_set_gpio2risc0_irq(gpio_pin_e pin, gpio_irq_trigger_type_e trigger_type);

/**
 * @brief     This function set a pin's IRQ_RISC1.
 * @param[in] pin 			- the pin needs to enable its IRQ.
 * @param[in] trigger_type  - gpio interrupt type 0  rising edge 1 falling edge 2 high level 3 low level
 * @return    none.
 */
void gpio_set_gpio2risc1_irq(gpio_pin_e pin, gpio_irq_trigger_type_e trigger_type);


/**
 * @brief      This function set the input function of a pin.
 * @param[in]  pin - the pin needs to set the input function.
 * @return     none.
 */
void gpio_input_en(gpio_pin_e pin);

/**
 * @brief      This function disable the input function of a pin.
 * @param[in]  pin - the pin needs to set the input function.
 * @return     none.
 */
void gpio_input_dis(gpio_pin_e pin);

/**
 * @brief      This function set the input function of a pin.
 * @param[in]  pin - the pin needs to set the input function
 * @param[in]  value - enable or disable the pin's input function(1: enable,0: disable )
 * @return     none
 */
void gpio_set_input(gpio_pin_e pin, unsigned char value);
/**
 * @brief      This function servers to set the specified GPIO as high resistor.
 * @param[in]  pin  - select the specified GPIO.
 * @return     none.
 */
void gpio_shutdown(gpio_pin_e pin);

/**
 * @brief     This function set a pin's pull-up/down resistor.
 * @param[in] pin - the pin needs to set its pull-up/down resistor.
 * @param[in] up_down_res - the type of the pull-up/down resistor.
 * @return    none.
 */
void gpio_set_up_down_res(gpio_pin_e pin, gpio_pull_type_e up_down_res);

/**
 * @brief     This function set pin's 30k pull-up registor.
 * @param[in] pin - the pin needs to set its pull-up registor.
 * @return    none.
 */
void gpio_set_pullup_res_30k(gpio_pin_e pin);


#endif




