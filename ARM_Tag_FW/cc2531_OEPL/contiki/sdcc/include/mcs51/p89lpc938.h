/*-------------------------------------------------------------------------
   p89lpc938.h - This header defines register addresses for the Philips
   P89LPC938 microcontroller for use with the SDCC compiler.

   Copyright (C) 2007, Kyle Guinn <elyk03@gmail.com>

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

#ifndef P89LPC938_H
#define P89LPC938_H

/* SFR byte addresses */
__sfr __at (0x80) P0;      /* Port 0                               */
__sfr __at (0x81) SP;      /* Stack pointer                        */
__sfr __at (0x82) DPL;     /* Data pointer low                     */
__sfr __at (0x83) DPH;     /* Data pointer high                    */
__sfr __at (0x84) P0M1;    /* Port 0 output mode 1                 */
__sfr __at (0x85) P0M2;    /* Port 0 output mode 2                 */
__sfr __at (0x86) KBMASK;  /* Keypad interrupt mask register       */
__sfr __at (0x87) PCON;    /* Power control register               */
__sfr __at (0x88) TCON;    /* Timer 0 and 1 control                */
__sfr __at (0x89) TMOD;    /* Timer 0 and 1 mode                   */
__sfr __at (0x8A) TL0;     /* Timer 0 low                          */
__sfr __at (0x8B) TL1;     /* Timer 1 low                          */
__sfr __at (0x8C) TH0;     /* Timer 0 high                         */
__sfr __at (0x8D) TH1;     /* Timer 1 high                         */
__sfr __at (0x8F) TAMOD;   /* Timer 0 and 1 auxiliary mode         */
__sfr __at (0x90) P1;      /* Port 1                               */
__sfr __at (0x91) P1M1;    /* Port 1 output mode 1                 */
__sfr __at (0x92) P1M2;    /* Port 1 output mode 2                 */
__sfr __at (0x93) KBPATN;  /* Keypad pattern register              */
__sfr __at (0x94) KBCON;   /* Keypad control register              */
__sfr __at (0x95) DIVM;    /* CPU clock divide-by-M control        */
__sfr __at (0x96) TRIM;    /* Internal oscillator trim register    */
__sfr __at (0x97) AD0CON;  /* ADC0 control register                */
__sfr __at (0x98) SCON;    /* Serial port control                  */
__sfr __at (0x99) SBUF;    /* Serial port data buffer register     */
__sfr __at (0xA0) P2;      /* Port 2                               */
__sfr __at (0xA1) AD0MODB; /* ADC0 mode register B                 */
__sfr __at (0xA2) AUXR1;   /* Auxiliary function register          */
__sfr __at (0xA3) AD0INS;  /* ADC0 input select                    */
__sfr __at (0xA4) P2M1;    /* Port 2 output mode 1                 */
__sfr __at (0xA5) P2M2;    /* Port 2 output mode 2                 */
__sfr __at (0xA7) WDCON;   /* Watchdog control register            */
__sfr __at (0xA8) IEN0;    /* Interrupt enable 0                   */
__sfr __at (0xA9) SADDR;   /* Serial port address register         */
__sfr __at (0xAA) ICRAL;   /* Input capture A register low         */
__sfr __at (0xAB) ICRAH;   /* Input capture A register high        */
__sfr __at (0xAC) CMP1;    /* Comparator 1 control register        */
__sfr __at (0xAD) CMP2;    /* Comparator 2 control register        */
__sfr __at (0xAE) ICRBL;   /* Input capture B register low         */
__sfr __at (0xAF) ICRBH;   /* Input capture B register high        */
__sfr __at (0xB0) P3;      /* Port 3                               */
__sfr __at (0xB1) P3M1;    /* Port 3 output mode 1                 */
__sfr __at (0xB2) P3M2;    /* Port 3 output mode 2                 */
__sfr __at (0xB5) PCONA;   /* Power control register A             */
__sfr __at (0xB7) IP0H;    /* Interrupt priority 0 high            */
__sfr __at (0xB8) IP0;     /* Interrupt priority 0                 */
__sfr __at (0xB9) SADEN;   /* Serial port address enable           */
__sfr __at (0xBA) SSTAT;   /* Serial port extended status register */
__sfr __at (0xBD) BRGCON;  /* Baud rate generator control          */
__sfr __at (0xBE) BRGR0;   /* Baud rate generator rate low         */
__sfr __at (0xBF) BRGR1;   /* Baud rate generator rate high        */
__sfr __at (0xC0) AD0MODA; /* ADC0 mode register A                 */
__sfr __at (0xC1) WDL;     /* Watchdog load                        */
__sfr __at (0xC2) WFEED1;  /* Watchdog feed 1                      */
__sfr __at (0xC3) WFEED2;  /* Watchdog feed 2                      */
__sfr __at (0xC8) TCR20;   /* CCU control register 0               */
__sfr __at (0xC9) TICR2;   /* CCU interrupt control register       */
__sfr __at (0xCA) TPCR2L;  /* Prescaler control register low       */
__sfr __at (0xCB) TPCR2H;  /* Prescaler control register high      */
__sfr __at (0xCC) TL2;     /* CCU timer low                        */
__sfr __at (0xCD) TH2;     /* CCU timer high                       */
__sfr __at (0xCE) TOR2L;   /* CCU reload register low              */
__sfr __at (0xCF) TOR2H;   /* CCU reload register high             */
__sfr __at (0xD0) PSW;     /* Program status word                  */
__sfr __at (0xD1) RTCCON;  /* RTC control                          */
__sfr __at (0xD2) RTCH;    /* RTC register high                    */
__sfr __at (0xD3) RTCL;    /* RTC register low                     */
__sfr __at (0xD5) IEN2;    /* Interrupt enable 2                   */
__sfr __at (0xD6) IP2;     /* Interrupt priority 2                 */
__sfr __at (0xD7) IP2H;    /* Interrupt priority 2 high            */
__sfr __at (0xD8) I2CON;   /* I²C control register                 */
__sfr __at (0xD9) I2STAT;  /* I²C status register                  */
__sfr __at (0xDA) I2DAT;   /* I²C data register                    */
__sfr __at (0xDB) I2ADR;   /* I²C slave address register           */
__sfr __at (0xDC) I2SCLL;  /* Serial clock generator low/
                            * SCL duty cycle register low          */
