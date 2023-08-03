/*---------------------------------------------------------------------------
   C8051F018.h - Register Declarations for the Cygnal/SiLabs C8051F018-F019
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

#ifndef C8051F018_H
#define C8051F018_H


/*  BYTE Registers  */
__sfr __at (0x80) P0           ;  /* PORT 0                                                  */
__sfr __at (0x81) SP           ;  /* STACK POINTER                                           */
__sfr __at (0x82) DPL          ;  /* DATA POINTER - LOW BYTE                                 */
__sfr __at (0x83) DPH          ;  /* DATA POINTER - HIGH BYTE                                */
__sfr __at (0x87) PCON         ;  /* POWER CONTROL                                           */
__sfr __at (0x88) TCON         ;  /* TIMER CONTROL                                           */
__sfr __at (0x89) TMOD         ;  /* TIMER MODE                                              */
__sfr __at (0x8A) TL0          ;  /* TIMER 0 - LOW BYTE                                      */
__sfr __at (0x8B) TL1          ;  /* TIMER 1 - LOW BYTE                                      */
__sfr __at (0x8C) TH0          ;  /* TIMER 0 - HIGH BYTE                                     */
__sfr __at (0x8D) TH1          ;  /* TIMER 1 - HIGH BYTE                                     */
__sfr __at (0x8E) CKCON        ;  /* CLOCK CONTROL                                           */
__sfr __at (0x8F) PSCTL        ;  /* PROGRAM STORE R/W CONTROL                               */
__sfr __at (0x90) P1           ;  /* PORT 1                                                  */
__sfr __at (0x91) TMR3CN       ;  /* TIMER 3 CONTROL                                         */
__sfr __at (0x92) TMR3RLL      ;  /* TIMER 3 RELOAD REGISTER - LOW BYTE                      */
__sfr __at (0x93) TMR3RLH      ;  /* TIMER 3 RELOAD REGISTER - HIGH BYTE                     */
__sfr __at (0x94) TMR3L        ;  /* TIMER 3 - LOW BYTE                                      */
__sfr __at (0x95) TMR3H        ;  /* TIMER 3 - HIGH BYTE                                     */
__sfr __at (0x98) SCON         ;  /* SERIAL PORT CONTROL                                     */
__sfr __at (0x99) SBUF         ;  /* SERIAL PORT BUFFER                                      */
__sfr __at (0x9A) SPI0CFG      ;  /* SERIAL PERIPHERAL INTERFACE 0 CONFIGURATION             */
__sfr __at (0x9B) SPI0DAT      ;  /* SERIAL PERIPHERAL INTERFACE 0 DATA                      */
__sfr __at (0x9D) SPI0CKR      ;  /* SERIAL PERIPHERAL INTERFACE 0 CLOCK RATE CONTROL        */
__sfr __at (0x9E) CPT0CN       ;  /* COMPARATOR 0 CONTROL                                    */
__sfr __at (0x9F) CPT1CN       ;  /* COMPARATOR 1 CONTROL                                    */
__sfr __at (0xA0) P2           ;  /* PORT 2                                                  */
__sfr __at (0xA4) PRT0CF       ;  /* PORT 0 CONFIGURATION                                    */
__sfr __at (0xA5) PRT1CF       ;  /* PORT 1 CONFIGURATION                                    */
__sfr __at (0xA6) PRT2CF       ;  /* PORT 2 CONFIGURATION                                    */
__sfr __at (0xA7) PRT3CF       ;  /* PORT 3 CONFIGURATION                                    */
__sfr __at (0xA8) IE           ;  /* INTERRUPT ENABLE                                        */
__sfr __at (0xAD) PRT1IF       ;  /* PORT 1 EXTERNAL INTERRUPT FLAGS                         */
__sfr __at (0xAF) EMI0CN       ;  /* EXTERNAL MEMORY INTERFACE CONTROL                       */
__sfr __at (0xAF) _XPAGE       ;  /* XDATA/PDATA PAGE                                        */
__sfr __at (0xB0) P3           ;  /* PORT 3                                                  */
__sfr __at (0xB1) OSCXCN       ;  /* EXTERNAL OSCILLATOR CONTROL                             */
__sfr __at (0xB2) OSCICN       ;  /* INTERNAL OSCILLATOR CONTROL                             */
__sfr __at (0xB6) FLSCL        ;  /* FLASH MEMORY TIMING PRESCALER                           */
__sfr __at (0xB7) FLACL        ;  /* FLASH ACESS LIMIT                                       */
__sfr __at (0xB8) IP           ;  /* INTERRUPT PRIORITY                                      */
__sfr __at (0xBA) AMX0CF       ;  /* ADC 0 MUX CONFIGURATION                                 */
__sfr __at (0xBB) AMX0SL       ;  /* ADC 0 MUX CHANNEL SELECTION                             */
__sfr __at (0xBC) ADC0CF       ;  /* ADC 0 CONFIGURATION                                     */
__sfr __at (0xBE) ADC0L        ;  /* ADC 0 DATA - LOW BYTE                                   */
__sfr __at (0xBF) ADC0H        ;  /* ADC 0 DATA - HIGH BYTE                                  */
__sfr __at (0xC0) SMB0CN       ;  /* SMBUS 0 CONTROL                                         */
__sfr __at (0xC1) SMB0STA      ;  /* SMBUS 0 STATUS                                          */
__sfr __at (0xC2) SMB0DAT      ;  /* SMBUS 0 DATA                                            */
__sfr __at (0xC3) SMB0ADR      ;  /* SMBUS 0 SLAVE ADDRESS                                   */
__sfr __at (0xC4) ADC0GTL      ;  /* ADC 0 GREATER-THAN REGISTER - LOW BYTE                  */
__sfr __at (0xC5) ADC0GTH      ;  /* ADC 0 GREATER-THAN REGISTER - HIGH BYTE                 */
__sfr __at (0xC6) ADC0LTL      ;  /* ADC 0 LESS-THAN REGISTER - LOW BYTE                     */
__sfr __at (0xC7) ADC0LTH      ;  /* ADC 0 LESS-THAN REGISTER - HIGH BYTE                    */
__sfr __at (0xC8) T2CON        ;  /* TIMER 2 CONTROL                                         */
__sfr __at (0xCA) RCAP2L       ;  /* TIMER 2 CAPTURE REGISTER - LOW BYTE                     */
__sfr __at (0xCB) RCAP2H       ;  /* TIMER 2 CAPTURE REGISTER - HIGH BYTE                    */
__sfr __at (0xCC) TL2          ;  /* TIMER 2 - LOW BYTE                                      */
__sfr __at (0xCD) TH2          ;  /* TIMER 2 - HIGH BYTE                                     */
__sfr __at (0xCF) SMB0CR       ;  /* SMBUS 0 CLOCK RATE                                      */
__sfr __at (0xD0) PSW          ;  /* PROGRAM STATUS WORD                                     */
__sfr __at (0xD1) REF0CN       ;  /* VOLTAGE REFERENCE 0 CONTROL                             */
__sfr __at (0xD8) PCA0CN       ;  /* PCA 0 COUNTER CONTROL                                   */
__sfr __at (0xD9) PCA0MD       ;  /* PCA 0 COUNTER MODE                                      */
__sfr __at (0xDA) PCA0CPM0     ;  /* CONTROL REGISTER FOR PCA 0 MODULE 0                     */
__sfr __at (0xDB) PCA0CPM1     ;  /* CONTROL REGISTER FOR PCA 0 MODULE 1                     */
__sfr __at (0xDC) PCA0CPM2     ;  /* CONTROL REGISTER FOR PCA 0 MODULE 2                     */
__sfr __at (0xDD) PCA0CPM3     ;  /* CONTROL REGISTER FOR PCA 0 MODULE 3                     */
__sfr __at (0xDE) PCA0CPM4     ;  /* CONTROL REGISTER FOR PCA 0 MODULE 4                     */
__sfr __at (0xE0) ACC          ;  /* ACCUMULATOR                                             */
__sfr __at (0xE1) XBR0         ;  /* DIGITAL CROSSBAR CONFIGURATION REGISTER 0               */
__sfr __at (0xE2) XBR1         ;  /* DIGITAL CROSSBAR CONFIGURATION REGISTER 1               */
__sfr __at (0xE3) XBR2         ;  /* DIGITAL CROSSBAR CONFIGURATION REGISTER 2               */
__sfr __at (0xE6) EIE1         ;  /* EXTERNAL INTERRUPT ENABLE 1                             */
__sfr __at (0xE7) EIE2         ;  /* EXTERNAL INTERRUPT ENABLE 2                             */
__sfr __at (0xE8) ADC0CN       ;  /* ADC 0 CONTROL                                           */
__sfr __at (0xE9) PCA0L        ;  /* PCA 0 TIMER - LOW BYTE                                  */
__sfr __at (0xEA) PCA0CPL0     ;  /* CAPTURE/COMPARE REGISTER FOR PCA 0 MODULE 0 - LOW BYTE  */
__sfr __at (0xEB) PCA0CPL1     ;  /* CAPTURE/COMPARE REGISTER FOR PCA 0 MODULE 1 - LOW BYTE  */
__sfr __at (0xEC) PCA0CPL2     ;  /* CAPTURE/COMPARE REGISTER FOR PCA 0 MODULE 2 - LOW BYTE  */
__sfr __at (0xED) PCA0CPL3     ;  /* CAPTURE/COMPARE REGISTER FOR PCA 0 MODULE 3 - LOW BYTE  */
__sfr __at (0xEE) PCA0CPL4     ;  /* CAPTURE/COMPARE REGISTER FOR PCA 0 MODULE 4 - LOW BYTE  */
__sfr __at (0xEF) RSTSRC       ;  /* RESET SOURCE                                            */
__sfr __at (0xF0) B            ;  /* B REGISTER                                              */
__sfr __at (0xF6) EIP1         ;  /* EXTERNAL INTERRUPT PRIORITY REGISTER 1                  */
__sfr __at (0xF7) EIP2         ;  /* EXTERNAL INTERRUPT PRIORITY REGISTER 2                  */
__sfr __at (0xF8) SPI0CN       ;  /* SERIAL PERIPHERAL INTERFACE 0 CONTROL                   */
__sfr __at (0xF9) PCA0H        ;  /* PCA 0 TIMER - HIGH BYTE                                 */
__sfr __at (0xFA) PCA0CPH0     ;  /* CAPTURE/COMPARE REGISTER FOR PCA 0 MODULE 0 - HIGH BYTE */
__sfr __at (0xFB) PCA0CPH1     ;  /* CAPTURE/COMPARE REGISTER FOR PCA 0 MODULE 1 - HIGH BYTE */
__sfr __at (0xFC) PCA0CPH2     ;  /* CAPTURE/COMPARE REGISTER FOR PCA 0 MODULE 2 - HIGH BYTE */
__sfr __at (0xFD) PCA0CPH3     ;  /* CAPTURE/COMPARE REGISTER FOR PCA 0 MODULE 3 - HIGH BYTE */
__sfr __at (0xFE) PCA0CPH4     ;  /* CAPTURE/COMPARE REGISTER FOR PCA 0 MODULE 4 - HIGH BYTE */
__sfr __at (0xFF) WDTCN        ;  /* WATCHDOG TIMER CONTROL                                  */


