/*-------------------------------------------------------------------------
   p89lpc935_6.h - This header allows to use the microcontrolers NXP
   (formerly Philips) p89lpc935, 936.

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
 http://www.standardics.nxp.com/products/lpc900/datasheet/p89lpc933.p89lpc934.p89lpc935.p89lpc936.pdf
-------------------------------------------------------------------------*/

#ifndef __P89LPC935_6_H__
#define __P89LPC935_6_H__
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
SFR(ADCON0, 0x8E); // A/D control register 0
	#define  ENBI0    0x80
	#define  ENADCI0  0x40
	#define  TMM0     0x20
	#define  EDGE0    0x10
	#define  ADCI0    0x08
	#define  ENADC0   0x04
	#define  ADCS01   0x02
	#define  ADCS00   0x01
SFR(ADCON1, 0x97); // A/D control register 1
	#define  ENBI1    0x80
	#define  ENADCI1  0x40
	#define  TMM1     0x20
	#define  EDGE1    0x10
	#define  ADCI1    0x08
	#define  ENADC1   0x04
	#define  ADCS11   0x02
	#define  ADCS10   0x01
SFR(ADINS, 0xA3); // A/D input select
	#define  ADI13    0x80
	#define  ADI12    0x40
	#define  ADI11    0x20
	#define  ADI10    0x10
	#define  ADI03    0x08
	#define  ADI02    0x04
	#define  ADI01    0x02
	#define  ADI00    0x01
SFR(ADMODA, 0xC0); // A/D mode register A
	SBIT(BNDI1,  0xC0, 7);
	SBIT(BURST1, 0xC0, 6);
	SBIT(SCC1,   0xC0, 5);
	SBIT(SCAN1,  0xC0, 4);
	SBIT(BNDI0,  0xC0, 3);
	SBIT(BURST0, 0xC0, 2);
	SBIT(SCC0,   0xC0, 1);
	SBIT(SCAN0,  0xC0, 0);
SFR(ADMODB, 0xA1); // A/D mode register B
	#define  CLK2     0x80
	#define  CLK1     0x40
	#define  CLK0     0x20
	#define  ENDAC1   0x08
	#define  ENDAC0   0x04
	#define  BSA1     0x02
	#define  BSA0     0x01
SFR(AD0BH,   0xBB); // A/D_0 boundary high register
SFR(AD0BL,   0xA6); // A/D_0 boundary low register
SFR(AD0DAT0, 0xC5); // A/D_0 data register 0
SFR(AD0DAT1, 0xC6); // A/D_0 data register 1
SFR(AD0DAT2, 0xC7); // A/D_0 data register 2
SFR(AD0DAT3, 0xF4); // A/D_0 data register 3
SFR(AD1BH,   0xC4); // A/D_1 boundary high register
SFR(AD1BL,   0xBC); // A/D_1 boundary low register
SFR(AD1DAT0, 0xD5); // A/D_1 data register 0
SFR(AD1DAT1, 0xD6); // A/D_1 data register 1
SFR(AD1DAT2, 0xD7); // A/D_1 data register 2
SFR(AD1DAT3, 0xF5); // A/D_1 data register 3
SFR(AUXR1,   0xA2); // Auxilary function register
	#define  CLKLP    0x80
	#define  EBRR     0x40
	#define  ENT1     0x20
	#define  ENT0     0x10
	#define  SRST     0x08
	#define  DPS      0x01 // Bit 2 is always 0
SFR(B, 0xF0); // B register
	SBIT(B7, 0xF0, 7);
	SBIT(B6, 0xF0, 6);
	SBIT(B5, 0xF0, 5);
	SBIT(B4, 0xF0, 4);
	SBIT(B3, 0xF0, 3);
	SBIT(B2, 0xF0, 2);
	SBIT(B1, 0xF0, 1);
	SBIT(B0, 0xF0, 0);
SFR(BRGR0,  0xBE); // Baud rate generator rate low
SFR(BRGR1,  0xBF); // Baud rate generator rate high
SFR(BRGCON, 0xBD); // Baud rate generator control
	#define  SBRGS    0x02
	#define  BRGEN    0x01
