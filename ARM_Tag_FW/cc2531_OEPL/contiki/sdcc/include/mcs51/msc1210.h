/*-------------------------------------------------------------------------
   msc1210.h - register declarations for Texas Intruments MSC12xx MCU family

   Copyright (C) 2006, Philippe Latu <philippe.latu AT linux-france.org>

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

#ifndef __MSC1210_H__
#define __MSC1210_H__

/*  BYTE Registers with bit definitions */

__sfr __at (0x80) P0;           /* Port 0                               */
__sfr __at (0x81) SP;           /* Stack Pointer                        */
__sfr __at (0x82) DPL;          /* Data Pointer 0: low byte             */
__sfr __at (0x82) DPL0;         /* Data Pointer 0: low byte             */
__sfr __at (0x83) DPH;          /* Data Pointer 0: high byte            */
__sfr __at (0x83) DPH0;         /* Data Pointer 0: high byte            */
__sfr __at (0x84) DPL1;         /* Data Pointer 1: low byte             */
__sfr __at (0x85) DPH1;         /* Data Pointer 1: high byte            */
__sfr __at (0x86) DPS;          /* Data Pointer Select                  */
__sfr __at (0x87) PCON;         /* Power Control                        */
__sfr __at (0x88) TCON;         /* Timer Control                        */
        /*  _7_ _6_ _5_ _4_ _3_ _2_ _1_ _0_                             */
        /* |TF1|TR1|TF0|TR0|IE1|IT1|IE0|IT0|                            */
  __sbit __at (0x88) IT0;         /* External Interrupt 0 Type          */
  __sbit __at (0x89) IE0;         /* External Interrupt 0 Edge Flag     */
  __sbit __at (0x8a) IT1;         /* External Interrupt 1 Type          */
  __sbit __at (0x8b) IE1;         /* External Interrupt 1 Edge Flag     */
  __sbit __at (0x8c) TR0;         /* Timer 0 On/Off Control             */
  __sbit __at (0x8d) TF0;         /* Timer 0 Overflow Flag              */
  __sbit __at (0x8e) TR1;         /* Timer 1 On/Off Control             */
  __sbit __at (0x8f) TF1;         /* Timer 1 Overflow Flag              */
__sfr __at (0x89) TMOD;         /* Timer Mode                           */
__sfr __at (0x8a) TL0;          /* Timer 0: low byte                    */
__sfr __at (0x8b) TL1;          /* Timer 1: low byte                    */
__sfr __at (0x8c) TH0;          /* Timer 0: high byte                   */
__sfr __at (0x8d) TH1;          /* Timer 1: high byte                   */
__sfr __at (0x8e) CKCON;        /* Clock Control                        */
__sfr __at (0x8f) MWS;          /* Memory Write Select                  */
__sfr __at (0x90) P1;           /* Port 1                               */
        /*  _7_ _6_ _5_ _4_ _3_ _2_ _1_  _0_                            */
        /* |   |   |   |   |   |   |T2EX|T2 |                           */
  __sbit __at (0x90) T2;          /* Timer 2 External Input             */
  __sbit __at (0x91) T2EX;        /* Timer 2 Capture/Reload             */
