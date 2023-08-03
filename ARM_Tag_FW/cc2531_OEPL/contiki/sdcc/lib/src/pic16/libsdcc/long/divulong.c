/*-------------------------------------------------------------------------
   divulong.c - routine for division of 32 bit unsigned long

   Copyright (C) 1999, Jean-Louis Vern <jlvern AT gmail.com>

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

#include <sdcc-lib.h>

#define MSB_SET(x) ((x >> (8*sizeof(x)-1)) & 1)

unsigned long
_divulong (unsigned long a, unsigned long b) _IL_REENTRANT
{
  unsigned long reste = 0L;
  unsigned char count = 32;
  unsigned char c;

  do
  {
    // reste: a <- 0;
    c = MSB_SET(a);
    a <<= 1;
    reste <<= 1;
    if (c)
      reste |= 1L;

    if (reste >= b)
    {
      reste -= b;
      // a <- (result = 1)
      a |= 1L;
    }
  }
  while (--count);
  return a;
}
