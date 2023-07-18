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

#if defined(NRF52_SERIES)

extern "C" {
#include <string.h>
}

#include <Arduino.h>
#include <wiring_private.h>

#include "Wire.h"

TwoWire::TwoWire(NRF_TWIM_Type * p_twim, NRF_TWIS_Type * p_twis, IRQn_Type IRQn, uint8_t pinSDA, uint8_t pinSCL)
{
  this->_p_twim = p_twim;
  this->_p_twis = p_twis;
  this->_IRQn = IRQn;
  this->_uc_pinSDA = pinSDA;
  this->_uc_pinSCL = pinSCL;
  transmissionBegun = false;
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

  _p_twim->FREQUENCY = TWIM_FREQUENCY_FREQUENCY_K100;
  _p_twim->ENABLE = (TWIM_ENABLE_ENABLE_Enabled << TWIM_ENABLE_ENABLE_Pos);
  _p_twim->PSEL.SCL = g_ADigitalPinMap[_uc_pinSCL];
  _p_twim->PSEL.SDA = g_ADigitalPinMap[_uc_pinSDA];

  NVIC_ClearPendingIRQ(_IRQn);
  NVIC_SetPriority(_IRQn, 2);
  NVIC_EnableIRQ(_IRQn);
}

void TwoWire::begin(uint8_t address) {
  //Slave mode
  master = false;

  NRF_GPIO->PIN_CNF[_uc_pinSCL] = ((uint32_t)GPIO_PIN_CNF_DIR_Input        << GPIO_PIN_CNF_DIR_Pos)
                                | ((uint32_t)GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)
                                | ((uint32_t)GPIO_PIN_CNF_PULL_Disabled    << GPIO_PIN_CNF_PULL_Pos)
                                | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0S1       << GPIO_PIN_CNF_DRIVE_Pos)
                                | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);

  NRF_GPIO->PIN_CNF[_uc_pinSDA] = ((uint32_t)GPIO_PIN_CNF_DIR_Input        << GPIO_PIN_CNF_DIR_Pos)
                                | ((uint32_t)GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)
                                | ((uint32_t)GPIO_PIN_CNF_PULL_Disabled    << GPIO_PIN_CNF_PULL_Pos)
                                | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0S1       << GPIO_PIN_CNF_DRIVE_Pos)
                                | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);

  _p_twis->ADDRESS[0] = address;
  _p_twis->CONFIG = TWIS_CONFIG_ADDRESS0_Msk;
  _p_twis->PSEL.SCL = g_ADigitalPinMap[_uc_pinSCL];
  _p_twis->PSEL.SDA = g_ADigitalPinMap[_uc_pinSDA];

  _p_twis->ORC = 0xff;

  _p_twis->INTENSET = TWIS_INTEN_STOPPED_Msk | TWIS_INTEN_ERROR_Msk | TWIS_INTEN_WRITE_Msk | TWIS_INTEN_READ_Msk;

  NVIC_ClearPendingIRQ(_IRQn);
  NVIC_SetPriority(_IRQn, 2);
  NVIC_EnableIRQ(_IRQn);

  _p_twis->ENABLE = (TWIS_ENABLE_ENABLE_Enabled << TWIS_ENABLE_ENABLE_Pos);
}

void TwoWire::setClock(uint32_t baudrate) {
  if (master) {
    _p_twim->ENABLE = (TWIM_ENABLE_ENABLE_Disabled << TWIM_ENABLE_ENABLE_Pos);

    uint32_t frequency;

    if (baudrate <= 100000)
    {
      frequency = TWIM_FREQUENCY_FREQUENCY_K100;
    }
    else if (baudrate <= 250000)
    {
      frequency = TWIM_FREQUENCY_FREQUENCY_K250;
    }
    else
    {
      frequency = TWIM_FREQUENCY_FREQUENCY_K400;
    }

    _p_twim->FREQUENCY = frequency;
    _p_twim->ENABLE = (TWIM_ENABLE_ENABLE_Enabled << TWIM_ENABLE_ENABLE_Pos);
  }
}

void TwoWire::end() {
  if (master)
  {
    _p_twim->ENABLE = (TWIM_ENABLE_ENABLE_Disabled << TWIM_ENABLE_ENABLE_Pos);
  }
  else
  {
    _p_twis->ENABLE = (TWIS_ENABLE_ENABLE_Disabled << TWIS_ENABLE_ENABLE_Pos);
  }
}

