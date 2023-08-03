/*-------------------------------------------------------------------------
   tancotf.c - Computes tan or cot of a 32-bit float as outlined in [1]

   Copyright (C) 2001, 2002, Jesus Calvino-Fraga <jesusc At ieee.org>

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

/* [1] William James Cody and W.  M.  Waite.  _Software manual for the
   elementary functions_, Englewood Cliffs, N.J.:Prentice-Hall, 1980. */

#include <math.h>
#include <errno.h>

#define P0  0.100000000E+1
#define P1 -0.958017723E-1
#define Q0  0.100000000E+1
#define Q1 -0.429135777E+0
#define Q2  0.971685835E-2

#define C1  1.5703125
#define C2  4.83826794897E-4

#define P(f,g) (P1*g*f+f)
#define Q(g) ((Q2*g+Q1)*g+Q0)

//A reasonable choice for YMAX is the integer part of B**(t/2)*PI/2:
#define YMAX 6433.0

float tancotf(float x, int iscotan)
{
    float f, g, xn, xnum, xden;
    int n;

    if (fabsf(x) > YMAX)
    {
        errno = ERANGE;
        return 0.0;
    }

    /*Round x*2*PI to the nearest integer*/
    n=(x*TWO_O_PI+(x>0.0?0.5:-0.5)); /*works for +-x*/
    xn=n;

    xnum=(int)x;
    xden=x-xnum;
    f=((xnum-xn*C1)+xden)-xn*C2;

    if (fabsf(f) < EPS)
    {
        xnum = f;
        xden = 1.0;
    }
    else
    {
        g = f*f;
        xnum = P(f,g);
        xden = Q(g);
    }

    if(n&1)
    //xn is odd
    {
        if(iscotan) return (-xnum/xden);
               else return (-xden/xnum);
    }
    else
    {
        if(iscotan) return (xden/xnum);
               else return (xnum/xden);
    }
}
