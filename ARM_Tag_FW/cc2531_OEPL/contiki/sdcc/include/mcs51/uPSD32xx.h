/*-------------------------------------------------------------------------
   uPSD23xx.h - Register Declarations for ST's uPSD325x
   Based upon  uPSD33xx.h from SDCC compiler set, and
   uPSD3254/3253 datasheet - November 2004
   Register Declarations for ST's uPSD32xx "Fast 8032 MCU with Programmable Logic"

   Copyright (C) 2009, GHJ Morsink
   Original 33xx file Written By -  Jesus Calvino-Fraga / jesusc at ece.ubc.ca (May 2007)

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

#ifndef REG_UPSD32XX_H
#define REG_UPSD32XX_H

#include <compiler.h>

#ifndef PSD_CSIOP
#define PSD_CSIOP 0x00                /* for reference for PSD registers as default (page 99 datasheet) */
                                      /* This must be set in PSDSoft "Define PSD pin/node Functions"
                                         Step3 'Next', tab "Chip Select Equations"  */
#endif

//Interrupt vector numbers (see table 24 on datasheet page 39)
#define EXT0_INTERRUPT   ((0x03-3)/8)
#define TIMER0_INTERRUPT ((0x0B-3)/8)
#define EXT1_INTERRUPT   ((0x13-3)/8)
#define TIMER1_INTERRUPT ((0x1B-3)/8)
#define UART0_INTERRUPT  ((0x23-3)/8)
#define TIMER2_INTERRUPT ((0x2B-3)/8)
#define TX2_INTERRUPT    ((0x2B-3)/8)
#define USB_INTERRUPT    ((0x33-3)/8)
#define DDC_INTERRUPT    ((0x3B-3)/8)
#define I2C_INTERRUPT    ((0x43-3)/8)
#define UART1_INTERRUPT  ((0x4B-3)/8)


SFR(P0, 0x80); // Port 0
    SBIT(P0_0, 0x80, 0); // Port 0 bit 0.
    SBIT(P0_1, 0x80, 1); // Port 0 bit 1.
    SBIT(P0_2, 0x80, 2); // Port 0 bit 2.
    SBIT(P0_3, 0x80, 3); // Port 0 bit 3.
    SBIT(P0_4, 0x80, 4); // Port 0 bit 4.
    SBIT(P0_5, 0x80, 5); // Port 0 bit 5.
    SBIT(P0_6, 0x80, 6); // Port 0 bit 6.
    SBIT(P0_7, 0x80, 7); // Port 0 bit 7.


SFR(SP,   0x81); // Stack Pointer.
SFR(DPL,  0x82); // Data Pointer Low.
SFR(DPH,  0x83); // Data Pointer High.

SFR(DPTC, 0x85); // Data Pointer Control Register.
SFR(DPS,  0x85); // Data Pointer Control Register alias for SDCC
    #define AT 0x40 //0:Manually Select Data Pointer / 1:Auto Toggle between DPTR0 and DPTR1
    #define DPSE0 0x01 // 0:DPTR0 Selected for use as DPTR / 1:DPTR1 Selected for use as DPTR

SFR(PCON, 0x87); // Power Control.
    #define SMOD0  0x80 //Baud Rate Double Bit (UART0)
    #define SMOD1  0x40 //Baud Rate Double Bit (UART1)
    #define POR    0x10 //Only a power-on reset sets this bit (cold reset).
    #define RCLK1  0x08 //Receive Clock Flag (UART1)
    #define TCLK1  0x04 //Transmit Clock Flag (UART1)
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

SFR(P1, 0x90); // Port 1
    SBIT(P1_0, 0x90, 0); // Port 1 bit 0.
    SBIT(P1_1, 0x90, 1); // Port 1 bit 1.
    SBIT(P1_2, 0x90, 2); // Port 1 bit 2.
    SBIT(P1_3, 0x90, 3); // Port 1 bit 3.
    SBIT(P1_4, 0x90, 4); // Port 1 bit 4.
    SBIT(P1_5, 0x90, 5); // Port 1 bit 5.
    SBIT(P1_6, 0x90, 6); // Port 1 bit 6.
    SBIT(P1_7, 0x90, 7); // Port 1 bit 7.
    //Alternate names (from figure 3)
    SBIT(T2,     0x90, 0); //Input to Timer/Counter 2.
    SBIT(T2X,    0x90, 1); //Capture/reload trigger for Counter 2.
    SBIT(RXD1,   0x90, 2);
    SBIT(TXD1,   0x90, 3);
    SBIT(SPICLK, 0x90, 4);
    SBIT(SPIRXD, 0x90, 5);
    SBIT(SPITXD, 0x90, 6);
    SBIT(SPISEL, 0x90, 7);

