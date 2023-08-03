/*-------------------------------------------------------------------------
   sab80515.h - Register Declarations for SIEMENS/INFINEON SAB 80515 Processor
   based on reg51.h by Sandeep Dutta sandeep.dutta@usa.net
   KEIL C compatible definitions are included

   Copyright (C) 2005, Bela Torok / bela.torok@kssg.ch

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

#ifndef SAB80515_H
#define SAB80515_H

/* BYTE addressable registers */
__sfr __at 0x80 P0          ;
__sfr __at 0x81 SP          ;
__sfr __at 0x82 DPL         ;
__sfr __at 0x83 DPH         ;
__sfr __at 0x87 PCON        ;
__sfr __at 0x88 TCON        ;
__sfr __at 0x89 TMOD        ;
__sfr __at 0x8A TL0         ;
__sfr __at 0x8B TL1         ;
__sfr __at 0x8C TH0         ;
__sfr __at 0x8D TH1         ;
__sfr __at 0x90 P1          ;
__sfr __at 0x98 SCON        ;
__sfr __at 0x99 SBUF        ;
__sfr __at 0xA0 P2          ;
__sfr __at 0xA8 IE          ;
__sfr __at 0xA8 IEN0        ; /* as called by Siemens */
__sfr __at 0xA9 IP0         ; /* interrupt priority register - SAB80515 specific */
__sfr __at 0xB0 P3          ;
__sfr __at 0xB8 IEN1        ; /* interrupt enable register - SAB80515 specific */
__sfr __at 0xB9 IP1         ; /* interrupt priority register as called by Siemens */
__sfr __at 0xC0 IRCON       ; /* interrupt control register - SAB80515 specific */
__sfr __at 0xC1 CCEN        ; /* compare/capture enable register */
__sfr __at 0xC2 CCL1        ; /* compare/capture register 1, low byte */
__sfr __at 0xC3 CCH1        ; /* compare/capture register 1, high byte */
__sfr __at 0xC4 CCL2        ; /* compare/capture register 2, low byte */
__sfr __at 0xC5 CCH2        ; /* compare/capture register 2, high byte */
__sfr __at 0xC6 CCL3        ; /* compare/capture register 3, low byte */
__sfr __at 0xC7 CCH3        ; /* compare/capture register 3, high byte */
__sfr __at 0xC8 T2CON       ;
__sfr __at 0xCA CRCL        ; /* compare/reload/capture register, low byte */
__sfr __at 0xCB CRCH        ; /* compare/reload/capture register, high byte */
__sfr __at 0xCC TL2         ;
__sfr __at 0xCD TH2         ;
__sfr __at 0xD0 PSW         ;
__sfr __at 0xD8 ADCON       ; /* A/D-converter control register */
__sfr __at 0xD9 ADDAT       ; /* A/D-converter data register */
__sfr __at 0xDA DAPR        ; /* D/A-converter program register */
__sfr __at 0xDB P6          ; /* Port 6 - SAB80515 specific */
__sfr __at 0xE0 ACC         ;
__sfr __at 0xE0 A           ;
__sfr __at 0xE8 P4          ; /* Port 4 - SAB80515 specific */
__sfr __at 0xF0 B           ;
__sfr __at 0xF8 P5          ; /* Port 5 - SAB80515 specific */


/* BIT addressable registers */
/* P0 */
__sbit __at 0x80 P0_0       ;
__sbit __at 0x81 P0_1       ;
__sbit __at 0x82 P0_2       ;
__sbit __at 0x83 P0_3       ;
__sbit __at 0x84 P0_4       ;
__sbit __at 0x85 P0_5       ;
__sbit __at 0x86 P0_6       ;
__sbit __at 0x87 P0_7       ;

/* TCON */
__sbit __at 0x88 IT0        ;
__sbit __at 0x89 IE0        ;
__sbit __at 0x8A IT1        ;
__sbit __at 0x8B IE1        ;
__sbit __at 0x8C TR0        ;
__sbit __at 0x8D TF0        ;
__sbit __at 0x8E TR1        ;
__sbit __at 0x8F TF1        ;

/* P1 */
__sbit __at 0x90 P1_0       ;
__sbit __at 0x91 P1_1       ;
__sbit __at 0x92 P1_2       ;
__sbit __at 0x93 P1_3       ;
__sbit __at 0x94 P1_4       ;
__sbit __at 0x95 P1_5       ;
__sbit __at 0x96 P1_6       ;
__sbit __at 0x97 P1_7       ;

__sbit __at 0x90 INT3_CC0   ; /* P1 alternate functions - SAB80515 specific */
__sbit __at 0x91 INT4_CC1   ;
__sbit __at 0x92 INT5_CC2   ;
__sbit __at 0x93 INT6_CC3   ;
__sbit __at 0x94 INT2       ;
__sbit __at 0x95 T2EX       ;
__sbit __at 0x96 CLKOUT     ;
__sbit __at 0x97 T2         ;

