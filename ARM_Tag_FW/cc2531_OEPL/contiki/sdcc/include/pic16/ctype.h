/*-------------------------------------------------------------------------
   ctype.h - ANSI functions forward declarations

   Copyright (C) 1998, Sandeep Dutta . sandeep.dutta@usa.net
   Modified for pic16 port by Vangelis Rokas, 2004, <vrokas AT otenet.gr>

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
/*
   Revisions:
   1.0 - June.1.2000 1.0 - Bela Torok / bela.torok@kssg.ch
   order: function definitions -> macros
   corretced macro: isalpha(c)  
   added macros: _tolower(c), _toupper(c), tolower(c), toupper(c) toascii(c)
*/

#ifndef __CTYPE_H
#define __CTYPE_H 1

/* link the C libarary */
#pragma library c

#include <sdcc-lib.h>

extern char  iscntrl   (unsigned char )  ;
extern char  isdigit   (unsigned char )  ;
extern char  isgraph   (unsigned char )  ;
extern char  islower   (unsigned char )  ;
extern char  isupper   (unsigned char )  ;
extern char  isprint   (unsigned char )  ;
extern char  ispunct   (unsigned char )  ;
extern char  isspace   (unsigned char )  ;
extern char  isxdigit  (unsigned char )  ;

#define isalnum(c)   (isalpha(c) || isdigit(c))
#define isalpha(c)   (isupper(c) || islower(c))

/* ANSI versions of _tolower & _toupper
#define _tolower(c)  ((c) - ('a' - 'A'))
#define _toupper(c)  ((c) + ('a' - 'A'))
*/

// The _tolower & _toupper functions below can applied to any 
// alpha characters regardless of the case (upper or lower)
#define _tolower(c)  ((c) | ('a' - 'A'))
#define _toupper(c)  ((c) & ~('a' - 'A'))

#define tolower(c)  ((isupper(c)) ? _tolower(c) : (c))
#define toupper(c)  ((islower(c)) ? _toupper(c) : (c))
#define toascii(c)  ((c) & 0x7F)

#endif
