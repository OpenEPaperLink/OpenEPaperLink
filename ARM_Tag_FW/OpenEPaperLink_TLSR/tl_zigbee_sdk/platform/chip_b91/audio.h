/********************************************************************************************************
 * @file	audio.h
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
/**	@page AUDIO
 *
 *	Introduction
 *	===============
 *	audio module consists of 3 parts: audio code, audio in path , and audio out path.
 *
 *	API Reference
 *	===============
 *	Header File: audio.h
 */
#ifndef audio_H
#define audio_H

#include "reg_include/register.h"
#include "i2c.h"
#include "pwm.h"
#include "compiler.h"

typedef enum{
	I2S_BCK_PC3      = GPIO_PC3,
	I2S_ADC_LR_PC4   = GPIO_PC4,
	I2S_ADC_DAT_PC5  = GPIO_PC5,
	I2S_DAC_LR_PC6   = GPIO_PC6,
	I2S_DAC_DAT_PC7  = GPIO_PC7,
}i2s_pin_e;


typedef enum{
	DMIC_B2_DAT_B3_CLK,//mono B3 clk1
	DMIC_C1_DAT_C2_CLK,//mono C2 clk1
	DMIC_D4_DAT_D5_CLK,//mono D5 clk1
	DMIC_GROUPB_B2_DAT_B3_B4_CLK,
	DMIC_GROUPC_C1_DAT_C2_C3_CLK,
	DMIC_GROUPD_D4_DAT_D5_D6_CLK,
}dmic_pin_group_e;


typedef enum{
	MIC_MODE,
	SPK_MODE,
	LOOP_MODE,
}audio_loop_mode_e;


typedef enum{
	CODEC_I2S ,
	IO_CODEC ,
	IO_I2S,
}audio_flow_e;


typedef enum{
	I2S_RJ_MODE ,
	I2S_LJ_MODE ,
	I2S_I2S_MODE,
	I2S_DSP_MODE,
}i2s_mode_select_e;

typedef enum{
	CODEC_PARALLEL_MODE ,
	CODEC_LJ_MODE ,
	CODEC_DSP_MODE,
	CODEC_I2S_MODE,
}codec_mode_select_e;


typedef enum{
	CODEC_ITF_AC,
	CODEC_ITF_PD,
}codec_interface_select_e;

typedef enum{
   	BIT_16_MONO,
	BIT_20_OR_24_MONO ,
	BIT_16_STEREO,
	BIT_20_OR_24_STEREO ,
}audio_in_mode_e;

typedef enum{
   	BIT_16_MONO_FIFO0 ,
	BIT_20_OR_24_MONO_FIFO0 ,
	BIT_16_STEREO_FIFO0 ,
	BIT_20_OR_24_STEREO_FIFO0 ,
	BIT_16_MONO_FIFO1 ,
	BIT_20_OR_24_MONO_FIFO1 ,
	BIT_16_STEREO_FIFO0_AND_FIFO1 ,
	BIT_20_OR_24STEREO_FIFO0_AND_FIFO1 ,

}audio_out_mode_e;

typedef enum{
	I2S_BIT_16_DATA,
	I2S_BIT_20_DATA ,
	I2S_BIT_24_DATA,
}i2s_data_select_e;

typedef enum{
	CODEC_BIT_16_DATA,
	CODEC_BIT_20_DATA=2,
	CODEC_BIT_24_DATA,
}codec_data_select_e;


typedef enum{
    MONO_BIT_16,
	MONO_BIT_20,
	MONO_BIT_24,
	STEREO_BIT_16,
	STEREO_BIT_20,
	STEREO_BIT_24,
}audio_channel_wl_mode_e;


typedef enum{
	AUDIO_BIT_16_DATA,
	AUDIO_BIT_20_DATA,
	AUDIO_BIT_24_DATA,
}audio_wl_select_e;

typedef enum{
	AUDIO_MONO,
	AUDIO_STEREO,
}audio_channel_select_e;


typedef enum{
	I2S_S_CODEC_M,
	I2S_M_CODEC_S,
}i2s_codec_m_s_mode_e;

typedef enum{
	MCU_WREG,
	I2C_WREG ,
}codec_wreg_mode_e;

typedef enum{
	I2S_DATA_INVERT_DIS,
	I2S_DATA_INVERT_EN ,
}audio_data_invert_e;

typedef enum{
	I2S_LR_CLK_INVERT_DIS,
	I2S_LR_CLK_INVERT_EN,
}audio_i2s_lr_clk_invert_e;

typedef enum
{
	DAC_OUTPUT_L_R_CHN,
	DAC_OUTPUT_L_CHN,
}audio_output_chn_e;

typedef enum
{
	SINGLE_ENDED_INPUT,
	DIFF_ENDED_INPUT,
}audio_input_mode_select_e;

typedef struct {
	unsigned char  audio_in_mode;
	unsigned char  audio_out_mode;
	unsigned char 	i2s_data_select;
	unsigned char  codec_data_select;
	unsigned char  i2s_codec_m_s_mode;
	unsigned char  in_digital_gain;
	unsigned char  in_analog_gain;
	unsigned char  out_digital_gain;
	unsigned char  out_analog_gain;
	unsigned char  mic_input_mode_select;
	unsigned char  dac_output_chn_select;
}aduio_i2s_codec_config_t;


