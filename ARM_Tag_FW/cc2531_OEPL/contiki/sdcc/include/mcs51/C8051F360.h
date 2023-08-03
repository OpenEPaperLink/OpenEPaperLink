/*-------------------------------------------------------------------------
   C8051F360.h - Register Declarations for the SiLabs C8051F36x
   Processor Range

   Copyright (C) 2007, Maarten Brock, sourceforge.brock@dse.nl

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

#ifndef C8051F360_H
#define C8051F360_H

#include <compiler.h>

/*  All Pages */

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
SFR(SFRNEXT,    0x85);      // SFR Stack Next Page
SFR(SFRLAST,    0x86);      // SFR Stack Last Page
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
SFR(P1,         0x90);      // Port 1
   SBIT(P1_0,     0x90, 0); // Port 1 bit 0
   SBIT(P1_1,     0x90, 1); // Port 1 bit 1
   SBIT(P1_2,     0x90, 2); // Port 1 bit 2
   SBIT(P1_3,     0x90, 3); // Port 1 bit 3
   SBIT(P1_4,     0x90, 4); // Port 1 bit 4
   SBIT(P1_5,     0x90, 5); // Port 1 bit 5
   SBIT(P1_6,     0x90, 6); // Port 1 bit 6
   SBIT(P1_7,     0x90, 7); // Port 1 bit 7
SFR(TMR3CN,     0x91);      // Timer 3 Control
SFR16(TMR3RL,   0x92);      // Timer 3 Reload Register Word
   SFR(TMR3RLL,    0x92);   // Timer 3 Reload Register Low Byte
   SFR(TMR3RLH,    0x93);   // Timer 3 Reload Register High Byte
SFR16(TMR3,     0x94);      // Timer 3 Word
   SFR(TMR3L,      0x94);   // Timer 3 Low Byte
   SFR(TMR3H,      0x95);   // Timer 3 High Byte
SFR16(IDA0,     0x96);      // IDAC 0 Word
   SFR(IDA0L,      0x96);   // IDAC 0 Low Byte
   SFR(IDA0H,      0x97);   // IDAC 0 High Byte
SFR(SCON0,      0x98);      // Serial Port 0 Control
   SBIT(RI0,      0x98, 0); // Receive Interrupt Flag
   SBIT(TI0,      0x98, 1); // Transmit Interrupt Flag
   SBIT(RB80,     0x98, 2); // Ninth Receive Bit
   SBIT(TB80,     0x98, 3); // Ninth Transmission Bit
   SBIT(REN0,     0x98, 4); // Receive Enable
   SBIT(MCE0,     0x98, 5); // Multiprocessor Communication Enable
   SBIT(S0MODE,   0x98, 7); // Serial Port 0 Operation Mode
SFR(SBUF0,      0x99);      // Serial Port 0 Data Buffer
SFR(CPT1CN,     0x9A);      // Comparator 1 Control
SFR(CPT0CN,     0x9B);      // Comparator 0 Control
SFR(CPT1MD,     0x9C);      // Comparator 1 Mode Selection
SFR(CPT0MD,     0x9D);      // Comparator 0 Mode Selection
SFR(CPT1MX,     0x9E);      // Comparator 1 MUX Selection
SFR(CPT0MX,     0x9F);      // Comparator 0 MUX Selection
SFR(P2,         0xA0);      // Port 2
   SBIT(P2_0,     0xA0, 0); // Port 2 bit 0
   SBIT(P2_1,     0xA0, 1); // Port 2 bit 1
   SBIT(P2_2,     0xA0, 2); // Port 2 bit 2
   SBIT(P2_3,     0xA0, 3); // Port 2 bit 3
   SBIT(P2_4,     0xA0, 4); // Port 2 bit 4
   SBIT(P2_5,     0xA0, 5); // Port 2 bit 5
   SBIT(P2_6,     0xA0, 6); // Port 2 bit 6
   SBIT(P2_7,     0xA0, 7); // Port 2 bit 7
