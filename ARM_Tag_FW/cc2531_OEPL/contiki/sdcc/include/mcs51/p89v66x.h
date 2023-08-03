/*-------------------------------------------------------------------------
   p89v66x.h - This header allows to use the microcontroler NXP
   (formerly Philips) p89v66x where x stands for 0,2,4.

   Copyright (C) 2008, Gudjon I. Gudjonsson <gudjon AT gudjon.org>

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

/*-------------------------------------------------------------------------
 The registered are ordered in the same way as in the NXP data sheet:
 http://www.standardics.nxp.com/products/80c51/datasheet/p89v660.p89v662.p89v664.pdf
-------------------------------------------------------------------------*/

#ifndef __P89V66X_H__
#define __P89V66X_H__
#include <compiler.h>

/*BYTE Registers*/
SFR(ACC, 0xE0); // Accumulator
	SBIT(ACC_7, 0xE0, 7);
	SBIT(ACC_6, 0xE0, 6);
	SBIT(ACC_5, 0xE0, 5);
	SBIT(ACC_4, 0xE0, 4);
	SBIT(ACC_3, 0xE0, 3);
	SBIT(ACC_2, 0xE0, 2);
	SBIT(ACC_1, 0xE0, 1);
	SBIT(ACC_0, 0xE0, 0);
SFR(AUXR, 0x8E); // Auxiliary
	#define  EXTRAM   0x02
	#define  AO       0x01
SFR(AUXR1, 0xA2); // Auxiliary 1
	#define  ENBOOT   0x20
	#define  GF2      0x08
	// Bit 2 must be zero
	#define  DPS      0x01
SFR(B, 0xF0); // B register
	SBIT(B7, 0xF0, 7);
	SBIT(B6, 0xF0, 6);
	SBIT(B5, 0xF0, 5);
	SBIT(B4, 0xF0, 4);
	SBIT(B3, 0xF0, 3);
	SBIT(B2, 0xF0, 2);
	SBIT(B1, 0xF0, 1);
	SBIT(B0, 0xF0, 0);
SFR(CCAP0H, 0xFA); // Module 0 Capture High
SFR(CCAP1H, 0xFB); // Module 1 Capture High
SFR(CCAP2H, 0xFC); // Module 2 Capture High
SFR(CCAP3H, 0xFD); // Module 3 Capture High
SFR(CCAP4H, 0xFE); // Module 4 Capture High
SFR(CCAP0L, 0xEA); // Module 0 Capture Low
SFR(CCAP1L, 0xEB); // Module 1 Capture Low
SFR(CCAP2L, 0xEC); // Module 2 Capture Low
SFR(CCAP3L, 0xED); // Module 3 Capture Low
SFR(CCAP4L, 0xEE); // Module 4 Capture Low
SFR(CCAPM0, 0xC2); // Module 0 Mode
	#define ECOM_0   0x40
	#define CAPP_0   0x20
	#define CAPN_0   0x10
	#define  MAT_0   0x08
	#define  TOG_0   0x04
	#define  PWM_0   0x02
	#define ECCF_0   0x01
SFR(CCAPM1, 0xC3); // Module 1 Mode
	#define ECOM_1   0x40
	#define CAPP_1   0x20
	#define CAPN_1   0x10
	#define  MAT_1   0x08
	#define  TOG_1   0x04
	#define  PWM_1   0x02
	#define ECCF_1   0x01
SFR(CCAPM2, 0xC4); // Module 2 Mode
	#define ECOM_2   0x40
	#define CAPP_2   0x20
	#define CAPN_2   0x10
	#define  MAT_2   0x08
	#define  TOG_2   0x04
	#define  PWM_2   0x02
	#define ECCF_2   0x01
SFR(CCAPM3, 0xC5); // Module 3 Mode
	#define ECOM_3   0x40
	#define CAPP_3   0x20
	#define CAPN_3   0x10
	#define  MAT_3   0x08
	#define  TOG_3   0x04
	#define  PWM_3   0x02
	#define ECCF_3   0x01
SFR(CCAPM4, 0xC6); // Module 4 Mode
	#define ECOM_4   0x40
	#define CAPP_4   0x20
	#define CAPN_4   0x10
	#define  MAT_4   0x08
	#define  TOG_4   0x04
	#define  PWM_4   0x02
	#define ECCF_4   0x01
	#define ECOM     0x40
	#define CAPP     0x20
	#define CAPN     0x10
	#define MAT      0x08
	#define TOG      0x04
	#define PWM      0x02
	#define ECCF     0x01
