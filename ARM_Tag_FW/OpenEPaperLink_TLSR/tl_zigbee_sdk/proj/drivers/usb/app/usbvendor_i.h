/********************************************************************************************************
 * @file    usbvendor_i.h
 *
 * @brief   This is the header file for usbvendor_i
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

#include "../usbdesc.h"

#if (USB_VENDOR_ENABLE)

static const u8 vendor_report_desc[] = {
	HID_DESCRIPTOR_VENDOR(1, 4, 0, 1, 64),
};

static inline u8 *usbvendor_get_report_desc(void){
	return (u8*)(vendor_report_desc);
}

static inline u16 usbvendor_get_report_desc_size(void){
	return (u16)(sizeof(vendor_report_desc));
}

#endif
