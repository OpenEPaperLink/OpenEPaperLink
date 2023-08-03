/*-------------------------------------------------------------------------
   i2copen.c

   Copyright (C) 2005, Vangelis Rokas <vrokas AT otenet.gr>

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

#include <pic18fregs.h>
#include <i2c.h>

void i2c_open(unsigned char mode, unsigned char slew, unsigned char addr_brd)
{
  SSPSTAT &= 0x3f;
  SSPCON1 = 0;
  SSPCON2 = 0;
  SSPCON1 |= mode;
  SSPSTAT |= slew;


#if    defined(__SDCC_PIC18F2455)  || defined(__SDCC_PIC18F2550) \
    || defined(__SDCC_PIC18F4455)  || defined(__SDCC_PIC18F4550) \
    || defined(__SDCC_PIC18F66J60) || defined(__SDCC_PIC18F66J65) || defined(__SDCC_PIC18F67J60) \
    || defined(__SDCC_PIC18F86J60) || defined(__SDCC_PIC18F86J65) || defined(__SDCC_PIC18F87J60) \
    || defined(__SDCC_PIC18F96J60) || defined(__SDCC_PIC18F96J65) || defined(__SDCC_PIC18F97J60)

  TRISBbits.TRISB1 = 1;
  TRISBbits.TRISB0 = 1;

#elif  defined(__SDCC_PIC18F24J50) || defined(__SDCC_PIC18F25J50) || defined(__SDCC_PIC18F26J50) \
    || defined(__SDCC_PIC18F44J50) || defined(__SDCC_PIC18F45J50) || defined(__SDCC_PIC18F46J50)

  TRISBbits.TRISB4 = 1;
  TRISBbits.TRISB5 = 1;

#else	/* all other devices */

  TRISCbits.TRISC3 = 1;
  TRISCbits.TRISC4 = 1;

#endif

  SSPADD = addr_brd;

  SSPCON1 |= 0x20;
}
