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

  3, // A0 - left pad
  2, // A1 - middle pad
  1, // A2 - right pad
  4, // A3 - COL1
  5, // A4 - COL2
  17, // BTN A
  12, // COL9
  11, // COL8
  18,
  10, // COL7
  6,  // A5 - COL3
  26, // BTN B
  20,
  23, // SCK
  22, // MISO
  21, // MOSI
  16,
  (uint32_t)-1, // 3.3V
  (uint32_t)-1, // 3.3V
  0,  // SCL
  30, // SDA
  25, // RX
  24, // TX
  7,  // COL4
  8,  // COL5
  9,  // COL6
  13, // ROW1
  14, // ROW2
  15, // ROW3
  28, // ACCEL INT 1
  27, // ACCEL INT 2
  29, // MAG INT 2
  19 // RST
};