/* SCON */
__sbit __at 0x98 RI         ;
__sbit __at 0x99 TI         ;
__sbit __at 0x9A RB8        ;
__sbit __at 0x9B TB8        ;
__sbit __at 0x9C REN        ;
__sbit __at 0x9D SM2        ;
__sbit __at 0x9E SM1        ;
__sbit __at 0x9F SM0        ;

/* P2 */
__sbit __at 0xA0 P2_0       ;
__sbit __at 0xA1 P2_1       ;
__sbit __at 0xA2 P2_2       ;
__sbit __at 0xA3 P2_3       ;
__sbit __at 0xA4 P2_4       ;
__sbit __at 0xA5 P2_5       ;
__sbit __at 0xA6 P2_6       ;
__sbit __at 0xA7 P2_7       ;

/* IEN0 */
__sbit __at 0xA8 EX0        ;
__sbit __at 0xA9 ET0        ;
__sbit __at 0xAA EX1        ;
__sbit __at 0xAB ET1        ;
__sbit __at 0xAC ES         ;
__sbit __at 0xAD ET2        ;
__sbit __at 0xAE WDT        ; /* watchdog timer reset - SAB80515 specific */
__sbit __at 0xAF EA         ;

__sbit __at 0xAF EAL        ; /* EA as called by Siemens */

/* P3 */
__sbit __at 0xB0 P3_0       ;
__sbit __at 0xB1 P3_1       ;
__sbit __at 0xB2 P3_2       ;
__sbit __at 0xB3 P3_3       ;
__sbit __at 0xB4 P3_4       ;
__sbit __at 0xB5 P3_5       ;
__sbit __at 0xB6 P3_6       ;
__sbit __at 0xB7 P3_7       ;

__sbit __at 0xB0 RXD        ;
__sbit __at 0xB1 TXD        ;
__sbit __at 0xB2 INT0       ;
__sbit __at 0xB3 INT1       ;
__sbit __at 0xB4 T0         ;
__sbit __at 0xB5 T1         ;
__sbit __at 0xB6 WR         ;
__sbit __at 0xB7 RD         ;

/* IEN1 */
__sbit __at 0xB8 EADC       ; /* A/D converter interrupt enable */
__sbit __at 0xB9 EX2        ;
__sbit __at 0xBA EX3        ;
__sbit __at 0xBB EX4        ;
__sbit __at 0xBC EX5        ;
__sbit __at 0xBD EX6        ;
__sbit __at 0xBE SWDT       ; /* watchdog timer start/reset */
__sbit __at 0xBF EXEN2      ; /* timer2 external reload interrupt enable */

/* IRCON */
__sbit __at 0xC0 IADC       ; /* A/D converter irq flag */
__sbit __at 0xC1 IEX2       ; /* external interrupt edge detect flag */
__sbit __at 0xC2 IEX3       ;
__sbit __at 0xC3 IEX4       ;
__sbit __at 0xC4 IEX5       ;
__sbit __at 0xC5 IEX6       ;
__sbit __at 0xC6 TF2        ; /* timer 2 owerflow flag  */
__sbit __at 0xC7 EXF2       ; /* timer2 reload flag */

/* T2CON */
__sbit __at 0xC8 T2CON_0    ;
__sbit __at 0xC9 T2CON_1    ;
__sbit __at 0xCA T2CON_2    ;
__sbit __at 0xCB T2CON_3    ;
__sbit __at 0xCC T2CON_4    ;
__sbit __at 0xCD T2CON_5    ;
__sbit __at 0xCE T2CON_6    ;
__sbit __at 0xCF T2CON_7    ;

__sbit __at 0xC8 T2I0       ;
__sbit __at 0xC9 T2I1       ;
__sbit __at 0xCA T2CM       ;
__sbit __at 0xCB T2R0       ;
__sbit __at 0xCC T2R1       ;
__sbit __at 0xCD I2FR       ;
__sbit __at 0xCE I3FR       ;
__sbit __at 0xCF T2PS       ;


/* PSW */
__sbit __at 0xD0 P          ;
__sbit __at 0xD1 FL         ;
__sbit __at 0xD2 OV         ;
__sbit __at 0xD3 RS0        ;
__sbit __at 0xD4 RS1        ;
__sbit __at 0xD5 F0         ;
__sbit __at 0xD6 AC         ;
__sbit __at 0xD7 CY         ;

__sbit __at 0xD1 F1         ;

/* ADCON */
__sbit __at 0xD8 MX0        ;
__sbit __at 0xD9 MX1        ;
__sbit __at 0xDA MX2        ;
__sbit __at 0xDB ADM        ;
__sbit __at 0xDC BSY        ;

__sbit __at 0xDE CLK        ;
__sbit __at 0xDF BD         ;

/* A */
__sbit __at 0xA0 AREG_F0    ;
__sbit __at 0xA1 AREG_F1    ;
__sbit __at 0xA2 AREG_F2    ;
__sbit __at 0xA3 AREG_F3    ;
__sbit __at 0xA4 AREG_F4    ;
__sbit __at 0xA5 AREG_F5    ;
__sbit __at 0xA6 AREG_F6    ;
__sbit __at 0xA7 AREG_F7    ;

