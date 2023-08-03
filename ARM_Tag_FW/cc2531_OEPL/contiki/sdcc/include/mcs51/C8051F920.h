/*-------------------------------------------------------------------------
   C8051F920.h -Register Declarations for the SiLabs C8051F92x-93x
   Processor Range

   Copyright (C) 2009, Steven Borley, steven.borley@partnerelectronics.com

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

#ifndef C8051F920_H
#define C8051F920_H

#include <compiler.h>


/*  BYTE Registers  */

/* Page 0x00 (and all pages) */
SFR(       P0, 0x80 )  ; /* PORT 0 */
SFR(       SP, 0x81 )  ; /* STACK POINTER */
SFR(      DPL, 0x82 )  ; /* DATA POINTER - LOW BYTE */
SFR(      DPH, 0x83 )  ; /* DATA POINTER - HIGH BYTE */
SFR(  SPI1CFG, 0x84 )  ; /* SPI1 Configuration */
SFR(  SPI1CKR, 0x85 )  ; /* SPI1 Clock Rate Control */
SFR(  SPI1DAT, 0x86 )  ; /* SPI1 Data */
SFR(     PCON, 0x87 )  ; /* POWER CONTROL */
SFR(     TCON, 0x88 )  ; /* TIMER CONTROL */
SFR(     TMOD, 0x89 )  ; /* TIMER MODE */
SFR(      TL0, 0x8A )  ; /* TIMER 0 - LOW BYTE */
SFR(      TL1, 0x8B )  ; /* TIMER 1 - LOW BYTE */
SFR(      TH0, 0x8C )  ; /* TIMER 0 - HIGH BYTE */
SFR(      TH1, 0x8D )  ; /* TIMER 1 - HIGH BYTE */
SFR(    CKCON, 0x8E )  ; /* CLOCK CONTROL */
SFR(    PSCTL, 0x8F )  ; /* Program Store R/W Control */
SFR(       P1, 0x90 )  ; /* PORT 1 */
SFR(   TMR3CN, 0x91 )  ; /* TIMER 3 CONTROL */
SFR(  TMR3RLL, 0x92 )  ; /* TIMER 3 CAPTURE REGISTER - LOW BYTE */
SFR(  TMR3RLH, 0x93 )  ; /* TIMER 3 CAPTURE REGISTER - HIGH BYTE */
SFR(    TMR3L, 0x94 )  ; /* TIMER 3 - LOW BYTE */
SFR(    TMR3H, 0x95 )  ; /* TIMER 3 - HIGH BYTE */
SFR(    DC0CF, 0x96 )  ; /* DC0 (DC-DC Converter) Configuration  */
SFR(    DC0CN, 0x97 )  ; /* DC0 (DC-DC Converter) Control  */
SFR(    SCON0, 0x98 )  ; /* Serial Port Control */
SFR(    SBUF0, 0x99 )  ; /* Serial Port Buffer */
SFR(   CPT1CN, 0x9A )  ; /* Comparator 1 Control  */
SFR(   CPT0CN, 0x9B )  ; /* Comparator 0 Control  */
SFR(   CPT1MD, 0x9C )  ; /* Comparator 1 Mode Selection  */
SFR(   CPT0MD, 0x9D )  ; /* Comparator 0 Mode Selection */
SFR(   CPT1MX, 0x9E )  ; /* Comparator 1 mux selection */
SFR(   CPT0MX, 0x9F )  ; /* Comparator 0 mux selection */
SFR(       P2, 0xA0 )  ; /* PORT 2 */
SFR(  SPI0CFG, 0xA1 )  ; /* SPI0 CONFIGURATION */
SFR(  SPI0CKR, 0xA2 )  ; /* SPI0 Clock Rate Control */
SFR(  SPI0DAT, 0xA3 )  ; /* SPI0 Data */
SFR(  P0MDOUT, 0xA4 )  ; /* PORT 0 OUTPUT MODE CONFIGURATION */
SFR(  P1MDOUT, 0xA5 )  ; /* PORT 1 OUTPUT MODE CONFIGURATION */
SFR(  P2MDOUT, 0xA6 )  ; /* PORT 2 OUTPUT MODE CONFIGURATION */
SFR(  SFRPAGE, 0xA7 )  ; /* SFR Page */
SFR(       IE, 0xA8 )  ; /* INTERRUPT ENABLE */
SFR(   CLKSEL, 0xA9 )  ; /* SYSTEM CLOCK SELECT */
SFR(   EMI0CN, 0xAA )  ; /* EMIF Control */
SFR(   _XPAGE, 0xAA )  ; /* XDATA/PDATA page alias for SDCC */
SFR(   EMI0CF, 0xAB )  ; /* EMIF Configuration  */
SFR(  RTC0ADR, 0xAC )  ; /* RTC0 Address  */
SFR(  RTC0DAT, 0xAD )  ; /* RTC0 Data */
SFR(  RTC0KEY, 0xAE )  ; /* RTC0 Key */
SFR(   EMI0TC, 0xAF )  ; /* EMIF Timing Control */
SFR(   SPI1CN, 0xB0 )  ; /* SPI1 Control  */
SFR(   OSCXCN, 0xB1 )  ; /* EXTERNAL OSCILLATOR CONTROL */
SFR(   OSCICN, 0xB2 )  ; /* INTERNAL OSCILLATOR CONTROL */
SFR(   OSCICL, 0xB3 )  ; /* INTERNAL OSCILLATOR CALIBRATION */
SFR(   PMU0CF, 0xB5 )  ; /* PMU0 Configuration */
SFR(    FLSCL, 0xB6 )  ; /* IFlash Scale  */
SFR(    FLKEY, 0xB7 )  ; /* Flash Lock And Key */
SFR(       IP, 0xB8 )  ; /* INTERRUPT PRIORITY */
SFR(  IREF0CN, 0xB9 )  ; /* Current Reference IREF Control  */
SFR(   ADC0AC, 0xBA )  ; /* ADC0 Accumulator Configuration  */
SFR(   ADC0MX, 0xBB )  ; /* AMUX0 Channel Select  */
SFR(   ADC0CF, 0xBC )  ; /* ADC 0 CONFIGURATION */
SFR(    ADC0L, 0xBD )  ; /* ADC 0 DATA - LOW BYTE */
SFR(    ADC0H, 0xBE )  ; /* ADC 0 DATA - HIGH BYTE */
SFR(   P1MASK, 0xBF )  ; /* Port 1 Mask */
SFR(   SMB0CN, 0xC0 )  ; /* SMBUS CONTROL */
SFR(   SMB0CF, 0xC1 )  ; /* SMBUS CONFIGURATION */
SFR(  SMB0DAT, 0xC2 )  ; /* SMBUS DATA */
SFR(  ADC0GTL, 0xC3 )  ; /* ADC 0 GREATER-THAN REGISTER - LOW BYTE */
SFR(  ADC0GTH, 0xC4 )  ; /* ADC 0 GREATER-THAN REGISTER - HIGH BYTE */
SFR(  ADC0LTL, 0xC5 )  ; /* ADC 0 LESS-THAN REGISTER - LOW BYTE */
SFR(  ADC0LTH, 0xC6 )  ; /* ADC 0 LESS-THAN REGISTER - HIGH BYTE */
SFR(   P0MASK, 0xC7 )  ; /* Port 0 Mask */
SFR(   TMR2CN, 0xC8 )  ; /* Timer 2 control  */
SFR(   REG0CN, 0xC9 )  ; /* Voltage Regulator (VREG0) Control */
SFR(  TMR2RLL, 0xCA )  ; /* Timer 2 capture register - low byte  */
SFR(  TMR2RLH, 0xCB )  ; /* Timer 2 capture register - high byte */
SFR(    TMR2L, 0xCC )  ; /* Timer 2 - low byte*/
SFR(    TMR2H, 0xCD )  ; /* Timer 2 - high byte */
SFR( PCA0CPM5, 0xCE )  ; /* PCA0 Module 5 Mode Register  */
SFR(    P1MAT, 0xCF )  ; /* Port 1 Match  */
SFR(      PSW, 0xD0 )  ; /* PROGRAM STATUS WORD */
SFR(   REF0CN, 0xD1 )  ; /* VOLTAGE REFERENCE 0 CONTROL */
SFR( PCA0CPL5, 0xD2 )  ; /* PCA0 Capture 5 Low */
SFR( PCA0CPH5, 0xD3 )  ; /* PCA0 Capture 5 High */
SFR(   P0SKIP, 0xD4 )  ; /* PORT 0 SKIP */
SFR(   P1SKIP, 0xD5 )  ; /* PORT 1 SKIP */
SFR(   P2SKIP, 0xD6 )  ; /* PORT 2 SKIP */
SFR(    P0MAT, 0xD7 )  ; /* Port 0 Match */
SFR(   PCA0CN, 0xD8 )  ; /* PCA CONTROL */
SFR(   PCA0MD, 0xD9 )  ; /* PCA MODE */
SFR( PCA0CPM0, 0xDA )  ; /* PCA0 Module 0 Mode Register */
SFR( PCA0CPM1, 0xDB )  ; /* PCA0 Module 1 Mode Register */
SFR( PCA0CPM2, 0xDC )  ; /* PCA0 Module 2 Mode Register  */
SFR( PCA0CPM3, 0xDD )  ; /* PCA0 Module 3 Mode Register */
SFR( PCA0CPM4, 0xDE )  ; /* PCA0 Module 4 Mode Register  */
SFR(  PCA0PWM, 0xDF )  ; /* PCA0 PWM Configuration  */
SFR(      ACC, 0xE0 )  ; /* ACCUMULATOR */
SFR(     XBR0, 0xE1 )  ; /* Port Mux Configuration Register 0 */
SFR(     XBR1, 0xE2 )  ; /* Port Mux Configuration Register 1 */
SFR(     XBR2, 0xE3 )  ; /* Port Mux Configuration Register 2 */
SFR(  INT01CF, 0xE4 )  ; /* INT0/INT1 Configuration Register */
SFR(     EIE1, 0xE6 )  ; /* EXTERNAL INTERRUPT ENABLE 1 */
SFR(     EIE2, 0xE7 )  ; /* EXTERNAL INTERRUPT ENABLE 2 */
SFR(   ADC0CN, 0xE8 )  ; /* ADC 0 CONTROL */
SFR( PCA0CPL1, 0xE9 )  ; /* PCA CAPTURE 1 LOW */
SFR( PCA0CPH1, 0xEA )  ; /* PCA CAPTURE 1 HIGH */
SFR( PCA0CPL2, 0xEB )  ; /* PCA CAPTURE 2 LOW */
SFR( PCA0CPH2, 0xEC )  ; /* PCA CAPTURE 2 HIGH */
SFR( PCA0CPL3, 0xED )  ; /* PCA0 Capture 3 Low */
SFR( PCA0CPH3, 0xEE )  ; /* PCA0 Capture 3 High */
SFR(   RSTSRC, 0xEF )  ; /* RESET SOURCE */
SFR(        B, 0xF0 )  ; /* B REGISTER */
SFR(   P0MDIN, 0xF1 )  ; /* Port 0 Input Mode Configuration */
SFR(   P1MDIN, 0xF2 )  ; /* Port 1 Input Mode Configuration  */
SFR(   P2MDIN, 0xF3 )  ; /* Port 2 Input Mode Configuration  */
SFR(  SMB0ADR, 0xF4 )  ; /* SMBus Slave Address */
SFR(  SMB0ADM, 0xF5 )  ; /* SMBus Slave Address Mask  */
SFR(     EIP1, 0xF6 )  ; /* EXTERNAL INTERRUPT PRIORITY REGISTER 1 */
SFR(     EIP2, 0xF7 )  ; /* EXTERNAL INTERRUPT PRIORITY REGISTER 2 */
SFR(   SPI0CN, 0xF8 )  ; /* SPI0 Control  */
SFR(    PCA0L, 0xF9 )  ; /* PCA COUNTER LOW */
SFR(    PCA0H, 0xFA )  ; /* PCA COUNTER HIGH */
SFR( PCA0CPL0, 0xFB )  ; /* PCA CAPTURE 0 LOW */
SFR( PCA0CPH0, 0xFC )  ; /* PCA CAPTURE 0 HIGH */
SFR( PCA0CPL4, 0xFD )  ; /* PCA0 Capture 4 Low */
SFR( PCA0CPH4, 0xFE )  ; /* PCA0 Capture 4 High */
SFR(   VDM0CN, 0xFF )  ; /* VDD Monitor Control */

