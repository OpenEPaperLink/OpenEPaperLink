/*-------------------------------------------------------------------------
   signal.h - Signal handler header

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

#ifndef __SIGNAL_H__
#define __SIGNAL_H__

/* interrupt testing arguments */
#define SIG_RB		SIG_RBIF
#define SIG_INT0	SIG_INT0IF
#define SIG_INT1	SIG_INT1IF
#define SIG_INT2	SIG_INT2IF
#define SIG_CCP1	SIG_CCP1IF
#define SIG_CCP2	SIG_CCP2IF
#define SIG_TMR0	SIG_TMR0IF
#define SIG_TMR1	SIG_TMR1IF
#define SIG_TMR2	SIG_TMR2IF
#define SIG_TMR3	SIG_TMR3IF
#define SIG_EE		SIG_EEIF
#define SIG_BCOL	SIG_BCOLIF
#define SIG_LVD		SIG_LVDIF
#define SIG_PSP		SIG_PSPIF
#define SIG_AD		SIG_ADIF
#define SIG_RC		SIG_RCIF
#define SIG_TX		SIG_TXIF
#define SIG_SSP		SIG_SSPIF
#define SIG_MSSP	SIG_SSPIF	/* just an alias */
#define SIG_USB		SIG_USBIF

/* define name to be the interrupt handler for interrupt #vecno */
#define DEF_ABSVECTOR(vecno, name)                      \
void __ivt_ ## name(void) __interrupt(vecno) __naked    \
{                                                       \
  __asm goto _ ## name __endasm;                        \
}

/* Define name to be the handler for high priority interrupts,
 * use like this:
 *   DEF_INTHIGH(high_handler)
 *     DEF_HANDLER(SIG_TMR0, timer0_handler)
 *     DEF_HANDLER2(SIG_TMR1, SIG_TMR1IE, timer1_handler)
 *     ...
 *   END_DEF
 *
 *   SIGHANDLER(timer0_handler)
 *   {
 *     // code to handle timer0 interrupts
 *   }
 *   SIGHANDLER(timer1_handler)
 *   {
 *     // code to handle timer1 interrupts
 *   }
 */
