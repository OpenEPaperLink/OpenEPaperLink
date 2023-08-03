/*-------------------------------------------------------------------------
   r2ka.h - definitions on the built in I/O ports for the Rabbit 2000A
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

#ifndef __R2KA_H__
#define __R2KA_H__ 1

#ifndef _RABREG
#define _RABREG(addr, reg) __sfr __at(addr) reg
#endif

#include "r2k.h"

_RABREG(0x2F, GREV);  // Global Revision Register
_RABREG(0xC2, SALR);  // Serial Port A Long Stop Register
_RABREG(0xD2, SBLR);  // Serial Port B Long Stop Register
_RABREG(0xE2, SCLR);  // Serial Port C Long Stop Register
_RABREG(0xF2, SDLR);  // Serial Port D Long Stop Register

#endif

