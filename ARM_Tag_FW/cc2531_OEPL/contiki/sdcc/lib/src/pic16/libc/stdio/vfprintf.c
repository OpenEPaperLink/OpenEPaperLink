/*-------------------------------------------------------------------------
   vfprintf.c - source file for reduced version of printf

   Copyright (C) 1999, Sandeep Dutta <sandeep.dutta AT ieee.org>
   Modified for pic16 port, by Vangelis Rokas, 2005 <vrokas AT otenet.gr>
   Bug-fixed and feature-enhanced by Mauro Giachero, 2008 <mauro.giachero AT gmail.com>

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

/* following formats are supported :-
   format     output type       argument-type
     %%        -                   -
     %u        unsigned            int
     %u*       unsigned            *
     %b        binary              int
     %lb       binary              long
     %hb       binary              char
     %d        decimal             int
     %lu       unsigned            long
     %hu       unsigned            char
     %l[di]    decimal             long
     %lu[di]   unsigned            long
     %h[di]    decimal             char
     %hu[di]   unsigned            char
     %[xX]     hexadecimal         int
     %l[xX]    hexadecimal         long
     %h[xX]    hexadecimal         char
     %o        octal               int
     %lo       octal               long
     %ho       octal               char
     %c        character           char
     %s        character           generic pointer
   Also supported are:
   - the '0', '-' and ' ' alignment modifiers
   - the '+' and ' ' modifiers
   - the width field for integral types
   - the precision field for strings
*/

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/***********************************************************
 * The following switches enable some "advanced" features. *
 * With all the switches enabled:                          *
 * ; Statistics:                                           *
 * ; code size:     2062 (0x080e) bytes ( 1.57%)           *
 * ;                1031 (0x0407) words                    *
 * ; udata size:      16 (0x0010) bytes ( 1.25%)           *
 * ; access size:     31 (0x001f) bytes                    *
 * With all the switches disabled:                         *
 * ; Statistics:                                           *
 * ; code size:     1278 (0x04fe) bytes ( 0.98%)           *
 * ;                 639 (0x027f) words                    *
 * ; udata size:      16 (0x0010) bytes ( 1.25%)           *
 * ; access size:     25 (0x0019) bytes                    *
 ***********************************************************/
/*
 * Define this to enable support of the field width, which
 * allows to specify the minimum number of characters an
 * integer must use.
 * Costs ~200 code words and 3 bytes in access RAM.
 */
#define FIELD_WIDTH
/*
 * Define this to enable support of the precision, which
 * allows to specify the maximum number of characters a
 * string can use. Note that this implementation doesn't
 * use this field for integers (as it should).
 * Costs ~85 code words and 1 byte in access RAM.
 */
#define PRECISION
/*
 * Define this to enable support of the '+' and ' ' modifiers,
 * which specify that a positive signed number must be
 * preceded respectively with a '+' or a ' ' character.
 * Costs ~70 code words and 2 words of access RAM
 */
#define SIGN_MODIFIERS
/*
 * With this macro defined, trying to print a float number
 * will generate the "<NO FLOAT>" string.
 * Costs ~25 code words
 */
#define FLOAT_PLACEHOLDER
/*
 * With this macro defined, printing floats will work.
 * This also enables PRECISION and disables FLOAT_PLACEHOLDER.
 */
#if defined(USE_FLOATS)
  /* The configure script always defines USE_FLOATS to 0 or 1. */
# if USE_FLOATS < 1
#  undef USE_FLOATS
# endif
#else
/* # define USE_FLOATS */
#endif

#if defined(USE_FLOATS)
#define PRECISION
#undef FLOAT_PLACEHOLDER
#endif
/*
 * This macro enables the use of the 'b' binary specifier and
 * the use of "%b", "%hb" and "%lb"
 */
/* #define BINARY_SPECIFIER */
/*
 * This macro enables the use of the 'i' integer specifier and
 * the use of "%u", "%lu", ... in place of "%ud", "%lud", ... .
 * costs ~10 code words
 */
