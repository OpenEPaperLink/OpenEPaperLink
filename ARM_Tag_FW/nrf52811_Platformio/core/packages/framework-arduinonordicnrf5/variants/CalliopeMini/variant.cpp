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
  // 0 - 19 (available on expension connector)
  0, // left pad (not analog)
  1, // bottom left pad; A1
  2, // bottom right pad; A2
  22, // right pad; SPI SCLK (not analog)
  4, // LED row 1; A3
  5, // LED row 2; A4
  6, // LED row 3; A5
  7, // LED row 4
  8, // LED row 5
  9, // LED row 6
  10, // LED row 7
  11, // LED row 8
  12, // LED row 9
  13, // LED column 1
  14, // LED column 2
  15, // LED column 3
  26, // serial RX; SPI MISO, A6
  27, // serial TX; SPI MOSI, A7
  20, // SDA
  19, // SCL

  // 20-25 (internal; not available on external connector)
  17, // button A
  23, // unassigned
  16, // button B
  28, // motor driver enable
  29, // motor driver in1
  30, // motor driver in2

  // 26-30
  18, // neopixel
  21, // accelerometer chip interrupt
  3,  // microphone; A0
  24, // usb serial TX
  25, // usb serial RX
};
