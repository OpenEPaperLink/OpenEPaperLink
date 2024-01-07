/********************************************************************************************************
 * @file    usbhw.h
 *
 * @brief   This is the header file for B85
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
#include "analog.h"
#include "usbhw_i.h"
#include "gpio.h"

enum {
	USB_EDP_PRINTER_IN 		= 8, 	// endpoint 8 is alias of enpoint 0,  becareful.  // default hw buf len = 64
	USB_EDP_MOUSE 			= 2,	// default hw buf len = 8
	USB_EDP_KEYBOARD_IN 	= 1,	// default hw buf len = 8
	USB_EDP_KEYBOARD_OUT 	= 3,	// default hw buf len = 16
	USB_EDP_AUDIO_IN 		= 4,	// default hw buf len = 64
	USB_EDP_PRINTER_OUT 	= 5,	// default hw buf len = 64
	USB_EDP_SPEAKER 		= 6,	// default hw buf len = 16
	USB_EDP_MIC 			= 7,	// default hw buf len = 16
#if(USB_GAMEPAD_ENABLE)
	USB_EDP_GAMEPAD_IN 		= 3,	// default hw buf len = 64
	USB_EDP_GAMEPAD_OUT 	= 5,	// default hw buf len = 64
#endif
	USB_EDP_MS_IN 			= USB_EDP_PRINTER_IN,	// mass storage
	USB_EDP_MS_OUT 			= USB_EDP_PRINTER_OUT,
	USB_EDP_SOMATIC_IN 		= USB_EDP_AUDIO_IN,		//  when USB_SOMATIC_ENABLE, USB_EDP_PRINTER_OUT disable
	USB_EDP_SOMATIC_OUT 	= USB_EDP_PRINTER_OUT,
    USB_EDP_CDC_IN 			= 4,
    USB_EDP_CDC_OUT 		= 5,
};

// #defined in the standard spec
enum {
	USB_HID_AUDIO       	= 2,
	USB_HID_MOUSE       	= 1,
	USB_HID_KB_MEDIA    	= 3,// media
	USB_HID_KB_SYS      	= 4,// system : power,sleep,wakeup
#if(USB_GAMEPAD_ENABLE)
	USB_HID_GAMEPAD			= 5,// somatic sensor,  may have many report ids
#endif
	USB_HID_SOMATIC			= 5,// somatic sensor,  may have many report ids
};

/**
 * @brief      This function enables or disables the internal pull-up resistor
 *             of DP pin of USB interface
 * @param[in]  En - enables or disables the internal pull-up resistor(1: enable 0: disable)
 * @return     none
 */
static inline void usb_dp_pullup_en(int en)
{
	unsigned char dat = analog_read(0x0b);
	if(en){
		dat = dat | BIT(7);
	}else{
		dat = dat & 0x7f ;
	}

	analog_write(0x0b, dat);
}

/**
 * @brief      This function serves to power on or down USB module
 * @param[in]  En - 1: power on 0: power down
 * @return     none
 */
static inline void usb_power_on(unsigned char en)
{
	if(en){
		analog_write(0x34, analog_read(0x34) & 0xfd);
	}else{
		analog_write(0x34, analog_read(0x34) | 0x02);
	}
}

/**
 * @brief      This function serves to set GPIO MUX function as DP and DM pin of USB
 * @param[in]  none.
 * @return     none.
 */
static inline void usb_set_pin_en(void)
{
	//clear DP first
	gpio_set_func(GPIO_PA6, AS_GPIO);
	gpio_set_output_en(GPIO_PA6, 1);
	gpio_write(GPIO_PA6, 0);

	gpio_set_func(GPIO_PA5, AS_USB);
	gpio_set_func(GPIO_PA6, AS_USB);
	usb_dp_pullup_en(1);
}

/**
 * @brief      This function disables the manual interrupt
 *             (Endpont8 is the alias of endpoint0)
 * @param[in]  m - the irq mode needs to set
 * @return     none
 */
void usbhw_disable_manual_interrupt(int m);

/**
 * @brief      This function enable the manual interrupt
 * @param[in]  m - the irq mode needs to set
 * @return     none
 */
void usbhw_enable_manual_interrupt(int m);

/**
 * @brief      This function sends a bulk of data to host via the specified endpoint
 * @param[in]  ep - the number of the endpoint
 * @param[in]  data - pointer to the data need to send
 * @param[in]  len - length in byte of the data need to send
 * @return     none
 */
void usbhw_write_ep(unsigned int ep, unsigned char * data, int len);

/**
 * @brief      This function sends two bytes data to host via the control endpoint
 *             (handy help function)
 * @param[in]  v - the two bytes data need to send
 * @return     none
 */
void usbhw_write_ctrl_ep_u16(unsigned short v);

/**
 * @brief   This function reads two bytes data from host via the control endpoint
 * @param   none
 * @return  the two bytes data read from the control endpoint
 */
unsigned short usbhw_read_ctrl_ep_u16(void);

