/*-------------------------------------------------------------------------
   sincosf.c - Computes sin or cos of a 32-bit float as outlined in [1]

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

#define r1      (-0.1666665668E+0)
#define r2      (0.8333025139E-2)
#define r3      (-0.1980741872E-3)
#define r4       (0.2601903036E-5)

/* PI=C1+C2 */
#define C1       3.140625
#define C2       9.676535897E-4

/*A reasonable value for YMAX is the int part of PI*B**(t/2)=3.1416*2**(12)*/
#define YMAX     12867.0

float sincosf(float x, int iscos)
{
    float y, f, r, g, XN;
    int N;
    char sign;

    if(iscos)
    {
        y=fabsf(x)+HALF_PI;
        sign=0;
    }
    else
    {
        if(x<0.0)
            { y=-x; sign=1; }
        else
            { y=x; sign=0; }
    }

    if(y>YMAX)
    {
        errno=ERANGE;
        return 0.0;
    }

    /*Round y/PI to the nearest integer*/
    N=((y*iPI)+0.5); /*y is positive*/

    /*If N is odd change sign*/
    if(N&1) sign=!sign;

    XN=N;
    /*Cosine required? (is done here to keep accuracy)*/
    if(iscos) XN-=0.5;

    y=fabsf(x);
    r=(int)y;
    g=y-r;
    f=((r-XN*C1)+g)-XN*C2;

    g=f*f;
    if(g>EPS2) //Used to be if(fabsf(f)>EPS)
    {
        r=(((r4*g+r3)*g+r2)*g+r1)*g;
        f+=f*r;
    }
    return (sign?-f:f);
}
