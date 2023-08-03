/*-------------------------------------------------------------------------
   mc68hc908jb8.h - Register Declarations for Motorola MC68HC908JB8

   Copyright (C) 2004, Bjorn Bringert <bjorn@bringert.net>
   Based on mc68hc908qy.h,
     written By - Erik Petrich epetrich@users.sourceforge.net (2003)

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


#ifndef _MC68HC908JB8_H
#define _MC68HC908JB8_H

#ifndef _UINT8
 #define _UINT8 unsigned char
#endif
#ifndef _UINT16
 #define _UINT16 unsigned int
#endif
#ifndef _VOLDATA
 #define _VOLDATA volatile __data
#endif
#ifndef _VOLXDATA
 #define _VOLXDATA volatile __xdata
#endif

struct __hc08_bits
{
  unsigned int bit0:1;
  unsigned int bit1:1;
  unsigned int bit2:1;
  unsigned int bit3:1;
  unsigned int bit4:1;
  unsigned int bit5:1;
  unsigned int bit6:1;
  unsigned int bit7:1;
};


_VOLDATA _UINT8 __at 0x00 PTA;     /* Port A Data Register */
  #define PTA0 ((struct __hc08_bits *)(&PTA))->bit0
  #define PTA1 ((struct __hc08_bits *)(&PTA))->bit1
  #define PTA2 ((struct __hc08_bits *)(&PTA))->bit2
  #define PTA3 ((struct __hc08_bits *)(&PTA))->bit3
  #define PTA4 ((struct __hc08_bits *)(&PTA))->bit4
  #define PTA5 ((struct __hc08_bits *)(&PTA))->bit5
  #define PTA6 ((struct __hc08_bits *)(&PTA))->bit6
  #define PTA7 ((struct __hc08_bits *)(&PTA))->bit7

_VOLDATA _UINT8 __at 0x01 PTB;     /* Port B Data Register */
  #define PTB0 ((struct __hc08_bits *)(&PTB))->bit0
  #define PTB1 ((struct __hc08_bits *)(&PTB))->bit1
  #define PTB2 ((struct __hc08_bits *)(&PTB))->bit2
  #define PTB3 ((struct __hc08_bits *)(&PTB))->bit3
  #define PTB4 ((struct __hc08_bits *)(&PTB))->bit4
  #define PTB5 ((struct __hc08_bits *)(&PTB))->bit5
  #define PTB6 ((struct __hc08_bits *)(&PTB))->bit6
  #define PTB7 ((struct __hc08_bits *)(&PTB))->bit7

_VOLDATA _UINT8 __at 0x02 PTC;     /* Port C Data Register */
  #define PTC0 ((struct __hc08_bits *)(&PTC))->bit0
  #define PTC1 ((struct __hc08_bits *)(&PTC))->bit1
  #define PTC2 ((struct __hc08_bits *)(&PTC))->bit2
  #define PTC3 ((struct __hc08_bits *)(&PTC))->bit3
  #define PTC4 ((struct __hc08_bits *)(&PTC))->bit4
  #define PTC5 ((struct __hc08_bits *)(&PTC))->bit5
  #define PTC6 ((struct __hc08_bits *)(&PTC))->bit6
  #define PTC7 ((struct __hc08_bits *)(&PTC))->bit7

_VOLDATA _UINT8 __at 0x03 PTD;     /* Port D Data Register */
  #define PTD0 ((struct __hc08_bits *)(&PTD))->bit0
  #define PTD1 ((struct __hc08_bits *)(&PTD))->bit1
  #define PTD2 ((struct __hc08_bits *)(&PTD))->bit2
  #define PTD3 ((struct __hc08_bits *)(&PTD))->bit3
  #define PTD4 ((struct __hc08_bits *)(&PTD))->bit4
  #define PTD5 ((struct __hc08_bits *)(&PTD))->bit5
  #define PTD6 ((struct __hc08_bits *)(&PTD))->bit6
  #define PTD7 ((struct __hc08_bits *)(&PTD))->bit7

