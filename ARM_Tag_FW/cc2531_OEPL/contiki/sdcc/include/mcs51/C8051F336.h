/*-------------------------------------------------------------------------
   C8051F336.h - Register Declarations for the SiLabs C8051F336/7/8/9
   Processor Range
 
   Copyright (C) 2008, Steven Borley, steven.borley@partnerelectronics.com

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

#ifndef C8051F336_H
#define C8051F336_H

#include <compiler.h>

/*  BYTE Registers  */

SFR(       P0, 0x80 )  ; /* PORT 0 */
SFR(       SP, 0x81 )  ; /* STACK POINTER */
SFR(      DPL, 0x82 )  ; /* DATA POINTER - LOW BYTE */
SFR(      DPH, 0x83 )  ; /* DATA POINTER - HIGH BYTE */
SFR(     PCON, 0x87 )  ; /* POWER CONTROL */
SFR(     TCON, 0x88 )  ; /* TIMER CONTROL */
SFR(     TMOD, 0x89 )  ; /* TIMER MODE */
SFR(      TL0, 0x8A )  ; /* TIMER 0 - LOW BYTE */
SFR(      TL1, 0x8B )  ; /* TIMER 1 - LOW BYTE */
SFR(      TH0, 0x8C )  ; /* TIMER 0 - HIGH BYTE */
SFR(      TH1, 0x8D )  ; /* TIMER 1 - HIGH BYTE */
SFR(    CKCON, 0x8E )  ; /* CLOCK CONTROL */
SFR(    PSCTL, 0x8F )  ; /* PROGRAM STORE R/W CONTROL */
SFR(       P1, 0x90 )  ; /* PORT 1 */
SFR(   TMR3CN, 0x91 )  ; /* TIMER 3 CONTROL */
SFR(  TMR3RLL, 0x92 )  ; /* TIMER 3 CAPTURE REGISTER - LOW BYTE */
SFR(  TMR3RLH, 0x93 )  ; /* TIMER 3 CAPTURE REGISTER - HIGH BYTE */
SFR(    TMR3L, 0x94 )  ; /* TIMER 3 - LOW BYTE */
SFR(    TMR3H, 0x95 )  ; /* TIMER 3 - HIGH BYTE */
SFR(    IDA0L, 0x96 )  ; /* CURRENT MODE DAC 0 - LOW BYTE */
SFR(    IDA0H, 0x97 )  ; /* CURRENT MODE DAC 0 - HIGH BYTE */
SFR(     SCON, 0x98 )  ; /* SERIAL PORT CONTROL */
SFR(    SCON0, 0x98 )  ; /* SERIAL PORT CONTROL */
SFR(     SBUF, 0x99 )  ; /* SERIAL PORT BUFFER */
SFR(    SBUF0, 0x99 )  ; /* SERIAL PORT BUFFER */
SFR(   CPT0CN, 0x9B )  ; /* COMPARATOR 0 CONTROL */
SFR(   CPT0MD, 0x9D )  ; /* COMPARATOR 0 MODE SELECTION */
SFR(   CPT0MX, 0x9F )  ; /* COMPARATOR 0 MUX SELECTION */
SFR(       P2, 0xA0 )  ; /* PORT 2 */
SFR(  SPI0CFG, 0xA1 )  ; /* SPI0 CONFIGURATION */
SFR(  SPI0CKR, 0xA2 )  ; /* SPI0 CLOCK RATE CONTROL */
SFR(  SPI0DAT, 0xA3 )  ; /* SPI0 DATA */
SFR(  P0MDOUT, 0xA4 )  ; /* PORT 0 OUTPUT MODE CONFIGURATION */
SFR(  P1MDOUT, 0xA5 )  ; /* PORT 1 OUTPUT MODE CONFIGURATION */
SFR(  P2MDOUT, 0xA6 )  ; /* PORT 2 OUTPUT MODE CONFIGURATION */
SFR(       IE, 0xA8 )  ; /* INTERRUPT ENABLE */
SFR(   CLKSEL, 0xA9 )  ; /* SYSTEM CLOCK SELECT */
SFR(   EMI0CN, 0xAA )  ; /* EXTERNAL MEMORY INTERFACE CONTROL */
SFR(   _XPAGE, 0xAA )  ; /* XDATA/PDATA PAGE */
SFR(   OSCXCN, 0xB1 )  ; /* EXTERNAL OSCILLATOR CONTROL */
SFR(   OSCICN, 0xB2 )  ; /* INTERNAL OSCILLATOR CONTROL */
SFR(   OSCICL, 0xB3 )  ; /* INTERNAL OSCILLATOR CALIBRATION */
SFR(    FLSCL, 0xB6 )  ; /* FLASH MEMORY TIMING PRESCALER */
SFR(    FLKEY, 0xB7 )  ; /* FLASH ACESS LIMIT */
SFR(       IP, 0xB8 )  ; /* INTERRUPT PRIORITY */
SFR(   IDA0CN, 0xB9 )  ; /* CURRENT MODE DAC 0 - CONTROL */
SFR(    AMX0N, 0xBA )  ; /* ADC 0 MUX NEGATIVE CHANNEL SELECTION */
SFR(    AMX0P, 0xBB )  ; /* ADC 0 MUX POSITIVE CHANNEL SELECTION */
SFR(   ADC0CF, 0xBC )  ; /* ADC 0 CONFIGURATION */
SFR(    ADC0L, 0xBD )  ; /* ADC 0 DATA WORD LSB */
SFR(    ADC0H, 0xBE )  ; /* ADC 0 DATA WORD MSB */
SFR(   SMB0CN, 0xC0 )  ; /* SMBUS CONTROL */
SFR(   SMB0CF, 0xC1 )  ; /* SMBUS CONFIGURATION */
SFR(  SMB0DAT, 0xC2 )  ; /* SMBUS DATA */
SFR(  ADC0GTL, 0xC3 )  ; /* ADC 0 GREATER-THAN LOW BYTE */
SFR(  ADC0GTH, 0xC4 )  ; /* ADC 0 GREATER-THAN HIGH BYTE */
SFR(  ADC0LTL, 0xC5 )  ; /* ADC 0 LESS-THAN LOW BYTE */
SFR(  ADC0LTH, 0xC6 )  ; /* ADC 0 LESS-THAN HIGH BYTE */
SFR(    T2CON, 0xC8 )  ; /* TIMER 2 CONTROL */
SFR(   TMR2CN, 0xC8 )  ; /* TIMER 2 CONTROL */
SFR(   RCAP2L, 0xCA )  ; /* TIMER 2 CAPTURE REGISTER - LOW BYTE */
SFR(  TMR2RLL, 0xCA )  ; /* TIMER 2 CAPTURE REGISTER - LOW BYTE */
SFR(   RCAP2H, 0xCB )  ; /* TIMER 2 CAPTURE REGISTER - HIGH BYTE */
SFR(  TMR2RLH, 0xCB )  ; /* TIMER 2 CAPTURE REGISTER - HIGH BYTE */
SFR(      TL2, 0xCC )  ; /* TIMER 2 - LOW BYTE */
SFR(    TMR2L, 0xCC )  ; /* TIMER 2 - LOW BYTE */
SFR(      TH2, 0xCD )  ; /* TIMER 2 - HIGH BYTE */
SFR(    TMR2H, 0xCD )  ; /* TIMER 2 - HIGH BYTE */
SFR(      PSW, 0xD0 )  ; /* PROGRAM STATUS WORD */
SFR(   REF0CN, 0xD1 )  ; /* VOLTAGE REFERENCE 0 CONTROL */
SFR(   P0SKIP, 0xD4 )  ; /* PORT 0 SKIP */
SFR(   P1SKIP, 0xD5 )  ; /* PORT 1 SKIP */
SFR(   P2SKIP, 0xD6 )  ; /* PORT 2 SKIP */
SFR(  SMB0ADR, 0xD7 )  ; /* SMBUS SLAVE ADDRESS */
SFR(   PCA0CN, 0xD8 )  ; /* PCA CONTROL */
SFR(   PCA0MD, 0xD9 )  ; /* PCA MODE */
SFR( PCA0CPM0, 0xDA )  ; /* PCA MODULE 0 MODE REGISTER */
SFR( PCA0CPM1, 0xDB )  ; /* PCA MODULE 1 MODE REGISTER */
SFR( PCA0CPM2, 0xDC )  ; /* PCA MODULE 2 MODE REGISTER */
SFR(      ACC, 0xE0 )  ; /* ACCUMULATOR */
SFR(     XBR0, 0xE1 )  ; /* PORT MUX CONFIGURATION REGISTER 0 */
SFR(     XBR1, 0xE2 )  ; /* PORT MUX CONFIGURATION REGISTER 1 */
SFR(   OSCLCN, 0xE3 )  ; /* LOW-FREQUENCY OSCILLATOR CONTROL */
SFR(   IT01CF, 0xE4 )  ; /* INT0/INT1 CONFIGURATION REGISTER */
SFR(  INT01CF, 0xE4 )  ; /* INT0/INT1 CONFIGURATION REGISTER */
SFR(     EIE1, 0xE6 )  ; /* EXTERNAL INTERRUPT ENABLE 1 */
SFR(  SMB0ADM, 0xE7 )  ; /* SMBUS SLAVE ADDRESS MASK */
SFR(   ADC0CN, 0xE8 )  ; /* ADC 0 CONTROL */
SFR( PCA0CPL1, 0xE9 )  ; /* PCA CAPTURE 1 LOW */
SFR( PCA0CPH1, 0xEA )  ; /* PCA CAPTURE 1 HIGH */
SFR( PCA0CPL2, 0xEB )  ; /* PCA CAPTURE 2 LOW */
SFR( PCA0CPH2, 0xEC )  ; /* PCA CAPTURE 2 HIGH */
SFR(    P1MAT, 0xED )  ; /* PORT 1 MATCH REGISTER */
SFR(   P1MASK, 0xEE )  ; /* PORT 1 MASK REGISTER */
SFR(   RSTSRC, 0xEF )  ; /* RESET SOURCE */
SFR(        B, 0xF0 )  ; /* B REGISTER */
SFR(   P0MODE, 0xF1 )  ; /* PORT 0 INPUT MODE CONFIGURATION */
SFR(   P0MDIN, 0xF1 )  ; /* PORT 0 INPUT MODE CONFIGURATION */
SFR(   P1MODE, 0xF2 )  ; /* PORT 1 INPUT MODE CONFIGURATION */
SFR(   P1MDIN, 0xF2 )  ; /* PORT 1 INPUT MODE CONFIGURATION */
SFR(   P2MDIN, 0xF3 )  ; /* PORT 2 INPUT MODE */
SFR(     EIP1, 0xF6 )  ; /* EXTERNAL INTERRUPT PRIORITY REGISTER 1 */
SFR(  PCA0PWM, 0xF7 )  ; /* PCA PWM CONFIGURATION */
SFR(   SPI0CN, 0xF8 )  ; /* SPI0 CONTROL */
SFR(    PCA0L, 0xF9 )  ; /* PCA COUNTER LOW */
SFR(    PCA0H, 0xFA )  ; /* PCA COUNTER HIGH */
SFR( PCA0CPL0, 0xFB )  ; /* PCA CAPTURE 0 LOW */
SFR( PCA0CPH0, 0xFC )  ; /* PCA CAPTURE 0 HIGH */
SFR(    P0MAT, 0xFD )  ; /* PORT 0 MATCH REGISTER */
SFR(   P0MASK, 0xFE )  ; /* PORT 0 MASK REGISTER */
SFR(   VDM0CN, 0xFF )  ; /* VDD MONITOR CONTROL */