/* Page 0x0F only */
SFR(    TOFFL, 0x85 )  ; /* Temperature Offset Low */
SFR(    TOFFH, 0x86 )  ; /* Temperature Offset High */
SFR(  CRC0DAT, 0x91 )  ; /* CRC0 Data  */
SFR(   CRC0CN, 0x92 )  ; /* CRC0 Control */
SFR(   CRC0IN, 0x93 )  ; /* CRC0 Input */
SFR( CRC0FLIP, 0x95 )  ; /* CRC0 Flip */
SFR( CRC0AUTO, 0x96 )  ; /* CRC0 Automatic Control */
SFR(  CRC0CNT, 0x97 )  ; /* CRC0 Automatic Flash Sector Count  */
SFR(    P0DRV, 0xA4 )  ; /* Port 0 Drive Strength */
SFR(    P1DRV, 0xA5 )  ; /* Port 1 Drive Strength */
SFR(    P2DRV, 0xA6 )  ; /* Port 2 Drive Strength */
SFR(  ADC0PWR, 0xBA)  ; /* ADC0 Burst Mode Power-Up Time */
SFR(   ADC0TK, 0xBD)  ; /* ADC0 Tracking Control */


/*  WORD/DWORD Registers  */

/* page 0x00 */
SFR16E(     TMR0, 0x8C8A )  ; /* TIMER 0 COUNTER */
SFR16E(     TMR1, 0x8D8B )  ; /* TIMER 1 COUNTER */
SFR16E(   TMR3RL, 0x9392 )  ; /* Timer 3 reload word */
SFR16E(     TMR3, 0x9594 )  ; /* Timer 3 counter word */
SFR16E(     ADC0, 0xBEBD )  ; /* ADC0 word */
SFR16E(   ADC0GT, 0xC4C3 )  ; /* ADC 0 GREATER-THAN REGISTER WORD */
SFR16E(   ADC0LT, 0xC6C5 )  ; /* ADC 0 LESS-THAN REGISTER WORD */
SFR16E(   TMR2RL, 0xCBCA )  ; /* Timer 2 reload word */
SFR16E(     TMR2, 0xCDCC )  ; /* Timer 2 counter word */
SFR16E(   TMR2RL, 0xCBCA )  ; /* Timer 2 Reload word */
SFR16E(     PCA0, 0xFAF9 )  ; /* PCA0 counter word */
SFR16E(  PCA0CP0, 0xFCFB )  ; /* PCA0 Capture 0 word */
SFR16E(  PCA0CP1, 0xEAE9 )  ; /* PCA0 Capture 1 word */
SFR16E(  PCA0CP2, 0xECEB )  ; /* PCA0 Capture 2 word */
SFR16E(  PCA0CP3, 0xEEED )  ; /* PCA0 Capture 3 word */
SFR16E(  PCA0CP4, 0xFEFD )  ; /* PCA0 Capture 4 word */
SFR16E(  PCA0CP5, 0xD3D4 )  ; /* PCA0 Capture 5 word */

