/********************************************************************************************************
 * @file    adc.h
 *
 * @brief   This is the header file for B87
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

#include "bit.h"
#include "analog.h"
#include "register.h"
#include "gpio.h"

/*
//ADC IO PINS
GPIO_PinTypeDef ADC_GPIO_tab[] = {
		GPIO_PB0,GPIO_PB1,
		GPIO_PB2,GPIO_PB3,
		GPIO_PB4,GPIO_PB5,
		GPIO_PB6,GPIO_PB7,
		GPIO_PC4,GPIO_PC5
};
*/

/**
 *  Some notice for ADC
 *  1:Just has MISC channel
 *  2:Just has 0.9V/1.2V reference voltage.
 *  3:Resolution:8/10/12/14.
 *  4:input mode,just has differential
 */

extern unsigned char adc_vbat_divider;

/**
 *  ADC reference voltage
 */
typedef enum{
	ADC_VREF_0P9V = 0x01,
	ADC_VREF_1P2V = 0x02,
}ADC_RefVolTypeDef;

/**
 *  ADC Vbat divider
 */
typedef enum{
	ADC_VBAT_DIVIDER_OFF = 0,
	ADC_VBAT_DIVIDER_1F3=2,
	/**
	 * Delete the ADC_VBAT_DIVIDER_1F2 enum due to it can't make sense.
	 * changed by chaofan. confirmed by lingyu.20201029.
	 */
}ADC_VbatDivTypeDef;

/**
 *	ADC analog input negative channel
 */
typedef enum {
	NOINPUTN,
	B0N,
	B1N,
	B2N,
	B3N,
	B4N,
	B5N,
	B6N,
	B7N,
	C4N,
	C5N,
	PGA0N,
	PGA1N,
	TEMSENSORN,
	TEMSENSORN_EE,
	GND,
}ADC_InputNchTypeDef;

/**
 *	ADC analog input positive channel
 */
typedef enum {
	NOINPUTP,
	B0P,
	B1P,
	B2P,
	B3P,
	B4P,
	B5P,
	B6P,
	B7P,
	C4P,
	C5P,
	PGA0P,
	PGA1P,
	TEMSENSORP,
	TEMSENSORP_EE,
	VBAT,
}ADC_InputPchTypeDef;

/**
 *	ADC resolution
 */
typedef enum{
	RES8,
	RES10,
	RES12,
	RES14
}ADC_ResTypeDef;

/**
 *	ADC channel input mode
 */
typedef enum{
	DIFFERENTIAL_MODE = 1,  //differential mode
}ADC_InputModeTypeDef;

/**
 *  ADC Sampling cycles
 */
typedef enum{
	SAMPLING_CYCLES_3,
	SAMPLING_CYCLES_6,
	SAMPLING_CYCLES_9,
	SAMPLING_CYCLES_12,
	SAMPLING_CYCLES_15,
	SAMPLING_CYCLES_18,
	SAMPLING_CYCLES_21,
	SAMPLING_CYCLES_24,
	SAMPLING_CYCLES_27,
	SAMPLING_CYCLES_30,
	SAMPLING_CYCLES_33,
	SAMPLING_CYCLES_36,
	SAMPLING_CYCLES_39,
	SAMPLING_CYCLES_42,
	SAMPLING_CYCLES_45,
	SAMPLING_CYCLES_48,
}ADC_SampCycTypeDef;

/**
 * ADC input channel: MISC
 */
typedef enum{
	ADC_MISC_CHN	= BIT(2),
}ADC_ChTypeDef;

/**
 *	ADC Prescaler Just for 1 or 1/8
 */
typedef enum{
	ADC_PRESCALER_1   = 0x00,
	ADC_PRESCALER_1F8 = 0x03,
}ADC_PreScalingTypeDef;


/**
 *	ADC mode: Normal mode mode
 */
typedef enum{
	ADC_NORMAL_MODE      = 0,
}ADC_ModeTypeDef;



/**
 * @brief      This function reset adc module
 * @param[in]  none.
 * @return     none.
 */
static inline void adc_reset_adc_module(void)
{
	reg_rst1 = FLD_RST1_ADC;
	reg_rst1 = 0;
}

/**
 * @brief      This function enable adc source clock: external 24M
 * @param[in]  en - variable of source clock state 1: enable;  0: disable.
 * @return     none.
 */
static inline void adc_enable_clk_24m_to_sar_adc(unsigned int en)
{
	if(en)
	{
		analog_write(areg_clk_setting, analog_read(areg_clk_setting) | FLD_CLK_24M_TO_SAR_EN);
	}
	else
	{
		analog_write(areg_clk_setting, analog_read(areg_clk_setting) & ~FLD_CLK_24M_TO_SAR_EN);
	}
}

