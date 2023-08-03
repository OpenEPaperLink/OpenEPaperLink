/*-------------------------------------------------------------------------
   ax8052f143.h - Register Declarations for the Axsem Integrated Radio

   Copyright (C) 2010, 2011, Axsem AG
     Author: Thomas Sailer, thomas.sailer@axsem.com

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2.1, or (at your option) any
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


#ifndef AX8052F143_H
#define AX8052F143_H

#include <compiler.h>

/* SFR Address Space */

SFR(ACC,              0xE0);     /* Accumulator */
SBIT(ACC_0,           0xE0, 0);  /* Accumulator bit 0 */
SBIT(ACC_1,           0xE0, 1);  /* Accumulator bit 1 */
SBIT(ACC_2,           0xE0, 2);  /* Accumulator bit 2 */
SBIT(ACC_3,           0xE0, 3);  /* Accumulator bit 3 */
SBIT(ACC_4,           0xE0, 4);  /* Accumulator bit 4 */
SBIT(ACC_5,           0xE0, 5);  /* Accumulator bit 5 */
SBIT(ACC_6,           0xE0, 6);  /* Accumulator bit 6 */
SBIT(ACC_7,           0xE0, 7);  /* Accumulator bit 7 */
SFR(B,                0xF0);     /* B Register */
SBIT(B_0,             0xF0, 0);  /* Register B bit 0 */
SBIT(B_1,             0xF0, 1);  /* Register B bit 1 */
SBIT(B_2,             0xF0, 2);  /* Register B bit 2 */
SBIT(B_3,             0xF0, 3);  /* Register B bit 3 */
SBIT(B_4,             0xF0, 4);  /* Register B bit 4 */
SBIT(B_5,             0xF0, 5);  /* Register B bit 5 */
SBIT(B_6,             0xF0, 6);  /* Register B bit 6 */
SBIT(B_7,             0xF0, 7);  /* Register B bit 7 */
SFR(DPH,              0x83);     /* Data Pointer 0 High Byte */
SFR(DPH1,             0x85);     /* Data Pointer 1 High Byte */
SFR(DPL,              0x82);     /* Data Pointer 0 Low Byte */
SFR(DPL1,             0x84);     /* Data Pointer 1 Low Byte */
SFR16(DPTR0,          0x82);     /* Data Pointer 0 */
SFR16(DPTR1,          0x84);     /* Data Pointer 1 */
SFR(DPS,              0x86);     /* Data Pointer Select */
SFR(E2IE,             0xA0);     /* 2nd Extended Interrupt Enable */
SBIT(E2IE_0,          0xA0, 0);  /* Output Compare 0 Interrupt Enable */
SBIT(E2IE_1,          0xA0, 1);  /* Output Compare 1 Interrupt Enable */
SBIT(E2IE_2,          0xA0, 2);  /* Input Capture 0 Interrupt Enable */
SBIT(E2IE_3,          0xA0, 3);  /* Input Capture 1 Interrupt Enable */
SBIT(E2IE_4,          0xA0, 4);  /* Random Number Generator Interrupt Enable */
SBIT(E2IE_5,          0xA0, 5);  /* AES Interrupt Enable */
SBIT(E2IE_6,          0xA0, 6);  /* DebugLink Interrupt Enable */
SBIT(E2IE_7,          0xA0, 7);  /*  */
SFR(E2IP,             0xC0);     /* 2nd Extended Interrupt Priority */
SBIT(E2IP_0,          0xC0, 0);  /* Output Compare 0 Interrupt Priority */
SBIT(E2IP_1,          0xC0, 1);  /* Output Compare 1 Interrupt Priority */
SBIT(E2IP_2,          0xC0, 2);  /* Input Capture 0 Interrupt Priority */
SBIT(E2IP_3,          0xC0, 3);  /* Input Capture 1 Interrupt Priority */
SBIT(E2IP_4,          0xC0, 4);  /* Random Number Generator Interrupt Priority */
SBIT(E2IP_5,          0xC0, 5);  /* AES Interrupt Priority */
SBIT(E2IP_6,          0xC0, 6);  /* DebugLink Interrupt Priority */
SBIT(E2IP_7,          0xC0, 7);  /*  */
SFR(EIE,              0x98);     /* Extended Interrupt Enable */
SBIT(EIE_0,           0x98, 0);  /* Timer 0 Interrupt Enable */
SBIT(EIE_1,           0x98, 1);  /* Timer 1 Interrupt Enable */
SBIT(EIE_2,           0x98, 2);  /* Timer 2 Interrupt Enable */
SBIT(EIE_3,           0x98, 3);  /* SPI 0 Interrupt Enable */
SBIT(EIE_4,           0x98, 4);  /* UART 0 Interrupt Enable */
SBIT(EIE_5,           0x98, 5);  /* UART 1 Interrupt Enable */
SBIT(EIE_6,           0x98, 6);  /* GPADC Interrupt Enable */
SBIT(EIE_7,           0x98, 7);  /* DMA Interrupt Enable */
SFR(EIP,              0xB0);     /* Extended Interrupt Priority */
SBIT(EIP_0,           0xB0, 0);  /* Timer 0 Interrupt Priority */
SBIT(EIP_1,           0xB0, 1);  /* Timer 1 Interrupt Priority */
SBIT(EIP_2,           0xB0, 2);  /* Timer 2 Interrupt Priority */
SBIT(EIP_3,           0xB0, 3);  /* SPI 0 Interrupt Priority */
SBIT(EIP_4,           0xB0, 4);  /* UART 0 Interrupt Priority */
SBIT(EIP_5,           0xB0, 5);  /* UART 1 Interrupt Priority */
SBIT(EIP_6,           0xB0, 6);  /* GPADC Interrupt Priority */
SBIT(EIP_7,           0xB0, 7);  /* DMA Interrupt Priority */
SFR(IE,               0xA8);     /* Interrupt Enable */
SBIT(IE_0,            0xA8, 0);  /* External 0 Interrupt Enable */
SBIT(IE_1,            0xA8, 1);  /* Wakeup Timer Interrupt Enable */
SBIT(IE_2,            0xA8, 2);  /* External 1 Interrupt Enable */
SBIT(IE_3,            0xA8, 3);  /* GPIO Interrupt Enable */
SBIT(IE_4,            0xA8, 4);  /* Radio Interrupt Enable */
SBIT(IE_5,            0xA8, 5);  /* Clock Management Interrupt Enable */
SBIT(IE_6,            0xA8, 6);  /* Power Management Interrupt Enable */
SBIT(IE_7,            0xA8, 7);  /* Global Interrupt Enable */
SBIT(EA,              0xA8, 7);  /* Global Interrupt Enable */
SFR(IP,               0xB8);     /* Interrupt Priority */
SBIT(IP_0,            0xB8, 0);  /* External 0 Interrupt Priority */
SBIT(IP_1,            0xB8, 1);  /* Wakeup Timer Interrupt Priority */
SBIT(IP_2,            0xB8, 2);  /* External 1 Interrupt Priority */
SBIT(IP_3,            0xB8, 3);  /* GPIO Interrupt Priority */
SBIT(IP_4,            0xB8, 4);  /* Radio Interrupt Priority */
SBIT(IP_5,            0xB8, 5);  /* Clock Management Interrupt Priority */
SBIT(IP_6,            0xB8, 6);  /* Power Management Interrupt Priority */
SBIT(IP_7,            0xB8, 7);  /*  */
SFR(PCON,             0x87);     /* Power Mode Control */
SFR(PSW,              0xD0);     /* Program Status Word */
SBIT(P,               0xD0, 0);  /* Parity Flag */
SBIT(F1,              0xD0, 1);  /* User-Defined Flag */
SBIT(OV,              0xD0, 2);  /* Overflow Flag */
SBIT(RS0,             0xD0, 3);  /* Register Bank Select 0 */
SBIT(RS1,             0xD0, 4);  /* Register Bank Select 1 */
SBIT(F0,              0xD0, 5);  /* User-Defined Flag */
SBIT(AC,              0xD0, 6);  /* Auxiliary Carry Flag */
SBIT(CY,              0xD0, 7);  /* Carry Flag */
SFR(SP,               0x81);     /* Stack Pointer */
SFR(WBTEST,           0x8F);     /* Debug Breakpoint Register */
SFR(XPAGE,            0xD9);     /* Memory Page Select */
SFR(_XPAGE,           0xD9);     /* Memory Page Select, SDCC name */
SFR(ADCCH0CONFIG,     0xCA);     /* ADC Channel 0 Configuration */
SFR(ADCCH1CONFIG,     0xCB);     /* ADC Channel 1 Configuration */
SFR(ADCCH2CONFIG,     0xD2);     /* ADC Channel 2 Configuration */
SFR(ADCCH3CONFIG,     0xD3);     /* ADC Channel 3 Configuration */
SFR(ADCCLKSRC,        0xD1);     /* ADC Clock Source */
SFR(ADCCONV,          0xC9);     /* ADC Conversion Source */
SFR(ANALOGCOMP,       0xE1);     /* Analog Comparators */
SFR(CLKCON,           0xC6);     /* Clock Control */
SFR(CLKSTAT,          0xC7);     /* Clock Status */
SFR(CODECONFIG,       0x97);     /* Code Space Configuration */
SFR(DBGLNKBUF,        0xE3);     /* Debug Link Buffer */
SFR(DBGLNKSTAT,       0xE2);     /* Debug Link Status */
SFR(DIRA,             0x89);     /* Port A Direction */
SFR(DIRB,             0x8A);     /* Port B Direction */
SFR(DIRC,             0x8B);     /* Port C Direction */
SFR(DIRR,             0x8E);     /* Port R Direction */
SFR(PINA,             0xC8);     /* Port A Input */
SBIT(PINA_0,          0xC8, 0);  /*  */
SBIT(PINA_1,          0xC8, 1);  /*  */
SBIT(PINA_2,          0xC8, 2);  /*  */
SBIT(PINA_3,          0xC8, 3);  /*  */
SBIT(PINA_4,          0xC8, 4);  /*  */
SBIT(PINA_5,          0xC8, 5);  /*  */
SBIT(PINA_6,          0xC8, 6);  /*  */
SBIT(PINA_7,          0xC8, 7);  /*  */
SFR(PINB,             0xE8);     /* Port B Input */
SBIT(PINB_0,          0xE8, 0);  /*  */
SBIT(PINB_1,          0xE8, 1);  /*  */
SBIT(PINB_2,          0xE8, 2);  /*  */
SBIT(PINB_3,          0xE8, 3);  /*  */
SBIT(PINB_4,          0xE8, 4);  /*  */
SBIT(PINB_5,          0xE8, 5);  /*  */
SBIT(PINB_6,          0xE8, 6);  /*  */
SBIT(PINB_7,          0xE8, 7);  /*  */
SFR(PINC,             0xF8);     /* Port C Input */
SBIT(PINC_0,          0xF8, 0);  /*  */
SBIT(PINC_1,          0xF8, 1);  /*  */
SBIT(PINC_2,          0xF8, 2);  /*  */
SBIT(PINC_3,          0xF8, 3);  /*  */
SBIT(PINC_4,          0xF8, 4);  /*  */
SBIT(PINC_5,          0xF8, 5);  /*  */
SBIT(PINC_6,          0xF8, 6);  /*  */
SBIT(PINC_7,          0xF8, 7);  /*  */
SFR(PINR,             0x8D);     /* Port R Input */
SFR(PORTA,            0x80);     /* Port A Output */
SBIT(PORTA_0,         0x80, 0);  /*  */
SBIT(PORTA_1,         0x80, 1);  /*  */
SBIT(PORTA_2,         0x80, 2);  /*  */
SBIT(PORTA_3,         0x80, 3);  /*  */
SBIT(PORTA_4,         0x80, 4);  /*  */
SBIT(PORTA_5,         0x80, 5);  /*  */
SBIT(PORTA_6,         0x80, 6);  /*  */
SBIT(PORTA_7,         0x80, 7);  /*  */
SFR(PORTB,            0x88);     /* Port B Output */
SBIT(PORTB_0,         0x88, 0);  /*  */
SBIT(PORTB_1,         0x88, 1);  /*  */
SBIT(PORTB_2,         0x88, 2);  /*  */
SBIT(PORTB_3,         0x88, 3);  /*  */
SBIT(PORTB_4,         0x88, 4);  /*  */
SBIT(PORTB_5,         0x88, 5);  /*  */
SBIT(PORTB_6,         0x88, 6);  /*  */
SBIT(PORTB_7,         0x88, 7);  /*  */
SFR(PORTC,            0x90);     /* Port C Output */
SBIT(PORTC_0,         0x90, 0);  /*  */
SBIT(PORTC_1,         0x90, 1);  /*  */
SBIT(PORTC_2,         0x90, 2);  /*  */
SBIT(PORTC_3,         0x90, 3);  /*  */
SBIT(PORTC_4,         0x90, 4);  /*  */
SBIT(PORTC_5,         0x90, 5);  /*  */
SBIT(PORTC_6,         0x90, 6);  /*  */
SBIT(PORTC_7,         0x90, 7);  /*  */
SFR(PORTR,            0x8C);     /* Port R Output */
SFR(IC0CAPT0,         0xCE);     /* Input Capture 0 Low Byte */
SFR(IC0CAPT1,         0xCF);     /* Input Capture 0 High Byte */
SFR16(IC0CAPT,        0xCE);     /* Input Capture 0 */
SFR(IC0MODE,          0xCC);     /* Input Capture 0 Mode */
SFR(IC0STATUS,        0xCD);     /* Input Capture 0 Status */
SFR(IC1CAPT0,         0xD6);     /* Input Capture 1 Low Byte */
SFR(IC1CAPT1,         0xD7);     /* Input Capture 1 High Byte */
SFR16(IC1CAPT,        0xD6);     /* Input Capture 1 */
SFR(IC1MODE,          0xD4);     /* Input Capture 1 Mode */
SFR(IC1STATUS,        0xD5);     /* Input Capture 1 Status */
SFR(NVADDR0,          0x92);     /* Non-Volatile Memory Address Low Byte */
SFR(NVADDR1,          0x93);     /* Non-Volatile Memory Address High Byte */
SFR16(NVADDR,         0x92);     /* Non-Volatile Memory Address */
SFR(NVDATA0,          0x94);     /* Non-Volatile Memory Data Low Byte */
SFR(NVDATA1,          0x95);     /* Non-Volatile Memory Data High Byte */
SFR16(NVDATA,         0x94);     /* Non-Volatile Memory Data */
SFR(NVKEY,            0x96);     /* Non-Volatile Memory Write/Erase Key */
SFR(NVSTATUS,         0x91);     /* Non-Volatile Memory Command / Status  */
SFR(OC0COMP0,         0xBC);     /* Output Compare 0 Low Byte */
SFR(OC0COMP1,         0xBD);     /* Output Compare 0 High Byte */
SFR16(OC0COMP,        0xBC);     /* Output Compare 0 */
SFR(OC0MODE,          0xB9);     /* Output Compare 0 Mode */
SFR(OC0PIN,           0xBA);     /* Output Compare 0 Pin Configuration */
SFR(OC0STATUS,        0xBB);     /* Output Compare 0 Status */
SFR(OC1COMP0,         0xC4);     /* Output Compare 1 Low Byte */
SFR(OC1COMP1,         0xC5);     /* Output Compare 1 High Byte */
SFR16(OC1COMP,        0xC4);     /* Output Compare 1 */
SFR(OC1MODE,          0xC1);     /* Output Compare 1 Mode */
SFR(OC1PIN,           0xC2);     /* Output Compare 1 Pin Configuration */
SFR(OC1STATUS,        0xC3);     /* Output Compare 1 Status */
SFR(RADIOACC,         0xB1);     /* Radio Controller Access Mode */
SFR(RADIOADDR0,       0xB3);     /* Radio Register Address Low Byte */
SFR(RADIOADDR1,       0xB2);     /* Radio Register Address High Byte */
SFR16E(RADIOADDR,     0xB2B3);   /* Radio Register Address */
SFR(RADIODATA0,       0xB7);     /* Radio Register Data 0 */
SFR(RADIODATA1,       0xB6);     /* Radio Register Data 1 */
SFR(RADIODATA2,       0xB5);     /* Radio Register Data 2 */
SFR(RADIODATA3,       0xB4);     /* Radio Register Data 3 */
SFR32E(RADIODATA,  0xB4B5B6B7);  /* Radio Register Data */
SFR(RADIOSTAT0,       0xBE);     /* Radio Access Status Low Byte */
SFR(RADIOSTAT1,       0xBF);     /* Radio Access Status High Byte */
SFR16(RADIOSTAT,      0xBE);     /* Radio Access Status */
SFR(SPCLKSRC,         0xDF);     /* SPI Clock Source */
SFR(SPMODE,           0xDC);     /* SPI Mode */
SFR(SPSHREG,          0xDE);     /* SPI Shift Register */
SFR(SPSTATUS,         0xDD);     /* SPI Status */
SFR(T0CLKSRC,         0x9A);     /* Timer 0 Clock Source */
SFR(T0CNT0,           0x9C);     /* Timer 0 Count Low Byte */
SFR(T0CNT1,           0x9D);     /* Timer 0 Count High Byte */
SFR16(T0CNT,          0x9C);     /* Timer 0 Count */
SFR(T0MODE,           0x99);     /* Timer 0 Mode */
SFR(T0PERIOD0,        0x9E);     /* Timer 0 Period Low Byte */
SFR(T0PERIOD1,        0x9F);     /* Timer 0 Period High Byte */
SFR16(T0PERIOD,       0x9E);     /* Timer 0 Period */
SFR(T0STATUS,         0x9B);     /* Timer 0 Status */
SFR(T1CLKSRC,         0xA2);     /* Timer 1 Clock Source */
SFR(T1CNT0,           0xA4);     /* Timer 1 Count Low Byte */
SFR(T1CNT1,           0xA5);     /* Timer 1 Count High Byte */
SFR16(T1CNT,          0xA4);     /* Timer 1 Count */
SFR(T1MODE,           0xA1);     /* Timer 1 Mode */
SFR(T1PERIOD0,        0xA6);     /* Timer 1 Period Low Byte */
SFR(T1PERIOD1,        0xA7);     /* Timer 1 Period High Byte */
SFR16(T1PERIOD,       0xA6);     /* Timer 1 Period */
SFR(T1STATUS,         0xA3);     /* Timer 1 Status */
SFR(T2CLKSRC,         0xAA);     /* Timer 2 Clock Source */
SFR(T2CNT0,           0xAC);     /* Timer 2 Count Low Byte */
SFR(T2CNT1,           0xAD);     /* Timer 2 Count High Byte */
SFR16(T2CNT,          0xAC);     /* Timer 2 Count */
SFR(T2MODE,           0xA9);     /* Timer 2 Mode */
SFR(T2PERIOD0,        0xAE);     /* Timer 2 Period Low Byte */
SFR(T2PERIOD1,        0xAF);     /* Timer 2 Period High Byte */
SFR16(T2PERIOD,       0xAE);     /* Timer 2 Period */
SFR(T2STATUS,         0xAB);     /* Timer 2 Status */
SFR(U0CTRL,           0xE4);     /* UART 0 Control */
SFR(U0MODE,           0xE7);     /* UART 0 Mode */
SFR(U0SHREG,          0xE6);     /* UART 0 Shift Register */
SFR(U0STATUS,         0xE5);     /* UART 0 Status */
SFR(U1CTRL,           0xEC);     /* UART 1 Control */
SFR(U1MODE,           0xEF);     /* UART 1 Mode */
SFR(U1SHREG,          0xEE);     /* UART 1 Shift Register */
SFR(U1STATUS,         0xED);     /* UART 1 Status */
SFR(WDTCFG,           0xDA);     /* Watchdog Configuration */
SFR(WDTRESET,         0xDB);     /* Watchdog Reset */
SFR(WTCFGA,           0xF1);     /* Wakeup Timer A Configuration */
SFR(WTCFGB,           0xF9);     /* Wakeup Timer B Configuration */
SFR(WTCNTA0,          0xF2);     /* Wakeup Counter A Low Byte */
SFR(WTCNTA1,          0xF3);     /* Wakeup Counter A High Byte */
SFR16(WTCNTA,         0xF2);     /* Wakeup Counter A */
SFR(WTCNTB0,          0xFA);     /* Wakeup Counter B Low Byte */
SFR(WTCNTB1,          0xFB);     /* Wakeup Counter B High Byte */
SFR16(WTCNTB,         0xFA);     /* Wakeup Counter B */
SFR(WTCNTR1,          0xEB);     /* Wakeup Counter High Byte Latch */
SFR(WTEVTA0,          0xF4);     /* Wakeup Event A Low Byte */
SFR(WTEVTA1,          0xF5);     /* Wakeup Event A High Byte */
SFR16(WTEVTA,         0xF4);     /* Wakeup Event A */
SFR(WTEVTB0,          0xF6);     /* Wakeup Event B Low Byte */
SFR(WTEVTB1,          0xF7);     /* Wakeup Event B High Byte */
SFR16(WTEVTB,         0xF6);     /* Wakeup Event B */
SFR(WTEVTC0,          0xFC);     /* Wakeup Event C Low Byte */
SFR(WTEVTC1,          0xFD);     /* Wakeup Event C High Byte */
SFR16(WTEVTC,         0xFC);     /* Wakeup Event C */
SFR(WTEVTD0,          0xFE);     /* Wakeup Event D Low Byte */
SFR(WTEVTD1,          0xFF);     /* Wakeup Event D High Byte */
SFR16(WTEVTD,         0xFE);     /* Wakeup Event D */
SFR(WTIRQEN,          0xE9);     /* Wakeup Timer Interrupt Enable */
SFR(WTSTAT,           0xEA);     /* Wakeup Timer Status */