typedef struct {
	unsigned char  i2s_lr_clk_invert_select;
	unsigned char  i2s_data_invert_select;
}audio_i2s_invert_config_t;



typedef enum{
	I2S_DATA_IN_FIFO ,
	USB_DATA_IN_FIFO ,
	SAR_ADC_DATA_IN_FIFO,
	IN_NO_USE,
}audio_mux_ain_e;

typedef enum{
	I2S_OUT ,
	USB_IOS_OUT ,
	OUT_NO_USE=3,
}audio_mux_aout_e;

/**
 * define audio rate value.
 */
typedef enum{
	AUDIO_8K,
	AUDIO_16K=3,
	AUDIO_32K=6,
	AUDIO_44EP1K,
	AUDIO_48K,
	AUDIO_192K=12,
	AUDIO_ADC_16K_DAC_48K,
}audio_sample_rate_e;

typedef enum{
	AMIC_INPUT,
	DMIC_INPUT,
	LINE_INPUT,
}audio_input_mode_e;

typedef enum{
	AMIC_IN_TO_BUF,
	DMIC_IN_TO_BUF,
	LINE_IN_TO_BUF,
	AMIC_IN_TO_BUF_TO_LINE_OUT,
	DMIC_IN_TO_BUF_TO_LINE_OUT,
	LINE_IN_TO_BUF_TO_LINE_OUT,
	BUF_TO_LINE_OUT,
}audio_flow_mode_e;



typedef enum{
	AUDIO_RATE_EQUAL,
	AUDIO_RATE_GT_L0,
	AUDIO_RATE_GT_L1,
	AUDIO_RATE_LT_L0,
	AUDIO_RATE_LT_L1,
	AUDIO_MATCH_SIZE,
}audio_rate_match_e;


/*[0,+43], 1 dB steps*/
typedef enum{
	CODEC_IN_D_GAIN_0_DB=0,
	CODEC_IN_D_GAIN_4_DB=4,
	CODEC_IN_D_GAIN_8_DB=8,
	CODEC_IN_D_GAIN_12_DB=12,
	CODEC_IN_D_GAIN_16_DB=16,
	CODEC_IN_D_GAIN_20_DB=20,
	CODEC_IN_D_GAIN_43_DB=43,
}codec_in_path_digital_gain_e;


typedef enum{
	CODEC_IN_A_GAIN_0_DB,
	CODEC_IN_A_GAIN_4_DB,
	CODEC_IN_A_GAIN_8_DB,
	CODEC_IN_A_GAIN_12_DB,
	CODEC_IN_A_GAIN_16_DB,
	CODEC_IN_A_GAIN_20_DB,
}codec_in_path_analog_gain_e;



typedef enum{
	CODEC_OUT_D_GAIN_0_DB=0,
	CODEC_OUT_D_GAIN_m1_DB,
	CODEC_OUT_D_GAIN_m2_DB,
	CODEC_OUT_D_GAIN_m3_DB,
	CODEC_OUT_D_GAIN_m4_DB,
	CODEC_OUT_D_GAIN_m5_DB,
	CODEC_OUT_D_GAIN_m6_DB,
	CODEC_OUT_D_GAIN_m7_DB,
	CODEC_OUT_D_GAIN_m8_DB,
	CODEC_OUT_D_GAIN_m9_DB,
	CODEC_OUT_D_GAIN_m10_DB,
	CODEC_OUT_D_GAIN_m11_DB,
	CODEC_OUT_D_GAIN_m12_DB,
	CODEC_OUT_D_GAIN_m13_DB,
	CODEC_OUT_D_GAIN_m14_DB,
	CODEC_OUT_D_GAIN_m15_DB,
	CODEC_OUT_D_GAIN_m16_DB,
	CODEC_OUT_D_GAIN_m17_DB,
	CODEC_OUT_D_GAIN_m18_DB,
	CODEC_OUT_D_GAIN_m19_DB,
	CODEC_OUT_D_GAIN_m20_DB,
	CODEC_OUT_D_GAIN_m21_DB,
	CODEC_OUT_D_GAIN_m22_DB,
	CODEC_OUT_D_GAIN_m23_DB,
	CODEC_OUT_D_GAIN_m24_DB,
	CODEC_OUT_D_GAIN_m25_DB,
	CODEC_OUT_D_GAIN_m26_DB,
	CODEC_OUT_D_GAIN_m27_DB,
	CODEC_OUT_D_GAIN_m28_DB,
	CODEC_OUT_D_GAIN_m29_DB,
	CODEC_OUT_D_GAIN_m30_DB,
	CODEC_OUT_D_GAIN_m31_DB=31,

	CODEC_OUT_D_GAIN_32_DB,
	CODEC_OUT_D_GAIN_31_DB,
	CODEC_OUT_D_GAIN_30_DB,
	CODEC_OUT_D_GAIN_29_DB,
	CODEC_OUT_D_GAIN_28_DB,
	CODEC_OUT_D_GAIN_27_DB,
	CODEC_OUT_D_GAIN_26_DB,
	CODEC_OUT_D_GAIN_25_DB,
	CODEC_OUT_D_GAIN_24_DB,
	CODEC_OUT_D_GAIN_23_DB,
	CODEC_OUT_D_GAIN_22_DB,
	CODEC_OUT_D_GAIN_21_DB,
	CODEC_OUT_D_GAIN_20_DB,
	CODEC_OUT_D_GAIN_19_DB,
	CODEC_OUT_D_GAIN_18_DB,
	CODEC_OUT_D_GAIN_17_DB,
	CODEC_OUT_D_GAIN_16_DB,
	CODEC_OUT_D_GAIN_15_DB,
	CODEC_OUT_D_GAIN_14_DB,
	CODEC_OUT_D_GAIN_13_DB,
	CODEC_OUT_D_GAIN_12_DB,
	CODEC_OUT_D_GAIN_11_DB,
	CODEC_OUT_D_GAIN_10_DB,
	CODEC_OUT_D_GAIN_9_DB,
	CODEC_OUT_D_GAIN_8_DB,
	CODEC_OUT_D_GAIN_7_DB,
	CODEC_OUT_D_GAIN_6_DB,
	CODEC_OUT_D_GAIN_5_DB,
	CODEC_OUT_D_GAIN_4_DB,
	CODEC_OUT_D_GAIN_3_DB,
	CODEC_OUT_D_GAIN_2_DB,
	CODEC_OUT_D_GAIN_1_DB,
}codec_out_path_digital_gain_e;



