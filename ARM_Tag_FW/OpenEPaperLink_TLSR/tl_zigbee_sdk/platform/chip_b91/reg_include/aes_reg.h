/********************************************************************************************************
 * @file	aes_reg.h
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
#ifndef _AES_REG_H_
#define _AES_REG_H_

#include "soc.h"


#define reg_aes_mode     			REG_ADDR32(0x1600b0)
enum{
	FLD_AES_START	=	BIT(0),
	FLD_AES_MODE	=	BIT(1),   /**< 0-ciher  1-deciher */
};

#define reg_embase_addr     		REG_ADDR32(0x140b04)

#define	reg_aes_irq_mask			REG_ADDR32(0x16000c)

#define reg_aes_irq_status			REG_ADDR32(0x160010)

#define reg_aes_clr_irq_status		REG_ADDR32(0x160018)
/**
 *  @brief  Define AES IRQ
 */
typedef enum{
	FLD_CRYPT_IRQ		= BIT(7),
}aes_irq_e;


#define reg_aes_key(v)     			REG_ADDR32(0x1600b4+(v*4))

#define reg_aes_ptr     			REG_ADDR32(0x1600c4)

#endif /* _AES_REG_H_ */