/*  WORD/DWORD Registers  */

SFR16E(     TMR0, 0x8C8A )  ; /* TIMER 0 COUNTER */
SFR16E(     TMR1, 0x8D8B )  ; /* TIMER 1 COUNTER */
SFR16E(     TMR2, 0xCDCC )  ; /* TIMER 2 COUNTER */
SFR16E(    RCAP2, 0xCBCA )  ; /* TIMER 2 CAPTURE REGISTER WORD */
SFR16E(   TMR2RL, 0xCBCA )  ; /* TIMER 2 CAPTURE REGISTER WORD */
SFR16E(     TMR3, 0x9594 )  ; /* TIMER 3 COUNTER */
SFR16E(   TMR3RL, 0x9392 )  ; /* TIMER 3 CAPTURE REGISTER WORD */
SFR16E(     IDA0, 0x9796 )  ; /* CURRENT MODE DAC 0 DATA WORD */
SFR16E(     ADC0, 0xBEBD )  ; /* ADC 0 DATA WORD */
SFR16E(   ADC0GT, 0xC4C3 )  ; /* ADC 0 GREATER-THAN REGISTER WORD */
SFR16E(   ADC0LT, 0xC6C5 )  ; /* ADC 0 LESS-THAN REGISTER WORD */
SFR16E(     PCA0, 0xFAF9 )  ; /* PCA COUNTER */
SFR16E(  PCA0CP0, 0xFCFB )  ; /* PCA CAPTURE 0 WORD */
SFR16E(  PCA0CP1, 0xEAE9 )  ; /* PCA CAPTURE 1 WORD */
SFR16E(  PCA0CP2, 0xECEB )  ; /* PCA CAPTURE 2 WORD */


