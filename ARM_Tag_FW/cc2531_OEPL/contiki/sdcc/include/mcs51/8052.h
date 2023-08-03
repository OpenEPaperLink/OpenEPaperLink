/*-------------------------------------------------------------------------
   8052.h: Register Declarations for the Intel 8052 Processor

   Copyright (C) 2000, Bela Torok / bela.torok@kssg.ch

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

#ifndef REG8052_H
#define REG8052_H

#include <8051.h>     /* load definitions for the 8051 core */

#ifdef REG8051_H
#undef REG8051_H
#endif

/* define 8052 specific registers only */

/* T2CON */
__sfr __at (0xC8) T2CON ;

/* RCAP2 L & H */
__sfr __at (0xCA) RCAP2L  ;
__sfr __at (0xCB) RCAP2H  ;
__sfr __at (0xCC) TL2     ;
__sfr __at (0xCD) TH2     ;

/*  IE  */
__sbit __at (0xAD) ET2    ; /* Enable timer2 interrupt */

/*  IP  */
__sbit __at (0xBD) PT2    ; /* T2 interrupt priority bit */

/* T2CON bits */
__sbit __at (0xC8) T2CON_0 ;
__sbit __at (0xC9) T2CON_1 ;
__sbit __at (0xCA) T2CON_2 ;
__sbit __at (0xCB) T2CON_3 ;
__sbit __at (0xCC) T2CON_4 ;
__sbit __at (0xCD) T2CON_5 ;
__sbit __at (0xCE) T2CON_6 ;
__sbit __at (0xCF) T2CON_7 ;

__sbit __at (0xC8) CP_RL2  ;
__sbit __at (0xC9) C_T2    ;
__sbit __at (0xCA) TR2     ;
__sbit __at (0xCB) EXEN2   ;
__sbit __at (0xCC) TCLK    ;
__sbit __at (0xCD) RCLK    ;
__sbit __at (0xCE) EXF2    ;
__sbit __at (0xCF) TF2     ;

#endif
