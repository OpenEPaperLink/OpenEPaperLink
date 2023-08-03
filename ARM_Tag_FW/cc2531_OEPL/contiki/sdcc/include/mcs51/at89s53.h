/*-------------------------------------------------------------------------
   at89s53.h - Register Declarations for the Atmel AT89S53 Processor

   Copyright (C) 2005, Jesus Calvino-Fraga / jesusc at ece.ubc.ca

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

#ifndef REG_AT89S53_H
#define REG_AT89S53_H

#include <8052.h>     /* load difinitions for the 8052 core */

#ifdef REG8052_H
#undef REG8052_H
#endif

/* define AT89S53 specific registers only */
__sfr __at (0x84) DP1L;     /* Data Pointer 1 Low Byte */
__sfr __at (0x85) DP1H;     /* Data Pointer 1 High Byte */
__sfr __at (0x86) SPDR;     /* SPI Data Register */
__sfr __at (0xAA) SPSR;     /* SPI Status Register */
__sfr __at (0x96) WMCON;    /* Watchdog and Memory Control Register */
__sfr __at (0xD5) SPCR;     /* SPI Control Register */

/*------------------------------------------------
SPSR (0xAA) Bit Values - Reset Value = 0000.0000
------------------------------------------------*/
#define WCOL_   0x40    /* SPI Write Collision Flag: 1=Collision */
#define SPIF_   0x80    /* SPI Interrupt Flag */

/*------------------------------------------------
WMCON (0x96) Bit Values
------------------------------------------------*/
#define WDTEN_   0x01

#define WDTRST_  0x02   /* Watchdog Timer Reset and EEPROM Ready,/Busy Flag*/
#define EERDY_   0x02   /* Watchdog Timer Reset and EEPROM Ready,/Busy Flag */

#define DPS_     0x04   /* Data Pointer Select: 0=DP0, 1=DP1 */
#define EEMEN_   0x08   /* Internal EEPROM Access Enable: 1=Enabled */
#define EEMWE_   0x10   /* Internal EEPROM Write Enable: 1=Enabled */
#define PS0_     0x20   /* Prescaler bit 0 for the Watchdog Timer */
#define PS1_     0x40   /* Prescaler bit 1 for the Watchdog Timer */
#define PS2_     0x80   /* Prescaler bit 2 for the Watchdog Timer */
                        /* 000 =   16ms Timeout */
                        /* 001 =   32ms Timeout */
                        /* 010 =   64ms Timeout */
                        /* 011 =  128ms Timeout */
                        /* 100 =  256ms Timeout */
                        /* 101 =  512ms Timeout */
                        /* 110 = 1024ms Timeout */
                        /* 111 = 2048ms Timeout */

/*------------------------------------------------
SPCR (0xD5) Bit Values - Reset Value = 0000.01XX
------------------------------------------------*/
#define SPR0_   0x01    /* SPI Clock Rate Select bit 0 */
#define SPR1_   0x02    /* SPI Clock Rate Select bit 1 */
                        /* 00 = Fosc / 4   */
                        /* 01 = Fosc / 16  */
                        /* 10 = Fosc / 64  */
                        /* 11 = Fosc / 128 */

#define CPHA_   0x04    /* SPI Clock Phase */
#define CPOL_   0x08    /* SPI Clock Polarity */
#define MSTR_   0x10    /* SPI Master/Slave Select: 0=Slave, 1=Master */
#define DORD_   0x20    /* SPI Data Order: 0=MSB First, 1=LSB First */
#define SPE_    0x40    /* SPI Enable: 0=Disabled, 1=Enabled */
#define SPIE_   0x80    /* SPI Interrupt Enable: 0=Disabled, 1=Enabled */

#endif /*REG_AT89S53_H*/
