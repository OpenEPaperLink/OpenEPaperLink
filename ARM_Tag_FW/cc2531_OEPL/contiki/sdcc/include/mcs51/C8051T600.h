/*-------------------------------------------------------------------------
   C8051T600.h - Register Declarations for the SiLabs C8051T60x Processor
   Range

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

#ifndef C8051T600_H
#define C8051T600_H

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
SFR(     SCON, 0x98 )  ; /* SERIAL PORT CONTROL */
SFR(    SCON0, 0x98 )  ; /* SERIAL PORT CONTROL */
SFR(     SBUF, 0x99 )  ; /* SERIAL PORT BUFFER */
SFR(    SBUF0, 0x99 )  ; /* SERIAL PORT BUFFER */
SFR(   CPT0MD, 0x9D )  ; /* COMPARATOR 0 MODE SELECTION */
SFR(   CPT0MX, 0x9F )  ; /* COMPARATOR 0 MUX SELECTION */
SFR(    TOFFL, 0xA2 )  ; /* TEMPERATURE SENSOR OFFSET - LOW BYTE */
SFR(    TOFFH, 0xA3 )  ; /* TEMPERATURE SENSOR OFFSET - HIGH BYTE */
SFR(  P0MDOUT, 0xA4 )  ; /* PORT 0 OUTPUT MODE CONFIGURATION */
SFR(       IE, 0xA8 )  ; /* INTERRUPT ENABLE */
SFR(   OSCXCN, 0xB1 )  ; /* EXTERNAL OSCILLATOR CONTROL */
SFR(   OSCICN, 0xB2 )  ; /* INTERNAL OSCILLATOR CONTROL */
SFR(   OSCICL, 0xB3 )  ; /* INTERNAL OSCILLATOR CALIBRATION */
SFR(       IP, 0xB8 )  ; /* INTERRUPT PRIORITY */
SFR(   AMX0SL, 0xBB )  ; /* ADC 0 MUX CHANNEL SELECTION */
SFR(   ADC0CF, 0xBC )  ; /* ADC 0 CONFIGURATION */
SFR(    ADC0L, 0xBD )  ; /* ADC 0 DATA - LOW BYTE */
SFR(    ADC0H, 0xBE )  ; /* ADC 0 DATA - HIGH BYTE */
SFR(   SMB0CN, 0xC0 )  ; /* SMBUS CONTROL */
SFR(   SMB0CF, 0xC1 )  ; /* SMBUS CONFIGURATION */
SFR(  SMB0DAT, 0xC2 )  ; /* SMBUS DATA */
SFR(  ADC0GTL, 0xC3 )  ; /* ADC 0 GREATER-THAN REGISTER - LOW BYTE */
SFR(  ADC0GTH, 0xC4 )  ; /* ADC 0 GREATER-THAN REGISTER - HIGH BYTE */
SFR(  ADC0LTL, 0xC5 )  ; /* ADC 0 LESS-THAN REGISTER - LOW BYTE */
SFR(  ADC0LTH, 0xC6 )  ; /* ADC 0 LESS-THAN REGISTER - HIGH BYTE */
SFR(   REG0CN, 0xC7 )  ; /* Voltage Regulator Control */
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
SFR(   PCA0CN, 0xD8 )  ; /* PCA CONTROL */
SFR(   PCA0MD, 0xD9 )  ; /* PCA MODE */
SFR( PCA0CPM0, 0xDA )  ; /* PCA MODULE 0 MODE REGISTER */
SFR( PCA0CPM1, 0xDB )  ; /* PCA MODULE 1 MODE REGISTER */
SFR( PCA0CPM2, 0xDC )  ; /* PCA MODULE 2 MODE REGISTER */
SFR(      ACC, 0xE0 )  ; /* ACCUMULATOR */
SFR(   PRT0MX, 0xE1 )  ; /* PORT MUX CONFIGURATION REGISTER 0 */
SFR(     XBR0, 0xE1 )  ; /* PORT MUX CONFIGURATION REGISTER 0 */
SFR(   PRT1MX, 0xE2 )  ; /* PORT MUX CONFIGURATION REGISTER 1 */
SFR(     XBR1, 0xE2 )  ; /* PORT MUX CONFIGURATION REGISTER 1 */
SFR(   PRT2MX, 0xE3 )  ; /* PORT MUX CONFIGURATION REGISTER 2 */
SFR(     XBR2, 0xE3 )  ; /* PORT MUX CONFIGURATION REGISTER 2 */
SFR(   IT01CF, 0xE4 )  ; /* INT0/INT1 CONFIGURATION REGISTER */
SFR(  INT01CF, 0xE4 )  ; /* INT0/INT1 CONFIGURATION REGISTER */
SFR(     EIE1, 0xE6 )  ; /* EXTERNAL INTERRUPT ENABLE 1 */
SFR(   ADC0CN, 0xE8 )  ; /* ADC 0 CONTROL */
SFR( PCA0CPL1, 0xE9 )  ; /* PCA CAPTURE 1 LOW */
SFR( PCA0CPH1, 0xEA )  ; /* PCA CAPTURE 1 HIGH */
SFR( PCA0CPL2, 0xEB )  ; /* PCA CAPTURE 2 LOW */
SFR( PCA0CPH2, 0xEC )  ; /* PCA CAPTURE 2 HIGH */
SFR(   RSTSRC, 0xEF )  ; /* RESET SOURCE */
SFR(        B, 0xF0 )  ; /* B REGISTER */
SFR(   P0MODE, 0xF1 )  ; /* PORT 0 INPUT MODE CONFIGURATION */
SFR(   P0MDIN, 0xF1 )  ; /* PORT 0 INPUT MODE CONFIGURATION */
SFR(     EIP1, 0xF6 )  ; /* EXTERNAL INTERRUPT PRIORITY REGISTER 1 */
SFR(   CPT0CN, 0xF8 )  ; /* COMPARATOR 0 CONTROL */
SFR(    PCA0L, 0xF9 )  ; /* PCA COUNTER LOW */
SFR(    PCA0H, 0xFA )  ; /* PCA COUNTER HIGH */
SFR( PCA0CPL0, 0xFB )  ; /* PCA CAPTURE 0 LOW */
SFR( PCA0CPH0, 0xFC )  ; /* PCA CAPTURE 0 HIGH */