/*  WORD/DWORD Registers  */

__sfr16 __at (0x8C8A) TMR0     ;  /* TIMER 0 COUNTER                                         */
__sfr16 __at (0x8D8B) TMR1     ;  /* TIMER 1 COUNTER                                         */
__sfr16 __at (0xCDCC) TMR2     ;  /* TIMER 2 COUNTER                                         */
__sfr16 __at (0xCBCA) RCAP2    ;  /* TIMER 2 CAPTURE REGISTER WORD                           */
__sfr16 __at (0x9594) TMR3     ;  /* TIMER 3 COUNTER                                         */
__sfr16 __at (0x9392) TMR3RL   ;  /* TIMER 3 CAPTURE REGISTER WORD                           */
__sfr16 __at (0xBFBE) ADC0     ;  /* ADC 0 DATA WORD                                         */
__sfr16 __at (0xC5C4) ADC0GT   ;  /* ADC 0 GREATER-THAN REGISTER WORD                        */
__sfr16 __at (0xC7C6) ADC0LT   ;  /* ADC 0 LESS-THAN REGISTER WORD                           */
__sfr16 __at (0xF9E9) PCA0     ;  /* PCA COUNTER                                             */
__sfr16 __at (0xFAEA) PCA0CP0  ;  /* PCA CAPTURE 0 WORD                                      */
__sfr16 __at (0xFBEB) PCA0CP1  ;  /* PCA CAPTURE 1 WORD                                      */
__sfr16 __at (0xFCEC) PCA0CP2  ;  /* PCA CAPTURE 2 WORD                                      */
__sfr16 __at (0xFDED) PCA0CP3  ;  /* PCA CAPTURE 3 WORD                                      */
__sfr16 __at (0xFEEE) PCA0CP4  ;  /* PCA CAPTURE 4 WORD                                      */


