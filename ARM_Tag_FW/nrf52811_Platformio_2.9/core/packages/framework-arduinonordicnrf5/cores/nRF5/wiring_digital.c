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

#include "nrf.h"

#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

void pinMode( uint32_t ulPin, uint32_t ulMode )
{
  if (ulPin >= PINS_COUNT) {
    return;
  }

  NRF_GPIO_Type* port = digitalPinToPort(ulPin);
  uint32_t pin = digitalPinToPin(ulPin);

  // Set pin mode according to chapter '22.6.3 I/O Pin Configuration'
  switch ( ulMode )
  {
    case INPUT:
      // Set pin to input mode
      port->PIN_CNF[pin] = ((uint32_t)GPIO_PIN_CNF_DIR_Input        << GPIO_PIN_CNF_DIR_Pos)
                        | ((uint32_t)GPIO_PIN_CNF_INPUT_Connect    << GPIO_PIN_CNF_INPUT_Pos)
                        | ((uint32_t)GPIO_PIN_CNF_PULL_Disabled    << GPIO_PIN_CNF_PULL_Pos)
                        | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0S1       << GPIO_PIN_CNF_DRIVE_Pos)
                        | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);
    break ;

    case INPUT_PULLUP:
      // Set pin to input mode with pull-up resistor enabled
      port->PIN_CNF[pin] = ((uint32_t)GPIO_PIN_CNF_DIR_Input        << GPIO_PIN_CNF_DIR_Pos)
                         | ((uint32_t)GPIO_PIN_CNF_INPUT_Connect    << GPIO_PIN_CNF_INPUT_Pos)
                         | ((uint32_t)GPIO_PIN_CNF_PULL_Pullup      << GPIO_PIN_CNF_PULL_Pos)
                         | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0S1       << GPIO_PIN_CNF_DRIVE_Pos)
                         | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);
    break ;

    case INPUT_PULLDOWN:
      // Set pin to input mode with pull-down resistor enabled
      port->PIN_CNF[pin] = ((uint32_t)GPIO_PIN_CNF_DIR_Input        << GPIO_PIN_CNF_DIR_Pos)
                         | ((uint32_t)GPIO_PIN_CNF_INPUT_Connect    << GPIO_PIN_CNF_INPUT_Pos)
                         | ((uint32_t)GPIO_PIN_CNF_PULL_Pulldown    << GPIO_PIN_CNF_PULL_Pos)
                         | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0S1       << GPIO_PIN_CNF_DRIVE_Pos)
                         | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);
    break ;

    case OUTPUT:
      // Set pin to output mode
      port->PIN_CNF[pin] = ((uint32_t)GPIO_PIN_CNF_DIR_Output       << GPIO_PIN_CNF_DIR_Pos)
                         | ((uint32_t)GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)
                         | ((uint32_t)GPIO_PIN_CNF_PULL_Disabled    << GPIO_PIN_CNF_PULL_Pos)
                         | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0S1       << GPIO_PIN_CNF_DRIVE_Pos)
                         | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);
    break ;

    default:
      // do nothing
    break ;
  }
}

void digitalWrite( uint32_t ulPin, uint32_t ulVal )
{
  if (ulPin >= PINS_COUNT) {
    return;
  }

  NRF_GPIO_Type* port = digitalPinToPort(ulPin);
  uint32_t mask = digitalPinToBitMask(ulPin);

  switch ( ulVal )
  {
    case LOW:
      port->OUTCLR = mask;
    break ;

    default:
      port->OUTSET = mask;
    break ;
  }

  return ;
}

int digitalRead( uint32_t ulPin )
{
  if (ulPin >= PINS_COUNT) {
    return 0;
  }

  NRF_GPIO_Type* port = digitalPinToPort(ulPin);
  uint32_t pin = digitalPinToPin(ulPin);

  return ((port->IN >> pin) & 1UL) ? HIGH : LOW ;
}

#ifdef __cplusplus
}
#endif