/* Page 0x0F */
SFR16E(     TOFF, 0x8685 )  ; /* TEMPERATURE SENSOR OFFSET WORD */

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

/*  SCON0  0x98 */
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
SBIT(    IEGF0, 0xA8, 6 )  ; /* IE.6 - GENERAL PURPOSE FLAG 0 */
SBIT(       EA, 0xA8, 7 )  ; /* IE.7 - GLOBAL INTERRUPT ENABLE */

/* SPI1CN 0xB0 */
SBIT(   SPI1EN, 0xB0, 0 )  ; /* SPI1 Enable */
SBIT(   TXBMT1, 0xB0, 1 )  ; /* SPI1 Transmit Buffer Empty */
SBIT(  NSS1MD0, 0xB0, 2 )  ; /* SPI1 Slave Select Mode bit-0 */
SBIT(  NSS1MD1, 0xB0, 3 )  ; /* SPI1 Slave Select Mode bit-1 */
SBIT(  RXOVRN1, 0xB0, 4 )  ; /* SPI1 Receive Overrun Flag */
SBIT(    MODF1, 0xB0, 5 )  ; /* SPI1 Mode Fault Flag */
SBIT(    WCOL1, 0xB0, 6 )  ; /* SPI1 Write Collision Flag */
SBIT(    SPIF1, 0xB0, 7 )  ; /* SPI1 Interrupt Flag */

