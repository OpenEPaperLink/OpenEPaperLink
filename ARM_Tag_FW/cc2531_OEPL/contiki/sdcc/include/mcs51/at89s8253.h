/*-------------------------------------------------------------------------
   at89s8253.h - register Declarations for ATMEL 89S8253 Processors

   Copyright (C) 2006, Krzysztof Polomka <del_p AT op.pl>
     based on at89S8252.h By - Dipl.-Ing. (FH) Michael Schmitt

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

#ifndef AT89S8253_H
#define AT89S8253_H

/* BYTE addressable registers */
__sfr __at (0x80) P0          ;
__sfr __at (0x81) SP          ;
__sfr __at (0x82) DPL         ;
__sfr __at (0x82) DP0L        ; /* as called by Atmel */
__sfr __at (0x83) DPH         ;
__sfr __at (0x83) DP0H        ; /* as called by Atmel */
__sfr __at (0x84) DP1L        ; /* at89S8253 specific register */
__sfr __at (0x85) DP1H        ; /* at89S8253 specific register */
__sfr __at (0x86) SPDR        ; /* at89S8253 specific register */
__sfr __at (0x87) PCON        ;
__sfr __at (0x88) TCON        ;
__sfr __at (0x89) TMOD        ;
__sfr __at (0x8A) TL0         ;
__sfr __at (0x8B) TL1         ;
__sfr __at (0x8C) TH0         ;
__sfr __at (0x8D) TH1         ;
__sfr __at (0x8E) AUXR        ; /* at89S8253 specific register */
__sfr __at (0x8F) CLKREG      ; /* at89S8253 specific register */
__sfr __at (0x90) P1          ;
__sfr __at (0x96) EECON       ; /* at89S8253 specific register */
__sfr __at (0x98) SCON        ;
__sfr __at (0x99) SBUF        ;
__sfr __at (0xA0) P2          ;
__sfr __at (0xA6) WDTRST      ; /* at89S8253 specific register */
__sfr __at (0xA7) WDTCON      ; /* at89S8253 specific register */
__sfr __at (0xA8) IE          ;
__sfr __at (0xA9) SADDR       ; /* at89S8253 specific register */
__sfr __at (0xAA) SPSR        ; /* at89S8253 specific register */
__sfr __at (0xB0) P3          ;
__sfr __at (0xB7) IPH         ; /* at89S8253 specific register */
__sfr __at (0xB8) IP          ;
__sfr __at (0xB9) SADEN       ; /* at89S8253 specific register */
__sfr __at (0xC8) T2CON       ;
__sfr __at (0xC9) T2MOD       ;
__sfr __at (0xCA) RCAP2L      ;
__sfr __at (0xCB) RCAP2H      ;
__sfr __at (0xCC) TL2         ;
__sfr __at (0xCD) TH2         ;
__sfr __at (0xD0) PSW         ;
__sfr __at (0xD5) SPCR        ; /* at89S8253 specific register */
__sfr __at (0xE0) ACC         ;
__sfr __at (0xE0) A           ;
__sfr __at (0xF0) B           ;


/* BIT addressable registers */
/* P0 */
__sbit __at (0x80) P0_0       ;
__sbit __at (0x81) P0_1       ;
__sbit __at (0x82) P0_2       ;
__sbit __at (0x83) P0_3       ;
__sbit __at (0x84) P0_4       ;
__sbit __at (0x85) P0_5       ;
__sbit __at (0x86) P0_6       ;
__sbit __at (0x87) P0_7       ;

/* TCON */
__sbit __at (0x88) IT0        ;
__sbit __at (0x89) IE0        ;
__sbit __at (0x8A) IT1        ;
__sbit __at (0x8B) IE1        ;
__sbit __at (0x8C) TR0        ;
__sbit __at (0x8D) TF0        ;
__sbit __at (0x8E) TR1        ;
__sbit __at (0x8F) TF1        ;