SFR(SPI0CFG,    0xA1);      // SPI Configuration
SFR(SPI0CKR,    0xA2);      // SPI Clock Rate Control
SFR(SPI0DAT,    0xA3);      // SPI Data
SFR(SFRPAGE,    0xA7);      // SFR Page Select
SFR(IE,         0xA8);      // Interrupt Enable
   SBIT(EX0,      0xA8, 0); // Enable External Interrupt 0
   SBIT(ET0,      0xA8, 1); // Enable Timer 0 Interrupt
   SBIT(EX1,      0xA8, 2); // Enable External Interrupt 1
   SBIT(ET1,      0xA8, 3); // Enable Timer 1 Interrupt
   SBIT(ES0,      0xA8, 4); // Enable Serial Port Interrupt
   SBIT(ET2,      0xA8, 5); // Enable Timer 2 Interrupt
   SBIT(ESPI0,    0xA8, 6); // Enable SPI0 Interrupt
   SBIT(EA,       0xA8, 7); // Global Interrupt Enable
SFR(EMI0CN,     0xAA);      // EMIF Control
SFR(_XPAGE,     0xAA);      // SDCC: XDATA/PDATA Page
SFR(P3,         0xB0);      // Port 3
   SBIT(P3_0,     0xB0, 0); // Port 3 bit 0
   SBIT(P3_1,     0xB0, 1); // Port 3 bit 1
   SBIT(P3_2,     0xB0, 2); // Port 3 bit 2
   SBIT(P3_3,     0xB0, 3); // Port 3 bit 3
   SBIT(P3_4,     0xB0, 4); // Port 3 bit 4
   SBIT(P3_5,     0xB0, 5); // Port 3 bit 5
   SBIT(P3_6,     0xB0, 6); // Port 3 bit 6
   SBIT(P3_7,     0xB0, 7); // Port 3 bit 7
SFR(P4,         0xB5);      // Port 4
SFR(IP,         0xB8);      // Interrupt Priority
   SBIT(PX0,      0xB8, 0); // External Interrupt 0 Priority
   SBIT(PT0,      0xB8, 1); // Timer 0 Interrupt Priority
   SBIT(PX1,      0xB8, 2); // External Interrupt 1 Priority
   SBIT(PT1,      0xB8, 3); // Timer 1 Interrupt Priority
   SBIT(PS0,      0xB8, 4); // Serial Port Interrupt Priority
   SBIT(PT2,      0xB8, 5); // Timer 2 Interrupt Priority
   SBIT(PSPI0,    0xB8, 6); // SPI0 Interrupt Priority
SFR(IDA0CN,     0xB9);      // IDAC 0 Control
SFR(AMX0N,      0xBA);      // AMUX 0 Negative Channel Select
SFR(AMX0P,      0xBB);      // AMUX 0 Positive Channel Select
SFR(ADC0CF,     0xBC);      // ADC0 Configuration
SFR16(ADC0,     0xBD);      // ADC0 Word
   SFR(ADC0L,     0xBD);    // ADC0 Low Byte
   SFR(ADC0H,     0xBE);    // ADC0 High Byte
SFR(SMB0CN,     0xC0);      // SMBus Control
   SBIT(SI,       0xC0, 0); // SMBus Interrupt Flag
   SBIT(ACK,      0xC0, 1); // SMBus Acknowledge Flag
   SBIT(ARBLOST,  0xC0, 2); // SMBus Arbitration Lost Indicator
   SBIT(ACKRQ,    0xC0, 3); // SMBus Acknowledge Request
   SBIT(STO,      0xC0, 4); // SMBus Stop Flag
   SBIT(STA,      0xC0, 5); // SMBus Start Flag
   SBIT(TXMODE,   0xC0, 6); // SMBus Transmit Mode Indicator
   SBIT(MASTER,   0xC0, 7); // SMBus Master/Slave Indicator
