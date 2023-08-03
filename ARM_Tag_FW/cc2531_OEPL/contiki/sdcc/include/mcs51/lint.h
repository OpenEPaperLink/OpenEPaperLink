/*-----------------------------------------------------------------------------
   lint.h - Include file to allow parsing mcs51 specific code with syntax
   checking tools 

   Copyright (c) 2005, Dr. Frieder Ferlemann <Frieder.Ferlemann AT web.de>

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

#ifndef _LINT_H
#define _LINT_H

  #if !defined(__SDCC_mcs51)

    #define __data
    #define __near
    #define __idata
    #define __xdata
    #define __far
    #define __pdata
    #define __code
    #define __bit bool
    #define __sfr volatile unsigned char
    #define __sbit volatile bool
    #define __critical
    #define __at(x)             /* use "__at (0xab)" instead of "__at 0xab" */
    #define __using(x)
    #define __interrupt(x)
    #define __naked

    #define data
    #define near 
    #define idata
    #define xdata
    #define far
    #define pdata
    #define code
    #define bit bool
    #define sfr volatile unsigned char
    #define sbit volatile bool
    #define critical
    #define at(x)
    #define using(x)
    #define interrupt(x)
    #define naked

    /* The tool Splint is available at http://www.splint.org
       Other tools might also be used for statically checking c-sources.
       Traditionally they could have "lint" in their name.
     */
    #if defined(S_SPLINT_S)

      /* Behaviour of splint can be modified by special comments.
         Some examples are shown below.

         Note 1: most probably you'll want to copy this complete file into
         your source directory, adapt the settings to your needs and use
         #include "lint.h" as the first include in your source file(s).
         You should then be able to either directly compile your file
         or to run a check with splint over it without other changes.

         Note 2: you need brackets around arguments for special
         keywords, so f.e. it's "interrupt (1)" instead of "interrupt 1".
       */

      /*@ +charindex @*/

    #endif

  #endif

#endif