_VOLDATA _UINT8 __at 0x04 DDRA;    /* Data Direction Register A */
  #define DDRA0 ((struct __hc08_bits *)(&DDRA))->bit0
  #define DDRA1 ((struct __hc08_bits *)(&DDRA))->bit1
  #define DDRA2 ((struct __hc08_bits *)(&DDRA))->bit2
  #define DDRA3 ((struct __hc08_bits *)(&DDRA))->bit3
  #define DDRA4 ((struct __hc08_bits *)(&DDRA))->bit4
  #define DDRA5 ((struct __hc08_bits *)(&DDRA))->bit5
  #define DDRA6 ((struct __hc08_bits *)(&DDRA))->bit6
  #define DDRA7 ((struct __hc08_bits *)(&DDRA))->bit7

_VOLDATA _UINT8 __at 0x05 DDRB;    /* Data Direction Register B */
  #define DDRB0 ((struct __hc08_bits *)(&DDRB))->bit0
  #define DDRB1 ((struct __hc08_bits *)(&DDRB))->bit1
  #define DDRB2 ((struct __hc08_bits *)(&DDRB))->bit2
  #define DDRB3 ((struct __hc08_bits *)(&DDRB))->bit3
  #define DDRB4 ((struct __hc08_bits *)(&DDRB))->bit4
  #define DDRB5 ((struct __hc08_bits *)(&DDRB))->bit5
  #define DDRB6 ((struct __hc08_bits *)(&DDRB))->bit6
  #define DDRB7 ((struct __hc08_bits *)(&DDRB))->bit7

_VOLDATA _UINT8 __at 0x06 DDRC;    /* Data Direction Register C */
  #define DDRC0 ((struct __hc08_bits *)(&DDRC))->bit0
  #define DDRC1 ((struct __hc08_bits *)(&DDRC))->bit1
  #define DDRC2 ((struct __hc08_bits *)(&DDRC))->bit2
  #define DDRC3 ((struct __hc08_bits *)(&DDRC))->bit3
  #define DDRC4 ((struct __hc08_bits *)(&DDRC))->bit4
  #define DDRC5 ((struct __hc08_bits *)(&DDRC))->bit5
  #define DDRC6 ((struct __hc08_bits *)(&DDRC))->bit6
  #define DDRC7 ((struct __hc08_bits *)(&DDRC))->bit7

_VOLDATA _UINT8 __at 0x07 DDRD;    /* Data Direction Register D */
  #define DDRD0 ((struct __hc08_bits *)(&DDRD))->bit0
  #define DDRD1 ((struct __hc08_bits *)(&DDRD))->bit1
  #define DDRD2 ((struct __hc08_bits *)(&DDRD))->bit2
  #define DDRD3 ((struct __hc08_bits *)(&DDRD))->bit3
  #define DDRD4 ((struct __hc08_bits *)(&DDRD))->bit4
  #define DDRD5 ((struct __hc08_bits *)(&DDRD))->bit5
  #define DDRD6 ((struct __hc08_bits *)(&DDRD))->bit6
  #define DDRD7 ((struct __hc08_bits *)(&DDRD))->bit7

_VOLDATA _UINT8 __at 0x08 PTE;     /* Port E Data Register */
  #define PTE0 ((struct __hc08_bits *)(&PTE))->bit0
  #define PTE1 ((struct __hc08_bits *)(&PTE))->bit1
  #define PTE2 ((struct __hc08_bits *)(&PTE))->bit2
  #define PTE3 ((struct __hc08_bits *)(&PTE))->bit3
  #define PTE4 ((struct __hc08_bits *)(&PTE))->bit4
  #define PTE5 ((struct __hc08_bits *)(&PTE))->bit5
  #define PTE6 ((struct __hc08_bits *)(&PTE))->bit6
  #define PTE7 ((struct __hc08_bits *)(&PTE))->bit7