SFR(SMB0CF,     0xC1);      // SMBus Configuration
SFR(SMB0DAT,    0xC2);      // SMBus Data
SFR16(ADC0GT,   0xC3);      // ADC0 Greater-Than Data Word
   SFR(ADC0GTL,   0xC3);    // ADC0 Greater-Than Data Low Byte
   SFR(ADC0GTH,   0xC4);    // ADC0 Greater-Than Data High Byte
SFR16(ADC0LT,   0xC5);      // ADC0 Less-Than Data Word
   SFR(ADC0LTL,   0xC5);    // ADC0 Less-Than Data Low Byte
   SFR(ADC0LTH,   0xC6);    // ADC0 Less-Than Data High Byte
SFR(TMR2CN,     0xC8);      // Timer/Counter 2 Control
   SBIT(T2XCLK,   0xC8, 0); // Timer 2 External Clock Select
   SBIT(TR2,      0xC8, 2); // Timer 2 Run Control
   SBIT(T2SPLIT,  0xC8, 3); // Timer 2 Split Mode Enable
   SBIT(TF2CEN,   0xC8, 4); // Timer 2 Low-Frequency Oscillator Capture Enable
   SBIT(TF2LEN,   0xC8, 5); // Timer 2 Low Byte Interrupt Enable
   SBIT(TF2L,     0xC8, 6); // Timer 2 Low Byte Overflow Flag
   SBIT(TF2H,     0xC8, 7); // Timer 2 High Byte Overflow Flag
SFR16(TMR2RL,   0xCA);      // Timer/Counter 2 Reload Word
   SFR(TMR2RLL,   0xCA);    // Timer/Counter 2 Reload Low Byte
   SFR(TMR2RLH,   0xCB);    // Timer/Counter 2 Reload High Byte
SFR16(TMR2,     0xCC);      // Timer/Counter 2 Word
   SFR(TMR2L,     0xCC);    // Timer/Counter 2 Low Byte
   SFR(TMR2H,     0xCD);    // Timer/Counter 2 High Byte
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
SFR(PCA0CN,     0xD8);      // PCA Control
   SBIT(CCF0,     0xD8, 0); // PCA Module 0 Capture/Compare Flag
   SBIT(CCF1,     0xD8, 1); // PCA Module 1 Capture/Compare Flag
   SBIT(CCF2,     0xD8, 2); // PCA Module 2 Capture/Compare Flag
   SBIT(CCF3,     0xD8, 3); // PCA Module 3 Capture/Compare Flag
   SBIT(CCF4,     0xD8, 4); // PCA Module 4 Capture/Compare Flag
   SBIT(CCF5,     0xD8, 5); // PCA Module 5 Capture/Compare Flag
   SBIT(CR,       0xD8, 6); // PCA Counter/Timer Run Control
   SBIT(CF,       0xD8, 7); // PCA Counter/Timer Overflow Flag
SFR(PCA0MD,     0xD9);      // PCA Mode
SFR(PCA0CPM0,   0xDA);      // PCA Module 0 Mode
SFR(PCA0CPM1,   0xDB);      // PCA Module 1 Mode
SFR(PCA0CPM2,   0xDC);      // PCA Module 2 Mode
SFR(PCA0CPM3,   0xDD);      // PCA Module 3 Mode
SFR(PCA0CPM4,   0xDE);      // PCA Module 4 Mode
SFR(PCA0CPM5,   0xDF);      // PCA Module 5 Mode
SFR(ACC,        0xE0);      // Accumulator
SFR(IT01CF,     0xE4);      // INT0/INT1 Configuration
SFR(EIE1,       0xE6);      // Extended Interrupt Enable 1
SFR(EIE2,       0xE7);      // Extended Interrupt Enable 2
SFR(ADC0CN,     0xE8);      // ADC0 Control
   SBIT(AD0CM0,   0xE8, 0); // ADC0 Conversion Start Mode Select Bit 0
   SBIT(AD0CM1,   0xE8, 1); // ADC0 Conversion Start Mode Select Bit 1
   SBIT(AD0CM2,   0xE8, 2); // ADC0 Conversion Start Mode Select Bit 2
   SBIT(AD0WINT,  0xE8, 3); // ADC0 Window Compare Interrupt Flag
   SBIT(AD0BUSY,  0xE8, 4); // ADC0 Busy Bit
   SBIT(AD0INT,   0xE8, 5); // ADC0 Conversion Complete Interrupt Flag
   SBIT(AD0TM,    0xE8, 6); // ADC0 Track Mode Bit
   SBIT(AD0EN,    0xE8, 7); // ADC0 Enable Bit
