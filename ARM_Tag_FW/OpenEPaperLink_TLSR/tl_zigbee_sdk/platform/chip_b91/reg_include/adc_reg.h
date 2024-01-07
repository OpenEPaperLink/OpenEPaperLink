/********************************************************************************************************
 * @file	adc_reg.h
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
#ifndef ADC_REG_H
#define ADC_REG_H
#include "soc.h"

#define areg_adc_clk_setting 0x82
enum
{
	FLD_CLK_24M_TO_SAR_EN = BIT(6),
};

#define areg_adc_vref					0xea
enum{
	FLD_ADC_VREF_CHN_M = 		       BIT_RNG(0,1),
};
#define areg_adc_ain_chn_misc			0xeb
enum{
	FLD_ADC_AIN_NEGATIVE = 		        BIT_RNG(0,3),
	FLD_ADC_AIN_POSITIVE = 		        BIT_RNG(4,7),
};
#define areg_adc_res_m					0xec
enum{
	FLD_ADC_RES_M         =             BIT_RNG(0,1),
	FLD_ADC_EN_DIFF_CHN_M =          	BIT(6),
};
#define areg_adc_tsmaple_m				0xee
enum{
	FLD_ADC_TSAMPLE_CYCLE_CHN_M = 		BIT_RNG(0,3),
};
#define areg_r_max_mc					0xef
enum{
	FLD_R_MAX_MC0	= 			BIT_RNG(0,7),//0xef<7:0>  r_max_mc[7:0]
};
#define	areg_r_max_s					0xf1
enum{
	FLD_R_MAX_S		=			BIT_RNG(0,3),//0xf1<3:0>  r_max_s
	FLD_R_MAX_MC1	= 			BIT_RNG(6,7),//0xf1<7:6>  r_max_mc[9:8]
};
#define areg_adc_chn_en					0xf2
enum{
	FLD_ADC_CHN_EN_M	= 		BIT(2),
	FLD_ADC_MAX_SCNT	=		BIT_RNG(4,5),
};
#define areg_adc_data_sample_control	0xf3
enum{
	FLD_NOT_SAMPLE_ADC_DATA 		= BIT(0),
};
#define areg_adc_sample_clk_div		0xf4
enum{
	FLD_ADC_SAMPLE_CLK_DIV  = 	       BIT_RNG(0,2),
};
#define areg_adc_misc_l					0xf7
#define areg_adc_misc_h					0xf8
#define areg_adc_vref_vbat_div			0xf9
enum{
	FLD_ADC_VREF_VBAT_DIV   = 	        BIT_RNG(2,3),
};
#define areg_ain_scale					0xfa
enum{
	FLD_ADC_ITRIM_PREAMP 	= 	BIT_RNG(0,1),
	FLD_ADC_ITRIM_VREFBUF	= 	BIT_RNG(2,3),
	FLD_ADC_ITRIM_VCMBUF	= 	BIT_RNG(4,5),
	FLD_SEL_AIN_SCALE 		= 	BIT_RNG(6,7),
};
#define areg_adc_pga_ctrl				0xfc
enum{
	FLD_SAR_ADC_POWER_DOWN 		= BIT(5),
};
#define areg_temp_sensor_ctrl 0x00
enum{
	FLD_TEMP_SENSOR_POWER_DOWN = BIT(4),
};
#endif
