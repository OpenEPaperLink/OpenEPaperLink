/*-------------------------------------------------------------------------
   stdio.h - ANSI functions forward declarations

   Copyright (C) 1998, Sandeep Dutta . sandeep.dutta@usa.net
   Ported to PIC16 port by Vangelis Rokas, 2004 <vrokas AT otenet.gr>

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

/* link the C library */
#pragma library c

#include <stdarg.h>

#include <sdcc-lib.h>

#ifndef NULL
  #define NULL (void *)0
#endif

#ifndef _SIZE_T_DEFINED
#define _SIZE_T_DEFINED
  typedef unsigned int size_t;
#endif

/* stream descriptor definition */
typedef char *FILE;

/* USART and MSSP module stream descriptors */

/* since FILE is declared as a generic pointer,
 * the upper byte is used to dereference the pointer
 * information. For the stream descriptors we
 * use the 5th bit and the lower nubble bits.
 * Descriptors are denoted by an 1 in bit 5,
 * further dereference is made for:
 * <stream>     <3:0> bits
 *     USART     0 (0x0)
 *     MSSP      1 (0x1)
 *     USER     15 (0xf)
 *
 * There is a special value for GPSIM specific (see below)
 * which is:
 *     GPSIM    14 (0xe)
 *
 *
 * if further stream descriptors need to be added then more
 * bits of the upper byte can be used
 */

#define USART_DEREF     0x0
#define MSSP_DEREF      0x1
#define USER_DEREF      0xf

#define STREAM_USART    ((FILE *)(0x00200000UL))
#define STREAM_MSSP     ((FILE *)(0x00210000UL))
#define STREAM_USER     ((FILE *)(0x002f0000UL))

/* this is a custom dereference which points to a custom
 * port of GPSIM simulator. This port redirects characters
 * to /tmp/gpsim.debug.1 file (used for debugging purposes)
 * NOTICE: This feature is not part of the official gpsim
 * distribution. Contact vrokas AT users.sourceforge.net
 * for more info */
#define GPSIM_DEREF     0xe
#define STREAM_GPSIM    ((FILE *)(0x002e0000UL))

extern FILE *stdin;
extern FILE *stdout;

/* printf_small() supports float print */
void printf_small (const char *fmt, ...);

/* printf_tiny() does not support float print */
void printf_tiny (const char *fmt, ...);  // __reentrant;

extern int printf (const char *fmt, ...);
extern int fprintf (FILE *stream, const char *fmt, ...);
extern int sprintf (char *str, const char *fmt, ...);

extern int vprintf (const char *fmt, va_list ap);
extern int vfprintf (FILE *stream, const char *fmt, va_list ap);
extern int vsprintf (char *str, const char *fmt, va_list ap);

#define PUTCHAR(C)  void putchar (char C) __wparam
extern PUTCHAR (c);

extern void __stream_putchar (FILE *stream, char c);

extern void __stream_usart_putchar (char c) __wparam __naked;
extern void __stream_mssp_putchar (char c) __wparam __naked;
extern void __stream_gpsim_putchar (char c) __wparam __naked;

extern char *gets (char *str);
extern char getchar (void);

#endif /* __STDIO_H */
