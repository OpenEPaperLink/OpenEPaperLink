/********************************************************************************************************
 * @file    link_cfg.S
 *
 * @brief   This is the boot file for link_cfg
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

#include "version_cfg.h"

LINK_CFG:
	.equ __BOOT_LOADER_IMAGE,	 	IS_BOOT_LOADER_IMAGE
	.equ __FW_RAMCODE_SIZE_MAX,		RESV_FOR_APP_RAM_CODE_SIZE
	.equ __FW_OFFSET,			 	IMAGE_OFFSET

	.global __BOOT_LOADER_IMAGE
	.global __FW_OFFSET
	.global __FW_RAMCODE_SIZE_MAX
LINK_CFG_END:
