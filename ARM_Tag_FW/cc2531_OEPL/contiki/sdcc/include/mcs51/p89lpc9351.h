/*-------------------------------------------------------------------------
   p89lpc9351.h - Register Declarations for NXP  the P89LPC9351/P89LPC9361
   (Based on user manual (UM10308_3) Rev. 03 — 17 June 2009)

   Copyright (C) 2009, Jesus Calvino-Fraga / jesusc at ece.ubc.ca

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

#ifndef REG_P89LPC9351_H
#define REG_P89LPC9351_H

#include <compiler.h>

SFR(ACC,     0xe0); // Accumulator
    SBIT(ACC_7,   0xe0, 7);
    SBIT(ACC_6,   0xe0, 6);
    SBIT(ACC_5,   0xe0, 5);
    SBIT(ACC_4,   0xe0, 4);
    SBIT(ACC_3,   0xe0, 3);
    SBIT(ACC_2,   0xe0, 2);
    SBIT(ACC_1,   0xe0, 1);
    SBIT(ACC_0,   0xe0, 0);
SFR(ADCON0,  0x8e); // A/D control register 0
    #define ENBI0    0x80
    #define ENADCI0  0x40
    #define TMM0     0x20
    #define EDGE0    0x10
    #define ADCI0    0x08
    #define ENADC0   0x04
    #define ADCS01   0x02
    #define ADCS00   0x01
SFR(ADCON1,  0x97); // A/D control register 1
    #define ENBI1    0x80
    #define ENADCI1  0x40
    #define TMM1     0x20
    #define EDGE1    0x10
    #define ADCI1    0x08
    #define ENADC1   0x04
    #define ADCS11   0x02
    #define ADCS10   0x01
SFR(ADINS,   0xa3); // A/D input select
    #define ADI13    0x80
    #define ADI12    0x40
    #define ADI11    0x20
    #define ADI10    0x10
    #define ADI03    0x08
    #define ADI02    0x04
    #define ADI01    0x02
    #define ADI00    0x01
SFR(ADMODA,  0xc0); // A/D mode register A
    SBIT(ADMODA_7, 0xc0, 7);
    SBIT(ADMODA_6, 0xc0, 6);
    SBIT(ADMODA_5, 0xc0, 5);
    SBIT(ADMODA_4, 0xc0, 4);
    SBIT(ADMODA_3, 0xc0, 3);
    SBIT(ADMODA_2, 0xc0, 2);
    SBIT(ADMODA_1, 0xc0, 1);
    SBIT(ADMODA_0, 0xc0, 0);
    SBIT(BNDI1,   0xc0, 7);
    SBIT(BURST1,  0xc0, 6);
    SBIT(SCC1,    0xc0, 5);
    SBIT(SCAN1,   0xc0, 4);
    SBIT(BNDI0,   0xc0, 3);
    SBIT(BURST0,  0xc0, 2);
    SBIT(SCC0,    0xc0, 1);
    SBIT(SCAN0,   0xc0, 0);
SFR(ADMODB,  0xa1); // A/D mode register B
    #define CLK2     0x80
    #define CLK1     0x40
    #define CLK0     0x20
    #define INBND0   0x10
    #define ENDAC1   0x08
    #define ENDAC0   0x04
    #define BSA1     0x02
    #define BSA0     0x01
SFR(AD0BH,   0xbb); // A/D_0 boundary high register
SFR(AD0BL,   0xa6); // A/D_0 boundary low register
SFR(AD0DAT0, 0xc5); // A/D_0 data register 0
SFR(AD0DAT1, 0xc6); // A/D_0 data register 1
SFR(AD0DAT2, 0xc7); // A/D_0 data register 2
SFR(AD0DAT3, 0xf4); // A/D_0 data register 3
SFR(AD1BH,   0xc4); // A/D_1 boundary high register
SFR(AD1BL,   0xbc); // A/D_1 boundary low register
SFR(AD1DAT0, 0xd5); // A/D_1 data register 0
SFR(AD1DAT1, 0xd6); // A/D_1 data register 1
SFR(AD1DAT2, 0xd7); // A/D_1 data register 2
SFR(AD1DAT3, 0xf5); // A/D_1 data register 3
SFR(AUXR1,   0xa2); // Auxiliary function register
    #define CLKLP    0x80
    #define EBRR     0x40
    #define ENT1     0x20
    #define ENT0     0x10
    #define SRST     0x08
    #define DPS      0x01
SFR(B,       0xf0); // B register
    SBIT(B_7,     0xf0, 7);
    SBIT(B_6,     0xf0, 6);
    SBIT(B_5,     0xf0, 5);
    SBIT(B_4,     0xf0, 4);
    SBIT(B_3,     0xf0, 3);
    SBIT(B_2,     0xf0, 2);
    SBIT(B_1,     0xf0, 1);
    SBIT(B_0,     0xf0, 0);
SFR(BRGR0,   0xbe); // Baud rate generator 0 rate low
SFR(BRGR1,   0xbf); // Baud rate generator 0 rate high
SFR(BRGCON,  0xbd); // Baud rate generator 0 control
    #define SBRGS    0x02
    #define BRGEN    0x01
SFR(CCCRA,   0xea); // Capture compare A control register
    #define ICECA2   0x80
    #define ICECA1   0x40
    #define ICECA0   0x20
    #define ICESA    0x10
    #define ICNFA    0x08
    #define FCOA     0x04
    #define OCMA1    0x02
    #define OCMA0    0x01
SFR(CCCRB,   0xeb); // Capture compare B control register
    #define ICECB2   0x80
    #define ICECB1   0x40
    #define ICECB0   0x20
    #define ICESB    0x10
    #define ICNFB    0x08
    #define FCOB     0x04
    #define OCMB1    0x02
    #define OCMB0    0x01
SFR(CCCRC,   0xec); // Capture compare C control register
    #define FCOC     0x04
    #define OCMC1    0x02
    #define OCMC0    0x01
SFR(CCCRD,   0xed); // Capture compare D control register
    #define FCOD     0x04
    #define OCMD1    0x02
    #define OCMD0    0x01
SFR(CMP1,    0xac); // Comparator 1 control register
    #define CE1      0x20
    #define CP1      0x10
    #define CN1      0x08
    #define OE1      0x04
    #define CO1      0x02
    #define CMF1     0x01
SFR(CMP2,    0xad); // Comparator 2 control register
    #define CE2      0x20
    #define CP2      0x10
    #define CN2      0x08
    #define OE2      0x04
    #define CO2      0x02
    #define CMF2     0x01
SFR(DEECON,  0xf1); // Data EEPROM control register
    #define EEIF     0x80
    #define HVERR    0x40
    #define ECTL1    0x20
    #define ECTL0    0x10
    #define EWERR1   0x04
    #define EWERR0   0x02
    #define EADR8    0x01
SFR(DEEDAT,  0xf2); // Data EEPROM data register
SFR(DEEADR,  0xf3); // Data EEPROM address register
SFR(DIVM,    0x95); // CPU clock divide-by-M control
SFR(DPH,     0x83); // Data pointer high
SFR(DPL,     0x82); // Data pointer low
SFR(FMADRH,  0xe7); // Program flash address high
SFR(FMADRL,  0xe6); // Program flash address low
SFR(FMCON,   0xe4); // Program flash control Read
    #define BUSY     0x80
    #define HVA      0x08
    #define HVE      0x04
    #define SV       0x02
    #define OI       0x01
    #define FMCMD_7  0x80
    #define FMCMD_6  0x40
    #define FMCMD_5  0x20
    #define FMCMD_4  0x10
    #define FMCMD_3  0x08
    #define FMCMD_2  0x04
    #define FMCMD_1  0x02
    #define FMCMD_0  0x01
SFR(FMDATA,  0xe5); // Program flash data
SFR(I2ADR,   0xdb); // I2C-bus slave address register
    #define I2ADR_6  0x80
    #define I2ADR_5  0x40
    #define I2ADR_4  0x20
    #define I2ADR_3  0x10
    #define I2ADR_2  0x08
    #define I2ADR_1  0x04
    #define I2ADR_0  0x02
    #define GC       0x01
SFR(I2CON,   0xd8); // I2C-bus control register
    SBIT(I2CON_7, 0xd8, 7);
    SBIT(I2CON_6, 0xd8, 6);
    SBIT(I2CON_5, 0xd8, 5);
    SBIT(I2CON_4, 0xd8, 4);
    SBIT(I2CON_3, 0xd8, 3);
    SBIT(I2CON_2, 0xd8, 2);
    SBIT(I2CON_1, 0xd8, 1);
    SBIT(I2CON_0, 0xd8, 0);
    SBIT(I2EN,    0xd8, 6);
    SBIT(STA,     0xd8, 5);
    SBIT(STO,     0xd8, 4);
    SBIT(SI,      0xd8, 3);
    SBIT(AA,      0xd8, 2);
    SBIT(CRSEL,   0xd8, 0);
SFR(I2DAT,   0xda); // I2C-bus data register
SFR(I2SCLH,  0xdd); // Serial clock generator/SCL duty cycle register high
SFR(I2SCLL,  0xdc); // Serial clock generator/SCL duty cycle register low
SFR(I2STAT,  0xd9); // I2C-bus status register
    #define STA_4    0x80
    #define STA_3    0x40
    #define STA_2    0x20
    #define STA_1    0x10
    #define STA_0    0x08
SFR(ICRAH,   0xab); // Input capture A register high
SFR(ICRAL,   0xaa); // Input capture A register low
SFR(ICRBH,   0xaf); // Input capture B register high
SFR(ICRBL,   0xae); // Input capture B register low
SFR(IEN0,    0xa8); // Interrupt enable 0
    SBIT(IEN0_7,  0xa8, 7);
    SBIT(IEN0_6,  0xa8, 6);
    SBIT(IEN0_5,  0xa8, 5);
    SBIT(IEN0_4,  0xa8, 4);
    SBIT(IEN0_3,  0xa8, 3);
    SBIT(IEN0_2,  0xa8, 2);
    SBIT(IEN0_1,  0xa8, 1);
    SBIT(IEN0_0,  0xa8, 0);
    SBIT(EA,      0xa8, 7);
    SBIT(EWDRT,   0xa8, 6);
    SBIT(EBO,     0xa8, 5);
    SBIT(ES,      0xa8, 4);
    SBIT(ESR,     0xa8, 4);
    SBIT(ET1,     0xa8, 3);
    SBIT(EX1,     0xa8, 2);
    SBIT(ET0,     0xa8, 1);
    SBIT(EX0,     0xa8, 0);
SFR(IEN1,    0xe8); // Interrupt enable 1
    SBIT(IEN1_7,  0xe8, 7);
    SBIT(IEN1_6,  0xe8, 6);
    SBIT(IEN1_5,  0xe8, 5);
    SBIT(IEN1_4,  0xe8, 4);
    SBIT(IEN1_3,  0xe8, 3);
    SBIT(IEN1_2,  0xe8, 2);
    SBIT(IEN1_1,  0xe8, 1);
    SBIT(IEN1_0,  0xe8, 0);
    SBIT(EADEE,   0xe8, 7);
    SBIT(EST,     0xe8, 6);
    SBIT(ECCU,    0xe8, 4);
    SBIT(ESPI,    0xe8, 3);
    SBIT(EC,      0xe8, 2);
    SBIT(EKBI,    0xe8, 1);
    SBIT(EI2C,    0xe8, 0);
SFR(IP0,     0xb8); // Interrupt priority 0
    SBIT(IP0_7,   0xb8, 7);
    SBIT(IP0_6,   0xb8, 6);
    SBIT(IP0_5,   0xb8, 5);
    SBIT(IP0_4,   0xb8, 4);
    SBIT(IP0_3,   0xb8, 3);
    SBIT(IP0_2,   0xb8, 2);
    SBIT(IP0_1,   0xb8, 1);
    SBIT(IP0_0,   0xb8, 0);
    SBIT(PWDRT,   0xb8, 6);
    SBIT(PBO,     0xb8, 5);
    SBIT(PS,      0xb8, 4);
    SBIT(PSR,     0xb8, 4);
    SBIT(PT1,     0xb8, 3);
    SBIT(PX1,     0xb8, 2);
    SBIT(PT0,     0xb8, 1);
    SBIT(PX0,     0xb8, 0);
SFR(IP0H,    0xb7); // Interrupt priority 0 high
    #define PWDRTH   0x40
    #define PBOH     0x20
    #define PSH      0x10
    #define PSRH     0x10
    #define PT1H     0x08
    #define PX1H     0x04
    #define PT0H     0x02
    #define PX0H     0x01
SFR(IP1,     0xf8); // Interrupt priority 1
    SBIT(IP1_7,   0xf8, 7);
    SBIT(IP1_6,   0xf8, 6);
    SBIT(IP1_5,   0xf8, 5);
    SBIT(IP1_4,   0xf8, 4);
    SBIT(IP1_3,   0xf8, 3);
    SBIT(IP1_2,   0xf8, 2);
    SBIT(IP1_1,   0xf8, 1);
    SBIT(IP1_0,   0xf8, 0);
    SBIT(PADEE,   0xf8, 7);
    SBIT(PST,     0xf8, 6);
    SBIT(PCCU,    0xf8, 4);
    SBIT(PSPI,    0xf8, 3);
    SBIT(PC,      0xf8, 2);
    SBIT(PKBI,    0xf8, 1);
    SBIT(PI2C,    0xf8, 0);
SFR(IP1H,    0xf7); // Interrupt priority 1 high
    #define PAEEH    0x80
    #define PSTH     0x40
    #define PCCUH    0x10
    #define PSPIH    0x08
    #define PCH      0x04
    #define PKBIH    0x02
    #define PI2CH    0x01
SFR(KBCON,   0x94); // Keypad control register
    #define PATN     0x02
    #define _SEL     0x01
SFR(KBMASK,  0x86); // Keypad interrupt mask register
SFR(KBPATN,  0x93); // Keypad pattern register
SFR(OCRAH,   0xef); // Output compare A register high
SFR(OCRAL,   0xee); // Output compare A register low
SFR(OCRBH,   0xfb); // Output compare B register high
SFR(OCRBL,   0xfa); // Output compare B register low
SFR(OCRCH,   0xfd); // Output compare C register high
SFR(OCRCL,   0xfc); // Output compare C register low
SFR(OCRDH,   0xff); // Output compare D register high
SFR(OCRDL,   0xfe); // Output compare D register low
SFR(P0,      0x80); // Port 0
    SBIT(P0_7,    0x80, 7);
    SBIT(P0_6,    0x80, 6);
    SBIT(P0_5,    0x80, 5);
    SBIT(P0_4,    0x80, 4);
    SBIT(P0_3,    0x80, 3);
    SBIT(P0_2,    0x80, 2);
    SBIT(P0_1,    0x80, 1);
    SBIT(P0_0,    0x80, 0);
    SBIT(T1,      0x80, 7);
    SBIT(KB7,     0x80, 7);
    SBIT(CMP_1,   0x80, 6);
    SBIT(KB6,     0x80, 6);
    SBIT(CMPREF,  0x80, 5);
    SBIT(KB5,     0x80, 5);
    SBIT(CIN1A,   0x80, 4);
    SBIT(KB4,     0x80, 4);
    SBIT(CIN1B,   0x80, 3);
    SBIT(KB3,     0x80, 3);
    SBIT(CIN2A,   0x80, 2);
    SBIT(KB2,     0x80, 2);
    SBIT(CIN2B,   0x80, 1);
    SBIT(KB1,     0x80, 1);
    SBIT(CMP_2,   0x80, 0);
    SBIT(KB0,     0x80, 0);
SFR(P1,      0x90); // Port 1
    SBIT(P1_7,    0x90, 7);
    SBIT(P1_6,    0x90, 6);
    SBIT(P1_5,    0x90, 5);
    SBIT(P1_4,    0x90, 4);
    SBIT(P1_3,    0x90, 3);
    SBIT(P1_2,    0x90, 2);
    SBIT(P1_1,    0x90, 1);
    SBIT(P1_0,    0x90, 0);
    SBIT(OCC,     0x90, 7);
    SBIT(OCB,     0x90, 6);
    SBIT(RST,     0x90, 5);
    SBIT(INT1,    0x90, 4);
    SBIT(INT0,    0x90, 3);
    SBIT(SDA,     0x90, 3);
    SBIT(T0,      0x90, 2);
    SBIT(SCL,     0x90, 2);
    SBIT(RXD,     0x90, 1);
    SBIT(TXD,     0x90, 0);
SFR(P2,      0xa0); // Port 2
    SBIT(P2_7,    0xa0, 7);
    SBIT(P2_6,    0xa0, 6);
    SBIT(P2_5,    0xa0, 5);
    SBIT(P2_4,    0xa0, 4);
    SBIT(P2_3,    0xa0, 3);
    SBIT(P2_2,    0xa0, 2);
    SBIT(P2_1,    0xa0, 1);
    SBIT(P2_0,    0xa0, 0);
    SBIT(ICA,     0xa0, 7);
    SBIT(OCA,     0xa0, 6);
    SBIT(SPICLK,  0xa0, 5);
    SBIT(SS,      0xa0, 4);
    SBIT(MISO,    0xa0, 3);
    SBIT(MOSI,    0xa0, 2);
    SBIT(OCD,     0xa0, 1);
    SBIT(ICB,     0xa0, 0);
SFR(P3,      0xb0); // Port 3
    SBIT(P3_7,    0xb0, 7);
    SBIT(P3_6,    0xb0, 6);
    SBIT(P3_5,    0xb0, 5);
    SBIT(P3_4,    0xb0, 4);
    SBIT(P3_3,    0xb0, 3);
    SBIT(P3_2,    0xb0, 2);
    SBIT(P3_1,    0xb0, 1);
    SBIT(P3_0,    0xb0, 0);
    SBIT(XTAL1,   0xb0, 1);
    SBIT(XTAL2,   0xb0, 0);
SFR(P0M1,    0x84); // Port 0 output mode 1
    #define P0M1_7   0x80
    #define P0M1_6   0x40
    #define P0M1_5   0x20
    #define P0M1_4   0x10
    #define P0M1_3   0x08
    #define P0M1_2   0x04
    #define P0M1_1   0x02
    #define P0M1_0   0x01
SFR(P0M2,    0x85); // Port 0 output mode 2
    #define P0M2_7   0x80
    #define P0M2_6   0x40
    #define P0M2_5   0x20
    #define P0M2_4   0x10
    #define P0M2_3   0x08
    #define P0M2_2   0x04
    #define P0M2_1   0x02
    #define P0M2_0   0x01
SFR(P1M1,    0x91); // Port 1 output mode 1
    #define P1M1_7   0x80
    #define P1M1_6   0x40
    #define P1M1_4   0x10
    #define P1M1_3   0x08
    #define P1M1_2   0x04
    #define P1M1_1   0x02
    #define P1M1_0   0x01
SFR(P1M2,    0x92); // Port 1 output mode 2
    #define P1M2_7   0x80
    #define P1M2_6   0x40
    #define P1M2_4   0x10
    #define P1M2_3   0x08
    #define P1M2_2   0x04
    #define P1M2_1   0x02
    #define P1M2_0   0x01
SFR(P2M1,    0xa4); // Port 2 output mode 1
    #define P2M1_7   0x80
    #define P2M1_6   0x40
    #define P2M1_5   0x20
    #define P2M1_4   0x10
    #define P2M1_3   0x08
    #define P2M1_2   0x04
    #define P2M1_1   0x02
    #define P2M1_0   0x01
SFR(P2M2,    0xa5); // Port 2 output mode 2
    #define P2M2_7   0x80
    #define P2M2_6   0x40
    #define P2M2_5   0x20
    #define P2M2_4   0x10
    #define P2M2_3   0x08
    #define P2M2_2   0x04
    #define P2M2_1   0x02
    #define P2M2_0   0x01
SFR(P3M1,    0xb1); // Port 3 output mode 1
    #define P3M1_1   0x02
    #define P3M1_0   0x01
SFR(P3M2,    0xb2); // Port 3 output mode 2
    #define P3M2_1   0x02
    #define P3M2_0   0x01
SFR(PCON,    0x87); // Power control register
    #define SMOD1    0x80
    #define SMOD0    0x40
    #define BOI      0x10
    #define GF1      0x08
    #define GF0      0x04
    #define PMOD1    0x02
    #define PMOD0    0x01
SFR(PCONA,   0xb5); // Power control register A
    #define RTCPD    0x80
    #define DEEPD    0x40
    #define VCPD     0x20
    #define ADPD     0x10
    #define I2PD     0x08
    #define SPPD     0x04
    #define SPD      0x02
    #define CCUPD    0x01
SFR(PSW,     0xd0); // Program status word
    SBIT(PSW_7,   0xd0, 7);
    SBIT(PSW_6,   0xd0, 6);
    SBIT(PSW_5,   0xd0, 5);
    SBIT(PSW_4,   0xd0, 4);
    SBIT(PSW_3,   0xd0, 3);
    SBIT(PSW_2,   0xd0, 2);
    SBIT(PSW_1,   0xd0, 1);
    SBIT(PSW_0,   0xd0, 0);
    SBIT(CY,      0xd0, 7);
    SBIT(AC,      0xd0, 6);
    SBIT(F0,      0xd0, 5);
    SBIT(RS1,     0xd0, 4);
    SBIT(RS0,     0xd0, 3);
    SBIT(OV,      0xd0, 2);
    SBIT(F1,      0xd0, 1);
    SBIT(P,       0xd0, 0);
SFR(PT0AD,   0xf6); // Port 0 digital input disable
    #define PT0AD_5  0x20
    #define PT0AD_4  0x10
    #define PT0AD_3  0x08
    #define PT0AD_2  0x04
    #define PT0AD_1  0x02
SFR(RSTSRC,  0xdf); // Reset source register
    #define BOIF     0x40
    #define BOF      0x20
    #define POF      0x10
    #define R_BK     0x08
    #define R_WD     0x04
    #define R_SF     0x02
    #define R_EX     0x01
SFR(RTCCON,  0xd1); // RTC control
    #define RTCF     0x80
    #define RTCS1    0x40
    #define RTCS0    0x20
    #define ERTC     0x02
    #define RTCEN    0x01
SFR(RTCH,    0xd2); // RTC register high
SFR(RTCL,    0xd3); // RTC register low
SFR(SADDR,   0xa9); // Serial port address register
SFR(SADEN,   0xb9); // Serial port address enable
SFR(SBUF,    0x99); // Serial Port data buffer register
SFR(SCON,    0x98); // Serial port control
    SBIT(SCON_7,  0x98, 7);
    SBIT(SCON_6,  0x98, 6);
    SBIT(SCON_5,  0x98, 5);
    SBIT(SCON_4,  0x98, 4);
    SBIT(SCON_3,  0x98, 3);
    SBIT(SCON_2,  0x98, 2);
    SBIT(SCON_1,  0x98, 1);
    SBIT(SCON_0,  0x98, 0);
    SBIT(SM0,     0x98, 7);
    SBIT(FE,      0x98, 7);
    SBIT(SM1,     0x98, 6);
    SBIT(SM2,     0x98, 5);
    SBIT(REN,     0x98, 4);
    SBIT(TB8,     0x98, 3);
    SBIT(RB8,     0x98, 2);
    SBIT(TI,      0x98, 1);
    SBIT(RI,      0x98, 0);
SFR(SSTAT,   0xba); // Serial port extended status register
    #define DBMOD    0x80
    #define INTLO    0x40
    #define CIDIS    0x20
    #define DBISEL   0x10
    #define FE       0x08
    #define BR       0x04
    #define OE       0x02
    #define STINT    0x01
SFR(SP,      0x81); // Stack pointer
SFR(SPCTL,   0xe2); // SPI control register
    #define SSIG     0x80
    #define SPEN     0x40
    #define DORD     0x20
    #define MSTR     0x10
    #define CPOL     0x08
    #define CPHA     0x04
    #define SPR1     0x02
    #define SPR0     0x01
SFR(SPSTAT,  0xe1); // SPI status register
    #define SPIF     0x80
    #define WCOL     0x40
SFR(SPDAT,   0xe3); // SPI data register
SFR(TAMOD,   0x8f); // Timer 0 and 1 auxiliary mode
    #define T1M2     0x10
    #define T0M2     0x01
SFR(TCON,    0x88); // Timer 0 and 1 control
    SBIT(TCON_7,  0x88, 7);
    SBIT(TCON_6,  0x88, 6);
    SBIT(TCON_5,  0x88, 5);
    SBIT(TCON_4,  0x88, 4);
    SBIT(TCON_3,  0x88, 3);
    SBIT(TCON_2,  0x88, 2);
    SBIT(TCON_1,  0x88, 1);
    SBIT(TCON_0,  0x88, 0);
    SBIT(TF1,     0x88, 7);
    SBIT(TR1,     0x88, 6);
    SBIT(TF0,     0x88, 5);
    SBIT(TR0,     0x88, 4);
    SBIT(IE1,     0x88, 3);
    SBIT(IT1,     0x88, 2);
    SBIT(IE0,     0x88, 1);
    SBIT(IT0,     0x88, 0);
SFR(TCR20,   0xc8); // CCU control register 0
    SBIT(TCR20_7, 0xc8, 7);
    SBIT(TCR20_6, 0xc8, 6);
    SBIT(TCR20_5, 0xc8, 5);
    SBIT(TCR20_4, 0xc8, 4);
    SBIT(TCR20_3, 0xc8, 3);
    SBIT(TCR20_2, 0xc8, 2);
    SBIT(TCR20_1, 0xc8, 1);
    SBIT(TCR20_0, 0xc8, 0);
    SBIT(PLEEN,   0xc8, 7);
    SBIT(HLTRN,   0xc8, 6);
    SBIT(HLTEN,   0xc8, 5);
    SBIT(ALTCD,   0xc8, 4);
    SBIT(ALTAB,   0xc8, 3);
    SBIT(TDIR2,   0xc8, 2);
    SBIT(TMOD21,  0xc8, 1);
    SBIT(TMOD20,  0xc8, 0);
SFR(TCR21,   0xf9); // CCU control register 1
    #define TCOU2    0x80
    #define PLLDV_3  0x08
    #define PLLDV_2  0x04
    #define PLLDV_1  0x02
    #define PLLDV_0  0x01
SFR(TH0,     0x8c); // Timer 0 high
SFR(TH1,     0x8d); // Timer 1 high
SFR(TH2,     0xcd); // CCU timer high
SFR(TICR2,   0xc9); // CCU interrupt control register
    #define TOIE2    0x80
    #define TOCIE2D  0x40
    #define TOCIE2C  0x20
    #define TOCIE2B  0x10
    #define TOCIE2A  0x08
    #define TICIE2B  0x02
    #define TICIE2A  0x01
SFR(TIFR2,   0xe9); // CCU interrupt flag register
    #define TOIF2    0x80
    #define TOCF2D   0x40
    #define TOCF2C   0x20
    #define TOCF2B   0x10
    #define TOCF2A   0x08
    #define TICF2B   0x02
    #define TICF2A   0x01
SFR(TISE2,   0xde); // CCU interrupt status encode register
    #define ENCINT_2 0x04
    #define ENCINT_1 0x02
    #define ENCINT_0 0x01
SFR(TL0,     0x8a); // Timer 0 low
SFR(TL1,     0x8b); // Timer 1 low
SFR(TL2,     0xcc); // CCU timer low
SFR(TMOD,    0x89); // Timer 0 and 1 mode
    #define T1GATE   0x80
    #define T1C_T    0x40
    #define T1M1     0x20
    #define T1M0     0x10
    #define T0GATE   0x08
    #define T0C_T    0x04
    #define T0M1     0x02
    #define T0M0     0x01
SFR(TOR2H,   0xcf); // CCU reload register high
SFR(TOR2L,   0xce); // CCU reload register low
SFR(TPCR2H,  0xcb); // Prescaler control register high
    #define TPCR2H_1 0x02
    #define TPCR2H_0 0x01
SFR(TPCR2L,  0xca); // Prescaler control register low
    #define TPCR2L_7 0x80
    #define TPCR2L_6 0x40
    #define TPCR2L_5 0x20
    #define TPCR2L_4 0x10
    #define TPCR2L_3 0x08
    #define TPCR2L_2 0x04
    #define TPCR2L_1 0x02
    #define TPCR2L_0 0x01
SFR(TRIM,    0x96); // Internal oscillator trim register
    #define RCCLK    0x80
    #define ENCLK    0x40
    #define TRIM_5   0x20
    #define TRIM_4   0x10
    #define TRIM_3   0x08
    #define TRIM_2   0x04
    #define TRIM_1   0x02
    #define TRIM_0   0x01
SFR(WDCON,   0xa7); // Watchdog control register
    #define PRE2     0x80
    #define PRE1     0x40
    #define PRE0     0x20
    #define WDRUN    0x04
    #define WDTOF    0x02
    #define WDCLK    0x01
SFR(WDL,     0xc1); // Watchdog load
SFR(WFEED1,  0xc2); // Watchdog feed 1
SFR(WFEED2,  0xc3); // Watchdog feed 2
SFRX(BODCFG,  0xffc8); // BOD configuration register
    #define BOICFG1  0x02
    #define BOICFG0  0x01
SFRX(CLKCON,  0xffde); // CLOCK Control register
    #define CLKOK    0x80
    #define XTALWD   0x10
    #define CLKDBL   0x08
    #define FOSC2    0x04
    #define FOSC1    0x02
    #define FOSC0    0x01
SFRX(PGACON1, 0xffe1); // PGA1 control register
    #define ENPGA1   0x80
    #define PGASEL1_1 0x40
    #define PGASEL1_0 0x20
    #define PGATRIM_1 0x10
    #define PGAG11   0x02
    #define PGAG10   0x01
SFRX(PGACON1B, 0xffe4); // PGA1 control register B
    #define PGAENO   0x01
    #define FF1      0x01
SFRX(PGA1TRIM8X16X, 0xffe3); // PGA1 trim register
    #define PGA1_16XTRIM3 0x80
    #define PGA1_16XTRIM2 0x40
    #define PGA1_16XTRIM1 0x20
    #define PGA1_16XTRIM0 0x10
    #define PGA1_8XTRIM3 0x08
    #define PGA1_8XTRIM2 0x04
    #define PGA1_8XTRIM1 0x02
    #define PGA1_8XTRIM0 0x01
SFRX(PGA1TRIM2X4X, 0xffe2); // PGA1 trim register
    #define PGA1_4XTRIM3 0x80
    #define PGA1_4XTRIM2 0x40
    #define PGA1_4XTRIM1 0x20
    #define PGA1_4XTRIM0 0x10
    #define PGA1_2XTRIM3 0x08
    #define PGA1_2XTRIM2 0x04
    #define PGA1_2XTRIM1 0x02
    #define PGA1_2XTRIM0 0x01
SFRX(PGACON0, 0xffca); // PGA0 control register
    #define ENPGA0   0x80
    #define PGASEL0_1 0x40
    #define PGASEL0_0 0x20
    #define PGATRIM_0 0x10
    #define TSEL1    0x08
    #define TSEL0    0x04
    #define PGAG01   0x02
    #define PGAG00   0x01
SFRX(PGACON0B, 0xffce); // PGA0 control register B
    #define PGAENO   0x01
    #define FF0      0x01
SFRX(PGA0TRIM8X16X, 0xffcd); // PGA0 trim register
    #define PGA0_16XTRIM3 0x80
    #define PGA0_16XTRIM2 0x40
    #define PGA0_16XTRIM1 0x20
    #define PGA0_16XTRIM0 0x10
    #define PGA0_8XTRIM3 0x08
    #define PGA0_8XTRIM2 0x04
    #define PGA0_8XTRIM1 0x02
    #define PGA0_8XTRIM0 0x01
SFRX(PGA0TRIM2X4X, 0xffcc); // PGA0 trim register
    #define PGA0_4XTRIM3 0x80
    #define PGA0_4XTRIM2 0x40
    #define PGA0_4XTRIM1 0x20
    #define PGA0_4XTRIM0 0x10
    #define PGA0_2XTRIM3 0x08
    #define PGA0_2XTRIM2 0x04
    #define PGA0_2XTRIM1 0x02
    #define PGA0_2XTRIM0 0x01
SFRX(RTCDATH, 0xffbf); // Real-time clock data register high
SFRX(RTCDATL, 0xffbe); // Real-time clock data register low
#endif /*REG_P89LPC9351_H*/
