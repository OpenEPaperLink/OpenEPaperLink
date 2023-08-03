/*-------------------------------------------------------------------------
   adc.c - A/D conversion module library header

   Copyright (C) 2004, Vangelis Rokas <vrokas AT otenet.gr>

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
/*
 * Devices implemented:
 *      PIC18F[24][45][28]
 *      PIC18F2455-style
 */

#ifndef __ADC_H__
#define __ADC_H__

/* link I/O libarary */
#pragma library io

/*
 * adc_open's `channel' argument:
 *
 * one of ADC_CHN_*
 */

/* channel selection (CHS field in ADCON0) */
#define ADC_CHN_0               0x00
#define ADC_CHN_1               0x01
#define ADC_CHN_2               0x02
#define ADC_CHN_3               0x03
#define ADC_CHN_4               0x04
#define ADC_CHN_5               0x05
#define ADC_CHN_6               0x06
#define ADC_CHN_7               0x07
#define ADC_CHN_8               0x08
#define ADC_CHN_9               0x09
#define ADC_CHN_10              0x0a
#define ADC_CHN_11              0x0b
#define ADC_CHN_12              0x0c
#define ADC_CHN_13              0x0d
#define ADC_CHN_14              0x0e
#define ADC_CHN_DAC             0x0e  /* 13k50-style */
#define ADC_CHN_15              0x0f
#define ADC_CHN_FVR             0x0f  /* 13k50-style */
/* more channels: 23k22-style */
#define ADC_CHN_16              0x10
#define ADC_CHN_17              0x11
#define ADC_CHN_18              0x12
#define ADC_CHN_19              0x13
#define ADC_CHN_20              0x14
#define ADC_CHN_21              0x15
#define ADC_CHN_22              0x16
#define ADC_CHN_23              0x17
#define ADC_CHN_24              0x18
#define ADC_CHN_25              0x19
#define ADC_CHN_26              0x1a
#define ADC_CHN_27              0x1b
#define ADC_CHN_K_CTMU          0x1d
#define ADC_CHN_K_DAC           0x1e
#define ADC_CHN_K_FVR           0x1f


/*
 * adc_open's `fosc' argument:
 *
 * ADC_FOSC_* | ADC_ACQT_* | ADC_CAL | ADC_TRIGSEL_*
 *
 *    7     6     5     4     3     2     1     0
 * +-----+-----+-----+-----+-----+-----+-----+-----+
 * | TRG | CAL |       ACQT      |    FOSC/ADCS    |
 * +-----+-----+-----+-----+-----+-----+-----+-----+
 */

/* oscillator frequency (ADCS field) */
#define ADC_FOSC_2              0x00
#define ADC_FOSC_4              0x04
#define ADC_FOSC_8              0x01
#define ADC_FOSC_16             0x05
#define ADC_FOSC_32             0x02
#define ADC_FOSC_64             0x06
#define ADC_FOSC_RC             0x07

/* acquisition time (13k50/2220/24j50/65j50-styles only) */
#define ADC_ACQT_0              (0x00 << 3)
#define ADC_ACQT_2              (0x01 << 3)
#define ADC_ACQT_4              (0x02 << 3)
#define ADC_ACQT_6              (0x03 << 3)
#define ADC_ACQT_8              (0x04 << 3)
#define ADC_ACQT_12             (0x05 << 3)
#define ADC_ACQT_16             (0x06 << 3)
#define ADC_ACQT_20             (0x07 << 3)

/* calibration enable (24j50/65j50-style only) */
#define ADC_CAL                 0x40

/* trigger selection (23k22-style only) */
#define ADC_TRIGGER             0x80


/*
 * adc_open's `pcfg' argment:
 *
 * ADC_CFG_* (see below, style-specific)
 */


/*
 * adc_open's `config' argument:
 *
 * ADC_FRM_* | ADC_INT_* | ADC_VCFG_* | ADC_NVCFG_* | ADC_PVCFG_*
 *
 *    7     6     5     4     3     2     1     0
 * +-----+-----+-----+-----+-----+-----+-----+-----+
 * | FRM | INT |   VCFG    |   PVCFG   |   NVCFG   |
 * +-----+-----+-----+-----+-----+-----+-----+-----+
 */

/* output format */
#define ADC_FRM_LJUST           0x00
#define ADC_FRM_RJUST           0x80

/* interrupt on/off flag */
#define ADC_INT_OFF             0x00
#define ADC_INT_ON              0x40

/* reference voltage configuration (not for 18f242-style ADC) */
#define ADC_VCFG_VDD_VSS        0x00
#define ADC_VCFG_AN3_VSS        0x10
#define ADC_VCFG_VDD_AN2        0x20
#define ADC_VCFG_AN3_AN2        0x30

