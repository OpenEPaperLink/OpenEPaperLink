/*---------------------------------------------------------------------------
   regc515c.h - This file contains definitions for the builtin CAN-Bus
   Controller of the Siemens c515c controller

   Copyright (C) 2005, Maarten Brock, sourceforge.brock@dse.nl

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

#ifndef _REGC515C_H
#define _REGC515C_H

/* define CPU_CLK_10MHZ or CPU_CLK_8MHZ to select the right values for */
/* the bit timing registers */

#define CPU_CLK_10MHZ

/* address of can controller in xmem */
#define CAN_CTRL	0xf700

/* size of message buffer including 1 dummy byte at end */
#define CAN_MSG_SZ	0x10

/* register offset definitions */
#define CR	0
#define SR	1
#define IR	2

#define BT_0	4
#define BT_1	5
#define GMS_0	6
#define GMS_1	7
#define GME_0	8
#define GME_1	9
#define GME_2	0xa
#define GME_3	0xb
#define MSG15MSK_0	0xc
#define MSG15MSK_1	0xd
#define MSG15MSK_2	0xe
#define MSG15MSK_3	0xf

/* register offsets  in message buffer */
#define MCR_0	0
#define MCR_1	1
#define ARB_0	2
#define ARB_1	3
#define ARB_2	4
#define ARB_3	5
#define MCFG	6
/* beginning of message data */
#define DATA	7

/* bits in cntr_x registers */
#define MSGVAL	0x80
#define TXIE	0x20
#define RXIE	0x8
#define INTPND	0x2
#define RMTPND	0x80
#define TXRQST	0x20
#define MSGLST	0x8
#define CPUUPD	0x8
#define NEWDAT	0x2

/* macros for setting and resetting above bits, see Siemens documentation */
#define MCR_BIT_SET(p,x) ((p) = (0xff & ~((x) >> 1)))
#define MCR_BIT_RES(p,x) ((p) = (0xff & ~(x)))

/* direction = transmit in mcfg */
#define DIR_TRANSMIT	0x8

/* constants for bit timing registers */
/* 8 MHZ */
#ifdef CPU_CLK_8MHZ
#define BT_0_125K	0x3
#define BT_1_125K	0x1c
#define BT_0_250K	0x1
#define BT_1_250K	0x1c
#define BT_0_500K	0x0
#define BT_1_500K	0x1c
#define BT_0_1M		0x0
#define BT_1_1M		0x14
#endif
/* dito, 10 MHZ */
#ifdef CPU_CLK_10MHZ
#define BT_0_125K	0x3
#define BT_1_125K	0x1c
#define BT_0_250K	0x1
#define BT_1_250K	0x1c
#define BT_0_500K	0x0
#define BT_1_500K	0x2f
#define BT_0_1M		0x0
#define BT_1_1M		0x25
#endif

/* Control register bits */

#define	CINIT	0x1
#define IE	0x2
#define SIE	0x4
#define EIE	0x8

#define CCE	0x40

/* status register bits */
#define	LEC0	0x1
#define	LEC1	0x2
#define	LEC2	0x4
#define	TXOK	0x8
#define	RXOK	0x10
#define	WAKE	0x20
#define	WARN	0x40
#define	BOFF	0x80


typedef struct can_msg
{
    unsigned char mcr_0;
    unsigned char mcr_1;
    unsigned char arb_0;
    unsigned char arb_1;
    unsigned char arb_2;
    unsigned char arb_3;
    unsigned char mcfg;
    unsigned char data_bytes[8];
    unsigned char dummy;
} *can_msgp;

__xdata __at CAN_CTRL struct
{
    unsigned	char	cr;
    unsigned	char	sr;
    unsigned	char	ir;
    unsigned	char	res0;
    unsigned	char	bt_0;
    unsigned	char	bt_1;
    unsigned	char	gms_0;
    unsigned	char	gms_1;
    unsigned	char	gme_0;
    unsigned	char	gme_1;
    unsigned	char	gme_2;
    unsigned	char	gme_3;
    unsigned	char	msg15msk_0;
    unsigned	char	msg15msk_1;
    unsigned	char	msg15msk_2;
    unsigned	char	msg15msk_3;
    struct	can_msg msgbufs[15];
} can_ctrl;

/* Byte registers in numerical order */

