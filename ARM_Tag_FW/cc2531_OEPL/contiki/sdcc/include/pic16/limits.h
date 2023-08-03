/*-------------------------------------------------------------------------
   limits.h - ANSI defines constants for sizes of integral types

   Copyright (C) 1999, Sandeep Dutta . sandeep.dutta@usa.net
   Adopted for the pic16 port by Vangelis Rokas <vrokas AT otenet.gr> 2004

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

#ifndef __LIMITS_H
#define __LIMITS_H 1

#define CHAR_BIT      8    /* bits in a char */
#define SCHAR_MAX   127
#define SCHAR_MIN  -128
#define UCHAR_MAX   0xff
#define UCHAR_MIN   0
#ifdef __SDCC_CHAR_UNSIGNED
#define CHAR_MAX    UCHAR_MAX
#define CHAR_MIN    UCHAR_MIN
#else
#define CHAR_MAX    SCHAR_MAX
#define CHAR_MIN    SCHAR_MIN
#endif
#define INT_MIN    -32768
#define INT_MAX     32767
#define SHRT_MAX    INT_MAX
#define SHRT_MIN    INT_MIN
#define UINT_MAX    0xffff
#define UINT_MIN    0
#define USHRT_MAX   UINT_MAX
#define USHRT_MIN   UINT_MIN
#define LONG_MIN   -2147483648
#define LONG_MAX    2147483647
#define ULONG_MAX   0xffffffff
#define ULONG_MIN   0

#endif