/*  BIT Registers  */

/*  P0  0x80 */
SBIT(     P0_0, 0x80, 0 )  ;
SBIT(     P0_1, 0x80, 1 )  ;
SBIT(     P0_2, 0x80, 2 )  ;
SBIT(     P0_3, 0x80, 3 )  ;
SBIT(     P0_4, 0x80, 4 )  ;
SBIT(     P0_5, 0x80, 5 )  ;
SBIT(     P0_6, 0x80, 6 )  ;
SBIT(     P0_7, 0x80, 7 )  ;

/*  TCON  0x88 */
SBIT(      IT0, 0x88, 0 )  ; /* TCON.0 - EXT. INTERRUPT 0 TYPE */
SBIT(      IE0, 0x88, 1 )  ; /* TCON.1 - EXT. INTERRUPT 0 EDGE FLAG */
SBIT(      IT1, 0x88, 2 )  ; /* TCON.2 - EXT. INTERRUPT 1 TYPE */
SBIT(      IE1, 0x88, 3 )  ; /* TCON.3 - EXT. INTERRUPT 1 EDGE FLAG */
SBIT(      TR0, 0x88, 4 )  ; /* TCON.4 - TIMER 0 ON/OFF CONTROL */
SBIT(      TF0, 0x88, 5 )  ; /* TCON.5 - TIMER 0 OVERFLOW FLAG */
SBIT(      TR1, 0x88, 6 )  ; /* TCON.6 - TIMER 1 ON/OFF CONTROL */
SBIT(      TF1, 0x88, 7 )  ; /* TCON.7 - TIMER 1 OVERFLOW FLAG */