SFR(P1SFS, 0x91); // Port 1 Special Function Select Register
    #define P1SF7 0x80
    #define P1SF6 0x40
    #define P1SF5 0x20
    #define P1SF4 0x10
    #define P1SF3 0x08
    #define P1SF2 0x04
    #define P1SF1 0x02
    #define P1SF0 0x01

SFR(P3SFS, 0x93); // Port 3 Special Function Select Register
    #define P3SF7 0x80
    #define P3SF6 0x40
    #define P3SF5 0x20
    #define P3SF4 0x10
    #define P3SF3 0x08
    #define P3SF2 0x04
    #define P3SF1 0x02
    #define P3SF0 0x01

SFR(P4SFS, 0x94); //Port 4 Special Function Select Register.
    #define P4SF7 0x80
    #define P4SF6 0x40
    #define P4SF5 0x20
    #define P4SF4 0x10
    #define P4SF3 0x08
    #define P4SF2 0x04
    #define P4SF1 0x02
    #define P4SF0 0x01

SFR(ASCL, 0x95); // ADC pre-scaler
    #define ADCCE  0x08 // ADC Conversion Reference Clock Enable.
    //ADC Reference Clock PreScaler. Only three Prescaler values are allowed:
    #define ADCPS2 0x02 // Resulting ADC clock is fOSC.
    #define ADCPS1 0x01 // Resulting ADC clock is fOSC/2.
    #define ADCPS0 0x00 // Resulting ADC clock is fOSC/4.

SFR(ADAT, 0x96); // A/D result register (bits 0 to 7).
SFR(ACON, 0x97); // A/D control register.
    #define AINTF  0x80 // ADC Interrupt flag. This bit must be cleared with software.
    #define AINTEN 0x40 // ADC Interrupt Enable.
    #define ADEN   0x20 // ADC Enable Bit.
    #define ADS2   0x10 // Analog channel Select bit 3.
    #define ADS1   0x08 // Analog channel Select bit 2.
    #define ADS0   0x04 // Analog channel Select bit 1.
    #define ADST   0x02 // ADC Start Bit.
    #define ADSF   0x01 // ADC Status Bit.

SFR(SCON, 0x98); // For compatibity with legacy code
SFR(SCON0, 0x98); // Serial Port UART0 Control Register
    SBIT(SM0, 0x98, 7); // Serial Port Mode Bit 0.
    SBIT(SM1, 0x98, 6); // Serial Port Mode Bit 1.
    SBIT(SM2, 0x98, 5); // Serial Port Mode Bit 2.
    SBIT(REN, 0x98, 4); // Enables serial reception.
    SBIT(TB8, 0x98, 3); // The 9th data bit that will be transmitted in Modes 2 and 3.
    SBIT(RB8, 0x98, 2); // In Modes 2 and 3, the 9th data bit that was received.
    SBIT(TI,  0x98, 1); // Transmit interrupt flag.
    SBIT(RI,  0x98, 0); // Receive interrupt flag.

SFR(SBUF, 0x99);  // For compatibity with legacy code.
SFR(SBUF0, 0x99); // Serial Port UART0 Data Buffer.

SFR(SCON2, 0x9A); // Serial Port Control.
/* These bits are not bit-addressable */
    #define SM01  0x80  // Serial Port Mode Bit 0.
    #define SM11  0x40  // Serial Port Mode Bit 1.
    #define SM21  0x20  // Serial Port Mode Bit 2.
    #define REN1  0x10  // Enables serial reception.
    #define TB81  0x08  // The 9th data bit that will be transmitted in Modes 2 and 3.
    #define RB81  0x04  // In Modes 2 and 3, the 9th data bit that was received.
    #define TI1   0x02  // Transmit interrupt flag.
    #define RI1   0x01  // Receive interrupt flag.

