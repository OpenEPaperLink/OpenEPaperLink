/*-------------------------------------------------------------------------
   _fsnormalize.c - Floating point library in optimized assembly for 8051

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
	.globl	fs_normalize_a
fs_normalize_a:
#ifdef FLOAT_SHIFT_SPEEDUP
	mov	r0, #4
	mov	a, r4
00001$:
	jnz	00003$
	mov	a, exp_a
	add	a, #248
	jnc	00002$	;denormalized
	mov	exp_a, a
	clr	a
	xch	a, r1
	xch	a, r2
	xch	a, r3
	mov	r4, a
	//mov	r4, ar3
	//mov	r3, ar2
	//mov	r2, ar1
	//mov	r1, #0
	djnz	r0, 00001$
	ret
#endif
00002$:
	mov	a, r4
00003$:
	mov	r0, #32
	inc	exp_a
00004$:
	jb	acc.7, 00006$
	djnz	exp_a, 00005$
	ret			;denormalized
00005$:
	clr	c
	mov	a, r1
	rlc	a
	mov	r1, a
	mov	a, r2
	rlc	a
	mov	r2, a
	mov	a, r3
	rlc	a
	mov	r3, a
	mov	a, r4
	rlc	a
	mov	r4, a
	djnz	r0, 00004$
00006$:
	dec	exp_a
	ret
	__endasm;
}

#endif