SFR(CCCRA,  0xEA); // Capture compare A control register
	#define  ICECA2   0x80
	#define  ICECA1   0x40
	#define  ICECA0   0x20
	#define  ICESA    0x10
	#define  ICNFA    0x08
	#define  FCOA     0x04
	#define  OCMA1    0x02
	#define  OCMA0    0x01
SFR(CCCRB,  0xEB); // Capture compare B control register
	#define  ICECB2   0x80
	#define  ICECB1   0x40
	#define  ICECB0   0x20
	#define  ICESB    0x10
	#define  ICNFB    0x08
	#define  FCOB     0x04
	#define  OCMB1    0x02
	#define  OCMB0    0x01
SFR(CCCRC,  0xEC); // Capture compare C control register
	#define  FCOC     0x04
	#define  OCMC1    0x02
	#define  OCMC0    0x01
SFR(CCCRD,  0xED); // Capture compare D control register
	#define  FCOD     0x04
	#define  OCMD1    0x02
	#define  OCMD0    0x01
SFR(CMP1,   0xAC); // Comparator 1 control register
	#define  CE1      0x20
	#define  CP1      0x10
	#define  CN1      0x08
	#define  OE1      0x04
	#define  CO1      0x02
	#define  CMF1     0x01
SFR(CMP2,   0xAD); // Comparator 2 control register
	#define  CE2      0x20
	#define  CP2      0x10
	#define  CN2      0x08
	#define  OE2      0x04
	#define  CO2      0x02
	#define  CMF2     0x01
SFR(DEECON, 0xF1); // Data EEPROM control register
	#define  EEIF     0x80
	#define  HVERR    0x40
	#define  ECTL1    0x20
	#define  ECTL0    0x10
	#define  EADR8    0x01
SFR(DEEDAT, 0xF2); // Data EEPROM data register
SFR(DEEADR, 0xF3); // Data EEPROM address register
SFR(DIVM,   0x95); // CPU clock divide-by-M control
SFR(DPH,    0x83); // Data Pointer High
SFR(DPL,    0x82); // Data Pointer Low
SFR(FMADRH, 0xE7); // Program flash address high
SFR(FMADRL, 0xE6); // Program flash address low
SFR(FMCON,  0xE4);
	// Program flash control (Read)
	#define  BUSY     0x80
	#define  HVA      0x08
	#define  HVE      0x04
	#define  SV       0x02
	#define  OI       0x01
	// Program flash control (Write)
	#define  FMCMD_7  0x80
	#define  FMCMD_6  0x40
	#define  FMCMD_5  0x20
	#define  FMCMD_4  0x10
	#define  FMCMD_3  0x08
	#define  FMCMD_2  0x04
	#define  FMCMD_1  0x02
	#define  FMCMD_0  0x01
SFR(FMDATA,  0xE5); // Program flash data
SFR(I2ADR,   0xDB); // I2C slave address register
	#define I2ADR_6  0x80
	#define I2ADR_5  0x40
	#define I2ADR_4  0x20
	#define I2ADR_3  0x10
	#define I2ADR_2  0x08
	#define I2ADR_1  0x04
	#define I2ADR_0  0x02
	#define GC       0x01
SFR(I2CON,  0xD8); // I2C control register
	SBIT(I2EN,  0xD8, 6);
	SBIT(STA,   0xD8, 5);
	SBIT(STO,   0xD8, 4);
	SBIT(SI,    0xD8, 3);
	SBIT(AA,    0xD8, 2);
	SBIT(CRSEL, 0xD8, 0);
SFR(I2DAT,  0xDA); // I2C data register
SFR(I2SCLH, 0xDD); // I2C serial clock generator/SCL duty cycle register high
SFR(I2SCLL, 0xDC); // I2C serial clock generator/SCL duty cycle register low
SFR(I2STAT, 0xD9); // I2C status register
	#define STA_4   0x80
	#define STA_3   0x40
	#define STA_2   0x20
	#define STA_1   0x10
	#define STA_0   0x08    // Only write 0 to the lowest three bits
SFR(ICRAH,  0xAB); // Input capture A register high
SFR(ICRAL,  0xAA); // Input capture A register low
SFR(ICRBH,  0xAF); // Input capture B register high
SFR(ICRBL,  0xAE); // Input capture B register low
SFR(IEN0,   0xA8); // Interrupt Enable 0
	SBIT(EA,     0xA8, 7);
	SBIT(EWDRT,  0xA8, 6);
	SBIT(EBO,    0xA8, 5);
	SBIT(ES_ESR, 0xA8, 4);
	SBIT(ET1,    0xA8, 3);
	SBIT(EX1,    0xA8, 2);
	SBIT(ET0,    0xA8, 1);
	SBIT(EX0,    0xA8, 0);
SFR(IEN1,  0xE8); // Interrupt Enable 1
	SBIT(EADEE,  0xE8, 7);
	SBIT(EST,    0xE8, 6);
	SBIT(ECCU,   0xE8, 4);
	SBIT(ESPI,   0xE8, 3);
	SBIT(EC,     0xE8, 2);
	SBIT(EKBI,   0xE8, 1);
	SBIT(EI2C,   0xE8, 0);
SFR(IP0, 0xB8); // Interrupt Priority 0
	SBIT(PWDRT,  0xB8, 6);
	SBIT(PBO,    0xB8, 5);
	SBIT(PS_PSR, 0xB8, 4);
	SBIT(PT1,    0xB8, 3);
	SBIT(PX1,    0xB8, 2);
	SBIT(PT0,    0xB8, 1);
	SBIT(PX0,    0xB8, 0);
SFR(IP0H, 0xB7); // Interrupt Priority 0 high
	#define PWDRTH    0x40
	#define PBOH      0x20
	#define PSH_PSRH  0x10
	#define PT1H      0x08
	#define PX1H      0x04
	#define PT0H      0x02
	#define PX0H      0x01
SFR(IP1,  0xF8); // Interrupt Priority 1
	SBIT(PADEE, 0xF8, 7);
	SBIT(PST,   0xF8, 6);
	SBIT(PCCU,  0xF8, 4);
	SBIT(PSPI,  0xF8, 3);
	SBIT(PC,    0xF8, 2);
	SBIT(PKBI,  0xF8, 1);
	SBIT(PI2C,  0xF8, 0);
SFR(IP1H, 0xF7); // Interrupt Priority 1 High
	#define PAEEH  0x80
	#define PSTH   0x40
	#define PCCUH  0x10
	#define PSPIH  0x08
	#define PCH    0x04
	#define PKBIH  0x02
	#define PI2CH  0x01
SFR(KBCON, 0x94); // Keypad control register
	#define PATN_SEL 0x02
	#define KBIF     0x01
SFR(KBMASK, 0x86); // Keypad interrupt mask register
SFR(KBPATN, 0x93); // Keypad pattern register
SFR(OCRAH,  0xEF); // Output compare A register high
SFR(OCRAL,  0xEE); // Output compare A register low
SFR(OCRBH,  0xFB); // Output compare B register high
SFR(OCRBL,  0xFA); // Output compare B register low
SFR(OCRCH,  0xFD); // Output compare C register high
SFR(OCRCL,  0xFC); // Output compare C register low
SFR(OCRDH,  0xFF); // Output compare D register high
SFR(OCRDL,  0xFE); // Output compare D register low
SFR(P0, 0x80); // Port 0
	SBIT(P0_7,   0x80, 7);
	SBIT(T1,     0x80, 7);
	SBIT(KB7,    0x80, 7);
	SBIT(P0_6,   0x80, 6);
	SBIT(CMP_1,  0x80, 6); // Renamed, not to conflict with the CMP1 register
	SBIT(KB6,    0x80, 6);
	SBIT(P0_5,   0x80, 5);
	SBIT(CMPREF, 0x80, 5);
	SBIT(KB5,    0x80, 5);
	SBIT(P0_4,   0x80, 4);
	SBIT(CIN1A,  0x80, 4);
	SBIT(KB4,    0x80, 4);
	SBIT(P0_3,   0x80, 3);
	SBIT(CIN1B,  0x80, 3);
	SBIT(KB3,    0x80, 3);
	SBIT(P0_2,   0x80, 2);
	SBIT(CIN2A,  0x80, 2);
	SBIT(KB2,    0x80, 2);
	SBIT(P0_1,   0x80, 1);
	SBIT(CIN2B,  0x80, 1);
	SBIT(KB1,    0x80, 1);
	SBIT(P0_0,   0x80, 0);
	SBIT(CMP_2,  0x80, 0); // Renamed, not to conflict with the CMP2 register
	SBIT(KB0,    0x80, 0);
