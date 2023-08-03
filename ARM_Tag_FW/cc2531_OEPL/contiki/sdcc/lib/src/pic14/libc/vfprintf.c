/*-------------------------------------------------------------------------
   vfprintf.c - source file for reduced version of printf

   Copyright (C) 1999, Sandeep Dutta <sandeep.dutta AT ieee.org>
   Modified for pic16 port, by Vangelis Rokas, 2005 <vrokas AT otenet.gr>
   Bug-fixed and feature-enhanced by Mauro Giachero, 2008 <mauro.giachero AT gmail.com>

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
#define USE_FLOATS
/*
 * This macro enables the use of the 'b' binary specifier and
 * the use of "%b", "%hb" and "%lb"
 */
#define BINARY_SPECIFIER
/*
 * This macro enables the use of the 'i' integer specifier and
 * the use of "%u", "%lu", ... in place of "%ud", "%lud", ... .
 * costs ~10 code words
 */
#define EXTRA_INTEGER
/*
 * Two ways to handle putchar and getchar.
 */
#define _STATIC_PUTCHAR 0
#define _STATIC_GETCHAR 0

/*
 * END OF CONFIGURATION SETTINGS
 */

#if defined(USE_FLOATS)
#define PRECISION
#undef FLOAT_PLACEHOLDER
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

/* flags */
#define _LONG       0x0001
#define _STR        0x0002
#define _CHAR       0x0004
#define _PTR        0x0008
#define _FLOAT      0x1000  // only when USE_FLOATS or FLOAT_PLACEHOLDER
#define _UNSIGNED   0x0010
#define _UPCASE     0x0020
#define _LEFTALIGN  0x0100  // only when FIELD_WIDTH
#define _ZEROPAD    0x0200  // only when FIELD_WIDTH
#define _PRINTSIGN  0x0400  // only when SIGN_MODIFIERS
#define _PRINTBLANK 0x0800  // only when SIGN_MODIFIERS

/* precision value meaning unset */
#define _UNSET  ((unsigned char)-1)

#if _STATIC_GETCHAR

static const char *_fmt;
static char ch;
static void _getchar (void)
{
        ch = *_fmt++;
}
#define _GETCHAR()  _getchar()

#else /* _STATIC_GETCHAR */
#define _GETCHAR()  do { ch = *fmt++; } while(0)
#endif

#if _STATIC_PUTCHAR

static FILE *_stream;
static unsigned char count;
static int _putchar (unsigned char c)
{
	if (fputc(c, _stream) < 0)
		return 1;
       	count++;
	return 0;
}
#define _PUTCHAR(c) do { if (_putchar(c)) goto _error; } while(0)

#else /* _STATIC_PUTCHAR */
#define _PUTCHAR(c) do { if (fputc(c, stream) < 0) goto _error; count++; } while(0)
#endif

