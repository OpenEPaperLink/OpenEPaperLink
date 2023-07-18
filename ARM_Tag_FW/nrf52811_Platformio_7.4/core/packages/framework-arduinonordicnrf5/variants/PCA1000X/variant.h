/*
  Copyright (c) 2014-2015 Arduino LLC.  All right reserved.
  Copyright (c) 2016 Sandeep Mistry All right reserved.
  Copyright (c) 2016 Frank Holtz. All right reserved.

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

#ifndef _VARIANT_PCA1000X_
#define _VARIANT_PCA1000X_

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
#ifdef PCA10000
  /* PCA10000
   * *********/

  #define PINS_COUNT           (7u)
  #define NUM_DIGITAL_PINS     (7u)
  #define NUM_ANALOG_INPUTS    (0u)
  #define NUM_ANALOG_OUTPUTS   (0u)

  // LEDs
  #define PIN_LED1                (4)
  #define PIN_LED2                (5)
  #define PIN_LED3                (6)
  #define LED_BUILTIN             PIN_LED1

  /*
   * Serial interfaces
   */
  #define PIN_SERIAL_RX       (3)
  #define PIN_SERIAL_CTS      (2)
  #define PIN_SERIAL_TX       (1)
  #define PIN_SERIAL_RTS      (0)
#elif defined(PCA10001)
  /* PCA10001
   * *********/

  #define PINS_COUNT           (32u)
  #define NUM_DIGITAL_PINS     (32u)
  #define NUM_ANALOG_INPUTS    (0u)
  #define NUM_ANALOG_OUTPUTS   (0u)

  // LEDs
  #define PIN_LED0                (18)
  #define PIN_LED1                (19)
  #define LED_BUILTIN             PIN_LED0

  // Buttons
  #define PIN_BUTTON0             (16)
  #define PIN_BUTTON1             (17)

  /*
   * Serial interfaces
   */
  #define PIN_SERIAL_RX       (11)
  #define PIN_SERIAL_CTS      (10)
  #define PIN_SERIAL_TX       (9)
  #define PIN_SERIAL_RTS      (8)
#else
  /* PCA1000[1,2,3,4] on nRF6310
   * ****************************/
  #define PINS_COUNT           (32u)
  #define NUM_DIGITAL_PINS     (32u)
  #define NUM_ANALOG_INPUTS    (6u)
  #define NUM_ANALOG_OUTPUTS   (0u)

  // LEDs
  #define PIN_LED1                (8)
  #define PIN_LED2                (9)
  #define PIN_LED3                (10)
  #define PIN_LED4                (11)
  #define PIN_LED5                (12)
  #define PIN_LED6                (13)
  #define PIN_LED7                (14)
  #define PIN_LED8                (15)
  #define LED_BUILTIN             PIN_LED1

  // Buttons
  #define PIN_BUTTON1             (0)
  #define PIN_BUTTON2             (1)
  #define PIN_BUTTON3             (2)
  #define PIN_BUTTON4             (3)
  #define PIN_BUTTON5             (4)
  #define PIN_BUTTON6             (5)
  #define PIN_BUTTON7             (6)
  #define PIN_BUTTON8             (7)

  /*
   * Analog pins
   */
  #define PIN_A0               (26)
  #define PIN_A1               (27)
  #define PIN_A2               (28)
  #define PIN_A3               (29)
  #define PIN_A4               (30)
  #define PIN_A5               (31)

  static const uint8_t A0  = PIN_A0 ;
  static const uint8_t A1  = PIN_A1 ;
  static const uint8_t A2  = PIN_A2 ;
  static const uint8_t A3  = PIN_A3 ;
  static const uint8_t A4  = PIN_A4 ;
  static const uint8_t A5  = PIN_A5 ;

  #define ADC_RESOLUTION    10

  /*
   * Serial interfaces
   */
  #define PIN_SERIAL_RX       (16)
  #define PIN_SERIAL_TX       (17)

  /*
   * SPI Interfaces
   */
  #define SPI_INTERFACES_COUNT 1

  #define PIN_SPI_MISO         (18)
  #define PIN_SPI_MOSI         (19)
  #define PIN_SPI_SCK          (20)

  static const uint8_t SS   = 25 ;
  static const uint8_t MOSI = PIN_SPI_MOSI ;
  static const uint8_t MISO = PIN_SPI_MISO ;
  static const uint8_t SCK  = PIN_SPI_SCK ;

  /*
   * Wire Interfaces
   */
  #define WIRE_INTERFACES_COUNT 1

  #define PIN_WIRE_SDA         (25u)
  #define PIN_WIRE_SCL         (24u)

  static const uint8_t SDA = PIN_WIRE_SDA;
  static const uint8_t SCL = PIN_WIRE_SCL;

#endif

#ifdef __cplusplus
}
#endif

#endif
