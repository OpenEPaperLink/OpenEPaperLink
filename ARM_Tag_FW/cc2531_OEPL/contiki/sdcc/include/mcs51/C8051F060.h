/*-------------------------------------------------------------------------
   C8051F060.h - Register Declarations for the Cygnal/SiLabs C8051F06x
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

#ifndef C8051F060_H
#define C8051F060_H


/*  BYTE Registers  */

/*  All Pages */
__sfr __at 0x80 P0       ;  /* PORT 0                                        */
__sfr __at 0x81 SP       ;  /* STACK POINTER                                 */
__sfr __at 0x82 DPL      ;  /* DATA POINTER - LOW BYTE                       */
__sfr __at 0x83 DPH      ;  /* DATA POINTER - HIGH BYTE                      */
__sfr __at 0x84 SFRPAGE  ;  /* SFR PAGE SELECT                               */
__sfr __at 0x85 SFRNEXT  ;  /* SFR STACK NEXT PAGE                           */
__sfr __at 0x86 SFRLAST  ;  /* SFR STACK LAST PAGE                           */
__sfr __at 0x87 PCON     ;  /* POWER CONTROL                                 */
__sfr __at 0x90 P1       ;  /* PORT 1                                        */
__sfr __at 0xA0 P2       ;  /* PORT 2                                        */
__sfr __at 0xA8 IE       ;  /* INTERRUPT ENABLE                              */
__sfr __at 0xB0 P3       ;  /* PORT 3                                        */
__sfr __at 0xB8 IP       ;  /* INTERRUPT PRIORITY                            */
__sfr __at 0xD0 PSW      ;  /* PROGRAM STATUS WORD                           */
__sfr __at 0xE0 ACC      ;  /* ACCUMULATOR                                   */
__sfr __at 0xE6 EIE1     ;  /* EXTERNAL INTERRUPT ENABLE 1                   */
__sfr __at 0xE7 EIE2     ;  /* EXTERNAL INTERRUPT ENABLE 2                   */
__sfr __at 0xF0 B        ;  /* B REGISTER                                    */
__sfr __at 0xF6 EIP1     ;  /* EXTERNAL INTERRUPT PRIORITY REGISTER 1        */
__sfr __at 0xF7 EIP2     ;  /* EXTERNAL INTERRUPT PRIORITY REGISTER 2        */
__sfr __at 0xFF WDTCN    ;  /* WATCHDOG TIMER CONTROL                        */

