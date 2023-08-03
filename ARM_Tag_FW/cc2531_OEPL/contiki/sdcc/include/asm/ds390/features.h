/*-------------------------------------------------------------------------
   features.h - DS390/DS400 specific features.

   Copyright (C) 2004, Maarten Brock, sourceforge.brock@dse.nl

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

#ifndef __SDC51_ASM_DS390_FEATURES_H
#define __SDC51_ASM_DS390_FEATURES_H   1

#define _REENTRANT	__reentrant
#define _CODE		__code

/* define _AUTOMEM to indicate the default storage class for
   automatic variables. To be used with pointers to automatic variables.
   Cannot be used in reentrant declared functions!

   void foo(void)
   {
     char Ar[10];
     char _AUTOMEM * pAr = Ar;
   }
*/
#if defined(__SDCC_STACK_AUTO)
  #if defined(__SDCC_USE_XSTACK)
    #define _AUTOMEM __pdata
  #elif defined(__SDCC_STACK_TENBIT)
    #define _AUTOMEM __xdata
  #else
    #define _AUTOMEM __idata
  #endif
#elif defined(__SDCC_MODEL_SMALL)
  #define _AUTOMEM __data
#else
  #define _AUTOMEM __xdata
#endif

/* define _STATMEM to indicate the default storage class for
   global/static variables. To be used with pointers to static variables.

   char Ar[10];
   void foo(void)
   {
     char _STATMEM * pAr = Ar;
   }
*/
#if defined(__SDCC_MODEL_SMALL)
  #define _STATMEM __data
#else
  #define _STATMEM __xdata
#endif

#endif
