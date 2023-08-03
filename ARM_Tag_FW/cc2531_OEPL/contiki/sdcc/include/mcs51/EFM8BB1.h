/*-------------------------------------------------------------------------
 *   EFM8BB1.h - Register Declarations for the SiLabs EFM8BB1 Processor
 *   Range
 *
 *   Copyright (C) 2015, Kharitonov Dmitriy, kharpost@altlinux.org
 *
 *   This library is free software; you can redistribute it and/or modify it
 *   under the terms of the GNU General Public License as published by the
 *   Free Software Foundation; either version 2, or (at your option) any
 *   later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this library; see the file COPYING. If not, write to the
 *   Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
 *   MA 02110-1301, USA.
 *
 *   As a special exception, if you link this library with other files,
 *   some of which are compiled with SDCC, to produce an executable,
 *   this library does not by itself cause the resulting executable to
 *   be covered by the GNU General Public License. This exception does
 *   not however invalidate any other reasons why the executable file
 *   might be covered by the GNU General Public License.
 * -------------------------------------------------------------------------*/
#ifndef EFM8BB1_H
#define EFM8BB1_H

/* Supported Devices:
  EFM8BB10F2G
  EFM8BB10F4G
  EFM8BB10F8G
*/

#include <compiler.h>

/*  BYTE Registers  */

SFR(       P0, 0x80 ); /* PORT 0 */
SFR(       SP, 0x81 ); /* STACK POINTER */
SFR(      DPL, 0x82 ); /* DATA POINTER - LOW BYTE */
SFR(      DPH, 0x83 ); /* DATA POINTER - HIGH BYTE */
SFR(    PCON0, 0x87 ); /* POWER CONTROL */
SFR(     TCON, 0x88 ); /* TIMER CONTROL */
SFR(     TMOD, 0x89 ); /* TIMER MODE */
SFR(      TL0, 0x8A ); /* TIMER 0 - LOW BYTE */
SFR(      TL1, 0x8B ); /* TIMER 1 - LOW BYTE */
SFR(      TH0, 0x8C ); /* TIMER 0 - HIGH BYTE */
SFR(      TH1, 0x8D ); /* TIMER 1 - HIGH BYTE */
SFR(   CKCON0, 0x8E ); /* CLOCK CONTROL */
SFR(    PSCTL, 0x8F ); /* PROGRAM STORE R/W CONTROL */
SFR(       P1, 0x90 ); /* PORT 1 */
SFR(  TMR3CN0, 0x91 ); /* TIMER 3 CONTROL */
SFR(  TMR3RLL, 0x92 ); /* TIMER 3 CAPTURE REGISTER - LOW BYTE */
SFR(  TMR3RLH, 0x93 ); /* TIMER 3 CAPTURE REGISTER - HIGH BYTE */
SFR(    TMR3L, 0x94 ); /* TIMER 3 - LOW BYTE */
SFR(    TMR3H, 0x95 ); /* TIMER 3 - HIGH BYTE */
SFR(  PCA0POL, 0x96 ); /* PCA Output Polarity */
SFR(    WDTCN, 0x97 ); /* Watchdog Timer Control*/
SFR(    SCON0, 0x98 ); /* SERIAL PORT CONTROL */
SFR(    SBUF0, 0x99 ); /* SERIAL PORT BUFFER */
SFR(  CMP0CN0, 0x9B ); /* COMPARATOR 0 CONTROL 0 */
SFR(  PCA0CLR, 0x9C ); /* PCA Comparator Clear Control */
SFR(   CMP0MD, 0x9D ); /* Comparator 0 Mode */
SFR( PCA0CENT, 0x9E ); /* PCA Center Alignment Enable */
SFR(   CMP0MX, 0x9F ); /* Comparator 0 Multiplexer Selection */
SFR(       P2, 0xA0 ); /* PORT 2 */
SFR(  SPI0CFG, 0xA1 ); /* SPI0 CONFIGURATION */
SFR(  SPI0CKR, 0xA2 ); /* SPI0 CLOCK RATE CONTROL */
SFR(  SPI0DAT, 0xA3 ); /* SPI0 DATA */
SFR(  P0MDOUT, 0xA4 ); /* PORT 0 OUTPUT MODE CONFIGURATION */
SFR(  P1MDOUT, 0xA5 ); /* PORT 1 OUTPUT MODE CONFIGURATION */
SFR(  P2MDOUT, 0xA6 ); /* PORT 2 OUTPUT MODE CONFIGURATION */
SFR(       IE, 0xA8 ); /* INTERRUPT ENABLE */
SFR(   CLKSEL, 0xA9 ); /* SYSTEM CLOCK SELECT */
SFR(   CMP1MX, 0xAA ); /* Comparator 1 Multiplexer Selection */
SFR(   CMP1MD, 0xAB ); /* Comparator 1 Mode */
SFR(   SMB0TC, 0xAC ); /* SMBus 0 Timing and Pin Control */
SFR(  DERIVID, 0xAD ); /* Derivative Identification */
SFR(   LFO0CN, 0xB1 ); /* Low Frequency Oscillator Control */
SFR(  ADC0CN1, 0xB2 ); /* ADC0 Control 1 */
SFR(   ADC0AC, 0xB3 ); /* ADC0 Accumulator Configuration */
SFR(  C2FPDAT, 0xB4 ); /* C2 Flash Programming Data */
SFR( DEVICEID, 0xB5 ); /* Device Identification */
SFR(    REVID, 0xB6 ); /* Revision Identifcation */
SFR(    FLKEY, 0xB7 ); /* Flash Lock and Key */
SFR(       IP, 0xB8 ); /* INTERRUPT PRIORITY */
SFR(   ADC0TK, 0xB9 ); /* ADC0 Burst Mode Track Time */
SFR(   ADC0MX, 0xBB ); /* ADC0 Multiplexer Selection */
SFR(   ADC0CF, 0xBC ); /* ADC 0 CONFIGURATION */
SFR(    ADC0L, 0xBD ); /* ADC 0 DATA WORD LSB */
SFR(    ADC0H, 0xBE ); /* ADC 0 DATA WORD MSB */
SFR(  CMP1CN0, 0xBF ); /* Comparator 1 Control 0 */
SFR(  SMB0CN0, 0xC0 ); /* SMBUS CONTROL */
SFR(   SMB0CF, 0xC1 ); /* SMBUS CONFIGURATION */
SFR(  SMB0DAT, 0xC2 ); /* SMBUS DATA */
SFR(  ADC0GTL, 0xC3 ); /* ADC 0 GREATER-THAN LOW BYTE */
SFR(  ADC0GTH, 0xC4 ); /* ADC 0 GREATER-THAN HIGH BYTE */
SFR(  ADC0LTL, 0xC5 ); /* ADC 0 LESS-THAN LOW BYTE */
SFR(  ADC0LTH, 0xC6 ); /* ADC 0 LESS-THAN HIGH BYTE */
SFR(  HFO0CAL, 0xC7 ); /* High Frequency Oscillator 0 Calibration */
SFR(  TMR2CN0, 0xC8 ); /* TIMER 2 CONTROL */
SFR(   REG0CN, 0xC9 ); /* TIMER 2 CONTROL */
SFR(  TMR2RLL, 0xCA ); /* TIMER 2 CAPTURE REGISTER - LOW BYTE */
SFR(  TMR2RLH, 0xCB ); /* TIMER 2 CAPTURE REGISTER - HIGH BYTE */
SFR(    TMR2L, 0xCC ); /* TIMER 2 - LOW BYTE */
SFR(    TMR2H, 0xCD ); /* TIMER 2 - HIGH BYTE */
SFR(  CRC0CN0, 0xCE ); /* CRC0 Control 0 */
SFR( CRC0FLIP, 0xCF ); /* CRC0 Bit Flip */
SFR(      PSW, 0xD0 ); /* PROGRAM STATUS WORD */
SFR(   REF0CN, 0xD1 ); /* VOLTAGE REFERENCE 0 CONTROL */
SFR( CRC0AUTO, 0xD2 ); /* CRC0 Automatic Control */
SFR(  CRC0CNT, 0xD3 ); /* CRC0 Automatic Flash Sector Count */
SFR(   P0SKIP, 0xD4 ); /* PORT 0 SKIP */
SFR(   P1SKIP, 0xD5 ); /* PORT 1 SKIP */
SFR(  SMB0ADM, 0xD6 ); /* SMBus 0 Slave Address Mask */
SFR(  SMB0ADR, 0xD7 ); /* SMBus 0 Slave Address */
SFR(  PCA0CN0, 0xD8 ); /* PCA CONTROL */
SFR(   PCA0MD, 0xD9 ); /* PCA MODE */
SFR( PCA0CPM0, 0xDA ); /* PCA MODULE 0 MODE REGISTER */
SFR( PCA0CPM1, 0xDB ); /* PCA MODULE 1 MODE REGISTER */
SFR( PCA0CPM2, 0xDC ); /* PCA MODULE 2 MODE REGISTER */
SFR(   CRC0IN, 0xDD ); /* CRC0 Data Input */
SFR(  CRC0DAT, 0xDE ); /* CRC0 Data Output */
SFR(  ADC0PWR, 0xDF ); /* ADC0 Power Control */
SFR(      ACC, 0xE0 ); /* ACCUMULATOR */
SFR(     XBR0, 0xE1 ); /* PORT MUX CONFIGURATION REGISTER 0 */
SFR(     XBR1, 0xE2 ); /* PORT MUX CONFIGURATION REGISTER 1 */
SFR(     XBR2, 0xE3 ); /* PORT MUX CONFIGURATION REGISTER 2 */
SFR(   IT01CF, 0xE4 ); /* INT0/INT1 CONFIGURATION REGISTER */
SFR(     EIE1, 0xE6 ); /* EXTERNAL INTERRUPT ENABLE 1 */
SFR(  ADC0CN0, 0xE8 ); /* ADC 0 CONTROL */
SFR( PCA0CPL1, 0xE9 ); /* PCA CAPTURE 1 LOW */
SFR( PCA0CPH1, 0xEA ); /* PCA CAPTURE 1 HIGH */
SFR( PCA0CPL2, 0xEB ); /* PCA CAPTURE 2 LOW */
SFR( PCA0CPH2, 0xEC ); /* PCA CAPTURE 2 HIGH */
SFR(    P1MAT, 0xED ); /* Port 1 Match */
SFR(   P1MASK, 0xEE ); /* Port 1 Mask */
SFR(   RSTSRC, 0xEF ); /* RESET SOURCE */
SFR(        B, 0xF0 ); /* B REGISTER */
SFR(   P0MDIN, 0xF1 ); /* PORT 0 INPUT MODE CONFIGURATION */
SFR(   P1MDIN, 0xF2 ); /* PORT 1 INPUT MODE CONFIGURATION */
SFR(     EIP1, 0xF3 ); /* EXTERNAL INTERRUPT PRIORITY REGISTER 1 */
SFR(   PRTDRV, 0xF6 ); /* Port Drive Strength */
SFR(  PCA0PWM, 0xF7 ); /* PCA PWM Configuration */
SFR(  SPI0CN0, 0xF8 ); /* SPI0 CONTROL */
SFR(    PCA0L, 0xF9 ); /* PCA COUNTER LOW */
SFR(    PCA0H, 0xFA ); /* PCA COUNTER HIGH */
SFR( PCA0CPL0, 0xFB ); /* PCA CAPTURE 0 LOW */
SFR( PCA0CPH0, 0xFC ); /* PCA CAPTURE 0 HIGH */
SFR(    P0MAT, 0xFD ); /* Port 0 Match */
SFR(   P0MASK, 0xFE ); /* Port 0 Mask */
SFR(   VDM0CN, 0xFF ); /* Supply Monitor Control */


/*  WORD/DWORD Registers  */

SFR16E(  TMR0, 0x8C8A ); /* TIMER 0 COUNTER */
SFR16E(  TMR1, 0x8D8B ); /* TIMER 1 COUNTER */
SFR16(     TMR2, 0xCC ); /* TIMER 2 COUNTER */
SFR16(   TMR2RL, 0xCA ); /* TIMER 2 CAPTURE REGISTER WORD */
SFR16(     TMR3, 0x94 ); /* TIMER 3 COUNTER */
SFR16(   TMR3RL, 0x92 ); /* TIMER 3 CAPTURE REGISTER WORD */
SFR16(     ADC0, 0xBD ); /* ADC 0 DATA WORD */
SFR16(   ADC0GT, 0xC3 ); /* ADC 0 GREATER-THAN REGISTER WORD */
SFR16(   ADC0LT, 0xC5 ); /* ADC 0 LESS-THAN REGISTER WORD */
SFR16(     PCA0, 0xF9 ); /* PCA COUNTER */
SFR16(  PCA0CP0, 0xFB ); /* PCA CAPTURE 0 WORD */
SFR16(  PCA0CP1, 0xE9 ); /* PCA CAPTURE 1 WORD */
SFR16(  PCA0CP2, 0xEB ); /* PCA CAPTURE 2 WORD */

/*  BIT Registers  */

/*  P0  0x80 */
SBIT(     P0_0, 0x80, 0 ); /* Port 0 bit 0 */
SBIT(     P0_1, 0x80, 1 ); /* Port 0 bit 1 */
SBIT(     P0_2, 0x80, 2 ); /* Port 0 bit 2 */
SBIT(     P0_3, 0x80, 3 ); /* Port 0 bit 3 */
SBIT(     P0_4, 0x80, 4 ); /* Port 0 bit 4 */
SBIT(     P0_5, 0x80, 5 ); /* Port 0 bit 5 */
SBIT(     P0_6, 0x80, 6 ); /* Port 0 bit 6 */
SBIT(     P0_7, 0x80, 7 ); /* Port 0 bit 7 */

/*  TCON  0x88 */
SBIT(      IT0, 0x88, 0 ); /* TCON.0 - EXT. INTERRUPT 0 TYPE */
SBIT(      IE0, 0x88, 1 ); /* TCON.1 - EXT. INTERRUPT 0 EDGE FLAG */
SBIT(      IT1, 0x88, 2 ); /* TCON.2 - EXT. INTERRUPT 1 TYPE */
SBIT(      IE1, 0x88, 3 ); /* TCON.3 - EXT. INTERRUPT 1 EDGE FLAG */
SBIT(      TR0, 0x88, 4 ); /* TCON.4 - TIMER 0 ON/OFF CONTROL */
SBIT(      TF0, 0x88, 5 ); /* TCON.5 - TIMER 0 OVERFLOW FLAG */
SBIT(      TR1, 0x88, 6 ); /* TCON.6 - TIMER 1 ON/OFF CONTROL */
SBIT(      TF1, 0x88, 7 ); /* TCON.7 - TIMER 1 OVERFLOW FLAG */

/*  P1  0x90 */
SBIT(     P1_0, 0x90, 0 ); /* Port 1 bit 0 */
SBIT(     P1_1, 0x90, 1 ); /* Port 1 bit 1 */
SBIT(     P1_2, 0x90, 2 ); /* Port 1 bit 2 */
SBIT(     P1_3, 0x90, 3 ); /* Port 1 bit 3 */
SBIT(     P1_4, 0x90, 4 ); /* Port 1 bit 4 */
SBIT(     P1_5, 0x90, 5 ); /* Port 1 bit 5 */
SBIT(     P1_6, 0x90, 6 ); /* Port 1 bit 6 */
SBIT(     P1_7, 0x90, 7 ); /* Port 1 bit 7 */

/*  SCON  0x98 */
SBIT(       RI, 0x98, 0 ); /* SCON.0 - RECEIVE INTERRUPT FLAG */
SBIT(      RI0, 0x98, 0 ); /* SCON.0 - RECEIVE INTERRUPT FLAG */
SBIT(       TI, 0x98, 1 ); /* SCON.1 - TRANSMIT INTERRUPT FLAG */
SBIT(      TI0, 0x98, 1 ); /* SCON.1 - TRANSMIT INTERRUPT FLAG */
SBIT(      RB8, 0x98, 2 ); /* SCON.2 - RECEIVE BIT 8 */
SBIT(     RB80, 0x98, 2 ); /* SCON.2 - RECEIVE BIT 8 */
SBIT(      TB8, 0x98, 3 ); /* SCON.3 - TRANSMIT BIT 8 */
SBIT(     TB80, 0x98, 3 ); /* SCON.3 - TRANSMIT BIT 8 */
SBIT(      REN, 0x98, 4 ); /* SCON.4 - RECEIVE ENABLE */
SBIT(     REN0, 0x98, 4 ); /* SCON.4 - RECEIVE ENABLE */
SBIT(      SM2, 0x98, 5 ); /* SCON.5 - MULTIPROCESSOR COMMUNICATION ENABLE */
SBIT(     MCE0, 0x98, 5 ); /* SCON.5 - MULTIPROCESSOR COMMUNICATION ENABLE */
SBIT(      SM0, 0x98, 7 ); /* SCON.7 - SERIAL MODE CONTROL BIT 0 */
SBIT(   S0MODE, 0x98, 7 ); /* SCON.7 - SERIAL MODE CONTROL BIT 0 */
SBIT(    SMODE, 0x98, 7 ); /* SCON.7 - SERIAL MODE CONTROL BIT 0 */

/*  P2  0xA0 */
SBIT(     P2_0, 0xA0, 0 ); /* Port 2 bit 0 */
SBIT(     P2_1, 0xA0, 1 ); /* Port 2 bit 1 */
SBIT(     P2_2, 0xA0, 2 ); /* Port 2 bit 2 */
SBIT(     P2_3, 0xA0, 3 ); /* Port 2 bit 3 */
SBIT(     P2_4, 0xA0, 4 ); /* Port 2 bit 4 */
SBIT(     P2_5, 0xA0, 5 ); /* Port 2 bit 5 */
SBIT(     P2_6, 0xA0, 6 ); /* Port 2 bit 6 */
SBIT(     P2_7, 0xA0, 7 ); /* Port 2 bit 7 */

/*  IE  0xA8 */
SBIT(      EX0, 0xA8, 0 ); /* IE.0 - EXTERNAL INTERRUPT 0 ENABLE */
SBIT(      ET0, 0xA8, 1 ); /* IE.1 - TIMER 0 INTERRUPT ENABLE */
SBIT(      EX1, 0xA8, 2 ); /* IE.2 - EXTERNAL INTERRUPT 1 ENABLE */
SBIT(      ET1, 0xA8, 3 ); /* IE.3 - TIMER 1 INTERRUPT ENABLE */
SBIT(       ES, 0xA8, 4 ); /* IE.4 - SERIAL PORT INTERRUPT ENABLE */
SBIT(      ES0, 0xA8, 4 ); /* IE.4 - SERIAL PORT INTERRUPT ENABLE */
SBIT(      ET2, 0xA8, 5 ); /* IE.5 - TIMER 2 INTERRUPT ENABLE */
SBIT(    ESPI0, 0xA8, 6 ); /* IE.6 - SPI0 INTERRUPT ENABLE */
SBIT(       EA, 0xA8, 7 ); /* IE.7 - GLOBAL INTERRUPT ENABLE */

/*  IP  0xB8 */
SBIT(      PX0, 0xB8, 0 ); /* IP.0 - EXTERNAL INTERRUPT 0 PRIORITY */
SBIT(      PT0, 0xB8, 1 ); /* IP.1 - TIMER 0 PRIORITY */
SBIT(      PX1, 0xB8, 2 ); /* IP.2 - EXTERNAL INTERRUPT 1 PRIORITY */
SBIT(      PT1, 0xB8, 3 ); /* IP.3 - TIMER 1 PRIORITY */
SBIT(       PS, 0xB8, 4 ); /* IP.4 - SERIAL PORT PRIORITY */
SBIT(      PS0, 0xB8, 4 ); /* IP.4 - SERIAL PORT PRIORITY */
SBIT(      PT2, 0xB8, 5 ); /* IP.5 - TIMER 2 PRIORITY */
SBIT(    PSPI0, 0xB8, 6 ); /* IP.6 - SPI0 PRIORITY */

/*  SMB0CN  0xC0 */
SBIT(       SI, 0xC0, 0 ); /* SMB0CN.0 - SMBUS 0 INTERRUPT PENDING FLAG */
SBIT(      ACK, 0xC0, 1 ); /* SMB0CN.1 - SMBUS 0 ACKNOWLEDGE FLAG */
SBIT(  ARBLOST, 0xC0, 2 ); /* SMB0CN.2 - SMBUS 0 ARBITRATION LOST INDICATOR */
SBIT(    ACKRQ, 0xC0, 3 ); /* SMB0CN.3 - SMBUS 0 ACKNOWLEDGE REQUEST */
SBIT(      STO, 0xC0, 4 ); /* SMB0CN.4 - SMBUS 0 STOP FLAG */
SBIT(      STA, 0xC0, 5 ); /* SMB0CN.5 - SMBUS 0 START FLAG */
SBIT(   TXMODE, 0xC0, 6 ); /* SMB0CN.6 - SMBUS 0 TRANSMIT MODE INDICATOR */
SBIT(   MASTER, 0xC0, 7 ); /* SMB0CN.7 - SMBUS 0 MASTER/SLAVE INDICATOR */

/*  TMR2CN  0xC8 */
SBIT(   T2XCLK, 0xC8, 0 ); /* TMR2CN.0 - TIMER 2 EXTERNAL CLOCK SELECT */
SBIT(      TR2, 0xC8, 2 ); /* TMR2CN.2 - TIMER 2 ON/OFF CONTROL */
SBIT(  T2SPLIT, 0xC8, 3 ); /* TMR2CN.3 - TIMER 2 SPLIT MODE ENABLE */
SBIT(   TF2CEN, 0xC8, 4 ); /* TMR2CN.5 - TIMER 2 CAPTURE ENABLE */
SBIT(   TF2LEN, 0xC8, 5 ); /* TMR2CN.5 - TIMER 2 LOW BYTE INTERRUPT ENABLE */
SBIT(     TF2L, 0xC8, 6 ); /* TMR2CN.6 - TIMER 2 LOW BYTE OVERFLOW FLAG */
SBIT(      TF2, 0xC8, 7 ); /* TMR2CN.7 - TIMER 2 OVERFLOW FLAG */
SBIT(     TF2H, 0xC8, 7 ); /* TMR2CN.7 - TIMER 2 HIGH BYTE OVERFLOW FLAG */

/*  PSW  0xD0 */
SBIT(   PARITY, 0xD0, 0 ); /* PSW.0 - ACCUMULATOR PARITY FLAG */
SBIT(       F1, 0xD0, 1 ); /* PSW.1 - FLAG 1 */
SBIT(       OV, 0xD0, 2 ); /* PSW.2 - OVERFLOW FLAG */
SBIT(      RS0, 0xD0, 3 ); /* PSW.3 - REGISTER BANK SELECT 0 */
SBIT(      RS1, 0xD0, 4 ); /* PSW.4 - REGISTER BANK SELECT 1 */
SBIT(       F0, 0xD0, 5 ); /* PSW.5 - FLAG 0 */
SBIT(       AC, 0xD0, 6 ); /* PSW.6 - AUXILIARY CARRY FLAG */
SBIT(       CY, 0xD0, 7 ); /* PSW.7 - CARRY FLAG */

