/*-------------------------------------------------------------------------
   cc2530.h - Register Declarations for Chipcon/Texas Intruments CC2530
   (Based on CC253x User's Guide (rev. B) SWRU191B)

   Copyright (C) 2011, Zafi Ramarosandratana / zramaro at gmail.com

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

#ifndef REG_CC2530_H
#define REG_CC2530_H

#include <compiler.h>

// Interrupt Vectors

#define RFERR_VECTOR    0   // RF TX FIFO underflow and RX FIFO overflow.
#define ADC_VECTOR      1   // ADC end of conversion
#define URX0_VECTOR     2   // USART0 RX complete
#define URX1_VECTOR     3   // USART1 RX complete
#define ENC_VECTOR      4   // AES encryption/decryption complete
#define ST_VECTOR       5   // Sleep Timer compare
#define P2INT_VECTOR    6   // Port 2 inputs
#define UTX0_VECTOR     7   // USART0 TX complete
#define DMA_VECTOR      8   // DMA transfer complete
#define T1_VECTOR       9   // Timer 1 (16-bit) capture/compare/overflow
#define T2_VECTOR       10  // Timer 2 (MAC Timer)
#define T3_VECTOR       11  // Timer 3 (8-bit) capture/compare/overflow
#define T4_VECTOR       12  // Timer 4 (8-bit) capture/compare/overflow
#define P0INT_VECTOR    13  // Port 0 inputs
#define UTX1_VECTOR     14  // USART1 TX complete
#define P1INT_VECTOR    15  // Port 1 inputs
#define RF_VECTOR       16  // RF general interrupts
#define WDT_VECTOR      17  // Watchdog overflow in timer mode

// SFR Registers and BITs

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
   SBIT(RFERRIF,  0x88, 1); // RF TX/RX FIFO interrupt flag
   SBIT(IT1,      0x88, 2); // reserved (must always be set to 1)
   SBIT(URX0IF,   0x88, 3); // USART0 RX Interrupt Flag
   SBIT(ADCIF,    0x88, 5); // ADC Interrupt Flag
   SBIT(URX1IF,   0x88, 7); // USART1 RX Interrupt Flag
SFR(P0IFG,    0x89); // Port 0 Interrupt Status Flag
SFR(P1IFG,    0x8A); // Port 1 Interrupt Status Flag
SFR(P2IFG,    0x8B); // Port 2 Interrupt Status Flag
SFR(PICTL,    0x8C); // Port Interrupt Control
SFR(P1IEN,    0x8D); // Port 1 Interrupt Mask
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
SFR(RFIRQF1,  0x91); // RF Interrupt Flags MSB
SFR(DPS,      0x92); // Data Pointer Select
SFR(MPAGE,    0x93); // Memory Page Select
SFR(_XPAGE,   0x93); // XDATA/PDATA page alias for SDCC
SFR(T2CTRL,   0x94); // Timer 2 Control
SFR(ST0,      0x95); // Sleep Timer 0
SFR(ST1,      0x96); // Sleep Timer 1
SFR(ST2,      0x97); // Sleep Timer 2
SFR(S0CON,    0x98); // Interrupt Flags 2
   SBIT(ENCIF_0,  0x98, 0); // AES Interrupt Flag 0
   SBIT(ENCIF_1,  0x98, 1); // AES Interrupt Flag 1
SFR(IEN2,     0x9A); // Interrupt Enable 2
SFR(S1CON,    0x9B); // Interrupt Flags 3
SFR(T2EVTCFG, 0x9C); // Timer 2 Event Configuration
SFR(SLEEPSTA, 0x9D); // Sleep Mode Control Status
SFR(CLKCONSTA,0x9E); // Clock Control Status
SFR(PSBANK,   0x9F); // Identifier Name used by the trampoline __sdcc_banked_call
SFR(FMAP,     0x9F); // Flash Memory Bank Mapping
SFR(P2,       0xA0); // Port 2
   SBIT(P2_0,     0xA0, 0); // Port 2 bit 0
   SBIT(P2_1,     0xA0, 1); // Port 2 bit 1
   SBIT(P2_2,     0xA0, 2); // Port 2 bit 2
   SBIT(P2_3,     0xA0, 3); // Port 2 bit 3
   SBIT(P2_4,     0xA0, 4); // Port 2 bit 4
   SBIT(P2_5,     0xA0, 5); // Port 2 bit 5
   SBIT(P2_6,     0xA0, 6); // Port 2 bit 6
   SBIT(P2_7,     0xA0, 7); // Port 2 bit 7
SFR(T2IRQF,   0xA1); // Timer 2 Interrupt Flags
SFR(T2M0,     0xA2); // Timer 2 Multiplexed Register 0
SFR(T2M1,     0xA3); // Timer 2 Multiplexed Rgeister 1
SFR(T2MOVF0,  0xA4); // Timer 2 Multiplexed Overflow Register 0
SFR(T2MOVF1,  0xA5); // Timer 2 Multiplexed Overflow Register 1
SFR(T2MOVF2,  0xA6); // Timer 2 Multiplexed Overflow Register 2
SFR(T2IRQM,   0xA7); // Timer 2 Interrupt Mask
SFR(IEN0,     0xA8); // Interrupt Enable 0
   SBIT(RFERRIE,  0xA8, 0); // RF TX/RX FIFO interrupt Enable
   SBIT(ADCIE,    0xA8, 1); // ADC Interrupt Enable
   SBIT(URX0IE,   0xA8, 2); // USART0 RX Interrupt Enable
   SBIT(URX1IE,   0xA8, 3); // USART1 RX Interrupt Enable
   SBIT(ENCIE,    0xA8, 4); // AES Encryption/Decryption Interrupt Enable
   SBIT(STIE,     0xA8, 5); // Sleep Timer Interrupt Enable
   SBIT(EA,       0xA8, 7); // Global Interrupt Enable
SFR(IP0,      0xA9); // Interrupt Priority 0
SFR(P0IEN,    0xAB); // Port 0 Interrupt Mask
SFR(P2IEN,    0xAC); // Port 2 Interrupt Mask
SFR(STLOAD,   0xAD); // Sleep Timer Load Status
SFR(PMUX,     0xAE); // Power Down Signal Mux
SFR(T1STAT,   0xAF); // Timer 1 Status
SFR(ENCDI,    0xB1); // Encryption Input Data
SFR(ENCDO,    0xB2); // Encryption Output Data
SFR(ENCCS,    0xB3); // Encryption Control and Status
SFR(ADCCON1,  0xB4); // ADC Control 1
SFR(ADCCON2,  0xB5); // ADC Control 2
SFR(ADCCON3,  0xB6); // ADC Control 3
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
SFR(SLEEPCMD, 0xBE); // Sleep Mode Control Command
SFR(RFERRF,   0xBF); // RF Error Interrupt Flags
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
SFR(T2MSEL,   0xC3); // Timer 2 Multiplex Select
SFR(U0UCR,    0xC4); // USART 0 UART Control
SFR(U0GCR,    0xC5); // USART 0 Generic Control
SFR(CLKCONCMD,0xC6); // Clock Control Command
SFR(MEMCTR,   0xC7); // Memory Arbitrer Control
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
SFR(T1CC0L,   0xDA); // Timer 1 Channel 0 Capture/Compare Value Low
SFR(T1CC0H,   0xDB); // Timer 1 Channel 0 Capture/Compare Value High
SFR(T1CC1L,   0xDC); // Timer 1 Channel 1 Capture/Compare Value Low
SFR(T1CC1H,   0xDD); // Timer 1 Channel 1 Capture/Compare Value High
SFR(T1CC2L,   0xDE); // Timer 1 Channel 2 Capture/Compare Value Low
SFR(T1CC2H,   0xDF); // Timer 1 Channel 2 Capture/Compare Value High
SFR(ACC,      0xE0); // Accumulator
   SBIT(ACC_0,    0xE0, 0); // Accumulator bit 0
   SBIT(ACC_1,    0xE0, 1); // Accumulator bit 1
   SBIT(ACC_2,    0xE0, 2); // Accumulator bit 2
   SBIT(ACC_3,    0xE0, 3); // Accumulator bit 3
   SBIT(ACC_4,    0xE0, 4); // Accumulator bit 4
   SBIT(ACC_5,    0xE0, 5); // Accumulator bit 5
   SBIT(ACC_6,    0xE0, 6); // Accumulator bit 6
   SBIT(ACC_7,    0xE0, 7); // Accumulator bit 7
SFR(RFST,     0xE1); // RF CSMA-CA / Strobe Processor
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
SFR(RFIRQF0,  0xE9); // RF Interrupt Flags LSB
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
SFR(PERCFG,   0xF1); // Peripheral I/O Control
SFR(APCFG,    0xF2); // Analog Peripheral I/O Configuration
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
//
//// From Table 2-2 : Overview of XREG Registers
//
SFRX(I2CCFG,     0x6230); // I2C control
SFRX(I2CSTAT,    0x6231); // I2C status
SFRX(I2CDATA,    0x6232); // I2C data
SFRX(I2CADDR,    0x6233); // I2C own slave address
SFRX(I2CWC,      0x6234); // Wrapper control
SFRX(I2CIO,      0x6235); // GPIO
SFRX(OBSSEL0,    0x6243); // Observation output control register 0
SFRX(OBSSEL1,    0x6244); // Observation output control register 1
SFRX(OBSSEL2,    0x6245); // Observation output control register 2
SFRX(OBSSEL3,    0x6246); // Observation output control register 3
SFRX(OBSSEL4,    0x6247); // Observation output control register 4
SFRX(OBSSEL5,    0x6248); // Observation output control register 5
SFRX(CHVER,      0x6249); // Chip version
SFRX(CHIPID,     0x624A); // Chip identification
SFRX(TESTREG0,   0x624B); // Test register 0, cannot use TR0 name from the datasheet due to TR0 (TCON.4) redefinition
SFRX(DBGDATA,    0x6260); // Debug interface write data
SFRX(SRCRC,      0x6262); // Sleep reset CRC
SFRX(BATTMON,    0x6264); // Battery monitor
SFRX(IVCTRL,     0x6265); // Analog control register
SFRX(FCTL,       0x6270); // Flash control
SFRX(FADDRL,     0x6271); // Flash address low
SFRX(FADDRH,     0x6272); // Flash address high
SFRX(FWDATA,     0x6273); // Flash write data
SFRX(CHIPINFO0,  0x6276); // Chip information byte 0
SFRX(CHIPINFO1,  0x6277); // Chip information byte 1
SFRX(IRCTL,      0x6281); // Timer 1 IR generation control
SFRX(CLD,        0x6290); // Clock-loss detection 
SFRX(X_T1CCTL0,    0x62A0); // Timer 1 channel 0 capture/compare control (additional XREG mapping of SFR register)
SFRX(X_T1CCTL1,    0x62A1); // Timer 1 channel 1 capture/compare control (additional XREG mapping of SFR register)
SFRX(X_T1CCTL2,    0x62A2); // Timer 1 channel 2 capture/compare control (additional XREG mapping of SFR register)
SFRX(T1CCTL3,    0x62A3); // Timer 1 channel 3 capture/compare control
SFRX(T1CCTL4,    0x62A4); // Timer 1 channel 4 capture/compare control
SFRX(X_T1CC0L,     0x62A6); // Timer 1 channel 0 capture/compare value low (additional XREG mapping of SFR register)
SFRX(X_T1CC0H,     0x62A7); // Timer 1 channel 0 capture/compare value high (additional XREG mapping of SFR register)
SFRX(X_T1CC1L,     0x62A8); // Timer 1 channel 1 capture/compare value low (additional XREG mapping of SFR register)
SFRX(X_T1CC1H,     0x62A9); // Timer 1 channel 1 capture/compare value high (additional XREG mapping of SFR register)
SFRX(X_T1CC2L,     0x62AA); // Timer 1 channel 2 capture/compare value low (additional XREG mapping of SFR register)
SFRX(X_T1CC2H,     0x62AB); // Timer 1 channel 2 capture/compare value high (additional XREG mapping of SFR register)
SFRX(T1CC3L,     0x62AC); // Timer 1 channel 3 capture/compare value low
SFRX(T1CC3H,     0x62AD); // Timer 1 channel 3 capture/compare value high
SFRX(T1CC4L,     0x62AE); // Timer 1 channel 4 capture/compare value low
SFRX(T1CC4H,     0x62AF); // Timer 1 channel 4 capture/compare value high
SFRX(STCC,       0x62B0); // Sleep Timer capture control
SFRX(STCS,       0x62B1); // Sleep Timer capture status
SFRX(STCV0,      0x62B2); // Sleep Timer capture value byte 0
SFRX(STCV1,      0x62B3); // Sleep Timer capture value byte 1
SFRX(STCV2,      0x62B4); // Sleep Timer capture value byte 2
SFRX(OPAMPC,     0x62C0); // Operational amplifier control
SFRX(OPAMPS,     0x62C1); // Operational amplifier status
SFRX(CMPCTL,     0x62D0); // Analog comparator control and status
//
//// From Section 21.12 : USB Registers
//
SFRX(USBADDR,    0x6200); // Function Address
SFRX(USBPOW,     0x6201); // Power/Control Register
SFRX(USBIIF,     0x6202); // IN Endpoints and EP0 Interrupt Flags
SFRX(USBOIF,     0x6204); // OUT-Endpoint Interrupt Flags
SFRX(USBCIF,     0x6206); // Common USB Interrupt Flags
SFRX(USBIIE,     0x6207); // IN Endpoints and EP0 Interrupt-Enable Mask
SFRX(USBOIE,     0x6209); // Out Endpoints Interrupt Enable Mask
SFRX(USBCIE,     0x620B); // Common USB Interrupt-Enable Mask
SFRX(USBFRML,    0x620C); // Current Frame Number (Low Byte)
SFRX(USBFRMH,    0x620D); // Current Frame Number (High Byte)
SFRX(USBINDEX,   0x620E); // Current-Endpoint Index Register
SFRX(USBCTRL,    0x620F); // USB Control Register
SFRX(USBMAXI,    0x6210); // Max. Packet Size for IN Endpoint{1–5}
SFRX(USBCS0,     0x6211); // EP0 Control and Status (USBINDEX = 0)
SFRX(USBCSIL,    0x6211); // IN EP{1–5} Control and Status, Low
SFRX(USBCSIH,    0x6212); // IN EP{1–5} Control and Status, High
SFRX(USBMAXO,    0x6213); // Max. Packet Size for OUT EP{1–5}
SFRX(USBCSOL,    0x6214); // OUT EP{1–5} Control and Status, Low
SFRX(USBCSOH,    0x6215); // OUT EP{1–5} Control and Status, High
SFRX(USBCNT0,    0x6216); // Number of Received Bytes in EP0 FIFO (USBINDEX = 0)
SFRX(USBCNTL,    0x6216); // Number of Bytes in EP{1–5} OUT FIFO, Low
SFRX(USBCNTH,    0x6217); // Number of Bytes in EP{1–5} OUT FIFO, High
SFRX(USBF0,      0x6220); // Endpoint-0 FIFO
SFRX(USBF1,      0x6222); // Endpoint-1 FIFO
SFRX(USBF2,      0x6224); // Endpoint-2 FIFO
SFRX(USBF3,      0x6226); // Endpoint-3 FIFO
SFRX(USBF4,      0x6228); // Endpoint-4 FIFO
SFRX(USBF5,      0x622A); // Endpoint-5 FIFO
//
//// From Table 23.1 : Frame Filtering
//
SFRX(SHORTADDRH, 0x6174); // Short Address High Byte
SFRX(SHORTADDRL, 0x6175); // Short Address Low Byte
SFRX(PANIDH,     0x6172); // PAN ID High Byte
SFRX(PANIDL,     0x6173); // PAN ID Low Byte
SFRX(IEEE_ADDR,  0x616A); // Extended Address MSB

//
//// From Table 23-5 : CC253x Radio Register Overview
//
SFRX(FRMFILT0,   0x6180); // Frame Filtering
SFRX(FRMFILT1,   0x6181); // Frame Filtering
SFRX(SRCMATCH,   0x6182); // Source Address Matching and Pending Bits
SFRX(SRCSHORTEN0,0x6183); // Short Address Matching
SFRX(SRCSHORTEN1,0x6184); // Short Address Matching
SFRX(SRCSHORTEN2,0x6185); // Short Address Matching
SFRX(SRCEXTEN0,  0x6186); // Extended Address Matching
SFRX(SRCEXTEN1,  0x6187); // Extended Address Matching
SFRX(SRCEXTEN2,  0x6188); // Extended Address Matching
SFRX(FRMCTRL0,   0x6189); // Frame Handling
SFRX(FRMCTRL1,   0x618A); // Frame Handling
SFRX(RXENABLE,   0x618B); // RX Enabling
SFRX(RXMASKSET,  0x618C); // RX Enabling
SFRX(RXMASKCLR,  0x618D); // RX Disabling
SFRX(FREQTUNE,   0x618E); // Crystal Oscillator Frequency Tuning
SFRX(FREQCTRL,   0x618F); // Controls the RF Frequency
SFRX(TXPOWER,    0x6190); // Controls the Output Power
SFRX(TXCTRL,     0x6191); // Controls the TX Settings
SFRX(FSMSTAT0,   0x6192); // Radio Status Register
SFRX(FSMSTAT1,   0x6193); // Radio Status Register
SFRX(FIFOPCTRL,  0x6194); // FIFOP Threshold
SFRX(FSMCTRL,    0x6195); // FSM Options
SFRX(CCACTRL0,   0x6196); // CCA Threshold
SFRX(CCACTRL1,   0x6197); // Other CCA Options
SFRX(RSSI,       0x6198); // RSSI Status Register
SFRX(RSSISTAT,   0x6199); // RSSI Valid Status Register
SFRX(RXFIRST,    0x619A); // First Byte in RXFIFO
SFRX(RXFIFOCNT,  0x619B); // Number of Bytes in RXFIFO
SFRX(TXFIFOCNT,  0x619C); // Number of Bytes in TXFIFO
SFRX(RXFIRST_PTR,0x619D); // RXFIFO Pointer
SFRX(RXLAST_PTR, 0x619E); // RXFIFO Pointer
SFRX(RXP1_PTR,   0x619F); // RXFIFO Pointer
SFRX(TXFIRST_PTR,0x61A1); // TXFIFO Pointer
SFRX(TXLAST_PTR, 0x61A2); // TXFIFO Pointer
SFRX(RFIRQM0,    0x61A3); // RF Interrupt Masks
SFRX(RFIRQM1,    0x61A4); // RF Interrupt Masks
SFRX(RFERRM,     0x61A5); // RF Error Interrupt Mask
SFRX(OPAMPMC,    0x61A6); // Operational amplifier mode control
SFRX(RFRND,      0x61A7); // Random Data
SFRX(MDMCTRL0,   0x61A8); // Controls Modem
SFRX(MDMCTRL1,   0x61A9); // Controls Modem
SFRX(FREQEST,    0x61AA); // Estimated RF Frequency Offset
SFRX(RXCTRL,     0x61AB); // Tune Receive Section
SFRX(FSCTRL,     0x61AC); // Tune Frequency Synthesizer
SFRX(FSCAL1,     0x61AE); // Tune Frequency Calibration
SFRX(FSCAL2,     0x61AF); // Tune Frequency Calibration
SFRX(FSCAL3,     0x61B0); // Tune Frequency Calibration
SFRX(AGCCTRL0,   0x61B1); // AGC Dynamic Range Control
SFRX(AGCCTRL1,   0x61B2); // AGC Reference Level
SFRX(AGCCTRL2,   0x61B3); // AGC Gain Override
SFRX(AGCCTRL3,   0x61B4); // AGC Control
SFRX(ADCTEST0,   0x61B5); // ADC Tuning
SFRX(ADCTEST1,   0x61B6); // ADC Tuning
SFRX(ADCTEST2,   0x61B7); // ADC Tuning
SFRX(MDMTEST0,   0x61B8); // Test Register for Modem
SFRX(MDMTEST1,   0x61B9); // Test Register for Modem
SFRX(DACTEST0,   0x61BA); // DAC Override Value
SFRX(DACTEST1,   0x61BB); // DAC Override Value
SFRX(DACTEST2,   0x61BC); // DAC Test Setting
SFRX(ATEST,      0x61BD); // Analog Test Control
SFRX(PTEST0,     0x61BE); // Override Power-Down Register
SFRX(PTEST1,     0x61BF); // Override Power-Down Register
SFRX(CSPPROG0,   0x61C0); // CSP Program 0
SFRX(CSPPROG1,   0x61C1); // CSP Program 1
SFRX(CSPPROG2,   0x61C2); // CSP Program 2
SFRX(CSPPROG3,   0x61C3); // CSP Program 3
SFRX(CSPPROG4,   0x61C4); // CSP Program 4
SFRX(CSPPROG5,   0x61C5); // CSP Program 5
SFRX(CSPPROG6,   0x61C6); // CSP Program 6
SFRX(CSPPROG7,   0x61C7); // CSP Program 7
SFRX(CSPPROG8,   0x61C8); // CSP Program 8
SFRX(CSPPROG9,   0x61C9); // CSP Program 9
SFRX(CSPPROG10,  0x61CA); // CSP Program 10
SFRX(CSPPROG11,  0x61CB); // CSP Program 11
SFRX(CSPPROG12,  0x61CC); // CSP Program 12
SFRX(CSPPROG13,  0x61CD); // CSP Program 13
SFRX(CSPPROG14,  0x61CE); // CSP Program 14
SFRX(CSPPROG15,  0x61CF); // CSP Program 15
SFRX(CSPPROG16,  0x61D0); // CSP Program 16
SFRX(CSPPROG17,  0x61D1); // CSP Program 17
SFRX(CSPPROG18,  0x61D2); // CSP Program 18
SFRX(CSPPROG19,  0x61D3); // CSP Program 19
SFRX(CSPPROG20,  0x61D4); // CSP Program 20
SFRX(CSPPROG21,  0x61D5); // CSP Program 21
SFRX(CSPPROG22,  0x61D6); // CSP Program 22
SFRX(CSPPROG23,  0x61D7); // CSP Program 23
SFRX(CSPCTRL,    0x61E0); // CSP Control Bit
SFRX(CSPSTAT,    0x61E1); // CSP Status Register
SFRX(CSPX,       0x61E2); // CSP X Register
SFRX(CSPY,       0x61E3); // CSP Y Register
SFRX(CSPZ,       0x61E4); // CSP Z Register
SFRX(CSPT,       0x61E5); // CSP T Register
SFRX(RFC_OBS_CTRL0, 0x61EB); // RF Observation Mux Control
SFRX(RFC_OBS_CTRL1, 0x61EC); // RF Observation Mux Control
SFRX(RFC_OBS_CTRL2, 0x61ED); // RF Observation Mux Control
SFRX(TXFILTCFG,  0x61FA); // TX Filter Configuration

#endif //REG_CC2530_H