/**************************************************************************************
afe_0xF4
    BIT<2:0>  adc_clk_div
      	  	  ADC clock(derive from external 24M crystal)
      	  	  ADC clk frequency = 24M/(adc_clk_div + 1)
    BIT<7:3>  reserved
 *************************************************************************************/
#define areg_adc_sampling_clk_div	   0xf4
enum{
	FLD_ADC_SAMPLE_CLK_DIV  = 	       BIT_RNG(0,2),
};

/**
 * @brief      This function sets adc sample clk. adc sample clk = 24M/(1+div)  div: 0~7.
 * @param[in]  div - the divider of adc sample clock.
 * @return     none
 */
static inline void adc_set_sample_clk(unsigned char div)
{
	//afe_0xF4<7:3> is reserved, so no need to care its value
	analog_write(areg_adc_sampling_clk_div, div & 0x07 );
}

/**************************************************************************************
afe_0xEA
    BIT<1:0>  adc_vrefm, Select VREF for Misc channel
			0x1: 0.9V
 	 	 	0x2: 1.2V
 	 	 	0x3: VBAT/N (N=2/3/4, when afe_0xF9<3:2> is set as 0x3/0x2/0x1).

	notice that: when choose ADC_VREF_VBAT_N, must use adc_set_vref_vbat_divider to config N(1/2/3/4)
 *************************************************************************************/
#define areg_adc_vref				   0xea
enum{
	FLD_ADC_VREF_CHN_M = 		       BIT_RNG(0,1),
};

/**
 * @brief      This function sets ADC reference voltage for Misc channel
 * @param[in]  v_ref_M - enum variable of adc reference voltage for Misc channel.
 * @return     none
 */
static inline void adc_set_vref(ADC_RefVolTypeDef vRef_M)
{
	analog_write(areg_adc_vref, vRef_M);
}

/**
 * @brief This function serves to set the channel reference voltage.
 * @param[in]  ch_n - enum variable of ADC input channel.
 * @param[in]  v_ref - enum variable of ADC reference voltage.
 * @return none
 */
void adc_set_ref_voltage(ADC_RefVolTypeDef v_ref);


#define areg_adc_vref_vbat_div			0xf9
enum{
	FLD_ADC_VREF_VBAT_DIV   = 	        BIT_RNG(2,3),
};
/**
 * @brief      This function select Vbat voltage divider
 * @param[in]  vbat_div - enum variable of Vbat voltage divider.
 * @return     none
 */
static inline void adc_set_vref_vbat_divider(ADC_VbatDivTypeDef vbat_div)
{
	analog_write(areg_adc_vref_vbat_div, (analog_read(areg_adc_vref_vbat_div) & (~FLD_ADC_VREF_VBAT_DIV)) | (vbat_div << 2));
	if(vbat_div){
		adc_vbat_divider = 5 - vbat_div;
	}else{
		adc_vbat_divider = 1;
	}
}



/**************************************************************************************
afe_0xEB
    BIT<3:0>  Select negative input for MISC channel
    BIT<7:4>  Select positive input for MISC channel

	negative input:
		0x0: No input
		0x1: B<0>
		0x2: B<1>
		...
		0x8: B<7>
		0x9: C<4>
		0xa: C<5>
		0xb: rsvd
		0xc: rsvd
		0xd: old tempsensor_n (Temperature sensor negative output)
		0xe: new tempsensor_n (Temperature sensor negative output)
		0xf: Ground

	positive input:
		0x0: No input
		0x1: B<0>
		0x2: B<1>
		...
		0x8: B<7>
		0x9: C<4>
		0xa: C<5>
		0xb: rsvd
		0xc: rsvd
		0xd: old tempsensor_p (Temperature sensor negative output)
		0xe: new tempsensor_p (Temperature sensor negative output)
		0xf: Vbat (Battery voltage)
 *************************************************************************************/
#define areg_adc_ain_chn_misc			0xeb
enum{
	FLD_ADC_AIN_NEGATIVE = 		        BIT_RNG(0,3),
	FLD_ADC_AIN_POSITIVE = 		        BIT_RNG(4,7),
};

/**
 * @brief This function serves to set input channel for the MISC channel.
 * @param[in]   p_ain - enum variable of ADC analog positive input channel.
 * @param[in]   n_ain - enum variable of ADC analog negative input channel.
 * @return none
 */