/*  BIT Registers  */

/*  P0  0x80 */
__sbit __at (0x87) P0_7        ;
__sbit __at (0x86) P0_6        ;
__sbit __at (0x85) P0_5        ;
__sbit __at (0x84) P0_4        ;
__sbit __at (0x83) P0_3        ;
__sbit __at (0x82) P0_2        ;
__sbit __at (0x81) P0_1        ;
__sbit __at (0x80) P0_0        ;

/*  TCON  0x88 */
__sbit __at (0x8F) TF1         ;  /* TIMER 1 OVERFLOW FLAG                                   */
__sbit __at (0x8E) TR1         ;  /* TIMER 1 ON/OFF CONTROL                                  */
__sbit __at (0x8D) TF0         ;  /* TIMER 0 OVERFLOW FLAG                                   */
__sbit __at (0x8C) TR0         ;  /* TIMER 0 ON/OFF CONTROL                                  */
__sbit __at (0x8B) IE1         ;  /* EXT. INTERRUPT 1 EDGE FLAG                              */
__sbit __at (0x8A) IT1         ;  /* EXT. INTERRUPT 1 TYPE                                   */
__sbit __at (0x89) IE0         ;  /* EXT. INTERRUPT 0 EDGE FLAG                              */
__sbit __at (0x88) IT0         ;  /* EXT. INTERRUPT 0 TYPE                                   */

