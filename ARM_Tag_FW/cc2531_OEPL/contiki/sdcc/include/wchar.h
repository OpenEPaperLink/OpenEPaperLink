/*-------------------------------------------------------------------------
   wchar.h - Extended and multibyte wide character utilitites (ISO C 11 7.29)

   Copyright (c) 2015-2016, Philipp Klaus Krause / pkk@spth.de

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

#ifndef __SDCC_WCHAR_H
#define __SDCC_WCHAR_H 1

#ifndef __WCHAR_T_DEFINED
#define __WCHAR_T_DEFINED
  typedef unsigned long int wchar_t;
#endif

#ifndef __SIZE_T_DEFINED
#define __SIZE_T_DEFINED
  typedef unsigned int size_t;
#endif

#ifndef __MBSTATE_T_DEFINED
#define __MBSTATE_T_DEFINED
  typedef struct {unsigned char c[3];} mbstate_t;
#endif

#ifndef __WINT_T_DEFINED
#define __WINT_T_DEFINED
  typedef unsigned long int wint_t;
#endif

struct tm;

#ifndef WEOF
  #define WEOF 0xfffffffful
#endif

/* C99 Wide string comparison functions (ISO C11 7.29.4.4) */
int wcscmp(const wchar_t *s1, const wchar_t *s2);

/* C99 Miscellaneous functions (ISO C11 7.29.4.6) */
size_t wcslen(const wchar_t *s);

/* C99 Single-byte/wide character conversion functions (ISO C 11 7.29.6.1) */
wint_t btowc(int c);
int wctob(wint_t c);

/* C99 Conversion state functions (ISO C 11 7.29.6.2) */
int mbsinit(const mbstate_t *ps);

/* C99 Restartable multibyte/wide character conversion functions (ISO C 11 7.29.6.3) */
size_t mbrlen(const char *restrict s, size_t n, mbstate_t *restrict ps);
size_t mbrtowc(wchar_t *restrict pwc, const char *restrict s, size_t n, mbstate_t *restrict ps);
size_t wcrtomb(char *restrict s, wchar_t wc, mbstate_t *restrict ps);

#endif /* __SDCC_WCHAR_H */
