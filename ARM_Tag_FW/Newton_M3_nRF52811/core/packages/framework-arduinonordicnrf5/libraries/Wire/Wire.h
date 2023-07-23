/*
 * TWI/I2C library for mRF5x
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

#ifndef TwoWire_h
#define TwoWire_h

#include "nrf.h"

#include "Stream.h"
#include "variant.h"

#include "RingBuffer.h"

// WIRE_HAS_END means Wire has end()
#define WIRE_HAS_END 1

class TwoWire : public Stream
{
  public:
#if defined(NRF52_SERIES)
    TwoWire(NRF_TWIM_Type * p_twim, NRF_TWIS_Type * p_twis, IRQn_Type IRQn, uint8_t pinSDA, uint8_t pinSCL);
#else
    TwoWire(NRF_TWI_Type * p_twi, uint8_t pinSDA, uint8_t pinSCL);
#endif
#ifdef ARDUINO_GENERIC
    void setPins(uint8_t pinSDA, uint8_t pinSCL);
#endif // ARDUINO_GENERIC
    void begin();
#if defined(NRF52_SERIES)
    void begin(uint8_t);
#endif
    void end();
    void setClock(uint32_t);

    void beginTransmission(uint8_t);
    uint8_t endTransmission(bool stopBit);
    uint8_t endTransmission(void);

    uint8_t requestFrom(uint8_t address, size_t quantity, bool stopBit);
    uint8_t requestFrom(uint8_t address, size_t quantity);

    size_t write(uint8_t data);
    size_t write(const uint8_t * data, size_t quantity);

    virtual int available(void);
    virtual int read(void);
    virtual int peek(void);
    virtual void flush(void);
#if defined(NRF52_SERIES)
    void onReceive(void(*)(int));
    void onRequest(void(*)(void));
    void onService(void);
#endif

    using Print::write;

  private:
#if defined(NRF52_SERIES)
    NRF_TWIM_Type * _p_twim;
    NRF_TWIS_Type * _p_twis;
#else
    NRF_TWI_Type * _p_twi;
#endif

    IRQn_Type _IRQn;

    uint8_t _uc_pinSDA;
    uint8_t _uc_pinSCL;

    bool master;
    bool receiving;
    bool transmissionBegun;
    bool suspended;

    // RX Buffer
    RingBuffer rxBuffer;

    // TX buffer
    RingBuffer txBuffer;
    uint8_t txAddress;

    // Callback user functions
    void (*onRequestCallback)(void);
    void (*onReceiveCallback)(int);

    // TWI clock frequency
    static const uint32_t TWI_CLOCK = 100000;
};

#if WIRE_INTERFACES_COUNT > 0
extern TwoWire Wire;
#endif

#if WIRE_INTERFACES_COUNT > 1
extern TwoWire Wire1;
#endif

#endif
