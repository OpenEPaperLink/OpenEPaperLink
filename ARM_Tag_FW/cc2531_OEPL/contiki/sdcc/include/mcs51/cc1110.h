/*-------------------------------------------------------------------------
   Register Declarations for Chipcon CC1110

   Written By -  Pravin Angolkar (February 2008)
   (Based on CC1110 PRELIMINARY Data Sheet (rev. F) )

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

   In other words, you are welcome to use, share and improve this program.
   You are forbidden to forbid anyone else to use, share and improve
   what you give them.   Help stamp out software-hoarding!
-------------------------------------------------------------------------*/

#ifndef REG_CC1110_H
#define REG_CC1110_H

#include<compiler.h>
/* ------------------------------------------------------------------------------------------------
 *                                        Interrupt Vectors
 * ------------------------------------------------------------------------------------------------
 */
#define  RFTXRX_VECTOR  0    /*  RF TX done / RX ready                       */
#define  ADC_VECTOR     1    /*  ADC End of Conversion                       */
#define  URX0_VECTOR    2    /*  USART0 RX Complete                          */
#define  URX1_VECTOR    3    /*  USART1 RX Complete                          */
#define  ENC_VECTOR     4    /*  AES Encryption/Decryption Complete          */
#define  ST_VECTOR      5    /*  Sleep Timer Compare                         */
#define  P2INT_VECTOR   6    /*  Port 2 Inputs                               */
#define  UTX0_VECTOR    7    /*  USART0 TX Complete                          */
#define  DMA_VECTOR     8    /*  DMA Transfer Complete                       */
#define  T1_VECTOR      9    /*  Timer 1 (16-bit) Capture/Compare/Overflow   */
#define  T2_VECTOR      10   /*  Timer 2 (MAC Timer) Overflow                */
#define  T3_VECTOR      11   /*  Timer 3 (8-bit) Capture/Compare/Overflow    */
#define  T4_VECTOR      12   /*  Timer 4 (8-bit) Capture/Compare/Overflow    */
#define  P0INT_VECTOR   13   /*  Port 0 Inputs                               */
#define  UTX1_VECTOR    14   /*  USART1 TX Complete                          */
#define  P1INT_VECTOR   15   /*  Port 1 Inputs                               */
#define  RF_VECTOR      16   /*  RF General Interrupts                       */
#define  WDT_VECTOR     17   /*  Watchdog Overflow in Timer Mode             */

SFR(P0,       0x80); // Port 0
   SBIT(P0_0,     0x80, 0); // Port 0 bit 0
   SBIT(P0_1,     0x80, 1); // Port 0 bit 1
   SBIT(P0_2,     0x80, 2); // Port 0 bit 2
   SBIT(P0_3,     0x80, 3); // Port 0 bit 3
   SBIT(P0_4,     0x80, 4); // Port 0 bit 4
   SBIT(P0_5,     0x80, 5); // Port 0 bit 5
   SBIT(P0_6,     0x80, 6); // Port 0 bit 6
   SBIT(P0_7,     0x80, 7); // Port 0 bit 7

SFR(SP,       0x81); // Stack Pointer
SFR(DPL0,     0x82); // Data Pointer 0 Low Byte
SFR(DPH0,     0x83); // Data Pointer 0 High Byte
SFR(DPL1,     0x84); // Data Pointer 1 Low Byte
SFR(DPH1,     0x85); // Data Pointer 1 High Byte
SFR(U0CSR,    0x86); // USART 0 Control and Status
SFR(PCON,     0x87); // Power Mode Control

SFR(TCON,     0x88); // Interrupt Flags
   SBIT(IT0,      0x88, 0); // reserved (must always be set to 1)
   SBIT(RFTXRXIF, 0x88, 1); // RFERR – RF TX/RX FIFO interrupt flag
   SBIT(IT1,      0x88, 2); // reserved (must always be set to 1)
   SBIT(URX0IF,   0x88, 3); // USART0 RX Interrupt Flag
   SBIT(ADCIF,    0x88, 5); // ADC Interrupt Flag
   SBIT(URX1IF,   0x88, 7); // USART1 RX Interrupt Flag