/*  Page 0x00 */
__sfr __at 0x88 TCON     ;  /* TIMER CONTROL                                 */
__sfr __at 0x89 TMOD     ;  /* TIMER MODE                                    */
__sfr __at 0x8A TL0      ;  /* TIMER 0 - LOW BYTE                            */
__sfr __at 0x8B TL1      ;  /* TIMER 1 - LOW BYTE                            */
__sfr __at 0x8C TH0      ;  /* TIMER 0 - HIGH BYTE                           */
__sfr __at 0x8D TH1      ;  /* TIMER 1 - HIGH BYTE                           */
__sfr __at 0x8E CKCON    ;  /* TIMER 0/1 CLOCK CONTROL                       */
__sfr __at 0x8F PSCTL    ;  /* FLASH WRITE/ERASE CONTROL                     */
__sfr __at 0x91 SSTA0    ;  /* UART 0 STATUS                                 */
__sfr __at 0x98 SCON0    ;  /* UART 0 CONTROL                                */
__sfr __at 0x98 SCON     ;  /* UART 0 CONTROL                                */
__sfr __at 0x99 SBUF0    ;  /* UART 0 BUFFER                                 */
__sfr __at 0x99 SBUF     ;  /* UART 0 BUFFER                                 */
__sfr __at 0x9A SPI0CFG  ;  /* SPI 0 CONFIGURATION                           */
__sfr __at 0x9B SPI0DAT  ;  /* SPI 0 DATA                                    */
__sfr __at 0x9D SPI0CKR  ;  /* SPI 0 CLOCK RATE CONTROL                      */
__sfr __at 0xA1 EMI0TC   ;  /* EMIF TIMING CONTROL                           */
__sfr __at 0xA2 EMI0CN   ;  /* EMIF CONTROL                                  */
__sfr __at 0xA2 _XPAGE   ;  /* XDATA/PDATA PAGE                              */
__sfr __at 0xA3 EMI0CF   ;  /* EMIF CONFIGURATION                            */
__sfr __at 0xA9 SADDR0   ;  /* UART 0 SLAVE ADDRESS                          */
__sfr __at 0xB7 FLSCL    ;  /* FLASH SCALE                                   */
__sfr __at 0xB9 SADEN0   ;  /* UART 0 SLAVE ADDRESS MASK                     */
__sfr __at 0xBB AMX0SL   ;  /* ADC 0 MUX CHANNEL SELECTION                   */
__sfr __at 0xBC ADC0CF   ;  /* ADC 0 CONFIGURATION                           */
__sfr __at 0xBE ADC0L    ;  /* ADC 0 DATA - LOW BYTE                         */
__sfr __at 0xBF ADC0H    ;  /* ADC 0 DATA - HIGH BYTE                        */
__sfr __at 0xC0 SMB0CN   ;  /* SMBUS 0 CONTROL                               */
__sfr __at 0xC1 SMB0STA  ;  /* SMBUS 0 STATUS                                */
__sfr __at 0xC2 SMB0DAT  ;  /* SMBUS 0 DATA                                  */
__sfr __at 0xC3 SMB0ADR  ;  /* SMBUS 0 SLAVE ADDRESS                         */
__sfr __at 0xC4 ADC0GTL  ;  /* ADC 0 GREATER-THAN REGISTER - LOW BYTE        */
__sfr __at 0xC5 ADC0GTH  ;  /* ADC 0 GREATER-THAN REGISTER - HIGH BYTE       */
__sfr __at 0xC6 ADC0LTL  ;  /* ADC 0 LESS-THAN REGISTER - LOW BYTE           */
__sfr __at 0xC7 ADC0LTH  ;  /* ADC 0 LESS-THAN REGISTER - HIGH BYTE          */
__sfr __at 0xC8 TMR2CN   ;  /* TIMER 2 CONTROL                               */
__sfr __at 0xC9 TMR2CF   ;  /* TIMER 2 CONFIGURATION                         */
__sfr __at 0xCA RCAP2L   ;  /* TIMER 2 CAPTURE REGISTER - LOW BYTE           */
__sfr __at 0xCB RCAP2H   ;  /* TIMER 2 CAPTURE REGISTER - HIGH BYTE          */
__sfr __at 0xCC TMR2L    ;  /* TIMER 2 - LOW BYTE                            */
__sfr __at 0xCC TL2      ;  /* TIMER 2 - LOW BYTE                            */
__sfr __at 0xCD TMR2H    ;  /* TIMER 2 - HIGH BYTE                           */
__sfr __at 0xCD TH2      ;  /* TIMER 2 - HIGH BYTE                           */
__sfr __at 0xCF SMB0CR   ;  /* SMBUS 0 CLOCK RATE                            */
__sfr __at 0xD1 REF0CN   ;  /* VOLTAGE REFERENCE 0 CONTROL                   */
__sfr __at 0xD2 DAC0L    ;  /* DAC 0 REGISTER - LOW BYTE                     */
__sfr __at 0xD3 DAC0H    ;  /* DAC 0 REGISTER - HIGH BYTE                    */
__sfr __at 0xD4 DAC0CN   ;  /* DAC 0 CONTROL                                 */
__sfr __at 0xD8 PCA0CN   ;  /* PCA 0 COUNTER CONTROL                         */
__sfr __at 0xD9 PCA0MD   ;  /* PCA 0 COUNTER MODE                            */
__sfr __at 0xDA PCA0CPM0 ;  /* PCA 0 MODULE 0 CONTROL                        */
__sfr __at 0xDB PCA0CPM1 ;  /* PCA 0 MODULE 1 CONTROL                        */
__sfr __at 0xDC PCA0CPM2 ;  /* PCA 0 MODULE 2 CONTROL                        */
__sfr __at 0xDD PCA0CPM3 ;  /* PCA 0 MODULE 3 CONTROL                        */
__sfr __at 0xDE PCA0CPM4 ;  /* PCA 0 MODULE 4 CONTROL                        */
__sfr __at 0xDF PCA0CPM5 ;  /* PCA 0 MODULE 5 CONTROL                        */
__sfr __at 0xE1 PCA0CPL5 ;  /* PCA 0 MODULE 5 CAPTURE/COMPARE - LOW BYTE     */
__sfr __at 0xE2 PCA0CPH5 ;  /* PCA 0 MODULE 5 CAPTURE/COMPARE - HIGH BYTE    */
__sfr __at 0xE8 ADC0CN   ;  /* ADC 0 CONTROL                                 */
__sfr __at 0xE9 PCA0CPL2 ;  /* PCA 0 MODULE 2 CAPTURE/COMPARE - LOW BYTE     */
__sfr __at 0xEA PCA0CPH2 ;  /* PCA 0 MODULE 2 CAPTURE/COMPARE - HIGH BYTE    */
__sfr __at 0xEB PCA0CPL3 ;  /* PCA 0 MODULE 3 CAPTURE/COMPARE - LOW BYTE     */
__sfr __at 0xEC PCA0CPH3 ;  /* PCA 0 MODULE 3 CAPTURE/COMPARE - HIGH BYTE    */
__sfr __at 0xED PCA0CPL4 ;  /* PCA 0 MODULE 4 CAPTURE/COMPARE - LOW BYTE     */
__sfr __at 0xEE PCA0CPH4 ;  /* PCA 0 MODULE 4 CAPTURE/COMPARE - HIGH BYTE    */
__sfr __at 0xEF RSTSRC   ;  /* RESET SOURCE                                  */
__sfr __at 0xF8 SPI0CN   ;  /* SPI 0 CONTROL                                 */
__sfr __at 0xF9 PCA0L    ;  /* PCA 0 TIMER - LOW BYTE                        */
__sfr __at 0xFA PCA0H    ;  /* PCA 0 TIMER - HIGH BYTE                       */
__sfr __at 0xFB PCA0CPL0 ;  /* PCA 0 MODULE 0 CAPTURE/COMPARE - LOW BYTE     */
__sfr __at 0xFC PCA0CPH0 ;  /* PCA 0 MODULE 0 CAPTURE/COMPARE - HIGH BYTE    */
__sfr __at 0xFD PCA0CPL1 ;  /* PCA 0 MODULE 1 CAPTURE/COMPARE - LOW BYTE     */
__sfr __at 0xFE PCA0CPH1 ;  /* PCA 0 MODULE 1 CAPTURE/COMPARE - HIGH BYTE    */

