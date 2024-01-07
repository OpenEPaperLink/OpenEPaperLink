/********************************************************************************************************
 * @file	trng_reg.h
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
#ifndef TRNG_REG_H_
#define TRNG_REG_H_

/*******************************      trng registers: 101800      ******************************/
#define REG_TRNG_BASE					0x101800


#define reg_trng_cr0					REG_ADDR8(REG_TRNG_BASE)
enum{
	FLD_TRNG_CR0_RBGEN		= 	BIT(0),
	FLD_TRNG_CR0_ROSEN0 	= 	BIT(1),
	FLD_TRNG_CR0_ROSEN1 	= 	BIT(2),
	FLD_TRNG_CR0_ROSEN2 	= 	BIT(3),
	FLD_TRNG_CR0_ROSEN3 	= 	BIT(4),
};

#define reg_trng_rtcr					REG_ADDR32(REG_TRNG_BASE+0x04)
enum{
	FLD_TRNG_RTCR_MSEL		= 	BIT(0),
};

#define reg_rbg_sr						REG_ADDR8(REG_TRNG_BASE+0x08)
enum{
	FLD_RBG_SR_DRDY			= 	BIT(0),
};

#define reg_rbg_dr						REG_ADDR32(REG_TRNG_BASE+0x0c)

#endif
