/*---------------------------------------------------------------------------
   C8051F200.h - Register Declarations for the Cygnal/SiLabs C8051F2xx
   Processor Range

   Copyright (C) 2006, Maarten Brock, sourceforge.brock@dse.nl

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

#ifndef C8051F200_H
#define C8051F200_H


/*  BYTE Registers  */
__sfr __at (0x80) P0           ;  /* PORT 0                                            */
__sfr __at (0x81) SP           ;  /* STACK POINTER                                     */
__sfr __at (0x82) DPL          ;  /* DATA POINTER - LOW BYTE                           */
__sfr __at (0x83) DPH          ;  /* DATA POINTER - HIGH BYTE                          */
__sfr __at (0x87) PCON         ;  /* POWER Control                                     */
__sfr __at (0x88) TCON         ;  /* TIMER Control                                     */
__sfr __at (0x89) TMOD         ;  /* TIMER MODE                                        */
__sfr __at (0x8A) TL0          ;  /* TIMER 0 - LOW BYTE                                */
__sfr __at (0x8B) TL1          ;  /* TIMER 1 - LOW BYTE                                */
__sfr __at (0x8C) TH0          ;  /* TIMER 0 - HIGH BYTE                               */
__sfr __at (0x8D) TH1          ;  /* TIMER 1 - HIGH BYTE                               */
__sfr __at (0x8E) CKCON        ;  /* CLOCK Control                                     */
__sfr __at (0x8F) PSCTL        ;  /* PROGRAM STORE R/W Control                         */
__sfr __at (0x90) P1           ;  /* PORT 1                                            */
__sfr __at (0x98) SCON         ;  /* SERIAL PORT Control                               */
__sfr __at (0x99) SBUF         ;  /* SERIAL PORT BUFFER                                */
__sfr __at (0x9A) SPI0CFG      ;  /* SERIAL PERIPHERAL INTERFACE 0 Configuration       */
__sfr __at (0x9B) SPI0DAT      ;  /* SERIAL PERIPHERAL INTERFACE 0 DATA                */
__sfr __at (0x9D) SPI0CKR      ;  /* SERIAL PERIPHERAL INTERFACE 0 CLOCK RATE Control  */
__sfr __at (0x9E) CPT0CN       ;  /* COMPARATOR 0 Control                              */
__sfr __at (0x9F) CPT1CN       ;  /* COMPARATOR 1 Control                              */
__sfr __at (0xA0) P2           ;  /* PORT 2                                            */
__sfr __at (0xA4) PRT0CF       ;  /* PORT 0 OUTPUT MODE Configuration                  */
__sfr __at (0xA5) PRT1CF       ;  /* PORT 1 OUTPUT MODE Configuration                  */
__sfr __at (0xA6) PRT2CF       ;  /* PORT 2 OUTPUT MODE Configuration                  */
__sfr __at (0xA7) PRT3CF       ;  /* PORT 3 OUTPUT MODE Configuration                  */
__sfr __at (0xA8) IE           ;  /* Interrupt Enable                                  */
__sfr __at (0xAD) SWCINT       ;  /* SOFTWARE-Controlled Interrupt FLAGS               */
__sfr __at (0xAD) PRT1IF       ;  /* SOFTWARE-Controlled Interrupt FLAGS (LEGACY NAME) */
__sfr __at (0xAF) EMI0CN       ;  /* EXTERNAL MEMORY INTERFACE Control (F206/F226/F236)*/
__sfr __at (0xAF) _XPAGE       ;  /* XDATA/PDATA PAGE                                  */
__sfr __at (0xB0) P3           ;  /* PORT 3                                            */
__sfr __at (0xB1) OSCXCN       ;  /* EXTERNAL OSCILLATOR Control                       */
__sfr __at (0xB2) OSCICN       ;  /* INTERNAL OSCILLATOR Control                       */
__sfr __at (0xB6) FLSCL        ;  /* FLASH MEMORY TIMING PRESCALER                     */
__sfr __at (0xB7) FLACL        ;  /* FLASH ACESS LIMIT                                 */
__sfr __at (0xB8) IP           ;  /* Interrupt Priority                                */
__sfr __at (0xBB) AMX0SL       ;  /* ADC 0 MUX CHANNEL SELECTION    (Not on F230/1/6)  */
__sfr __at (0xBC) ADC0CF       ;  /* ADC 0 Configuration            (Not on F230/1/6)  */
__sfr __at (0xBE) ADC0L        ;  /* ADC 0 Data LOW                 ( F206 only )      */
__sfr __at (0xBF) ADC0H        ;  /* ADC 0 Data High                                   */
__sfr __at (0xC4) ADC0GTL      ;  /* ADC 0 GREATER-THAN Register LOW( F206 only )      */
__sfr __at (0xC5) ADC0GTH      ;  /* ADC 0 GREATER-THAN Register    (Not on F230/1/6)  */
__sfr __at (0xC6) ADC0LTL      ;  /* ADC 0 LESS-THAN Register LOW   ( F206 only )      */
__sfr __at (0xC7) ADC0LTH      ;  /* ADC 0 LESS-THAN Register       (Not on F230/1/6)  */
__sfr __at (0xC8) T2CON        ;  /* TIMER 2 Control                                   */
__sfr __at (0xCA) RCAP2L       ;  /* TIMER 2 CAPTURE Register - LOW BYTE               */
__sfr __at (0xCB) RCAP2H       ;  /* TIMER 2 CAPTURE Register - HIGH BYTE              */
__sfr __at (0xCC) TL2          ;  /* TIMER 2 - LOW BYTE                                */
__sfr __at (0xCD) TH2          ;  /* TIMER 2 - HIGH BYTE                               */
__sfr __at (0xD0) PSW          ;  /* PROGRAM STATUS WORD                               */
__sfr __at (0xD1) REF0CN       ;  /* VOLTAGE REFERENCE 0 Control                       */
__sfr __at (0xE0) ACC          ;  /* ACCUMULATOR                                       */
__sfr __at (0xE1) PRT0MX       ;  /* PORT MUX Configuration Register 0                 */
__sfr __at (0xE2) PRT1MX       ;  /* PORT MUX Configuration Register 1                 */
__sfr __at (0xE3) PRT2MX       ;  /* PORT MUX Configuration Register 2                 */
__sfr __at (0xE6) EIE1         ;  /* EXTERNAL Interrupt Enable 1                       */
__sfr __at (0xE7) EIE2         ;  /* EXTERNAL Interrupt Enable 2                       */
__sfr __at (0xE8) ADC0CN       ;  /* ADC 0 Control                   (Not on F230/1/6) */
__sfr __at (0xEF) RSTSRC       ;  /* RESET SOURCE                                      */
__sfr __at (0xF0) B            ;  /* B Register                                        */
__sfr __at (0xF1) P0MODE       ;  /* PORT 0 INPUT MODE Configuration                   */
__sfr __at (0xF2) P1MODE       ;  /* PORT 1 INPUT MODE Configuration                   */
__sfr __at (0xF3) P2MODE       ;  /* PORT 2 INPUT MODE Configuration                   */
__sfr __at (0xF4) P3MODE       ;  /* PORT 3 INPUT MODE Configuration (Not on F221/F231)*/
__sfr __at (0xF6) EIP1         ;  /* EXTERNAL Interrupt Priority Register 1            */
__sfr __at (0xF7) EIP2         ;  /* EXTERNAL Interrupt Priority Register 2            */
__sfr __at (0xF8) SPI0CN       ;  /* SERIAL PERIPHERAL INTERFACE 0 Control             */
__sfr __at (0xFF) WDTCN        ;  /* WATCHDOG TIMER Control                            */