typedef enum{

	CODEC_OUT_A_GAIN_12_DB,
	CODEC_OUT_A_GAIN_11_DB,
	CODEC_OUT_A_GAIN_10_DB,
	CODEC_OUT_A_GAIN_9_DB,
	CODEC_OUT_A_GAIN_8_DB,
	CODEC_OUT_A_GAIN_7_DB,
	CODEC_OUT_A_GAIN_6_DB,
	CODEC_OUT_A_GAIN_5_DB,
	CODEC_OUT_A_GAIN_4_DB,
	CODEC_OUT_A_GAIN_3_DB,
	CODEC_OUT_A_GAIN_2_DB,
	CODEC_OUT_A_GAIN_1_DB,
	CODEC_OUT_A_GAIN_0_DB,
	CODEC_OUT_A_GAIN_m1_DB,
	CODEC_OUT_A_GAIN_m2_DB,
	CODEC_OUT_A_GAIN_m3_DB,
	CODEC_OUT_A_GAIN_m4_DB,
	CODEC_OUT_A_GAIN_m5_DB,
	CODEC_OUT_A_GAIN_m6_DB,
	CODEC_OUT_A_GAIN_m7_DB,
	CODEC_OUT_A_GAIN_m8_DB,
	CODEC_OUT_A_GAIN_m9_DB,
	CODEC_OUT_A_GAIN_m10_DB,
	CODEC_OUT_A_GAIN_m11_DB,
	CODEC_OUT_A_GAIN_m12_DB,
	CODEC_OUT_A_GAIN_m13_DB,
	CODEC_OUT_A_GAIN_m14_DB,
	CODEC_OUT_A_GAIN_m15_DB,
	CODEC_OUT_A_GAIN_m16_DB,
	CODEC_OUT_A_GAIN_m17_DB,
	CODEC_OUT_A_GAIN_m18_DB,
	CODEC_OUT_A_GAIN_m19_DB,
}codec_out_path_analog_gain_e;


/*The Wind Noise filter (WNF) is a programmable high pass filter feature enabling to reduce wind noise .
The wind noise filter is a 1st order filter.
                   Mode1  -3dB   59Hz
corner frequency   Mode2  -3dB   117Hz
 	 	 	 	   Mode3  -3dB   235Hz
*/
typedef enum{
CODEC_ADC_WNF_INACTIVE,
CODEC_ADC_WNF_MODE1,
CODEC_ADC_WNF_MODE2,
CODEC_ADC_WNF_MODE3,
}adc_wnf_mode_sel_e;



typedef enum
{
	INNER_CODEC,
	EXT_CODEC,//wm8731
}codec_type_e;




typedef enum
{
	CODEC_1P8V,
	CODEC_2P8V,
}codec_volt_supply_e;

typedef struct {
	unsigned char    i2s_clk_step;
	unsigned char    i2s_clk_mode;
	unsigned char 	 i2s_bclk_div;
	unsigned short   i2s_lrclk_adc_div;
	unsigned short   i2s_lrclk_dac_div;
}aduio_i2s_clk_config_t;
/**
 * 	@brief      This function serves to set the clock of i2s
 * 	@param[in]  step - the dividing factor of step.
 * 	@param[in]  mod - the dividing factor of mod.
 * 	@return     none
 */
static inline void audio_set_i2s_clk(unsigned char step,unsigned char  mod)
{
	reg_i2s_step = (step&FLD_I2S_STEP)|FLD_I2S_CLK_EN;
	reg_i2s_mod = mod;
}

/**
 * 	@brief      This function serves to set the clock of codec
 * 	@param[in]  step - the dividing factor of step.
 * 	@param[in]  mod - the dividing factor of mod.
 * 	@return     none
 * 	@attention  codec clock  divider from pll,so pll must be 192M
 */
