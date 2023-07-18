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

#ifndef _VARIANT_ELECTRONUT_HACKABLE_
#define _VARIANT_ELECTRONUT_HACKABLE_

/** Master clock frequency */
#define VARIANT_MCK       (64000000ul)

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "WVariant.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

// Number of pins defined in PinDescription array
#define PINS_COUNT           (27u)
#define NUM_DIGITAL_PINS     (27u)
#define NUM_ANALOG_INPUTS    (8u)
#define NUM_ANALOG_OUTPUTS   (0u)

// LEDs
#define PIN_LEDR               (24)
#define PIN_LEDG               (25)
#define PIN_LEDB               (26)
#define LED_BUILTIN            PIN_LEDB

/*
 * Analog pins
 */
#define PIN_A0               (13)
#define PIN_A1               (12)
#define PIN_A2               (11)
#define PIN_A3               (10)
#define PIN_A4               (20)
#define PIN_A5               (21)
#define PIN_A6               (22)
#define PIN_A7               (23)

static const uint8_t A0  = PIN_A0 ; // AIN0 / AREF / P0.02
static const uint8_t A1  = PIN_A1 ; // AIN1
static const uint8_t A2  = PIN_A2 ; // AIN2
static const uint8_t A3  = PIN_A3 ; // AIN3
static const uint8_t A4  = PIN_A4 ; // AIN4
static const uint8_t A5  = PIN_A5 ; // AIN5
static const uint8_t A6  = PIN_A6 ; // AIN6
static const uint8_t A7  = PIN_A7 ; // AIN7
#define ADC_RESOLUTION    10

// Other pins
#define PIN_AREF           (PIN_A0)
static const uint8_t AREF = PIN_AREF;

/*
 * Serial interfaces
 */
// Serial
#define PIN_SERIAL_RX       (0)
#define PIN_SERIAL_TX       (1)

/*
 * SPI Interfaces
 */
#define SPI_INTERFACES_COUNT 1

#define PIN_SPI_MISO         (4)
#define PIN_SPI_MOSI         (5)
#define PIN_SPI_SCK          (6)
#define PIN_SPI_SS           (3)
  
static const uint8_t SS   = PIN_SPI_SS ;
static const uint8_t MOSI = PIN_SPI_MOSI ;
static const uint8_t MISO = PIN_SPI_MISO ;
static const uint8_t SCK  = PIN_SPI_SCK ;

/*
 * Wire Interfaces
 */
#define WIRE_INTERFACES_COUNT 1

#define PIN_WIRE_SDA         (7)
#define PIN_WIRE_SCL         (8)

/*
 * Reset Button at P0.21
 */
#define RESET_PIN            21

#ifdef __cplusplus
}
#endif

/*----------------------------------------------------------------------------
 *        Arduino objects - C++ only
 *----------------------------------------------------------------------------*/

#endif
