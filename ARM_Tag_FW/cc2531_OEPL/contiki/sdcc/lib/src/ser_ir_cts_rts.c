/*-------------------------------------------------------------------------
   ser_ir_cts_rts.c - source file for serial routines

   Copyright (C) 1999, Josef Wolf <jw AT raven.inka.de>

   Revisions:
   1.0  Bela Torok <bela.torok.kssg.ch> Jul. 2000
   RTS / CTS protocol added

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License 
   along with this library; see the file COPYING. If not, write to the
   Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.

   As a special exception, if you link this library with other files,
   some of which are compiled with SDCC, to produce an executable,
   this library does not by itself cause the resulting executable to
   be covered by the GNU General Public License. This exception does
   not however invalidate any other reasons why the executable file
   might be covered by the GNU General Public License.
-------------------------------------------------------------------------*/

/* This file implements a serial interrupt handler and its supporting
* routines. Compared with the existing serial.c and _ser.c it has
* following advantages:
* - You can specify arbitrary buffer sizes (umm, up to 255 bytes),
*   so it can run on devices with _little_ memory like at89cx051.
* - It won't overwrite characters which already are stored in the
*   receive-/transmit-buffer.
*/

/* BUG: those definitions (and the #include) should be set dynamically
* (while linking or at runtime) to make this file a _real_ library.
*/

/* RTS/CTS protocol howto:


   Shematic of cable for RTS/CTS protocol (B. Torok - Jun. 2000)

<- DB9 female connector -><- RS232 driver/receiver -><- 8051 system ->
   connect to PC             e.g. MAX232

                             RS232         TTL
                             level         level

   DCD    DTR                
   Pin1---Pin4               
                            Transmitters/Receivers
   RXD
   Pin2-----------------------------<<<-------------------TXD

   TXD
   Pin3----------------------------->>>-------------------RXD

   GND
   Pin5---------------------------------------------------GND

   DSR    CTS
   Pin6---Pin8----------------------<<<-------------------CTS (see #define CTS)

   RTS
   Pin7----------------------------->>>-------------------RTS (see #define RTS)
*/


#include <8051.h>
#include "ser_ir.h"

#define TXBUFLEN 3
#define RXBUFLEN 18      // The minimum rx buffer size for safe communications
                         // is 17. (The UART in the PC has a 16-byte FIFO.)
// TXBUFLEN & RXBUFLEN can be highher if rxbuf[] and txbuf[] is in xdata, max size is limited to 256!

#define THRESHOLD 16
#define ENABLE    0
#define DISABLE   1

#define CTS P3_6          // CTS & RTS can be assigned to any free pins
#define RTS P3_7

// You might want to specify idata, pdata or xdata for the buffers
static unsigned char rxbuf[RXBUFLEN], txbuf[TXBUFLEN];
static unsigned char rxcnt, txcnt, rxpos, txpos;
static bit busy;

void
ser_init ()
{
  ES = 0;
  rxcnt = txcnt = rxpos = txpos = 0;  // init buffers
  busy = 0;
  SCON = 0x50;               // mode 1 - 8-bit UART
  PCON |= 0x80;              // SMOD = 1;
  TMOD &= 0x0f;              // use timer 1
  TMOD |= 0x20;
//  TL1 = TH1 = 256 - 104;      // 600bps with 12 MHz crystal
//  TL1 = TH1 = 256 - 52;      // 1200bps with 12 MHz crystal
//  TL1 = TH1 = 256 - 26;      // 2400bps with 12 MHz crystal
  TL1 = TH1 = 256 - 13;      // 4800bps with 12 MHz crystal

  TR1 = 1;                   // Enable timer 1
  ES = 1;

  CTS = ENABLE;
}

void
ser_handler (void) interrupt 4
{
  if (RI) {
    RI = 0;
    /* don't overwrite chars already in buffer */
    if(rxcnt < RXBUFLEN) rxbuf [(unsigned char)(rxpos + rxcnt++) % RXBUFLEN] = SBUF;
    if(rxcnt >= (RXBUFLEN - THRESHOLD)) CTS = DISABLE;
  }

  if (TI) {
    TI = 0;
    if (busy = txcnt) {   /* Assignment, _not_ comparison! */
      txcnt--;
      SBUF = txbuf[txpos++];
      if(txpos >= TXBUFLEN) txpos = 0;
    }
  }
}

void
ser_putc (unsigned char c)
{
  while(txcnt >= TXBUFLEN);   // wait for room in buffer

  while(RTS == DISABLE);

  ES = 0;
  if (busy) {
    txbuf[(unsigned char)(txpos + txcnt++) % TXBUFLEN] = c;
  } else {
    SBUF = c;
    busy = 1;
  }
  ES = 1;
}

unsigned char
ser_getc (void)
{
  unsigned char c;

  while (!rxcnt) {        // wait for a character
    CTS = ENABLE;
  }

  ES = 0;
  rxcnt--;
  c = rxbuf[rxpos++];
  if (rxpos >= RXBUFLEN) rxpos = 0;
  ES = 1;
  return (c);
}

#pragma save
#pragma noinduction
void
ser_puts (unsigned char *s)
{
  unsigned char c;
  while (c= *s++) {
    if (c == '\n') ser_putc('\r');
    ser_putc (c);
  }
}
#pragma restore

void
ser_gets (unsigned char *s, unsigned char len)
{
  unsigned char pos, c;

  pos = 0;
  while (pos <= len) {
    c = ser_getc();
    if (c == '\r') continue;        // discard CR's
    s[pos++] = c;
    if (c == '\n') break;           // NL terminates
  }
  s[pos] = '\0';                  // terminate string
}

unsigned char
ser_can_xmt (void)
{
  return TXBUFLEN - txcnt;
}

unsigned char
ser_can_rcv (void)
{
  return rxcnt;
}