/*  Page 0x01 */
__sfr __at 0x88 CPT0CN   ;  /* COMPARATOR 0 CONTROL                          */
__sfr __at 0x89 CPT0MD   ;  /* COMPARATOR 0 CONFIGURATION                    */
__sfr __at 0x98 SCON1    ;  /* UART 1 CONTROL                                */
__sfr __at 0x99 SBUF1    ;  /* UART 1 BUFFER                                 */
__sfr __at 0xBC ADC1CF   ;  /* ADC 1 CONFIGURATION                           */
__sfr __at 0xBE ADC1L    ;  /* ADC 1 DATA - LOW BYTE                         */
__sfr __at 0xBF ADC1H    ;  /* ADC 1 DATA - HIGH BYTE                        */
__sfr __at 0xC0 CAN0STA  ;  /* CAN 0 STATUS                                  */
__sfr __at 0xC8 TMR3CN   ;  /* TIMER 3 CONTROL                               */
__sfr __at 0xC9 TMR3CF   ;  /* TIMER 3 CONFIGURATION                         */
__sfr __at 0xCA RCAP3L   ;  /* TIMER 3 CAPTURE REGISTER - LOW BYTE           */
__sfr __at 0xCB RCAP3H   ;  /* TIMER 3 CAPTURE REGISTER - HIGH BYTE          */
__sfr __at 0xCC TMR3L    ;  /* TIMER 3 - LOW BYTE                            */
__sfr __at 0xCD TMR3H    ;  /* TIMER 3 - HIGH BYTE                           */
__sfr __at 0xD1 REF1CN   ;  /* VOLTAGE REFERENCE 1 CONTROL                   */
__sfr __at 0xD2 DAC1L    ;  /* DAC 1 REGISTER - LOW BYTE                     */
__sfr __at 0xD3 DAC1H    ;  /* DAC 1 REGISTER - HIGH BYTE                    */
__sfr __at 0xD4 DAC1CN   ;  /* DAC 1 CONTROL                                 */
__sfr __at 0xD8 CAN0DATL ;  /* CAN 0 DATA REGISTER LOW                       */
__sfr __at 0xD9 CAN0DATH ;  /* CAN 0 DATA REGISTER HIGH                      */
__sfr __at 0xDA CAN0ADR  ;  /* CAN 0 ADDRESS                                 */
__sfr __at 0xDB CAN0TST  ;  /* CAN 0 TEST REGISTER                           */
__sfr __at 0xE8 ADC1CN   ;  /* ADC 1 CONTROL                                 */
__sfr __at 0xF8 CAN0CN   ;  /* CAN 0 CONTROL                                 */

/*  Page 0x02 */
__sfr __at 0x88 CPT1CN   ;  /* COMPARATOR 1 CONTROL                          */
__sfr __at 0x89 CPT1MD   ;  /* COMPARATOR 1 CONFIGURATION                    */
__sfr __at 0xBA AMX2CF   ;  /* ADC 2 MUX CONFIGURATION                       */
__sfr __at 0xBB AMX2SL   ;  /* ADC 2 MUX CHANNEL SELECTION                   */
__sfr __at 0xBC ADC2CF   ;  /* ADC 2 CONFIGURATION                           */
__sfr __at 0xBE ADC2L    ;  /* ADC 2 DATA - LOW BYTE                         */
__sfr __at 0xBF ADC2H    ;  /* ADC 2 DATA - HIGH BYTE                        */
__sfr __at 0xC4 ADC2GTL  ;  /* ADC 2 GREATER-THAN REGISTER - LOW BYTE        */
__sfr __at 0xC5 ADC2GTH  ;  /* ADC 2 GREATER-THAN REGISTER - HIGH BYTE       */
__sfr __at 0xC6 ADC2LTL  ;  /* ADC 2 LESS-THAN REGISTER - LOW BYTE           */
__sfr __at 0xC7 ADC2LTH  ;  /* ADC 2 LESS-THAN REGISTER - HIGH BYTE          */
__sfr __at 0xC8 TMR4CN   ;  /* TIMER 4 CONTROL                               */
__sfr __at 0xC9 TMR4CF   ;  /* TIMER 4 CONFIGURATION                         */
__sfr __at 0xCA RCAP4L   ;  /* TIMER 4 CAPTURE REGISTER - LOW BYTE           */
__sfr __at 0xCB RCAP4H   ;  /* TIMER 4 CAPTURE REGISTER - HIGH BYTE          */
__sfr __at 0xCC TMR4L    ;  /* TIMER 4 - LOW BYTE                            */
__sfr __at 0xCD TMR4H    ;  /* TIMER 4 - HIGH BYTE                           */
__sfr __at 0xD1 REF2CN   ;  /* VOLTAGE REFERENCE 2 CONTROL                   */
__sfr __at 0xE8 ADC2CN   ;  /* ADC 2 CONTROL                                 */

/*  Page 0x03 */
__sfr __at 0x88 CPT2CN   ;  /* COMPARATOR 2 CONTROL                          */
__sfr __at 0x89 CPT2MD   ;  /* COMPARATOR 2 CONFIGURATION                    */
__sfr __at 0xD8 DMA0CN   ;  /* DMA0 CONTROL                                  */
__sfr __at 0xD9 DMA0DAL  ;  /* DMA0 DATA ADDRESS BEGINNING LOW BYTE          */
__sfr __at 0xDA DMA0DAH  ;  /* DMA0 DATA ADDRESS BEGINNING HIGH BYTE         */
__sfr __at 0xDB DMA0DSL  ;  /* DMA0 DATA ADDRESS POINTER LOW BYTE            */
__sfr __at 0xDC DMA0DSH  ;  /* DMA0 DATA ADDRESS POINTER HIGH BYTE           */
__sfr __at 0xDD DMA0IPT  ;  /* DMA0 INSTRUCTION WRITE ADDRESS                */
__sfr __at 0xDE DMA0IDT  ;  /* DMA0 INSTRUCTION WRITE DATA                   */
__sfr __at 0xF8 DMA0CF   ;  /* DMA0 CONFIGURATION                            */
__sfr __at 0xF9 DMA0CTL  ;  /* DMA0 REPEAT COUNTER LIMIT LOW BYTE            */
__sfr __at 0xFA DMA0CTH  ;  /* DMA0 REPEAT COUNTER LIMIT HIGH BYTE           */
__sfr __at 0xFB DMA0CSL  ;  /* DMA0 REPEAT COUNTER STATUS LOW BYTE           */
__sfr __at 0xFC DMA0CSH  ;  /* DMA0 REPEAT COUNTER STATUS HIGH BYTE          */
__sfr __at 0xFD DMA0BND  ;  /* DMA0 INSTRUCTION BOUNDARY                     */
__sfr __at 0xFE DMA0ISW  ;  /* DMA0 INSTRUCTION STATUS                       */

