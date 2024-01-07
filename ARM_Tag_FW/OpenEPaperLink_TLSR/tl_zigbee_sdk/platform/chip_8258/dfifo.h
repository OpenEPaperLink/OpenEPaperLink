/********************************************************************************************************
 * @file    dfifo.h
 *
 * @brief   This is the header file for B85
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

#ifndef 	DFIFO_H
#define 	DFIFO_H

#include "register.h"


/**
 * @brief      This function performs to enable audio input of DFIFO2.
 * @param[in]  none.
 * @return     none.
 */
static inline void dfifo_enable_dfifo2(void)
{
	reg_dfifo_mode |= FLD_AUD_DFIFO2_IN;
}

/**
 * @brief      This function performs to disable audio input of DFIFO2.
 * @param[in]  none.
 * @return     none.
 */
static inline void dfifo_disable_dfifo2(void)
{
	reg_dfifo_mode &= ~FLD_AUD_DFIFO2_IN;
}

/**
 * @brief      This function performs to start w/r data into/from DFIFO0.
 * @param[in]  pbuff - address in DFIFO0.
 * @param[in]  size_buff - depth of DFIFO0.
 * @return     none.
 */
static inline void dfifo_set_dfifo0(unsigned short* pbuff,unsigned int size_buff)
{
	reg_dfifo0_addr = (unsigned short)((unsigned int)pbuff);
	reg_dfifo0_size = (size_buff>>4)-1;
}

/**
 * @brief      This function performs to start w/r data into/from DFIFO1.
 * @param[in]  pbuff - address in DFIFO1.
 * @param[in]  size_buff - depth of DFIFO1.
 * @return     none.
 */
static inline void dfifo_set_dfifo1(unsigned short* pbuff,unsigned int size_buff)
{
	reg_dfifo1_addr = (unsigned short)((unsigned int)pbuff);
	reg_dfifo1_size = (size_buff>>4)-1;
}

/**
 * @brief      This function performs to start w/r data into/from DFIFO2.
 * @param[in]  pbuff - address in DFIFO2.
 * @param[in]  size_buff - depth of DFIFO2.
 * @return     none.
 */
static inline void dfifo_set_dfifo2(unsigned short* pbuff,unsigned int size_buff)
{
	reg_dfifo2_addr = (unsigned short)((unsigned int)pbuff);
	reg_dfifo2_size = (size_buff>>4)-1;
}

/**
 * @brief      This function performs to set MISC channel.
 * @param[in]  pbuff - address in FIFO2.
 * @param[in]  size_buff - depth of FIFO2.
 * @return     none.
 */
static inline void adc_config_misc_channel_buf(unsigned short* pbuff,unsigned int size_buff)
{
	reg_dfifo_misc_chn_addr = (unsigned short)((unsigned int)pbuff);
	reg_dfifo_misc_chn_size = (size_buff>>4)-1;

	reg_dfifo2_wptr = 0;  //clear dfifo2 write pointer
}

/**
 * @brief     configure the mic buffer's address and size
 * @param[in] pbuff - the first address of SRAM buffer to store MIC data.
 * @param[in] size_buff - the size of pbuff.
 * @return    none
 */
static inline void audio_config_mic_buf(unsigned short* pbuff,unsigned int size_buff)
{
	reg_dfifo_audio_addr = (unsigned short)((unsigned int)pbuff);
	reg_dfifo_audio_size = (size_buff>>4)-1;
}

#endif


