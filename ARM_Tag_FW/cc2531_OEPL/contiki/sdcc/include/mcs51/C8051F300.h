/*-------------------------------------------------------------------------
   C8051F300.h - Register Declarations for the Cygnal/SiLabs C8051F30x
   Processor Range

   Copyright (C) 2004, Maarten Brock, sourceforge.brock@dse.nl

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


#ifndef C8051F300_H
#define C8051F300_H


/*  BYTE Registers  */

__sfr __at (0x80) P0           ;  /* PORT 0                                        */
__sfr __at (0x81) SP           ;  /* STACK POINTER                                 */
__sfr __at (0x82) DPL          ;  /* DATA POINTER - LOW BYTE                       */
__sfr __at (0x83) DPH          ;  /* DATA POINTER - HIGH BYTE                      */
__sfr __at (0x87) PCON         ;  /* POWER CONTROL                                 */
__sfr __at (0x88) TCON         ;  /* TIMER CONTROL                                 */
__sfr __at (0x89) TMOD         ;  /* TIMER MODE                                    */
__sfr __at (0x8A) TL0          ;  /* TIMER 0 - LOW BYTE                            */
__sfr __at (0x8B) TL1          ;  /* TIMER 1 - LOW BYTE                            */
__sfr __at (0x8C) TH0          ;  /* TIMER 0 - HIGH BYTE                           */
__sfr __at (0x8D) TH1          ;  /* TIMER 1 - HIGH BYTE                           */
__sfr __at (0x8E) CKCON        ;  /* CLOCK CONTROL                                 */
__sfr __at (0x8F) PSCTL        ;  /* PROGRAM STORE R/W CONTROL                     */
__sfr __at (0x98) SCON         ;  /* SERIAL PORT CONTROL                           */
__sfr __at (0x98) SCON0        ;  /* SERIAL PORT CONTROL                           */
__sfr __at (0x99) SBUF         ;  /* SERIAL PORT BUFFER                            */
__sfr __at (0x99) SBUF0        ;  /* SERIAL PORT BUFFER                            */
__sfr __at (0x9D) CPT0MD       ;  /* COMPARATOR 0 MODE SELECTION                   */
__sfr __at (0x9F) CPT0MX       ;  /* COMPARATOR 0 MUX SELECTION                    */
__sfr __at (0xA4) P0MDOUT      ;  /* PORT 0 OUTPUT MODE CONFIGURATION              */
__sfr __at (0xA8) IE           ;  /* INTERRUPT ENABLE                              */
__sfr __at (0xB1) OSCXCN       ;  /* EXTERNAL OSCILLATOR CONTROL                   */
__sfr __at (0xB2) OSCICN       ;  /* INTERNAL OSCILLATOR CONTROL                   */
__sfr __at (0xB3) OSCICL       ;  /* INTERNAL OSCILLATOR CALIBRATION               */
__sfr __at (0xB6) FLSCL        ;  /* FLASH MEMORY TIMING PRESCALER                 */
__sfr __at (0xB7) FLKEY        ;  /* FLASH ACESS LIMIT                             */
__sfr __at (0xB8) IP           ;  /* INTERRUPT PRIORITY                            */
__sfr __at (0xBB) AMX0SL       ;  /* ADC 0 MUX CHANNEL SELECTION                   */
__sfr __at (0xBC) ADC0CF       ;  /* ADC 0 CONFIGURATION                           */
__sfr __at (0xBE) ADC0         ;  /* ADC 0 DATA                                    */
__sfr __at (0xC0) SMB0CN       ;  /* SMBUS CONTROL                                 */
__sfr __at (0xC1) SMB0CF       ;  /* SMBUS CONFIGURATION                           */
__sfr __at (0xC2) SMB0DAT      ;  /* SMBUS DATA                                    */
__sfr __at (0xC4) ADC0GT       ;  /* ADC 0 GREATER-THAN REGISTER                   */
__sfr __at (0xC6) ADC0LT       ;  /* ADC 0 LESS-THAN REGISTER                      */
__sfr __at (0xC8) T2CON        ;  /* TIMER 2 CONTROL                               */
__sfr __at (0xC8) TMR2CN       ;  /* TIMER 2 CONTROL                               */
__sfr __at (0xCA) RCAP2L       ;  /* TIMER 2 CAPTURE REGISTER - LOW BYTE           */
__sfr __at (0xCA) TMR2RLL      ;  /* TIMER 2 CAPTURE REGISTER - LOW BYTE           */
__sfr __at (0xCB) RCAP2H       ;  /* TIMER 2 CAPTURE REGISTER - HIGH BYTE          */
__sfr __at (0xCB) TMR2RLH      ;  /* TIMER 2 CAPTURE REGISTER - HIGH BYTE          */
__sfr __at (0xCC) TL2          ;  /* TIMER 2 - LOW BYTE                            */
__sfr __at (0xCC) TMR2L        ;  /* TIMER 2 - LOW BYTE                            */
__sfr __at (0xCD) TH2          ;  /* TIMER 2 - HIGH BYTE                           */
__sfr __at (0xCD) TMR2H        ;  /* TIMER 2 - HIGH BYTE                           */
__sfr __at (0xD0) PSW          ;  /* PROGRAM STATUS WORD                           */
__sfr __at (0xD1) REF0CN       ;  /* VOLTAGE REFERENCE 0 CONTROL                   */
__sfr __at (0xD8) PCA0CN       ;  /* PCA CONTROL                                   */
__sfr __at (0xD9) PCA0MD       ;  /* PCA MODE                                      */
__sfr __at (0xDA) PCA0CPM0     ;  /* PCA MODULE 0 MODE REGISTER                    */
__sfr __at (0xDB) PCA0CPM1     ;  /* PCA MODULE 1 MODE REGISTER                    */
__sfr __at (0xDC) PCA0CPM2     ;  /* PCA MODULE 2 MODE REGISTER                    */
__sfr __at (0xE0) ACC          ;  /* ACCUMULATOR                                   */
__sfr __at (0xE1) PRT0MX       ;  /* PORT MUX CONFIGURATION REGISTER 0             */
__sfr __at (0xE1) XBR0         ;  /* PORT MUX CONFIGURATION REGISTER 0             */
__sfr __at (0xE2) PRT1MX       ;  /* PORT MUX CONFIGURATION REGISTER 1             */
__sfr __at (0xE2) XBR1         ;  /* PORT MUX CONFIGURATION REGISTER 1             */
__sfr __at (0xE3) PRT2MX       ;  /* PORT MUX CONFIGURATION REGISTER 2             */
__sfr __at (0xE3) XBR2         ;  /* PORT MUX CONFIGURATION REGISTER 2             */
__sfr __at (0xE4) IT01CF       ;  /* INT0/INT1 CONFIGURATION REGISTER              */
__sfr __at (0xE4) INT01CF      ;  /* INT0/INT1 CONFIGURATION REGISTER              */
__sfr __at (0xE6) EIE1         ;  /* EXTERNAL INTERRUPT ENABLE 1                   */
__sfr __at (0xE8) ADC0CN       ;  /* ADC 0 CONTROL                                 */
__sfr __at (0xE9) PCA0CPL1     ;  /* PCA CAPTURE 1 LOW                             */
__sfr __at (0xEA) PCA0CPH1     ;  /* PCA CAPTURE 1 HIGH                            */
__sfr __at (0xEB) PCA0CPL2     ;  /* PCA CAPTURE 2 LOW                             */
__sfr __at (0xEC) PCA0CPH2     ;  /* PCA CAPTURE 2 HIGH                            */
__sfr __at (0xEF) RSTSRC       ;  /* RESET SOURCE                                  */
__sfr __at (0xF0) B            ;  /* B REGISTER                                    */
__sfr __at (0xF1) P0MODE       ;  /* PORT 0 INPUT MODE CONFIGURATION               */
__sfr __at (0xF1) P0MDIN       ;  /* PORT 0 INPUT MODE CONFIGURATION               */
__sfr __at (0xF6) EIP1         ;  /* EXTERNAL INTERRUPT PRIORITY REGISTER 1        */
__sfr __at (0xF8) CPT0CN       ;  /* COMPARATOR 0 CONTROL                          */
__sfr __at (0xF9) PCA0L        ;  /* PCA COUNTER LOW                               */
__sfr __at (0xFA) PCA0H        ;  /* PCA COUNTER HIGH                              */
__sfr __at (0xFB) PCA0CPL0     ;  /* PCA CAPTURE 0 LOW                             */
__sfr __at (0xFC) PCA0CPH0     ;  /* PCA CAPTURE 0 HIGH                            */


