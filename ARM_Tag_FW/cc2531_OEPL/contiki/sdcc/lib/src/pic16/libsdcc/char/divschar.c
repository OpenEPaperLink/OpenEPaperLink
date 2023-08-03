/*-------------------------------------------------------------------------
   _divschar.c - routine for signed char (8 bit) division. just calls
                 routine for unsigned division after sign adjustment

   Copyright (C) 1999, Sandeep Dutta <sandeep.dutta AT usa.net> 
   Adopted for char (8-bit) and pic16 port by
     Vangelis Rokas, <vrokas AT otenet.gr> (2004)

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
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

#include <sdcc-lib.h>

unsigned char _divuchar (unsigned char a, unsigned char b);

int _divschar (signed char a, signed char b) _IL_REENTRANT
{
  register unsigned char r;
  char ta, tb;

    if(a<0)ta = -a; else ta = a;
    if(b<0)tb = -b; else tb = b;
	
    r = _divuchar(ta, tb);
    
    if ((a < 0) ^ (b < 0)) return -r;
    else return r;
}