SFR(CCON,   0xC0); // PCA Counter Control
	SBIT(CF,   0xC0, 7);
	SBIT(CR,   0xC0, 6);
	SBIT(CCF4, 0xC0, 4);
	SBIT(CCF3, 0xC0, 3);
	SBIT(CCF2, 0xC0, 2);
	SBIT(CCF1, 0xC0, 1);
	SBIT(CCF0, 0xC0, 0);
SFR(CH,   0xF9); // PCA Counter High
SFR(CL,   0xE9); // PCA Counter Low
SFR(CMOD, 0xC1); // PCA Counter Mode
	#define CIDL    0x80
	#define WDTE    0x40
	#define CPS1    0x04
	#define CPS0    0x02
	#define  ECF    0x01
SFR(DPH,  0x83); // Data Pointer High
SFR(DPL,  0x82); // Data Pointer Low
SFR(IEN0, 0xA8); // Interrupt Enable 0
	SBIT(EA,  0xA8, 7);
	SBIT(EC,  0xA8, 6);
	SBIT(ES1, 0xA8, 5);
	SBIT(ES0, 0xA8, 4);
	SBIT(ET1, 0xA8, 3);
	SBIT(EX1, 0xA8, 2);
	SBIT(ET0, 0xA8, 1);
	SBIT(EX0, 0xA8, 0);
SFR(IEN1, 0xE8); // Interrupt Enable 1
	SBIT(ES3, 0xE8, 2);
	SBIT(ES2, 0xE8, 1);
	SBIT(ET2, 0xE8, 0);
SFR(IP0, 0xB8); // Interrupt Priority 0
	SBIT(PT2, 0xB8, 7);
	SBIT(PPC, 0xB8, 6);
	SBIT(PS1, 0xB8, 5);
	SBIT(PS0, 0xB8, 4);
	SBIT(PT1, 0xB8, 3);
	SBIT(PX1, 0xB8, 2);
	SBIT(PT0, 0xB8, 1);
	SBIT(PX0, 0xB8, 0);
SFR(IP0H, 0xB7); // Interrupt Priority 0 High
	#define PT2H  0x80
	#define PPCH  0x40
	#define PS1H  0x20
	#define PS0H  0x10
	#define PT1H  0x08
	#define PX1H  0x04
	#define PT0H  0x02
	#define PX0H  0x01
SFR(IP1, 0x91); // Interrupt Priority 1
	#define PS3  0x02
	#define PS2  0x01
SFR(IP1H, 0x92); // Interrupt Priority 1 High
	#define PS3H  0x02
	#define PS2H  0x01
SFR(P0, 0x80); // Port 0
	SBIT(AD7,  0x80, 7);
	SBIT(P0_7, 0x80, 7);
	SBIT(AD6,  0x80, 6);
	SBIT(P0_6, 0x80, 6);
	SBIT(AD5,  0x80, 5);
	SBIT(P0_5, 0x80, 5);
	SBIT(AD4,  0x80, 4);
	SBIT(P0_4, 0x80, 4);
	SBIT(AD3,  0x80, 3);
	SBIT(P0_3, 0x80, 3);
	SBIT(AD2,  0x80, 2);
	SBIT(P0_2, 0x80, 2);
	SBIT(AD1,  0x80, 1);
	SBIT(P0_1, 0x80, 1);
	SBIT(AD0,  0x80, 0);
	SBIT(P0_0, 0x80, 0);
SFR(P1, 0x90); // Port 1
	SBIT(T1_CEX4, 0x90, 7);
	SBIT(P1_7,    0x90, 7);
	SBIT(T0_CEX3, 0x90, 6);
	SBIT(P1_6,    0x90, 6);
	SBIT(CEX2,    0x90, 5);
	SBIT(P1_5,    0x90, 5);
	SBIT(CEX1,    0x90, 4);
	SBIT(P1_4,    0x90, 4);
	SBIT(CEX0,    0x90, 3);
	SBIT(P1_3,    0x90, 3);
	SBIT(ECI,     0x90, 2);
	SBIT(P1_2,    0x90, 2);
	SBIT(T2EX,    0x90, 1);
	SBIT(P1_1,    0x90, 1);
	SBIT(T2,      0x90, 0);
	SBIT(P1_0,    0x90, 0);
