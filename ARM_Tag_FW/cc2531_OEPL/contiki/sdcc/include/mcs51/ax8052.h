/*-------------------------------------------------------------------------
   ax8052.h - Register Declarations for the Axsem Microfoot Processor Range

   Copyright (C) 2010, 2011 Axsem AG
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


#ifndef AX8052_H
#define AX8052_H

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


#endif /* AX8052_H */