_VOLDATA _UINT8 __at 0x09 DDRE;    /* Data Direction Register E */
  #define DDRE0 ((struct __hc08_bits *)(&DDRE))->bit0
  #define DDRE1 ((struct __hc08_bits *)(&DDRE))->bit1
  #define DDRE2 ((struct __hc08_bits *)(&DDRE))->bit2
  #define DDRE3 ((struct __hc08_bits *)(&DDRE))->bit3
  #define DDRE4 ((struct __hc08_bits *)(&DDRE))->bit4
  #define DDRE5 ((struct __hc08_bits *)(&DDRE))->bit5
  #define DDRE6 ((struct __hc08_bits *)(&DDRE))->bit6
  #define DDRE7 ((struct __hc08_bits *)(&DDRE))->bit7

_VOLDATA _UINT8 __at 0x0a TSC;     /* TIM Status and Control */
  #define PS0   ((struct __hc08_bits *)(&TSC))->bit0
  #define PS1   ((struct __hc08_bits *)(&TSC))->bit1
  #define PS2   ((struct __hc08_bits *)(&TSC))->bit2
  #define TRST  ((struct __hc08_bits *)(&TSC))->bit4
  #define TSTOP ((struct __hc08_bits *)(&TSC))->bit5
  #define TOIE  ((struct __hc08_bits *)(&TSC))->bit6
  #define TOF   ((struct __hc08_bits *)(&TSC))->bit7

_VOLDATA _UINT16 __at 0x0c TCNT;   /* TIM Counter High & Low Registers */
_VOLDATA _UINT8 __at 0x0c TCNTH;   /* TIM Counter Register High */
_VOLDATA _UINT8 __at 0x0d TCNTL;   /* TIM Counter Register Low */

_VOLDATA _UINT16 __at 0x0e TMOD;   /* TIM Counter Modulo High & Low Registers */
_VOLDATA _UINT8 __at 0x0e TMODH;   /* TIM Counter Modulo Register High */
_VOLDATA _UINT8 __at 0x0f TMODL;   /* TIM Counter Modulo Register Low */

_VOLDATA _UINT8 __at 0x10 TSC0;    /* TIM Channel 0 Status and Control Register */
  #define CH0MAX ((struct __hc08_bits *)(&TSC0))->bit0
  #define TOV0   ((struct __hc08_bits *)(&TSC0))->bit1
  #define ELS0A  ((struct __hc08_bits *)(&TSC0))->bit2
  #define ELS0B  ((struct __hc08_bits *)(&TSC0))->bit3
  #define MS0A   ((struct __hc08_bits *)(&TSC0))->bit4
  #define MS0B   ((struct __hc08_bits *)(&TSC0))->bit5
  #define CH0IE  ((struct __hc08_bits *)(&TSC0))->bit6
  #define CH0F   ((struct __hc08_bits *)(&TSC0))->bit7

_VOLDATA _UINT16 __at 0x11 TCH0;   /* TIM Channel 0 High & Low Registers */
_VOLDATA _UINT8 __at 0x11 TCH0H;   /* TIM Channel 0 Register High */
_VOLDATA _UINT8 __at 0x12 TCH0L;   /* TIM Channel 0 Register Low */

_VOLDATA _UINT8 __at 0x13 TSC1;    /* TIM Channel 1 Status and Control Register */
  #define CH1MAX ((struct __hc08_bits *)(&TSC1))->bit0
  #define TOV1   ((struct __hc08_bits *)(&TSC1))->bit1
  #define ELS1A  ((struct __hc08_bits *)(&TSC1))->bit2
  #define ELS1B  ((struct __hc08_bits *)(&TSC1))->bit3
  #define MS1A   ((struct __hc08_bits *)(&TSC1))->bit4
  #define CH1IE  ((struct __hc08_bits *)(&TSC1))->bit6
  #define CH1F   ((struct __hc08_bits *)(&TSC1))->bit7

_VOLDATA _UINT16 __at 0x14 TCH1;   /* TIM Channel 1 High & Low Registers */
_VOLDATA _UINT8 __at 0x14 TCH1H;   /* TIM Channel 1 Register High */
_VOLDATA _UINT8 __at 0x15 TCH1L;   /* TIM Channel 1 Register Low */