/* X Address Space */

#define AX8052_RADIOBASE		0x4000
#define AX8052_RADIOBASENB		0x5000

SFRX(ADCCALG00GAIN0,  0x7030);  /* ADC Calibration Range 00 Gain Low Byte */
SFRX(ADCCALG00GAIN1,  0x7031);  /* ADC Calibration Range 00 Gain High Byte */
SFR16LEX(ADCCALG00GAIN, 0x7030);  /* ADC Calibration Range 00 Gain */
SFRX(ADCCALG01GAIN0,  0x7032);  /* ADC Calibration Range 01 Gain Low Byte */
SFRX(ADCCALG01GAIN1,  0x7033);  /* ADC Calibration Range 01 Gain High Byte */
SFR16LEX(ADCCALG01GAIN, 0x7032);  /* ADC Calibration Range 01 Gain */
SFRX(ADCCALG10GAIN0,  0x7034);  /* ADC Calibration Range 10 Gain Low Byte */
SFRX(ADCCALG10GAIN1,  0x7035);  /* ADC Calibration Range 10 Gain High Byte */
SFR16LEX(ADCCALG10GAIN, 0x7034);  /* ADC Calibration Range 10 Gain */
SFRX(ADCCALTEMPGAIN0, 0x7038);  /* ADC Calibration Temperature Gain Low Byte */
SFRX(ADCCALTEMPGAIN1, 0x7039);  /* ADC Calibration Temperature Gain High Byte */
SFR16LEX(ADCCALTEMPGAIN, 0x7038);  /* ADC Calibration Temperature Gain */
SFRX(ADCCALTEMPOFFS0, 0x703A);  /* ADC Calibration Temperature Offset Low Byte */
SFRX(ADCCALTEMPOFFS1, 0x703B);  /* ADC Calibration Temperature Offset High Byte */
SFR16LEX(ADCCALTEMPOFFS, 0x703A);  /* ADC Calibration Temperature Offset */
SFRX(ADCCH0VAL0,      0x7020);  /* ADC Channel 0 Low Byte */
SFRX(ADCCH0VAL1,      0x7021);  /* ADC Channel 0 High Byte */
SFR16LEX(ADCCH0VAL,     0x7020);  /* ADC Channel 0 */
SFRX(ADCCH1VAL0,      0x7022);  /* ADC Channel 1 Low Byte */
SFRX(ADCCH1VAL1,      0x7023);  /* ADC Channel 1 High Byte */
SFR16LEX(ADCCH1VAL,     0x7022);  /* ADC Channel 1 */
SFRX(ADCCH2VAL0,      0x7024);  /* ADC Channel 2 Low Byte */
SFRX(ADCCH2VAL1,      0x7025);  /* ADC Channel 2 High Byte */
SFR16LEX(ADCCH2VAL,     0x7024);  /* ADC Channel 2 */
SFRX(ADCCH3VAL0,      0x7026);  /* ADC Channel 3 Low Byte */
SFRX(ADCCH3VAL1,      0x7027);  /* ADC Channel 3 High Byte */
SFR16LEX(ADCCH3VAL,     0x7026);  /* ADC Channel 3 */
SFRX(ADCTUNE0,        0x7028);  /* ADC Tuning 0 */
SFRX(ADCTUNE1,        0x7029);  /* ADC Tuning 1 */
SFRX(ADCTUNE2,        0x702A);  /* ADC Tuning 2 */
SFRX(AESCONFIG,       0x7091);  /* AES Configuration */
SFRX(AESCURBLOCK,     0x7098);  /* AES Current Block Number */
SFRX(AESINADDR0,      0x7094);  /* AES Input Address Low Byte */
SFRX(AESINADDR1,      0x7095);  /* AES Input Address High Byte */
SFR16LEX(AESINADDR,     0x7094);  /* AES Input Address */
SFRX(AESKEYADDR0,     0x7092);  /* AES Keystream Address Low Byte */
SFRX(AESKEYADDR1,     0x7093);  /* AES Keystream Address High Byte */
SFR16LEX(AESKEYADDR,    0x7092);  /* AES Keystream Address */
SFRX(AESMODE,         0x7090);  /* AES Mode */
SFRX(AESOUTADDR0,     0x7096);  /* AES Output Address Low Byte */
SFRX(AESOUTADDR1,     0x7097);  /* AES Output Address High Byte */
SFR16LEX(AESOUTADDR,    0x7096);  /* AES Output Address */
SFRX(CLOCKGATE,       0x7F1B);  /* Clock Gating */
SFRX(DMA0ADDR0,       0x7010);  /* DMA Channel 0 Address Low Byte */
SFRX(DMA0ADDR1,       0x7011);  /* DMA Channel 0 Address High Byte */
SFR16LEX(DMA0ADDR,      0x7010);  /* DMA Channel 0 Address */
SFRX(DMA0CONFIG,      0x7014);  /* DMA Channel 0 Configuration */
SFRX(DMA1ADDR0,       0x7012);  /* DMA Channel 1 Address Low Byte */
SFRX(DMA1ADDR1,       0x7013);  /* DMA Channel 1 Address High Byte */
SFR16LEX(DMA1ADDR,      0x7012);  /* DMA Channel 1 Address */
SFRX(DMA1CONFIG,      0x7015);  /* DMA Channel 1 Configuration */
SFRX(FRCOSCCONFIG,    0x7070);  /* Fast RC Oscillator Calibration Configuration */
SFRX(FRCOSCCTRL,      0x7071);  /* Fast RC Oscillator Control */
SFRX(FRCOSCFREQ0,     0x7076);  /* Fast RC Oscillator Frequency Tuning Low Byte */
SFRX(FRCOSCFREQ1,     0x7077);  /* Fast RC Oscillator Frequency Tuning High Byte */
SFR16LEX(FRCOSCFREQ,    0x7076);  /* Fast RC Oscillator Frequency Tuning */
SFRX(FRCOSCKFILT0,    0x7072);  /* Fast RC Oscillator Calibration Filter Constant Low Byte */
SFRX(FRCOSCKFILT1,    0x7073);  /* Fast RC Oscillator Calibration Filter Constant High Byte */
SFR16LEX(FRCOSCKFILT,   0x7072);  /* Fast RC Oscillator Calibration Filter Constant */
SFRX(FRCOSCPER0,      0x7078);  /* Fast RC Oscillator Period Low Byte */
SFRX(FRCOSCPER1,      0x7079);  /* Fast RC Oscillator Period High Byte */
SFR16LEX(FRCOSCPER,     0x7078);  /* Fast RC Oscillator Period */
SFRX(FRCOSCREF0,      0x7074);  /* Fast RC Oscillator Reference Frequency Low Byte */
SFRX(FRCOSCREF1,      0x7075);  /* Fast RC Oscillator Reference Frequency High Byte */
SFR16LEX(FRCOSCREF,     0x7074);  /* Fast RC Oscillator Reference Frequency */
SFRX(ANALOGA,         0x7007);  /* Port A Analog Mode */
SFRX(GPIOENABLE,      0x700C);  /* GPIO Port Enable */
SFRX(EXTIRQ,          0x7003);  /* External IRQ Configuration */
SFRX(INTCHGA,         0x7000);  /* Port A Interrupt on Change */
SFRX(INTCHGB,         0x7001);  /* Port B Interrupt on Change */
SFRX(INTCHGC,         0x7002);  /* Port C Interrupt on Change */
SFRX(PALTA,           0x7008);  /* Port A Alternate Function */
SFRX(PALTB,           0x7009);  /* Port B Alternate Function */
SFRX(PALTC,           0x700A);  /* Port C Alternate Function */
SFRX(PINCHGA,         0x7004);  /* Port A Level Change */
SFRX(PINCHGB,         0x7005);  /* Port B Level Change */
SFRX(PINCHGC,         0x7006);  /* Port C Level Change */
SFRX(PINSEL,          0x700B);  /* Port Input Selection */
SFRX(LPOSCCONFIG,     0x7060);  /* Low Power Oscillator Calibration Configuration */
SFRX(LPOSCFREQ0,      0x7066);  /* Low Power Oscillator Frequency Tuning Low Byte */
SFRX(LPOSCFREQ1,      0x7067);  /* Low Power Oscillator Frequency Tuning High Byte */
SFR16LEX(LPOSCFREQ,     0x7066);  /* Low Power Oscillator Frequency Tuning */
SFRX(LPOSCKFILT0,     0x7062);  /* Low Power Oscillator Calibration Filter Constant Low Byte */
SFRX(LPOSCKFILT1,     0x7063);  /* Low Power Oscillator Calibration Filter Constant High Byte */
SFR16LEX(LPOSCKFILT,    0x7062);  /* Low Power Oscillator Calibration Filter Constant */
SFRX(LPOSCPER0,       0x7068);  /* Low Power Oscillator Period Low Byte */
SFRX(LPOSCPER1,       0x7069);  /* Low Power Oscillator Period High Byte */
SFR16LEX(LPOSCPER,      0x7068);  /* Low Power Oscillator Period */
SFRX(LPOSCREF0,       0x7064);  /* Low Power Oscillator Reference Frequency Low Byte */
SFRX(LPOSCREF1,       0x7065);  /* Low Power Oscillator Reference Frequency High Byte */
SFR16LEX(LPOSCREF,      0x7064);  /* Low Power Oscillator Reference Frequency */
SFRX(LPXOSCGM,        0x7054);  /* Low Power Crystal Oscillator Transconductance */
SFRX(OSCCALIB,        0x7053);  /* Oscillator Calibration Interrupt / Status */
SFRX(OSCFORCERUN,     0x7050);  /* Oscillator Run Force */
SFRX(OSCREADY,        0x7052);  /* Oscillator Ready Status */
SFRX(OSCRUN,          0x7051);  /* Oscillator Run Status */
SFRX(POWCTRL0,        0x7F10);  /* Power Control 0 */
SFRX(POWCTRL1,        0x7F11);  /* Power Control 1 */
SFRX(POWCTRL2,        0x7F12);  /* Power Control 2 */
SFRX(RADIOFDATAADDR0, 0x7040);  /* Radio FIFO Data Register Address Low Byte */
SFRX(RADIOFDATAADDR1, 0x7041);  /* Radio FIFO Data Register Address High Byte */
SFR16LEX(RADIOFDATAADDR, 0x7040);  /* Radio FIFO Data Register Address */
SFRX(RADIOFSTATADDR0, 0x7042);  /* Radio FIFO Status Register Address Low Byte */
SFRX(RADIOFSTATADDR1, 0x7043);  /* Radio FIFO Status Register Address High Byte */
SFR16LEX(RADIOFSTATADDR, 0x7042);  /* Radio FIFO Status Register Address */
SFRX(RADIOMUX,        0x7044);  /* Radio Multiplexer Control */
SFRX(RNGBYTE,         0x7081);  /* True Random Byte */
SFRX(RNGCLKSRC0,      0x7082);  /* True Random Number Generator Clock Source 0 */
SFRX(RNGCLKSRC1,      0x7083);  /* True Random Number Generator Clock Source 1 */
SFRX(RNGMODE,         0x7080);  /* True Random Number Generator Mode */
SFRX(SCRATCH0,        0x7084);  /* Scratch Register 0 */
SFRX(SCRATCH1,        0x7085);  /* Scratch Register 1 */
SFRX(SCRATCH2,        0x7086);  /* Scratch Register 2 */
SFRX(SCRATCH3,        0x7087);  /* Scratch Register 3 */
SFRX(SILICONREV,      0x7F00);  /* Silicon Revision */
SFRX(SPAREIN,         0x7F02);  /* Spare Input */
SFRX(SPAREOUT,        0x7F01);  /* Spare Output */
SFRX(TESTMUXA,        0x7F09);  /* Test Multiplexer Port A */
SFRX(TESTMUXB,        0x7F0A);  /* Test Multiplexer Port B */
SFRX(TESTMUXC,        0x7F0B);  /* Test Multiplexer Port C */
SFRX(TESTOBS,         0x7F08);  /* Test Observation Address */
SFRX(TESTOBSREAD0,    0x7F0E);  /* Test Observation Vector Low Byte */
SFRX(TESTOBSREAD1,    0x7F0F);  /* Test Observation Vector High Byte */
SFRX(TESTOBSSKIP0,    0x7F0C);  /* Test Observation Vector Skip Low Byte */
SFRX(TESTOBSSKIP1,    0x7F0D);  /* Test Observation Vector Skip High Byte */
SFRX(TMMUX,           0x7F03);  /* Test Multiplexer Control */
SFRX(XTALAMPL,        0x7F19);  /* Crystal Oscillator Amplitude Control */
SFRX(XTALOSC,         0x7F18);  /* Crystal Oscillator Configuration */
SFRX(XTALREADY,       0x7F1A);  /* Crystal Oscillator Ready Mode */