__sfr __at 0x80 P0;
__sfr __at 0x81 SP;
__sfr __at 0x82 DPL;
__sfr __at 0x83 DPH;
__sfr __at 0x86 WDTREL;
__sfr __at 0x87 PCON;
__sfr __at 0x88 TCON;
__sfr __at 0x88 PCON1;
__sfr __at 0x89 TMOD;
__sfr __at 0x8A TL0;
__sfr __at 0x8B TL1;
__sfr __at 0x8C TH0;
__sfr __at 0x8D TH1;
__sfr __at 0x90 P1;
__sfr __at 0x91 XPAGE;
__sfr __at 0x92 DPSEL;
__sfr __at 0x93 SSCCON;
__sfr __at 0x94 STB;
__sfr __at 0x95 SRB;
__sfr __at 0x96 SSCMOD;
__sfr __at 0x98 SCON;
__sfr __at 0x99 SBUF;
__sfr __at 0x9A IEN2;
__sfr __at 0xA0 P2;
__sfr __at 0xA8 IEN0;
__sfr __at 0xA9 IP0;
__sfr __at 0xAA SRELL;
__sfr __at 0xAB SCF;
__sfr __at 0xAC SCIEN;
__sfr __at 0xB0 P3;
__sfr __at 0xB1 SYSCON;
__sfr __at 0xB8 IEN1;
__sfr __at 0xB9 IP1;
__sfr __at 0xBA SRELH;
__sfr __at 0xC0 IRCON;
__sfr __at 0xC1 CCEN;
__sfr __at 0xC2 CCL1;
__sfr __at 0xC3 CCH1;
__sfr __at 0xC4 CCL2;
__sfr __at 0xC5 CCH2;
__sfr __at 0xC6 CCL3;
__sfr __at 0xC7 CCH3;
__sfr __at 0xC8 T2CON;
__sfr __at 0xCA CRCL;
__sfr __at 0xCB CRCH;
__sfr __at 0xCC TL2;
__sfr __at 0xCD TH2;
__sfr __at 0xD0 PSW;
__sfr __at 0xD8 ADCON0;
__sfr __at 0xD9 ADDATH;
__sfr __at 0xDA ADDATL;
__sfr __at 0xDB P6;
__sfr __at 0xDC ADCON1;
__sfr __at 0xE0 ACC;
__sfr __at 0xE8 P4;
__sfr __at 0xF0 B;
__sfr __at 0xF8 P5;
__sfr __at 0xF8 DIR5;
__sfr __at 0xFA P7;


/* defining bits in SFR P0 */
__sbit __at 0x80  P0_0;
__sbit __at 0x81  P0_1;
__sbit __at 0x82  P0_2;
__sbit __at 0x83  P0_3;
__sbit __at 0x84  P0_4;
__sbit __at 0x85  P0_5;
__sbit __at 0x86  P0_6;
__sbit __at 0x87  P0_7;



/* defining bits in SFR PCON1 */
__sbit __at 0x88  IT0;
__sbit __at 0x89  IE0;
__sbit __at 0x8a  IT1;
__sbit __at 0x8b  IE1;
__sbit __at 0x8c  TR0;
__sbit __at 0x8d  TF0;
__sbit __at 0x8e  TR1;
__sbit __at 0x8f  TF1;
__sbit __at 0x8f  EWPD;



/* defining bits in SFR P1 */
__sbit __at 0x90  P1_0;
__sbit __at 0x90  INT3;
__sbit __at 0x91  P1_1;
__sbit __at 0x91  INT4;
__sbit __at 0x92  P1_2;
__sbit __at 0x92  INT5;
__sbit __at 0x93  P1_3;
__sbit __at 0x93  INT6;
__sbit __at 0x94  P1_4;
__sbit __at 0x94  INT2;
__sbit __at 0x95  P1_5;
__sbit __at 0x95  T2EX;
__sbit __at 0x96  P1_6;
__sbit __at 0x96  CLKOUT;
__sbit __at 0x97  P1_7;
__sbit __at 0x97  T2;



/* defining bits in SFR SCON */
__sbit __at 0x98  RI;
__sbit __at 0x99  TI;
__sbit __at 0x9a  RB8;
__sbit __at 0x9b  TB8;
__sbit __at 0x9c  REN;
__sbit __at 0x9d  SM2;
__sbit __at 0x9e  SM1;
__sbit __at 0x9f  SM0;



/* defining bits in SFR P2 */
__sbit __at 0xa0  P2_0;
__sbit __at 0xa1  P2_1;
__sbit __at 0xa2  P2_2;
__sbit __at 0xa3  P2_3;
__sbit __at 0xa4  P2_4;
__sbit __at 0xa5  P2_5;
__sbit __at 0xa6  P2_6;
__sbit __at 0xa7  P2_7;



/* defining bits in SFR IEN0 */
__sbit __at 0xa8  EX0;
__sbit __at 0xa9  ET0;
__sbit __at 0xaa  EX1;
__sbit __at 0xab  ET1;
__sbit __at 0xac  ES;
__sbit __at 0xad  ET2;
__sbit __at 0xae  WDT;
__sbit __at 0xaf  EA;