/* P1 */
__sbit __at (0x90) P1_0       ;
__sbit __at (0x91) P1_1       ;
__sbit __at (0x92) P1_2       ;
__sbit __at (0x93) P1_3       ;
__sbit __at (0x94) P1_4       ;
__sbit __at (0x95) P1_5       ;
__sbit __at (0x96) P1_6       ;
__sbit __at (0x97) P1_7       ;

__sbit __at (0x90) T2         ;
__sbit __at (0x91) T2EX       ;

/* P1 SPI portpins */
__sbit __at (0x94) SS         ; /* SPI: SS - Slave port select input */
__sbit __at (0x95) MOSI       ; /* SPI: MOSI - Master data output, slave data input */
__sbit __at (0x96) MISO       ; /* SPI: MISO - Master data input, slave data output */
__sbit __at (0x97) SCK        ; /* SPI: SCK - Master clock output, slave clock input */


/* SCON */
__sbit __at (0x98) RI         ;
__sbit __at (0x99) TI         ;
__sbit __at (0x9A) RB8        ;
__sbit __at (0x9B) TB8        ;
__sbit __at (0x9C) REN        ;
__sbit __at (0x9D) SM2        ;
__sbit __at (0x9E) SM1        ;
__sbit __at (0x9F) SM0        ;

/* P2 */
__sbit __at (0xA0) P2_0       ;
__sbit __at (0xA1) P2_1       ;
__sbit __at (0xA2) P2_2       ;
__sbit __at (0xA3) P2_3       ;
__sbit __at (0xA4) P2_4       ;
__sbit __at (0xA5) P2_5       ;
__sbit __at (0xA6) P2_6       ;
__sbit __at (0xA7) P2_7       ;

/* IE */
__sbit __at (0xA8) EX0        ;
__sbit __at (0xA9) ET0        ;
__sbit __at (0xAA) EX1        ;
__sbit __at (0xAB) ET1        ;
__sbit __at (0xAC) ES         ;
__sbit __at (0xAD) ET2        ;
__sbit __at (0xAF) EA         ;

/* P3 */
__sbit __at (0xB0) P3_0       ;
__sbit __at (0xB1) P3_1       ;
__sbit __at (0xB2) P3_2       ;
__sbit __at (0xB3) P3_3       ;
__sbit __at (0xB4) P3_4       ;
__sbit __at (0xB5) P3_5       ;
__sbit __at (0xB6) P3_6       ;
__sbit __at (0xB7) P3_7       ;

__sbit __at (0xB0) RXD        ;
__sbit __at (0xB1) TXD        ;
__sbit __at (0xB2) INT0       ;
__sbit __at (0xB3) INT1       ;
__sbit __at (0xB4) T0         ;
__sbit __at (0xB5) T1         ;
__sbit __at (0xB6) WR         ;
__sbit __at (0xB7) RD         ;

/* IP */
__sbit __at (0xB8) PX0        ;
__sbit __at (0xB9) PT0        ;
__sbit __at (0xBA) PX1        ;
__sbit __at (0xBB) PT1        ;
__sbit __at (0xBC) PS         ;
__sbit __at (0xBD) PT2        ;

/* T2CON */
__sbit __at (0xC8) T2CON_0    ;
__sbit __at (0xC9) T2CON_1    ;
__sbit __at (0xCA) T2CON_2    ;
__sbit __at (0xCB) T2CON_3    ;
__sbit __at (0xCC) T2CON_4    ;
__sbit __at (0xCD) T2CON_5    ;
__sbit __at (0xCE) T2CON_6    ;
__sbit __at (0xCF) T2CON_7    ;

__sbit __at (0xC8) CP_RL2     ;
__sbit __at (0xC9) C_T2       ;
__sbit __at (0xCA) TR2        ;
__sbit __at (0xCB) EXEN2      ;
__sbit __at (0xCC) TCLK       ;
__sbit __at (0xCD) RCLK       ;
__sbit __at (0xCE) EXF2       ;
__sbit __at (0xCF) TF2        ;

