/*-------------------------------------------------------------------------
   ds80c390.h - Register Declarations for the DALLAS DS80C390 Processor
                 far from complete, e.g. no CAN

   Copyright (C) 2000, Johan Knol <johan.knol AT iduna.nl>

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

#ifndef DS80C390_H
#define DS80C390_H

__sfr __at 0x80 P4; 		/* ce3..ce0, a19..a16 */
__sfr __at 0x81 SP; 		/* stack pointer */
__sfr __at 0x82 DPL; 		/* data pointer 0 lsb */
__sfr __at 0x83 DPH; 		/* data pointer 0 msb */
__sfr __at 0x84 DPL1; 		/* data pointer 1 lsb */
__sfr __at 0x85 DPH1; 		/* data pointer 1 msb */
__sfr __at 0x86 DPS; 		/* data pointer select */
__sfr __at 0x87 PCON; 		/* power control */
__sfr __at 0x88 TCON; 		/* timer/counter control */
  __sbit __at 0x88 IT0;
  __sbit __at 0x89 IE0;
  __sbit __at 0x8a IT1;
  __sbit __at 0x8b IE1;
  __sbit __at 0x8c TR0;
  __sbit __at 0x8d TF0;
  __sbit __at 0x8e TR1;
  __sbit __at 0x8f TF1;
__sfr __at 0x89 TMOD; 		/* timer mode control */
__sfr __at 0x8a TL0; 		/* timer 0 lsb */
__sfr __at 0x8b TL1; 		/* timer 1 msb */
__sfr __at 0x8c TH0; 		/* timer 0 msb */
__sfr __at 0x8d TH1; 		/* timer 1 msb */
__sfr __at 0x8e CKCON; 		/* clock control */
__sfr __at 0x90 P1;
  __sbit __at 0x90 T2;
  __sbit __at 0x91 T2EX;
  __sbit __at 0x92 RXD1;
  __sbit __at 0x93 TXD1;
  __sbit __at 0x94 INT2;
  __sbit __at 0x95 INT3;
  __sbit __at 0x96 INT4;
  __sbit __at 0x97 INT5;
__sfr __at 0x91 EXIF; 		/* external interrupt flag */
__sfr __at 0x92 P4CNT;
__sfr __at 0x93 DPX; 		/* extended datapointer 0 */
__sfr __at 0x95 DPX1; 		/* extended datapointer 1 */
__sfr __at 0x98 SCON0; 		/* serial 0 control */
  __sbit __at 0x98 RI_0;
  __sbit __at 0x99 TI_0;
  __sbit __at 0x9a RB8_0;
  __sbit __at 0x9b TB8_0;
  __sbit __at 0x9c REN_0;
  __sbit __at 0x9d SM2_0;
  __sbit __at 0x9e SM1_0;
  __sbit __at 0x9f SM0_0;
  __sbit __at 0x9f FE_0;	/* depending on SMOD0 */
__sfr __at 0x99 SBUF0;		/* serial 0 data buffer */
__sfr __at 0x9b ESP; 		/* extended stack pointer */
__sfr __at 0x9c AP; 		/* address page */
__sfr __at 0x9d ACON; 		/* address control */
__sfr __at 0xa0 P2; 		/* never mind the sbits */
__sfr __at 0xa1 P5;
__sfr __at 0xa2 P5CNT;
__sfr __at 0xa8 IE; 		/* interrupt enable */
  __sbit __at 0xa8 EX0;
  __sbit __at 0xa9 ET0;
  __sbit __at 0xaa EX1;
  __sbit __at 0xab ET1;
  __sbit __at 0xac ES0;
  __sbit __at 0xad ET2;
  __sbit __at 0xae ES1;
  __sbit __at 0xaf EA;
__sfr __at 0xb0 P3;
  __sbit __at 0xb0 RXD0;
  __sbit __at 0xb1 TXD0;
  __sbit __at 0xb2 INT0;
  __sbit __at 0xb3 INT1;
  __sbit __at 0xb4 T0;
  __sbit __at 0xb5 T1;
  __sbit __at 0xb6 WR;
  __sbit __at 0xb7 RD;
__sfr __at 0xb8 IP; 		/* interupt priority */
  __sbit __at 0xb8 PX0; 	/* external 0 */
  __sbit __at 0xb9 PT0; 	/* timer 0 */
  __sbit __at 0xba PX1; 	/* external 1 */
  __sbit __at 0xbb PT1; 	/* timer 1 */
  __sbit __at 0xbc PS0; 	/* serial port 0 */
  __sbit __at 0xbd PT2; 	/* timer 2 */
  __sbit __at 0xbe PS1; 	/* serial port 1 */
