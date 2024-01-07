/********************************************************************************************************
 * @file	swire_reg.h
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
#ifndef SWIRE_REG_H
#define SWIRE_REG_H
#include "soc.h"




#define    SWIRE_BASE_ADDR			        0x100c00


#define   reg_swire_data      		    REG_ADDR8(SWIRE_BASE_ADDR)

#define   reg_swire_ctl				    REG_ADDR8(SWIRE_BASE_ADDR+1)
enum{
	FLD_SWIRE_WR	        	= BIT(0),
	FLD_SWIRE_RD 	     	    = BIT(1),
	FLD_SWIRE_CMD 		        = BIT(2),
	FLD_SWIRE_ERR_FLAG			= BIT(3),
	// default :FLD_SWIRE_EOP = 1: 1byte format--- 1bit cmd + 8bit data +1 clock end(0)
	// FLD_SWIRE_EOP = 0: 1byte format--- 1bit cmd + 8bit data +1bit(1) + 1 clock end(0)
	FLD_SWIRE_EOP     			= BIT(4),
	FLD_SWIRE_USB_DET    	  	= BIT(6),
	FLD_SWIRE_USB_EN      		= BIT(7),
};
#define   reg_swire_ctl2				 REG_ADDR8(SWIRE_BASE_ADDR+2)
enum{
	FLD_SWIRE_CLK_DIV	        = BIT_RNG(0,6),
};

#define   reg_swire_id				    REG_ADDR8(SWIRE_BASE_ADDR+3)
enum{
	FLD_SWIRE_ID_VALID	        = BIT_RNG(0,4),
	FLD_SWIRE_FIFO_MODE	        = BIT(7),
};
#endif
