/*-------------------------------------------------------------------------
   SST89x5xRDx.h Register Declarations for SST SST89E516RD2, ST89E516RD,
   SST89V516RD2, and SST89V516RD Processors
   (Based on datasheed S71273-03-000 1/07)

   Copyright (C) 2007, Jesus Calvino-Fraga / jesusc at ece.ubc.ca

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

#ifndef REG_SST89x5xRDx_H
#define REG_SST89x5xRDx_H

#include <compiler.h>

// From TABLE 3-5: CPU related SFRs

SFR(ACC, 0xE0); // Accumulator
   SBIT(ACC_0, 0xE0, 0); // Accumulator bit 0
   SBIT(ACC_1, 0xE0, 1); // Accumulator bit 1
   SBIT(ACC_2, 0xE0, 2); // Accumulator bit 2
   SBIT(ACC_3, 0xE0, 3); // Accumulator bit 3
   SBIT(ACC_4, 0xE0, 4); // Accumulator bit 4
   SBIT(ACC_5, 0xE0, 5); // Accumulator bit 5
   SBIT(ACC_6, 0xE0, 6); // Accumulator bit 6
   SBIT(ACC_7, 0xE0, 7); // Accumulator bit 7
SFR(B,   0xF0); // B Register
   SBIT(B_0, 0xF0, 0); // Register B bit 0
   SBIT(B_1, 0xF0, 1); // Register B bit 1
   SBIT(B_2, 0xF0, 2); // Register B bit 2
   SBIT(B_3, 0xF0, 3); // Register B bit 3
   SBIT(B_4, 0xF0, 4); // Register B bit 4
   SBIT(B_5, 0xF0, 5); // Register B bit 5
   SBIT(B_6, 0xF0, 6); // Register B bit 6
   SBIT(B_7, 0xF0, 7); // Register B bit 7
SFR(PSW, 0xD0); // Program Status Word
   SBIT(P,   0xD0, 0); // Parity Flag
   SBIT(F1,  0xD0, 1); // User-Defined Flag
   SBIT(OV,  0xD0, 2); // Overflow Flag
   SBIT(RS0, 0xD0, 3); // Register Bank Select 0
   SBIT(RS1, 0xD0, 4); // Register Bank Select 1
   SBIT(F0,  0xD0, 5); // User-Defined Flag
   SBIT(AC,  0xD0, 6); // Auxiliary Carry Flag
   SBIT(CY,  0xD0, 7); // Carry Flag
SFR(SP,  0x81); // Stack Pointer
SFR(DPL, 0x82); // Data Pointer Low
SFR(DPH, 0x83); // Data Pointer High
SFR(IE,  0xA8); // Interrupt Enable
   SBIT(EA,   0xA8, 7); // Global Interrupt Enable
   SBIT(EC,   0xA8, 6); // PCA Interrupt Enable
   SBIT(ET2,  0xA8, 5); // Timer 2 Interrupt Enable
   SBIT(ES,   0xA8, 4); // Serial Interrupt Enable
   SBIT(ET1,  0xA8, 3); // Timer 1 Interrupt Enable
   SBIT(EX1,  0xA8, 2); // External 1 Interrupt Enable
   SBIT(ET0,  0xA8, 1); // Timer 0 Interrupt Enable
   SBIT(EX0,  0xA8, 0); // External 0 Interrupt Enable
SFR(IEA, 0xE8); // Interrupt Enable A
   SBIT(EBO,  0xE8, 3); // Brown-out Interrupt Enable. (Vector is 0x00b4)
SFR(IP,  0xB8); // Interrupt Priority Reg
   SBIT(PPC,  0xB8, 6); // PCA interrupt priority bit
   SBIT(PT2,  0xB8, 5); // Timer 2 interrupt priority bit
   SBIT(PS,   0xB8, 4); // Serial Port interrupt priority bit
   SBIT(PT1,  0xB8, 3); // Timer 1 interrupt priority bit
   SBIT(PX1,  0xB8, 2); // External interrupt 1 priority bit
   SBIT(PT0,  0xB8, 1); // Timer 0 interrupt priority bit
   SBIT(PX0,  0xB8, 0); // External interrupt 0 priority bit
SFR(IPH, 0xB7); // Interrupt Priority Reg High
   #define PPCH 0x40   // PCA Interrupt Priority High Bit
   #define PT2H 0x20   // Timer 2 Interrupt Interrupt Priority High Bit
   #define PSH  0x10   // Serial Port Interrupt Priority High Bit
   #define PT1H 0x08   // Timer 1 Interrupt Priority High Bit
   #define PX1H 0x04   // External Interrupt 1 Priority High Bit
   #define PT0H 0x02   // Timer 0 Interrupt Priority High Bit
   #define PX0H 0x01   // External Interrupt 0 Priority High Bit
SFR(IP1, 0xF8); // Interrupt Priority Reg A
   SBIT(PBO, 0xF8, 4);   // Brown-out interrupt priority bit
   SBIT(PX2, 0xF8, 1);   // External Interrupt 2 priority bit
   SBIT(PX3, 0xF8, 2);   // External Interrupt 3 priority bit
SFR(IP1H, 0xF7); // Interrupt Priority Reg A High
   #define PBOH 0x08   // Brown-out Interrupt priority bit high
   #define PX2H 0x02   // External Interrupt 2 priority bit high
   #define PX3H 0x04   // External Interrupt 3 priority bit high
SFR(PCON, 0x87); // Power Control
   #define SMOD1 0x80  // Double Baud rate bit
   #define SMOD0 0x40  // FE/SM0 Selection bit
   #define BOF   0x20  // Brown-out detection status bit
   #define POF   0x10  // Power-on reset status bit
   #define GF1   0x08  // General-purpose flag bit
   #define GF0   0x04  // General-purpose flag bit
   #define PD    0x02  // Power-down bit
   #define IDL   0x01  // Idle mode bit
SFR(AUXR, 0x8E); // Auxiliary Reg
   #define EXTRAM 0x02 // Internal/External RAM access
   #define AO     0x01 // Disable/Enable ALE
SFR(AUXR1, 0xA2); // Auxiliary Reg 1
   #define GF2    0x08 // General purpose user-defined flag
   #define DPS    0x01 // DPTR registers select bit
SFR(XICON, 0xAE); // External Interrupt Control
   #define EX2 0x04
   #define IE2 0x02
   #define IT2 0x01
   #define EX3 0x40
   #define IE3 0x20
   #define IT3 0x10

// TABLE 3-6: Flash Memory Programming SFRs

SFR(SFCF, 0xB1); // SuperFlash Configuration
   #define IAPEN  0x40 // Enable IAP operation
   #define SWR    0x02 // Software Reset
   #define BSEL   0x01 // Program memory block switching bit
SFR(SFCM, 0xB2); // SuperFlash Command
   #define FIE                 0x80 // Flash Interrupt Enable
   #define CHIP_ERASE          0x01
   #define SECTOR_ERASE        0x0B
   #define BLOCK_ERASE         0x0D
   #define BYTE_VERIFY         0x0C
   #define BYTE_PROGRAM        0x0E
   #define PROG_SB1            0x0F
   #define PROG_SB2            0x03
   #define PROG_SB3            0x05
   #define PROG_SC0            0x09
   #define ENABLE_CLOCK_DOUBLE 0x08
SFR(SFAL, 0xB3); // SuperFlash Address Low Register - A7 to A0
SFR(SFAH, 0xB4); // SuperFlash Address High Register - A15 to A8
SFR(SFDT, 0xB5); // SuperFlash Data Register
SFR(SFST, 0xB6); // SuperFlash Status
   #define SB1_i      0x80 // Security Bit 1 status (inverse of SB1 bit)
   #define SB2_i      0x40 // Security Bit 2 status (inverse of SB2 bit)
   #define SB3_i      0x20 // Security Bit 3 status (inverse of SB3 bit)
   #define EDC_i      0x08 // Double Clock Status
   #define FLASH_BUSY 0x04 // Flash operation completion polling bit

// TABLE 3-7: Watchdog Timer SFRs

SFR(WDTC, 0xC0); // Watchdog Timer Control
   SBIT(WDOUT, 0xC0, 4); // Watchdog output enable
   SBIT(WDRE,  0xC0, 3); // Watchdog timer reset enable
   SBIT(WDTS,  0xC0, 2); // Watchdog timer reset flag
   SBIT(WDT,   0xC0, 1); // Watchdog timer refresh
   SBIT(SWDT,  0xC0, 0); // Start watchdog timer
SFR(WDTD, 0x85); // Watchdog Timer Data/Reload

// TABLE 3-8: Timer/Counters SFRs

SFR(TMOD,   0x89); // Timer/Counter Mode Control GATE C/T# M1 M0 GATE C/T# M1 M0
   #define GATE1 0x80 // External enable for timer 1
   #define C_T1  0x40 // Timer or counter select for timer 1
   #define M1_1  0x20 // Operation mode bit 1 for timer 1
   #define M0_1  0x10 // Operation mode bit 0 for timer 1
   #define GATE0 0x08 // External enable for timer 0
   #define C_T0  0x04 // Timer or counter select for timer 0
   #define M1_0  0x02 // Operation mode bit 1 for timer 0
   #define M0_0  0x01 // Operation mode bit 0 for timer 0
SFR(TCON,   0x88); // Timer/Counter Control  TF1 TR1 TF0 TR0 IE1 IT1 IE0 IT0
   SBIT(TF1, 0x88, 7); // Timer 1 overflow flag
   SBIT(TR1, 0x88, 6); // Timer 1 run control flag
   SBIT(TF0, 0x88, 5); // Timer 0 overflow flag
   SBIT(TR0, 0x88, 4); // Timer 0 run control flag
   SBIT(IE1, 0x88, 3); // Interrupt 1 flag
   SBIT(IT1, 0x88, 2); // Interrupt 1 type control bit
   SBIT(IE0, 0x88, 1); // Interrupt 0 flag
   SBIT(IT0, 0x88, 0); // Interrupt 0 type control bit
SFR(TH0,   0x8C); // Timer 0 MSB
SFR(TL0,   0x8A); // Timer 0 LSB
SFR(TH1,   0x8D); // Timer 1 MSB
SFR(TL1,   0x8B); // Timer 1 LSB
SFR(T2CON, 0xC8); // Timer / Counter 2 Control
   SBIT(TF2,   0xC8, 7); // Timer 2 overflow flag
   SBIT(EXF2,  0xC8, 6); // Timer 2 external flag
   SBIT(RCLK,  0xC8, 5); // Receive clock flag
   SBIT(TCLK,  0xC8, 4); // Transmit clock flag
   SBIT(EXEN2, 0xC8, 3); // Timer 2 external enable flag
   SBIT(TR2,   0xC8, 2); // Start/stop control for timer 2
   SBIT(C_T2,  0xC8, 1); // Timer or coutner select
   SBIT(CP_RL2,0xC8, 0); // Capture/reload flag
SFR(T2MOD,  0xC9); // Timer 2 Mode Control
   #define DCEN  0x02 // Down count enable bit
   #define T2OE  0x01 // Timer 2 output enable bit
SFR(TH2,    0xCD); // Timer 2 MSB
SFR(TL2,    0xCC); // Timer 2 LSB
SFR(RCAP2H, 0xCB); // Timer 2 Capture MSB
SFR(RCAP2L, 0xCA); // Timer 2 Capture LSB

// TABLE 3-9: Interface SFRs

SFR(SBUF, 0x99); // Serial Data Buffer
SFR(SCON, 0x98); // Serial Port Control
   SBIT(FE,  0x98, 7); // Framing Error when reading, SM0 when writing
   SBIT(SM0, 0x98, 7); // Serial Port Mode Bit 0
   SBIT(SM1, 0x98, 6); // Serial Port Mode Bit 1
   SBIT(SM2, 0x98, 5); // Serial Port Mode Bit 2
   SBIT(REN, 0x98, 4); // Enables serial reception
   SBIT(TB8, 0x98, 3); // The 9th data bit that will be transmitted in Modes 2 and 3
   SBIT(RB8, 0x98, 2); // In Modes 2 and 3, the 9th data bit that was received
   SBIT(TI,  0x98, 1); // Transmit interrupt flag
   SBIT(RI,  0x98, 0); // Receive interrupt flag
SFR(SADDR, 0xA9); // Slave Address
SFR(SADEN, 0xB9); // Slave Address Mask
SFR(SPCR,  0xD5); // SPI Control Register
   #define SPIE 0x80 // If both SPIE and ES are set to one, SPI interrupts are enabled
   #define SPE  0x40 // SPI enable bit.  When set enables SPI
   #define DORD 0x20 // Data trans. order. 0=MSB first; 1=LSB first
   #define MSTR 0x10 // 1=master mode.  0=slave mode
   #define CPOL 0x08 // 1=SCK is high when idle (active low), 0=SCK is low when idle (active high)
   #define CPHA 0x04 // 1=shift triggered on the trailing edge of SCK.  0=shift trig. on leading edge
   #define SPR1 0x02 // SPI Clork Rate select bit 1
   #define SPR0 0x01 // SPI Clork Rate select bit 0
                     // 00 = Fosc/4
                     // 01 = Fosc/16
                     // 10 = Fosc/64
                     // 11 = Fosc/128
SFR(SPSR, 0xAA); // SPI Status Register
   #define SPIF 0x80 // SPI interrupt flag
   #define WCOL 0x40 // Write collision Flag
SFR(SPDR, 0x86); // SPI Data Register
SFR(P0, 0x80); // Port 0
   SBIT(P0_0, 0x80, 0); // Port 0 bit 0
   SBIT(P0_1, 0x80, 1); // Port 0 bit 1
   SBIT(P0_2, 0x80, 2); // Port 0 bit 2
   SBIT(P0_3, 0x80, 3); // Port 0 bit 3
   SBIT(P0_4, 0x80, 4); // Port 0 bit 4
   SBIT(P0_5, 0x80, 5); // Port 0 bit 5
   SBIT(P0_6, 0x80, 6); // Port 0 bit 6
   SBIT(P0_7, 0x80, 7); // Port 0 bit 7
SFR(P1, 0x90); // Port 1
   SBIT(P1_0, 0x90, 0); // Port 1 bit 0
   SBIT(P1_1, 0x90, 1); // Port 1 bit 1
   SBIT(P1_2, 0x90, 2); // Port 1 bit 2
   SBIT(P1_3, 0x90, 3); // Port 1 bit 3
   SBIT(P1_4, 0x90, 4); // Port 1 bit 4
   SBIT(P1_5, 0x90, 5); // Port 1 bit 5
   SBIT(P1_6, 0x90, 6); // Port 1 bit 6
   SBIT(P1_7, 0x90, 7); // Port 1 bit 7
   // Alternate names
   SBIT(T2,   0x90, 0); // Port 1 bit 0
   SBIT(T2EX, 0x90, 1); // Port 1 bit 1
   SBIT(ECI,  0x90, 2); // Port 1 bit 2
   SBIT(CEX0, 0x90, 3); // Port 1 bit 3
   SBIT(CEX1, 0x90, 4); // Port 1 bit 4
   SBIT(CEX2, 0x90, 5); // Port 1 bit 5
   SBIT(CEX3, 0x90, 6); // Port 1 bit 6
   SBIT(CEX4, 0x90, 7); // Port 1 bit 7
   // More alternate names
   SBIT(SS,   0x90, 4); // Port 1 bit 4
   SBIT(MOSI, 0x90, 5); // Port 1 bit 5
   SBIT(MISO, 0x90, 6); // Port 1 bit 6
   SBIT(SCK,  0x90, 7); // Port 1 bit 7
SFR(P2, 0xA0); // Port 2
   SBIT(P2_0, 0xA0, 0); // Port 2 bit 0
   SBIT(P2_1, 0xA0, 1); // Port 2 bit 1
   SBIT(P2_2, 0xA0, 2); // Port 2 bit 2
   SBIT(P2_3, 0xA0, 3); // Port 2 bit 3
   SBIT(P2_4, 0xA0, 4); // Port 2 bit 4
   SBIT(P2_5, 0xA0, 5); // Port 2 bit 5
   SBIT(P2_6, 0xA0, 6); // Port 2 bit 6
   SBIT(P2_7, 0xA0, 7); // Port 2 bit 7
SFR(P3, 0xB0); // Port 3
   SBIT(P3_0, 0xB0, 0); // Port 2 bit 0
   SBIT(P3_1, 0xB0, 1); // Port 2 bit 1
   SBIT(P3_2, 0xB0, 2); // Port 2 bit 2
   SBIT(P3_3, 0xB0, 3); // Port 2 bit 3
   SBIT(P3_4, 0xB0, 4); // Port 2 bit 4
   SBIT(P3_5, 0xB0, 5); // Port 2 bit 5
   SBIT(P3_6, 0xB0, 6); // Port 2 bit 6
   SBIT(P3_7, 0xB0, 7); // Port 2 bit 7
   // Alternate names
   SBIT(RXD,  0xB0, 0); // Port 2 bit 0
   SBIT(TXD,  0xB0, 1); // Port 2 bit 1
   SBIT(INT0, 0xB0, 2); // Port 2 bit 2
   SBIT(INT1, 0xB0, 3); // Port 2 bit 3
   SBIT(T0,   0xB0, 4); // Port 2 bit 4
   SBIT(T1,   0xB0, 5); // Port 2 bit 5
   SBIT(WR,   0xB0, 6); // Port 2 bit 6
   SBIT(RD,   0xB0, 7); // Port 2 bit 7
SFR(P4, 0xA5); // Port 4 - not bit addressable
   #define P4_0 0x01
   #define P4_1 0x02
   #define P4_2 0x04
   #define P4_3 0x08

// TABLE 3-10: PCA SFRs

SFR(CH, 0xF9); // PCA Timer/Counter High
SFR(CL, 0xE9); // PCA Timer/Counter Low
SFR(CCON, 0xD8); // PCA Timer/Counter Control Register  CF CR - CCF4 CCF3 CCF2 CCF1 CCF0 00x00000b
   SBIT(CF,   0xD8, 7); // PCA Counter overflow flag
   SBIT(CR,   0xD8, 6); // PCA Counter Run Control Bit
   SBIT(CCF4, 0xD8, 4); // PCA Module 4 Interrupt Flag
   SBIT(CCF3, 0xD8, 3); // PCA Module 3 Interrupt Flag
   SBIT(CCF2, 0xD8, 2); // PCA Module 2 Interrupt Flag
   SBIT(CCF1, 0xD8, 1); // PCA Module 1 Interrupt Flag
   SBIT(CCF0, 0xD8, 0); // PCA Module 0 Interrupt Flag
SFR(CMOD, 0xD9); // PCA Timer/Counter Mode Register
   #define CIDL 0x80 // CIDL=0 program the PCA counter to work during idle mode
   #define WDTE 0x40 // Watchdog Timer Enable
   #define CPS1 0x04 // PCA Count Pulse Select bit 1
   #define CPS0 0x02 // PCA Count Pulse Select bit 0
                     // 00=Internal clock, Fosc/6
                     // 01=Internal clock, Fosc/6
                     // 10=Timer 0 overflow
                     // 11=External clock at ECI/P1.2 pin (max rate=Fosc/4)
   #define ECF 0x01  // PCA Enable Counter Overflow Interrupt
SFR(CCAP0H, 0xFA); // PCA Module 0 Compare/Capture Register High
SFR(CCAP0L, 0xEA); // PCA Module 0 Compare/Capture Register Low
SFR(CCAP1H, 0xFB); // PCA Module 1 Compare/Capture Register High
SFR(CCAP1L, 0xEB); // PCA Module 1 Compare/Capture Register Low
SFR(CCAP2H, 0xFC); // PCA Module 2 Compare/Capture Register High
SFR(CCAP2L, 0xEC); // PCA Module 2 Compare/Capture Register Low
SFR(CCAP3H, 0xFD); // PCA Module 3 Compare/Capture Register High
SFR(CCAP3L, 0xED); // PCA Module 3 Compare/Capture Register Low
SFR(CCAP4H, 0xFE); // PCA Module 4 Compare/Capture Register High
SFR(CCAP4L, 0xEE); // PCA Module 4 Compare/Capture Register Low
SFR(CCAPM0, 0xDA); // PCA Compare/Capture Module 0 Mode Register
SFR(CCAPM1, 0xDB); // PCA Compare/Capture Module 1 Mode Register
SFR(CCAPM2, 0xDC); // PCA Compare/Capture Module 2 Mode Register
SFR(CCAPM3, 0xDD); // PCA Compare/Capture Module 3 Mode Register
SFR(CCAPM4, 0xDE); // PCA Compare/Capture Module 4 Mode Register
// The preceding five registers have the following bits:
   #define ECOM 0x40 // Enable Comparator
   #define CAPP 0x20 // 1=enables positive edge capture
   #define CAPN 0x10 // 1=enables negative edge capture
   #define MAT  0x08 // When counter matches sets CCFn bit causing and interrupt
   #define TOG  0x04 // Toggle output on match
   #define PWM  0x02 // Pulse width modulation mode
   #define ECCF 0x01 // Enable CCF interrupt

#endif /*REG_SST89x5xRDx_H*/