SFR(SBUF2, 0x9B);   // Data buffer for UART1.

SFR(P2, 0xA0); // Port 2
    SBIT(P2_0, 0xA0, 0); // Port 2 bit 0.
    SBIT(P2_1, 0xA0, 1); // Port 2 bit 1.
    SBIT(P2_2, 0xA0, 2); // Port 2 bit 2.
    SBIT(P2_3, 0xA0, 3); // Port 2 bit 3.
    SBIT(P2_4, 0xA0, 4); // Port 2 bit 4.
    SBIT(P2_5, 0xA0, 5); // Port 2 bit 5.
    SBIT(P2_6, 0xA0, 6); // Port 2 bit 6.
    SBIT(P2_7, 0xA0, 7); // Port 2 bit 7.

SFR(PWMCON, 0xA1);      // PWM control register for 4+1 channels
    #define PWML   0x80 // Polarity 0-3
    #define PWMP   0x40 // Polarity 4
    #define PWME   0x20 // Enable Bit. (0 = disabled)
    #define CFG4   0x10 // Output4: 0=open drain, 1=pushpull
    #define CFG3   0x08 // Output3: 0=open drain, 1=pushpull
    #define CFG2   0x04 // Output2: 0=open drain, 1=pushpull
    #define CFG1   0x02 // Output1: 0=open drain, 1=pushpull
    #define CFG0   0x01 // Output0: 0=open drain, 1=pushpull


SFR(PWM0, 0xA2); // PWM0 counter.
SFR(PWM1, 0xA3); //  counter.
SFR(PWM2, 0xA4); //
SFR(PWM3, 0xA5); //
SFR(PWM4P, 0xAA); // PWM4 Period
SFR(PWM4W, 0xAB); // PWM4 Width
SFR(PSCL0L, 0xB1); // The low 8 bits of PSCL 0 16-bit counter. (Prescaler for PWM)
SFR(PSCL0H, 0xB2); // The high 8 bits of PSCL 0 16-bit counter.
SFR(PSCL1L, 0xB3); // The low 8 bits of PSCL 1 16-bit counter.
SFR(PSCL1H, 0xB4); // The high 8 bits of PSCL 1 16-bit counter.


SFR(IEA, 0xA7); // Interrupt Enable Addition Register.
    #define EADC 0x80 // Enable ADC Interrupt.
    #define ESPI 0x40 // Enable SPI Interrupt.
    #define EPCA 0x20 // Enable Programmable Counter Array Interrupt.
    #define ES1  0x10 // Enable UART1 Interrupt.
    #define EI2C 0x02 // Enable I2C Interrupt.

SFR(IE, 0xA8); // Interrupt Enable Register.
    SBIT(EA,  0xA8, 7); // Global disable bit.
    SBIT(ET2, 0xA8, 5); // Enable Timer 2 Interrupt.
    SBIT(ES0, 0xA8, 4); // Enable UART0 Interrupt.
    SBIT(ET1, 0xA8, 3); // Enable Timer 1 Interrupt.
    SBIT(EX1, 0xA8, 2); // Enable External Interrupt INT1.
    SBIT(ET0, 0xA8, 1); // Enable Timer 0 Interrupt.
    SBIT(EX0, 0xA8, 0); // Enable External Interrupt INT0.


SFR(WDRST, 0xA6);   // Watchdog Timer Reset Counter Register.
SFR(WDKEY, 0xAE);   //Watchdog Timer Key Register.

SFR(P3, 0xB0); // Port 3
    SBIT(P3_0, 0xB0, 0); // Port 3 bit 0.
    SBIT(P3_1, 0xB0, 1); // Port 3 bit 1.
    SBIT(P3_2, 0xB0, 2); // Port 3 bit 2.
    SBIT(P3_3, 0xB0, 3); // Port 3 bit 3.
    SBIT(P3_4, 0xB0, 4); // Port 3 bit 4.
    SBIT(P3_5, 0xB0, 5); // Port 3 bit 5.
    SBIT(P3_6, 0xB0, 6); // Port 3 bit 6.
    SBIT(P3_7, 0xB0, 7); // Port 3 bit 7.

