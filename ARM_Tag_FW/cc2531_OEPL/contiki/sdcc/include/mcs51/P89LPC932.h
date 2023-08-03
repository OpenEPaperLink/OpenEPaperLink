/*--------------------------------------------------------------------------
P89LPC932.H
(English)
This header allows to use the microcontroler Philips P89LPC932
with the compiler SDCC.

Copyright (c) 2005 Omar Espinosa--e-mail: opiedrahita2003 AT yahoo.com.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

(Spanish-Español)
Archivo encabezador para el ucontrolador Philips P89LPC932.
Derechos de copy (DC) 2005.  OMAR ESPINOSA P.  E-mail: opiedrahita2003 AT yahoo.com
Uso libre
--------------------------------------------------------------------------*/
#ifndef __REG932_H__
#define __REG932_H__

/*  BYTE Registers  */
__sfr __at (0x80) P0     ;
__sfr __at (0x90) P1     ;
__sfr __at (0xA0) P2     ;
__sfr __at (0xB0) P3     ;
__sfr __at (0xD0) PSW    ;
__sfr __at (0xE0) ACC    ;
__sfr __at (0xF0) B      ;
__sfr __at (0x81) SP     ;
__sfr __at (0x82) DPL    ;
__sfr __at (0x83) DPH    ;
__sfr __at (0x87) PCON   ;
__sfr __at (0x88) TCON   ;
__sfr __at (0x89) TMOD   ;
__sfr __at (0x8A) TL0    ;
__sfr __at (0x8B) TL1    ;
__sfr __at (0x8C) TH0    ;
__sfr __at (0x8D) TH1    ;
__sfr __at (0xA8) IEN0   ;
__sfr __at (0xB8) IP0    ;
__sfr __at (0x98) SCON   ;
__sfr __at (0x99) SBUF   ;


__sfr __at (0xA2) AUXR1  ;
__sfr __at (0xA9) SADDR  ;
__sfr __at (0xB9) SADEN  ;
__sfr __at (0xCC) TL2    ;
__sfr __at (0xCD) TH2    ;
__sfr __at (0xBE) BRGR0  ;
__sfr __at (0xBF) BRGR1  ;
__sfr __at (0xBD) BRGCON ;
__sfr __at (0xEA) CCCRA  ;
__sfr __at (0xEB) CCCRB  ;
__sfr __at (0xEC) CCCRC  ;
__sfr __at (0xED) CCCRD  ;
__sfr __at (0xAC) CMP1   ;
__sfr __at (0xAD) CMP2   ;
__sfr __at (0xF1) DEECON ;
__sfr __at (0xF2) DEEDAT ;
__sfr __at (0xF3) DEEADR ;
__sfr __at (0x95) DIVM   ;
__sfr __at (0xDB) I2ADR  ;
__sfr __at (0xD8) I2CON  ;
__sfr __at (0xDA) I2DAT  ;
__sfr __at (0xDD) I2SCLH ;
__sfr __at (0xDC) I2SCLL ;
__sfr __at (0xD9) I2STAT ;
__sfr __at (0xAB) ICRAH  ;
__sfr __at (0xAA) ICRAL  ;
__sfr __at (0xAF) ICRBH  ;
__sfr __at (0xAE) ICRBL  ;
__sfr __at (0xE8) IEN1   ;
__sfr __at (0xF8) IP1    ;
__sfr __at (0xF7) IP1H   ;
__sfr __at (0x94) KBCON  ;
__sfr __at (0x86) KBMASK ;
__sfr __at (0x93) KBPATN ;
__sfr __at (0xEF) OCRAH  ;
__sfr __at (0xEE) OCRAL  ;
__sfr __at (0xFB) OCRBH  ;
__sfr __at (0xFA) OCRBL  ;
__sfr __at (0xFD) OCRCH  ;
__sfr __at (0xFC) OCRCL  ;
__sfr __at (0xFF) OCRDH  ;
__sfr __at (0xFE) OCRDL  ;
__sfr __at (0x84) P0M1   ;
__sfr __at (0x85) P0M2   ;
__sfr __at (0x91) P1M1   ;
__sfr __at (0x92) P1M2   ;
__sfr __at (0xA4) P2M1   ;
__sfr __at (0xA5) P2M2   ;
__sfr __at (0xB1) P3M1   ;
__sfr __at (0xB2) P3M2   ;
__sfr __at (0xB5) PCONA  ;
__sfr __at (0xF6) PT0AD  ;
__sfr __at (0xDF) RSTSRC ;
__sfr __at (0xD1) RTCCON ;
__sfr __at (0xD2) RTCH   ;
__sfr __at (0xD3) RTCL   ;
__sfr __at (0xBA) SSTAT  ;
__sfr __at (0xE2) SPCTL  ;
__sfr __at (0xE1) SPSTAT ;
__sfr __at (0xE3) SPDAT  ;
__sfr __at (0x8F) TAMOD  ;
__sfr __at (0xC8) TCR20  ;
__sfr __at (0xF9) TCR21  ;
__sfr __at (0xC9) TICR2  ;
__sfr __at (0xE9) TIFR2  ;
__sfr __at (0xDE) TISE2  ;
__sfr __at (0xCF) TOR2H  ;
__sfr __at (0xCE) TOR2L  ;
__sfr __at (0xCB) TPCR2H ;
__sfr __at (0xCA) TPCR2L ;
__sfr __at (0x96) TRIM   ;
__sfr __at (0xA7) WDCON  ;
__sfr __at (0xC1) WDL    ;
__sfr __at (0xC2) WFEED1 ;
__sfr __at (0xC3) WFEED2 ;
__sfr __at (0xB7) IP0H   ;