SFR(P1, 0x90); // Port 1
	SBIT(P1_7,   0x90, 7);
	SBIT(OCC,    0x90, 7);
	SBIT(P1_6,   0x90, 6);
	SBIT(OCB,    0x90, 6);
	SBIT(P1_5,   0x90, 5);
	SBIT(RST,    0x90, 5);
	SBIT(P1_4,   0x90, 4);
	SBIT(INT1,   0x90, 4);
	SBIT(P1_3,   0x90, 3);
	SBIT(INT0,   0x90, 3);
	SBIT(SDA,    0x90, 3);
	SBIT(P1_2,   0x90, 2);
	SBIT(T0,     0x90, 2);
	SBIT(SCL,    0x90, 2);
	SBIT(P1_1,   0x90, 1);
	SBIT(RXD,    0x90, 1);
	SBIT(P1_0,   0x90, 0);
	SBIT(TXD,    0x90, 0);
SFR(P2, 0xA0); // Port 2
	SBIT(P2_7,   0xA0, 7);
	SBIT(ICA,    0xA0, 7);
	SBIT(P2_6,   0xA0, 6);
	SBIT(OCA,    0xA0, 6);
	SBIT(P2_5,   0xA0, 5);
	SBIT(SPICLK, 0xA0, 5);
	SBIT(P2_4,   0xA0, 4);
	SBIT(SS,     0xA0, 4);
	SBIT(P2_3,   0xA0, 3);
	SBIT(MISO,   0xA0, 3);
	SBIT(P2_2,   0xA0, 2);
	SBIT(MOSI,   0xA0, 2);
	SBIT(P2_1,   0xA0, 1);
	SBIT(OCD,    0xA0, 1);
	SBIT(P2_0,   0xA0, 0);
	SBIT(ICB,    0xA0, 0);
SFR(P3, 0xB0); // Port 3
	SBIT(P3_7, 0xB0, 7);
	SBIT(P3_6, 0xB0, 6);
	SBIT(P3_5, 0xB0, 5);
	SBIT(P3_4, 0xB0, 4);
	SBIT(P3_3, 0xB0, 3);
	SBIT(P3_2, 0xB0, 2);
	SBIT(P3_1, 0xB0, 1);
	SBIT(XTAL1,0xB0, 1);
	SBIT(P3_0, 0xB0, 0);
	SBIT(XTAL2,0xB0, 0);
SFR(P0M1, 0x84); // Port 0 output mode 1
	#define P0M1_7  0x80
	#define P0M1_6  0x40
	#define P0M1_5  0x20
	#define P0M1_4  0x10
	#define P0M1_3  0x08
	#define P0M1_2  0x04
	#define P0M1_1  0x02
	#define P0M1_0  0x01
SFR(P0M2, 0x85); // Port 0 output mode 2
	#define P0M2_7  0x80
	#define P0M2_6  0x40
	#define P0M2_5  0x20
	#define P0M2_4  0x10
	#define P0M2_3  0x08
	#define P0M2_2  0x04
	#define P0M2_1  0x02
	#define P0M2_0  0x01
SFR(P1M1, 0x91); // Port 1 output mode 1
	#define P1M1_7  0x80
	#define P1M1_6  0x40
	#define P1M1_4  0x10
	#define P1M1_3  0x08
	#define P1M1_2  0x04
	#define P1M1_1  0x02
	#define P1M1_0  0x01
SFR(P1M2, 0x92); // Port 1 output mode 2
	#define P1M2_7  0x80
	#define P1M2_6  0x40
	#define P1M2_4  0x10
	#define P1M2_3  0x08
	#define P1M2_2  0x04
	#define P1M2_1  0x02
	#define P1M2_0  0x01
SFR(P2M1, 0xA4); // Port 2 output mode 1
	#define P2M1_7  0x80
	#define P2M1_6  0x40
	#define P2M1_5  0x20
	#define P2M1_4  0x10
	#define P2M1_3  0x08
	#define P2M1_2  0x04
	#define P2M1_1  0x02
	#define P2M1_0  0x01
