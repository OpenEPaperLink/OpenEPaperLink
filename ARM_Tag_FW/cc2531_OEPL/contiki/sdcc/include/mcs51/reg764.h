/*-------------------------------------------------------------------------
   reg764 - register Declarations for 87C764

   Copyright (C) 2005, Robert Lacoste <robert_lacoste AT yahoo.fr>
     based upon reg51.h written by Sandeep Dutta
     Registers are taken from the Phillips Semiconductor

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

#ifndef REGC764_H
#define REGC764_H

/*  Special Function Registers  */

__sfr __at 0x80 P0    ;   // Port 0
__sfr __at 0x81 SP    ;   // Stack Pointer
__sfr __at 0x82 DPL   ;   // Data Pointer Low
__sfr __at 0x83 DPH   ;   // Data Pointer High
__sfr __at 0x84 P0M1  ;   // Port 0 output mode 1
__sfr __at 0x85 P0M2  ;   // Port 0 output mode 2
__sfr __at 0x86 KBI   ;   // Keyboard interrupt
__sfr __at 0x87 PCON  ;   // Power Control
__sfr __at 0x88 TCON  ;   // Timer Control
__sfr __at 0x89 TMOD  ;   // Timer Mode
__sfr __at 0x8A TL0   ;   // Timer Low 0
__sfr __at 0x8B TL1   ;   // Timer Low 1
__sfr __at 0x8C TH0   ;   // Timer High 0
__sfr __at 0x8D TH1   ;   // Timer High 1

__sfr __at 0x90 P1    ;   // Port 1
__sfr __at 0x91 P1M1  ;   // Port 1 output mode 1
__sfr __at 0x92 P1M2  ;   // Port 1 output mode 2
__sfr __at 0x95 DIVM  ;   // CPU clock divide by N control
__sfr __at 0x98 SCON  ;   // Serial Control
__sfr __at 0x99 SBUF  ;   // Serial Data Buffer

__sfr __at 0xA0 P2    ;   // Port 2
__sfr __at 0xA2 AUXR1 ;   // Auxilliary 1 (not available on 80C51FA/87C51Fx)
__sfr __at 0xA4 P2M1  ;   // Port 2 output mode 1
__sfr __at 0xA5 P2M2  ;   // Port 2 output mode 2
__sfr __at 0xA6 WDRST ;   // Watchdog reset register
__sfr __at 0xA7 WDCON ;   // Watchdog control register
__sfr __at 0xA8 IEN0  ;   // Interrupt Enable 0
__sfr __at 0xA9 SADDR ;   // Serial slave Address
__sfr __at 0xAC CMP1  ;   // Comparator 1 control register
__sfr __at 0xAD CMP2  ;   // Comparator 2 control register

__sfr __at 0xB7 IP0H  ;   // Interrupt Priority 0 High 
__sfr __at 0xB8 IP0   ;   // Interrupt Priority 0
__sfr __at 0xB9 SADEN ;   // Serial slave Address Mask

__sfr __at 0xC8 I2CFG ;   // I2C configuration register

__sfr __at 0xD0 PSW   ;   // Program Status Word
__sfr __at 0xD8 I2CON ;   // I2C control register
__sfr __at 0xD9 I2DAT ;   // I2C data register

__sfr __at 0xE0 ACC   ;   // Accumulator
__sfr __at 0xE8 IEN1  ;   // Interrupt enable 1

__sfr __at 0xF0 B     ;   // B Register
__sfr __at 0xF6 PT0AD ;   // Port 0 digital input disable
__sfr __at 0xF7 IP1H  ;   // Interrupt Priority 1 High 
__sfr __at 0xF8 IP1   ;   // Interrupt Priority 1


/*  Bit Addressable Registers  */

/*  P0    */
__sbit __at 0x80 P0_0 ; // Also CMP2
__sbit __at 0x81 P0_1 ; // Also CIN2B
__sbit __at 0x82 P0_2 ; // Also CIN2A
__sbit __at 0x83 P0_3 ; // Also CIN1B
__sbit __at 0x84 P0_4 ; // Also CIN1A
__sbit __at 0x85 P0_5 ; // Also CMPREF
__sbit __at 0x86 P0_6 ; // Also CMP1
__sbit __at 0x87 P0_7 ; // Also T1

