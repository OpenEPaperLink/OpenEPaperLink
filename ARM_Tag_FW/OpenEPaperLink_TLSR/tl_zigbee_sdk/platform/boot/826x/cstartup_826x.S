/********************************************************************************************************
 * @file    cstartup_826x.S
 *
 * @brief   This is the boot file for B86
 *
 * @author  Driver & Zigbee Group
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

#ifdef MCU_CORE_826x

#include "version_cfg.h"

#ifndef __LOAD_RAM_SIZE__
#define	__LOAD_RAM_SIZE__		0xc
#endif

#ifndef __IRQ_STK_SIZE__
#define	__IRQ_STK_SIZE__		0x200
#endif
	.code	16
@********************************************************************************************************
@                                           MACROS AND DEFINIITIONS
@********************************************************************************************************
					@ Mode, correspords to bits 0-5 in CPSR
	.equ MODE_BITS,		0x1F	@ Bit mask for mode bits in CPSR
	.equ IRQ_MODE, 		0x12	@ Interrupt Request mode
	.equ SVC_MODE, 		0x13	@ Supervisor mode 

	.equ IRQ_STK_SIZE,	__IRQ_STK_SIZE__
	.equ __LOAD_RAM, 	__LOAD_RAM_SIZE__
	
#ifdef CHIP_8269
	.equ RAM_ADDR_END,	0x810000 	@for 8269
#else
	.equ RAM_ADDR_END,	0x80c000  	@for 8267
#endif
@********************************************************************************************************
@                                            TC32 EXCEPTION VECTORS
@********************************************************************************************************

	.section	.vectors,"ax"
	.global		__reset
	.global	 	__irq
	.global 	__start
	.global		__LOAD_RAM

__start:					@ MUST,  referenced by boot.link

	.extern irq_handler


	.extern  _ramcode_size_div_16_
	.extern  _ramcode_size_div_256_
	.extern  _ramcode_size_div_16_align_256_
	.extern  _ictag_start_
	.extern  _ictag_end_

	.extern FILE_VERSION
	.extern MANUFACTURER_CODE_TELINK
	.extern IMAGE_TYPE

	.org 0x0
	tj	__reset
	.word	(FILE_VERSION)
	.org 0x8
	.word	(0x544c4e4b)
	.word	(0x00880000 + _ramcode_size_div_16_align_256_)

	.org 0x10
	tj		__irq

	.org 0x12
	.short  (MANUFACTURER_CODE_TELINK)
	.short  (IMAGE_TYPE)

	.org 0x18
	.word	(_bin_size_)//Firmware size

@********************************************************************************************************
@                                   LOW-LEVEL INITIALIZATION
@********************************************************************************************************
	.extern  main

	.org 0x20
__reset:

	@add debug, PC4 output 1
@	tloadr		r1, GPIO_PC4	@0x800592	PC4 open
@	tmov		r0,	#139		@0b11101111 = 0xef = #139
@	tstorerb	r0, [r1, #0]
@	tmov		r0, #16			@0b00010000 = 0x10 = #16
@	tstorerb	r0, [r1, #1]	@0x800593	PC4 output

	tloadr	r0, FLL_D
	tloadr	r1, FLL_D+4
	tloadr	r2, FLL_D+8

FLL_STK:
	tcmp	r1, r2
	tjge	FLL_STK_END
	tstorer r0, [r1, #0]
	tadd    r1, #4
	tj		FLL_STK
FLL_STK_END:

	tloadr	r0, DAT0
	tmcsr	r0			
	tloadr	r0, DAT0 + 8
	tmov	r13, r0  

	tloadr	r0, DAT0 + 4
	tmcsr	r0	
	tloadr	r0, DAT0 + 12
	tmov	r13, r0  

	tmov	r0, #0
	tloadr	r1, DAT0 + 16
	tloadr	r2, DAT0 + 20

ZERO:
	tcmp	r1, r2
	tjge	ZERO_END
	tstorer	r0, [r1, #0]
	tadd    r1, #4
	tj		ZERO
ZERO_END:

	tloadr	r1, DAT0 + 28
	tloadr	r2, DAT0 + 32

ZERO_TAG:
	tcmp	r1, r2
	tjge	ZERO_TAG_END
	tstorer	r0, [r1, #0]
	tadd    r1, #4
	tj		ZERO_TAG
ZERO_TAG_END:

SETIC:
	tloadr     	r1, DAT0 + 24
	tloadr      r0, DAT0 + 36					@ IC tag start
	tstorerb	r0, [r1, #0]
	tadd    	r0, #1							@ IC tag end
	tstorerb	r0, [r1, #1]
	@tmov		r0, #0;
	@tstorerb	r0, [r1, #2]


	tloadr		r1, DATA_I
	tloadr		r2, DATA_I+4
	tloadr		r3, DATA_I+8
COPY_DATA:
	tcmp		r2, r3
	tjge		COPY_DATA_END
	tloadr		r0, [r1, #0]
	tstorer 	r0, [r2, #0]
	tadd    	r1, #4
	tadd		r2, #4
	tj			COPY_DATA
COPY_DATA_END:

	tjl	main
END:	tj	END

	.balign	4
DAT0:
	.word	0x12			    @IRQ    @0
	.word	0x13			    @SVC    @4
	.word	(irq_stk + IRQ_STK_SIZE)
	.word	(RAM_ADDR_END)		@12  stack end
	.word	(_start_bss_)               @16
	.word	(_end_bss_)                 @20
	.word	(0x80060c)                  @24
	.word	_ictag_start_               @28		@ IC tag start
	.word	_ictag_end_	            	@32		@ IC tag end
	.word	_ramcode_size_div_256_		@36

@	.word	(0x80058c)                  @36		gpio
@	.word	(0x800620)                  @40		watchdog
@	.word	(0x802c01)                  @44		watchdog
DATA_I:	
	.word	_dstored_
	.word	_start_data_
	.word	_end_data_
	.word	_rstored_

@GPIO_PC4:
@	.word	(0x800592)

FLL_D:
	.word	0xffffffff
	.word	(_start_data_)
	.word	(RAM_ADDR_END)

	.align 4
__irq:
	tpush    	{r14}
	tpush    	{r0-r7}
	tmrss    	r0
	
	tmov		r1, r8
	tmov		r2, r9
	tmov		r3, r10
	tmov		r4, r11
	tmov		r5, r12
	tpush		{r0-r5}
	
	tjl      	irq_handler

	tpop		{r0-r5}
	tmov		r8, r1
	tmov		r9, r2
	tmov		r10,r3
	tmov		r11,r4
	tmov		r12,r5

	tmssr    	r0
	tpop		{r0-r7}
	treti    	{r15}

ASMEND:

	.section .bss
	.align 4
	.lcomm irq_stk, IRQ_STK_SIZE
	.end

#endif