/*  P1  0x90 */
__sbit __at (0x97) P1_7        ;
__sbit __at (0x96) P1_6        ;
__sbit __at (0x95) P1_5        ;
__sbit __at (0x94) P1_4        ;
__sbit __at (0x93) P1_3        ;
__sbit __at (0x92) P1_2        ;
__sbit __at (0x91) P1_1        ;
__sbit __at (0x90) P1_0        ;

/*  SCON  0x98 */
__sbit __at (0x9F) SM0         ;  /* SERIAL MODE CONTROL BIT 0                               */
__sbit __at (0x9E) SM1         ;  /* SERIAL MODE CONTROL BIT 1                               */
__sbit __at (0x9D) SM2         ;  /* MULTIPROCESSOR COMMUNICATION ENABLE                     */
__sbit __at (0x9C) REN         ;  /* RECEIVE ENABLE                                          */
__sbit __at (0x9B) TB8         ;  /* TRANSMIT BIT 8                                          */
__sbit __at (0x9A) RB8         ;  /* RECEIVE BIT 8                                           */
__sbit __at (0x99) TI          ;  /* TRANSMIT INTERRUPT FLAG                                 */
__sbit __at (0x98) RI          ;  /* RECEIVE INTERRUPT FLAG                                  */

/*  P2  0xA0 */
__sbit __at (0xA7) P2_7        ;
__sbit __at (0xA6) P2_6        ;
__sbit __at (0xA5) P2_5        ;
__sbit __at (0xA4) P2_4        ;
__sbit __at (0xA3) P2_3        ;
__sbit __at (0xA2) P2_2        ;
__sbit __at (0xA1) P2_1        ;
__sbit __at (0xA0) P2_0        ;