uint8_t TwoWire::requestFrom(uint8_t address, size_t quantity, bool stopBit)
{
  if(quantity == 0)
  {
    return 0;
  }

  size_t byteRead = 0;
  rxBuffer.clear();

  _p_twim->ADDRESS = address;

  _p_twim->TASKS_RESUME = 0x1UL;
  _p_twim->RXD.PTR = (uint32_t)rxBuffer._aucBuffer;
  _p_twim->RXD.MAXCNT = quantity;
  _p_twim->TASKS_STARTRX = 0x1UL;

  while(!_p_twim->EVENTS_RXSTARTED && !_p_twim->EVENTS_ERROR);
  _p_twim->EVENTS_RXSTARTED = 0x0UL;

  while(!_p_twim->EVENTS_LASTRX && !_p_twim->EVENTS_ERROR);
  _p_twim->EVENTS_LASTRX = 0x0UL;

  if (stopBit || _p_twim->EVENTS_ERROR)
  {
    _p_twim->TASKS_STOP = 0x1UL;
    while(!_p_twim->EVENTS_STOPPED);
    _p_twim->EVENTS_STOPPED = 0x0UL;
  }
  else
  {
    _p_twim->TASKS_SUSPEND = 0x1UL;
    while(!_p_twim->EVENTS_SUSPENDED);
    _p_twim->EVENTS_SUSPENDED = 0x0UL;
  }

  if (_p_twim->EVENTS_ERROR)
  {
    _p_twim->EVENTS_ERROR = 0x0UL;
  }

  byteRead = rxBuffer._iHead = _p_twim->RXD.AMOUNT;

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
  transmissionBegun = false ;

  // Start I2C transmission
  _p_twim->ADDRESS = txAddress;

  _p_twim->TASKS_RESUME = 0x1UL;

  _p_twim->TXD.PTR = (uint32_t)txBuffer._aucBuffer;
  _p_twim->TXD.MAXCNT = txBuffer.available();

  _p_twim->TASKS_STARTTX = 0x1UL;

  while(!_p_twim->EVENTS_TXSTARTED && !_p_twim->EVENTS_ERROR);
  _p_twim->EVENTS_TXSTARTED = 0x0UL;

  if (txBuffer.available()) {
    while(!_p_twim->EVENTS_LASTTX && !_p_twim->EVENTS_ERROR);
  }
  _p_twim->EVENTS_LASTTX = 0x0UL;

  if (stopBit || _p_twim->EVENTS_ERROR)
  {
    _p_twim->TASKS_STOP = 0x1UL;
    while(!_p_twim->EVENTS_STOPPED);
    _p_twim->EVENTS_STOPPED = 0x0UL;
  }
  else
  {
    _p_twim->TASKS_SUSPEND = 0x1UL;
    while(!_p_twim->EVENTS_SUSPENDED);
    _p_twim->EVENTS_SUSPENDED = 0x0UL;
  }

  if (_p_twim->EVENTS_ERROR)
  {
    _p_twim->EVENTS_ERROR = 0x0UL;

    uint32_t error = _p_twim->ERRORSRC;

    _p_twim->ERRORSRC = error;

    if (error == TWIM_ERRORSRC_ANACK_Msk)
    {
      return 2;
    }
    else if (error == TWIM_ERRORSRC_DNACK_Msk)
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

void TwoWire::onReceive(void(*function)(int))
{
  onReceiveCallback = function;
}

void TwoWire::onRequest(void(*function)(void))
{
  onRequestCallback = function;
}

void TwoWire::onService(void)
{
  if (_p_twis->EVENTS_WRITE)
  {
    _p_twis->EVENTS_WRITE = 0x0UL;

    receiving = true;

    rxBuffer.clear();

    _p_twis->RXD.PTR = (uint32_t)rxBuffer._aucBuffer;
    _p_twis->RXD.MAXCNT = sizeof(rxBuffer._aucBuffer);

    _p_twis->TASKS_PREPARERX = 0x1UL;
  }

  if (_p_twis->EVENTS_READ)
  {
    _p_twis->EVENTS_READ = 0x0UL;

    receiving = false;
    transmissionBegun = true;

    txBuffer.clear();

    if (onRequestCallback)
    {
      onRequestCallback();
    }

    transmissionBegun = false;

    _p_twis->TXD.PTR = (uint32_t)txBuffer._aucBuffer;
    _p_twis->TXD.MAXCNT = txBuffer.available();

    _p_twis->TASKS_PREPARETX = 0x1UL;
  }

  if (_p_twis->EVENTS_STOPPED)
  {
    _p_twis->EVENTS_STOPPED = 0x0UL;

    if (receiving)
    {
      int rxAmount = _p_twis->RXD.AMOUNT;

      rxBuffer._iHead = rxAmount;

      if (onReceiveCallback)
      {
        onReceiveCallback(rxAmount);
      }
    }
  }

  if (_p_twis->EVENTS_ERROR)
  {
    _p_twis->EVENTS_ERROR = 0x0UL;

    uint32_t error = _p_twis->ERRORSRC;
    _p_twis->ERRORSRC = error;

    _p_twis->TASKS_STOP = 0x1UL;
  }
}

TwoWire Wire(NRF_TWIM1, NRF_TWIS1, SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn, PIN_WIRE_SDA, PIN_WIRE_SCL);

#if WIRE_INTERFACES_COUNT > 0
extern "C"
{
  void SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler(void)
  {
    Wire.onService();
  }
}
#endif

#if WIRE_INTERFACES_COUNT > 1

TwoWire Wire1(NRF_TWIM0, NRF_TWIS0, SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn, PIN_WIRE1_SDA, PIN_WIRE1_SCL);

extern "C"
{
  void SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler(void)
  {
    Wire1.onService();
  }
}
#endif

#endif