SFR(P0IFG,    0x89); // Port 0 Interrupt Status Flag
SFR(P1IFG,    0x8A); // Port 1 Interrupt Status Flag
SFR(P2IFG,    0x8B); // Port 2 Interrupt Status Flag
SFR(PICTL,    0x8C); // Port Interrupt Control
SFR(P1IEN,    0x8D); // Port 1 Interrupt Mask
SFR(_SFR8E,   0x8E); // not used
SFR(P0INP,    0x8F); // Port 0 Input Mode

SFR(P1,       0x90); // Port 1
   SBIT(P1_0,     0x90, 0); // Port 1 bit 0
   SBIT(P1_1,     0x90, 1); // Port 1 bit 1
   SBIT(P1_2,     0x90, 2); // Port 1 bit 2
   SBIT(P1_3,     0x90, 3); // Port 1 bit 3
   SBIT(P1_4,     0x90, 4); // Port 1 bit 4
   SBIT(P1_5,     0x90, 5); // Port 1 bit 5
   SBIT(P1_6,     0x90, 6); // Port 1 bit 6
   SBIT(P1_7,     0x90, 7); // Port 1 bit 7

SFR(RFIM,     0x91); // RF Interrupt Mask
SFR(DPS,      0x92); // Data Pointer Select
SFR(MPAGE,    0x93); // Memory Page Select
SFR(_XPAGE,   0x93); // Memory Page Select under the name SDCC needs it
SFR(_SFR94,   0x94); // not used
SFR(_SFR95,   0x95); // not used
SFR(_SFR96,   0x96); // not used
SFR(_SFR97,   0x97); // not used

SFR(S0CON,    0x98); // Interrupt Flags 2
   SBIT(ENCIF_0,  0x98, 0); // AES Interrupt Flag 0
   SBIT(ENCIF_1,  0x98, 1); // AES Interrupt Flag 1

SFR(_SFR99,   0x99); // not used
SFR(IEN2,     0x9A); // Interrupt Enable 2
SFR(S1CON,    0x9B); // CPU Interrupt Flag 3
SFR(T2CT,     0x9C); // Timer 2 Count
SFR(T2PR,     0x9D); // Timer 2 Prescaler
SFR(T2CTL,    0x9E); // Timer 2 Control
SFR(_SFR9F,   0x9F); // not used

SFR(P2,       0xA0); // Port 2
   SBIT(P2_0,     0xA0, 0); // Port 2 bit 0
   SBIT(P2_1,     0xA0, 1); // Port 2 bit 1
   SBIT(P2_2,     0xA0, 2); // Port 2 bit 2
   SBIT(P2_3,     0xA0, 3); // Port 2 bit 3
   SBIT(P2_4,     0xA0, 4); // Port 2 bit 4
   SBIT(P2_5,     0xA0, 5); // Port 2 bit 5
   SBIT(P2_6,     0xA0, 6); // Port 2 bit 6
   SBIT(P2_7,     0xA0, 7); // Port 2 bit 7

SFR(WORIRQ,   0xA1); // Sleep Timer Interrupt Control
SFR(WORCTRL,  0xA2); // Sleep Timer Control
SFR(WOREVT0,  0xA3); // Sleep Timer Event0 Timeout Low
SFR(WOREVT1,  0xA4); // Sleep Timer Event0 Timeout High
SFR(WORTIME0, 0xA5); // Sleep Timer Low Byte
SFR(WORTIME1, 0xA6); // Sleep Timer High Byte
SFR(_SFRA7,   0xA7); // not used

SFR(IEN0,     0xA8); // Interrupt Enable 0
   SBIT(RFTXRXIE, 0xA8, 0); // RF TX/RX FIFO interrupt enable
   SBIT(ADCIE,    0xA8, 1); // ADC Interrupt Enable
   SBIT(URX0IE,   0xA8, 2); // USART0 RX Interrupt Enable
   SBIT(URX1IE,   0xA8, 3); // USART1 RX Interrupt Enable
   SBIT(ENCIE,    0xA8, 4); // AES Encryption/Decryption Interrupt Enable
   SBIT(STIE,     0xA8, 5); // Sleep Timer Interrupt Enable
   SBIT(EA,       0xA8, 7); // Global Interrupt Enable

SFR(IP0,      0xA9); // Interrupt Priority 0
SFR(_SFRAA,   0xAA); // not used
SFR(FWT,      0xAB); // Flash Write Timing
SFR(FADDRL,   0xAC); // Flash Address Low Byte
SFR(FADDRH,   0xAD); // Flash Address High Byte
SFR(FCTL,     0xAE); // Flash Control
SFR(FWDATA,   0xAF); // Flash Write Data