/*  IE  0xA8 */
__sbit __at (0xAF) EA          ;  /* GLOBAL INTERRUPT ENABLE                                 */
__sbit __at (0xAD) ET2         ;  /* TIMER 2 INTERRUPT ENABLE                                */
__sbit __at (0xAC) ES          ;  /* SERIAL PORT INTERRUPT ENABLE                            */
__sbit __at (0xAB) ET1         ;  /* TIMER 1 INTERRUPT ENABLE                                */
__sbit __at (0xAA) EX1         ;  /* EXTERNAL INTERRUPT 1 ENABLE                             */
__sbit __at (0xA9) ET0         ;  /* TIMER 0 INTERRUPT ENABLE                                */
__sbit __at (0xA8) EX0         ;  /* EXTERNAL INTERRUPT 0 ENABLE                             */

/*  P3  0xB0 */
__sbit __at (0xB7) P3_7        ;
__sbit __at (0xB6) P3_6        ;
__sbit __at (0xB5) P3_5        ;
__sbit __at (0xB4) P3_4        ;
__sbit __at (0xB3) P3_3        ;
__sbit __at (0xB2) P3_2        ;
__sbit __at (0xB1) P3_1        ;
__sbit __at (0xB0) P3_0        ;

/*  IP  0xB8 */
__sbit __at (0xBD) PT2         ;  /* TIMER 2 PRIORITY                                        */
__sbit __at (0xBC) PS          ;  /* SERIAL PORT PRIORITY                                    */
__sbit __at (0xBB) PT1         ;  /* TIMER 1 PRIORITY                                        */
__sbit __at (0xBA) PX1         ;  /* EXTERNAL INTERRUPT 1 PRIORITY                           */
__sbit __at (0xB9) PT0         ;  /* TIMER 0 PRIORITY                                        */
__sbit __at (0xB8) PX0         ;  /* EXTERNAL INTERRUPT 0 PRIORITY                           */

/*  SMB0CN  0xC0 */
__sbit __at (0xC7) BUSY        ;  /* SMBUS 0 BUSY                                            */
__sbit __at (0xC6) ENSMB       ;  /* SMBUS 0 ENABLE                                          */
__sbit __at (0xC5) STA         ;  /* SMBUS 0 START FLAG                                      */
__sbit __at (0xC4) STO         ;  /* SMBUS 0 STOP FLAG                                       */
__sbit __at (0xC3) SI          ;  /* SMBUS 0 INTERRUPT PENDING FLAG                          */
__sbit __at (0xC2) AA          ;  /* SMBUS 0 ASSERT/ACKNOWLEDGE FLAG                         */
__sbit __at (0xC1) SMBFTE      ;  /* SMBUS 0 FREE TIMER ENABLE                               */
__sbit __at (0xC0) SMBTOE      ;  /* SMBUS 0 TIMEOUT ENABLE                                  */

/*  T2CON  0xC8 */
__sbit __at (0xCF) TF2         ;  /* TIMER 2 OVERFLOW FLAG                                   */
__sbit __at (0xCE) EXF2        ;  /* EXTERNAL FLAG                                           */
__sbit __at (0xCD) RCLK        ;  /* RECEIVE CLOCK FLAG                                      */
__sbit __at (0xCC) TCLK        ;  /* TRANSMIT CLOCK FLAG                                     */
__sbit __at (0xCB) EXEN2       ;  /* TIMER 2 EXTERNAL ENABLE FLAG                            */
__sbit __at (0xCA) TR2         ;  /* TIMER 2 ON/OFF CONTROL                                  */
__sbit __at (0xC9) CT2         ;  /* TIMER OR COUNTER SELECT                                 */
__sbit __at (0xC8) CPRL2       ;  /* CAPTURE OR RELOAD SELECT                                */