SFR(IPA, 0xB7); // Interrupt Priority Addition register.
    #define PDDC 0x80 // DDC Interrupt priority level.
    #define PS1  0x10 // UART1 Interrupt priority.
    #define PI2C 0x02 // I2C Interrupt priority level.
    #define PUSB 0x01 // USB Interrupt priority level.

SFR(IP, 0xB8);  // Interrupt Priority Register.
    SBIT(PT2, 0xB8, 5); // Timer 2 Interrupt priority level.
    SBIT(PS0, 0xB8, 4); // UART0 Interrupt priority level.
    SBIT(PT1, 0xB8, 3); // Timer 1 Interrupt priority level.
    SBIT(PX1, 0xB8, 2); // External Interrupt INT1 priority level.
    SBIT(PT0, 0xB8, 1); // Timer 0 Interrupt priority level.
    SBIT(PX0, 0xB8, 0); // External Interrupt INT0 priority level.

SFR(P4, 0xC0); // I/O Port 4 Register
    SBIT(P4_0, 0xC0, 0); // Port 4 bit 0.
    SBIT(P4_1, 0xC0, 1); // Port 4 bit 1.
    SBIT(P4_2, 0xC0, 2); // Port 4 bit 2.
    SBIT(P4_3, 0xC0, 3); // Port 4 bit 3.
    SBIT(P4_4, 0xC0, 4); // Port 4 bit 4.
    SBIT(P4_5, 0xC0, 5); // Port 4 bit 5.
    SBIT(P4_6, 0xC0, 6); // Port 4 bit 6.
    SBIT(P4_7, 0xC0, 7); // Port 4 bit 7.


SFR(T2CON, 0xC8); // Timer / Counter 2 Control.
    SBIT(TF2,   0xC8, 7); // Timer 2 overflow flag.
    SBIT(EXF2,  0xC8, 6); // Timer 2 external flag.
    SBIT(RCLK,  0xC8, 5); // Receive clock flag.
    SBIT(TCLK,  0xC8, 4); // Transmit clock flag.
    SBIT(EXEN2, 0xC8, 3); // Timer 2 external enable flag.
    SBIT(TR2,   0xC8, 2); // Start/stop control for timer 2.
    SBIT(CNT2,  0xC8, 1); // Timer or coutner select.
    SBIT(CAP2,  0xC8, 0); // Capture/reload flag.

SFR(T2MOD, 0xC9); // Timer / Counter 2 Mode
   #define DCEN   0x01   // DC Enable

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
    SBIT(P,   0xD0, 0); // Parity Flag.

SFR(S1SETUP, 0xD1); // I2C START Condition Sample Setup register.
    #define EN_SS     0x80 // Enable Sample Setup.
    #define SMPL_SET6 0x40 // Sample Setting bit 7.
    #define SMPL_SET5 0x20 // Sample Setting bit 6.
    #define SMPL_SET4 0x10 // Sample Setting bit 5.
    #define SMPL_SET3 0x08 // Sample Setting bit 4.
    #define SMPL_SET2 0x04 // Sample Setting bit 3.
    #define SMPL_SET1 0x02 // Sample Setting bit 2.
    #define SMPL_SET0 0x01 // Sample Setting bit 1.

SFR(S2SETUP, 0xD2); // I2C (S2) SETUP
SFR(RAMBUF,  0xD4); // DDC Ram Buffer
SFR(DDCDAT,  0xD5); // DDC (I2C) data xmit register
SFR(DDCADR,  0xD6); // DDC (I2C) address pointer register
SFR(DDCCON,  0xD7); // DDC Control Register
   #define EX_DAT 0x40
   #define SWENB  0x20
   #define DDC_AX 0x10
   #define DDCINT 0x08
   #define DDC1EN 0x04
   #define SWHINT 0x02
   #define M0     0x01