#define EXTRA_INTEGER

#if defined(USE_FLOATS)
/* x_ftoa requires up to 8 digits (integral part) + '.' + 24 digits
 * (fractional part). Adding a sign and a NUL byte yields 35 byte. */
# define BUF_SIZE       36
#elif defined(BINARY_SPECIFIER)
/* "%lb" = "0" - "11111111111111111111111111111111" */
# define BUF_SIZE       33
#else
/* "%lo" = "0" - "37777777777" or  "-20000000000" - "17777777777" */
# define BUF_SIZE       13
#endif

#if _DEBUG
extern void io_long (unsigned long);
extern void io_str (char *);
extern void io_int (unsigned int);
#endif

int
vfprintf (FILE * stream, const char *fmt, va_list ap)
{
  unsigned char radix;
  unsigned char flong;
  unsigned char fstr;
  unsigned char fchar;
#if defined(FLOAT_PLACEHOLDER) || defined(USE_FLOATS)
  unsigned char ffloat;
#endif
  unsigned char nosign;
  unsigned char upcase;
#ifdef FIELD_WIDTH
  unsigned char fieldwidth;
  unsigned char lalign;
  char padchar;
  const char *str1;
#endif
#ifdef PRECISION
  unsigned char precision;
#endif
#ifdef SIGN_MODIFIERS
  unsigned char printsign;
  char positivechar;
#endif
  int count = 0;
  const char *ch;
  char *str;
  long val;
  char buffer[BUF_SIZE];
  char *stringbuffer;

  if (0x80 == (unsigned char)(((unsigned long)stream) >> 16)) {
    /* strmputchar will modify *(char **)stream, thus confusing the user */
    stringbuffer = (char *) stream;
    stream = (FILE *) &stringbuffer;
  }

#if _DEBUG
  io_str ("vfprintf: ");
  io_long ((unsigned long) stream);
  io_long ((unsigned long) fmt);
#endif

//    va_start(ap,fmt);
  ch = fmt;

  while (*ch) //for (; *fmt ; fmt++ )
    {
      if (*ch == '%')
        {
          flong = 0;
          fstr = 0;
          fchar = 0;
#if defined(FLOAT_PLACEHOLDER) || defined(USE_FLOATS)
          ffloat = 0;
#endif
          nosign = 0;
          radix = 10;
          upcase = 0;
#ifdef FIELD_WIDTH
          fieldwidth = 0;
          lalign = 0;
          padchar = ' ';
#endif
#ifdef PRECISION
          // precision == -1 is used as an "unlimited" precision marker
          precision = (unsigned char)-1;
#endif
#ifdef SIGN_MODIFIERS
          printsign = 0;
          positivechar = '+';
#endif
          ++ch;

          if (*ch == '%')
            {
              __stream_putchar (stream, *ch);
              ++count;
              ++ch;
              continue;
            }

#ifdef FIELD_WIDTH
          if (*ch == '0')
            {
              padchar = '0';
              ++ch;
            }

          if (*ch == '-')
            {
              lalign = 1;
              ++ch;
            }
#endif
#ifdef SIGN_MODIFIERS
          if (*ch == ' ')
            {
              printsign = 1;
              positivechar = ' ';
              ++ch;
            }

          if (*ch == '+')
            {
              printsign = 1;
              ++ch;
            }
#endif

#ifdef FIELD_WIDTH
          if ((*ch >= '1') && (*ch <= '9'))
            {
              while ((*ch >= '0') && (*ch <= '9'))
                {
                  fieldwidth = 10 * fieldwidth + (*ch) - '0';
                  ++ch;
                }
            }
#endif

#ifdef PRECISION
          if (*ch == '.')
            {
              ++ch;
              precision = 0;
              while ((*ch >= '0') && (*ch <= '9'))
                {
                  precision = 10 * precision + (*ch) - '0';
                  ++ch;
                }
            }
#endif

          if (*ch == 'l')
            {
              flong = 1;
              ++ch;
            }
          else if (*ch == 'h')
            {
              fchar = 1;
              ++ch;
            }

          if (*ch == 'u')
            {
              nosign = 1;
              ++ch;
            }

          if (*ch == 's')
            {
              fstr = 1;
#ifdef FIELD_WIDTH
              padchar = ' ';    /* Strings are always space-padded */
#endif
            }
          else if (*ch == 'x')
            radix = 16;
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
#if defined(FLOAT_PLACEHOLDER) || defined(USE_FLOATS)
          else if (*ch == 'f')
            {
              ffloat = 1;
            }
#endif
#ifdef EXTRA_INTEGER
          else if ((*ch == 'd') || (*ch == 'i'))  /* This is the default */
            ;
          else if (nosign)                        /* %u alone is the same as %ud */
            --ch;
#else
          else if (*ch == 'd')
            ;
#endif
          else
            {
              __stream_putchar (stream, *ch);
              ++count;
              ++ch;
              continue;
            }

          if (fstr)
            {
              str = va_arg (ap, char *);
#if defined(USE_FLOATS)
            }
          else if (ffloat)
            {
              float f = va_arg(ap, float);
              str = buffer;
              x_ftoa (f, buffer, BUF_SIZE, precision);
              precision = -1;
#elif defined(FLOAT_PLACEHOLDER)
            }
          else if (ffloat)
            {
              str = (char*)"<NO FLOAT>";
              va_arg (ap, float);
#ifdef PRECISION
              precision = (unsigned char)-1;
#endif /* PRECISION */
#endif /* FLOAT_PLACEHOLDER */
            }
          else
            {
#ifdef PRECISION
              precision = (unsigned char)-1; //FIXME: No support for the precision field on numerals
#endif
              val = 0;
              if (flong)
                {
                  val = va_arg (ap, long);
#if _DEBUG
                  io_long (val);
#endif
                }
              else if (fchar)
                {
                  val = (char) va_arg (ap, int);  // FIXME: SDCC passes 1-byte char varargs as 2-byte ints...
                  if ((radix != 10) || nosign)
                    val = (unsigned char) val;    //Avoid unwanted sign extension
#if _DEBUG
                  io_long (val);
#endif
                }
              else
                {
                  val = va_arg (ap, int);
                  if ((radix != 10) || nosign)
                    val = (unsigned int) val;   //Avoid unwanted sign extension
#if _DEBUG
                  io_long (val);
#endif
                }

              str = buffer + 1; //Reserve space for a forced '+'
              if (radix)
                {
                  if (nosign)
                    ultoa (val, buffer + 1, radix);
                  else
                    ltoa (val, buffer + 1, radix);
#ifdef SIGN_MODIFIERS
                  if (printsign && (*str != '-'))
                    {
                      --str;
                      *str = positivechar;
                    }
#endif
                }
              else
                {
                  *str = (unsigned char) val;
                  *(str + 1) = '\0';
                }
            }

#ifdef FIELD_WIDTH
          //Count how many pad chars are required in fieldwidth
          str1 = str;
          while (fieldwidth && *str1)
            {
              ++str1;
              --fieldwidth;
            }
          //Left padding
          if (!lalign)
            {
              while (fieldwidth)
                {
                  __stream_putchar (stream, padchar);
                  ++count;
                  --fieldwidth;
                }
            }
#endif
          while (*str
#ifdef PRECISION
                 && (!~precision || precision--)
#endif
            )
            {
              radix = *str;
              if (upcase)
                {
                  radix = toupper (radix);
                }
              __stream_putchar (stream, radix);
              ++str;
              ++count;
              if (fieldwidth)
                {
                  fieldwidth--;
                }
            }
#ifdef FIELD_WIDTH
          //Right padding (with spaces)
          if (lalign)
            {
              while (fieldwidth)
                {
                  __stream_putchar (stream, ' ');
                  ++count;
                  --fieldwidth;
                }
            }
#endif
        }
      else
        {
          __stream_putchar (stream, *ch);
          ++count;
        }

      ++ch;
    }

  return count;
}
