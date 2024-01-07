/********************************************************************************************************
 * @file    drv_gpio.h
 *
 * @brief   This is the header file for drv_gpio
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
	RISING_EDGE,
	FALLING_EDGE,
	HIGH_LEVEL,	/* Only for b91 platform. */
	LOW_LEVEL,	/* Only for b91 platform. */
}drv_gpioPoll_e;

typedef enum{
	GPIO_IRQ_INVALID,
	GPIO_IRQ_MODE,
	GPIO_IRQ_RISC0_MODE,
	GPIO_IRQ_RISC1_MODE,
}drv_gpioIrqMode_t;

typedef void (*irq_callback)(void);

/****
* brief: initialize the gpio interrupt
* param[in] pin, the gpio interrupt pin
* param[in] polarity, the falling edge or the rising edge.
* param[in] gpio_irq_callback, the callback function
* @return, 0 success, -1 failed(fail to allocate the node buffer)
*/
int drv_gpio_irq_config(drv_gpioIrqMode_t mode, u32 pin, drv_gpioPoll_e polarity, irq_callback gpio_irq_callback);

void drv_gpio_irq_handler(void);
void drv_gpio_irq_risc0_handler(void);
void drv_gpio_irq_risc1_handler(void);

void drv_gpio_irq_set(u32 pin, drv_gpioPoll_e polarity);
void drv_gpio_irq_risc0_set(u32 pin, drv_gpioPoll_e polarity);
void drv_gpio_irq_risc1_set(u32 pin, drv_gpioPoll_e polarity);

void drv_gpio_irq_en(u32 pin);
void drv_gpio_irq_risc0_en(u32 pin);
void drv_gpio_irq_risc1_en(u32 pin);

void drv_gpio_irq_dis(u32 pin);
void drv_gpio_irq_risc0_dis(u32 pin);
void drv_gpio_irq_risc1_dis(u32 pin);

void drv_gpio_func_set(u32 pin);
void drv_gpio_output_en(u32 pin, bool enable);
void drv_gpio_input_en(u32 pin, bool enable);
void drv_gpio_up_down_resistor(u32 pin, u8 res);
void drv_gpio_write(u32 pin, bool value);
bool drv_gpio_read(u32 pin);
void drv_gpio_read_all(u8 *p);
