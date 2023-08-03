/*-------------------------------------------------------------------------
   _logexpf.c

   Copyright (C) 2005, Paul Stoffregen

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

#define __SDCC_MATH_LIB
#include <math.h>


#ifdef MATH_ASM_MCS51

// This code is shared by both logf() and expf(), so it goes in this
// separate file to allow the linker to include it when either
// function is needed, but only 1 copy when both are used.

void _fs_cordic_rshift_r765_unsigned(void) __naked
{
	__asm
	ar2 = 0x02
	ar3 = 0x03
	ar4 = 0x04
	ar5 = 0x05
	ar6 = 0x06
	ar7 = 0x07
	ar0 = 0x00
	ar1 = 0x01

	add	a, #248
	jnc	00003$
	mov	b, r5
	mov	r5, ar6
	mov	r6, ar7
	mov	r7, #0
	add	a, #248
	jnc	00003$
	mov	b, r5
	mov	r5, ar6
	mov	r6, #0
	add	a, #248
	jnc	00003$
	mov	b, r5
	mov	r5, #0
	add	a, #248
	jnc	00003$
	mov	b, #0
	ret
00003$:
	add	a, #8
	jz	00030$
	push	ar0
	mov	r0, a
00010$:
	clr	c
	mov	a, r7
	rrc	a
	mov	r7, a
	mov	a, r6
	rrc	a
	mov	r6, a
	mov	a, r5
	rrc	a
	mov	r5, a
	mov	a, b
	rrc	a
	mov	b, a
	djnz	r0, 00010$
	pop	ar0
00030$:
	ret
	__endasm;
}

__code unsigned char _fs_natural_log_table[] = {
0xFF, 0x42, 0x2E, 0x16,         // 0.693147180560
0xF6, 0x91, 0xF9, 0x0C,         // 0.405465108108
0xF2, 0xFD, 0x23, 0x07,         // 0.223143551314
0xEE, 0xE0, 0xC4, 0x03,         // 0.117783035656
0x0C, 0xA3, 0xF0, 0x01,         // 0.060624621816
0xD8, 0x14, 0xFC, 0x00,         // 0.030771658667
0xA3, 0x02, 0x7F, 0x00,         // 0.015504186536
0x55, 0xC0, 0x3F, 0x00,         // 0.007782140442
0x0B, 0xF0, 0x1F, 0x00,         // 0.003898640416
0x01, 0xFC, 0x0F, 0x00,         // 0.001951220131
0x00, 0xFF, 0x07, 0x00,         // 0.000976085973
0xC0, 0xFF, 0x03, 0x00,         // 0.000488162080
0xF0, 0xFF, 0x01, 0x00,         // 0.000244110828
0xFC, 0xFF, 0x00, 0x00,         // 0.000122062863
0xFF, 0x7F, 0x00, 0x00,         // 0.000061033294
0x00, 0x40, 0x00, 0x00,         // 0.000030517112
0x00, 0x20, 0x00, 0x00,         // 0.000015258673
0x00, 0x10, 0x00, 0x00,         // 0.000007629365
0x00, 0x08, 0x00, 0x00,         // 0.000003814690
0x00, 0x04, 0x00, 0x00,         // 0.000001907347
0x00, 0x02, 0x00, 0x00,         // 0.000000953674
0x00, 0x01, 0x00, 0x00,         // 0.000000476837
0x80, 0x00, 0x00, 0x00,         // 0.000000238419
0x40, 0x00, 0x00, 0x00,         // 0.000000119209
0x20, 0x00, 0x00, 0x00,         // 0.000000059605
0x10, 0x00, 0x00, 0x00,         // 0.000000029802
0x08, 0x00, 0x00, 0x00,         // 0.000000014901
0x04, 0x00, 0x00, 0x00,         // 0.000000007451
0x02, 0x00, 0x00, 0x00,         // 0.000000003725
0x01, 0x00, 0x00, 0x00          // 0.000000001863
};

#endif