__sfr __at (0xDD) I2SCLH;  /* Serial clock generator high/
                            * SCL duty cycle register high         */
__sfr __at (0xDE) TISE2;   /* CCU interrupt status encode register */
__sfr __at (0xDF) RSTSRC;  /* Reset source register                */
__sfr __at (0xE0) ACC;     /* Accumulator                          */
__sfr __at (0xE1) SPSTAT;  /* SPI status register                  */
__sfr __at (0xE2) SPCTL;   /* SPI control register                 */
__sfr __at (0xE3) SPDAT;   /* SPI data register                    */
__sfr __at (0xE4) FMCON;   /* Program Flash control (Read)/
                            * Program Flash control (Write)        */
__sfr __at (0xE5) FMDATA;  /* Program Flash data                   */
__sfr __at (0xE6) FMADRL;  /* Program Flash address low            */
__sfr __at (0xE7) FMADRH;  /* Program Flash address high           */
__sfr __at (0xE8) IEN1;    /* Interrupt enable 1                   */
__sfr __at (0xE9) TIFR2;   /* CCU interrupt flag register          */
__sfr __at (0xEA) CCCRA;   /* Capture compare A control register   */
__sfr __at (0xEB) CCCRB;   /* Capture compare B control register   */
__sfr __at (0xEC) CCCRC;   /* Capture compare C control register   */
__sfr __at (0xED) CCCRD;   /* Capture compare D control register   */
__sfr __at (0xEE) OCRAL;   /* Output compare A register low        */
__sfr __at (0xEF) OCRAH;   /* Output compare A register high       */
__sfr __at (0xF0) B;       /* B register                           */
__sfr __at (0xF1) DEECON;  /* Data EEPROM control register         */
__sfr __at (0xF2) DEEDAT;  /* Data EEPROM data register            */
__sfr __at (0xF3) DEEADR;  /* Data EEPROM address register         */
__sfr __at (0xF6) PT0AD;   /* Port 0 digital input disable         */
__sfr __at (0xF7) IP1H;    /* Interrupt priority 1 high            */
__sfr __at (0xF8) IP1;     /* Interrupt priority 1                 */
__sfr __at (0xF9) TCR21;   /* CCU control register 1               */
__sfr __at (0xFA) OCRBL;   /* Output compare B register low        */
__sfr __at (0xFB) OCRBH;   /* Output compare B register high       */
__sfr __at (0xFC) OCRCL;   /* Output compare C register low        */
__sfr __at (0xFD) OCRCH;   /* Output compare C register high       */
__sfr __at (0xFE) OCRDL;   /* Output compare D register low        */
__sfr __at (0xFF) OCRDH;   /* Output compare D register high       */