SFR(S1CON, 0xD8); // I2C Interface Control Register.
SFR(S2CON, 0xDC); // I2C Interface Control Register.
    #define CR2  0x80 // SCL clock frequency select bit 3.
    #define ENI1 0x40 // I2C Interface Enable.
    #define STA  0x20 // START flag.
    #define STO  0x10 // STOP flag.
    #define ADDR 0x08 // Slave mode address.
    #define AA   0x04 // Assert Acknowledge enable.
    #define CR1  0x02 // SCL clock frequency select bit 2.
    #define CR0  0x01 // SCL clock frequency select bit 1.

SFR(S1STA, 0xD9); // I2C Interface Status Register.
SFR(S2STA, 0xDD); // I2C Interface Status Register.
    #define GC       0x80 // General Call flag.
    #define STOP     0x40 // STOP flag.
    #define INTR     0x20 // Interrupt flag.
    #define TX_MODE  0x10 // Transmission Mode flag.
    #define BBUSY    0x08 // Bus Busy flag.
    #define BLOST    0x04 // Bus Lost flag.
    #define ACK_RESP 0x02 // Not Acknowledge Response flag.
    #define SLV      0x01 // Slave Mode flag.

SFR(S1DAT, 0xDA); // I2C Data Shift Register.
SFR(S1ADR, 0xDB); // I2C Address Register (bit 0 not used).
SFR(S2DAT, 0xDE); // I2C Data Shift Register.
SFR(S2ADR, 0xDF); // I2C Address Register (bit 0 not used).

SFR(A, 0xE0);
SFR(ACC, 0xE0); // Accumulator
    SBIT(ACC_0, 0xE0, 0); // Accumulator bit 0.
    SBIT(ACC_1, 0xE0, 1); // Accumulator bit 1.
    SBIT(ACC_2, 0xE0, 2); // Accumulator bit 2.
    SBIT(ACC_3, 0xE0, 3); // Accumulator bit 3.
    SBIT(ACC_4, 0xE0, 4); // Accumulator bit 4.
    SBIT(ACC_5, 0xE0, 5); // Accumulator bit 5.
    SBIT(ACC_6, 0xE0, 6); // Accumulator bit 6.
    SBIT(ACC_7, 0xE0, 7); // Accumulator bit 7.

/*
 The USB function are described in the datasheet page 83 and further.
 Further notes can be found in application note AN1877 Rev.1 page 37 up; downloadable from ST.Com
 */
SFR(USCL, 0xE1);  // USB Prescaler

SFR(UDT1,  0xE6); // USB Endpt1 Data Xmit
SFR(UDT0,  0xE7); // USB Endpt0 Data Xmit
SFR(UISTA, 0xE8); // USB Interrupt Status
   #define SUSPND 0x80
   #define RSTF   0x20
   #define TXD0F  0x10
   #define RXD0F  0x08
   #define RXD1F  0x04
   #define EOPF   0x02
   #define RESUMF 0x01

SFR(UIEN, 0xE9); // USB Interrupt Enable
   #define SUSPNDIE  0x80
   #define RSTE      0x40
   #define RSTFIE    0x20
   #define TXD0IE    0x10
   #define RXD0IE    0x08
   #define TXD1IE    0x04
   #define EOPIE     0x02
   #define RESUMIE   0x01

SFR(UCON0, 0xEA); // USB Endpt0 Xmit Control
   #define TSEQ0     0x80
   #define STALL0    0x40
   #define TX0E      0x20
   #define RX0E      0x10
   #define TP0SIZ3   0x08
   #define TP0SIZ2   0x04
   #define TP0SIZ1   0x02
   #define TP0SIZ0   0x01

SFR(UCON1, 0xEB);  // USB Endpt1 Xmit Control
   #define TSEQ1     0x80
   #define EP12SEL   0x40
   #define FRESUM    0x10
   #define TP1SIZ3   0x08
   #define TP1SIZ2   0x04
   #define TP1SIZ1   0x02
   #define TP1SIZ0   0x01

SFR(UCON2, 0xEC );  // USB Control Register
   #define SOUT      0x10
   #define EP2E      0x08
   #define EP1E      0x04
   #define STALL2    0x02
   #define STALL1    0x01

SFR(USTA, 0xED); // USB Endpt0 Status
   #define RSEQ      0x80
   #define SETUP     0x40
   #define IN        0x20
   #define OUT       0x10
   #define RP0SIZ3   0x08
   #define RP0SIZ2   0x04
   #define RP0SIZ1   0x02
   #define RP0SIZ0   0x01

