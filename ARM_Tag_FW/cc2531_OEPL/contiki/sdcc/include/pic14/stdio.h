/*-------------------------------------------------------------------------
   stdio.h - ANSI functions forward declarations

   Copyright (C) 1998, Sandeep Dutta . sandeep.dutta@usa.net
   Ported to PIC16 port by Vangelis Rokas, 2004 <vrokas AT otenet.gr>

   Modifications for PIC14 by
   Copyright (C) 2019 Gonzalo Pérez de Olaguer Córdoba <salo@gpoc.es>

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

#ifndef __STDIO_H
#define __STDIO_H 1

#include <stdarg.h>

#include <sdcc-lib.h>

#ifndef EOF
# define EOF (-1)
#endif

#ifndef NULL
  #define NULL (void *)0
#endif

#ifndef __SIZE_T_DEFINED
#define __SIZE_T_DEFINED
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

/* -------------------------------------------------------------------------
 * streams description
 *
 * FILE is a generic pointer, so a stream (FILE*) is a pointer to a generic pointer.
 *
 * If stream is NULL data will be written by calling to putchar.
 *
 * If (*stream) points to __data space it is considered a pointer to the
 * char* buffer being written (sprintf et al.)
 *
 * If (*stream) points to __code space it is considered a pointer to the
 * _stream_out_handler that actually handles the write.
 * -------------------------------------------------------------------------*/

typedef void *FILE;
typedef int _stream_out_handler (char c, FILE *stream);

/* -------------------------------------------------------------------------
 * Implemented streams. They will be linked to the program only if used.
 * -------------------------------------------------------------------------*/

extern FILE *usart_out;
extern FILE *mssp_out;
extern FILE *gpsim_out;

/* -------------------------------------------------------------------------
 * The stdout stream is provided by the library and initialized to NULL,
 * so by default the functions writing to stdout will call putchar.
 *
 * If another behaviour is desired, the application must assign
 * to stdout the stream that writes to the desired peripheral.
 * -------------------------------------------------------------------------*/

extern FILE *stdout;

/* -------------------------------------------------------------------------
 * The following functions end up calling fputc with the proper stream.
 * -------------------------------------------------------------------------*/

extern int printf (const char *fmt, ...);
extern int sprintf (char *str, const char *fmt, ...);
extern int fprintf (FILE *stream, const char *fmt, ...);

extern int vprintf (const char *fmt, va_list ap);
extern int vsprintf (char *str, const char *fmt, va_list ap);
extern int vfprintf (FILE *stream, const char *fmt, va_list ap);

extern int fputs (const char *s, FILE *stream);
extern int fputc (char c, FILE *stream);
#define putc(c,s)	fputc(c,s)

/* -------------------------------------------------------------------------
 * The following functions DO NOT use streams.
 * They end up calling getchar or putchar.
 * -------------------------------------------------------------------------*/

extern void printf_small (const char *fmt, ...);

extern int puts(const char *);

#if __STDC_VERSION__ < 201112L
extern char *gets(char *);
#endif

/* -------------------------------------------------------------------------
 * These functions are provided by the library as dummy functions.
 * They do nothing and return EOF.
 * The application must provide alternative functions if needed.
 * -------------------------------------------------------------------------*/

extern int getchar(void);
extern int putchar(int);

#endif /* __STDIO_H */