/* X Address Space aliases of SFR Address Space Registers */

SFR16LEX(XDPTR0,        0x3F82);  /* Data Pointer 0 */
SFR16LEX(XDPTR1,        0x3F84);  /* Data Pointer 1 */
SFRX(XIE,             0x3FA8);  /* Interrupt Enable */
SFRX(XIP,             0x3FB8);  /* Interrupt Priority */
SFRX(XPCON,           0x3F87);  /* Power Mode Control */
SFRX(XADCCH0CONFIG,   0x3FCA);  /* ADC Channel 0 Configuration */
SFRX(XADCCH1CONFIG,   0x3FCB);  /* ADC Channel 1 Configuration */
SFRX(XADCCH2CONFIG,   0x3FD2);  /* ADC Channel 2 Configuration */
SFRX(XADCCH3CONFIG,   0x3FD3);  /* ADC Channel 3 Configuration */
SFRX(XADCCLKSRC,      0x3FD1);  /* ADC Clock Source */
SFRX(XADCCONV,        0x3FC9);  /* ADC Conversion Source */
SFRX(XANALOGCOMP,     0x3FE1);  /* Analog Comparators */
SFRX(XCLKCON,         0x3FC6);  /* Clock Control */
SFRX(XCLKSTAT,        0x3FC7);  /* Clock Status */
SFRX(XCODECONFIG,     0x3F97);  /* Code Space Configuration */
SFRX(XDBGLNKBUF,      0x3FE3);  /* Debug Link Buffer */
SFRX(XDBGLNKSTAT,     0x3FE2);  /* Debug Link Status */
SFRX(XDIRA,           0x3F89);  /* Port A Direction */
SFRX(XDIRB,           0x3F8A);  /* Port B Direction */
SFRX(XDIRC,           0x3F8B);  /* Port C Direction */
SFRX(XDIRR,           0x3F8E);  /* Port R Direction */
SFRX(XPINA,           0x3FC8);  /* Port A Input */
SFRX(XPINB,           0x3FE8);  /* Port B Input */
SFRX(XPINC,           0x3FF8);  /* Port C Input */
SFRX(XPINR,           0x3F8D);  /* Port R Input */
SFRX(XPORTA,          0x3F80);  /* Port A Output */
SFRX(XPORTB,          0x3F88);  /* Port B Output */
SFRX(XPORTC,          0x3F90);  /* Port C Output */
SFRX(XPORTR,          0x3F8C);  /* Port R Output */
SFRX(XIC0CAPT0,       0x3FCE);  /* Input Capture 0 Low Byte */
SFRX(XIC0CAPT1,       0x3FCF);  /* Input Capture 0 High Byte */
SFR16LEX(XIC0CAPT,      0x3FCE);  /* Input Capture 0 */
SFRX(XIC0MODE,        0x3FCC);  /* Input Capture 0 Mode */
SFRX(XIC0STATUS,      0x3FCD);  /* Input Capture 0 Status */
SFRX(XIC1CAPT0,       0x3FD6);  /* Input Capture 1 Low Byte */
SFRX(XIC1CAPT1,       0x3FD7);  /* Input Capture 1 High Byte */
SFR16LEX(XIC1CAPT,      0x3FD6);  /* Input Capture 1 */
SFRX(XIC1MODE,        0x3FD4);  /* Input Capture 1 Mode */
SFRX(XIC1STATUS,      0x3FD5);  /* Input Capture 1 Status */
SFRX(XNVADDR0,        0x3F92);  /* Non-Volatile Memory Address Low Byte */
SFRX(XNVADDR1,        0x3F93);  /* Non-Volatile Memory Address High Byte */
SFR16LEX(XNVADDR,       0x3F92);  /* Non-Volatile Memory Address */
SFRX(XNVDATA0,        0x3F94);  /* Non-Volatile Memory Data Low Byte */
SFRX(XNVDATA1,        0x3F95);  /* Non-Volatile Memory Data High Byte */
SFR16LEX(XNVDATA,       0x3F94);  /* Non-Volatile Memory Data */
SFRX(XNVKEY,          0x3F96);  /* Non-Volatile Memory Write/Erase Key */
SFRX(XNVSTATUS,       0x3F91);  /* Non-Volatile Memory Command / Status  */
SFRX(XOC0COMP0,       0x3FBC);  /* Output Compare 0 Low Byte */
SFRX(XOC0COMP1,       0x3FBD);  /* Output Compare 0 High Byte */
SFR16LEX(XOC0COMP,      0x3FBC);  /* Output Compare 0 */
SFRX(XOC0MODE,        0x3FB9);  /* Output Compare 0 Mode */
SFRX(XOC0PIN,         0x3FBA);  /* Output Compare 0 Pin Configuration */
SFRX(XOC0STATUS,      0x3FBB);  /* Output Compare 0 Status */
SFRX(XOC1COMP0,       0x3FC4);  /* Output Compare 1 Low Byte */
SFRX(XOC1COMP1,       0x3FC5);  /* Output Compare 1 High Byte */
SFR16LEX(XOC1COMP,      0x3FC4);  /* Output Compare 1 */
SFRX(XOC1MODE,        0x3FC1);  /* Output Compare 1 Mode */
SFRX(XOC1PIN,         0x3FC2);  /* Output Compare 1 Pin Configuration */
SFRX(XOC1STATUS,      0x3FC3);  /* Output Compare 1 Status */
SFRX(XRADIOACC,       0x3FB1);  /* Radio Controller Access Mode */
SFRX(XRADIOADDR0,     0x3FB3);  /* Radio Register Address Low Byte */
SFRX(XRADIOADDR1,     0x3FB2);  /* Radio Register Address High Byte */
SFRX(XRADIODATA0,     0x3FB7);  /* Radio Register Data 0 */
SFRX(XRADIODATA1,     0x3FB6);  /* Radio Register Data 1 */
SFRX(XRADIODATA2,     0x3FB5);  /* Radio Register Data 2 */
SFRX(XRADIODATA3,     0x3FB4);  /* Radio Register Data 3 */
SFRX(XRADIOSTAT0,     0x3FBE);  /* Radio Access Status Low Byte */
SFRX(XRADIOSTAT1,     0x3FBF);  /* Radio Access Status High Byte */
SFR16LEX(XRADIOSTAT,    0x3FBE);  /* Radio Access Status */
SFRX(XSPCLKSRC,       0x3FDF);  /* SPI Clock Source */
SFRX(XSPMODE,         0x3FDC);  /* SPI Mode */
SFRX(XSPSHREG,        0x3FDE);  /* SPI Shift Register */
SFRX(XSPSTATUS,       0x3FDD);  /* SPI Status */
SFRX(XT0CLKSRC,       0x3F9A);  /* Timer 0 Clock Source */
SFRX(XT0CNT0,         0x3F9C);  /* Timer 0 Count Low Byte */
SFRX(XT0CNT1,         0x3F9D);  /* Timer 0 Count High Byte */
SFR16LEX(XT0CNT,        0x3F9C);  /* Timer 0 Count */
SFRX(XT0MODE,         0x3F99);  /* Timer 0 Mode */
SFRX(XT0PERIOD0,      0x3F9E);  /* Timer 0 Period Low Byte */
SFRX(XT0PERIOD1,      0x3F9F);  /* Timer 0 Period High Byte */
SFR16LEX(XT0PERIOD,     0x3F9E);  /* Timer 0 Period */
SFRX(XT0STATUS,       0x3F9B);  /* Timer 0 Status */
SFRX(XT1CLKSRC,       0x3FA2);  /* Timer 1 Clock Source */
SFRX(XT1CNT0,         0x3FA4);  /* Timer 1 Count Low Byte */
SFRX(XT1CNT1,         0x3FA5);  /* Timer 1 Count High Byte */
SFR16LEX(XT1CNT,        0x3FA4);  /* Timer 1 Count */
SFRX(XT1MODE,         0x3FA1);  /* Timer 1 Mode */
SFRX(XT1PERIOD0,      0x3FA6);  /* Timer 1 Period Low Byte */
SFRX(XT1PERIOD1,      0x3FA7);  /* Timer 1 Period High Byte */
SFR16LEX(XT1PERIOD,     0x3FA6);  /* Timer 1 Period */
SFRX(XT1STATUS,       0x3FA3);  /* Timer 1 Status */
SFRX(XT2CLKSRC,       0x3FAA);  /* Timer 2 Clock Source */
SFRX(XT2CNT0,         0x3FAC);  /* Timer 2 Count Low Byte */
SFRX(XT2CNT1,         0x3FAD);  /* Timer 2 Count High Byte */
SFR16LEX(XT2CNT,        0x3FAC);  /* Timer 2 Count */
SFRX(XT2MODE,         0x3FA9);  /* Timer 2 Mode */
SFRX(XT2PERIOD0,      0x3FAE);  /* Timer 2 Period Low Byte */
SFRX(XT2PERIOD1,      0x3FAF);  /* Timer 2 Period High Byte */
SFR16LEX(XT2PERIOD,     0x3FAE);  /* Timer 2 Period */
SFRX(XT2STATUS,       0x3FAB);  /* Timer 2 Status */
SFRX(XU0CTRL,         0x3FE4);  /* UART 0 Control */
SFRX(XU0MODE,         0x3FE7);  /* UART 0 Mode */
SFRX(XU0SHREG,        0x3FE6);  /* UART 0 Shift Register */
SFRX(XU0STATUS,       0x3FE5);  /* UART 0 Status */
SFRX(XU1CTRL,         0x3FEC);  /* UART 1 Control */
SFRX(XU1MODE,         0x3FEF);  /* UART 1 Mode */
SFRX(XU1SHREG,        0x3FEE);  /* UART 1 Shift Register */
SFRX(XU1STATUS,       0x3FED);  /* UART 1 Status */
SFRX(XWDTCFG,         0x3FDA);  /* Watchdog Configuration */
SFRX(XWDTRESET,       0x3FDB);  /* Watchdog Reset */
SFRX(XWTCFGA,         0x3FF1);  /* Wakeup Timer A Configuration */
SFRX(XWTCFGB,         0x3FF9);  /* Wakeup Timer B Configuration */
SFRX(XWTCNTA0,        0x3FF2);  /* Wakeup Counter A Low Byte */
SFRX(XWTCNTA1,        0x3FF3);  /* Wakeup Counter A High Byte */
SFR16LEX(XWTCNTA,       0x3FF2);  /* Wakeup Counter A */
SFRX(XWTCNTB0,        0x3FFA);  /* Wakeup Counter B Low Byte */
SFRX(XWTCNTB1,        0x3FFB);  /* Wakeup Counter B High Byte */
SFR16LEX(XWTCNTB,       0x3FFA);  /* Wakeup Counter B */
SFRX(XWTCNTR1,        0x3FEB);  /* Wakeup Counter High Byte Latch */
SFRX(XWTEVTA0,        0x3FF4);  /* Wakeup Event A Low Byte */
SFRX(XWTEVTA1,        0x3FF5);  /* Wakeup Event A High Byte */
SFR16LEX(XWTEVTA,       0x3FF4);  /* Wakeup Event A */
SFRX(XWTEVTB0,        0x3FF6);  /* Wakeup Event B Low Byte */
SFRX(XWTEVTB1,        0x3FF7);  /* Wakeup Event B High Byte */
SFR16LEX(XWTEVTB,       0x3FF6);  /* Wakeup Event B */
SFRX(XWTEVTC0,        0x3FFC);  /* Wakeup Event C Low Byte */
SFRX(XWTEVTC1,        0x3FFD);  /* Wakeup Event C High Byte */
SFR16LEX(XWTEVTC,       0x3FFC);  /* Wakeup Event C */
SFRX(XWTEVTD0,        0x3FFE);  /* Wakeup Event D Low Byte */
SFRX(XWTEVTD1,        0x3FFF);  /* Wakeup Event D High Byte */
SFR16LEX(XWTEVTD,       0x3FFE);  /* Wakeup Event D */
SFRX(XWTIRQEN,        0x3FE9);  /* Wakeup Timer Interrupt Enable */
SFRX(XWTSTAT,         0x3FEA);  /* Wakeup Timer Status */


/* Radio Registers, X Address Space */

