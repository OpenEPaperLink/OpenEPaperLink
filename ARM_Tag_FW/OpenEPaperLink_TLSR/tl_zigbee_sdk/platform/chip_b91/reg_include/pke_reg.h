/********************************************************************************************************
 * @file	pke_reg.h
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

#include "soc.h"



/*******************************      pke registers: 0x110000      ******************************/

#define PKE_BASE             (0x80110000)
#define PKE_CTRL             (*((volatile unsigned int *)(PKE_BASE)))
#define PKE_CONF             (*((volatile unsigned int *)(PKE_BASE+0x04)))
typedef enum{
	FLD_PKE_CONF_IRQ_EN			= BIT(8),
	FLD_PKE_CONF_PARTIAL_RADIX	= BIT_RNG(16,23),
	FLD_PKE_CONF_BASE_RADIX		= BIT_RNG(24,26),
}pke_conf_e;
#define PKE_MC_PTR           (*((volatile unsigned int *)(PKE_BASE+0x10)))
#define PKE_STAT             (*((volatile unsigned int *)(PKE_BASE+0x20)))
#define PKE_RT_CODE          (*((volatile unsigned int *)(PKE_BASE+0x24)))
#define PKE_EXE_CONF         (*((volatile unsigned int *)(PKE_BASE+0x50)))
#define PKE_VERSION          (*((volatile unsigned int *)(PKE_BASE+0x80)))
#define PKE_A(a, step)       ((volatile unsigned int *)(PKE_BASE+0x0400+(a)*(step)))
#define PKE_B(a, step)       ((volatile unsigned int *)(PKE_BASE+0x1000+(a)*(step)))