/*  PCA0CN  0xD8 */
SBIT(     CCF0, 0xD8, 0 ); /* PCA0CN.0 - PCA MODULE 0 CAPTURE/COMPARE FLAG */
SBIT(     CCF1, 0xD8, 1 ); /* PCA0CN.1 - PCA MODULE 1 CAPTURE/COMPARE FLAG */
SBIT(     CCF2, 0xD8, 2 ); /* PCA0CN.2 - PCA MODULE 2 CAPTURE/COMPARE FLAG */
SBIT(       CR, 0xD8, 6 ); /* PCA0CN.6 - PCA COUNTER/TIMER RUN CONTROL */
SBIT(       CF, 0xD8, 7 ); /* PCA0CN.7 - PCA COUNTER/TIMER OVERFLOW FLAG */

/*  ADC0CN  0xE8 */
SBIT(    ADCM0, 0xE8, 0 ); /* ADC0CN.0 - ADC 0 START OF CONV. MODE BIT 0 */
SBIT(    ADCM1, 0xE8, 1 ); /* ADC0CN.1 - ADC 0 START OF CONV. MODE BIT 1 */
SBIT(    ADCM2, 0xE8, 2 ); /* ADC0CN.2 - ADC 0 START OF CONV. MODE BIT 2 */
SBIT(   ADWINT, 0xE8, 3 ); /* ADC0CN.3 - ADC 0 WINDOW COMPARE INT. FLAG */
SBIT(  AD0WINT, 0xE8, 3 ); /* ADC0CN.3 - ADC 0 WINDOW COMPARE INT. FLAG */
SBIT(   ADBUSY, 0xE8, 4 ); /* ADC0CN.4 - ADC 0 BUSY FLAG */
SBIT(  AD0BUSY, 0xE8, 4 ); /* ADC0CN.4 - ADC 0 BUSY FLAG */
SBIT(    ADINT, 0xE8, 5 ); /* ADC0CN.5 - ADC 0 CONV. COMPLETE INT. FLAG */
SBIT(   AD0INT, 0xE8, 5 ); /* ADC0CN.5 - ADC 0 CONV. COMPLETE INT. FLAG */
SBIT(   ADBMEN, 0xE8, 6 ); /* ADC0CN.6 - ADC 0 BURST MODE ENABLE */
SBIT(     ADEN, 0xE8, 7 ); /* ADC0CN.7 - ADC 0 ENABLE */
SBIT(    AD0EN, 0xE8, 7 ); /* ADC0CN.7 - ADC 0 ENABLE */

/*  SPI0CN  0xF8 */
SBIT(    SPIEN, 0xF8, 0 ); /* SPI0CN.0 - SPI0 ENABLE */
SBIT(    TXBMT, 0xF8, 1 ); /* SPI0CN.1 - TRANSMIT BUFFER EMPTY */
SBIT(   NSSMD0, 0xF8, 2 ); /* SPI0CN.2 - SLAVE SELECT MODE BIT 0 */
SBIT(   NSSMD1, 0xF8, 3 ); /* SPI0CN.3 - SLAVE SELECT MODE BIT 1 */
SBIT(   RXOVRN, 0xF8, 4 ); /* SPI0CN.4 - RECEIVE OVERRUN FLAG */
SBIT(     MODF, 0xF8, 5 ); /* SPI0CN.5 - MODE FAULT FLAG */
SBIT(     WCOL, 0xF8, 6 ); /* SPI0CN.6 - WRITE COLLISION FLAG */
SBIT(     SPIF, 0xF8, 7 ); /* SPI0CN.7 - SPI0 INTERRUPT FLAG */


/* Predefined SFR Bit Masks */

#define PCON_IDLE         0x01    /* PCON */
#define PCON_STOP         0x02    /* PCON */
#define T1M               0x08    /* CKCON */
#define PSWE              0x01    /* PSCTL */
#define PSEE              0x02    /* PSCTL */
#define ECP0              0x20    /* EIE1 */
#define ECP1              0x40    /* EIE1 */
#define PORSF             0x02    /* RSTSRC */
#define SWRSF             0x10    /* RSTSRC */
#define ECCF              0x01    /* PCA0CPMn */
#define PWM               0x02    /* PCA0CPMn */
#define TOG               0x04    /* PCA0CPMn */
#define MAT               0x08    /* PCA0CPMn */
#define CAPN              0x10    /* PCA0CPMn */
#define CAPP              0x20    /* PCA0CPMn */
#define ECOM              0x40    /* PCA0CPMn */
#define PWM16             0x80    /* PCA0CPMn */
#define CP0E              0x10    /* XBR0 */
#define CP0OEN            0x10    /* XBR0 */
#define CP0AE             0x20    /* XBR0 */
#define CP0AOEN           0x20    /* XBR0 */
#define CP1E              0x40    /* XBR0 */
#define CP1AE             0x80    /* XBR0 */

/* Interrupts */

#define EXT0_VECTOR          0      /* External Interrupt 0 */
#define TIMER0_VECTOR        1      /* Timer0 Overflow */
#define EXT1_VECTOR          2      /* External Interrupt 1 */
#define TIMER1_VECTOR        3      /* Timer1 Overflow */
#define UART0_VECTOR         4      /* Serial Port 0 */
#define TIMER2_VECTOR        5      /* Timer2 Overflow */
#define SPI0_VECTOR          6      /* Serial Peripheral Interface 0 */
#define SMBUS0_VECTOR        7      /* SMBus0 Interface */
#define PMAT_VECTOR          8      /* Port Match */
#define ADC0_WINDOW_VECTOR   9      /* ADC0 Window Comparison */
#define ADC0_EOC_VECTOR      10     /* ADC0 End Of Conversion */
#define PCA0_VECTOR          11     /* PCA0 Peripheral */
#define CMP0_VECTOR          12     /* Comparator0 */
#define CMP1_VECTOR          13     /* Comparator1 */
#define TIMER3_VECTOR        14     /* Timer3 Overflow */

/*------------------------------------------------------------------------------ */
/* ADC0CN0 Enums (ADC0 Control 0 @ 0xE8)                                         */
/*------------------------------------------------------------------------------ */
#define ADCM__FMASK            0x07 /* Start of Conversion Mode Select */
#define ADCM__SHIFT            0x00 /* Start of Conversion Mode Select */
#define ADCM__ADBUSY           0x00 /* ADC0 conversion initiated on write of 1 to ADBUSY. */
#define ADCM__TIMER0           0x01 /* ADC0 conversion initiated on overflow of Timer 0. */
#define ADCM__TIMER2           0x02 /* ADC0 conversion initiated on overflow of Timer 2. */
#define ADCM__TIMER3           0x03 /* ADC0 conversion initiated on overflow of Timer 3. */
#define ADCM__CNVSTR           0x04 /* ADC0 conversion initiated on rising edge of CNVSTR. */

#define ADWINT__BMASK          0x08 /* Window Compare Interrupt Flag */
#define ADWINT__SHIFT          0x03 /* Window Compare Interrupt Flag */
#define ADWINT__NOT_SET        0x00 /* An ADC window compare event did not occur. */
#define ADWINT__SET            0x08 /* An ADC window compare event occurred. */

#define ADBUSY__BMASK          0x10 /* ADC Busy */
#define ADBUSY__SHIFT          0x04 /* ADC Busy */
#define ADBUSY__NOT_SET        0x00 /* An ADC0 conversion is not currently in progress. */
#define ADBUSY__SET            0x10 /* ADC0 conversion is in progress or start an ADC0 conversion. */

#define ADINT__BMASK           0x20 /* Conversion Complete Interrupt Flag */
#define ADINT__SHIFT           0x05 /* Conversion Complete Interrupt Flag */
#define ADINT__NOT_SET         0x00 /* ADC0 has not completed a conversion since the last time ADINT was cleared. */
#define ADINT__SET             0x20 /* ADC0 completed a data conversion. */

#define ADBMEN__BMASK          0x40 /* Burst Mode Enable */
#define ADBMEN__SHIFT          0x06 /* Burst Mode Enable */
#define ADBMEN__BURST_DISABLED 0x00 /* Disable ADC0 burst mode. */
#define ADBMEN__BURST_ENABLED  0x40 /* Enable ADC0 burst mode. */

#define ADEN__BMASK            0x80 /* ADC Enable */
#define ADEN__SHIFT            0x07 /* ADC Enable */
#define ADEN__DISABLED         0x00 /* Disable ADC0 (low-power shutdown). */
#define ADEN__ENABLED          0x80 /* Enable ADC0 (active and ready for data conversions). */

/*------------------------------------------------------------------------------ */
/* ADC0AC Enums (ADC0 Accumulator Configuration @ 0xB3)                          */
/*------------------------------------------------------------------------------ */
#define AD0RPT__FMASK            0x07 /* Repeat Count */
#define AD0RPT__SHIFT            0x00 /* Repeat Count */
#define AD0RPT__ACC_1            0x00 /* Perform and Accumulate 1 conversion (not used in 12-bit mode). */
#define AD0RPT__ACC_4            0x01 /* Perform and Accumulate 4 conversions (1 conversion in 12-bit mode). */
#define AD0RPT__ACC_8            0x02 /* Perform and Accumulate 8 conversions (2 conversions in 12-bit mode). */
#define AD0RPT__ACC_16           0x03 /* Perform and Accumulate 16 conversions (4 conversions in 12-bit mode). */
#define AD0RPT__ACC_32           0x04 /* Perform and Accumulate 32 conversions (8 conversions in 12-bit mode). */
#define AD0RPT__ACC_64           0x05 /* Perform and Accumulate 64 conversions (16 conversions in 12-bit mode). */

#define AD0SJST__FMASK           0x38 /* Accumulator Shift and Justify */
#define AD0SJST__SHIFT           0x03 /* Accumulator Shift and Justify */
#define AD0SJST__RIGHT_NO_SHIFT  0x00 /* Right justified. No shifting applied. */
#define AD0SJST__RIGHT_SHIFT_1   0x08 /* Right justified. Shifted right by 1 bit. */
#define AD0SJST__RIGHT_SHIFT_2   0x10 /* Right justified. Shifted right by 2 bits. */
#define AD0SJST__RIGHT_SHIFT_3   0x18 /* Right justified. Shifted right by 3 bits. */
#define AD0SJST__LEFT_NO_SHIFT   0x20 /* Left justified. No shifting applied. */

#define AD0AE__BMASK             0x40 /* Accumulate Enable */
#define AD0AE__SHIFT             0x06 /* Accumulate Enable */
#define AD0AE__ACC_DISABLED      0x00 /* ADC0H:ADC0L contain the result of the latest conversion when Burst Mode is disabled. */
#define AD0AE__ACC_ENABLED       0x40 /* ADC0H:ADC0L contain the accumulated conversion results when Burst Mode is disabled. Firmware must write 0x0000 to ADC0H:ADC0L to clear the accumulated result. */

#define AD12BE__BMASK            0x80 /* 12-Bit Mode Enable */
#define AD12BE__SHIFT            0x07 /* 12-Bit Mode Enable */
#define AD12BE__12_BIT_DISABLED  0x00 /* Disable 12-bit mode. */
#define AD12BE__12_BIT_ENABLED   0x80 /* Enable 12-bit mode. */

/*------------------------------------------------------------------------------ */
/* ADC0CF Enums (ADC0 Configuration @ 0xBC)                                      */
/*------------------------------------------------------------------------------ */
#define ADGN__BMASK         0x01 /* Gain Control */
#define ADGN__SHIFT         0x00 /* Gain Control */
#define ADGN__GAIN_0P5      0x00 /* The on-chip PGA gain is 0.5. */
#define ADGN__GAIN_1        0x01 /* The on-chip PGA gain is 1. */

#define ADTM__BMASK         0x02 /* Track Mode */
#define ADTM__SHIFT         0x01 /* Track Mode */
#define ADTM__TRACK_NORMAL  0x00 /* Normal Track Mode. When ADC0 is enabled, conversion begins immediately following the start-of-conversion signal. */
#define ADTM__TRACK_DELAYED 0x02 /* Delayed Track Mode. When ADC0 is enabled, conversion begins 4 SAR clock cycles following the start-of-conversion signal. The ADC is allowed to track during this time. */

#define AD8BE__BMASK        0x04 /* 8-Bit Mode Enable */
#define AD8BE__SHIFT        0x02 /* 8-Bit Mode Enable */
#define AD8BE__NORMAL       0x00 /* ADC0 operates in 10-bit or 12-bit mode (normal operation). */
#define AD8BE__8_BIT        0x04 /* ADC0 operates in 8-bit mode. */

#define ADSC__FMASK         0xF8 /* SAR Clock Divider */
#define ADSC__SHIFT         0x03 /* SAR Clock Divider */

/*------------------------------------------------------------------------------ */
/* P0MDIN Enums (Port 0 Input Mode @ 0xF1)                                       */
/*------------------------------------------------------------------------------ */
#define B0__BMASK   0x01 /* Port 0 Bit 0 Input Mode */
#define B0__SHIFT   0x00 /* Port 0 Bit 0 Input Mode */
#define B0__ANALOG  0x00 /* P0.0 pin is configured for analog mode. */
#define B0__DIGITAL 0x01 /* P0.0 pin is configured for digital mode. */

#define B1__BMASK   0x02 /* Port 0 Bit 1 Input Mode */
#define B1__SHIFT   0x01 /* Port 0 Bit 1 Input Mode */
#define B1__ANALOG  0x00 /* P0.1 pin is configured for analog mode. */
#define B1__DIGITAL 0x02 /* P0.1 pin is configured for digital mode. */

#define B2__BMASK   0x04 /* Port 0 Bit 2 Input Mode */
#define B2__SHIFT   0x02 /* Port 0 Bit 2 Input Mode */
#define B2__ANALOG  0x00 /* P0.2 pin is configured for analog mode. */
#define B2__DIGITAL 0x04 /* P0.2 pin is configured for digital mode. */

#define B3__BMASK   0x08 /* Port 0 Bit 3 Input Mode */
#define B3__SHIFT   0x03 /* Port 0 Bit 3 Input Mode */
#define B3__ANALOG  0x00 /* P0.3 pin is configured for analog mode. */
#define B3__DIGITAL 0x08 /* P0.3 pin is configured for digital mode. */

#define B4__BMASK   0x10 /* Port 0 Bit 4 Input Mode */
#define B4__SHIFT   0x04 /* Port 0 Bit 4 Input Mode */
#define B4__ANALOG  0x00 /* P0.4 pin is configured for analog mode. */
#define B4__DIGITAL 0x10 /* P0.4 pin is configured for digital mode. */

#define B5__BMASK   0x20 /* Port 0 Bit 5 Input Mode */
#define B5__SHIFT   0x05 /* Port 0 Bit 5 Input Mode */
#define B5__ANALOG  0x00 /* P0.5 pin is configured for analog mode. */
#define B5__DIGITAL 0x20 /* P0.5 pin is configured for digital mode. */

#define B6__BMASK   0x40 /* Port 0 Bit 6 Input Mode */
#define B6__SHIFT   0x06 /* Port 0 Bit 6 Input Mode */
#define B6__ANALOG  0x00 /* P0.6 pin is configured for analog mode. */
#define B6__DIGITAL 0x40 /* P0.6 pin is configured for digital mode. */

#define B7__BMASK   0x80 /* Port 0 Bit 7 Input Mode */
#define B7__SHIFT   0x07 /* Port 0 Bit 7 Input Mode */
#define B7__ANALOG  0x00 /* P0.7 pin is configured for analog mode. */
#define B7__DIGITAL 0x80 /* P0.7 pin is configured for digital mode. */

/*------------------------------------------------------------------------------ */
/* P0MDOUT Enums (Port 0 Output Mode @ 0xA4)                                     */
/*------------------------------------------------------------------------------ */
#define B0__BMASK      0x01 /* Port 0 Bit 0 Output Mode */
#define B0__SHIFT      0x00 /* Port 0 Bit 0 Output Mode */
#define B0__OPEN_DRAIN 0x00 /* P0.0 output is open-drain. */
#define B0__PUSH_PULL  0x01 /* P0.0 output is push-pull. */

#define B1__BMASK      0x02 /* Port 0 Bit 1 Output Mode */
#define B1__SHIFT      0x01 /* Port 0 Bit 1 Output Mode */
#define B1__OPEN_DRAIN 0x00 /* P0.1 output is open-drain. */
#define B1__PUSH_PULL  0x02 /* P0.1 output is push-pull. */

#define B2__BMASK      0x04 /* Port 0 Bit 2 Output Mode */
#define B2__SHIFT      0x02 /* Port 0 Bit 2 Output Mode */
#define B2__OPEN_DRAIN 0x00 /* P0.2 output is open-drain. */
#define B2__PUSH_PULL  0x04 /* P0.2 output is push-pull. */

#define B3__BMASK      0x08 /* Port 0 Bit 3 Output Mode */
#define B3__SHIFT      0x03 /* Port 0 Bit 3 Output Mode */
#define B3__OPEN_DRAIN 0x00 /* P0.3 output is open-drain. */
#define B3__PUSH_PULL  0x08 /* P0.3 output is push-pull. */

#define B4__BMASK      0x10 /* Port 0 Bit 4 Output Mode */
#define B4__SHIFT      0x04 /* Port 0 Bit 4 Output Mode */
#define B4__OPEN_DRAIN 0x00 /* P0.4 output is open-drain. */
#define B4__PUSH_PULL  0x10 /* P0.4 output is push-pull. */

#define B5__BMASK      0x20 /* Port 0 Bit 5 Output Mode */
#define B5__SHIFT      0x05 /* Port 0 Bit 5 Output Mode */
#define B5__OPEN_DRAIN 0x00 /* P0.5 output is open-drain. */
#define B5__PUSH_PULL  0x20 /* P0.5 output is push-pull. */

#define B6__BMASK      0x40 /* Port 0 Bit 6 Output Mode */
#define B6__SHIFT      0x06 /* Port 0 Bit 6 Output Mode */
#define B6__OPEN_DRAIN 0x00 /* P0.6 output is open-drain. */
#define B6__PUSH_PULL  0x40 /* P0.6 output is push-pull. */

#define B7__BMASK      0x80 /* Port 0 Bit 7 Output Mode */
#define B7__SHIFT      0x07 /* Port 0 Bit 7 Output Mode */
#define B7__OPEN_DRAIN 0x00 /* P0.7 output is open-drain. */
#define B7__PUSH_PULL  0x80 /* P0.7 output is push-pull. */

/*------------------------------------------------------------------------------ */
/* P0SKIP Enums (Port 0 Skip @ 0xD4)                                             */
/*------------------------------------------------------------------------------ */
#define B0__BMASK       0x01 /* Port 0 Bit 0 Skip */
#define B0__SHIFT       0x00 /* Port 0 Bit 0 Skip */
#define B0__NOT_SKIPPED 0x00 /* P0.0 pin is not skipped by the crossbar. */
#define B0__SKIPPED     0x01 /* P0.0 pin is skipped by the crossbar. */

#define B1__BMASK       0x02 /* Port 0 Bit 1 Skip */
#define B1__SHIFT       0x01 /* Port 0 Bit 1 Skip */
#define B1__NOT_SKIPPED 0x00 /* P0.1 pin is not skipped by the crossbar. */
#define B1__SKIPPED     0x02 /* P0.1 pin is skipped by the crossbar. */

#define B2__BMASK       0x04 /* Port 0 Bit 2 Skip */
#define B2__SHIFT       0x02 /* Port 0 Bit 2 Skip */
#define B2__NOT_SKIPPED 0x00 /* P0.2 pin is not skipped by the crossbar. */
#define B2__SKIPPED     0x04 /* P0.2 pin is skipped by the crossbar. */

#define B3__BMASK       0x08 /* Port 0 Bit 3 Skip */
#define B3__SHIFT       0x03 /* Port 0 Bit 3 Skip */
#define B3__NOT_SKIPPED 0x00 /* P0.3 pin is not skipped by the crossbar. */
#define B3__SKIPPED     0x08 /* P0.3 pin is skipped by the crossbar. */

#define B4__BMASK       0x10 /* Port 0 Bit 4 Skip */
#define B4__SHIFT       0x04 /* Port 0 Bit 4 Skip */
#define B4__NOT_SKIPPED 0x00 /* P0.4 pin is not skipped by the crossbar. */
#define B4__SKIPPED     0x10 /* P0.4 pin is skipped by the crossbar. */

#define B5__BMASK       0x20 /* Port 0 Bit 5 Skip */
#define B5__SHIFT       0x05 /* Port 0 Bit 5 Skip */
#define B5__NOT_SKIPPED 0x00 /* P0.5 pin is not skipped by the crossbar. */
#define B5__SKIPPED     0x20 /* P0.5 pin is skipped by the crossbar. */

#define B6__BMASK       0x40 /* Port 0 Bit 6 Skip */
#define B6__SHIFT       0x06 /* Port 0 Bit 6 Skip */
#define B6__NOT_SKIPPED 0x00 /* P0.6 pin is not skipped by the crossbar. */
#define B6__SKIPPED     0x40 /* P0.6 pin is skipped by the crossbar. */

#define B7__BMASK       0x80 /* Port 0 Bit 7 Skip */
#define B7__SHIFT       0x07 /* Port 0 Bit 7 Skip */
#define B7__NOT_SKIPPED 0x00 /* P0.7 pin is not skipped by the crossbar. */
#define B7__SKIPPED     0x80 /* P0.7 pin is skipped by the crossbar. */

/*------------------------------------------------------------------------------ */
/* P0 Enums (Port 0 Pin Latch @ 0x80)                                            */
/*------------------------------------------------------------------------------ */
#define B0__BMASK 0x01 /* Port 0 Bit 0 Latch */
#define B0__SHIFT 0x00 /* Port 0 Bit 0 Latch */
#define B0__LOW   0x00 /* P0.0 is low. Set P0.0 to drive low. */
#define B0__HIGH  0x01 /* P0.0 is high. Set P0.0 to drive or float high. */

#define B1__BMASK 0x02 /* Port 0 Bit 1 Latch */
#define B1__SHIFT 0x01 /* Port 0 Bit 1 Latch */
#define B1__LOW   0x00 /* P0.1 is low. Set P0.1 to drive low. */
#define B1__HIGH  0x02 /* P0.1 is high. Set P0.1 to drive or float high. */

#define B2__BMASK 0x04 /* Port 0 Bit 2 Latch */
#define B2__SHIFT 0x02 /* Port 0 Bit 2 Latch */
#define B2__LOW   0x00 /* P0.2 is low. Set P0.2 to drive low. */
#define B2__HIGH  0x04 /* P0.2 is high. Set P0.2 to drive or float high. */

#define B3__BMASK 0x08 /* Port 0 Bit 3 Latch */
#define B3__SHIFT 0x03 /* Port 0 Bit 3 Latch */
#define B3__LOW   0x00 /* P0.3 is low. Set P0.3 to drive low. */
#define B3__HIGH  0x08 /* P0.3 is high. Set P0.3 to drive or float high. */