SFRX(AX5043_AFSKCTRL,              0x4114);  /* AFSK Control */
SFRX(AX5043_AFSKMARK0,             0x4113);  /* AFSK Mark (1) Frequency 0 */
SFRX(AX5043_AFSKMARK1,             0x4112);  /* AFSK Mark (1) Frequency 1 */
SFRX(AX5043_AFSKSPACE0,            0x4111);  /* AFSK Space (0) Frequency 0 */
SFRX(AX5043_AFSKSPACE1,            0x4110);  /* AFSK Space (0) Frequency 1 */
SFRX(AX5043_AGCCOUNTER,            0x4043);  /* AGC Counter */
SFRX(AX5043_AMPLFILTER,            0x4115);  /* Amplitude Filter */
SFRX(AX5043_BBOFFSCAP,             0x4189);  /* Baseband Offset Compensation Capacitors */
SFRX(AX5043_BBTUNE,                0x4188);  /* Baseband Tuning */
SFRX(AX5043_BGNDRSSI,              0x4041);  /* Background RSSI */
SFRX(AX5043_BGNDRSSIGAIN,          0x422E);  /* Background RSSI Averaging Time Constant */
SFRX(AX5043_BGNDRSSITHR,           0x422F);  /* Background RSSI Relative Threshold */
SFRX(AX5043_CRCINIT0,              0x4017);  /* CRC Initial Value 0 */
SFRX(AX5043_CRCINIT1,              0x4016);  /* CRC Initial Value 1 */
SFRX(AX5043_CRCINIT2,              0x4015);  /* CRC Initial Value 2 */
SFRX(AX5043_CRCINIT3,              0x4014);  /* CRC Initial Value 3 */
SFRX(AX5043_DACCONFIG,             0x4332);  /* DAC Configuration */
SFRX(AX5043_DACVALUE0,             0x4331);  /* DAC Value 0 */
SFRX(AX5043_DACVALUE1,             0x4330);  /* DAC Value 1 */
SFRX(AX5043_DECIMATION,            0x4102);  /* Decimation Factor  */
SFRX(AX5043_DIVERSITY,             0x4042);  /* Antenna Diversity Configuration */
SFRX(AX5043_DSPMODECFG,            0x4320);  /* DSP Mode Setting */
SFRX(AX5043_DSPMODESHREG,          0x406F);  /* DSPmode SPI Shift Register Access */
SFRX(AX5043_DSPMODESKIP0,          0x4322);  /* DSP Mode Skip 0 */
SFRX(AX5043_DSPMODESKIP1,          0x4321);  /* DSP Mode Skip 1 */
SFRX(AX5043_ENCODING,              0x4011);  /* Encoding */
SFRX(AX5043_FEC,                   0x4018);  /* Forward Error Correction */
SFRX(AX5043_FECSTATUS,             0x401A);  /* Forward Error Correction Status */
SFRX(AX5043_FECSYNC,               0x4019);  /* Forward Error Correction Sync Threshold */
SFRX(AX5043_FIFOCOUNT0,            0x402B);  /* Number of Words currently in FIFO 0 */
SFRX(AX5043_FIFOCOUNT1,            0x402A);  /* Number of Words currently in FIFO 1 */
SFRX(AX5043_FIFODATA,              0x4029);  /* FIFO Data */
SFRX(AX5043_FIFOFREE0,             0x402D);  /* Number of Words that can be written to FIFO 0 */
SFRX(AX5043_FIFOFREE1,             0x402C);  /* Number of Words that can be written to FIFO 1 */
SFRX(AX5043_FIFOSTAT,              0x4028);  /* FIFO Control */
SFRX(AX5043_FIFOTHRESH0,           0x402F);  /* FIFO Threshold 0 */
SFRX(AX5043_FIFOTHRESH1,           0x402E);  /* FIFO Threshold 1 */
SFRX(AX5043_FRAMING,               0x4012);  /* Framing Mode */
SFRX(AX5043_FREQA0,                0x4037);  /* Frequency A 0 */
SFRX(AX5043_FREQA1,                0x4036);  /* Frequency A 1 */
SFRX(AX5043_FREQA2,                0x4035);  /* Frequency A 2 */
SFRX(AX5043_FREQA3,                0x4034);  /* Frequency A 3 */
SFRX(AX5043_FREQB0,                0x403F);  /* Frequency B 0 */
SFRX(AX5043_FREQB1,                0x403E);  /* Frequency B 1 */
SFRX(AX5043_FREQB2,                0x403D);  /* Frequency B 2 */
SFRX(AX5043_FREQB3,                0x403C);  /* Frequency B 3 */
SFRX(AX5043_FSKDEV0,               0x4163);  /* FSK Deviation 0 */
SFRX(AX5043_FSKDEV1,               0x4162);  /* FSK Deviation 1 */
SFRX(AX5043_FSKDEV2,               0x4161);  /* FSK Deviation 2 */
SFRX(AX5043_FSKDMAX0,              0x410D);  /* Four FSK Rx Maximum Deviation 0 */
SFRX(AX5043_FSKDMAX1,              0x410C);  /* Four FSK Rx Maximum Deviation 1 */
SFRX(AX5043_FSKDMIN0,              0x410F);  /* Four FSK Rx Minimum Deviation 0 */
SFRX(AX5043_FSKDMIN1,              0x410E);  /* Four FSK Rx Minimum Deviation 1 */
SFRX(AX5043_GPADC13VALUE0,         0x4309);  /* GPADC13 Value 0 */
SFRX(AX5043_GPADC13VALUE1,         0x4308);  /* GPADC13 Value 1 */
SFRX(AX5043_GPADC1VALUE0,          0x430B);  /* GPADC1 Value 0 */
SFRX(AX5043_GPADC1VALUE1,          0x430A);  /* GPADC1 Value 1 */
SFRX(AX5043_GPADC2VALUE0,          0x430D);  /* GPADC2 Value 0 */
SFRX(AX5043_GPADC2VALUE1,          0x430C);  /* GPADC2 Value 1 */
SFRX(AX5043_GPADC3VALUE0,          0x430F);  /* GPADC3 Value 0 */
SFRX(AX5043_GPADC3VALUE1,          0x430E);  /* GPADC3 Value 1 */
SFRX(AX5043_GPADCCTRL,             0x4300);  /* General Purpose ADC Control */
SFRX(AX5043_GPADCPERIOD,           0x4301);  /* GPADC Sampling Period */
SFRX(AX5043_IFFREQ0,               0x4101);  /* 2nd LO / IF Frequency 0 */
SFRX(AX5043_IFFREQ1,               0x4100);  /* 2nd LO / IF Frequency 1 */
SFRX(AX5043_IRQINVERSION0,         0x400B);  /* IRQ Inversion 0 */
SFRX(AX5043_IRQINVERSION1,         0x400A);  /* IRQ Inversion 1 */
SFRX(AX5043_IRQMASK0,              0x4007);  /* IRQ Mask 0 */
SFRX(AX5043_IRQMASK1,              0x4006);  /* IRQ Mask 1 */
SFRX(AX5043_IRQREQUEST0,           0x400D);  /* IRQ Request 0 */
SFRX(AX5043_IRQREQUEST1,           0x400C);  /* IRQ Request 1 */
SFRX(AX5043_LPOSCCONFIG,           0x4310);  /* Low Power Oscillator Calibration Configuration */
SFRX(AX5043_LPOSCFREQ0,            0x4317);  /* Low Power Oscillator Frequency Tuning Low Byte */
SFRX(AX5043_LPOSCFREQ1,            0x4316);  /* Low Power Oscillator Frequency Tuning High Byte */
SFRX(AX5043_LPOSCKFILT0,           0x4313);  /* Low Power Oscillator Calibration Filter Constant Low Byte */
SFRX(AX5043_LPOSCKFILT1,           0x4312);  /* Low Power Oscillator Calibration Filter Constant High Byte */
SFRX(AX5043_LPOSCPER0,             0x4319);  /* Low Power Oscillator Period Low Byte */
SFRX(AX5043_LPOSCPER1,             0x4318);  /* Low Power Oscillator Period High Byte */
SFRX(AX5043_LPOSCREF0,             0x4315);  /* Low Power Oscillator Reference Frequency Low Byte */
SFRX(AX5043_LPOSCREF1,             0x4314);  /* Low Power Oscillator Reference Frequency High Byte */
SFRX(AX5043_LPOSCSTATUS,           0x4311);  /* Low Power Oscillator Calibration Status */
SFRX(AX5043_MATCH0LEN,             0x4214);  /* Pattern Match Unit 0, Pattern Length */
SFRX(AX5043_MATCH0MAX,             0x4216);  /* Pattern Match Unit 0, Maximum Match */
SFRX(AX5043_MATCH0MIN,             0x4215);  /* Pattern Match Unit 0, Minimum Match */
SFRX(AX5043_MATCH0PAT0,            0x4213);  /* Pattern Match Unit 0, Pattern 0 */
SFRX(AX5043_MATCH0PAT1,            0x4212);  /* Pattern Match Unit 0, Pattern 1 */
SFRX(AX5043_MATCH0PAT2,            0x4211);  /* Pattern Match Unit 0, Pattern 2 */
SFRX(AX5043_MATCH0PAT3,            0x4210);  /* Pattern Match Unit 0, Pattern 3 */
SFRX(AX5043_MATCH1LEN,             0x421C);  /* Pattern Match Unit 1, Pattern Length */
SFRX(AX5043_MATCH1MAX,             0x421E);  /* Pattern Match Unit 1, Maximum Match */
SFRX(AX5043_MATCH1MIN,             0x421D);  /* Pattern Match Unit 1, Minimum Match */
SFRX(AX5043_MATCH1PAT0,            0x4219);  /* Pattern Match Unit 1, Pattern 0 */
SFRX(AX5043_MATCH1PAT1,            0x4218);  /* Pattern Match Unit 1, Pattern 1 */
SFRX(AX5043_MAXDROFFSET0,          0x4108);  /* Maximum Receiver Datarate Offset 0 */
SFRX(AX5043_MAXDROFFSET1,          0x4107);  /* Maximum Receiver Datarate Offset 1 */
SFRX(AX5043_MAXDROFFSET2,          0x4106);  /* Maximum Receiver Datarate Offset 2 */
SFRX(AX5043_MAXRFOFFSET0,          0x410B);  /* Maximum Receiver RF Offset 0 */
SFRX(AX5043_MAXRFOFFSET1,          0x410A);  /* Maximum Receiver RF Offset 1 */
SFRX(AX5043_MAXRFOFFSET2,          0x4109);  /* Maximum Receiver RF Offset 2 */
SFRX(AX5043_MODCFGA,               0x4164);  /* Modulator Configuration A */
SFRX(AX5043_MODCFGF,               0x4160);  /* Modulator Configuration F */
SFRX(AX5043_MODULATION,            0x4010);  /* Modulation */
SFRX(AX5043_PINFUNCANTSEL,         0x4025);  /* Pin Function ANTSEL */
SFRX(AX5043_PINFUNCDATA,           0x4023);  /* Pin Function DATA */
SFRX(AX5043_PINFUNCDCLK,           0x4022);  /* Pin Function DCLK */
SFRX(AX5043_PINFUNCIRQ,            0x4024);  /* Pin Function IRQ */
SFRX(AX5043_PINFUNCPWRAMP,         0x4026);  /* Pin Function PWRAMP */
SFRX(AX5043_PINFUNCSYSCLK,         0x4021);  /* Pin Function SYSCLK */
SFRX(AX5043_PINSTATE,              0x4020);  /* Pin State */
SFRX(AX5043_PKTACCEPTFLAGS,        0x4233);  /* Packet Controller Accept Flags */
SFRX(AX5043_PKTCHUNKSIZE,          0x4230);  /* Packet Chunk Size */
SFRX(AX5043_PKTMISCFLAGS,          0x4231);  /* Packet Controller Miscellaneous Flags */
SFRX(AX5043_PKTSTOREFLAGS,         0x4232);  /* Packet Controller Store Flags */
SFRX(AX5043_PLLCPI,                0x4031);  /* PLL Charge Pump Current */
SFRX(AX5043_PLLCPIBOOST,           0x4039);  /* PLL Charge Pump Current (Boosted) */
SFRX(AX5043_PLLLOCKDET,            0x4182);  /* PLL Lock Detect Delay */
SFRX(AX5043_PLLLOOP,               0x4030);  /* PLL Loop Filter Settings */
SFRX(AX5043_PLLLOOPBOOST,          0x4038);  /* PLL Loop Filter Settings (Boosted) */
SFRX(AX5043_PLLRANGINGA,           0x4033);  /* PLL Autoranging A */
SFRX(AX5043_PLLRANGINGB,           0x403B);  /* PLL Autoranging B */
SFRX(AX5043_PLLRNGCLK,             0x4183);  /* PLL Autoranging Clock */
SFRX(AX5043_PLLVCODIV,             0x4032);  /* PLL Divider Settings */
SFRX(AX5043_PLLVCOI,               0x4180);  /* PLL VCO Current */
SFRX(AX5043_PLLVCOIR,              0x4181);  /* PLL VCO Current Readback */
SFRX(AX5043_POWIRQMASK,            0x4005);  /* Power Management Interrupt Mask */
SFRX(AX5043_POWSTAT,               0x4003);  /* Power Management Status */
SFRX(AX5043_POWSTICKYSTAT,         0x4004);  /* Power Management Sticky Status */
SFRX(AX5043_PWRAMP,                0x4027);  /* PWRAMP Control */
SFRX(AX5043_PWRMODE,               0x4002);  /* Power Mode */
SFRX(AX5043_RADIOEVENTMASK0,       0x4009);  /* Radio Event Mask 0 */
SFRX(AX5043_RADIOEVENTMASK1,       0x4008);  /* Radio Event Mask 1 */
SFRX(AX5043_RADIOEVENTREQ0,        0x400F);  /* Radio Event Request 0 */
SFRX(AX5043_RADIOEVENTREQ1,        0x400E);  /* Radio Event Request 1 */
SFRX(AX5043_RADIOSTATE,            0x401C);  /* Radio Controller State */
SFRX(AX5043_RSSI,                  0x4040);  /* Received Signal Strength Indicator */
SFRX(AX5043_RSSIABSTHR,            0x422D);  /* RSSI Absolute Threshold */
SFRX(AX5043_RSSIREFERENCE,         0x422C);  /* RSSI Offset */
SFRX(AX5043_RXDATARATE0,           0x4105);  /* Receiver Datarate 0 */
SFRX(AX5043_RXDATARATE1,           0x4104);  /* Receiver Datarate 1 */
SFRX(AX5043_RXDATARATE2,           0x4103);  /* Receiver Datarate 2 */
SFRX(AX5043_SCRATCH,               0x4001);  /* Scratch */
SFRX(AX5043_SILICONREVISION,       0x4000);  /* Silicon Revision */
SFRX(AX5043_TIMER0,                0x405B);  /* 1MHz Timer 0 */
SFRX(AX5043_TIMER1,                0x405A);  /* 1MHz Timer 1 */
SFRX(AX5043_TIMER2,                0x4059);  /* 1MHz Timer 2 */
SFRX(AX5043_TMGRXAGC,              0x4227);  /* Receiver AGC Settling Time */
SFRX(AX5043_TMGRXBOOST,            0x4223);  /* Receive PLL Boost Time */
SFRX(AX5043_TMGRXCOARSEAGC,        0x4226);  /* Receive Coarse AGC Time */
SFRX(AX5043_TMGRXOFFSACQ,          0x4225);  /* Receive Baseband DC Offset Acquisition Time */
SFRX(AX5043_TMGRXPREAMBLE1,        0x4229);  /* Receiver Preamble 1 Timeout */
SFRX(AX5043_TMGRXPREAMBLE2,        0x422A);  /* Receiver Preamble 2 Timeout */
SFRX(AX5043_TMGRXPREAMBLE3,        0x422B);  /* Receiver Preamble 3 Timeout */
SFRX(AX5043_TMGRXRSSI,             0x4228);  /* Receiver RSSI Settling Time */
SFRX(AX5043_TMGRXSETTLE,           0x4224);  /* Receive PLL (post Boost) Settling Time */
SFRX(AX5043_TMGTXBOOST,            0x4220);  /* Transmit PLL Boost Time */
SFRX(AX5043_TMGTXSETTLE,           0x4221);  /* Transmit PLL (post Boost) Settling Time */
SFRX(AX5043_TRKAFSKDEMOD0,         0x4055);  /* AFSK Demodulator Tracking 0 */
SFRX(AX5043_TRKAFSKDEMOD1,         0x4054);  /* AFSK Demodulator Tracking 1 */
SFRX(AX5043_TRKAMPLITUDE0,         0x4049);  /* Amplitude Tracking 0 */
SFRX(AX5043_TRKAMPLITUDE1,         0x4048);  /* Amplitude Tracking 1 */
SFRX(AX5043_TRKDATARATE0,          0x4047);  /* Datarate Tracking 0 */
SFRX(AX5043_TRKDATARATE1,          0x4046);  /* Datarate Tracking 1 */
SFRX(AX5043_TRKDATARATE2,          0x4045);  /* Datarate Tracking 2 */
SFRX(AX5043_TRKFREQ0,              0x4051);  /* Frequency Tracking 0 */
SFRX(AX5043_TRKFREQ1,              0x4050);  /* Frequency Tracking 1 */
SFRX(AX5043_TRKFSKDEMOD0,          0x4053);  /* FSK Demodulator Tracking 0 */
SFRX(AX5043_TRKFSKDEMOD1,          0x4052);  /* FSK Demodulator Tracking 1 */
SFRX(AX5043_TRKPHASE0,             0x404B);  /* Phase Tracking 0 */
SFRX(AX5043_TRKPHASE1,             0x404A);  /* Phase Tracking 1 */
SFRX(AX5043_TRKRFFREQ0,            0x404F);  /* RF Frequency Tracking 0 */
SFRX(AX5043_TRKRFFREQ1,            0x404E);  /* RF Frequency Tracking 1 */
SFRX(AX5043_TRKRFFREQ2,            0x404D);  /* RF Frequency Tracking 2 */
SFRX(AX5043_TXPWRCOEFFA0,          0x4169);  /* Transmitter Predistortion Coefficient A 0 */
SFRX(AX5043_TXPWRCOEFFA1,          0x4168);  /* Transmitter Predistortion Coefficient A 1 */
SFRX(AX5043_TXPWRCOEFFB0,          0x416B);  /* Transmitter Predistortion Coefficient B 0 */
SFRX(AX5043_TXPWRCOEFFB1,          0x416A);  /* Transmitter Predistortion Coefficient B 1 */
SFRX(AX5043_TXPWRCOEFFC0,          0x416D);  /* Transmitter Predistortion Coefficient C 0 */
SFRX(AX5043_TXPWRCOEFFC1,          0x416C);  /* Transmitter Predistortion Coefficient C 1 */
SFRX(AX5043_TXPWRCOEFFD0,          0x416F);  /* Transmitter Predistortion Coefficient D 0 */
SFRX(AX5043_TXPWRCOEFFD1,          0x416E);  /* Transmitter Predistortion Coefficient D 1 */
SFRX(AX5043_TXPWRCOEFFE0,          0x4171);  /* Transmitter Predistortion Coefficient E 0 */
SFRX(AX5043_TXPWRCOEFFE1,          0x4170);  /* Transmitter Predistortion Coefficient E 1 */
SFRX(AX5043_TXRATE0,               0x4167);  /* Transmitter Bitrate 0 */
SFRX(AX5043_TXRATE1,               0x4166);  /* Transmitter Bitrate 1 */
SFRX(AX5043_TXRATE2,               0x4165);  /* Transmitter Bitrate 2 */
SFRX(AX5043_WAKEUP0,               0x406B);  /* Wakeup Time 0 */
SFRX(AX5043_WAKEUP1,               0x406A);  /* Wakeup Time 1 */
SFRX(AX5043_WAKEUPFREQ0,           0x406D);  /* Wakeup Frequency 0 */
SFRX(AX5043_WAKEUPFREQ1,           0x406C);  /* Wakeup Frequency 1 */
SFRX(AX5043_WAKEUPTIMER0,          0x4069);  /* Wakeup Timer 0 */
SFRX(AX5043_WAKEUPTIMER1,          0x4068);  /* Wakeup Timer 1 */
SFRX(AX5043_WAKEUPXOEARLY,         0x406E);  /* Wakeup Crystal Oscillator Early */
SFRX(AX5043_XTALCAP,               0x4184);  /* Crystal Oscillator Load Capacitance */
SFRX(AX5043_XTALSTATUS,            0x401D);  /* Crystal Oscillator Status */