/*  P1  0x90 */
SBIT(     P1_0, 0x90, 0 )  ;
SBIT(     P1_1, 0x90, 1 )  ;
SBIT(     P1_2, 0x90, 2 )  ;
SBIT(     P1_3, 0x90, 3 )  ;
SBIT(     P1_4, 0x90, 4 )  ;
SBIT(     P1_5, 0x90, 5 )  ;
SBIT(     P1_6, 0x90, 6 )  ;
SBIT(     P1_7, 0x90, 7 )  ;

/*  SCON  0x98 */
SBIT(       RI, 0x98, 0 )  ; /* SCON.0 - RECEIVE INTERRUPT FLAG */
SBIT(      RI0, 0x98, 0 )  ; /* SCON.0 - RECEIVE INTERRUPT FLAG */
SBIT(       TI, 0x98, 1 )  ; /* SCON.1 - TRANSMIT INTERRUPT FLAG */
SBIT(      TI0, 0x98, 1 )  ; /* SCON.1 - TRANSMIT INTERRUPT FLAG */
SBIT(      RB8, 0x98, 2 )  ; /* SCON.2 - RECEIVE BIT 8 */
SBIT(     RB80, 0x98, 2 )  ; /* SCON.2 - RECEIVE BIT 8 */
SBIT(      TB8, 0x98, 3 )  ; /* SCON.3 - TRANSMIT BIT 8 */
SBIT(     TB80, 0x98, 3 )  ; /* SCON.3 - TRANSMIT BIT 8 */
SBIT(      REN, 0x98, 4 )  ; /* SCON.4 - RECEIVE ENABLE */
SBIT(     REN0, 0x98, 4 )  ; /* SCON.4 - RECEIVE ENABLE */
SBIT(      SM2, 0x98, 5 )  ; /* SCON.5 - MULTIPROCESSOR COMMUNICATION ENABLE */
SBIT(     MCE0, 0x98, 5 )  ; /* SCON.5 - MULTIPROCESSOR COMMUNICATION ENABLE */
SBIT(      SM0, 0x98, 7 )  ; /* SCON.7 - SERIAL MODE CONTROL BIT 0 */
SBIT(   S0MODE, 0x98, 7 )  ; /* SCON.7 - SERIAL MODE CONTROL BIT 0 */