#define B4__BMASK 0x10 /* Port 0 Bit 4 Latch */
#define B4__SHIFT 0x04 /* Port 0 Bit 4 Latch */
#define B4__LOW   0x00 /* P0.4 is low. Set P0.4 to drive low. */
#define B4__HIGH  0x10 /* P0.4 is high. Set P0.4 to drive or float high. */

#define B5__BMASK 0x20 /* Port 0 Bit 5 Latch */
#define B5__SHIFT 0x05 /* Port 0 Bit 5 Latch */
#define B5__LOW   0x00 /* P0.5 is low. Set P0.5 to drive low. */
#define B5__HIGH  0x20 /* P0.5 is high. Set P0.5 to drive or float high. */

#define B6__BMASK 0x40 /* Port 0 Bit 6 Latch */
#define B6__SHIFT 0x06 /* Port 0 Bit 6 Latch */
#define B6__LOW   0x00 /* P0.6 is low. Set P0.6 to drive low. */
#define B6__HIGH  0x40 /* P0.6 is high. Set P0.6 to drive or float high. */

#define B7__BMASK 0x80 /* Port 0 Bit 7 Latch */
#define B7__SHIFT 0x07 /* Port 0 Bit 7 Latch */
#define B7__LOW   0x00 /* P0.7 is low. Set P0.7 to drive low. */
#define B7__HIGH  0x80 /* P0.7 is high. Set P0.7 to drive or float high. */

/*------------------------------------------------------------------------------ */
/* P0MASK Enums (Port 0 Mask @ 0xFE)                                             */
/*------------------------------------------------------------------------------ */
#define B0__BMASK    0x01 /* Port 0 Bit 0 Mask Value */
#define B0__SHIFT    0x00 /* Port 0 Bit 0 Mask Value */
#define B0__IGNORED  0x00 /* P0.0 pin logic value is ignored and will not cause a port mismatch event. */
#define B0__COMPARED 0x01 /* P0.0 pin logic value is compared to P0MAT.0. */

#define B1__BMASK    0x02 /* Port 0 Bit 1 Mask Value */
#define B1__SHIFT    0x01 /* Port 0 Bit 1 Mask Value */
#define B1__IGNORED  0x00 /* P0.1 pin logic value is ignored and will not cause a port mismatch event. */
#define B1__COMPARED 0x02 /* P0.1 pin logic value is compared to P0MAT.1. */

#define B2__BMASK    0x04 /* Port 0 Bit 2 Mask Value */
#define B2__SHIFT    0x02 /* Port 0 Bit 2 Mask Value */
#define B2__IGNORED  0x00 /* P0.2 pin logic value is ignored and will not cause a port mismatch event. */
#define B2__COMPARED 0x04 /* P0.2 pin logic value is compared to P0MAT.2. */

#define B3__BMASK    0x08 /* Port 0 Bit 3 Mask Value */
#define B3__SHIFT    0x03 /* Port 0 Bit 3 Mask Value */
#define B3__IGNORED  0x00 /* P0.3 pin logic value is ignored and will not cause a port mismatch event. */
#define B3__COMPARED 0x08 /* P0.3 pin logic value is compared to P0MAT.3. */

#define B4__BMASK    0x10 /* Port 0 Bit 4 Mask Value */
#define B4__SHIFT    0x04 /* Port 0 Bit 4 Mask Value */
#define B4__IGNORED  0x00 /* P0.4 pin logic value is ignored and will not cause a port mismatch event. */
#define B4__COMPARED 0x10 /* P0.4 pin logic value is compared to P0MAT.4. */

#define B5__BMASK    0x20 /* Port 0 Bit 5 Mask Value */
#define B5__SHIFT    0x05 /* Port 0 Bit 5 Mask Value */
#define B5__IGNORED  0x00 /* P0.5 pin logic value is ignored and will not cause a port mismatch event. */
#define B5__COMPARED 0x20 /* P0.5 pin logic value is compared to P0MAT.5. */

#define B6__BMASK    0x40 /* Port 0 Bit 6 Mask Value */
#define B6__SHIFT    0x06 /* Port 0 Bit 6 Mask Value */
#define B6__IGNORED  0x00 /* P0.6 pin logic value is ignored and will not cause a port mismatch event. */
#define B6__COMPARED 0x40 /* P0.6 pin logic value is compared to P0MAT.6. */

#define B7__BMASK    0x80 /* Port 0 Bit 7 Mask Value */
#define B7__SHIFT    0x07 /* Port 0 Bit 7 Mask Value */
#define B7__IGNORED  0x00 /* P0.7 pin logic value is ignored and will not cause a port mismatch event. */
#define B7__COMPARED 0x80 /* P0.7 pin logic value is compared to P0MAT.7. */

/*------------------------------------------------------------------------------ */
/* P0MAT Enums (Port 0 Match @ 0xFD) */
/*------------------------------------------------------------------------------ */
#define B0__BMASK 0x01 /* Port 0 Bit 0 Match Value */
#define B0__SHIFT 0x00 /* Port 0 Bit 0 Match Value */
#define B0__LOW   0x00 /* P0.0 pin logic value is compared with logic LOW. */
#define B0__HIGH  0x01 /* P0.0 pin logic value is compared with logic HIGH. */

#define B1__BMASK 0x02 /* Port 0 Bit 1 Match Value */
#define B1__SHIFT 0x01 /* Port 0 Bit 1 Match Value */
#define B1__LOW   0x00 /* P0.1 pin logic value is compared with logic LOW. */
#define B1__HIGH  0x02 /* P0.1 pin logic value is compared with logic HIGH. */

#define B2__BMASK 0x04 /* Port 0 Bit 2 Match Value */
#define B2__SHIFT 0x02 /* Port 0 Bit 2 Match Value */
#define B2__LOW   0x00 /* P0.2 pin logic value is compared with logic LOW. */
#define B2__HIGH  0x04 /* P0.2 pin logic value is compared with logic HIGH. */

#define B3__BMASK 0x08 /* Port 0 Bit 3 Match Value */
#define B3__SHIFT 0x03 /* Port 0 Bit 3 Match Value */
#define B3__LOW   0x00 /* P0.3 pin logic value is compared with logic LOW. */
#define B3__HIGH  0x08 /* P0.3 pin logic value is compared with logic HIGH. */

#define B4__BMASK 0x10 /* Port 0 Bit 4 Match Value */
#define B4__SHIFT 0x04 /* Port 0 Bit 4 Match Value */
#define B4__LOW   0x00 /* P0.4 pin logic value is compared with logic LOW. */
#define B4__HIGH  0x10 /* P0.4 pin logic value is compared with logic HIGH. */

#define B5__BMASK 0x20 /* Port 0 Bit 5 Match Value */
#define B5__SHIFT 0x05 /* Port 0 Bit 5 Match Value */
#define B5__LOW   0x00 /* P0.5 pin logic value is compared with logic LOW. */
#define B5__HIGH  0x20 /* P0.5 pin logic value is compared with logic HIGH. */

#define B6__BMASK 0x40 /* Port 0 Bit 6 Match Value */
#define B6__SHIFT 0x06 /* Port 0 Bit 6 Match Value */
#define B6__LOW   0x00 /* P0.6 pin logic value is compared with logic LOW. */
#define B6__HIGH  0x40 /* P0.6 pin logic value is compared with logic HIGH. */

#define B7__BMASK 0x80 /* Port 0 Bit 7 Match Value */
#define B7__SHIFT 0x07 /* Port 0 Bit 7 Match Value */
#define B7__LOW   0x00 /* P0.7 pin logic value is compared with logic LOW. */
#define B7__HIGH  0x80 /* P0.7 pin logic value is compared with logic HIGH. */

/*------------------------------------------------------------------------------ */
/* P1 Enums (Port 1 Pin Latch @ 0x90) */
/*------------------------------------------------------------------------------ */
#define B0__BMASK 0x01 /* Port 1 Bit 0 Latch */
#define B0__SHIFT 0x00 /* Port 1 Bit 0 Latch */
#define B0__LOW   0x00 /* P1.0 is low. Set P1.0 to drive low. */
#define B0__HIGH  0x01 /* P1.0 is high. Set P1.0 to drive or float high. */

#define B1__BMASK 0x02 /* Port 1 Bit 1 Latch */
#define B1__SHIFT 0x01 /* Port 1 Bit 1 Latch */
#define B1__LOW   0x00 /* P1.1 is low. Set P1.1 to drive low. */
#define B1__HIGH  0x02 /* P1.1 is high. Set P1.1 to drive or float high. */

#define B2__BMASK 0x04 /* Port 1 Bit 2 Latch */
#define B2__SHIFT 0x02 /* Port 1 Bit 2 Latch */
#define B2__LOW   0x00 /* P1.2 is low. Set P1.2 to drive low. */
#define B2__HIGH  0x04 /* P1.2 is high. Set P1.2 to drive or float high. */

#define B3__BMASK 0x08 /* Port 1 Bit 3 Latch */
#define B3__SHIFT 0x03 /* Port 1 Bit 3 Latch */
#define B3__LOW   0x00 /* P1.3 is low. Set P1.3 to drive low. */
#define B3__HIGH  0x08 /* P1.3 is high. Set P1.3 to drive or float high. */

#define B4__BMASK 0x10 /* Port 1 Bit 4 Latch */
#define B4__SHIFT 0x04 /* Port 1 Bit 4 Latch */
#define B4__LOW   0x00 /* P1.4 is low. Set P1.4 to drive low. */
#define B4__HIGH  0x10 /* P1.4 is high. Set P1.4 to drive or float high. */

#define B5__BMASK 0x20 /* Port 1 Bit 5 Latch */
#define B5__SHIFT 0x05 /* Port 1 Bit 5 Latch */
#define B5__LOW   0x00 /* P1.5 is low. Set P1.5 to drive low. */
#define B5__HIGH  0x20 /* P1.5 is high. Set P1.5 to drive or float high. */

#define B6__BMASK 0x40 /* Port 1 Bit 6 Latch */
#define B6__SHIFT 0x06 /* Port 1 Bit 6 Latch */
#define B6__LOW   0x00 /* P1.6 is low. Set P1.6 to drive low. */
#define B6__HIGH  0x40 /* P1.6 is high. Set P1.6 to drive or float high. */

#define B7__BMASK 0x80 /* Port 1 Bit 7 Latch */
#define B7__SHIFT 0x07 /* Port 1 Bit 7 Latch */
#define B7__LOW   0x00 /* P1.7 is low. Set P1.7 to drive low. */
#define B7__HIGH  0x80 /* P1.7 is high. Set P1.7 to drive or float high. */

/*------------------------------------------------------------------------------ */
/* P1MASK Enums (Port 1 Mask @ 0xEE) */
/*------------------------------------------------------------------------------ */
#define B0__BMASK    0x01 /* Port 1 Bit 0 Mask Value */
#define B0__SHIFT    0x00 /* Port 1 Bit 0 Mask Value */
#define B0__IGNORED  0x00 /* P1.0 pin logic value is ignored and will not cause a port mismatch event. */
#define B0__COMPARED 0x01 /* P1.0 pin logic value is compared to P1MAT.0. */

#define B1__BMASK    0x02 /* Port 1 Bit 1 Mask Value */
#define B1__SHIFT    0x01 /* Port 1 Bit 1 Mask Value */
#define B1__IGNORED  0x00 /* P1.1 pin logic value is ignored and will not cause a port mismatch event. */
#define B1__COMPARED 0x02 /* P1.1 pin logic value is compared to P1MAT.1. */

#define B2__BMASK    0x04 /* Port 1 Bit 2 Mask Value */
#define B2__SHIFT    0x02 /* Port 1 Bit 2 Mask Value */
#define B2__IGNORED  0x00 /* P1.2 pin logic value is ignored and will not cause a port mismatch event. */
#define B2__COMPARED 0x04 /* P1.2 pin logic value is compared to P1MAT.2. */

#define B3__BMASK    0x08 /* Port 1 Bit 3 Mask Value */
#define B3__SHIFT    0x03 /* Port 1 Bit 3 Mask Value */
#define B3__IGNORED  0x00 /* P1.3 pin logic value is ignored and will not cause a port mismatch event. */
#define B3__COMPARED 0x08 /* P1.3 pin logic value is compared to P1MAT.3. */

#define B4__BMASK    0x10 /* Port 1 Bit 4 Mask Value */
#define B4__SHIFT    0x04 /* Port 1 Bit 4 Mask Value */
#define B4__IGNORED  0x00 /* P1.4 pin logic value is ignored and will not cause a port mismatch event. */
#define B4__COMPARED 0x10 /* P1.4 pin logic value is compared to P1MAT.4. */

#define B5__BMASK    0x20 /* Port 1 Bit 5 Mask Value */
#define B5__SHIFT    0x05 /* Port 1 Bit 5 Mask Value */
#define B5__IGNORED  0x00 /* P1.5 pin logic value is ignored and will not cause a port mismatch event. */
#define B5__COMPARED 0x20 /* P1.5 pin logic value is compared to P1MAT.5. */

#define B6__BMASK    0x40 /* Port 1 Bit 6 Mask Value */
#define B6__SHIFT    0x06 /* Port 1 Bit 6 Mask Value */
#define B6__IGNORED  0x00 /* P1.6 pin logic value is ignored and will not cause a port mismatch event. */
#define B6__COMPARED 0x40 /* P1.6 pin logic value is compared to P1MAT.6. */

#define B7__BMASK    0x80 /* Port 1 Bit 7 Mask Value */
#define B7__SHIFT    0x07 /* Port 1 Bit 7 Mask Value */
#define B7__IGNORED  0x00 /* P1.7 pin logic value is ignored and will not cause a port mismatch event. */
#define B7__COMPARED 0x80 /* P1.7 pin logic value is compared to P1MAT.7. */

/*------------------------------------------------------------------------------ */
/* P1MAT Enums (Port 1 Match @ 0xED) */
/*------------------------------------------------------------------------------ */
#define B0__BMASK 0x01 /* Port 1 Bit 0 Match Value */
#define B0__SHIFT 0x00 /* Port 1 Bit 0 Match Value */
#define B0__LOW   0x00 /* P1.0 pin logic value is compared with logic LOW. */
#define B0__HIGH  0x01 /* P1.0 pin logic value is compared with logic HIGH. */

#define B1__BMASK 0x02 /* Port 1 Bit 1 Match Value */
#define B1__SHIFT 0x01 /* Port 1 Bit 1 Match Value */
#define B1__LOW   0x00 /* P1.1 pin logic value is compared with logic LOW. */
#define B1__HIGH  0x02 /* P1.1 pin logic value is compared with logic HIGH. */

#define B2__BMASK 0x04 /* Port 1 Bit 2 Match Value */
#define B2__SHIFT 0x02 /* Port 1 Bit 2 Match Value */
#define B2__LOW   0x00 /* P1.2 pin logic value is compared with logic LOW. */
#define B2__HIGH  0x04 /* P1.2 pin logic value is compared with logic HIGH. */

#define B3__BMASK 0x08 /* Port 1 Bit 3 Match Value */
#define B3__SHIFT 0x03 /* Port 1 Bit 3 Match Value */
#define B3__LOW   0x00 /* P1.3 pin logic value is compared with logic LOW. */
#define B3__HIGH  0x08 /* P1.3 pin logic value is compared with logic HIGH. */

#define B4__BMASK 0x10 /* Port 1 Bit 4 Match Value */
#define B4__SHIFT 0x04 /* Port 1 Bit 4 Match Value */
#define B4__LOW   0x00 /* P1.4 pin logic value is compared with logic LOW. */
#define B4__HIGH  0x10 /* P1.4 pin logic value is compared with logic HIGH. */

#define B5__BMASK 0x20 /* Port 1 Bit 5 Match Value */
#define B5__SHIFT 0x05 /* Port 1 Bit 5 Match Value */
#define B5__LOW   0x00 /* P1.5 pin logic value is compared with logic LOW. */
#define B5__HIGH  0x20 /* P1.5 pin logic value is compared with logic HIGH. */

#define B6__BMASK 0x40 /* Port 1 Bit 6 Match Value */
#define B6__SHIFT 0x06 /* Port 1 Bit 6 Match Value */
#define B6__LOW   0x00 /* P1.6 pin logic value is compared with logic LOW. */
#define B6__HIGH  0x40 /* P1.6 pin logic value is compared with logic HIGH. */

#define B7__BMASK 0x80 /* Port 1 Bit 7 Match Value */
#define B7__SHIFT 0x07 /* Port 1 Bit 7 Match Value */
#define B7__LOW   0x00 /* P1.7 pin logic value is compared with logic LOW. */
#define B7__HIGH  0x80 /* P1.7 pin logic value is compared with logic HIGH. */

/*------------------------------------------------------------------------------ */
/* P1MDIN Enums (Port 1 Input Mode @ 0xF2) */
/*------------------------------------------------------------------------------ */
#define B0__BMASK   0x01 /* Port 1 Bit 0 Input Mode */
#define B0__SHIFT   0x00 /* Port 1 Bit 0 Input Mode */
#define B0__ANALOG  0x00 /* P1.0 pin is configured for analog mode. */
#define B0__DIGITAL 0x01 /* P1.0 pin is configured for digital mode. */

#define B1__BMASK   0x02 /* Port 1 Bit 1 Input Mode */
#define B1__SHIFT   0x01 /* Port 1 Bit 1 Input Mode */
#define B1__ANALOG  0x00 /* P1.1 pin is configured for analog mode. */
#define B1__DIGITAL 0x02 /* P1.1 pin is configured for digital mode. */

#define B2__BMASK   0x04 /* Port 1 Bit 2 Input Mode */
#define B2__SHIFT   0x02 /* Port 1 Bit 2 Input Mode */
#define B2__ANALOG  0x00 /* P1.2 pin is configured for analog mode. */
#define B2__DIGITAL 0x04 /* P1.2 pin is configured for digital mode. */

#define B3__BMASK   0x08 /* Port 1 Bit 3 Input Mode */
#define B3__SHIFT   0x03 /* Port 1 Bit 3 Input Mode */
#define B3__ANALOG  0x00 /* P1.3 pin is configured for analog mode. */
#define B3__DIGITAL 0x08 /* P1.3 pin is configured for digital mode. */

#define B4__BMASK   0x10 /* Port 1 Bit 4 Input Mode */
#define B4__SHIFT   0x04 /* Port 1 Bit 4 Input Mode */
#define B4__ANALOG  0x00 /* P1.4 pin is configured for analog mode. */
#define B4__DIGITAL 0x10 /* P1.4 pin is configured for digital mode. */

#define B5__BMASK   0x20 /* Port 1 Bit 5 Input Mode */
#define B5__SHIFT   0x05 /* Port 1 Bit 5 Input Mode */
#define B5__ANALOG  0x00 /* P1.5 pin is configured for analog mode. */
#define B5__DIGITAL 0x20 /* P1.5 pin is configured for digital mode. */

#define B6__BMASK   0x40 /* Port 1 Bit 6 Input Mode */
#define B6__SHIFT   0x06 /* Port 1 Bit 6 Input Mode */
#define B6__ANALOG  0x00 /* P1.6 pin is configured for analog mode. */
#define B6__DIGITAL 0x40 /* P1.6 pin is configured for digital mode. */

#define B7__BMASK   0x80 /* Port 1 Bit 7 Input Mode */
#define B7__SHIFT   0x07 /* Port 1 Bit 7 Input Mode */
#define B7__ANALOG  0x00 /* P1.7 pin is configured for analog mode. */
#define B7__DIGITAL 0x80 /* P1.7 pin is configured for digital mode. */

/*------------------------------------------------------------------------------ */
/* P1MDOUT Enums (Port 1 Output Mode @ 0xA5)                                     */
/*------------------------------------------------------------------------------ */
#define B0__BMASK      0x01 /* Port 1 Bit 0 Output Mode */
#define B0__SHIFT      0x00 /* Port 1 Bit 0 Output Mode */
#define B0__OPEN_DRAIN 0x00 /* P1.0 output is open-drain. */
#define B0__PUSH_PULL  0x01 /* P1.0 output is push-pull. */

#define B1__BMASK      0x02 /* Port 1 Bit 1 Output Mode */
#define B1__SHIFT      0x01 /* Port 1 Bit 1 Output Mode */
#define B1__OPEN_DRAIN 0x00 /* P1.1 output is open-drain. */
#define B1__PUSH_PULL  0x02 /* P1.1 output is push-pull. */

#define B2__BMASK      0x04 /* Port 1 Bit 2 Output Mode */
#define B2__SHIFT      0x02 /* Port 1 Bit 2 Output Mode */
#define B2__OPEN_DRAIN 0x00 /* P1.2 output is open-drain. */
#define B2__PUSH_PULL  0x04 /* P1.2 output is push-pull. */

#define B3__BMASK      0x08 /* Port 1 Bit 3 Output Mode */
#define B3__SHIFT      0x03 /* Port 1 Bit 3 Output Mode */
#define B3__OPEN_DRAIN 0x00 /* P1.3 output is open-drain. */
#define B3__PUSH_PULL  0x08 /* P1.3 output is push-pull. */

#define B4__BMASK      0x10 /* Port 1 Bit 4 Output Mode */
#define B4__SHIFT      0x04 /* Port 1 Bit 4 Output Mode */
#define B4__OPEN_DRAIN 0x00 /* P1.4 output is open-drain. */
#define B4__PUSH_PULL  0x10 /* P1.4 output is push-pull. */

#define B5__BMASK      0x20 /* Port 1 Bit 5 Output Mode */
#define B5__SHIFT      0x05 /* Port 1 Bit 5 Output Mode */
#define B5__OPEN_DRAIN 0x00 /* P1.5 output is open-drain. */
#define B5__PUSH_PULL  0x20 /* P1.5 output is push-pull. */

#define B6__BMASK      0x40 /* Port 1 Bit 6 Output Mode */
#define B6__SHIFT      0x06 /* Port 1 Bit 6 Output Mode */
#define B6__OPEN_DRAIN 0x00 /* P1.6 output is open-drain. */
#define B6__PUSH_PULL  0x40 /* P1.6 output is push-pull. */

#define B7__BMASK      0x80 /* Port 1 Bit 7 Output Mode */
#define B7__SHIFT      0x07 /* Port 1 Bit 7 Output Mode */
#define B7__OPEN_DRAIN 0x00 /* P1.7 output is open-drain. */
#define B7__PUSH_PULL  0x80 /* P1.7 output is push-pull. */

/*------------------------------------------------------------------------------ */
/* P1SKIP Enums (Port 1 Skip @ 0xD5)                                             */
/*------------------------------------------------------------------------------ */
#define B0__BMASK       0x01 /* Port 1 Bit 0 Skip */
#define B0__SHIFT       0x00 /* Port 1 Bit 0 Skip */
#define B0__NOT_SKIPPED 0x00 /* P1.0 pin is not skipped by the crossbar. */
#define B0__SKIPPED     0x01 /* P1.0 pin is skipped by the crossbar. */