SFR(_SFRB0,   0xB0); // not used
SFR(ENCDI,    0xB1); // Encryption Input Data
SFR(ENCDO,    0xB2); // Encryption Output Data
SFR(ENCCS,    0xB3); // Encryption Control and Status
SFR(ADCCON1,  0xB4); // ADC Control 1
SFR(ADCCON2,  0xB5); // ADC Control 2
SFR(ADCCON3,  0xB6); // ADC Control 3
SFR(_SFRB7,   0xB7); // not used

SFR(IEN1,     0xB8); // Interrupt Enable 1
   SBIT(DMAIE,    0xB8, 0); // DMA Transfer Interrupt Enable
   SBIT(T1IE,     0xB8, 1); // Timer 1 Interrupt Enable
   SBIT(T2IE,     0xB8, 2); // Timer 2 Interrupt Enable
   SBIT(T3IE,     0xB8, 3); // Timer 3 Interrupt Enable
   SBIT(T4IE,     0xB8, 4); // Timer 4 Interrupt Enable
   SBIT(P0IE,     0xB8, 5); // Port 0 Interrupt Enable

SFR(IP1,      0xB9); // Interrupt Priority 1
SFR(ADCL,     0xBA); // ADC Data Low
SFR(ADCH,     0xBB); // ADC Data High
SFR(RNDL,     0xBC); // Random Number Generator Data Low Byte
SFR(RNDH,     0xBD); // Random Number Generator Data High Byte
SFR(SLEEP,    0xBE); // Sleep Mode Control
SFR(_SFRBF,   0xBF); // not used

SFR(IRCON,    0xC0); // Interrupt Flags 4
   SBIT(DMAIF,    0xC0, 0); // DMA Complete Interrupt Flag
   SBIT(T1IF,     0xC0, 1); // Timer 1 Interrupt Flag
   SBIT(T2IF,     0xC0, 2); // Timer 2 Interrupt Flag
   SBIT(T3IF,     0xC0, 3); // Timer 3 Interrupt Flag
   SBIT(T4IF,     0xC0, 4); // Timer 4 Interrupt Flag
   SBIT(P0IF,     0xC0, 5); // Port 0 Interrupt Flag
   SBIT(STIF,     0xC0, 7); // Sleep Timer Interrupt Flag

SFR(U0DBUF,   0xC1); // USART 0 Receive/Transmit Data Buffer
SFR(U0BAUD,   0xC2); // USART 0 Baud Rate Control
SFR(_SFRC3,   0xC3); // not in use
SFR(U0UCR,    0xC4); // USART 0 UART Control
SFR(U0GCR,    0xC5); // USART 0 Generic Control
SFR(CLKCON,   0xC6); // Clock Control
SFR(MEMCTR,   0xC7); // Memory Arbiter Control

SFR(_SFRC8,   0xC8); // not in use
SFR(WDCTL,    0xC9); // Watchdog Timer Control
SFR(T3CNT,    0xCA); // Timer 3 Counter
SFR(T3CTL,    0xCB); // Timer 3 Control
SFR(T3CCTL0,  0xCC); // Timer 3 Channel 0 Capture/Compare Control
SFR(T3CC0,    0xCD); // Timer 3 Channel 0 Capture/Compare Value
SFR(T3CCTL1,  0xCE); // Timer 3 Channel 1 Capture/Compare Control
SFR(T3CC1,    0xCF); // Timer 3 Channel 1 Capture/Compare Value

SFR(PSW,      0xD0); // Program Status Word
   SBIT(P,        0xD0, 0); // Parity Flag
   SBIT(F1,       0xD0, 1); // User-Defined Flag
   SBIT(OV,       0xD0, 2); // Overflow Flag
   SBIT(RS0,      0xD0, 3); // Register Bank Select 0
   SBIT(RS1,      0xD0, 4); // Register Bank Select 1
   SBIT(F0,       0xD0, 5); // User-Defined Flag
   SBIT(AC,       0xD0, 6); // Auxiliary Carry Flag
   SBIT(CY,       0xD0, 7); // Carry Flag