static inline void adc_set_ain_chn_misc(ADC_InputPchTypeDef p_ain, ADC_InputNchTypeDef n_ain)
{
	analog_write(areg_adc_ain_chn_misc, n_ain | p_ain << 4);
}

/**
 * @brief      This function sets ADC analog negative input channel for the MISC channel
 * @param[in]  v_ain - enum variable of ADC analog negative input.
 * @return     none
 */
static inline void adc_set_ain_negative_chn_misc(ADC_InputNchTypeDef v_ain)
{
	analog_write(areg_adc_ain_chn_misc, (analog_read(areg_adc_ain_chn_misc) & (~FLD_ADC_AIN_NEGATIVE)) | (v_ain));
}
/**
 * @brief      This function sets ADC analog positive input channel for the MISC channel
 * @param[in]  v_ain - enum variable of ADC analog positive input.
 * @return     none
 */
static inline void adc_set_ain_positive_chn_misc(ADC_InputPchTypeDef v_ain)
{
	analog_write(areg_adc_ain_chn_misc, (analog_read(areg_adc_ain_chn_misc) & (~FLD_ADC_AIN_POSITIVE)) | (v_ain << 4));
}

/**
 * @brief This function serves to set resolution.
 * @param[in]  ch_n - enum variable of ADC input channel.
 * @param[in]  v_res - enum variable of ADC resolution.
 * @return none
 */
void adc_set_resolution(ADC_ResTypeDef v_res);



#define anareg_adc_res_m					0xec
enum{
	FLD_ADC_RES_M         =             BIT_RNG(0,1),
	FLD_ADC_EN_DIFF_CHN_M =          	BIT(6),
};
/**
 * @brief      This function sets ADC resolution for the MISC channel
 * @param[in]  v_res - enum variable of ADC resolution.
 * @return     none
 */
static inline void adc_set_resolution_chn_misc(ADC_ResTypeDef v_res)
{
	analog_write(anareg_adc_res_m, (analog_read(anareg_adc_res_m) & (~FLD_ADC_RES_M)) | (v_res));
}

/**
 * @brief      This function sets ADC input mode for the MISC channel
 * @param[in]  m_input - enum variable of ADC channel input mode.
 * @return     none
 */
static inline void adc_set_input_mode_chn_misc(ADC_InputModeTypeDef m_input)
{
	 //differential mode
	analog_write(anareg_adc_res_m, analog_read(anareg_adc_res_m) | FLD_ADC_EN_DIFF_CHN_M);
}

/**
 * @brief This function serves to set input_mode.
 * @param[in]  ch_n - enum variable of ADC input channel.
 * @param[in]  m_input - enum variable of ADC channel input mode.
 * @return none
 */
void adc_set_input_mode(ADC_InputModeTypeDef m_input);


#define areg_adc_tsmaple_m				0xee
enum{
	FLD_ADC_TSAMPLE_CYCLE_CHN_M = 		BIT_RNG(0,3),
};

/**
 * @brief      This function sets ADC sample time(the number of adc clocks for sample cycles) for the MISC channel.
 * @param[in]  adcST - enum variable of adc sample cycles.
 * @return     none
 */
static inline void adc_set_tsample_cycle_chn_misc(ADC_SampCycTypeDef adcST)
{
	//ana_ee<7:4> is reserved, so no need care its value
	analog_write(areg_adc_tsmaple_m, adcST);  //optimize, <7:4> not cared
}

/**
 * @brief This function serves to set sample_cycle.
 * @param[in]  ch_n - enum variable of ADC input channel.
 * @param[in]  adcST - enum variable of ADC Sampling cycles.
 * @return none
 */
void adc_set_tsample_cycle(ADC_SampCycTypeDef adcST);

/**************************************************************************************
afe_0xEF<7:0>  	r_max_mc[7:0]
afe_0xF0<7:0>	rsvd
afe_0xF1
		<3:0>   r_max_s
		<5:4>   rsvd
		<7:6>   r_max_mc[9:8]

	r_max_mc[9:0]: serves to set length of capture state for Misc channel.
	r_max_s:       serves to set length of set state for Misc channel.

	Note: State length indicates number of 24M clock cycles occupied by the state.
 *************************************************************************************/
#define areg_r_max_mc	        0xef
enum{
	FLD_R_MAX_MC0	= 			BIT_RNG(0,7),//0xef<7:0>  r_max_mc[7:0]
};

#define	areg_r_max_s			0xf1
enum{
	FLD_R_MAX_S		=			BIT_RNG(0,3),//0xf1<3:0>  r_max_s
	FLD_R_MAX_MC1	= 			BIT_RNG(6,7),//0xf1<7:6>  r_max_mc[9:8]
};

