/*-------------------------------------------------------------------------
   at89c51ed2.h: Register Declarations for the Atmel AT89C51RD2/ED2 Processor

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

#ifndef REG_AT89C51ED2_H
#define REG_AT89C51ED2_H

#include <8052.h>     // Load definitions for the 8052

#ifdef REG8052_H
#undef REG8052_H
#endif

// Define AT89C51RD2/ED2 specific registers only

__sfr __at (0x8E) AUXR;		//Auxiliary function register
	#define DPU  0x80		//'1'=Disables weak pull-up
	#define M0   0x20		//'1'=Strechs MOVX control signals
	#define XRS2 0x10		// XRAM select bit 2
	#define XRS1 0x08		// XRAM select bit 1
	#define XRS0 0x04		// XRAM select bit 0
     	// XRS2 XRS1 XRS2  XRAM Size
	    //  0    0    0    256  bytes
    	//  0    0    1    512  bytes
	    //  0    1    0    768  bytes (default)
    	//  0    1    1    1024 bytes
    	//  1    0    0    1792 bytes
	#define EXTRAM 0x02		//'0'=uses internal XRAM.
	#define AO 0x01			//'1'=Disables ALE generation.

__sfr __at (0xA2) AUXR1;	//Auxiliary function register 1
    #define ENBOOT 0x20     //'0'=Disables boot ROM
	#define GF3 0x08		//General purpose user-defined flag.
	#define DPS 0x01		//Data pointer select.

__sfr __at (0x97) CKRL;     //Clock Reload Register
__sfr __at (0x8F) CKCON0;   //Clock control Register 0
    #define WDTX2 0x40      //Watch Dog Clock speed '1'=12 ck/cy, '0'=6 ck/cy
    #define PCAX2 0x20      //Programmable Counter Array Clock speed '1'=12 ck/cy, '0'=6 ck/cy
    #define SIX2  0x10      //Enhanced UART Clock (Mode 0 and 2) speed '1'=12 ck/cy, '0'=6 ck/cy
    #define T2X2  0x08      //Timer2 Clock speed '1'=12 ck/cy, '0'=6 ck/cy
    #define T1X2  0x04      //Timer1 Clock speed '1'=12 ck/cy, '0'=6 ck/cy
    #define T0X2  0x02      //Timer0 Clock speed '1'=12 ck/cy, '0'=6 ck/cy
    #define X2    0x01      //CPU Clock '0'=12 ck/cy, '1'=6 ck/cy
__sfr __at (0xAF) CKCON1;   //Clock control Register 1
    #define XPIX2 0x01      //SPI Clock speed '1'=12 ck/cy, '0'=6 ck/cy

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

__sfr __at (0xD8) CCON;	//PCA Counter Control
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

//Attention IEN0 is the same as register IE found in <8051.h> only bit EC added here.
__sfr __at (0xA8) IEN0;		//Interrupt Enable 1.
	__sbit __at (0xAE) EC;	//PCA Interrupt Enable bit.

__sfr __at (0xB1) IEN1;		//Interrupt Enable 1
	#define ESPI 0x04		//SPA Interrupt Enable bit.
	#define KBD  0x01		//Keyboard Interrupt Enable bit.

//Attention IPL0 is the same as register IP found in <8051.h>
__sfr __at (0xB8) IPL0;	    //Interrupt Priority 0 LOW
	__sbit __at (0xBE) PPCL;//PCA Interrupt Priority low bit.
	__sbit __at (0xBD) PT2L;//Timer 2 Interrupt Priority Low Bit.
	__sbit __at (0xBC) PSL; //Serial Port Interrupt Priority Low Bit.
	__sbit __at (0xBB) PT1L;//Timer 1 Interrupt Priority Low Bit.
	__sbit __at (0xBA) PX1L;//External Interrupt 1 Priority Low Bit.
	__sbit __at (0xB9) PT0L;//Timer 0 Interrupt Priority Low Bit.
	__sbit __at (0xB8) PX0L;//External Interrupt 0 Priority Low Bit.

__sfr __at (0xB7) IPH0;		//Interrupt Priority 0 HIGH
	#define PPCH 0x40		//PCA Interrupt Priority High Bit.
	#define PT2H 0x20		//Timer 2 Interrupt Priority High Bit.
	#define PSH  0x10		//Serial Port Interrupt Priority High Bit.
	#define PT1H 0x08		//Timer 1 Interrupt Priority High Bit.
	#define PX1H 0x04		//External Interrupt 1 Priority High Bit.
	#define PT0H 0x02		//Timer 0 Interrupt Priority High Bit.
	#define PX0H 0x01		//External Interrupt 0 Priority High Bit.

__sfr __at (0xB2) IPL1;		//Interrupt Priority 1 LOW.
    #define SPIL 0x04       //SPI Priority Low Bit
    #define KBDL 0x01       //Keyboard Priority Low Bit

__sfr __at (0xB3) IPH1;		//Interrupt Priority 1 HIGH.
    #define SPIH 0x04       //SPI Priority High Bit
    #define KBDH 0x01       //Keyboard Priority High Bit

__sfr __at (0xC0) P4;       //8-bit port 4
    __sbit __at (0xC0) P4_0 ;
    __sbit __at (0xC1) P4_1 ;
    __sbit __at (0xC2) P4_2 ;
    __sbit __at (0xC3) P4_3 ;
    __sbit __at (0xC4) P4_4 ;
    __sbit __at (0xC5) P4_5 ;
    __sbit __at (0xC6) P4_6 ;
    __sbit __at (0xC7) P4_7 ;

// S. Qu  12/30/2008 4:23PM
__sfr __at (0xE8) P5;       //8-bit port 5
    __sbit __at (0xE8) P5_0 ;
    __sbit __at (0xE9) P5_1 ;
    __sbit __at (0xEA) P5_2 ;
    __sbit __at (0xEB) P5_3 ;
    __sbit __at (0xEC) P5_4 ;
    __sbit __at (0xED) P5_5 ;
    __sbit __at (0xEE) P5_6 ;
    __sbit __at (0xEF) P5_7 ;

__sfr __at (0xA6) WDTRST;   //WatchDog Timer Reset
__sfr __at (0xA7) WDTPRG;   //WatchDog Timer Program
	#define WTO2 0x04		//WDT Time-out select bit 2
	#define WTO1 0x02		//WDT Time-out select bit 1
	#define WTO0 0x01		//WDT Time-out select bit 0
	//This names appear also in the datasheet:
	#define S2 0x04 		//WDT Time-out select bit 2
	#define S1 0x02 		//WDT Time-out select bit 1
	#define S0 0x01 		//WDT Time-out select bit 0
	//S2 S1 S0 Selected Time-out
	//0  0  0  (2^14 - 1) machine cycles, 16.3 ms @ FOSCA=12 MHz
	//0  0  1  (2^15 - 1) machine cycles, 32.7 ms @ FOSCA=12 MHz
	//0  1  0  (2^16 - 1) machine cycles, 65.5 ms @ FOSCA=12 MHz
	//0  1  1  (2^17 - 1) machine cycles, 131  ms @ FOSCA=12 MHz
	//1  0  0  (2^18 - 1) machine cycles, 262  ms @ FOSCA=12 MHz
	//1  0  1  (2^19 - 1) machine cycles, 542  ms @ FOSCA=12 MHz
	//1  1  0  (2^20 - 1) machine cycles, 1.05  s @ FOSCA=12 MHz
	//1  1  1  (2^21 - 1) machine cycles, 2.09  s @ FOSCA=12 MHz

__sfr __at (0xA9) SADDR;	//Serial Port Address Register.
__sfr __at (0xB9) SADEN;	//Serial Port Address Enable.

__sfr __at (0xC3) SPCON;	//SPI Control Register
	#define SPR2  0x80		//SPI Clork Rate select bit 2.
	#define SPEN  0x40      //SPI enable bit.  When set enables SPI.
	#define SSDIS 0x20		//Cleared to enable SS in both Master and Slave modes.
	#define MSTR  0x10		//1=master mode.  0=slave mode.
	#define CPOL  0x08		//1=SCK is high when idle (active low), 0=SCK is low when idle (active high).
	#define CPHA  0x04		//1=shift triggered on the trailing edge of SCK.  0=shift trig. on leading edge.
	#define SPR1  0x02		//SPI Clork Rate select bit 1.
	#define SPR0  0x01		//SPI Clork Rate select bit 0.
		//SPR2 SPR1 SPR0  Baud Rate Divisor
		//  0    0    0   2
		//  0    0    1   4
		//  0    1    0   8
		//  0    1    1   16
		//  1    0    0   32
		//  1    0    1   64
		//  1    1    0   128
		//  1    1    1   Invalid: Don't Use

__sfr __at (0xC4) SPSTA;	//Serial Peripheral Status register
	#define SPIF  0x80		//Serial Peripheral Data Transfer Flag
	#define WCOL  0x40		//Write collision Flag.
	#define SSERR 0x20      //Synchronous Serial Slave Error Flag
	#define MODF  0x10      //Mode Fault Flag

__sfr __at (0xC5) SPDAT;	//SPI Data

__sfr __at (0xC9) T2MOD;	//Timer 2 mode control
	#define T2OE  0x02		//Timer 2 Output Enable bit.
	#define DCEN  0x01		//Down count enable

__sfr __at (0x9B) BDRCON;	//Baud Rate Control
	#define BRR   0x10		//Baud Rate Run Control bit. '1'=enable
	#define TBCK  0x08		//Transmission Baud rate Generator Selection bit for UART
	#define RBCK  0x04		//Reception Baud Rate Generator Selection bit for UART
	#define SPD   0x02		//Baud Rate Speed Control bit for UART
	#define SRC   0x01		//Baud Rate Source select bit in Mode 0 for UART

__sfr __at (0x9A) BRL;      //Baud Rate Reload

__sfr __at (0x9C) KBLS;     //Keyboard level Selector
__sfr __at (0x9D) KBE;      //Keyboard Input Enable
__sfr __at (0x9E) KBF;      //Keyboard Flag Register

__sfr __at (0xD2) EECON;    //EEPROM Data Control
	#define EEE    0x02		//EEPROM Enable. '1'=use EEPROM, '0'=use XRAM
	#define EEBUSY 0x01		//EEPROM Busy. '1'=EEPROM is busy programming

// PCON bit definitions
	#define SMOD1 0x80
	#define SMOD0 0x40
	#define POF   0x10

/* Interrupt numbers: address = (number * 8) + 3 */
#define TF2_VECTOR      5       /* 0x2b timer 2 */
#define PCA_VECTOR      6       /* 0x33 Programmable Counter Array */
#define KBD_VECTOR      7       /* 0x3b Keyboard Interface */
#define SPI_VECTOR      9       /* 0x4b Serial Port Interface */

#endif /*REG_AT89C51ED2_H*/