SFR(DMAIRQ,   0xD1); // DMA Interrupt Flag
SFR(DMA1CFGL, 0xD2); // DMA Channel 1-4 Configuration Address Low Byte
SFR(DMA1CFGH, 0xD3); // DMA Channel 1-4 Configuration Address High Byte
SFR(DMA0CFGL, 0xD4); // DMA Channel 0 Configuration Address Low Byte
SFR(DMA0CFGH, 0xD5); // DMA Channel 0 Configuration Address High Byte
SFR(DMAARM,   0xD6); // DMA Channel Arm
SFR(DMAREQ,   0xD7); // DMA Channel Start Request and Status

SFR(TIMIF,    0xD8); // Timers 1/3/4 Interrupt Mask/Flag
   SBIT(T3OVFIF,  0xD8, 0); // Timer 3 overflow interrupt flag 0:no pending 1:pending
   SBIT(T3CH0IF,  0xD8, 1); // Timer 3 channel 0 interrupt flag 0:no pending 1:pending
   SBIT(T3CH1IF,  0xD8, 2); // Timer 3 channel 1 interrupt flag 0:no pending 1:pending
   SBIT(T4OVFIF,  0xD8, 3); // Timer 4 overflow interrupt flag 0:no pending 1:pending
   SBIT(T4CH0IF,  0xD8, 4); // Timer 4 channel 0 interrupt flag 0:no pending 1:pending
   SBIT(T4CH1IF,  0xD8, 5); // Timer 4 channel 1 interrupt flag 0:no pending 1:pending
   SBIT(OVFIM,    0xD8, 6); // Timer 1 overflow interrupt mask

SFR(RFD,      0xD9); // RF Data
SFR(T1CC0L,   0xDA); // Timer 1 Channel 0 Capture/Compare Value Low Byte
SFR(T1CC0H,   0xDB); // Timer 1 Channel 0 Capture/Compare Value High Byte
SFR(T1CC1L,   0xDC); // Timer 1 Channel 1 Capture/Compare Value Low Byte
SFR(T1CC1H,   0xDD); // Timer 1 Channel 1 Capture/Compare Value High Byte
SFR(T1CC2L,   0xDE); // Timer 1 Channel 2 Capture/Compare Value Low Byte
SFR(T1CC2H,   0xDF); // Timer 1 Channel 2 Capture/Compare Value High Byte

SFR(ACC,      0xE0); // Accumulator
   SBIT(ACC_0,    0xE0, 0); // Accumulator bit 0
   SBIT(ACC_1,    0xE0, 1); // Accumulator bit 1
   SBIT(ACC_2,    0xE0, 2); // Accumulator bit 2
   SBIT(ACC_3,    0xE0, 3); // Accumulator bit 3
   SBIT(ACC_4,    0xE0, 4); // Accumulator bit 4
   SBIT(ACC_5,    0xE0, 5); // Accumulator bit 5
   SBIT(ACC_6,    0xE0, 6); // Accumulator bit 6
   SBIT(ACC_7,    0xE0, 7); // Accumulator bit 7

SFR(RFST,     0xE1); // RF Strobe Commands
SFR(T1CNTL,   0xE2); // Timer 1 Counter Low
SFR(T1CNTH,   0xE3); // Timer 1 Counter High
SFR(T1CTL,    0xE4); // Timer 1 Control and Status
SFR(T1CCTL0,  0xE5); // Timer 1 Channel 0 Capture/Compare Control
SFR(T1CCTL1,  0xE6); // Timer 1 Channel 1 Capture/Compare Control
SFR(T1CCTL2,  0xE7); // Timer 1 Channel 2 Capture/Compare Control

SFR(IRCON2,   0xE8); // Interrupt Flags 5
   SBIT(P2IF,     0xE8, 0); // Port 2 Interrupt Flag
   SBIT(UTX0IF,   0xE8, 1); // USART0 TX Interrupt Flag
   SBIT(UTX1IF,   0xE8, 2); // USART1 TX Interrupt Flag
   SBIT(P1IF,     0xE8, 3); // Port 1 Interrupt Flag
   SBIT(WDTIF,    0xE8, 4); // Watchdog Timer Interrupt Flag

SFR(RFIF,     0xE9); // RF Interrupt Flags
SFR(T4CNT,    0xEA); // Timer 4 Counter
SFR(T4CTL,    0xEB); // Timer 4 Control
SFR(T4CCTL0,  0xEC); // Timer 4 Channel 0 Capture/Compare Control
SFR(T4CC0,    0xED); // Timer 4 Channel 0 Capture/Compare Value
SFR(T4CCTL1,  0xEE); // Timer 4 Channel 1 Capture/Compare Control
SFR(T4CC1,    0xEF); // Timer 4 Channel 1 Capture/Compare Value

