/*-------------------------------------------------------------------------
   uputc - write a character to USART

   Copyright (C) 2004, Vangelis Rokas <vrokas AT otenet.gr>

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

/*
 * Devices implemented:
 *	PIC18F[24][45][28]
 */


#include <pic18fregs.h>
#include <usart.h>

extern union USART USART_Status;

void usart_putc(unsigned char dat) __wparam __naked
{
#if 0
  if(TXSTAbits.TX9) {
    TXSTAbits.TX9D = 0;
    if(USART_Status.TX_NINE)TXSTAbits.TX9D = 1;
  }

  TXREG = dat;      // Write the data byte to the USART
#else
  dat;
  __asm
    btfss       _TXSTAbits, 6
    bra         _01_
    
    bcf         _TXSTAbits, 0
    banksel     _USART_Status
    btfsc       _USART_Status, 1, b
    bsf         _TXSTAbits, 0

_01_:
    movwf       _TXREG
    return
  __endasm;
#endif
}
