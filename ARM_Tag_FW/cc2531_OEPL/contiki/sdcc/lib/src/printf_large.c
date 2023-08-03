/*-----------------------------------------------------------------
   printf_large.c - formatted output conversion

   Copyright (C) 1999, Martijn van Balen <aed AT iae.nl>
   Added %f By - <johan.knol AT iduna.nl> (2000)
   Refactored by - Maarten Brock (2004)

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

#if defined (__SDCC_ds390) || defined (__SDCC_USE_XSTACK) || defined (__SDCC_MODEL_HUGE)
#define USE_FLOATS 1
#endif

#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <sdcc-lib.h>

#define PTR value.ptr

#ifdef __SDCC_ds390
#define NULL_STRING "<NULL>"
#define NULL_STRING_LENGTH 6
#endif

#if defined (__SDCC_mcs51) && defined (__SDCC_MODEL_SMALL) && !defined (__SDCC_STACK_AUTO)
# define MEM_SPACE_BUF __idata
# define MEM_SPACE_BUF_PP __idata
#else
# define MEM_SPACE_BUF
# define MEM_SPACE_BUF_PP _AUTOMEM
#endif

/****************************************************************************/

//typedef const char * ptr_t;
#define ptr_t const char *

#ifdef toupper
#undef toupper
#endif
#ifdef tolower
#undef tolower
#endif
#ifdef islower
#undef islower
#endif
#ifdef isdigit
#undef isdigit
#endif

//#define toupper(c) ((c)&=~0x20)
#define toupper(c) ((c)&=0xDF)
#define tolower(c) ((c)|=0x20)
#define islower(c) ((unsigned char)c >= (unsigned char)'a' && (unsigned char)c <= (unsigned char)'z')
#define isdigit(c) ((unsigned char)c >= (unsigned char)'0' && (unsigned char)c <= (unsigned char)'9')

typedef union
{
  unsigned char  byte[5];
  long           l;
  unsigned long  ul;
  float          f;
  const char     *ptr;
} value_t;

#ifndef __SDCC_STACK_AUTO
  static bool lower_case;
  static pfn_outputchar output_char;
  static void* p;
  static value_t value;
  static int charsOutputted;
#endif

/****************************************************************************/

#ifdef __SDCC_STACK_AUTO
  #define OUTPUT_CHAR(c, p) { output_char (c, p); charsOutputted++; }
#else
  #define OUTPUT_CHAR(c, p) _output_char (c)
  static void
  _output_char (unsigned char c)
  {
    output_char( c, p );
    charsOutputted++;
  }
#endif

/*--------------------------------------------------------------------------*/

#ifdef __SDCC_STACK_AUTO
  static void
  output_digit (unsigned char n, bool lower_case, pfn_outputchar output_char, void* p)
  {
    register unsigned char c = n + (unsigned char)'0';

    if (c > (unsigned char)'9')
    {
      c += (unsigned char)('A' - '0' - 10);
      if (lower_case)
         c += (unsigned char)('a' - 'A');
    }
    output_char( c, p );
  }
#else
  static void
  output_digit (unsigned char n)
  {
    register unsigned char c = n + (unsigned char)'0';

    if (c > (unsigned char)'9')
    {
      c += (unsigned char)('A' - '0' - 10);
      if (lower_case)
         c = tolower(c);
    }
    _output_char( c );
  }
#endif

/*--------------------------------------------------------------------------*/

#ifdef __SDCC_STACK_AUTO
  #define OUTPUT_2DIGITS( B )   { output_2digits( B, lower_case, output_char, p ); charsOutputted += 2; }
  static void
  output_2digits (unsigned char b, bool lower_case, pfn_outputchar output_char, void* p)
  {
    output_digit( b>>4,   lower_case, output_char, p );
    output_digit( b&0x0F, lower_case, output_char, p );
  }
#else
  #define OUTPUT_2DIGITS( B )   output_2digits( B )
  static void
  output_2digits (unsigned char b)
  {
    output_digit( b>>4   );
    output_digit( b&0x0F );
  }
#endif

/*--------------------------------------------------------------------------*/

