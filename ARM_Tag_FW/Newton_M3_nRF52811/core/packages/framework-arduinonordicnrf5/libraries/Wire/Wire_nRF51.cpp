/*
 * TWI/I2C library for nRF5x
 * Copyright (c) 2015 Arduino LLC. All rights reserved.
 * Copyright (c) 2016 Sandeep Mistry All right reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "nrf.h"

#if defined(NRF51_SERIES)

extern "C" {
#include <string.h>
}


#include <Arduino.h>
#include <wiring_private.h>

#include "Wire.h"

TwoWire::TwoWire(NRF_TWI_Type * p_twi, uint8_t pinSDA, uint8_t pinSCL)
{
  this->_p_twi = p_twi;
  this->_uc_pinSDA = pinSDA;
  this->_uc_pinSCL = pinSCL;
  this->transmissionBegun = false;
  this->suspended = false;
}

#ifdef ARDUINO_GENERIC
void TwoWire::setPins(uint8_t pinSDA, uint8_t pinSCL)
{
  this->_uc_pinSDA = pinSDA;
  this->_uc_pinSCL = pinSCL;
}
#endif // ARDUINO_GENERIC

void TwoWire::begin(void) {
  //Master Mode
  master = true;

  NRF_GPIO_Type* portSCL = digitalPinToPort(_uc_pinSCL);
  NRF_GPIO_Type* portSDA = digitalPinToPort(_uc_pinSDA);
  uint32_t pinSCL = digitalPinToPin(_uc_pinSCL);
  uint32_t pinSDA = digitalPinToPin(_uc_pinSDA);

  portSCL->PIN_CNF[pinSCL] = ((uint32_t)GPIO_PIN_CNF_DIR_Input       << GPIO_PIN_CNF_DIR_Pos)
                            | ((uint32_t)GPIO_PIN_CNF_INPUT_Connect  << GPIO_PIN_CNF_INPUT_Pos)
                            | ((uint32_t)GPIO_PIN_CNF_PULL_Pullup    << GPIO_PIN_CNF_PULL_Pos)
                            | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0D1     << GPIO_PIN_CNF_DRIVE_Pos)
                            | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);

  portSDA->PIN_CNF[pinSDA] = ((uint32_t)GPIO_PIN_CNF_DIR_Input       << GPIO_PIN_CNF_DIR_Pos)
                            | ((uint32_t)GPIO_PIN_CNF_INPUT_Connect  << GPIO_PIN_CNF_INPUT_Pos)
                            | ((uint32_t)GPIO_PIN_CNF_PULL_Pullup    << GPIO_PIN_CNF_PULL_Pos)
                            | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0D1     << GPIO_PIN_CNF_DRIVE_Pos)
                            | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);

  _p_twi->FREQUENCY = (TWI_FREQUENCY_FREQUENCY_K100 << TWI_FREQUENCY_FREQUENCY_Pos);
  _p_twi->ENABLE = (TWI_ENABLE_ENABLE_Enabled << TWI_ENABLE_ENABLE_Pos);
  _p_twi->PSELSCL = g_ADigitalPinMap[_uc_pinSCL];
  _p_twi->PSELSDA = g_ADigitalPinMap[_uc_pinSDA];
}

void TwoWire::setClock(uint32_t baudrate) {
  _p_twi->ENABLE = (TWI_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos);

  uint32_t frequency;

  if (baudrate <= 100000)
  {
    frequency = TWI_FREQUENCY_FREQUENCY_K100;
  }
  else if (baudrate <= 250000)
  {
    frequency = TWI_FREQUENCY_FREQUENCY_K250;
  }
  else
  {
    frequency = TWI_FREQUENCY_FREQUENCY_K400;
  }

  _p_twi->FREQUENCY = (frequency << TWI_FREQUENCY_FREQUENCY_Pos);
  _p_twi->ENABLE = (TWI_ENABLE_ENABLE_Enabled << TWI_ENABLE_ENABLE_Pos);
}

void TwoWire::end() {
  _p_twi->ENABLE = (TWI_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos);
}

uint8_t TwoWire::requestFrom(uint8_t address, size_t quantity, bool stopBit)
{
  if (quantity == 0)
  {
    return 0;
  }
  if (quantity > SERIAL_BUFFER_SIZE)
  {
    quantity = SERIAL_BUFFER_SIZE;
  }

  size_t byteRead = 0;
  rxBuffer.clear();

  _p_twi->ADDRESS = address;
  _p_twi->SHORTS = 0x1UL;    // To trigger suspend task when a byte is received

  if (!this->suspended) {
    _p_twi->TASKS_RESUME = 0x1UL;
    _p_twi->TASKS_STARTRX = 0x1UL;
  }

  for (byteRead = 0; byteRead < quantity; byteRead++)
  {
    if (byteRead == quantity - 1)
    {
      // To trigger stop task when last byte is received, set before resume task.
      if (stopBit) {
        _p_twi->SHORTS = 0x2UL;
      }
    }

    _p_twi->TASKS_RESUME = 0x1UL;

    while (!_p_twi->EVENTS_RXDREADY && !_p_twi->EVENTS_ERROR);

    if (_p_twi->EVENTS_ERROR)
    {
      break;
    }

    _p_twi->EVENTS_RXDREADY = 0x0UL;

    rxBuffer.store_char(_p_twi->RXD);
  }

  if (stopBit || _p_twi->EVENTS_ERROR)
  {
    this->suspended = false;
    _p_twi->TASKS_STOP = 0x1UL;
    while(!_p_twi->EVENTS_STOPPED);
    _p_twi->EVENTS_STOPPED = 0x0UL;
  }
  else
  {
    this->suspended = true;
    _p_twi->TASKS_SUSPEND = 0x1UL;
    while(!_p_twi->EVENTS_SUSPENDED);
    _p_twi->EVENTS_SUSPENDED = 0x0UL;
  }

  if (_p_twi->EVENTS_ERROR)
  {
    _p_twi->EVENTS_ERROR = 0x0UL;
  }

  return byteRead;
}

uint8_t TwoWire::requestFrom(uint8_t address, size_t quantity)
{
  return requestFrom(address, quantity, true);
}

void TwoWire::beginTransmission(uint8_t address) {
  // save address of target and clear buffer
  txAddress = address;
  txBuffer.clear();

  transmissionBegun = true;
}

// Errors:
//  0 : Success
//  1 : Data too long
//  2 : NACK on transmit of address
//  3 : NACK on transmit of data
//  4 : Other error
uint8_t TwoWire::endTransmission(bool stopBit)
{
  transmissionBegun = false;

  // Start I2C transmission
  _p_twi->ADDRESS = txAddress;
  _p_twi->SHORTS = 0x0UL;
  _p_twi->TASKS_RESUME = 0x1UL;
  _p_twi->TASKS_STARTTX = 0x1UL;

  while (txBuffer.available())
  {
    _p_twi->TXD = txBuffer.read_char();

    while(!_p_twi->EVENTS_TXDSENT && !_p_twi->EVENTS_ERROR);

    if (_p_twi->EVENTS_ERROR)
    {
      break;
    }

    _p_twi->EVENTS_TXDSENT = 0x0UL;
  }

  if (stopBit || _p_twi->EVENTS_ERROR)
  {
    _p_twi->TASKS_STOP = 0x1UL;
    while(!_p_twi->EVENTS_STOPPED);
    _p_twi->EVENTS_STOPPED = 0x0UL;
  }
  else
  {
    _p_twi->TASKS_SUSPEND = 0x1UL;
    while(!_p_twi->EVENTS_SUSPENDED);
    _p_twi->EVENTS_SUSPENDED = 0x0UL;
  }

  if (_p_twi->EVENTS_ERROR)
  {
    _p_twi->EVENTS_ERROR = 0x0UL;

    uint32_t error = _p_twi->ERRORSRC;

    _p_twi->ERRORSRC = error;

    if (error == TWI_ERRORSRC_ANACK_Msk)
    {
      return 2;
    }
    else if (error == TWI_ERRORSRC_DNACK_Msk)
    {
      return 3;
    }
    else
    {
      return 4;
    }
  }

  return 0;
}

uint8_t TwoWire::endTransmission()
{
  return endTransmission(true);
}

size_t TwoWire::write(uint8_t ucData)
{
  // No writing, without begun transmission or a full buffer
  if ( !transmissionBegun || txBuffer.isFull() )
  {
    return 0 ;
  }

  txBuffer.store_char( ucData ) ;

  return 1 ;
}

size_t TwoWire::write(const uint8_t *data, size_t quantity)
{
  //Try to store all data
  for(size_t i = 0; i < quantity; ++i)
  {
    //Return the number of data stored, when the buffer is full (if write return 0)
    if(!write(data[i]))
      return i;
  }

  //All data stored
  return quantity;
}

int TwoWire::available(void)
{
  return rxBuffer.available();
}

int TwoWire::read(void)
{
  return rxBuffer.read_char();
}

int TwoWire::peek(void)
{
  return rxBuffer.peek();
}

void TwoWire::flush(void)
{
  // Do nothing, use endTransmission(..) to force
  // data transfer.
}

#if WIRE_INTERFACES_COUNT > 0
TwoWire Wire(NRF_TWI1, PIN_WIRE_SDA, PIN_WIRE_SCL);
#endif

#if WIRE_INTERFACES_COUNT > 1
TwoWire Wire1(NRF_TWI0, PIN_WIRE1_SDA, PIN_WIRE1_SCL);
#endif

#endif
