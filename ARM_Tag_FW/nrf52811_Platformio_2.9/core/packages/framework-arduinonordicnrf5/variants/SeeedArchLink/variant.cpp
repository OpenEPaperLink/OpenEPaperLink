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

  // D0 - D7
  7, //RX
  8, //TX
  9, //W5500 interrupt
  10,
  11, //SS_SDCARD
  12,
  13,
  17, //W5500 reset

  // D8 - D13
  18,
  23,
  24, //SS_W5500
  25, //SPI MOSI
  28, //SPI MISO
  29, //SPI SCK

  // A0 - A6
  1,
  2,
  3,
  4,
  5, //I2C SDA
  6, //I2C SDA
  30, //LED

  // AREF
  0,
};
