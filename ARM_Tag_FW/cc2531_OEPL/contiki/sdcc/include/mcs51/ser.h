/*-------------------------------------------------------------------------
   ser.h - this file is the header to be included by modules which use the
           ser.c module.

   Copyright (C) 1999, Sandeep Dutta <sandeep.dutta AT ieee.org>

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

/*KA******************************************************************
* PROJECT: PLOne/8052
**********************************************************************
* FILE: ser.h
**********************************************************************
* CHANGES:
* date      author            description
* --------------------------------------------------------------------
* 04/26/99  we                update
* 04/27/99  we                add comments/header
**********************************************************************
* DESCRIPTION:
* This file is the header to be included by modules which use the
* ser.c module.
**********************************************************************
* FUNCTIONS DECLARED:
* see ser.c
**********************************************************************
* COMPILE TIME OPTIONS: -
* DEBUG OPTIONS: -
******************************************************************KE*/

#ifndef _SER_H_
#define _SER_H_

void ser_init(void);
void ser_interrupt_handler(void) __interrupt 4 __using 1;
void ser_putc(unsigned char);
unsigned char ser_getc(void);
void ser_printString(char *String);
char ser_charAvail(void);

/*********************End of File************************************/
#endif
