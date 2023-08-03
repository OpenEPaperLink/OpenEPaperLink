/*-------------------------------------------------------------------------
   sincoshf.c - Computes sinh or cosh of a 32-bit float as outlined in [1]

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
#include <stdbool.h>

#define P0 -0.713793159E+1
#define P1 -0.190333999E+0
#define Q0 -0.428277109E+2
#define Q1  0.100000000E+1

#define P(z) (P1*z+P0)
#define Q(z) (Q1*z+Q0)

#define K1 0.69316101074218750000E+0 /* ln(v)   */
#define K2 0.24999308500451499336E+0 /* v**(-2) */
#define K3 0.13830277879601902638E-4 /* v/2-1   */

//WMAX is defined as ln(HUGE_VALF)-ln(v)+0.69
#define WMAX 44.93535952E+0
//WBAR 0.35*(b+1)
#define WBAR 1.05
#define YBAR 9.0 /*Works for me*/

float sincoshf(float x, bool iscosh)
{
    float y, w, z;
    bool sign;

    if (x<0.0) { y=-x; sign=1; }
          else { y=x;  sign=0; }

    if ((y>1.0) || iscosh)
    {
        if(y>YBAR)
        {
            w=y-K1;
            if (w>WMAX)
            {
                errno=ERANGE;
                z=HUGE_VALF;
            }
            else
            {
                z=expf(w);
                z+=K3*z;
            }
        }
        else
        {
            z=expf(y);
            w=1.0/z;
            if(!iscosh) w=-w;
            z=(z+w)*0.5;
        }
        if(sign) z=-z;
    }
    else
    {
        if (y<EPS)
            z=x;
        else
        {
            z=x*x;
            z=x+x*z*P(z)/Q(z);
        }
    }
    return z;
}
