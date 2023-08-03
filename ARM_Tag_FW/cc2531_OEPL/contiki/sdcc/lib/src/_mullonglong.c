/*-------------------------------------------------------------------------
   _mullonglong.c - routine for multiplication of 64 bit long long

   Copyright (C) 2012, Philipp Klaus Krause . philipp@informatik.uni-frankfurt.de

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

#ifdef __SDCC_LONGLONG

long long _mullonglong(long long ll, long long lr)
{
  unsigned long long ret = 0ull;
  unsigned char i, j;

  for (i = 0; i < sizeof (long long); i++)
    {
      unsigned char l = ll >> (i * 8);
      for(j = 0; (i + j) < sizeof (long long); j++)
        {
          unsigned char r = lr >> (j * 8);
          ret += (unsigned long long)((unsigned short)(l * r)) << ((i + j) * 8);
        }
    }

  return(ret);
}

#endif

