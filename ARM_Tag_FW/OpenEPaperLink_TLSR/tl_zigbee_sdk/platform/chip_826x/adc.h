/********************************************************************************************************
 * @file    adc.h
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

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#include "bit.h"
#include "bsp.h"


#define INTER_RESIS_ONE_THIRD_EN  1

typedef enum {
	Battery_Chn_VCC,
	Battery_Chn_B7,
}BATT_ChnTypeDef;

/**
 *  ADC reference voltage
 */
typedef enum {
	RV_1P428,
	RV_AVDD,
	RV_1P224,
}ADC_RefVolTypeDef;

/**
 *ADC resolution
 */
typedef enum {
	RES7,
	RES9,
	RES10,
	RES11,
	RES12,
	RES13,
	RES14,
}ADC_ResTypeDef;

/**
 *ADC Sampling time
 */
typedef enum {
	S_3,
	S_6,
	S_9,
	S_12,
	S_18,
	S_24,
	S_48,
	S_144,
}ADC_SampCycTypeDef;

/**
 *ADC analog input channel selection enum
 */
typedef enum {
	NOINPUT,
	C0,
	C1,
	C6,
	C7,
	B0,
	B1,
	B2,
	B3,
	B4,
	B5,
	B6,
	B7,
	PGAVOM,
	PGAVOP,
	TEMSENSORN,
	TEMSENSORP,
	AVSS,
	OTVDD,//1/3 voltage division detection
}ADC_InputPTypeDef;
/**
 *ADC channel input mode
 */
typedef enum {
	SINGLEEND,
	INVERTB_1,
	INVERTB_3,
	PGAVOPM,
}ADC_InputMTypeDef;

/**
 *
 */
 #define    REG_ADC_CLK_STEP        READ_REG8(0x69)
 enum{
 	ADC_STEP_L    = BIT_RNG(0,7),
 	ADC_CLK_STEP  = BIT_RNG(0,7),
 };
 #define    REG_ADC_CLK_MODE        READ_REG8(0x6a)
 enum{
 	ADC_MODE_L    = BIT_RNG(0,7),
 	ADC_CLK_MODE  = BIT_RNG(0,7),
 };
/**
 *
 */
#define    ADC_STEP_MODE            READ_REG8(0x6b)
enum{
	ADC_CLK_EN = BIT(7),
	ADC_STEP_H = BIT_RNG(4,6),
	ADC_MODE_H = BIT_RNG(0,3),
};
#define    EN_ADCCLK                ADC_STEP_MODE |= (ADC_CLK_EN)
#define    DIS_ADCCLK               ADC_STEP_MODE &= (~ADC_CLK_EN)

#define    SET_ADC_STEP(v)  do{\
								REG_ADC_CLK_STEP = (unsigned char)(v);\
								ADC_STEP_MODE &= (~ADC_STEP_H);\
								ADC_STEP_MODE |= ((((unsigned char)((v)>>8))&0x07)<<4);\
							}while(0)

#define    SET_ADC_MODE(v)  do{\
								REG_ADC_CLK_MODE = (unsigned char)(v);\
								ADC_STEP_MODE &= (~ADC_MODE_H);\
								ADC_STEP_MODE |= (((unsigned char)((v)>>8))&0x0F);\
							}while(0)

 /**
 *
 */
#define    ADC_REF_VOL              READ_REG8(0x2b)
enum{
	ADC_MISC_REF_V  = BIT_RNG(0,1),
	ADC_LEFT_REF_V  = BIT_RNG(2,3),
};
#define    SET_MISC_REF_VOL(v)	do{\
									ADC_REF_VOL &= (~ADC_MISC_REF_V);\
									ADC_REF_VOL |= (unsigned char)((v)&0x03);\
								}while(0)

#define    SET_LEFT_REF_VOL(v)  do{\
									ADC_REF_VOL &= (~ADC_LEFT_REF_V);\
									ADC_REF_VOL |= (((unsigned char)((v)&0x03))<<2);\
								}while(0)
 /**
 *
 */
 #define    REG_ADC_MISC_INPUT      READ_REG8(0x2c)
 enum{
 	MISC_INPUT_P   = BIT_RNG(0,4),
 	MISC_INPUT_M   = BIT_RNG(5,6),
	MISC_DATA_FORMAT= BIT(7),
 };

 /**
  *
  */
