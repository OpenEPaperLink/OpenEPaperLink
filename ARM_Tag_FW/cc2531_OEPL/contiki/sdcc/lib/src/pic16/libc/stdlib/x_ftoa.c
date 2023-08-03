/*-------------------------------------------------------------------------
   x_ftoa.c - wrapper function to use _convert_float

   Copyright (C) 2004, Vangelis Rokas <vrokas at otenet.gr>

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

#include <float.h>

extern int convert_frac;
extern int convert_int;

/* char x_ftoa(float, __data char *, unsigned char, unsigned char); */


extern int POSTDEC1;
extern int PLUSW2;
extern int FSR0L;
extern int FSR0H;
extern int PREINC1;
extern int PREINC2;
extern int FSR2L;
extern int FSR2H;

#define _vv0x00	0x00
#define _vv0x01	0x01
#define _vv0x02	0x02
#define _vv0x03	0x03
#define _vv0x04	0x04

/* do not warn about unreferenced arguments/missing return values */
#pragma save
#pragma disable_warning 59
#pragma disable_warning 85

char x_cnvint_wrap(unsigned long num, __data char *buffer)
{
  __asm
    movff	_vv0x00, _POSTDEC1
    movff	_vv0x01, _POSTDEC1
    movff	_vv0x02, _POSTDEC1
    movff	_vv0x03, _POSTDEC1

    movlw	2
    movff	_PLUSW2, _vv0x00
    movlw	3
    movff	_PLUSW2, _vv0x01
    movlw	4
    movff	_PLUSW2, _vv0x02
    movlw	5
    movff	_PLUSW2, _vv0x03

    movlw	6
    movff	_PLUSW2, _FSR0L
    movlw	7
    movff	_PLUSW2, _FSR0H

    call	_convert_int

    /* return value is already in WREG */

    movff	_PREINC1, _vv0x03
    movff	_PREINC1, _vv0x02
    movff	_PREINC1, _vv0x01
    movff	_PREINC1, _vv0x00
  __endasm ;
}

char x_cnvfrac_wrap(unsigned long num, __data char *buffer, unsigned char prec)
{
  num;
  buffer;
  prec;

  __asm
    movff	_vv0x00, _POSTDEC1
    movff	_vv0x01, _POSTDEC1
    movff	_vv0x02, _POSTDEC1
    movff	_vv0x03, _POSTDEC1
    movff	_vv0x04, _POSTDEC1

    movlw	2
    movff	_PLUSW2, _vv0x00
    movlw	3
    movff	_PLUSW2, _vv0x01
    movlw	4
    movff	_PLUSW2, _vv0x02
    movlw	5
    movff	_PLUSW2, _vv0x03

    movlw	6
    movff	_PLUSW2, _FSR0L
    movlw	7
    movff	_PLUSW2, _FSR0H

    movlw	8
    movff	_PLUSW2, _vv0x04

    call	_convert_frac

    /* return value is already in WREG */

    movff	_PREINC1, _vv0x04
    movff	_PREINC1, _vv0x03
    movff	_PREINC1, _vv0x02
    movff	_PREINC1, _vv0x01
    movff	_PREINC1, _vv0x00
  __endasm ;
}
#pragma restore


union float_long {
  unsigned long l;
  float f;
};

char x_ftoa(float num, __data char *buffer, unsigned char buflen, unsigned char prec)
{
  char len;
  signed char expn;
  unsigned long ll;
  unsigned long li;
//  volatile
  union float_long f_l;

    len = buflen;
    while(len--)buffer[len] = 0;

    f_l.f = num;

    if((f_l.l & SIGNBIT) == SIGNBIT) {
      f_l.l &= ~SIGNBIT;
      *buffer = '-';
      buffer++;
    }

    expn = EXCESS - EXP(f_l.l);	// - 24;

    ll = MANT(f_l.l);
    li = 0;

    while( expn ) {
      if(expn < 0) {
        li <<= 1;
        if(ll & 0x00800000UL)li |= 1;
        ll <<= 1;
        expn++;
      } else {
        ll >>= 1;
        expn--;
      }
    }

    if(li)
      len = x_cnvint_wrap(li, buffer);
    else {
      *buffer = '0'; len = 1;
    }

    buffer += len;

    if(prec) {
      *buffer = '.'; len++;
      buffer++;

      len += x_cnvfrac_wrap(ll, buffer, 24-prec);
      buffer[ prec ] = '\0';
    }

  return (len);
}