SFR(UADR, 0xEE); // USB Address Register
   #define USBEN     0x80

SFR(UDR0, 0xEF); // USB Endpt0 Data Recv

SFR(B, 0xF0); // B Register
    SBIT(B_0, 0xF0, 0); // Register B bit 0.
    SBIT(B_1, 0xF0, 1); // Register B bit 1.
    SBIT(B_2, 0xF0, 2); // Register B bit 2.
    SBIT(B_3, 0xF0, 3); // Register B bit 3.
    SBIT(B_4, 0xF0, 4); // Register B bit 4.
    SBIT(B_5, 0xF0, 5); // Register B bit 5.
    SBIT(B_6, 0xF0, 6); // Register B bit 6.
    SBIT(B_7, 0xF0, 7); // Register B bit 7.


// PSD registers definition - by Jan Waclawek - wek at efton dot sk - May 2007
// all defines here are with PSD_ prefix to identify them as PSD-related
//
// Based on uPSD33xx datasheet (preliminary) - Jan 2005, Table 79 at pages 145/146
// and subsequent text
// Based on uPSD3254A datasheet Rev.4 - 4 Nov 2004, Table 84 at page 99
// and subsequent text

// requires to have PSD_CSIOP defined to the base address of the PSD IO area,
//     as defined in PSDSoftExpress or CUPS
#ifndef PSD_CSIOP
#error PSD_CSIOP has to be #define-d (before #include-ing this file) to the base address of the PSD registers area, according to csiop setting in CUPS/PSDSoftExpress
#else
                                         //  -- Port A not available on 52-pin uPSD33xx devices
SFRX(PSD_DATAIN_A,    PSD_CSIOP+0x00);   // MCU I/O Mode Port A Data In Register
                                         //   reads 0 if pin is log.0, 1 if pin is log. 1
                                         //   READ only
SFRX(PSD_DATAOUT_A,   PSD_CSIOP+0x04);   // MCU I/O Mode Port A Data Out Register
                                         //   write 0 to set pin to log. 0, 1 to set pin to log. 1
                                         //   read back written value
                                         //   reset default = 00
SFRX(PSD_DIRECTION_A, PSD_CSIOP+0x06);   // MCU I/O Mode Port A Direction Register
                                         //   write 1 to set pin as output, 0 to set pin as input
                                         //   read back written value
                                         //   reset default = 00
SFRX(PSD_DRIVE_A,     PSD_CSIOP+0x08);   // Select Open Drain or High Slew Rate for port A
                                         //   PA0-PA3: write 0 to select standard push-pull CMOS output, 1 to select High Slew Rate push-pull CMOS output
                                         //   PA4-PA7: write 0 to select standard push-pull CMOS output, 1 to select Open Drain output
                                         //   reset default = 00
SFRX(PSD_CONTROL_A,   PSD_CSIOP+0x02);   // Selects MCU I/O or Latched Address Out mode for port A
                                         //   write 0 to select standard I/O pin, 1 to drive demultiplexed address signal on pin
                                         //   read back written value
                                         //   reset default = 00
SFRX(PSD_OUTENABLE_A, PSD_CSIOP+0x0C);   // Read state of Output Enable Logic on each I/O port driver of Port A
                                         //   1 - driver output is enabled, 0 - driver is off (high impedance)
                                         //   READ only

                                         // -- for comment on individual registers, see above Port A
SFRX(PSD_DATAIN_B,    PSD_CSIOP+0x01);   // MCU I/O Mode Port B Data In Register
SFRX(PSD_DATAOUT_B,   PSD_CSIOP+0x05);   // MCU I/O Mode Port B Data Out Register
SFRX(PSD_DIRECTION_B, PSD_CSIOP+0x07);   // MCU I/O Mode Port B Direction Register
SFRX(PSD_DRIVE_B,     PSD_CSIOP+0x09);   // Select Open Drain or High Slew Rate for port B
                                         // PB0-PB3: standard/High Slew Rate, PB4-PB7: standard/Open Drain
