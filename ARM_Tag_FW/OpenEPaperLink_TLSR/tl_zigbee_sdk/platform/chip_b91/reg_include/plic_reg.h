/********************************************************************************************************
 * @file	plic_reg.h
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
#ifndef INTERRUPT_REG_H
#define INTERRUPT_REG_H
#include "soc.h"


/*******************************     interrupt registers:     ******************************/
#define reg_irq_feature		         (*(volatile unsigned long*)(0 + (0xe4000000)))

#define reg_irq_pending(i)           (*(volatile unsigned long*)(0 + (0xe4001000+((i>31) ? 4 : 0))))

#define reg_irq_src0             	 (*(volatile unsigned long*)(0 + (0xe4002000)))
#define reg_irq_src1             	 (*(volatile unsigned long*)(0 + (0xe4002004)))

#define reg_irq_src(i)             	 (*(volatile unsigned long*)(0 + (0xe4002000+((i>31) ? 4 : 0) )))

#define reg_irq_threshold            (*(volatile unsigned long*)(0 + (0xe4200000)))
#define reg_irq_done          	  	 (*(volatile unsigned long*)(0 + (0xe4200004)))

#define reg_irq_src_priority(i)      (*(volatile unsigned long*)(0 + 0xe4000000+((i)<<2)))

#endif