/*  TCON  */
__sbit __at 0x88 IT0  ; // External Interrupt 0 Type
__sbit __at 0x89 IE0  ; // External Interrupt 0 Edge Flag
__sbit __at 0x8A IT1  ; // External Interrupt 1 Type
__sbit __at 0x8B IE1  ; // External Interrupt 1 Edge Flag
__sbit __at 0x8C TR0  ; // Timer 0 Run Control
__sbit __at 0x8D TF0  ; // Timer 0 Overflow Flag
__sbit __at 0x8E TR1  ; // Timer 1 Run Control
__sbit __at 0x8F TF1  ; // Timer 1 Overflow Flag

/*  P1 */
__sbit __at 0x90 P1_0 ; // Also TxD
__sbit __at 0x91 P1_1 ; // Also RxD
__sbit __at 0x92 P1_2 ; // Also T0
__sbit __at 0x93 P1_3 ; // Also INT0
__sbit __at 0x94 P1_4 ; // Also INT1
__sbit __at 0x95 P1_5 ; // Also RST
__sbit __at 0x96 P1_6 ;
__sbit __at 0x97 P1_7 ;

/*  SCON  */
__sbit __at 0x98 RI   ; // Receive Interrupt Flag
__sbit __at 0x99 TI   ; // Transmit Interrupt Flag
__sbit __at 0x9A RB8  ; // Receive Bit 8
__sbit __at 0x9B TB8  ; // Transmit Bit 8
__sbit __at 0x9C REN  ; // Receiver Enable
__sbit __at 0x9D SM2  ; // Serial Mode Control Bit 2
__sbit __at 0x9E SM1  ; // Serial Mode Control Bit 1
__sbit __at 0x9F SM0  ; // Serial Mode Control Bit 0

/*  P2    */
__sbit __at 0xA0 P2_0 ; // Also X2
__sbit __at 0xA1 P2_1 ; // Also X1

/*  IEN0 */
__sbit __at 0xA8 EX0  ; // External Interrupt 0 Enable
__sbit __at 0xA9 ET0  ; // Timer 0 Interrupt Enable
__sbit __at 0xAA EX1  ; // External Interrupt 1 Enable
__sbit __at 0xAB ET1  ; // Timer 1 Interrupt Enable
__sbit __at 0xAC ES   ; // Serial Port Interrupt Enable
__sbit __at 0xAD EBO  ; // Brownout Interrupt Enable
__sbit __at 0xAE EWD  ; // Watchdog Interrupt Enable
__sbit __at 0xAF EA   ; // Global Interrupt Enable

/*  IP0   */ 
__sbit __at 0xB8 PX0  ; // External Interrupt 0 Priority
__sbit __at 0xB9 PT0  ; // Timer 0 Interrupt Priority
__sbit __at 0xBA PX1  ; // External Interrupt 1 Priority
__sbit __at 0xBB PT1  ; // Timer 1 Interrupt Priority
__sbit __at 0xBC PS   ; // Serial Port Interrupt Priority
__sbit __at 0xBD PBO  ; // Brownout Interrupt Priority
__sbit __at 0xBE PWD  ; // Watchdog Interrupt Priority

/*  I2CFG */
__sbit __at 0xC8 CT0   ; // Clock Time Select 0
__sbit __at 0xC9 CT1   ; // Clock Time Select 1
__sbit __at 0xCC TIRUN ; // Timer I Run Enable
__sbit __at 0xCD CLRTI ; // Clear Timer I
__sbit __at 0xCE MASTRQ; // Master Request
__sbit __at 0xCF SLAVEN; // Slave Enable

/*  PSW   */
__sbit __at 0xD0 P    ; // Accumulator Parity Flag
__sbit __at 0xD1 F1   ; // Flag 1
__sbit __at 0xD2 OV   ; // Overflow Flag
__sbit __at 0xD3 RS0  ; // Register Bank Select 0
__sbit __at 0xD4 RS1  ; // Register Bank Select 1
__sbit __at 0xD5 F0   ; // Flag 0
__sbit __at 0xD6 AC   ; // Auxiliary Carry Flag
__sbit __at 0xD7 CY   ; // Carry Flag

/*  I2CON */
__sbit __at 0xD8 XSTP ;
__sbit __at 0xD9 MASTER;// Master Status
__sbit __at 0xDA STP  ; // Stop Detect Flag
__sbit __at 0xDB STR  ; // Start Detect Flag
__sbit __at 0xDC ARL  ; // Arbitration Loss Flag
__sbit __at 0xDD DRDY ; // Data Ready Flag
__sbit __at 0xDE ATN  ; // Attention: I2C Interrupt Flag
__sbit __at 0xDF RDAT ; // I2C Read Data

