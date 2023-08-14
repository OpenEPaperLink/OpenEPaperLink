/*
  Copyright (c) 2014 Arduino LLC.  All right reserved.
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

#include "Arduino.h"
#include "wiring_private.h"

#if defined(NRF51_SERIES)

#ifdef __cplusplus
extern "C" {
#endif

#define PWM_COUNT 3
#define PIN_FREE 0xffffffff

struct PWMContext {
  uint32_t pin;
  uint32_t value;
  uint32_t channel;
  uint32_t mask;
  uint32_t event;
};

static struct PWMContext pwmContext[PWM_COUNT] = {
  { PIN_FREE, 0, 1, TIMER_INTENSET_COMPARE1_Msk, 1 },
  { PIN_FREE, 0, 2, TIMER_INTENSET_COMPARE2_Msk, 2 },
  { PIN_FREE, 0, 3, TIMER_INTENSET_COMPARE3_Msk, 3 }
};

static int timerEnabled = 0;

static uint32_t adcReference = ADC_CONFIG_REFSEL_SupplyOneThirdPrescaling;
static uint32_t adcPrescaling = ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling;

static uint32_t readResolution = 10;
static uint32_t writeResolution = 8;

void analogReadResolution( int res )
{
  readResolution = res;
}

void analogWriteResolution( int res )
{
  writeResolution = res;
}

static inline uint32_t mapResolution( uint32_t value, uint32_t from, uint32_t to )
{
  if ( from == to )
  {
    return value ;
  }

  if ( from > to )
  {
    return value >> (from-to) ;
  }
  else
  {
    return value << (to-from) ;
  }
}

/*
 * Internal VBG Reference is 1.2 V.
 * External References AREF0 and AREF1 should be between 0.83 V - 1.3 V.
 *
 * Warning : ADC should not be exposed to > 2.4 V, calculated after prescaling.
 *           GPIO pins must not be exposed to higher voltage than VDD + 0.3 V.
 */
void analogReference( eAnalogReference ulMode )
{
  switch ( ulMode ) {
    case AR_VBG:
      // 1.2 Reference, 1/3 prescaler = 0 V - 3.6 V range
      // Minimum VDD for full range in safe operation = 3.3V
      adcReference = ADC_CONFIG_REFSEL_VBG;
      adcPrescaling = ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling;
      break;

    case AR_SUPPLY_ONE_HALF:
      // 1/2 VDD Reference, 2/3 prescaler = 0 V - 0.75VDD range
      adcReference = ADC_CONFIG_REFSEL_SupplyOneHalfPrescaling;
      adcPrescaling = ADC_CONFIG_INPSEL_AnalogInputTwoThirdsPrescaling;
      break;

    case AR_EXT0:
      // ARF0 reference, 2/3 prescaler = 0 V - 1.5 ARF0
      adcReference = ADC_CONFIG_REFSEL_External | (ADC_CONFIG_EXTREFSEL_AnalogReference0 << ADC_CONFIG_EXTREFSEL_Pos);
      adcPrescaling = ADC_CONFIG_INPSEL_AnalogInputTwoThirdsPrescaling;
      break;

    case AR_EXT1:
      // ARF1 reference, 2/3 prescaler = 0 V - 1.5 ARF1
      adcReference = (ADC_CONFIG_REFSEL_External | ADC_CONFIG_EXTREFSEL_AnalogReference1 << ADC_CONFIG_EXTREFSEL_Pos);
      adcPrescaling = ADC_CONFIG_INPSEL_AnalogInputTwoThirdsPrescaling;
      break;

    case AR_SUPPLY_ONE_THIRD:
    case AR_DEFAULT:
    default:
      // 1/3 VDD Reference, 1/3 prescaler = 0 V - VDD range
      adcReference = ADC_CONFIG_REFSEL_SupplyOneThirdPrescaling;
      adcPrescaling = ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling;
      break;
  }
}

