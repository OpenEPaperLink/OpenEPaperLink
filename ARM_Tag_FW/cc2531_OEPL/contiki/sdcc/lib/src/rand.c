/*-------------------------------------------------------------------------
   rand.c - random number generator

   Copyright (C) 2017, Philipp Klaus Krause, pkk@spth.de

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

   This is an xorshift PRNG. See George Marsaglia, "Xorshift RNGs" for details.
   The width of 32 bits was chosen to not regress in period length vs. the
   PRNG from the C standard, while at the same time minimizing RAM usage.
   The parameters a, b, c were chosen to allow the generation of efficient code.
-------------------------------------------------------------------------*/

#include <stdlib.h>

#include <stdint.h>

static uint32_t s = 0x80000001;

int rand(void)
{
	register unsigned long t = s;

	t ^= t >> 10;
	t ^= t << 9;
	t ^= t >> 25;

	s = t;

	return(t & RAND_MAX);
}

void srand(unsigned int seed)
{
    s = seed | 0x80000000; /* s shall not become 0 */
}