/*  WORD/DWORD Registers  */

__sfr16 __at (0x8C8A) TMR0     ;  /* TIMER 0 COUNTER                               */
__sfr16 __at (0x8D8B) TMR1     ;  /* TIMER 1 COUNTER                               */
__sfr16 __at (0xCDCC) TMR2     ;  /* TIMER 2 COUNTER                               */
__sfr16 __at (0xCBCA) RCAP2    ;  /* TIMER 2 CAPTURE REGISTER WORD                 */
__sfr16 __at (0xCBCA) TMR2RL   ;  /* TIMER 2 CAPTURE REGISTER WORD                 */
__sfr16 __at (0xFAF9) PCA0     ;  /* PCA COUNTER                                   */
__sfr16 __at (0xFCFB) PCA0CP0  ;  /* PCA CAPTURE 0 WORD                            */
__sfr16 __at (0xEAE9) PCA0CP1  ;  /* PCA CAPTURE 1 WORD                            */
__sfr16 __at (0xECEB) PCA0CP2  ;  /* PCA CAPTURE 2 WORD                            */


/*  BIT Registers  */

/*  P0  0x80 */
__sbit __at (0x80) P0_0        ;
__sbit __at (0x81) P0_1        ;
__sbit __at (0x82) P0_2        ;
__sbit __at (0x83) P0_3        ;
__sbit __at (0x84) P0_4        ;
__sbit __at (0x85) P0_5        ;
__sbit __at (0x86) P0_6        ;
__sbit __at (0x87) P0_7        ;