#define    REG_ADC_LEFT_INPUT       READ_REG8(0x2d)
enum{
	ADC_LEFT_INPUT_P = BIT_RNG(0,4),
	ADC_LEFT_INPUT_M = BIT_RNG(5,6),
	ADC_LEFT_DATA_FORMAT = BIT(7),
};
#define    SET_LEFT_DATA_FORMAT(v)  do{\
									   REG_ADC_LEFT_INPUT &= (~ADC_LEFT_DATA_FORMAT);\
									   REG_ADC_LEFT_INPUT |= ((v)<<7);\
								    }while(0)

#define    SET_LEFT_INPUT_P(v)      do{\
										REG_ADC_LEFT_INPUT &= (~ADC_LEFT_INPUT_P);\
										REG_ADC_LEFT_INPUT |= ((v)&0x1F);\
									}while(0)

#define    SET_LEFT_INPUT_M(v)      do{\
										REG_ADC_LEFT_INPUT &= (~ADC_LEFT_INPUT_M);\
										REG_ADC_LEFT_INPUT |= (((v)&0x03)<<5);\
								    }while(0)
 /**
  *
  */
#define     ADC_RESOLUTION_LEFT        READ_REG8(0x2f)
enum{
	ADC_MISC_RESOLUTION = BIT_RNG(0,2),
};
#define     SET_LEFT_RES(v)             do{\
											ADC_RESOLUTION_LEFT &= (~ADC_MISC_RESOLUTION);\
											ADC_RESOLUTION_LEFT |= ((v)&0x07);\
									    }while(0)
 /**
  *
  */
 #define    REG_ADC_MISC_PERIOD        READ_REG16(0x30)
 enum{
 	MISC_PERIOD_LOW   = BIT_RNG(0,7),
 	MISC_PERIOD_HIG   = BIT_RNG(8,15),
 };

#define    SET_PFM(v)			       REG_ADC_MISC_PERIOD = ((v<<2)&0x0FFF)      //WRITE_REG16(0x30,(v<<2)&0x0FFF)

/**
 *  set period for L
 */
#define    REG_ADC_LEFT_PERIOD         READ_REG16(0x32)
#define    SET_PFL(v)			       REG_ADC_LEFT_PERIOD = v        //WRITE_REG8(0x32,v)
 /**
  *
  */
#define    ADC_CTRL_REG                READ_REG8(0x33)
enum{
	AUTO_MODE_LEFT_EN      = BIT(0),
	AUTO_MODE_RIGHT_EN     = BIT(1),
	AUDIO_OUTPUT_EN        = BIT(2),
	AUTO_MODE_MISC_EN      = BIT(3),
	AUDIO_MONO_STEREO_SEL  = BIT_RNG(4,5),
	ADC_DONE_EDGE_FLAG     = BIT_RNG(6,7),
};
enum{
	NO_AUDIO = 0,
	MONO_AUDIO = BIT(4),
	STEREO_AUDIO,
};
enum{
	ADC_DONE_RISING = BIT(6),
	ADC_DONE_FALLING = BIT(7),
};
/**
 *
 */
#define    AREG_BATT_DET_CTRL         0x02
 enum{
 	BATT_DET_INPUT  = BIT_RNG(4,5),
 };
/**
 *
 */
#define    REG_ADC_MISC_RES_SAMPLE       READ_REG8(0x3c)
 enum{
 	MISC_SAMPLE     = BIT_RNG(0,2),
 	MISC_RESOLUTION = BIT_RNG(3,5),

 };
/**
 *
 */
#define    ADC_SAMPLE_CYCLE_LEFT        READ_REG8(0x3d)
enum{
	ADC_LEFT_SMPLE_CYCLE = BIT_RNG(0,2),
};
#define     SET_SAMPLE_CYCLE_LEFT(v)    do{\
										    ADC_SAMPLE_CYCLE_LEFT &= (~ADC_LEFT_SMPLE_CYCLE);\
										    ADC_SAMPLE_CYCLE_LEFT |= ((v)&0x07);\
									    }while(0)