SFR16(PCA0CP1,  0xE9);      // PCA Capture 1 Word
   SFR(PCA0CPL1,  0xE9);    // PCA Capture 1 Low Byte
   SFR(PCA0CPH1,  0xEA);    // PCA Capture 1 High Byte
SFR16(PCA0CP2,  0xEB);      // PCA Capture 2 Word
   SFR(PCA0CPL2,  0xEB);    // PCA Capture 2 Low Byte
   SFR(PCA0CPH2,  0xEC);    // PCA Capture 2 High Byte
SFR16(PCA0CP3,  0xED);      // PCA Capture 3 Word
   SFR(PCA0CPL3,  0xED);    // PCA Capture 3 Low Byte
   SFR(PCA0CPH3,  0xEE);    // PCA Capture 3 High Byte
SFR(RSTSRC,     0xEF);      // Reset Source Configuration/Status
SFR(B,          0xF0);      // B Register
SFR16(PCA0CP5,  0xF5);      // PCA Capture 5 Word
   SFR(PCA0CPL5,  0xF5);    // PCA Capture 5 Low Byte
   SFR(PCA0CPH5,  0xF6);    // PCA Capture 5 High Byte
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
   SFR(PCA0L,     0xF9);    // PCA Counter Low Byte
   SFR(PCA0H,     0xFA);    // PCA Counter High Byte
SFR16(PCA0CP0,  0xFB);      // PCA Capture 0 Word
   SFR(PCA0CPL0,  0xFB);    // PCA Capture 0 Low Byte
   SFR(PCA0CPH0,  0xFC);    // PCA Capture 0 High Byte
SFR16(PCA0CP4,  0xFD);      // PCA Capture 4 Word
   SFR(PCA0CPL4,  0xFD);    // PCA Capture 4 Low Byte
   SFR(PCA0CPH4,  0xFE);    // PCA Capture 4 High Byte
SFR(VDM0CN,     0xFF);      // VDD Monitor Control

/*  Page 0x00 */

SFR(PSCTL,      0x8F);      // Program Store R/W Control
SFR16(MAC0A,    0xA4);      // MAC0 A Register Word
   SFR(MAC0AL,    0xA4);    // MAC0 A Register Low Byte
   SFR(MAC0AH,    0xA5);    // MAC0 A Register High Byte
SFR16(MAC0RND,  0xAE);      // MAC0 Rounding Register Word
   SFR(MAC0RNDL,  0xAE);    // MAC0 Rounding Register Low Byte
   SFR(MAC0RNDH,  0xAF);    // MAC0 Rounding Register High Byte
SFR(P2MAT,      0xB1);      // Port 2 Match
SFR(P2MASK,     0xB2);      // Port 2 Mask
SFR(FLSCL,      0xB6);      // Flash Scale
SFR(FLKEY,      0xB7);      // Flash Lock and Key
SFR(MAC0STA,    0xCF);      // MAC0 Status Register
SFR32(MAC0ACC,  0xD2);      // MAC0 Accumulator Long Word
   SFR(MAC0ACC0,  0xD2);    // MAC0 Accumulator Byte 0 (LSB)
   SFR(MAC0ACC1,  0xD3);    // MAC0 Accumulator Byte 1
   SFR(MAC0ACC2,  0xD4);    // MAC0 Accumulator Byte 2
   SFR(MAC0ACC3,  0xD5);    // MAC0 Accumulator Byte 3 (MSB)
