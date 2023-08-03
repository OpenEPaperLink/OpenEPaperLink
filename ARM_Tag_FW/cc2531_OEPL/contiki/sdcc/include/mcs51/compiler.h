/*-------------------------------------------------------------------------
   compiler.h

   Copyright (C) 2006, Maarten Brock, sourceforge.brock@dse.nl
   Portions of this file are Copyright 2014 Silicon Laboratories, Inc.
   http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt

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
  * Header file to overcome 8051 compiler differences for specifying
  * special function registers. The following compilers are supported:
  * SDCC, Keil, Raisonance, IAR, Hi-Tech, Tasking, Crossware, Wickenhaeuser.
  * Unfortunately not for use with Dunfield. The compilers are identified by
  * their unique predefined macros. See also:
  * http://predef.sourceforge.net/precomp.html
  *
  * SBIT and SFR define special bit and special function registers at the given
  * address. SFR16 and SFR32 define sfr combinations at adjacent addresses in
  * little-endian format. SFR16E and SFR32E define sfr combinations without
  * prerequisite byte order or adjacency. None of these multi-byte sfr
  * combinations will guarantee the order in which they are accessed when read
  * or written.
  * SFR16X and SFR32X for 16 bit and 32 bit xdata registers are not defined
  * to avoid portability issues because of compiler endianness.
  * SFR16LEX is provided for 16 bit little endian xdata registers. It is usable
  * on little endian compilers only; on big endian compilers, these registers
  * will not be defined.
  * This file is to be included in every microcontroller specific header file.
  * Example:
  *
  * // my_mcu.h: sfr definitions for my mcu
  * #include <compiler.h>
  *
  * SBIT  (P0_1, 0x80, 1);      // Port 0 pin 1
  *
  * SFR   (P0, 0x80);           // Port 0
  *
  * SFRX  (CPUCS, 0xE600);      // Cypress FX2 Control and Status register in xdata memory at 0xE600
  *
  * SFR16 (TMR2, 0xCC);         // Timer 2, lsb at 0xCC, msb at 0xCD
  *
  * SFR16E(TMR0, 0x8C8A);       // Timer 0, lsb at 0x8A, msb at 0x8C
  *
  * SFR32 (MAC0ACC, 0x93);      // SiLabs C8051F120 32 bits MAC0 Accumulator, lsb at 0x93, msb at 0x96
  *
  * SFR32E(SUMR, 0xE5E4E3E2);   // TI MSC1210 SUMR 32 bits Summation register, lsb at 0xE2, msb at 0xE5
  *
 */

#ifndef COMPILER_H
#define COMPILER_H

/** SDCC - Small Device C Compiler
  * http://sdcc.sf.net
 */
#if defined (SDCC) || defined (__SDCC)
# define SBIT(name, addr, bit)  __sbit  __at(addr+bit)                    name
# define SFR(name, addr)        __sfr   __at(addr)                        name
# define SFRX(name, addr)       __xdata volatile unsigned char __at(addr) name
# define SFR16(name, addr)      __sfr16 __at(((addr+1U)<<8) | addr)       name
# define SFR16E(name, fulladdr) __sfr16 __at(fulladdr)                    name
# define SFR16LEX(name, addr)   __xdata volatile unsigned short __at(addr) name
# define SFR32(name, addr)      __sfr32 __at(((addr+3UL)<<24) | ((addr+2UL)<<16) | ((addr+1UL)<<8) | addr) name
# define SFR32E(name, fulladdr) __sfr32 __at(fulladdr)                    name

# define INTERRUPT(name, vector) void name (void) __interrupt (vector)
# define INTERRUPT_USING(name, vector, regnum) void name (void) __interrupt (vector) __using (regnum)

// NOP () macro support
#define NOP() __asm NOP __endasm

/** Keil C51
  * http://www.keil.com
 */
#elif defined __CX51__
# define SBIT(name, addr, bit)  sbit  name = addr^bit
# define SFR(name, addr)        sfr   name = addr
# define SFRX(name, addr)       volatile unsigned char xdata name _at_ addr
# define SFR16(name, addr)      sfr16 name = addr
# define SFR16E(name, fulladdr) /* not supported */
# define SFR16LEX(name, addr)   /* not supported */
# define SFR32(name, fulladdr)  /* not supported */
# define SFR32E(name, fulladdr) /* not supported */

# define INTERRUPT(name, vector) void name (void) interrupt vector
# define INTERRUPT_USING(name, vector, regnum) void name (void) interrupt vector using regnum

// NOP () macro support
extern void _nop_ (void);
#define NOP() _nop_()

/** Raisonance
  * http://www.raisonance.com
 */
#elif defined __RC51__
# define SBIT(name, addr, bit)  at (addr+bit) sbit                   name
# define SFR(name, addr)        sfr at addr                          name
# define SFRX(name, addr)       xdata at addr volatile unsigned char name
# define SFR16(name, addr)      sfr16 at addr                        name
# define SFR16E(name, fulladdr) /* not supported */
# define SFR16LEX(name, addr)   /* not supported */
# define SFR32(name, fulladdr)  /* not supported */
# define SFR32E(name, fulladdr) /* not supported */

