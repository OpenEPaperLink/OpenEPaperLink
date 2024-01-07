/********************************************************************************************************
 * @file    usbmouse.h
 *
 * @brief   This is the header file for usbmouse
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

#include "usbmouse_i.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	extern "C" {
#endif



u8 usbmouse_hid_report(u8 report_id, u8 *data, int cnt);

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	}
#endif
