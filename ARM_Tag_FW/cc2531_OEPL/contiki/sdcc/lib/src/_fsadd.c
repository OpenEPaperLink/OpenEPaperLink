/*-------------------------------------------------------------------------
   _fsadd.c - Floating point library in optimized assembly for 8051

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
#include <stdbool.h>
#include <sdcc-lib.h>

#ifdef FLOAT_ASM_MCS51

// float __fsadd (float a, float b) __reentrant
static void dummy(void) __naked
{
	__asm

	// extract the two inputs, placing them into:
	//      sign     exponent   mantissa
	//      ----     --------   --------
	//  a:  sign_a   exp_a      r4/r3/r2
	//  b:  sign_b   exp_b      r7/r6/r5
	//
	// r1: used to extend precision of a's mantissa
	// r0: general purpose loop counter

	.globl	___fsadd
___fsadd:
	lcall	fsgetargs

	.globl	fsadd_direct_entry
fsadd_direct_entry:
	// we're going to extend mantissa to 32 bits temporarily
	mov	r1, #0

	// which exponent is greater?
	mov	a, exp_b
	cjne	a, exp_a, 00005$
	sjmp	00011$
00005$:	jnc	00010$

	// a's exponent was greater, so shift b's mantissa
	lcall	fs_swap_a_b

00010$:
	// b's exponent was greater, so shift a's mantissa
	mov	a, exp_b
	clr	c
	subb	a, exp_a
	lcall	fs_rshift_a	// acc has # of shifts to do

00011$:
	// decide if we need to add or subtract
	// sign_a and sign_b are stored in the flag bits of psw,
	// so this little trick checks if the arguements have the
	// same sign.
	mov	a, psw
	swap	a
	xrl	a, psw
	jb	acc.1, 00022$

00020$:
	// add the mantissas (both positive or both negative)
	mov	a, r2
	add	a, r5
	mov	r2, a
	mov	a, r3
	addc	a, r6
	mov	r3, a
	mov	a, r4
	addc	a, r7
	mov	r4, a
	// check for overflow past 24 bits
	jnc	00021$
	mov	a, #1
	lcall	fs_rshift_a
	mov	a, r4
	orl	a, #0x80
	mov	r4, a
00021$:
	ljmp	fs_round_and_return



00022$:
	// subtract the mantissas (one of them is negative)
	clr	c
	mov	a, r2
	subb	a, r5
	mov	r2, a
	mov	a, r3
	subb	a, r6
	mov	r3, a
	mov	a, r4
	subb	a, r7
	mov	r4, a
	jnc	00025$
	// if we get a negative result, turn it positive and
	// flip the sign bit
	clr	c
	clr	a
	subb	a, r1
	mov	r1, a
	clr	a
	subb	a, r2
	mov	r2, a
	clr	a
	subb	a, r3
	mov	r3, a
	clr	a
	subb	a, r4
	mov	r4, a
	cpl	sign_a
00025$:
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

union float_long
  {
    float f;
    unsigned long l;
  };

/* add two floats */
float __fsadd (float a1, float a2)
{
  long mant1, mant2;
  long _AUTOMEM *pfl1;
  long _AUTOMEM *pfl2;
  int exp1, exp2, expd;
  bool sign = false;

  pfl2 = (long _AUTOMEM *)&a2;
  exp2 = EXP (*pfl2);
  mant2 = MANT (*pfl2) << 4;
  if (SIGN (*pfl2))
    mant2 = -mant2;
  /* check for zero args */
  if (!*pfl2)
    return (a1);

  pfl1 = (long _AUTOMEM *)&a1;
  exp1 = EXP (*pfl1);
  mant1 = MANT (*pfl1) << 4;
  if (SIGN(*pfl1))
  if (*pfl1 & 0x80000000)
    mant1 = -mant1;
  /* check for zero args */
  if (!*pfl1)
    return (a2);

  expd = exp1 - exp2;
  if (expd > 25)
    return (a1);
  if (expd < -25)
    return (a2);

  if (expd < 0)
    {
      expd = -expd;
      exp1 += expd;
      mant1 >>= expd;
    }
  else
    {
      mant2 >>= expd;
    }
  mant1 += mant2;

  sign = false;

  if (mant1 < 0)
    {
      mant1 = -mant1;
      sign = true;
    }
  else if (!mant1)
    return (0);

  /* normalize */
  while (mant1 < (HIDDEN<<4)) {
    mant1 <<= 1;
    exp1--;
  }

  /* round off */
  while (mant1 & 0xf0000000) {
    if (mant1&1)
      mant1 += 2;
    mant1 >>= 1;
    exp1++;
  }

  /* turn off hidden bit */
  mant1 &= ~(HIDDEN<<4);

  /* pack up and go home */
  if (exp1 >= 0x100)
    *pfl1 = (sign ? (SIGNBIT | __INFINITY) : __INFINITY);
  else if (exp1 < 0)
    *pfl1 = 0;
  else
    *pfl1 = PACK (sign ? SIGNBIT : 0 , exp1, mant1>>4);
  return (a1);
}

#endif