#define B1__BMASK       0x02 /* Port 1 Bit 1 Skip */
#define B1__SHIFT       0x01 /* Port 1 Bit 1 Skip */
#define B1__NOT_SKIPPED 0x00 /* P1.1 pin is not skipped by the crossbar. */
#define B1__SKIPPED     0x02 /* P1.1 pin is skipped by the crossbar. */

#define B2__BMASK       0x04 /* Port 1 Bit 2 Skip */
#define B2__SHIFT       0x02 /* Port 1 Bit 2 Skip */
#define B2__NOT_SKIPPED 0x00 /* P1.2 pin is not skipped by the crossbar. */
#define B2__SKIPPED     0x04 /* P1.2 pin is skipped by the crossbar. */

#define B3__BMASK       0x08 /* Port 1 Bit 3 Skip */
#define B3__SHIFT       0x03 /* Port 1 Bit 3 Skip */
#define B3__NOT_SKIPPED 0x00 /* P1.3 pin is not skipped by the crossbar. */
#define B3__SKIPPED     0x08 /* P1.3 pin is skipped by the crossbar. */

#define B4__BMASK       0x10 /* Port 1 Bit 4 Skip */
#define B4__SHIFT       0x04 /* Port 1 Bit 4 Skip */
#define B4__NOT_SKIPPED 0x00 /* P1.4 pin is not skipped by the crossbar. */
#define B4__SKIPPED     0x10 /* P1.4 pin is skipped by the crossbar. */

#define B5__BMASK       0x20 /* Port 1 Bit 5 Skip */
#define B5__SHIFT       0x05 /* Port 1 Bit 5 Skip */
#define B5__NOT_SKIPPED 0x00 /* P1.5 pin is not skipped by the crossbar. */
#define B5__SKIPPED     0x20 /* P1.5 pin is skipped by the crossbar. */

#define B6__BMASK       0x40 /* Port 1 Bit 6 Skip */
#define B6__SHIFT       0x06 /* Port 1 Bit 6 Skip */
#define B6__NOT_SKIPPED 0x00 /* P1.6 pin is not skipped by the crossbar. */
#define B6__SKIPPED     0x40 /* P1.6 pin is skipped by the crossbar. */

#define B7__BMASK       0x80 /* Port 1 Bit 7 Skip */
#define B7__SHIFT       0x07 /* Port 1 Bit 7 Skip */
#define B7__NOT_SKIPPED 0x00 /* P1.7 pin is not skipped by the crossbar. */
#define B7__SKIPPED     0x80 /* P1.7 pin is skipped by the crossbar. */

/*------------------------------------------------------------------------------ */
/* P2 Enums (Port 2 Pin Latch @ 0xA0)                                            */
/*------------------------------------------------------------------------------ */
#define B0__BMASK 0x01 /* Port 2 Bit 0 Latch */
#define B0__SHIFT 0x00 /* Port 2 Bit 0 Latch */
#define B0__LOW   0x00 /* P2.0 is low. Set P2.0 to drive low. */
#define B0__HIGH  0x01 /* P2.0 is high. Set P2.0 to drive or float high. */

#define B1__BMASK 0x02 /* Port 2 Bit 1 Latch */
#define B1__SHIFT 0x01 /* Port 2 Bit 1 Latch */
#define B1__LOW   0x00 /* P2.1 is low. Set P2.1 to drive low. */
#define B1__HIGH  0x02 /* P2.1 is high. Set P2.1 to drive or float high. */

/*------------------------------------------------------------------------------ */
/* P2MDOUT Enums (Port 2 Output Mode @ 0xA6)                                     */
/*------------------------------------------------------------------------------ */
#define B0__BMASK      0x01 /* Port 2 Bit 0 Output Mode */
#define B0__SHIFT      0x00 /* Port 2 Bit 0 Output Mode */
#define B0__OPEN_DRAIN 0x00 /* P2.0 output is open-drain. */
#define B0__PUSH_PULL  0x01 /* P2.0 output is push-pull. */

#define B1__BMASK      0x02 /* Port 2 Bit 1 Output Mode */
#define B1__SHIFT      0x01 /* Port 2 Bit 1 Output Mode */
#define B1__OPEN_DRAIN 0x00 /* P2.1 output is open-drain. */
#define B1__PUSH_PULL  0x02 /* P2.1 output is push-pull. */

/*------------------------------------------------------------------------------ */
/* XBR0 Enums (Port I/O Crossbar 0 @ 0xE1)                                       */
/*------------------------------------------------------------------------------ */
#define URT0E__BMASK     0x01 /* UART I/O Output Enable */
#define URT0E__SHIFT     0x00 /* UART I/O Output Enable */
#define URT0E__DISABLED  0x00 /* UART I/O unavailable at Port pin. */
#define URT0E__ENABLED   0x01 /* UART TX, RX routed to Port pins P0.4 and P0.5. */

#define SPI0E__BMASK     0x02 /* SPI I/O Enable */
#define SPI0E__SHIFT     0x01 /* SPI I/O Enable */
#define SPI0E__DISABLED  0x00 /* SPI I/O unavailable at Port pins. */
#define SPI0E__ENABLED   0x02 /* SPI I/O routed to Port pins. The SPI can be assigned either 3 or 4 GPIO pins. */

#define SMB0E__BMASK     0x04 /* SMB0 I/O Enable */
#define SMB0E__SHIFT     0x02 /* SMB0 I/O Enable */
#define SMB0E__DISABLED  0x00 /* SMBus 0 I/O unavailable at Port pins. */
#define SMB0E__ENABLED   0x04 /* SMBus 0 I/O routed to Port pins. */

#define CP0E__BMASK      0x08 /* Comparator0 Output Enable */
#define CP0E__SHIFT      0x03 /* Comparator0 Output Enable */
#define CP0E__DISABLED   0x00 /* CP0 unavailable at Port pin. */
#define CP0E__ENABLED    0x08 /* CP0 routed to Port pin. */

#define CP0AE__BMASK     0x10 /* Comparator0 Asynchronous Output Enable */
#define CP0AE__SHIFT     0x04 /* Comparator0 Asynchronous Output Enable */
#define CP0AE__DISABLED  0x00 /* Asynchronous CP0 unavailable at Port pin. */
#define CP0AE__ENABLED   0x10 /* Asynchronous CP0 routed to Port pin. */

#define CP1E__BMASK      0x20 /* Comparator1 Output Enable */
#define CP1E__SHIFT      0x05 /* Comparator1 Output Enable */
#define CP1E__DISABLED   0x00 /* CP1 unavailable at Port pin. */
#define CP1E__ENABLED    0x20 /* CP1 routed to Port pin. */

#define CP1AE__BMASK     0x40 /* Comparator1 Asynchronous Output Enable */
#define CP1AE__SHIFT     0x06 /* Comparator1 Asynchronous Output Enable */
#define CP1AE__DISABLED  0x00 /* Asynchronous CP1 unavailable at Port pin. */
#define CP1AE__ENABLED   0x40 /* Asynchronous CP1 routed to Port pin. */

#define SYSCKE__BMASK    0x80 /* SYSCLK Output Enable */
#define SYSCKE__SHIFT    0x07 /* SYSCLK Output Enable */
#define SYSCKE__DISABLED 0x00 /* SYSCLK unavailable at Port pin. */
#define SYSCKE__ENABLED  0x80 /* SYSCLK output routed to Port pin. */

/*------------------------------------------------------------------------------ */
/* XBR1 Enums (Port I/O Crossbar 1 @ 0xE2)                                       */
/*------------------------------------------------------------------------------ */
#define PCA0ME__FMASK          0x03 /* PCA Module I/O Enable */
#define PCA0ME__SHIFT          0x00 /* PCA Module I/O Enable */
#define PCA0ME__DISABLED       0x00 /* All PCA I/O unavailable at Port pins. */
#define PCA0ME__CEX0           0x01 /* CEX0 routed to Port pin. */
#define PCA0ME__CEX0_CEX1      0x02 /* CEX0, CEX1 routed to Port pins. */
#define PCA0ME__CEX0_CEX1_CEX2 0x03 /* CEX0, CEX1, CEX2 routed to Port pins. */

#define ECIE__BMASK            0x04 /* PCA0 External Counter Input Enable */
#define ECIE__SHIFT            0x02 /* PCA0 External Counter Input Enable */
#define ECIE__DISABLED         0x00 /* ECI unavailable at Port pin. */
#define ECIE__ENABLED          0x04 /* ECI routed to Port pin. */

#define T0E__BMASK             0x08 /* T0 Enable */
#define T0E__SHIFT             0x03 /* T0 Enable */
#define T0E__DISABLED          0x00 /* T0 unavailable at Port pin. */
#define T0E__ENABLED           0x08 /* T0 routed to Port pin. */

#define T1E__BMASK             0x10 /* T1 Enable */
#define T1E__SHIFT             0x04 /* T1 Enable */
#define T1E__DISABLED          0x00 /* T1 unavailable at Port pin. */
#define T1E__ENABLED           0x10 /* T1 routed to Port pin. */

#define T2E__BMASK             0x20 /* T2 Enable */
#define T2E__SHIFT             0x05 /* T2 Enable */
#define T2E__DISABLED          0x00 /* T2 unavailable at Port pin. */
#define T2E__ENABLED           0x20 /* T2 routed to Port pin. */

/*------------------------------------------------------------------------------ */
/* XBR2 Enums (Port I/O Crossbar 2 @ 0xE3)                                       */
/*------------------------------------------------------------------------------ */
#define XBARE__BMASK               0x40 /* Crossbar Enable */
#define XBARE__SHIFT               0x06 /* Crossbar Enable */
#define XBARE__DISABLED            0x00 /* Crossbar disabled. */
#define XBARE__ENABLED             0x40 /* Crossbar enabled. */

#define WEAKPUD__BMASK             0x80 /* Port I/O Weak Pullup Disable */
#define WEAKPUD__SHIFT             0x07 /* Port I/O Weak Pullup Disable */
#define WEAKPUD__PULL_UPS_ENABLED  0x00 /* Weak Pullups enabled (except for Ports whose I/O are configured for analog mode). */
#define WEAKPUD__PULL_UPS_DISABLED 0x80 /* Weak Pullups disabled. */

/*------------------------------------------------------------------------------ */
/* ADC0CN1 Enums (ADC0 Control 1 @ 0xB2)                                         */
/*------------------------------------------------------------------------------ */
#define ADCMBE__BMASK              0x01 /* Common Mode Buffer Enable */
#define ADCMBE__SHIFT              0x00 /* Common Mode Buffer Enable */
#define ADCMBE__CM_BUFFER_DISABLED 0x00 /* Disable the common mode buffer. This setting should be used only if the tracking time of the signal is greater than 1.5 us. */
#define ADCMBE__CM_BUFFER_ENABLED  0x01 /* Enable the common mode buffer. This setting should be used in most cases, and will give the best dynamic ADC performance. The common mode buffer must be enabled if signal tracking time is less than or equal to 1.5 us. */

/*------------------------------------------------------------------------------ */
/* ADC0GTH Enums (ADC0 Greater-Than High Byte @ 0xC4)                            */
/*------------------------------------------------------------------------------ */
#define ADC0GTH__FMASK 0xFF /* Greater-Than High Byte */
#define ADC0GTH__SHIFT 0x00 /* Greater-Than High Byte */

/*------------------------------------------------------------------------------ */
/* ADC0GTL Enums (ADC0 Greater-Than Low Byte @ 0xC3)                             */
/*------------------------------------------------------------------------------ */
#define ADC0GTL__FMASK 0xFF /* Greater-Than Low Byte */
#define ADC0GTL__SHIFT 0x00 /* Greater-Than Low Byte */

/*------------------------------------------------------------------------------ */
/* ADC0H Enums (ADC0 Data Word High Byte @ 0xBE)                                 */
/*------------------------------------------------------------------------------ */
#define ADC0H__FMASK 0xFF /* Data Word High Byte */
#define ADC0H__SHIFT 0x00 /* Data Word High Byte */

/*------------------------------------------------------------------------------ */
/* ADC0L Enums (ADC0 Data Word Low Byte @ 0xBD)                                  */
/*------------------------------------------------------------------------------ */
#define ADC0L__FMASK 0xFF /* Data Word Low Byte */
#define ADC0L__SHIFT 0x00 /* Data Word Low Byte */

/*------------------------------------------------------------------------------ */
/* ADC0LTH Enums (ADC0 Less-Than High Byte @ 0xC6)                               */
/*------------------------------------------------------------------------------ */
#define ADC0LTH__FMASK 0xFF /* Less-Than High Byte */
#define ADC0LTH__SHIFT 0x00 /* Less-Than High Byte */

/*------------------------------------------------------------------------------ */
/* ADC0LTL Enums (ADC0 Less-Than Low Byte @ 0xC5)                                */
/*------------------------------------------------------------------------------ */
#define ADC0LTL__FMASK 0xFF /* Less-Than Low Byte */
#define ADC0LTL__SHIFT 0x00 /* Less-Than Low Byte */

/*------------------------------------------------------------------------------ */
/* ADC0MX Enums (ADC0 Multiplexer Selection @ 0xBB)                              */
/*------------------------------------------------------------------------------ */
#define ADC0MX__FMASK   0x1F /* AMUX0 Positive Input Selection */
#define ADC0MX__SHIFT   0x00 /* AMUX0 Positive Input Selection */
#define ADC0MX__ADC0P0  0x00 /* Select ADC0.0. */
#define ADC0MX__ADC0P1  0x01 /* Select ADC0.1. */
#define ADC0MX__ADC0P2  0x02 /* Select ADC0.2. */
#define ADC0MX__ADC0P3  0x03 /* Select ADC0.3. */
#define ADC0MX__ADC0P4  0x04 /* Select ADC0.4. */
#define ADC0MX__ADC0P5  0x05 /* Select ADC0.5. */
#define ADC0MX__ADC0P6  0x06 /* Select ADC0.6. */
#define ADC0MX__ADC0P7  0x07 /* Select ADC0.7. */
#define ADC0MX__ADC0P8  0x08 /* Select ADC0.8. */
#define ADC0MX__ADC0P9  0x09 /* Select ADC0.9. */
#define ADC0MX__ADC0P10 0x0A /* Select ADC0.10. */
#define ADC0MX__ADC0P11 0x0B /* Select ADC0.11. */
#define ADC0MX__ADC0P12 0x0C /* Select ADC0.12. */
#define ADC0MX__ADC0P13 0x0D /* Select ADC0.13. */
#define ADC0MX__ADC0P14 0x0E /* Select ADC0.14. */
#define ADC0MX__ADC0P15 0x0F /* Select ADC0.15. */
#define ADC0MX__TEMP    0x10 /* Select ADC0.16. */
#define ADC0MX__LDO_OUT 0x11 /* Select ADC0.17. */
#define ADC0MX__VDD     0x12 /* Select ADC0.18. */
#define ADC0MX__GND     0x13 /* Select ADC0.19. */
#define ADC0MX__NONE    0x1F /* No input selected. */

/*------------------------------------------------------------------------------ */
/* ADC0PWR Enums (ADC0 Power Control @ 0xDF)                                     */
/*------------------------------------------------------------------------------ */
#define ADPWR__FMASK                 0x0F /* Burst Mode Power Up Time */
#define ADPWR__SHIFT                 0x00 /* Burst Mode Power Up Time */

#define ADLPM__BMASK                 0x10 /* Low Power Mode Enable */
#define ADLPM__SHIFT                 0x04 /* Low Power Mode Enable */
#define ADLPM__LP_BUFFER_DISABLED    0x00 /* Disable low power mode. */
#define ADLPM__LP_BUFFER_ENABLED     0x10 /* Enable low power mode (requires extended tracking time). */

#define ADMXLP__BMASK                0x20 /* Mux and Reference Low Power Mode Enable */
#define ADMXLP__SHIFT                0x05 /* Mux and Reference Low Power Mode Enable */
#define ADMXLP__LP_MUX_VREF_DISABLED 0x00 /* Low power mode disabled. */
#define ADMXLP__LP_MUX_VREF_ENABLED  0x20 /* Low power mode enabled (SAR clock < 4 MHz). */

#define ADBIAS__FMASK                0xC0 /* Bias Power Select */
#define ADBIAS__SHIFT                0x06 /* Bias Power Select */
#define ADBIAS__MODE0                0x00 /* Select bias current mode 0. Recommended to use modes 1, 2, or 3. */
#define ADBIAS__MODE1                0x40 /* Select bias current mode 1 (SARCLK <= 16 MHz). */
#define ADBIAS__MODE2                0x80 /* Select bias current mode 2. */
#define ADBIAS__MODE3                0xC0 /* Select bias current mode 3 (SARCLK <= 4 MHz). */

/*------------------------------------------------------------------------------ */
/* ADC0TK Enums (ADC0 Burst Mode Track Time @ 0xB9)                              */
/*------------------------------------------------------------------------------ */
#define ADTK__FMASK         0x3F /* Burst Mode Tracking Time */
#define ADTK__SHIFT         0x00 /* Burst Mode Tracking Time */

#define AD12SM__BMASK       0x80 /* 12-Bit Sampling Mode */
#define AD12SM__SHIFT       0x07 /* 12-Bit Sampling Mode */
#define AD12SM__SAMPLE_FOUR 0x00 /* The ADC will re-track and sample the input four times during a 12-bit conversion. */
#define AD12SM__SAMPLE_ONCE 0x80 /* The ADC will sample the input once at the beginning of each 12-bit conversion. The ADTK field can be set to 63 to maximize throughput. */

/*------------------------------------------------------------------------------ */
/* REF0CN Enums (Voltage Reference Control @ 0xD1)                               */
/*------------------------------------------------------------------------------ */
#define TEMPE__BMASK         0x04 /* Temperature Sensor Enable */
#define TEMPE__SHIFT         0x02 /* Temperature Sensor Enable */
#define TEMPE__TEMP_DISABLED 0x00 /* Disable the Temperature Sensor. */
#define TEMPE__TEMP_ENABLED  0x04 /* Enable the Temperature Sensor. */

#define REFSL__FMASK         0x18 /* Voltage Reference Select */
#define REFSL__SHIFT         0x03 /* Voltage Reference Select */
#define REFSL__VREF_PIN      0x00 /* The ADC0 voltage reference is the P0.0/VREF pin. */
#define REFSL__VDD_PIN       0x08 /* The ADC0 voltage reference is the VDD pin. */
#define REFSL__INTERNAL_LDO  0x10 /* The ADC0 voltage reference is the internal 1.8 V digital supply voltage. */
#define REFSL__INTERNAL_VREF 0x18 /* The ADC0 voltage reference is the internal voltage reference. */

#define GNDSL__BMASK         0x20 /* Analog Ground Reference */
#define GNDSL__SHIFT         0x05 /* Analog Ground Reference */
#define GNDSL__GND_PIN       0x00 /* The ADC0 ground reference is the GND pin. */
#define GNDSL__AGND_PIN      0x20 /* The ADC0 ground reference is the P0.1/AGND pin. */

#define IREFLVL__BMASK       0x80 /* Internal Voltage Reference Level */
#define IREFLVL__SHIFT       0x07 /* Internal Voltage Reference Level */
#define IREFLVL__1P65        0x00 /* The internal reference operates at 1.65 V nominal. */
#define IREFLVL__2P4         0x80 /* The internal reference operates at 2.4 V nominal. */

/*------------------------------------------------------------------------------ */
/* REG0CN Enums (Voltage Regulator 0 Control @ 0xC9)                             */
/*------------------------------------------------------------------------------ */
#define STOPCF__BMASK    0x08 /* Stop Mode Configuration */
#define STOPCF__SHIFT    0x03 /* Stop Mode Configuration */
#define STOPCF__ACTIVE   0x00 /* Regulator is still active in stop mode. Any enabled reset source will reset the device. */
#define STOPCF__SHUTDOWN 0x08 /* Regulator is shut down in stop mode. Only the RSTb pin or power cycle can reset the device. */

/*------------------------------------------------------------------------------ */
/* CLKSEL Enums (Clock Select @ 0xA9)                                            */
/*------------------------------------------------------------------------------ */
#define CLKSL__FMASK           0x03 /* Clock Source Select */
#define CLKSL__SHIFT           0x00 /* Clock Source Select */
#define CLKSL__HFOSC           0x00 /* Clock derived from the Internal High-Frequency Oscillator. */
#define CLKSL__EXTOSC          0x01 /* Clock derived from the External CMOS clock circuit. */
#define CLKSL__LFOSC           0x02 /* Clock derived from the Internal Low-Frequency Oscillator. */

#define CLKDIV__FMASK          0x70 /* Clock Source Divider */
#define CLKDIV__SHIFT          0x04 /* Clock Source Divider */
#define CLKDIV__SYSCLK_DIV_1   0x00 /* SYSCLK is equal to selected clock source divided by 1. */
#define CLKDIV__SYSCLK_DIV_2   0x10 /* SYSCLK is equal to selected clock source divided by 2. */
#define CLKDIV__SYSCLK_DIV_4   0x20 /* SYSCLK is equal to selected clock source divided by 4. */
#define CLKDIV__SYSCLK_DIV_8   0x30 /* SYSCLK is equal to selected clock source divided by 8. */
#define CLKDIV__SYSCLK_DIV_16  0x40 /* SYSCLK is equal to selected clock source divided by 16. */
#define CLKDIV__SYSCLK_DIV_32  0x50 /* SYSCLK is equal to selected clock source divided by 32. */
#define CLKDIV__SYSCLK_DIV_64  0x60 /* SYSCLK is equal to selected clock source divided by 64. */
#define CLKDIV__SYSCLK_DIV_128 0x70 /* SYSCLK is equal to selected clock source divided by 128. */

/*------------------------------------------------------------------------------ */
/* TMR2CN0 Enums (Timer 2 Control 0 @ 0xC8)                                      */
/*------------------------------------------------------------------------------ */
#define T2XCLK__BMASK          0x01 /* Timer 2 External Clock Select */
#define T2XCLK__SHIFT          0x00 /* Timer 2 External Clock Select */
#define T2XCLK__SYSCLK_DIV_12  0x00 /* Timer 2 clock is the system clock divided by 12. */
#define T2XCLK__EXTOSC_DIV_8   0x01 /* Timer 2 clock is the external oscillator divided by 8 (synchronized with SYSCLK). */

#define TR2__BMASK             0x04 /* Timer 2 Run Control */
#define TR2__SHIFT             0x02 /* Timer 2 Run Control */
#define TR2__STOP              0x00 /* Stop Timer 2. */
#define TR2__RUN               0x04 /* Start Timer 2 running. */

#define T2SPLIT__BMASK         0x08 /* Timer 2 Split Mode Enable */
#define T2SPLIT__SHIFT         0x03 /* Timer 2 Split Mode Enable */
#define T2SPLIT__16_BIT_RELOAD 0x00 /* Timer 2 operates in 16-bit auto-reload mode. */
#define T2SPLIT__8_BIT_RELOAD  0x08 /* Timer 2 operates as two 8-bit auto-reload timers. */