/* P4 */
__sbit __at 0xE8 P4_0       ;
__sbit __at 0xE9 P4_1       ;
__sbit __at 0xEA P4_2       ;
__sbit __at 0xEB P4_3       ;
__sbit __at 0xEC P4_4       ;
__sbit __at 0xED P4_5       ;
__sbit __at 0xEE P4_6       ;
__sbit __at 0xEF P4_7       ;

/* B */
__sbit __at 0xF0 BREG_F0    ;
__sbit __at 0xF1 BREG_F1    ;
__sbit __at 0xF2 BREG_F2    ;
__sbit __at 0xF3 BREG_F3    ;
__sbit __at 0xF4 BREG_F4    ;
__sbit __at 0xF5 BREG_F5    ;
__sbit __at 0xF6 BREG_F6    ;
__sbit __at 0xF7 BREG_F7    ;

/* P5 */
__sbit __at 0xF8 P5_0       ;
__sbit __at 0xF9 P5_1       ;
__sbit __at 0xFA P5_2       ;
__sbit __at 0xFB P5_3       ;
__sbit __at 0xFC P5_4       ;
__sbit __at 0xFD P5_5       ;
__sbit __at 0xFE P5_6       ;
__sbit __at 0xFF P5_7       ;

/* BIT definitions for bits that are not directly accessible */
/* PCON bits */
#define IDL             0x01
#define PD              0x02
#define GF0             0x04
#define GF1             0x08
#define SMOD            0x80

#define IDL_            0x01
#define PD_             0x02
#define GF0_            0x04
#define GF1_            0x08
#define SMOD_           0x80

/* TMOD bits */
#define M0_0            0x01
#define M1_0            0x02
#define C_T0            0x04
#define GATE0           0x08
#define M0_1            0x10
#define M1_1            0x20
#define C_T1            0x40
#define GATE1           0x80

#define M0_0_           0x01
#define M1_0_           0x02
#define C_T0_           0x04
#define GATE0_          0x08
#define M0_1_           0x10
#define M1_1_           0x20
#define C_T1_           0x40
#define GATE1_          0x80

#define T0_M0           0x01
#define T0_M1           0x02
#define T0_CT           0x04
#define T0_GATE         0x08
#define T1_M0           0x10
#define T1_M1           0x20
#define T1_CT           0x40
#define T1_GATE         0x80

#define T0_M0_          0x01
#define T0_M1_          0x02
#define T0_CT_          0x04
#define T0_GATE_        0x08
#define T1_M0_          0x10
#define T1_M1_          0x20
#define T1_CT_          0x40
#define T1_GATE_        0x80

#define T0_MASK         0x0F
#define T1_MASK         0xF0

#define T0_MASK_        0x0F
#define T1_MASK_        0xF0

/* T2MOD bits */
#define DCEN            0x01
#define T2OE            0x02

#define DCEN_           0x01
#define T2OE_           0x02

/* WMCON bits */
#define WMCON_WDTEN     0x01
#define WMCON_WDTRST    0x02
#define WMCON_DPS       0x04
#define WMCON_EEMEN     0x08
#define WMCON_EEMWE     0x10
#define WMCON_PS0       0x20
#define WMCON_PS1       0x40
#define WMCON_PS2       0x80

/* SPCR-SPI bits */
#define SPCR_SPR0       0x01
#define SPCR_SPR1       0x02
#define SPCR_CPHA       0x04
#define SPCR_CPOL       0x08
#define SPCR_MSTR       0x10
#define SPCR_DORD       0x20
#define SPCR_SPE        0x40
#define SPCR_SPIE       0x80

/* SPSR-SPI bits */
#define SPSR_WCOL       0x40
#define SPSR_SPIF       0x80

/* SPDR-SPI bits */
#define SPDR_SPD0       0x10
#define SPDR_SPD1       0x20
#define SPDR_SPD2       0x40
#define SPDR_SPD3       0x80
#define SPDR_SPD4       0x10
#define SPDR_SPD5       0x20
#define SPDR_SPD6       0x40
#define SPDR_SPD7       0x80

/* Interrupt numbers: address = (number * 8) + 3 */
#define IE0_VECTOR      0       /* 0x03 external interrupt 0 */
#define TF0_VECTOR      1       /* 0x0b timer 0 */
#define IE1_VECTOR      2       /* 0x13 external interrupt 1 */
#define TF1_VECTOR      3       /* 0x1b timer 1 */
#define SI0_VECTOR      4       /* 0x23 serial port 0 */
#define TF2_VECTOR      5       /* 0x2B timer 2 */
#define EX2_VECTOR      5       /* 0x2B external interrupt 2 */

#define IADC_VECTOR     8       /* 0x43 A/D converter interrupt */
#define IEX2_VECTOR     9       /* 0x4B external interrupt 2 */
#define IEX3_VECTOR    10       /* 0x53 external interrupt 3 */
#define IEX4_VECTOR    11       /* 0x5B external interrupt 4 */
#define IEX5_VECTOR    12       /* 0x63 external interrupt 5 */
#define IEX6_VECTOR    13       /* 0x6B external interrupt 6 */

#endif