/*  Page 0x0F */
__sfr __at 0x8A OSCICN   ;  /* INTERNAL OSCILLATOR CONTROL                   */
__sfr __at 0x8B OSCICL   ;  /* INTERNAL OSCILLATOR CALIBRATION               */
__sfr __at 0x8C OSCXCN   ;  /* EXTERNAL OSCILLATOR CONTROL                   */
__sfr __at 0x96 SFRPGCN  ;  /* SFR PAGE CONTROL                              */
__sfr __at 0x97 CLKSEL   ;  /* SYSTEM CLOCK SELECT                           */
__sfr __at 0x9C P4MDOUT  ;  /* PORT 4 OUTPUT MODE                            */
__sfr __at 0x9D P5MDOUT  ;  /* PORT 5 OUTPUT MODE                            */
__sfr __at 0x9E P6MDOUT  ;  /* PORT 6 OUTPUT MODE                            */
__sfr __at 0x9F P7MDOUT  ;  /* PORT 7 OUTPUT MODE                            */
__sfr __at 0xA4 P0MDOUT  ;  /* PORT 0 OUTPUT MODE                            */
__sfr __at 0xA5 P1MDOUT  ;  /* PORT 1 OUTPUT MODE                            */
__sfr __at 0xA6 P2MDOUT  ;  /* PORT 2 OUTPUT MODE CONFIGURATION              */
__sfr __at 0xA7 P3MDOUT  ;  /* PORT 3 OUTPUT MODE CONFIGURATION              */
__sfr __at 0xAD P1MDIN   ;  /* PORT 1 INPUT MODE                             */
__sfr __at 0xAE P2MDIN   ;  /* PORT 2 INPUT MODE                             */
__sfr __at 0xB7 FLACL    ;  /* FLASH ACCESS LIMIT                            */
__sfr __at 0xBA ADC0CPT  ;  /* ADC0 CALIBRATION POINTER                      */
__sfr __at 0xBB ADC0CCF  ;  /* ADC0 CALIBRATION COEFFICIENT                  */
__sfr __at 0xC8 P4       ;  /* PORT 4                                        */
__sfr __at 0xD8 P5       ;  /* PORT 5                                        */
__sfr __at 0xE1 XBR0     ;  /* CROSSBAR CONFIGURATION REGISTER 0             */
__sfr __at 0xE2 XBR1     ;  /* CROSSBAR CONFIGURATION REGISTER 1             */
__sfr __at 0xE3 XBR2     ;  /* CROSSBAR CONFIGURATION REGISTER 2             */
__sfr __at 0xE4 XBR3     ;  /* CROSSBAR CONFIGURATION REGISTER 3             */
__sfr __at 0xE8 P6       ;  /* PORT 6                                        */
__sfr __at 0xF8 P7       ;  /* PORT 7                                        */


/*  BIT Registers  */

/*  P0  0x80 */
__sbit __at 0x80 P0_0    ;
__sbit __at 0x81 P0_1    ;
__sbit __at 0x82 P0_2    ;
__sbit __at 0x83 P0_3    ;
__sbit __at 0x84 P0_4    ;
__sbit __at 0x85 P0_5    ;
__sbit __at 0x86 P0_6    ;
__sbit __at 0x87 P0_7    ;

/*  TCON  0x88 */
__sbit __at 0x88 IT0     ;  /* EXT. INTERRUPT 0 TYPE                         */
__sbit __at 0x89 IE0     ;  /* EXT. INTERRUPT 0 EDGE FLAG                    */
__sbit __at 0x8A IT1     ;  /* EXT. INTERRUPT 1 TYPE                         */
__sbit __at 0x8B IE1     ;  /* EXT. INTERRUPT 1 EDGE FLAG                    */
__sbit __at 0x8C TR0     ;  /* TIMER 0 ON/OFF CONTROL                        */
__sbit __at 0x8D TF0     ;  /* TIMER 0 OVERFLOW FLAG                         */
__sbit __at 0x8E TR1     ;  /* TIMER 1 ON/OFF CONTROL                        */
__sbit __at 0x8F TF1     ;  /* TIMER 1 OVERFLOW FLAG                         */

/*  CPT0CN  0x88 */
__sbit __at 0x88 CP0HYN0 ;  /* COMPARATOR 0 NEGATIVE HYSTERESIS 0            */
__sbit __at 0x89 CP0HYN1 ;  /* COMPARATOR 0 NEGATIVE HYSTERESIS 1            */
__sbit __at 0x8A CP0HYP0 ;  /* COMPARATOR 0 POSITIVE HYSTERESIS 0            */
__sbit __at 0x8B CP0HYP1 ;  /* COMPARATOR 0 POSITIVE HYSTERESIS 1            */
__sbit __at 0x8C CP0FIF  ;  /* COMPARATOR 0 FALLING EDGE INTERRUPT           */
__sbit __at 0x8D CP0RIF  ;  /* COMPARATOR 0 RISING EDGE INTERRUPT            */
__sbit __at 0x8E CP0OUT  ;  /* COMPARATOR 0 OUTPUT                           */
__sbit __at 0x8F CP0EN   ;  /* COMPARATOR 0 ENABLE                           */