/*  WORD/DWORD Registers  */

__sfr16 __at (0x8C8A) TMR0     ;  /* TIMER 0 COUNTER                                   */
__sfr16 __at (0x8D8B) TMR1     ;  /* TIMER 1 COUNTER                                   */
__sfr16 __at (0xCDCC) TMR2     ;  /* TIMER 2 COUNTER                                   */
__sfr16 __at (0xCBCA) RCAP2    ;  /* TIMER 2 CAPTURE REGISTER WORD                     */
__sfr16 __at (0xBFBE) ADC0     ;  /* ADC 0 DATA WORD                                   */
__sfr16 __at (0xC5C4) ADC0GT   ;  /* ADC 0 GREATER-THAN REGISTER WORD                  */
__sfr16 __at (0xC7C6) ADC0LT   ;  /* ADC 0 LESS-THAN REGISTER WORD                     */


/*  BIT Registers  */

/*  P0  0x80 */
__sbit __at (0x80) P0_0        ;
__sbit __at (0x81) P0_1        ;
__sbit __at (0x82) P0_2        ;
__sbit __at (0x83) P0_3        ;
__sbit __at (0x84) P0_4        ; /* Port0 I/O Bits */
__sbit __at (0x85) P0_5        ;
__sbit __at (0x86) P0_6        ;
__sbit __at (0x87) P0_7        ;

