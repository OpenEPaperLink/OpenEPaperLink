/*-------------------------------------------------------------------------
   _fsswapargs.c - Floating point library in optimized assembly for 8051

   Copyright (c) 2004, Paul Stoffregen, paul@pjrc.com

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License 
   along with this library; see the file COPYING. If not, write to the
   Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.

   As a special exception, if you link this library with other files,
   some of which are compiled with SDCC, to produce an executable,
   this library does not by itself cause the resulting executable to
   be covered by the GNU General Public License. This exception does
   not however invalidate any other reasons why the executable file
   might be covered by the GNU General Public License.
-------------------------------------------------------------------------*/



#define __SDCC_FLOAT_LIB
#include <float.h>


#ifdef FLOAT_ASM_MCS51

static void dummy(void) __naked
{
	__asm
	.globl	fs_swap_a_b
fs_swap_a_b:
	mov	a, exp_a
	xch	a, exp_b
	mov	exp_a, a
	// is there a faster way to swap these 2 bits???
	// this trick with psw can play havoc with the resiter bank setting
	// mov	a, psw
	// swap	a		// depends on sign bits in psw.1 & psw.5
	// mov	psw, a
	mov	c, sign_a
	rlc	a
	mov	c, sign_b
	mov	sign_a, c
	rrc	a
	mov	sign_b, c
	mov	a, r4
	xch	a, r7
	mov	r4, a
	mov	a, r3
	xch	a, r6
	mov	r3, a
	mov	a, r2
	xch	a, r5
	mov	r2, a
	ret
	__endasm;
}

#endif