SFR(P2M2, 0xA5); // Port 2 output mode 2
	#define P2M2_7  0x80
	#define P2M2_6  0x40
	#define P2M2_5  0x20
	#define P2M2_4  0x10
	#define P2M2_3  0x08
	#define P2M2_2  0x04
	#define P2M2_1  0x02
	#define P2M2_0  0x01
SFR(P3M1, 0xB1); // Port 3 output mode 1
	#define P3M1_1  0x02
	#define P3M1_0  0x01
SFR(P3M2, 0xB2); // Port 3 output mode 2
	#define P3M2_1  0x02
	#define P3M2_0  0x01
SFR(PCON,  0x87); // Power control register
	#define SMOD1   0x80
	#define SMOD0   0x40
	#define BOPD    0x20
	#define BOI     0x10
	#define GF1     0x08
	#define GF0     0x04
	#define PMOD1   0x02
	#define PMOD0   0x01
SFR(PCONA, 0xB5); // Power control register A
	#define RTCPD   0x80
	#define DEEPD   0x40
	#define VCPD    0x20
	#define ADPD    0x10
	#define I2PD    0x08
	#define SPPD    0x04
	#define SPD     0x02
	#define CCUPR   0x01
SFR(PSW, 0xD0); // Program Status Word
	SBIT(CY,  0xD0, 7);
	SBIT(AC,  0xD0, 6);
	SBIT(F0,  0xD0, 5);
	SBIT(RS1, 0xD0, 4);
	SBIT(RS0, 0xD0, 3);
	SBIT(OV,  0xD0, 2);
	SBIT(F1,  0xD0, 1);
	SBIT(P,   0xD0, 0);
SFR(PT0AD, 0xF6); // Port 0 digital input disable
	#define PT0AD_5   0x20
	#define PT0AD_4   0x10
	#define PT0AD_3   0x08
	#define PT0AD_2   0x04
	#define PT0AD_1   0x02
SFR(RSTSRC, 0xDF); // Reset source register
	#define BOF    0x20
	#define POF    0x10
	#define R_BK   0x08
	#define R_WD   0x04
	#define R_SF   0x02
	#define R_EX   0x01
SFR(RTCCON, 0xD1); // Real-time clock control
	#define RTCF   0x80
	#define RTCS1  0x40
	#define RTCS0  0x20
	#define ERTC   0x02
	#define RTCEN  0x01
SFR(RTCH,  0xD2); // Real-time clock register high
SFR(RTCL,  0xD3); // Real-time clock register low
SFR(SADDR, 0xA9); // Serial port address register
SFR(SADEN, 0xB9); // Serial port address enable
SFR(SBUF,  0x99); // Serial port data buffer register
SFR(SCON,  0x98); // Serial port control
	SBIT(SM0_FE, 0x98, 7);
	SBIT(SM1,    0x98, 6);
	SBIT(SM2,    0x98, 5);
	SBIT(REN,    0x98, 4);
	SBIT(TB8,    0x98, 3);
	SBIT(RB8,    0x98, 2);
	SBIT(TI,     0x98, 1);
	SBIT(RI,     0x98, 0);
SFR(SSTAT, 0xBA); // Serial port extended status register
	#define DBMOD  0x80
	#define INTLO  0x40
	#define CIDIS  0x20
	#define DBISEL 0x10
	#define FE     0x08
	#define BR     0x04
	#define OE     0x02
	#define STINT  0x01
SFR(SP,     0x81); // Stack Pointer
SFR(SPCTL,  0xE2); // SPI control register
	#define SSIG  0x80
	#define SPEN  0x40
	#define DORD  0x20
	#define MSTR  0x10
	#define CPOL  0x08
	#define CPHA  0x04
	#define SPR1  0x02
	#define SPR0  0x01
SFR(SPSTAT, 0xE1); // SPI status register
	#define SPIF  0x80
	#define WCOL  0x40
SFR(SPDAT,  0xE3); // SPI data register
SFR(TAMOD,  0x8F); // Timer 0 and 1 auxiliary mode
	#define T1M2  0x10
	#define T0M2  0x01
