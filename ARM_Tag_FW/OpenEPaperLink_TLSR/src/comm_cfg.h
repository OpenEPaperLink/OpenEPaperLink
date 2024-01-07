/********************************************************************************************************
 * @file    comm_cfg.h
 *
 * @brief   This is the header file for comm_cfg
 *
 * @author  Zigbee Group
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


/**********************************************************************
 * User configuration whether to use boot loader mode.
 *
 * NOTE:
 * We do not recommend using BootLoader mode on 512K flash chips,
 * because in boot loader mode, it can only support up to 196k firmware
 * according to the current default Flash Map.
 *
 * Please refer to the drv_nv.h file, and check if the firmware size
 * exceeds the maximum limit.
 *
 * The user can configure the CHIP_TYPE used by the corresponding
 * project in the version.h file according to the actual size
 * of the flash at hand.
 *
 * If BOOT_LOADER_MODE is 0, it means that the SDK uses Normal Mode,
 * hardware multi-address (0x0000 or 0x40000) startup mode.
 * If BOOT_LOADER_MODE is 1, it means that the SDK uses Boot Loader Mode.
 *
 * Normal mode is used by default.
 */
#define BOOT_LOADER_MODE					0



/* Boot loader address. */
#define BOOT_LOADER_IMAGE_ADDR				0x0

/* APP image address. */
#if (BOOT_LOADER_MODE)
	#define APP_IMAGE_ADDR					0x8000
#else
	#define APP_IMAGE_ADDR					0x0
#endif


/* Chip IDs */
#define TLSR_8267							0x00
#define TLSR_8269							0x01
#define TLSR_8258_512K						0x02
#define TLSR_8258_1M						0x03
#define TLSR_8278							0x04
#define TLSR_B91							0x05

/* Image types */
#if (BOOT_LOADER_MODE)
	#define IMAGE_TYPE_BOOT_FLAG			1
#else
	#define IMAGE_TYPE_BOOT_FLAG			0
#endif
#define IMAGE_TYPE_BOOTLOADER				(0xFF)
#define IMAGE_TYPE_GW						(0x00 | (IMAGE_TYPE_BOOT_FLAG << 7))
#define IMAGE_TYPE_LIGHT					(0x01 | (IMAGE_TYPE_BOOT_FLAG << 7))
#define IMAGE_TYPE_SWITCH					(0x02 | (IMAGE_TYPE_BOOT_FLAG << 7))
#define IMAGE_TYPE_CONTACT_SENSOR			(0x03 | (IMAGE_TYPE_BOOT_FLAG << 7))
