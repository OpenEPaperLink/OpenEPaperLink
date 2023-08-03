/*-------------------------------------------------------------------------
   x_ftoa.c - wrapper function to use _convert_float

   Copyright (C) 2004, Vangelis Rokas <vrokas at otenet.gr>

   Modifications for PIC14 by
   Copyright (C) 2019 Gonzalo Pérez de Olaguer Córdoba <salo@gpoc.es>

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
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
#include <stdlib.h>

union {
    unsigned long l;
    float f;
} lf;
int e2;
int e10;

#define M       lf.l    /* mantissa */
#define F       lf.f    /* float */
#define E2      e2      /* exponent base 2 */
#define E10     e10     /* exponent base 10 */

/* While processing decimal output we consider the mantissa as a
 * fixed point number with the integer part in bits 31:28 and the
 * fractional part in bits 27:0.
 */
#define MANT_UNIT       (1UL << 28)

static char *S; /* the output string */

static void _div2 (void)
{
    M = ((M + 1) >> 1);
}

static void _mul2 (void)
{
    M = (M << 1);
}

static void _div5 (void)
{
    M = ((M + 2) / 5);
}

static void _mul5 (void)
{
    M = (M + (M << 2));
}

static void _div10 (void)
{
    M = ((M + 5) / 10);
}

static void _mul10 (void)
{
    M = ((M + (M << 2)) << 1);
}

static void _round (int x)
{
    if (x >= 0)
    {
        unsigned long r = M;
        M = (MANT_UNIT >> 1);
        while (x > 0)
        {
            _div10();
            --x;
        }
        M += r;
        if (M >= 10 * MANT_UNIT)
        {
            _div10();
            ++E10;
        }
    }
}

static void _put_char (char c)
{
    *S++ = c;
}

static void _put_digit (void)
{
    _put_char( '0' + ((M >> 28) & 0x0f));
    M &= 0x0fffffffUL;
    _mul10();
}

void _ftoa (float value, char *str, unsigned char prec)
{
    F = value;
    S = str;
 
    /* output sign */
    if (SIGN(M))
        _put_char('-');
 
    E2 = EXP(M);
    M = MANT(M);
 
    /* infinity or not a number */
    if (E2 == 255)
    {
        if (M == HIDDEN)
        {
            /* infinity */
            _put_char('i');
            _put_char('n');
            _put_char('F');
        }
        else
        {
            /* not a number */
            _put_char('n');
            _put_char('a');
            _put_char('n');
        }
        _put_char('\0');
        return;
    }

    /* zero or denormal number */
    if (E2 == 0)
    {
        if (M == HIDDEN)
        {
            /* zero */
            _put_char('0');
            E10 = prec & PREC_P;
            if (E10)
            {
                _put_char('.');
                while (E10--)
                    _put_char('0');
            }
            if (prec & PREC_E)
            {
                _put_char('E2');
                _put_char('+');
                _put_char('0');
                _put_char('0');
            }
            _put_char('\0');
            return;
        }
        else
        {
            /* denormal number: value = (M - HIDDEN) * pow (2, - 149) */
            M &= ~HIDDEN;
            E2 = 1;
            while (M < HIDDEN)
            {
                 _mul2();
                 --E2;
            }
        }
    }

    /* Now we have a normalized number with value = M * pow (2, E2 - 150)
     * where (1 << 23) <= M < (1 << 24) and -22 <= E2 < 255
     *
     * We adjust it to make M a base2 fixed point value with the implicit
     * decimal point at bit 28 (so bits 31:28 are the integer part and
     * bits 27:0 are the fractional part) and E2 is the unbiased base2
     * exponent.
     *
     * Then we convert it to M * pow (10, E10)
     */
    M <<= (28-23);      /* now MANT_UNIT <= M < 2 * MANT_UNIT */
    E2 -= EXCESS + 1;       /* now -149 <= E2 < 128 */
    E10 = 0;        /* base 10 exponent */
    while (E2 > 0)
    {
        if (M < 5 * MANT_UNIT)
        {
            _mul2();
        }
        else
        {
            _div5();
            ++E10;
        }
        --E2;
    }
    while (E2 < 0)
    {
        if (M < 2 * MANT_UNIT)
        {
            _mul5();
            --E10;
        }
        else
        {
            _div2();
        }
        ++E2;
    }
    while (M < MANT_UNIT)
    {
        _mul10();
        --E10;
    }
    /* Now we have a number with value = M * pow (10, E10)
     * where 1 <= M < 10 with fixed point after bit 28.
     */
    /* Format G: use format E10 if exp < -4 or exp >= prec; format F otherwise. */
    if ((prec & (PREC_E|PREC_F)) == 0)
    {
        if (E10 < -4 || E10 >= prec)
            prec |= PREC_E;
        else
            prec |= PREC_F;
    }
    /* Format E: [-]d.dddE+-dd rounded to 'prec' fractional digits (prec >= 0) */
    if (prec & PREC_E)
    {
        _round(prec & PREC_P);
        _put_digit();
        E2 = prec & PREC_P;
        if (E2 > 0)
        {
            _put_char('.');
            while (E2 > 0)
            {
                _put_digit();
                --E2;
            }
        }
        _put_char('E');
        if (E10 < 0)
        {
            _put_char('-');
            E10 = - E10;
        }
        else
        {
            _put_char('+');
        }
        _uitoa (E10, S, 10);
    }
    /* Format F: [-]ddd.ddd rounded to 'prec' fractional digits (prec >= 0) */
    else
    {
        _round(E10 + (prec & PREC_P));
        /* Print the integer part */
        if (E10 >= 0)
        {
            while (E10 >= 0)
            {
                _put_digit();
                --E10;
            }
        }
        else
        {
            _put_char('0');
        }
        E2 = prec & PREC_P;
        if (E2 > 0)
        {
            /* Print the fractional part */
            _put_char('.');
            while (E2 > 0)
            {
                if (E10 < -1)
                {
                    _put_char('0');
                    ++E10;
                }
                else
                {
                    _put_digit();
                }
                --E2;
            }
        }
        _put_char('\0');
    }
}
