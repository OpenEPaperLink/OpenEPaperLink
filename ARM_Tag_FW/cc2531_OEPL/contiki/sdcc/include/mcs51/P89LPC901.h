/*--------------------------------------------------------------------------
P89LPC901.H
(English)
This header allows to use the microcontroler Philips P89LPC901
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
Archivo encabezador para el ucontrolador Philips P89LPC901.
Derechos de copy (DC) 2005.  OMAR ESPINOSA P.  E-mail: opiedrahita2003 AT yahoo.com
Uso libre
--------------------------------------------------------------------------*/

#ifndef __REG901_H__
#define __REG901_H__

/*  BYTE Registers  */
__sfr __at (0x80) P0       ;
__sfr __at (0x84) P0M1     ;
__sfr __at (0x85) P0M2     ;

__sfr __at (0x90) P1       ;
__sfr __at (0x91) P1M1     ;
__sfr __at (0x92) P1M2     ;

__sfr __at (0xB0) P3       ;
__sfr __at (0xB1) P3M1     ;
__sfr __at (0xB2) P3M2     ;
//------------------
__sfr __at (0xD0) PSW      ;
__sfr __at (0xE0) ACC      ;
__sfr __at (0xF0) B        ;
__sfr __at (0x81) SP       ;
__sfr __at (0x82) DPL      ;
__sfr __at (0x83) DPH      ;
//------------------
__sfr __at (0xA2) AUXR1    ;
__sfr __at (0xAC) CMP1     ;
__sfr __at (0x95) DIVM     ;

__sfr __at (0xE7) FMADRH   ;
__sfr __at (0xE6) FMADRL   ;
__sfr __at (0xE4) FMCON    ;
__sfr __at (0xE5) FMDATA   ;

__sfr __at (0xA8) IEN0     ;
__sfr __at (0xE8) IEN1     ;

__sfr __at (0xB8) IP0      ;
__sfr __at (0xB7) IP0H     ;
__sfr __at (0xF8) IP1      ;
__sfr __at (0xF7) IP1H     ;

__sfr __at (0x94) KBCON    ;
__sfr __at (0x86) KBMASK   ;
__sfr __at (0x93) KBPATN   ;

__sfr __at (0x87) PCON     ;
__sfr __at (0xB5) PCONA    ;
__sfr __at (0xB6) PCONB    ;

__sfr __at (0xF6) PT0AD    ;
__sfr __at (0xDF) RSTSRC   ;

__sfr __at (0xD1) RTCCON   ;
__sfr __at (0xD2) RTCH     ;
__sfr __at (0xD3) RTCL     ;

__sfr __at (0x8F) TAMOD    ;
__sfr __at (0x88) TCON     ;
__sfr __at (0x8A) TL0      ;
__sfr __at (0x8B) TL1      ;
__sfr __at (0x8C) TH0      ;
__sfr __at (0x8D) TH1      ;
__sfr __at (0x89) TMOD     ;
__sfr __at (0x96) TRIM     ;

__sfr __at (0xA7) WDCON    ;
__sfr __at (0xC1) WDL      ;
__sfr __at (0xC2) WFEED1   ;
__sfr __at (0xC3) WFEED2   ;


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

#define  CY      PSW_7
#define  AC      PSW_6
#define  F0      PSW_5
#define  RS1     PSW_4
#define  RS0     PSW_3
#define  OV      PSW_2
#define  F1      PSW_1
#define  P       PSW_0

/*  TCON  */
__sbit __at (0x8F)    TCON_7;
__sbit __at (0x8E)    TCON_6;
__sbit __at (0x8D)    TCON_5;
__sbit __at (0x8C)    TCON_4;

#define TF1      TCON_7
#define TR1      TCON_6
#define TF0      TCON_5
#define TR0      TCON_4

/*  IEN0   */
__sbit __at (0xAF)    IEN0_7;
__sbit __at (0xAE)    IEN0_6;
__sbit __at (0xAD)    IEN0_5;
__sbit __at (0xAB)    IEN0_3;
__sbit __at (0xA9)    IEN0_1;

#define EA       IEN0_7
#define EWDRT    IEN0_6
#define EBO      IEN0_5
#define ET1      IEN0_3
#define ET0      IEN0_1

/*  IEN1   */
__sbit __at (0xEA)    IEN1_2;
__sbit __at (0xE9)    IEN1_1;

#define EC       IEN1_2
#define EKBI     IEN1_1

/*  IP0   */
__sbit __at (0xBE)    IP0_6;
__sbit __at (0xBD)    IP0_5;
__sbit __at (0xBB)    IP0_3;
__sbit __at (0xB9)    IP0_1;

#define PWDRT    IP0_6
#define PB0      IP0_5
#define PT1      IP0_3
#define PT0      IP0_1

/*  P0  */
__sbit __at (0x85)    P0_5;
__sbit __at (0x84)    P0_4;

#define KB5      P0_5
#define CMPREF   P0_5
#define KB4      P0_4
#define CIN1A    P0_4

/*  P1  */
__sbit __at (0x95)    P1_5;
__sbit __at (0x92)    P1_2;

#define RST      P1_5
#define T0       P1_2

/*  P3  */
__sbit __at (0xB1)    P3_1;
__sbit __at (0xB0)    P3_0;

#define XTAL1    P3_1
#define XTAL2    P3_0

#endif
