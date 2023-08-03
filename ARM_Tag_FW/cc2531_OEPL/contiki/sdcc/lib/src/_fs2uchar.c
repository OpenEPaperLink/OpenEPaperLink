/*-------------------------------------------------------------------------
   _fs2uchar.c - Floating point library in optimized assembly for 8051

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

// unsigned char __fs2uchar (float x)
static void dummy(void) __naked
{
	__asm
	.globl	___fs2uchar
___fs2uchar:
	mov	r7, #134
	lcall	fs2ulong_begin
	mov	dpl, a
	ret
	__endasm;
}

#else

/* convert float to unsigned char */
unsigned char __fs2uchar (float f)
{
  unsigned long ul=__fs2ulong(f);
  if (ul>=UCHAR_MAX) return UCHAR_MAX;
  return ul;
}

#endif
