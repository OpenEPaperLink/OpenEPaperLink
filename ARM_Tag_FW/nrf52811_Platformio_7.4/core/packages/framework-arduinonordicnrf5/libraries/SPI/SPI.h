/*
 * SPI Master library for nRF5x.
 * Copyright (c) 2015 Arduino LLC
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

#ifndef _SPI_H_INCLUDED
#define _SPI_H_INCLUDED

#include <Arduino.h>

// SPI_HAS_TRANSACTION means SPI has
//   - beginTransaction()
//   - endTransaction()
//   - usingInterrupt()
//   - SPISetting(clock, bitOrder, dataMode)
#define SPI_HAS_TRANSACTION 1

#define SPI_MODE0 0x02
#define SPI_MODE1 0x00
#define SPI_MODE2 0x03
#define SPI_MODE3 0x01


class SPISettings {
  public:
  SPISettings(uint32_t clock, BitOrder bitOrder, uint8_t dataMode) {
    if (__builtin_constant_p(clock)) {
      init_AlwaysInline(clock, bitOrder, dataMode);
    } else {
      init_MightInline(clock, bitOrder, dataMode);
    }
  }

  // Default speed set to 4MHz, SPI mode set to MODE 0 and Bit order set to MSB first.
  SPISettings() { init_AlwaysInline(4000000, MSBFIRST, SPI_MODE0); }

  private:
  void init_MightInline(uint32_t clock, BitOrder bitOrder, uint8_t dataMode) {
    init_AlwaysInline(clock, bitOrder, dataMode);
  }

  void init_AlwaysInline(uint32_t clock, BitOrder bitOrder, uint8_t dataMode) __attribute__((__always_inline__)) {
    if (clock <= 125000) {
      this->clockFreq = SPI_FREQUENCY_FREQUENCY_K125;
    } else if (clock <= 250000) {
      this->clockFreq = SPI_FREQUENCY_FREQUENCY_K250;
    } else if (clock <= 500000) {
      this->clockFreq = SPI_FREQUENCY_FREQUENCY_K500;
    } else if (clock <= 1000000) {
      this->clockFreq = SPI_FREQUENCY_FREQUENCY_M1;
    } else if (clock <= 2000000) {
      this->clockFreq = SPI_FREQUENCY_FREQUENCY_M2;
    } else if (clock <= 4000000) {
      this->clockFreq = SPI_FREQUENCY_FREQUENCY_M4;
    } else {
      this->clockFreq = SPI_FREQUENCY_FREQUENCY_M8;
    }

    this->bitOrder = (bitOrder == MSBFIRST ? SPI_CONFIG_ORDER_MsbFirst : SPI_CONFIG_ORDER_LsbFirst);
    this->dataMode = dataMode;
  }

  uint32_t clockFreq;
  uint8_t dataMode;
  uint32_t bitOrder;

  friend class SPIClass;
};

class SPIClass {
  public:
  SPIClass(NRF_SPI_Type *p_spi, uint8_t uc_pinMISO, uint8_t uc_pinSCK, uint8_t uc_pinMOSI);


  byte transfer(uint8_t data);
  uint16_t transfer16(uint16_t data);
  inline void transfer(void *buf, size_t count);

  // Transaction Functions
  void usingInterrupt(int interruptNumber);
  void beginTransaction(SPISettings settings);
  void endTransaction(void);

  // SPI Configuration methods
  void attachInterrupt();
  void detachInterrupt();

#ifdef ARDUINO_GENERIC
  void setPins(uint8_t uc_pinMISO, uint8_t uc_pinSCK, uint8_t uc_pinMOSI);
#endif // ARDUINO_GENERIC
  void begin();
  void end();

  void setBitOrder(BitOrder order);
  void setDataMode(uint8_t uc_mode);
  void setClockDivider(uint8_t uc_div);

  private:
  void init();
  void config(SPISettings settings);

  NRF_SPI_Type *_p_spi;
  uint8_t _uc_pinMiso;
  uint8_t _uc_pinMosi;
  uint8_t _uc_pinSCK;

  uint8_t _dataMode;
  uint32_t _bitOrder;

  bool initialized;
  uint8_t interruptMode;
  char interruptSave;
  uint32_t interruptMask;
};

void SPIClass::transfer(void *buf, size_t count)
{
  // TODO: Optimize for faster block-transfer
  uint8_t *buffer = reinterpret_cast<uint8_t *>(buf);
  for (size_t i=0; i<count; i++)
    buffer[i] = transfer(buffer[i]);
}

#if SPI_INTERFACES_COUNT > 0
extern SPIClass SPI;
#endif
#if SPI_INTERFACES_COUNT > 1
extern SPIClass SPI1;
#endif

// For compatibility with sketches designed for AVR @ 16 MHz
// New programs should use SPI.beginTransaction to set the SPI clock
#ifdef F_CPU
#define DIVISOR F_CPU / 16000000
#define SPI_CLOCK_DIV2   2 * DIVISOR
#define SPI_CLOCK_DIV4   4 * DIVISOR
#define SPI_CLOCK_DIV8   8 * DIVISOR
#define SPI_CLOCK_DIV16  16 * DIVISOR
#define SPI_CLOCK_DIV32  32 * DIVISOR
#define SPI_CLOCK_DIV64  64 * DIVISOR
#define SPI_CLOCK_DIV128 128 * DIVISOR
#endif

#endif