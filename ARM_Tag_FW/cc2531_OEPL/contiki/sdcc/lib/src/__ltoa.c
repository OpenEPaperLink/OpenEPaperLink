/*-------------------------------------------------------------------------
   _ltoa.c - integer to string conversion

   Copyright (c) 1999, Bela Torok, bela.torok@kssg.ch

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

/*-------------------------------------------------------------------------
 usage:

 __ultoa(unsigned long value, char* string, int radix)
 __ltoa(long value, char* string, int radix)

 value  ->  Number to be converted
 string ->  Result
 radix  ->  Base of value (e.g.: 2 for binary, 10 for decimal, 16 for hex)
---------------------------------------------------------------------------*/

#include <stdlib.h>

/* "11110000111100001111000011110000" base 2 */
/* "37777777777" base 8 */
/* "4294967295" base 10 */
#define NUMBER_OF_DIGITS 32	/* eventually adapt if base 2 not needed */

#if NUMBER_OF_DIGITS < 32
# warning _ltoa() and _ultoa() are not save for radix 2
#endif

#if defined (__SDCC_mcs51) && defined (__SDCC_MODEL_SMALL) && !defined (__SDCC_STACK_AUTO)
# define MEMSPACE_BUFFER __idata	/* eventually __pdata or __xdata */
# pragma nogcse
#else
# define MEMSPACE_BUFFER
#endif

void __ultoa(unsigned long value, char* string, unsigned char radix)
{
  char MEMSPACE_BUFFER buffer[NUMBER_OF_DIGITS];  /* no space for '\0' */
  unsigned char index = NUMBER_OF_DIGITS;

  do {
    unsigned char c = '0' + (value % radix);
    if (c > (unsigned char)'9')
       c += 'A' - '9' - 1;
    buffer[--index] = c;
    value /= radix;
  } while (value);

  do {
    *string++ = buffer[index];
  } while ( ++index != NUMBER_OF_DIGITS );

  *string = 0;  /* string terminator */
}

void __ltoa(long value, char* string, unsigned char radix)
{
  if (value < 0 && radix == 10) {
    *string++ = '-';
    value = -value;
  }
  __ultoa(value, string, radix);
}

