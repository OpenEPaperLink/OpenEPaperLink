/*-------------------------------------------------------------------------
   c16stombs.c - convert a wide character string to a multibyte string

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
#include <string.h>
#include <limits.h>
#include <wchar.h>

#ifndef __STDC_UTF_16__
#error Encoding for char16_t strings must be UTF-16
#endif
#ifndef __STDC_ISO_10646__
#error Encoding for wchar_t strings must be UCS-4
#endif

// This implementation assumes that wctomb() does not keep internal state.
size_t __c16stombs(char *restrict s, const char16_t *restrict c16s, size_t n)
{
	size_t m = 0;
	char b[MB_LEN_MAX];

	for(;;)
	{
		int l;
		wchar_t codepoint;

		if(c16s[0] < 0xd800 || c16s[0] >= 0xe000) // Basic multilingual plane
		{
			codepoint = c16s[0];
			c16s++;
		}
                else if(c16s[0] > 0xdbff || c16s[1] < 0xdc00 || c16s[1] > 0xdfff) // Unpaired surrogate
			return(-1);
		else
		{
			codepoint = ((unsigned long)(c16s[0]) << 10) -(0xd800ul << 10) + (unsigned long)(c16s[1]) - 0xdc00ul + 0x10000ul;
			c16s += 2;
		}

		if(!codepoint)
		{
			if(m < n)
				*s = 0;
			break;
		}

		l = wctomb(b, codepoint);

		if(l < 0)
			return(-1);

		if(m + l > n)
			break;

		memcpy(s, b, l);
		s += l;
		m += l;
	}

	return(m);
}