__sfr __at (0x91) EXIF;         /* External Interrupt Flag              */
__sfr __at (0x92) MPAGE;        /* Memory Page                          */
__sfr __at (0x92) _XPAGE;       /* XDATA/PDATA PAGE                     */
__sfr __at (0x93) CADDR;        /* Configuration Address Register       */
__sfr __at (0x94) CDATA;        /* Configuration Data Register          */
__sfr __at (0x95) MCON;         /* Memory Configuration                 */
__sfr __at (0x98) SCON;         /* Serial Control 0                     */
__sfr __at (0x98) SCON0;        /* Serial Control 0                     */
        /*  _7_ _6_ _5_ _4_ _3_ _2_ _1_ _0_                             */
        /* |SM0|SM1|SM2|REN|TB8|RB8|TI |RI |                            */
  __sbit __at (0x98) RI;          /* Receive Interrupt Flag             */
  __sbit __at (0x98) RI0;         /* Receive Interrupt Flag             */
  __sbit __at (0x98) RI_0;        /* Receive Interrupt Flag             */
  __sbit __at (0x99) TI;          /* Transmit Interrupt Flag            */
  __sbit __at (0x99) TI0;         /* Transmit Interrupt Flag            */
  __sbit __at (0x99) TI_0;        /* Transmit Interrupt Flag            */
  __sbit __at (0x9a) RB8;         /* Receive Bit 8                      */
  __sbit __at (0x9a) RB8_0;       /* Receive Bit 8                      */
  __sbit __at (0x9b) TB8;         /* Transmit Bit 8                     */
  __sbit __at (0x9b) TB8_0;       /* Transmit Bit 8                     */
  __sbit __at (0x9c) REN;         /* Receive Enable                     */
  __sbit __at (0x9c) REN_0;       /* Receive Enable                     */
  __sbit __at (0x9d) SM2;         /* Multiprocessor Communication Enable*/
  __sbit __at (0x9d) SM2_0;       /* Multiprocessor Communication Enable*/
  __sbit __at (0x9e) SM1;         /* Serial Port Select Mode 1          */
  __sbit __at (0x9e) SM1_0;       /* Serial Port Select Mode 1          */
  __sbit __at (0x9f) SM0;         /* Serial Port Select Mode 0          */
  __sbit __at (0x9f) SM0_0;       /* Serial Port Select Mode 0          */
__sfr __at (0x99) SBUF;         /* Serial Buffer 0                      */
__sfr __at (0x99) SBUF0;        /* Serial Buffer 0                      */
__sfr __at (0x9a) SPICON;       /* SPI Control                          */
__sfr __at (0x9a) I2CCON;       /* I2C Control                          */
        /*  _7_   _6_   _5_   _4_  _3_   _2_  _1_  _0_                  */
        /* |SCLK2|SCLK1|SCLK0|FIFO|ORDER|MSTR|CPHA|CPOL|                */
  __sbit __at (0x9a) CPOL;        /* Serial Clock Polarity              */
  __sbit __at (0x9b) CPHA;        /* Serial Clock Phase Control         */
  __sbit __at (0x9c) MSTR;        /* Set Master Mode                    */
  __sbit __at (0x9d) ORDER;       /* Set Bit Order Transmit/Receive     */
  __sbit __at (0x9e) FIFO;        /* Enable Fifo Buffer                 */
  __sbit __at (0x9f) SCLK0;       /* Clock Divider Select 0             */
  __sbit __at (0xa0) SCLK1;       /* Clock Divider Select 1             */
  __sbit __at (0xa1) SCLK2;       /* Clock Divider Select 2             */
__sfr __at (0x9b) SPIDATA;      /* SPI Data                             */
__sfr __at (0x9b) I2CDATA;      /* I2C Data                             */
__sfr __at (0x9c) SPIRCON;      /* SPI Receive Control                  */
__sfr __at (0x9c) I2CGM;        /* I2C GM Register                      */
__sfr __at (0x9d) SPITCON;      /* SPI Transmit Control                 */
__sfr __at (0x9d) I2CSTAT;      /* I2C Status                           */
__sfr __at (0x9e) SPISTART;     /* SPI Buffer Start Address             */
__sfr __at (0x9e) I2CSTART;     /* I2C Start                            */
__sfr __at (0x9f) SPIEND;       /* SPI Buffer End Address               */
__sfr __at (0xa0) P2;           /* Port 2                               */
__sfr __at (0xa1) PWMCON;       /* PWM Control                          */
        /*  _7_ _6_ _5_  _4_    _3_    _2_     _1_     _0_              */
        /* |   |   |PPOL|PWMSEL|SPDSEL|TPCNTL2|TPCNTL1|TPCNTL0|         */
  __sbit __at (0xa1) TPCNTL0;     /* Generator Control                  */
  __sbit __at (0xa2) TPCNTL1;     /* Generator Control                  */
  __sbit __at (0xa3) TPCNTL2;     /* Generator Control                  */
  __sbit __at (0xa4) SPDSEL;      /* Speed Selection                    */
  __sbit __at (0xa5) PWMSEL;      /* PWM Register Select                */
  __sbit __at (0xa6) PPOL;        /* Period Polarity                    */