/*  BIT Registers  */
/*  PSW   */
__sbit __at (0xD7)    PSW_7;
__sbit __at (0xD6)    PSW_6;
__sbit __at (0xD5)    PSW_5;
__sbit __at (0xD4)    PSW_4;
__sbit __at (0xD3)    PSW_3;
__sbit __at (0xD2)    PSW_2;
__sbit __at (0xD1)    PSW_1;
__sbit __at (0xD0)    PSW_0;

#define CY      PSW_7
#define AC      PSW_6
#define F0      PSW_5
#define RS1     PSW_4
#define RS0     PSW_3
#define OV      PSW_2
#define F1      PSW_1
#define P       PSW_0

/*  TCON  */
__sbit __at (0x8F)    TCON_7;
__sbit __at (0x8E)    TCON_6;
__sbit __at (0x8D)    TCON_5;
__sbit __at (0x8C)    TCON_4;
__sbit __at (0x8B)    TCON_3;
__sbit __at (0x8A)    TCON_2;
__sbit __at (0x89)    TCON_1;
__sbit __at (0x88)    TCON_0;

#define  TF1    TCON_7
#define  TR1    TCON_6
#define  TF0    TCON_5
#define  TR0    TCON_4
#define  IE1    TCON_3
#define  IT1    TCON_2
#define  IE0    TCON_1
#define  IT0    TCON_0

/*  IEN0   */
__sbit __at (0xAF)    IEN0_7;
__sbit __at (0xAE)    IEN0_6;
__sbit __at (0xAD)    IEN0_5;
__sbit __at (0xAC)    IEN0_4; // alternatively "ESR"
__sbit __at (0xAC)    IEN0_4;
__sbit __at (0xAB)    IEN0_3;
__sbit __at (0xAA)    IEN0_2;
__sbit __at (0xA9)    IEN0_1;
__sbit __at (0xA8)    IEN0_0;

#define  EA	IEN0_7
#define  EWDRT  IEN0_6
#define  EBO    IEN0_5
#define  ES     IEN0_4  // alternatively "ESR"
#define  ESR    IEN0_4
#define  ET1    IEN0_3
#define  EX1    IEN0_2
#define  ET0    IEN0_1
#define  EX0    IEN0_0

/*  IEN1   */
__sbit __at (0xEF)   IEN1_7;
__sbit __at (0xEE)   IEN1_6;
__sbit __at (0xEC)   IEN1_4;
__sbit __at (0xEB)   IEN1_3;
__sbit __at (0xEA)   IEN1_2;
__sbit __at (0xE9)   IEN1_1;
__sbit __at (0xE8)   IEN1_0;

#define  EIEE   IEN1_7
#define  EST    IEN1_6
#define  ECCU   IEN1_4
#define  ESPI   IEN1_3
#define  EC     IEN1_2
#define  EKBI   IEN1_1
#define  EI2C   IEN1_0

/*  IP0   */
__sbit __at (0xBE)    IP0_6;
__sbit __at (0xBD)    IP0_5;
__sbit __at (0xBC)    IP0_4; // alternatively "PSR"
__sbit __at (0xBC)    IP0_4;
__sbit __at (0xBB)    IP0_3;
__sbit __at (0xBA)    IP0_2;
__sbit __at (0xB9)    IP0_1;
__sbit __at (0xB8)    IP0_0;

#define  PWDRT  IP0_6
#define  PB0    IP0_5
#define  PS     IP0_4 // alternatively "PSR"
#define  PSR    IP0_4
#define  PT1    IP0_3
#define  PX1    IP0_2
#define  PT0    IP0_1
#define  PX0    IP0_0