SFR(B,        0xF0); // B Register
   SBIT(B_0,      0xF0, 0); // Register B bit 0
   SBIT(B_1,      0xF0, 1); // Register B bit 1
   SBIT(B_2,      0xF0, 2); // Register B bit 2
   SBIT(B_3,      0xF0, 3); // Register B bit 3
   SBIT(B_4,      0xF0, 4); // Register B bit 4
   SBIT(B_5,      0xF0, 5); // Register B bit 5
   SBIT(B_6,      0xF0, 6); // Register B bit 6
   SBIT(B_7,      0xF0, 7); // Register B bit 7

SFR(PERCFG,   0xF1); // Peripheral Control
SFR(ADCCFG,   0xF2); // ADC Input Configuration
SFR(P0SEL,    0xF3); // Port 0 Function Select
SFR(P1SEL,    0xF4); // Port 1 Function Select
SFR(P2SEL,    0xF5); // Port 2 Function Select
SFR(P1INP,    0xF6); // Port 1 Input Mode
SFR(P2INP,    0xF7); // Port 2 Input Mode

SFR(U1CSR,    0xF8); // USART 1 Control and Status
   SBIT(ACTIVE,   0xF8, 0); // USART transmit/receive active status 0:idle 1:busy
   SBIT(TX_BYTE,  0xF8, 1); // Transmit byte status 0:Byte not transmitted 1:Last byte transmitted
   SBIT(RX_BYTE,  0xF8, 2); // Receive byte status 0:No byte received 1:Received byte ready
   SBIT(ERR,      0xF8, 3); // UART parity error status 0:No error 1:parity error
   SBIT(FE,       0xF8, 4); // UART framing error status 0:No error 1:incorrect stop bit level
   SBIT(SLAVE,    0xF8, 5); // SPI master or slave mode select 0:master 1:slave
   SBIT(RE,       0xF8, 6); // UART receiver enable 0:disabled 1:enabled
   SBIT(MODE,     0xF8, 7); // USART mode select 0:SPI 1:UART

SFR(U1DBUF,   0xF9); // USART 1 Receive/Transmit Data Buffer
SFR(U1BAUD,   0xFA); // USART 1 Baud Rate Control
SFR(U1UCR,    0xFB); // USART 1 UART Control
SFR(U1GCR,    0xFC); // USART 1 Generic Control
SFR(P0DIR,    0xFD); // Port 0 Direction
SFR(P1DIR,    0xFE); // Port 1 Direction
SFR(P2DIR,    0xFF); // Port 2 Direction

/* ------------------------------------------------------------------------------------------------
 *                            Xdata Radio Registers
 * ------------------------------------------------------------------------------------------------
 */

SFRX(MDMCTRL0H,   0xDF02);