/*  P2  0xA0 */
SBIT(     P2_0, 0xA0, 0 )  ;
SBIT(     P2_1, 0xA0, 1 )  ;
SBIT(     P2_2, 0xA0, 2 )  ;
SBIT(     P2_3, 0xA0, 3 )  ;
SBIT(     P2_4, 0xA0, 4 )  ;
SBIT(     P2_5, 0xA0, 5 )  ;
SBIT(     P2_6, 0xA0, 6 )  ;
SBIT(     P2_7, 0xA0, 7 )  ;

/*  IE  0xA8 */
SBIT(      EX0, 0xA8, 0 )  ; /* IE.0 - EXTERNAL INTERRUPT 0 ENABLE */
SBIT(      ET0, 0xA8, 1 )  ; /* IE.1 - TIMER 0 INTERRUPT ENABLE */
SBIT(      EX1, 0xA8, 2 )  ; /* IE.2 - EXTERNAL INTERRUPT 1 ENABLE */
SBIT(      ET1, 0xA8, 3 )  ; /* IE.3 - TIMER 1 INTERRUPT ENABLE */
SBIT(       ES, 0xA8, 4 )  ; /* IE.4 - SERIAL PORT INTERRUPT ENABLE */
SBIT(      ES0, 0xA8, 4 )  ; /* IE.4 - SERIAL PORT INTERRUPT ENABLE */
SBIT(      ET2, 0xA8, 5 )  ; /* IE.5 - TIMER 2 INTERRUPT ENABLE */
SBIT(    ESPI0, 0xA8, 6 )  ; /* IE.6 - SPI0 INTERRUPT ENABLE */
SBIT(       EA, 0xA8, 7 )  ; /* IE.7 - GLOBAL INTERRUPT ENABLE */

/*  IP  0xB8 */
SBIT(      PX0, 0xB8, 0 )  ; /* IP.0 - EXTERNAL INTERRUPT 0 PRIORITY */
SBIT(      PT0, 0xB8, 1 )  ; /* IP.1 - TIMER 0 PRIORITY */
SBIT(      PX1, 0xB8, 2 )  ; /* IP.2 - EXTERNAL INTERRUPT 1 PRIORITY */
SBIT(      PT1, 0xB8, 3 )  ; /* IP.3 - TIMER 1 PRIORITY */
SBIT(       PS, 0xB8, 4 )  ; /* IP.4 - SERIAL PORT PRIORITY */
SBIT(      PS0, 0xB8, 4 )  ; /* IP.4 - SERIAL PORT PRIORITY */
SBIT(      PT2, 0xB8, 5 )  ; /* IP.5 - TIMER 2 PRIORITY */
SBIT(    PSPI0, 0xB8, 6 )  ; /* IP.6 - SPI0 PRIORITY */

