/*-------------------------------------------------------------------------
   _fscmp.c - Floating point library in optimized assembly for 8051

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

	.globl	fs_compare_uint32
fs_compare_uint32:
	mov	r1, #1
	mov	r2, dpl
	mov	a, @r0
	mov	dpl, r7
	cjne	a, dpl, compare32_done
	dec	r0
	mov	a, @r0
	cjne	a, b, compare32_done
	dec	r0
	mov	a, @r0
	cjne	a, dph, compare32_done
	dec	r0
	mov	a, @r0
	mov	dpl, r2
	cjne	a, dpl, compare32_done
	mov	r1, #0
compare32_done:
	ret


	.globl	fs_check_negative_zeros
fs_check_negative_zeros:
a_check:
	cjne	a, #0x80, a_not_neg_zero
	mov	a, b
	jnz	a_not_neg_zero_cleanup
	mov	a, dph
	jnz	a_not_neg_zero_cleanup
	mov	a, dpl
	jnz	a_not_neg_zero_cleanup
	mov	r7, #0
a_not_neg_zero_cleanup:
	mov	a, r7
a_not_neg_zero:

b_check:
	cjne	@r0, #0x80, b_not_neg_zero
	dec	r0
	cjne	@r0, #0, b_not_neg_zero_cleanup_1
	dec	r0
	cjne	@r0, #0, b_not_neg_zero_cleanup_2
	dec	r0
	cjne	@r0, #0, b_not_neg_zero_cleanup_3
	inc	r0
	inc	r0
	inc	r0
	mov	@r0, #0
	ret
b_not_neg_zero_cleanup_3:
	inc	r0
b_not_neg_zero_cleanup_2:
	inc	r0
b_not_neg_zero_cleanup_1:
	inc	r0
b_not_neg_zero:
	ret

	__endasm;
}

#endif