static inline void audio_set_codec_clk(unsigned char step,unsigned char  mod)
{
	BM_CLR(reg_dmic_clk_set,BIT(0));//set dmic_div
	reg_dmic_step = (step&FLD_DMIC_STEP)|FLD_DMIC_SEL;
	reg_dmic_mod = mod;
}


/**
 * 	@brief      This function serves to set enable i2s clk
 * 	@return     none
 */
static inline void audio_i2s_clk_en(void)
{
	BM_SET(reg_i2s_step,FLD_I2S_CLK_EN);
}

/**
 * 	@brief      This function serves to set disable i2s clk.
 * 	@return     none
 */
static inline void audio_i2s_clk_dis(void)
{
	BM_CLR(reg_i2s_step,FLD_I2S_CLK_EN);
}


/**
 * 	@brief      This function serves to set disable codec clk
 * 	@return     none
 */
static inline void audio_codec_clk_en(void)
{
	BM_SET(reg_dmic_step,FLD_DMIC_SEL);
}


/**
 * 	@brief      This function serves to set disable codec clk
 * 	@return     none
 */
static inline void audio_codec_clk_dis(void)
{
	BM_CLR(reg_dmic_step,FLD_DMIC_SEL);
}


/**
 * 	@brief      This function serves to set the bclk
 * 	@param[in]  div  - bclk=i2s_clk/(div*2),if div=0,i2s_clk=bclk.
 * 	@return     none
 */
static inline void audio_set_i2s_bclk(unsigned char div)
{
	reg_pcm_clk_num=div&0x0f;
}

/**
 * 	@brief      This function serves to set the lrclk divider.
 * 	@param[in]  adc_div - adc_lrclk=bclk/(adc_div)
 * 	@param[in]  dac_div - dac_lrclk=bclk/(dac_div)
 * 	@return     none
 */
static inline void audio_set_lrclk(unsigned short adc_div,unsigned short dac_div)
{
	reg_int_pcm_num=(adc_div-1);
	reg_dec_pcm_num=(dac_div-1);
}



/**
 * 	@brief      This function serves to set  tx buff length,
 * 	@param[in]  len - the length of tx buff
 * 	@return     none
 */
static inline void audio_set_tx_buff_len(unsigned short len)
{
	reg_tx_max=((len)>>2)-1;
}

/**
 * 	@brief      This function serves to set rx buff length,
 * 	@param[in]  len - the length of rx buff
 * 	@return     none
 */
static inline void audio_set_rx_buff_len(unsigned short len)
{
   reg_rx_max=((len)>>2)-1;
}


/**
 * 	@brief      This function serves to write codec register by mc.
 * 	@param[in]  addr - the address of codec register
 * 	@param[in]  data - Data to be written
 * 	@return     none
 */
static inline void audio_write_codec_reg(unsigned char addr,unsigned char  data)
{

	reg_audio_codec_reg(addr)=data;

}

/**
 * 	@brief      This function serves to read codec register by mc.
 * 	@param[in]  addr - the address of codec register
 * 	@return     data
 */
static inline unsigned char audio_read_codec_reg(unsigned char addr)
{
	return reg_audio_codec_reg(addr);

}

/**
 * 	@brief      This function serves to tx fifo trigger number.
 * 	@param[in]  number - the number of dma trigger
 * 	@return     none
 */
static inline void audio_set_fifo_tx_trig_num(unsigned char number)
{
	reg_fifo_trig0 &=~FLD_AUDIO_FIFO_AOUT0_TRIG_NUM;
	reg_fifo_trig0 |=(number&FLD_AUDIO_FIFO_AOUT0_TRIG_NUM);
}

/**
 * 	@brief      This function serves to rx fifo trigger number.
 * 	@param[in]  number - the number of dma trigger
 * 	@return     none
 */
static inline void  audio_set_fifo_rx_trig_num(unsigned char number)
{
	 reg_fifo_trig0 &= ~FLD_AUDIO_FIFO_AIN0_TRIG_NUM;
	 reg_fifo_trig0 |=((number&0x0f)<<4);
}

/**
 * 	@brief      This function serves to enable or disable i2s clk and codec mc clk .
 * 	@param[in]  i2s_clk_en - 1 enable  : 0 disable
 * 	@param[in]  mc_clk_en  - 1 enable  : 0 disable
 * 	@return     none
 */
static inline void  audio_clk_en(unsigned char i2s_clk_en,unsigned char mc_clk_en)
{
	reg_audio_en=MASK_VAL( FLD_AUDIO_I2S_CLK_EN, i2s_clk_en,\
			      FLD_AUDIO_MC_CLK_EN, mc_clk_en);
}


/**
 * 	@brief      This function serves to get dma rx buff pointer.
 * 	@param[in]  chn - dma channel
 * 	@return     none
 */
static inline unsigned int audio_get_rx_dma_wptr (dma_chn_e chn)
{
	return   convert_ram_addr_bus2cpu(reg_dma_dst_addr(chn));
}

/**
 * 	@brief      This function serves to get dma tx buff pointer.
 * 	@param[in]  chn - dma channel
 * 	@return     none
 */
