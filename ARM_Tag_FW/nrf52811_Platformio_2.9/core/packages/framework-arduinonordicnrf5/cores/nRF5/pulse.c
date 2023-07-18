/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.

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

#include "nrf.h"

#include <Arduino.h>

// See pulse_asm.S
extern unsigned long countPulseASM(const volatile uint32_t *port, uint32_t bit, uint32_t stateMask, unsigned long maxloops);

/* Measures the length (in microseconds) of a pulse on the pin; state is HIGH
 * or LOW, the type of pulse to measure.  Works on pulses from 2-3 microseconds
 * to 3 minutes in length, but must be called at least a few dozen microseconds
 * before the start of the pulse. */
uint32_t pulseIn(uint32_t ulPin, uint32_t state, uint32_t timeout)
{
  // cache the port and bit of the pin in order to speed up the
  // pulse width measuring loop and achieve finer resolution.  calling
  // digitalRead() instead yields much coarser resolution.
  // PinDescription p = g_APinDescription[pin];
  NRF_GPIO_Type* port = digitalPinToPort(ulPin);
  uint32_t bit = digitalPinToBitMask(ulPin);
  uint32_t stateMask = state ? bit : 0;

  // convert the timeout from microseconds to a number of times through
  // the initial loop; it takes (roughly) 10 clock cycles per iteration.
  uint32_t maxloops = microsecondsToClockCycles(timeout) / 10;

  // count low-level loops during the pulse (or until maxLoops)
  // a zero loopCount means that a complete pulse was not detected within the timeout
  uint32_t loopCount = countPulseASM(&(port->IN), bit, stateMask, maxloops);

  // convert the reading to (approximate) microseconds. The loop time as measured with an
  // oscilloscope is 10 cycles on a BBC micro:bit 1.3 (nRF51822). There is error because the
  // time is quantized to an integral number of loops and because interrupt may steal cycles.
  return clockCyclesToMicroseconds(10 * loopCount);
}