/* 16-bit SFRs (duplicates of above) */
__sfr16 __at (0x8382) DPTR;  /* Data pointer               */
__sfr16 __at (0x8C8A) TMR0;  /* Timer 0 count              */
__sfr16 __at (0x8D8B) TMR1;  /* Timer 1 count              */
__sfr16 __at (0xABAA) ICRA;  /* Input capture A register   */
__sfr16 __at (0xAFAE) ICRB;  /* Input capture B register   */
__sfr16 __at (0xBFBE) BRGR;  /* Baud rate generator        */
__sfr16 __at (0xCBCA) TPCR2; /* Prescaler control register */
__sfr16 __at (0xCDCC) TMR2;  /* Timer 2 count              */
__sfr16 __at (0xCFCE) TOR2;  /* CCU reload register        */
__sfr16 __at (0xD2D3) RTC;   /* RTC register               */
__sfr16 __at (0xDDDC) I2SCL; /* Serial clock generator/
                              * SCL duty cycle register    */
__sfr16 __at (0xE7E6) FMADR; /* Program Flash address      */
__sfr16 __at (0xEFEE) OCRA;  /* Output compare A register  */
__sfr16 __at (0xFBFA) OCRB;  /* Output compare B register  */
__sfr16 __at (0xFDFC) OCRC;  /* Output compare C register  */
__sfr16 __at (0xFFFE) OCRD;  /* Output compare D register  */