/*  TCON  0x88 */
__sbit __at (0x88) IT0         ; /* EXT. Interrupt 0 TYPE                              */
__sbit __at (0x89) IE0         ; /* EXT. Interrupt 0 EDGE FLAG                         */
__sbit __at (0x8A) IT1         ; /* EXT. Interrupt 1 TYPE                              */
__sbit __at (0x8B) IE1         ; /* EXT. Interrupt 1 EDGE FLAG                         */
__sbit __at (0x8C) TR0         ; /* TIMER 0 ON/OFF Control                             */
__sbit __at (0x8D) TF0         ; /* TIMER 0 Overflow FLAG                              */
__sbit __at (0x8E) TR1         ; /* TIMER 1 ON/OFF Control                             */
__sbit __at (0x8F) TF1         ; /* TIMER 1 Overflow FLAG                              */

/*  P1  0x90 */
__sbit __at (0x90) P1_0        ;
__sbit __at (0x91) P1_1        ;
__sbit __at (0x92) P1_2        ;
__sbit __at (0x93) P1_3        ;
__sbit __at (0x94) P1_4        ; /* Port1 I/O Bits */
__sbit __at (0x95) P1_5        ;
__sbit __at (0x96) P1_6        ;
__sbit __at (0x97) P1_7        ;

/*  SCON  0x98 */
__sbit __at (0x98) RI          ; /* RECEIVE Interrupt FLAG                             */
__sbit __at (0x99) TI          ; /* TRANSMIT Interrupt FLAG                            */
__sbit __at (0x9A) RB8         ; /* RECEIVE BIT 8                                      */
__sbit __at (0x9B) TB8         ; /* TRANSMIT BIT 8                                     */
__sbit __at (0x9C) REN         ; /* RECEIVE Enable                                     */
__sbit __at (0x9D) SM2         ; /* MULTIPROCESSOR COMMUNICATION Enable                */
__sbit __at (0x9E) SM1         ; /* SERIAL MODE Control BIT 1                          */
__sbit __at (0x9F) SM0         ; /* SERIAL MODE Control BIT 0                          */

/*  P2  0xA0 */
__sbit __at (0xA0) P2_0        ;
__sbit __at (0xA1) P2_1        ;
__sbit __at (0xA2) P2_2        ;
__sbit __at (0xA3) P2_3        ;
__sbit __at (0xA4) P2_4        ; /* Port2 I/O Bits */
__sbit __at (0xA5) P2_5        ;
__sbit __at (0xA6) P2_6        ;
__sbit __at (0xA7) P2_7        ;

/*  IE  0xA8 */
__sbit __at (0xA8) EX0         ; /* EXTERNAL Interrupt 0 Enable                        */
__sbit __at (0xA9) ET0         ; /* TIMER 0 Interrupt Enable                           */
__sbit __at (0xAA) EX1         ; /* EXTERNAL Interrupt 1 Enable                        */
__sbit __at (0xAB) ET1         ; /* TIMER 1 Interrupt Enable                           */
__sbit __at (0xAC) ES          ; /* SERIAL PORT Interrupt Enable                       */
__sbit __at (0xAD) ET2         ; /* TIMER 2 Interrupt Enable                           */
//-------------                  /* Bit 6 not used                                     */
__sbit __at (0xAF) EA          ; /* GLOBAL Interrupt Enable                            */

