/*-------------------------------------------------------------------------
   _fsget2args.c - Floating point library in optimized assembly for 8051

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
	// arg1: passed in a,b,dph,dpl
	// arg2: passed on stack
	__asm
	.globl	fsgetargs
fsgetargs:
	// extract the two inputs, placing them into:
	//      sign     exponent   mantissa
	//      ----     --------   --------
	//  a:  sign_a   exp_a      r4/r3/r2
	//  b:  sign_b   exp_b      r7/r6/r5
	//
	mov	r2, dpl
	mov	r3, dph
	mov	c, b.7
	rlc	a
	mov	sign_a, c
	mov	exp_a, a
	jz	00001$
	setb	b.7
00001$:
	mov	r4, b
	// now extract the 2nd parameter from the stack
	mov	a, sp
	add	a, #249
	mov	r0, a
	mov	a, @r0
	mov	r5, a
	inc	r0
	mov	a, @r0
	mov	r6, a
	inc	r0
	mov	b, @r0
	inc	r0
	mov	a, @r0
	mov	c, b.7
	rlc	a
	mov	sign_b, c
	mov	exp_b, a
	jz	00002$
	setb	b.7
00002$:
	mov	r7, b
	ret
	__endasm;
}

#endif