/* "Extended SFRs" (logically in __xdata memory space) */
#define BNDSTA0  (*(__xdata volatile unsigned char*)0xFFED) /* ADC0 boundary status register           */
#define ADC0LBND (*(__xdata volatile unsigned char*)0xFFEE) /* ADC0 low_boundary register (MSB)        */
#define ADC0HBND (*(__xdata volatile unsigned char*)0xFFEF) /* ADC0 high_boundary register, left (MSB) */
#define AD0DAT7R (*(__xdata volatile unsigned char*)0xFFF0) /* ADC0 data register 7, right (LSB)       */
#define AD0DAT7L (*(__xdata volatile unsigned char*)0xFFF1) /* ADC0 data register 7, left (MSB)        */
#define AD0DAT6R (*(__xdata volatile unsigned char*)0xFFF2) /* ADC0 data register 6, right (LSB)       */
#define AD0DAT6L (*(__xdata volatile unsigned char*)0xFFF3) /* ADC0 data register 6, left (MSB)        */
#define AD0DAT5R (*(__xdata volatile unsigned char*)0xFFF4) /* ADC0 data register 5, right (LSB)       */
#define AD0DAT5L (*(__xdata volatile unsigned char*)0xFFF5) /* ADC0 data register 5, left (MSB)        */
#define AD0DAT4R (*(__xdata volatile unsigned char*)0xFFF6) /* ADC0 data register 4, right (LSB)       */
#define AD0DAT4L (*(__xdata volatile unsigned char*)0xFFF7) /* ADC0 data register 4, left (MSB)        */
#define AD0DAT3R (*(__xdata volatile unsigned char*)0xFFF8) /* ADC0 data register 3, right (LSB)       */
#define AD0DAT3L (*(__xdata volatile unsigned char*)0xFFF9) /* ADC0 data register 3, left (MSB)        */
#define AD0DAT2R (*(__xdata volatile unsigned char*)0xFFFA) /* ADC0 data register 2, right (LSB)       */
#define AD0DAT2L (*(__xdata volatile unsigned char*)0xFFFB) /* ADC0 data register 2, left (MSB)        */
#define AD0DAT1R (*(__xdata volatile unsigned char*)0xFFFC) /* ADC0 data register 1, right (LSB)       */
#define AD0DAT1L (*(__xdata volatile unsigned char*)0xFFFD) /* ADC0 data register 1, left (MSB)        */
#define AD0DAT0R (*(__xdata volatile unsigned char*)0xFFFE) /* ADC0 data register 0, right (LSB)       */
#define AD0DAT0L (*(__xdata volatile unsigned char*)0xFFFF) /* ADC0 data register 0, left (MSB)        */

/* Special Function Bits */
/* P0 (0x80) */
__sbit __at (0x80) P0_0;
__sbit __at (0x81) P0_1;
__sbit __at (0x82) P0_2;
__sbit __at (0x83) P0_3;
__sbit __at (0x84) P0_4;
__sbit __at (0x85) P0_5;
__sbit __at (0x86) P0_6;
__sbit __at (0x87) P0_7;
#define CMP_2  P0_0 /* Renamed:  Name conflicts with SFR 0xAD */
#define KB0    P0_0
#define CIN2B  P0_1
#define KB1    P0_1
#define CIN2A  P0_2
#define KB2    P0_2
#define CIN1B  P0_3
#define KB3    P0_3
#define CIN1A  P0_4
#define KB4    P0_4
#define CMPREF P0_5
#define KB5    P0_5
#define CMP_1  P0_6 /* Renamed:  Name conflicts with SFR 0xAC */
#define KB6    P0_6
#define T1     P0_7
#define KB7    P0_7

/* TCON (0x88) */
__sbit __at (0x88) TCON_0;
__sbit __at (0x89) TCON_1;
__sbit __at (0x8A) TCON_2;
__sbit __at (0x8B) TCON_3;
__sbit __at (0x8C) TCON_4;
__sbit __at (0x8D) TCON_5;
__sbit __at (0x8E) TCON_6;
__sbit __at (0x8F) TCON_7;
#define IT0 TCON_0
#define IE0 TCON_1
#define IT1 TCON_2
#define IE1 TCON_3
#define TR0 TCON_4
#define TF0 TCON_5
#define TR1 TCON_6
#define TF1 TCON_7

/* P1 (0x90) */
__sbit __at (0x90) P1_0;
__sbit __at (0x91) P1_1;
__sbit __at (0x92) P1_2;
__sbit __at (0x93) P1_3;
__sbit __at (0x94) P1_4;
__sbit __at (0x95) P1_5;
__sbit __at (0x96) P1_6;
__sbit __at (0x97) P1_7;
#define TXD  P1_0
#define RXD  P1_1
#define T0   P1_2
#define SCL  P1_2
#define INT0 P1_3
#define SDA  P1_3
#define INT1 P1_4
#define RST  P1_5
#define OCB  P1_6
#define OCC  P1_7