__sfr __at (0xa2) PWMLOW;       /* PWM low byte                         */
__sfr __at (0xa2) TONELOW;      /* Tone low byte                        */
__sfr __at (0xa3) PWMHI;        /* PWM high byte                        */
__sfr __at (0xa3) TONEHI;       /* Tone high byte                       */
__sfr __at (0xa4) AIPOL;        /* Auxiliary Interrupt Poll             */
__sfr __at (0xa5) PAI;          /* Pending Auxiliary Interrupt          */
__sfr __at (0xa6) AIE;          /* Auxiliary Interrupt Enable           */
__sfr __at (0xa7) AISTAT;       /* Auxiliary Interrupt Status           */
__sfr __at (0xa8) IE;           /* Interrupt Enable                     */
        /*  _7_ _6_ _5_ _4_ _3_ _2_ _1_ _0_                             */
        /* |EA |ES1|ET2|ES0|ET1|EX1|ET0|EX0|                            */
  __sbit __at (0xa8) EX0;         /* Enable External Interrupt 0        */
  __sbit __at (0xa9) ET0;         /* Enable Timer 0 Interrupt           */
  __sbit __at (0xaa) EX1;         /* Enable External Interrupt 1        */
  __sbit __at (0xab) ET1;         /* Enable Timer 1 Interrupt           */
  __sbit __at (0xac) ES0;         /* Enable Serial Port 0 Interrupt     */
  __sbit __at (0xad) ET2;         /* Enable Timer 2 Interrupt           */
  __sbit __at (0xae) ES1;         /* Enable Serial Port 1 Interrupt     */
  __sbit __at (0xaf) EA;          /* Global Interrupt Enable            */
__sfr __at (0xa9) BPCON;        /* Breakpoint Control                   */
__sfr __at (0xaa) BPL;          /* Breakpoint Address Low               */
__sfr __at (0xab) BPH;          /* Breakpoint Address High              */
__sfr __at (0xac) P0DDRL;       /* Port 0 Data Direction Low            */
__sfr __at (0xad) P0DDRH;       /* Port 0 Data Direction High           */
__sfr __at (0xae) P1DDRL;       /* Port 1 Data Direction Low            */
__sfr __at (0xaf) P1DDRH;       /* Port 1 Data Direction High           */
__sfr __at (0xb0) P3;           /* Port 3                               */
        /*  _7_ _6_ _5_ _4_ _3_  _2_  _1_ _0_                           */
        /* |RD |WR |T1 |T0 |INT1|INT0|TXD|RXD|                          */
  __sbit __at (0xb0) RXD;         /* Serial Port 0 Receive              */
  __sbit __at (0xb0) RXD0;        /* Serial Port 0 Receive              */
  __sbit __at (0xb1) TXD;         /* Serial Port 0 Transmit             */
  __sbit __at (0xb1) TXD0;        /* Serial Port 0 Transmit             */
  __sbit __at (0xb2) INT0;        /* External Interrupt 0               */
  __sbit __at (0xb3) INT1;        /* External Interrupt 1               */
  __sbit __at (0xb4) T0;          /* Timer 0 External Input             */
  __sbit __at (0xb5) T1;          /* Timer 1 External Input             */
  __sbit __at (0xb6) WR;          /* External Memory Write Strobe       */
  __sbit __at (0xb7) RD;          /* External Memory Read Strobe        */