#if defined AX5043V1
SFRX(AX5043_AGCGAIN0,              0x4120);  /* AGC Speed */
SFRX(AX5043_AGCGAIN1,              0x412E);  /* AGC Speed */
SFRX(AX5043_AGCGAIN2,              0x413C);  /* AGC Speed */
SFRX(AX5043_AGCGAIN3,              0x414A);  /* AGC Speed */
SFRX(AX5043_AGCTARGET0,            0x4121);  /* AGC Target */
SFRX(AX5043_AGCTARGET1,            0x412F);  /* AGC Target */
SFRX(AX5043_AGCTARGET2,            0x413D);  /* AGC Target */
SFRX(AX5043_AGCTARGET3,            0x414B);  /* AGC Target */
SFRX(AX5043_AMPLITUDEGAIN0,        0x4129);  /* Amplitude Estimator Bandwidth */
SFRX(AX5043_AMPLITUDEGAIN1,        0x4137);  /* Amplitude Estimator Bandwidth */
SFRX(AX5043_AMPLITUDEGAIN2,        0x4145);  /* Amplitude Estimator Bandwidth */
SFRX(AX5043_AMPLITUDEGAIN3,        0x4153);  /* Amplitude Estimator Bandwidth */
SFRX(AX5043_BBOFFSRES0,            0x412D);  /* Baseband Offset Compensation Resistors */
SFRX(AX5043_BBOFFSRES1,            0x413B);  /* Baseband Offset Compensation Resistors */
SFRX(AX5043_BBOFFSRES2,            0x4149);  /* Baseband Offset Compensation Resistors */
SFRX(AX5043_BBOFFSRES3,            0x4157);  /* Baseband Offset Compensation Resistors */
SFRX(AX5043_DRGAIN0,               0x4123);  /* Data Rate Estimator Bandwidth */
SFRX(AX5043_DRGAIN1,               0x4131);  /* Data Rate Estimator Bandwidth */
SFRX(AX5043_DRGAIN2,               0x413F);  /* Data Rate Estimator Bandwidth */
SFRX(AX5043_DRGAIN3,               0x414D);  /* Data Rate Estimator Bandwidth */
SFRX(AX5043_FOURFSK0,              0x412C);  /* Four FSK Control */
SFRX(AX5043_FOURFSK1,              0x413A);  /* Four FSK Control */
SFRX(AX5043_FOURFSK2,              0x4148);  /* Four FSK Control */
SFRX(AX5043_FOURFSK3,              0x4156);  /* Four FSK Control */
SFRX(AX5043_FREQDEV00,             0x412B);  /* Receiver Frequency Deviation 0 */
SFRX(AX5043_FREQDEV01,             0x4139);  /* Receiver Frequency Deviation 0 */
SFRX(AX5043_FREQDEV02,             0x4147);  /* Receiver Frequency Deviation 0 */
SFRX(AX5043_FREQDEV03,             0x4155);  /* Receiver Frequency Deviation 0 */
SFRX(AX5043_FREQDEV10,             0x412A);  /* Receiver Frequency Deviation 1 */
SFRX(AX5043_FREQDEV11,             0x4138);  /* Receiver Frequency Deviation 1 */
SFRX(AX5043_FREQDEV12,             0x4146);  /* Receiver Frequency Deviation 1 */
SFRX(AX5043_FREQDEV13,             0x4154);  /* Receiver Frequency Deviation 1 */
SFRX(AX5043_FREQUENCYGAINA0,       0x4125);  /* Frequency Estimator Bandwidth A */
SFRX(AX5043_FREQUENCYGAINA1,       0x4133);  /* Frequency Estimator Bandwidth A */
SFRX(AX5043_FREQUENCYGAINA2,       0x4141);  /* Frequency Estimator Bandwidth A */
SFRX(AX5043_FREQUENCYGAINA3,       0x414F);  /* Frequency Estimator Bandwidth A */
SFRX(AX5043_FREQUENCYGAINB0,       0x4126);  /* Frequency Estimator Bandwidth B */
SFRX(AX5043_FREQUENCYGAINB1,       0x4134);  /* Frequency Estimator Bandwidth B */
SFRX(AX5043_FREQUENCYGAINB2,       0x4142);  /* Frequency Estimator Bandwidth B */
SFRX(AX5043_FREQUENCYGAINB3,       0x4150);  /* Frequency Estimator Bandwidth B */
SFRX(AX5043_FREQUENCYGAINC0,       0x4127);  /* Frequency Estimator Bandwidth C */
SFRX(AX5043_FREQUENCYGAINC1,       0x4135);  /* Frequency Estimator Bandwidth C */
SFRX(AX5043_FREQUENCYGAINC2,       0x4143);  /* Frequency Estimator Bandwidth C */
SFRX(AX5043_FREQUENCYGAINC3,       0x4151);  /* Frequency Estimator Bandwidth C */
SFRX(AX5043_FREQUENCYGAIND0,       0x4128);  /* Frequency Estimator Bandwidth D */
SFRX(AX5043_FREQUENCYGAIND1,       0x4136);  /* Frequency Estimator Bandwidth D */
SFRX(AX5043_FREQUENCYGAIND2,       0x4144);  /* Frequency Estimator Bandwidth D */
SFRX(AX5043_FREQUENCYGAIND3,       0x4152);  /* Frequency Estimator Bandwidth D */
SFRX(AX5043_PHASEGAIN0,            0x4124);  /* Phase Estimator Bandwidth */
SFRX(AX5043_PHASEGAIN1,            0x4132);  /* Phase Estimator Bandwidth */
SFRX(AX5043_PHASEGAIN2,            0x4140);  /* Phase Estimator Bandwidth */
SFRX(AX5043_PHASEGAIN3,            0x414E);  /* Phase Estimator Bandwidth */
SFRX(AX5043_PKTADDR0,              0x4203);  /* Packet Address 0 */
SFRX(AX5043_PKTADDR1,              0x4202);  /* Packet Address 1 */
SFRX(AX5043_PKTADDRCFG,            0x4201);  /* Packet Address Config */
SFRX(AX5043_PKTADDRMASK0,          0x4205);  /* Packet Address Mask 0 */
SFRX(AX5043_PKTADDRMASK1,          0x4204);  /* Packet Address Mask 1 */
SFRX(AX5043_PKTLENCFG,             0x4206);  /* Packet Length Configuration */
SFRX(AX5043_PKTLENOFFSET,          0x4207);  /* Packet Length Offset */
SFRX(AX5043_PKTMAXLEN,             0x4208);  /* Packet Maximum Length */
SFRX(AX5043_RXPARAMCURSET,         0x4117);  /* Receiver Parameter Current Set */
SFRX(AX5043_RXPARAMSETS,           0x4116);  /* Receiver Parameter Set Indirection */
SFRX(AX5043_TIMEGAIN0,             0x4122);  /* Time Estimator Bandwidth */
SFRX(AX5043_TIMEGAIN1,             0x4130);  /* Time Estimator Bandwidth */
SFRX(AX5043_TIMEGAIN2,             0x413E);  /* Time Estimator Bandwidth */
SFRX(AX5043_TIMEGAIN3,             0x414C);  /* Time Estimator Bandwidth */
#else
SFRX(AX5043_AGCAHYST0,             0x4122);  /* AGC Analog Hysteresis */
SFRX(AX5043_AGCAHYST1,             0x4132);  /* AGC Analog Hysteresis */
SFRX(AX5043_AGCAHYST2,             0x4142);  /* AGC Analog Hysteresis */
SFRX(AX5043_AGCAHYST3,             0x4152);  /* AGC Analog Hysteresis */
SFRX(AX5043_AGCGAIN0,              0x4120);  /* AGC Speed */
SFRX(AX5043_AGCGAIN1,              0x4130);  /* AGC Speed */
SFRX(AX5043_AGCGAIN2,              0x4140);  /* AGC Speed */
SFRX(AX5043_AGCGAIN3,              0x4150);  /* AGC Speed */
SFRX(AX5043_AGCMINMAX0,            0x4123);  /* AGC Analog Update Behaviour */
SFRX(AX5043_AGCMINMAX1,            0x4133);  /* AGC Analog Update Behaviour */
SFRX(AX5043_AGCMINMAX2,            0x4143);  /* AGC Analog Update Behaviour */
SFRX(AX5043_AGCMINMAX3,            0x4153);  /* AGC Analog Update Behaviour */
SFRX(AX5043_AGCTARGET0,            0x4121);  /* AGC Target */
SFRX(AX5043_AGCTARGET1,            0x4131);  /* AGC Target */
SFRX(AX5043_AGCTARGET2,            0x4141);  /* AGC Target */
SFRX(AX5043_AGCTARGET3,            0x4151);  /* AGC Target */
SFRX(AX5043_AMPLITUDEGAIN0,        0x412B);  /* Amplitude Estimator Bandwidth */
SFRX(AX5043_AMPLITUDEGAIN1,        0x413B);  /* Amplitude Estimator Bandwidth */
SFRX(AX5043_AMPLITUDEGAIN2,        0x414B);  /* Amplitude Estimator Bandwidth */
SFRX(AX5043_AMPLITUDEGAIN3,        0x415B);  /* Amplitude Estimator Bandwidth */
SFRX(AX5043_BBOFFSRES0,            0x412F);  /* Baseband Offset Compensation Resistors */
SFRX(AX5043_BBOFFSRES1,            0x413F);  /* Baseband Offset Compensation Resistors */
SFRX(AX5043_BBOFFSRES2,            0x414F);  /* Baseband Offset Compensation Resistors */
SFRX(AX5043_BBOFFSRES3,            0x415F);  /* Baseband Offset Compensation Resistors */
SFRX(AX5043_DRGAIN0,               0x4125);  /* Data Rate Estimator Bandwidth */
SFRX(AX5043_DRGAIN1,               0x4135);  /* Data Rate Estimator Bandwidth */
SFRX(AX5043_DRGAIN2,               0x4145);  /* Data Rate Estimator Bandwidth */
SFRX(AX5043_DRGAIN3,               0x4155);  /* Data Rate Estimator Bandwidth */
SFRX(AX5043_FOURFSK0,              0x412E);  /* Four FSK Control */
SFRX(AX5043_FOURFSK1,              0x413E);  /* Four FSK Control */
SFRX(AX5043_FOURFSK2,              0x414E);  /* Four FSK Control */
SFRX(AX5043_FOURFSK3,              0x415E);  /* Four FSK Control */
SFRX(AX5043_FREQDEV00,             0x412D);  /* Receiver Frequency Deviation 0 */
SFRX(AX5043_FREQDEV01,             0x413D);  /* Receiver Frequency Deviation 0 */
SFRX(AX5043_FREQDEV02,             0x414D);  /* Receiver Frequency Deviation 0 */
SFRX(AX5043_FREQDEV03,             0x415D);  /* Receiver Frequency Deviation 0 */
SFRX(AX5043_FREQDEV10,             0x412C);  /* Receiver Frequency Deviation 1 */
SFRX(AX5043_FREQDEV11,             0x413C);  /* Receiver Frequency Deviation 1 */
SFRX(AX5043_FREQDEV12,             0x414C);  /* Receiver Frequency Deviation 1 */
SFRX(AX5043_FREQDEV13,             0x415C);  /* Receiver Frequency Deviation 1 */
SFRX(AX5043_FREQUENCYGAINA0,       0x4127);  /* Frequency Estimator Bandwidth A */
SFRX(AX5043_FREQUENCYGAINA1,       0x4137);  /* Frequency Estimator Bandwidth A */
SFRX(AX5043_FREQUENCYGAINA2,       0x4147);  /* Frequency Estimator Bandwidth A */
SFRX(AX5043_FREQUENCYGAINA3,       0x4157);  /* Frequency Estimator Bandwidth A */
SFRX(AX5043_FREQUENCYGAINB0,       0x4128);  /* Frequency Estimator Bandwidth B */
SFRX(AX5043_FREQUENCYGAINB1,       0x4138);  /* Frequency Estimator Bandwidth B */
SFRX(AX5043_FREQUENCYGAINB2,       0x4148);  /* Frequency Estimator Bandwidth B */
SFRX(AX5043_FREQUENCYGAINB3,       0x4158);  /* Frequency Estimator Bandwidth B */
SFRX(AX5043_FREQUENCYGAINC0,       0x4129);  /* Frequency Estimator Bandwidth C */
SFRX(AX5043_FREQUENCYGAINC1,       0x4139);  /* Frequency Estimator Bandwidth C */
SFRX(AX5043_FREQUENCYGAINC2,       0x4149);  /* Frequency Estimator Bandwidth C */
SFRX(AX5043_FREQUENCYGAINC3,       0x4159);  /* Frequency Estimator Bandwidth C */
SFRX(AX5043_FREQUENCYGAIND0,       0x412A);  /* Frequency Estimator Bandwidth D */
SFRX(AX5043_FREQUENCYGAIND1,       0x413A);  /* Frequency Estimator Bandwidth D */
SFRX(AX5043_FREQUENCYGAIND2,       0x414A);  /* Frequency Estimator Bandwidth D */
SFRX(AX5043_FREQUENCYGAIND3,       0x415A);  /* Frequency Estimator Bandwidth D */
SFRX(AX5043_FREQUENCYLEAK,         0x4116);  /* Baseband Frequency Recovery Loop Leakiness */
SFRX(AX5043_PHASEGAIN0,            0x4126);  /* Phase Estimator Bandwidth */
SFRX(AX5043_PHASEGAIN1,            0x4136);  /* Phase Estimator Bandwidth */
SFRX(AX5043_PHASEGAIN2,            0x4146);  /* Phase Estimator Bandwidth */
SFRX(AX5043_PHASEGAIN3,            0x4156);  /* Phase Estimator Bandwidth */
SFRX(AX5043_PKTADDR0,              0x4207);  /* Packet Address 0 */
SFRX(AX5043_PKTADDR1,              0x4206);  /* Packet Address 1 */
SFRX(AX5043_PKTADDR2,              0x4205);  /* Packet Address 2 */
SFRX(AX5043_PKTADDR3,              0x4204);  /* Packet Address 3 */
SFRX(AX5043_PKTADDRCFG,            0x4200);  /* Packet Address Config */
SFRX(AX5043_PKTADDRMASK0,          0x420B);  /* Packet Address Mask 0 */
SFRX(AX5043_PKTADDRMASK1,          0x420A);  /* Packet Address Mask 1 */
SFRX(AX5043_PKTADDRMASK2,          0x4209);  /* Packet Address Mask 2 */
SFRX(AX5043_PKTADDRMASK3,          0x4208);  /* Packet Address Mask 3 */
SFRX(AX5043_PKTLENCFG,             0x4201);  /* Packet Length Configuration */
SFRX(AX5043_PKTLENOFFSET,          0x4202);  /* Packet Length Offset */
SFRX(AX5043_PKTMAXLEN,             0x4203);  /* Packet Maximum Length */
SFRX(AX5043_RXPARAMCURSET,         0x4118);  /* Receiver Parameter Current Set */
SFRX(AX5043_RXPARAMSETS,           0x4117);  /* Receiver Parameter Set Indirection */
SFRX(AX5043_TIMEGAIN0,             0x4124);  /* Time Estimator Bandwidth */
SFRX(AX5043_TIMEGAIN1,             0x4134);  /* Time Estimator Bandwidth */
SFRX(AX5043_TIMEGAIN2,             0x4144);  /* Time Estimator Bandwidth */
SFRX(AX5043_TIMEGAIN3,             0x4154);  /* Time Estimator Bandwidth */
#endif