_VOLDATA _UINT8 __at 0x16 KBSCR;   /* Keyboard Status and Control Register */
  #define MODEK  ((struct __hc08_bits *)(&KBSCR))->bit0
  #define IMASKK ((struct __hc08_bits *)(&KBSCR))->bit1
  #define ACKK   ((struct __hc08_bits *)(&KBSCR))->bit2
  #define KEYF   ((struct __hc08_bits *)(&KBSCR))->bit3

_VOLDATA _UINT8 __at 0x17 KBIER;   /* Keyboard Interrupt Enable Register */
  #define KBIE0 ((struct __hc08_bits *)(&KBIER))->bit0
  #define KBIE1 ((struct __hc08_bits *)(&KBIER))->bit1
  #define KBIE2 ((struct __hc08_bits *)(&KBIER))->bit2
  #define KBIE3 ((struct __hc08_bits *)(&KBIER))->bit3
  #define KBIE4 ((struct __hc08_bits *)(&KBIER))->bit4
  #define KBIE5 ((struct __hc08_bits *)(&KBIER))->bit5
  #define KBIE6 ((struct __hc08_bits *)(&KBIER))->bit6
  #define KBIE7 ((struct __hc08_bits *)(&KBIER))->bit7

_VOLDATA _UINT8 __at 0x18 UIR2;   /* USB Interrupt Register 2 */
  #define RXD0FR  ((struct __hc08_bits *)(&UIR2))->bit0
  #define TXD0FR  ((struct __hc08_bits *)(&UIR2))->bit1
  #define RESUMFR ((struct __hc08_bits *)(&UIR2))->bit2
  #define TXD1FR  ((struct __hc08_bits *)(&UIR2))->bit3
  #define RXD2FR  ((struct __hc08_bits *)(&UIR2))->bit4
  #define TXD2FR  ((struct __hc08_bits *)(&UIR2))->bit5
  #define RSTFR   ((struct __hc08_bits *)(&UIR2))->bit6
  #define EOPFR   ((struct __hc08_bits *)(&UIR2))->bit7

_VOLDATA _UINT8 __at 0x19 UCR2;   /* USB Control Register 2 */
  #define TP2SIZ0 ((struct __hc08_bits *)(&UCR2))->bit0
  #define TP2SIZ1 ((struct __hc08_bits *)(&UCR2))->bit1
  #define TP2SIZ2 ((struct __hc08_bits *)(&UCR2))->bit2
  #define TP2SIZ3 ((struct __hc08_bits *)(&UCR2))->bit3
  #define RX2E    ((struct __hc08_bits *)(&UCR2))->bit4
  #define TX2E    ((struct __hc08_bits *)(&UCR2))->bit5
  #define STALL2  ((struct __hc08_bits *)(&UCR2))->bit6
  #define T2SEQ   ((struct __hc08_bits *)(&UCR2))->bit7

_VOLDATA _UINT8 __at 0x1a UCR3;   /* USB Control Register 3 */
  #define ENABLE1 ((struct __hc08_bits *)(&UCR3))->bit0
  #define ENABLE2 ((struct __hc08_bits *)(&UCR3))->bit1
  #define PULLEN  ((struct __hc08_bits *)(&UCR3))->bit2
  #define ISTALL0 ((struct __hc08_bits *)(&UCR3))->bit4
  #define OSTALL0 ((struct __hc08_bits *)(&UCR3))->bit5
  #define TX1STR  ((struct __hc08_bits *)(&UCR3))->bit6
  #define TX1ST   ((struct __hc08_bits *)(&UCR3))->bit7

_VOLDATA _UINT8 __at 0x1b UCR4;   /* USB Control Register 4 */
  #define FDM   ((struct __hc08_bits *)(&UCR4))->bit0
  #define FDP   ((struct __hc08_bits *)(&UCR4))->bit1
  #define FUSB0 ((struct __hc08_bits *)(&UCR4))->bit2

_VOLDATA _UINT8 __at 0x1c IOCR;   /* IRQ Option Control Register */
  #define IRQPD  ((struct __hc08_bits *)(&IOCR))->bit0
  #define PTE4IE ((struct __hc08_bits *)(&IOCR))->bit1
  #define PTE4IF ((struct __hc08_bits *)(&IOCR))->bit2