__sfr __at (0xb1) P2DDRL;       /* Port 2 Data Direction Low            */
__sfr __at (0xb2) P2DDRH;       /* Port 2 Data Direction High           */
__sfr __at (0xb3) P3DDRL;       /* Port 3 Data Direction Low            */
__sfr __at (0xb4) P3DDRH;       /* Port 3 Data Direction High           */
__sfr __at (0xb5) DACL;         /* Digital-to-Analog Converter Low      */
__sfr __at (0xb6) DACH;         /* Digital-to-Analog Converter High     */
__sfr __at (0xb7) DACSEL;       /* Digital-to-Analog Converter Select   */
__sfr __at (0xb8) IP;           /* Interrupt Priority                   */
        /*  _7_ _6_ _5_ _4_ _3_ _2_ _1_ _0_                             */
        /* |   |   |PT2|PS |PT1|PX1|PT0|PX0|                            */
  __sbit __at (0xb8) PX0;         /* External Interrupt 0               */
  __sbit __at (0xb9) PT0;         /* Timer 0                            */
  __sbit __at (0xba) PX1;         /* External Interrupt 1               */
  __sbit __at (0xbb) PT1;         /* Timer 1                            */
  __sbit __at (0xbc) PS;          /* Serial Port                        */
  __sbit __at (0xbd) PT2;         /* Timer 2                            */
__sfr __at (0xc0) SCON1;        /* Serial Control 1                     */
        /*  _7_ _6_ _5_ _4_ _3_ _2_ _1_ _0_                             */
        /* |SM0|SM1|SM2|REN|TB8|RB8|TI |RI |                            */
  __sbit __at (0xc0) RI1;         /* Receive Interrupt Flag             */
  __sbit __at (0xc0) RI_1;        /* Receive Interrupt Flag             */
  __sbit __at (0xc1) TI1;         /* Transmit Interrupt Flag            */
  __sbit __at (0xc1) TI_1;        /* Transmit Interrupt Flag            */
  __sbit __at (0xc2) RB8_1;       /* Receive Bit 8                      */
  __sbit __at (0xc3) TB8_1;       /* Transmit Bit 8                     */
  __sbit __at (0xc4) REN_1;       /* Receive Enable                     */
  __sbit __at (0xc5) SM2_1;       /* Multiprocessor Communication Enable*/
  __sbit __at (0xc6) SM1_1;       /* Serial Port Select Mode 1          */
  __sbit __at (0xc7) SM0_1;       /* Serial Port Select Mode 0          */
__sfr __at (0xc1) SBUF1;        /* Serial Buffer 1                      */
__sfr __at (0xc6) EWU;          /* Enable Wake Up                       */
        /*  _7_ _6_ _5_ _4_ _3_ _2_    _1_    _0_                       */
        /* |   |   |   |   |   |EWUEX1|EWUEX0|EWUWDT|                   */
  __sbit __at (0xc6) EWUWDT;      /* Enable Watchdog Interrupt          */
  __sbit __at (0xc7) EWUEX0;      /* Enable External Interrupt 0        */
  __sbit __at (0xc8) EWUEX1;      /* Enable External Interrupt 1        */
__sfr __at (0xc7) SYSCLK;       /* System Clock Divider                 */
__sfr __at (0xc8) T2CON;        /* Timer 2 Control                      */
        /*  _7_ _6_  _5_  _4_  _3_   _2_ _1_  _0_                       */
        /* |TF2|EXF2|RCLK|TCLK|EXEN2|TR2|C_T2|CP_RL2 |                  */
  __sbit __at (0xc8) CP_RL2;      /* Capture/Reload Flag                */
  __sbit __at (0xc9) C_T2;        /* Overflow Flag                      */
  __sbit __at (0xca) TR2;         /* Timer Run                          */
  __sbit __at (0xcb) EXEN2;       /* Timer External Enable              */
  __sbit __at (0xcc) TCLK;        /* Transmit Clock Flag                */
  __sbit __at (0xcd) RCLK;        /* Receive Clock Flag                 */
  __sbit __at (0xce) EXF2;        /* External Flag                      */
  __sbit __at (0xcf) TF2;         /* Overflow Flag                      */
