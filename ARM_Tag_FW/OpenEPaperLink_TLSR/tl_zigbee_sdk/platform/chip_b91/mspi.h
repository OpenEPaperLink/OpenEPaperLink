/********************************************************************************************************
 * @file	mspi.h
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
#pragma once

#include "compiler.h"
#include "gpio.h"
#include "reg_include/mspi_reg.h"


/**
 * @brief 	Data line mode of mspi
 */
typedef enum{
	MSPI_SINGLE_LINE	= 0x00,
	MSPI_DUAL_LINE		= 0x04,
	MSPI_QUAD_LINE		= 0x0c,
}mspi_data_line_e;


/**
  * @brief     This function servers to set the spi wait.
  * @return    none.
  */
_attribute_ram_code_sec_ static inline void mspi_wait(void){
	while(reg_mspi_status & FLD_MSPI_BUSY);
}

/**
 * @brief     This function servers to enable read triggle spi.
 * @return    none.
 */
_attribute_ram_code_sec_ static inline void mspi_fm_rd_trig_en(void){
	reg_mspi_fm |= FLD_MSPI_RD_TRIG_EN;
}

/**
 * @brief     This function servers to disable read triggle spi.
 * @return    none.
 */
_attribute_ram_code_sec_ static inline void mspi_fm_rd_trig_dis(void){
	reg_mspi_fm &= ~FLD_MSPI_RD_TRIG_EN;
}

/**
 * @brief     This function is used to configure mspi to read mode.
 * @return    none.
 */
_attribute_ram_code_sec_ static inline void mspi_fm_read_en(void){
	reg_mspi_fm |= FLD_MSPI_RD_MODE;
}

/**
 * @brief     This function is used to configure mspi to write mode.
 * @return    none.
 */
_attribute_ram_code_sec_ static inline void mspi_fm_write_en(void){
	reg_mspi_fm &= ~FLD_MSPI_RD_MODE;
}

/**
 * @brief     This function is used to configure mspi for several-line communication mode.
 * @param[in] line	- several-line communication mode.
 * @return    none.
 */
_attribute_ram_code_sec_ static inline void mspi_fm_data_line(mspi_data_line_e line){
	reg_mspi_fm = (reg_mspi_fm & (~FLD_MSPI_DATA_LINE)) | line;
}

/**
 * @brief     This function servers to set spi interface csn signal.
 * @return    none.
 */
_attribute_ram_code_sec_ static inline void mspi_high(void){
	reg_mspi_fm |= FLD_MSPI_CSN;
}

/**
 * @brief     This function servers to clear spi interface csn signal.
 * @return    none.
 */
_attribute_ram_code_sec_ static inline void mspi_low(void){
	reg_mspi_fm &= ~FLD_MSPI_CSN;
}

/**
 * @brief		This function servers to gets the spi data.
 * @return		the spi data.
 */
_attribute_ram_code_sec_ static inline unsigned char mspi_get(void){
	return reg_mspi_data;
}

/**
 * @brief		This function servers to write the spi.
 * @param[in]	c	- the char need to be write.
 * @return		none.
 */
_attribute_ram_code_sec_ static inline void mspi_write(unsigned char c){
	reg_mspi_data = c;
}

/**
 * @brief		This function servers to control the write.
 * @param[in]	c	- need to be write.
 * @return		none.
 */
_attribute_ram_code_sec_ static inline void mspi_fm_write(unsigned char c){
	reg_mspi_fm = c;
}

/**
 * @brief		This function servers to spi read.
 * @return		read result.
 */
_attribute_ram_code_sec_ static inline unsigned char mspi_read(void){
	mspi_write(0);		// dummy, issue clock
	mspi_wait();
	return mspi_get();
}

/**
 * @brief		This function serves to Stop XIP operation before flash.
 * @return		none.
 */
_attribute_ram_code_sec_ static inline void mspi_stop_xip(void)
{
	mspi_wait();	//wait xip busy=0
	mspi_high();	//mspi_cn=1, stop xip read
	while(gpio_get_level(GPIO_PF3) == 0);	//wait cn=1
}



