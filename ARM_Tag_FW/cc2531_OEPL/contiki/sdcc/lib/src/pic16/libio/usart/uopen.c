/*-------------------------------------------------------------------------
   uopen - initialize USART module

   Copyright (C) 2004, Vangelis Rokas <vrokas AT otenet.gr>

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

/*
 * Devices implemented:
 *	PIC18F[24][45][28]
 */


#include <pic18fregs.h>
#include <usart.h>

// USART Status Structure
extern union USART USART_Status;

void
usart_open (unsigned char config, sdcc_spbrg_t spbrg) __wparam
{
  TXSTA = 0;           // Reset USART registers to POR state
  RCSTA = 0;

  if (config & 0x01)
    TXSTAbits.SYNC = 1;

  if (config & 0x02)
    {
      TXSTAbits.TX9 = 1;
      RCSTAbits.RX9 = 1;
    }

  if (config & 0x04)
    TXSTAbits.CSRC = 1;

  if (config & 0x08)
    RCSTAbits.CREN = 1;
  else
    RCSTAbits.SREN = 1;

  if (config & 0x10)
    {
      TXSTAbits.BRGH = 1;
#if !__SDCC_NO_SPBRGH
      BAUDCONbits.BRG16 = 1;
    }
  else
    {
      BAUDCONbits.BRG16 = 0;
#endif  /* !__SDCC_NO_SPBRGH */
    }

  /* TX interrupts */

  PIR1bits.TXIF = 0;

  if (config & 0x40)
    PIE1bits.RCIE = 1;
  else
    PIE1bits.RCIE = 0;

  /* RX interrupts */
  PIR1bits.RCIF = 0;

  if (config & 0x80)
    PIE1bits.TXIE = 1;
  else
    PIE1bits.TXIE = 0;

#if !__SDCC_NO_SPBRGH
  SPBRGH = (spbrg >> 8);
#endif  /* !__SDCC_NO_SPBRGH */
  SPBRG = spbrg;

#if (__SDCC_USART_STYLE == 1812200)
  /* Configure RX/TX pins as digital pins. */
  ADCON1bits.PCFG5 = 1;
  ADCON1bits.PCFG6 = 1;

  /* Configure RX/TX pins as inputs. */
  TRISBbits.TRISB1 = 1;
  TRISBbits.TRISB4 = 1;
#elif (__SDCC_USART_STYLE == 1812300)
  /* Configure RX/TX pins as inputs. */
  TRISAbits.TRISA2 = 1;
  TRISAbits.TRISA3 = 1;
#elif (__SDCC_USART_STYLE == 1813502)
  /* Configure RX pin as digital pin. */
  ANSELHbits.ANS11 = 0;

  /* Configure RX/TX pins as inputs. */
  TRISBbits.TRISB5 = 1;
  TRISBbits.TRISB7 = 1;
#elif (__SDCC_USART_STYLE == 1822200)
  /* Configure RX/TX pins. */
  TRISCbits.TRISC6 = 0;
  TRISCbits.TRISC7 = 1;
#elif (__SDCC_USART_STYLE == 1822210) \
   || (__SDCC_USART_STYLE == 1865850)
  /* Configure RX/TX pins. */
  TRISCbits.TRISC6 = 1;
  TRISCbits.TRISC7 = 1;
#elif (__SDCC_USART_STYLE == 1824500)
  /* Configure RX/TX pins. */
  TRISCbits.TRISC6 = 0;
  TRISCbits.TRISC7 = 1;
#elif (__SDCC_USART_STYLE == 1824501) \
   || (__SDCC_USART_STYLE == 1865200)
  /* Configure RX1/TX1 pins. */
  TRISCbits.TRISC6 = 0;
  TRISCbits.TRISC7 = 1;
#else /* other devices */
#error Invalid USART style.
#endif /* other devices */

  TXSTAbits.TXEN = 1;
  RCSTAbits.SPEN = 1;
}