#define TF2CEN__BMASK          0x10 /* Timer 2 Capture Enable */
#define TF2CEN__SHIFT          0x04 /* Timer 2 Capture Enable */
#define TF2CEN__DISABLED       0x00 /* Disable capture mode. */
#define TF2CEN__ENABLED        0x10 /* Enable capture mode. */

#define TF2LEN__BMASK          0x20 /* Timer 2 Low Byte Interrupt Enable */
#define TF2LEN__SHIFT          0x05 /* Timer 2 Low Byte Interrupt Enable */
#define TF2LEN__DISABLED       0x00 /* Disable low byte interrupts. */
#define TF2LEN__ENABLED        0x20 /* Enable low byte interrupts. */

#define TF2L__BMASK            0x40 /* Timer 2 Low Byte Overflow Flag */
#define TF2L__SHIFT            0x06 /* Timer 2 Low Byte Overflow Flag */
#define TF2L__NOT_SET          0x00 /* Timer 2 low byte did not overflow. */
#define TF2L__SET              0x40 /* Timer 2 low byte overflowed. */

#define TF2H__BMASK            0x80 /* Timer 2 High Byte Overflow Flag */
#define TF2H__SHIFT            0x07 /* Timer 2 High Byte Overflow Flag */
#define TF2H__NOT_SET          0x00 /* Timer 2 8-bit high byte or 16-bit value did not overflow. */
#define TF2H__SET              0x80 /* Timer 2 8-bit high byte or 16-bit value overflowed. */

/*------------------------------------------------------------------------------ */
/* TMR3CN0 Enums (Timer 3 Control 0 @ 0x91)                                      */
/*------------------------------------------------------------------------------ */
#define T3XCLK__BMASK          0x01 /* Timer 3 External Clock Select */
#define T3XCLK__SHIFT          0x00 /* Timer 3 External Clock Select */
#define T3XCLK__SYSCLK_DIV_12  0x00 /* Timer 3 clock is the system clock divided by 12. */
#define T3XCLK__EXTOSC_DIV_8   0x01 /* Timer 3 clock is the external oscillator divided by 8 (synchronized with SYSCLK). */

#define TR3__BMASK             0x04 /* Timer 3 Run Control */
#define TR3__SHIFT             0x02 /* Timer 3 Run Control */
#define TR3__STOP              0x00 /* Stop Timer 3. */
#define TR3__RUN               0x04 /* Start Timer 3 running. */

#define T3SPLIT__BMASK         0x08 /* Timer 3 Split Mode Enable */
#define T3SPLIT__SHIFT         0x03 /* Timer 3 Split Mode Enable */
#define T3SPLIT__16_BIT_RELOAD 0x00 /* Timer 3 operates in 16-bit auto-reload mode. */
#define T3SPLIT__8_BIT_RELOAD  0x08 /* Timer 3 operates as two 8-bit auto-reload timers. */

#define TF3CEN__BMASK          0x10 /* Timer 3 Capture Enable */
#define TF3CEN__SHIFT          0x04 /* Timer 3 Capture Enable */
#define TF3CEN__DISABLED       0x00 /* Disable capture mode. */
#define TF3CEN__ENABLED        0x10 /* Enable capture mode. */

#define TF3LEN__BMASK          0x20 /* Timer 3 Low Byte Interrupt Enable */
#define TF3LEN__SHIFT          0x05 /* Timer 3 Low Byte Interrupt Enable */
#define TF3LEN__DISABLED       0x00 /* Disable low byte interrupts. */
#define TF3LEN__ENABLED        0x20 /* Enable low byte interrupts. */

#define TF3L__BMASK            0x40 /* Timer 3 Low Byte Overflow Flag */
#define TF3L__SHIFT            0x06 /* Timer 3 Low Byte Overflow Flag */
#define TF3L__NOT_SET          0x00 /* Timer 3 low byte did not overflow. */
#define TF3L__SET              0x40 /* Timer 3 low byte overflowed. */

#define TF3H__BMASK            0x80 /* Timer 3 High Byte Overflow Flag */
#define TF3H__SHIFT            0x07 /* Timer 3 High Byte Overflow Flag */
#define TF3H__NOT_SET          0x00 /* Timer 3 8-bit high byte or 16-bit value did not overflow. */
#define TF3H__SET              0x80 /* Timer 3 8-bit high byte or 16-bit value overflowed. */

/*------------------------------------------------------------------------------ */
/* PCA0CPM0 Enums (PCA Channel 0 Capture/Compare Mode @ 0xDA) */
/*------------------------------------------------------------------------------ */
#define ECCF__BMASK    0x01 /* Channel 0 Capture/Compare Flag Interrupt Enable */
#define ECCF__SHIFT    0x00 /* Channel 0 Capture/Compare Flag Interrupt Enable */
#define ECCF__DISABLED 0x00 /* Disable CCF0 interrupts. */
#define ECCF__ENABLED  0x01 /* Enable a Capture/Compare Flag interrupt request when CCF0 is set. */

#define PWM__BMASK     0x02 /* Channel 0 Pulse Width Modulation Mode Enable */
#define PWM__SHIFT     0x01 /* Channel 0 Pulse Width Modulation Mode Enable */
#define PWM__DISABLED  0x00 /* Disable PWM function. */
#define PWM__ENABLED   0x02 /* Enable PWM function. */

#define TOG__BMASK     0x04 /* Channel 0 Toggle Function Enable */
#define TOG__SHIFT     0x02 /* Channel 0 Toggle Function Enable */
#define TOG__DISABLED  0x00 /* Disable toggle function. */
#define TOG__ENABLED   0x04 /* Enable toggle function. */

#define MAT__BMASK     0x08 /* Channel 0 Match Function Enable */
#define MAT__SHIFT     0x03 /* Channel 0 Match Function Enable */
#define MAT__DISABLED  0x00 /* Disable match function. */
#define MAT__ENABLED   0x08 /* Enable match function. */

#define CAPN__BMASK    0x10 /* Channel 0 Capture Negative Function Enable */
#define CAPN__SHIFT    0x04 /* Channel 0 Capture Negative Function Enable */
#define CAPN__DISABLED 0x00 /* Disable negative edge capture. */
#define CAPN__ENABLED  0x10 /* Enable negative edge capture. */

#define CAPP__BMASK    0x20 /* Channel 0 Capture Positive Function Enable */
#define CAPP__SHIFT    0x05 /* Channel 0 Capture Positive Function Enable */
#define CAPP__DISABLED 0x00 /* Disable positive edge capture. */
#define CAPP__ENABLED  0x20 /* Enable positive edge capture. */

#define ECOM__BMASK    0x40 /* Channel 0 Comparator Function Enable */
#define ECOM__SHIFT    0x06 /* Channel 0 Comparator Function Enable */
#define ECOM__DISABLED 0x00 /* Disable comparator function. */
#define ECOM__ENABLED  0x40 /* Enable comparator function. */

#define PWM16__BMASK   0x80 /* Channel 0 16-bit Pulse Width Modulation Enable */
#define PWM16__SHIFT   0x07 /* Channel 0 16-bit Pulse Width Modulation Enable */
#define PWM16__8_BIT   0x00 /* 8 to 11-bit PWM selected. */
#define PWM16__16_BIT  0x80 /* 16-bit PWM selected. */

/*------------------------------------------------------------------------------ */
/* PCA0CPM1 Enums (PCA Channel 1 Capture/Compare Mode @ 0xDB)                    */
/*------------------------------------------------------------------------------ */
#define ECCF__BMASK    0x01 /* Channel 1 Capture/Compare Flag Interrupt Enable */
#define ECCF__SHIFT    0x00 /* Channel 1 Capture/Compare Flag Interrupt Enable */
#define ECCF__DISABLED 0x00 /* Disable CCF1 interrupts. */
#define ECCF__ENABLED  0x01 /* Enable a Capture/Compare Flag interrupt request when CCF1 is set. */

#define PWM__BMASK     0x02 /* Channel 1 Pulse Width Modulation Mode Enable */
#define PWM__SHIFT     0x01 /* Channel 1 Pulse Width Modulation Mode Enable */
#define PWM__DISABLED  0x00 /* Disable PWM function. */
#define PWM__ENABLED   0x02 /* Enable PWM function. */

#define TOG__BMASK     0x04 /* Channel 1 Toggle Function Enable */
#define TOG__SHIFT     0x02 /* Channel 1 Toggle Function Enable */
#define TOG__DISABLED  0x00 /* Disable toggle function. */
#define TOG__ENABLED   0x04 /* Enable toggle function. */

#define MAT__BMASK     0x08 /* Channel 1 Match Function Enable */
#define MAT__SHIFT     0x03 /* Channel 1 Match Function Enable */
#define MAT__DISABLED  0x00 /* Disable match function. */
#define MAT__ENABLED   0x08 /* Enable match function. */

#define CAPN__BMASK    0x10 /* Channel 1 Capture Negative Function Enable */
#define CAPN__SHIFT    0x04 /* Channel 1 Capture Negative Function Enable */
#define CAPN__DISABLED 0x00 /* Disable negative edge capture. */
#define CAPN__ENABLED  0x10 /* Enable negative edge capture. */

#define CAPP__BMASK    0x20 /* Channel 1 Capture Positive Function Enable */
#define CAPP__SHIFT    0x05 /* Channel 1 Capture Positive Function Enable */
#define CAPP__DISABLED 0x00 /* Disable positive edge capture. */
#define CAPP__ENABLED  0x20 /* Enable positive edge capture. */

#define ECOM__BMASK    0x40 /* Channel 1 Comparator Function Enable */
#define ECOM__SHIFT    0x06 /* Channel 1 Comparator Function Enable */
#define ECOM__DISABLED 0x00 /* Disable comparator function. */
#define ECOM__ENABLED  0x40 /* Enable comparator function. */

#define PWM16__BMASK   0x80 /* Channel 1 16-bit Pulse Width Modulation Enable */
#define PWM16__SHIFT   0x07 /* Channel 1 16-bit Pulse Width Modulation Enable */
#define PWM16__8_BIT   0x00 /* 8 to 11-bit PWM selected. */
#define PWM16__16_BIT  0x80 /* 16-bit PWM selected. */

/*------------------------------------------------------------------------------ */
/* PCA0CPM2 Enums (PCA Channel 2 Capture/Compare Mode @ 0xDC)                    */
/*------------------------------------------------------------------------------ */
#define ECCF__BMASK    0x01 /* Channel 2 Capture/Compare Flag Interrupt Enable */
#define ECCF__SHIFT    0x00 /* Channel 2 Capture/Compare Flag Interrupt Enable */
#define ECCF__DISABLED 0x00 /* Disable CCF2 interrupts. */
#define ECCF__ENABLED  0x01 /* Enable a Capture/Compare Flag interrupt request */
/* when CCF2 is set. */

#define PWM__BMASK     0x02 /* Channel 2 Pulse Width Modulation Mode Enable */
#define PWM__SHIFT     0x01 /* Channel 2 Pulse Width Modulation Mode Enable */
#define PWM__DISABLED  0x00 /* Disable PWM function. */
#define PWM__ENABLED   0x02 /* Enable PWM function. */

#define TOG__BMASK     0x04 /* Channel 2 Toggle Function Enable */
#define TOG__SHIFT     0x02 /* Channel 2 Toggle Function Enable */
#define TOG__DISABLED  0x00 /* Disable toggle function. */
#define TOG__ENABLED   0x04 /* Enable toggle function. */

#define MAT__BMASK     0x08 /* Channel 2 Match Function Enable */
#define MAT__SHIFT     0x03 /* Channel 2 Match Function Enable */
#define MAT__DISABLED  0x00 /* Disable match function. */
#define MAT__ENABLED   0x08 /* Enable match function. */

#define CAPN__BMASK    0x10 /* Channel 2 Capture Negative Function Enable */
#define CAPN__SHIFT    0x04 /* Channel 2 Capture Negative Function Enable */
#define CAPN__DISABLED 0x00 /* Disable negative edge capture. */
#define CAPN__ENABLED  0x10 /* Enable negative edge capture. */

#define CAPP__BMASK    0x20 /* Channel 2 Capture Positive Function Enable */
#define CAPP__SHIFT    0x05 /* Channel 2 Capture Positive Function Enable */
#define CAPP__DISABLED 0x00 /* Disable positive edge capture. */
#define CAPP__ENABLED  0x20 /* Enable positive edge capture. */

#define ECOM__BMASK    0x40 /* Channel 2 Comparator Function Enable */
#define ECOM__SHIFT    0x06 /* Channel 2 Comparator Function Enable */
#define ECOM__DISABLED 0x00 /* Disable comparator function. */
#define ECOM__ENABLED  0x40 /* Enable comparator function. */

#define PWM16__BMASK   0x80 /* Channel 2 16-bit Pulse Width Modulation Enable */
#define PWM16__SHIFT   0x07 /* Channel 2 16-bit Pulse Width Modulation Enable */
#define PWM16__8_BIT   0x00 /* 8 to 11-bit PWM selected. */
#define PWM16__16_BIT  0x80 /* 16-bit PWM selected. */

/*------------------------------------------------------------------------------ */
/* PCA0CENT Enums (PCA Center Alignment Enable @ 0x9E)                           */
/*------------------------------------------------------------------------------ */
#define CEX0CEN__BMASK  0x01 /* CEX0 Center Alignment Enable */
#define CEX0CEN__SHIFT  0x00 /* CEX0 Center Alignment Enable */
#define CEX0CEN__EDGE   0x00 /* Edge-aligned. */
#define CEX0CEN__CENTER 0x01 /* Center-aligned. */

#define CEX1CEN__BMASK  0x02 /* CEX1 Center Alignment Enable */
#define CEX1CEN__SHIFT  0x01 /* CEX1 Center Alignment Enable */
#define CEX1CEN__EDGE   0x00 /* Edge-aligned. */
#define CEX1CEN__CENTER 0x02 /* Center-aligned. */

#define CEX2CEN__BMASK  0x04 /* CEX2 Center Alignment Enable */
#define CEX2CEN__SHIFT  0x02 /* CEX2 Center Alignment Enable */
#define CEX2CEN__EDGE   0x00 /* Edge-aligned. */
#define CEX2CEN__CENTER 0x04 /* Center-aligned. */

/*------------------------------------------------------------------------------ */
/* PCA0CLR Enums (PCA Comparator Clear Control @ 0x9C)                           */
/*------------------------------------------------------------------------------ */
#define CPCE0__BMASK    0x01 /* Comparator Clear Enable for CEX0 */
#define CPCE0__SHIFT    0x00 /* Comparator Clear Enable for CEX0 */
#define CPCE0__DISABLED 0x00 /* Disable the comparator clear function on PCA channel 0. */
#define CPCE0__ENABLED  0x01 /* Enable the comparator clear function on PCA channel 0. */

#define CPCE1__BMASK    0x02 /* Comparator Clear Enable for CEX1 */
#define CPCE1__SHIFT    0x01 /* Comparator Clear Enable for CEX1 */
#define CPCE1__DISABLED 0x00 /* Disable the comparator clear function on PCA channel 1. */
#define CPCE1__ENABLED  0x02 /* Enable the comparator clear function on PCA channel 1. */

#define CPCE2__BMASK    0x04 /* Comparator Clear Enable for CEX2 */
#define CPCE2__SHIFT    0x02 /* Comparator Clear Enable for CEX2 */
#define CPCE2__DISABLED 0x00 /* Disable the comparator clear function on PCA channel 2. */
#define CPCE2__ENABLED  0x04 /* Enable the comparator clear function on PCA channel 2. */

#define CPCPOL__BMASK   0x80 /* Comparator Clear Polarity */
#define CPCPOL__SHIFT   0x07 /* Comparator Clear Polarity */
#define CPCPOL__LOW     0x00 /* PCA channel(s) will be cleared when comparator result goes logic low. */
#define CPCPOL__HIGH    0x80 /* PCA channel(s) will be cleared when comparator result goes logic high. */

/*------------------------------------------------------------------------------ */
/* PCA0CN0 Enums (PCA Control @ 0xD8)                                            */
/*------------------------------------------------------------------------------ */
#define CCF0__BMASK   0x01 /* PCA Module 0 Capture/Compare Flag */
#define CCF0__SHIFT   0x00 /* PCA Module 0 Capture/Compare Flag */
#define CCF0__NOT_SET 0x00 /* A match or capture did not occur on channel 0. */
#define CCF0__SET     0x01 /* A match or capture occurred on channel 0. */

#define CCF1__BMASK   0x02 /* PCA Module 1 Capture/Compare Flag */
#define CCF1__SHIFT   0x01 /* PCA Module 1 Capture/Compare Flag */
#define CCF1__NOT_SET 0x00 /* A match or capture did not occur on channel 1. */
#define CCF1__SET     0x02 /* A match or capture occurred on channel 1. */

#define CCF2__BMASK   0x04 /* PCA Module 2 Capture/Compare Flag */
#define CCF2__SHIFT   0x02 /* PCA Module 2 Capture/Compare Flag */
#define CCF2__NOT_SET 0x00 /* A match or capture did not occur on channel 2. */
#define CCF2__SET     0x04 /* A match or capture occurred on channel 2. */

#define CR__BMASK     0x40 /* PCA Counter/Timer Run Control */
#define CR__SHIFT     0x06 /* PCA Counter/Timer Run Control */
#define CR__STOP      0x00 /* Stop the PCA Counter/Timer. */
#define CR__RUN       0x40 /* Start the PCA Counter/Timer running. */

#define CF__BMASK     0x80 /* PCA Counter/Timer Overflow Flag */
#define CF__SHIFT     0x07 /* PCA Counter/Timer Overflow Flag */
#define CF__NOT_SET   0x00 /* The PCA counter/timer did not overflow. */
#define CF__SET       0x80 /* The PCA counter/timer overflowed. */

/*------------------------------------------------------------------------------ */
/* PCA0MD Enums (PCA Mode @ 0xD9)                                                */
/*------------------------------------------------------------------------------ */
#define ECF__BMASK            0x01 /* PCA Counter/Timer Overflow Interrupt Enable */
#define ECF__SHIFT            0x00 /* PCA Counter/Timer Overflow Interrupt Enable */
#define ECF__OVF_INT_DISABLED 0x00 /* Disable the CF interrupt. */
#define ECF__OVF_INT_ENABLED  0x01 /* Enable a PCA Counter/Timer Overflow interrupt request when CF is set. */

#define CPS__FMASK            0x0E /* PCA Counter/Timer Pulse Select */
#define CPS__SHIFT            0x01 /* PCA Counter/Timer Pulse Select */
#define CPS__SYSCLK_DIV_12    0x00 /* System clock divided by 12. */
#define CPS__SYSCLK_DIV_4     0x02 /* System clock divided by 4. */
#define CPS__T0_OVERFLOW      0x04 /* Timer 0 overflow. */
#define CPS__ECI              0x06 /* High-to-low transitions on ECI (max rate = system clock divided by 4). */
#define CPS__SYSCLK           0x08 /* System clock. */
#define CPS__EXTOSC_DIV_8     0x0A /* External clock divided by 8 (synchronized with the system clock). */
#define CPS__LFOSC_DIV_8      0x0C /* Low frequency oscillator divided by 8. */

#define CIDL__BMASK           0x80 /* PCA Counter/Timer Idle Control */
#define CIDL__SHIFT           0x07 /* PCA Counter/Timer Idle Control */
#define CIDL__NORMAL          0x00 /* PCA continues to function normally while the system controller is in Idle Mode. */
#define CIDL__SUSPEND         0x80 /* PCA operation is suspended while the system controller is in Idle Mode. */

/*------------------------------------------------------------------------------ */
/* PCA0POL Enums (PCA Output Polarity @ 0x96)                                    */
/*------------------------------------------------------------------------------ */
#define CEX0POL__BMASK   0x01 /* CEX0 Output Polarity */
#define CEX0POL__SHIFT   0x00 /* CEX0 Output Polarity */
#define CEX0POL__DEFAULT 0x00 /* Use default polarity. */
#define CEX0POL__INVERT  0x01 /* Invert polarity. */

#define CEX1POL__BMASK   0x02 /* CEX1 Output Polarity */
#define CEX1POL__SHIFT   0x01 /* CEX1 Output Polarity */
#define CEX1POL__DEFAULT 0x00 /* Use default polarity. */
#define CEX1POL__INVERT  0x02 /* Invert polarity. */

#define CEX2POL__BMASK   0x04 /* CEX2 Output Polarity */
#define CEX2POL__SHIFT   0x02 /* CEX2 Output Polarity */
#define CEX2POL__DEFAULT 0x00 /* Use default polarity. */
#define CEX2POL__INVERT  0x04 /* Invert polarity. */

/*------------------------------------------------------------------------------ */
/* PCA0PWM Enums (PCA PWM Configuration @ 0xF7)                                  */
/*------------------------------------------------------------------------------ */
#define CLSEL__FMASK             0x07 /* Cycle Length Select */
#define CLSEL__SHIFT             0x00 /* Cycle Length Select */
#define CLSEL__8_BITS            0x00 /* 8 bits. */
#define CLSEL__9_BITS            0x01 /* 9 bits. */
#define CLSEL__10_BITS           0x02 /* 10 bits. */
#define CLSEL__11_BITS           0x03 /* 11 bits. */

#define COVF__BMASK              0x20 /* Cycle Overflow Flag */
#define COVF__SHIFT              0x05 /* Cycle Overflow Flag */
#define COVF__NO_OVERFLOW        0x00 /* No overflow has occurred since the last time this bit was cleared. */
#define COVF__OVERFLOW           0x20 /* An overflow has occurred since the last time this bit was cleared. */

#define ECOV__BMASK              0x40 /* Cycle Overflow Interrupt Enable */
#define ECOV__SHIFT              0x06 /* Cycle Overflow Interrupt Enable */
#define ECOV__COVF_MASK_DISABLED 0x00 /* COVF will not generate PCA interrupts. */
#define ECOV__COVF_MASK_ENABLED  0x40 /* A PCA interrupt will be generated when COVF is set. */

#define ARSEL__BMASK             0x80 /* Auto-Reload Register Select */
#define ARSEL__SHIFT             0x07 /* Auto-Reload Register Select */
#define ARSEL__CAPTURE_COMPARE   0x00 /* Read/Write Capture/Compare Registers at PCA0CPHn and PCA0CPLn. */
#define ARSEL__AUTORELOAD        0x80 /* Read/Write Auto-Reload Registers at PCA0CPHn and PCA0CPLn. */

/*------------------------------------------------------------------------------ */
/* SPI0CFG Enums (SPI0 Configuration @ 0xA1)                                     */
/*------------------------------------------------------------------------------ */
#define RXBMT__BMASK                0x01 /* Receive Buffer Empty */
#define RXBMT__SHIFT                0x00 /* Receive Buffer Empty */
#define RXBMT__NOT_SET              0x00 /* New data is available in the receive buffer (Slave mode). */
#define RXBMT__SET                  0x01 /* No new data in the receive buffer (Slave mode). */

#define SRMT__BMASK                 0x02 /* Shift Register Empty */
#define SRMT__SHIFT                 0x01 /* Shift Register Empty */
#define SRMT__NOT_SET               0x00 /* The shift register is not empty. */
#define SRMT__SET                   0x02 /* The shift register is empty. */

