/*-------------------------------------------------------------------------
   mbstoc16s.c - convert a multibyte string to a wide character string

   Copyright (C) 2018, Philipp Klaus Krause, krauseph@informatik.uni-freiburg.de

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
#include <wchar.h>

#ifndef __STDC_UTF_16__
#error Encoding for char16_t strings must be UTF-16
#endif
#ifndef __STDC_ISO_10646__
#error Encoding for wchar_t strings must be UCS-4
#endif

// This implementation assumes that mbtowc() does not keep internal state
// and that mbtowc() does not read beyond a terminating 0.
size_t __mbstoc16s(char16_t *restrict c16s, const char *restrict s, size_t n)
{
	size_t m = 0;

	for(;;)
	{
		int l;
		wchar_t codepoint;

		l = mbtowc(&codepoint, s, MB_LEN_MAX);

		if(l < 0)
			return(-1);
		else if(!l)
		{
			if(m < n)
				*c16s = 0;
			break;
		}

		if (codepoint <= 0xffff) // Basic multilingual plane
		{
			if (m >= n)
				break;

			*c16s++ = codepoint;
			m++;
		}
		else
		{
			if (m + 1 >= n)
				break;

			codepoint -= 0x100000;
			*c16s++ = ((codepoint >> 10) & 0x3ff) + 0xd800;
			*c16s++ = (codepoint & 0x3ff) + 0xdc00;
			m += 2;
		}
		s += l;
	}
	
	return(m);
}