__sfr __at (0xca) RCAP2L;       /* Timer 2 Capture Low                  */
__sfr __at (0xcb) RCAP2H;       /* Timer 2 Capture High                 */
__sfr __at (0xcc) TL2;          /* Timer 2 Low byte                     */
__sfr __at (0xcd) TH2;          /* Timer 2 High byte                    */
__sfr __at (0xd0) PSW;          /* Program Status Word                  */
        /*  _7_ _6_ _5_ _4_ _3_ _2_ _1_ _0_                             */
        /* |CY |AC |F0 |RS1|RS0|OV |F1 |P  |                            */
  __sbit __at (0xd0) P;           /* Parity Flag                        */
  __sbit __at (0xd1) F1;          /* General Purpose User Flag 1        */
  __sbit __at (0xd2) OV;          /* Overflow Flag                      */
  __sbit __at (0xd3) RS0;         /* Register Bank Select 0 Flag        */
  __sbit __at (0xd4) RS1;         /* Register Bank Select 1 Flag        */
  __sbit __at (0xd5) F0;          /* General Purpose User Flag 0        */
  __sbit __at (0xd6) AC;          /* Auxiliary Carry Flag               */
  __sbit __at (0xd7) CY;          /* Carry Flag                         */
__sfr __at (0xd1) OCL;          /* (ADC) Offset Calibration Low byte    */
__sfr __at (0xd2) OCM;          /* (ADC) Offset Calibration Middle byte */
__sfr __at (0xd3) OCH;          /* (ADC) Offset Calibration High byte   */
__sfr __at (0xd4) GCL;          /* (ADC) Gain Low byte                  */
__sfr __at (0xd5) GCM;          /* (ADC) Gain Middle byte               */
__sfr __at (0xd6) GCH;          /* (ADC) Gain High byte                 */
__sfr __at (0xd7) ADMUX;        /* ADC Multiplexer Register             */
__sfr __at (0xd8) EICON;        /* Enable Interrupt Control             */
        /*  _7_   _6_ _5_ _4_ _3_  _2_ _1_ _0_                          */
        /* |SMOD1|   |EAI|AI |WDTI|   |   |   |                         */
  __sbit __at (0xdb) WDTI;        /* Watchdog Timer Interrupt Flag      */
  __sbit __at (0xdc) AI;          /* Auxiliary Interrupt Flag           */
  __sbit __at (0xdd) EAI;         /* Enable Auxiliary Interrupt         */
  __sbit __at (0xdf) SMOD1;       /* Serial Port 1 Mode                 */
__sfr __at (0xd9) ADRESL;       /* ADC Conversion Result Low byte       */
__sfr __at (0xda) ADRESM;       /* ADC Conversion Result Middle byte    */
__sfr __at (0xdb) ADRESH;       /* ADC Conversion Result High byte      */
__sfr __at (0xdc) ADCON0;       /* ADC Control 0                        */
__sfr __at (0xdd) ADCON1;       /* ADC Control 1                        */
__sfr __at (0xde) ADCON2;       /* ADC Control 2                        */
__sfr __at (0xdf) ADCON3;       /* ADC Control 3                        */
__sfr __at (0xe0) ACC;          /* Accumulator                          */
__sfr __at (0xe1) SSCON;        /* Summation and Shift Control          */
__sfr __at (0xe2) SUMR0;        /* Summation Register 0 (LSB)           */
__sfr __at (0xe3) SUMR1;        /* Summation Register 1                 */
__sfr __at (0xe4) SUMR2;        /* Summation Register 2                 */
__sfr __at (0xe5) SUMR3;        /* Summation Register 3 (MSB)           */
__sfr __at (0xe6) ODAC;         /* (ADC) Offset DAC Register            */
__sfr __at (0xe7) LVDCON;       /* Low Voltage Detection Control        */
__sfr __at (0xe8) EIE;          /* Extended Interrupt Enable            */
        /*  _7_ _6_ _5_ _4_  _3_ _2_ _1_ _0_                            */
        /* |   |   |   |EWDI|EX5|EX4|EX3|EX2|                           */
  __sbit __at (0xe8) EX2;         /* Enable External Interrupt 2        */
  __sbit __at (0xe9) EX3;         /* Enable External Interrupt 3        */
  __sbit __at (0xea) EX4;         /* Enable External Interrupt 4        */
  __sbit __at (0xeb) EX5;         /* Enable External Interrupt 5        */
  __sbit __at (0xec) EWDI;        /* Enable Watchdog Interrupt          */