__sfr __at 0xc0 SCON1; 		/* serial 1 control */
  __sbit __at 0xc0 RI_1;
  __sbit __at 0xc1 TI_1;
  __sbit __at 0xc2 RB8_1;
  __sbit __at 0xc3 TB8_1;
  __sbit __at 0xc4 REN_1;
  __sbit __at 0xc5 SM2_1;
  __sbit __at 0xc6 SM1_1;
  __sbit __at 0xc7 SM0_1;
  __sbit __at 0xc7 FE_1;	/* depending on SMOD0 */
__sfr __at 0xc1 SBUF1; 		/* serial 1 data buffer */
__sfr __at 0xc4 PMR; 		/* power managment */
__sfr __at 0xc6 MCON; 		/* memory control register */
__sfr __at 0xc7 TA; 		/* timed access register */
__sfr __at 0xc8 T2CON; 		/* timer 2 control */
  __sbit __at 0xc8 CP_RL;	/* capture/reload */
  __sbit __at 0xc9 C_T; 	/* count/timer */
  __sbit __at 0xca TR2; 	/* stop/run */
  __sbit __at 0xcb EXEN2;
  __sbit __at 0xcc TCLK;
  __sbit __at 0xcd RCLK;
  __sbit __at 0xce EXF2;
  __sbit __at 0xcf TF2; 	/* overflow flag */
__sfr __at 0xc9 T2MOD; 		/* timer 2 mode */
__sfr __at 0xca RCAP2L; 	/* timer 2 capture/reload */
__sfr __at 0xca RTL2; 		/* depends on CP_RL */
__sfr __at 0xcb RCAP2H;
__sfr __at 0xcb RTH2;
__sfr __at 0xcc TL2; 		/* timer 2 lsb */
__sfr __at 0xcd TH2; 		/* timer 2 msb */
__sfr __at 0xd0 PSW; 		/* program status word (byte actually) */
  __sbit __at 0xd0 P; 		/* parity */
  __sbit __at 0xd1 F1; 		/* user flag 1 */
  __sbit __at 0xd2 OV; 		/* overflow flag */
  __sbit __at 0xd3 RS0; 	/* register select l */
  __sbit __at 0xd4 RS1; 	/* register select h */
  __sbit __at 0xd5 F0; 		/* user flag 0 */
  __sbit __at 0xd6 AC; 		/* auxiliary carry flag */
  __sbit __at 0xd7 CY; 		/* carry flag */
__sfr __at 0xd1 MCNT0; 		/* arithmetic accellerator */
__sfr __at 0xd2 MCNT1;
__sfr __at 0xd3 MA;
__sfr __at 0xd4 MB;
__sfr __at 0xd5 MC;
__sfr __at 0xd8 WDCON; 		/* watch dog */
  __sbit __at 0xd8 RWT;
  __sbit __at 0xd9 EWT;
  __sbit __at 0xda WDRF;
  __sbit __at 0xdb WDIF;
  __sbit __at 0xdc PFI;
  __sbit __at 0xdd EPFI;
  __sbit __at 0xde POR;
  __sbit __at 0xdf SMOD_1;
__sfr __at 0xe0 ACC; 		/* accumulator */
__sfr __at 0xe8 EIE; 		/* extended interrupt enable */
  __sbit __at 0xe8 EX2;
  __sbit __at 0xe9 EX3;
  __sbit __at 0xea EX4;
  __sbit __at 0xeb EX5;
  __sbit __at 0xec EWDI;
  __sbit __at 0xed C1IE;
  __sbit __at 0xee C0IE;
  __sbit __at 0xef CANBIE;
__sfr __at 0xea MXAX; 		/* extended address register */
__sfr __at 0xf0 B; 			/* aux accumulator */
__sfr __at 0xf8 EIP; 		/* extended interrupt priority */
  __sbit __at 0xf8 PX2;
  __sbit __at 0xf9 PX3;
  __sbit __at 0xfa PX4;
  __sbit __at 0xfb PX5;
  __sbit __at 0xfc PWDI;
  __sbit __at 0xfd C1IP;
  __sbit __at 0xfe C0IP;
  __sbit __at 0xff CANBIP;

/*  WORD/DWORD Registers  */

__sfr16 __at (0x8C8A) TMR0;  /* TIMER 0 COUNTER   				*/
__sfr16 __at (0x8D8B) TMR1;  /* TIMER 1 COUNTER                 */
__sfr16 __at (0xCDCC) TMR2;  /* TIMER 2 COUNTER                 */
__sfr16 __at (0xCBCA) RCAP2; /* TIMER 2 CAPTURE REGISTER WORD	*/

#endif /* DS80C390_H */