/* Radio Registers, X Address Space, Non-Blocking Version */

SFRX(AX5043_AFSKCTRLNB,            0x5114);  /* AFSK Control, Non-Blocking */
SFRX(AX5043_AFSKMARK0NB,           0x5113);  /* AFSK Mark (1) Frequency 0, Non-Blocking */
SFRX(AX5043_AFSKMARK1NB,           0x5112);  /* AFSK Mark (1) Frequency 1, Non-Blocking */
SFRX(AX5043_AFSKSPACE0NB,          0x5111);  /* AFSK Space (0) Frequency 0, Non-Blocking */
SFRX(AX5043_AFSKSPACE1NB,          0x5110);  /* AFSK Space (0) Frequency 1, Non-Blocking */
SFRX(AX5043_AGCCOUNTERNB,          0x5043);  /* AGC Counter, Non-Blocking */
SFRX(AX5043_AMPLFILTERNB,          0x5115);  /* Amplitude Filter, Non-Blocking */
SFRX(AX5043_BBOFFSCAPNB,           0x5189);  /* Baseband Offset Compensation Capacitors, Non-Blocking */
SFRX(AX5043_BBTUNENB,              0x5188);  /* Baseband Tuning, Non-Blocking */
SFRX(AX5043_BGNDRSSINB,            0x5041);  /* Background RSSI, Non-Blocking */
SFRX(AX5043_BGNDRSSIGAINNB,        0x522E);  /* Background RSSI Averaging Time Constant, Non-Blocking */
SFRX(AX5043_BGNDRSSITHRNB,         0x522F);  /* Background RSSI Relative Threshold, Non-Blocking */
SFRX(AX5043_CRCINIT0NB,            0x5017);  /* CRC Initial Value 0, Non-Blocking */
SFRX(AX5043_CRCINIT1NB,            0x5016);  /* CRC Initial Value 1, Non-Blocking */
SFRX(AX5043_CRCINIT2NB,            0x5015);  /* CRC Initial Value 2, Non-Blocking */
SFRX(AX5043_CRCINIT3NB,            0x5014);  /* CRC Initial Value 3, Non-Blocking */
SFRX(AX5043_DACCONFIGNB,           0x5332);  /* DAC Configuration, Non-Blocking */
SFRX(AX5043_DACVALUE0NB,           0x5331);  /* DAC Value 0, Non-Blocking */
SFRX(AX5043_DACVALUE1NB,           0x5330);  /* DAC Value 1, Non-Blocking */
SFRX(AX5043_DECIMATIONNB,          0x5102);  /* Decimation Factor , Non-Blocking */
SFRX(AX5043_DIVERSITYNB,           0x5042);  /* Antenna Diversity Configuration, Non-Blocking */
SFRX(AX5043_DSPMODECFGNB,          0x5320);  /* DSP Mode Setting, Non-Blocking */
SFRX(AX5043_DSPMODESHREGNB,        0x506F);  /* DSPmode SPI Shift Register Access, Non-Blocking */
SFRX(AX5043_DSPMODESKIP0NB,        0x5322);  /* DSP Mode Skip 0, Non-Blocking */
SFRX(AX5043_DSPMODESKIP1NB,        0x5321);  /* DSP Mode Skip 1, Non-Blocking */
SFRX(AX5043_ENCODINGNB,            0x5011);  /* Encoding, Non-Blocking */
SFRX(AX5043_FECNB,                 0x5018);  /* Forward Error Correction, Non-Blocking */
SFRX(AX5043_FECSTATUSNB,           0x501A);  /* Forward Error Correction Status, Non-Blocking */
SFRX(AX5043_FECSYNCNB,             0x5019);  /* Forward Error Correction Sync Threshold, Non-Blocking */
SFRX(AX5043_FIFOCOUNT0NB,          0x502B);  /* Number of Words currently in FIFO 0, Non-Blocking */
SFRX(AX5043_FIFOCOUNT1NB,          0x502A);  /* Number of Words currently in FIFO 1, Non-Blocking */
SFRX(AX5043_FIFODATANB,            0x5029);  /* FIFO Data, Non-Blocking */
SFRX(AX5043_FIFOFREE0NB,           0x502D);  /* Number of Words that can be written to FIFO 0, Non-Blocking */
SFRX(AX5043_FIFOFREE1NB,           0x502C);  /* Number of Words that can be written to FIFO 1, Non-Blocking */
SFRX(AX5043_FIFOSTATNB,            0x5028);  /* FIFO Control, Non-Blocking */
SFRX(AX5043_FIFOTHRESH0NB,         0x502F);  /* FIFO Threshold 0, Non-Blocking */
SFRX(AX5043_FIFOTHRESH1NB,         0x502E);  /* FIFO Threshold 1, Non-Blocking */
SFRX(AX5043_FRAMINGNB,             0x5012);  /* Framing Mode, Non-Blocking */
SFRX(AX5043_FREQA0NB,              0x5037);  /* Frequency A 0, Non-Blocking */
SFRX(AX5043_FREQA1NB,              0x5036);  /* Frequency A 1, Non-Blocking */
SFRX(AX5043_FREQA2NB,              0x5035);  /* Frequency A 2, Non-Blocking */
SFRX(AX5043_FREQA3NB,              0x5034);  /* Frequency A 3, Non-Blocking */
SFRX(AX5043_FREQB0NB,              0x503F);  /* Frequency B 0, Non-Blocking */
SFRX(AX5043_FREQB1NB,              0x503E);  /* Frequency B 1, Non-Blocking */
SFRX(AX5043_FREQB2NB,              0x503D);  /* Frequency B 2, Non-Blocking */
SFRX(AX5043_FREQB3NB,              0x503C);  /* Frequency B 3, Non-Blocking */
SFRX(AX5043_FSKDEV0NB,             0x5163);  /* FSK Deviation 0, Non-Blocking */
SFRX(AX5043_FSKDEV1NB,             0x5162);  /* FSK Deviation 1, Non-Blocking */
SFRX(AX5043_FSKDEV2NB,             0x5161);  /* FSK Deviation 2, Non-Blocking */
SFRX(AX5043_FSKDMAX0NB,            0x510D);  /* Four FSK Rx Maximum Deviation 0, Non-Blocking */
SFRX(AX5043_FSKDMAX1NB,            0x510C);  /* Four FSK Rx Maximum Deviation 1, Non-Blocking */
SFRX(AX5043_FSKDMIN0NB,            0x510F);  /* Four FSK Rx Minimum Deviation 0, Non-Blocking */
SFRX(AX5043_FSKDMIN1NB,            0x510E);  /* Four FSK Rx Minimum Deviation 1, Non-Blocking */
SFRX(AX5043_GPADC13VALUE0NB,       0x5309);  /* GPADC13 Value 0, Non-Blocking */
SFRX(AX5043_GPADC13VALUE1NB,       0x5308);  /* GPADC13 Value 1, Non-Blocking */
SFRX(AX5043_GPADC1VALUE0NB,        0x530B);  /* GPADC1 Value 0, Non-Blocking */
SFRX(AX5043_GPADC1VALUE1NB,        0x530A);  /* GPADC1 Value 1, Non-Blocking */
SFRX(AX5043_GPADC2VALUE0NB,        0x530D);  /* GPADC2 Value 0, Non-Blocking */
SFRX(AX5043_GPADC2VALUE1NB,        0x530C);  /* GPADC2 Value 1, Non-Blocking */
SFRX(AX5043_GPADC3VALUE0NB,        0x530F);  /* GPADC3 Value 0, Non-Blocking */
SFRX(AX5043_GPADC3VALUE1NB,        0x530E);  /* GPADC3 Value 1, Non-Blocking */
SFRX(AX5043_GPADCCTRLNB,           0x5300);  /* General Purpose ADC Control, Non-Blocking */
SFRX(AX5043_GPADCPERIODNB,         0x5301);  /* GPADC Sampling Period, Non-Blocking */
SFRX(AX5043_IFFREQ0NB,             0x5101);  /* 2nd LO / IF Frequency 0, Non-Blocking */
SFRX(AX5043_IFFREQ1NB,             0x5100);  /* 2nd LO / IF Frequency 1, Non-Blocking */
SFRX(AX5043_IRQINVERSION0NB,       0x500B);  /* IRQ Inversion 0, Non-Blocking */
SFRX(AX5043_IRQINVERSION1NB,       0x500A);  /* IRQ Inversion 1, Non-Blocking */
SFRX(AX5043_IRQMASK0NB,            0x5007);  /* IRQ Mask 0, Non-Blocking */
SFRX(AX5043_IRQMASK1NB,            0x5006);  /* IRQ Mask 1, Non-Blocking */
SFRX(AX5043_IRQREQUEST0NB,         0x500D);  /* IRQ Request 0, Non-Blocking */
SFRX(AX5043_IRQREQUEST1NB,         0x500C);  /* IRQ Request 1, Non-Blocking */
SFRX(AX5043_LPOSCCONFIGNB,         0x5310);  /* Low Power Oscillator Calibration Configuration, Non-Blocking */
SFRX(AX5043_LPOSCFREQ0NB,          0x5317);  /* Low Power Oscillator Frequency Tuning Low Byte, Non-Blocking */
SFRX(AX5043_LPOSCFREQ1NB,          0x5316);  /* Low Power Oscillator Frequency Tuning High Byte, Non-Blocking */
SFRX(AX5043_LPOSCKFILT0NB,         0x5313);  /* Low Power Oscillator Calibration Filter Constant Low Byte, Non-Blocking */
SFRX(AX5043_LPOSCKFILT1NB,         0x5312);  /* Low Power Oscillator Calibration Filter Constant High Byte, Non-Blocking */
SFRX(AX5043_LPOSCPER0NB,           0x5319);  /* Low Power Oscillator Period Low Byte, Non-Blocking */
SFRX(AX5043_LPOSCPER1NB,           0x5318);  /* Low Power Oscillator Period High Byte, Non-Blocking */
SFRX(AX5043_LPOSCREF0NB,           0x5315);  /* Low Power Oscillator Reference Frequency Low Byte, Non-Blocking */
SFRX(AX5043_LPOSCREF1NB,           0x5314);  /* Low Power Oscillator Reference Frequency High Byte, Non-Blocking */
SFRX(AX5043_LPOSCSTATUSNB,         0x5311);  /* Low Power Oscillator Calibration Status, Non-Blocking */
SFRX(AX5043_MATCH0LENNB,           0x5214);  /* Pattern Match Unit 0, Pattern Length, Non-Blocking */
SFRX(AX5043_MATCH0MAXNB,           0x5216);  /* Pattern Match Unit 0, Maximum Match, Non-Blocking */
SFRX(AX5043_MATCH0MINNB,           0x5215);  /* Pattern Match Unit 0, Minimum Match, Non-Blocking */
SFRX(AX5043_MATCH0PAT0NB,          0x5213);  /* Pattern Match Unit 0, Pattern 0, Non-Blocking */
SFRX(AX5043_MATCH0PAT1NB,          0x5212);  /* Pattern Match Unit 0, Pattern 1, Non-Blocking */
SFRX(AX5043_MATCH0PAT2NB,          0x5211);  /* Pattern Match Unit 0, Pattern 2, Non-Blocking */
SFRX(AX5043_MATCH0PAT3NB,          0x5210);  /* Pattern Match Unit 0, Pattern 3, Non-Blocking */
SFRX(AX5043_MATCH1LENNB,           0x521C);  /* Pattern Match Unit 1, Pattern Length, Non-Blocking */
SFRX(AX5043_MATCH1MAXNB,           0x521E);  /* Pattern Match Unit 1, Maximum Match, Non-Blocking */
SFRX(AX5043_MATCH1MINNB,           0x521D);  /* Pattern Match Unit 1, Minimum Match, Non-Blocking */
SFRX(AX5043_MATCH1PAT0NB,          0x5219);  /* Pattern Match Unit 1, Pattern 0, Non-Blocking */
SFRX(AX5043_MATCH1PAT1NB,          0x5218);  /* Pattern Match Unit 1, Pattern 1, Non-Blocking */
SFRX(AX5043_MAXDROFFSET0NB,        0x5108);  /* Maximum Receiver Datarate Offset 0, Non-Blocking */
SFRX(AX5043_MAXDROFFSET1NB,        0x5107);  /* Maximum Receiver Datarate Offset 1, Non-Blocking */
SFRX(AX5043_MAXDROFFSET2NB,        0x5106);  /* Maximum Receiver Datarate Offset 2, Non-Blocking */
SFRX(AX5043_MAXRFOFFSET0NB,        0x510B);  /* Maximum Receiver RF Offset 0, Non-Blocking */
SFRX(AX5043_MAXRFOFFSET1NB,        0x510A);  /* Maximum Receiver RF Offset 1, Non-Blocking */
SFRX(AX5043_MAXRFOFFSET2NB,        0x5109);  /* Maximum Receiver RF Offset 2, Non-Blocking */
SFRX(AX5043_MODCFGANB,             0x5164);  /* Modulator Configuration A, Non-Blocking */
SFRX(AX5043_MODCFGFNB,             0x5160);  /* Modulator Configuration F, Non-Blocking */
SFRX(AX5043_MODULATIONNB,          0x5010);  /* Modulation, Non-Blocking */
SFRX(AX5043_PINFUNCANTSELNB,       0x5025);  /* Pin Function ANTSEL, Non-Blocking */
SFRX(AX5043_PINFUNCDATANB,         0x5023);  /* Pin Function DATA, Non-Blocking */
SFRX(AX5043_PINFUNCDCLKNB,         0x5022);  /* Pin Function DCLK, Non-Blocking */
SFRX(AX5043_PINFUNCIRQNB,          0x5024);  /* Pin Function IRQ, Non-Blocking */
SFRX(AX5043_PINFUNCPWRAMPNB,       0x5026);  /* Pin Function PWRAMP, Non-Blocking */
SFRX(AX5043_PINFUNCSYSCLKNB,       0x5021);  /* Pin Function SYSCLK, Non-Blocking */
SFRX(AX5043_PINSTATENB,            0x5020);  /* Pin State, Non-Blocking */
SFRX(AX5043_PKTACCEPTFLAGSNB,      0x5233);  /* Packet Controller Accept Flags, Non-Blocking */
SFRX(AX5043_PKTCHUNKSIZENB,        0x5230);  /* Packet Chunk Size, Non-Blocking */
SFRX(AX5043_PKTMISCFLAGSNB,        0x5231);  /* Packet Controller Miscellaneous Flags, Non-Blocking */
SFRX(AX5043_PKTSTOREFLAGSNB,       0x5232);  /* Packet Controller Store Flags, Non-Blocking */
SFRX(AX5043_PLLCPINB,              0x5031);  /* PLL Charge Pump Current, Non-Blocking */
SFRX(AX5043_PLLCPIBOOSTNB,         0x5039);  /* PLL Charge Pump Current (Boosted), Non-Blocking */
SFRX(AX5043_PLLLOCKDETNB,          0x5182);  /* PLL Lock Detect Delay, Non-Blocking */
SFRX(AX5043_PLLLOOPNB,             0x5030);  /* PLL Loop Filter Settings, Non-Blocking */
SFRX(AX5043_PLLLOOPBOOSTNB,        0x5038);  /* PLL Loop Filter Settings (Boosted), Non-Blocking */
SFRX(AX5043_PLLRANGINGANB,         0x5033);  /* PLL Autoranging A, Non-Blocking */
SFRX(AX5043_PLLRANGINGBNB,         0x503B);  /* PLL Autoranging B, Non-Blocking */
SFRX(AX5043_PLLRNGCLKNB,           0x5183);  /* PLL Autoranging Clock, Non-Blocking */
SFRX(AX5043_PLLVCODIVNB,           0x5032);  /* PLL Divider Settings, Non-Blocking */
SFRX(AX5043_PLLVCOINB,             0x5180);  /* PLL VCO Current, Non-Blocking */
SFRX(AX5043_PLLVCOIRNB,            0x5181);  /* PLL VCO Current Readback, Non-Blocking */
SFRX(AX5043_POWIRQMASKNB,          0x5005);  /* Power Management Interrupt Mask, Non-Blocking */
SFRX(AX5043_POWSTATNB,             0x5003);  /* Power Management Status, Non-Blocking */
SFRX(AX5043_POWSTICKYSTATNB,       0x5004);  /* Power Management Sticky Status, Non-Blocking */
SFRX(AX5043_PWRAMPNB,              0x5027);  /* PWRAMP Control, Non-Blocking */
SFRX(AX5043_PWRMODENB,             0x5002);  /* Power Mode, Non-Blocking */
SFRX(AX5043_RADIOEVENTMASK0NB,     0x5009);  /* Radio Event Mask 0, Non-Blocking */
SFRX(AX5043_RADIOEVENTMASK1NB,     0x5008);  /* Radio Event Mask 1, Non-Blocking */
SFRX(AX5043_RADIOEVENTREQ0NB,      0x500F);  /* Radio Event Request 0, Non-Blocking */
SFRX(AX5043_RADIOEVENTREQ1NB,      0x500E);  /* Radio Event Request 1, Non-Blocking */
SFRX(AX5043_RADIOSTATENB,          0x501C);  /* Radio Controller State, Non-Blocking */
SFRX(AX5043_RSSINB,                0x5040);  /* Received Signal Strength Indicator, Non-Blocking */
SFRX(AX5043_RSSIABSTHRNB,          0x522D);  /* RSSI Absolute Threshold, Non-Blocking */
SFRX(AX5043_RSSIREFERENCENB,       0x522C);  /* RSSI Offset, Non-Blocking */
SFRX(AX5043_RXDATARATE0NB,         0x5105);  /* Receiver Datarate 0, Non-Blocking */
SFRX(AX5043_RXDATARATE1NB,         0x5104);  /* Receiver Datarate 1, Non-Blocking */
SFRX(AX5043_RXDATARATE2NB,         0x5103);  /* Receiver Datarate 2, Non-Blocking */
SFRX(AX5043_SCRATCHNB,             0x5001);  /* Scratch, Non-Blocking */
SFRX(AX5043_SILICONREVISIONNB,     0x5000);  /* Silicon Revision, Non-Blocking */
SFRX(AX5043_TIMER0NB,              0x505B);  /* 1MHz Timer 0, Non-Blocking */
SFRX(AX5043_TIMER1NB,              0x505A);  /* 1MHz Timer 1, Non-Blocking */
SFRX(AX5043_TIMER2NB,              0x5059);  /* 1MHz Timer 2, Non-Blocking */
SFRX(AX5043_TMGRXAGCNB,            0x5227);  /* Receiver AGC Settling Time, Non-Blocking */
SFRX(AX5043_TMGRXBOOSTNB,          0x5223);  /* Receive PLL Boost Time, Non-Blocking */
SFRX(AX5043_TMGRXCOARSEAGCNB,      0x5226);  /* Receive Coarse AGC Time, Non-Blocking */
SFRX(AX5043_TMGRXOFFSACQNB,        0x5225);  /* Receive Baseband DC Offset Acquisition Time, Non-Blocking */
SFRX(AX5043_TMGRXPREAMBLE1NB,      0x5229);  /* Receiver Preamble 1 Timeout, Non-Blocking */
SFRX(AX5043_TMGRXPREAMBLE2NB,      0x522A);  /* Receiver Preamble 2 Timeout, Non-Blocking */
SFRX(AX5043_TMGRXPREAMBLE3NB,      0x522B);  /* Receiver Preamble 3 Timeout, Non-Blocking */
SFRX(AX5043_TMGRXRSSINB,           0x5228);  /* Receiver RSSI Settling Time, Non-Blocking */
SFRX(AX5043_TMGRXSETTLENB,         0x5224);  /* Receive PLL (post Boost) Settling Time, Non-Blocking */
SFRX(AX5043_TMGTXBOOSTNB,          0x5220);  /* Transmit PLL Boost Time, Non-Blocking */
SFRX(AX5043_TMGTXSETTLENB,         0x5221);  /* Transmit PLL (post Boost) Settling Time, Non-Blocking */
SFRX(AX5043_TRKAFSKDEMOD0NB,       0x5055);  /* AFSK Demodulator Tracking 0, Non-Blocking */
SFRX(AX5043_TRKAFSKDEMOD1NB,       0x5054);  /* AFSK Demodulator Tracking 1, Non-Blocking */
SFRX(AX5043_TRKAMPLITUDE0NB,       0x5049);  /* Amplitude Tracking 0, Non-Blocking */
SFRX(AX5043_TRKAMPLITUDE1NB,       0x5048);  /* Amplitude Tracking 1, Non-Blocking */
SFRX(AX5043_TRKDATARATE0NB,        0x5047);  /* Datarate Tracking 0, Non-Blocking */
SFRX(AX5043_TRKDATARATE1NB,        0x5046);  /* Datarate Tracking 1, Non-Blocking */
SFRX(AX5043_TRKDATARATE2NB,        0x5045);  /* Datarate Tracking 2, Non-Blocking */
SFRX(AX5043_TRKFREQ0NB,            0x5051);  /* Frequency Tracking 0, Non-Blocking */
SFRX(AX5043_TRKFREQ1NB,            0x5050);  /* Frequency Tracking 1, Non-Blocking */
SFRX(AX5043_TRKFSKDEMOD0NB,        0x5053);  /* FSK Demodulator Tracking 0, Non-Blocking */
SFRX(AX5043_TRKFSKDEMOD1NB,        0x5052);  /* FSK Demodulator Tracking 1, Non-Blocking */
SFRX(AX5043_TRKPHASE0NB,           0x504B);  /* Phase Tracking 0, Non-Blocking */
SFRX(AX5043_TRKPHASE1NB,           0x504A);  /* Phase Tracking 1, Non-Blocking */
SFRX(AX5043_TRKRFFREQ0NB,          0x504F);  /* RF Frequency Tracking 0, Non-Blocking */
SFRX(AX5043_TRKRFFREQ1NB,          0x504E);  /* RF Frequency Tracking 1, Non-Blocking */
SFRX(AX5043_TRKRFFREQ2NB,          0x504D);  /* RF Frequency Tracking 2, Non-Blocking */
SFRX(AX5043_TXPWRCOEFFA0NB,        0x5169);  /* Transmitter Predistortion Coefficient A 0, Non-Blocking */
SFRX(AX5043_TXPWRCOEFFA1NB,        0x5168);  /* Transmitter Predistortion Coefficient A 1, Non-Blocking */
SFRX(AX5043_TXPWRCOEFFB0NB,        0x516B);  /* Transmitter Predistortion Coefficient B 0, Non-Blocking */
SFRX(AX5043_TXPWRCOEFFB1NB,        0x516A);  /* Transmitter Predistortion Coefficient B 1, Non-Blocking */
SFRX(AX5043_TXPWRCOEFFC0NB,        0x516D);  /* Transmitter Predistortion Coefficient C 0, Non-Blocking */
SFRX(AX5043_TXPWRCOEFFC1NB,        0x516C);  /* Transmitter Predistortion Coefficient C 1, Non-Blocking */
SFRX(AX5043_TXPWRCOEFFD0NB,        0x516F);  /* Transmitter Predistortion Coefficient D 0, Non-Blocking */
SFRX(AX5043_TXPWRCOEFFD1NB,        0x516E);  /* Transmitter Predistortion Coefficient D 1, Non-Blocking */
SFRX(AX5043_TXPWRCOEFFE0NB,        0x5171);  /* Transmitter Predistortion Coefficient E 0, Non-Blocking */
SFRX(AX5043_TXPWRCOEFFE1NB,        0x5170);  /* Transmitter Predistortion Coefficient E 1, Non-Blocking */
SFRX(AX5043_TXRATE0NB,             0x5167);  /* Transmitter Bitrate 0, Non-Blocking */
SFRX(AX5043_TXRATE1NB,             0x5166);  /* Transmitter Bitrate 1, Non-Blocking */
SFRX(AX5043_TXRATE2NB,             0x5165);  /* Transmitter Bitrate 2, Non-Blocking */
SFRX(AX5043_WAKEUP0NB,             0x506B);  /* Wakeup Time 0, Non-Blocking */
SFRX(AX5043_WAKEUP1NB,             0x506A);  /* Wakeup Time 1, Non-Blocking */
SFRX(AX5043_WAKEUPFREQ0NB,         0x506D);  /* Wakeup Frequency 0, Non-Blocking */
SFRX(AX5043_WAKEUPFREQ1NB,         0x506C);  /* Wakeup Frequency 1, Non-Blocking */
SFRX(AX5043_WAKEUPTIMER0NB,        0x5069);  /* Wakeup Timer 0, Non-Blocking */
SFRX(AX5043_WAKEUPTIMER1NB,        0x5068);  /* Wakeup Timer 1, Non-Blocking */
SFRX(AX5043_WAKEUPXOEARLYNB,       0x506E);  /* Wakeup Crystal Oscillator Early, Non-Blocking */
SFRX(AX5043_XTALCAPNB,             0x5184);  /* Crystal Oscillator Load Capacitance, Non-Blocking */
SFRX(AX5043_XTALSTATUSNB,          0x501D);  /* Crystal Oscillator Status, Non-Blocking */