_VOLDATA _UINT8 __at 0x1d POCR;   /* Port Option Control Register */
 #define PAP     ((struct __hc08_bits *)(&POCR))->bit0
 #define PBP     ((struct __hc08_bits *)(&POCR))->bit1
 #define PCP     ((struct __hc08_bits *)(&POCR))->bit2
 #define PTE3P   ((struct __hc08_bits *)(&POCR))->bit3
 #define PTE4P   ((struct __hc08_bits *)(&POCR))->bit4
 #define PTDILDD ((struct __hc08_bits *)(&POCR))->bit5
 #define PTDLDD  ((struct __hc08_bits *)(&POCR))->bit6
 #define PTE20P  ((struct __hc08_bits *)(&POCR))->bit7

_VOLDATA _UINT8 __at 0x1e ISCR;   /* IRQ Status and Control Register */
  #define MODE  ((struct __hc08_bits *)(&ISCR))->bit0
  #define IMASK ((struct __hc08_bits *)(&ISCR))->bit1
  #define ACK   ((struct __hc08_bits *)(&ISCR))->bit2
  #define IRQF  ((struct __hc08_bits *)(&ISCR))->bit3

_VOLDATA _UINT8 __at 0x1f CONFIG; /* Configuration Register 1 */
/* CONFIG1 is one-time writeable, so can't use bitfields */

_VOLDATA _UINT8 __at 0x20 UE0D0;  /* USB Endpoint 0 Data Register 0 */
_VOLDATA _UINT8 __at 0x21 UE0D1;  /* USB Endpoint 0 Data Register 1 */
_VOLDATA _UINT8 __at 0x22 UE0D2;  /* USB Endpoint 0 Data Register 2 */
_VOLDATA _UINT8 __at 0x23 UE0D3;  /* USB Endpoint 0 Data Register 3 */
_VOLDATA _UINT8 __at 0x24 UE0D4;  /* USB Endpoint 0 Data Register 4 */
_VOLDATA _UINT8 __at 0x25 UE0D5;  /* USB Endpoint 0 Data Register 5 */
_VOLDATA _UINT8 __at 0x26 UE0D6;  /* USB Endpoint 0 Data Register 6 */
_VOLDATA _UINT8 __at 0x27 UE0D7;  /* USB Endpoint 0 Data Register 7 */

_VOLDATA _UINT8 __at 0x28 UE1D0;  /* USB Endpoint 1 Data Register 0 */
_VOLDATA _UINT8 __at 0x29 UE1D1;  /* USB Endpoint 1 Data Register 1 */
_VOLDATA _UINT8 __at 0x2a UE1D2;  /* USB Endpoint 1 Data Register 2 */
_VOLDATA _UINT8 __at 0x2b UE1D3;  /* USB Endpoint 1 Data Register 3 */
_VOLDATA _UINT8 __at 0x2c UE1D4;  /* USB Endpoint 1 Data Register 4 */
_VOLDATA _UINT8 __at 0x2d UE1D5;  /* USB Endpoint 1 Data Register 5 */
_VOLDATA _UINT8 __at 0x2e UE1D6;  /* USB Endpoint 1 Data Register 6 */
_VOLDATA _UINT8 __at 0x2f UE1D7;  /* USB Endpoint 1 Data Register 7 */

_VOLDATA _UINT8 __at 0x30 UE2D0;  /* USB Endpoint 2 Data Register 0 */
_VOLDATA _UINT8 __at 0x31 UE2D1;  /* USB Endpoint 2 Data Register 1 */
_VOLDATA _UINT8 __at 0x32 UE2D2;  /* USB Endpoint 2 Data Register 2 */
_VOLDATA _UINT8 __at 0x33 UE2D3;  /* USB Endpoint 2 Data Register 3 */
_VOLDATA _UINT8 __at 0x34 UE2D4;  /* USB Endpoint 2 Data Register 4 */
_VOLDATA _UINT8 __at 0x35 UE2D5;  /* USB Endpoint 2 Data Register 5 */
_VOLDATA _UINT8 __at 0x36 UE2D6;  /* USB Endpoint 2 Data Register 6 */
_VOLDATA _UINT8 __at 0x37 UE2D7;  /* USB Endpoint 2 Data Register 7 */

