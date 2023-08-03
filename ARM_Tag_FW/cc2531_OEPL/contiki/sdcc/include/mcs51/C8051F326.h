/*-------------------------------------------------------------------------
   C8051F326.h - Register Declarations for the Cygnal/SiLabs C8051F326/7
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

#ifndef C8051F326_H
#define C8051F326_H


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
__sfr __at (0x91) SBCON0       ;  /* BAUDRATE GENERATOR 0 CONTROL                  */
__sfr __at (0x93) SBRLL0       ;  /* BAUDRATE GENERATOR 0 RELOAD VALUE - LOW BYTE  */
__sfr __at (0x94) SBRLH0       ;  /* BAUDRATE GENERATOR 0 RELOAD VALUE - HIGH BYTE */
__sfr __at (0x96) USB0ADR      ;  /* USB0 INDIRECT ADDRESS REGISTER                */
__sfr __at (0x97) USB0DAT      ;  /* USB0 DATA REGISTER                            */
__sfr __at (0x98) SCON         ;  /* UART0 CONTROL                                 */
__sfr __at (0x98) SCON0        ;  /* UART0 CONTROL                                 */
__sfr __at (0x99) SBUF         ;  /* UART0 BUFFER                                  */
__sfr __at (0x99) SBUF0        ;  /* UART0 BUFFER                                  */
__sfr __at (0x9A) SMOD0        ;  /* UART0 MODE                                    */
__sfr __at (0xA0) P2           ;  /* PORT 2                                        */
__sfr __at (0xA4) P0MDOUT      ;  /* PORT 0 OUTPUT MODE CONFIGURATION              */
__sfr __at (0xA6) P2MDOUT      ;  /* PORT 2 OUTPUT MODE CONFIGURATION              */
__sfr __at (0xA7) P3MDOUT      ;  /* PORT 3 OUTPUT MODE CONFIGURATION              */
__sfr __at (0xA8) IE           ;  /* INTERRUPT ENABLE                              */
__sfr __at (0xA9) CLKSEL       ;  /* SYSTEM CLOCK SELECT                           */
__sfr __at (0xAA) EMI0CN       ;  /* EXTERNAL MEMORY INTERFACE CONTROL             */
__sfr __at (0xAA) _XPAGE       ;  /* XDATA/PDATA PAGE                              */
__sfr __at (0xB0) P3           ;  /* PORT 3                                        */
__sfr __at (0xB2) OSCICN       ;  /* INTERNAL OSCILLATOR CONTROL                   */
__sfr __at (0xB3) OSCICL       ;  /* INTERNAL OSCILLATOR CALIBRATION               */
__sfr __at (0xB6) FLSCL        ;  /* FLASH MEMORY TIMING PRESCALER                 */
__sfr __at (0xB7) FLKEY        ;  /* FLASH ACESS LIMIT                             */
__sfr __at (0xB8) IP           ;  /* INTERRUPT PRIORITY                            */
__sfr __at (0xB9) CLKMUL       ;  /* CLOCK MULTIPLIER CONTROL REGISTER             */
__sfr __at (0xC9) REG0CN       ;  /* VOLTAGE REGULATOR CONTROL                     */
__sfr __at (0xD0) PSW          ;  /* PROGRAM STATUS WORD                           */
__sfr __at (0xD7) USB0XCN      ;  /* USB0 TRANSCEIVER CONTROL                      */
__sfr __at (0xE0) ACC          ;  /* ACCUMULATOR                                   */
__sfr __at (0xE2) GPIOCN       ;  /* GLOBAL PORT I/O CONTROL                       */
__sfr __at (0xE3) OSCLCN       ;  /* LOW-FREQUENCY OSCILLATOR CONTROL              */
__sfr __at (0xE6) EIE1         ;  /* EXTERNAL INTERRUPT ENABLE 1                   */
__sfr __at (0xE7) EIE2         ;  /* EXTERNAL INTERRUPT ENABLE 2                   */
__sfr __at (0xEF) RSTSRC       ;  /* RESET SOURCE                                  */
__sfr __at (0xF0) B            ;  /* B REGISTER                                    */
__sfr __at (0xF6) EIP1         ;  /* EXTERNAL INTERRUPT PRIORITY REGISTER 1        */
__sfr __at (0xF7) EIP2         ;  /* EXTERNAL INTERRUPT PRIORITY REGISTER 2        */
__sfr __at (0xFF) VDM0CN       ;  /* VDD MONITOR CONTROL                           */


/*  WORD/DWORD Registers  */