SFR(P2, 0xA0); // Port 2
	SBIT(AD15, 0xA0, 7);
	SBIT(P2_7, 0xA0, 7);
	SBIT(AD14, 0xA0, 6);
	SBIT(P2_6, 0xA0, 6);
	SBIT(AD13, 0xA0, 5);
	SBIT(P2_5, 0xA0, 5);
	SBIT(AD12, 0xA0, 4);
	SBIT(P2_4, 0xA0, 4);
	SBIT(AD11, 0xA0, 3);
	SBIT(P2_3, 0xA0, 3);
	SBIT(AD10, 0xA0, 2);
	SBIT(P2_2, 0xA0, 2);
	SBIT(AD9,  0xA0, 1);
	SBIT(P2_1, 0xA0, 1);
	SBIT(AD8,  0xA0, 0);
	SBIT(P2_0, 0xA0, 0);
SFR(P3, 0xB0); // Port 3
	SBIT(RD,   0xB0, 7);
	SBIT(P3_7, 0xB0, 7);
	SBIT(WR,   0xB0, 6);
	SBIT(P3_6, 0xB0, 6);
	SBIT(T1,   0xB0, 5);
	SBIT(P3_5, 0xB0, 5);
	SBIT(T0,   0xB0, 4);
	SBIT(P3_4, 0xB0, 4);
	SBIT(INT1, 0xB0, 3);
	SBIT(P3_3, 0xB0, 3);
	SBIT(INT0, 0xB0, 2);
	SBIT(P3_2, 0xB0, 2);
	SBIT(TXD,  0xB0, 1);
	SBIT(P3_1, 0xB0, 1);
	SBIT(RXD,  0xB0, 0);
	SBIT(P3_0, 0xB0, 0);
SFR(P4, 0xA1); // Port 3
	#define SS    0x08
	#define P4_3  0x08
	#define MOSI  0x04
	#define P4_2  0x04
	#define MISO  0x02
	#define SDA_1 0x02
	#define P4_1  0x02
	#define SCK   0x01
	#define SCL_1 0x01
	#define P4_0  0x01
SFR(PCON, 0x87); // Power Control
	#define SMOD1  0x80
	#define SMOD0  0x40
	#define POF    0x10
	#define GF1    0x08
	#define GF0    0x04
	#define PD     0x02
	#define IDL    0x01
SFR(PSW, 0xD0); // Program Status Word
	SBIT(CY,  0xD0, 7);
	SBIT(AC,  0xD0, 6);
	SBIT(F0,  0xD0, 5);
	SBIT(RS1, 0xD0, 4);
	SBIT(RS0, 0xD0, 3);
	SBIT(OV,  0xD0, 2);
	SBIT(F1,  0xD0, 1);
	SBIT(P,   0xD0, 0);
SFR(RCAP2H, 0xCB); // Timer 2 Capture High
SFR(RCAP2L, 0xCA); // Timer 2 Capture Low
SFR(S0CON,  0x98); // Serial Control 
	SBIT(SM0_FE,0x98, 7);
	SBIT(SM1,   0x98, 6);
	SBIT(SM2,   0x98, 5);
	SBIT(REN,   0x98, 4);
	SBIT(TB8,   0x98, 3);
	SBIT(RB8,   0x98, 2);
	SBIT(TI,    0x98, 1);
	SBIT(RI,    0x98, 0);
SFR(S0BUF, 0x99); // Serial Data Buffer
SFR(SADDR, 0xA9); // I2C Slave Address
SFR(SADEN, 0xB9); // I2C Slave Address Mask
SFR(SPCR,  0xD5); // SPI Control Register
	#define SPIE  0x80
	#define SPEN  0x40
	#define DORD  0x20
	#define MSTR  0x10
	#define CPOL  0x08
	#define CPHA  0x04
	#define SPR1  0x02
	#define SPR0  0x01
/*	SBIT(SPIE,   0x80, 7);   // SPCR is not bit addressable, not yet at least, according to the manual.
	SBIT(SPEN,   0x80, 6);
	SBIT(DORD,   0x80, 5);
	SBIT(MSTR,   0x80, 4);
	SBIT(CPOL,   0x80, 3);
	SBIT(CPHA,   0x80, 2);
	SBIT(SPR1,   0x80, 1);
	SBIT(SPR0,   0x80, 0);*/