static inline unsigned int audio_get_tx_dma_rptr (dma_chn_e chn)
{
	return convert_ram_addr_bus2cpu(reg_dma_src_addr(chn));
}


/**
 * @brief      This function serves to invert data between R channel and L channel.
 * @return     none
 */
static inline void audio_invert_data_en(void)
{
	BM_SET(reg_i2s_cfg,FLD_AUDIO_I2S_LRSWAP);
}

/**
 * @brief     This function serves to set fifo0 data path .
 * @param[in] ain0_sel :fifo0 input source select
 * @param[in] aout0_sel:fifo0 output source select
 * @return    none
 */
static inline void audio_data_fifo0_path_sel (audio_mux_ain_e ain0_sel, audio_mux_aout_e aout0_sel)
{
	reg_audio_sel = MASK_VAL( FLD_AUDIO_AIN0_SEL, ain0_sel,FLD_AUDIO_AOUT0_SEL, aout0_sel);
}


/**
 * @brief  This function serves to set fifo1 data path .
 * @param[in] ain1_sel :fifo1 input source select
 * @param[in] aout1_sel:fifo1 output source select
 * @return    none
 */
static inline void audio_data_fifo1_path_sel ( audio_mux_ain_e ain1_sel, audio_mux_aout_e aout1_sel)
{
	reg_audio_sel = MASK_VAL( FLD_AUDIO_AIN1_SEL, ain1_sel, FLD_AUDIO_AOUT1_SEL, aout1_sel);
}


/**
 *  @brief      This function serves to write codec indirect register .
 *  @param[in]  reg -  the address of codec  direct register  corresponding indirect access
 *  @param[in]  addr - the indirect access register address
 *  @param[in]  data - the data of indirect access register
 *  @return     none
 */
static inline void audio_write_codec_ind_reg(unsigned char reg ,unsigned char addr,unsigned char data )
{
	reg_audio_codec_reg(reg) = addr&0x3f;
	reg_audio_codec_reg(reg+1) = data;
	reg_audio_codec_reg(reg) = (addr&0x3f)|0x40;
}


/**
 *  @brief      This function serves to read codec indirect register.
 *  @param[in]  reg -  the address of codec  direct register  corresponding indirect access
 *  @param[in]  addr - the indirect access register address
 *  @return     data - the data of indirect access register
 */
static inline unsigned char audio_read_codec_ind_reg(unsigned char reg ,unsigned char addr )
{
	reg_audio_codec_reg(reg) = (addr&0x3f);
	return  reg_audio_codec_reg(reg+1);
}


/**
 * @brief      This function serves to invert data between R channel and L channel.
 * @return     none
 */
static inline void audio_invert_data_dis(void)
{
	BM_CLR(reg_i2s_cfg,FLD_AUDIO_I2S_LRSWAP);
}

/**
 * @brief      This function reset AUDIO module.
 * @return     none
 */
static inline void audio_reset(void)
{
	reg_rst2&=(~FLD_RST2_AUD);
	reg_rst2 |=FLD_RST2_AUD;
}

/**
 * @brief      This function reset CODEC module.
 * @return     none
 */
static inline void codec_reset(void)
{
	reg_rst3&=(~FLD_RST3_CODEC);
	reg_rst3 |=FLD_RST3_CODEC;
}

/**
 * @brief  This function serves to set wind noise filter(WNF),it is a programmable high pass filter feature enabling to reduce wind noise.
 * @param[in] mode - the wind noise filter mode,the wind noise filter is a 1st order filter.
 *                                             CODEC_ADC_WNF_INACTIVE  disable
 *
 *                                             CODEC_ADC_WNF_MODE1  -3dB   59Hz
 *  Wind Noise Filter corner frequency         CODEC_ADC_WNF_MODE2  -3dB   117Hz
 *			                                   CODEC_ADC_WNF_MODE3  -3dB   235Hz
 * @return    none
 */
static inline void audio_set_codec_adc_wnf(adc_wnf_mode_sel_e mode)
{
	reg_audio_codec_adc_wnf_ctr = mode;
}

/**
 * @brief      This function serves to mute adc.
 * @return     none
 */
static inline void audio_set_codec_adc_mute(void)
{
	BM_SET(reg_audio_codec_adc12_ctr,FLD_AUDIO_CODEC_ADC12_SOFT_MUTE); //adc mute
	while(reg_audio_codec_stat2_ctr&FLD_AUDIO_CODEC_ADC12_SMUTE_IN_PROG);//wait codec adc mute is completed
}

/**
 * @brief      This function serves to unmute adc.
 * @return     none
 */
static inline void audio_set_codec_adc_unmute(void)
{
	 BM_CLR(reg_audio_codec_adc12_ctr,FLD_AUDIO_CODEC_ADC12_SOFT_MUTE);//adc unmute
	 while(reg_audio_codec_stat2_ctr&FLD_AUDIO_CODEC_ADC12_SMUTE_IN_PROG);//wait codec adc unmute is completed
}

/**
 * @brief      This function serves to mute dac.
 * @return     none
 */