/* reference voltage configuration (13k50-style) */
#define ADC_NVCFG_VSS           0x00
#define ADC_NVCFG_AN5           0x01

#define ADC_PVCFG_VDD           (0x00 << 2)
#define ADC_PVCFG_AN4           (0x01 << 2)
#define ADC_PVCFG_FVR           (0x02 << 2)

/* reference voltage configuration (23k22-style) */
#define ADC_NVCFG_AN2           0x01
#define ADC_PVCFG_AN3           (0x01 << 2)
#define ADC_TRIGSEL_CCP5        (0x00 << 7)
#define ADC_TRIGSEL_CTMU        (0x01 << 7)


/*
 * Distinguishing between ADC-styles:
 * - 18f24j50-style devices have separate ANCON0/ANCON1
 *   registers for A/D port pin configuration, whereas
 *   18f65j50-style devices multiplex ANCONx and ADCONx
 *
 * ADCON0:
 * bit  18f242  18f1220 18f1230 18f13k50  18f2220 18f24j50  18f65j50  18f23k22
 *  0   ADON    ADON    ADON    ADON      ADON    ADON      ADON      ADON
 *  1   -       GO      GO      GO        GO      GO        GO        GO
 *  2   GO      CHS0    CHS0    CHS0      CHS0    CHS0      CHS0      CHS0
 *  3   CHS0    CHS1    CHS1    CHS1      CHS1    CHS1      CHS1      CHS1
 *  4   CHS1    CHS2    -       CHS2      CHS2    CHS2      CHS2      CHS2
 *  5   CHS2    -       -       CHS3      CHS3    CHS3      CHS3      CHS3
 *  6   ADCS0   VCFG0   -       -         -       VCFG0     VCFG0     CHS4
 *  7   ADCS1   VCFG1   SEVTEN  -         (ADCAL) VCFG1     VCFG1     -
 *
 * ADCON1:
 *  bit 18f242  18f1220 18f1230 18f13k50  18f2220 18f24j50  18f65j50  18f23k22
 *   0  PCFG0   PCFG0   PCFG0   NVCFG0    PCFG0   ADCS0     ADCS0     NVCFG0
 *   1  PCFG1   PCFG1   PCFG1   NVCFG1    PCFG1   ADCS1     ADCS1     NVCFG1
 *   2  PCFG2   PCFG2   PCFG2   PVCFG0    PCFG2   ADCS2     ADCS2     PVCFG0
 *   3  PCFG3   PCFG3   PCFG3   PVCFG1    PCFG3   ACQT0     ACQT0     PVCFG1
 *   4  -       PCFG4   VCFG0   -         VCFG0   ACQT1     ACQT1     -
 *   5  -       PCFG5           -         VCFG1   ACQT2     ACQT2     -
 *   6  ADCS2   PCFG6           -         -       ADCAL     ADCAL     -
 *   7  ADFM    -               -         -       ADFM      ADFM      TRIGSEL
 *
 * ADCON2:
 *  bit 18f242  18f1220 18f1230 18f13k50  18f2220 18f24j50  18f65j50  18f23k22
 *   0                  ADCS0   ADCS0     ADCS0                       ADCS0
 *   1                  ADCS1   ADCS1     ADCS1                       ADCS1
 *   2                  ADCS2   ADCS2     ADCS2                       ADCS2
 *   3                  ACQT0   ACQT0     ADQT0                       ACQT0
 *   4                  ACQT1   ACQT1     ADQT1                       ACQT1
 *   5                  ACQT2   ACQT2     ADQT2                       ACQT2
 *   6                  -       -         -                           -
 *   7                  ADFM    ADFM      ADFM                        ADFM
 */
#include "pic18fam.h"



/* Port configuration (PCFG (and VCFG) field(s) in ADCON1) */
#if (__SDCC_ADC_STYLE == 0)

#warning The target device is not supported by the SDCC PIC16 ADC library.

#elif (__SDCC_ADC_STYLE == 1802420)

#define ADC_CFG_8A_0R   0x00
#define ADC_CFG_7A_1R   0x01
#define ADC_CFG_5A_0R   0x02
#define ADC_CFG_4A_1R   0x03
#define ADC_CFG_3A_0R   0x04
#define ADC_CFG_2A_1R   0x05
#define ADC_CFG_0A_0R   0x06
#define ADC_CFG_6A_2R   0x08
#define ADC_CFG_6A_0R   0x09
#define ADC_CFG_5A_1R   0x0a
#define ADC_CFG_4A_2R   0x0b
#define ADC_CFG_3A_2R   0x0c
#define ADC_CFG_2A_2R   0x0d
#define ADC_CFG_1A_0R   0x0e
#define ADC_CFG_1A_2R   0x0f

