/*-------------------------------------------------------------------------
   cc2510fx.h - Register Declarations for Chipcon CC2510Fx/CC2511Fx
   (Based on CC2510Fx/CC2511Fx PRELIMINARY Data Sheet (Rev. 1.2) SWRS055A)

   Copyright (C) 2006, Jesus Calvino-Fraga / jesusc at ece.ubc.ca

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

#ifndef REG_CC2510FX_H
#define REG_CC2510FX_H

#include <compiler.h>

// Interrupt Vectors
#define RFTXRX_VECTOR   0 // RF TX done / RX ready  
#define ADC_VECTOR      1 // ADC end of conversion  
#define URX0_VECTOR     2 // USART0 RX complete  
#define URX1_VECTOR     3 // USART1 RX complete (I2S RX complete)  
#define ENC_VECTOR      4 // AES encryption/decryption complete  
#define ST_VECTOR       5 // Sleep Timer compare 
#define P2INT_VECTOR    6 // Port 2 inputs (Also used for USB on CC2511Fx) 
#define UTX0_VECTOR     7 // USART0 TX complete 
#define DMA_VECTOR      8 // DMA transfer complete 
#define T1_VECTOR       9 // Timer 1 (16-bit) capture/Compare/overflow 
#define T2_VECTOR      10 // Timer 2 (MAC Timer) overflow 
#define T3_VECTOR      11 // Timer 3 (8-bit) capture/compare/overflow 
#define T4_VECTOR      12 // Timer 4 (8-bit) capture/compare/overflow 
#define P0INT_VECTOR   13 // Port 0 inputs (Note: USB Resume from suspend interrupt on P0_7 on CC2511Fx ) 
#define UTX1_VECTOR    14 // USART1 TX complete (I2S TX complete) 
#define P1INT_VECTOR   15 // Port 1 inputs 
#define RF_VECTOR      16 // RF general interrupts 
#define WDT_VECTOR     17 // Watchdog overflow in timer mode 

//Shared Interrupt Vectors (I2S and USB)
#define I2SRX_VECTOR    3 //I2S RX complete 
#define I2STX_VECTOR   14 //I2S TX complete 
#define USB_VECTOR      6 //USB Interrupt pending ( CC2511Fx ) 

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
SFR(TCON,     0x88); // Interrupt Flag 
   SBIT(IT0,      0x88, 0); // reserved (must always be set to 1) 
   SBIT(RFTXRXIF, 0x88, 1); // RF TX/RX FIFO Interrupt Flag 
   SBIT(IT1,      0x88, 2); // reserved (must always be set to 1) 
   SBIT(URX0IF,   0x88, 3); // USART0 RX Interrupt Flag 
   SBIT(ADCIF,    0x88, 5); // ADC Interrupt Flag 
   SBIT(URX1IF,   0x88, 7); // USART1 RX Interrupt Flag 
   SBIT(I2SRXIF,  0x88, 7); // I2S RX interrupt flag (same loc as URX1IF) 
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
SFR(RFIM,     0x91); // RF Interrupt Mask
SFR(DPS,      0x92); // Data Pointer Select 
SFR(MPAGE,    0x93); // Memory Page Select 
SFR(_XPAGE,   0x93); // Memory Page Select under the name SDCC needs it
SFR(ENDIAN,   0x95); // USB Endianess Control (CC2511Fx) 
SFR(S0CON,    0x98); // Interrupt Flag 2
   SBIT(ENCIF_0,  0x98, 0); // AES Interrupt Flag 0 
   SBIT(ENCIF_1,  0x98, 1); // AES Interrupt Flag 1 
SFR(IEN2,     0x9A); // Interrupt Enable 2 Register
SFR(S1CON,    0x9B); // Interrupt Flag 3 
SFR(T2CT,     0x9C); // Timer 2 Count
SFR(T2PR,     0x9D); // Timer 2 Prescaler
SFR(T2CTL,    0x9E); // Timer 2 Control 
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
SFR(WORCTL,   0xA2); // Sleep Timer Control
SFR(WORCTRL,  0xA2); // Sleep Timer Control (typo in datasheet?)
SFR(WOREVT0,  0xA3); // Sleep Timer Event0 Timeout Low
SFR(WOREVT1,  0xA4); // Sleep Timer Event0 Timeout High 
SFR(WORTIME0, 0xA5); // Sleep Timer Low Byte 
SFR(WORTIME1, 0xA6); // Sleep Timer High Byte 
SFR(IEN0,     0xA8); // Interrupt Enable 0 Register
   SBIT(RFTXRXIE, 0xA8, 0); // RF TX/RX done interrupt enable 
   SBIT(ADCIE,    0xA8, 1); // ADC Interrupt Enable 
   SBIT(URX0IE,   0xA8, 2); // USART0 RX Interrupt Enable 
   SBIT(URX1IE,   0xA8, 3); // USART1 RX Interrupt Enable
   SBIT(I2SRXIE,  0xA8, 3); // I2S RX interrupt enable (Same loc as URX1IE)
   SBIT(ENCIE,    0xA8, 4); // AES Encryption/Decryption Interrupt Enable 
   SBIT(STIE,     0xA8, 5); // Sleep Timer Interrupt Enable 
   SBIT(EA,       0xA8, 7); // Global Interrupt Enable 
SFR(IP0,      0xA9); // Interrupt Priority 0
SFR(FWT,      0xAB); // Flash Write Timing
SFR(FADDRL,   0xAC); // Flash Address Low Byte 
SFR(FADDRH,   0xAD); // Flash Address High Byte 
SFR(FCTL,     0xAE); // Flash Control 
SFR(FWDATA,   0xAF); // Flash Write Data
SFR(ENCDI,    0xB1); // Encryption Input Data
SFR(ENCDO,    0xB2); // Encryption Output Data
SFR(ENCCS,    0xB3); // Encryption Control and Status 
SFR(ADCCON1,  0xB4); // ADC Control 1 
SFR(ADCCON2,  0xB5); // ADC Control 2 
SFR(ADCCON3,  0xB6); // ADC Control 3 
SFR(IEN1,     0xB8); // Interrupt Enable 1 Register
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
SFR(IRCON,    0xC0); // Interrupt Flag 4 
   SBIT(DMAIF,    0xC0, 0); // DMA Complete Interrupt Flag 
   SBIT(T1IF,     0xC0, 1); // Timer 1 Interrupt Flag 
   SBIT(T2IF,     0xC0, 2); // Timer 2 Interrupt Flag 
   SBIT(T3IF,     0xC0, 3); // Timer 3 Interrupt Flag 
   SBIT(T4IF,     0xC0, 4); // Timer 4 Interrupt Flag 
   SBIT(P0IF,     0xC0, 5); // Port 0 Interrupt Flag 
   SBIT(STIF,     0xC0, 7); // Sleep Timer Interrupt Flag 
SFR(U0DBUF,   0xC1); // USART 0 Receive/Transmit Data Buffer
SFR(U0BAUD,   0xC2); // USART 0 Baud Rate Control
SFR(U0UCR,    0xC4); // USART 0 UART Control 
SFR(U0GCR,    0xC5); // USART 0 Generic Control
SFR(CLKCON,   0xC6); // Clock Control
SFR(MEMCTR,   0xC7); // Memory Arbiter Control
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
SFR(RFST,     0xE1); // RF Strobe Commands
SFR(T1CNTL,   0xE2); // Timer 1 Counter Low 
SFR(T1CNTH,   0xE3); // Timer 1 Counter High 
SFR(T1CTL,    0xE4); // Timer 1 Control and Status
SFR(T1CCTL0,  0xE5); // Timer 1 Channel 0 Capture/Compare Control
SFR(T1CCTL1,  0xE6); // Timer 1 Channel 1 Capture/Compare Control
SFR(T1CCTL2,  0xE7); // Timer 1 Channel 2 Capture/Compare Control
SFR(IRCON2,   0xE8); // Interrupt Flag 5 
   SBIT(P2IF,     0xE8, 0); // Port 2 Interrupt Flag 
   SBIT(USBIF,    0xE8, 0); // USB Interrupt Flag (same bit as P2IF)
   SBIT(UTX0IF,   0xE8, 1); // USART0 TX Interrupt Flag 
   SBIT(UTX1IF,   0xE8, 2); // USART1 TX Interrupt Flag 
   SBIT(I2STXIF,  0xE8, 2); // I2S TX Interrupt Flag (same bit as UTX1FIF)
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

// From table 68: xdata RF registers

SFRX(SYNC1,      0xDF00); // Sync word, high byte 
SFRX(SYNC0,      0xDF01); // Sync word, low byte 
SFRX(PKTLEN,     0xDF02); // Packet length 
SFRX(PKTCTRL1,   0xDF03); // Packet automation control 
SFRX(PKTCTRL0,   0xDF04); // Packet automation control 
SFRX(ADDR,       0xDF05); // Device address 
SFRX(CHANNR,     0xDF06); // Channel number 
SFRX(FSCTRL1,    0xDF07); // Frequency synthesizer control 
SFRX(FSCTRL0,    0xDF08); // Frequency synthesizer control 
SFRX(FREQ2,      0xDF09); // Frequency control word, high byte 
SFRX(FREQ1,      0xDF0A); // Frequency control word, middle byte 
SFRX(FREQ0,      0xDF0B); // Frequency control word, low byte 
SFRX(MDMCFG4,    0xDF0C); // Modem configuration 
SFRX(MDMCFG3,    0xDF0D); // Modem configuration 
SFRX(MDMCFG2,    0xDF0E); // Modem configuration 
SFRX(MDMCFG1,    0xDF0F); // Modem configuration 
SFRX(MDMCFG0,    0xDF10); // Modem configuration 
SFRX(DEVIATN,    0xDF11); // Modem deviation setting 
SFRX(MCSM2,      0xDF12); // Main Radio Control State Machine configuration 
SFRX(MCSM1,      0xDF13); // Main Radio Control State Machine configuration 
SFRX(MCSM0,      0xDF14); // Main Radio Control State Machine configuration 
SFRX(FOCCFG,     0xDF15); // Frequency Offset Compensation configuration 
SFRX(BSCFG,      0xDF16); // Bit Synchronization configuration 
SFRX(AGCCTRL2,   0xDF17); // AGC control 
SFRX(AGCCTRL1,   0xDF18); // AGC control 
SFRX(AGCCTRL0,   0xDF19); // AGC control 
SFRX(FREND1,     0xDF1A); // Front end RX configuration 
SFRX(FREND0,     0xDF1B); // Front end TX configuration 
SFRX(FSCAL3,     0xDF1C); // Frequency synthesizer calibration 
SFRX(FSCAL2,     0xDF1D); // Frequency synthesizer calibration 
SFRX(FSCAL1,     0xDF1E); // Frequency synthesizer calibration 
SFRX(FSCAL0,     0xDF1F); // Frequency synthesizer calibration 
SFRX(PA_TABLE7,  0xDF27); // PA output power setting 
SFRX(PA_TABLE6,  0xDF28); // PA output power setting 
SFRX(PA_TABLE5,  0xDF29); // PA output power setting 
SFRX(PA_TABLE4,  0xDF2A); // PA output power setting 
SFRX(PA_TABLE3,  0xDF2B); // PA output power setting 
SFRX(PA_TABLE2,  0xDF2C); // PA output power setting 
SFRX(PA_TABLE1,  0xDF2D); // PA output power setting 
SFRX(PA_TABLE0,  0xDF2E); // PA output power setting 
SFRX(IOCFG2,     0xDF2F); // GDO2 output pin configuration 
SFRX(IOCFG1,     0xDF30); // GDO1 output pin configuration 
SFRX(IOCFG0,     0xDF31); // GDO0 output pin configuration 
SFRX(PARTNUM,    0xDF36); // Chip Identifier 
SFRX(VERSION,    0xDF37); // Configuration 
SFRX(FREQEST,    0xDF38); // Frequency Offset Estimate 
SFRX(LQI,        0xDF39); // Link Quality Indicator 
SFRX(RSSI,       0xDF3A); // Received Signal Strength Indication 
SFRX(MARCSTATE,  0xDF3B); // Main Radio Control State 
SFRX(PKTSTATUS,  0xDF3C); // Packet status 
SFRX(VCO_VC_DAC, 0xDF3D); // PLL calibration current 

// I2S Registers

SFRX(I2SCFG0,  0xDF40); // I2S Configuration Register 0 
SFRX(I2SCFG1,  0xDF41); // I2S Configuration Register 1 
SFRX(I2SDATL,  0xDF42); // I2S Data Low Byte 
SFRX(I2SDATH,  0xDF43); // I2S Data High Byte 
SFRX(I2SWCNT,  0xDF44); // I2S Word Count Register 
SFRX(I2SSTAT,  0xDF45); // I2S Status Register 
SFRX(I2SCLKF0, 0xDF46); // I2S Clock Configuration Register 0 
SFRX(I2SCLKF1, 0xDF47); // I2S Clock Configuration Register 1 
SFRX(I2SCLKF2, 0xDF48); // I2S Clock Configuration Register 2

// Common USB Registers

SFRX(USBADDR,  0xDE00); // Function Address
SFRX(USBPOW,   0xDE01); // Power/Control Register
SFRX(USBIIF,   0xDE02); // IN Endpoints and EP0 Interrupt Flags
SFRX(USBOIF,   0xDE04); // OUT Endpoints Interrupt Flags
SFRX(USBCIF,   0xDE06); // Common USB Interrupt Flags
SFRX(USBIIE,   0xDE07); // IN Endpoints and EP0 Interrupt Enable Mask
SFRX(USBOIE,   0xDE09); // Out Endpoints Interrupt Enable Mask
SFRX(USBCIE,   0xDE0B); // Common USB Interrupt Enable Mask
SFRX(USBFRML,  0xDE0C); // Current Frame Number (Low byte)
SFRX(USBFRMH,  0xDE0D); // Current Frame Number (High byte)
SFRX(USBINDEX, 0xDE0E); // Selects current endpoint.

// Indexed Endpoint Registers

SFRX(USBMAXI, 0xDE10); // Max. packet size for IN endpoint
SFRX(USBCS0,  0xDE11); // EP0 Control and Status (USBINDEX = 0)
SFRX(USBCSIL, 0xDE11); // IN EP{1-5} Control and Status Low
SFRX(USBCSIH, 0xDE12); // IN EP{1-5} Control and Status High
SFRX(USBMAXO, 0xDE13); // Max. packet size for OUT endpoint
SFRX(USBCSOL, 0xDE14); // OUT EP{1-5} Control and Status Low
SFRX(USBCSOH, 0xDE15); // OUT EP{1-5} Control and Status High
SFRX(USBCNT0, 0xDE16); // Number of received bytes in EP0 FIFO (USBINDEX = 0)
SFRX(USBCNTL, 0xDE16); // Number of bytes in OUT FIFO Low
SFRX(USBCNTH, 0xDE17); // Number of bytes in OUT FIFO High

// Endpoint FIFO Registers

SFRX(USBF0, 0xDE20); //  Endpoint 0 FIFO
SFRX(USBF1, 0xDE22); //  Endpoint 1 FIFO
SFRX(USBF2, 0xDE24); //  Endpoint 2 FIFO
SFRX(USBF3, 0xDE26); //  Endpoint 3 FIFO
SFRX(USBF4, 0xDE28); //  Endpoint 4 FIFO
SFRX(USBF5, 0xDE2A); //  Endpoint 5 FIFO

// SFRs also accesible through XDATA space

SFRX(X_U0CSR,    0xDF86); // USART 0 Control and Status
SFRX(X_P0IFG,    0xDF89); // Port 0 Interrupt Status Flag
SFRX(X_P1IFG,    0xDF8A); // Port 1 Interrupt Status Flag
SFRX(X_P2IFG,    0xDF8B); // Port 2 Interrupt Status Flag
SFRX(X_PICTL,    0xDF8C); // Port Pins Interrupt Mask and Edge
SFRX(X_P1IEN,    0xDF8D); // Port 1 Interrupt Mask
SFRX(X_P0INP,    0xDF8F); // Port 0 Input Mode
SFRX(X_RFIM,     0xDF91); // RF Interrupt Mask
SFRX(X_MPAGE,    0xDF93); // Memory Page Select 
SFRX(X_T2CT,     0xDF9C); // Timer 2 Control
SFRX(X_T2PR,     0xDF9D); // Timer 2 Prescaler
SFRX(X_T2CTL,    0xDF9E); // Timer 2 Control
SFRX(X_WORIRQ,   0xDFA1); // Timer Sleep Timer Interrupts
SFRX(X_WORCTL,   0xDFA2); // Sleep Timer Control 
SFRX(X_WOREVT0,  0xDFA3); // Timer Sleep Timer Event 0 Timeout Low Byte
SFRX(X_WOREVT1,  0xDFA4); // Timer Sleep Timer Event 0 Timeout High Byte
SFRX(X_WORTIME0, 0xDFA5); // Sleep Timer Value 0 
SFRX(X_WORTIME1, 0xDFA6); // Sleep Timer Value 1 
SFRX(X_FWT,      0xDFAB); // Flash Write Timing
SFRX(X_FADDRL,   0xDFAC); // Flash Address Low
SFRX(X_FADDRH,   0xDFAD); // Flash Address High
SFRX(X_FCTL,     0xDFAE); // Flash Control
SFRX(X_FWDATA,   0xDFAF); // Flash Write Data
SFRX(X_ENCDI,    0xDFB1); // Encryption/Decryption Input Data
SFRX(X_ENCDO,    0xDFB2); // Encryption/Decryption Output Data
SFRX(X_ENCCS,    0xDFB3); // Encryption/Decryption Control and Status
SFRX(X_ADCCON1,  0xDFB4); // ADC Control 1
SFRX(X_ADCCON2,  0xDFB5); // ADC Control 2
SFRX(X_ADCCON3,  0xDFB6); // ADC Control 3
SFRX(X_ADCL,     0xDFBA); // ADC Data Low
SFRX(X_ADCH,     0xDFBB); // ADC Data High
SFRX(X_RNDL,     0xDFBC); // Random Number Generator Data Low
SFRX(X_RNDH,     0xDFBD); // Random Number Generator Data High
SFRX(X_SLEEP,    0xDFBE); // Flash Write Timing
SFRX(X_U0DBUF,   0xDFC1); // USART 0 Receive/Transmit Data Buffer
SFRX(X_U0BAUD,   0xDFC2); // USART 0 Baud Rate Control
SFRX(X_U0UCR,    0xDFC4); // USART 0 UART Control
SFRX(X_U0GCR,    0xDFC5); // USART 0 Generic Control
SFRX(X_CLKCON,   0xDFC6); // Clock Control
SFRX(X_MEMCTR,   0xDFC7); // Memory System Control
SFRX(X_WDCTL,    0xDFC9); // Watchdog Timer Control
SFRX(X_T3CNT,    0xDFCA); // Timer 3 Counter
SFRX(X_T3CTL,    0xDFCB); // Timer 3 Control
SFRX(X_T3CCTL0,  0xDFCC); // Timer 3 Channel 0 Capture/Compare Control
SFRX(X_T3CC0,    0xDFCD); // Timer 3 Channel 0 Capture/Compare Value
SFRX(X_T3CCTL1,  0xDFCE); // Timer 3 Channel 1 Capture/Compare Control
SFRX(X_T3CC1,    0xDFCF); // Timer 3 Channel 1 Capture/Compare Value
SFRX(X_DMAIRQ,   0xDFD1); // DMA Interrupt Flag
SFRX(X_DMA1CFGL, 0xDFD2); // DMA Channel 1-4 Configuration Address Low Byte
SFRX(X_DMA1CFGH, 0xDFD3); // DMA Channel 1-4 Configuration Address High Byte
SFRX(X_DMA0CFGL, 0xDFD4); // DMA Channel 0 Configuration Address Low Byte
SFRX(X_DMA0CFGH, 0xDFD5); // DMA Channel 0 Configuration Address High Byte
SFRX(X_DMAARM,   0xDFD6); // DMA Channel Arm
SFRX(X_DMAREQ,   0xDFD7); // DMA Channel Start Request and Status
SFRX(X_TIMIF,    0xDFD8); // Timers 1/3/4 Joint Interrupt Mask/Flags
SFRX(X_RFD,      0xDFD9); // RF Data
SFRX(X_T1CC0L,   0xDFDA); // Timer 1 Channel 0 Capture/Compare Value Low
SFRX(X_T1CC0H,   0xDFDB); // Timer 1 Channel 0 Capture/Compare Value High
SFRX(X_T1CC1L,   0xDFDC); // Timer 1 Channel 1 Capture/Compare Value Low
SFRX(X_T1CC1H,   0xDFDD); // Timer 1 Channel 1 Capture/Compare Value High
SFRX(X_T1CC2L,   0xDFDE); // Timer 1 Channel 2 Capture/Compare Value Low
SFRX(X_T1CC2H,   0xDFDF); // Timer 1 Channel 2 Capture/Compare Value High
SFRX(X_RFST,     0xDFE1); // RF Strobe Commands
SFRX(X_T1CNTL,   0xDFE2); // Timer 1 Counter Low
SFRX(X_T1CNTH,   0xDFE3); // Timer 1 Counter High
SFRX(X_T1CTL,    0xDFE4); // Timer 1 Control and Status
SFRX(X_T1CCTL0,  0xDFE5); // Timer 1 Channel 0 Capture/Compare Control
SFRX(X_T1CCTL1,  0xDFE6); // Timer 1 Channel 1 Capture/Compare Control
SFRX(X_T1CCTL2,  0xDFE7); // Timer 1 Channel 2 Capture/Compare Control
SFRX(X_RFIF,     0xDFE9); // RF Interrupt flags
SFRX(X_T4CNT,    0xDFEA); // Timer 4 Counter
SFRX(X_T4CTL,    0xDFEB); // Timer 4 Control
SFRX(X_T4CCTL0,  0xDFEC); // Timer 4 Channel 0 Capture/Compare Control
SFRX(X_T4CC0,    0xDFED); // Timer 4 Channel 0 Capture/Compare Value
SFRX(X_T4CCTL1,  0xDFEE); // Timer 4 Channel 1 Capture/Compare Control
SFRX(X_T4CC1,    0xDFEF); // Timer 4 Channel 1 Capture/Compare Value
SFRX(X_PERCFG,   0xDFF1); // Peripheral I/O Control
SFRX(X_ADCCFG,   0xDFF2); // ADC Input Configuration
SFRX(X_P0SEL,    0xDFF3); // Port 0 Function Select
SFRX(X_P1SEL,    0xDFF4); // Port 1 Function Select
SFRX(X_P2SEL,    0xDFF5); // Port 2 Function Select
SFRX(X_P1INP,    0xDFF6); // Port 1 Input Mode
SFRX(X_P2INP,    0xDFF7); // Port 2 Input Mode
SFRX(X_U1CSR,    0xDFF8); // USART 1 Control and Status
SFRX(X_U1DBUF,   0xDFF9); // USART 1 Receive/Transmit Data Buffer
SFRX(X_U1BAUD,   0xDFFA); // USART 1 Baud Rate Control
SFRX(X_U1UCR,    0xDFFB); // USART 1 UART Control
SFRX(X_U1GCR,    0xDFFC); // USART 1 Generic Control
SFRX(X_P0DIR,    0xDFFD); // Port 0 Direction
SFRX(X_P1DIR,    0xDFFE); // Port 1 Direction
SFRX(X_P2DIR,    0xDFFF); // Port 2 Direction

#endif //REG_CC2510FX_H

