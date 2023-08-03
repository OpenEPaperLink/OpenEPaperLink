/*-------------------------------------------------------------------------
   printf_tiny.c - source file for reduced version of printf

   Copyright (C) 1999, Sandeep Dutta <sandeep.dutta AT ieee.org>
   Modified for pic16 port, by Vangelis Rokas, 2004 <vrokas AT otenet.gr>

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

/* This function uses function putchar() to dump a character
 * to standard output. putchar() is defined in libc18f.lib
 * as dummy function, which will be linked if no putchar()
 * function is provided by the user.
 * The user can write his own putchar() function and link it
 * with the source *BEFORE* the libc18f.lib library. This way
 * the linker will link the first function (i.e. the user's function) */

/* following formats are supported :-
   format     output type       argument-type
     %u*       unsigned            *

     %b        binary
     %d        decimal             int
     %ld       decimal             long
     %hd       decimal             char
     %x        hexadecimal         int
     %lxX      hexadecimal         long
     %hxX      hexadecimal         char
     %o        octal               int
     %lo       octal               long
     %ho       octal               char
     %c        character           char
     %s        character           generic pointer
*/

/*
 * This macro enables the use of the 'b' binary specifier and
 * the use of "%b", "%hb" and "%lb"
 */
/* #define BINARY_SPECIFIER */

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#if 0
#define DPUT(c) putchar(c)
#else
#define DPUT(c)
#endif

#define ISLONG          (flong)
#define ISSTR           (fstr)
#define ISCHAR          (fchar)
#define HAVESIGN        (nosign)

#ifdef BINARY_SPECIFIER
/* "%lb" = "0" - "11111111111111111111111111111111" */
# define BUF_SIZE       33
#else
/* "%lo" = "0" - "37777777777" or  "-21777777777" - "17777777777" */
# define BUF_SIZE       13
#endif

void
printf_tiny (const char *fmt, ...)
{
  char radix;
  char flong, fstr;
  char fchar, nosign;
  char upcase;

  const char *str, *ch;
  __data char *str1;
  long val;
  char buffer[BUF_SIZE];
  va_list ap;

  va_start (ap, fmt);
  ch = fmt;

  while (*ch) //for (; *fmt ; fmt++ )
    {
      if (*ch == '%')
        {
          ISLONG = 0;
          ISSTR = 0;
          ISCHAR = 0;
          HAVESIGN = 0;
          radix = 0;
          upcase = 0;
          ch++;

          if (*ch == 'u')
            {
              HAVESIGN = 1;
              ++ch;
            }

          if (*ch == 'l')
            {
              ISLONG = 1;
              ++ch;
            }
          else if (*ch == 'h')
            {
              ISCHAR = 1;
              ++ch;
            }

          if (*ch == 's')
            ISSTR = 1;
          else if (*ch == 'd')
            radix = 10;
          else if (*ch == 'x')
            {
              radix = 16;
              upcase = 0;
            }
          else if (*ch == 'X')
            {
              radix = 16;
              upcase = 1;
            }
          else if (*ch == 'c')
            radix = 0;
          else if (*ch == 'o')
            radix = 8;
#ifdef BINARX_SPECIFIER
          else if (*ch == 'b')
            radix = 2;
#endif

          if (ISSTR)
            {
              str = va_arg (ap, const char *);
              while (*str)
                {
                  putchar (*str);
                  ++str;
                }
            }
          else
            {
              if (ISLONG)
                val = va_arg (ap, long);
              else if (ISCHAR)
                {
                  val = (unsigned char) va_arg (ap, int);       // FIXME: SDCC casts char arguments into ints
                  if (!HAVESIGN)
                    val = (char) val;   // FIXME cont'd: sign-extend if required
                }
              else
                val = va_arg (ap, int);

              if (radix)
                {
                  if (HAVESIGN)
                    ultoa (val, buffer, radix);
                  else
                    ltoa (val, buffer, radix);

                  str1 = buffer;
                  while ((*str1))
                    {
                      radix = *str1;
                      if (upcase)
                        radix = toupper (radix);
                      putchar (radix);
                      ++str1;
                    }
                }
              else
                putchar ((char) val);
            }
        }
      else
        putchar (*ch);

      ++ch;
    }
}
