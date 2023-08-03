/*-------------------------------------------------------------------------
   _rlulonglong.c - routine for left shift of 64 bit unsigned long long

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

#if defined(__SDCC_hc08) || defined(__SDCC_s08) || defined(__SDCC_stm8) // Big-endian

unsigned long long _rlulonglong(unsigned long long l, char s)
{
	uint32_t *const top = (uint32_t *)((char *)(&l) + 0);
	uint16_t *const middle = (uint16_t *)((char *)(&l) + 4);
	uint32_t *const bottom = (uint32_t *)((char *)(&l) + 4);
	uint16_t *const b = (uint16_t *)(&l);

	for(;s >= 16; s-= 16)
	{
		b[0] = b[1];
		b[1] = b[2];
		b[2] = b[3];
		b[3] = 0;
	}

	(*top) <<= s;
	(*top) |= (((uint32_t)((*middle) & 0xffffu) << s) >> 16);
	(*bottom) <<= s;

	return(l);
}

#else // Little-endian

unsigned long long _rlulonglong(unsigned long long l, char s)
{
	uint32_t *const top = (uint32_t *)((char *)(&l) + 4);
	uint16_t *const middle = (uint16_t *)((char *)(&l) + 2);
	uint32_t *const bottom = (uint32_t *)(&l);
	uint16_t *const b = (uint16_t *)(&l);

	for(;s >= 16; s-= 16)
	{
		b[3] = b[2];
		b[2] = b[1];
		b[1] = b[0];
		b[0] = 0;
	}

	(*top) <<= s;
	(*top) |= (((uint32_t)((*middle) & 0xffffu) << s) >> 16);
	(*bottom) <<= s;

	return(l);
}

#endif

#endif

