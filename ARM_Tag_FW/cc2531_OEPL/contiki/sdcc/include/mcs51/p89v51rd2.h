/*-------------------------------------------------------------------------
   p89v51rd2.h - Register Declarations for the Philips P89V51RD2 Processor

   Copyright (C) 2005, Jesus Calvino-Fraga / jesusc at ece.ubc.ca

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


#ifndef REG_P89V51RD2_H
#define REG_P89V51RD2_H

#include <8052.h>     // Load definitions for the 8052

#ifdef REG8052_H
#undef REG8052_H
#endif

// Define P89V51RD2 specific registers only

__sfr __at (0x8E) AUXR;		//Auxiliary function register (Reset value 0x00).
	#define EXTRAM 0x02		//'0'=uses internal XRAM.
	#define AO 0x01			//'1'=Disables ALE generation.

__sfr __at (0xA2) AUXR1;	//Auxiliary function register 1 (Reset value 0x00).  
	#define GF2 0x08		//General purpose user-defined flag.
	#define DPS 0x01		//Data pointer select.

__sfr __at (0xFA) CCAP0H;	//Module 0 Capture HIGH. 
__sfr __at (0xFB) CCAP1H;	//Module 1 Capture HIGH. 
__sfr __at (0xFC) CCAP2H;	//Module 2 Capture HIGH. 
__sfr __at (0xFD) CCAP3H;	//Module 3 Capture HIGH. 
__sfr __at (0xFE) CCAP4H;	//Module 4 Capture HIGH. 
__sfr __at (0xEA) CCAP0L;	//Module 0 Capture LOW. 
__sfr __at (0xEB) CCAP1L;	//Module 1 Capture LOW. 
__sfr __at (0xEC) CCAP2L;	//Module 2 Capture LOW. 
__sfr __at (0xED) CCAP3L;	//Module 3 Capture LOW. 
__sfr __at (0xEE) CCAP4L;	//Module 4 Capture LOW. 

__sfr __at (0xDA) CCAPM0;	//Module 0 Mode.  
__sfr __at (0xDB) CCAPM1;	//Module 1 Mode.  
__sfr __at (0xDC) CCAPM2;	//Module 2 Mode.  
__sfr __at (0xDD) CCAPM3;	//Module 3 Mode.  
__sfr __at (0xDE) CCAPM4;	//Module 4 Mode.
//The preceding five registers have the following bits:  
	#define ECOM 0x40		//Enable Comparator.
	#define CAPP 0x20		//1=enables positive edge capture.
	#define CAPN 0x10		//1=enables negative edge capture.
	#define MAT  0x08		//When counter matches sets CCF_n bit causing and interrupt.
	#define TOG  0x04		//Toggle output on match.
	#define PWM  0x02		//Pulse width modulation mode.
	#define ECCF 0x01		//Enable CCF interrupt.

__sfr __at (0xD8) CCON;	//PCA Counter Control (Reset value 0x00)
	__sbit __at (0xDF) CF;	//PCA Counter overflow flag.
	__sbit __at (0xDE) CR ; //PCA Counter Run Control Bit. 1=counter on.  0=counter off.
	__sbit __at (0xDC) CCF4;//PCA Module 4 Interrupt Flag.
	__sbit __at (0xDB) CCF3;//PCA Module 3 Interrupt Flag.
	__sbit __at (0xDA) CCF2;//PCA Module 2 Interrupt Flag.
	__sbit __at (0xD9) CCF1;//PCA Module 1 Interrupt Flag.
	__sbit __at (0xD8) CCF0;//PCA Module 0 Interrupt Flag.

__sfr __at (0xF9) CH;		//PCA Counter HIGH. 
__sfr __at (0xE9) CL;		//PCA Counter LOW. 

__sfr __at (0xD9) CMOD;		//PCA Counter Mode.  
	#define CIDL 0x80		//CIDL=0 program the PCA counter to work during idle mode.
	#define WDTE 0x40		//Watchdog Timer Enable.
	#define CPS1 0x04		//PCA Count Pulse Select bit 1.
	#define CPS0 0x02		//PCA Count Pulse Select bit 0.
							//00=Internal clock, Fosc/6
							//01=Internal clock, Fosc/6
							//10=Timer 0 overflow
							//11=External clock at ECI/P1.2 pin (max rate=Fosc/4)
	#define ECF 0x01		//PCA Enable Counter Overflow Interrupt.

__sfr __at (0xB6) FST;		//Flash Status Register.  
	#define SB 0x40
	#define EDC 0x08

__sfr __at (0xB1) FCF;		//Flash program memory bank selection. 
	#define SWR  0x02
	#define BSEL 0x01

//Attention IEN0 is the same as register IE found in <8051.h> only bit EC added here.
__sfr __at (0xA8) IEN0;		//Interrupt Enable 1.
	__sbit __at (0xAE) EC;	//PCA Interrupt Enable bit.

__sfr __at (0xE8) IEN1;		//Interrupt Enable 1
	__sbit __at (0xEB) EBO; //Brown-out Interrupt Enable. (Vector is 0x00b4).

//Attention IP0 is the same as register IP found in <8051.h> only bit PPC added here.
__sfr __at (0xB8) IP0;		//Interrupt Priority 0 HIGH.  
	__sbit __at (0xBE) PPC;	//PCA Interrupt Priority low bit.

__sfr __at (0xB7) IP0H;		//Interrupt Priority 0 HIGH  
	#define PPCH 0x40		//PCA Interrupt Priority High Bit.
	#define PT2H 0x20		//Timer 2 Interrupt Interrupt Priority High Bit.
	#define PSH  0x10		//Serial Port Interrupt Priority High Bit.
	#define PT1H 0x08		//Timer 1 Interrupt Priority High Bit.
	#define PX1H 0x04		//External Interrupt 1 Priority High Bit.
	#define PT0H 0x02		//Timer 0 Interrupt Priority High Bit.
	#define PX0H 0x01		//External Interrupt 0 Priority High Bit.

__sfr __at (0xF8) IP1;		//Interrupt Priority 1.
	__sbit __at (0xFB) PBO; //Brown-out Interrupt Priority Bit.

__sfr __at (0xF7) IP1H;		//Interrupt Priority 1 HIGH.  
	#define PBOH 0x08		//Brown-out Interrupt Priority High Bit.

__sfr __at (0xA9) SADDR;	//Serial Port Address Register. 
__sfr __at (0xB9) SADEN;	//Serial Port Address Enable. 

__sfr __at (0xD5) SPCR;		//SPI Control Register (Reset value 00000000B). 
__sfr __at (0xD5) SPCTL;	//SPI Control Register (This name appears also in the datasheet).   
	#define SPIE 0x80		//If both SPIE and ES are set to one, SPI interrupts are enabled.
	#define SPEN 0x40		//SPI enable bit.  When set enables SPI.
	#define SPE  0x40		//Same as above.  This name appears also in the manual :-(
	#define DORD 0x20		//Data trans. order. 0=MSB first; 1=LSB first.
	#define MSTR 0x10		//1=master mode.  0=slave mode.
	#define CPOL 0x08		//1=SCK is high when idle (active low), 0=SCK is low when idle (active high).
	#define CPHA 0x04		//1=shift triggered on the trailing edge of SCK.  0=shift trig. on leading edge.
	#define SPR1 0x02		//SPI Clork Rate select bit 1.
	#define SPR0 0x01		//SPI Clork Rate select bit 0.
							//00 = Fosc/4
							//01 = Fosc/16
							//10 = Fosc/64
							//11 = Fosc/128

__sfr __at (0xAA) SPSR;		//SPI Configuration Register (Reset value 00000000B).  
__sfr __at (0xAA) SPCFG;	//SPI Configuration Register (This name appears also in the datasheet).  
	#define SPIF 0x80		//SPI interrupt flag.
	#define SPWCOL 0x40		//Write collision Flag.

__sfr __at (0x86) SPDR;		//SPI Data 
__sfr __at (0x86) SPDAT;	//SPI Data (This name appears also in the datasheet). 

__sfr __at (0xC0) WDTC;		//Watchdog Timer Control (Reset value 0x00). 
	__sbit __at (0xC4) WDOUT;//Watchdog output enable.
	__sbit __at (0xC3) WDRE; //Watchdog timer reset enable.
	__sbit __at (0xC2) WDTS; //Watchdog timer reset flag.
	__sbit __at (0xC1) WDT;  //Watchdog timer refresh.
	__sbit __at (0xC0) SWDT; //Start watchdog timer.

__sfr __at (0x85) WDTD;		//Watchdog Timer Data/Reload. 

__sfr __at (0xC9) T2MOD;	//Timer 2 mode control
	#define DCEN  0x01 		//Down count enable bit
	#define T2OE  0x02		//Timer 2 Output Enable bit.
	#define ENT2  0x20		//No description???

#endif /*REG_P89V51RD2_H*/