SFR(SPSR,  0xAA); // SPI Configuration Register
	#define SPIF  0x80
	#define WCOL  0x40
SFR(SPDAT, 0x86); // SPI Data
SFR(SP,    0x81); // Stack Pointer
SFR(S1DAT, 0xDA); // I2C Serial 1 Data
SFR(S1ADR, 0xDB); // I2C Serial 1 Address 
	#define S1ADR_6  0x80
	#define S1ADR_5  0x40
	#define S1ADR_4  0x20
	#define S1ADR_3  0x10
	#define S1ADR_2  0x08
	#define S1ADR_1  0x04
	#define S1ADR_0  0x02
	#define S1GC     0x01
SFR(S1STA, 0xD9); // I2C Serial 1 Status 
	#define SC4  0x80
	#define SC3  0x40
	#define SC2  0x20
	#define SC1  0x10
	#define SC0  0x08    // Only write 0 to the lowest three bits
SFR(S1CON, 0xD8); // I2C Serial 1 Control
	SBIT(CR2,  0xD8, 7);
	SBIT(ENS1, 0xD8, 6);
	SBIT(STA,  0xD8, 5);
	SBIT(STO,  0xD8, 4);
	SBIT(SI,   0xD8, 3);
	SBIT(AA,   0xD8, 2);
	SBIT(CR1,  0xD8, 1);
	SBIT(CR0,  0xD8, 0);
SFR(S2DAT, 0xE2); // I2C Serial 1 Data
SFR(S2ADR, 0xE3); // I2C Serial 1 Address 
	#define S2ADR_6  0x80
	#define S2ADR_5  0x40
	#define S2ADR_4  0x20
	#define S2ADR_3  0x10
	#define S2ADR_2  0x08
	#define S2ADR_1  0x04
	#define S2ADR_0  0x02
	#define S2GC     0x01
SFR(S2STA, 0xE1); // I2C Serial 1 Status 
	#define SC24  0x80
	#define SC23  0x40
	#define SC22  0x20
	#define SC21  0x10
	#define SC20  0x08  // Only write 0 to the lowest three bits
SFR(S2CON, 0xF8); // I2C Serial 1 Control
	SBIT(CR22,  0xF8, 7);
	SBIT(ENS21, 0xF8, 6);
	SBIT(STA2,  0xF8, 5);
	SBIT(STO2,  0xF8, 4);
	SBIT(SI2,   0xF8, 3);
	SBIT(AA2,   0xF8, 2);
	SBIT(CR21,  0xF8, 1);
	SBIT(CR20,  0xF8, 0);
SFR(TCON, 0x88); // Timer Control
	SBIT(TF1, 0x88, 7);
	SBIT(TR1, 0x88, 6);
	SBIT(TF0, 0x88, 5);
	SBIT(TR0, 0x88, 4);
	SBIT(IE1, 0x88, 3);
	SBIT(IT1, 0x88, 2);
	SBIT(IE0, 0x88, 1);
	SBIT(IT0, 0x88, 0);
SFR(T2CON, 0xC8); // Timer 2 Control
	SBIT(TF2,    0xC8, 7);
	SBIT(EXF2,   0xC8, 6);
	SBIT(RCLK,   0xC8, 5);
	SBIT(TCLK,   0xC8, 4);
	SBIT(EXEN2,  0xC8, 3);
	SBIT(TR2,    0xC8, 2);
	SBIT(C_T2,   0xC8, 1);
	SBIT(CP_RL2, 0xC8, 0);
SFR(T2MOD,  0xC9); // Timer 2 Mode Control
	#define ENT2  0x20
	#define T2OE  0x02
	#define DCEN  0x01
SFR(TH0,  0x8C); // Timer High 0
SFR(TH1,  0x8D); // Timer High 1
SFR(TH2,  0xCD); // Timer High 2
SFR(TL0,  0x8A); // Timer Low 0
SFR(TL1,  0x8B); // Timer Low 1
SFR(TL2,  0xCC); // Timer Low 2
SFR(TMOD, 0x89); // Timer Mode
	#define GATE_1  0x80
	#define  C_T_1  0x40
	#define   M1_1  0x20
	#define   M0_1  0x10
	#define GATE_0  0x08
	#define  C_T_0  0x04
	#define   M1_0  0x02
	#define   M0_0  0x01
SFR(WDTRST, 0xA6); // Watchdog Timer Reset

#endif  //__P89V66X_H__