# define INTERRUPT(name, vector) void name (void) interrupt vector
# define INTERRUPT_USING(name, vector, regnum) void name (void) interrupt vector using regnum

// NOP () macro support -- NOP is opcode 0x00
#define NOP() asm { 0x00 }

/** IAR 8051
  * http://www.iar.com
 */
#elif defined __ICC8051__
# define SBIT(name, addr, bit)  __bit __no_init volatile bool name @ (addr+bit)
# define SFR(name, addr)        __sfr __no_init volatile unsigned char name @ addr
# define SFRX(name, addr)       __xdata __no_init volatile unsigned char name @ addr
# define SFR16(name, addr)      __sfr __no_init volatile unsigned int  name @ addr
# define SFR16E(name, fulladdr) /* not supported */
# define SFR16LEX(name, addr)   /* not supported */
# define SFR32(name, fulladdr)  __sfr __no_init volatile unsigned long name @ addr
# define SFR32E(name, fulladdr) /* not supported */

# define _PPTOSTR_(x) #x
# define _PPARAM_(address) _PPTOSTR_(vector=address * 8 + 3)
# define _PPARAM2_(regbank) _PPTOSTR_(register_bank=regbank)
# define INTERRUPT(name, vector) _Pragma(_PPARAM_(vector)) __interrupt void name(void)
# define INTERRUPT_USING(name, vector, regnum) _Pragma(_PPARAM2_(regnum)) _Pragma(_PPARAM_(vector)) __interrupt void name(void)

extern __intrinsic void __no_operation (void);
#define NOP() __no_operation()

/** Tasking / Altium
  * http://www.altium.com/tasking
 */
#elif defined _CC51
# define SBIT(name, addr, bit)  _sfrbit  name _at(addr+bit)
# define SFR(name, addr)        _sfrbyte name _at(addr)
# define SFRX(name, addr)       _xdat volatile unsigned char name _at(addr)
#if _CC51 > 71
# define SFR16(name, addr)      _sfrword _little name _at(addr)
#else
# define SFR16(name, addr)      /* not supported */
#endif
# define SFR16E(name, fulladdr) /* not supported */
# define SFR16LEX(name, addr)   /* not supported */
# define SFR32(name, fulladdr)  /* not supported */
# define SFR32E(name, fulladdr) /* not supported */

# define INTERRUPT(name, vector) _interrupt (vector) void name (void)
# define INTERRUPT_USING(name, vector, regnum) _interrupt (vector) _using(regnum) void name (void)

// NOP () macro support
extern void _nop (void);
#define NOP() _nop()

/** Hi-Tech 8051
  * http://www.htsoft.com
 */
#elif defined HI_TECH_C
# define SBIT(name, addr, bit)  volatile bit           name @ (addr+bit)
# define SFR(name, addr)        volatile unsigned char name @ addr
# define SFRX(name, addr)       volatile far unsigned char name @ addr
# define SFR16(name, addr)      /* not supported */
# define SFR16E(name, fulladdr) /* not supported */
# define SFR16LEX(name, addr)   /* not supported */
# define SFR32(name, fulladdr)  /* not supported */
# define SFR32E(name, fulladdr) /* not supported */

# define INTERRUPT(name, vector)       void name (void) interrupt vector
# define INTERRUPT_PROTO(name, vector)

// NOP () macro support
#define NOP() asm(" nop ")

/** Crossware
  * http://www.crossware.com
 */
#elif defined _XC51_VER
# define SBIT(name, addr, bit)  _sfrbit  name = (addr+bit)
# define SFR(name, addr)        _sfr     name = addr
# define SFRX(name, addr)       volatile unsigned char _xdata name _at addr
# define SFR16(name, addr)      _sfrword name = addr
# define SFR16E(name, fulladdr) /* not supported */
# define SFR16LEX(name, addr)   /* not supported */
# define SFR32(name, fulladdr)  /* not supported */
# define SFR32E(name, fulladdr) /* not supported */

/** Wickenhaeuser
  * http://www.wickenhaeuser.de
 */
#elif defined __UC__
# define SBIT(name, addr, bit)  unsigned char bit  name @ (addr+bit)
# define SFR(name, addr)        near unsigned char name @ addr
# define SFRX(name, addr)       xdata volatile unsigned char name @ addr
# define SFR16(name, addr)      /* not supported */
# define SFR16E(name, fulladdr) /* not supported */
# define SFR16LEX(name, addr)   /* not supported */
# define SFR32(name, fulladdr)  /* not supported */
# define SFR32E(name, fulladdr) /* not supported */

/** default
  * unrecognized compiler
 */
#else
# warning unrecognized compiler
# define SBIT(name, addr, bit)  volatile bool           name
# define SFR(name, addr)        volatile unsigned char  name
# define SFRX(name, addr)       volatile unsigned char  name
# define SFR16(name, addr)      volatile unsigned short name
# define SFR16E(name, fulladdr) volatile unsigned short name
# define SFR16LEX(name, addr)   volatile unsigned short name
# define SFR32(name, fulladdr)  volatile unsigned long  name
# define SFR32E(name, fulladdr) volatile unsigned long  name

#endif

#endif //COMPILER_H