__sfr __at (0xe9) HWPC0;        /* Hardware Product Code 0              */
__sfr __at (0xea) HWPC1;        /* Hardware Product Code 1              */
__sfr __at (0xeb) HWVER;        /* Hardware Version number              */
__sfr __at (0xee) FMCON;        /* Flash Memory Control                 */
__sfr __at (0xef) FTCON;        /* Flash Memory Timing Control          */
__sfr __at (0xf0) B;            /* B Register                           */
__sfr __at (0xf1) PDCON;        /* Power Down Control                   */
        /*  _7_ _6_ _5_ _4_   _3_  _2_   _1_  _0_                       */
        /* |   |   |   |PDPWM|PDAD|PDWDT|PDST|PDSPI|                    */
  __sbit __at (0xf1) PDSPI;       /* SPI System Control                 */
  __sbit __at (0xf2) PDST;        /* System Timer Control               */
  __sbit __at (0xf3) PDWDT;       /* Watchdog Timer Control             */
  __sbit __at (0xf4) PDAD;        /* A/D Control                        */
  __sbit __at (0xf5) PDPWM;       /* PWM Control                        */
__sfr __at (0xf2) PASEL;        /* /PSEN|ALE Select                     */
__sfr __at (0xf6) ACLK;         /* Analog Clock                         */
__sfr __at (0xf7) SRST;         /* System Reset Register                */
__sfr __at (0xf8) EIP;          /* Extended Interrupt Priority          */
        /*  _7_ _6_ _5_ _4_  _3_ _2_ _1_ _0_                            */
        /* |   |   |   |PWDI|PX5|PX4|PX3|PX2|                           */
  __sbit __at (0xf8) PX2;         /* External Interrupt 2 Priority      */
  __sbit __at (0xf9) PX3;         /* External Interrupt 3 Priority      */
  __sbit __at (0xfa) PX4;         /* External Interrupt 4 Priority      */
  __sbit __at (0xfb) PX5;         /* External Interrupt 5 Priority      */
  __sbit __at (0xfc) PWDI;        /* Watchdog Interrupt Priority        */
__sfr __at (0xf9) SECINT;       /* Seconds Timer Interrupt              */
__sfr __at (0xfa) MSINT;        /* Milliseconds Interrupt               */
__sfr __at (0xfb) USEC;         /* Microsecond Register                 */
__sfr __at (0xfc) MSECL;        /* Millisecond Low byte                 */
__sfr __at (0xfd) MSECH;        /* Millisecond High byte                */
__sfr __at (0xfe) HMSEC;        /* Hundred Millisecond Clock            */
__sfr __at (0xff) WDTCON;       /* Watchdog Control                     */

/* Word Registers */
__sfr16  __at (0x8c8a) TMR0;
__sfr16  __at (0x8d8b) TMR1;
__sfr16  __at (0xa3a2) PWM;
__sfr16  __at (0xa3a2) TONE;
__sfr16  __at (0xabaa) BP;
__sfr16  __at (0xabaa) BREAKPT;
__sfr16  __at (0xadac) P0DDR;
__sfr16  __at (0xafae) P1DDR;
__sfr16  __at (0xb2b1) P2DDR;
__sfr16  __at (0xb4b3) P3DDR;
__sfr16  __at (0xcbca) RCAP2;
__sfr16  __at (0xcdcc) TMR2;
__sfr16  __at (0xdfde) DECIMATION;
__sfr16  __at (0xfdfc) ONEMS;
__sfr16  __at (0xfdfc) MSEC;

/* Double Word Registers */
__sfr32  __at (0xe5e4e3e2) SUMR;

#endif
