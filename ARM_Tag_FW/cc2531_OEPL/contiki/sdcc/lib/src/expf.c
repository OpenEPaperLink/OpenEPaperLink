/*-------------------------------------------------------------------------
   expf.c - Computes e**x of a 32-bit float as outlined in [1]

   Copyright (C) 2001, 2002, Jesus Calvino-Fraga, jesusc@ieee.org

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

/* [1] William James Cody and W.  M.  Waite.  _Software manual for the
   elementary functions_, Englewood Cliffs, N.J.:Prentice-Hall, 1980. */

/* Version 1.0 - Initial release */

#define __SDCC_MATH_LIB
#include <math.h>
#include <errno.h>
#include <stdbool.h>

#ifdef MATH_ASM_MCS51

#define __SDCC_FLOAT_LIB
#include <float.h>

// TODO: share with other temps
static __bit sign_bit;
static __data unsigned char expf_y[4];
static __data unsigned char n;

float expf(float x)
{
	x;
	__asm
	mov	c, acc.7
	mov	_sign_bit, c	// remember sign
	clr	acc.7		// and make input positive
	mov	r0, a
	mov	c, b.7
	rlc	a		// extract exponent
	add	a, #153
	jc	expf_not_zero
	// input is a very small number, so e^x is 1.000000
	mov	dptr, #0
	mov	b, #0x80
	mov	a, #0x3F
	ljmp	expf_exit
expf_not_zero:
	// TODO: check exponent for very small values, and return zero
	mov	_n, #0
	mov	a, dpl
	add	a, #0xE8	// is x >= 0.69314718
	mov	a, dph
	addc	a, #0x8D
	mov	a, b
	addc	a, #0xCE
	mov	a, r0
	addc	a, #0xC0
	mov	a, r0
	jnc	expf_no_range_reduction
expf_range_reduction:
	mov	(_expf_y + 0), dpl	// keep copy of x in "exp_y"
	mov	(_expf_y + 1), dph
	mov	(_expf_y + 2), b
	mov	(_expf_y + 3), a
	mov     r0, #0x3B
	push    ar0
	mov     r0, #0xAA
	push    ar0
	mov     r0, #0xB8
	push    ar0
	mov     r0, #0x3F
	push    ar0
	lcall	___fsmul	// x * 1.442695041 = x / ln(2)
	dec	sp
	dec	sp
	dec	sp
	dec	sp
	lcall	___fs2uchar	// n = int(x * 1.442695041)
	mov	a, dpl
	mov	_n, a
	add	a, #128
	jnc	expf_range_ok
	lcall	fs_return_inf	// exponent overflow
	ljmp	expf_exit
expf_range_ok:
	mov     r0,#0x00
	mov     r1,#0x80
	mov     r2,#0x31
	mov     r3,#0xBF
	lcall	expf_scale_and_add
	mov	(_expf_y + 0), dpl
	mov	(_expf_y + 1), dph
	mov	(_expf_y + 2), b
	mov	(_expf_y + 3), a
	mov     r0,#0x83
	mov     r1,#0x80
	mov     r2,#0x5E
	mov     r3,#0x39
	lcall	expf_scale_and_add
expf_no_range_reduction:


// Compute e^x using the cordic algorithm.  This works over an
// input range of 0 to 0.69314712.  Can be extended to work from
// 0 to 1.0 if the results are normalized, but over the range
// we use, the result is always from 1.0 to 1.99999988 (fixed
// exponent of 127)

expf_cordic_begin:
	mov	c, b.7
	rlc	a		// extract exponent to acc
	setb	b.7
	mov	r1, dpl		// mantissa to r4/r3/r2/r1
	mov	r2, dph
	mov	r3, b
	mov	r4, #0

	// first, align the input into a 32 bit long
	cjne	a, #121, exp_lshift
	sjmp	exp_noshift
exp_lshift:
	jc	exp_rshift
	// exp_a is greater than 121, so left shift
	add	a, #135
	lcall	fs_lshift_a
	sjmp	exp_noshift
exp_rshift:
	// exp_a is less than 121, so right shift
	cpl	a
	add	a, #122
	lcall	fs_rshift_a
exp_noshift:				// r4/r3/r2/r1 = x
	clr	a
	mov	(_expf_y + 0), a	// y = 1.0;
	mov	(_expf_y + 1), a
	mov	(_expf_y + 2), a
	mov	(_expf_y + 3), #0x20
	mov	dptr, #__fs_natural_log_table
	mov	r0, a			// r0 = i
exp_cordic_loop:
	clr	a
	movc	a, @a+dptr
	mov	b, a
	inc	dptr
	clr	a
	movc	a, @a+dptr		// r7/r6/r5/b = table[i]
	mov	r5, a
	inc	dptr
	clr	a
	movc	a, @a+dptr
	mov	r6, a
	inc	dptr
	clr	a
	movc	a, @a+dptr
	mov	r7, a
	inc	dptr
	clr	c
	mov	a, r1
	subb	a, b			// compare x to table[i]
	mov	a, r2
	subb	a, r5
	mov	a, r3
	subb	a, r6
	mov	a, r4
	subb	a, r7
	jc	exp_cordic_skip		// if table[i] < x
	clr	c
	mov	a, r1
	subb	a, b
	mov	r1, a			// x -= table[i]
	mov	a, r2
	subb	a, r5
	mov	r2, a
	mov	a, r3
	subb	a, r6
	mov	r3, a
	mov	a, r4
	subb	a, r7
	mov	r4, a
	mov	b,  (_expf_y + 0)
	mov	r5, (_expf_y + 1)	// r7/r6/r5/b = y >> i
	mov	r6, (_expf_y + 2)
	mov	r7, (_expf_y + 3)
	mov	a, r0
	lcall	__fs_cordic_rshift_r765_unsigned
	mov	a, (_expf_y + 0)
	add	a, b
	mov	(_expf_y + 0), a
	mov	a, (_expf_y + 1)
	addc	a, r5
	mov	(_expf_y + 1), a	// y += (y >> i)
	mov	a, (_expf_y + 2)
	addc	a, r6
	mov	(_expf_y + 2), a
	mov	a, (_expf_y + 3)
	addc	a, r7
	mov	(_expf_y + 3), a
exp_cordic_skip:
	inc	r0
	cjne	r0, #27, exp_cordic_loop
	mov	r4, (_expf_y + 3)
	mov	r3, (_expf_y + 2)
	mov	r2, (_expf_y + 1)
	mov	r1, (_expf_y + 0)
	mov	exp_a, #121
	lcall	fs_normalize_a		// end of cordic

	mov	a, #127
	add	a, _n			// ldexpf(x, n)
	mov	exp_a, a
	lcall	fs_round_and_return

	jnb	_sign_bit, expf_done
	push	dpl
	push	dph
	push	b
	push	acc
	mov	dptr, #0
	mov	b, #0x80
	mov	a, #0x3F
	lcall	___fsdiv		// 1.0 / x
	dec	sp
	dec	sp
	dec	sp
	dec	sp
expf_done:
	clr	acc.7		// Result is always positive!
expf_exit:
	__endasm;
#pragma less_pedantic
}

