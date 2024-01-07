/********************************************************************************************************
 * @file    div_mod.S
 *
 * @brief   This is the div & mod file for b85m
 *
 * @author  Driver Group
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

#define 	UDIV		#0
#define 	SDIV		#1
#define 	UMOD		#2
#define 	SMOD		#3

	.code	16
	.text

	.align	2
	.global	__modsi3
	.code 16
	.thumb_func
	.type	__modsi3, %function
__modsi3:
	tmov	r2, SMOD
	tj	div
	.size	__modsi3, .-__modsi3

	.align	2
	.global	__divsi3
	.code 16
	.thumb_func
	.type	__divsi3, %function
__divsi3:
	tmov	r2, SDIV
	tj	div
	.size	__divsi3, .-__divsi3

	.align	2
	.global	__umodsi3
	.code 16
	.thumb_func
	.type	__umodsi3, %function
__umodsi3:
	tmov	r2, UMOD
	tj	div
	.size	__umodsi3, .-__umodsi3

	.align	2
	.global	__udivsi3
	.code 16
	.thumb_func
	.type	__udivsi3, %function
__udivsi3:
	tmov	r2, UDIV
	tj	div
	.size	__udivsi3, .-__udivsi3

	.align	2
	.global	div
	.code 16
	.thumb_func
	.type	div, %function
div:
	tmrcs	r3
	tpush	{r3, r4}
	tmov	r4, #0x80
	tor	r3, r4
	tmcsr	r3
	
	tloadr	r3, .L11
	tstorer	r0, [r3]
	tadd	r3, r3, #4
	tstorer	r1, [r3]
	tsub	r3, r3, #8
	tstorerb	r2, [r3]

.L2:
	tloadrb	r0, [r3]
	tcmp	r0, #0
	tjne	.L2
	tcmp	r2, #1
	tjls	.L4
	tadd	r3, r3, #8
	tloadr	r0, [r3]
	tj	.L6
.L4:
	tadd	r3, r3, #4
	tloadr	r0, [r3]

.L6:
	tpop	{r3, r4}
	tmcsr	r3
	tjex	lr

	.align	4
.L11:
	.word(0x800664)
	.word(0x800660)
	.word(0x800668)
	.size	div, .-div