#if defined __SDCC_STACK_AUTO
static void
calculate_digit (value_t _AUTOMEM * value, unsigned char radix)
{
  unsigned long ul = value->ul;
  unsigned char _AUTOMEM * pb4 = &value->byte[4];
  unsigned char i = 32;

  do
  {
    *pb4 = (*pb4 << 1) | ((ul >> 31) & 0x01);
    ul <<= 1;

    if (radix <= *pb4 )
    {
      *pb4 -= radix;
      ul |= 1;
    }
  } while (--i);
  value->ul = ul;
}
#else
static void
calculate_digit (unsigned char radix)
{
  register unsigned long ul = value.ul;
  register unsigned char b4 = value.byte[4];
  register unsigned char i = 32;

  do
  {
    b4 = (b4 << 1);
    b4 |= (ul >> 31) & 0x01;
    ul <<= 1;

    if (radix <= b4 )
    {
      b4 -= radix;
      ul |= 1;
    }
  } while (--i);
  value.ul = ul;
  value.byte[4] = b4;
}
#endif

#if USE_FLOATS

/* This is a very inefficient but direct approach, since we have no math
   library yet (e.g. log()).
   It does most of the modifiers, but has some restrictions. E.g. the
   abs(float) shouldn't be bigger than an unsigned long (that's
   about 4294967295), but still makes it usefull for most real-life
   applications.
*/

#define DEFAULT_FLOAT_PRECISION 6

