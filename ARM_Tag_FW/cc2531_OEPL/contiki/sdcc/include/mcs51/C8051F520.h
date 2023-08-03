/*-------------------------------------------------------------------------
   C8051F520.h - Register Declarations for the SiLabs C8051F52x-F53x
   Processor Range

   Copyright (C) 2006, Maarten Brock, sourceforge.brock@dse.nl

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

#ifndef C8051F520_H
#define C8051F520_H

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
SFR(SP,         0x81);      // Stack Pointer
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
SFR(CKCON,      0x8E);      // Clock Control
SFR(PSCTL,      0x8F);      // Program Store R/W Control
SFR(P1,         0x90);      // Port 1
   SBIT(P1_0,     0x90, 0); // Port 1 bit 0
   SBIT(P1_1,     0x90, 1); // Port 1 bit 1
   SBIT(P1_2,     0x90, 2); // Port 1 bit 2
   SBIT(P1_3,     0x90, 3); // Port 1 bit 3
   SBIT(P1_4,     0x90, 4); // Port 1 bit 4
   SBIT(P1_5,     0x90, 5); // Port 1 bit 5
   SBIT(P1_6,     0x90, 6); // Port 1 bit 6
   SBIT(P1_7,     0x90, 7); // Port 1 bit 7
SFR(LINADDR,    0x92);      // LIN Indirect Address Pointer
SFR(LINDATA,    0x93);      // LIN Indirect Data Buffer
SFR(LINCF,      0x95);      // LIN Control Mode
SFR(SCON0,      0x98);      // Serial Port 0 Control
   SBIT(RI0,      0x98, 0); // Receive Interrupt Flag
   SBIT(TI0,      0x98, 1); // Transmit Interrupt Flag
   SBIT(RB80,     0x98, 2); // Ninth Receive Bit
   SBIT(TB80,     0x98, 3); // Ninth Transmission Bit
   SBIT(REN0,     0x98, 4); // Receive Enable
   SBIT(MCE0,     0x98, 5); // Multiprocessor Communication Enable
   SBIT(S0MODE,   0x98, 7); // Serial Port 0 Operation Mode
SFR(SBUF0,      0x99);      // Serial Port 0 Data Buffer
SFR(CPT0CN,     0x9B);      // Comparator 0 Control
SFR(CPT0MD,     0x9D);      // Comparator 0 Mode Selection
SFR(CPT0MX,     0x9F);      // Comparator 0 MUX Selection
SFR(SPI0CFG,    0xA1);      // SPI Configuration
SFR(SPI0CKR,    0xA2);      // SPI Clock Rate Control
SFR(SPI0DAT,    0xA3);      // SPI Data
SFR(P0MDOUT,    0xA4);      // Port 0 Output Mode Configuration
SFR(P1MDOUT,    0xA5);      // Port 1 Output Mode Configuration
SFR(IE,         0xA8);      // Interrupt Enable
   SBIT(EX0,      0xA8, 0); // Enable External Interrupt 0
   SBIT(ET0,      0xA8, 1); // Enable Timer 0 Interrupt
   SBIT(EX1,      0xA8, 2); // Enable External Interrupt 1
   SBIT(ET1,      0xA8, 3); // Enable Timer 1 Interrupt
   SBIT(ES0,      0xA8, 4); // Enable Serial Port Interrupt
   SBIT(ET2,      0xA8, 5); // Enable Timer 2 Interrupt
   SBIT(ESPI0,    0xA8, 6); // Enable SPI0 Interrupt
   SBIT(EA,       0xA8, 7); // Global Interrupt Enable
SFR(CLKSEL,     0xA9);      // Clock Select
SFR(OSCIFIN,    0xB0);      // Internal Oscillator Fine Calibration
SFR(OSCXCN,     0xB1);      // External Oscillator Control
SFR(OSCICN,     0xB2);      // Internal Oscillator Control
SFR(OSCICL,     0xB3);      // Internal Oscillator Calibration
SFR(FLKEY,      0xB7);      // Flash Lock and Key
SFR(IP,         0xB8);      // Interrupt Priority
   SBIT(PX0,      0xB8, 0); // External Interrupt 0 Priority
   SBIT(PT0,      0xB8, 1); // Timer 0 Interrupt Priority
   SBIT(PX1,      0xB8, 2); // External Interrupt 1 Priority
   SBIT(PT1,      0xB8, 3); // Timer 1 Interrupt Priority
   SBIT(PS0,      0xB8, 4); // Serial Port Interrupt Priority
   SBIT(PT2,      0xB8, 5); // Timer 2 Interrupt Priority
   SBIT(PSPI0,    0xB8, 6); // SPI0 Interrupt Priority
SFR(ADC0TK,     0xBA);      // ADC0 Tracking Mode Select
SFR(ADC0MX,     0xBB);      // ADC0 Channel Select
SFR(ADC0CF,     0xBC);      // ADC0 Configuration
SFR16(ADC0,     0xBD);      // ADC0 Word
   SFR(ADC0L,     0xBD);    // ADC0 Low Byte
   SFR(ADC0H,     0xBE);    // ADC0 High Byte
SFR(P1MASK,     0xBF);      // Port 1 Mask
SFR16(ADC0GT,   0xC3);      // ADC0 Greater-Than Data Word
   SFR(ADC0GTL,   0xC3);    // ADC0 Greater-Than Data Low Byte
   SFR(ADC0GTH,   0xC4);    // ADC0 Greater-Than Data High Byte
SFR16(ADC0LT,   0xC5);      // ADC0 Less-Than Data Word
   SFR(ADC0LTL,   0xC5);    // ADC0 Less-Than Data Low Byte
   SFR(ADC0LTH,   0xC6);    // ADC0 Less-Than Data High Byte
SFR(P0MASK,     0xC7);      // Port 0 Mask
SFR(TMR2CN,     0xC8);      // Timer/Counter 2 Control
SFR(REG0CN,     0xC9);      // Voltage Regulator Control
SFR16(TMR2RL,   0xCA);      // Timer/Counter 2 Reload Word
   SFR(TMR2RLL,   0xCA);    // Timer/Counter 2 Reload Low Byte
   SFR(TMR2RLH,   0xCB);    // Timer/Counter 2 Reload High Byte
SFR16(TMR2,     0xCC);      // Timer/Counter 2 Word
   SFR(TMR2L,     0xCC);    // Timer/Counter 2 Low Byte
   SFR(TMR2H,     0xCD);    // Timer/Counter 2 High Byte
SFR(P1MAT,      0xCF);      // Port 1 Match
SFR(PSW,        0xD0);      // Program Status Word
   SBIT(P,        0xD0, 0); // Parity Flag
   SBIT(F1,       0xD0, 1); // User-Defined Flag
   SBIT(OV,       0xD0, 2); // Overflow Flag
   SBIT(RS0,      0xD0, 3); // Register Bank Select 0
   SBIT(RS1,      0xD0, 4); // Register Bank Select 1
   SBIT(F0,       0xD0, 5); // User-Defined Flag
   SBIT(AC,       0xD0, 6); // Auxiliary Carry Flag
   SBIT(CY,       0xD0, 7); // Carry Flag
SFR(REF0CN,     0xD1);      // Voltage Reference Control
SFR(P0SKIP,     0xD4);      // Port 0 Skip
SFR(P1SKIP,     0xD5);      // Port 1 Skip
SFR(P0MAT,      0xD7);      // Port 0 Match
SFR(PCA0CN,     0xD8);      // PCA Control
   SBIT(CCF0,     0xD8, 0); // PCA Module 0 Capture/Compare Flag
   SBIT(CCF1,     0xD8, 1); // PCA Module 1 Capture/Compare Flag
   SBIT(CCF2,     0xD8, 2); // PCA Module 2 Capture/Compare Flag
   SBIT(CR,       0xD8, 6); // PCA Counter/Timer Run Control
   SBIT(CF,       0xD8, 7); // PCA Counter/Timer Overflow Flag
SFR(PCA0MD,     0xD9);      // PCA Mode
SFR(PCA0CPM0,   0xDA);      // PCA Module 0 Mode
SFR(PCA0CPM1,   0xDB);      // PCA Module 1 Mode
SFR(PCA0CPM2,   0xDC);      // PCA Module 2 Mode
SFR(ACC,        0xE0);      // Accumulator
SFR(XBR0,       0xE1);      // Port I/O Crossbar Control 0
SFR(XBR1,       0xE2);      // Port I/O Crossbar Control 1
SFR(IT01CF,     0xE4);      // INT0/INT1 Configuration
SFR(EIE1,       0xE6);      // Extended Interrupt Enable 1
SFR(ADC0CN,     0xE8);      // ADC0 Control
   SBIT(AD0CM0,   0xE8, 0); // ADC0 Conversion Start Mode Select Bit 0
   SBIT(AD0CM1,   0xE8, 1); // ADC0 Conversion Start Mode Select Bit 1
   SBIT(AD0LJST,  0xE8, 2); // ADC0 Left Justify Select
   SBIT(AD0WINT,  0xE8, 3); // ADC0 Window Compare Interrupt Flag
   SBIT(AD0BUSY,  0xE8, 4); // ADC0 Busy Bit
   SBIT(AD0INT,   0xE8, 5); // ADC0 Conversion Complete Interrupt Flag
   SBIT(BURSTEN,  0xE8, 6); // ADC0 Burst Mode Enable Bit
   SBIT(AD0EN,    0xE8, 7); // ADC0 Enable Bit
SFR16(PCA0CP1,  0xE9);      // PCA Capture 1 Word
   SFR(PCA0CPL1,  0xE9);    // PCA Capture 1 Low Byte
   SFR(PCA0CPH1,  0xEA);    // PCA Capture 1 High Byte
SFR16(PCA0CP2,  0xEB);      // PCA Capture 2 Word
   SFR(PCA0CPL2,  0xEB);    // PCA Capture 2 Low Byte
   SFR(PCA0CPH2,  0xEC);    // PCA Capture 2 High Byte
SFR(RSTSRC,     0xEF);      // Reset Source Configuration/Status
SFR(B,          0xF0);      // B Register
SFR(P0MDIN,     0xF1);      // Port 0 Input Mode Configuration
SFR(P1MDIN,     0xF2);      // Port 1 Input Mode Configuration
SFR(EIP1,       0xF6);      // Extended Interrupt Priority 1
SFR(SPI0CN,     0xF8);      // SPI0 Control
   SBIT(SPIEN,    0xF8, 0); // SPI0 Enable
   SBIT(TXBMT,    0xF8, 1); // SPI0 Transmit Buffer Empty
   SBIT(NSSMD0,   0xF8, 2); // SPI0 Slave Select Mode Bit 0
   SBIT(NSSMD1,   0xF8, 3); // SPI0 Slave Select Mode Bit 1
   SBIT(RXOVRN,   0xF8, 4); // SPI0 Receive Overrun Flag
   SBIT(MODF,     0xF8, 5); // SPI0 Mode Fault Flag
   SBIT(WCOL,     0xF8, 6); // SPI0 Write Collision Flag
   SBIT(SPIF,     0xF8, 7); // SPI0 Interrupt Flag
SFR16(PCA0,     0xF9);      // PCA Counter Word
   SFR(PCA0L,    0xF9);    // PCA Counter Low Byte
   SFR(PCA0H,     0xFA);    // PCA Counter High Byte
SFR16(PCA0CP0,  0xFB);      // PCA Capture 0 Word
   SFR(PCA0CPL0,  0xFB);    // PCA Capture 0 Low Byte
   SFR(PCA0CPH0,  0xFC);    // PCA Capture 0 High Byte
SFR(VDDMON,     0xFF);      // VDD Control

/* Predefined SFR Bit Masks */

