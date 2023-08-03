/*-------------------------------------------------------------------------
   uchar.h: Unicode utilities  (ISO C 11 7.28)

   Copyright (C) 2015-2016, Philipp Klaus Krause, pkk@spth.de

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

#ifndef __SDCC_UCHAR_H
#define __SDCC_UCHAR_H 1

#ifndef __MBSTATE_T_DEFINED
#define __MBSTATE_T_DEFINED
  typedef struct {unsigned char c[3];} mbstate_t;
#endif

#ifndef __SIZE_T_DEFINED
#define __SIZE_T_DEFINED
  typedef unsigned int size_t;
#endif

#ifndef __CHAR16_T_DEFINED
#define __CHAR16_T_DEFINED
  typedef unsigned int char16_t;
#endif

#ifndef __CHAR32_T_DEFINED
#define __CHAR32_T_DEFINED
  typedef unsigned long int char32_t;
#endif

size_t mbrtoc16(char16_t *restrict pc16, const char *restrict s, size_t n, mbstate_t *restrict ps);
size_t c16rtomb(char *restrict s, char16_t c16, mbstate_t *restrict ps);
size_t mbrtoc32(char32_t *restrict pc32, const char *restrict s, size_t n, mbstate_t *restrict ps);
size_t c32rtomb(char *restrict s, char32_t c32, mbstate_t *restrict ps);

size_t __mbstoc16s(char16_t *restrict c16s, const char *restrict s, size_t n);
size_t __c16stombs(char *restrict s, const char16_t *restrict c16s, size_t n);

#endif

