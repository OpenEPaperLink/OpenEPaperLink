/*-------------------------------------------------------------------------
   c32rtomb.c - convert UTF-16 to UTF-8

   Copyright (C) 2016, Philipp Klaus Krause, pkk@spth.de

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

#include <uchar.h>

#include <stdlib.h>
#include <limits.h>
#include <errno.h>

size_t c16rtomb(char *restrict s, char16_t c16, mbstate_t *restrict ps)
{
	wchar_t codepoint;
	char buf[MB_LEN_MAX];
	static mbstate_t sps;

	if(!s)
		s = buf;

	if(!ps)
		ps = &sps;

	if (!c16) // 0 always resets conversion state.
	{
		ps->c[1] = ps->c[2] = 0;
		codepoint = 0;
	}
	else if(ps->c[1] || ps->c[2]) // We already have the high surrogate. Now get the low surrogate
	{
		char16_t high_surrogate;

		if(c16 < 0xdc00 || c16 > 0xdfff)
			goto eilseq;

		high_surrogate = ps->c[1] + (ps->c[2] << 8);
		ps->c[1] = ps->c[2] = 0;
		codepoint = (high_surrogate << 10) - (0xd800 << 10) + c16 - 0xdc00 + 0x10000;
	}
	else if(c16 < 0xd7ff || c16 >= 0xe000) // Basic multilingual plane.
		codepoint = c16;
	else // Get the high surrogate
	{
		if(c16 > 0xdbff)
			goto eilseq;

		ps->c[1] = c16 & 0xff;
		ps->c[2] = c16 >> 8;
		return(0);
	}

	return(wctomb(s, codepoint));

eilseq:
	errno = EILSEQ;
	return(-1);
}