/*  IP  0xB8 */
SBIT(      PX0, 0xB8, 0 )  ; /* IP.0 - EXTERNAL INTERRUPT 0 PRIORITY */
SBIT(      PT0, 0xB8, 1 )  ; /* IP.1 - TIMER 0 PRIORITY */
SBIT(      PX1, 0xB8, 2 )  ; /* IP.2 - EXTERNAL INTERRUPT 1 PRIORITY */
SBIT(      PT1, 0xB8, 3 )  ; /* IP.3 - TIMER 1 PRIORITY */
SBIT(       PS, 0xB8, 4 )  ; /* IP.4 - SERIAL PORT PRIORITY */
SBIT(      PS0, 0xB8, 4 )  ; /* IP.4 - SERIAL PORT PRIORITY */
SBIT(      PT2, 0xB8, 5 )  ; /* IP.5 - TIMER 2 PRIORITY */

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
SBIT(   SPI0EN, 0xF8, 0 )  ; /* SPI0 Enable */
SBIT(   TXBMT0, 0xF8, 1 )  ; /* SPI0 Transmit Buffer Empty */
SBIT(  NSS0MD0, 0xF8, 2 )  ; /* SPI0 Slave Select Mode bit-0 */
SBIT(  NSS0MD1, 0xF8, 3 )  ; /* SPI0 Slave Select Mode bit-1 */
SBIT(  RXOVRN0, 0xF8, 4 )  ; /* SPI0 Receive Overrun Flag */
SBIT(    MODF0, 0xF8, 5 )  ; /* SPI0 Mode Fault Flag */
SBIT(    WCOL0, 0xF8, 6 )  ; /* SPI0 Write Collision Flag */
SBIT(    SPIF0, 0xF8, 7 )  ; /* SPI0 Interrupt Flag */


