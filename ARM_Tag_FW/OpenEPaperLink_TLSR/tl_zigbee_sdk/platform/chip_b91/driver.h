/********************************************************************************************************
 * @file	driver.h
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
#pragma once

#include "lib/include/axon/axon_api.h"
#include "lib/include/axon/axon_dep.h"
#include "lib/include/pke/pke.h"
#include "lib/include/emi.h"
#include "lib/include/plic.h"
#include "lib/include/pm.h"
#include "lib/include/rf.h"
#include "lib/include/swire.h"
#include "lib/include/sys.h"
#include "lib/include/trng.h"

#include "lib/include/pke/pke_algorithm.h"

#include "audio.h"
#include "analog.h"
#include "dma.h"
#include "gpio.h"
#include "i2c.h"
#include "spi.h"
#include "pwm.h"
#include "timer.h"
#include "flash.h"
#include "usbhw.h"
#include "watchdog.h"
#include "s7816.h"
#include "nds_intrinsic.h"
#include "mdec.h"

#include "npe.h"
#include "uart.h"
#include "stimer.h"
#include "aes.h"

#include "lpc.h"
#include "clock.h"
#include "mspi.h"
#include "adc.h"
#include "flash/flash_type.h"