/*  CPT1CN  0x88 */
__sbit __at 0x88 CP1HYN0 ;  /* COMPARATOR 1 NEGATIVE HYSTERESIS 0            */
__sbit __at 0x89 CP1HYN1 ;  /* COMPARATOR 1 NEGATIVE HYSTERESIS 1            */
__sbit __at 0x8A CP1HYP0 ;  /* COMPARATOR 1 POSITIVE HYSTERESIS 0            */
__sbit __at 0x8B CP1HYP1 ;  /* COMPARATOR 1 POSITIVE HYSTERESIS 1            */
__sbit __at 0x8C CP1FIF  ;  /* COMPARATOR 1 FALLING EDGE INTERRUPT           */
__sbit __at 0x8D CP1RIF  ;  /* COMPARATOR 1 RISING EDGE INTERRUPT            */
__sbit __at 0x8E CP1OUT  ;  /* COMPARATOR 1 OUTPUT                           */
__sbit __at 0x8F CP1EN   ;  /* COMPARATOR 1 ENABLE                           */

/*  CPT2CN  0x88 */
__sbit __at 0x88 CP2HYN0 ;  /* COMPARATOR 2 NEGATIVE HYSTERESIS 0            */
__sbit __at 0x89 CP2HYN1 ;  /* COMPARATOR 2 NEGATIVE HYSTERESIS 1            */
__sbit __at 0x8A CP2HYP0 ;  /* COMPARATOR 2 POSITIVE HYSTERESIS 0            */
__sbit __at 0x8B CP2HYP1 ;  /* COMPARATOR 2 POSITIVE HYSTERESIS 1            */
__sbit __at 0x8C CP2FIF  ;  /* COMPARATOR 2 FALLING EDGE INTERRUPT           */
__sbit __at 0x8D CP2RIF  ;  /* COMPARATOR 2 RISING EDGE INTERRUPT            */
__sbit __at 0x8E CP2OUT  ;  /* COMPARATOR 2 OUTPUT                           */
__sbit __at 0x8F CP2EN   ;  /* COMPARATOR 2 ENABLE                           */

/*  P1  0x90 */
__sbit __at 0x90 P1_0    ;
__sbit __at 0x91 P1_1    ;
__sbit __at 0x92 P1_2    ;
__sbit __at 0x93 P1_3    ;
__sbit __at 0x94 P1_4    ;
__sbit __at 0x95 P1_5    ;
__sbit __at 0x96 P1_6    ;
__sbit __at 0x97 P1_7    ;

/*  SCON0  0x98 */
__sbit __at 0x98 RI0     ;  /* UART 0 RX INTERRUPT FLAG                      */
__sbit __at 0x98 RI      ;  /* UART 0 RX INTERRUPT FLAG                      */
__sbit __at 0x99 TI0     ;  /* UART 0 TX INTERRUPT FLAG                      */
__sbit __at 0x99 TI      ;  /* UART 0 TX INTERRUPT FLAG                      */
__sbit __at 0x9A RB80    ;  /* UART 0 RX BIT 8                               */
__sbit __at 0x9B TB80    ;  /* UART 0 TX BIT 8                               */
__sbit __at 0x9C REN0    ;  /* UART 0 RX ENABLE                              */
__sbit __at 0x9C REN     ;  /* UART 0 RX ENABLE                              */
__sbit __at 0x9D SM20    ;  /* UART 0 MULTIPROCESSOR EN                      */
__sbit __at 0x9E SM10    ;  /* UART 0 MODE 1                                 */
__sbit __at 0x9F SM00    ;  /* UART 0 MODE 0                                 */

/*  SCON1  0x98 */
__sbit __at 0x98 RI1     ;  /* UART 1 RX INTERRUPT FLAG                      */
__sbit __at 0x99 TI1     ;  /* UART 1 TX INTERRUPT FLAG                      */
__sbit __at 0x9A RB81    ;  /* UART 1 RX BIT 8                               */
__sbit __at 0x9B TB81    ;  /* UART 1 TX BIT 8                               */
__sbit __at 0x9C REN1    ;  /* UART 1 RX ENABLE                              */
__sbit __at 0x9D MCE1    ;  /* UART 1 MCE                                    */
__sbit __at 0x9F S1MODE  ;  /* UART 1 MODE                                   */

/*  P2  0xA0 */
__sbit __at 0xA0 P2_0    ;
__sbit __at 0xA1 P2_1    ;
__sbit __at 0xA2 P2_2    ;
__sbit __at 0xA3 P2_3    ;
__sbit __at 0xA4 P2_4    ;
__sbit __at 0xA5 P2_5    ;
__sbit __at 0xA6 P2_6    ;
__sbit __at 0xA7 P2_7    ;

/*  IE  0xA8 */
__sbit __at 0xA8 EX0     ;  /* EXTERNAL INTERRUPT 0 ENABLE                   */
__sbit __at 0xA9 ET0     ;  /* TIMER 0 INTERRUPT ENABLE                      */
__sbit __at 0xAA EX1     ;  /* EXTERNAL INTERRUPT 1 ENABLE                   */
__sbit __at 0xAB ET1     ;  /* TIMER 1 INTERRUPT ENABLE                      */
__sbit __at 0xAC ES0     ;  /* UART0 INTERRUPT ENABLE                        */
__sbit __at 0xAC ES      ;  /* UART0 INTERRUPT ENABLE                        */
__sbit __at 0xAD ET2     ;  /* TIMER 2 INTERRUPT ENABLE                      */
__sbit __at 0xAF EA      ;  /* GLOBAL INTERRUPT ENABLE                       */