/* Indirectly accessed registers */

/* smaRTClock Internal Registers */
#define CAPTURE0         0x00    /* smaRTClock Capture register 0 */
#define CAPTURE1         0x01    /* smaRTClock Capture register 1 */
#define CAPTURE2         0x02    /* smaRTClock Capture register 2 */
#define CAPTURE3         0x03    /* smaRTClock Capture register 3 */
#define RTC0CN           0x04    /* smaRTClock Control */
#define RTC0XCN          0x05    /* smaRTClock Oscillator Control */
#define RTC0XCF          0x06    /* smaRTClock Oscillator Configuration */
#define RTC0PIN          0x07    /* smaRTClock Pin Configuration */
#define ALARM0           0x08    /* smaRTClock Alarm Register 0 */
#define ALARM1           0x09    /* smaRTClock Alarm Register 1 */
#define ALARM2           0x0A    /* smaRTClock Alarm Register 2 */
#define ALARM3           0x0B    /* smaRTClock Alarm Register 3 */


/* Predefined SFR Bit Masks */

/* PCON 0x87 */
#define PCON_IDLE       (1<<0)    /* PCON                                */
#define PCON_STOP       (1<<1)    /* PCON                                */

/* CKON 0x8E  */
#define T0M             (1<<2)    /* CKCON  Timer 0 Clock Select         */
#define T1M             (1<<3)    /* CKCON  Timer 1 Clock Select         */

/* PSCTL 0x8F */
#define PSWE            (1<<0)    /* Program Store Write Enable          */
#define PSEE            (1<<1)    /* Program Store Erase Enable          */
#define SFLE            (1<<2)    /* Scratchpad Flash Access Enable      */

/* EIE1 0xE6 */
#define ESMB0           (1<<0)    /* Enable SMBus (SMB0) Interrupt       */
#define ERTC0A          (1<<1)    /* Enable smaRTClock Alarm Interrupts  */
#define EWADC0          (1<<2)    /* Enable Window Comparison ADC0 Int.  */
#define EADC0           (1<<3)    /* Enable ADC0 Convert Complete Int.   */
#define EPCA0           (1<<4)    /* Enable PCA0 Interrupt               */
#define ECP0            (1<<5)    /* Enable Comparator0 (CP0) Interrupt  */
#define ECP1            (1<<6)    /* Enable Comparator1 (CP1) Interrupt  */
#define ET3             (1<<7)    /* Enable Timer 3 Interrupt            */

/* RSTSRC */
#define PINRSF          (1<<0)    /* HW Pin Reset Flag                   */
#define PORSF           (1<<1)    /* Power-on/fail Reset Rlag            */
#define MCDRSF          (1<<2)    /* Missing Clock Detector Reset Rlag   */
#define WDTRSF          (1<<3)    /* Watchdog Timer Reset Rlag           */
#define SWRSF           (1<<4)    /* Software Force/Reset Rlag           */
#define C0RSEF          (1<<5)    /* Comparator0 Reset Rlag              */
#define FERROR          (1<<6)    /* Flash Error Reset Rlag              */
#define RTC0RE          (1<<7)    /* smaRTClock Reset Rlag               */

/* PCA0CPMn */
#define ECCF            (1<<0)    /* Capture/Compare Flag Interrupt En.  */
#define PWM             (1<<1)    /* Pulse Width Modulation Mode Enable  */
#define TOG             (1<<2)    /* Toggle Function Enable              */
#define MAT             (1<<3)    /* Match Function Enable               */
#define CAPN            (1<<4)    /* Capture Negative Function Enable    */
#define CAPP            (1<<5)    /* Capture Positive Function Enable.   */
#define ECOM            (1<<6)    /* Comparator Function Enable.         */
#define PWM16           (1<<7)    /* 16-bit Pulse Width Modulation Enable*/

