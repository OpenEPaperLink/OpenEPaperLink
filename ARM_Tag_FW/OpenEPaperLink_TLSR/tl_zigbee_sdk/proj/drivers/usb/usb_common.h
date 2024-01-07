/********************************************************************************************************
 * @file    usb_common.h
 *
 * @brief   This is the header file for usb_common
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

#include "usb.h"

#include "app/usbcdc.h"
#include "app/usbmouse.h"
#include "app/usbkb.h"
#include "app/usbvendor.h"
#if (__PROJECT_TL_SNIFFER__)
#include "app/usbsniffer.h"
#endif


