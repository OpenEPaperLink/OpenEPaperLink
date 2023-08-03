/*--------------------------------------------------------------------------
P89c51RD2.H
(English)
This header allows to use the microcontroler Philips P89c51RD2
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
Archivo encabezador para el ucontrolador Philips P89c51RD2.
Derechos de copy (DC) 2005.  OMAR ESPINOSA P.  E-mail: opiedrahita2003 AT yahoo.com
Uso libre
--------------------------------------------------------------------------*/

#ifndef __P89c51RD2_H__
#define __P89c51RD2_H__

/*  BYTE Registers  */
__sfr __at (0x80) P0       ;
__sfr __at (0x90) P1       ;
__sfr __at (0xA0) P2       ;
__sfr __at (0xB0) P3       ;
__sfr __at (0xD0) PSW      ;
__sfr __at (0xE0) ACC      ;
__sfr __at (0xF0) B        ;
__sfr __at (0x81) SP       ;
__sfr __at (0x82) DPL      ;
__sfr __at (0x83) DPH      ;
__sfr __at (0x87) PCON     ;
__sfr __at (0x88) TCON     ;
__sfr __at (0x89) TMOD     ;
__sfr __at (0x8A) TL0      ;
__sfr __at (0x8B) TL1      ;
__sfr __at (0x8C) TH0      ;
__sfr __at (0x8D) TH1      ;
__sfr __at (0xA8) IE       ;
__sfr __at (0xB8) IP       ;
__sfr __at (0x98) SCON     ;
__sfr __at (0x99) SBUF     ;

/*  80C51Fx/Rx Extensions  */
__sfr __at (0x8E) AUXR     ;
__sfr __at (0xA2) AUXR1    ;
__sfr __at (0xA9) SADDR    ;
__sfr __at (0xB7) IPH      ;
__sfr __at (0xB9) SADEN    ;
__sfr __at (0xC8) T2CON    ;
__sfr __at (0xC9) T2MOD    ;
__sfr __at (0xCA) RCAP2L   ;
__sfr __at (0xCB) RCAP2H   ;
__sfr __at (0xCC) TL2      ;
__sfr __at (0xCD) TH2      ;
__sfr __at (0xD8) CCON     ;
__sfr __at (0xD9) CMOD     ;
__sfr __at (0xDA) CCAPM0   ;
__sfr __at (0xDB) CCAPM1   ;
__sfr __at (0xDC) CCAPM2   ;
__sfr __at (0xDD) CCAPM3   ;
__sfr __at (0xDE) CCAPM4   ;
__sfr __at (0xE9) CL       ;
__sfr __at (0xEA) CCAP0L   ;
__sfr __at (0xEB) CCAP1L   ;
__sfr __at (0xEC) CCAP2L   ;
__sfr __at (0xED) CCAP3L   ;
__sfr __at (0xEE) CCAP4L   ;
__sfr __at (0xF9) CH       ;
__sfr __at (0xFA) CCAP0H   ;
__sfr __at (0xFB) CCAP1H   ;
__sfr __at (0xFC) CCAP2H   ;
__sfr __at (0xFD) CCAP3H   ;
__sfr __at (0xFE) CCAP4H   ;


/*  BIT Registers  */
/*  PSW   */

__sbit __at (0xD7)    PSW_7;
__sbit __at (0xD6)    PSW_6;
__sbit __at (0xD5)    PSW_5;
__sbit __at (0xD4)    PSW_4;
__sbit __at (0xD3)    PSW_3;
__sbit __at (0xD2)    PSW_2;
__sbit __at (0xD0)    PSW_0;

#define  CY     PSW_7
#define  AC     PSW_6
#define  F0     PSW_5
#define  RS1    PSW_4
#define  RS0    PSW_3
#define  OV     PSW_2
#define  P      PSW_0

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

/*  IE   */
__sbit __at (0xAF)    IE_7;
__sbit __at (0xAE)    IE_6;
__sbit __at (0xAD)    IE_5;
__sbit __at (0xAC)    IE_4;
__sbit __at (0xAB)    IE_3;
__sbit __at (0xAA)    IE_2;
__sbit __at (0xA9)    IE_1;
__sbit __at (0xA8)    IE_0;