/* defining bits in SFR P3 */
__sbit __at 0xb0  P3_0;
__sbit __at 0xb0  RXD;
__sbit __at 0xb1  P3_1;
__sbit __at 0xb1  TXD;
__sbit __at 0xb2  P3_2;
__sbit __at 0xb2  INT0;
__sbit __at 0xb3  P3_3;
__sbit __at 0xb3  INT1;
__sbit __at 0xb4  P3_4;
__sbit __at 0xb4  T0;
__sbit __at 0xb5  P3_5;
__sbit __at 0xb5  T1;
__sbit __at 0xb6  P3_6;
__sbit __at 0xb6  WR;
__sbit __at 0xb7  P3_7;
__sbit __at 0xb7  RD;



/* defining bits in SFR IEN1 */
__sbit __at 0xb8  EADC;
__sbit __at 0xb9  EX2;
__sbit __at 0xba  EX3;
__sbit __at 0xbb  EX4;
__sbit __at 0xbc  EX5;
__sbit __at 0xbd  EX6;
__sbit __at 0xbe  SWDT;
__sbit __at 0xbf  EXEN2;



/* defining bits in SFR IRCON */
__sbit __at 0xc0  IADC;
__sbit __at 0xc1  IEX2;
__sbit __at 0xc2  IEX3;
__sbit __at 0xc3  IEX4;
__sbit __at 0xc4  IEX5;
__sbit __at 0xc5  IEX6;
__sbit __at 0xc6  TF2;
__sbit __at 0xc7  EXF2;



/* defining bits in SFR T2CON */
__sbit __at 0xc8  T2I0;
__sbit __at 0xc9  T2I1;
__sbit __at 0xca  T2CM;
__sbit __at 0xcb  T2R0;
__sbit __at 0xcc  T2R1;
__sbit __at 0xcd  I2FR;
__sbit __at 0xce  I3FR;
__sbit __at 0xcf  T2PS;



/* defining bits in SFR PSW */
__sbit __at 0xd0  P;
__sbit __at 0xd1  F1;
__sbit __at 0xd2  OV;
__sbit __at 0xd3  RS0;
__sbit __at 0xd4  RS1;
__sbit __at 0xd5  F0;
__sbit __at 0xd6  AC;
__sbit __at 0xd7  CY;



/* defining bits in SFR ADCON0 */
__sbit __at 0xd8  MX0;
__sbit __at 0xd9  MX1;
__sbit __at 0xda  MX2;
__sbit __at 0xdb  ADM;
__sbit __at 0xdc  BSY;
__sbit __at 0xdd  ADEX;
__sbit __at 0xde  CLK;
__sbit __at 0xdf  BD;



/* defining bits in SFR ACC */
__sbit __at 0xe0  ACC_0;
__sbit __at 0xe1  ACC_1;
__sbit __at 0xe2  ACC_2;
__sbit __at 0xe3  ACC_3;
__sbit __at 0xe4  ACC_4;
__sbit __at 0xe5  ACC_5;
__sbit __at 0xe6  ACC_6;
__sbit __at 0xe7  ACC_7;



/* defining bits in SFR P4 */
__sbit __at 0xe8  P4_0;
__sbit __at 0xe8  ADST;
__sbit __at 0xe9  P4_1;
__sbit __at 0xe9  SCLK;
__sbit __at 0xea  P4_2;
__sbit __at 0xea  SRI;
__sbit __at 0xeb  P4_3;
__sbit __at 0xeb  STO;
__sbit __at 0xec  P4_4;
__sbit __at 0xec  SLS;
__sbit __at 0xed  P4_5;
__sbit __at 0xed  INT8;
__sbit __at 0xee  P4_6;
__sbit __at 0xee  TXDC;
__sbit __at 0xef  P4_7;
__sbit __at 0xef  RXDC;



/* defining bits in SFR B */
__sbit __at 0xf0  B_0;
__sbit __at 0xf1  B_1;
__sbit __at 0xf2  B_2;
__sbit __at 0xf3  B_3;
__sbit __at 0xf4  B_4;
__sbit __at 0xf5  B_5;
__sbit __at 0xf6  B_6;
__sbit __at 0xf7  B_7;



/* defining bits in SFR DIR5 */
__sbit __at 0xf8  P5_0;
__sbit __at 0xf9  P5_1;
__sbit __at 0xfa  P5_2;
__sbit __at 0xfb  P5_3;
__sbit __at 0xfc  P5_4;
__sbit __at 0xfd  P5_5;
__sbit __at 0xfe  P5_6;
__sbit __at 0xff  P5_7;
__sbit __at 0xf8  DIR5_0;
__sbit __at 0xf9  DIR5_1;
__sbit __at 0xfa  DIR5_2;
__sbit __at 0xfb  DIR5_3;
__sbit __at 0xfc  DIR5_4;
__sbit __at 0xfd  DIR5_5;
__sbit __at 0xfe  DIR5_6;
__sbit __at 0xff  DIR5_7;

#endif /* _REGC515C_H */


