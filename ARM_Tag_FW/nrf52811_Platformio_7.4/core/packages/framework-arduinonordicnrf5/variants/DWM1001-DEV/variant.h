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

#ifndef _VARIANT_DWM1001_DEV_
#define _VARIANT_DWM1001_DEV_

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
#define NUM_DIGITAL_PINS     (11u)
#define NUM_ANALOG_INPUTS    (2u)
#define NUM_ANALOG_OUTPUTS   (0u)

// LEDs
#define PIN_LEDRT            (22)
#define PIN_LEDRB            (23)
#define PIN_LEDG             (24)
#define PIN_LEDB             (25)
#define LED_BUILTIN          PIN_LEDRT
static const uint8_t LED_RED_TOP = PIN_LEDRT ;
static const uint8_t LED_RED_BOT = PIN_LEDRB ;
static const uint8_t LED_GREEN   = PIN_LEDG ;
static const uint8_t LED_BLUE    = PIN_LEDB ;

// Button
#define PIN_BUTTON           (26)

static const uint8_t BTN = PIN_BUTTON ;

/*
 * Analog pins
 */
#define PIN_A0               (9)
#define PIN_A1               (10)

static const uint8_t A0  = PIN_A0 ; // AIN2
static const uint8_t A1  = PIN_A1 ; // AIN1
#define ADC_RESOLUTION    14

/*
 * Serial interfaces
 */
// Serial
#define PIN_SERIAL_RX       (13)
#define PIN_SERIAL_TX       (14)

/*
 * SPI Interfaces
 */
#define SPI_INTERFACES_COUNT 2

#define PIN_SPI_MISO         (8)
#define PIN_SPI_MOSI         (7)
#define PIN_SPI_SCK          (PIN_A0)

static const uint8_t SS   = PIN_A1 ;
static const uint8_t MOSI = PIN_SPI_MOSI ;
static const uint8_t MISO = PIN_SPI_MISO ;
static const uint8_t SCK  = PIN_SPI_SCK ;

#define PIN_SPI1_SS          (15)
#define PIN_SPI1_MISO        (16)
#define PIN_SPI1_MOSI        (17)
#define PIN_SPI1_SCK         (18)

static const uint8_t SS1   = PIN_SPI1_SS ;
static const uint8_t MOSI1 = PIN_SPI1_MOSI ;
static const uint8_t MISO1 = PIN_SPI1_MISO ;
static const uint8_t SCK1  = PIN_SPI1_SCK ;

/*
 * Wire Interfaces
 */
#define WIRE_INTERFACES_COUNT 1

#define PIN_WIRE_SDA         (11)
#define PIN_WIRE_SCL         (12)

static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;

/*
 * Reset Button at P0.21
 */
#define RESET_PIN            21

/*
 * Other DWM1001 internal IOs
 */
#define PIN_DW_RST           (19)
#define PIN_DW_IRQ           (20)

static const uint8_t DW_RST = PIN_DW_RST;
static const uint8_t DW_IRQ = PIN_DW_IRQ;

#define PIN_ACC_IRQ          (21)

static const uint8_t ACC_IRQ = PIN_ACC_IRQ;

#ifdef __cplusplus
}
#endif

/*----------------------------------------------------------------------------
 *        Arduino objects - C++ only
 *----------------------------------------------------------------------------*/

#endif