#define NSSIN__BMASK                0x04 /* NSS Instantaneous Pin Input */
#define NSSIN__SHIFT                0x02 /* NSS Instantaneous Pin Input */
#define NSSIN__LOW                  0x00 /* The NSS pin is low. */
#define NSSIN__HIGH                 0x04 /* The NSS pin is high. */

#define SLVSEL__BMASK               0x08 /* Slave Selected Flag */
#define SLVSEL__SHIFT               0x03 /* Slave Selected Flag */
#define SLVSEL__NOT_SELECTED        0x00 /* The Slave is not selected (NSS is high). */
#define SLVSEL__SELECTED            0x08 /* The Slave is selected (NSS is low). */

#define CKPOL__BMASK                0x10 /* SPI0 Clock Polarity */
#define CKPOL__SHIFT                0x04 /* SPI0 Clock Polarity */
#define CKPOL__IDLE_LOW             0x00 /* SCK line low in idle state. */
#define CKPOL__IDLE_HIGH            0x10 /* SCK line high in idle state. */

#define CKPHA__BMASK                0x20 /* SPI0 Clock Phase */
#define CKPHA__SHIFT                0x05 /* SPI0 Clock Phase */
#define CKPHA__DATA_CENTERED_FIRST  0x00 /* Data centered on first edge of SCK period. */
#define CKPHA__DATA_CENTERED_SECOND 0x20 /* Data centered on second edge of SCK period. */

#define MSTEN__BMASK                0x40 /* Master Mode Enable */
#define MSTEN__SHIFT                0x06 /* Master Mode Enable */
#define MSTEN__MASTER_DISABLED      0x00 /* Disable master mode. Operate in slave mode. */
#define MSTEN__MASTER_ENABLED       0x40 /* Enable master mode. Operate as a master. */

#define SPIBSY__BMASK               0x80 /* SPI Busy */
#define SPIBSY__SHIFT               0x07 /* SPI Busy */
#define SPIBSY__NOT_SET             0x00 /* A SPI transfer is not in progress. */
#define SPIBSY__SET                 0x80 /* A SPI transfer is in progress. */

/*------------------------------------------------------------------------------ */
/* SPI0CN0 Enums (SPI0 Control @ 0xF8)                                           */
/*------------------------------------------------------------------------------ */
#define SPIEN__BMASK                  0x01 /* SPI0 Enable */
#define SPIEN__SHIFT                  0x00 /* SPI0 Enable */
#define SPIEN__DISABLED               0x00 /* Disable the SPI module. */
#define SPIEN__ENABLED                0x01 /* Enable the SPI module. */

#define TXBMT__BMASK                  0x02 /* Transmit Buffer Empty */
#define TXBMT__SHIFT                  0x01 /* Transmit Buffer Empty */
#define TXBMT__NOT_SET                0x00 /* The transmit buffer is not empty. */
#define TXBMT__SET                    0x02 /* The transmit buffer is empty. */

#define NSSMD__FMASK                  0x0C /* Slave Select Mode */
#define NSSMD__SHIFT                  0x02 /* Slave Select Mode */
#define NSSMD__3_WIRE                 0x00 /* 3-Wire Slave or 3-Wire Master Mode. NSS signal is not routed to a port pin. */
#define NSSMD__4_WIRE_SLAVE           0x04 /* 4-Wire Slave or Multi-Master Mode. NSS is an input to the device. */
#define NSSMD__4_WIRE_MASTER_NSS_LOW  0x08 /* 4-Wire Single-Master Mode. NSS is an output and logic low. */
#define NSSMD__4_WIRE_MASTER_NSS_HIGH 0x0C /* 4-Wire Single-Master Mode. NSS is an output and logic high. */

#define RXOVRN__BMASK                 0x10 /* Receive Overrun Flag */
#define RXOVRN__SHIFT                 0x04 /* Receive Overrun Flag */
#define RXOVRN__NOT_SET               0x00 /* A receive overrun did not occur. */
#define RXOVRN__SET                   0x10 /* A receive overrun occurred. */

#define MODF__BMASK                   0x20 /* Mode Fault Flag */
#define MODF__SHIFT                   0x05 /* Mode Fault Flag */
#define MODF__NOT_SET                 0x00 /* A master collision did not occur. */
#define MODF__SET                     0x20 /* A master collision occurred. */

#define WCOL__BMASK                   0x40 /* Write Collision Flag */
#define WCOL__SHIFT                   0x06 /* Write Collision Flag */
#define WCOL__NOT_SET                 0x00 /* A write collision did not occur. */
#define WCOL__SET                     0x40 /* A write collision occurred. */

#define SPIF__BMASK                   0x80 /* SPI0 Interrupt Flag */
#define SPIF__SHIFT                   0x07 /* SPI0 Interrupt Flag */
#define SPIF__NOT_SET                 0x00 /* A data transfer has not completed since the last time SPIF was cleared. */
#define SPIF__SET                     0x80 /* A data transfer completed. */

/*------------------------------------------------------------------------------ */
/* EIE1 Enums (Extended Interrupt Enable 1 @ 0xE6)                               */
/*------------------------------------------------------------------------------ */
#define ESMB0__BMASK     0x01 /* SMBus (SMB0) Interrupt Enable */
#define ESMB0__SHIFT     0x00 /* SMBus (SMB0) Interrupt Enable */
#define ESMB0__DISABLED  0x00 /* Disable all SMB0 interrupts. */
#define ESMB0__ENABLED   0x01 /* Enable interrupt requests generated by SMB0. */

#define EMAT__BMASK      0x02 /* Port Match Interrupts Enable */
#define EMAT__SHIFT      0x01 /* Port Match Interrupts Enable */
#define EMAT__DISABLED   0x00 /* Disable all Port Match interrupts. */
#define EMAT__ENABLED    0x02 /* Enable interrupt requests generated by a Port Match. */

#define EWADC0__BMASK    0x04 /* ADC0 Window Comparison Interrupt Enable */
#define EWADC0__SHIFT    0x02 /* ADC0 Window Comparison Interrupt Enable */
#define EWADC0__DISABLED 0x00 /* Disable ADC0 Window Comparison interrupt. */
#define EWADC0__ENABLED  0x04 /* Enable interrupt requests generated by ADC0 Window Compare flag (ADWINT). */

#define EADC0__BMASK     0x08 /* ADC0 Conversion Complete Interrupt Enable */
#define EADC0__SHIFT     0x03 /* ADC0 Conversion Complete Interrupt Enable */
#define EADC0__DISABLED  0x00 /* Disable ADC0 Conversion Complete interrupt. */
#define EADC0__ENABLED   0x08 /* Enable interrupt requests generated by the ADINT flag. */

#define EPCA0__BMASK     0x10 /* Programmable Counter Array (PCA0) Interrupt Enable */
#define EPCA0__SHIFT     0x04 /* Programmable Counter Array (PCA0) Interrupt Enable */
#define EPCA0__DISABLED  0x00 /* Disable all PCA0 interrupts. */
#define EPCA0__ENABLED   0x10 /* Enable interrupt requests generated by PCA0. */

#define ECP0__BMASK      0x20 /* Comparator0 (CP0) Interrupt Enable */
#define ECP0__SHIFT      0x05 /* Comparator0 (CP0) Interrupt Enable */
#define ECP0__DISABLED   0x00 /* Disable CP0 interrupts. */
#define ECP0__ENABLED    0x20 /* Enable interrupt requests generated by the comparator 0 CPRIF or CPFIF flags. */

#define ECP1__BMASK      0x40 /* Comparator1 (CP1) Interrupt Enable */
#define ECP1__SHIFT      0x06 /* Comparator1 (CP1) Interrupt Enable */
#define ECP1__DISABLED   0x00 /* Disable CP1 interrupts. */
#define ECP1__ENABLED    0x40 /* Enable interrupt requests generated by the comparator 1 CPRIF or CPFIF flags. */

#define ET3__BMASK       0x80 /* Timer 3 Interrupt Enable */
#define ET3__SHIFT       0x07 /* Timer 3 Interrupt Enable */
#define ET3__DISABLED    0x00 /* Disable Timer 3 interrupts. */
#define ET3__ENABLED     0x80 /* Enable interrupt requests generated by the TF3L or TF3H flags. */

/*------------------------------------------------------------------------------ */
/* EIP1 Enums (Extended Interrupt Priority 1 @ 0xF3)                             */
/*------------------------------------------------------------------------------ */
#define PSMB0__BMASK  0x01 /* SMBus (SMB0) Interrupt Priority Control */
#define PSMB0__SHIFT  0x00 /* SMBus (SMB0) Interrupt Priority Control */
#define PSMB0__LOW    0x00 /* SMB0 interrupt set to low priority level. */
#define PSMB0__HIGH   0x01 /* SMB0 interrupt set to high priority level. */

#define PMAT__BMASK   0x02 /* Port Match Interrupt Priority Control */
#define PMAT__SHIFT   0x01 /* Port Match Interrupt Priority Control */
#define PMAT__LOW     0x00 /* Port Match interrupt set to low priority level. */
#define PMAT__HIGH    0x02 /* Port Match interrupt set to high priority level. */

#define PWADC0__BMASK 0x04 /* ADC0 Window Comparator Interrupt Priority Control */
#define PWADC0__SHIFT 0x02 /* ADC0 Window Comparator Interrupt Priority Control */
#define PWADC0__LOW   0x00 /* ADC0 Window interrupt set to low priority level. */
#define PWADC0__HIGH  0x04 /* ADC0 Window interrupt set to high priority level. */

#define PADC0__BMASK  0x08 /* ADC0 Conversion Complete Interrupt Priority Control */
#define PADC0__SHIFT  0x03 /* ADC0 Conversion Complete Interrupt Priority Control */
#define PADC0__LOW    0x00 /* ADC0 Conversion Complete interrupt set to low priority level. */
#define PADC0__HIGH   0x08 /* ADC0 Conversion Complete interrupt set to high priority level. */

#define PPCA0__BMASK  0x10 /* Programmable Counter Array (PCA0) Interrupt Priority Control */
#define PPCA0__SHIFT  0x04 /* Programmable Counter Array (PCA0) Interrupt Priority Control */
#define PPCA0__LOW    0x00 /* PCA0 interrupt set to low priority level. */
#define PPCA0__HIGH   0x10 /* PCA0 interrupt set to high priority level. */

#define PCP0__BMASK   0x20 /* Comparator0 (CP0) Interrupt Priority Control */
#define PCP0__SHIFT   0x05 /* Comparator0 (CP0) Interrupt Priority Control */
#define PCP0__LOW     0x00 /* CP0 interrupt set to low priority level. */
#define PCP0__HIGH    0x20 /* CP0 interrupt set to high priority level. */

#define PCP1__BMASK   0x40 /* Comparator1 (CP1) Interrupt Priority Control */
#define PCP1__SHIFT   0x06 /* Comparator1 (CP1) Interrupt Priority Control */
#define PCP1__LOW     0x00 /* CP1 interrupt set to low priority level. */
#define PCP1__HIGH    0x40 /* CP1 interrupt set to high priority level. */

#define PT3__BMASK    0x80 /* Timer 3 Interrupt Priority Control */
#define PT3__SHIFT    0x07 /* Timer 3 Interrupt Priority Control */
#define PT3__LOW      0x00 /* Timer 3 interrupts set to low priority level. */
#define PT3__HIGH     0x80 /* Timer 3 interrupts set to high priority level. */

/*------------------------------------------------------------------------------ */
/* IE Enums (Interrupt Enable @ 0xA8)                                            */
/*------------------------------------------------------------------------------ */
#define EX0__BMASK      0x01 /* External Interrupt 0 Enable */
#define EX0__SHIFT      0x00 /* External Interrupt 0 Enable */
#define EX0__DISABLED   0x00 /* Disable external interrupt 0. */
#define EX0__ENABLED    0x01 /* Enable interrupt requests generated by the INT0 input. */

#define ET0__BMASK      0x02 /* Timer 0 Interrupt Enable */
#define ET0__SHIFT      0x01 /* Timer 0 Interrupt Enable */
#define ET0__DISABLED   0x00 /* Disable all Timer 0 interrupt. */
#define ET0__ENABLED    0x02 /* Enable interrupt requests generated by the TF0 flag. */

#define EX1__BMASK      0x04 /* External Interrupt 1 Enable */
#define EX1__SHIFT      0x02 /* External Interrupt 1 Enable */
#define EX1__DISABLED   0x00 /* Disable external interrupt 1. */
#define EX1__ENABLED    0x04 /* Enable interrupt requests generated by the INT1 input. */

#define ET1__BMASK      0x08 /* Timer 1 Interrupt Enable */
#define ET1__SHIFT      0x03 /* Timer 1 Interrupt Enable */
#define ET1__DISABLED   0x00 /* Disable all Timer 1 interrupt. */
#define ET1__ENABLED    0x08 /* Enable interrupt requests generated by the TF1 flag. */

#define ES0__BMASK      0x10 /* UART0 Interrupt Enable */
#define ES0__SHIFT      0x04 /* UART0 Interrupt Enable */
#define ES0__DISABLED   0x00 /* Disable UART0 interrupt. */
#define ES0__ENABLED    0x10 /* Enable UART0 interrupt. */

#define ET2__BMASK      0x20 /* Timer 2 Interrupt Enable */
#define ET2__SHIFT      0x05 /* Timer 2 Interrupt Enable */
#define ET2__DISABLED   0x00 /* Disable Timer 2 interrupt. */
#define ET2__ENABLED    0x20 /* Enable interrupt requests generated by the TF2L or TF2H flags. */

#define ESPI0__BMASK    0x40 /* SPI0 Interrupt Enable */
#define ESPI0__SHIFT    0x06 /* SPI0 Interrupt Enable */
#define ESPI0__DISABLED 0x00 /* Disable all SPI0 interrupts. */
#define ESPI0__ENABLED  0x40 /* Enable interrupt requests generated by SPI0. */

#define EA__BMASK       0x80 /* All Interrupts Enable */
#define EA__SHIFT       0x07 /* All Interrupts Enable */
#define EA__DISABLED    0x00 /* Disable all interrupt sources. */
#define EA__ENABLED     0x80 /* Enable each interrupt according to its individual mask setting. */

/*------------------------------------------------------------------------------ */
/* SMB0ADM Enums (SMBus 0 Slave Address Mask @ 0xD6)                             */
/*------------------------------------------------------------------------------ */
#define EHACK__BMASK             0x01 /* Hardware Acknowledge Enable */
#define EHACK__SHIFT             0x00 /* Hardware Acknowledge Enable */
#define EHACK__ADR_ACK_MANUAL    0x00 /* Firmware must manually acknowledge all incoming address and data bytes. */
#define EHACK__ADR_ACK_AUTOMATIC 0x01 /* Automatic slave address recognition and hardware acknowledge is enabled. */

#define SLVM__FMASK              0xFE /* SMBus Slave Address Mask */
#define SLVM__SHIFT              0x01 /* SMBus Slave Address Mask */

/*------------------------------------------------------------------------------ */
/* SMB0ADR Enums (SMBus 0 Slave Address @ 0xD7)                                  */
/*------------------------------------------------------------------------------ */
#define GC__BMASK      0x01 /* General Call Address Enable */
#define GC__SHIFT      0x00 /* General Call Address Enable */
#define GC__IGNORED    0x00 /* General Call Address is ignored. */
#define GC__RECOGNIZED 0x01 /* General Call Address is recognized. */

#define SLV__FMASK     0xFE /* SMBus Hardware Slave Address */
#define SLV__SHIFT     0x01 /* SMBus Hardware Slave Address */

/*------------------------------------------------------------------------------ */
/* SMB0CF Enums (SMBus 0 Configuration @ 0xC1)                                   */
/*------------------------------------------------------------------------------ */
#define SMBCS__FMASK             0x03 /* SMBus Clock Source Selection */
#define SMBCS__SHIFT             0x00 /* SMBus Clock Source Selection */
#define SMBCS__TIMER0            0x00 /* Timer 0 Overflow. */
#define SMBCS__TIMER1            0x01 /* Timer 1 Overflow. */
#define SMBCS__TIMER2_HIGH       0x02 /* Timer 2 High Byte Overflow. */
#define SMBCS__TIMER2_LOW        0x03 /* Timer 2 Low Byte Overflow. */

#define SMBFTE__BMASK            0x04 /* SMBus Free Timeout Detection Enable */
#define SMBFTE__SHIFT            0x02 /* SMBus Free Timeout Detection Enable */
#define SMBFTE__FREE_TO_DISABLED 0x00 /* Disable bus free timeouts. */
#define SMBFTE__FREE_TO_ENABLED  0x04 /* Enable bus free timeouts. The bus the bus will be considered free if SCL and SDA remain high for more than 10 SMBus clock source periods. */

#define SMBTOE__BMASK            0x08 /* SMBus SCL Timeout Detection Enable */
#define SMBTOE__SHIFT            0x03 /* SMBus SCL Timeout Detection Enable */
#define SMBTOE__SCL_TO_DISABLED  0x00 /* Disable SCL low timeouts. */
#define SMBTOE__SCL_TO_ENABLED   0x08 /* Enable SCL low timeouts. */

#define EXTHOLD__BMASK           0x10 /* SMBus Setup and Hold Time Extension Enable */
#define EXTHOLD__SHIFT           0x04 /* SMBus Setup and Hold Time Extension Enable */
#define EXTHOLD__DISABLED        0x00 /* Disable SDA extended setup and hold times. */
#define EXTHOLD__ENABLED         0x10 /* Enable SDA extended setup and hold times. */

#define BUSY__BMASK              0x20 /* SMBus Busy Indicator */
#define BUSY__SHIFT              0x05 /* SMBus Busy Indicator */
#define BUSY__NOT_SET            0x00 /* The bus is not busy. */
#define BUSY__SET                0x20 /* The bus is busy and a transfer is currently in progress. */

#define INH__BMASK               0x40 /* SMBus Slave Inhibit */
#define INH__SHIFT               0x06 /* SMBus Slave Inhibit */
#define INH__SLAVE_ENABLED       0x00 /* Slave states are enabled. */
#define INH__SLAVE_DISABLED      0x40 /* Slave states are inhibited. */

#define ENSMB__BMASK             0x80 /* SMBus Enable */
#define ENSMB__SHIFT             0x07 /* SMBus Enable */
#define ENSMB__DISABLED          0x00 /* Disable the SMBus module. */
#define ENSMB__ENABLED           0x80 /* Enable the SMBus module. */

/*------------------------------------------------------------------------------ */
/* SMB0CN0 Enums (SMBus 0 Control @ 0xC0)                                        */
/*------------------------------------------------------------------------------ */
#define SI__BMASK           0x01 /* SMBus Interrupt Flag */
#define SI__SHIFT           0x00 /* SMBus Interrupt Flag */
#define SI__NOT_SET         0x00 /* */
#define SI__SET             0x01 /* */

#define ACK__BMASK          0x02 /* SMBus Acknowledge */
#define ACK__SHIFT          0x01 /* SMBus Acknowledge */
#define ACK__NOT_SET        0x00 /* Generate a NACK, or the response was a NACK. */
#define ACK__SET            0x02 /* Generate an ACK, or the response was an ACK. */

#define ARBLOST__BMASK      0x04 /* SMBus Arbitration Lost Indicator */
#define ARBLOST__SHIFT      0x02 /* SMBus Arbitration Lost Indicator */
#define ARBLOST__NOT_SET    0x00 /* No arbitration error. */
#define ARBLOST__ERROR      0x04 /* Arbitration error occurred. */

#define ACKRQ__BMASK        0x08 /* SMBus Acknowledge Request */
#define ACKRQ__SHIFT        0x03 /* SMBus Acknowledge Request */
#define ACKRQ__NOT_SET      0x00 /* No ACK requested. */
#define ACKRQ__REQUESTED    0x08 /* ACK requested. */

#define STO__BMASK          0x10 /* SMBus Stop Flag */
#define STO__SHIFT          0x04 /* SMBus Stop Flag */
#define STO__NOT_SET        0x00 /* A STOP is not pending. */
#define STO__SET            0x10 /* Generate a STOP or a STOP is currently pending. */

#define STA__BMASK          0x20 /* SMBus Start Flag */
#define STA__SHIFT          0x05 /* SMBus Start Flag */
#define STA__NOT_SET        0x00 /* A START was not detected. */
#define STA__SET            0x20 /* Generate a START, repeated START, or a START is currently pending. */

#define TXMODE__BMASK       0x40 /* SMBus Transmit Mode Indicator */
#define TXMODE__SHIFT       0x06 /* SMBus Transmit Mode Indicator */
#define TXMODE__RECEIVER    0x00 /* SMBus in Receiver Mode. */
#define TXMODE__TRANSMITTER 0x40 /* SMBus in Transmitter Mode. */

#define MASTER__BMASK       0x80 /* SMBus Master/Slave Indicator */
#define MASTER__SHIFT       0x07 /* SMBus Master/Slave Indicator */
#define MASTER__SLAVE       0x00 /* SMBus operating in slave mode. */
#define MASTER__MASTER      0x80 /* SMBus operating in master mode. */

/*------------------------------------------------------------------------------ */
/* SMB0TC Enums (SMBus 0 Timing and Pin Control @ 0xAC)                          */
/*------------------------------------------------------------------------------ */
#define SDD__FMASK         0x03 /* SMBus Start Detection Window */
#define SDD__SHIFT         0x00 /* SMBus Start Detection Window */
#define SDD__NONE          0x00 /* No additional hold time window (0-1 SYSCLK). */
#define SDD__ADD_2_SYSCLKS 0x01 /* Increase hold time window to 2-3 SYSCLKs. */
#define SDD__ADD_4_SYSCLKS 0x02 /* Increase hold time window to 4-5 SYSCLKs. */
#define SDD__ADD_8_SYSCLKS 0x03 /* Increase hold time window to 8-9 SYSCLKs. */

#define SWAP__BMASK        0x80 /* SMBus Swap Pins */
#define SWAP__SHIFT        0x07 /* SMBus Swap Pins */
#define SWAP__SDA_LOW_PIN  0x00 /* SDA is mapped to the lower-numbered port pin, and SCL is mapped to the higher-numbered port pin. */
#define SWAP__SDA_HIGH_PIN 0x80 /* SCL is mapped to the lower-numbered port pin, and SDA is mapped to the higher-numbered port pin. */

/*------------------------------------------------------------------------------ */
/* SCON0 Enums (UART0 Serial Port Control @ 0x98)                                */
/*------------------------------------------------------------------------------ */
#define RI__BMASK             0x01 /* Receive Interrupt Flag */
#define RI__SHIFT             0x00 /* Receive Interrupt Flag */
#define RI__NOT_SET           0x00 /* A byte of data has not been received by UART0. */
#define RI__SET               0x01 /* UART0 received a byte of data. */

#define TI__BMASK             0x02 /* Transmit Interrupt Flag */
#define TI__SHIFT             0x01 /* Transmit Interrupt Flag */
#define TI__NOT_SET           0x00 /* A byte of data has not been transmitted by UART0. */
#define TI__SET               0x02 /* UART0 transmitted a byte of data. */