#if defined AX5043V1
SFRX(AX5043_AGCGAIN0NB,            0x5120);  /* AGC Speed, Non-Blocking */
SFRX(AX5043_AGCGAIN1NB,            0x512E);  /* AGC Speed, Non-Blocking */
SFRX(AX5043_AGCGAIN2NB,            0x513C);  /* AGC Speed, Non-Blocking */
SFRX(AX5043_AGCGAIN3NB,            0x514A);  /* AGC Speed, Non-Blocking */
SFRX(AX5043_AGCTARGET0NB,          0x5121);  /* AGC Target, Non-Blocking */
SFRX(AX5043_AGCTARGET1NB,          0x512F);  /* AGC Target, Non-Blocking */
SFRX(AX5043_AGCTARGET2NB,          0x513D);  /* AGC Target, Non-Blocking */
SFRX(AX5043_AGCTARGET3NB,          0x514B);  /* AGC Target, Non-Blocking */
SFRX(AX5043_AMPLITUDEGAIN0NB,      0x5129);  /* Amplitude Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_AMPLITUDEGAIN1NB,      0x5137);  /* Amplitude Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_AMPLITUDEGAIN2NB,      0x5145);  /* Amplitude Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_AMPLITUDEGAIN3NB,      0x5153);  /* Amplitude Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_BBOFFSRES0NB,          0x512D);  /* Baseband Offset Compensation Resistors, Non-Blocking */
SFRX(AX5043_BBOFFSRES1NB,          0x513B);  /* Baseband Offset Compensation Resistors, Non-Blocking */
SFRX(AX5043_BBOFFSRES2NB,          0x5149);  /* Baseband Offset Compensation Resistors, Non-Blocking */
SFRX(AX5043_BBOFFSRES3NB,          0x5157);  /* Baseband Offset Compensation Resistors, Non-Blocking */
SFRX(AX5043_DRGAIN0NB,             0x5123);  /* Data Rate Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_DRGAIN1NB,             0x5131);  /* Data Rate Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_DRGAIN2NB,             0x513F);  /* Data Rate Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_DRGAIN3NB,             0x514D);  /* Data Rate Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_FOURFSK0NB,            0x512C);  /* Four FSK Control, Non-Blocking */
SFRX(AX5043_FOURFSK1NB,            0x513A);  /* Four FSK Control, Non-Blocking */
SFRX(AX5043_FOURFSK2NB,            0x5148);  /* Four FSK Control, Non-Blocking */
SFRX(AX5043_FOURFSK3NB,            0x5156);  /* Four FSK Control, Non-Blocking */
SFRX(AX5043_FREQDEV00NB,           0x512B);  /* Receiver Frequency Deviation 0, Non-Blocking */
SFRX(AX5043_FREQDEV01NB,           0x5139);  /* Receiver Frequency Deviation 0, Non-Blocking */
SFRX(AX5043_FREQDEV02NB,           0x5147);  /* Receiver Frequency Deviation 0, Non-Blocking */
SFRX(AX5043_FREQDEV03NB,           0x5155);  /* Receiver Frequency Deviation 0, Non-Blocking */
SFRX(AX5043_FREQDEV10NB,           0x512A);  /* Receiver Frequency Deviation 1, Non-Blocking */
SFRX(AX5043_FREQDEV11NB,           0x5138);  /* Receiver Frequency Deviation 1, Non-Blocking */
SFRX(AX5043_FREQDEV12NB,           0x5146);  /* Receiver Frequency Deviation 1, Non-Blocking */
SFRX(AX5043_FREQDEV13NB,           0x5154);  /* Receiver Frequency Deviation 1, Non-Blocking */
SFRX(AX5043_FREQUENCYGAINA0NB,     0x5125);  /* Frequency Estimator Bandwidth A, Non-Blocking */
SFRX(AX5043_FREQUENCYGAINA1NB,     0x5133);  /* Frequency Estimator Bandwidth A, Non-Blocking */
SFRX(AX5043_FREQUENCYGAINA2NB,     0x5141);  /* Frequency Estimator Bandwidth A, Non-Blocking */
SFRX(AX5043_FREQUENCYGAINA3NB,     0x514F);  /* Frequency Estimator Bandwidth A, Non-Blocking */
SFRX(AX5043_FREQUENCYGAINB0NB,     0x5126);  /* Frequency Estimator Bandwidth B, Non-Blocking */
SFRX(AX5043_FREQUENCYGAINB1NB,     0x5134);  /* Frequency Estimator Bandwidth B, Non-Blocking */
SFRX(AX5043_FREQUENCYGAINB2NB,     0x5142);  /* Frequency Estimator Bandwidth B, Non-Blocking */
SFRX(AX5043_FREQUENCYGAINB3NB,     0x5150);  /* Frequency Estimator Bandwidth B, Non-Blocking */
SFRX(AX5043_FREQUENCYGAINC0NB,     0x5127);  /* Frequency Estimator Bandwidth C, Non-Blocking */
SFRX(AX5043_FREQUENCYGAINC1NB,     0x5135);  /* Frequency Estimator Bandwidth C, Non-Blocking */
SFRX(AX5043_FREQUENCYGAINC2NB,     0x5143);  /* Frequency Estimator Bandwidth C, Non-Blocking */
SFRX(AX5043_FREQUENCYGAINC3NB,     0x5151);  /* Frequency Estimator Bandwidth C, Non-Blocking */
SFRX(AX5043_FREQUENCYGAIND0NB,     0x5128);  /* Frequency Estimator Bandwidth D, Non-Blocking */
SFRX(AX5043_FREQUENCYGAIND1NB,     0x5136);  /* Frequency Estimator Bandwidth D, Non-Blocking */
SFRX(AX5043_FREQUENCYGAIND2NB,     0x5144);  /* Frequency Estimator Bandwidth D, Non-Blocking */
SFRX(AX5043_FREQUENCYGAIND3NB,     0x5152);  /* Frequency Estimator Bandwidth D, Non-Blocking */
SFRX(AX5043_PHASEGAIN0NB,          0x5124);  /* Phase Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_PHASEGAIN1NB,          0x5132);  /* Phase Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_PHASEGAIN2NB,          0x5140);  /* Phase Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_PHASEGAIN3NB,          0x514E);  /* Phase Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_PKTADDR0NB,            0x5203);  /* Packet Address 0, Non-Blocking */
SFRX(AX5043_PKTADDR1NB,            0x5202);  /* Packet Address 1, Non-Blocking */
SFRX(AX5043_PKTADDRCFGNB,          0x5201);  /* Packet Address Config, Non-Blocking */
SFRX(AX5043_PKTADDRMASK0NB,        0x5205);  /* Packet Address Mask 0, Non-Blocking */
SFRX(AX5043_PKTADDRMASK1NB,        0x5204);  /* Packet Address Mask 1, Non-Blocking */
SFRX(AX5043_PKTLENCFGNB,           0x5206);  /* Packet Length Configuration, Non-Blocking */
SFRX(AX5043_PKTLENOFFSETNB,        0x5207);  /* Packet Length Offset, Non-Blocking */
SFRX(AX5043_PKTMAXLENNB,           0x5208);  /* Packet Maximum Length, Non-Blocking */
SFRX(AX5043_RXPARAMCURSETNB,       0x5117);  /* Receiver Parameter Current Set, Non-Blocking */
SFRX(AX5043_RXPARAMSETSNB,         0x5116);  /* Receiver Parameter Set Indirection, Non-Blocking */
SFRX(AX5043_TIMEGAIN0NB,           0x5122);  /* Time Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_TIMEGAIN1NB,           0x5130);  /* Time Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_TIMEGAIN2NB,           0x513E);  /* Time Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_TIMEGAIN3NB,           0x514C);  /* Time Estimator Bandwidth, Non-Blocking */
#else
SFRX(AX5043_AGCAHYST0NB,           0x5122);  /* AGC Analog Hysteresis, Non-Blocking */
SFRX(AX5043_AGCAHYST1NB,           0x5132);  /* AGC Analog Hysteresis, Non-Blocking */
SFRX(AX5043_AGCAHYST2NB,           0x5142);  /* AGC Analog Hysteresis, Non-Blocking */
SFRX(AX5043_AGCAHYST3NB,           0x5152);  /* AGC Analog Hysteresis, Non-Blocking */
SFRX(AX5043_AGCGAIN0NB,            0x5120);  /* AGC Speed, Non-Blocking */
SFRX(AX5043_AGCGAIN1NB,            0x5130);  /* AGC Speed, Non-Blocking */
SFRX(AX5043_AGCGAIN2NB,            0x5140);  /* AGC Speed, Non-Blocking */
SFRX(AX5043_AGCGAIN3NB,            0x5150);  /* AGC Speed, Non-Blocking */
SFRX(AX5043_AGCMINMAX0NB,          0x5123);  /* AGC Analog Update Behaviour, Non-Blocking */
SFRX(AX5043_AGCMINMAX1NB,          0x5133);  /* AGC Analog Update Behaviour, Non-Blocking */
SFRX(AX5043_AGCMINMAX2NB,          0x5143);  /* AGC Analog Update Behaviour, Non-Blocking */
SFRX(AX5043_AGCMINMAX3NB,          0x5153);  /* AGC Analog Update Behaviour, Non-Blocking */
SFRX(AX5043_AGCTARGET0NB,          0x5121);  /* AGC Target, Non-Blocking */
SFRX(AX5043_AGCTARGET1NB,          0x5131);  /* AGC Target, Non-Blocking */
SFRX(AX5043_AGCTARGET2NB,          0x5141);  /* AGC Target, Non-Blocking */
SFRX(AX5043_AGCTARGET3NB,          0x5151);  /* AGC Target, Non-Blocking */
SFRX(AX5043_AMPLITUDEGAIN0NB,      0x512B);  /* Amplitude Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_AMPLITUDEGAIN1NB,      0x513B);  /* Amplitude Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_AMPLITUDEGAIN2NB,      0x514B);  /* Amplitude Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_AMPLITUDEGAIN3NB,      0x515B);  /* Amplitude Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_BBOFFSRES0NB,          0x512F);  /* Baseband Offset Compensation Resistors, Non-Blocking */
SFRX(AX5043_BBOFFSRES1NB,          0x513F);  /* Baseband Offset Compensation Resistors, Non-Blocking */
SFRX(AX5043_BBOFFSRES2NB,          0x514F);  /* Baseband Offset Compensation Resistors, Non-Blocking */
SFRX(AX5043_BBOFFSRES3NB,          0x515F);  /* Baseband Offset Compensation Resistors, Non-Blocking */
SFRX(AX5043_DRGAIN0NB,             0x5125);  /* Data Rate Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_DRGAIN1NB,             0x5135);  /* Data Rate Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_DRGAIN2NB,             0x5145);  /* Data Rate Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_DRGAIN3NB,             0x5155);  /* Data Rate Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_FOURFSK0NB,            0x512E);  /* Four FSK Control, Non-Blocking */
SFRX(AX5043_FOURFSK1NB,            0x513E);  /* Four FSK Control, Non-Blocking */
SFRX(AX5043_FOURFSK2NB,            0x514E);  /* Four FSK Control, Non-Blocking */
SFRX(AX5043_FOURFSK3NB,            0x515E);  /* Four FSK Control, Non-Blocking */
SFRX(AX5043_FREQDEV00NB,           0x512D);  /* Receiver Frequency Deviation 0, Non-Blocking */
SFRX(AX5043_FREQDEV01NB,           0x513D);  /* Receiver Frequency Deviation 0, Non-Blocking */
SFRX(AX5043_FREQDEV02NB,           0x514D);  /* Receiver Frequency Deviation 0, Non-Blocking */
SFRX(AX5043_FREQDEV03NB,           0x515D);  /* Receiver Frequency Deviation 0, Non-Blocking */
SFRX(AX5043_FREQDEV10NB,           0x512C);  /* Receiver Frequency Deviation 1, Non-Blocking */
SFRX(AX5043_FREQDEV11NB,           0x513C);  /* Receiver Frequency Deviation 1, Non-Blocking */
SFRX(AX5043_FREQDEV12NB,           0x514C);  /* Receiver Frequency Deviation 1, Non-Blocking */
SFRX(AX5043_FREQDEV13NB,           0x515C);  /* Receiver Frequency Deviation 1, Non-Blocking */
SFRX(AX5043_FREQUENCYGAINA0NB,     0x5127);  /* Frequency Estimator Bandwidth A, Non-Blocking */
SFRX(AX5043_FREQUENCYGAINA1NB,     0x5137);  /* Frequency Estimator Bandwidth A, Non-Blocking */
SFRX(AX5043_FREQUENCYGAINA2NB,     0x5147);  /* Frequency Estimator Bandwidth A, Non-Blocking */
SFRX(AX5043_FREQUENCYGAINA3NB,     0x5157);  /* Frequency Estimator Bandwidth A, Non-Blocking */
SFRX(AX5043_FREQUENCYGAINB0NB,     0x5128);  /* Frequency Estimator Bandwidth B, Non-Blocking */
SFRX(AX5043_FREQUENCYGAINB1NB,     0x5138);  /* Frequency Estimator Bandwidth B, Non-Blocking */
SFRX(AX5043_FREQUENCYGAINB2NB,     0x5148);  /* Frequency Estimator Bandwidth B, Non-Blocking */
SFRX(AX5043_FREQUENCYGAINB3NB,     0x5158);  /* Frequency Estimator Bandwidth B, Non-Blocking */
SFRX(AX5043_FREQUENCYGAINC0NB,     0x5129);  /* Frequency Estimator Bandwidth C, Non-Blocking */
SFRX(AX5043_FREQUENCYGAINC1NB,     0x5139);  /* Frequency Estimator Bandwidth C, Non-Blocking */
SFRX(AX5043_FREQUENCYGAINC2NB,     0x5149);  /* Frequency Estimator Bandwidth C, Non-Blocking */
SFRX(AX5043_FREQUENCYGAINC3NB,     0x5159);  /* Frequency Estimator Bandwidth C, Non-Blocking */
SFRX(AX5043_FREQUENCYGAIND0NB,     0x512A);  /* Frequency Estimator Bandwidth D, Non-Blocking */
SFRX(AX5043_FREQUENCYGAIND1NB,     0x513A);  /* Frequency Estimator Bandwidth D, Non-Blocking */
SFRX(AX5043_FREQUENCYGAIND2NB,     0x514A);  /* Frequency Estimator Bandwidth D, Non-Blocking */
SFRX(AX5043_FREQUENCYGAIND3NB,     0x515A);  /* Frequency Estimator Bandwidth D, Non-Blocking */
SFRX(AX5043_FREQUENCYLEAKNB,       0x5116);  /* Baseband Frequency Recovery Loop Leakiness, Non-Blocking */
SFRX(AX5043_PHASEGAIN0NB,          0x5126);  /* Phase Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_PHASEGAIN1NB,          0x5136);  /* Phase Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_PHASEGAIN2NB,          0x5146);  /* Phase Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_PHASEGAIN3NB,          0x5156);  /* Phase Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_PKTADDR0NB,            0x5207);  /* Packet Address 0, Non-Blocking */
SFRX(AX5043_PKTADDR1NB,            0x5206);  /* Packet Address 1, Non-Blocking */
SFRX(AX5043_PKTADDR2NB,            0x5205);  /* Packet Address 2, Non-Blocking */
SFRX(AX5043_PKTADDR3NB,            0x5204);  /* Packet Address 3, Non-Blocking */
SFRX(AX5043_PKTADDRCFGNB,          0x5200);  /* Packet Address Config, Non-Blocking */
SFRX(AX5043_PKTADDRMASK0NB,        0x520B);  /* Packet Address Mask 0, Non-Blocking */
SFRX(AX5043_PKTADDRMASK1NB,        0x520A);  /* Packet Address Mask 1, Non-Blocking */
SFRX(AX5043_PKTADDRMASK2NB,        0x5209);  /* Packet Address Mask 2, Non-Blocking */
SFRX(AX5043_PKTADDRMASK3NB,        0x5208);  /* Packet Address Mask 3, Non-Blocking */
SFRX(AX5043_PKTLENCFGNB,           0x5201);  /* Packet Length Configuration, Non-Blocking */
SFRX(AX5043_PKTLENOFFSETNB,        0x5202);  /* Packet Length Offset, Non-Blocking */
SFRX(AX5043_PKTMAXLENNB,           0x5203);  /* Packet Maximum Length, Non-Blocking */
SFRX(AX5043_RXPARAMCURSETNB,       0x5118);  /* Receiver Parameter Current Set, Non-Blocking */
SFRX(AX5043_RXPARAMSETSNB,         0x5117);  /* Receiver Parameter Set Indirection, Non-Blocking */
SFRX(AX5043_TIMEGAIN0NB,           0x5124);  /* Time Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_TIMEGAIN1NB,           0x5134);  /* Time Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_TIMEGAIN2NB,           0x5144);  /* Time Estimator Bandwidth, Non-Blocking */
SFRX(AX5043_TIMEGAIN3NB,           0x5154);  /* Time Estimator Bandwidth, Non-Blocking */
#endif

