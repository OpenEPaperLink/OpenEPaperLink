/*-------------------------------------------------------------------------
   r3k.h - definitions on the built in I/O ports for the Rabbit 3000
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

#ifndef __R3K_H__
#define __R3K_H__ 1

#ifndef _RABREG
#define _RABREG(addr, reg) __sfr __at(addr) reg
#endif

#include "r2kc.h"

_RABREG(0x0D, GPSCR); // Global Power Save Control Register

_RABREG(0x18, MECR);  // MMu Expanded Code Register

_RABREG(0x1C, BDCR);  // Breakpoint/Debug Control Register

_RABREG(0x2C, GROM);  // Global RAM Configuration Register
_RABREG(0x2D, GRAM);  // Global ROM Configuration Register

_RABREG(0x38, PFDR);  // Port F Data Register
_RABREG(0x3C, PFCR);  // Port F Control Register
_RABREG(0x3D, PFFR);  // Port F Function Register
_RABREG(0x3E, PFDCR); // Port F Drive Control Register
_RABREG(0x3F, PFDDR); // Port F Data Direction Register

_RABREG(0x47, PBDDR); // Port B Data Direction Register

_RABREG(0x48, PGDR);  // Port G Data Register
_RABREG(0x4C, PGCR);  // Port G Control Register
_RABREG(0x4D, PGFR);  // Port G Function Register
_RABREG(0x4E, PGDCR); // Port G Drive Control Register
_RABREG(0x4F, PGDDR); // Port G Data Direction Register

// TODO: Input capture registers, section 20.1.2 of the Rabbit 3000 User manual.

// TODO: PWM registers, section 22.1.2 of the Rabbit 3000 User manual.

_RABREG(0x90, QDCSR); // Quad Decode Ctrl/Status Register
_RABREG(0x91, QDCR);  // Quad Decode Control Register
_RABREG(0x94, QDC1R); // Quad Decode Count 1 Register
_RABREG(0x96, QDC2R); // Quad Decode Count 2 Register

_RABREG(0xA5, TAT2R); // Timer A Time Constant 2 Register
_RABREG(0xA6, TAT8R); // Timer A Time Constant 8 Register
_RABREG(0xA7, TAT3R); // Timer A Time Constant 3 Register
_RABREG(0xA8, TAT9R); // Timer A Time Constant 9 Register
_RABREG(0xAA, TAT10R);// Timer A Time Constant 10 Register

_RABREG(0xC5, SAER);  // Serial Port A Extended Register

_RABREG(0xD5, SBER);  // Serial Port B Extended Register

_RABREG(0xE5, SCER);  // Serial Port C Extended Register

_RABREG(0xF5, SDER);  // Serial Port D Extended Register

_RABREG(0xC8, SEDR);  // Serial Port E Data Register
_RABREG(0xC9, SEAR);  // Serial Port E Address Register
_RABREG(0xCA, SELR);  // Serial Port E Long Stop Register
_RABREG(0xCB, SESR);  // Serial Port E Status Register
_RABREG(0xCC, SECR);  // Serial Port E Control Register
_RABREG(0xCD, SEER);  // Serial Port E Extended Register

_RABREG(0xD8, SFDR);  // Serial Port F Data Register
_RABREG(0xD9, SFAR);  // Serial Port F Address Register
_RABREG(0xDA, SFLR);  // Serial Port F Long Stop Register
_RABREG(0xDB, SFSR);  // Serial Port F Status Register
_RABREG(0xDC, SFCR);  // Serial Port F Control Register
_RABREG(0xDD, SFER);  // Serial Port F Extended Register

#endif

