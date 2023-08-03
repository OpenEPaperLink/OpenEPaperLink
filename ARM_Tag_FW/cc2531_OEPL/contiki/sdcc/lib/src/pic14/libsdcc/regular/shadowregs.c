/*-------------------------------------------------------------------------
   shadowregs.c - provide shadow register for use during interrupts

   Copyright (C) 2007, Raphael Neider <rneider AT web.de>

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
 * This file is part of SDCC's pic14 library and distributed under
 * the terms of the GPLv2 with linking exception; see COPYING in some
 * parent directory for details.
 */

/* 
 * We should make sure these always reside in the same bank,
 * so that we can save two BANKSELs in the interrupt entry code.
 */

unsigned char __sdcc_saved_fsr;
//unsigned char __sdcc_saved_pclath;
//unsigned char __sdcc_saved_status;

