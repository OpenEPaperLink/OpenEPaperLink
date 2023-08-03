/*-------------------------------------------------------------------------
   rtc390.c - rtc routines for the DS1315 (tested on TINI)

   Copyright (C) 2001, Johan Knol <johan.knol AT iduna.nl>

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

#include <tinibios.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>

/* this is the address of the ds1315 phantom time chip, although
   it doesn't really matter as long as it's in the 300000-3fffff
   range since the chip only uses CE3*
*/

__xdata __at (0x310000) static volatile unsigned char rtc;

// this is the 64bit pattern that has to be recognized by the ds1315
__code unsigned char rtcMagic[8]={0xc5,0x3a,0xa3,0x5c,0xc5,0x3a,0xa3,0x5c};

#define BCDtoINT(x) (((x)&0x0f)+((x)>>4)*10)
#define INTtoBCD(x) (((x)%10)+(((x)/10)<<4))

static void RtcSync(void) {
  unsigned char dummy, byte,bitMask;

  // reset rtc chip
  dummy=rtc;

  // say the magic word
  for (byte=0; byte<8; byte++) {
    for (bitMask=0x01; bitMask; bitMask<<=1) {
      rtc = (rtcMagic[byte]&bitMask) ? 0xff : 0x00;
    }
  }
}

unsigned char RtcRead(struct tm *rtcDate) {
  unsigned char rtcBytes[8];
  unsigned char byte,bitMask;

  RtcSync();

  for (byte=0; byte<8; byte++) {
    rtcBytes[byte]=0;
    for (bitMask=0x01; bitMask; bitMask<<=1) {
      if (rtc&1) {
	rtcBytes[byte]|=bitMask;
      }
    }
  }
  rtcDate->tm_year=BCDtoINT(rtcBytes[7])+100; // year since 1900
  rtcDate->tm_mon=BCDtoINT(rtcBytes[6])-1; // jan=0
  rtcDate->tm_mday=BCDtoINT(rtcBytes[5]);
  rtcDate->tm_wday=(rtcBytes[4]&0x07)-1; // monday=0?
  rtcDate->tm_hour=BCDtoINT(rtcBytes[3]);
  rtcDate->tm_min=BCDtoINT(rtcBytes[2]);
  rtcDate->tm_sec=BCDtoINT(rtcBytes[1]);
  rtcDate->tm_hundredth=BCDtoINT(rtcBytes[0]);
  if ((rtcBytes[4]&0x30) || (rtcBytes[3]&0x80)) {
    //oscillator not running, reset not active or in 12h mode
    return 0;
  }
  return 1;
}

void RtcWrite(struct tm *rtcDate) {
  unsigned char rtcBytes[8];
  unsigned char byte,bitMask;

  rtcBytes[7]=INTtoBCD(rtcDate->tm_year%100);
  rtcBytes[6]=INTtoBCD(rtcDate->tm_mon)+1;
  rtcBytes[5]=INTtoBCD(rtcDate->tm_mday);
  rtcBytes[4]=(INTtoBCD(rtcDate->tm_wday)+1)&0x07; //set 24h  mode
  rtcBytes[3]=INTtoBCD(rtcDate->tm_hour)&0x3f; // oscilator on, reset on
  rtcBytes[2]=INTtoBCD(rtcDate->tm_min);
  rtcBytes[1]=INTtoBCD(rtcDate->tm_sec);
  //rtcBytes[0]=INTtoBCD(rtcDate->hundredth);
  rtcBytes[0]=0;

  RtcSync();

  for (byte=0; byte<8; byte++) {
    for (bitMask=0x01; bitMask; bitMask<<=1) {
      rtc = (rtcBytes[byte]&bitMask) ? 0xff : 0x00;
    }
  }
}