SFRX(PSD_CONTROL_B,   PSD_CSIOP+0x03);   // Selects MCU I/O or Latched Address Out mode for port B
SFRX(PSD_OUTENABLE_B, PSD_CSIOP+0x0D);   // Read state of Output Enable Logic on each I/O port driver of Port B

                                         // -- for comment on individual registers, see above Port A
                                         // only pins PC2, PC3, PC4, PC7 available; other bits in registers are undefined
SFRX(PSD_DATAIN_C,    PSD_CSIOP+0x10);   // MCU I/O Mode Port C Data In Register
SFRX(PSD_DATAOUT_C,   PSD_CSIOP+0x12);   // MCU I/O Mode Port C Data Out Register
SFRX(PSD_DIRECTION_C, PSD_CSIOP+0x14);   // MCU I/O Mode Port C Direction Register
SFRX(PSD_DRIVE_C,     PSD_CSIOP+0x16);   // Select Open Drain for port C
SFRX(PSD_OUTENABLE_C, PSD_CSIOP+0x1A);   // Read state of Output Enable Logic on each I/O port driver of Port C

                                         // -- for comment on individual registers, see above Port A
                                         // only pins PD1, PD2 available (PD2 not available on 52-pin package); other bits in registers are undefined
SFRX(PSD_DATAIN_D,    PSD_CSIOP+0x11);   // MCU I/O Mode Port D Data In Register
SFRX(PSD_DATAOUT_D,   PSD_CSIOP+0x13);   // MCU I/O Mode Port D Data Out Register
SFRX(PSD_DIRECTION_D, PSD_CSIOP+0x15);   // MCU I/O Mode Port D Direction Register
SFRX(PSD_DRIVE_D,     PSD_CSIOP+0x17);   // Select High Slew Rate for port D
SFRX(PSD_OUTENABLE_D, PSD_CSIOP+0x1B);   // Read state of Output Enable Logic on each I/O port driver of Port D

SFRX(PSD_IMC_A,       PSD_CSIOP+0x0A);   // Read to obtain logic state of Input Macrocells connected to Port A
                                         //   READ only
SFRX(PSD_IMC_B,       PSD_CSIOP+0x0B);   // Read to obtain logic state of Input Macrocells connected to Port B
SFRX(PSD_IMC_C,       PSD_CSIOP+0x18);   // Read to obtain logic state of Input Macrocells connected to Port C
                                         //   only pins PC2, PC3, PC4, PC7 available; other bits in register are undefined
SFRX(PSD_OMC_AB,      PSD_CSIOP+0x20);   // Read logic state of macrocells AB. Write to load macrocell AB flip-flops.
SFRX(PSD_OMC_BC,      PSD_CSIOP+0x21);   // Read logic state of macrocells BC. Write to load macrocell BC flip-flops.
SFRX(PSD_OMCMASK_AB,  PSD_CSIOP+0x22);   // Write to set mask for macrocell AB.
                                         //    1 blocks READs/WRITEs of OMF, 0 will pass OMF value
                                         //    Read back written value.
SFRX(PSD_OMCMASK_BC,  PSD_CSIOP+0x23);   // Write to set mask for macrocell BC.

// -- all three Power Management Register are set to 00 after PowerUp, but unchanged during reset (/RST)
SFRX(PSD_PMMR0,       PSD_CSIOP+0xB0);   // -- Power Management Register 0 - write/read
// bit 0 unused and should be set to 0
#define PSD_APD_ENA            0x02      // 0 - Automatic Power Down (APD) counter is disabled, 1 - APD enabled
// bit 2 unused and should be set to 0
#define PSD_TURBO_DISA         0x08      // 0 - PSD Turbo mode enabled, 1 - Turbo mode off, saving power
#define PSD_BLOCK_CLKIN_PLD    0x10      // 0 - CLKIN to PLD not blocked, 1 - no CLKIN to PLD Input Bus, saving power
#define PSD_BLOCK_CLKIN_OMC    0x20      // 0 - CLKIN to Output Macrocells not blocked, 1 - blocked, saving power
// bits 6 and 7 unused and should be set to 0