#elif (__SDCC_ADC_STYLE == 1812200)

/*
 * These devices use a bitmask in ADCON1 to configure AN0..AN6
 * as digital ports (bit set) or analog input (bit clear).
 *
 * These settings are selected based on their similarity with
 * the 2220-style settings; 1220-style is more flexible, though.
 *
 * Reference voltages are configured via adc_open's config parameter
 * using ADC_VCFG_*.
 */

#define ADC_CFG_6A      0x00
#define ADC_CFG_5A      0x20
#define ADC_CFG_4A      0x30
#define ADC_CFG_3A      0x38
#define ADC_CFG_2A      0x3c
#define ADC_CFG_1A      0x3e
#define ADC_CFG_0A      0x3f

#elif (__SDCC_ADC_STYLE == 1812300)

/*
 * These devices use a bitmask in ADCON1 to configure AN0..AN3
 * as digital ports (bit set) or analog input (bit clear).
 *
 * These settings are selected based on their similarity with
 * the 2220-style settings; 1230-style is more flexible, though.
 *
 * Reference voltages are configured via adc_open's config parameter
 * using ADC_VCFG_*.
 */

#define ADC_CFG_4A      0x00
#define ADC_CFG_3A      0x08
#define ADC_CFG_2A      0x0c
#define ADC_CFG_1A      0x0e
#define ADC_CFG_0A      0x0f

#define ADC_VCFG_AVDD   0x00    /* AVdd */
#define ADC_VCFG_VREF   0x10    /* Vref+ */

#elif (__SDCC_ADC_STYLE == 1813502)

/*
 * These devices use a bitmask in ANSEL/H to configure
 * AN7..0/AN15..8 as digital ports (bit clear) or analog
 * inputs (bit set).
 *
 * These settings are selected based on their similarity with
 * the 2220-style settings; 13k50-style is more flexible, though.
 *
 * Reference voltages are configured via adc_open's config parameter
 * using ADC_PVCFG_* and ADC_NVCFG_*.
 */

#define ADC_CFG_16A     0xFFFF
#define ADC_CFG_15A     0x7FFF
#define ADC_CFG_14A     0x3FFF
#define ADC_CFG_13A     0x1FFF
#define ADC_CFG_12A     0x0FFF
#define ADC_CFG_11A     0x07FF
#define ADC_CFG_10A     0x03FF
#define ADC_CFG_9A      0x01FF
#define ADC_CFG_8A      0x00FF
#define ADC_CFG_7A      0x007F
#define ADC_CFG_6A      0x003F
#define ADC_CFG_5A      0x001F
#define ADC_CFG_4A      0x000F
#define ADC_CFG_3A      0x0007
#define ADC_CFG_2A      0x0003
#define ADC_CFG_1A      0x0001
#define ADC_CFG_0A      0x0000

#elif (__SDCC_ADC_STYLE == 1822200)

/*
 * The reference voltage configuration should be factored out into
 * the config argument (ADC_VCFG_*) to adc_open to facilitate a
 * merger with the 1220-style ADC.
 */

#define ADC_CFG_16A     0x00
/* 15 analog ports cannot be configured! */
#define ADC_CFG_14A     0x01
#define ADC_CFG_13A     0x02
#define ADC_CFG_12A     0x03
#define ADC_CFG_11A     0x04
#define ADC_CFG_10A     0x05
#define ADC_CFG_9A      0x06
#define ADC_CFG_8A      0x07
#define ADC_CFG_7A      0x08
#define ADC_CFG_6A      0x09
#define ADC_CFG_5A      0x0a
#define ADC_CFG_4A      0x0b
#define ADC_CFG_3A      0x0c
#define ADC_CFG_2A      0x0d
#define ADC_CFG_1A      0x0e
#define ADC_CFG_0A      0x0f

/*
 * For compatibility only: Combined port and reference voltage selection.
 * Consider using ADC_CFG_nA and a separate ADC_VCFG_* instead!
 */

#define ADC_CFG_16A_0R  0x00
#define ADC_CFG_16A_1R  0x10
#define ADC_CFG_16A_2R  0x30

/* Can only select 14 or 16 analog ports ... */
#define ADC_CFG_15A_0R  0x00
#define ADC_CFG_15A_1R  0x10
#define ADC_CFG_15A_2R  0x30