/*  P3  0xB0 */
__sbit __at 0xB0 P3_0    ;
__sbit __at 0xB1 P3_1    ;
__sbit __at 0xB2 P3_2    ;
__sbit __at 0xB3 P3_3    ;
__sbit __at 0xB4 P3_4    ;
__sbit __at 0xB5 P3_5    ;
__sbit __at 0xB6 P3_6    ;
__sbit __at 0xB7 P3_7    ;

/*  IP  0xB8 */
__sbit __at 0xB8 PX0     ;  /* EXTERNAL INTERRUPT 0 PRIORITY                 */
__sbit __at 0xB9 PT0     ;  /* TIMER 0 PRIORITY                              */
__sbit __at 0xBA PX1     ;  /* EXTERNAL INTERRUPT 1 PRIORITY                 */
__sbit __at 0xBB PT1     ;  /* TIMER 1 PRIORITY                              */
__sbit __at 0xBC PS0     ;  /* SERIAL PORT PRIORITY                          */
__sbit __at 0xBC PS      ;  /* SERIAL PORT PRIORITY                          */
__sbit __at 0xBD PT2     ;  /* TIMER 2 PRIORITY                              */

/*  SMB0CN  0xC0 */
__sbit __at 0xC0 SMBTOE  ;  /* SMBUS 0 TIMEOUT ENABLE                        */
__sbit __at 0xC1 SMBFTE  ;  /* SMBUS 0 FREE TIMER ENABLE                     */
__sbit __at 0xC2 AA      ;  /* SMBUS 0 ASSERT/ACKNOWLEDGE FLAG               */
__sbit __at 0xC3 SI      ;  /* SMBUS 0 INTERRUPT PENDING FLAG                */
__sbit __at 0xC4 STO     ;  /* SMBUS 0 STOP FLAG                             */
__sbit __at 0xC5 STA     ;  /* SMBUS 0 START FLAG                            */
__sbit __at 0xC6 ENSMB   ;  /* SMBUS 0 ENABLE                                */
__sbit __at 0xC7 BUSY    ;  /* SMBUS 0 BUSY                                  */

/*  CAN0STA  0xC0 */
__sbit __at 0xC3 CANTXOK ;  /* CAN TRANSMITTED A MESSAGE SUCCESSFULLY        */
__sbit __at 0xC4 CANRXOK ;  /* CAN RECEIVED A MESSAGE SUCCESSFULLY           */
__sbit __at 0xC5 CANEPASS;  /* CAN ERROR PASSIVE                             */
__sbit __at 0xC6 CANEWARN;  /* CAN WARNING STATUS                            */
__sbit __at 0xC7 CANBOFF ;  /* CAN BUSOFF STATUS                             */

/*  TMR2CN  0xC8 */
__sbit __at 0xC8 CPRL2   ;  /* TIMER 2 CAPTURE SELECT                        */
__sbit __at 0xC9 CT2     ;  /* TIMER 2 COUNTER SELECT                        */
__sbit __at 0xCA TR2     ;  /* TIMER 2 ON/OFF CONTROL                        */
__sbit __at 0xCB EXEN2   ;  /* TIMER 2 EXTERNAL ENABLE FLAG                  */
__sbit __at 0xCE EXF2    ;  /* TIMER 2 EXTERNAL FLAG                         */
__sbit __at 0xCF TF2     ;  /* TIMER 2 OVERFLOW FLAG                         */

/*  TMR3CN  0xC8 */
__sbit __at 0xC8 CPRL3   ;  /* TIMER 3 CAPTURE SELECT                        */
__sbit __at 0xC9 CT3     ;  /* TIMER 3 COUNTER SELECT                        */
__sbit __at 0xCA TR3     ;  /* TIMER 3 ON/OFF CONTROL                        */
__sbit __at 0xCB EXEN3   ;  /* TIMER 3 EXTERNAL ENABLE FLAG                  */
__sbit __at 0xCE EXF3    ;  /* TIMER 3 EXTERNAL FLAG                         */
__sbit __at 0xCF TF3     ;  /* TIMER 3 OVERFLOW FLAG                         */

/*  TMR4CN  0xC8 */
__sbit __at 0xC8 CPRL4   ;  /* TIMER 4 CAPTURE SELECT                        */
__sbit __at 0xC9 CT4     ;  /* TIMER 4 COUNTER SELECT                        */
__sbit __at 0xCA TR4     ;  /* TIMER 4 ON/OFF CONTROL                        */
__sbit __at 0xCB EXEN4   ;  /* TIMER 4 EXTERNAL ENABLE FLAG                  */
__sbit __at 0xCE EXF4    ;  /* TIMER 4 EXTERNAL FLAG                         */
__sbit __at 0xCF TF4     ;  /* TIMER 4 OVERFLOW FLAG                         */

/*  P4  0xC8 */
__sbit __at 0xC8 P4_0    ;
__sbit __at 0xC9 P4_1    ;
__sbit __at 0xCA P4_2    ;
__sbit __at 0xCB P4_3    ;
__sbit __at 0xCC P4_4    ;
__sbit __at 0xCD P4_5    ;
__sbit __at 0xCE P4_6    ;
__sbit __at 0xCF P4_7    ;

