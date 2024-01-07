/********************************************************************************************************
 * @file    spi_i.h
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

#include "register.h"
#include "compiler.h"

 /**
  * @brief     This function servers to set the spi wait.
  * @param[in] none
  * @return    none
  */
_attribute_ram_code_sec_ static inline void mspi_wait(void){
	while(reg_mspi_ctrl & FLD_MSPI_BUSY)
		;
}

/**
 * @brief     This function servers to set the spi high level.
 * @param[in] none
 * @return    none
 */
_attribute_ram_code_sec_ static inline void mspi_high(void){
	reg_mspi_ctrl = FLD_MSPI_CS;
}

/**
 * @brief     This function servers to set the spi low level.
 * @param[in] none
 * @return    none
 */
_attribute_ram_code_sec_ static inline void mspi_low(void){
	reg_mspi_ctrl = 0;
}

/**
 * @brief     This function servers to gets the spi data.
 * @param[in] none.
 * @return    the spi data.
 */
_attribute_ram_code_sec_ static inline unsigned char mspi_get(void){
	return reg_mspi_data;
}

/**
 * @brief     This function servers to write the spi.
 * @param[in] c - the char need to be write.
 * @return    none
 */
_attribute_ram_code_sec_ static inline void mspi_write(unsigned char c){
	reg_mspi_data = c;
}

/**
 * @brief     This function servers to control the write.
 * @param[in] c - need to be write.
 * @return    none
 */
_attribute_ram_code_sec_ static inline void mspi_ctrl_write(unsigned char c){
	reg_mspi_ctrl = c;
}

/**
 * @brief     This function servers to spi read.
 * @param[in] none.
 * @return    read reault.
 */
_attribute_ram_code_sec_ static inline unsigned char mspi_read(void){
	mspi_write(0);		// dummy, issue clock
	mspi_wait();
	return mspi_get();
}






