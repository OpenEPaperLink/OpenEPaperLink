/*-------------------------------------------------------------------------
   _uchar2fs.c - Floating point library in optimized assembly for 8051

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

// float long __uchar2fs (float x)
static void dummy(void) __naked
{
	__asm
	.globl	___uchar2fs
___uchar2fs:
	clr	a
	mov	r4, dpl
	mov	r3, a
	mov	r2, a
	mov	r1, a
	mov	a, #134
	ljmp	ulong2fs_doit
	__endasm;
}

#else

/* convert unsigned char to float */
float __uchar2fs (unsigned char uc) {
  return __ulong2fs(uc);
}

#endif
