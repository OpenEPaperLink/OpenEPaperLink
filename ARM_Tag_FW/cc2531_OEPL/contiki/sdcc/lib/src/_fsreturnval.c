/*-------------------------------------------------------------------------
   _fsreturnval.c - Floating point library in optimized assembly for 8051

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

	.globl	fs_round_and_return
fs_round_and_return:
#ifdef FLOAT_FULL_ACCURACY
	// discard the extra 8 bits of precision we kept around in r1
	cjne	r1, #128, 00001$
	mov	a, r2
	rrc	a
	cpl	c
00001$:
	jc	fs_zerocheck_return
	mov	a, r2
	add	a, #1
	mov	r2, a
	clr	a
	addc	a, r3
	mov	r3, a
	clr	a
	addc	a, r4
	mov	r4, a
	jnc	fs_zerocheck_return
	mov	r4, #0x80
	inc	exp_a
#endif

	.globl	fs_zerocheck_return
fs_zerocheck_return:
	// zero output is a special case
	cjne	r4, #0, fs_direct_return
	cjne	r3, #0, fs_direct_return
	cjne	r2, #0, fs_direct_return

	.globl	fs_return_zero
fs_return_zero:
	clr	a
	mov	b, a
	mov	dph, a
	mov	dpl, a
	ret

	.globl	fs_direct_return
fs_direct_return:
	// collect all pieces and return
	mov	c, sign_a
	mov	a, exp_a
	rrc	a
	mov	b, r4
	mov	b.7, c
	mov	dph, r3
	mov	dpl, r2
	ret

	.globl	fs_return_inf
fs_return_inf:
	clr	a
	mov	dph, a
	mov	dpl, a
	mov	b, #0x80
	cpl	a
	mov	c, sign_a
	rrc	a
	ret

	.globl	fs_return_nan
fs_return_nan:
	clr	a
	mov	dph, a
	mov	dpl, a
	mov	b, #0xC0
	mov	a, #0x7F
	ret

	__endasm;
}

#endif
