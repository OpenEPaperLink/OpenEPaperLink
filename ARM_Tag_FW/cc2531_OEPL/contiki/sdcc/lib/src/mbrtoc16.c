/*-------------------------------------------------------------------------
   mbrtoc32.c - convert UTF-8 to UTF-16

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

#include <limits.h>
#include <wchar.h>

size_t mbrtoc16(char16_t *restrict pc16, const char *restrict s, size_t n, mbstate_t *restrict ps)
{
	char32_t codepoint;
	size_t ret;
	static mbstate_t sps;
	char16_t low_surrogate;

	if(!s)
		return(mbrtoc16(0, "", 1, ps));

	if(!ps)
		ps = &sps;

	if(!ps->c[0] && (ps->c[1] || ps->c[2]))
	{
		if(pc16)
			*pc16 = ps->c[1] + (ps->c[2] << 8);
		ps->c[1] = ps->c[2] = 0;
		return(-3);
	}

	ret = mbrtowc(&codepoint, s, n, ps);

	if(ret > MB_LEN_MAX)
		return(ret);

	if (codepoint <= 0xffff) // Basic multilingual plane
	{
		if(pc16)
			*pc16 = codepoint;
		return(ret);
	}

	codepoint -= 0x100000;
	if(pc16)
		*pc16 = ((codepoint >> 10) & 0x3ff) + 0xd800;
	low_surrogate = (codepoint & 0x3ff) + 0xdc00;
	ps->c[0] = 0;
	ps->c[1] = low_surrogate & 0xff;
	ps->c[2] = low_surrogate >> 8;

	return(ret);
}