/*  PSW  0xD0 */
__sbit __at (0xD7) CY          ;  /* CARRY FLAG                                              */
__sbit __at (0xD6) AC          ;  /* AUXILIARY CARRY FLAG                                    */
__sbit __at (0xD5) F0          ;  /* USER FLAG 0                                             */
__sbit __at (0xD4) RS1         ;  /* REGISTER BANK SELECT 1                                  */
__sbit __at (0xD3) RS0         ;  /* REGISTER BANK SELECT 0                                  */
__sbit __at (0xD2) OV          ;  /* OVERFLOW FLAG                                           */
__sbit __at (0xD1) F1          ;  /* USER FLAG 1                                             */
__sbit __at (0xD0) P           ;  /* ACCUMULATOR PARITY FLAG                                 */

/*  PCA0CN  0xD8H */
__sbit __at (0xDF) CF          ;  /* PCA 0 COUNTER OVERFLOW FLAG                             */
__sbit __at (0xDE) CR          ;  /* PCA 0 COUNTER RUN CONTROL BIT                           */
__sbit __at (0xDC) CCF4        ;  /* PCA 0 MODULE 4 INTERRUPT FLAG                           */
__sbit __at (0xDB) CCF3        ;  /* PCA 0 MODULE 3 INTERRUPT FLAG                           */
__sbit __at (0xDA) CCF2        ;  /* PCA 0 MODULE 2 INTERRUPT FLAG                           */
__sbit __at (0xD9) CCF1        ;  /* PCA 0 MODULE 1 INTERRUPT FLAG                           */
__sbit __at (0xD8) CCF0        ;  /* PCA 0 MODULE 0 INTERRUPT FLAG                           */

/*  ADC0CN  0xE8H */
__sbit __at (0xEF) AD0EN       ;  /* ADC 0 ENABLE                                            */
__sbit __at (0xEE) AD0TM       ;  /* ADC 0 TRACK MODE                                        */
__sbit __at (0xED) AD0INT      ;  /* ADC 0 CONVERISION COMPLETE INTERRUPT FLAG               */
__sbit __at (0xEC) AD0BUSY     ;  /* ADC 0 BUSY FLAG                                         */
__sbit __at (0xEB) ADSTM1      ;  /* ADC 0 START OF CONVERSION MODE BIT 1                    */
__sbit __at (0xEA) ADSTM0      ;  /* ADC 0 START OF CONVERSION MODE BIT 0                    */
__sbit __at (0xE9) AD0WINT     ;  /* ADC 0 WINDOW COMPARE INTERRUPT FLAG                     */
__sbit __at (0xE8) ADLJST      ;  /* ADC 0 RIGHT JUSTIFY DATA BIT                            */

/*  SPI0CN  0xF8H */
__sbit __at (0xFF) SPIF        ;  /* SPI 0 INTERRUPT FLAG                                    */
__sbit __at (0xFE) WCOL        ;  /* SPI 0 WRITE COLLISION FLAG                              */
__sbit __at (0xFD) MODF        ;  /* SPI 0 MODE FAULT FLAG                                   */
__sbit __at (0xFC) RXOVRN      ;  /* SPI 0 RX OVERRUN FLAG                                   */
__sbit __at (0xFB) TXBSY       ;  /* SPI 0 TX BUSY FLAG                                      */
__sbit __at (0xFA) SLVSEL      ;  /* SPI 0 SLAVE SELECT                                      */
__sbit __at (0xF9) MSTEN       ;  /* SPI 0 MASTER ENABLE                                     */
__sbit __at (0xF8) SPIEN       ;  /* SPI 0 SPI ENABLE                                        */


/* Predefined SFR Bit Masks */

#define PCON_IDLE         0x01    /* PCON                                */
#define PCON_STOP         0x02    /* PCON                                */
#define TF3               0x80    /* TMR3CN                              */
#define CPFIF             0x10    /* CPTnCN                              */
#define CPRIF             0x20    /* CPTnCN                              */
#define CPOUT             0x40    /* CPTnCN                              */
#define ECCF              0x01    /* PCA0CPMn                            */
#define PWM               0x02    /* PCA0CPMn                            */
#define TOG               0x04    /* PCA0CPMn                            */
#define MAT               0x08    /* PCA0CPMn                            */
#define CAPN              0x10    /* PCA0CPMn                            */
#define CAPP              0x20    /* PCA0CPMn                            */
#define ECOM              0x40    /* PCA0CPMn                            */

#endif