#define RB8__BMASK            0x04 /* Ninth Receive Bit */
#define RB8__SHIFT            0x02 /* Ninth Receive Bit */
#define RB8__CLEARED_TO_0     0x00 /* In Mode 0, the STOP bit was 0. In Mode 1, the 9th bit was 0. */
#define RB8__SET_TO_1         0x04 /* In Mode 0, the STOP bit was 1. In Mode 1, the 9th bit was 1. */

#define TB8__BMASK            0x08 /* Ninth Transmission Bit */
#define TB8__SHIFT            0x03 /* Ninth Transmission Bit */
#define TB8__CLEARED_TO_0     0x00 /* In Mode 1, set the 9th transmission bit to 0. */
#define TB8__SET_TO_1         0x08 /* In Mode 1, set the 9th transmission bit to 1. */

#define REN__BMASK            0x10 /* Receive Enable */
#define REN__SHIFT            0x04 /* Receive Enable */
#define REN__RECEIVE_DISABLED 0x00 /* UART0 reception disabled. */
#define REN__RECEIVE_ENABLED  0x10 /* UART0 reception enabled. */

#define MCE__BMASK            0x20 /* Multiprocessor Communication Enable */
#define MCE__SHIFT            0x05 /* Multiprocessor Communication Enable */
#define MCE__MULTI_DISABLED   0x00 /* Ignore level of 9th bit / Stop bit. */
#define MCE__MULTI_ENABLED    0x20 /* RI is set and an interrupt is generated only when the stop bit is logic 1 (Mode 0) or when the 9th bit is logic 1 (Mode 1). */

#define SMODE__BMASK          0x80 /* Serial Port 0 Operation Mode */
#define SMODE__SHIFT          0x07 /* Serial Port 0 Operation Mode */
#define SMODE__8_BIT          0x00 /* 8-bit UART with Variable Baud Rate (Mode 0). */
#define SMODE__9_BIT          0x80 /* 9-bit UART with Variable Baud Rate (Mode 1). */

/*------------------------------------------------------------------------------ */
/* CMP0CN0 Enums (Comparator 0 Control 0 @ 0x9B)                                 */
/*------------------------------------------------------------------------------ */
#define CPHYN__FMASK                0x03 /* Comparator Negative Hysteresis Control */
#define CPHYN__SHIFT                0x00 /* Comparator Negative Hysteresis Control */
#define CPHYN__DISABLED             0x00 /* Negative Hysteresis disabled. */
#define CPHYN__ENABLED_MODE1        0x01 /* Negative Hysteresis = Hysteresis 1. */
#define CPHYN__ENABLED_MODE2        0x02 /* Negative Hysteresis = Hysteresis 2. */
#define CPHYN__ENABLED_MODE3        0x03 /* Negative Hysteresis = Hysteresis 3 (Maximum). */

#define CPHYP__FMASK                0x0C /* Comparator Positive Hysteresis Control */
#define CPHYP__SHIFT                0x02 /* Comparator Positive Hysteresis Control */
#define CPHYP__DISABLED             0x00 /* Positive Hysteresis disabled. */
#define CPHYP__ENABLED_MODE1        0x04 /* Positive Hysteresis = Hysteresis 1. */
#define CPHYP__ENABLED_MODE2        0x08 /* Positive Hysteresis = Hysteresis 2. */
#define CPHYP__ENABLED_MODE3        0x0C /* Positive Hysteresis = Hysteresis 3 (Maximum). */

#define CPFIF__BMASK                0x10 /* Comparator Falling-Edge Flag */
#define CPFIF__SHIFT                0x04 /* Comparator Falling-Edge Flag */
#define CPFIF__NOT_SET              0x00 /* No comparator falling edge has occurred since this flag was last cleared. */
#define CPFIF__FALLING_EDGE         0x10 /* Comparator falling edge has occurred. */

#define CPRIF__BMASK                0x20 /* Comparator Rising-Edge Flag */
#define CPRIF__SHIFT                0x05 /* Comparator Rising-Edge Flag */
#define CPRIF__NOT_SET              0x00 /* No comparator rising edge has occurred since this flag was last cleared. */
#define CPRIF__RISING_EDGE          0x20 /* Comparator rising edge has occurred. */

#define CPOUT__BMASK                0x40 /* Comparator Output State Flag */
#define CPOUT__SHIFT                0x06 /* Comparator Output State Flag */
#define CPOUT__POS_LESS_THAN_NEG    0x00 /* Voltage on CP0P < CP0N. */
#define CPOUT__POS_GREATER_THAN_NEG 0x40 /* Voltage on CP0P > CP0N. */

#define CPEN__BMASK                 0x80 /* Comparator Enable */
#define CPEN__SHIFT                 0x07 /* Comparator Enable */
#define CPEN__DISABLED              0x00 /* Comparator disabled. */
#define CPEN__ENABLED               0x80 /* Comparator enabled. */

/*------------------------------------------------------------------------------ */
/* CMP0MD Enums (Comparator 0 Mode @ 0x9D)                                       */
/*------------------------------------------------------------------------------ */
#define CPMD__FMASK              0x03 /* Comparator Mode Select */
#define CPMD__SHIFT              0x00 /* Comparator Mode Select */
#define CPMD__MODE0              0x00 /* Mode 0 (Fastest Response Time, Highest Power */
/* Consumption) */
#define CPMD__MODE1              0x01 /* Mode 1 */
#define CPMD__MODE2              0x02 /* Mode 2 */
#define CPMD__MODE3              0x03 /* Mode 3 (Slowest Response Time, Lowest Power */
/* Consumption) */

#define CPFIE__BMASK             0x10 /* Comparator Falling-Edge Interrupt Enable */
#define CPFIE__SHIFT             0x04 /* Comparator Falling-Edge Interrupt Enable */
#define CPFIE__FALL_INT_DISABLED 0x00 /* Comparator falling-edge interrupt disabled. */
#define CPFIE__FALL_INT_ENABLED  0x10 /* Comparator falling-edge interrupt enabled. */

#define CPRIE__BMASK             0x20 /* Comparator Rising-Edge Interrupt Enable */
#define CPRIE__SHIFT             0x05 /* Comparator Rising-Edge Interrupt Enable */
#define CPRIE__RISE_INT_DISABLED 0x00 /* Comparator rising-edge interrupt disabled. */
#define CPRIE__RISE_INT_ENABLED  0x20 /* Comparator rising-edge interrupt enabled. */

#define CPLOUT__BMASK            0x80 /* Comparator Latched Output Flag */
#define CPLOUT__SHIFT            0x07 /* Comparator Latched Output Flag */
#define CPLOUT__LOW              0x00 /* Comparator output was logic low at last PCA overflow. */
#define CPLOUT__HIGH             0x80 /* Comparator output was logic high at last PCA overflow. */
#define CPMD__FMASK              0x03 /* Comparator Mode Select */
#define CPMD__SHIFT              0x00 /* Comparator Mode Select */
#define CPMD__MODE0              0x00 /* Mode 0 (Fastest Response Time, Highest Power Consumption) */
#define CPMD__MODE1              0x01 /* Mode 1 */
#define CPMD__MODE2              0x02 /* Mode 2 */
#define CPMD__MODE3              0x03 /* Mode 3 (Slowest Response Time, Lowest Power Consumption) */

#define CPFIE__BMASK             0x10 /* Comparator Falling-Edge Interrupt Enable */
#define CPFIE__SHIFT             0x04 /* Comparator Falling-Edge Interrupt Enable */
#define CPFIE__FALL_INT_DISABLED 0x00 /* Comparator falling-edge interrupt disabled. */
#define CPFIE__FALL_INT_ENABLED  0x10 /* Comparator falling-edge interrupt enabled. */

#define CPRIE__BMASK             0x20 /* Comparator Rising-Edge Interrupt Enable */
#define CPRIE__SHIFT             0x05 /* Comparator Rising-Edge Interrupt Enable */
#define CPRIE__RISE_INT_DISABLED 0x00 /* Comparator rising-edge interrupt disabled. */
#define CPRIE__RISE_INT_ENABLED  0x20 /* Comparator rising-edge interrupt enabled. */

#define CPLOUT__BMASK            0x80 /* Comparator Latched Output Flag */
#define CPLOUT__SHIFT            0x07 /* Comparator Latched Output Flag */
#define CPLOUT__LOW              0x00 /* Comparator output was logic low at last PCA overflow. */
#define CPLOUT__HIGH             0x80 /* Comparator output was logic high at last PCA overflow. */

/*------------------------------------------------------------------------------ */
/* CMP0MX Enums (Comparator 0 Multiplexer Selection @ 0x9F)                      */
/*------------------------------------------------------------------------------ */
#define CMXP__FMASK   0x0F /* Comparator Positive Input MUX Selection */
#define CMXP__SHIFT   0x00 /* Comparator Positive Input MUX Selection */
#define CMXP__CMP0P0  0x00 /* External pin CMP0P.0. */
#define CMXP__CMP0P1  0x01 /* External pin CMP0P.1. */
#define CMXP__CMP0P2  0x02 /* External pin CMP0P.2. */
#define CMXP__CMP0P3  0x03 /* External pin CMP0P.3. */
#define CMXP__CMP0P4  0x04 /* External pin CMP0P.4. */
#define CMXP__CMP0P5  0x05 /* External pin CMP0P.5. */
#define CMXP__CMP0P6  0x06 /* External pin CMP0P.6. */
#define CMXP__CMP0P7  0x07 /* External pin CMP0P.7. */
#define CMXP__LDO_OUT 0x08 /* External pin CMP0P.8. */
#define CMXP__NONE    0x0F /* No input selected. */

#define CMXN__FMASK   0xF0 /* Comparator Negative Input MUX Selection */
#define CMXN__SHIFT   0x04 /* Comparator Negative Input MUX Selection */
#define CMXN__CMP0N0  0x00 /* External pin CMP0N.0. */
#define CMXN__CMP0N1  0x10 /* External pin CMP0N.1. */
#define CMXN__CMP0N2  0x20 /* External pin CMP0N.2. */
#define CMXN__CMP0N3  0x30 /* External pin CMP0N.3. */
#define CMXN__CMP0N4  0x40 /* External pin CMP0N.4. */
#define CMXN__CMP0N5  0x50 /* External pin CMP0N.5. */
#define CMXN__CMP0N6  0x60 /* External pin CMP0N.6. */
#define CMXN__CMP0N7  0x70 /* External pin CMP0N.7. */
#define CMXN__GND     0x80 /* External pin CMP0N.8. */
#define CMXN__NONE    0xF0 /* No input selected. */

/*------------------------------------------------------------------------------ */
/* CMP1CN0 Enums (Comparator 1 Control 0 @ 0xBF)                                 */
/*------------------------------------------------------------------------------ */
#define CPHYN__FMASK                0x03 /* Comparator Negative Hysteresis Control */
#define CPHYN__SHIFT                0x00 /* Comparator Negative Hysteresis Control */
#define CPHYN__DISABLED             0x00 /* Negative Hysteresis disabled. */
#define CPHYN__ENABLED_MODE1        0x01 /* Negative Hysteresis = Hysteresis 1. */
#define CPHYN__ENABLED_MODE2        0x02 /* Negative Hysteresis = Hysteresis 2. */
#define CPHYN__ENABLED_MODE3        0x03 /* Negative Hysteresis = Hysteresis 3 (Maximum). */

#define CPHYP__FMASK                0x0C /* Comparator Positive Hysteresis Control */
#define CPHYP__SHIFT                0x02 /* Comparator Positive Hysteresis Control */
#define CPHYP__DISABLED             0x00 /* Positive Hysteresis disabled. */
#define CPHYP__ENABLED_MODE1        0x04 /* Positive Hysteresis = Hysteresis 1. */
#define CPHYP__ENABLED_MODE2        0x08 /* Positive Hysteresis = Hysteresis 2. */
#define CPHYP__ENABLED_MODE3        0x0C /* Positive Hysteresis = Hysteresis 3 (Maximum). */

#define CPFIF__BMASK                0x10 /* Comparator Falling-Edge Flag */
#define CPFIF__SHIFT                0x04 /* Comparator Falling-Edge Flag */
#define CPFIF__NOT_SET              0x00 /* No comparator falling edge has occurred since this flag was last cleared. */
#define CPFIF__FALLING_EDGE         0x10 /* Comparator falling edge has occurred. */

#define CPRIF__BMASK                0x20 /* Comparator Rising-Edge Flag */
#define CPRIF__SHIFT                0x05 /* Comparator Rising-Edge Flag */
#define CPRIF__NOT_SET              0x00 /* No comparator rising edge has occurred since this flag was last cleared. */
#define CPRIF__RISING_EDGE          0x20 /* Comparator rising edge has occurred. */

#define CPOUT__BMASK                0x40 /* Comparator Output State Flag */
#define CPOUT__SHIFT                0x06 /* Comparator Output State Flag */
#define CPOUT__POS_LESS_THAN_NEG    0x00 /* Voltage on CP1P < CP1N. */
#define CPOUT__POS_GREATER_THAN_NEG 0x40 /* Voltage on CP1P > CP1N. */

#define CPEN__BMASK                 0x80 /* Comparator Enable */
#define CPEN__SHIFT                 0x07 /* Comparator Enable */
#define CPEN__DISABLED              0x00 /* Comparator disabled. */
#define CPEN__ENABLED               0x80 /* Comparator enabled. */

/*------------------------------------------------------------------------------ */
/* CMP1MD Enums (Comparator 1 Mode @ 0xAB)                                       */
/*------------------------------------------------------------------------------ */
#define CPMD__FMASK              0x03 /* Comparator Mode Select */
#define CPMD__SHIFT              0x00 /* Comparator Mode Select */
#define CPMD__MODE0              0x00 /* Mode 0 (Fastest Response Time, Highest Power Consumption) */
#define CPMD__MODE1              0x01 /* Mode 1 */
#define CPMD__MODE2              0x02 /* Mode 2 */
#define CPMD__MODE3              0x03 /* Mode 3 (Slowest Response Time, Lowest Power Consumption) */

#define CPFIE__BMASK             0x10 /* Comparator Falling-Edge Interrupt Enable */
#define CPFIE__SHIFT             0x04 /* Comparator Falling-Edge Interrupt Enable */
#define CPFIE__FALL_INT_DISABLED 0x00 /* Comparator falling-edge interrupt disabled. */
#define CPFIE__FALL_INT_ENABLED  0x10 /* Comparator falling-edge interrupt enabled. */

#define CPRIE__BMASK             0x20 /* Comparator Rising-Edge Interrupt Enable */
#define CPRIE__SHIFT             0x05 /* Comparator Rising-Edge Interrupt Enable */
#define CPRIE__RISE_INT_DISABLED 0x00 /* Comparator rising-edge interrupt disabled. */
#define CPRIE__RISE_INT_ENABLED  0x20 /* Comparator rising-edge interrupt enabled. */

#define CPLOUT__BMASK            0x80 /* Comparator Latched Output Flag */
#define CPLOUT__SHIFT            0x07 /* Comparator Latched Output Flag */
#define CPLOUT__LOW              0x00 /* Comparator output was logic low at last PCA overflow. */
#define CPLOUT__HIGH             0x80 /* Comparator output was logic high at last PCA overflow. */

/*------------------------------------------------------------------------------ */
/* CMP1MX Enums (Comparator 1 Multiplexer Selection @ 0xAA)                      */
/*------------------------------------------------------------------------------ */
#define CMXP__FMASK   0x0F /* Comparator Positive Input MUX Selection */
#define CMXP__SHIFT   0x00 /* Comparator Positive Input MUX Selection */
#define CMXP__CMP1P0  0x00 /* External pin CMP1P.0. */
#define CMXP__CMP1P1  0x01 /* External pin CMP1P.1. */
#define CMXP__CMP1P2  0x02 /* External pin CMP1P.2. */
#define CMXP__CMP1P3  0x03 /* External pin CMP1P.3. */
#define CMXP__CMP1P4  0x04 /* External pin CMP1P.4. */
#define CMXP__CMP1P5  0x05 /* External pin CMP1P.5. */
#define CMXP__CMP1P6  0x06 /* External pin CMP1P.6. */
#define CMXP__CMP1P7  0x07 /* External pin CMP1P.7. */
#define CMXP__LDO_OUT 0x08 /* External pin CMP1P.8. */
#define CMXP__NONE    0x0F /* No input selected. */

#define CMXN__FMASK   0xF0 /* Comparator Negative Input MUX Selection */
#define CMXN__SHIFT   0x04 /* Comparator Negative Input MUX Selection */
#define CMXN__CMP1N0  0x00 /* External pin CMP1N.0. */
#define CMXN__CMP1N1  0x10 /* External pin CMP1N.1. */
#define CMXN__CMP1N2  0x20 /* External pin CMP1N.2. */
#define CMXN__CMP1N3  0x30 /* External pin CMP1N.3. */
#define CMXN__CMP1N4  0x40 /* External pin CMP1N.4. */
#define CMXN__CMP1N5  0x50 /* External pin CMP1N.5. */
#define CMXN__CMP1N6  0x60 /* External pin CMP1N.6. */
#define CMXN__CMP1N7  0x70 /* External pin CMP1N.7. */
#define CMXN__GND     0x80 /* External pin CMP1N.8. */
#define CMXN__NONE    0xF0 /* No input selected. */

/*------------------------------------------------------------------------------ */
/* CRC0AUTO Enums (CRC0 Automatic Control @ 0xD2)                                */
/*------------------------------------------------------------------------------ */
#define CRCST__FMASK     0x3F /* Automatic CRC Calculation Starting Block */
#define CRCST__SHIFT     0x00 /* Automatic CRC Calculation Starting Block */

#define AUTOEN__BMASK    0x80 /* Automatic CRC Calculation Enable */
#define AUTOEN__SHIFT    0x07 /* Automatic CRC Calculation Enable */
#define AUTOEN__DISABLED 0x00 /* Disable automatic CRC operations on flash. */
#define AUTOEN__ENABLED  0x80 /* Enable automatic CRC operations on flash. */

/*------------------------------------------------------------------------------ */
/* CRC0CN0 Enums (CRC0 Control 0 @ 0xCE)                                         */
/*------------------------------------------------------------------------------ */
#define CRCPNT__BMASK        0x01 /* CRC Result Pointer */
#define CRCPNT__SHIFT        0x00 /* CRC Result Pointer */
#define CRCPNT__ACCESS_LOWER 0x00 /* CRC0DAT accesses bits 7-0 of the 16-bit CRC result. */
#define CRCPNT__ACCESS_UPPER 0x01 /* CRC0DAT accesses bits 15-8 of the 16-bit CRC result. */

#define CRCVAL__BMASK        0x04 /* CRC Initialization Value */
#define CRCVAL__SHIFT        0x02 /* CRC Initialization Value */
#define CRCVAL__SET_ZEROES   0x00 /* CRC result is set to 0x0000 on write of 1 to CRCINIT. */
#define CRCVAL__SET_ONES     0x04 /* CRC result is set to 0xFFFF on write of 1 to CRCINIT. */

#define CRCINIT__BMASK       0x08 /* CRC Initialization Enable */
#define CRCINIT__SHIFT       0x03 /* CRC Initialization Enable */
#define CRCINIT__DO_NOT_INIT 0x00 /* Do not initialize the CRC result. */
#define CRCINIT__INIT        0x08 /* Initialize the CRC result to ones or zeroes vased on the value of CRCVAL. */

/*------------------------------------------------------------------------------ */
/* CRC0CNT Enums (CRC0 Automatic Flash Sector Count @ 0xD3)                      */
/*------------------------------------------------------------------------------ */
#define CRCCNT__FMASK  0x1F /* Automatic CRC Calculation Block Count */
#define CRCCNT__SHIFT  0x00 /* Automatic CRC Calculation Block Count */

#define CRCDN__BMASK   0x80 /* Automatic CRC Calculation Complete */
#define CRCDN__SHIFT   0x07 /* Automatic CRC Calculation Complete */
#define CRCDN__NOT_SET 0x00 /* A CRC calculation is in progress. */
#define CRCDN__SET     0x80 /* A CRC calculation is not in progress. */


/*------------------------------------------------------------------------------ */
/* DERIVID Enums (Derivative Identification @ 0xAD)                              */
/*------------------------------------------------------------------------------ */
#define DERIVID__FMASK              0xFF /* Derivative ID */
#define DERIVID__SHIFT              0x00 /* Derivative ID */
#define DERIVID__EFM8BB10F8G_QSOP24 0x01 /* EFM8BB10F8G-{R}-QSOP24 */
#define DERIVID__EFM8BB10F8G_QFN20  0x02 /* EFM8BB10F8G-{R}-QFN20 */
#define DERIVID__EFM8BB10F8G_SOIC16 0x03 /* EFM8BB10F8G-{R}-SOIC16 */
#define DERIVID__EFM8BB10F4G_QFN20  0x05 /* EFM8BB10F4G-{R}-QFN20 */
#define DERIVID__EFM8BB10F2G_QFN20  0x08 /* EFM8BB10F2G-{R}-QFN20 */

/*------------------------------------------------------------------------------ */
/* REVID Enums (Revision Identifcation @ 0xB6)                                   */
/*------------------------------------------------------------------------------ */
#define REVID__FMASK 0xFF /* Revision ID */
#define REVID__SHIFT 0x00 /* Revision ID */
#define REVID__REV_A 0x02 /* Revision A */

/*------------------------------------------------------------------------------ */
/* IT01CF Enums (INT0/INT1 Configuration @ 0xE4)                                 */
/*------------------------------------------------------------------------------ */
#define IN0SL__FMASK       0x07 /* INT0 Port Pin Selection */
#define IN0SL__SHIFT       0x00 /* INT0 Port Pin Selection */
#define IN0SL__P0_0        0x00 /* Select P0.0. */
#define IN0SL__P0_1        0x01 /* Select P0.1. */
#define IN0SL__P0_2        0x02 /* Select P0.2. */
#define IN0SL__P0_3        0x03 /* Select P0.3. */
#define IN0SL__P0_4        0x04 /* Select P0.4. */
#define IN0SL__P0_5        0x05 /* Select P0.5. */
#define IN0SL__P0_6        0x06 /* Select P0.6. */
#define IN0SL__P0_7        0x07 /* Select P0.7. */

#define IN0PL__BMASK       0x08 /* INT0 Polarity */
#define IN0PL__SHIFT       0x03 /* INT0 Polarity */
#define IN0PL__ACTIVE_LOW  0x00 /* INT0 input is active low. */
#define IN0PL__ACTIVE_HIGH 0x08 /* INT0 input is active high. */

#define IN1SL__FMASK       0x70 /* INT1 Port Pin Selection */
#define IN1SL__SHIFT       0x04 /* INT1 Port Pin Selection */
#define IN1SL__P0_0        0x00 /* Select P0.0. */
#define IN1SL__P0_1        0x10 /* Select P0.1. */
#define IN1SL__P0_2        0x20 /* Select P0.2. */
#define IN1SL__P0_3        0x30 /* Select P0.3. */
#define IN1SL__P0_4        0x40 /* Select P0.4. */
#define IN1SL__P0_5        0x50 /* Select P0.5. */
#define IN1SL__P0_6        0x60 /* Select P0.6. */
#define IN1SL__P0_7        0x70 /* Select P0.7. */

