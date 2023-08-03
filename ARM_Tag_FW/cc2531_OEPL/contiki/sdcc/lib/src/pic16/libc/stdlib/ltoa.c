/*-------------------------------------------------------------------------
   ltoa.c - long integer to string conversion

   Copyright (C) 1999, Bela Torok <bela.torok at kssg.ch>

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

/*-------------------------------------------------------------------------
 value  ->  Number to be converted
 string ->  Result
 radix  ->  Base of value (e.g.: 2 for binary, 10 for decimal, 16 for hex)
---------------------------------------------------------------------------*/

#define NUMBER_OF_DIGITS 32

#if _DEBUG
extern void io_long(unsigned long);
extern void io_str(char *);
#endif


void ultoa(unsigned long value, __data char* str, unsigned char radix)
{
  unsigned int index;
  unsigned char ch;
  unsigned char buffer[NUMBER_OF_DIGITS];  /* space for NUMBER_OF_DIGITS + '\0' */

    index = NUMBER_OF_DIGITS;

    do {
      ch = '0' + (value % radix);
      if ( ch > (unsigned char)'9') ch += 'a' - '9' - 1;

#if _DEBUG
      io_str( "ultoa: " );
      io_long( value );
      io_long( (unsigned long) ch );
#endif

      buffer[ --index ] = ch;
      value /= radix;
    } while (value != 0);

    do {
      *str++ = buffer[index++];
    } while ( index < NUMBER_OF_DIGITS );

    *str = 0;  /* string terminator */
}

void ltoa(long value, __data char* str, unsigned char radix)
{
#if _DEBUG
  io_str( "ltoa: " );
  io_long( (unsigned long)value );
#endif

  if (value < 0 && radix == 10) {
    *str++ = '-';
    value = -value;
  }



  ultoa((unsigned long)value, str, radix);
}
