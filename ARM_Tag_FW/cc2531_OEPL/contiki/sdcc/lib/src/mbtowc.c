/*-------------------------------------------------------------------------
   mbtowc.c - convert a multibyte sequence to a wide character

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

#include <stdlib.h>

int mbtowc(wchar_t *pwc, const char *restrict s, size_t n)
{
	wchar_t codepoint;
	unsigned char seqlen, i;
	unsigned char first_byte;

	if(!s)
		return(0);

	seqlen = 1;
	first_byte = *s;

	if(first_byte & 0x80)
	{
		while (first_byte & (0x80 >> seqlen))
			seqlen++;
		first_byte &= (0xff >> (seqlen + 1));
	}

	if(seqlen > 4 || n < seqlen)
		return(-1);

	for(i = 1; i < seqlen; i++)
		if((s[i] & 0xc0) != 0x80)
			return(-1);

	codepoint = first_byte;

	for(s++, i = seqlen - 1; i; i--)
	{
		codepoint <<= 6;
		codepoint |= (*s & 0x3f);
		s++;
	}

	if(codepoint >= 0xd800 && codepoint <= 0xdfff) // UTF-16 surrogate.
		return(-1);

	if(pwc)
		*pwc = codepoint;
	return(codepoint ? seqlen : 0);
}