/*  PSW  0xD0 */
__sbit __at 0xD0 P       ;  /* ACCUMULATOR PARITY FLAG                       */
__sbit __at 0xD1 F1      ;  /* USER FLAG 1                                   */
__sbit __at 0xD2 OV      ;  /* OVERFLOW FLAG                                 */
__sbit __at 0xD3 RS0     ;  /* REGISTER BANK SELECT 0                        */
__sbit __at 0xD4 RS1     ;  /* REGISTER BANK SELECT 1                        */
__sbit __at 0xD5 F0      ;  /* USER FLAG 0                                   */
__sbit __at 0xD6 AC      ;  /* AUXILIARY CARRY FLAG                          */
__sbit __at 0xD7 CY      ;  /* CARRY FLAG                                    */

/*  PCA0CN  0xD8 */
__sbit __at 0xD8 CCF0    ;  /* PCA 0 MODULE 0 INTERRUPT FLAG                 */
__sbit __at 0xD9 CCF1    ;  /* PCA 0 MODULE 1 INTERRUPT FLAG                 */
__sbit __at 0xDA CCF2    ;  /* PCA 0 MODULE 2 INTERRUPT FLAG                 */
__sbit __at 0xDB CCF3    ;  /* PCA 0 MODULE 3 INTERRUPT FLAG                 */
__sbit __at 0xDC CCF4    ;  /* PCA 0 MODULE 4 INTERRUPT FLAG                 */
__sbit __at 0xDD CCF5    ;  /* PCA 0 MODULE 5 INTERRUPT FLAG                 */
__sbit __at 0xDE CR      ;  /* PCA 0 COUNTER RUN CONTROL BIT                 */
__sbit __at 0xDF CF      ;  /* PCA 0 COUNTER OVERFLOW FLAG                   */

/*  DMA0CN  0xD8 */
__sbit __at 0xD8 DMA0DO0 ;  /* ADC0 Data Overflow Warning Flag               */
__sbit __at 0xD9 DMA0DO1 ;  /* ADC1 Data Overflow Warning Flag               */
__sbit __at 0xDA DMA0DOE ;  /* Data Overflow Warning Interrupt Enable        */
__sbit __at 0xDB DMA0DE0 ;  /* ADC0 Data Overflow Error Flag                 */
__sbit __at 0xDC DMA0DE1 ;  /* ADC1 Data Overflow Error Flag                 */
__sbit __at 0xDD DMA0MD  ;  /* DMA0 Mode Select                              */
__sbit __at 0xDE DMA0INT ;  /* DMA0 Operations Complete Flag                 */
__sbit __at 0xDF DMA0EN  ;  /* DMA0 Enable                                   */

/*  P5  0xD8 */
__sbit __at 0xD8 P5_0    ;
__sbit __at 0xD9 P5_1    ;
__sbit __at 0xDA P5_2    ;
__sbit __at 0xDB P5_3    ;
__sbit __at 0xDC P5_4    ;
__sbit __at 0xDD P5_5    ;
__sbit __at 0xDE P5_6    ;
__sbit __at 0xDF P5_7    ;

/*  ADC0CN  0xE8 */
__sbit __at 0xE9 AD0WINT ;  /* ADC 0 WINDOW INTERRUPT FLAG                   */
__sbit __at 0xEA AD0CM0  ;  /* ADC 0 CONVERT START MODE BIT 0                */
__sbit __at 0xEB AD0CM1  ;  /* ADC 0 CONVERT START MODE BIT 1                */
__sbit __at 0xEC AD0BUSY ;  /* ADC 0 BUSY FLAG                               */
__sbit __at 0xED AD0INT  ;  /* ADC 0 EOC INTERRUPT FLAG                      */
__sbit __at 0xEE AD0TM   ;  /* ADC 0 TRACK MODE                              */
__sbit __at 0xEF AD0EN   ;  /* ADC 0 ENABLE                                  */

/*  ADC1CN  0xE8 */
__sbit __at 0xE9 AD1CM0  ;  /* ADC 1 CONVERT START MODE BIT 0                */
__sbit __at 0xEA AD1CM1  ;  /* ADC 1 CONVERT START MODE BIT 1                */
__sbit __at 0xEB AD1CM2  ;  /* ADC 1 CONVERT START MODE BIT 1                */
__sbit __at 0xEC AD1BUSY ;  /* ADC 1 BUSY FLAG                               */
__sbit __at 0xED AD1INT  ;  /* ADC 1 EOC INTERRUPT FLAG                      */
__sbit __at 0xEE AD1TM   ;  /* ADC 1 TRACK MODE                              */
__sbit __at 0xEF AD1EN   ;  /* ADC 1 ENABLE                                  */

/*  ADC2CN  0xE8 */
__sbit __at 0xE8 AD2LJST ;  /* ADC 2 LEFT JUSTIFY SELECT                     */
__sbit __at 0xE9 AD2WINT ;  /* ADC 2 WINDOW INTERRUPT FLAG                   */
__sbit __at 0xEA AD2CM0  ;  /* ADC 2 CONVERT START MODE BIT 0                */
__sbit __at 0xEB AD2CM1  ;  /* ADC 2 CONVERT START MODE BIT 1                */
__sbit __at 0xEC AD2BUSY ;  /* ADC 2 BUSY FLAG                               */
__sbit __at 0xED AD2INT  ;  /* ADC 2 EOC INTERRUPT FLAG                      */
__sbit __at 0xEE AD2TM   ;  /* ADC 2 TRACK MODE                              */
__sbit __at 0xEF AD2EN   ;  /* ADC 2 ENABLE                                  */

/*  P6  0xE8 */
__sbit __at 0xE8 P6_0    ;
__sbit __at 0xE9 P6_1    ;
__sbit __at 0xEA P6_2    ;
__sbit __at 0xEB P6_3    ;
__sbit __at 0xEC P6_4    ;
__sbit __at 0xED P6_5    ;
__sbit __at 0xEE P6_6    ;
__sbit __at 0xEF P6_7    ;

