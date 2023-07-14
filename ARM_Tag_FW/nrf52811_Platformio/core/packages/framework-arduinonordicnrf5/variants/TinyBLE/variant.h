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

#ifndef _VARIANT_TINYBLE_
#define _VARIANT_TINYBLE_

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
#define PINS_COUNT           (32u)
//3 LEDS, 1 Button, 4 exposed pins
#define NUM_DIGITAL_PINS     (8u)
#define NUM_ANALOG_INPUTS    (4u)
#define NUM_ANALOG_OUTPUTS   (0u)

// LEDs
#define PIN_LED1                (21)
#define PIN_LED2                (22)
#define PIN_LED3                (23)

#define LED_BUILTIN             PIN_LED1

// Buttons
#define PIN_BUTTON              (17)

/*
 * Analog pins
 */
#define PIN_A0               (3)
#define PIN_A1               (4)
#define PIN_A2               (5)
#define PIN_A3               (6)

static const uint8_t A0  = PIN_A0 ;
static const uint8_t A1  = PIN_A1 ;
static const uint8_t A2  = PIN_A2 ;
static const uint8_t A3  = PIN_A3 ;
#define ADC_RESOLUTION    10

/*
 * SPI Interfaces
 */
#define SPI_INTERFACES_COUNT 1

#define PIN_SPI_MISO         (3)
#define PIN_SPI_MOSI         (4)
#define PIN_SPI_SCK          (5)

static const uint8_t SS   = 6 ;
static const uint8_t MOSI = PIN_SPI_MOSI ;
static const uint8_t MISO = PIN_SPI_MISO ;
static const uint8_t SCK  = PIN_SPI_SCK ;

/*
 * Wire Interfaces
 */
#define WIRE_INTERFACES_COUNT 1

//MPU6050_INT_PIN              (20)
#define PIN_WIRE_SDA         (12)
#define PIN_WIRE_SCL         (13)

static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;

/*
 * Serial interfaces
 */
// Serial
#define PIN_SERIAL_RX       (11)
#define PIN_SERIAL_TX       (9)

#ifdef __cplusplus
}
#endif

#endif