static inline void audio_set_codec_dac_mute(void)
{
	BM_SET(reg_audio_codec_dac_ctr,FLD_AUDIO_CODEC_DAC_SOFT_MUTE); //dac mute
	while(reg_audio_codec_stat2_ctr&FLD_AUDIO_CODEC_DAC_SMUTE_IN_PROG);//wait codec dac mute is completed
}

/**
 * @brief      This function serves to unmute dac.
 * @return     none
 */
static inline void audio_set_codec_dac_unmute(void)
{
	BM_CLR(reg_audio_codec_dac_ctr,FLD_AUDIO_CODEC_DAC_SOFT_MUTE);//dac unmute
	while(reg_audio_codec_stat2_ctr&FLD_AUDIO_CODEC_DAC_SMUTE_IN_PROG);//wait codec dac unmute is completed
}

/**
 * 	@brief      This function serves to set adc analog gain.
 * 	@param[in]  a_gain - analog  gain value
 * 	@return     none
 */
static inline void audio_set_codec_adc_a_gain (codec_in_path_analog_gain_e a_gain )
{
	/*analog 0/4/8/12/16/20 dB boost gain*/
	 reg_audio_codec_mic_l_R_gain= MASK_VAL( FLD_AUDIO_CODEC_AMIC_L_GAIN, a_gain,\
	           FLD_AUDIO_CODEC_AMIC_R_GAIN, a_gain);
}

/**
 * @brief      This function serves to set adc digital gain.
 * @param[in]  d_gain - digital gain value
 * @return     none
 */
static inline void audio_set_codec_adc_d_gain(codec_in_path_digital_gain_e d_gain)
{
	 /*0db~43db  1db step ,digital programmable gain*/
	 reg_audio_adc1_gain=MASK_VAL(  FLD_AUDIO_CODEC_ADC_LRGID,1,\
	       FLD_AUDIO_CODEC_ADC_GID1,d_gain);
}

/**
 * @brief      This function serves to set dac analog gain.
 * @param[in]  d_gain - digital gain value
 * @return     none
 */
static inline void audio_set_codec_dac_a_gain(codec_out_path_analog_gain_e a_gain)
{
	 /*disable Headphone gain coupling, set Left channel HP amplifier gain*/
			reg_audio_codec_hpl_gain=MASK_VAL(FLD_AUDIO_CODEC_HPL_LRGO,0,\
												FLD_AUDIO_CODEC_HPL_GOL,a_gain);
			reg_audio_codec_hpr_gain=MASK_VAL(FLD_AUDIO_CODEC_HPR_GOR, a_gain); /* Right channel HP amplifier gain programming*/
}


/**
 * @brief      This function serves to set dac analog gain.
 * @param[in]  d_gain - digital gain value
 * @return     none
 */
static inline void audio_set_codec_dac_d_gain(codec_out_path_digital_gain_e d_gain)
{
	/*disable DAC digital gain coupling, Left channel DAC digital gain*/
	reg_audio_codec_dacl_gain=MASK_VAL(FLD_AUDIO_CODEC_DAC_LRGOD,0,\
											FLD_AUDIO_CODEC_DAC_GODL,d_gain);
	reg_audio_codec_dacr_gain=MASK_VAL(FLD_AUDIO_CODEC_DAC_GODR,d_gain); /*Right channel DAC digital gain*/
}

/**
 * 	@brief      This function serves to set codec supply voltage
 * 	@param[in]  volt - the voltage of codec supply.A1 2.8V default,A0 1.8V default.
 * 	@return     none
 *
 */
void audio_set_codec_supply (codec_volt_supply_e volt);

/**
 * @brief     This function configures dmic pin.
 * @param[in] pin_gp - the group of dmic pin
 * @return    none
 */
void audio_set_dmic_pin(dmic_pin_group_e pin_gp);

/**
 * @brief     This function serves to set data path.
 * @param[in] audio_flow    - audio flow  select
 * @param[in] ain0_mode     - fifo0 input mode select
 * @param[in] ain1_mode     - fifo1 input mode select
 * @param[in] i2s_aout_mode - fifo output source select
 * @return    none
 */
void audio_mux_config(audio_flow_e audio_flow, audio_in_mode_e ain0_mode , audio_in_mode_e ain1_mode,audio_out_mode_e i2s_aout_mode);


/**
 * @brief     This function serves to config codec for dac.
 * @param[in] mode        - select i2s as master or slave
 * @param[in] rate		  - audio sampling rate
 * @param[in] data_select - codec dac word length
 * @param[in] wreg_mode   - mcu or i2c config codec
 * @return    none
 */
void audio_codec_dac_config(i2s_codec_m_s_mode_e mode,audio_sample_rate_e rate,codec_data_select_e data_select,codec_wreg_mode_e  wreg_mode);

/**
 * @brief     This function serves to config codec for adc.
 * @param[in] mode        - select i2s as master or slave
 * @param[in] in_mode     - line_in/amic/dmic input mode select
 * @param[in] rate        - audio sampling rate
 * @param[in] data_select - codec adc word length
 * @param[in] wreg_mode   - mcu or i2c config codec
 * @return    none
 */
