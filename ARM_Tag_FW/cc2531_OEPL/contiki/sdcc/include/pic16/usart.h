/*-------------------------------------------------------------------------
   usart.h - USART communications module library header

   Copyright (C) 2005, Vangelis Rokas <vrokas AT otenet.gr>

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

#ifndef __USART_H__
#define __USART_H__

#pragma library io


#define RAM_SCLS	__data

/* configuration bit masks for open function */
#define USART_TX_INT_ON   0xff
#define USART_TX_INT_OFF  0x7f
#define USART_RX_INT_ON   0xff
#define USART_RX_INT_OFF  0xbf
#define USART_BRGH_HIGH   0xff
#define USART_BRGH_LOW    0xef
#define USART_CONT_RX     0xff
#define USART_SINGLE_RX   0xf7
#define USART_SYNC_MASTER 0xff
#define USART_SYNC_SLAVE  0xfb
#define USART_NINE_BIT    0xff
#define USART_EIGHT_BIT   0xfd
#define USART_SYNCH_MODE  0xff
#define USART_ASYNCH_MODE 0xfe

/*
 * USART styles:
 *
 * --- Families with 1 USART ---
 *
 * INIT:
 *      RCSTA<7> = 1    (SPEN)
 *      TXSTA<4> = 0    (SYNC)
 *      TXSTA<5> = 1    (TXEN)
 *
 * 18f1220:
 *      RB1/AN5/TX and RB4/AN6/RX
 *
 *      TRISB<1> = TRISB<4> = 1 (TX, RX)
 *      ADCON1<5> = ADCON1<6> = 1 (PCFG<5>, PCFG<6>)
 *      SPBRGH:SPBRG
 *
 * 18f13k50:
 *      RB7/TX and RB5/AN11/RX
 *
 *      TRISB<7> = TRISB<5> = 1 (TX, RX)
 *      ANSELH<3> = 0 (ANS11/RX)
 *      SPBRGH:SPBRG
 *
 * 18f2220:
 *      RC6/TX and RC7/RX
 *
 *      TRISC<6> = 0    (TX)
 *      TRISC<7> = 1    (RX)
 *      SPBRG
 *
 * 18f2221/18f2331/18f23k20/18f2410/18f2420/18f2423/18f2455/18f24j10/18f2525:
 *      RC6/TX and RC7/RX
 *
 *      TRISC<6> = TRISC<7> = 1 (TX, RX)
 *      SPBRGH:SPBRG
 *
 * 18f2450/18f2480/18f2585/18f2682/18f6585/18f6680/18f8585/18f8680:
 *      RC6/TX and RC7/RX
 *
 *      TRISC<6> = 0    (TX)
 *      TRISC<7> = 1    (RX)
 *      SPBRGH:SPBRG
 *
 * --- Families with 2+ USARTs ---
 *
 * INIT:
 *      RCSTA1<7> = 1   (SPEN)
 *      TXSTA1<4> = 0   (SYNC)
 *      TXSTA1<5> = 1   (TXEN)
 *
 * 18f24j50/18f6527/18f65j50/18f66j60:
 *      RC6/TX1 and RC7/RX1 (EUSART1)
 *
 *      TRISC<6> = 0    (TX1)
 *      TRISC<7> = 1    (RX1)
 *      SPBRGH1:SPBRG1
 *
 * 18f6520:
 *      RC6/TX1 and RC7/RX1 (EUSART1)
 *
 *      TRISC<6> = 0    (TX1)
 *      TRISC<7> = 1    (RX1)
 *      SPBRG1
 *
 */
#include "pic18fam.h"

#if (__SDCC_USART_STYLE == 0)
#warning The target device is not supported by the SDCC PIC16 USART library.
#endif

#if (__SDCC_USART_STYLE == 1822200) || \
    (__SDCC_USART_STYLE == 1865200)
#define __SDCC_NO_SPBRGH        1
#endif  /* device lacks SPBRGH */


#if __SDCC_NO_SPBRGH
typedef unsigned char sdcc_spbrg_t;
#else   /* !__SDCC_NO_SPBRGH */
typedef unsigned int sdcc_spbrg_t;
#endif  /* !__SDCC_NO_SPBRGH */


/* status bits */
union USART
{
  unsigned char val;
  struct
  {
    unsigned RX_NINE:1;
    unsigned TX_NINE:1;
    unsigned FRAME_ERROR:1;
    unsigned OVERRUN_ERROR:1;
    unsigned fill:4;
  };
};

void usart_open (unsigned char config, sdcc_spbrg_t spbrg) __wparam;
void usart_close (void);

unsigned char usart_busy (void) __naked;
unsigned char usart_drdy (void) __naked;

unsigned char usart_getc (void);
void usart_gets (RAM_SCLS char * buffer, unsigned char len);

void usart_putc (unsigned char data) __wparam __naked;
void usart_puts (char * data);


void usart_baud (sdcc_spbrg_t baudconfig) __wparam;

#endif