/*  ACC   */
__sbit __at 0xE0 ACC_0;
__sbit __at 0xE1 ACC_1;
__sbit __at 0xE2 ACC_2;
__sbit __at 0xE3 ACC_3;
__sbit __at 0xE4 ACC_4;
__sbit __at 0xE5 ACC_5;
__sbit __at 0xE6 ACC_6;
__sbit __at 0xE7 ACC_7;

/*  IEN1  */
__sbit __at 0xE8 EI2  ; // I2C Interrupt Enable
__sbit __at 0xE9 EKB  ; // Keyboard Interrupt Enable
__sbit __at 0xEA EC2  ; // Comparator 2 Interrupt Enable
__sbit __at 0xED EC1  ; // Comparator 1 Interrupt Enable
__sbit __at 0xEF ETI  ; // Timer I Interrupt Enable

/*  B     */
__sbit __at 0xF0 B_0;
__sbit __at 0xF1 B_1;
__sbit __at 0xF2 B_2;
__sbit __at 0xF3 B_3;
__sbit __at 0xF4 B_4;
__sbit __at 0xF5 B_5;
__sbit __at 0xF6 B_6;
__sbit __at 0xF7 B_7;

/*  IP1  */ 
__sbit __at 0xF8 PI2; // I2C Interrupt Priority
__sbit __at 0xF9 PKB; // Keyboard Interrupt Priority
__sbit __at 0xFA PC2; // Comparator 2 Interrupt Priority
__sbit __at 0xFD PC1; // Comparator 1 Interrupt Priority
__sbit __at 0xFF PTI; // Timer I Interrupt Priority

/* Bitmasks for SFRs */

/* AUXR1 bits  */
#define DPS     0x01
#define SRST    0x08
#define LPEP    0x10
#define BOI     0x20
#define BOD     0x40
#define KBF     0x80

/* CMP1 bits   */
#define CMF1    0x01
#define CO1     0x02
#define OE1     0x04
#define CN1     0x08
#define CP1     0x10
#define CE1     0x20

/* CMP2 bits   */
#define CMF2    0x01
#define CO2     0x02
#define OE2     0x04
#define CN2     0x08
#define CP2     0x10
#define CE2     0x20

/* I2DAT bits  */
#define RDAT    0x80
#define XDAT    0x80

/* IP1H bits   */
#define PI2H    0x01
#define PKBH    0x02
#define PC2H    0x04
#define PC1H    0x20
#define PTIH    0x80

/* PCON bits   */
#define IDL     0x01
#define PD      0x02
#define GF0     0x04
#define GF1     0x08
#define POF     0x10
#define BOF     0x20
#define SMOD0   0x40
#define SMOD1   0x80

/* P2M1 bits   */
#define ENT0    0x04
#define ENT1    0x08
#define ENTCLK  0x10
#define P0S     0x20
#define P1S     0x40
#define P2S     0x80

/* TMOD bits */
#define M0_0    0x01
#define M1_0    0x02
#define C_T0    0x04
#define GATE0   0x08
#define M0_1    0x10
#define M1_1    0x20
#define C_T1    0x40
#define GATE1   0x80

/* WDCON bits */
#define WDS0    0x01
#define WDS1    0x02
#define WDS2    0x04
#define WDCLK   0x08
#define WDRUN   0x10
#define WDOVF   0x20


/*  Masks for I2CFG bits */
#define BTIR    0x10       // Mask for TIRUN bit.
#define BMRQ    0x40       // Mask for MASTRQ bit.
#define BSLV    0x80       // Mask for SLAVEN bit.


/* Masks for I2CON bits */
#define BCXA    0x80       // Mask for CXA bit.
#define BIDLE   0x40       // Mask for IDLE bit.
#define BCDR    0x20       // Mask for CDR bit.
#define BCARL   0x10       // Mask for CARL bit.
#define BCSTR   0x08       // Mask for CSTR bit.
#define BCSTP   0x04       // Mask for CSTP bit.
#define BXSTR   0x02       // Mask for XSTR bit.
#define BXSTP   0x01       // Mask for XSTP bit.


#endif
