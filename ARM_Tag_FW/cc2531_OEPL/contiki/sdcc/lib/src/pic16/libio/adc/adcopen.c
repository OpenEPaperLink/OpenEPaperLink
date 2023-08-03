/*-------------------------------------------------------------------------
   adcopen - initialize AD module

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

#include <pic18fregs.h>
#include <adc.h>


/*
 * parameters are:
 *   channel: one of ADC_CHN_*
 *   fosc:    one of ADC_FOSC_* | ADC_ACQT_* | ADC_CAL
 *   pcfg:    one of ADC_CFG_* (a bitmask with set bits denoting digital ports for many styles)
 *   config:  ADC_FRM_* | ADC_INT_* | ADC_VCFG_* | ADC_NVCFG_* | ADC_PVCFG_*
 */

void
adc_open(unsigned char channel, unsigned char fosc, sdcc_pcfg_t pcfg, unsigned char config)
{
  /* disable ADC */
#if (__SDCC_ADC_STYLE == 1865501)
  WDTCONbits.ADSHR = 0; /* access ADCON0/1 */
#endif
  ADCON0 = 0;

#if (__SDCC_ADC_STYLE == 1802420)
  ADCON0 = ((channel & 0x07) << 3) | ((fosc & 0x03) << 6);
  ADCON1 = (pcfg & 0x0f) | (config & ADC_FRM_RJUST);
  if (fosc & 0x04) {
    ADCON1bits.ADCS2 = 1;
  }
#elif (__SDCC_ADC_STYLE == 1812200)
  ADCON0 = ((channel & 0x07) | (config & ADC_VCFG_AN3_AN2)) << 2;
  ADCON1 = (pcfg & 0x7f);
  ADCON2 = (ADCON2 & 0x38) | (fosc & 0x07) | (config & ADC_FRM_RJUST);
#elif (__SDCC_ADC_STYLE == 1812300)
  ADCON0 = ((channel & 0x03) << 2);
  ADCON1 = (pcfg & 0x0f) | (config & ADC_VCFG_VREF);
  ADCON2 = (fosc & 0x7f) | (config & ADC_FRM_RJUST);
#elif (__SDCC_ADC_STYLE == 1813502)
  ANSEL = pcfg;
  ANSELH = (pcfg >> 8);
  ADCON0 = ((channel & 0x0f) << 2);
  ADCON1 = (config & 0x0f);
  ADCON2 = (config & ADC_FRM_RJUST) | (fosc & 0x3f);
#elif (__SDCC_ADC_STYLE == 1823222)
  /* use ANSELA, ANSELB, ANSELC, ANSELD, ANSELE registers and
     TRISA, TRISB, TRISC, TRISD, TRISE registers to set
     corresponding port to analog mode */
  /* 46k22 supports up to 28 ADC ports */
  ADCON0 = ((channel & 0x1f) << 2);
  /* TRIGSEL — — — PVCFG<1:0> NVCFG<1:0> */
  ADCON1 = (fosc & ADC_TRIGGER) | (config & 0x0f);
  /* ADFM — ACQT<2:0> ADCS<2:0> */
  ADCON2 = (config & ADC_FRM_RJUST) | (fosc & 0x3f);
  (void)pcfg; /* quieten compiler warning */
#elif (__SDCC_ADC_STYLE == 1822200)
  ADCON0 = (channel & 0x0f) << 2;
  /* XXX: Should be (pcfg & 0x0f) as VCFG comes from config,
   * but we retain compatibility for now ... */
  ADCON1 = (pcfg & 0x3f) | (config & ADC_VCFG_AN3_AN2);
  ADCON2 = (config & ADC_FRM_RJUST) | (fosc & 0x3f);
#elif (__SDCC_ADC_STYLE == 1824501)
  ANCON0 = pcfg;
  ANCON1 = (pcfg >> 8);
  ADCON0 = ((channel & 0x0f) << 2) | ((config & ADC_VCFG_AN3_AN2) << 2);
  ADCON1 = (config & ADC_FRM_RJUST) | (fosc & 0x7f);
#elif (__SDCC_ADC_STYLE == 1865501)
  WDTCONbits.ADSHR = 1; /* access ANCON0/1 */
  ANCON0 = pcfg;
  ANCON1 = (pcfg >> 8);
  WDTCONbits.ADSHR = 0; /* access ADCON0/1 */
  ADCON0 = ((channel & 0x0f) << 2) | ((config & ADC_VCFG_AN3_AN2) << 2);
  ADCON1 = (config & ADC_FRM_RJUST) | (fosc & 0x7f);
#else /* unsupported ADC style */
#error Unsupported ADC style.
#endif

  if (config & ADC_INT_ON) {
    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 1;
    INTCONbits.PEIE = 1;
  }

  /* enable the A/D module */
  ADCON0bits.ADON = 1;
}
