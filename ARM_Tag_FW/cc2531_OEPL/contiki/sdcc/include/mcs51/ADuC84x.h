/*-------------------------------------------------------------------------
   ADuC84x.h: Register Declarations for Analog Devices
   ADuC841/ADuC842/ADuC843 (Based on datasheet Rev 0, 11/2003 )

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

#ifndef REG_ADUC84X_H
#define REG_ADUC84X_H

#include <compiler.h>

SFR(CFG841, 0xAF); // ADuC841 Configuration SFR.
SFR(CFG842, 0xAF); // ADuC842/ADuC843 Configuration SFR.
	//For the two SFRs above these are the bits.  Warning: some are different for
	//the ADuC841 and ADuC842/ADuC843.  Check the datasheet!
    #define EPM2   0x10 // Flash/EE Controller and PWM Clock Frequency Configuration Bit 2.
    #define EPM1   0x08 // Flash/EE Controller and PWM Clock Frequency Configuration Bit 1.
    #define EPM0   0x04 // Flash/EE Controller and PWM Clock Frequency Configuration Bit 0.
    #define EXSP   0x80 // Extended SP Enable.
    #define PWPO   0x40 // PWM Pin Out Selection.
    #define DBUF   0x20 // DAC Output Buffer.
    #define EXTCLK 0x10 // Set by the user to 1 to select an external clock input on P3.4.
    #define MSPI   0x02 // Set to 1 by the user to move the SPI functionality of.
    #define XRAMEN 0x01 // XRAM Enable Bit.

SFR(SP,    0x81); // Stack Pointer.
SFR(SPH,   0xB7); // Stack pointer high.

SFR(DPL,   0x82); // Data Pointer Low.
SFR(DPH,   0x83); // Data Pointer High.
SFR(DPP,   0x84); // Data Pointer Page Byte.
SFR(DPCON, 0xA7); // Data Pointer Control SFR.

SFR(PCON, 0x87); // Power Control.
   #define SMOD   0x80 //Double UART Baud Rate.
   #define SERIPD 0x40 //I2C/SPI Power-Down Interrupt Enable.
   #define INT0PD 0x20 //INT0 Power-Down Interrupt Enable.
   #define ALEOFF 0x10 //Disable ALE Output.
   #define GF1    0x08 //General-Purpose Flag Bit.
   #define GF0    0x04 //General-Purpose Flag Bit.
   #define PD     0x02 //Power-Down Mode Enable.
   #define IDL    0x01 //Idle Mode Enable.

SFR(TCON, 0x88); // Timer/Counter Control.
    SBIT(TF1, 0x88, 7); // Timer 1 overflow flag.
    SBIT(TR1, 0x88, 6); // Timer 1 run control flag.
    SBIT(TF0, 0x88, 5); // Timer 0 overflow flag.
    SBIT(TR0, 0x88, 4); // Timer 0 run control flag.
    SBIT(IE1, 0x88, 3); // Interrupt 1 flag.
    SBIT(IT1, 0x88, 2); // Interrupt 1 type control bit.
    SBIT(IE0, 0x88, 1); // Interrupt 0 flag.
    SBIT(IT0, 0x88, 0); // Interrupt 0 type control bit.

SFR(TMOD, 0x89); // Timer/Counter Mode Control.
   #define GATE1 0x80 // External enable for timer 1.
   #define C_T1  0x40 // Timer or counter select for timer 1.
   #define M1_1  0x20 // Operation mode bit 1 for timer 1.
   #define M0_1  0x10 // Operation mode bit 0 for timer 1.
   #define GATE0 0x08 // External enable for timer 0.
   #define C_T0  0x04 // Timer or counter select for timer 0.
   #define M1_0  0x02 // Operation mode bit 1 for timer 0.
   #define M0_0  0x01 // Operation mode bit 0 for timer 0.

SFR(TL0, 0x8A); // Timer 0 LSB.
SFR(TL1, 0x8B); // Timer 1 LSB.
SFR(TH0, 0x8C); // Timer 0 MSB.
SFR(TH1, 0x8D); // Timer 1 MSB.

SFR(P0, 0x80); // Port 0
    SBIT(P0_0, 0x80, 0); // Port 0 bit 0.
    SBIT(P0_1, 0x80, 1); // Port 0 bit 1.
    SBIT(P0_2, 0x80, 2); // Port 0 bit 2.
    SBIT(P0_3, 0x80, 3); // Port 0 bit 3.
    SBIT(P0_4, 0x80, 4); // Port 0 bit 4.
    SBIT(P0_5, 0x80, 5); // Port 0 bit 5.
    SBIT(P0_6, 0x80, 6); // Port 0 bit 6.
    SBIT(P0_7, 0x80, 7); // Port 0 bit 7.

SFR(P1, 0x90); // Port 1
    SBIT(P1_0, 0x90, 0); // Port 1 bit 0.
    SBIT(P1_1, 0x90, 1); // Port 1 bit 1.
    SBIT(P1_2, 0x90, 2); // Port 1 bit 2.
    SBIT(P1_3, 0x90, 3); // Port 1 bit 3.
    SBIT(P1_4, 0x90, 4); // Port 1 bit 4.
    SBIT(P1_5, 0x90, 5); // Port 1 bit 5.
    SBIT(P1_6, 0x90, 6); // Port 1 bit 6.
    SBIT(P1_7, 0x90, 7); // Port 1 bit 7.
    // Alternate names
    SBIT(T2EX, 0x90, 1); //Capture/reload trigger for Counter 2.
    SBIT(T2,   0x90, 0); //Input to Timer/Counter 2.

SFR(P2, 0xA0); // Port 2
   SBIT(P2_0, 0xA0, 0); // Port 2 bit 0.
   SBIT(P2_1, 0xA0, 1); // Port 2 bit 1.
   SBIT(P2_2, 0xA0, 2); // Port 2 bit 2.
   SBIT(P2_3, 0xA0, 3); // Port 2 bit 3.
   SBIT(P2_4, 0xA0, 4); // Port 2 bit 4.
   SBIT(P2_5, 0xA0, 5); // Port 2 bit 5.
   SBIT(P2_6, 0xA0, 6); // Port 2 bit 6.
   SBIT(P2_7, 0xA0, 7); // Port 2 bit 7.

SFR(P3, 0xB0); // Port 3
   SBIT(P3_0, 0xB0, 0); // Port 3 bit 0.
   SBIT(P3_1, 0xB0, 1); // Port 3 bit 1.
   SBIT(P3_2, 0xB0, 2); // Port 3 bit 2.
   SBIT(P3_3, 0xB0, 3); // Port 3 bit 3.
   SBIT(P3_4, 0xB0, 4); // Port 3 bit 4.
   SBIT(P3_5, 0xB0, 5); // Port 3 bit 5.
   SBIT(P3_6, 0xB0, 6); // Port 3 bit 6.
   SBIT(P3_7, 0xB0, 7); // Port 3 bit 7.
   // Alternate names.
   SBIT(RXD,  0xB0, 0);
   SBIT(TXD,  0xB0, 1);
   SBIT(INT0, 0xB0, 2);
   SBIT(INT1, 0xB0, 3);
   SBIT(T0,   0xB0, 4);
   SBIT(T1,   0xB0, 5);
   SBIT(WR,   0xB0, 6);
   SBIT(RD,   0xB0, 7);

SFR(I2CADD,  0x9B); // I2C Address Register 1.
SFR(I2CADD1, 0x91); // I2C Address Register 2.
SFR(I2CADD2, 0x92); // I2C Address Register 3.
SFR(I2CADD3, 0x93); // I2C Address Register 4.
SFR(I2CDAT,  0x9A); // I2C Data Register.

SFR(SCON, 0x98); // Serial Port Control.
   SBIT(SM0, 0x98, 7); // Serial Port Mode Bit 0.
   SBIT(SM1, 0x98, 6); // Serial Port Mode Bit 1.
   SBIT(SM2, 0x98, 5); // Serial Port Mode Bit 2.
   SBIT(REN, 0x98, 4); // Enables serial reception.
   SBIT(TB8, 0x98, 3); // The 9th data bit that will be transmitted in Modes 2 and 3.
   SBIT(RB8, 0x98, 2); // In Modes 2 and 3, the 9th data bit that was received.
   SBIT(TI,  0x98, 1); // Transmit interrupt flag.
   SBIT(RI,  0x98, 0); // Receive interrupt flag.

SFR(SBUF, 0x99); // Serial Data Buffer.

//For these two, you may want to check page 68 of the datasheet...
SFR(T3FD,  0x9D);       // Fractional divider ratio.
SFR(T3CON, 0x9E);       // T3CON is the baud rate control SFR, allowing Timer 3 to be
                        // used to set up the UART baud rate, and setting up the binary
                        // divider (DIV).
#define T3BAUDEN 0x80   // T3 UART BAUD Enable.
#define DIV2 0x04       // Binary Divider Factor bit 3.
#define DIV1 0x02       // Binary Divider Factor bit 2.
#define DIV0 0x01       // Binary Divider Factor bit 1.

SFR(TIMECON, 0xA1); // TIC Control Register.
    #define TFH  0x40 // Twenty-Four Hour Select Bit.
    #define ITS1 0x20 // Interval Timebase Selection Bit 1.
    #define ITS0 0x10 // Interval Timebase Selection Bit 1.
    #define STI  0x08 // Single Time Interval Bit.
    #define TII  0x04 // TIC Interrupt Bit.
    #define TIEN 0x02 // Time Interval Enable Bit.
    #define TCEN 0x01 // Time Clock Enable Bit.

SFR(HTHSEC, 0xA2); // Hundredths Seconds Time Register.
SFR(SEC,    0xA3); // Seconds Time Register.
SFR(MIN,    0xA4); // Minutes Time Register.
SFR(HOUR,   0xA5); // Hours Time Register.
SFR(INTVAL, 0xA6); // User Time Interval Select Register.

SFR(IE,  0xA8); // Interrupt Enable.
   SBIT(EA,   0xA8, 7); // Global Interrupt Enable.
   SBIT(EADC, 0xA8, 6); // ADC Interrupt Enable.
   SBIT(ET2,  0xA8, 5); // Timer 2 Interrupt Enable.
   SBIT(ES,   0xA8, 4); // Serial Interrupt Enable.
   SBIT(ET1,  0xA8, 3); // Timer 1 Interrupt Enable.
   SBIT(EX1,  0xA8, 2); // External 1 Interrupt Enable.
   SBIT(ET0,  0xA8, 1); // Timer 0 Interrupt Enable.
   SBIT(EX0,  0xA8, 0); // External 0 Interrupt Enable.

SFR(IEIP2, 0xA9); // Secondary Interrupt Enable Register
    #define PTI   0x40 // Priority for time interval interrupt.
    #define PPSM  0x20 // Priority for power supply monitor interrupt.
    #define PSI2  0x10 // Priority for SPI/I2C interrupt. Same name as bit in IP???
    #define ETI   0x04 // Enable time interval counter interrupts.
    #define EPSMI 0x02 // Enable power supply monitor interrupts.
    #define ESI   0x01 // Enable SPI or I2C serial port interrupts.

SFR(PWMCON, 0xAE); //PWM control.
    #define SNGL  0x80 // Turns off PMW output at P2.6 or P3.4.
    #define MD2   0x40 // PWM Mode Bit 2.
    #define MD1   0x20 // PWM Mode Bit 2.
    #define MD0   0x10 // PWM Mode Bit 2.
    #define CDIV1 0x08 // PWM Clock Divider bit 1.
    #define CDIV0 0x04 // PWM Clock Divider bit 1.
    #define CSEL1 0x02 // PWM Clock Select bit 1. (Typo in datasheet???)
    #define CSEL0 0x01 // PWM Clock Select bit 0.

SFR(PWM0L, 0xB1); // PWM 0 duty cycle low.
SFR(PWM0H, 0xB2); // PWM 0 duty cycle high.
SFR(PWM1L, 0xB3); // PWM 1 duty cycle low.
SFR(PWM1H, 0xB4); // PWM 1 duty cycle high.

SFR(IP,  0xB8); // Interrupt Priority Reg.
   SBIT(PSI,  0xB8, 7); // Priority for SPI/I2C interrupt.
   SBIT(PADC, 0xB8, 6); // ADC interrupt priority bit.
   SBIT(PT2,  0xB8, 5); // Timer 2 interrupt priority bit.
   SBIT(PS,   0xB8, 4); // Serial Port interrupt priority bit.
   SBIT(PT1,  0xB8, 3); // Timer 1 interrupt priority bit.
   SBIT(PX1,  0xB8, 2); // External interrupt 1 priority bit.
   SBIT(PT0,  0xB8, 1); // Timer 0 interrupt priority bit.
   SBIT(PX0,  0xB8, 0); // External interrupt 0 priority bit.

SFR(ECON,   0xB9); // Flash/EE Memory Control SFR.
SFR(EDATA1, 0xBC); // EE page data byte 1.
SFR(EDATA2, 0xBD); // EE page data byte 2.
SFR(EDATA3, 0xBE); // EE page data byte 3.
SFR(EDATA4, 0xBF); // EE page data byte 4.
SFR(EADRL,  0xC6); // EE address low.
SFR(EADRH,  0xC7); // EE address high.

SFR(WDCON, 0xC0); //Watchdog Timer Control Register.
    SBIT(PRE3, 0xc0, 7); // Timeout period bit 4.
    SBIT(PRE2, 0xc0, 6); // Timeout period bit 3.
    SBIT(PRE1, 0xc0, 5); // Timeout period bit 2.
    SBIT(PRE0, 0xc0, 4); // Timeout period bit 1.
    SBIT(WDIR, 0xc0, 3); // Watchdog Interrupt Response Enable Bit.
    SBIT(WDS,  0xc0, 2); // Watchdog Status Bit.
    SBIT(WDE,  0xc0, 1); // Watchdog Enable Bit.
    SBIT(WDWR, 0xc0, 0); // Watchdog Write Enable Bit.

SFR(CHIPID, 0xC2); // System Self-Identification? WARNING: No description in the datasheet.

SFR(T2CON, 0xC8); // Timer / Counter 2 Control.
   SBIT(TF2,   0xC8, 7); // Timer 2 overflow flag.
   SBIT(EXF2,  0xC8, 6); // Timer 2 external flag.
   SBIT(RCLK,  0xC8, 5); // Receive clock flag.
   SBIT(TCLK,  0xC8, 4); // Transmit clock flag.
   SBIT(EXEN2, 0xC8, 3); // Timer 2 external enable flag.
   SBIT(TR2,   0xC8, 2); // Start/stop control for timer 2.
   SBIT(CNT2,  0xC8, 1); // Timer or coutner select.
   SBIT(CAP2,  0xC8, 0); // Capture/reload flag.

SFR(RCAP2L, 0xCA); // Timer 2 Capture LSB.
SFR(RCAP2H, 0xCB); // Timer 2 Capture MSB.
SFR(TL2,    0xCC); // Timer 2 LSB.
SFR(TH2,    0xCD); // Timer 2 MSB.

SFR(PSW, 0xD0); // Program Status Word.
   SBIT(CY,  0xD0, 7); // Carry Flag.
   SBIT(AC,  0xD0, 6); // Auxiliary Carry Flag.
   SBIT(F0,  0xD0, 5); // User-Defined Flag.
   SBIT(RS1, 0xD0, 4); // Register Bank Select 1.
   SBIT(RS0, 0xD0, 3); // Register Bank Select 0.
   SBIT(OV,  0xD0, 2); // Overflow Flag.
   SBIT(F1,  0xD0, 1); // User-Defined Flag.
   SBIT(P,   0xD0, 0); // Parity Flag.

SFR(DMAL,   0xD2); // DMA mode address pointer low.
SFR(DMAH,   0xD3); // DMA mode address pointer high.
SFR(DMAP,   0xD4); // DMA mode address pointer page. (?)
SFR(PLLCON, 0xD7); // PLL Control Register.

SFR(PSMCON, 0xDF); // Power supply monitor.
    #define CMPD  0x40 // DVDD Comparator Bit.
    #define PSMI  0x20 // Power Supply Monitor Interrupt Bit.
    #define TPD1  0x10 // DVDD Trip Point Selection Bit 2.
    #define TPD0  0x08 // DVDD Trip Point Selection Bit 1.
    #define PSMEN 0x01 // Power Supply Monitor Enable Bit.

SFR(ACC, 0xE0); // Accumulator
   SBIT(ACC_0, 0xE0, 0); // Accumulator bit 0.
   SBIT(ACC_1, 0xE0, 1); // Accumulator bit 1.
   SBIT(ACC_2, 0xE0, 2); // Accumulator bit 2.
   SBIT(ACC_3, 0xE0, 3); // Accumulator bit 3.
   SBIT(ACC_4, 0xE0, 4); // Accumulator bit 4.
   SBIT(ACC_5, 0xE0, 5); // Accumulator bit 5.
   SBIT(ACC_6, 0xE0, 6); // Accumulator bit 6.
   SBIT(ACC_7, 0xE0, 7); // Accumulator bit 7.

SFR(I2CCON, 0xE8); // I2C Control Register
    // Master mode
    SBIT(MDO,    0xE8, 7); // I2C Software Master Data Output Bit.
    SBIT(MDE,    0xE8, 6); // I2C Software Master Data Output Enable Bit.
    SBIT(MCO,    0xE8, 5); // I2C Software Master Clock Output Bit.
    SBIT(MDI,    0xE8, 4); // I2C Software Master Data Input Bit.
    // Slave mode
    SBIT(I2CSI,  0xE8, 7); // I2C Stop Interrupt Enable Bit.
    SBIT(I2CGC,  0xE8, 6); // I2C General Call Status Bit.
    SBIT(I2CID1, 0xE8, 5); // I2C Interrupt Decode Bit 2.
    SBIT(I2CID0, 0xE8, 4); // I2C Interrupt Decode Bit 1.
    SBIT(I2CM,   0xE8, 3); // I2C Master/Slave Mode Bit.
    SBIT(I2CRS,  0xE8, 2); // I2C Reset Bit.
    SBIT(I2CTX,  0xE8, 1); // I2C Direction Transfer Bit.
    SBIT(I2CI,   0xE8, 0); // I2C Interrupt Bit.

SFR(B,   0xF0); // B Register
   SBIT(B_0, 0xF0, 0); // Register B bit 0.
   SBIT(B_1, 0xF0, 1); // Register B bit 1.
   SBIT(B_2, 0xF0, 2); // Register B bit 2.
   SBIT(B_3, 0xF0, 3); // Register B bit 3.
   SBIT(B_4, 0xF0, 4); // Register B bit 4.
   SBIT(B_5, 0xF0, 5); // Register B bit 5.
   SBIT(B_6, 0xF0, 6); // Register B bit 6.
   SBIT(B_7, 0xF0, 7); // Register B bit 7.

SFR(ADCCON1,  0xEF); //ADC Control SFR 1
	//WARNING: bit 7 is named MD1 in the datasheet, but that name is already used.
    #define MD      0x80 // The mode bit selects the active operating mode of the ADC.
    #define EXT_REF 0x40 // Set by the user to select an external reference.
    #define CK1     0x20 // The ADC clock divide bit 1.
    #define CK0     0x10 // The ADC clock divide bit 0.
    #define AQ1     0x08 // ADC acquisition select bit 1.
    #define AQ0     0x04 // ADC acquisition select bit 0.
    #define T2C     0x02 // The Timer 2 conversion bit.
    #define EXC     0x01 // The external trigger enable bit.

SFR(ADCCON2,  0xD8); // ADC Control SFR 2.
    SBIT(ADCI,  0xD8, 7); // ADC Interrupt Bit.
    SBIT(DMA,   0xD8, 6); // DMA Mode Enable Bit.
    SBIT(CCONV, 0xD8, 5); // Continuous Conversion Bit.
    SBIT(SCONV, 0xD8, 4); // Single Conversion Bit.
    SBIT(CS3,   0xD8, 3); // Channel Selection Bit 4.
    SBIT(CS2,   0xD8, 2); // Channel Selection Bit 3.
    SBIT(CS1,   0xD8, 1); // Channel Selection Bit 2.
    SBIT(CS0,   0xD8, 0); // Channel Selection Bit 1.

SFR(ADCCON3,  0xF5); //ADC Control SFR 3
    #define BUSY    0x80 // ADC Busy Status Bit.
    #define AVGS1   0x20 // Number of Average Selection Bit 2.
    #define AVGS0   0x10 // Number of Average Selection Bit 1.
    #define TYPICAL 0x02 // Calibration Type Select Bit.
    #define SCAL    0x01 // Start Calibration Cycle Bit.

SFR(ADCDATAL, 0xD9); // ADC Result byte low.
SFR(ADCDATAH, 0xDA); // ADC Result byte high.  WARNING: bits 4 to 7 are the ADC channel!
SFR(ADCOFSL,  0xF1); // Offset calibration coefficient byte (low).
SFR(ADCOFSH,  0xF2); // Offset calibration coefficient byte (high).
SFR(ADCGAINL, 0xF3); // Gain calibration coefficient byte (low).
SFR(ADCGAINH, 0xF4); // Gain calibration coefficient byte (high).

SFR(SPIDAT, 0xF7); // SPI Data Register.

SFR(SPICON, 0xF8); // SPI Control Register.
    SBIT(ISPI, 0xF8, 7); // SPI Interrupt Bit.
    SBIT(WCOL, 0xF8, 6); // Write Collision Error Bit.
    SBIT(SPE,  0xF8, 5); // SPI Interface Enable Bit.
    SBIT(SPIM, 0xF8, 4); // SPI Master/Slave Mode Select Bit.
    SBIT(CPOL, 0xF8, 3); // Clock Polarity Select Bit.
    SBIT(CPHA, 0xF8, 2); // Clock Phase Select Bit.
    SBIT(SPR1, 0xF8, 1); // SPI Bit Rate Select Bit 2.
    SBIT(SPR0, 0xF8, 0); // SPI Bit Rate Select Bit 1.

SFR(DAC0L,  0xF9); // DAC0 Data Low Byte.
SFR(DAC0H,  0xFA); // DAC0 Data High Byte.
SFR(DAC1L,  0xFB); // DAC1 Data Low Byte.
SFR(DAC1H,  0xFC); // DAC1 Data High Byte.
SFR(DACCON, 0xFD); // DAC Control Register.
    #define MODE 0x80 // 8/12 bit selection.
    #define RNG1 0x40 // DAC1 Range Select Bit.
    #define RNG0 0x20 // DAC0 Range Select Bit.
    #define CLR1 0x10 // DAC1 Clear Bit.
    #define CLR0 0x08 // DAC0 Clear Bit.
    #define SYNC 0x04 // DAC0/1 Update Synchronization Bit.
    #define PD1  0x02 // DAC1 Power-Down Bit.
    #define PD0  0x01 // DAC0 Power-Down Bit.

#endif //REG_ADUC84X_H
