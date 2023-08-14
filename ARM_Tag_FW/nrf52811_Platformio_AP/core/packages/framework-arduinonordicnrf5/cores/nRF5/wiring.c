/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.
  Copyright (c) 2016 Sandeep Mistry All right reserved.

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

#include <nrf.h>

#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

void init( void )
{
  NVIC_SetPriority(RTC1_IRQn, 15);
  NVIC_ClearPendingIRQ(RTC1_IRQn);
  NVIC_EnableIRQ(RTC1_IRQn);

  #if defined(USE_LFXO)
    NRF_CLOCK->LFCLKSRC = (uint32_t)((CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos) & CLOCK_LFCLKSRC_SRC_Msk);
  #elif defined(USE_LFSYNT)
    NRF_CLOCK->LFCLKSRC = (uint32_t)((CLOCK_LFCLKSRC_SRC_Synth << CLOCK_LFCLKSRC_SRC_Pos) & CLOCK_LFCLKSRC_SRC_Msk);
  #else //USE_LFRC
    NRF_CLOCK->LFCLKSRC = (uint32_t)((CLOCK_LFCLKSRC_SRC_RC << CLOCK_LFCLKSRC_SRC_Pos) & CLOCK_LFCLKSRC_SRC_Msk);
  #endif
  NRF_CLOCK->TASKS_LFCLKSTART = 1UL;

  NRF_RTC1->PRESCALER = 0;
  NRF_RTC1->INTENSET = RTC_INTENSET_OVRFLW_Msk;
  NRF_RTC1->EVTENSET = RTC_EVTEN_OVRFLW_Msk;
  NRF_RTC1->TASKS_START = 1;

  #if defined(RESET_PIN)
  if (((NRF_UICR->PSELRESET[0] & UICR_PSELRESET_CONNECT_Msk) != (UICR_PSELRESET_CONNECT_Connected << UICR_PSELRESET_CONNECT_Pos)) ||
      ((NRF_UICR->PSELRESET[1] & UICR_PSELRESET_CONNECT_Msk) != (UICR_PSELRESET_CONNECT_Connected << UICR_PSELRESET_CONNECT_Pos))){
      NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
      while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
      NRF_UICR->PSELRESET[0] = RESET_PIN;
      while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
      NRF_UICR->PSELRESET[1] = RESET_PIN;
      while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
      NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;
      while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
      NVIC_SystemReset();
  }
  #endif
}

#ifdef __cplusplus
}
#endif