static void dummy1(void) __naked
{
	__asm
	.globl	fs_lshift_a
expf_scale_and_add:
	push    ar0
	push    ar1
	push    ar2
	push    ar3
	mov	dpl, _n
	lcall	___uchar2fs	// turn n into float
	lcall	___fsmul	// n * scale_factor
	dec	sp
	dec	sp
	dec	sp
	dec	sp
	push	dpl
	push	dph
	push	b
	push	acc
	mov	dpl, (_expf_y + 0)
	mov	dph, (_expf_y + 1)
	mov	b,   (_expf_y + 2)
	mov	a,   (_expf_y + 3)
	lcall	___fsadd	// x += (n * scale_factor)
	dec	sp
	dec	sp
	dec	sp
	dec	sp
	ret
	__endasm;
}

static void dummy(void) __naked
{
	__asm
	.globl	fs_lshift_a
fs_lshift_a:
	jz	fs_lshift_done
	push	ar0
	mov	r0, a
fs_lshift_loop:
	clr	c
	mov	a, r1
	rlc	a
	mov	r1, a
	mov	a, r2
	rlc	a
	mov	r2, a
	mov	a, r3
	rlc	a
	mov	r3, a
	mov	a, r4
	rlc	a
	mov	r4, a
	djnz	r0, fs_lshift_loop
	pop	ar0
fs_lshift_done:
	ret
	__endasm;
}

#else // not MATH_ASM_MCS51

#define P0      0.2499999995E+0
#define P1      0.4160288626E-2
#define Q0      0.5000000000E+0
#define Q1      0.4998717877E-1

#define P(z) ((P1*z)+P0)
#define Q(z) ((Q1*z)+Q0)

#define C1       0.693359375
#define C2      -2.1219444005469058277e-4

#define BIGX    88.72283911  /* ln(HUGE_VALF) */
#define EXPEPS  1.0E-7       /* exp(1.0E-7)=0.0000001 */
#define K1      1.4426950409 /* 1/ln(2) */

float expf(float x) _FLOAT_FUNC_REENTRANT
{
    int n;
    float xn, g, r, z, y;
    bool sign;

    if(x>=0.0)
        { y=x;  sign=0; }
    else
        { y=-x; sign=1; }

    if(y<EXPEPS) return 1.0;

    if(y>BIGX)
    {
        if(sign)
        {
            errno=ERANGE;
            return HUGE_VALF
            ;
        }
        else
        {
            return 0.0;
        }
    }

    z=y*K1;
    n=z;

    if(n<0) --n;
    if(z-n>=0.5) ++n;
    xn=n;
    g=((y-xn*C1))-xn*C2;
    z=g*g;
    r=P(z)*g;
    r=0.5+(r/(Q(z)-r));

    n++;
    z=ldexpf(r, n);
    if(sign)
        return 1.0/z;
    else
        return z;
}

#endif
