/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "nrf.h"

#include "delay.h"
#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

static volatile uint32_t overflows = 0;

uint32_t millis( void )
{
  uint64_t ticks = (uint64_t)((uint64_t)overflows << (uint64_t)24) | (uint64_t)(NRF_RTC1->COUNTER);

  return (ticks * 1000) / 32768;
}

uint32_t micros( void )
{
  uint64_t ticks = (uint64_t)((uint64_t)overflows << (uint64_t)24) | (uint64_t)(NRF_RTC1->COUNTER);

  return (ticks * 1000000) / 32768;
}

void delay( uint32_t ms )
{
  if ( ms == 0 )
  {
    return ;
  }

  uint32_t start = millis() ;

  do
  {
    yield() ;
  } while ( millis() - start < ms ) ;
}

void RTC1_IRQHandler(void)
{
  NRF_RTC1->EVENTS_OVRFLW = 0;

#if __CORTEX_M == 0x04
    volatile uint32_t dummy = NRF_RTC1->EVENTS_OVRFLW;
    (void)dummy;
#endif

  overflows = (overflows + 1) & 0xff;
}

#ifdef __cplusplus
}
#endif