/*  SCON  */
__sbit __at (0x9F)    SCON_7; // alternatively "FE"
__sbit __at (0x9E)    SCON_6;
__sbit __at (0x9D)    SCON_5;
__sbit __at (0x9C)    SCON_4;
__sbit __at (0x9B)    SCON_3;
__sbit __at (0x9A)    SCON_2;
__sbit __at (0x99)    SCON_1;
__sbit __at (0x98)    SCON_0;

#define  SM0    SCON_7 // alternatively "FE"
#define  FE     SCON_7
#define  SM1    SCON_6
#define  SM2    SCON_5
#define  REN    SCON_4
#define  TB8    SCON_3
#define  RB8    SCON_2
#define  TI     SCON_1
#define  RI     SCON_0

/*  I2CON  */
__sbit __at (0xDE)    I2CON_6;
__sbit __at (0xDD)    I2CON_5;
__sbit __at (0xDC)    I2CON_4;
__sbit __at (0xDB)    I2CON_3;
__sbit __at (0xDA)    I2CON_2;
__sbit __at (0xD8)    I2CON_0;

#define  I2EN   I2CON_6;
#define  STA    I2CON_5;
#define  STO    I2CON_4;
#define  SI     I2CON_3;
#define  AA     I2CON_2;
#define  CRSEL  I2CON_0;

/*  P0  */
__sbit __at (0x87)    P0_7;
__sbit __at (0x86)    P0_6; // alternatively "CMP1"
__sbit __at (0x85)    P0_5;
__sbit __at (0x84)    P0_4;
__sbit __at (0x83)    P0_3;
__sbit __at (0x82)    P0_2;
__sbit __at (0x81)    P0_1;
__sbit __at (0x80)    P0_0; // alternatively "CMP2"

#define  KB7    P0_7  // alternatively "T1"
#define  T1     P0_7
#define  KB6    P0_6  // alternatively "CMP_1"
#define  CMP_1  P0_6
#define  KB5    P0_5
#define  KB4    P0_4
#define  KB3    P0_3
#define  KB2    P0_2
#define  KB1    P0_1
#define  KB0    P0_0  // alternatively "CMP_2"
#define  CMP_2  P0_0

/*  P1  */
__sbit __at (0x97)    P1_7;
__sbit __at (0x96)    P1_6;
__sbit __at (0x95)    P1_5;
__sbit __at (0x94)    P1_4;
__sbit __at (0x93)    P1_3;
__sbit __at (0x92)    P1_2;
__sbit __at (0x91)    P1_1;
__sbit __at (0x90)    P1_0;

#define  OCC    P1_7
#define  OCB    P1_6
#define  RST    P1_5
#define  INT1   P1_4
#define  INT0   P1_3 // alternatively "SDA"
#define  SDA    P1_3
#define  T0     P1_2 // alternatively "SCL"
#define  SCL    P1_2
#define  RxD    P1_1
#define  TxD    P1_0

/*  P2  */
__sbit __at (0xA7)    P2_7;
__sbit __at (0xA6)    P2_6;
__sbit __at (0xA5)    P2_5;
__sbit __at (0xA4)    P2_4;
__sbit __at (0xA3)    P2_3;
__sbit __at (0xA2)    P2_2;
__sbit __at (0xA1)    P2_1;
__sbit __at (0xA0)    P2_0;

#define  ICA    P2_7
#define  OCA    P2_6
#define  SPICLK P2_5
#define  SS     P2_4
#define  MISO   P2_3
#define  MOSI   P2_2
#define  OCD    P2_1
#define  ICB    P2_0

/*  P3  */
__sbit __at (0xB1)    P3_1;
__sbit __at (0xB0)    P3_0;

#define  XTAL1  P3_1
#define  XTAL2  P3_0

/* TCR20 */
__sbit __at (0xCF)    TCR20_7;
__sbit __at (0xCE)    TCR20_6;
__sbit __at (0xCD)    TCR20_5;
__sbit __at (0xCC)    TCR20_4;
__sbit __at (0xCB)    TCR20_3;
__sbit __at (0xCA)    TCR20_2;
__sbit __at (0xC9)    TCR20_1;
__sbit __at (0xC8)    TCR20_0;

#define  PLLEN  TCR20_7
#define  HLTRN  TCR20_6
#define  HLTEN  TCR20_5
#define  ALTCD  TCR20_4
#define  ALTAB  TCR20_3
#define  TDIR2  TCR20_2
#define  TMOD21 TCR20_1
#define  TMOD20 TCR20_0

#endif