uint32_t analogRead( uint32_t ulPin )
{
  uint32_t pin = ADC_CONFIG_PSEL_Disabled;
  uint32_t adcResolution;
  uint32_t resolution;
  int16_t value;

  if (ulPin >= PINS_COUNT) {
    return 0;
  }

  ulPin = g_ADigitalPinMap[ulPin];

  switch ( ulPin ) {
    case 26:
      pin = ADC_CONFIG_PSEL_AnalogInput0;
      break;

    case 27:
      pin = ADC_CONFIG_PSEL_AnalogInput1;
      break;

    case 1:
      pin = ADC_CONFIG_PSEL_AnalogInput2;
      break;

    case 2:
      pin = ADC_CONFIG_PSEL_AnalogInput3;
      break;

    case 3:
      pin = ADC_CONFIG_PSEL_AnalogInput4;
      break;

    case 4:
      pin = ADC_CONFIG_PSEL_AnalogInput5;
      break;

    case 5:
      pin = ADC_CONFIG_PSEL_AnalogInput6;
      break;

    case 6:
      pin = ADC_CONFIG_PSEL_AnalogInput7;
      break;

    default:
      return 0;
  }

  if (readResolution <= 8) {
    resolution = 8;
    adcResolution = ADC_CONFIG_RES_8bit;
  } else if (readResolution <= 9) {
    resolution = 9;
    adcResolution = ADC_CONFIG_RES_9bit;
  } else {
    resolution = 10;
    adcResolution = ADC_CONFIG_RES_10bit;
  }

  NRF_ADC->ENABLE = 1;

  uint32_t config_reg = 0;

  config_reg |= ((uint32_t)adcResolution << ADC_CONFIG_RES_Pos) & ADC_CONFIG_RES_Msk;
  config_reg |= ((uint32_t)adcPrescaling << ADC_CONFIG_INPSEL_Pos) & ADC_CONFIG_INPSEL_Msk;
  config_reg |= ((uint32_t)adcReference << ADC_CONFIG_REFSEL_Pos) & ADC_CONFIG_REFSEL_Msk;

  if (adcReference & ADC_CONFIG_EXTREFSEL_Msk)
  {
      config_reg |= adcReference & ADC_CONFIG_EXTREFSEL_Msk;
  }

  NRF_ADC->CONFIG = ((uint32_t)pin << ADC_CONFIG_PSEL_Pos) | (NRF_ADC->CONFIG & ~ADC_CONFIG_PSEL_Msk);

  NRF_ADC->CONFIG = config_reg | (NRF_ADC->CONFIG & ADC_CONFIG_PSEL_Msk);

  NRF_ADC->TASKS_START = 1;

  while(!NRF_ADC->EVENTS_END);
  NRF_ADC->EVENTS_END = 0;

  value = (int32_t)NRF_ADC->RESULT;

  NRF_ADC->TASKS_STOP = 1;

  NRF_ADC->ENABLE = 0;

  return mapResolution(value, resolution, readResolution);
}

// Right now, PWM output only works on the pins with
// hardware support.  These are defined in the appropriate
// pins_*.c file.  For the rest of the pins, we default
// to digital output.
void analogWrite( uint32_t ulPin, uint32_t ulValue )
{
  if (ulPin >= PINS_COUNT) {
    return;
  }

  ulPin = g_ADigitalPinMap[ulPin];

  if (!timerEnabled) {
    NVIC_SetPriority(TIMER1_IRQn, 3);
    NVIC_ClearPendingIRQ(TIMER1_IRQn);
    NVIC_EnableIRQ(TIMER1_IRQn);

    NRF_TIMER1->MODE = (NRF_TIMER1->MODE & ~TIMER_MODE_MODE_Msk) | ((TIMER_MODE_MODE_Timer << TIMER_MODE_MODE_Pos) & TIMER_MODE_MODE_Msk);

    NRF_TIMER1->BITMODE = (NRF_TIMER1->BITMODE & ~TIMER_BITMODE_BITMODE_Msk) | ((TIMER_BITMODE_BITMODE_08Bit << TIMER_BITMODE_BITMODE_Pos) & TIMER_BITMODE_BITMODE_Msk);

    NRF_TIMER1->PRESCALER = (NRF_TIMER1->PRESCALER & ~TIMER_PRESCALER_PRESCALER_Msk) | ((7 << TIMER_PRESCALER_PRESCALER_Pos) & TIMER_PRESCALER_PRESCALER_Msk);

    NRF_TIMER1->CC[0] = 0;

    NRF_TIMER1->INTENSET = TIMER_INTENSET_COMPARE0_Msk;

    NRF_TIMER1->TASKS_START = 0x1UL;

    timerEnabled = true;
  }

  for (int i = 0; i < PWM_COUNT; i++) {
    if (pwmContext[i].pin == PIN_FREE || pwmContext[i].pin == ulPin) {
      pwmContext[i].pin = ulPin;

      NRF_GPIO->PIN_CNF[ulPin] = ((uint32_t)GPIO_PIN_CNF_DIR_Output       << GPIO_PIN_CNF_DIR_Pos)
                               | ((uint32_t)GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)
                               | ((uint32_t)GPIO_PIN_CNF_PULL_Disabled    << GPIO_PIN_CNF_PULL_Pos)
                               | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0S1       << GPIO_PIN_CNF_DRIVE_Pos)
                               | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);

      ulValue = mapResolution(ulValue, writeResolution, 8);

      pwmContext[i].value = ulValue;

      NRF_TIMER1->CC[pwmContext[i].channel] = ulValue;

      NRF_TIMER1->INTENSET = pwmContext[i].mask;

      break;
    }
  }
}

void TIMER1_IRQHandler(void)
{
  if (NRF_TIMER1->EVENTS_COMPARE[0]) {
    for (int i = 0; i < PWM_COUNT; i++) {
      if (pwmContext[i].pin != PIN_FREE && pwmContext[i].value != 0) {
        NRF_GPIO->OUTSET = (1UL << pwmContext[i].pin);
      }
    }

    NRF_TIMER1->EVENTS_COMPARE[0] = 0x0UL;
  }

  for (int i = 0; i < PWM_COUNT; i++) {
    if (NRF_TIMER1->EVENTS_COMPARE[pwmContext[i].event]) {
      if (pwmContext[i].pin != PIN_FREE && pwmContext[i].value != 255) {
        NRF_GPIO->OUTCLR = (1UL << pwmContext[i].pin);
      }

      NRF_TIMER1->EVENTS_COMPARE[pwmContext[i].event] = 0x0UL;
    }
  }
}

#ifdef __cplusplus
}
#endif

#endif
