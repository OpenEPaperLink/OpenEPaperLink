/********************************************************************************************************
 * @file	mdec_reg.h
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

#define mdec_rst_addr                   0x53
typedef enum{
	FLD_SELE_PA0		= BIT(0),
	FLD_SELE_PB7		= BIT(1),
	FLD_SELE_PC4		= BIT(2),
	FLD_SELE_PD0		= BIT(3),
	FLD_SELE_PE0		= BIT(4),
}mdec_pin_e;
typedef enum{
	FLD_CLS_MDEC	    = BIT_RNG(0,4),
	FLD_RSVD			= BIT_RNG(5,6),
	FLD_MDEC_RST		= BIT(7),
}mdec_set_e;

#define	mdec_ctrl						0x54