_VOLDATA _UINT8 __at 0x38 UADDR;  /* USB Address Register */
 #define USBEN ((struct __hc08_bits *)(&UADDR))->bit7

_VOLDATA _UINT8 __at 0x39 UIR0;   /* USB Interrupt Register 0 */
  #define RXD0IE  ((struct __hc08_bits *)(&UIR0))->bit0
  #define TXD0IE  ((struct __hc08_bits *)(&UIR0))->bit1
  #define TXD1IE  ((struct __hc08_bits *)(&UIR0))->bit3
  #define RXD2IE  ((struct __hc08_bits *)(&UIR0))->bit4
  #define TXD2IE  ((struct __hc08_bits *)(&UIR0))->bit5
  #define SUSPND  ((struct __hc08_bits *)(&UIR0))->bit6
  #define EOPIE   ((struct __hc08_bits *)(&UIR0))->bit7

_VOLDATA _UINT8 __at 0x3a UIR1;   /* USB Interrupt Register 1 */
  #define RXD0F   ((struct __hc08_bits *)(&UIR1))->bit0
  #define TXD0F   ((struct __hc08_bits *)(&UIR1))->bit1
  #define RESUMF  ((struct __hc08_bits *)(&UIR1))->bit2
  #define TXD1F   ((struct __hc08_bits *)(&UIR1))->bit3
  #define RXD2F   ((struct __hc08_bits *)(&UIR1))->bit4
  #define TXD2F   ((struct __hc08_bits *)(&UIR1))->bit5
  #define RSTF    ((struct __hc08_bits *)(&UIR1))->bit6
  #define EOPF    ((struct __hc08_bits *)(&UIR1))->bit7

_VOLDATA _UINT8 __at 0x3b UCR0;   /* USB Control Register 0 */
  #define TP0SIZ0 ((struct __hc08_bits *)(&UCR0))->bit0
  #define TP0SIZ1 ((struct __hc08_bits *)(&UCR0))->bit1
  #define TP0SIZ2 ((struct __hc08_bits *)(&UCR0))->bit2
  #define TP0SIZ3 ((struct __hc08_bits *)(&UCR0))->bit3
  #define RX0E    ((struct __hc08_bits *)(&UCR0))->bit4
  #define TX0E    ((struct __hc08_bits *)(&UCR0))->bit5
  #define T0SEQ   ((struct __hc08_bits *)(&UCR0))->bit7

_VOLDATA _UINT8 __at 0x3c UCR1;   /* USB Control Register 1 */
  #define TP1SIZ0 ((struct __hc08_bits *)(&UCR1))->bit0
  #define TP1SIZ1 ((struct __hc08_bits *)(&UCR1))->bit1
  #define TP1SIZ2 ((struct __hc08_bits *)(&UCR1))->bit2
  #define TP1SIZ3 ((struct __hc08_bits *)(&UCR1))->bit3
  #define FRESUM  ((struct __hc08_bits *)(&UCR1))->bit4
  #define TX1E    ((struct __hc08_bits *)(&UCR1))->bit5
  #define STALL1  ((struct __hc08_bits *)(&UCR1))->bit6
  #define T1SEQ   ((struct __hc08_bits *)(&UCR1))->bit7

_VOLDATA _UINT8 __at 0x3d USR0;   /* USB Status Register 0 */
  #define RP0SIZ0 ((struct __hc08_bits *)(&USR0))->bit0
  #define RP0SIZ1 ((struct __hc08_bits *)(&USR0))->bit1
  #define RP0SIZ2 ((struct __hc08_bits *)(&USR0))->bit2
  #define RP0SIZ3 ((struct __hc08_bits *)(&USR0))->bit3
  #define SETUP   ((struct __hc08_bits *)(&USR0))->bit6
  #define R0SEQ   ((struct __hc08_bits *)(&USR0))->bit7