/*  TCON  0x88 */
__sbit __at (0x88) IT0         ;  /* TCON.0 - EXT. INTERRUPT 0 TYPE                */
__sbit __at (0x89) IE0         ;  /* TCON.1 - EXT. INTERRUPT 0 EDGE FLAG           */
__sbit __at (0x8A) IT1         ;  /* TCON.2 - EXT. INTERRUPT 1 TYPE                */
__sbit __at (0x8B) IE1         ;  /* TCON.3 - EXT. INTERRUPT 1 EDGE FLAG           */
__sbit __at (0x8C) TR0         ;  /* TCON.4 - TIMER 0 ON/OFF CONTROL               */
__sbit __at (0x8D) TF0         ;  /* TCON.5 - TIMER 0 OVERFLOW FLAG                */
__sbit __at (0x8E) TR1         ;  /* TCON.6 - TIMER 1 ON/OFF CONTROL               */
__sbit __at (0x8F) TF1         ;  /* TCON.7 - TIMER 1 OVERFLOW FLAG                */

/*  SCON  0x98 */
__sbit __at (0x98) RI          ;  /* SCON.0 - RECEIVE INTERRUPT FLAG               */
__sbit __at (0x98) RI0         ;  /* SCON.0 - RECEIVE INTERRUPT FLAG               */
__sbit __at (0x99) TI          ;  /* SCON.1 - TRANSMIT INTERRUPT FLAG              */
__sbit __at (0x99) TI0         ;  /* SCON.1 - TRANSMIT INTERRUPT FLAG              */
__sbit __at (0x9A) RB8         ;  /* SCON.2 - RECEIVE BIT 8                        */
__sbit __at (0x9A) RB80        ;  /* SCON.2 - RECEIVE BIT 8                        */
__sbit __at (0x9B) TB8         ;  /* SCON.3 - TRANSMIT BIT 8                       */
__sbit __at (0x9B) TB80        ;  /* SCON.3 - TRANSMIT BIT 8                       */
__sbit __at (0x9C) REN         ;  /* SCON.4 - RECEIVE ENABLE                       */
__sbit __at (0x9C) REN0        ;  /* SCON.4 - RECEIVE ENABLE                       */
__sbit __at (0x9D) SM2         ;  /* SCON.5 - MULTIPROCESSOR COMMUNICATION ENABLE  */
__sbit __at (0x9D) MCE0        ;  /* SCON.5 - MULTIPROCESSOR COMMUNICATION ENABLE  */
__sbit __at (0x9F) SM0         ;  /* SCON.7 - SERIAL MODE CONTROL BIT 0            */
__sbit __at (0x9F) S0MODE      ;  /* SCON.7 - SERIAL MODE CONTROL BIT 0            */

