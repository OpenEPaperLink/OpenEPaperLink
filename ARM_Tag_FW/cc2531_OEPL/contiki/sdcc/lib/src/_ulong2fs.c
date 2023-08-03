/*-------------------------------------------------------------------------
   _ulong2fs.c - Floating point library in optimized assembly for 8051

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

// float long __ulong2fs (float x)
static void dummy(void) __naked
{
	__asm
	.globl	___ulong2fs
___ulong2fs:
	mov	r4, a
	mov	r3, b
	mov	r2, dph
	mov	r1, dpl
	mov	a, #158
	.globl	ulong2fs_doit
ulong2fs_doit:
	clr	sign_a
long2fs_doit:
	mov	exp_a, a
	lcall	fs_normalize_a
	ljmp	fs_round_and_return
	__endasm;
}

#else

/*
** libgcc support for software floating point.
** Copyright (C) 1991 by Pipeline Associates, Inc.  All rights reserved.
** Permission is granted to do *anything* you want with this file,
** commercial or otherwise, provided this message remains intact.  So there!
** I would appreciate receiving any updates/patches/changes that anyone
** makes, and am willing to be the repository for said changes (am I
** making a big mistake?).
**
** Pat Wood
** Pipeline Associates, Inc.
** pipeline!phw@motown.com or
** sun!pipeline!phw or
** uunet!motown!pipeline!phw
*/

/* (c)2000/2001: hacked a little by johan.knol@iduna.nl for sdcc */

union float_long
  {
    float f;
    long l;
  };

float __ulong2fs (unsigned long a )
{
  unsigned char exp = 24 + EXCESS;
  volatile union float_long fl;

  if (!a)
    {
      return 0.0;
    }

  while (a < HIDDEN)
    {
      a <<= 1;
      exp--;
    }

  while (a & NORM) 
    {
      // we lose accuracy here
      if (a & 1)
        a += 2;
      a >>= 1;
      exp++;
    }
  
  a &= ~HIDDEN ;
  /* pack up and go home */
  fl.l = PACK(0,(unsigned long)exp, a);

  return (fl.f);
}

#endif