/* PSW */
__sbit __at (0xD0) P          ;
__sbit __at (0xD1) FL         ;
__sbit __at (0xD2) OV         ;
__sbit __at (0xD3) RS0        ;
__sbit __at (0xD4) RS1        ;
__sbit __at (0xD5) F0         ;
__sbit __at (0xD6) AC         ;
__sbit __at (0xD7) CY         ;

/* B */
__sbit __at (0xF0) BREG_F0    ;
__sbit __at (0xF1) BREG_F1    ;
__sbit __at (0xF2) BREG_F2    ;
__sbit __at (0xF3) BREG_F3    ;
__sbit __at (0xF4) BREG_F4    ;
__sbit __at (0xF5) BREG_F5    ;
__sbit __at (0xF6) BREG_F6    ;
__sbit __at (0xF7) BREG_F7    ;


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

/* EECON bits */
#define EECON_WRTINH    0x01
#define EECON_RDY       0x02
#define EECON_DPS       0x04
#define EECON_EEMEN     0x08
#define EECON_EEMWE     0x10
#define EECON_EELD      0x20

/* WDTCON bits */
#define WDTCON_WDTEN    0x01
#define WDTCON_WSWRST   0x02
#define WDTCON_HWDT     0x04
#define WDTCON_DISRTO   0x08
#define WDTCON_WDIDLE   0x10
#define WDTCON_PS0      0x20
#define WDTCON_PS1      0x40
#define WDTCON_PS2      0x80

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
#define SPSR_ENH        0x01
#define SPSR_DISSO      0x02
#define SPSR_LDEN       0x20
#define SPSR_WCOL       0x40
#define SPSR_SPIF       0x80

/* SPDR-SPI bits */
#define SPDR_SPD0       0x01
#define SPDR_SPD1       0x02
#define SPDR_SPD2       0x04
#define SPDR_SPD3       0x08
#define SPDR_SPD4       0x10
#define SPDR_SPD5       0x20
#define SPDR_SPD6       0x40
#define SPDR_SPD7       0x80

/* IPH bits */
#define IPH_PX0H        0x01
#define IPH_PT0H        0x02
#define IPH_PX1H        0x04
#define IPH_PT1H        0x08
#define IPH_PSH         0x10
#define IPH_PT2H        0x20

/* Interrupt numbers: address = (number * 8) + 3 */
#define IE0_VECTOR      0       /* 0x03 external interrupt 0 */
#define EX0_VECTOR      0       /* 0x03 external interrupt 0 */
#define TF0_VECTOR      1       /* 0x0b timer 0 */
#define IE1_VECTOR      2       /* 0x13 external interrupt 1 */
#define EX1_VECTOR      2       /* 0x13 external interrupt 1 */
#define TF1_VECTOR      3       /* 0x1b timer 1 */
#define SI0_VECTOR      4       /* 0x23 serial port 0 */
#define TF2_VECTOR      5       /* 0x2B timer 2 */
#define EX2_VECTOR      5       /* 0x2B external interrupt 2 */

/* AUXR bits */
#define AUXR_DISALE          0x01
#define AUXR_INTEL_PWD_EXIT  0x02

/* CLKREG bits */
#define CLKREG_X2         0x01

/* This is one of the addons coming from Bernd Krueger-Knauber   */

/* ALE (0x8E) Bit Values */
__sfr __at (0x8E) ALE;      /* at89S8252 specific register */

/* Macro to enable and disable the toggling of the ALE-pin (EMV) */

/* Explanation : Original Intel 8051 Cores (Atmel has to use the */
/* Intel Core) have a feature that ALE is only active during     */
/* MOVX or MOVC instruction. Otherwise the ALE-Pin is weakly     */
/* pulled high. This can be used to force some external devices  */
/* into standby mode and reduced EMI noise                       */

#define ALE_OFF  ALE = ALE | 0x01
#define ALE_ON   ALE = ALE & 0xFE

#endif