/* Interrupt Numbers */

#define INT_EXTERNAL0    0
#define INT_WAKEUPTIMER  1
#define INT_EXTERNAL1    2
#define INT_GPIO         3
#define INT_RADIO        4
#define INT_CLOCKMGMT    5
#define INT_POWERMGMT    6
#define INT_TIMER0       7
#define INT_TIMER1       8
#define INT_TIMER2       9
#define INT_SPI0         10
#define INT_UART0        11
#define INT_UART1        12
#define INT_GPADC        13
#define INT_DMA          14
#define INT_OUTPUTCOMP0  15
#define INT_OUTPUTCOMP1  16
#define INT_INPUTCAPT0   17
#define INT_INPUTCAPT1   18
#define INT_RNG          19
#define INT_AES          20
#define INT_DEBUGLINK    21

/* DMA Sources */

#define DMASOURCE_XRAMTOOTHER 0x00
#define DMASOURCE_SPITX       0x01
#define DMASOURCE_UART0TX     0x02
#define DMASOURCE_UART1TX     0x03
#define DMASOURCE_TIMER0      0x04
#define DMASOURCE_TIMER1      0x05
#define DMASOURCE_TIMER2      0x06
#define DMASOURCE_RADIOTX     0x07
#define DMASOURCE_OC0         0x08
#define DMASOURCE_OC1         0x09
#define DMASOURCE_OTHERTOXRAM 0x10
#define DMASOURCE_SPIRX       0x11
#define DMASOURCE_UART0RX     0x12
#define DMASOURCE_UART1RX     0x13
#define DMASOURCE_ADC         0x14
#define DMASOURCE_RADIORX     0x17
#define DMASOURCE_IC0         0x18
#define DMASOURCE_IC1         0x19

#endif /* AX8052F143_H */
