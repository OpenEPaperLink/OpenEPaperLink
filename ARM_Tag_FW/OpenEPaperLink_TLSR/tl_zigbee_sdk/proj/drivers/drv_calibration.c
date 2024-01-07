/********************************************************************************************************
 * @file    drv_calibration.c
 *
 * @brief   This is the source file for drv_calibration
 *
 * @author  Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#include "../tl_common.h"

#if defined(MCU_CORE_8258) || defined(MCU_CORE_8278) || defined(MCU_CORE_B91)
static void drv_calib_adc_verf(void)
{
	u8 adc_vref_calib_value[7] = {0};
	u16 gpio_calib_vref = 0;
	s8 gpio_calib_vref_offset = 0;

	flash_read(CFG_ADC_CALIBRATION, 7, adc_vref_calib_value);

#if defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	//Check the two-point gpio calibration value whether is exist
	if((adc_vref_calib_value[4] != 0xff) &&
	   (adc_vref_calib_value[4] <= 0x7f) &&
	   (adc_vref_calib_value[5] != 0xff) &&
	   (adc_vref_calib_value[6] != 0xff)){
		/****** Method of calculating two-point gpio calibration Flash_gain and Flash_offset value: ********/
		/****** Vref = [(Seven_Byte << 8) + Six_Byte + 1000]mv ********/
		/****** offset = [Five_Byte - 20] mv. ********/
		gpio_calib_vref = (adc_vref_calib_value[6] << 8) + adc_vref_calib_value[5] + 1000;
		gpio_calib_vref_offset = adc_vref_calib_value[4] - 20;

		adc_set_gpio_calib_vref(gpio_calib_vref);
		adc_set_gpio_two_point_calib_offset(gpio_calib_vref_offset);
	}else{
		/****** If flash do not exist the two-point gpio calibration value,use the one-point gpio calibration value ********/
		/****** Method of calculating one-point gpio calibration Flash_gpio_Vref value: ********/
		/****** Vref = [1175 +First_Byte-255+Second_Byte] mV = [920 + First_Byte + Second_Byte] mV ********/
		gpio_calib_vref = 920 + adc_vref_calib_value[0] + adc_vref_calib_value[1];
		/****** Check the one-point calibration value whether is correct ********/
		if((gpio_calib_vref >= 1047) && (gpio_calib_vref <= 1302)){
			adc_set_gpio_calib_vref(gpio_calib_vref);
		}
	}
#if defined(MCU_CORE_8278)
	/****** Method of calculating calibration Flash_vbat_Vref value: ********/
	/****** Vref = [1175 +First_Byte-255+Second_Byte] mV = [920 + First_Byte + Second_Byte] mV ********/
	u16 vbat_calib_vref = 920 + adc_vref_calib_value[2] + adc_vref_calib_value[3];
	/****** If flash do not exist the vbat one-point calibration value or the value is incorrect,use the Efuse vbat calibration value ********/
	if((vbat_calib_vref >= 1047) && (vbat_calib_vref <= 1302)){
		adc_set_vbat_calib_vref(vbat_calib_vref);
	}
#endif
#elif defined(MCU_CORE_B91)
	//Check the two-point gpio calibration value whether is exist
	if((adc_vref_calib_value[4] != 0xff) &&
	   (adc_vref_calib_value[4] <= 0x7f) &&
	   (adc_vref_calib_value[5] != 0xff) &&
	   (adc_vref_calib_value[6] != 0xff)){
		/****** Method of calculating two-point gpio calibration Flash_gain and Flash_offset value: ********/
		/****** Vref = [(Seven_Byte << 8) + Six_Byte + 1000]mv ********/
		/****** offset = [Five_Byte - 20] mv ********/
		gpio_calib_vref = (adc_vref_calib_value[6] << 8) + adc_vref_calib_value[5] + 1000;
		gpio_calib_vref_offset = adc_vref_calib_value[4] - 20;

		adc_set_gpio_calib_vref(gpio_calib_vref);
		adc_set_gpio_two_point_calib_offset(gpio_calib_vref_offset);
	}else{
		if(efuse_get_adc_calib_value(&gpio_calib_vref, &gpio_calib_vref_offset)){
			adc_set_gpio_calib_vref(gpio_calib_vref);
			adc_set_gpio_two_point_calib_offset(gpio_calib_vref_offset);
		}else{
			/****** Method of calculating one-point calibration Flash_gpio_Vref value: ********/
			/****** Vref = [1175 +First_Byte-255+Second_Byte] mV = [920 + First_Byte + Second_Byte] mV ********/
			gpio_calib_vref = 920 + adc_vref_calib_value[0] + adc_vref_calib_value[1];
			/****** Check the calibration value whether is correct ********/
			if((gpio_calib_vref > 1047) && (gpio_calib_vref < 1302)){
				adc_set_gpio_calib_vref(gpio_calib_vref);
			}
		}
	}
#endif
}

static void drv_calib_freq_offset(void)
{
#if defined(MCU_CORE_B91)
	u8 freq_offset_value = 0xff;

	flash_read(CFG_FREQUENCY_OFFSET, 1, &freq_offset_value);

	if(freq_offset_value != 0xff){
		rf_update_internal_cap(freq_offset_value);
	}
#endif
}
#endif

void drv_calibration(void)
{
#if defined(MCU_CORE_8258) || defined(MCU_CORE_8278) || defined(MCU_CORE_B91)
	u32 flash_mid = 0;
	u8 flash_uid[16] = {0};

	if(flash_read_mid_uid_with_check(&flash_mid, flash_uid)){
		drv_calib_adc_verf();
		drv_calib_freq_offset();
	}
#endif
}
