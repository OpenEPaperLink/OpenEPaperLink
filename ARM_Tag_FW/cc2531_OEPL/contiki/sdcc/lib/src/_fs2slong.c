/*-------------------------------------------------------------------------
   _fs2slong.c - Floating point library in optimized assembly for 8051

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

// long __fs2slong (float x)
static void dummy(void) __naked
{
	__asm
	.globl	___fs2slong
___fs2slong:
	lcall	fsgetarg
	clr	c
	mov	a, #158
	subb	a, exp_a
	jc	fs2slong_maxval		//  |x| >= 2^32
fs2slong_int_ok:
	mov	r1, #0
	lcall	fs_rshift_a
	jnb	sign_a, fs2slong_pos
fs2slong_neg:
	mov	a, r1
	cpl	a
	add	a, #1
	mov	dpl, a
	mov	a, r2
	cpl	a
	addc	a, #0
	mov	dph, a
	mov	a, r3
	cpl	a
	addc	a, #0
	mov	b, a
	mov	a, r4
	cpl	a
	addc	a, #0
	//Check for zero
	jnz fs2slong_not_zero
	mov a, dpl
	orl a, dph
	orl a, b
	jnz fs2slong_clr_a
	ret
fs2slong_clr_a:
	clr a
fs2slong_not_zero:
	jnb acc.7, fs2slong_maxval_neg  // x < -0x80000000
	ret
fs2slong_pos:
	mov	a, r4
	jb	acc.7, fs2slong_maxval_pos  //  x > 0x7FFFFFFF
	mov	dpl, r1
	mov	dph, r2
	mov	b, r3
	ret
fs2slong_maxval:
	jnb	sign_a, fs2slong_maxval_pos
fs2slong_maxval_neg:
	clr	a
	mov	dpl, a
	mov	dph, a
	mov	b, a
	mov	a, #0x80
	ret
fs2slong_maxval_pos:
	mov	a, #0xFF
	mov	dpl, a
	mov	dph, a
	mov	b, a
	mov	a, #0x7F
	ret
	__endasm;
}

#else

/* convert float to signed long */
signed long __fs2slong (float f)
{

  if (!f)
    return 0;

  if (f<0) {
    return -__fs2ulong(-f);
  } else {
    return __fs2ulong(f);
  }
}

#endif
