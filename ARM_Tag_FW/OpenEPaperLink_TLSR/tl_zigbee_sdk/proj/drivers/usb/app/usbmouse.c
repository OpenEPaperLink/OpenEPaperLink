/********************************************************************************************************
 * @file    usbmouse.c
 *
 * @brief   This is the source file for usbmouse
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

#include "usbmouse.h"

#if (USB_MOUSE_ENABLE)

u8 usbmouse_hid_report(u8 report_id, u8 *data, int cnt){
    if(!reg_usb_host_conn){
    	reg_usb_ep_ctrl(USB_EDP_MOUSE) = 0;
    	return 0;
    }

	if(usbhw_is_ep_busy(USB_EDP_MOUSE))
		return 0;

	reg_usb_ep_ptr(USB_EDP_MOUSE) = 0;

	// please refer to usbmouse_i.h mouse_report_desc
	extern u8 usb_mouse_report_proto;

	if(!usb_mouse_report_proto){
		reg_usb_ep_dat(USB_EDP_MOUSE) = data[0];
		reg_usb_ep_dat(USB_EDP_MOUSE) = data[1];
		reg_usb_ep_dat(USB_EDP_MOUSE) = data[2];
	}else{
		reg_usb_ep_dat(USB_EDP_MOUSE) = report_id;
		foreach(i, cnt){
			reg_usb_ep_dat(USB_EDP_MOUSE) = data[i];
		}
	}

	reg_usb_ep_ctrl(USB_EDP_MOUSE) = FLD_EP_DAT_ACK;		// ACK

	return 1;
}


#endif
