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

/*
                                Top view
            RX - D0 | P0.14                 P0.27 | D7  - SDA
            Tx - D1 | P0.13                 P0.26 | D8  - SCL
                 D2 | P0.12                 P0.25 | D9
            SS - D3 | P0.11            AIN3/P0.05 | D10 - A3
          MISO - D4 | P0.08            AIN2/P0.04 | D11 - A2
          MOSI - D5 | P0.07            AIN1/P0.03 | D12 - A1
           SCK - D6 | P0.06            AIN0/P0.02 | D13 - A0
                    | NFC2/P0.10            Vin   |
                    | NFC1/P0.09            Gnd   |


                            Bottom view (smt pads)
                  D19 | P0.24            P0.23 | D14
             A4 - D20 | P0.28/AIN4       P0.22 | D15
             A5 - D21 | P0.29/AIN5       P0.18 | D16
             A6 - D22 | P0.30/AIN6       P0.16 | D17
             A7 - D23 | P0.31/AIN7       P0.15 | D18


                      LEDs
                  Red   - P0.19 | D24
                  Green - P0.19 | D25
                  Blue  - P0.19 | D26 - LED_BUILTIN

*/

const uint32_t g_ADigitalPinMap[] = {
  14,
  13,
  12,
  11,
  8,
  7,
  6,
  27,
  26,
  25,
  5,
  4,
  3,
  2,
  23,
  22,
  18,
  16,
  15,
  24,
  28,
  29,
  30,
  31,
  19,
  20,
  17,
};