SFRX(SYNC1,       0xDF00);  /*  Sync word, high byte                                */
SFRX(SYNC0,       0xDF01);  /*  Sync word, low byte                                 */
SFRX(PKTLEN,      0xDF02);  /*  Packet length                                       */
SFRX(PKTCTRL1,    0xDF03);  /*  Packet automation control                           */
SFRX(PKTCTRL0,    0xDF04);  /*  Packet automation control                           */
SFRX(ADDR,        0xDF05);  /*  Device address                                      */
SFRX(CHANNR,      0xDF06);  /*  Channel number                                      */
SFRX(FSCTRL1,     0xDF07);  /*  Frequency synthesizer control                       */
SFRX(FSCTRL0,     0xDF08);  /*  Frequency synthesizer control                       */
SFRX(FREQ2,       0xDF09);  /*  Frequency control word, high byte                   */
SFRX(FREQ1,       0xDF0A);  /*  Frequency control word, middle byte                 */
SFRX(FREQ0,       0xDF0B);  /*  Frequency control word, low byte                    */
SFRX(MDMCFG4,     0xDF0C);  /*  Modem configuration                                 */
SFRX(MDMCFG3,     0xDF0D);  /*  Modem configuration                                 */
SFRX(MDMCFG2,     0xDF0E);  /*  Modem configuration                                 */
SFRX(MDMCFG1,     0xDF0F);  /*  Modem configuration                                 */
SFRX(MDMCFG0,     0xDF10);  /*  Modem configuration                                 */
SFRX(DEVIATN,     0xDF11);  /*  Modem deviation setting                             */
SFRX(MCSM2,       0xDF12);  /*  Main Radio Control State Machine configuration      */
SFRX(MCSM1,       0xDF13);  /*  Main Radio Control State Machine configuration      */
SFRX(MCSM0,       0xDF14);  /*  Main Radio Control State Machine configuration      */
SFRX(FOCCFG,      0xDF15);  /*  Frequency Offset Compensation configuration         */
SFRX(BSCFG,       0xDF16);  /*  Bit Synchronization configuration                   */
SFRX(AGCCTRL2,    0xDF17);  /*  AGC control                                         */
SFRX(AGCCTRL1,    0xDF18);  /*  AGC control                                         */
SFRX(AGCCTRL0,    0xDF19);  /*  AGC control                                         */
SFRX(FREND1,      0xDF1A);  /*  Front end RX configuration                          */
SFRX(FREND0,      0xDF1B);  /*  Front end TX configuration                          */
SFRX(FSCAL3,      0xDF1C);  /*  Frequency synthesizer calibration                   */
SFRX(FSCAL2,      0xDF1D);  /*  Frequency synthesizer calibration                   */
SFRX(FSCAL1,      0xDF1E);  /*  Frequency synthesizer calibration                   */
SFRX(FSCAL0,      0xDF1F);  /*  Frequency synthesizer calibration                   */
SFRX(_XREGDF20,   0xDF20);  /*  reserved                                            */
SFRX(_XREGDF21,   0xDF21);  /*  reserved                                            */
SFRX(_XREGDF22,   0xDF22);  /*  reserved                                            */
SFRX(TEST2,       0xDF23);  /*  Various test settings                               */
SFRX(TEST1,       0xDF24);  /*  Various test settings                               */
SFRX(TEST0,       0xDF25);  /*  Various test settings                               */
SFRX(_XREGDF26,   0xDF26);  /*  reserved                                            */
SFRX(PA_TABLE7,   0xDF27);  /*  PA power setting 7                                  */
SFRX(PA_TABLE6,   0xDF28);  /*  PA power setting 6                                  */
SFRX(PA_TABLE5,   0xDF29);  /*  PA power setting 5                                  */
SFRX(PA_TABLE4,   0xDF2A);  /*  PA power setting 4                                  */
SFRX(PA_TABLE3,   0xDF2B);  /*  PA power setting 3                                  */
SFRX(PA_TABLE2,   0xDF2C);  /*  PA power setting 2                                  */
SFRX(PA_TABLE1,   0xDF2D);  /*  PA power setting 1                                  */
SFRX(PA_TABLE0,   0xDF2E);  /*  PA power setting 0                                  */
SFRX(IOCFG2,      0xDF2F);  /*  Radio Test Signal Configuration (P1_7)              */
SFRX(IOCFG1,      0xDF30);  /*  Radio Test Signal Configuration (P1_6)              */
SFRX(IOCFG0,      0xDF31);  /*  Radio Test Signal Configuration (P1_5)              */
SFRX(_XREGDF32,   0xDF32);  /*  reserved                                            */
SFRX(_XREGDF33,   0xDF33);  /*  reserved                                            */
SFRX(_XREGDF34,   0xDF34);  /*  reserved                                            */
SFRX(_XREGDF35,   0xDF35);  /*  reserved                                            */
SFRX(PARTNUM,     0xDF36);  /*  Chip ID [15:8]                                      */
SFRX(VERSION,     0xDF37);  /*  Chip ID [7:0]                                       */
SFRX(FREQEST,     0xDF38);  /*  Frequency Offset Estimate                           */
SFRX(LQI,         0xDF39);  /*  Link Quality Indicator                              */
SFRX(RSSI,        0xDF3A);  /*  Received Signal Strength Indication                 */
SFRX(MARCSTATE,   0xDF3B);  /*  Main Radio Control State                            */
SFRX(PKTSTATUS,   0xDF3C);  /*  Packet status                                       */
SFRX(VCO_VC_DAC,  0xDF3D);  /*  PLL calibration current                             */

/* ------------------------------------------------------------------------------------------------
 *                             Xdata I2S Registers
 * ------------------------------------------------------------------------------------------------
 */
