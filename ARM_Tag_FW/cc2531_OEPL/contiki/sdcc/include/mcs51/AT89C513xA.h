/*-------------------------------------------------------------------------
   Register Declarations for the Atmel AT89C513xA Processor Range

   Copyright (C) 2010 - Anirban Brahmachari, a.brahmachari@gmail.com

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

#ifndef AT89C5131AUM_H
#define AT89C5131AUM_H

#include <compiler.h>

SFR(P0,         0x80);      // Port 0
   SBIT(P0_0,     0x80, 0); // Port 0 bit 0
   SBIT(P0_1,     0x80, 1); // Port 0 bit 1
   SBIT(P0_2,     0x80, 2); // Port 0 bit 2
   SBIT(P0_3,     0x80, 3); // Port 0 bit 3
   SBIT(P0_4,     0x80, 4); // Port 0 bit 4
   SBIT(P0_5,     0x80, 5); // Port 0 bit 5
   SBIT(P0_6,     0x80, 6); // Port 0 bit 6
   SBIT(P0_7,     0x80, 7); // Port 0 bit 7
SFR(SP,         0x81);      // Stack Pointer ; LSB of SPX
SFR(DPL,        0x82);      // Data Pointer Low Byte
SFR(DPH,        0x83);      // Data Pointer High Byte
SFR(PCON,       0x87);      // Power Mode Control
SFR(TCON,       0x88);      // Timer Control
   SBIT(IT0,      0x88, 0); // Ext. Interrupt 0 Type Select
   SBIT(IE0,      0x88, 1); // Ext. Interrupt 0 Flag
   SBIT(IT1,      0x88, 2); // Ext. Interrupt 1 Type Select
   SBIT(IE1,      0x88, 3); // Ext. Interrupt 1 Flag
   SBIT(TR0,      0x88, 4); // Timer 0 Run Control
   SBIT(TF0,      0x88, 5); // Timer 0 Overflow Flag
   SBIT(TR1,      0x88, 6); // Timer 1 Run Control
   SBIT(TF1,      0x88, 7); // Timer 1 Overflow Flag
SFR(TMOD,       0x89);      // Timer Mode
SFR16E(TMR0,    0x8C8A);    // Timer/Counter 0 Word
   SFR(TL0,       0x8A);    // Timer/Counter 0 Low Byte
   SFR(TH0,       0x8C);    // Timer/Counter 0 High Byte
SFR16E(TMR1,    0x8D8B);    // Timer/Counter 1 Word
   SFR(TL1,       0x8B);    // Timer/Counter 1 Low Byte
   SFR(TH1,       0x8D);    // Timer/Counter 1 High Byte
SFR(AUXR,       0x8E);      // Auxiliary Register
SFR(CKCON0,     0x8F);      // Clock Control 0
SFR(P1,         0x90);      // Port 1
   SBIT(P1_0,     0x90, 0); // Port 1 bit 0
   SBIT(P1_1,     0x90, 1); // Port 1 bit 1
   SBIT(P1_2,     0x90, 2); // Port 1 bit 2
   SBIT(P1_3,     0x90, 3); // Port 1 bit 3
   SBIT(P1_4,     0x90, 4); // Port 1 bit 4
   SBIT(P1_5,     0x90, 5); // Port 1 bit 5
   SBIT(P1_6,     0x90, 6); // Port 1 bit 6
   SBIT(P1_7,     0x90, 7); // Port 1 bit 7
SFR(SSCON,      0x93);      // Synchronous Serial Control
SFR(SSCS,       0x94);      // Synchronous Serial Control-Status
SFR(SSDAT,      0x95);      // Synchronous Serial Data
SFR(SSADR,      0x96);      // Synchronous Serial Address
SFR(SCON,       0x98);      // Serial Port 0 Control
   SBIT(RI,       0x98, 0); // Receive Interrupt Flag
   SBIT(TI,       0x98, 1); // Transmit Interrupt Flag
   SBIT(RB8,      0x98, 2); // Ninth Receive Bit
   SBIT(TB8,      0x98, 3); // Ninth Transmission Bit
   SBIT(REN,      0x98, 4); // Receive Enable
   SBIT(SM2,      0x98, 5); // Multiprocessor Communication Enable
   SBIT(SM1,      0x98, 6); // Serial Port Mode 1
   SBIT(SM0,      0x98, 7); // Serial Port Mode 0
   SBIT(FE,       0x98, 7); // Framing Error
SFR(SBUF,       0x99);      // Serial Port Data Buffer
SFR(BRL,        0x9A);      // Baud Rate Reload
SFR(BDRCON,     0x9B);      // Baud Rate Control
SFR(KBLS,       0x9C);      // Keyboard Level Selector Register
SFR(KBE,        0x9D);      // Keyboard Input Enable Register
SFR(KBF,        0x9E);      // Keyboard Flag Register
SFR(P2,         0xA0);      // Port 2
   SBIT(P2_0,     0xA0, 0); // Port 2 bit 0
   SBIT(P2_1,     0xA0, 1); // Port 2 bit 1
   SBIT(P2_2,     0xA0, 2); // Port 2 bit 2
   SBIT(P2_3,     0xA0, 3); // Port 2 bit 3
   SBIT(P2_4,     0xA0, 4); // Port 2 bit 4
   SBIT(P2_5,     0xA0, 5); // Port 2 bit 5
   SBIT(P2_6,     0xA0, 6); // Port 2 bit 6
   SBIT(P2_7,     0xA0, 7); // Port 2 bit 7
SFR(AUXR1,      0xA2);      // Auxiliary Register 1
SFR(PLLCON,     0xA3);      // PLL Control
SFR(PLLDIV,     0xA4);      // PLL Divider
SFR(WDTRST,     0xA6);      // Watchdog Timer Reset
SFR(WDTPRG,     0xA7);      // Watchdog Timer Program
SFR(IE,         0xA8);      // Interrupt Enable (traditional 8051 name)
SFR(IEN0,       0xA8);      // Interrupt Enable Control 0
   SBIT(EX0,      0xA8, 0); // Enable External Interrupt 0
   SBIT(ET0,      0xA8, 1); // Enable Timer 0 Interrupt
   SBIT(EX1,      0xA8, 2); // Enable External Interrupt 1
   SBIT(ET1,      0xA8, 3); // Enable Timer 1 Interrupt
   SBIT(ES,       0xA8, 4); // Enable Serial Port Interrupt
   SBIT(ET2,      0xA8, 5); // Enable Timer 2 Interrupt
   SBIT(EC,       0xA8, 6); // Enable PCA Interrupt
   SBIT(EA,       0xA8, 7); // Global Interrupt Enable
SFR(SADDR,      0xA9);      // Slave Address
SFR(CKCON1,     0xAF);      // Clock Control 1
SFR(P3,         0xB0);      // Port 3
   SBIT(P3_0,     0xB0, 0); // Port 3 bit 0
   SBIT(P3_1,     0xB0, 1); // Port 3 bit 1
   SBIT(P3_2,     0xB0, 2); // Port 3 bit 2
   SBIT(P3_3,     0xB0, 3); // Port 3 bit 3
   SBIT(P3_4,     0xB0, 4); // Port 3 bit 4
   SBIT(P3_5,     0xB0, 5); // Port 3 bit 5
   SBIT(P3_6,     0xB0, 6); // Port 3 bit 6
   SBIT(P3_7,     0xB0, 7); // Port 3 bit 7
SFR(IEN1,       0xB1);      // Interrupt Enable Control 1
SFR(IPL1,       0xB2);      // Interrupt Priority Control Low 1
SFR(IPH1,       0xB3);      // Interrupt Priority Control High 1
SFR(IPH0,       0xB7);      // Interrupt Priority Control High 0
SFR(IPL0,       0xB8);      // Interrupt Priority Control Low 0
   SBIT(PX0L,     0xB8, 0); // External interrupt 0 Priority bit
   SBIT(PT0L,     0xB8, 1); // Timer 0 overflow interrupt Priority bit
   SBIT(PX1L,     0xB8, 2); // External interrupt 1 Priority bit
   SBIT(PT1L,     0xB8, 3); // Timer 1 overflow interrupt Priority bit
   SBIT(PSL,      0xB8, 4); // Serial port Priority bit
   SBIT(PT2L,     0xB8, 5); // Timer 2 overflow interrupt Priority bit
   SBIT(PPCL,     0xB8, 6); // PCA interrupt Priority bit
SFR(SADEN,      0xB9);      // Slave Address Mask
SFR16E(UFNUM,   0xBBBA);    // USB Frame Number Word
   SFR(UFNUML,    0xBA);    // USB Frame Number Low
   SFR(UFNUMH,    0xBB);    // USB Frame Number High
SFR(USBCON,     0xBC);      // USB Global Control
SFR(USBINT,     0xBD);      // USB Global Interrupt
SFR(USBIEN,     0xBE);      // USB Global Interrupt Enable
SFR(P4,         0xC0);      // Port 4
   SBIT(P4_0,     0xC0, 0); // Port 4 bit 0
   SBIT(P4_1,     0xC0, 1); // Port 4 bit 1
SFR(UEPIEN,     0xC2);      // USB Endpoint Interrupt Enable
SFR(SPCON,      0xC3);      // Serial Peripheral Control
SFR(SPSTA,      0xC4);      // Serial Peripheral Status-Control
SFR(SPDAT,      0xC5);      // Serial Peripheral Data
SFR(USBADDR,    0xC6);      // USB Address
SFR(UEPNUM,     0xC7);      // USB Endpoint Number
SFR(T2CON,      0xC8);      // Timer 2 Control
   SBIT(CP_RL2,   0xC8, 0); // Timer 2 Capture/Reload bit
   SBIT(C_T2,     0xC8, 1); // Timer/Counter 2 select bit
   SBIT(TR2,      0xC8, 2); // Timer 2 Run control bit
   SBIT(EXEN2,    0xC8, 3); // Timer 2 External Enable bit
   SBIT(TCLK,     0xC8, 4); // Transmit Clock bit
   SBIT(RCLK,     0xC8, 5); // Receive Clock bit
   SBIT(EXF2,     0xC8, 6); // Timer 2 External Flag
   SBIT(TF2,      0xC8, 7); // Timer 2 overflow Flag
SFR(T2MOD,      0xC9);      // Timer 2 Mode
SFR16E(RCAP2,   0xCBCA);    // Timer/Counter 2 Reload/Capture Word
   SFR(RCAP2L,    0xCA);    // Timer/Counter 2 Reload/Capture Low byte
   SFR(RCAP2H,    0xCB);    // Timer/Counter 2 Reload/Capture High byte
SFR16E(TMR2,    0xCDCC);    // Timer/Counter 2 Word
   SFR(TL2,     0xCC);      // Timer/Counter 2 Low Byte
   SFR(TH2,     0xCD);      // Timer/Counter 2 High Byte
SFR(UEPSTAX,    0xCE);      // USB Endpoint X Status
SFR(UEPDATX,    0xCF);      // USB Endpoint X FIFO Data
SFR(PSW,        0xD0);      // Program Status Word
   SBIT(P,        0xD0, 0); // Parity Flag
   SBIT(F1,       0xD0, 1); // User-Defined Flag
   SBIT(OV,       0xD0, 2); // Overflow Flag
   SBIT(RS0,      0xD0, 3); // Register Bank Select 0
   SBIT(RS1,      0xD0, 4); // Register Bank Select 1
   SBIT(F0,       0xD0, 5); // User-Defined Flag
   SBIT(AC,       0xD0, 6); // Auxiliary Carry Flag
   SBIT(CY,       0xD0, 7); // Carry Flag
SFR(FCON,       0xD1);      // Flash Control
SFR(EECON,      0xD2);      // EEPROM Contol
SFR(UEPCONX,    0xD4);      // USB Endpoint X Control
SFR(UEPRST,     0xD5);      // USB Endpoint Reset
SFR(CCON,       0xD8);      // PCA Timer/Counter Control
   SBIT(CCF0,     0xD8,0);  // PCA Module 0 interrupt flag
   SBIT(CCF1,     0xD8,1);  // PCA Module 1 interrupt flag
   SBIT(CCF2,     0xD8,2);  // PCA Module 2 interrupt flag
   SBIT(CCF3,     0xD8,3);  // PCA Module 3 interrupt flag
   SBIT(CCF4,     0xD8,4);  // PCA Module 4 interrupt flag
   SBIT(CR,       0xD8,6);  // PCA Counter Run control bit
   SBIT(CF,       0xD8,7);  // PCA Counter Overflow flag
SFR(CMOD,       0xD9);      // PCA Timer/Counter Mode
SFR(CCAPM0,     0xDA);      // PCA Timer/Counter 0 Mode
SFR(CCAPM1,     0xDB);      // PCA Timer/Counter 1 Mode
SFR(CCAPM2,     0xDC);      // PCA Timer/Counter 2 Mode
SFR(CCAPM3,     0xDD);      // PCA Timer/Counter 3 Mode
SFR(CCAPM4,     0xDE);      // PCA Timer/Counter 4 Mode
SFR(ACC,        0xE0);      // Accumulator
SFR16E(UBYCTX,  0xE3E2);    // USB Byte Counter Word
   SFR(UBYCTLX,   0xE2);    // USB Byte Counter Low
   SFR(UBYCTHX,   0xE3);    // USB Byte Counter High
SFR16E(PCA,     0xF9E9);    // PCA Timer/Counter Word
   SFR(CL,        0xE9);    // PCA Timer/Counter Low byte
   SFR(CH,        0xF9);    // PCA Timer/Counter High byte
SFR16E(CCAP0,   0xFAEA);    // PCA Compare Capture Module 0 Word
   SFR(CCAP0L,    0xEA);    // PCA Compare Capture Module 0 Low byte
   SFR(CCAP0H,    0xFA);    // PCA Compare Capture Module 0 High byte
SFR16E(CCAP1,   0xFBEB);    // PCA Compare Capture Module 1 Word
   SFR(CCAP1L,    0xEB);    // PCA Compare Capture Module 1 Low byte
   SFR(CCAP1H,    0xFB);    // PCA Compare Capture Module 1 High byte
SFR16E(CCAP2,   0xFCEC);    // PCA Compare Capture Module 2 Word
   SFR(CCAP2L,    0xEC);    // PCA Compare Capture Module 2 Low byte
   SFR(CCAP2H,    0xFC);    // PCA Compare Capture Module 2 High byte
SFR16E(CCAP3,   0xFDED);    // PCA Compare Capture Module 3 Word
   SFR(CCAP3L,    0xED);    // PCA Compare Capture Module 3 Low byte
   SFR(CCAP3H,    0xFD);    // PCA Compare Capture Module 3 High byte
SFR16E(CCAP4,   0xFEEE);    // PCA Compare Capture Module 4 Word
   SFR(CCAP4L,    0xEE);    // PCA Compare Capture Module 4 Low byte
   SFR(CCAP4H,    0xFE);    // PCA Compare Capture Module 4 High byte
SFR(B,          0xF0);      // B Register
SFR(LEDCON,     0xF1);      // LED Control
SFR(UEPINT,     0xF8);      // USB Endpoint Interrupt
   SBIT(EP0INT,   0xF8,0);  // Endpoint 0 Interrupt
   SBIT(EP1INT,   0xF8,1);  // Endpoint 1 Interrupt
   SBIT(EP2INT,   0xF8,2);  // Endpoint 2 Interrupt
   SBIT(EP3INT,   0xF8,3);  // Endpoint 3 Interrupt
   SBIT(EP4INT,   0xF8,4);  // Endpoint 4 Interrupt
   SBIT(EP5INT,   0xF8,5);  // Endpoint 5 Interrupt
   SBIT(EP6INT,   0xF8,6);  // Endpoint 6 Interrupt

/* Predefined SFR Bit Masks */

#define PCON_IDLE         0x01    /* PCON                                */
#define PCON_STOP         0x02    /* PCON                                */

/* Interrupts */
#define INT_RESET         0       // Reset
#define INT_EXT0          1       // External Interrupt 0
#define INT_TIMER0        2       // Timer0 Overflow
#define INT_EXT1          3       // External Interrupt 1
#define INT_TIMER1        4       // Timer1 Overflow
#define INT_UART0         5       // Serial Port 0
#define INT_TIMER2        6       // Timer2 Overflow
#define INT_PCA           7       // Programmable Counter Array
#define INT_KEYBOARD      8       // Keyboard on P1
#define INT_TWI           9       // Two Wire Interface
#define INT_SPI           10      // Serial Peripheral Interface
//                        11         Reserved
//                        12         Reserved
//                        13         Reserved
#define INT_USB           14      // USB events
//                        15         Reserved

#endif
