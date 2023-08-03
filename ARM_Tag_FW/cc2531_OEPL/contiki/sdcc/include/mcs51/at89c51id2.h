/*-------------------------------------------------------------------------
   at89c51id2.h: Register Declarations for the Atmel AT89C51ID2 Processor

   Copyright (C) 2014, Victor Munoz / victor at munoz.name

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

#ifndef REG_AT89C51ID2_H
#define REG_AT89C51ID2_H

#include <at89c51ed2.h>     // Load definitions for the at89c51ed2.h

//Clock control Register 0  CKCON0(0x8F) additional definitions
    #define TWIX2   0x80    // 2-wire clock (CPU clock X2 only) '1'=12 ck/cy, '0'=6 ck/cy

// Two wire interface control registers
__sfr __at (0x93) SSCON;    // Synchronous Serial Control register (93h)
    #define CR2     0x80    // Control Rate bit 2
    #define SSIE    0x40    // Synchronous Serial Interface Enable bit
    #define STA     0x20    // Start flag
    #define STO     0x10    // Stop flag
    #define SI      0x08    // Synchronous Serial Interrupt flag
    #define AA      0x04    // Assert Acknowledge flag
    #define CR1     0x02    // Control Rate bit 1
    #define CR0     0x01    // Control Rate bit 0
__sfr __at (0x94) SSCS;     // Synchronous Serial Control and Status Register (read) (094h)
    #define SC4     0x80    // Status Code bit 4
    #define SC3     0x40    // Status Code bit 3
    #define SC2     0x20    // Status Code bit 2
    #define SC1     0x10    // Status Code bit 1
    #define SC0     0x08    // Status Code bit 0
__sfr __at (0x95) SSDAT;    // Synchronous Serial Data register (read/write) (095h)
__sfr __at (0x96) SSADR;    // Synchronous Serial Address Register (read/write) (096h)
    #define GC      0x01    // General Call bit

//Interrupt Enable 1  IEN1(0xB1) additional definitions
	#define ETWI    0x02	//  Two Wire Interrupt Enable bit.

//Interrupt Priority 1 LOW  IPL1(0xB2) additional definitions
    #define TWIL    0x02

//Interrupt Priority 1 HIGH  IPH1(0xB3) additional definitions
    #define TWIH    0x02

/* Interrupt numbers: address = (number * 8) + 3 */
#define TWI_VECTOR      8       /* 0x43 Two wire interface */

#endif /*REG_AT89C51ID2_H*/