/*  P2  0xB0 */
__sbit __at (0xB0) P3_0        ;
__sbit __at (0xB1) P3_1        ;
__sbit __at (0xB2) P3_2        ;
__sbit __at (0xB3) P3_3        ;
__sbit __at (0xB4) P3_4        ; /* Port3 I/O Bits */
__sbit __at (0xB5) P3_5        ;
__sbit __at (0xB6) P3_6        ;
__sbit __at (0xB7) P3_7        ;

/*  IP  0xB8 */
__sbit __at (0xB8) PX0         ; /* EXTERNAL Interrupt 0 Priority                      */
__sbit __at (0xB9) PT0         ; /* TIMER 0 Priority                                   */
__sbit __at (0xBA) PX1         ; /* EXTERNAL Interrupt 1 Priority                      */
__sbit __at (0xBB) PT1         ; /* TIMER 1 Priority                                   */
__sbit __at (0xBC) PS          ; /* SERIAL PORT Priority                               */
__sbit __at (0xBD) PT2         ; /* TIMER 2 Priority                                   */
//-------------                  /* Bit 6 not used                                     */
//-------------                  /* Bit 7 not used                                     */

/*  T2CON  0xC8 */
__sbit __at (0xC8) CPRL2       ; /* CAPTURE OR RELOAD SELECT                           */
__sbit __at (0xC9) CT2         ; /* TIMER OR COUNTER SELECT                            */
__sbit __at (0xCA) TR2         ; /* TIMER 2 ON/OFF Control                             */
__sbit __at (0xCB) EXEN2       ; /* TIMER 2 EXTERNAL Enable FLAG                       */
__sbit __at (0xCC) TCLK        ; /* TRANSMIT CLOCK FLAG                                */
__sbit __at (0xCD) RCLK        ; /* RECEIVE CLOCK FLAG                                 */
__sbit __at (0xCE) EXF2        ; /* EXTERNAL FLAG                                      */
__sbit __at (0xCF) TF2         ; /* TIMER 2 Overflow FLAG                              */

/*  PSW  0xD0 */
__sbit __at (0xD0) P           ; /* ACCUMULATOR PARITY FLAG                            */
__sbit __at (0xD1) F1          ; /* USER FLAG 1                                        */
__sbit __at (0xD2) OV          ; /* Overflow FLAG                                      */
__sbit __at (0xD3) RS0         ; /* Register BANK SELECT 0                             */
__sbit __at (0xD4) RS1         ; /* Register BANK SELECT 1                             */
__sbit __at (0xD5) F0          ; /* USER FLAG 0                                        */
__sbit __at (0xD6) AC          ; /* AUXILIARY CARRY FLAG                               */
__sbit __at (0xD7) CY          ; /* CARRY FLAG                                         */

/* ADC0CN 0xE8 */
__sbit __at (0xE8) ADLJST      ; /* Left Justify Data (F206 only)                      */
__sbit __at (0xE9) ADWINT      ; /* WINDOW COMPARE Interrupt FLAG                      */
__sbit __at (0xEA) ADSTM0      ; /* START OF CONVERSION MODE BIT 0                     */
__sbit __at (0xEB) ADSTM1      ; /* START OF CONVERSION MODE BIT 1                     */
__sbit __at (0xEC) ADBUSY      ; /* BUSY FLAG                                          */
__sbit __at (0xED) ADCINT      ; /* CONVERISION COMPLETE Interrupt FLAG                */
__sbit __at (0xEE) ADCTM       ; /* TRACK MODE                                         */
__sbit __at (0xEF) ADCEN       ; /* Enable                                             */

/* SPI0CN 0xF8 */
__sbit __at (0xF8) SPIEN       ; /* SPI Enable                                         */
__sbit __at (0xF9) MSTEN       ; /* MASTER Enable                                      */
__sbit __at (0xFA) SLVSEL      ; /* SLAVE SELECT                                       */
__sbit __at (0xFB) TXBSY       ; /* TX BUSY FLAG                                       */
__sbit __at (0xFC) RXOVRN      ; /* RX OVERRUN FLAG                                    */
__sbit __at (0xFD) MODF        ; /* MODE FAULT FLAG                                    */
__sbit __at (0xFE) WCOL        ; /* WRITE COLLISION FLAG                               */
__sbit __at (0xFF) SPIF        ; /* Interrupt FLAG                                     */

#endif