/* XBR0 0xE1 */
#define URT0E           (1<<0)    /* UART0 I/O enable                    */
#define SPI0E           (1<<1)    /* SPI0 I/O Enable                     */
#define SMB0E           (1<<2)    /* SMBus I/O Enable                    */
#define SYSCKE          (1<<3)    /* SYSCLK Output Enable.               */
#define CP0E            (1<<4)    /* Comparator0 Output Enable           */
#define CP0AE           (1<<5)    /* Comparator0 Asynchronous Output En. */
#define CP1E            (1<<6)    /* Comparator1 Output Enable           */
#define CP1AE           (1<<7)    /* Comparator1 Asynchronous Output En. */

/* XBR1 0xE2 */
#define PCA0ME0         (1<<0)    /* PCA0 Module I/O Enable bit-0        */
#define PCA0ME1         (1<<1)    /* PCA0 Module I/O Enable bit-1        */
#define PCA0ME2         (1<<2)    /* PCA0 Module I/O Enable bit-2        */
#define ECIE            (1<<3)    /* PCA0 Ext. Counter Input Enable      */
#define T0E             (1<<4)    /* Timer0 Input Enable                 */
#define T1E             (1<<5)    /* Timer1 Input Enable                 */
#define SPI1E           (1<<6)    /* SPI1 I/O Enable                     */

/* XBR2 0xE3 */
#define XBARE           (1<<6)    /* Crossbar Enable                     */
#define WEAKPUD         (1<<7)    /* Port I/O Weak Pullup Disable        */

/* Interrupts */

#define INT_EXT0          0       /* External Interrupt 0*/
#define INT_TIMER0        1       /* Timer0 Overflow */
#define INT_EXT1          2       /* External Interrupt 1 */
#define INT_TIMER1        3       /* Timer1 Overflow */
#define INT_UART0         4       /* Serial Port 0 */
#define INT_TIMER2        5       /* Timer2 Overflow */
#define INT_SPI0          6       /* SPI0 */
#define INT_SMBUS0        7       /* SMBus0 Interface */
#define INT_ALARM         8       /* smaRTClock Alarm  */
#define INT_ADC0_WINDOW   9       /* ADC0 Window Comparison */
#define INT_ADC0_EOC     10       /* ADC0 End Of Conversion */
#define INT_PCA0         11       /* PCA0 Peripheral */
#define INT_CP0          12       /* Comparator 0 */
#define INT_CP1          13       /* Comparator 1 */
#define INT_TIMER3       14       /* Timer3 Overflow */
#define INT_VWARN        15       /* VDD/DC+ Supply Monitor early warning */
#define INT_MATCH        16       /* Port Match */
#define INT_OSCFAIL      17       /* smaRTClock Oscillator Fail */
#define INT_SPI1         18       /* SPI1 */

/* aliases - these map alternative names to names use in the datasheet */
#define SCON             SCON0    /* Serial Port Control */
#define SBUF             SBUF0    /* Serial Port Buffer */
#define T2CON            TMR2CN	  /* Timer 2 control */
#define RCAP2            TMR2RL   /* Timer 2 capture register word */
#define RCAP2L           TMR2RLL  /* Timer 2 capture register - low byte */
#define RCAP2H           TMR2RLH  /* Timer 2 capture register - high byte */
#define T2               TMR2     /* Timer 2 - word */
#define TL2              TMR2L    /* Timer 2 - low byte */
#define TH2              TMR2H    /* Timer 2 - high byte */
#define PRT0MX           XBR0     /* Port Mux Configuration Register 0 */
#define PRT1MX           XBR1	  /* Port Mux Configuration Register 1 */
#define PRT2MX           XBR2	  /* Port Mux Configuration Register 2 */
#define IT01CF           INT01CF  /* INT0/INT1 Configuration Register */
#define P0MODE           P0MDIN   /* Port 0 Input Mode Configuration */
#define P1MODE           P1MDIN   /* Port 1 Input Mode Configuration */
#define P2MODE           P2MDIN   /* Port 2 Input Mode Configuration */
#define CP0OEN           CP0E     /* Comparator 0 Output Enable bit */
#define CP0AOEN          CP0AE    /* Comparator 0 Asynchronous Output En. bit */


#endif /* C8051F920_H */