/**
 * @brief      This function sets length of each set state
 * @param[in]  r_max_s - variable of length of "set" state
 * @return     none
 */
static inline void adc_set_length_set_state(unsigned char r_max_s)
{
	analog_write(areg_r_max_s, (analog_read(areg_r_max_s) & (~FLD_R_MAX_S)) | (r_max_s));
}

/**
 * @brief      This function sets length of each "capture" state for MISC channel.
 * @param[in]  r_max_mc[9:0] - variable of length of "capture" state for MISC channel.
 * @return     none
 * note :  0xef<7:0>  r_max_mc[7:0]
 *         0xf1<7:6>  r_max_mc[9:8]
 */
static inline void adc_set_length_capture_state_for_chn_misc_rns (unsigned short r_max_mc)
{
	analog_write(areg_r_max_mc, (r_max_mc & 0x0ff));
	analog_write(areg_r_max_s, ((analog_read(areg_r_max_s) & (~FLD_R_MAX_MC1)) | ((r_max_mc & 0x3ff) >> 8) << 6));
}

/**
 * @brief This function serves to set state and capture_state length.
 * @param[in]   r_max_mc - Value of length of "capture" state for MISC channel.
 * @param[in]   r_max_s - Value of length of "set" state for MISC channel.
 * @return none
 */
static inline void adc_set_state_length(unsigned short R_max_mc,unsigned char R_max_s)
{
	analog_write(areg_r_max_mc, R_max_mc);
	analog_write(areg_r_max_s, ((R_max_mc >> 8) << 6)| (R_max_s & FLD_R_MAX_S));
}

/***************************************************************************************
afe_0xF2
    BIT<0>   rsvd
    BIT<1>   rsvd
	BIT<2>   r_en_Misc    Enable Misc  channel. 1: enable;  0: disable
	BIT<3>   writetocore_enable/disable 0:enable;  1:disable
	BIT<5:4> r_max_scnt   Set total length for sampling state machine (i.e. max state index)
	BIT<6>   rsvd
    BIT<7>   rsvd
 *************************************************************************************/
#define areg_adc_chn_en			0xf2
enum{
	FLD_ADC_CHN_EN_M	= 		BIT(2),
	FLD_ADC_CHN_EN_RNS	= 		BIT(3),
	FLD_ADC_MAX_SCNT	=		BIT_RNG(4,5),
};
/**
 * @brief      This function sets ADC input channel.
 * @param[in]  ad_ch - enum variable of ADC input channel.
 * @return     none
 */
static inline void adc_set_chn_enable(ADC_ChTypeDef ad_ch)
{
	analog_write(areg_adc_chn_en, (analog_read(areg_adc_chn_en) & 0xf0) | ad_ch);
}

/**
 * @brief      This function sets total state index for the channel.
 * @param[in]  ad_ch - enum variable of ADC input channel.
 * @param[in]  s_cnt - sum of the channel state index start with 0x0.
 * @return     none
 */
static inline void adc_set_chn_enable_and_max_state_cnt(ADC_ChTypeDef ad_ch, unsigned char s_cnt)
{
	analog_write(areg_adc_chn_en, ad_ch | ((s_cnt & 0x07) << 4));
}

/**
 * @brief      This function sets total state index for sampling state.
 * @param[in]  s_cnt - sum of state index start with 0x0.
 * @return     none
 */
static inline void adc_set_max_state_cnt(unsigned char s_cnt)
{
	analog_write(areg_adc_chn_en, (analog_read(areg_adc_chn_en) & (~FLD_ADC_MAX_SCNT)) | ((s_cnt & 0x07) << 4));
}

/**************************************************************************************
afe_0xFA
		BIT<1:0>  adc_itrim_preamp, Comparator preamp bias current trimming
		BIT<3:2>  adc_itrim_vrefbuf, Vref buffer bias current trimming
		BIT<5:4>  adc_itrim_vcmbuf, Vref buffer bias current trimming

		BIT<7:6>  Analog input pre-scaling select
					sel_ai_scale[1:0]: scaling factor
							0x0: 1
							0x3: 1/8

 *************************************************************************************/
#define    areg_ain_scale        		0xfa
enum{
	FLD_ADC_ITRIM_PREAMP 	= 	BIT_RNG(0,1),
	FLD_ADC_ITRIM_VREFBUF	= 	BIT_RNG(2,3),
	FLD_ADC_ITRIM_VCMBUF	= 	BIT_RNG(4,5),
	FLD_SEL_AIN_SCALE 		= 	BIT_RNG(6,7),
};


