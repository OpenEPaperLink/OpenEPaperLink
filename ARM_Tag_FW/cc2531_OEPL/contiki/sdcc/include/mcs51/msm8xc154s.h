/*-------------------------------------------------------------------------
   msm8xc154s.h - Register Declarations for the Oki MSM80C154S and
   MSM83C154S

   Copyright (C) 2005,  Matthias Arndt / marndt@asmsoftware.de

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

#ifndef MSM8xC154S_H
#define MSM8xC154S_H

#include <8052.h>     /* load definitions for the 8052 core */

#ifdef REG8052_H
#undef REG8052_H
#endif

/* byte SFRs */
__sfr __at (0xf8) 	IOCON;	/* IOCON register */

/* bit locations */
__sbit __at (0xf8)	ALF;	/* floating status on power down control */
__sbit __at (0xf9)	P1HZ;	/* P1 high impedance input control */
__sbit __at (0xfa)	P2HZ;	/* P2 high impedance input control */
__sbit __at (0xfb)	P3HZ;	/* P3 high impedance input control */
__sbit __at (0xfc)	IZC;	/* 10kO pull-up resistor control */
__sbit __at (0xfd)	SERR;	/* Serial port reception flag */
__sbit __at (0xfe)	T32;	/* interconnect T0 and T1 to 32bit timer/counter */

/* Bits in IP (0xb8) */
__sbit __at (0xbf)	PCT;	/* Priority interrupt circuit control bit */

/* Bits in PCON (0x87) */
#define RPD 0x20            /* Bit used to specify cancellation of CPU power down mode */
#define HPD 0x40            /* The hard power down setting mode is enabled when this bit is set to "1". */

#endif