/**
 *  set period for Misc
 */
#define		SET_P(v)			write_reg16(0x30,(v<<2)&0x0FFF)

 /**
  *  Check adc status, busy return 1
  */
#define     REG_ADC_BUSY_FLAG    READ_REG8(0x3a)
enum{
	ADC_BUSY_FLAG = BIT(0),
};
#define		CHECKADCSTATUS		((REG_ADC_BUSY_FLAG & ADC_BUSY_FLAG)?1:0)



#define  	ADC2AUDIO()			do{\
									ADC_CTRL_REG &= 0x00;\
									ADC_CTRL_REG |= (AUTO_MODE_LEFT_EN|AUDIO_OUTPUT_EN|MONO_AUDIO|ADC_DONE_FALLING);\
								}while(0)

#define  	AUDIO2ADC()			do{\
									ADC_CTRL_REG &= 0x00;\
									ADC_CTRL_REG |= (ADC_DONE_FALLING|AUTO_MODE_MISC_EN);\
								}while(0)
/**
 * @brief      This function sets ADC resolution for channel Misc
 * @param[in]  adcRes - enum variable adc resolution.
 * @return     none
 */
extern void ADC_ResSet(ADC_ResTypeDef adcRes);

/**
 * @brief      This function sets ADC input channel
 * @param[in]  adcInCha - enum variable of adc input channel.
 * @return     none
 */
extern void ADC_AnaChSet( ADC_InputPTypeDef adcInCha);

/**
 * @brief      This function sets ADC input channel mode - signle-end or differential mode
 * @param[in]  inM - enum variable of ADCINPUTMODE.
 * @return     none
 */
extern void ADC_AnaModeSet( ADC_InputMTypeDef inM);

/**
 * @brief      This function sets ADC sample time(the number of adc clocks for each sample)
 * @param[in]  adcST - enum variable of adc sample time.
 * @return     none
 */
extern void ADC_SampleTimeSet(ADC_SampCycTypeDef adcST);

/**
 * @brief      This function sets ADC reference voltage for the Misc and L channel
 * @param[in]  adcRF - enum variable of adc reference voltage.
 * @return     none
 */
extern void ADC_RefVoltageSet(ADC_RefVolTypeDef adcRF);

/**
 * @brief	   ADC initiate function, set the ADC clock details (4MHz) and start the ADC clock.
 *			   ADC clock relys on PLL.
 * @param[in]  none
 * @return	   1: set success ;
 *             0: set error
 */
extern unsigned char ADC_Init(void);

/**
 * @brief     set input channel,set reference voltage, set resolution bits, set sample cycle
 * @param[in] chl          - enum variable ADCINPUTCH ,acd channel
 * @param[in] ref_vol      - enum variable ADCRFV
 * @param[in] resolution   - enum variable ADCRESOLUTION
 * @param[in] sample_cycle - enum variable ADCST
 * @return    none
 */
extern void ADC_ParamSetting(ADC_InputPTypeDef chn,ADC_InputMTypeDef mode,ADC_RefVolTypeDef ref_vol,ADC_ResTypeDef resolution,ADC_SampCycTypeDef sample_cycle);

/**
 * @brief       Initiate function for the battery check function
 * @param[in]   checkM - Battery check mode,
 *              0: for battery dircetly connected to chip,
 *              1: for battery connected to chip via boost DCDC
 * @return      none
 */
extern void ADC_BatteryCheckInit(BATT_ChnTypeDef checkM);

/**
 * @brief  get the battery value
 * @param  none
 * @return the sampling value
 */
extern unsigned short ADC_BatteryValueGet(void);

/**
 * @brief  Initiate function for the temparture sensor
 * @param  ADC_InputPTypeDef
 * @return none
 */
extern void ADC_TemSensorInit(ADC_InputPTypeDef chn);

/**
 * @brief  get adc sampled value
 * @param  none
 * @return sampled_value, raw data
 */
extern unsigned short ADC_SampleValueGet(void);



/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif

