/*-------------------------------------------------------------------------
   _rrulonglong.c - routine for right shift of 64 bit unsigned long long

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
// This function is the same as the one from rrulonglong_rrx_s.c, except for the type of top.

#if defined(__SDCC_hc08) || defined(__SDCC_s08) || defined(__SDCC_stm8) // Big-endian

long long _rrslonglong(long long l, char s)
{
	int32_t *top = (uint32_t *)((char *)(&l) + 0);
	uint32_t *middle = (uint16_t *)((char *)(&l) + 2);
	uint32_t *bottom = (uint32_t *)((char *)(&l) + 4);
	uint16_t *b = (uint16_t *)(&l);

	for(;s >= 16; s-= 16)
	{
		b[3] = b[2];
		b[2] = b[1];
		b[1] = b[0];
		b[0] = (b[0] & 0x8000) ? 0xffff : 0x000000;
	}

	(*bottom) >>= s;
	(*middle) |= (((*middle & 0xffff0000ul) >> s) & 0x0000fffful);
	(*top) >>= s;

	return(l);
}

#else // Little-endian

long long _rrslonglong(long long l, char s)
{
	int32_t *top = (uint32_t *)((char *)(&l) + 4);
	uint16_t *middle = (uint16_t *)((char *)(&l) + 3);
	uint32_t *bottom = (uint32_t *)(&l);
	uint16_t *b = (uint16_t *)(&l);

	for(;s >= 16; s-= 16)
	{
		b[0] = b[1];
		b[1] = b[2];
		b[2] = b[3];
		b[3] = (b[3] & 0x8000) ? 0xffff : 0x000000;
	}

	(*bottom) >>= s;
	(*bottom) |= ((uint32_t)((*middle) >> s) << 16);
	(*top) |= (((*middle) & 0xffff0000) >> s);

	return(l);
}
#endif

#endif

