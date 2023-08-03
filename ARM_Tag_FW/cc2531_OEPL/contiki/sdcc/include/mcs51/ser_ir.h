/*-------------------------------------------------------------------------
   ser_ir.h - header file for serial routines

   Copyright (C) 1999, Josef Wolf <jw AT raven.inka.de>

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

#ifndef __SER_IR_H
#define __SER_IR_H

/* call this one first on startup */
void ser_init (void);

/* the following ones should be obvious */
void ser_putc (unsigned char c);
void ser_puts (unsigned char *s);
void ser_gets (unsigned char *s, unsigned char len);
unsigned char ser_getc (void);

/* return the number of chars that can be received/transmitted without
* blocking.
*/
unsigned char ser_can_rcv (void);
unsigned char ser_can_xmt (void);

/* needs to be defined somewhere :-() */
void ser_handler (void) __interrupt 4;

#endif  /* __SER_IR_H */
