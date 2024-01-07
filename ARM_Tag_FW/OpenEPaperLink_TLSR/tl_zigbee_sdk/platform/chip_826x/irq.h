/********************************************************************************************************
 * @file    irq.h
 *
 * @brief   This is the header file for B86
 *
 * @author  Driver & Zigbee Group
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

#include "register.h"
#include "bit.h"


// enalbe interrupt
static inline unsigned char irq_enable(void){
	unsigned char r = reg_irq_en;		// don't worry,  the compiler will optimize the return value if not used
	reg_irq_en = 1;
	return r;
}

static inline unsigned char irq_disable(void){
	unsigned char r = reg_irq_en;		// don't worry,  the compiler will optimize the return value if not used
	reg_irq_en = 0;
	return r;
}

static inline void irq_restore(unsigned char en){
	reg_irq_en = en;
}

static inline unsigned int irq_get_mask(void){
	return reg_irq_mask;
}

static inline void irq_set_mask(unsigned long msk){
    SET_FLD(reg_irq_mask, msk);
}

static inline void irq_clr_mask(unsigned long msk){
    CLR_FLD(reg_irq_mask, msk);
}

static inline unsigned long irq_get_src(void){
	return reg_irq_src;
}

static inline void irq_clr_all_src(void){
	reg_irq_src = ONES_32;	// set to clear
}

static inline void irq_enable_type(unsigned long msk){
	irq_set_mask(msk);
}

static inline void irq_disable_type(unsigned long msk){
	irq_clr_mask(msk);
}

static inline void irq_init(unsigned long irq_mask){
	reg_irq_mask = irq_mask;
}

static inline void dma_init(unsigned char dma_mask){
	reg_dma_chn_irq_msk = dma_mask;
}

static inline void dma_irq_clear(void){
	reg_dma_chn_irq_msk = 0;
}

static inline void dma_irq_enable(unsigned char msk)
{
	reg_dma_chn_irq_msk |= msk;
}

static inline void dma_irq_disable(unsigned char msk)
{
	reg_dma_chn_irq_msk &= ~msk;
}

static inline void dma_channel_enable_all(void){
	reg_dma_chn_en = 0xff;
}

static inline void dma_channel_disable_all(void){
	reg_dma_chn_en = 0;
}

/**
 * @brief      Get IRQ status of RF.
 * @param[in]  none
 * @return     none
 */
static inline unsigned short rf_irq_src_get(void){
    return reg_rf_irq_status;
}

static inline void rf_irq_clr_src(unsigned short msk)
{
    reg_rf_irq_status = msk;
}

/**
 * @brief      Get IRQ status of uart.
 * @param[in]  irq_src - select tx or rx irq.
 * @return     none
 */
static inline unsigned char dma_chn_irq_status_get(void){
    return reg_dma_irq_src;
}

/**
 * @brief      Clear IRQ status of uart.
 * @param[in]  irq_src - select tx or rx irq.
 * @return     none
 */
static inline void dma_chn_irq_status_clr(unsigned char irq_status){
	reg_dma_irq_src = irq_status;
}

