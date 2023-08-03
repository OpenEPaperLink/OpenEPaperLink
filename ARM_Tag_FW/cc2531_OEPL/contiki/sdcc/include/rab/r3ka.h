/*-------------------------------------------------------------------------
   r3ka.h - definitions on the built in I/O ports for the Rabbit 3000A
            for use with SDCC

   Copyright (C) 2020, Philipp Klaus Krause <pkk AT spth.de>

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

#ifndef __R3KA_H__
#define __R3KA_H__ 1

#ifndef _RABREG
#define _RABREG(addr, reg) __sfr __banked __at(addr) reg
#endif

#include "r3k.h"

_RABREG(0x000C, SWDTR);  // Secondary Watchdog Timer Register

_RABREG(0x0095, QDC1HR); // Quad Decode Count 1 High Register
_RABREG(0x0097, QDC2HR); // Quad Decode Count 2 High Register

_RABREG(0x0300, RTUER);  // Real Time Clock User Enable Register
_RABREG(0x0320, SPUER);  // Slave Port User Enable Register
_RABREG(0x0330, PAUER);  // Parallel Port A User Enable Register
_RABREG(0x0338, PFUER);  // Parallel Port F User Enable Register
_RABREG(0x0340, PBUER);  // Parallel Port B User Enable Register
_RABREG(0x0348, PGUER);  // Parallel Port G User Enable Register
_RABREG(0x0350, PCUER);  // Parallel Port C User Enable Register
_RABREG(0x0358, ICUER);  // Input Capture C User Enable Register
_RABREG(0x0360, PDUER);  // Parallel Port D User Enable Register
_RABREG(0x0370, PEUER);  // Parallel Port E User Enable Register
_RABREG(0x0380, IBUER);  // I/O Bank User Enable Register
_RABREG(0x0388, PWUER);  // PWM User Enable Register
_RABREG(0x0390, QDUER);  // Quad Decode User Enable Register
_RABREG(0x0398, IUER);   // External Interrupt User Enable Register
_RABREG(0x03A0, TAUER);  // Timer A User Enable Register
_RABREG(0x03B0, TBUER);  // Timer B User Enable Register
_RABREG(0x03C0, SAUER);  // Serial Port A User Enable Register
_RABREG(0x03C8, SEUER);  // Serial Port E User Enable Register
_RABREG(0x03D0, SBUER);  // Serial Port B User Enable Register
_RABREG(0x03D8, SFUER);  // Serial Port F User Enable Register
_RABREG(0x03E0, SCUER);  // Serial Port C User Enable Register
_RABREG(0x03F0, SDUER);  // Serial Port D User Enable Register

_RABREG(0x0420, EDMR);   // Enable Dual Mode Register

_RABREG(0x0440, WPCR);   // Write Protect Control Register

_RABREG(0x0444, STKKCR); // Stack Limit Control Register
_RABREG(0x0445, STKLLR); // Stack Low Limit Register
_RABREG(0x0446, STKHLR); // Stack High Limit Register

_RABREG(0x0448, RAMSR);  // RAM Segment Register

_RABREG(0x0460, WPLR);   // Write Protect Low Register
_RABREG(0x0461, WPHR);   // Write Protect High Register
_RABREG(0x0480, WPSAR);  // Write Protect Segment A Register
_RABREG(0x0481, WPSALR); // Write Protect Segment A Low Register
_RABREG(0x0482, WPSAHR); // Write Protect Segment A High Register
_RABREG(0x0484, WPSBR);  // Write Protect Segment B Register
_RABREG(0x0485, WPSBLR); // Write Protect Segment B Low Register
_RABREG(0x0486, WPSBHR); // Write Protect Segment B High Register

#endif

