/*-------------------------------------------------------------------------
   mbrtowc.c - convert a multibyte sequence to a wide character

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

#include <wchar.h>
#include <errno.h>

size_t mbrtowc(wchar_t *restrict pwc, const char *restrict s, size_t n, mbstate_t *restrict ps)
{
	unsigned char first_byte;
	unsigned char seqlen;
	char mbseq[4];
	wchar_t codepoint;
	unsigned char i, j;
	static mbstate_t sps;

	if(!s)
		return(mbrtowc(0, "", 1, ps));
	if(!n)
		goto eilseq;
	if(!ps)
	{
		ps = &sps;
	}

	for(i = 0; ps->c[i] && i < 3; i++)
		mbseq[i] = ps->c[i];

	seqlen = 1;
	first_byte = ps->c[0] ? ps->c[0] : *s;

	if(first_byte & 0x80)
	{
		while (first_byte & (0x80 >> seqlen))
			seqlen++;
		first_byte &= (0xff >> (seqlen + 1));
	}

	if(seqlen > 4)
		goto eilseq;

	if(i + n < seqlen) // Incomplete multibyte character
	{
		for(;n-- ; i++)
			ps->c[i] = *s++;
		return(-2);
	}

	for(j = 0; j < i; j++)
		ps->c[j] = 0;

	for(n = 1, i = i ? i : 1; i < seqlen; i++, n++)
	{
		mbseq[i] = *s++;
		if((mbseq[i] & 0xc0) != 0x80)
			goto eilseq;
	}

	codepoint = first_byte;

	for(s = mbseq + 1, seqlen--; seqlen; seqlen--)
	{
		codepoint <<= 6;
		codepoint |= (*s & 0x3f);
		s++;
	}

	if(codepoint >= 0xd800 && codepoint <= 0xdfff) // UTF-16 surrogate.
		return(-1);

	if(pwc)
		*pwc = codepoint;
	return(n);

eilseq:
	errno = EILSEQ;
	return(-1);
}