/**************************************************************************************
afe_0xFC
	BIT<5>	  adc_pd, Power down ADC
						1: Power down
						0: Power up
 *************************************************************************************/
#define areg_adc_pga_ctrl		0xfc
enum{
	FLD_SAR_ADC_POWER_DOWN 		= BIT(5),
};

/**
 * @brief      This function sets sar_adc power.
 * @param[in]  on - 1 : power on; off-0 : power off.
 * @return     none
 * caution power on cause  audio  creepage
 */
static inline void adc_power_on_sar_adc(unsigned char on_off)
{
	analog_write(areg_adc_pga_ctrl, (analog_read(areg_adc_pga_ctrl) & (~FLD_SAR_ADC_POWER_DOWN)) | (!on_off) << 5);
}

/*************************************************************************************
afe_0xF7<7:0>   adc_dat[7:0]  	Read only, Misc adc dat[7:0]
afe_0xF8<7:0>   adc_dat[15:8]  	Read only
 *************************************************************************************/
#define    areg_adc_misc_l      0xf7
#define    areg_adc_misc_h      0xf8

/**
 * @brief This function serves to set input channel in differential_mode.
 * @param[in]  ch_n - enum variable of ADC input channel.
 * @param[in]  InPCH - enum variable of ADC analog positive input channel.
 * @param[in]  InNCH - enum variable of ADC analog negative input channel.
 * @return none
 */
void adc_set_ain_channel_differential_mode(ADC_InputPchTypeDef InPCH, ADC_InputNchTypeDef InNCH);

/**
 * @brief This function serves to set pre_scaling.
 * @param[in]  v_scl - enum variable of ADC pre_scaling factor.
 * @return none
 */
void adc_set_ain_pre_scaler(ADC_PreScalingTypeDef v_scl);


/**
 * @brief This function serves to ADC init.
 * @param[in]   none
 * @return none
 */
void adc_init(void);

/**
 * @brief This function is used to calib ADC 1.2V vref for GPIO.
 * @param[in] data - GPIO sampling calibration value.
 * @return none
 */
void adc_set_gpio_calib_vref(unsigned short data);
/**
 * @brief This function is used to calib ADC 1.2V vref offset for GPIO two-point.
 * @param[in] offset - GPIO sampling two-point calibration value offset.
 * @return none
 */
void adc_set_gpio_two_point_calib_offset(signed char offset);
/**
 * @brief This function is used to calib ADC 1.2V vref for VBAT.
 * @param[in] data - VBAT sampling calibration value
 * @return none
 */
void adc_set_vbat_calib_vref(unsigned short data);

/**
 * @brief This function is used for IO port configuration of ADC IO port voltage sampling.
 * @param[in]  pin - GPIO_PinTypeDef
 * @return none
 */
void adc_base_pin_init(GPIO_PinTypeDef pin);

/**
 * @brief This function is used for IO port configuration of ADC supply voltage sampling.
 * @param[in]  pin - GPIO_PinTypeDef
 * @return none
 */
void adc_vbat_pin_init(GPIO_PinTypeDef pin);

/**
 * @brief This function is used for ADC configuration of ADC IO voltage sampling.
 * @param[in]   pin - GPIO_PinTypeDef
 * @return none
 */
void adc_base_init(GPIO_PinTypeDef pin);

/**
 * @brief This function servers to test ADC temp.
 * @param[in]  none.
 * @return     none.
 */
void adc_temp_init(void);

/**
 * @brief This function is used for ADC configuration of ADC supply voltage sampling.
 * @param[in]    pin - GPIO_PinTypeDef
 * @return none
 */
void adc_vbat_init(GPIO_PinTypeDef pin);


/**
 * @brief This function is used for ADC configuration of ADC supply voltage sampling.
 * @param[in]    pin - GPIO_PinTypeDef
 * @return none
 */
void adc_vbat_channel_init(void);

/**
 * @brief This function serves to set adc sampling and get results.
 * @param[in]  none.
 * @return the result of sampling.
 */
unsigned int adc_sample_and_get_result(void);


#define adc_data_sample_control		0xf3
enum{
	NOT_SAMPLE_ADC_DATA 		= BIT(0),
};

/**
 * @brief      This function serves to set adc sampling and get results in manual mode for Base and Vbat mode.
 *             In base mode just PB2 PB3 PB4 PC4 can get the right value!
 * @param[in]  none.
 * @return the result of sampling.
 */
unsigned short adc_sample_and_get_result_manual_mode(void);

/**
 * @brief This function serves to get adc sampling temperature results.
 * @param[in]  none.
 * @return the result of temperature.
 */
signed short adc_temp_result(void);