#define ADC_CFG_14A_0R  0x01
#define ADC_CFG_14A_1R  0x11
#define ADC_CFG_14A_2R  0x31
#define ADC_CFG_13A_0R  0x02
#define ADC_CFG_13A_1R  0x12
#define ADC_CFG_13A_2R  0x32
#define ADC_CFG_12A_0R  0x03
#define ADC_CFG_12A_1R  0x13
#define ADC_CFG_12A_2R  0x33
#define ADC_CFG_11A_0R  0x04
#define ADC_CFG_11A_1R  0x14
#define ADC_CFG_11A_2R  0x34
#define ADC_CFG_10A_0R  0x05
#define ADC_CFG_10A_1R  0x15
#define ADC_CFG_10A_2R  0x35
#define ADC_CFG_09A_0R  0x06
#define ADC_CFG_09A_1R  0x16
#define ADC_CFG_09A_2R  0x36
#define ADC_CFG_08A_0R  0x07
#define ADC_CFG_08A_1R  0x17
#define ADC_CFG_08A_2R  0x37
#define ADC_CFG_07A_0R  0x08
#define ADC_CFG_07A_1R  0x18
#define ADC_CFG_07A_2R  0x38
#define ADC_CFG_06A_0R  0x09
#define ADC_CFG_06A_1R  0x19
#define ADC_CFG_06A_2R  0x39
#define ADC_CFG_05A_0R  0x0a
#define ADC_CFG_05A_1R  0x1a
#define ADC_CFG_05A_2R  0x3a
#define ADC_CFG_04A_0R  0x0b
#define ADC_CFG_04A_1R  0x1b
#define ADC_CFG_04A_2R  0x3b
#define ADC_CFG_03A_0R  0x0c
#define ADC_CFG_03A_1R  0x1c
#define ADC_CFG_03A_2R  0x3c
#define ADC_CFG_02A_0R  0x0d
#define ADC_CFG_02A_1R  0x1d
#define ADC_CFG_02A_2R  0x3d
#define ADC_CFG_01A_0R  0x0e
#define ADC_CFG_01A_1R  0x1e
#define ADC_CFG_01A_2R  0x3e
#define ADC_CFG_00A_0R  0x0f

#elif (__SDCC_ADC_STYLE == 1823222)

/* use ANSELA, ANSELB, ANSELC, ANSELD, ANSELE registers and
 * TRISA, TRISB, TRISC, TRISD, TRISE registers to set
 * corresponding port to analog mode
 * Note: 46k22 supports up to 28 ADC ports */


#elif (__SDCC_ADC_STYLE == 1824501) || (__SDCC_ADC_STYLE == 1865501)

/*
 * These devices use a bitmask in ANCON0/1 to configure
 * AN7..0/AN15..8 as digital ports (bit set) or analog
 * inputs (bit clear).
 *
 * These settings are selected based on their similarity with
 * the 2220-style settings; 24j50/65j50-style is more flexible, though.
 *
 * Reference voltages are configured via adc_open's config parameter
 * using ADC_VCFG_*.
 */

#define ADC_CFG_16A     0x0000
#define ADC_CFG_15A     0x8000
#define ADC_CFG_14A     0xC000
#define ADC_CFG_13A     0xE000
#define ADC_CFG_12A     0xF000
#define ADC_CFG_11A     0xF800
#define ADC_CFG_10A     0xFC00
#define ADC_CFG_9A      0xFE00
#define ADC_CFG_8A      0xFF00
#define ADC_CFG_7A      0xFF80
#define ADC_CFG_6A      0xFFC0
#define ADC_CFG_5A      0xFFE0
#define ADC_CFG_4A      0xFFF0
#define ADC_CFG_3A      0xFFF8
#define ADC_CFG_2A      0xFFFC
#define ADC_CFG_1A      0xFFFE
#define ADC_CFG_0A      0xFFFF

#else   /* unhandled ADC style */

#error No supported ADC style selected.

#endif  /* __SDCC_ADC_STYLE */



#if    (__SDCC_ADC_STYLE == 1813502) \
    || (__SDCC_ADC_STYLE == 1824501) \
    || (__SDCC_ADC_STYLE == 1865501)
typedef unsigned int sdcc_pcfg_t;
#else /* other styles */
typedef unsigned char sdcc_pcfg_t;
#endif

/* initialize AD module */
void adc_open (unsigned char channel, unsigned char fosc, sdcc_pcfg_t pcfg, unsigned char config);

/* shutdown AD module */
void adc_close (void);

/* begin a conversion */
void adc_conv (void);

/* return 1 if AD is performing a conversion, 0 if done */
char adc_busy (void) __naked;

/* get value of conversion */
int adc_read (void) __naked;

/* setup conversion channel */
void adc_setchannel (unsigned char channel);

#endif