SFRX(PSD_PMMR2,       PSD_CSIOP+0xB4);   // -- Power Management Register 2 - write/read
// bits 0 and 1 unused and should be set to 0
#define PSD_BLOCK_WR_PLD       0x04      // 0 - /WR from 8032 to PLD Input Bus not blocked, 1 - blocked, saving power
#define PSD_BLOCK_RD_PLD       0x08      // 0 - /RD from 8032 to PLD Input Bus not blocked, 1 - blocked, saving power
#define PSD_BLOCK_PSEN_PLD     0x10      // 0 - /PSEN from 8032 to PLD Input Bus not blocked, 1 - blocked, saving power
#define PSD_BLOCK_ALE_PLD      0x20      // 0 - ALE from 8032 to PLD Input Bus not blocked, 1 - blocked, saving power
#define PSD_BLOCK_PC7_PDL      0x40      // 0 - input from Port C pin 7 to PLD Input Bus not blocked, 1 - blocked, saving power
// bit 7 unused and should be set to 0

SFRX(PSD_PMMR3,       PSD_CSIOP+0xC7);   // -- Power Management Register 3 - write/read
// bit 0 unused and should be set to 0
#define PSD_FORCE_PD           0x02      // 0 - APD counter, if enabled, will cause powerdown, 1 - powerdown will be entered immediately
                                         //   - once set, cleared only by reset condition
// bit 2 not defined by datasheet
// bits 3 to 7 unused and should be set to 0

SFRX(PSD_MAINPROTECT, PSD_CSIOP+0xC0);   // -- Main Flash Memory Protection Definition
                                         //   bit 0 to bit 7 - sector 0 to sector 7 protection status
                                         //      - 1 - flash sector write protected, 0 - not write protected
                                         //   READ only
SFRX(PSD_ALTPROTECT,  PSD_CSIOP+0xC2);   // -- Secondary Flash Memory Protection Definition
                                         //   bit 0 to bit 3 - sector 0 to sector 3 protection status
                                         //      - 1 - flash sector write protected, 0 - not write protected
                                         //   bit 7 - Security Bit
                                         //      - 1 - device is secured against external reading and writing, 0 - not secured
                                         //   READ only

SFRX(PSD_PAGE,        PSD_CSIOP+0xE0);   // -- Memory Page Register

SFRX(PSD_VM,          PSD_CSIOP+0xE2);   // -- Memory Mapping Register
                                         //   Places PSD Module memories into 8032 Program Address Space
                                         //   and/or 8032 XDATA Address Space
                                         //   Default value of bits 0 to 4 is loaded from Non-Volatile
                                         //   setting as specified from PSDsoft Express upon any reset
                                         //   or power-up condition. The default value of these bits
                                         //   can be overridden by 8032 at run-time.
#define PSD_VM_SRAM_CODE       0x01      // 0 - SRAM not accessible as CODE (/PSEN) memory, 1 - SRAM accessible as CODE memory
#define PSD_VM_ALT_CODE        0x02      // 0 - secondary FLASH not accessible as CODE (/PSEN) memory, 1 - secondary FLASH accessible as CODE memory
#define PSD_VM_MAIN_CODE       0x04      // 0 - primary FLASH not accessible as CODE (/PSEN) memory, 1 - primary FLASH accessible as CODE memory
#define PSD_VM_ALT_XDATA       0x08      // 0 - secondary FLASH not accessible as XDATA (/RD/WR) memory, 1 - secondary FLASH accessible as XDATA memory
#define PSD_VM_MAIN_XDATA      0x10      // 0 - primary FLASH not accessible as XDATA (/RD/WR) memory, 1 - primary FLASH accessible as XDATA memory
// bits 5 and 6 unused
#define PSD_VM_PIO_EN          0x80      // 0 - disable, 1- enable peripheral I/O mode on Port A

// another terminology for FLASH - MAIN/ALTERNATIVE -> PRIMARY/SECONDARY
#define PSD_VM_PRI_CODE   PSD_VM_MAIN_CODE
#define PSD_VM_SEC_CODE   PSD_VM_ALT_CODE
#define PSD_VM_PRI_XDATA  PSD_VM_MAIN_XDATA
#define PSD_VM_SEC_XDATA  PSD_VM_ALT_XDATA

#endif

#endif //REG_UPSD32XX_H
