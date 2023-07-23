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
  21,
  22,
  23,
  24,
  25,
  28,
  29,
  30,

  // ??
  // ??

  // A0 - A7
  6,
  5,
  4,
  3,
  2,
  1,
  0,
  13,

  // RX, TX
  12,
  8,

  // SS1, MOSI1, MISO1, SCK1
  15,
  9,
  14,
  10,

  // BTN
  7,

  // RBG
  19,
  18,
  17
};
