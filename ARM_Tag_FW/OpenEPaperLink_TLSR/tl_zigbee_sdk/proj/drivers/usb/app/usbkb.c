/********************************************************************************************************
 * @file    usbkb.c
 *
 * @brief   This is the source file for usbkb
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

#include "usbkb.h"

#if (MODULE_KEYBOARD_ENABLE)

u8 usbkb_hid_report_normal(u8 ctrl_key, u8 *keycode){
	if(usbhw_is_ep_busy(USB_EDP_KEYBOARD_IN))
		return 0;

	reg_usb_ep_ptr(USB_EDP_KEYBOARD_IN) = 0;

	// please refer to keyboard_report_desc
	reg_usb_ep_dat(USB_EDP_KEYBOARD_IN) = ctrl_key;
	reg_usb_ep_dat(USB_EDP_KEYBOARD_IN) = 0;//resv
	foreach(i, KEYBOARD_REPORT_KEY_MAX){
	    reg_usb_ep_dat(USB_EDP_KEYBOARD_IN) = keycode[i];
	}
	reg_usb_ep_ctrl(USB_EDP_KEYBOARD_IN) = FLD_EP_DAT_ACK;		// ACK
	return 1;
}


#endif
