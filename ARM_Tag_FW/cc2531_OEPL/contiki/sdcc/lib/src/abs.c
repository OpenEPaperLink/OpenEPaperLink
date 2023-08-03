/*-------------------------------------------------------------------------
   abs.c - computes absolute value of an integer.

   Copyright (C) 2004, Maarten Brock, sourceforge.brock@dse.nl

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

#if defined (__SDCC_mcs51)

#if defined(__SDCC)
 #include <sdcc-lib.h>
#endif

static void dummy(void) __naked
{
	__asm
	.globl	_abs
_abs:
	mov	a, dph
	jnb	acc.7, 00001$
	xrl	dpl,#0xFF
	xrl	dph,#0xFF
	inc	dptr
00001$:
	_RETURN
	__endasm;
}

#else

int abs(int j)
{
	return (j < 0) ? -j : j;
}

#endif

//END OF MODULE
