/*-----------------------------------------------------------------
    printfl.c - source file for reduced version of printf

   Copyright (C) 1999, Sandeep Dutta . sandeep.dutta@usa.net
   2001060401: Improved by was@icb.snz.chel.su

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

/* following formats are supported :-
   format     output type       argument-type
     %d        decimal             int
     %ld       decimal             long
     %hd       decimal             char
     %x        hexadecimal         int
     %lx       hexadecimal         long
     %hx       hexadecimal         char
     %o        octal               int
     %lo       octal               long
     %ho       octal               char
     %c        character           char
     %s        character           generic pointer
*/

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static __data char radix ;
static __bit  long_flag = 0;
static __bit  string_flag =0;
static __bit  char_flag = 0;
static char * __data str ;
static __data long val;

/* This great loop fails with the ds390 port (2003-01-13).

   At the beginning resp. end of the loop the compiler inserts a "push ar2"
   resp. "pop ar2", which badly interferes with the push/pop in the source.

   Library functions should be rock solid and portable. There's an __ltoa in
   the library, so let's use it and don't reinvent the wheel.

   Bernhard
*/

#if NICE_LIFO_IMPLEMENTATION_BUT_NOT_PORTABLE
/* just for the SP */
#include <8051.h>

static __data volatile char ch;
static __bit sign;

static void pval(void)
{
        volatile char sp;
        unsigned long lval;
        sp = SP;

        if (val < 0 && radix != 16)
        {
           lval = -val;
           sign = 1;
        }
        else { sign = 0; lval = val;}

        if (!long_flag) {
          lval &= 0x0000ffff;
        }
        if (char_flag) {
          lval &= 0x000000ff;
        }

        do
        {

#  if 1
                if(radix != 16)  ch = (lval % radix) + '0';
                else ch = "0123456789ABCDEF"[(unsigned char)lval & 0x0f];
                __asm push _ch __endasm;
                lval /= radix;
#  else
                // This only looks more efficient, but isn't. see the .map
                ch = (lval % radix) + '0';
                if (ch>'9') ch+=7;
                __asm push _ch __endasm;
                lval /= radix;
#  endif
        }
        while (lval);

        if (sign) {
                ch = '-';
                __asm push _ch __endasm;
        }

        while (sp != SP) {
                __asm pop _ch __endasm;
                putchar(ch);
        }
}
#endif

void printf_small (char * fmt, ... ) __reentrant
{
    va_list ap ;

    va_start(ap,fmt);

    for (; *fmt ; fmt++ ) {
        if (*fmt == '%') {
            long_flag = string_flag = char_flag = 0;
            fmt++ ;
            switch (*fmt) {
            case 'l':
                long_flag = 1;
                fmt++;
                break;
            case 'h':
                char_flag = 1;
                fmt++;
            }

            switch (*fmt) {
            case 's':
                string_flag = 1;
                break;
            case 'd':
                radix = 10;
                break;
            case 'x':
                radix = 16;
                break;
            case 'c':
                radix = 0;
                break;
            case 'o':
                radix = 8;
                break;
            }

            if (string_flag) {
                str = va_arg(ap, char *);
                while (*str) putchar(*str++);
                continue ;
            }

            if (long_flag)
                val = va_arg(ap,long);
            else
                if (char_flag)
                    val = va_arg(ap,char);
                else
                    val = va_arg(ap,int);

#if NICE_LIFO_IMPLEMENTATION_BUT_NOT_PORTABLE
            if (radix) pval();
#else
            if (radix)
            {
              static char __idata buffer[12]; /* 37777777777(oct) */
              char __idata * stri;

              __ltoa (val, buffer, radix);
              stri = buffer;
              while (*stri)
                {
                  putchar (*stri);
                  stri++;
                }
            }
#endif
            else
              putchar((char)val);

        } else
            putchar(*fmt);
    }
}