/*  IE  0xA8 */
__sbit __at (0xA8) EX0         ;  /* IE.0 - EXTERNAL INTERRUPT 0 ENABLE            */
__sbit __at (0xA9) ET0         ;  /* IE.1 - TIMER 0 INTERRUPT ENABLE               */
__sbit __at (0xAA) EX1         ;  /* IE.2 - EXTERNAL INTERRUPT 1 ENABLE            */
__sbit __at (0xAB) ET1         ;  /* IE.3 - TIMER 1 INTERRUPT ENABLE               */
__sbit __at (0xAC) ES          ;  /* IE.4 - SERIAL PORT INTERRUPT ENABLE           */
__sbit __at (0xAC) ES0         ;  /* IE.4 - SERIAL PORT INTERRUPT ENABLE           */
__sbit __at (0xAD) ET2         ;  /* IE.5 - TIMER 2 INTERRUPT ENABLE               */
__sbit __at (0xAE) IEGF0       ;  /* IE.6 - GENERAL PURPOSE FLAG 0                 */
__sbit __at (0xAF) EA          ;  /* IE.7 - GLOBAL INTERRUPT ENABLE                */

/*  IP  0xB8 */
__sbit __at (0xB8) PX0         ;  /* IP.0 - EXTERNAL INTERRUPT 0 PRIORITY          */
__sbit __at (0xB9) PT0         ;  /* IP.1 - TIMER 0 PRIORITY                       */
__sbit __at (0xBA) PX1         ;  /* IP.2 - EXTERNAL INTERRUPT 1 PRIORITY          */
__sbit __at (0xBB) PT1         ;  /* IP.3 - TIMER 1 PRIORITY                       */
__sbit __at (0xBC) PS          ;  /* IP.4 - SERIAL PORT PRIORITY                   */
__sbit __at (0xBC) PS0         ;  /* IP.4 - SERIAL PORT PRIORITY                   */
__sbit __at (0xBD) PT2         ;  /* IP.5 - TIMER 2 PRIORITY                       */

/*  SMB0CN  0xC0 */
__sbit __at (0xC0) SI          ;  /* SMB0CN.0 - SMBUS 0 INTERRUPT PENDING FLAG     */
__sbit __at (0xC1) ACK         ;  /* SMB0CN.1 - SMBUS 0 ACKNOWLEDGE FLAG           */
__sbit __at (0xC2) ARBLOST     ;  /* SMB0CN.2 - SMBUS 0 ARBITRATION LOST INDICATOR */
__sbit __at (0xC3) ACKRQ       ;  /* SMB0CN.3 - SMBUS 0 ACKNOWLEDGE REQUEST        */
__sbit __at (0xC4) STO         ;  /* SMB0CN.4 - SMBUS 0 STOP FLAG                  */
__sbit __at (0xC5) STA         ;  /* SMB0CN.5 - SMBUS 0 START FLAG                 */
__sbit __at (0xC6) TXMODE      ;  /* SMB0CN.6 - SMBUS 0 TRANSMIT MODE INDICATOR    */
__sbit __at (0xC7) MASTER      ;  /* SMB0CN.7 - SMBUS 0 MASTER/SLAVE INDICATOR     */

/*  TMR2CN  0xC8 */
__sbit __at (0xC8) T2XCLK      ;  /* TMR2CN.0 - TIMER 2 EXTERNAL CLOCK SELECT      */
__sbit __at (0xCA) TR2         ;  /* TMR2CN.2 - TIMER 2 ON/OFF CONTROL             */
__sbit __at (0xCB) T2SPLIT     ;  /* TMR2CN.3 - TIMER 2 SPLIT MODE ENABLE          */
__sbit __at (0xCD) TF2LEN      ;  /* TMR2CN.5 - TIMER 2 LOW BYTE INTERRUPT ENABLE  */
__sbit __at (0xCE) TF2L        ;  /* TMR2CN.6 - TIMER 2 LOW BYTE OVERFLOW FLAG     */
__sbit __at (0xCF) TF2         ;  /* TMR2CN.7 - TIMER 2 OVERFLOW FLAG              */
__sbit __at (0xCF) TF2H        ;  /* TMR2CN.7 - TIMER 2 HIGH BYTE OVERFLOW FLAG    */