#define IN1PL__BMASK       0x80 /* INT1 Polarity */
#define IN1PL__SHIFT       0x07 /* INT1 Polarity */
#define IN1PL__ACTIVE_LOW  0x00 /* INT1 input is active low. */
#define IN1PL__ACTIVE_HIGH 0x80 /* INT1 input is active high. */

/*------------------------------------------------------------------------------ */
/* FLKEY Enums (Flash Lock and Key @ 0xB7)                                       */
/*------------------------------------------------------------------------------ */
#define FLKEY__FMASK    0xFF /* Flash Lock and Key */
#define FLKEY__SHIFT    0x00 /* Flash Lock and Key */
#define FLKEY__LOCKED   0x00 /* Flash is write/erase locked. */
#define FLKEY__FIRST    0x01 /* The first key code has been written (0xA5). */
#define FLKEY__UNLOCKED 0x02 /* Flash is unlocked (writes/erases allowed). */
#define FLKEY__DISABLED 0x03 /* Flash writes/erases are disabled until the next reset. */
#define FLKEY__KEY1     0xA5 /* Flash writes and erases are enabled by writing 0xA5 followed by 0xF1 to the FLKEY register. */
#define FLKEY__KEY2     0xF1 /* Flash writes and erases are enabled by writing 0xA5 followed by 0xF1 to the FLKEY register. */

/*------------------------------------------------------------------------------ */
/* PSCTL Enums (Program Store Control @ 0x8F)                                    */
/*------------------------------------------------------------------------------ */
#define PSWE__BMASK          0x01 /* Program Store Write Enable */
#define PSWE__SHIFT          0x00 /* Program Store Write Enable */
#define PSWE__WRITE_DISABLED 0x00 /* Writes to flash program memory disabled. */
#define PSWE__WRITE_ENABLED  0x01 /* Writes to flash program memory enabled; the MOVX write instruction targets flash memory. */

#define PSEE__BMASK          0x02 /* Program Store Erase Enable */
#define PSEE__SHIFT          0x01 /* Program Store Erase Enable */
#define PSEE__ERASE_DISABLED 0x00 /* Flash program memory erasure disabled. */
#define PSEE__ERASE_ENABLED  0x02 /* Flash program memory erasure enabled. */

/*------------------------------------------------------------------------------ */
/* IP Enums (Interrupt Priority @ 0xB8)                                          */
/*------------------------------------------------------------------------------ */
#define PX0__BMASK   0x01 /* External Interrupt 0 Priority Control */
#define PX0__SHIFT   0x00 /* External Interrupt 0 Priority Control */
#define PX0__LOW     0x00 /* External Interrupt 0 set to low priority level. */
#define PX0__HIGH    0x01 /* External Interrupt 0 set to high priority level. */

#define PT0__BMASK   0x02 /* Timer 0 Interrupt Priority Control */
#define PT0__SHIFT   0x01 /* Timer 0 Interrupt Priority Control */
#define PT0__LOW     0x00 /* Timer 0 interrupt set to low priority level. */
#define PT0__HIGH    0x02 /* Timer 0 interrupt set to high priority level. */

#define PX1__BMASK   0x04 /* External Interrupt 1 Priority Control */
#define PX1__SHIFT   0x02 /* External Interrupt 1 Priority Control */
#define PX1__LOW     0x00 /* External Interrupt 1 set to low priority level. */
#define PX1__HIGH    0x04 /* External Interrupt 1 set to high priority level. */

#define PT1__BMASK   0x08 /* Timer 1 Interrupt Priority Control */
#define PT1__SHIFT   0x03 /* Timer 1 Interrupt Priority Control */
#define PT1__LOW     0x00 /* Timer 1 interrupt set to low priority level. */
#define PT1__HIGH    0x08 /* Timer 1 interrupt set to high priority level. */

#define PS0__BMASK   0x10 /* UART0 Interrupt Priority Control */
#define PS0__SHIFT   0x04 /* UART0 Interrupt Priority Control */
#define PS0__LOW     0x00 /* UART0 interrupt set to low priority level. */
#define PS0__HIGH    0x10 /* UART0 interrupt set to high priority level. */

#define PT2__BMASK   0x20 /* Timer 2 Interrupt Priority Control */
#define PT2__SHIFT   0x05 /* Timer 2 Interrupt Priority Control */
#define PT2__LOW     0x00 /* Timer 2 interrupt set to low priority level. */
#define PT2__HIGH    0x20 /* Timer 2 interrupt set to high priority level. */

#define PSPI0__BMASK 0x40 /* Serial Peripheral Interface (SPI0) Interrupt Priority Control */
#define PSPI0__SHIFT 0x06 /* Serial Peripheral Interface (SPI0) Interrupt Priority Control */
#define PSPI0__LOW   0x00 /* SPI0 interrupt set to low priority level. */
#define PSPI0__HIGH  0x40 /* SPI0 interrupt set to high priority level. */

/*------------------------------------------------------------------------------ */
/* LFO0CN Enums (Low Frequency Oscillator Control @ 0xB1)                        */
/*------------------------------------------------------------------------------ */
typedef union {
    uint8_t reg;
    struct {
        uint8_t OSCLD : 2;
        uint8_t OSCLF : 4;
        uint8_t OSCLRDY : 1;
        uint8_t OSCLEN : 1;
    } bf;
} LFO0CN_t;

#define OSCLD__DIVIDE_BY_8 0x00 /* Divide by 8 selected. */
#define OSCLD__DIVIDE_BY_4 0x01 /* Divide by 4 selected. */
#define OSCLD__DIVIDE_BY_2 0x02 /* Divide by 2 selected. */
#define OSCLD__DIVIDE_BY_1 0x03 /* Divide by 1 selected. */

#define OSCLRDY__NOT_SET   0x00 /* Internal L-F Oscillator frequency not stabilized. */
#define OSCLRDY__SET       0x01 /* Internal L-F Oscillator frequency stabilized. */

#define OSCLEN__DISABLED   0x00 /* Internal L-F Oscillator Disabled. */
#define OSCLEN__ENABLED    0x01 /* Internal L-F Oscillator Enabled. */

/*------------------------------------------------------------------------------ */
/* PRTDRV Enums (Port Drive Strength @ 0xF6)                                     */
/*------------------------------------------------------------------------------ */
#define P0DRV__BMASK      0x01 /* Port 0 Drive Strength */
#define P0DRV__SHIFT      0x00 /* Port 0 Drive Strength */
#define P0DRV__LOW_DRIVE  0x00 /* All pins on P0 use low drive strength. */
#define P0DRV__HIGH_DRIVE 0x01 /* All pins on P0 use high drive strength. */

#define P1DRV__BMASK      0x02 /* Port 1 Drive Strength */
#define P1DRV__SHIFT      0x01 /* Port 1 Drive Strength */
#define P1DRV__LOW_DRIVE  0x00 /* All pins on P1 use low drive strength. */
#define P1DRV__HIGH_DRIVE 0x02 /* All pins on P1 use high drive strength. */

#define P2DRV__BMASK      0x04 /* Port 2 Drive Strength */
#define P2DRV__SHIFT      0x02 /* Port 2 Drive Strength */
#define P2DRV__LOW_DRIVE  0x00 /* All pins on P2 use low drive strength. */
#define P2DRV__HIGH_DRIVE 0x04 /* All pins on P2 use high drive strength. */

/*------------------------------------------------------------------------------ */
/* PCON0 Enums (Power Control @ 0x87)                                            */
/*------------------------------------------------------------------------------ */
#define IDLE__BMASK  0x01 /* Idle Mode Select */
#define IDLE__SHIFT  0x00 /* Idle Mode Select */
#define IDLE__NORMAL 0x00 /* Idle mode not activated. */
#define IDLE__IDLE   0x01 /* CPU goes into Idle mode (shuts off clock to CPU, but clocks to enabled peripherals are still active). */

#define STOP__BMASK  0x02 /* Stop Mode Select */
#define STOP__SHIFT  0x01 /* Stop Mode Select */
#define STOP__NORMAL 0x00 /* Stop mode not activated. */
#define STOP__STOP   0x02 /* CPU goes into Stop mode (internal oscillator stopped). */

#define GF0__BMASK   0x04 /* General Purpose Flag 0 */
#define GF0__SHIFT   0x02 /* General Purpose Flag 0 */
#define GF0__NOT_SET 0x00 /* The GF0 flag is not set. Clear the GF0 flag. */
#define GF0__SET     0x04 /* The GF0 flag is set. Set the GF0 flag. */

#define GF1__BMASK   0x08 /* General Purpose Flag 1 */
#define GF1__SHIFT   0x03 /* General Purpose Flag 1 */
#define GF1__NOT_SET 0x00 /* The GF1 flag is not set. Clear the GF1 flag. */
#define GF1__SET     0x08 /* The GF1 flag is set. Set the GF1 flag. */

#define GF2__BMASK   0x10 /* General Purpose Flag 2 */
#define GF2__SHIFT   0x04 /* General Purpose Flag 2 */
#define GF2__NOT_SET 0x00 /* The GF2 flag is not set. Clear the GF2 flag. */
#define GF2__SET     0x10 /* The GF2 flag is set. Set the GF2 flag. */

#define GF3__BMASK   0x20 /* General Purpose Flag 3 */
#define GF3__SHIFT   0x05 /* General Purpose Flag 3 */
#define GF3__NOT_SET 0x00 /* The GF3 flag is not set. Clear the GF3 flag. */
#define GF3__SET     0x20 /* The GF3 flag is set. Set the GF3 flag. */

#define GF4__BMASK   0x40 /* General Purpose Flag 4 */
#define GF4__SHIFT   0x06 /* General Purpose Flag 4 */
#define GF4__NOT_SET 0x00 /* The GF4 flag is not set. Clear the GF4 flag. */
#define GF4__SET     0x40 /* The GF4 flag is set. Set the GF4 flag. */

#define GF5__BMASK   0x80 /* General Purpose Flag 5 */
#define GF5__SHIFT   0x07 /* General Purpose Flag 5 */
#define GF5__NOT_SET 0x00 /* The GF5 flag is not set. Clear the GF5 flag. */
#define GF5__SET     0x80 /* The GF5 flag is set. Set the GF5 flag. */

/*------------------------------------------------------------------------------ */
/* RSTSRC Enums (Reset Source @ 0xEF)                                            */
/*------------------------------------------------------------------------------ */
#define PINRSF__BMASK   0x01 /* HW Pin Reset Flag */
#define PINRSF__SHIFT   0x00 /* HW Pin Reset Flag */
#define PINRSF__NOT_SET 0x00 /* The RSTb pin did not cause the last reset. */
#define PINRSF__SET     0x01 /* The RSTb pin caused the last reset. */

#define PORSF__BMASK    0x02 /* Power-On / Supply Monitor Reset Flag, and Supply Monitor Reset Enable */
#define PORSF__SHIFT    0x01 /* Power-On / Supply Monitor Reset Flag, and Supply Monitor Reset Enable */
#define PORSF__NOT_SET  0x00 /* A power-on or supply monitor reset did not occur. */
#define PORSF__SET      0x02 /* A power-on or supply monitor reset occurred. */

#define MCDRSF__BMASK   0x04 /* Missing Clock Detector Enable and Flag */
#define MCDRSF__SHIFT   0x02 /* Missing Clock Detector Enable and Flag */
#define MCDRSF__NOT_SET 0x00 /* A missing clock detector reset did not occur. */
#define MCDRSF__SET     0x04 /* A missing clock detector reset occurred. */

#define WDTRSF__BMASK   0x08 /* Watchdog Timer Reset Flag */
#define WDTRSF__SHIFT   0x03 /* Watchdog Timer Reset Flag */
#define WDTRSF__NOT_SET 0x00 /* A watchdog timer overflow reset did not occur. */
#define WDTRSF__SET     0x08 /* A watchdog timer overflow reset occurred. */

#define SWRSF__BMASK    0x10 /* Software Reset Force and Flag */
#define SWRSF__SHIFT    0x04 /* Software Reset Force and Flag */
#define SWRSF__NOT_SET  0x00 /* A software reset did not occur. */
#define SWRSF__SET      0x10 /* A software reset occurred. */

#define C0RSEF__BMASK   0x20 /* Comparator0 Reset Enable and Flag */
#define C0RSEF__SHIFT   0x05 /* Comparator0 Reset Enable and Flag */
#define C0RSEF__NOT_SET 0x00 /* A Comparator 0 reset did not occur. */
#define C0RSEF__SET     0x20 /* A Comparator 0 reset occurred. */

#define FERROR__BMASK   0x40 /* Flash Error Reset Flag */
#define FERROR__SHIFT   0x06 /* Flash Error Reset Flag */
#define FERROR__NOT_SET 0x00 /* A flash error reset did not occur. */
#define FERROR__SET     0x40 /* A flash error reset occurred. */

/*------------------------------------------------------------------------------ */
/* CKCON0 Enums (Clock Control 0 @ 0x8E)                                         */
/*------------------------------------------------------------------------------ */
#define SCA__FMASK           0x03 /* Timer 0/1 Prescale */
#define SCA__SHIFT           0x00 /* Timer 0/1 Prescale */
#define SCA__SYSCLK_DIV_12   0x00 /* System clock divided by 12. */
#define SCA__SYSCLK_DIV_4    0x01 /* System clock divided by 4. */
#define SCA__SYSCLK_DIV_48   0x02 /* System clock divided by 48. */
#define SCA__EXTOSC_DIV_8    0x03 /* External oscillator divided by 8 (synchronized with the system clock). */

#define T0M__PRESCALE        0x00 /* Counter/Timer 0 uses the clock defined by the prescale field, SCA. */
#define T0M__SYSCLK          0x04 /* Counter/Timer 0 uses the system clock. */

#define T1M__PRESCALE        0x00 /* Timer 1 uses the clock defined by the prescale field, SCA. */
#define T1M__SYSCLK          0x08 /* Timer 1 uses the system clock. */

#define T2ML__BMASK          0x10 /* Timer 2 Low Byte Clock Select */
#define T2ML__SHIFT          0x04 /* Timer 2 Low Byte Clock Select */
#define T2ML__EXTERNAL_CLOCK 0x00 /* Timer 2 low byte uses the clock defined by T2XCLK in TMR2CN0. */
#define T2ML__SYSCLK         0x10 /* Timer 2 low byte uses the system clock. */

#define T2MH__BMASK          0x20 /* Timer 2 High Byte Clock Select */
#define T2MH__SHIFT          0x05 /* Timer 2 High Byte Clock Select */
#define T2MH__EXTERNAL_CLOCK 0x00 /* Timer 2 high byte uses the clock defined by T2XCLK in TMR2CN0. */
#define T2MH__SYSCLK         0x20 /* Timer 2 high byte uses the system clock. */

#define T3ML__BMASK          0x40 /* Timer 3 Low Byte Clock Select */
#define T3ML__SHIFT          0x06 /* Timer 3 Low Byte Clock Select */
#define T3ML__EXTERNAL_CLOCK 0x00 /* Timer 3 low byte uses the clock defined by T3XCLK in TMR3CN0. */
#define T3ML__SYSCLK         0x40 /* Timer 3 low byte uses the system clock. */

#define T3MH__BMASK          0x80 /* Timer 3 High Byte Clock Select */
#define T3MH__SHIFT          0x07 /* Timer 3 High Byte Clock Select */
#define T3MH__EXTERNAL_CLOCK 0x00 /* Timer 3 high byte uses the clock defined by T3XCLK in TMR3CN0. */
#define T3MH__SYSCLK         0x80 /* Timer 3 high byte uses the system clock. */

/*------------------------------------------------------------------------------ */
/* TCON Enums (Timer 0/1 Control @ 0x88)                                         */
/*------------------------------------------------------------------------------ */
#define IT0__BMASK   0x01 /* Interrupt 0 Type Select */
#define IT0__SHIFT   0x00 /* Interrupt 0 Type Select */
#define IT0__LEVEL   0x00 /* INT0 is level triggered. */
#define IT0__EDGE    0x01 /* INT0 is edge triggered. */

#define IE0__BMASK   0x02 /* External Interrupt 0 */
#define IE0__SHIFT   0x01 /* External Interrupt 0 */
#define IE0__NOT_SET 0x00 /* Edge/level not detected. */
#define IE0__SET     0x02 /* Edge/level detected */

#define IT1__BMASK   0x04 /* Interrupt 1 Type Select */
#define IT1__SHIFT   0x02 /* Interrupt 1 Type Select */
#define IT1__LEVEL   0x00 /* INT1 is level triggered. */
#define IT1__EDGE    0x04 /* INT1 is edge triggered. */

#define IE1__BMASK   0x08 /* External Interrupt 1 */
#define IE1__SHIFT   0x03 /* External Interrupt 1 */
#define IE1__NOT_SET 0x00 /* Edge/level not detected. */
#define IE1__SET     0x08 /* Edge/level detected */

#define TR0__BMASK   0x10 /* Timer 0 Run Control */
#define TR0__SHIFT   0x04 /* Timer 0 Run Control */
#define TR0__STOP    0x00 /* Stop Timer 0. */
#define TR0__RUN     0x10 /* Start Timer 0 running. */

#define TF0__BMASK   0x20 /* Timer 0 Overflow Flag */
#define TF0__SHIFT   0x05 /* Timer 0 Overflow Flag */
#define TF0__NOT_SET 0x00 /* Timer 0 did not overflow. */
#define TF0__SET     0x20 /* Timer 0 overflowed. */

#define TR1__BMASK   0x40 /* Timer 1 Run Control */
#define TR1__SHIFT   0x06 /* Timer 1 Run Control */
#define TR1__STOP    0x00 /* Stop Timer 1. */
#define TR1__RUN     0x40 /* Start Timer 1 running. */

#define TF1__BMASK   0x80 /* Timer 1 Overflow Flag */
#define TF1__SHIFT   0x07 /* Timer 1 Overflow Flag */
#define TF1__NOT_SET 0x00 /* Timer 1 did not overflow. */
#define TF1__SET     0x80 /* Timer 1 overflowed. */

/*------------------------------------------------------------------------------ */
/* TMOD Enums (Timer 0/1 Mode @ 0x89)                                            */
/*------------------------------------------------------------------------------ */
#define T0M__MODE0      0x00 /* Mode 0, 13-bit Counter/Timer */
#define T0M__MODE1      0x01 /* Mode 1, 16-bit Counter/Timer */
#define T0M__MODE2      0x02 /* Mode 2, 8-bit Counter/Timer with Auto-Reload */
#define T0M__MODE3      0x03 /* Mode 3, Two 8-bit Counter/Timers */

#define CT0__BMASK      0x04 /* Counter/Timer 0 Select */
#define CT0__SHIFT      0x02 /* Counter/Timer 0 Select */
#define CT0__TIMER      0x00 /* Timer Mode. Timer 0 increments on the clock defined by T0M in the CKCON0 register. */
#define CT0__COUNTER    0x04 /* Counter Mode. Timer 0 increments on high-to-low transitions of an external pin (T0). */

#define GATE0__BMASK    0x08 /* Timer 0 Gate Control */
#define GATE0__SHIFT    0x03 /* Timer 0 Gate Control */
#define GATE0__DISABLED 0x00 /* Timer 0 enabled when TR0 = 1 irrespective of INT0 logic level. */
#define GATE0__ENABLED  0x08 /* Timer 0 enabled only when TR0 = 1 and INT0 is active as defined by bit IN0PL in register IT01CF. */

#define T1M__FMASK      0x30 /* Timer 1 Mode Select */
#define T1M__SHIFT      0x04 /* Timer 1 Mode Select */
#define T1M__MODE0      0x00 /* Mode 0, 13-bit Counter/Timer */
#define T1M__MODE1      0x10 /* Mode 1, 16-bit Counter/Timer */
#define T1M__MODE2      0x20 /* Mode 2, 8-bit Counter/Timer with Auto-Reload */
#define T1M__MODE3      0x30 /* Mode 3, Timer 1 Inactive */

#define CT1__BMASK      0x40 /* Counter/Timer 1 Select */
#define CT1__SHIFT      0x06 /* Counter/Timer 1 Select */
#define CT1__TIMER      0x00 /* Timer Mode. Timer 1 increments on the clock defined by T1M in the CKCON0 register. */
#define CT1__COUNTER    0x40 /* Counter Mode. Timer 1 increments on high-to-low transitions of an external pin (T1). */

#define GATE1__BMASK    0x80 /* Timer 1 Gate Control */
#define GATE1__SHIFT    0x07 /* Timer 1 Gate Control */
#define GATE1__DISABLED 0x00 /* Timer 1 enabled when TR1 = 1 irrespective of INT1 logic level. */
#define GATE1__ENABLED  0x80 /* Timer 1 enabled only when TR1 = 1 and INT1 is active as defined by bit IN1PL in register IT01CF. */

/*------------------------------------------------------------------------------ */
/* VDM0CN Enums (Supply Monitor Control @ 0xFF)                                  */
/*------------------------------------------------------------------------------ */
#define VDDSTAT__BMASK  0x40 /* Supply Status */
#define VDDSTAT__SHIFT  0x06 /* Supply Status */
#define VDDSTAT__BELOW  0x00 /* VDD is at or below the supply monitor threshold. */
#define VDDSTAT__ABOVE  0x40 /* VDD is above the supply monitor threshold. */

#define VDMEN__BMASK    0x80 /* Supply Monitor Enable */
#define VDMEN__SHIFT    0x07 /* Supply Monitor Enable */
#define VDMEN__DISABLED 0x00 /* Supply Monitor Disabled. */
#define VDMEN__ENABLED  0x80 /* Supply Monitor Enabled. */

/*------------------------------------------------------------------------------ */
/* Watchdog Timer Control                                                        */
/*------------------------------------------------------------------------------ */

#ifdef WDT_no

#define WDT_lockout() 
#define WDT_reset()   
#define WDT_enable()  
#define WDT_1ms()     
#define WDT_2ms()     
#define WDT_13ms()    
#define WDT_51ms()    
#define WDT_204ms()   
#define WDT_819ms()   
#define WDT_32767ms() 
#define WDT_13107ms() 

#else

#define WDT_lockout() WDTCN=0xff
#define WDT_reset()   WDTCN=0xA5
#define WDT_enable()  WDTCN=0xA5
#define WDT_1ms()     WDTCN=0x00
#define WDT_2ms()     WDTCN=0x01
#define WDT_13ms()    WDTCN=0x02
#define WDT_51ms()    WDTCN=0x03
#define WDT_204ms()   WDTCN=0x04
#define WDT_819ms()   WDTCN=0x05
#define WDT_32767ms() WDTCN=0x06
#define WDT_13107ms() WDTCN=0x07

#endif /* WDT_no */

#endif /* EFM8BB1_H */