__sfr16 __at (0x8C8A) TMR0     ;  /* TIMER 0 COUNTER                               */
__sfr16 __at (0x8D8B) TMR1     ;  /* TIMER 1 COUNTER                               */
__sfr16 __at (0x9493) SBRL0    ;  /* BAUDRATE GENERATOR 0 RELOAD VALUE WORD        */


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
__sbit __at (0x9A) RBX0        ;  /* SCON.2 - EXTRA RECEIVE BIT                    */
__sbit __at (0x9B) TB8         ;  /* SCON.3 - TRANSMIT BIT 8                       */
__sbit __at (0x9B) TBX0        ;  /* SCON.3 - EXTRA TRANSMIT BIT                   */
__sbit __at (0x9C) REN         ;  /* SCON.4 - RECEIVE ENABLE                       */
__sbit __at (0x9C) REN0        ;  /* SCON.4 - RECEIVE ENABLE                       */
__sbit __at (0x9E) PERR0       ;  /* SCON.6 - PARITY ERROR FLAG                    */
__sbit __at (0x9F) OVR0        ;  /* SCON.7 - RECEIVE FIFO OVERRUN FLAG            */

/*  P2  0xA0 */
__sbit __at (0xA0) P2_0        ;
__sbit __at (0xA1) P2_1        ;
__sbit __at (0xA2) P2_2        ;
__sbit __at (0xA3) P2_3        ;
__sbit __at (0xA4) P2_4        ;
__sbit __at (0xA5) P2_5        ;

/*  IE  0xA8 */
__sbit __at (0xA8) EX0         ;  /* IE.0 - EXTERNAL INTERRUPT 0 ENABLE            */
__sbit __at (0xA9) ET0         ;  /* IE.1 - TIMER 0 INTERRUPT ENABLE               */
__sbit __at (0xAA) EX1         ;  /* IE.2 - EXTERNAL INTERRUPT 1 ENABLE            */
__sbit __at (0xAB) ET1         ;  /* IE.3 - TIMER 1 INTERRUPT ENABLE               */
__sbit __at (0xAC) ES          ;  /* IE.4 - SERIAL PORT INTERRUPT ENABLE           */
__sbit __at (0xAC) ES0         ;  /* IE.4 - SERIAL PORT INTERRUPT ENABLE           */
__sbit __at (0xAF) EA          ;  /* IE.7 - GLOBAL INTERRUPT ENABLE                */

/*  P3  0xB0 */
__sbit __at (0xB0) P3_0        ;

/*  IP  0xB8 */
__sbit __at (0xB8) PX0         ;  /* IP.0 - EXTERNAL INTERRUPT 0 PRIORITY          */
__sbit __at (0xB9) PT0         ;  /* IP.1 - TIMER 0 PRIORITY                       */
__sbit __at (0xBA) PX1         ;  /* IP.2 - EXTERNAL INTERRUPT 1 PRIORITY          */
__sbit __at (0xBB) PT1         ;  /* IP.3 - TIMER 1 PRIORITY                       */
__sbit __at (0xBC) PS          ;  /* IP.4 - SERIAL PORT PRIORITY                   */
__sbit __at (0xBC) PS0         ;  /* IP.4 - SERIAL PORT PRIORITY                   */

/*  PSW  0xD0 */
__sbit __at (0xD0) PARITY      ;  /* PSW.0 - ACCUMULATOR PARITY FLAG               */
__sbit __at (0xD1) F1          ;  /* PSW.1 - FLAG 1                                */
__sbit __at (0xD2) OV          ;  /* PSW.2 - OVERFLOW FLAG                         */
__sbit __at (0xD3) RS0         ;  /* PSW.3 - REGISTER BANK SELECT 0                */
__sbit __at (0xD4) RS1         ;  /* PSW.4 - REGISTER BANK SELECT 1                */
__sbit __at (0xD5) F0          ;  /* PSW.5 - FLAG 0                                */
__sbit __at (0xD6) AC          ;  /* PSW.6 - AUXILIARY CARRY FLAG                  */
__sbit __at (0xD7) CY          ;  /* PSW.7 - CARRY FLAG                            */


/* Predefined SFR Bit Masks */

#define PCON_IDLE         0x01    /* PCON                                */
#define PCON_STOP         0x02    /* PCON                                */
#define T0M               0x04    /* CKCON                               */
#define T1M               0x08    /* CKCON                               */
#define PSWE              0x01    /* PSCTL                               */
#define PSEE              0x02    /* PSCTL                               */
#define EUSB0             0x02    /* EIE1                                */
#define EVBUS             0x01    /* EIE2                                */
#define PORSF             0x02    /* RSTSRC                              */
#define SWRSF             0x10    /* RSTSRC                              */

#endif
