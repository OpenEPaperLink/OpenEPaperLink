/*-------------------------------------------------------------------------
   wctomb.c - convert a wide character to a multibyte sequence

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

int wctomb(char *s, wchar_t wc)
{
	if(!s)
		return(0);

	if(wc < 0x80)
	{
		s[0] = wc;
		return(1);
	}
	else if(wc < 0x800)
	{
		s[0] = (wc >> 6) & 0x1f | 0xc0;
		s[1] = (wc >> 0) & 0x3f | 0x80;
		return(2);
	}
	else if(wc >= 0xd800 && wc < 0xe000) // Unpaired surrogate
		return(-1);
	else if(wc < 0x10000)
	{
		s[0] = (wc >> 12) & 0x0f | 0xe0;
		s[1] = (wc >> 6) & 0x3f  | 0x80;
		s[2] = (wc >> 0) & 0x3f  | 0x80;
		return(3);
	}
	else if(wc < 0x110000)
	{
		s[0] = (wc >> 18) & 0x07 | 0xf0;
		s[1] = (wc >> 12) & 0x3f | 0x80;
		s[2] = (wc >> 6) & 0x3f  | 0x80;
		s[3] = (wc >> 0) & 0x3f  | 0x80;
		return(4);
	}
	else
		return(-1);
}

