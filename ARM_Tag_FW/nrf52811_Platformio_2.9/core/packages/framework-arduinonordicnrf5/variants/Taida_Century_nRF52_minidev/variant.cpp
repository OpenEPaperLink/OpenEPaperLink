/*
  Copyright (c) 2014-2015 Arduino LLC.  All right reserved.
  Copyright (c) 2016 Sandeep Mistry All right reserved.
  Copyright (c) 2016 Frank Holtz. All right reserved.

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
  25, // D0, near Radio! -> Low drive, low frequency I/O only.
  26, // D1, near Radio! -> Low drive, low frequency I/O only.
  27, // D2, near Radio! -> Low drive, low frequency I/O only.
  28, // D3, near Radio! -> Low drive, low frequency I/O only.
  29, // D4, NOT CONNECTED, near Radio! -> Low drive, low frequency I/O only.
  30, // D5, LED1, near Radio! -> Low drive, low frequency I/O only.
  31, // D6, LED2, near Radio! -> Low drive, low frequency I/O only.
  2,  // D7, PIN_WIRE_SDA

  // D8 - D13
  3,  // D8, PIN_WIRE_SCL
  4,  // D9, BUTTON1, NFC-Antenna 1
  5,  // D10
  0,  // D11, NOT CONNECTED
  1,  // D12, NOT CONNECTED
  6,  // D13

  // D14 - D30
  7,  // D14
  8,  // D15
  9,  // D16, NOT CONNECTED
  10, // D17, NFC-Antenna 2, NOT CONNECTED
  11, // D18, RXD
  12, // D19, TXD
  13, // D20, SS
  14, // D21, MISO
  15, // D22, MOSI
  16, // D23, SCK
  17, // D24, A0
  18, // D25, A1
  19, // D26, A2
  20, // D27, A3
  22, // D28, A4, near Radio! -> Low drive, low frequency I/O only.
  23, // D29, A5, near Radio! -> Low drive, low frequency I/O only.
  24, // D30,
  
  21, // RESET
};
