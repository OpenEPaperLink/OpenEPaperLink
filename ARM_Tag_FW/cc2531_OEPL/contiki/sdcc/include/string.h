/*-------------------------------------------------------------------------
   string.h - ISO header for string library functions

   Copyright (C) 1998, Sandeep Dutta
   Copyright (C) 2009-2019, Philipp Klaus Krause pkk@spth.de

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

#ifndef __SDCC_STRING_H
#define __SDCC_STRING_H 1

#ifndef NULL
# define NULL (void *)0
#endif

#ifndef __SIZE_T_DEFINED
# define __SIZE_T_DEFINED
  typedef unsigned int size_t;
#endif

/* Bounds-checking interfaces from annex K of the C11 standard. */
#if defined (__STDC_WANT_LIB_EXT1__) && __STDC_WANT_LIB_EXT1__

#ifndef __RSIZE_T_DEFINED
#define __RSIZE_T_DEFINED
typedef size_t rsize_t;
#endif

#ifndef __ERRNO_T_DEFINED
#define __ERRNO_T_DEFINED
typedef int errno_t;
#endif

#endif

#if defined(__SDCC_mcs51) || defined(__SDCC_hc08) || defined(__SDCC_ds390) || defined(__SDCC_pic14) || defined(__SDCC_pic16)
#define __SDCC_BROKEN_STRING_FUNCTIONS
#endif

/* The function prototypes are ordered as in the ISO C11 standard. */

/* Todo: fix the "restrict" stuff for C99 compliance. */

/* Copying functions: */
void *memccpy (void */*restrict*/ dst, const void */*restict*/ src, int c, size_t n);
extern void *memcpy (void * /*restrict */ dest, const void * /*restrict*/ src, size_t n);
#if defined(__SDCC_z80) || defined(__SDCC_z180) || defined(__SDCC_r2k) || defined(__SDCC_r2ka) || defined(__SDCC_r3ka)
extern void *memmove (void *dest, const void *src, size_t n) __preserves_regs(iyl, iyh);
#else
extern void *memmove (void *dest, const void *src, size_t n);
#endif
#if defined(__SDCC_z80) || defined(__SDCC_z180) || defined(__SDCC_r2k) || defined(__SDCC_r2ka) || defined(__SDCC_r3ka) || defined(__SDCC_tlcs90) || defined (__SDCC_ez80_z80) || defined (__SDCC_z80n)
extern char *strcpy (char * /*restrict*/ dest, const char * /*restrict*/ src) __preserves_regs(iyl, iyh);
#else
extern char *strcpy (char * /*restrict*/ dest, const char * /*restrict*/ src);
#endif
extern char *strncpy (char * /*restrict*/ dest, const char * /*restrict*/ src, size_t n);

/* Concatenation functions: */
extern char *strcat (char * /*restrict*/ dest, const char * /*restrict*/ src);
extern char *strncat (char * /*restrict*/ dest, const char * /*restrict*/ src, size_t n);

/* C2X Duplication functions */
extern char *strdup (const char *s);
extern char *strndup (const char *s, size_t n);

/* Comparison functions: */
extern int memcmp (const void *s1, const void *s2, size_t n);
extern int strcmp (const char *s1, const char *s2);
#define strcoll(s1, s2) strcmp(s1, s2)
/*int strcoll(const char *s1, const char *s2) {return strcmp(s1, s2);}*/
extern int strncmp (const char *s1, const char *s2, size_t n);
extern size_t strxfrm (char *dest, const char *src, size_t n);

/* Search functions: */
extern void *memchr (const void *s, int c, size_t n);
#ifdef __SDCC_BROKEN_STRING_FUNCTIONS
extern char *strchr (const char *s, char c); /* c should be int according to standard. */
#else
extern char *strchr (const char *s, int c);
#endif
extern size_t strcspn(const char *s, const char *reject);
extern char *strpbrk(const char *s, const char *accept);
#ifdef __SDCC_BROKEN_STRING_FUNCTIONS
extern char *strrchr(const char *s, char c); /* c should be int according to standard. */
#else
extern char *strrchr(const char *s, int c);
#endif
extern size_t strspn (const char *s, const char *accept);
extern char *strstr (const char *haystack, const char *needle);
extern char *strtok (char * /* restrict*/ str, const char * /*restrict*/ delim);

/* Miscanelleous functions: */
#ifdef __SDCC_BROKEN_STRING_FUNCTIONS
extern void *memset (void *s, unsigned char c, size_t n); /* c should be int according to standard. */
#else
extern void *memset (void *s, int c, size_t n);
#endif

extern void *memset_explicit (void *s, int c, size_t n);

/* extern char *strerror(int errnum); */
#if defined(__SDCC_z80) || defined(__SDCC_z180) || defined(__SDCC_tlcs90) || defined (__SDCC_ez80_z80) || defined (__SDCC_z80n)
extern size_t strlen (const char *s) __preserves_regs(d, e, iyl, iyh);
#else
extern size_t strlen (const char *s);
#endif

#ifdef __SDCC_ds390
extern void __xdata * memcpyx(void __xdata *, void __xdata *, int) __naked;
#endif

#if defined(__SDCC_z80) || defined(__SDCC_z180) || defined(__SDCC_r2k) || defined(__SDCC_r2ka) || defined(__SDCC_r3ka) || defined (__SDCC_ez80_z80) || defined (__SDCC_z80n)
#define memcpy(dst, src, n) __builtin_memcpy(dst, src, n)
#define strcpy(dst, src) __builtin_strcpy(dst, src)
#define strncpy(dst, src, n) __builtin_strncpy(dst, src, n)
#define strchr(s, c) __builtin_strchr(s, c)
#define memset(dst, c, n) __builtin_memset(dst, c, n)
#else
extern void *__memcpy (void * /*restrict */ dest, const void * /*restrict*/ src, size_t n);
#define memcpy(dst, src, n) __memcpy(dst, src, n)
#endif

#endif

