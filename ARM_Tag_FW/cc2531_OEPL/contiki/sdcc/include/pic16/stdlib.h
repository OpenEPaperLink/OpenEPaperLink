/*-------------------------------------------------------------------------
   stdlib.h - ANSI functions forward declarations

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

#ifndef __STDLIB_H__
#define __STDLIB_H__ 1

#pragma library c

#include <stdint.h>

#ifndef NULL
# define NULL (void *)0
#endif

#define RAND_MAX        0x7fffffff

/* absolute value */
int abs (int j);
long int labs (long int j);

/* initialize random seed */
void srand (unsigned long seed);

/* return a random number between 0 and RAND_MAX */
long rand (void);

/* reentrant version of rand() */
long rand_r (unsigned long *ctx);

/* returns the CRC16 checksum of the data buffer, takes as
 * last argument an old value of crc16 checksum */
uint16_t crc16 (uint8_t *, uint32_t, uint16_t);

/* convert a ASCII string to float */
float atof (char *);

/* convert a ASCII string to integer */
int atoi (char *);

/* convert a ASCII string to long */
long atol (char *);

/* convert an unsigned/signed integer to ASCII string */
void uitoa (unsigned int, __data char *, unsigned char);
void itoa (int, __data char*, unsigned char);

/* convert an unsigned/signed long integer to ASCII string */
void ultoa (unsigned long, __data char *, unsigned char);
void ltoa (long, __data char*, unsigned char);

/* helper functions: convert a float to ASCII string */
extern char x_ftoa (float, __data char *, unsigned char, unsigned char);

/* George M. Gallant's version of ftoa() */
extern void g_ftoa (__data char *, float, char);


#endif	/* __STDLIB_H__ */
