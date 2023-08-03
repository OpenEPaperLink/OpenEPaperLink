/*-------------------------------------------------------------------------
   _fs2ulong.c - Floating point library in optimized assembly for 8051

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

// unsigned long __fs2ulong (float x)
static void dummy(void) __naked
{
	__asm
	.globl	___fs2ulong
___fs2ulong:
	mov	r7, #158
	.globl	fs2ulong_begin
fs2ulong_begin:
	lcall	fsgetarg
	jnb	sign_a, fs2ulong_int
	ljmp	fs_return_zero

fs2ulong_int:
	clr	c
	mov	a, r7
	subb	a, exp_a
	jnc	fs2ulong_int_ok
	// if we get here, x >= 2^32
	mov	a, #0xFF
	mov	b, a
	mov	dph, a
	mov	dpl, a
	ret

fs2ulong_int_ok:
	mov	r1, #0
	lcall	fs_rshift_a

fs2ulong_done:
	mov	dpl, r1
	mov	dph, r2
	mov	b, r3
	mov	a, r4
	ret
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

/* convert float to unsigned long */
unsigned long 
__fs2ulong (float a1)
{
  volatile union float_long fl1;
  volatile int exp;
  volatile long l;
  
  fl1.f = a1;
  
  if (!fl1.l || SIGN(fl1.l))
    return (0);

  exp = EXP (fl1.l) - EXCESS - 24;
  l = MANT (fl1.l);
  
  l >>= -exp;

  return l;
}

#endif
