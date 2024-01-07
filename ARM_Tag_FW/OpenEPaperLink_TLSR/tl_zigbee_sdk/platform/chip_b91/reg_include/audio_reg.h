/********************************************************************************************************
 * @file	audio_reg.h
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
#ifndef AUDIO_REG_H
#define AUDIO_REG_H
#include "soc.h"

#define 	REG_AUDIO_AHB_BASE		    0x80120000
#define 	REG_CODEC_BASE_ADDR		    0x120200
#define 	REG_AUDIO_APB_BASE		    0x140500
#define 	reg_fifo_buf_adr(i) 		REG_AUDIO_AHB_BASE+(i)*0x40
#define     reg_audio_en				    REG_ADDR8(REG_AUDIO_APB_BASE+0x00)

#define reg_audio_codec_reg(x)			REG_ADDR8(REG_CODEC_BASE_ADDR+((x)<<2))


enum
{
	FLD_AUDIO_I2S_CLK_EN               =  BIT(0),
	FLD_AUDIO_CLK_DIV2                 =  BIT(1),
	FLD_AUDIO_MC_CLK_EN                =  BIT(2),
	FLD_AUDIO_MC_CLK_INV_O             =  BIT(3),
};

#define   reg_i2s_cfg		         REG_ADDR8(REG_AUDIO_APB_BASE+0x01)
enum
{
	FLD_AUDIO_I2S_FORMAT                  =  BIT_RNG(0,1),
	FLD_AUDIO_I2S_WL                      =  BIT_RNG(2,3),
	FLD_AUDIO_I2S_LRP                     =  BIT(4),
	FLD_AUDIO_I2S_LRSWAP                  =  BIT(5),
	FLD_AUDIO_I2S_ADC_DCI_MS              =  BIT(6),
	FLD_AUDIO_I2S_DAC_DCI_MS              =  BIT(7),
};

#define   reg_i2s_cfg2		         REG_ADDR8(REG_AUDIO_APB_BASE+0x02)

enum
{
	FLD_AUDIO_FIFO1_RST                     =  BIT(3),
};
#define   reg_audio_ctrl				    REG_ADDR8(REG_AUDIO_APB_BASE+0x03)
enum
{
	FLD_AUDIO_I2S_CMODE                  =  BIT_RNG(0,1),
	FLD_AUDIO_CODEC_I2S_SEL              =  BIT(2),
	FLD_AUDIO_I2S_OUT_BIT_SEL            =  BIT(3),
};

#define   reg_audio_tune				    REG_ADDR8(REG_AUDIO_APB_BASE+0x04)
enum
{
	FLD_AUDIO_I2S_I2S_AIN0_COME            =  BIT_RNG(0,1),
	FLD_AUDIO_I2S_I2S_AIN1_COME            =  BIT_RNG(2,3),
	FLD_AUDIO_I2S_I2S_AOUT_COME            =  BIT_RNG(4,6),
};


#define   reg_audio_sel				    REG_ADDR8(REG_AUDIO_APB_BASE+0x05)
enum
{
	FLD_AUDIO_AIN0_SEL             =  BIT_RNG(0,1),
	FLD_AUDIO_AOUT0_SEL            =  BIT_RNG(2,3),
	FLD_AUDIO_AIN1_SEL             =  BIT_RNG(4,5),
	FLD_AUDIO_AOUT1_SEL            =  BIT_RNG(6,7),
};

#define   reg_audio_i2c_addr			    REG_ADDR8(REG_AUDIO_APB_BASE+0x08)
#define   reg_audio_i2c_mode			    REG_ADDR8(REG_AUDIO_APB_BASE+0x09)

#define   reg_fifo_trig0				    REG_ADDR8(REG_AUDIO_APB_BASE+0x0a)

#define   reg_audio_ptr_set			   	    REG_ADDR8(REG_AUDIO_APB_BASE+0x10)
enum
{
	FLD_AUDIO_TX_PTR_SEL                 =  BIT(0),
	FLD_AUDIO_RX_PTR_SEL                 =  BIT(4),
};

#define   reg_audio_ptr_en			    REG_ADDR8(REG_AUDIO_APB_BASE+0x11)
enum
{
	FLD_AUDIO_TX_WPTR_PTR_EN                 =  BIT(0),
	FLD_AUDIO_TX_RPTR_PTR_EN                 =  BIT(1),
	FLD_AUDIO_RX_WPTR_PTR_EN                 =  BIT(2),
	FLD_AUDIO_RX_RPTR_PTR_EN                 =  BIT(3),

};

enum
{
	FLD_AUDIO_FIFO_AOUT0_TRIG_NUM        =  BIT_RNG(0,3),
	FLD_AUDIO_FIFO_AIN0_TRIG_NUM         =  BIT_RNG(4,7),
};

#define   fifo_trig1				    REG_ADDR8(REG_AUDIO_APB_BASE+0x0b)

enum
{
	FLD_AUDIO_FIFO_AOUT1_TRIG_NUM        =  BIT_RNG(0,3),
	FLD_AUDIO_FIFO_AIN1_TRIG_NUM         =  BIT_RNG(4,7),
};

#define   reg_tx_wptr			    REG_ADDR16(REG_AUDIO_APB_BASE+0x20)
#define   reg_tx_rptr			    REG_ADDR16(REG_AUDIO_APB_BASE+0x22)

#define   reg_tx_max			    REG_ADDR16(REG_AUDIO_APB_BASE+0x26)

#define   reg_rx_rptr			    REG_ADDR16(REG_AUDIO_APB_BASE+0x2a)
#define   reg_rx_wptr			    REG_ADDR16(REG_AUDIO_APB_BASE+0x28)

#define   reg_rx_max			    REG_ADDR16(REG_AUDIO_APB_BASE+0x2e)

#define   reg_th0_h1			    REG_ADDR16(REG_AUDIO_APB_BASE+0x30)//tx
#define   reg_th0_l1			    REG_ADDR16(REG_AUDIO_APB_BASE+0x32)//tx

#define   reg_th0_h2			    REG_ADDR16(REG_AUDIO_APB_BASE+0x38)//tx
#define   reg_th0_l2			    REG_ADDR16(REG_AUDIO_APB_BASE+0x3a)//tx


#define   reg_th1_h1			    REG_ADDR16(REG_AUDIO_APB_BASE+0x40)//rx
#define   reg_th1_l1			    REG_ADDR16(REG_AUDIO_APB_BASE+0x42)//rx

#define   reg_th1_h2			    REG_ADDR16(REG_AUDIO_APB_BASE+0x48)//rx
#define   reg_th1_l2			    REG_ADDR16(REG_AUDIO_APB_BASE+0x4a)//rx



#define   reg_irq_fifo_state        REG_ADDR8(REG_AUDIO_APB_BASE+0x5c)
typedef enum
{
	FLD_AUDIO_IRQ_TXFIFO_L_L1           =  BIT(0),
	FLD_AUDIO_IRQ_TXFIFO_H_L1           =  BIT(1),
	FLD_AUDIO_IRQ_TXFIFO_L_L2           =  BIT(2),
	FLD_AUDIO_IRQ_TXFIFO_H_L2           =  BIT(3),

	FLD_AUDIO_IRQ_RXFIFO_L_L1           =  BIT(4),
	FLD_AUDIO_IRQ_RXFIFO_H_L1           =  BIT(5),
	FLD_AUDIO_IRQ_RXFIFO_L_L2           =  BIT(6),
	FLD_AUDIO_IRQ_RXFIFO_H_L2           =  BIT(7),

}audio_fifo_irq_status_type_e;

#define   reg_irq_fifo_mask        REG_ADDR8(REG_AUDIO_APB_BASE+0x5d)

typedef enum
{
	FLD_AUDIO_IRQ_TXFIFO_L_L1_EN           =  BIT(0),
	FLD_AUDIO_IRQ_TXFIFO_H_L1_EN           =  BIT(1),
	FLD_AUDIO_IRQ_TXFIFO_L_L2_EN           =  BIT(2),
	FLD_AUDIO_IRQ_TXFIFO_H_L2_EN           =  BIT(3),

	FLD_AUDIO_IRQ_RXFIFO_L_L1_EN           =  BIT(4),
	FLD_AUDIO_IRQ_RXFIFO_H_L1_EN           =  BIT(5),
	FLD_AUDIO_IRQ_RXFIFO_L_L2_EN           =  BIT(6),
	FLD_AUDIO_IRQ_RXFIFO_H_L2_EN           =  BIT(7),

}audio_fifo_irq_mask_type_e;



#define   reg_irq_manual_en		   REG_ADDR8(REG_AUDIO_APB_BASE+0x5e)
enum
{
	FLD_AUDIO_IRQ_TXFIFO_L_L1_MAN_EN           =  BIT(0),
	FLD_AUDIO_IRQ_TXFIFO_H_L1_MAN_EN           =  BIT(1),
	FLD_AUDIO_IRQ_TXFIFO_L_L2_MAN_EN           =  BIT(2),
	FLD_AUDIO_IRQ_TXFIFO_H_L2_MAN_EN           =  BIT(3),

	FLD_AUDIO_IRQ_RXFIFO_L_L1_MAN_EN           =  BIT(4),
	FLD_AUDIO_IRQ_RXFIFO_H_L1_MAN_EN           =  BIT(5),
	FLD_AUDIO_IRQ_RXFIFO_L_L2_MAN_EN           =  BIT(6),
	FLD_AUDIO_IRQ_RXFIFO_H_L2_MAN_EN           =  BIT(7)

};

#define   reg_int_pcm_num           REG_ADDR16(REG_AUDIO_APB_BASE+0x50)
#define   reg_dec_pcm_num           REG_ADDR16(REG_AUDIO_APB_BASE+0x52)

#define   reg_pcm_clk_num			REG_ADDR8(REG_AUDIO_APB_BASE+0x54)



#define   reg_audio_codec_stat_ctr          REG_ADDR8(REG_CODEC_BASE_ADDR+(0x00<<2))
#define   addr_audio_codec_stat_ctr         0x00
enum
{
	FLD_AUDIO_CODEC_ADC12_LOCKED        =  BIT(3),
	FLD_AUDIO_CODEC_DAC_LOCKED        	=  BIT(4),
	FLD_AUDIO_CODEC_PON_ACK      	    =  BIT(7),
};


#define   reg_audio_codec_stat2_ctr          REG_ADDR8(REG_CODEC_BASE_ADDR+(0x01<<2))
#define   addr_audio_codec_stat2_ctr         0x01
enum
{
	FLD_AUDIO_CODEC_ADC12_SMUTE_IN_PROG =  BIT(6),
	FLD_AUDIO_CODEC_DAC_SMUTE_IN_PROG  	=  BIT(7),
};


#define   reg_audio_codec_vic_ctr          REG_ADDR8(REG_CODEC_BASE_ADDR+(0x06<<2))
#define   addr_audio_codec_vic_ctr          0x06
enum
{
	FLD_AUDIO_CODEC_SB           	    =  BIT(0),
	FLD_AUDIO_CODEC_SB_ANALOG        	=  BIT(1),
	FLD_AUDIO_CODEC_SLEEP_ANALOG        =  BIT(2),
};



#define   reg_audio_codec_dac_itf_ctr           REG_ADDR8(REG_CODEC_BASE_ADDR+(0x08<<2))
#define   addr_audio_codec_dac_itf_ctr           0x08
enum
{
	FLD_AUDIO_CODEC_DAC_AUDIOIF         =  BIT_RNG(0,1),
	FLD_AUDIO_CODEC_SB_AICR_DAC        	=  BIT(4),
	FLD_AUDIO_CODEC_DAC_SLAVE        	=  BIT(5),
	FLD_AUDIO_CODEC_DAC_ADWL            =  BIT_RNG(6,7),
};

#define   reg_audio_codec_adc_itf_ctr           REG_ADDR8(REG_CODEC_BASE_ADDR+(0x09<<2))
#define   addr_audio_codec_adc_itf_ctr           0x09
enum
{
	FLD_AUDIO_CODEC_FORMAT              =  BIT_RNG(0,1),
	FLD_AUDIO_CODEC_DAC_ITF_SB          =  BIT(4),
	FLD_AUDIO_CODEC_SLAVE        	    =  BIT(5),
	FLD_AUDIO_CODEC_WL           	    =  BIT_RNG(6,7),
};

#define   reg_audio_codec_adc2_ctr           REG_ADDR8(REG_CODEC_BASE_ADDR+(0x0a<<2))
#define   addr_audio_codec_adc2_ctr          0x0a
enum
{
	FLD_AUDIO_CODEC_ADC12_SB              =  BIT(0),
};

#define   reg_audio_codec_dac_freq_ctr           REG_ADDR8(REG_CODEC_BASE_ADDR+(0x0b<<2))
#define   addr_audio_codec_dac_freq_ctr           0x0b
enum
{
	FLD_AUDIO_CODEC_DAC_FREQ                   =   BIT_RNG(0,3),
};

#define   reg_audio_codec_adc_wnf_ctr           REG_ADDR8(REG_CODEC_BASE_ADDR+(0x0e<<2))
#define   addr_audio_codec_adc_wnf_ctr           0x0e
enum
{
	FLD_AUDIO_CODEC_ADC12_WNF                   =   BIT_RNG(0,1),
};



#define   reg_audio_codec_adc_freq_ctr           REG_ADDR8(REG_CODEC_BASE_ADDR+(0x0f<<2))
#define   addr_audio_codec_adc_freq_ctr          0x0f
enum
{
	FLD_AUDIO_CODEC_ADC_FREQ       	    =  BIT_RNG(0,3),
	FLD_AUDIO_CODEC_ADC12_HPF_EN        =  BIT(4),
};



#define   reg_audio_dmic_12           REG_ADDR8(REG_CODEC_BASE_ADDR+(0x10<<2))
#define   addr_audio_dmic_12          0x10
enum
{
	FLD_AUDIO_CODEC_ADC_DMIC_SEL2      =  BIT_RNG(0,1),
	FLD_AUDIO_CODEC_ADC_DMIC_SEL1      =  BIT_RNG(2,3),
	FLD_AUDIO_CODEC_DMIC2_SB           =  BIT(6),
	FLD_AUDIO_CODEC_DMIC1_SB           =  BIT(7),
};


#define   reg_audio_codec_hpl_gain          REG_ADDR8(REG_CODEC_BASE_ADDR+(0x15<<2))
#define   addr_audio_codec_hpl_gain          0x15

enum
{
	FLD_AUDIO_CODEC_HPL_GOL       =  BIT_RNG(0,4),
	FLD_AUDIO_CODEC_HPL_LRGO      =  BIT(7),
};

#define   reg_audio_codec_hpr_gain           REG_ADDR8(REG_CODEC_BASE_ADDR+(0x16<<2))
#define   addr_audio_codec_hpr_gain           0x16
enum
{
	FLD_AUDIO_CODEC_HPR_GOR        =  BIT_RNG(0,4),
};


#define   reg_audio_codec_mic1_ctr          REG_ADDR8(REG_CODEC_BASE_ADDR+(0x17<<2))
#define   addr_audio_codec_mic1_ctr          0x17

enum
{
	FLD_AUDIO_CODEC_MIC1_SEL          =  BIT(0),
	FLD_AUDIO_CODEC_MICBIAS1_SB       =  BIT(5),
	FLD_AUDIO_CODEC_MIC_DIFF1         =  BIT(6),
	FLD_AUDIO_CODEC_MICBIAS1_V        =  BIT(7),
};

#define   reg_audio_codec_mic2_ctr          REG_ADDR8(REG_CODEC_BASE_ADDR+(0x18<<2))
#define   addr_audio_codec_mic2_ctr          0x18
enum
{
	FLD_AUDIO_CODEC_MIC2_SEL          =  BIT(0),
	FLD_AUDIO_CODEC_MIC_DIFF2         =  BIT(6),
};



#define   reg_audio_codec_dacl_gain          REG_ADDR8(REG_CODEC_BASE_ADDR+(0x2a<<2))
#define   addr_audio_codec_dacl_gain         0x2a
enum
{
	FLD_AUDIO_CODEC_DAC_GODL        =  BIT_RNG(0,5),
	FLD_AUDIO_CODEC_DAC_LRGOD      =  BIT(7),
};

#define   reg_audio_codec_dacr_gain           REG_ADDR8(REG_CODEC_BASE_ADDR+(0x2b<<2))
#define   addr_audio_codec_dacr_gain           0x2b
enum
{
	FLD_AUDIO_CODEC_DAC_GODR        =  BIT_RNG(0,5),
};

#define   reg_audio_codec_mic_l_R_gain           REG_ADDR8(REG_CODEC_BASE_ADDR+(0x1f<<2))
#define   addr_audio_codec_mic_l_R_gain          0x1f
enum
{
	FLD_AUDIO_CODEC_AMIC_L_GAIN        =  BIT_RNG(0,2),
	FLD_AUDIO_CODEC_AMIC_R_GAIN        =  BIT_RNG(3,5),
};


#define   reg_audio_codec_dac_ctr           REG_ADDR8(REG_CODEC_BASE_ADDR+(0x23<<2))
#define   addr_audio_codec_dac_ctr           0x23
enum
{
	FLD_AUDIO_CODEC_DAC_SB               =  BIT(4),
	FLD_AUDIO_CODEC_DAC_LEFT_ONLY        =  BIT(5),
	FLD_AUDIO_CODEC_DAC_SOFT_MUTE        =  BIT(7),
};



#define   reg_audio_codec_adc12_ctr           REG_ADDR8(REG_CODEC_BASE_ADDR+(0x24<<2))
#define   addr_audio_codec_adc12_ctr           0x24
enum
{
	FLD_AUDIO_CODEC_ADC1_SB              =  BIT(4),
	FLD_AUDIO_CODEC_ADC2_SB              =  BIT(5),
	FLD_AUDIO_CODEC_ADC12_SOFT_MUTE      =  BIT(7),
};


#define   reg_audio_adc1_gain           REG_ADDR8(REG_CODEC_BASE_ADDR+(0x2c<<2))
#define   addr_audio_adc1_gain           0x2c
enum
{
	FLD_AUDIO_CODEC_ADC_GID1       =  BIT_RNG(0,5),
	FLD_AUDIO_CODEC_ADC_LRGID      =  BIT(7),
};

#define   reg_audio_adc2_gain           REG_ADDR8(REG_CODEC_BASE_ADDR+(0x2d<<2))
#define   addr_audio_adc2_gain           0x2d
enum
{
	FLD_AUDIO_CODEC_ADC_GID2    =  BIT_RNG(0,5),

};


#define   reg_audio_mix_dac_l           REG_ADDR8(REG_CODEC_BASE_ADDR+(0x34<<2))
#define   addr_reg_audio_mix_dac_l          0x34
enum
{
	FLD_AUDIO_CODEC_DAC_MIXER_GAIN_L        =  BIT_RNG(0,4),
	FLD_AUDIO_CODEC_DAC_MIXER_GAIN_SEL      =  BIT(7),
};

#define   reg_audio_mix_dac_r           REG_ADDR8(REG_CODEC_BASE_ADDR+(0x35<<2))
#define   addr_reg_audio_mix_dac_r          0x35
enum
{
	FLD_AUDIO_CODEC_DAC_MIXER_GAIN_R        =  BIT_RNG(0,4),
};


#define   reg_audio_mix_adc_l           REG_ADDR8(REG_CODEC_BASE_ADDR+(0x36<<2))
#define   addr_reg_audio_mix_adc_l          0x36
enum
{
	FLD_AUDIO_CODEC_ADC_MIXER_GAIN_L        =  BIT_RNG(0,4),
	FLD_AUDIO_CODEC_ADC_MIXER_GAIN_SEL      =  BIT(7),
};

#define   reg_audio_mix_adc_r           REG_ADDR8(REG_CODEC_BASE_ADDR+(0x37<<2))
#define   addr_reg_audio_adc_dac_r          0x37
enum
{
	FLD_AUDIO_CODEC_ADC_MIXER_GAIN_R        =  BIT_RNG(0,4),
};
//Indirect Register
//AGC control Register
#define reg_audio_dac_agc_cmd				REG_ADDR8(REG_CODEC_BASE_ADDR+(0x38<<2))
#define	addr_audio_dac_agc_cmd				0x38
enum
{
	FLD_AUDIO_CODEC_DAC_AGC_ADD     =  BIT_RNG(0,5),
	FLD_AUDIO_CODEC_DAC_AGC_LOAD    =  BIT(6),
	FLD_AUDIO_CODEC_DAC_AGC_EN      =  BIT(7),
};

#define reg_audio_dac_agc_data				REG_ADDR8(REG_CODEC_BASE_ADDR+(0x39<<2))
#define	addr_audio_dac_agc_data				0x39

#define	ind_reg_audio_dac_agc_0					0x00
enum
{
	FLD_AUDIO_CODEC_DAC_AGC_LTHRES    	=  BIT_RNG(0,4),
	FLD_AUDIO_CODEC_DAC_AGC_LR_DRC      =  BIT(7),
};

#define	ind_reg_audio_dac_agc_1					0x01
enum
{
	FLD_AUDIO_CODEC_DAC_AGC_LCOMP    	=  BIT_RNG(0,2),
};

#define	ind_reg_audio_dac_agc_2					0x02
enum
{
	FLD_AUDIO_CODEC_DAC_AGC_RTHRES    	=  BIT_RNG(0,4),
};
#define	ind_reg_audio_dac_agc_3					0x03
enum
{
	FLD_AUDIO_CODEC_DAC_AGC_RCOMP    	=  BIT_RNG(0,2),
};


#define reg_audio_adc_agc_cmd				REG_ADDR8(REG_CODEC_BASE_ADDR+(0x3a<<2))
#define	addr_audio_adc_agc_cmd				0x3a
enum
{
	FLD_AUDIO_CODEC_ADC_AGC_ADD    =  BIT_RNG(0,5),
	FLD_AUDIO_CODEC_ADC_AGC_LOAD      =  BIT(6),

};

#define reg_audio_adc_agc_data				REG_ADDR8(REG_CODEC_BASE_ADDR+(0x3b<<2))
#define	addr_audio_adc_agc_data				0x3b





#define	ind_reg_audio_agc_0					0x00
enum
{
	FLD_AUDIO_CODEC_ADC_AGC_TARGET    	=  BIT_RNG(2,5),
	FLD_AUDIO_CODEC_ADC_AGC_STEREO      =  BIT(6),
};

#define	ind_reg_audio_agc_1					0x01
enum
{
	FLD_AUDIO_CODEC_ADC_AGC_HOLD    	=  BIT_RNG(0,3),
	FLD_AUDIO_CODEC_ADC_AGC_NG_THR    	=  BIT_RNG(4,6),
	FLD_AUDIO_CODEC_ADC_AGC_NG_EN      	=  BIT(7),
};

#define	ind_reg_audio_agc_2					0x02
enum
{
	FLD_AUDIO_CODEC_ADC_AGC_DCY    	=  BIT_RNG(0,3),
	FLD_AUDIO_CODEC_ADC_AGC_ATK    	=  BIT_RNG(4,7),
};

#define	ind_reg_audio_agc_3					0x03
enum
{
	FLD_AUDIO_CODEC_ADC_AGC_MAX    	=  BIT_RNG(0,4),
};

#define	ind_reg_audio_agc_4					0x04
enum
{
	FLD_AUDIO_CODEC_ADC_AGC_MIN    	=  BIT_RNG(0,4),
};

#define	ind_reg_audio_agc_5					0x05
enum
{
	FLD_AUDIO_CODEC_ADC_AGC_EN      	=  BIT(1),
};

#endif