/* SCON (0x98) */
__sbit __at (0x98) SCON_0;
__sbit __at (0x99) SCON_1;
__sbit __at (0x9A) SCON_2;
__sbit __at (0x9B) SCON_3;
__sbit __at (0x9C) SCON_4;
__sbit __at (0x9D) SCON_5;
__sbit __at (0x9E) SCON_6;
__sbit __at (0x9F) SCON_7;
#define RI  SCON_0
#define TI  SCON_1
#define RB8 SCON_2
#define TB8 SCON_3
#define REN SCON_4
#define SM2 SCON_5
#define SM1 SCON_6
#define SM0 SCON_7
#define FE  SCON_7

/* P2 (0xA0) */
__sbit __at (0xA0) P2_0;
__sbit __at (0xA1) P2_1;
__sbit __at (0xA2) P2_2;
__sbit __at (0xA3) P2_3;
__sbit __at (0xA4) P2_4;
__sbit __at (0xA5) P2_5;
__sbit __at (0xA6) P2_6;
__sbit __at (0xA7) P2_7;
#define ICB    P2_0
#define OCD    P2_1
#define MOSI   P2_2
#define MISO   P2_3
#define SS     P2_4
#define SPICLK P2_5
#define OCA    P2_6
#define ICA    P2_7

/* IEN0 (0xA8) */
__sbit __at (0xA8) IEN0_0;
__sbit __at (0xA9) IEN0_1;
__sbit __at (0xAA) IEN0_2;
__sbit __at (0xAB) IEN0_3;
__sbit __at (0xAC) IEN0_4;
__sbit __at (0xAD) IEN0_5;
__sbit __at (0xAE) IEN0_6;
__sbit __at (0xAF) IEN0_7;
#define EX0   IEN0_0
#define ET0   IEN0_1
#define EX1   IEN0_2
#define ET1   IEN0_3
#define ES    IEN0_4
#define ESR   IEN0_4
#define EBO   IEN0_5
#define EWDRT IEN0_6
#define EA    IEN0_7

/* P3 (0xB0) */
__sbit __at (0xB0) P3_0;
__sbit __at (0xB1) P3_1;
__sbit __at (0xB2) P3_2;
__sbit __at (0xB3) P3_3;
__sbit __at (0xB4) P3_4;
__sbit __at (0xB5) P3_5;
__sbit __at (0xB6) P3_6;
__sbit __at (0xB7) P3_7;
#define XTAL2 P3_0
#define XTAL1 P3_1

/* IP0 (0xB8) */
__sbit __at (0xB8) IP0_0;
__sbit __at (0xB9) IP0_1;
__sbit __at (0xBA) IP0_2;
__sbit __at (0xBB) IP0_3;
__sbit __at (0xBC) IP0_4;
__sbit __at (0xBD) IP0_5;
__sbit __at (0xBE) IP0_6;
__sbit __at (0xBF) IP0_7;
#define PX0   IP0_0
#define PT0   IP0_1
#define PX1   IP0_2
#define PT1   IP0_3
#define PS    IP0_4
#define PSR   IP0_4
#define PBO   IP0_5
#define PWDRT IP0_6

/* AD0MODA (0xC0) */
__sbit __at (0xC0) AD0MODA_0;
__sbit __at (0xC1) AD0MODA_1;
__sbit __at (0xC2) AD0MODA_2;
__sbit __at (0xC3) AD0MODA_3;
__sbit __at (0xC4) AD0MODA_4;
__sbit __at (0xC5) AD0MODA_5;
__sbit __at (0xC6) AD0MODA_6;
__sbit __at (0xC7) AD0MODA_7;
#define SCAN0  AD0MODA_4
#define SCC0   AD0MODA_5
#define BURST0 AD0MODA_6
#define BNDI0  AD0MODA_7

