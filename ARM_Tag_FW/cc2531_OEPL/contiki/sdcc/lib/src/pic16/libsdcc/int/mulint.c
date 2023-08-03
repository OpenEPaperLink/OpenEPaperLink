/*-------------------------------------------------------------------------
   mulint.c - routine for (unsigned) int (16 bit) multiplication

   Copyright (C) 1999, Sandeep Dutta <sandeep.dutta at usa.net>
   Modified for pic16 port by - Vangelis Rokas, <vrokas at otenet.gr> (2004)

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

/* Signed and unsigned multiplication are the same - as long as the output
   has the same precision as the input.
*/

#include <sdcc-lib.h>

union uu {
	struct { unsigned char lo,hi ;} s;
        unsigned int t;
} ;

int _mulint (int a, int b) _IL_REENTRANT
{
  union uu x;
  union uu y;
  union uu t;

    x.t = a;
    y.t = b;
    t.t = x.s.lo * y.s.lo;
    t.s.hi += (x.s.lo * y.s.hi) + (x.s.hi * y.s.lo);

  return t.t;
}
