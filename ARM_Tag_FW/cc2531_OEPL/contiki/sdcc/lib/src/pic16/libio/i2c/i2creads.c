/*-------------------------------------------------------------------------
   i2creads.c

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


char i2c_readstr(_I2CPARAM_SPEC unsigned char *ptr, unsigned char len)
{
  unsigned char count=0;
  
  while( len-- ) {
    *ptr++ = i2c_readchar();
    
    while(SSPCON2bits.RCEN) {
      if(PIR2bits.BCLIF)return (-1);
      count++;
    
      if(len) {
        I2C_ACK();
        while(SSPCON2bits.ACKEN);
      }
    }
  }
  
  return count;
}
