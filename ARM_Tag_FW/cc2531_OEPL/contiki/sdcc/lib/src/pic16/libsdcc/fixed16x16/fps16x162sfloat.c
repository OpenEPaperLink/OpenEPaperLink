/*-------------------------------------------------------------------------
   fps16x162sfloat.c

   Copyright (C) 2005, Raphael Neider <rneider at web.de> 

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

float __fps16x162sfloat(__fixed16x16 fixd)
{
  union {
    __fixed16x16 fix;
    unsigned long value;
  } u;
  float tmp=0, exp=2;

    u.fix = fixd;
    tmp = (u.value & 0xffff0000) >> 16;

    while(u.value) {
      u.value &= 0xffff;
      if(u.value & 0x8000)tmp += 1/exp;
      exp *= 2;
      u.value <<= 1;
    }

  return (tmp);
}
