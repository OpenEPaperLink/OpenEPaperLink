/*-------------------------------------------------------------------------
   stc89c.h - Register Declarations for STC89Cxx Series
   Based on stc12.h

   Copyright (c) 2012, intron@intron.ac
   Copyright (c) 2020, flameeyes@flameeyes.com

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
   Brief:

       STC89Cxx/STC89LExx series are 8051-compatible MCU's. The "official"
   websites are http://www.stcmcu.com/ (In Chinese Han only) and
   http://www.stcmicro.com/ (in English), and datasheets in Chinese Han and
   English can be downloaded there.

   Reference:

   1. 
   1. The "official" C header file (written for another C51 compiler):
        http://www.stcmcu.com/datasheet/stc/STC-AD-PDF/STC_NEW_8051.H
   2. Datasheets for STC89(C/LE)xx series:
      English:
        http://www.stcmicro.com/datasheet/STC89C51RC-en.pdf
 */

#ifndef _STC89_H_
#define _STC89_H_

#include <8051.h>
#include <compiler.h>

#ifdef REG8051_H
#undef REG8051_H
#endif

/*
 * Auxiliary Register
 *   Bit Mapping: - - - - - - EXTRAM ALEOFF
 *   Reset Value: xxxx,xx00
 */
SFR(AUXR, 0x8E);

/*
 * Auxiliary Register 1
 *   Bit Mapping: - - - - - - GF2 - - DPS
 *   Reset Value: 0xxx,0xx0
 */
SFR(AUXR1, 0xA2);

/* Slave Address for Serial Communication
 *   Reset Value: 0000,0000
 */
SFR(SADDR, 0xA9);

/*
 * Higher bits for Interrupt Priority
 *   Bit Mapping: PX3H PX2H PT2H PSH PT1H PX1H PT0H PX0H
 *   Reset Value: 0000,0000
 */
SFR(IPH, 0xB7);

/* Slave Address Mask for Serial Communication 
 *   Reset Value: 0000,0000
 */
SFR(SADEN, 0xB9);

/* Auxiliary Input Control
 *   Bit Mapping: PX3 EX3 IE3 IT3 PX2 EX2 HE2 IT2
 *   Reset Value: 0000,0000
 */
SFR(XICON, 0xC0);

/* Timer/Counter 2 registers are compatible with AT89x52 */

/* Timer/Counter 2 Control
 *   Bit Mapping: TF2 EXF2 RCLK TCLK EXEN2 TR2 C/!T2 CP/!RL2
 *   Reset Value: 0000,0000
 */
SFR(T2CON, 0xC8);

/* Timer/Counter 2 Mode
 *   Bit Mapping: - - - - - - T2OE DCEN
 *   Reset Value: xxxx,xx00
 */
SFR(T2MOD, 0xC9H);

/* Timer/Counter 2 Reload/Capture Low Byte
 *   Reset Value: 0000,0000
 */
SFR(RCAP2L, 0xCA);

/* Timer/Counter 2 Reload/Capture High Byte
 *   Reset Value: 0000,0000
 */
SFR(RCAP2L, 0xCB);

/* Timer/Counter 2 Low Byte
 *   Reset Value: 0000,0000
 */
SFR(RCAP2L, 0xCC);

/* Timer/Counter 2 High Byte
 *   Reset Value: 0000,0000
 */
SFR(RCAP2L, 0xCD);

/*
 * Watchdog Timer Control Register
 *   Bit Mapping: - - EN_WDT CLR_WDT IDLE_WDT PS2 PS1 PS0
 *   Reset Value: xx00,0000
 */
SFR(WDT_CONTR, 0xC1);

/* In-Application-Programming Data Register
 *   Reset Value: 1111,1111
 */
SFR(IAP_DATA, 0xE2);

/* In-Application-Programming Address Registers
 *   Reset Values: 0000, 0000
 *                 0000, 0000
 */
SFR(IAP_ADDRH, 0xE3); /* Higher 8 bits */
SFR(IAP_ADDRL, 0xE4); /* Lower 8 bits */

/*
 * In-Application-Programming Address Registers
 *   Bit Mapping: - - - - - MS2 MS1 MS0
 *   Reset Value: xxxx,x000
 */
SFR(IAP_CMD, 0xE5);

/* In-Application-Programming Trigger Registers
 *   Reset Value: xxxx,xxxx
 */
SFR(IAP_TRIG, 0xE6);

/*
 * In-Application-Programming Control Register
 *   Bit Mapping: ISPEN SWBS SWRST - - WT2 WT1 WT0
 *   Reset Value: 000x,x000
 */
SFR(IAP_CONTR, 0xE7);

/*
 * One Extended GPIO Port: P4
 * - For PLCC-44 and LQFP-44 packages, only lower 4 bits of P4 are available.
 */
SFR(P4, 0xE8);
    SBIT(P4_0, 0xC0, 0);
    SBIT(P4_1, 0xC0, 1);
    SBIT(P4_2, 0xC0, 2);
    SBIT(P4_3, 0xC0, 3);

#endif /* _STC89_H_ */

