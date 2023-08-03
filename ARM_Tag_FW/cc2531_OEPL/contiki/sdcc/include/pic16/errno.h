/*-------------------------------------------------------------------------
   errno.h - Error codes used in the math functions

   Copyright (C) 2001, Jesus Calvino-Fraga jesusc@ieee.org 
   Ported to PIC16 port by Vangelis Rokas, 2004 <vrokas AT otenet.gr>

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

#ifndef _PIC16_ERRNO_H
#define _PIC16_ERRNO_H

/* link with C library */
#pragma library c

extern int errno;

/* Error Codes: */

#define EDOM        33  /* Math argument out of domain of functions */
#define ERANGE      34  /* Math result not representable */

#endif  /* _PIC16_ERRNO_H */