#define DEF_INTHIGH(name)                \
DEF_ABSVECTOR(1, name)                   \
void name(void) __naked __interrupt      \
{
  
/* Define name to be the handler for high priority interrupts,
 * use like this:
 *   DEF_INTLOW(low_handler)
 *     DEF_HANDLER(SIG_RB, portb_handler)
 *     DEF_HANDLER2(SIG_LVD, SIG_LVDIE, lowvolt_handler)
 *     ...
 *   END_DEF
 *
 *   SIGHANDLER(portb_handler)
 *   {
 *     // code to handle PORTB change interrupts
 *   }
 *   SIGHANDLER(lowvolt_handler)
 *   {
 *     // code to handle low voltage interrupts
 *   }
 */
#define DEF_INTLOW(name)                 \
DEF_ABSVECTOR(2, name)                   \
void name(void) __naked __interrupt      \
{

/* finish an interrupt handler definition */
#define END_DEF                                 \
  __asm retfie __endasm;                        \
}

/* Declare handler to be the handler function for the given signal.
 * sig should be one of SIG_xxx from above, handler should be a
 * function defined using SIGHANDLER(handler) or
 * SIGHANDLERNAKED(handler).
 * ATTENTION: This macro ignores the signal's enable bit!
 *            Use DEF_HANDLER2(SIG_xxx, SIGxxxIE, handler) instead!
 * To be used together with DEF_INTHIGH and DEF_INTLOW.
 */
#define DEF_HANDLER(sig, handler)               \
    __asm btfsc sig, 0 __endasm;                \
    __asm goto  _ ## handler __endasm;

/* Declare handler to be the handler function for the given signal.
 * sig should be one of SIG_xxx from above,
 * sig2 should also be a signal (probably SIG_xxxIE from below) and
 * handler should be a function defined using SIGHANDLER(handler)
 * or SIGHANDLERNAKED(handler).
 * To be used together with DEF_INTHIGH and DEF_INTLOW.
 */
#define DEF_HANDLER2(sig1,sig2,handler)         \
    __asm btfss sig1, 0 __endasm;               \
    __asm bra   $+8 __endasm;                   \
    __asm btfsc sig2, 0 __endasm;               \
    __asm goto  _ ## handler __endasm;

/* Declare or define an interrupt handler function. */
#define SIGHANDLER(handler)             void handler (void) __interrupt
#define SIGHANDLERNAKED(handler)        void handler (void) __naked __interrupt


/*
 * inline assembly compatible bit definitions
 */
#define SIG_RBIF	_INTCON, 0
#define SIG_RBIE	_INTCON, 3
#define SIG_RBIP	_INTCON2, 0

#define SIG_INT0IF	_INTCON, 1
#define SIG_INT0IE	_INTCON, 4
/*#define SIG_INT0IP not selectable, always ? */

#define SIG_TMR0IF	_INTCON, 2
#define SIG_TMR0IE	_INTCON, 5
#define SIG_TMR0IP	_INTCON2, 2

#define SIG_INT1IF	_INTCON3, 0
#define SIG_INT1IE	_INTCON3, 3
#define SIG_INT1IP	_INTCON3, 6

#define SIG_INT2IF	_INTCON3, 1
#define SIG_INT2IE	_INTCON3, 4
#define SIG_INT2IP	_INTCON3, 7

/* device dependent -- should be moved to pic18f*.h */
#define SIG_TMR1IDX	0
#define SIG_TMR1SUF	1
#define SIG_TMR2IDX	1
#define SIG_TMR2SUF	1
#define SIG_CCP1IDX	2
#define SIG_CCP1SUF	1
#define SIG_SSPIDX	3
#define SIG_SSPSUF	1
#define SIG_TXIDX	4
#define SIG_TXSUF	1
#define SIG_RCIDX	5
#define SIG_RCSUF	1
#define SIG_ADIDX	6
#define SIG_ADSUF	1
#define SIG_PSPIDX	7
#define SIG_PSPSUF	1

#define SIG_CCP2IDX	0
#define SIG_CCP2SUF	2
#define SIG_TMR3IDX	1
#define SIG_TMR3SUF	2
#define SIG_LVDIDX	2
#define SIG_LVDSUF	2
#define SIG_BCOLIDX	3
#define SIG_BCOLSUF	2
#define SIG_EEIDX	4
#define SIG_EESUF	2
#define SIG_USBIDX	5
#define SIG_USBSUF	2

/* device independent */
#define __concat(a,b)   __concat2(a,b)
#define __concat2(a,b)  a ## b

#define SIG_PIR(suf)	__concat(_PIR,suf)
#define SIG_PIE(suf)	__concat(_PIE,suf)
#define SIG_IPR(suf)	__concat(_IPR,suf)

#define SIG_TMR1IF	SIG_PIR(SIG_TMR1SUF), SIG_TMR1IDX
#define SIG_TMR1IE	SIG_PIE(SIG_TMR1SUF), SIG_TMR1IDX
#define SIG_TMR1IP	SIG_IPR(SIG_TMR1SUF), SIG_TMR1IDX

#define SIG_TMR2IF	SIG_PIR(SIG_TMR2SUF), SIG_TMR2IDX
#define SIG_TMR2IE	SIG_PIE(SIG_TMR2SUF), SIG_TMR2IDX
#define SIG_TMR2IP	SIG_IPR(SIG_TMR2SUF), SIG_TMR2IDX

#define SIG_CCP1IF	SIG_PIR(SIG_CCP1SUF), SIG_CCP1IDX
#define SIG_CCP1IE	SIG_PIE(SIG_CCP1SUF), SIG_CCP1IDX
#define SIG_CCP1IP	SIG_IPR(SIG_CCP1SUF), SIG_CCP1IDX

#define SIG_SSPIF	SIG_PIR(SIG_SSPSUF), SIG_SSPIDX
#define SIG_SSPIE	SIG_PIE(SIG_SSPSUF), SIG_SSPIDX
#define SIG_SSPIP	SIG_IPR(SIG_SSPSUF), SIG_SSPIDX
/* aliases: MSSP */
#define SIG_MSSPIF	SIG_SSPIF	//SIG_PIR(SIG_SSPSUF), SIG_SSPIDX
#define SIG_MSSPIE	SIG_SSPIE	//SIG_PIE(SIG_SSPSUF), SIG_SSPIDX
#define SIG_MSSPIP	SIG_SSPIP	//SIG_IPR(SIG_SSPSUF), SIG_SSPIDX

#define SIG_TXIF	SIG_PIR(SIG_TXSUF), SIG_TXIDX
#define SIG_TXIE	SIG_PIE(SIG_TXSUF), SIG_TXIDX
#define SIG_TXIP	SIG_IPR(SIG_TXSUF), SIG_TXIDX

#define SIG_RCIF	SIG_PIR(SIG_RCSUF), SIG_RCIDX
#define SIG_RCIE	SIG_PIE(SIG_RCSUF), SIG_RCIDX
#define SIG_RCIP	SIG_IPR(SIG_RCSUF), SIG_RCIDX

#define SIG_ADIF	SIG_PIR(SIG_ADSUF), SIG_ADIDX
#define SIG_ADIE	SIG_PIE(SIG_ADSUF), SIG_ADIDX
#define SIG_ADIP	SIG_IPR(SIG_ADSUF), SIG_ADIDX

#define SIG_PSPIF	SIG_PIR(SIG_PSPSUF), SIG_PSPIDX
#define SIG_PSPIE	SIG_PIE(SIG_PSPSUF), SIG_PSPIDX
#define SIG_PSPIP	SIG_IPR(SIG_PSPSUF), SIG_PSPIDX

#define SIG_CCP2IF	SIG_PIR(SIG_CCP2SUF), SIG_CCP2IDX
#define SIG_CCP2IE	SIG_PIE(SIG_CCP2SUF), SIG_CCP2IDX
#define SIG_CCP2IP	SIG_IPR(SIG_CCP2SUF), SIG_CCP2IDX

#define SIG_TMR3IF	SIG_PIR(SIG_TMR3SUF), SIG_TMR3IDX
#define SIG_TMR3IE	SIG_PIE(SIG_TMR3SUF), SIG_TMR3IDX
#define SIG_TMR3IP	SIG_IPR(SIG_TMR3SUF), SIG_TMR3IDX

#define SIG_LVDIF	SIG_PIR(SIG_LVDSUF), SIG_LVDIDX
#define SIG_LVDIE	SIG_PIE(SIG_LVDSUF), SIG_LVDIDX
#define SIG_LVDIP	SIG_IPR(SIG_LVDSUF), SIG_LVDIDX

#define SIG_BCOLIF	SIG_PIR(SIG_BCOLSUF), SIG_BCOLIDX
#define SIG_BCOLIE	SIG_PIE(SIG_BCOLSUF), SIG_BCOLIDX
#define SIG_BCOLIP	SIG_IPR(SIG_BCOLSUF), SIG_BCOLIDX

#define SIG_EEIF	SIG_PIR(SIG_EESUF), SIG_EEIDX
#define SIG_EEIE	SIG_PIE(SIG_EESUF), SIG_EEIDX
#define SIG_EEIP	SIG_IPR(SIG_EESUF), SIG_EEIDX

#define SIG_USBIF	SIG_PIR(SIG_USBSUF), SIG_USBIDX
#define SIG_USBIE	SIG_PIE(SIG_USBSUF), SIG_USBIDX
#define SIG_USBIP	SIG_IPR(SIG_USBSUF), SIG_USBIDX

#endif  /* __SIGNAL_H__ */
