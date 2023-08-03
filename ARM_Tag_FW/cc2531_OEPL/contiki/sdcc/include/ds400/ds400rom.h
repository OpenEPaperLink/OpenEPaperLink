/*-------------------------------------------------------------------------
   ds400rom.h - Interface to DS80C400 ROM functions

   Copyright (C) 2000, Kevin Vigor

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

#ifndef DS400ROM_H_
#define DS400ROM_H_

extern unsigned char init_rom(void __xdata *loMem,
			      void __xdata *hiMem) __naked;

extern unsigned long task_gettimemillis_long(void) __naked;

extern unsigned char task_getthreadID(void) __naked;

/** Timer reload value for 14.746 MHz crystal. */
#define RELOAD_14_746 0xfb33

/** Timer reload value for 18.432 MHz crystal. */
#define RELOAD_18_432 0xfa00

/** Timer reload value for 29.491 MHz crystal. */
#define RELOAD_29_491 0xfd99

/** Timer reload value for 36.864 MHz crystal. */
#define RELOAD_36_864 0xfd00

/** Timer reload value for 58.982 MHz crystal. */
#define RELOAD_58_982 0xfecc

/** Timer reload value for 73.728 MHz crystal. */
#define RELOAD_73_728 0xfe80

extern unsigned int task_gettickreload(void);

extern void task_settickreload(unsigned);

#endif
