/*-------------------------------------------------------------------------
   _modslonglong.c - routine for modulo of 64 bit unsigned long long

   Copyright (C) 2014, Philipp Klaus Krause . pkk@spth.de

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
long long 
_modslonglong (long long numerator, long long denominator)
{
  bool numeratorneg = (numerator < 0);
  bool denominatorneg = (denominator < 0);
  long long r;

  if (numeratorneg)
    numerator = -numerator;
  if (denominatorneg)
    denominator = -denominator;

  r = (unsigned long long)numerator % (unsigned long long)denominator;

  return (numeratorneg ? -r : r);
}
#endif

