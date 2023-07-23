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

#ifndef _VARIANT_CALLIOPE_MINI_
#define _VARIANT_CALLIOPE_MINI_

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

#define PINS_COUNT           (31)
#define NUM_DIGITAL_PINS     (26)
#define NUM_ANALOG_INPUTS    (6)
#define NUM_ANALOG_OUTPUTS   (0)

// LEDs

#define PIN_LED              (0) // no user LED; use pad 0
#define LED_BUILTIN          PIN_LED

// Buttons

#define PIN_BUTTON_A         (20)
#define PIN_BUTTON_B         (22)

// Analog pins

#define PIN_A0               (28) // microphone (note: pad 0 is not an analog input!)
#define PIN_A1               (1) // pad 1
#define PIN_A2               (2) // pad 2
#define PIN_A3               (4) // connector pin 4
#define PIN_A4               (5) // connector pin 5
#define PIN_A5               (6) // connector pin 6
#define PIN_A6               (16) // connector pin 16
#define PIN_A7               (17) // connector pin 17

static const uint8_t A0  = PIN_A0;
static const uint8_t A1  = PIN_A1;
static const uint8_t A2  = PIN_A2;
static const uint8_t A3  = PIN_A3;
static const uint8_t A4  = PIN_A4;
static const uint8_t A5  = PIN_A5;
static const uint8_t A6  = PIN_A6;
static const uint8_t A7  = PIN_A7;
#define ADC_RESOLUTION 10

// Serial interface

#define PIN_SERIAL_TX        (29)
#define PIN_SERIAL_RX        (30)

// SPI Interfaces

#define SPI_INTERFACES_COUNT 1

#define PIN_SPI_MISO         (16)
#define PIN_SPI_MOSI         (17)
#define PIN_SPI_SCK          (22)

static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK  = PIN_SPI_SCK;

// Wire Interfaces

#define WIRE_INTERFACES_COUNT 1

#define PIN_WIRE_SDA         (18)
#define PIN_WIRE_SCL         (19)

static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;

// I2C device IDs

#define ACCEL_ID             24
#define GYRO_ID              104
#define MAG_ID               24

#ifdef __cplusplus
}
#endif

#endif