int
vfprintf (FILE *stream, const char *fmt, va_list ap)
{
  unsigned flags;
  unsigned char radix;      // 2(binary), 8(octal), 10(decimal), 16(hexadecimal) or 0(char)

#if defined(FLOAT_PLACEHOLDER) || defined(USE_FLOATS)
  float f;
#endif

#ifdef FIELD_WIDTH
  unsigned char fieldwidth; // field width in number of characters
#endif

#ifdef PRECISION
  unsigned char precision;  // precission width in number of characters
#endif

  const char *cstr;
  char *str;
  long val;
  char buffer[BUF_SIZE];

#if _STATIC_GETCHAR
  _fmt = fmt;
#else
  char ch;
#endif

#if _STATIC_PUTCHAR
  _stream = stream;
  count = 0;
#else
  unsigned char count = 0;
#endif

  while (1)
    {
      _GETCHAR();
      if (ch == 0)
        return count;
      if (ch != '%')
        {
          _PUTCHAR(ch);
          continue;
        }
      _GETCHAR();
      if (ch == '%')
        {
          _PUTCHAR(ch);
          continue;
        }

      flags = 0;
      radix = 10;

#ifdef FIELD_WIDTH
      fieldwidth = 0;
#endif

#ifdef PRECISION
      // _UNSET is used as an "unlimited" precision marker
      precision = _UNSET;
#endif

#ifdef FIELD_WIDTH
      if (ch == '0')
        {
          flags |= _ZEROPAD;
          _GETCHAR();
        }

      if (ch == '-')
        {
          flags |= _LEFTALIGN;
          _GETCHAR();
        }
#endif

#ifdef SIGN_MODIFIERS
      if (ch == ' ')
        {
          flags |= _PRINTSIGN;
          flags |= _PRINTBLANK;
          _GETCHAR();
        }

      if (ch == '+')
        {
          flags |= _PRINTSIGN;
          _GETCHAR();
        }
#endif

#ifdef FIELD_WIDTH
      if ((ch >= '1') && (ch <= '9'))
        {
          while ((ch >= '0') && (ch <= '9'))
            {
              fieldwidth = 10 * fieldwidth + (ch) - '0';
              _GETCHAR();
            }
        }
#endif

#ifdef PRECISION
      if (ch == '.')
        {
          _GETCHAR();
          precision = 0;
          while ((ch >= '0') && (ch <= '9'))
            {
              precision = 10 * precision + (ch) - '0';
              _GETCHAR();
            }
        }
#endif

      if (ch == 'l')
        {
          flags |= _LONG;
          _GETCHAR();
        }
      else if (ch == 'h')
        {
          flags |= _CHAR;
          _GETCHAR();
        }

      if (ch == 'u')
        {
          flags |= _UNSIGNED;
          _GETCHAR();
        }

      if (ch == 's')
        {
          flags |= _STR;
#ifdef FIELD_WIDTH
          flags &= ~_ZEROPAD;   /* Strings are always space-padded */
#endif
        }
      else if (ch == 'p')
        {
          flags |= _PTR;
          flags |= _UNSIGNED;
          radix = 16;
        }
      else if (ch == 'x')
        radix = 16;
      else if (ch == 'X')
        {
          radix = 16;
          flags |= _UPCASE;
        }
      else if (ch == 'c')
        radix = 0;
      else if (ch == 'o')
        radix = 8;
#ifdef BINARY_SPECIFIER
      else if (ch == 'b')
        radix = 2;
#endif
#if defined(FLOAT_PLACEHOLDER) || defined(USE_FLOATS)
      else if (ch == 'f')
        {
          flags |= _FLOAT;
        }
#endif
#ifdef EXTRA_INTEGER
      else if ((ch == 'd') || (ch == 'i'))  /* This is the default */
            ;
      else if (flags & _UNSIGNED)           /* %u alone is the same as %ud */
#if _STATIC_GETCHAR
        --_fmt;
#else
        --fmt;
#endif
#else
      else if (ch == 'd')
            ;
#endif
      else
        {
          goto _error;
        }

      /* Here starts the formatting code */

      if (flags & _STR)
        {
          str = va_arg (ap, char *);
        }
#if defined(USE_FLOATS)
      else if (flags & _FLOAT)
        {
          f = va_arg (ap, float);
          str = buffer;
          _ftoa (f, buffer, PREC(PREC_F, precision == _UNSET ? PREC_D : precision));
          precision = _UNSET;
        }
#elif defined(FLOAT_PLACEHOLDER)
      else if (flags & _FLOAT)
        {
          /* read but ignore the argument */
          f = va_arg (ap, float);
          str = (char*)"<NO FLOAT>";
#ifdef PRECISION
          precision = _UNSET;
#endif /* PRECISION */
        }
#endif /* FLOAT_PLACEHOLDER */
      else
        {
#ifdef PRECISION
          precision = _UNSET;   //FIXME: No support for the precision field on numerals
#endif
          val = 0;
          if (flags & _LONG)
            {
              val = va_arg (ap, long);
            }
          else if (flags & _PTR)
            {
              str = va_arg (ap, void*);
              val = 0x00ffffffUL & *(unsigned long*)&str;
            }
          else if (flags & _CHAR)
            {
              val = va_arg (ap, char);
              if ((radix != 10) || (flags & _UNSIGNED))
                val = (unsigned char) val;    //Avoid unwanted sign extension
            }
          else
            {
              val = va_arg (ap, int);
              if ((radix != 10) || (flags & _UNSIGNED))
                val = (unsigned int) val;   //Avoid unwanted sign extension
            }

          str = buffer + 1; //Reserve space for a forced '+'
          if (radix)
            {
              if (flags & _UNSIGNED)
                _ultoa (val, buffer + 1, radix);
              else
                _ltoa (val, buffer + 1, radix);
#ifdef SIGN_MODIFIERS
              if ((flags & _PRINTSIGN) && (*str != '-'))
                {
                  --str;
                  *str = (flags & _PRINTBLANK ? ' ' : '+');
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
      cstr = str;
      while (fieldwidth && *cstr)
        {
          ++cstr;
          --fieldwidth;
        }
      //Left padding
      if (!(flags & _LEFTALIGN))
        {
          ch = flags & _ZEROPAD ? '0' : ' ';
          while (fieldwidth)
            {
              _PUTCHAR(ch);
              --fieldwidth;
            }
        }
#endif

#ifdef PRECISION
      while (*str && ((precision == _UNSET) || precision--))
#else
      while (*str)
#endif
        {
          ch = *str++;
          if (radix == 16 && !(flags & _UPCASE))
            {
              ch = tolower (ch);
            }
          _PUTCHAR(ch);
        }

#ifdef FIELD_WIDTH
      //Right padding (with spaces)
      if (flags & _LEFTALIGN)
        {
          while (fieldwidth)
            {
              _PUTCHAR(' ');
              --fieldwidth;
            }
        }
#endif

    }

_error:
  return EOF;
}
