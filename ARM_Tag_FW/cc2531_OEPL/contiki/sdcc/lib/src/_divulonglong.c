/*-------------------------------------------------------------------------
   _divslonglong.c - routine for divsion of 64 bit unsigned long long

   Copyright (C) 1999, Jean-Louis Vern <jlvern AT gmail.com>
   Copyright (C) 2012, Philipp Klaus Krause . pkk@spth.de

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

#pragma std_c99

#include <stdint.h>
#include <stdbool.h>

#ifdef __SDCC_LONGLONG
#define MSB_SET(x) ((x >> (8*sizeof(x)-1)) & 1)

unsigned long long
_divulonglong (unsigned long long x, unsigned long long y)
{
  unsigned long long reste = 0L;
  unsigned char count = 64;
  bool c;

  do
  {
    // reste: x <- 0;
    c = MSB_SET(x);
    x <<= 1;
    reste <<= 1;
    if (c)
      reste |= 1L;

    if (reste >= y)
    {
      reste -= y;
      // x <- (result = 1)
      x |= 1L;
    }
  }
  while (--count);
  return x;
}
#endif

