/*-------------------------------------------------------------------------
   at89Sx051.h - Register Declarations for Atmel AT89S2051 and AT89S4051
   Processors

   Copyright (C) 2009, Ben Boeckel mathstuf AT gmail.com

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

/*-------------------------------------------------------------------------
   based on 8051.h (8051.h must be in mcs51 subdirectory)

   KEIL C compatible definitions are included
-------------------------------------------------------------------------*/

#ifndef AT89Sx051_H
#define AT89Sx051_H

#include <at89x051.h>     /* at89Sx051 adds to at89Cx051 definitions */

/* BYTE Register */
__sfr __at (0x8F) CLKREG ;
__sfr __at (0x97) ACSR   ;
__sfr __at (0xA9) SADDR  ;
__sfr __at (0xB7) IPH    ;
__sfr __at (0xB9) SADEN  ;

/* BIT register */
/* IE */
__sbit __at (0xAE) EC    ;

/* IP */
__sbit __at (0xBE) PC    ;

/* BIT definitions for bits that are not directly accessible */
/* PCON */
#define PWMEN            0x20
#define SMOD0            0x40
#define SMOD1            0x80

/* CLKREG */
#define X2               0x01
#define PWDEX            0x02

/* ACSR */
#define CM0              0x01
#define CM1              0x02
#define CM2              0x04
#define CEN              0x08
#define CF               0x10

/* IPH */
#define PX0H             0x01
#define PT0H             0x02
#define PX1H             0x04
#define PT1H             0x08
#define PSH              0x10
#define PCH              0x40


/* Interrupt numbers: address = (number * 8) + 3 */
#define CF0_VECTOR       6      /* 0x33 analog comparator */

#endif
