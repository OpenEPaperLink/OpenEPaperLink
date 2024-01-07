/********************************************************************************************************
 * @file    usb.h
 *
 * @brief   This is the header file for usb
 *
 * @author  Driver & Zigbee Group
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

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

enum {
	USB_TIME_BEFORE_ALLOW_SUSPEND = (3000*1000),//  3000 ms
};

enum {
	USB_IRQ_SETUP_REQ = 0,
	USB_IRQ_DATA_REQ,
};

// telink usb report ctrl command. used mixed with USB_AUD_PLAY_PAUSE...
enum{
	USB_REPORT_NO_EVENT		= 0xf0,
	USB_REPORT_RELEASE 		= 0xff,
};

/**
 * @brief This function initializes the USB interface
 * @param   none
 * @return none
 */
void usb_init(void);

/**
 * @brief This function handles the USB related irq.
 * @param   none
 * @return none
 */
void usb_handle_irq(void);


/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif

