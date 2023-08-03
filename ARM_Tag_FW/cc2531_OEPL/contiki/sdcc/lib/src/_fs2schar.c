/*-------------------------------------------------------------------------
   _fs2schar.c - Floating point library in optimized assembly for 8051

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

// char __fs2schar (float x)
static void dummy(void) __naked
{
        __asm
        .globl  ___fs2schar
___fs2schar:
        lcall   ___fs2slong
        jnz     fs2schar_not_zero
        mov     a, dpl
        orl     a, dph
        orl     a, b
        jnz     fs2schar_clr_a
        ret
fs2schar_clr_a:
        clr a
fs2schar_not_zero:
        jnb     sign_a, fs2schar_pos
fs2schar_neg:
        cpl     a
        jnz     fs2schar_maxval_neg
        mov     a, b
        cpl     a
        jnz     fs2schar_maxval_neg
        mov     a, dph
        cpl     a
        jnz     fs2schar_maxval_neg
        mov     a, dpl
        jnb     acc.7, fs2schar_maxval_neg
        ret
fs2schar_maxval_neg:
        mov     dpl, #0x80
        ret
fs2schar_pos:
        jnz     fs2schar_maxval_pos
        mov     a, b
        jnz     fs2schar_maxval_pos
        mov     a, dph
        jnz     fs2schar_maxval_pos
        mov     a, dpl
        jb      acc.7, fs2schar_maxval_pos
        ret
fs2schar_maxval_pos:
        mov     dpl, #0x7F
        ret
        __endasm;
}

#else

/* convert float to signed char */
signed char __fs2schar (float f)
{
  signed long sl=__fs2slong(f);
  if (sl>=SCHAR_MAX)
    return SCHAR_MAX;
  if (sl<=SCHAR_MIN)
    return -SCHAR_MIN;
  return sl;
}

#endif