#define PCON_IDLE         0x01    /* PCON                                */
#define PCON_STOP         0x02    /* PCON                                */
#define T1M               0x08    /* CKCON                               */
#define PSWE              0x01    /* PSCTL                               */
#define PSEE              0x02    /* PSCTL                               */
#define PORSF             0x02    /* RSTSRC                              */
#define SWRSF             0x10    /* RSTSRC                              */
#define ECCF              0x01    /* PCA0CPMn                            */
#define PWM               0x02    /* PCA0CPMn                            */
#define TOG               0x04    /* PCA0CPMn                            */
#define MAT               0x08    /* PCA0CPMn                            */
#define CAPN              0x10    /* PCA0CPMn                            */
#define CAPP              0x20    /* PCA0CPMn                            */
#define ECOM              0x40    /* PCA0CPMn                            */
#define PWM16             0x80    /* PCA0CPMn                            */
#define CP0E              0x10    /* XBR0                                */
#define CP0AE             0x20    /* XBR0                                */

/* Interrupts */

#define INT_EXT0          0       // External Interrupt 0
#define INT_TIMER0        1       // Timer0 Overflow
#define INT_EXT1          2       // External Interrupt 1
#define INT_TIMER1        3       // Timer1 Overflow
#define INT_UART0         4       // Serial Port 0
#define INT_TIMER2        5       // Timer2 Overflow
#define INT_SPI0          6       // Serial Peripheral Interface 0
#define INT_ADC0_WINDOW   7       // ADC0 Window Comparison
#define INT_ADC0_EOC      8       // ADC0 End Of Conversion
#define INT_PCA0          9       // PCA0 Peripheral
#define INT_COMP_FALLING  10      // Comparator0 Falling
#define INT_COMP_RISING   11      // Comparator0 Rising
#define INT_LIN           12      // LIN
#define INT_VREG_DROPOUT  13      // VREG dropout
#define INT_PORT_MATCH    14      // Port Match

#endif
