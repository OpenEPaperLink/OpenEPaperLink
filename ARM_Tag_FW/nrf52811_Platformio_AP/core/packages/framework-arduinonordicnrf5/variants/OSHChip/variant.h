/*
  Copyright (c) 2014-2015 Arduino LLC.  All right reserved.
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

#ifndef _VARIANT_OSHCHIP_
#define _VARIANT_OSHCHIP_

/** Master clock frequency */
#define VARIANT_MCK       (16000000ul)

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "WVariant.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

// Number of pins defined in PinDescription array
#define PINS_COUNT           (17u)
#define NUM_DIGITAL_PINS     (14u)
#define NUM_ANALOG_INPUTS    (6u)
#define NUM_ANALOG_OUTPUTS   (0u)

// LEDs
#define PIN_LED                 (PIN_LEDR)
#define PIN_LEDR                (14)
#define PIN_LEDG                (15)
#define PIN_LEDB                (16)
#define LED_BUILTIN             PIN_LED

/*
 * Analog pins
 */
#define PIN_A0               (10)
#define PIN_A1               (11)
#define PIN_A2               (12)
#define PIN_A3               (13)

static const uint8_t A0  = PIN_A0 ;
static const uint8_t A1  = PIN_A1 ;
static const uint8_t A2  = PIN_A2 ;
static const uint8_t A3  = PIN_A3 ;
#define ADC_RESOLUTION    10

/*
 * Serial interfaces
 */
// Serial
#define PIN_SERIAL_RX       (1)
#define PIN_SERIAL_TX       (0)

/*
 * SPI Interfaces
 */
#define SPI_INTERFACES_COUNT 0

/*
 * Wire Interfaces
 */
#define WIRE_INTERFACES_COUNT 0

#ifdef __cplusplus
}
#endif

#endif
