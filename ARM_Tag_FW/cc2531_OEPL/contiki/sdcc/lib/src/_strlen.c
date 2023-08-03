/*-------------------------------------------------------------------------
   _strlen.c - part of string library functions
 
   Copyright (C) 1999, Sandeep Dutta . sandeep.dutta@usa.net
   mcs51 assembler by Frieder Ferlemann (2007)

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

#include <string.h>

#if (!defined (__SDCC_mcs51))

  /* Generic routine first */
  size_t strlen ( const char * str )
  {
    register int i = 0 ;

    while (*str++)
      i++ ;

    return i;
  }

#else

#if defined(__SDCC)
 #include <sdcc-lib.h>
#endif

  /* Assembler version for mcs51 */
  size_t strlen ( const char * str ) __naked
  {
    str;     /* hush the compiler */

    __asm
      ; dptr holds pointer
      ; b holds pointer memspace
      ;

      ; char *ptr = str:
      mov     r2,dpl
      mov     r3,dph
      ;

      ; while ( *ptr ) ptr++;
    L00101$:
      lcall   __gptrget
      jz      L00102$
      inc     dptr
      sjmp    L00101$
      ;

    L00102$:
      ; return ptr - str;
      clr     c
      mov     a,dpl
      subb    a,r2
      mov     dpl,a
      ;
      mov     a,dph
      subb    a,r3
      mov     dph,a
      ;
      _RETURN
    __endasm;
  }

#endif
