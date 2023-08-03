/*-------------------------------------------------------------------------
   tanhf.c - Computes tanh(x) where x is a 32-bit float as outlined in [1].

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

#define P0 -0.8237728127E+0
#define P1 -0.3831010665E-2
#define Q0  0.2471319654E+1
#define Q1  0.1000000000E+1

/* ln(3)/2 */
#define K1  0.5493061443E+0
/* SBIG=[ln(2)+(t+1)*ln(B)]/2 */
#define SBIG 9.01091

#define P(g) ((P1*g+P0)*g)
#define Q(g) (Q1*g+Q0)

float tanhf(float x) _FLOAT_FUNC_REENTRANT
{
    float f, g, r;

    f=fabsf(x);
    if(f>SBIG) r=1.0;
    else if(f>K1)
    {
        r=0.5-1.0/(expf(f+f)+1.0);
        r+=r;
    }
    else if(f<EPS) r=f;
    else
    {
        g=f*f;
        r=f+f*(P(g)/Q(g));
    }
    if(x<0.0) r=-r;
    return r;
}