SFRX(I2SCFG0,     0xDF40);  /*  I2S Configuration Register 0                        */
SFRX(I2SCFG1,     0xDF41);  /*  I2S Configuration Register 1                        */
SFRX(I2SDATL,     0xDF42);  /*  I2S Data Low Byte                                   */
SFRX(I2SDATH,     0xDF43);  /*  I2S Data High Byte                                  */
SFRX(I2SWCNT,     0xDF44);  /*  I2S Word Count Register                             */
SFRX(I2SSTAT,     0xDF45);  /*  I2S Status Register                                 */
SFRX(I2SCLKF0,    0xDF46);  /*  I2S Clock Configuration Register 0                  */
SFRX(I2SCLKF1,    0xDF47);  /*  I2S Clock Configuration Register 1                  */
SFRX(I2SCLKF2,    0xDF48);  /*  I2S Clock Configuration Register 2                  */

/* ------------------------------------------------------------------------------------------------
 *                             Xdata Mapped SFRs
 * ------------------------------------------------------------------------------------------------
 */

/*
 *   Most SFRs are also accessible through XDATA address space.  The register definitions for
 *   this type of access are listed below.  The register names are identical to the SFR names
 *   but with the prefix X_ to denote an XDATA register.
 *
 *   Some SFRs are not accessible through XDATA space.  For clarity, entries are included for these
 *   registers.  They have a prefix of _NA to denote "not available."
 *
 *   The SFRs prefixed with _X_ are not in use, but are listed for a better overview.
 *
 *   For register descriptions, refer to the actual SFR declarations elsewhere in this file.
 */

SFRX(_NA_P0,      0xDF80);
SFRX(_NA_SP,      0xDF81);
SFRX(_NA_DPL0,    0xDF82);
SFRX(_NA_DPH0,    0xDF83);
SFRX(_NA_DPL1,    0xDF84);
SFRX(_NA_DPH1,    0xDF85);
SFRX(X_U0CSR,     0xDF86);
SFRX(_NA_PCON,    0xDF87);

SFRX(_NA_TCON,    0xDF88);
SFRX(X_P0IFG,     0xDF89);
SFRX(X_P1IFG,     0xDF8A);
SFRX(X_P2IFG,     0xDF8B);
SFRX(X_PICTL,     0xDF8C);
SFRX(X_P1IEN,     0xDF8D);
SFRX(_X_SFR8E,    0xDF8E);
SFRX(X_P0INP,     0xDF8F);

SFRX(_NA_P1,      0xDF90);
SFRX(X_RFIM,      0xDF91);
SFRX(_NA_DPS,     0xDF92);
SFRX(X_MPAGE,     0xDF93);
SFRX(_X_SFR94,    0xDF94);
SFRX(_X_SFR95,    0xDF95);
SFRX(_X_SFR96,    0xDF96);
SFRX(_X_SFR97,    0xDF97);

SFRX(_NA_S0CON,   0xDF98);
SFRX(_X_SFR99,    0xDF99);
SFRX(_NA_IEN2,    0xDF9A);
SFRX(_NA_S1CON,   0xDF9B);
SFRX(X_T2CT,      0xDF9C);
SFRX(X_T2PR,      0xDF9D);
SFRX(X_T2CTL,     0xDF9E);
SFRX(_X_SFR9F,    0xDF9F);

SFRX(_NA_P2,      0xDFA0);
SFRX(X_WORIRQ,    0xDFA1);
SFRX(X_WORCTRL,   0xDFA2);
SFRX(X_WOREVT0,   0xDFA3);
SFRX(X_WOREVT1,   0xDFA4);
SFRX(X_WORTIME0,  0xDFA5);
SFRX(X_WORTIME1,  0xDFA6);
SFRX(_X_SFRA7,    0xDFA7);

SFRX(_NA_IEN0,    0xDFA8);
SFRX(_NA_IP0,     0xDFA9);
SFRX(_X_SFRAA,    0xDFAA);
SFRX(X_FWT,       0xDFAB);
SFRX(X_FADDRL,    0xDFAC);
SFRX(X_FADDRH,    0xDFAD);
SFRX(X_FCTL,      0xDFAE);
SFRX(X_FWDATA,    0xDFAF);

SFRX(_X_SFRB0,    0xDFB0);
SFRX(X_ENCDI,     0xDFB1);
SFRX(X_ENCDO,     0xDFB2);
SFRX(X_ENCCS,     0xDFB3);
SFRX(X_ADCCON1,   0xDFB4);
SFRX(X_ADCCON2,   0xDFB5);
SFRX(X_ADCCON3,   0xDFB6);
SFRX(_X_SFRB7,    0xDFB7);