void audio_codec_adc_config(i2s_codec_m_s_mode_e mode,audio_input_mode_e in_mode,audio_sample_rate_e rate,codec_data_select_e data_select,codec_wreg_mode_e  wreg_mode);



/**
 * @brief     This function serves to config interface, word length, and m/s .
 * @param[in] i2s_format - interface protocol
 * @param[in] wl   		 - audio data word length
 * @param[in] m_s        - select i2s as master or slave
 * @param[in] i2s_config_t - the prt of i2s_config_t that configure i2s lr_clk phase and lr_clk swap.
 *  i2s_config_t->i2s_lr_clk_invert_select-lr_clk phase control(in RJ,LJ or i2s modes),in i2s mode(opposite phasing in  RJ,LJ mode), 0=right channel data when lr_clk high ,1=right channel data when lr_clk low.
 *                                                                                     in DSP mode(in DSP mode only), DSP mode A/B select,0=DSP mode A ,1=DSP mode B.
 *            i2s_config_t->i2s_data_invert_select - 0=left channel data left,1=right channel data left.
 * @attention:If the left and right channels are both active in i2s mode,there will be a phase difference(about 1 sample) between the left and right channels,you can set i2s_lr_clk_invert_select=1 to eliminate the phase difference,
 * but data output channel will be inverted,you can also set i2s_config_t->i2s_data_invert_select=1 to recovery it.
 * @return    none
 */
void audio_i2s_config(i2s_mode_select_e i2s_format,i2s_data_select_e wl,  i2s_codec_m_s_mode_e m_s , audio_i2s_invert_config_t * i2s_config_t);

/**
 * @brief     This function serves to set i2s clock and audio sampling rate when i2s as master.
 * @param[in] audio_rate - audio sampling rate
 * @param[in] match		 - the match of audio rate.
 * @param[in] match_en   - initial must 0, then change rate must 1
 * @return    none
 * @attention i2s clock  divider from pll,sampling rate calculation is based on pll=192M,so pll must be 192M
 */
_attribute_ram_code_sec_noinline_ void  audio_set_i2s_clock (audio_sample_rate_e audio_rate,audio_rate_match_e match, unsigned char match_en);

/**
 * @brief     This function serves to config  rx_dma channel.
 * @param[in] chn          - dma channel
 * @param[in] dst_addr     - the dma address of destination
 * @param[in] data_len     - the length of dma rx size by byte
 * @param[in] head_of_list - the head address of dma llp.
 * @return    none
 */
void audio_rx_dma_config(dma_chn_e chn,unsigned short * dst_addr,unsigned int data_len,dma_chain_config_t *head_of_list);

/**
 * @brief     This function serves to set rx dma chain transfer
 * @param[in] rx_config - the head of list of llp_pointer.
 * @param[in] llpointer - the next element of llp_pointer.
 * @param[in] dst_addr  -the dma address of destination.
 * @param[in] data_len  -the length of dma size by byte.
 * @return    none
 */
void audio_rx_dma_add_list_element(dma_chain_config_t * rx_config,dma_chain_config_t *llpointer ,unsigned short * dst_addr,unsigned int data_len);

/**
 * @brief     This function serves to config  tx_dma channel.
 * @param[in] chn          - dma channel
 * @param[in] src_addr     - the address of source
 * @param[in] data_len     - the length of dma rx size by byte
 * @param[in] head_of_list - the head address of dma llp.
 * @return    none
 */
void audio_tx_dma_config(dma_chn_e chn,unsigned short * src_addr, unsigned int data_len,dma_chain_config_t * head_of_list);

/**
 * @brief     This function serves to set tx dma chain transfer
 * @param[in] config_addr - the head of list of llp_pointer.
 * @param[in] llpointer   - the next element of llp_pointer.
 * @param[in] src_addr    - the address of source
 * @param[in] data_len    - the length of dma size by byte.
 * @return    none
 */
void audio_tx_dma_add_list_element(dma_chain_config_t *config_addr,dma_chain_config_t *llpointer ,unsigned short * src_addr,unsigned int data_len);

/**
 * @brief     This function serves to enable rx_dma channel.
 * @return    none
 */
void audio_rx_dma_en(void);

/**
 * @brief     This function serves to disable rx_dma channel.
 * @return    none
 */
 void audio_rx_dma_dis(void);

/**
 * @brief     This function serves to enable tx_dma channel.
 * @return    none
 */
void audio_tx_dma_en(void);


/**
 * @brief     This function serves to disable tx_dma channel.
 * @return    none
 */
void audio_tx_dma_dis(void);


/**
 * @brief     This function serves to  initialize audio by mc
 * @param[in] flow_mode  - select input out flow mode
 * @param[in] rate       - audio sampling rate.
 * @param[in] channel_wl - word length and channel number.
 * @return    none
 */
void audio_init(audio_flow_mode_e flow_mode,audio_sample_rate_e rate,audio_channel_wl_mode_e channel_wl);


/**
 * @brief     This function serves to  initialize audio by i2c
 * @param[in] flow_mode  - select input out flow mode
 * @param[in] rate       - audio sampling rate.
 * @param[in] channel_wl - word length and channel number.
 * @return    none
 */
void audio_init_i2c(audio_flow_mode_e flow_mode,audio_sample_rate_e rate,audio_channel_wl_mode_e channel_wl);