/*  SMB0CN  0xC0 */
SBIT(       SI, 0xC0, 0 )  ; /* SMB0CN.0 - SMBUS 0 INTERRUPT PENDING FLAG */
SBIT(      ACK, 0xC0, 1 )  ; /* SMB0CN.1 - SMBUS 0 ACKNOWLEDGE FLAG */
SBIT(  ARBLOST, 0xC0, 2 )  ; /* SMB0CN.2 - SMBUS 0 ARBITRATION LOST INDICATOR */
SBIT(    ACKRQ, 0xC0, 3 )  ; /* SMB0CN.3 - SMBUS 0 ACKNOWLEDGE REQUEST */
SBIT(      STO, 0xC0, 4 )  ; /* SMB0CN.4 - SMBUS 0 STOP FLAG */
SBIT(      STA, 0xC0, 5 )  ; /* SMB0CN.5 - SMBUS 0 START FLAG */
SBIT(   TXMODE, 0xC0, 6 )  ; /* SMB0CN.6 - SMBUS 0 TRANSMIT MODE INDICATOR */
SBIT(   MASTER, 0xC0, 7 )  ; /* SMB0CN.7 - SMBUS 0 MASTER/SLAVE INDICATOR */

/*  TMR2CN  0xC8 */
SBIT(   T2XCLK, 0xC8, 0 )  ; /* TMR2CN.0 - TIMER 2 EXTERNAL CLOCK SELECT */
SBIT(      TR2, 0xC8, 2 )  ; /* TMR2CN.2 - TIMER 2 ON/OFF CONTROL */
SBIT(  T2SPLIT, 0xC8, 3 )  ; /* TMR2CN.3 - TIMER 2 SPLIT MODE ENABLE */
SBIT(   TF2CEN, 0xC8, 4 )  ; /* TMR2CN.4 - TIMER 2 LOW-FREQ OSC CAPTURE ENABLE*/
SBIT(   TF2LEN, 0xC8, 5 )  ; /* TMR2CN.5 - TIMER 2 LOW BYTE INTERRUPT ENABLE */
SBIT(     TF2L, 0xC8, 6 )  ; /* TMR2CN.6 - TIMER 2 LOW BYTE OVERFLOW FLAG */
SBIT(      TF2, 0xC8, 7 )  ; /* TMR2CN.7 - TIMER 2 OVERFLOW FLAG */
SBIT(     TF2H, 0xC8, 7 )  ; /* TMR2CN.7 - TIMER 2 HIGH BYTE OVERFLOW FLAG */

/*  PSW  0xD0 */
SBIT(   PARITY, 0xD0, 0 )  ; /* PSW.0 - ACCUMULATOR PARITY FLAG */
SBIT(       F1, 0xD0, 1 )  ; /* PSW.1 - FLAG 1 */
SBIT(       OV, 0xD0, 2 )  ; /* PSW.2 - OVERFLOW FLAG */
SBIT(      RS0, 0xD0, 3 )  ; /* PSW.3 - REGISTER BANK SELECT 0 */
SBIT(      RS1, 0xD0, 4 )  ; /* PSW.4 - REGISTER BANK SELECT 1 */
SBIT(       F0, 0xD0, 5 )  ; /* PSW.5 - FLAG 0 */
SBIT(       AC, 0xD0, 6 )  ; /* PSW.6 - AUXILIARY CARRY FLAG */
SBIT(       CY, 0xD0, 7 )  ; /* PSW.7 - CARRY FLAG */

/*  PCA0CN  0xD8 */
SBIT(     CCF0, 0xD8, 0 )  ; /* PCA0CN.0 - PCA MODULE 0 CAPTURE/COMPARE FLAG */
SBIT(     CCF1, 0xD8, 1 )  ; /* PCA0CN.1 - PCA MODULE 1 CAPTURE/COMPARE FLAG */
SBIT(     CCF2, 0xD8, 2 )  ; /* PCA0CN.2 - PCA MODULE 2 CAPTURE/COMPARE FLAG */
SBIT(       CR, 0xD8, 6 )  ; /* PCA0CN.6 - PCA COUNTER/TIMER RUN CONTROL */
SBIT(       CF, 0xD8, 7 )  ; /* PCA0CN.7 - PCA COUNTER/TIMER OVERFLOW FLAG */

