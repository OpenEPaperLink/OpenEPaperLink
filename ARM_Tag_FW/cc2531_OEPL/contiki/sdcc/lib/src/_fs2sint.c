/*-------------------------------------------------------------------------
   _fs2sint.c - Floating point library in optimized assembly for 8051

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

// int __fs2sint (float x)
static void dummy(void) __naked
{
	__asm
	.globl	___fs2sint
___fs2sint:
	lcall	___fs2slong
	jnz fs2sint_not_zero
	mov a, dpl
	orl a, dph
	orl a, b
	jnz fs2sint_clr_a
	ret
fs2sint_clr_a:
	clr a
fs2sint_not_zero:
	jnb	sign_a, fs2sint_pos
fs2sint_neg:
	cpl	a
	jnz	fs2sint_maxval_neg
	mov	a, b
	cpl	a
	jnz	fs2sint_maxval_neg
	mov	a, dph
	jnb	acc.7, fs2sint_maxval_neg
	ret
fs2sint_maxval_neg:
	mov	dptr, #0x8000
	ret
fs2sint_pos:
	jnz	fs2sint_maxval_pos
	mov	a, b
	jnz	fs2sint_maxval_pos
	mov	a, dph
	jb	acc.7, fs2sint_maxval_pos
	ret
fs2sint_maxval_pos:
	mov	dptr, #0x7FFF
	ret
	__endasm;
}

#else

/* convert float to signed int */
signed int __fs2sint (float f)
{
  signed long sl=__fs2slong(f);
  if (sl>=INT_MAX)
    return INT_MAX;
  if (sl<=INT_MIN)
    return -INT_MIN;
  return sl;
}

#endif
