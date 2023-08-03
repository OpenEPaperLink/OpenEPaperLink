/*-------------------------------------------------------------------------
   logf.c: Computes the natural log of a 32 bit float as outlined in [1].

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

/*Constans for 24 bits or less (8 decimal digits)*/
#define A0 -0.5527074855E+0
#define B0 -0.6632718214E+1
#define A(w) (A0)
#define B(w) (w+B0)

#define C0  0.70710678118654752440
#define C1  0.693359375 /*355.0/512.0*/
#define C2 -2.121944400546905827679E-4

float logf(float x) _MATH_REENTRANT
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