/*  ADC0CN  0xE8 */
SBIT(   AD0CM0, 0xE8, 0 )  ; /* ADC0CN.0 - ADC 0 START OF CONV. MODE BIT 0 */
SBIT(   AD0CM1, 0xE8, 1 )  ; /* ADC0CN.1 - ADC 0 START OF CONV. MODE BIT 1 */
SBIT(   AD0CM2, 0xE8, 2 )  ; /* ADC0CN.2 - ADC 0 START OF CONV. MODE BIT 2 */
SBIT(  AD0WINT, 0xE8, 3 )  ; /* ADC0CN.3 - ADC 0 WINDOW COMPARE INT. FLAG */
SBIT(  AD0BUSY, 0xE8, 4 )  ; /* ADC0CN.4 - ADC 0 BUSY FLAG */
SBIT(   AD0INT, 0xE8, 5 )  ; /* ADC0CN.5 - ADC 0 CONV. COMPLETE INT. FLAG */
SBIT(    AD0TM, 0xE8, 6 )  ; /* ADC0CN.6 - ADC 0 TRACK MODE */
SBIT(    AD0EN, 0xE8, 7 )  ; /* ADC0CN.7 - ADC 0 ENABLE */

/*  SPI0CN  0xF8 */
SBIT(    SPIEN, 0xF8, 0 )  ; /* SPI0CN.0 - SPI0 ENABLE */
SBIT(    TXBMT, 0xF8, 1 )  ; /* SPI0CN.1 - TRANSMIT BUFFER EMPTY */
SBIT(   NSSMD0, 0xF8, 2 )  ; /* SPI0CN.2 - SLAVE SELECT MODE BIT 0 */
SBIT(   NSSMD1, 0xF8, 3 )  ; /* SPI0CN.3 - SLAVE SELECT MODE BIT 1 */
SBIT(   RXOVRN, 0xF8, 4 )  ; /* SPI0CN.4 - RECEIVE OVERRUN FLAG */
SBIT(     MODF, 0xF8, 5 )  ; /* SPI0CN.5 - MODE FAULT FLAG */
SBIT(     WCOL, 0xF8, 6 )  ; /* SPI0CN.6 - WRITE COLLISION FLAG */
SBIT(     SPIF, 0xF8, 7 )  ; /* SPI0CN.7 - SPI0 INTERRUPT FLAG */


/* Predefined SFR Bit Masks */

#define PCON_IDLE         0x01    /* PCON                                */
#define PCON_STOP         0x02    /* PCON                                */
#define T1M               0x08    /* CKCON                               */
#define PSWE              0x01    /* PSCTL                               */
#define PSEE              0x02    /* PSCTL                               */
#define ECP0              0x20    /* EIE1                                */
#define PORSF             0x02    /* RSTSRC                              */
#define SWRSF             0x10    /* RSTSRC                              */
#define ECCF              0x01    /* PCA0CPMn                            */
#define PWM               0x02    /* PCA0CPMn                            */
#define TOG               0x04    /* PCA0CPMn                            */
#define MAT               0x08    /* PCA0CPMn                            */
#define CAPN              0x10    /* PCA0CPMn                            */
#define CAPP              0x20    /* PCA0CPMn                            */
#define ECOM              0x40    /* PCA0CPMn                            */
#define PWM16             0x80    /* PCA0CPMn                            */
#define CP0E              0x10    /* XBR0                                */
#define CP0OEN            0x10    /* XBR0                                */
#define CP0AE             0x20    /* XBR0                                */
#define CP0AOEN           0x20    /* XBR0                                */

/* Interrupts */

#define INT_EXT0          0       /* External Interrupt 0 */
#define INT_TIMER0        1       /* Timer0 Overflow */
#define INT_EXT1          2       /* External Interrupt 1 */
#define INT_TIMER1        3       /* Timer1 Overflow */
#define INT_UART0         4       /* Serial Port 0 */
#define INT_TIMER2        5       /* Timer2 Overflow */
#define INT_SPI0          6       /* Serial Peripheral Interface 0 */
#define INT_SMBUS0        7       /* SMBus0 Interface */
#define INT_PMAT          8       /* Port match */
#define INT_ADC0_WINDOW   9       /* ADC0 Window Comparison */
#define INT_ADC0_EOC      10      /* ADC0 End Of Conversion */
#define INT_PCA0          11      /* PCA0 Peripheral */
#define INT_COMPARATOR0   12      /* Comparator0 */
/*                        13         Reserved */
#define INT_TIMER3        14      /* Timer3 Overflow */

#endif