/*  SPI0CN  0xF8 */
__sbit __at 0xF8 SPIEN   ;  /* SPI 0 SPI ENABLE                              */
__sbit __at 0xF9 TXBMT   ;  /* SPI 0 TX BUFFER EMPTY FLAG                    */
__sbit __at 0xFA NSSMD0  ;  /* SPI 0 SLAVE SELECT MODE 0                     */
__sbit __at 0xFB NSSMD1  ;  /* SPI 0 SLAVE SELECT MODE 1                     */
__sbit __at 0xFC RXOVRN  ;  /* SPI 0 RX OVERRUN FLAG                         */
__sbit __at 0xFD MODF    ;  /* SPI 0 MODE FAULT FLAG                         */
__sbit __at 0xFE WCOL    ;  /* SPI 0 WRITE COLLISION FLAG                    */
__sbit __at 0xFF SPIF    ;  /* SPI 0 INTERRUPT FLAG                          */

/*  CAN0CN  0xF8 */
__sbit __at 0xF8 CANINIT ;  /* CAN INITIALIZATION                            */
__sbit __at 0xF9 CANIE   ;  /* CAN MODULE INTERRUPT ENABLE                   */
__sbit __at 0xFA CANSIE  ;  /* CAN STATUS CHANGE INTERRUPT ENABLE            */
__sbit __at 0xFB CANEIE  ;  /* CAN ERROR INTERRUPT ENABLE                    */
__sbit __at 0xFC CANIF   ;  /* CAN INTERRUPT FLAG                            */
__sbit __at 0xFD CANDAR  ;  /* CAN DISABLE AUTOMATIC RETRANSMISSION          */
__sbit __at 0xFE CANCCE  ;  /* CAN CONFIGURATION CHANGE ENABLE               */
__sbit __at 0xFF CANTEST ;  /* CAN TEST MODE ENABLE                          */

/*  DMA0CF  0xF8 */
__sbit __at 0xF8 DMA0EO  ;  /* END-OF-OPERATION FLAG                         */
__sbit __at 0xF9 DMA0EOE ;  /* END-OF-OPERATION INTERRUPT ENABLE             */
__sbit __at 0xFA DMA0CI  ;  /* REPEAT COUNTER OVERFLOW FLAG                  */
__sbit __at 0xFB DMA0CIE ;  /* REPEAT COUNTER OVERFLOW INTERRUPT ENABLE      */
__sbit __at 0xFE DMA0XBY ;  /* OFF-CHIP XRAM BUSY FLAG                       */
__sbit __at 0xFF DMA0HLT ;  /* HALT DMA0 OFF-CHIP XRAM ACCESS                */

/*  P7  0xF8 */
__sbit __at 0xF8 P7_0    ;
__sbit __at 0xF9 P7_1    ;
__sbit __at 0xFA P7_2    ;
__sbit __at 0xFB P7_3    ;
__sbit __at 0xFC P7_4    ;
__sbit __at 0xFD P7_5    ;
__sbit __at 0xFE P7_6    ;
__sbit __at 0xFF P7_7    ;


/* Predefined SFR Bit Masks */

#define IDLE              0x01    /* PCON                                */
#define STOP              0x02    /* PCON                                */
#define ECCF              0x01    /* PCA0CPMn                            */
#define PWM               0x02    /* PCA0CPMn                            */
#define TOG               0x04    /* PCA0CPMn                            */
#define MAT               0x08    /* PCA0CPMn                            */
#define CAPN              0x10    /* PCA0CPMn                            */
#define CAPP              0x20    /* PCA0CPMn                            */
#define ECOM              0x40    /* PCA0CPMn                            */
#define PWM16             0x80    /* PCA0CPMn                            */
#define PORSF             0x02    /* RSTSRC                              */
#define SWRSF             0x10    /* RSTSRC                              */


/* SFR PAGE DEFINITIONS */

#define CONFIG_PAGE       0x0F     /* SYSTEM AND PORT CONFIGURATION PAGE */
#define LEGACY_PAGE       0x00     /* LEGACY SFR PAGE                    */
#define TIMER01_PAGE      0x00     /* TIMER 0 AND TIMER 1                */
#define CPT0_PAGE         0x01     /* COMPARATOR 0                       */
#define CPT1_PAGE         0x02     /* COMPARATOR 1                       */
#define CPT2_PAGE         0x03     /* COMPARATOR 2                       */
#define UART0_PAGE        0x00     /* UART 0                             */
#define UART1_PAGE        0x01     /* UART 1                             */
#define SPI0_PAGE         0x00     /* SPI 0                              */
#define EMI0_PAGE         0x00     /* EXTERNAL MEMORY INTERFACE          */
#define ADC0_PAGE         0x00     /* ADC 0                              */
#define ADC1_PAGE         0x01     /* ADC 1                              */
#define ADC2_PAGE         0x02     /* ADC 2                              */
#define SMB0_PAGE         0x00     /* SMBUS 0                            */
#define TMR2_PAGE         0x00     /* TIMER 2                            */
#define TMR3_PAGE         0x01     /* TIMER 3                            */
#define TMR4_PAGE         0x02     /* TIMER 4                            */
#define DAC0_PAGE         0x00     /* DAC 0                              */
#define DAC1_PAGE         0x01     /* DAC 1                              */
#define PCA0_PAGE         0x00     /* PCA 0                              */
#define DMA0_PAGE         0x03     /* DMA 0                              */
#define CAN0_PAGE         0x01     /* CAN 0                              */

#endif
