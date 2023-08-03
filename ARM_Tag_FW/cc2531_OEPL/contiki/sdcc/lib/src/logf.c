/*-------------------------------------------------------------------------
   logf.c - Computes the natural log of a 32 bit float as outlined in [1].

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


#ifdef MATH_ASM_MCS51

#define __SDCC_FLOAT_LIB
#include <float.h>

// TODO: share with other temps
static __data unsigned char logf_tmp[4];

float logf(float x)
{
	x;
	__asm

	// extract the two input, placing it into:
	//      sign     exponent   mantiassa
	//      ----     --------   ---------
	//  x:  sign_a   exp_a     r4/r3/r2

	lcall	fsgetarg
logf_neg_check:
	jnb	sign_a, logf_zero_check
	// TODO: set errno to EDOM (negative numbers not allowed)
	lcall	fs_return_nan
	ljmp	logf_exit

logf_zero_check:
	cjne	r4, #0, logf_ok
	// TODO: set errno to ERANGE (zero not allowed)
	setb	sign_a
	lcall	fs_return_inf
	ljmp	logf_exit

logf_ok:
	push	exp_a
	mov	a, #3
	mov	r1, #0
	lcall	fs_rshift_a

	clr	a
	mov	(_logf_tmp + 0), a	// y = 0
	mov	(_logf_tmp + 1), a
	mov	(_logf_tmp + 2), a
	mov	(_logf_tmp + 3), a
	mov	dptr, #__fs_natural_log_table
	mov	r0, a
logf_cordic_loop:
	mov	ar7, r4		// r7/r6/r5 = x >> i
	mov	ar6, r3
	mov	ar5, r2
	mov	b, r1
	mov	a, r0
	lcall	__fs_cordic_rshift_r765_unsigned
	mov	a, r1		// check if x + (x >> i) > 1.0
	add	a, b
	mov	a, r2
	addc	a, r5
	mov	a, r3
	addc	a, r6
	mov	a, r4
	addc	a, r7
	anl	a, #0xE0
	jnz	logf_cordic_skip
	mov	a, r1		// x = x + (x >> i)
	add	a, b
	mov	r1, a
	mov	a, r2
	addc	a, r5
	mov	r2, a
	mov	a, r3
	addc	a, r6
	mov	r3, a
	mov	a, r4
	addc	a, r7
	mov	r4, a
	clr	a		// y = y + log_table[i]
	movc	a, @a+dptr
	add	a, (_logf_tmp + 0)
	mov	(_logf_tmp + 0), a
	mov	a, #1
	movc	a, @a+dptr
	addc	a, (_logf_tmp + 1)
	mov	(_logf_tmp + 1), a
	mov	a, #2
	movc	a, @a+dptr
	addc	a, (_logf_tmp + 2)
	mov	(_logf_tmp + 2), a
	mov	a, #3
	movc	a, @a+dptr
	addc	a, (_logf_tmp + 3)
	mov	(_logf_tmp + 3), a
logf_cordic_skip:
	inc	dptr
	inc	dptr
	inc	dptr
	inc	dptr
	inc	r0
	cjne	r0, #30, logf_cordic_loop
	// at this point, _logf_tmp has the natural log of the positive
	// normalized fractional part (0.5 to 1.0 -> 0.693 to 0.0)
	mov	r4, (_logf_tmp + 3)
	mov	r3, (_logf_tmp + 2)
	mov	r2, (_logf_tmp + 1)
	mov	r1, (_logf_tmp + 0)
	mov	exp_a, #129
	setb	sign_a
	lcall	fs_normalize_a
	pop	acc
	cjne	a, #126, logf_exponent
	// if the input exponent was 126, then we don't need to add
	// anything and we can just return the with we have already

	// TODO: which of these gives best accuracy???
	lcall	fs_zerocheck_return
	//lcall	fs_round_and_return
	sjmp	logf_exit
logf_exponent:
	jc	logf_exp_neg
	// the input exponent was greater than 126
logf_exp_pos:
	add	a, #130
	clr	sign_b
	sjmp	logf_exp_scale
logf_exp_neg:
	// the input exponent was less than 126
	cpl	a
	add	a, #127
	setb	sign_b
logf_exp_scale:
	// r0 has abs(exp - 126)
	mov	r0, a
	// put the log of faction into b, so we can use a to compute
	// the offset to be added to it or subtracted from it
	lcall	fs_swap_a_b
	// multiply r0 by log(2), or r0 * 0xB17218
	mov	a, #0x18
	mov	b, r0
	mul	ab
	mov	r1, a
	mov	r2, b
	mov	a, #0xB1
	mov	b, r0
	mul	ab
	mov	r3, a
	mov	r4, b
	mov	a, #0x72
	mov	b, r0
	mul	ab
	add	a, r2
	mov	r2, a
	mov	a, b
	addc	a, r3
	mov	r3, a
	clr	a
	addc	a, r4
	mov	r4, a
	// turn r0 * log(2) into a proper float
	mov	exp_a, #134
	lcall	fs_normalize_a
	// now just add log(fractional) +/- log(2) * abs(exp - 126)
	lcall	fsadd_direct_entry
logf_exit:
	__endasm;
#pragma less_pedantic
}

#else // not MATH_ASM_MCS51

/*Constants for 24 bits or less (8 decimal digits)*/
#define A0 -0.5527074855E+0
#define B0 -0.6632718214E+1
#define A(w) (A0)
#define B(w) (w+B0)

#define C0  0.70710678118654752440
#define C1  0.693359375 /*355.0/512.0*/
#define C2 -2.121944400546905827679E-4

float logf(float x) _FLOAT_FUNC_REENTRANT
{
#if     defined(__SDCC_mcs51) && defined(__SDCC_MODEL_SMALL) \
    && !defined(__SDCC_NOOVERLAY)
    volatile
#endif
    float Rz;
    float f, z, w, znum, zden, xn;
    int n;

    if (x<=0.0)
    {
        errno=EDOM;
        return 0.0;
    }
    f=frexpf(x, &n);
    znum=f-0.5;
    if (f>C0)
    {
        znum-=0.5;
        zden=(f*0.5)+0.5;
    }
    else
    {
        n--;
        zden=znum*0.5+0.5;
    }
    z=znum/zden;
    w=z*z;

    Rz=z+z*(w*A(w)/B(w));
    xn=n;
    return ((xn*C2+Rz)+xn*C1);
}

#endif