#ifdef __SDCC_STACK_AUTO
#define OUTPUT_FLOAT(F, W, D, L, Z, S, P)       output_float(F, W, D, L, Z, S, P, output_char, p)
static unsigned char
output_float (float f, unsigned char reqWidth,
              signed char reqDecimals,
              bool left, bool zero, bool sign, bool space,
              pfn_outputchar output_char, void* p)
{
  unsigned char charsOutputted = 0;
 #if defined (__SDCC_mcs51)
  char fpBuffer[16];      //mcs51 has only a small stack
 #else
  char fpBuffer[128];
 #endif
#else
#define OUTPUT_FLOAT(F, W, D, L, Z, S, P)       output_float(F, W, D, L, Z, S, P)
static void
output_float (float f, unsigned char reqWidth,
              signed char reqDecimals,
              bool left, bool zero, bool sign, bool space)
{
  __xdata char fpBuffer[128];
#endif //__SDCC_STACK_AUTO
  bool negative = 0;
  unsigned long integerPart;
  float rounding;
  float decimalPart;
  char fpBI=0, fpBD;
  unsigned char minWidth, i;
  signed char exp = -128;

  // save the sign
  if (f<0)
  {
    negative=1;
    f=-f;
  }

  if (f>0x00ffffff)
  {
    // this part is from Frank van der Hulst

    for (exp = 0; f >= 10.0; exp++) f /=10.0;
    for (       ; f < 1.0;   exp--) f *=10.0;

    if (negative)
    {
      OUTPUT_CHAR ('-', p);
    }
    else
    {
      if (sign)
      {
        OUTPUT_CHAR ('+', p);
      }
    }
    reqWidth = 0;
    left = 0;
    zero = 0;
    sign = 0;
    space = 0;
  }

  // display some decimals as default
  if (reqDecimals==-1)
    reqDecimals=DEFAULT_FLOAT_PRECISION;

  // round the float
  rounding = 0.5;
  for (i=reqDecimals; i>0; i--)
  {
      rounding /= 10.0;
  }
  f += rounding;

  // split the float
  integerPart = f;
  decimalPart = f - integerPart;

  // fill the buffer with the integerPart (in reversed order!)
  while (integerPart)
  {
    fpBuffer[fpBI++]='0' + integerPart%10;
    integerPart /= 10;
  }
  if (!fpBI)
  {
    // we need at least a 0
    fpBuffer[fpBI++]='0';
  }

  // fill buffer with the decimalPart (in normal order)
  fpBD=fpBI;

  for (i=reqDecimals; i>0; i--)
  {
      decimalPart *= 10.0;
      // truncate the float
      integerPart = decimalPart;
      fpBuffer[fpBD++] = '0' + integerPart;
      decimalPart -= integerPart;
  }

  minWidth=fpBI; // we need at least these
  minWidth+=reqDecimals?reqDecimals+1:0; // maybe these
  if (negative || sign || space)
    minWidth++; // and maybe even this :)

  if (!left && reqWidth>i)
  {
    if (zero)
    {
      if (negative)
      {
        OUTPUT_CHAR('-', p);
      }
      else if (sign)
      {
        OUTPUT_CHAR('+', p);
      }
      else if (space)
      {
        OUTPUT_CHAR(' ', p);
      }
      while (reqWidth-->minWidth)
      {
        OUTPUT_CHAR('0', p);
      }
    }
    else
    {
      while (reqWidth-->minWidth)
      {
        OUTPUT_CHAR(' ', p);
      }
      if (negative)
      {
        OUTPUT_CHAR('-', p);
      }
      else if (sign)
      {
        OUTPUT_CHAR('+', p);
      }
      else if (space)
      {
        OUTPUT_CHAR(' ', p);
      }
    }
  }
  else
  {
    if (negative)
    {
      OUTPUT_CHAR('-', p);
    }
    else if (sign)
    {
      OUTPUT_CHAR('+', p);
    }
    else if (space)
    {
      OUTPUT_CHAR(' ', p);
    }
  }

  // output the integer part
  i=fpBI-1;
  do {
    OUTPUT_CHAR (fpBuffer[i], p);
  } while (i--);

  // ouput the decimal part
  if (reqDecimals)
  {
    OUTPUT_CHAR ('.', p);
    i=fpBI;
    while (reqDecimals--)
    {
      OUTPUT_CHAR (fpBuffer[i++], p);
    }
  }

  if (left && reqWidth>minWidth)
  {
    while (reqWidth-->minWidth)
    {
      OUTPUT_CHAR(' ', p);
    }
  }

  if (exp != -128)
  {
    OUTPUT_CHAR ('e', p);
    if (exp<0)
    {
      OUTPUT_CHAR ('-', p);
      exp = -exp;
    }
    OUTPUT_CHAR ('0'+exp/10, p);
    OUTPUT_CHAR ('0'+exp%10, p);
  }
#ifdef __SDCC_STACK_AUTO
  return charsOutputted;
#else
  return;
#endif //__SDCC_STACK_AUTO
}
#endif //USE_FLOATS

int
_print_format (pfn_outputchar pfn, void* pvoid, const char *format, va_list ap)
{
  bool   left_justify;
  bool   zero_padding;
  bool   prefix_sign;
  bool   prefix_space;
  bool   signed_argument;
  bool   char_argument;
  bool   long_argument;
  bool   float_argument;
#ifdef __SDCC_STACK_AUTO
  bool   lower_case;
  value_t value;
  int charsOutputted;
#endif
  bool   lsd;

  unsigned char radix;
  size_t  width;
  int decimals;
  size_t  length;
  char           c;

#ifdef __SDCC_STACK_AUTO
  #define output_char   pfn
  #define p             pvoid
#else
  output_char = pfn;
  p = pvoid;
#endif

  // reset output chars
  charsOutputted = 0;

#ifdef __SDCC_ds390
  if (format==0)
  {
    format=NULL_STRING;
  }
#endif

  while( c=*format++ )
  {
    if ( c=='%' )
    {
      left_justify    = 0;
      zero_padding    = 0;
      prefix_sign     = 0;
      prefix_space    = 0;
      signed_argument = 0;
      char_argument   = 0;
      long_argument   = 0;
      float_argument  = 0;
      radix           = 0;
      width           = 0;
      decimals        = -1;

get_conversion_spec:

      c = *format++;

      if (c=='%')
      {
        OUTPUT_CHAR(c, p);
        continue;
      }

      if (isdigit(c))
      {
        if (decimals==-1)
        {
          width = 10*width + c - '0';
          if (width == 0)
          {
            /* first character of width is a zero */
            zero_padding = 1;
          }
        }
        else
        {
          decimals = 10*decimals + c - '0';
        }
        goto get_conversion_spec;
      }

      if (c=='.')
      {
        if (decimals==-1)
          decimals=0;
        else
          ; // duplicate, ignore
        goto get_conversion_spec;
      }

      if (islower(c))
      {
        c = toupper(c);
        lower_case = 1;
      }
      else
        lower_case = 0;

      switch( c )
      {
      case '-':
        left_justify = 1;
        goto get_conversion_spec;
      case '+':
        prefix_sign = 1;
        goto get_conversion_spec;
      case ' ':
        prefix_space = 1;
        goto get_conversion_spec;
      case 'B': /* byte */
        char_argument = 1;
        goto get_conversion_spec;
//      case '#': /* not supported */
      case 'H': /* short */
      case 'J': /* intmax_t */
      case 'T': /* ptrdiff_t */
      case 'Z': /* size_t */
        goto get_conversion_spec;
      case 'L': /* long */
        long_argument = 1;
        goto get_conversion_spec;

      case 'C':
        if( char_argument )
          c = va_arg(ap,char);
        else
          c = va_arg(ap,int);
        OUTPUT_CHAR( c, p );
        break;

      case 'S':
        PTR = va_arg(ap,ptr_t);

#ifdef __SDCC_ds390
        if (PTR==0)
        {
          PTR=NULL_STRING;
          length=NULL_STRING_LENGTH;
        }
        else
        {
          length = strlen(PTR);
        }
#else
        length = strlen(PTR);
#endif
        if ( decimals == -1 )
        {
          decimals = length;
        }
        if ( ( !left_justify ) && (length < width) )
        {
          width -= length;
          while( width-- != 0 )
          {
            OUTPUT_CHAR( ' ', p );
          }
        }

        while ( (c = *PTR)  && (decimals-- > 0))
        {
          OUTPUT_CHAR( c, p );
          PTR++;
        }

        if ( left_justify && (length < width))
        {
          width -= length;
          while( width-- != 0 )
          {
            OUTPUT_CHAR( ' ', p );
          }
        }
        break;

      case 'P':
        PTR = va_arg(ap,ptr_t);

#if defined (__SDCC_ds390)
        {
          unsigned char memtype = value.byte[3];
          if (memtype >= 0x80)
            c = 'C';
          else if (memtype >= 0x60)
            c = 'P';
          else if (memtype >= 0x40)
            c = 'I';
          else
            c = 'X';
        }
        OUTPUT_CHAR(c, p);
        OUTPUT_CHAR(':', p);
        OUTPUT_CHAR('0', p);
        OUTPUT_CHAR('x', p);
        OUTPUT_2DIGITS( value.byte[2] );
        OUTPUT_2DIGITS( value.byte[1] );
        OUTPUT_2DIGITS( value.byte[0] );
#elif defined (__SDCC_mcs51)
        {
          unsigned char memtype = value.byte[2];
          if (memtype >= 0x80)
            c = 'C';
          else if (memtype >= 0x60)
            c = 'P';
          else if (memtype >= 0x40)
            c = 'I';
          else
            c = 'X';
        }
        OUTPUT_CHAR(c, p);
        OUTPUT_CHAR(':', p);
        OUTPUT_CHAR('0', p);
        OUTPUT_CHAR('x', p);
        if ((c != 'I' /* idata */) &&
            (c != 'P' /* pdata */))
        {
          OUTPUT_2DIGITS( value.byte[1] );
        }
        OUTPUT_2DIGITS( value.byte[0] );
#else
        OUTPUT_CHAR('0', p);
        OUTPUT_CHAR('x', p);
        OUTPUT_2DIGITS( value.byte[1] );
        OUTPUT_2DIGITS( value.byte[0] );
#endif
        break;

      case 'D':
      case 'I':
        signed_argument = 1;
        radix = 10;
        break;

      case 'O':
        radix = 8;
        break;

      case 'U':
        radix = 10;
        break;

      case 'X':
        radix = 16;
        break;

      case 'F':
        float_argument=1;
        break;

      default:
        // nothing special, just output the character
        OUTPUT_CHAR( c, p );
        break;
      }

      if (float_argument)
      {
        value.f = va_arg(ap, float);
#if !USE_FLOATS
        PTR="<NO FLOAT>";
        while (c=*PTR++)
        {
          OUTPUT_CHAR (c, p);
        }
        // treat as long hex
        //radix=16;
        //long_argument=1;
        //zero_padding=1;
        //width=8;
#else
        // ignore b and l conversion spec for now
#ifdef __SDCC_STACK_AUTO
        charsOutputted += OUTPUT_FLOAT(value.f, width, decimals, left_justify,
                                     zero_padding, prefix_sign, prefix_space);
#else
        OUTPUT_FLOAT(value.f, width, decimals, left_justify,
                     zero_padding, prefix_sign, prefix_space);
#endif //__SDCC_STACK_AUTO
#endif //USE_FLOATS
      }
      else if (radix != 0)
      {
        // Apparently we have to output an integral type
        // with radix "radix"
        unsigned char MEM_SPACE_BUF store[6];
        unsigned char MEM_SPACE_BUF_PP *pstore = &store[5];

        // store value in byte[0] (LSB) ... byte[3] (MSB)
        if (char_argument)
        {
          value.l = va_arg(ap, char);
          if (!signed_argument)
          {
            value.l &= 0xFF;
          }
        }
        else if (long_argument)
        {
          value.l = va_arg(ap, long);
        }
        else // must be int
        {
          value.l = va_arg(ap, int);
          if (!signed_argument)
          {
            value.l &= 0xFFFF;
          }
        }

        if ( signed_argument )
        {
          if (value.l < 0)
            value.l = -value.l;
          else
            signed_argument = 0;
        }

        length=0;
        lsd = 1;

        do {
          value.byte[4] = 0;
#if defined __SDCC_STACK_AUTO
          calculate_digit(&value, radix);
#else
          calculate_digit(radix);
#endif
          if (!lsd)
          {
            *pstore = (value.byte[4] << 4) | (value.byte[4] >> 4) | *pstore;
            pstore--;
          }
          else
          {
            *pstore = value.byte[4];
          }
          length++;
          lsd = !lsd;
        } while( value.ul );

        if (width == 0)
        {
          // default width. We set it to 1 to output
          // at least one character in case the value itself
          // is zero (i.e. length==0)
          width = 1;
        }

        /* prepend spaces if needed */
        if (!zero_padding && !left_justify)
        {
          while ( width > (unsigned char) (length+1) )
          {
            OUTPUT_CHAR( ' ', p );
            width--;
          }
        }

        if (signed_argument) // this now means the original value was negative
        {
          OUTPUT_CHAR( '-', p );
          // adjust width to compensate for this character
          width--;
        }
        else if (length != 0)
        {
          // value > 0
          if (prefix_sign)
          {
            OUTPUT_CHAR( '+', p );
            // adjust width to compensate for this character
            width--;
          }
          else if (prefix_space)
          {
            OUTPUT_CHAR( ' ', p );
            // adjust width to compensate for this character
            width--;
          }
        }

        /* prepend zeroes/spaces if needed */
        if (!left_justify)
        {
          while ( width-- > length )
          {
            OUTPUT_CHAR( zero_padding ? '0' : ' ', p );
          }
        }
        else
        {
          /* spaces are appended after the digits */
          if (width > length)
            width -= length;
          else
            width = 0;
        }

        /* output the digits */
        while( length-- )
        {
          lsd = !lsd;
          if (!lsd)
          {
            pstore++;
            value.byte[4] = *pstore >> 4;
          }
          else
          {
            value.byte[4] = *pstore & 0x0F;
          }
#ifdef __SDCC_STACK_AUTO
          output_digit( value.byte[4], lower_case, output_char, p );
          charsOutputted++;
#else
          output_digit( value.byte[4] );
#endif
        }
        if (left_justify)
        {
          while (width-- > 0)
          {
            OUTPUT_CHAR(' ', p);
          }
        }
      }
    }
    else
    {
      // nothing special, just output the character
      OUTPUT_CHAR( c, p );
    }
  }

  return charsOutputted;
}

/****************************************************************************/