/*  PSW  0xD0 */
__sbit __at (0xD0) PARITY      ;  /* PSW.0 - ACCUMULATOR PARITY FLAG               */
__sbit __at (0xD1) F1          ;  /* PSW.1 - FLAG 1                                */
__sbit __at (0xD2) OV          ;  /* PSW.2 - OVERFLOW FLAG                         */
__sbit __at (0xD3) RS0         ;  /* PSW.3 - REGISTER BANK SELECT 0                */
__sbit __at (0xD4) RS1         ;  /* PSW.4 - REGISTER BANK SELECT 1                */
__sbit __at (0xD5) F0          ;  /* PSW.5 - FLAG 0                                */
__sbit __at (0xD6) AC          ;  /* PSW.6 - AUXILIARY CARRY FLAG                  */
__sbit __at (0xD7) CY          ;  /* PSW.7 - CARRY FLAG                            */

/*  PCA0CN  0xD8 */
__sbit __at (0xD8) CCF0        ;  /* PCA0CN.0 - PCA MODULE 0 CAPTURE/COMPARE FLAG  */
__sbit __at (0xD9) CCF1        ;  /* PCA0CN.1 - PCA MODULE 1 CAPTURE/COMPARE FLAG  */
__sbit __at (0xDA) CCF2        ;  /* PCA0CN.2 - PCA MODULE 2 CAPTURE/COMPARE FLAG  */
__sbit __at (0xDE) CR          ;  /* PCA0CN.6 - PCA COUNTER/TIMER RUN CONTROL      */
__sbit __at (0xDF) CF          ;  /* PCA0CN.7 - PCA COUNTER/TIMER OVERFLOW FLAG    */

/*  ADC0CN  0xE8 */
__sbit __at (0xE8) AD0CM0      ;  /* ADC0CN.0 - ADC 0 START OF CONV. MODE BIT 0    */
__sbit __at (0xE9) AD0CM1      ;  /* ADC0CN.1 - ADC 0 START OF CONV. MODE BIT 1    */
__sbit __at (0xEA) AD0CM2      ;  /* ADC0CN.2 - ADC 0 START OF CONV. MODE BIT 2    */
__sbit __at (0xEB) AD0WINT     ;  /* ADC0CN.3 - ADC 0 WINDOW COMPARE INT. FLAG     */
__sbit __at (0xEC) AD0BUSY     ;  /* ADC0CN.4 - ADC 0 BUSY FLAG                    */
__sbit __at (0xED) AD0INT      ;  /* ADC0CN.5 - ADC 0 CONV. COMPLETE INT. FLAG     */
__sbit __at (0xEE) AD0TM       ;  /* ADC0CN.6 - ADC 0 TRACK MODE                   */
__sbit __at (0xEF) AD0EN       ;  /* ADC0CN.7 - ADC 0 ENABLE                       */

/*  CPT0CN  0xF8 */
__sbit __at (0xF8) CP0HYN0     ;  /* CPT0CN.0 - Comp.0 Neg. Hysteresis Control Bit0*/
__sbit __at (0xF9) CP0HYN1     ;  /* CPT0CN.1 - Comp.0 Neg. Hysteresis Control Bit1*/
__sbit __at (0xFA) CP0HYP0     ;  /* CPT0CN.2 - Comp.0 Pos. Hysteresis Control Bit0*/
__sbit __at (0xFB) CP0HYP1     ;  /* CPT0CN.3 - Comp.0 Pos. Hysteresis Control Bit1*/
__sbit __at (0xFC) CP0FIF      ;  /* CPT0CN.4 - Comparator0 Falling-Edge Int. Flag */
__sbit __at (0xFD) CP0RIF      ;  /* CPT0CN.5 - Comparator0 Rising-Edge Int. Flag  */
__sbit __at (0xFE) CP0OUT      ;  /* CPT0CN.6 - Comparator0 Output State Flag      */
__sbit __at (0xFF) CP0EN       ;  /* CPT0CN.7 - Comparator0 Enable Bit             */


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

#endif
