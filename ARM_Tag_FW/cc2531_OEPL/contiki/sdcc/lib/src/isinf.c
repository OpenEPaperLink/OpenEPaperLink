/*-------------------------------------------------------------------------
   isinf.c - Check if a float value is an infinite (positive or negative)

   Copyright (C) 2016, Ben Shi, powerstudio1st@163.com

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

#define __SDCC_FLOAT_LIB
#include <float.h>
#include <math.h>

int isinf (float f)
{
  unsigned long *pl = (unsigned long *) &f;
  return *pl == 0x7f800000 || *pl == 0xff800000;
}
