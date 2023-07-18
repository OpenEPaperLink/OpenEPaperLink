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
  11,
  9,
  10,
  8,
  21,
  23,
  16,
  17,

  // D8 - D13
  19,
  18,
  14,
  12,
  13,
  15,

  // A0 - A5
  1,
  2,
  3,
  4,
  5,
  6,

  // SDA, SCL
  29,
  28,

  // MISO, SCK, MOSI
  22,
  25,
  30,


  // AREF
  0
};