/*  WORD/DWORD Registers  */

SFR16E(     TMR0, 0x8C8A )  ; /* TIMER 0 COUNTER */
SFR16E(     TMR1, 0x8D8B )  ; /* TIMER 1 COUNTER */
SFR16E(     TOFF, 0xA3A2 )  ; /* TEMPERATURE SENSOR OFFSET WORD */
SFR16E(     ADC0, 0xAEAD )  ; /* ADC0 DATA WORD */
SFR16E(   ADC0GT, 0xC4C3 )  ; /* ADC 0 GREATER-THAN REGISTER WORD */
SFR16E(   ADC0LT, 0xC6C5 )  ; /* ADC 0 LESS-THAN REGISTER WORD */
SFR16E(     TMR2, 0xCDCC )  ; /* TIMER 2 COUNTER */
SFR16E(    RCAP2, 0xCBCA )  ; /* TIMER 2 CAPTURE REGISTER WORD */
SFR16E(   TMR2RL, 0xCBCA )  ; /* TIMER 2 CAPTURE REGISTER WORD */
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

/*  CPT0CN  0xF8 */
SBIT(  CP0HYN0, 0xF8, 0 )  ; /* CPT0CN.0 - Comp.0 Neg. Hysteresis Control Bit0*/
SBIT(  CP0HYN1, 0xF8, 1 )  ; /* CPT0CN.1 - Comp.0 Neg. Hysteresis Control Bit1*/
SBIT(  CP0HYP0, 0xF8, 2 )  ; /* CPT0CN.2 - Comp.0 Pos. Hysteresis Control Bit0*/
SBIT(  CP0HYP1, 0xF8, 3 )  ; /* CPT0CN.3 - Comp.0 Pos. Hysteresis Control Bit1*/
SBIT(   CP0FIF, 0xF8, 4 )  ; /* CPT0CN.4 - Comparator0 Falling-Edge Int. Flag */
SBIT(   CP0RIF, 0xF8, 5 )  ; /* CPT0CN.5 - Comparator0 Rising-Edge Int. Flag */
SBIT(   CP0OUT, 0xF8, 6 )  ; /* CPT0CN.6 - Comparator0 Output State Flag */
SBIT(    CP0EN, 0xF8, 7 )  ; /* CPT0CN.7 - Comparator0 Enable Bit */


/* Predefined SFR Bit Masks */

#define PCON_IDLE         0x01    /* PCON                                */
#define PCON_STOP         0x02    /* PCON                                */
#define T1M               0x10    /* CKCON                               */
#define PSWE              0x01    /* PSCTL                               */
#define PSEE              0x02    /* PSCTL                               */
#define ECP0F             0x10    /* EIE1                                */
#define ECP0R             0x20    /* EIE1                                */
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
#define CP0E              0x10    /* XBR1                                */
#define CP0OEN            0x10    /* XBR1                                */
#define CP0AE             0x20    /* XBR1                                */
#define CP0AOEN           0x20    /* XBR1                                */

/* Interrupts */

#define INT_EXT0          0       /* External Interrupt 0 */
#define INT_TIMER0        1       /* Timer0 Overflow */
#define INT_EXT1          2       /* External Interrupt 1 */
#define INT_TIMER1        3       /* Timer1 Overflow */
#define INT_UART0         4       /* Serial Port 0 */
#define INT_TIMER2        5       /* Timer2 Overflow */
#define INT_SMBUS0        6       /* SMBus0 Interface */
#define INT_ADC0_WINDOW   7       /* ADC0 Window Comparison */
#define INT_ADC0_EOC      8       /* ADC0 End Of Conversion */
#define INT_PCA0          9       /* PCA0 Peripheral */
#define INT_CP0F          10      /* Comparator0 falling edge */
#define INT_CP0R          11      /* Comparator1 rising edge */

#endif