/* TCR20 (0xC8) */
__sbit __at (0xC8) TCR20_0;
__sbit __at (0xC9) TCR20_1;
__sbit __at (0xCA) TCR20_2;
__sbit __at (0xCB) TCR20_3;
__sbit __at (0xCC) TCR20_4;
__sbit __at (0xCD) TCR20_5;
__sbit __at (0xCE) TCR20_6;
__sbit __at (0xCF) TCR20_7;
#define TMOD20 TCR20_0
#define TMOD21 TCR20_1
#define TDIR2  TCR20_2
#define ALTAB  TCR20_3
#define ALTCD  TCR20_4
#define HLTEN  TCR20_5
#define HLTRN  TCR20_6
#define PLEEN  TCR20_7

/* PSW (0xD0) */
__sbit __at (0xD0) PSW_0;
__sbit __at (0xD1) PSW_1;
__sbit __at (0xD2) PSW_2;
__sbit __at (0xD3) PSW_3;
__sbit __at (0xD4) PSW_4;
__sbit __at (0xD5) PSW_5;
__sbit __at (0xD6) PSW_6;
__sbit __at (0xD7) PSW_7;
#define P   PSW_0
#define F1  PSW_1
#define OV  PSW_2
#define RS0 PSW_3
#define RS1 PSW_4
#define F0  PSW_5
#define AC  PSW_6
#define CY  PSW_7

/* I2CON (0xD8) */
__sbit __at (0xD8) I2CON_0;
__sbit __at (0xD9) I2CON_1;
__sbit __at (0xDA) I2CON_2;
__sbit __at (0xDB) I2CON_3;
__sbit __at (0xDC) I2CON_4;
__sbit __at (0xDD) I2CON_5;
__sbit __at (0xDE) I2CON_6;
__sbit __at (0xDF) I2CON_7;
#define CRSEL I2CON_0
#define AA    I2CON_2
#define SI    I2CON_3
#define STO   I2CON_4
#define STA   I2CON_5
#define I2EN  I2CON_6

/* ACC (0xE0) */
__sbit __at (0xE0) ACC_0;
__sbit __at (0xE1) ACC_1;
__sbit __at (0xE2) ACC_2;
__sbit __at (0xE3) ACC_3;
__sbit __at (0xE4) ACC_4;
__sbit __at (0xE5) ACC_5;
__sbit __at (0xE6) ACC_6;
__sbit __at (0xE7) ACC_7;

/* IEN1 (0xE8) */
__sbit __at (0xE8) IEN1_0;
__sbit __at (0xE9) IEN1_1;
__sbit __at (0xEA) IEN1_2;
__sbit __at (0xEB) IEN1_3;
__sbit __at (0xEC) IEN1_4;
__sbit __at (0xED) IEN1_5;
__sbit __at (0xEE) IEN1_6;
__sbit __at (0xEF) IEN1_7;
#define EI2C IEN1_0
#define EKBI IEN1_1
#define EC   IEN1_2
#define ESPI IEN1_3
#define ECCU IEN1_4
#define EST  IEN1_6
#define EIEE IEN1_7

/* B (0xF0) */
__sbit __at (0xF0) B_0;
__sbit __at (0xF1) B_1;
__sbit __at (0xF2) B_2;
__sbit __at (0xF3) B_3;
__sbit __at (0xF4) B_4;
__sbit __at (0xF5) B_5;
__sbit __at (0xF6) B_6;
__sbit __at (0xF7) B_7;

/* IP1 (0xF8) */
__sbit __at (0xF8) IP1_0;
__sbit __at (0xF9) IP1_1;
__sbit __at (0xFA) IP1_2;
__sbit __at (0xFB) IP1_3;
__sbit __at (0xFC) IP1_4;
__sbit __at (0xFD) IP1_5;
__sbit __at (0xFE) IP1_6;
__sbit __at (0xFF) IP1_7;
#define PI2C  IP1_0
#define PKBI  IP1_1
#define PC    IP1_2
#define PSPI  IP1_3
#define PCCU  IP1_4
#define PST   IP1_6
#define PADEE IP1_7

#endif /* P89LPC938_H */
