/*-------------------------------------------------------------------------
   adcbusy - check whether the AD module is busy

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


char adc_busy(void) __naked
{
#if (__SDCC_ADC_STYLE == 1865501)
  WDTCONbits.ADSHR = 0; /* access ADCON0/1 */
#endif
#if 0
  return (ADCON0bits.GO);
#else
#if (__SDCC_ADC_STYLE == 1802420)
  __asm
    movlw       0x00
    btfsc       _ADCON0bits, 2
    addlw       0x01
    return
  __endasm;
#elif (__SDCC_ADC_STYLE == 1812200) \
   || (__SDCC_ADC_STYLE == 1812300) \
   || (__SDCC_ADC_STYLE == 1813502) \
   || (__SDCC_ADC_STYLE == 1822200) \
   || (__SDCC_ADC_STYLE == 1823222) \
   || (__SDCC_ADC_STYLE == 1824501) \
   || (__SDCC_ADC_STYLE == 1865501)
  __asm
    movlw       0x00
    btfsc       _ADCON0bits, 1
    addlw       0x01
    return
  __endasm;
#else /* unsupported ADC style */
#error Unsupported ADC style.
#endif
#endif
}
