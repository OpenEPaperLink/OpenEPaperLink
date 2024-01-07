/********************************************************************************************************
 * @file    drv_adc.h
 *
 * @brief   This is the header file for drv_adc
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

#pragma once



typedef enum{
	DRV_ADC_BASE_MODE,
	DRV_ADC_VBAT_MODE,
}drv_adc_mode_t;

/****
* brief: ADC initiate function
* param[in] null
*
* @return	  1: set success ;
*             0: set error
*/
bool drv_adc_init(void);

/****
* brief: get the sample data
* param[in] null
* @return,the result
*/
u16 drv_get_adc_data(void);

/****
* brief: set ADC test mode and pin
* param[in]
* @return
*/
#if defined (MCU_CORE_826x)
void drv_adc_mode_pin_set(drv_adc_mode_t mode, ADC_InputPTypeDef pin);
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
void drv_adc_mode_pin_set(drv_adc_mode_t mode, GPIO_PinTypeDef pin);
#elif defined(MCU_CORE_B91)
void drv_adc_mode_pin_set(drv_adc_mode_t mode, adc_input_pin_def_e pin);
#endif

/****
* brief: start/stop ADC
* param[in] TRUE/FALSE
* @return
*/
void drv_adc_enable(bool enable);


