;--------------------------------------------------------------------------
;  p18fxxx.inc
;
;  Copyright (C) 2005, Vangelis Rokas <vrokas AT otenet.gr>
;
;  This library is free software; you can redistribute it and/or modify it
;  under the terms of the GNU General Public License as published by the
;  Free Software Foundation; either version 2, or (at your option) any
;  later version.
;
;  This library is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
;  GNU General Public License for more details.
;
;  You should have received a copy of the GNU General Public License 
;  along with this library; see the file COPYING. If not, write to the
;  Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
;   MA 02110-1301, USA.
;
;  As a special exception, if you link this library with other files,
;  some of which are compiled with SDCC, to produce an executable,
;  this library does not by itself cause the resulting executable to
;  be covered by the GNU General Public License. This exception does
;  not however invalidate any other reasons why the executable file
;  might be covered by the GNU General Public License.
;--------------------------------------------------------------------------

#ifndef __P18FXXX__
#define	__P18FXXX__	1


		list	r=dec, n=96, st=off, mm=off

	        nolist

; This header file defines configurations, registers, and other useful bits of
; information common to all PIC18Fxxx microcontrollers.

FSR0            equ	0
FSR1            equ	1
FSR2            equ	2

FAST            equ	1

W               equ	0
A               equ	0
ACCESS          equ	0
BANKED		equ	1

;	Register Files

TOSU            equ	0x0FFF
TOSH            equ	0x0FFE
TOSL            equ	0x0FFD

STKPTR          equ	0x0FFC

PCLATU          equ	0x0FFB
PCLATH          equ	0x0FFA
PCL             equ	0x0FF9

TBLPTRU         equ	0x0FF8
TBLPTRH         equ	0x0FF7
TBLPTRL         equ	0x0FF6
TABLAT          equ	0x0FF5

PRODH           equ	0x0FF4
PRODL           equ	0x0FF3

INDF0           equ	0x0FEF
POSTINC0        equ	0x0FEE
POSTDEC0        equ	0x0FED
PREINC0         equ	0x0FEC
PLUSW0          equ	0x0FEB
FSR0H           equ	0x0FEA
FSR0L           equ	0x0FE9

WREG            equ	0x0FE8

INDF1           equ	0x0FE7
POSTINC1        equ	0x0FE6
POSTDEC1        equ	0x0FE5
PREINC1         equ	0x0FE4
PLUSW1          equ	0x0FE3
FSR1H           equ	0x0FE2
FSR1L           equ	0x0FE1

BSR             equ	0x0FE0

INDF2           equ	0x0FDF
POSTINC2        equ	0x0FDE
POSTDEC2        equ	0x0FDD
PREINC2         equ	0x0FDC
PLUSW2          equ	0x0FDB
FSR2H           equ	0x0FDA
FSR2L           equ	0x0FD9

STATUS          equ	0x0FD8

PORTC		equ	0x0F82
PORTB		equ	0x0F81
PORTA		equ	0x0F80

;	Status Register Bit Definitions

C		equ	0
DC		equ	1
Z		equ	2
OV		equ	3
N		equ	4

		list
#endif
