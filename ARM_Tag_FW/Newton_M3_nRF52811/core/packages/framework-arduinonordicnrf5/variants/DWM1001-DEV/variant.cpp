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

#include "variant.h"

const uint32_t g_ADigitalPinMap[] = {
  // D0 - D8
  12,
  27,
  23,
  13,
  15,
  8,
  26,
  6,
  7,

  // A0, A1
  4,
  3,

  // SDA, SCL
  29,
  28,

  // RX, TX
  11,
  5,

  // DWM1000
  // SPI SS1, MISO1, MOSI1, SCK1
  17,
  18,
  20,
  16,
  // RST, IRQ
  24,
  19,

  // ACC IRQ
  25,

  // LEDS
  14,
  22,
  30,
  31,

  // BTN
  2

};