#define  EA     IE_7
#define  EC     IE_6
#define  ET2    IE_5
#define  ES     IE_4
#define  ET1    IE_3
#define  EX1    IE_2
#define  ET0    IE_1
#define  EX0    IE_0

/*  IP   */
__sbit __at (0xBE)    IP_6;
__sbit __at (0xBD)    IP_5;
__sbit __at (0xBC)    IP_4;
__sbit __at (0xBB)    IP_3;
__sbit __at (0xBA)    IP_2;
__sbit __at (0xB9)    IP_1;
__sbit __at (0xB8)    IP_0;

#define  PPC    IP_6
#define  PT2    IP_5
#define  PS     IP_4
#define  PT1    IP_3
#define  PX1    IP_2
#define  PT0    IP_1
#define  PX0    IP_0

/*  P3  */
__sbit __at (0xB7)    P3_7;
__sbit __at (0xB6)    P3_6;
__sbit __at (0xB5)    P3_5;
__sbit __at (0xB4)    P3_4;
__sbit __at (0xB3)    P3_3;
__sbit __at (0xB2)    P3_2;
__sbit __at (0xB1)    P3_1;
__sbit __at (0xB0)    P3_0;

#define  RD     P3_7
#define  WR     P3_6
#define  T1     P3_5
#define  T0     P3_4
#define  INT1   P3_3
#define  INT0   P3_2
#define  TXD    P3_1
#define  RXD    P3_0

/*  SCON  */
__sbit __at (0x9F)    SCON_7; // alternatively "FE"
__sbit __at (0x9E)    SCON_6;
__sbit __at (0x9D)    SCON_5;
__sbit __at (0x9C)    SCON_4;
__sbit __at (0x9B)    SCON_3;
__sbit __at (0x9A)    SCON_2;
__sbit __at (0x99)    SCON_1;
__sbit __at (0x98)    SCON_0;

#define  SM0    SCON_7  // alternatively "FE"
#define  FE     SCON_7
#define  SM1    SCON_6
#define  SM2    SCON_5
#define  REN    SCON_4
#define  TB8    SCON_3
#define  RB8    SCON_2
#define  TI     SCON_1
#define  RI     SCON_0

/*  P1  */
__sbit __at (0x97)    P1_7;
__sbit __at (0x96)    P1_6;
__sbit __at (0x95)    P1_5;
__sbit __at (0x94)    P1_4;
__sbit __at (0x93)    P1_3;
__sbit __at (0x92)    P1_2;
__sbit __at (0x91)    P1_1;
__sbit __at (0x90)    P1_0;

#define  CEX4   P1_7
#define  CEX3   P1_6
#define  CEX2   P1_5
#define  CEX1   P1_4
#define  CEX0   P1_3
#define  ECI    P1_2
#define  T2EX   P1_1
#define  T2     P1_0

/*  T2CON  */
__sbit __at (0xCF)    T2CON_7;
__sbit __at (0xCE)    T2CON_6;
__sbit __at (0xCD)    T2CON_5;
__sbit __at (0xCC)    T2CON_4;
__sbit __at (0xCB)    T2CON_3;
__sbit __at (0xCA)    T2CON_2;
__sbit __at (0xC9)    T2CON_1;
__sbit __at (0xC8)    T2CON_0;

#define  TF2     T2CON_7
#define  EXF2    T2CON_6
#define  RCLK    T2CON_5
#define  TCLK    T2CON_4
#define  EXEN2   T2CON_3
#define  TR2     T2CON_2
#define  C_T2    T2CON_1
#define  CP_RL2  T2CON_0

/*  CCON  */
__sbit __at (0xDF)    CCON_7;
__sbit __at (0xDE)    CCON_6;
__sbit __at (0xDC)    CCON_4;
__sbit __at (0xDB)    CCON_3;
__sbit __at (0xDA)    CCON_2;
__sbit __at (0xD9)    CCON_1;
__sbit __at (0xD8)    CCON_0;

#define  CF      CCON_7
#define  CR      CCON_6
#define  CCF4    CCON_4
#define  CCF3    CCON_3
#define  CCF2    CCON_2
#define  CCF1    CCON_1
#define  CCF0    CCON_0

#endif
