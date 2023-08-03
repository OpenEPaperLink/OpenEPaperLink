/*-------------------------------------------------------------------------
   serial_IO.h - Default putchar() and getchar() to the serial port

   Copyright (C) 2006, Jesus Calvino-Fraga / jesusc at ece.ubc.ca

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

#ifndef SERIAL_IO_H
#define SERIAL_IO_H

__sfr __at (0x87) SIO_PCON;
__sfr __at (0x89) SIO_TMOD;
__sfr __at (0x8D) SIO_TH1;
__sfr __at (0x8B) SIO_TL1;
__sfr __at (0x98) SIO_SCON;
__sfr __at (0x99) SIO_SBUF;
__sbit __at (0x8E) SIO_TR1;

/*SCON bits*/
__sbit __at (0x98) SIO_RI;
__sbit __at (0x99) SIO_TI;
__sbit __at (0x9A) SIO_RB8;
__sbit __at (0x9B) SIO_TB8;
__sbit __at (0x9C) SIO_REN;
__sbit __at (0x9D) SIO_SM2;
__sbit __at (0x9E) SIO_SM1;
__sbit __at (0x9F) SIO_SM0;

void inituart (unsigned char t1_reload)
{
	SIO_TR1=0;
	SIO_TMOD=(SIO_TMOD&0x0f)|0x20;
	SIO_PCON|=0x80;
	SIO_TH1=SIO_TL1=t1_reload;
	SIO_TR1=1;
	SIO_SCON=0x52;
}

void putchar (char c)
{
	if((!SIO_SM0)&&(!SIO_SM1)) inituart(0xff);
	if (c=='\n')
	{
		while (!SIO_TI);
		SIO_TI=0;
		SIO_SBUF='\r';
	}
	while (!SIO_TI);
	SIO_TI=0;
	SIO_SBUF=c;
}

char getchar (void)
{
	char c;
	
	if((!SIO_SM0)&&(!SIO_SM1)) inituart(0xff);

	while (!SIO_RI);
	SIO_RI=0;
	c=SIO_SBUF;
	return c;
}
#endif
