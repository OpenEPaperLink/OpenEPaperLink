/*-------------------------------------------------------------------------
   atanf.c - Computes arctan of a 32-bit float as outlined in [1]

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

#include <math.h>
#include <errno.h>

#define P0 -0.4708325141E+0
#define P1 -0.5090958253E-1
#define Q0  0.1412500740E+1
#define Q1  0.1000000000E+1

#define P(g,f) ((P1*g+P0)*g*f)
#define Q(g) (Q1*g+Q0)

#define K1  0.2679491924 /* 2-sqrt(3) */
#define K2  0.7320508076 /* sqrt(3)-1 */
#define K3  1.7320508076 /* sqrt(3)   */

#ifdef __SDCC_mcs51
   #define myconst __code
#else
   #define myconst const
#endif

float atanf(float x) _FLOAT_FUNC_REENTRANT
{
    float f, r, g;
    int n=0;
	static myconst float a[]={  0.0, 0.5235987756, 1.5707963268, 1.0471975512 };

    f=fabsf(x);
    if(f>1.0)
    {
        f=1.0/f;
        n=2;
    }
    if(f>K1)
    {
        f=((K2*f-1.0)+f)/(K3+f);
        // What it is actually wanted is this more accurate formula,
        // but SDCC optimizes it and then it does not work:
        // f=(((K2*f-0.5)-0.5)+f)/(K3+f);
        n++;
    }
    if(fabsf(f)<EPS) r=f;
    else
    {
        g=f*f;
        r=f+P(g,f)/Q(g);
    }
    if(n>1) r=-r;
    r+=a[n];
    if(x<0.0) r=-r;
    return r;
}