_VOLDATA _UINT8 __at 0x3e USR1;   /* USB Status Register 1 */
  #define RP2SIZ0 ((struct __hc08_bits *)(&USR1))->bit0
  #define RP2SIZ1 ((struct __hc08_bits *)(&USR1))->bit1
  #define RP2SIZ2 ((struct __hc08_bits *)(&USR1))->bit2
  #define RP2SIZ3 ((struct __hc08_bits *)(&USR1))->bit3
  #define TXSTL   ((struct __hc08_bits *)(&USR1))->bit4
  #define TXNAK   ((struct __hc08_bits *)(&USR1))->bit5
  #define TXACK   ((struct __hc08_bits *)(&USR1))->bit6
  #define R2SEQ   ((struct __hc08_bits *)(&USR1))->bit7

_VOLXDATA _UINT8 __at 0xfe00 BSR;     /* Break Status Register */
  #define SBSW ((struct __hc08_bits *)(&BSR))->bit1

_VOLXDATA _UINT8 __at 0xfe01 RSR;    /* Reset Status Register */
  #define LVI  ((struct __hc08_bits *)(&RSR))->bit1
  #define USB  ((struct __hc08_bits *)(&RSR))->bit2
  #define ILAD ((struct __hc08_bits *)(&RSR))->bit3
  #define ILOP ((struct __hc08_bits *)(&RSR))->bit4
  #define COP  ((struct __hc08_bits *)(&RSR))->bit5
  #define PIN  ((struct __hc08_bits *)(&RSR))->bit6
  #define POR  ((struct __hc08_bits *)(&RSR))->bit7

_VOLXDATA _UINT8 __at 0xfe03 BFCR;    /* Break Flag Control Register */
  #define BFCE ((struct __hc08_bits *)(&BFCR))->bit7

_VOLXDATA _UINT8 __at 0xfe04 INT1;    /* Interrupt Status Register 1 */
  #define IF1 ((struct __hc08_bits *)(&INT1))->bit2
  #define IF2 ((struct __hc08_bits *)(&INT1))->bit3
  #define IF3 ((struct __hc08_bits *)(&INT1))->bit4
  #define IF4 ((struct __hc08_bits *)(&INT1))->bit5
  #define IF5 ((struct __hc08_bits *)(&INT1))->bit6
  #define IF6 ((struct __hc08_bits *)(&INT1))->bit7

_VOLXDATA _UINT8 __at 0xfe08 FLCR;    /* FLASH Control Register */
  #define PGM   ((struct __hc08_bits *)(&FLCR))->bit0
  #define ERASE ((struct __hc08_bits *)(&FLCR))->bit1
  #define MASS  ((struct __hc08_bits *)(&FLCR))->bit2
  #define HVEN  ((struct __hc08_bits *)(&FLCR))->bit3

_VOLXDATA _UINT8 __at 0xfe09 FLBPR;    /* FLASH Block Protect Register */
  #define BPR0 ((struct __hc08_bits *)(&FLBPR))->bit0
  #define BPR1 ((struct __hc08_bits *)(&FLBPR))->bit1
  #define BPR2 ((struct __hc08_bits *)(&FLBPR))->bit2
  #define BPR3 ((struct __hc08_bits *)(&FLBPR))->bit3
  #define BPR4 ((struct __hc08_bits *)(&FLBPR))->bit4
  #define BPR5 ((struct __hc08_bits *)(&FLBPR))->bit5
  #define BPR6 ((struct __hc08_bits *)(&FLBPR))->bit6
  #define BPR7 ((struct __hc08_bits *)(&FLBPR))->bit7

_VOLXDATA _UINT16 __at 0xfe0c BRK;    /* Break Address High & Low Registers */
_VOLXDATA _UINT8 __at 0xfe0c BRKH;    /* Break Address High Register */
_VOLXDATA _UINT8 __at 0xfe0d BRKL;    /* Break Address Low Register */

_VOLXDATA _UINT8 __at 0xfe0e BRKSCR;  /* Break Status and Control Register */
  #define BRKA ((struct __hc08_bits *)(&BRKSCR))->bit6
  #define BRKE ((struct __hc08_bits *)(&BRKSCR))->bit7

_VOLXDATA _UINT8 __at 0xffff COPCTL;  /* COP Control Register */

#endif
