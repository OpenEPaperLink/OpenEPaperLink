/*-------------------------------------------------------------------------
   wctomb.c - convert a wide character string to a multibyte string

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

#include <limits.h>

size_t wcstombs(char *restrict s, const wchar_t *restrict pwcs, size_t n)
{
	size_t m = 0;
	char buffer[MB_LEN_MAX];

	while(n > MB_LEN_MAX || n >= wctomb(buffer, *pwcs))
	{
		int b = wctomb(s, *pwcs);

		if(b == 1 && !*s)
			break;
		if(b < 0)
			return(-1);

		n -= b;
		m += b;
		s += b;
		pwcs++;
	}

	return(m);
}