SFR(MAC0OVR,    0xD6);      // MAC0 Accumulator Overflow
SFR(MAC0CF,     0xD7);      // MAC0 Configuration
SFR(P1MAT,      0xE1);      // Port 1 Match
SFR(P1MASK,     0xE2);      // Port 1 Mask
// No sfr16 definition for MAC0B because MAC0BL must be written last
   SFR(MAC0BL,    0xF1);    // MAC0 B Register Low Byte
   SFR(MAC0BH,    0xF2);    // MAC0 B Register High Byte
SFR(P0MAT,      0xF3);      // Port 0 Match
SFR(P0MASK,     0xF4);      // Port 0 Mask

/*  Page 0x0F */

SFR(CCH0CN,     0x84);      // Cache Control
SFR(CLKSEL,     0x8F);      // Clock Select
SFR(P0MDOUT,    0xA4);      // Port 0 Output Mode Configuration
SFR(P1MDOUT,    0xA5);      // Port 1 Output Mode Configuration
SFR(P2MDOUT,    0xA6);      // Port 2 Output Mode Configuration
SFR(PLL0DIV,    0xA9);      // PLL Divider
SFR(FLSTAT,     0xAC);      // Flash Status
SFR(OSCLCN,     0xAD);      // Internal Low-Frequency Oscillator Control
SFR(P4MDOUT,    0xAE);      // Port 4 Output Mode Configuration
SFR(P3MDOUT,    0xAF);      // Port 3 Output Mode Configuration
SFR(PLL0MUL,    0xB1);      // PLL Multiplier
SFR(PLL0FLT,    0xB2);      // PLL Filter
SFR(PLL0CN,     0xB3);      // PLL Control
SFR(OSCXCN,     0xB6);      // External Oscillator Control
SFR(OSCICN,     0xB7);      // Internal Oscillator Control
SFR(OSCICL,     0xBF);      // Internal Oscillator Calibration
SFR(EMI0CF,     0xC7);      // EMIF Configuration
SFR(CCH0TN,     0xC9);      // Cache Tuning
SFR(EIP1,       0xCE);      // Extended Interrupt Priority 1
SFR(EIP2,       0xCF);      // Extended Interrupt Priority 2
SFR(CCH0LC,     0xD2);      // Cache Lock
SFR(CCH0MA,     0xD3);      // Cache Miss Accumulator
SFR(P0SKIP,     0xD4);      // Port 0 Skip
SFR(P1SKIP,     0xD5);      // Port 1 Skip
SFR(P2SKIP,     0xD6);      // Port 2 Skip
SFR(P3SKIP,     0xD7);      // Port 3 Skip
SFR(XBR0,       0xE1);      // Port I/O Crossbar Control 0
SFR(XBR1,       0xE2);      // Port I/O Crossbar Control 1
SFR(SFR0CN,     0xE5);      // SFR Page Control
SFR(P0MDIN,     0xF1);      // Port 0 Input Mode Configuration
SFR(P1MDIN,     0xF2);      // Port 1 Input Mode Configuration
SFR(P2MDIN,     0xF3);      // Port 2 Input Mode Configuration
SFR(P3MDIN,     0xF4);      // Port 3 Input Mode Configuration
SFR(EMI0TC,     0xF7);      // EMIF Timing Control

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
#define INT_SMBUS0        7       // SMBus0 Interface
//                        8          Reserved
#define INT_ADC0_WINDOW   9       // ADC0 Window Comparison
#define INT_ADC0_EOC      10      // ADC0 End Of Conversion
#define INT_PCA0          11      // PCA0 Peripheral
#define INT_COMPARATOR0   12      // Comparator0
#define INT_COMPARATOR1   13      // Comparator1
#define INT_TIMER3        14      // Timer3 Overflow
//                        15         Reserved
#define INT_PORT_MATCH    16      // Port Match

#endif
