/*-------------------------------------------------------------------------
   _fsrshift.c - Floating point library in optimized assembly for 8051

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
	.globl	fs_rshift_a
fs_rshift_a:
	jz	00020$
	mov	r0, a
	add	a, exp_a	// adjust exponent
	jnc	00001$
	mov	a, #255		// don't roll over
00001$:
	mov	exp_a, a
#ifdef FLOAT_SHIFT_SPEEDUP
	mov	a, r0
	add	a, #248
	jnc	00003$
	xch	a, r4
	xch	a, r3
	xch	a, r2
	mov	r1, a
	clr	a
	xch	a, r4
	//mov	r1, ar2		// avoid dependence on register bank
	//mov	r2, ar3
	//mov	r3, ar4
	//mov	r4, #0
	add	a, #248
	jnc	00003$
	xch	a, r3
	xch	a, r2
	mov	r1, a
	clr	a
	xch	a, r3
	//mov	r1, ar2
	//mov	r2, ar3
	//mov	r3, #0
	add	a, #248
	jnc	00003$
	xch	a, r2
	mov	r1, a
	clr	a
	xch	a, r2
	//mov	r1, ar2
	//mov	r2, #0
	add	a, #248
	jnc	00003$
	mov	r1, #0
	ret
00003$:
	add	a, #8
	jz	00020$
	mov	r0, a
#endif
00005$:
	clr	c
	mov	a, r4
	rrc	a
	mov	r4, a
	mov	a, r3
	rrc	a
	mov	r3, a
	mov	a, r2
	rrc	a
	mov	r2, a
	mov	a, r1
	rrc	a
	mov	r1, a
	djnz	r0, 00005$
00020$:
	ret
	__endasm;
}

#endif