/**
 * @brief This function serves to initialize audio(external codec WM8731) by i2c.
 * @param[in]  pwm0_pin - the pin of pwm0
 * @param[in]  sda_pin  - the pin port selected as I2C sda pin port.
 * @param[in]  scl_pin  - the pin port selected as I2C scl pin port.
 * @return    none
 */
void audio_i2s_init(pwm_pin_e pwm0_pin, i2c_sda_pin_e sda_pin,i2c_scl_pin_e scl_pin);

/**
 * @brief  This function serves to set audio rx dma chain transfer.
 * @param[in] chn       -  dma channel
 * @param[in] in_buff   - the pointer of rx_buff.
 * @param[in] buff_size - the size of rx_buff.
 * @return    none
 */
void audio_rx_dma_chain_init (dma_chn_e chn,unsigned short * in_buff,unsigned int buff_size );

/**
 * @brief  This function serves to initialize audio tx dma chain transfer.
 * @param[in] chn       - dma channel
 * @param[in] out_buff  - the pointer of tx_buff.
 * @param[in] buff_size - the size of tx_buff.
 * @return    none
 */
void audio_tx_dma_chain_init (dma_chn_e chn,unsigned short * out_buff,unsigned int buff_size);

/**
 * @brief      This function serves to invert data between R channel and L channel.
 * @param[in]  en - I2S_DATA_INVERT_DIS: L channel ( left channel data left);  I2S_DATA_INVERT_EN(right channel data left)
 * @attention must be set before audio_init().
 * @return     none
 */
void audio_set_mono_chn(audio_data_invert_e en);

/**
 * @brief      This function serves to invert LR-clk.
 * @param[in]  en -lr_clk phase control(in RJ,LJ or i2s modes),in i2s mode(opposite phasing in  RJ,LJ mode), 0=right channel data when lr_clk high ,1=right channel data when lr_clk low.
 *                                                             in DSP mode(in DSP mode only), DSP mode A/B select,0=DSP mode A ,1=DSP mode B
 * @attention  If the left and right channels are both active,there will be a phase difference(about 1 sample) between the left and right channels,invert lr_clk can eliminate the phase difference,but data output channel will invert.
 * @attention must be set before audio_init().
 * @return     none
 */
void audio_invert_i2s_lr_clk(audio_i2s_lr_clk_invert_e en);


/**
 * @brief      This function serves to set mic input mode.
 * @param[in]  input_mode - 0 single-ended input, 1 differential input.
 * @attention must be set before audio_init().
 * @return     none
 */
void audio_set_codec_mic_input_mode (audio_input_mode_select_e input_mode);

/**
 * 	@brief      This function serves to set in path digital and analog gain  .
 * 	@param[in]  d_gain - digital gain value
 * 	@param[in]  a_gain - analog  gain value
 * 	@return     none
 */
void audio_set_codec_in_path_a_d_gain (codec_in_path_digital_gain_e d_gain,codec_in_path_analog_gain_e a_gain );

/**
 * 	@brief      This function serves to set out path digital and analog gain  .
 * 	@param[in]  d_gain - digital gain value
 * 	@param[in]  a_gain - analog  gain value
 * 	@return     none
 */
 void audio_set_codec_out_path_a_d_gain (codec_out_path_digital_gain_e d_gain,codec_out_path_analog_gain_e a_gain);

 /**
  * @brief      This function serves to choose which is master to provide clock.
  * @param[in]  m_s - I2S_S_CODEC_M: i2s as slave ,codec as master; I2S_M_CODEC_S: i2s as  master, codec  as slave.
  * @attention must be set before audio_init().
  * @return     none
  */
 void audio_set_i2s_codec_m_s (i2s_codec_m_s_mode_e m_s);



 /**
  * @brief  This function serves to set dac output channel.
  * @param[in] chn -DAC_OUTPUT_L_R_CHN - right and left channel both active ; DAC_OUTPUT_L_CHN, only left channel active.
  * @return    none
  * @attention must be set before audio_init().
  */
 void audio_set_output_chn(audio_output_chn_e chn);

 /**
  * @brief    This function serves to active soft mute dac and disable dma.
  * @return    none
  */
  void audio_pause_out_path(void);


  /**
   * @brief    This function serves to inactive soft mute dac and enable dma .
   * @return    none
   */
  void audio_resume_out_path(void);

 /**
  * @brief     This function serves to change sample rate for dac.
  * @param[in] rate     -  the sample rate of dac
  * @return    none
  */
 _attribute_ram_code_sec_noinline_ void audio_change_sample_rate (audio_sample_rate_e  rate);


 /**
  * @brief     This function serves to power down codec_dac.
  * @return    none
  */
void audio_codec_dac_power_down(void);

 /**
  * @brief     This function serves to power on codec_dac.
  * @return    none
  */
void audio_codec_dac_power_on(void);

/**
 * @brief     This function serves to power down codec_adc.
 * @return    none
 */
void audio_codec_adc_power_down(void);

/**
 * @brief     This function serves to power on codec_adc.
 * @return    none
 */
void audio_codec_adc_power_on(void);
#endif