SFR(TCON,   0x88); // Timer 0 and 1 control
	SBIT(TF1, 0x88, 7);
	SBIT(TR1, 0x88, 6);
	SBIT(TF0, 0x88, 5);
	SBIT(TR0, 0x88, 4);
	SBIT(IE1, 0x88, 3);
	SBIT(IT1, 0x88, 2);
	SBIT(IE0, 0x88, 1);
	SBIT(IT0, 0x88, 0);
SFR(TCR20,  0xC8); // CCU control register 0
	SBIT(PLEEN,  0xC8, 7);
	SBIT(HLTRN,  0xC8, 6);
	SBIT(HLTEN,  0xC8, 5);
	SBIT(ALTCD,  0xC8, 4);
	SBIT(ALTAB,  0xC8, 3);
	SBIT(TDIR2,  0xC8, 2);
	SBIT(TMOD21, 0xC8, 1);
	SBIT(TMOD20, 0xC8, 0);
SFR(TCR21,  0xF9); // CCU control register 1
	#define TCOU2   0x80
	#define PLLDV_3 0x08
	#define PLLDV_2 0x04
	#define PLLDV_1 0x02
	#define PLLDV_0 0x01
SFR(TH0,  0x8C); // Timer 0 high
SFR(TH1,  0x8D); // Timer 1 high
SFR(TH2,  0xCD); // CCU timer high
SFR(TICR2,0xC9); // CCU interrupt control register
	#define TOIE2    0x80
	#define TOCIE2D  0x40
	#define TOCIE2C  0x20
	#define TOCIE2B  0x10
	#define TOCIE2A  0x08
	#define TICIE2B  0x02
	#define TICIE2A  0x01
SFR(TIFR2,0xE9); // CCU interrupt flag register
	#define TOIF2    0x80
	#define TOCF2D  0x40
	#define TOCF2C  0x20
	#define TOCF2B  0x10
	#define TOCF2A  0x08
	#define TICF2B  0x02
	#define TICF2A  0x01
SFR(TISE2,0xDE); // CCU interrupt status encode register
	#define ENCINT2  0x04
	#define ENCINT1  0x02
	#define ENCINT0  0x01
SFR(TL0,   0x8A); // Timer 0 low
SFR(TL1,   0x8B); // Timer 1 low
SFR(TL2,   0xCC); // CCU timer low
SFR(TMOD, 0x89); // Timer 0 and 1 mode
	#define T1GATE  0x80
	#define T1C_T   0x40
	#define T1M1    0x20
	#define T1M0    0x10
	#define T0GATE  0x08
	#define T0C_T   0x04
	#define T0M1    0x02
	#define T0M0    0x01
SFR(TOR2H, 0xCF); // CCU reload register high
SFR(TOR2L, 0xCE); // CCU reload register low
SFR(TPCR2H,0xCB); // Prescaler control register high
	#define TPCR2H_1 0x02
	#define TPCR2H_0 0x01
SFR(TPCR2L,0xCA); // Prescaler control register low
	#define TPCR2L_7 0x80
	#define TPCR2L_6 0x40
	#define TPCR2L_5 0x20
	#define TPCR2L_4 0x10
	#define TPCR2L_3 0x08
	#define TPCR2L_2 0x04
	#define TPCR2L_1 0x02
	#define TPCR2L_0 0x01
SFR(TRIM, 0x96); // Internal oscillator trim register
	#define RCCLK   0x80
	#define ENCLK   0x40
	#define TRIM_5  0x20
	#define TRIM_4  0x10
	#define TRIM_3  0x08
	#define TRIM_2  0x04
	#define TRIM_1  0x02
	#define TRIM_0  0x01
SFR(WDCON, 0xA7); // Watchdog control register
	#define PRE2    0x80
	#define PRE1    0x40
	#define PRE0    0x20
	#define WDRUN   0x04
	#define WDTOF   0x02
	#define WDCLK   0x01
SFR(WDL,    0xC1); // Watchdog load
SFR(WFEED1, 0xC2); // Watchdog feed 1
SFR(WFEED2, 0xC3); // Watchdog feed 2
#endif // __P89LPC935_6_H__