SFRX(_NA_IEN1,    0xDFB8);
SFRX(_NA_IP1,     0xDFB9);
SFRX(X_ADCL,      0xDFBA);
SFRX(X_ADCH,      0xDFBB);
SFRX(X_RNDL,      0xDFBC);
SFRX(X_RNDH,      0xDFBD);
SFRX(X_SLEEP,     0xDFBE);
SFRX(_X_SFRBF,    0xDFBF);

SFRX(_NA_IRCON,   0xDFC0);
SFRX(X_U0DBUF,    0xDFC1);
SFRX(X_U0BAUD,    0xDFC2);
SFRX(_X_SFRC3,    0xDFC3);
SFRX(X_U0UCR,     0xDFC4);
SFRX(X_U0GCR,     0xDFC5);
SFRX(X_CLKCON,    0xDFC6);
SFRX(X_MEMCTR,    0xDFC7);

SFRX(_X_SFRC8,    0xDFC8);
SFRX(X_WDCTL,     0xDFC9);
SFRX(X_T3CNT,     0xDFCA);
SFRX(X_T3CTL,     0xDFCB);
SFRX(X_T3CCTL0,   0xDFCC);
SFRX(X_T3CC0,     0xDFCD);
SFRX(X_T3CCTL1,   0xDFCE);
SFRX(X_T3CC1,     0xDFCF);

SFRX(_NA_PSW,     0xDFD0);
SFRX(X_DMAIRQ,    0xDFD1);
SFRX(X_DMA1CFGL,  0xDFD2);
SFRX(X_DMA1CFGH,  0xDFD3);
SFRX(X_DMA0CFGL,  0xDFD4);
SFRX(X_DMA0CFGH,  0xDFD5);
SFRX(X_DMAARM,    0xDFD6);
SFRX(X_DMAREQ,    0xDFD7);

SFRX(X_TIMIF,     0xDFD8);
SFRX(X_RFD,       0xDFD9);
SFRX(X_T1CC0L,    0xDFDA);
SFRX(X_T1CC0H,    0xDFDB);
SFRX(X_T1CC1L,    0xDFDC);
SFRX(X_T1CC1H,    0xDFDD);
SFRX(X_T1CC2L,    0xDFDE);
SFRX(X_T1CC2H,    0xDFDF);

SFRX(_NA_ACC,     0xDFE0);
SFRX(X_RFST,      0xDFE1);
SFRX(X_T1CNTL,    0xDFE2);
SFRX(X_T1CNTH,    0xDFE3);
SFRX(X_T1CTL,     0xDFE4);
SFRX(X_T1CCTL0,   0xDFE5);
SFRX(X_T1CCTL1,   0xDFE6);
SFRX(X_T1CCTL2,   0xDFE7);

SFRX(_NA_IRCON2,  0xDFE8);
SFRX(X_RFIF,      0xDFE9);
SFRX(X_T4CNT,     0xDFEA);
SFRX(X_T4CTL,     0xDFEB);
SFRX(X_T4CCTL0,   0xDFEC);
SFRX(X_T4CC0,     0xDFED);
SFRX(X_T4CCTL1,   0xDFEE);
SFRX(X_T4CC1,     0xDFEF);

SFRX(_NA_B,       0xDFF0);
SFRX(X_PERCFG,    0xDFF1);
SFRX(X_ADCCFG,    0xDFF2);
SFRX(X_P0SEL,     0xDFF3);
SFRX(X_P1SEL,     0xDFF4);
SFRX(X_P2SEL,     0xDFF5);
SFRX(X_P1INP,     0xDFF6);
SFRX(X_P2INP,     0xDFF7);

SFRX(X_U1CSR,     0xDFF8);
SFRX(X_U1DBUF,    0xDFF9);
SFRX(X_U1BAUD,    0xDFFA);
SFRX(X_U1UCR,     0xDFFB);
SFRX(X_U1GCR,     0xDFFC);
SFRX(X_P0DIR,     0xDFFD);
SFRX(X_P1DIR,     0xDFFE);
SFRX(X_P2DIR,     0xDFFF);

/* ------------------------------------------------------------------------------------------------
 */

#endif
