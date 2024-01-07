/********************************************************************************************************
 * @file    platform.h
 *
 * @brief   This is the header file for platform
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


#if defined (MCU_CORE_826x)
#include "chip_826x/driver_826x.h"
#include "chip_826x/gpio_default.h"
#elif defined (MCU_CORE_8258)
#include "chip_8258/driver_8258.h"
#include "chip_8258/gpio_default.h"
#elif defined (MCU_CORE_8278)
#include "chip_8278/driver_8278.h"
#include "chip_8278/gpio_default.h"
#elif defined (MCU_CORE_B91)
#include "chip_b91/driver.h"
#include "chip_b91/gpio_default.h"
#endif

