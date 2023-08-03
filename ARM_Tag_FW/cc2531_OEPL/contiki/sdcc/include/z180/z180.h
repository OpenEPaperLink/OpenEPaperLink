/*-------------------------------------------------------------------------
   z180.h - definitions on the built in I/O ports for the Z180/HD64180
            for use with SDCC

   Copyright (C) 2003, Peter Townson <Peter.Townson AT tattsgroup.com>

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

#if !defined( __Z180_H__ )
    #define   __Z180_H__ 

#if !defined( Z180_IO_BASE )
    #define   Z180_IO_BASE  0  /* zero is the Reset default */
#endif

/* will want this to be the case by default (I think) */
#pragma portmode z180

static void _ENABLE_Z180_ASSEMBLER_(void) __naked { __asm .hd64 __endasm; }

/*
 *---------------------------------------------------------------------------
 *      Z180/HD64180 internal port addresses
 */
__sfr __at (Z180_IO_BASE+0x00) CNTLA0;  /* ASCI control register A channel 0 */
__sfr __at (Z180_IO_BASE+0x01) CNTLA1;  /* ASCI control register A channel 1 */
__sfr __at (Z180_IO_BASE+0x02) CNTLB0;  /* ASCI control register B channel 0 */
__sfr __at (Z180_IO_BASE+0x03) CNTLB1;  /* ASCI control register B channel 0 */
__sfr __at (Z180_IO_BASE+0x04) STAT0 ;  /* ASCI status register    channel 0 */
__sfr __at (Z180_IO_BASE+0x05) STAT1 ;  /* ASCI status register    channel 1 */
__sfr __at (Z180_IO_BASE+0x06) TDR0  ;  /* ASCI transmit data reg, channel 0 */
__sfr __at (Z180_IO_BASE+0x07) TDR1  ;  /* ASCI transmit data reg, channel 1 */
__sfr __at (Z180_IO_BASE+0x08) RDR0  ;  /* ASCI receive data reg,  channel 0 */
__sfr __at (Z180_IO_BASE+0x09) RDR1  ;  /* ASCI receive data reg,  channel 0 */
__sfr __at (Z180_IO_BASE+0x0A) CNTR  ;  /* CSI/0 control register */
__sfr __at (Z180_IO_BASE+0x0B) TRDR  ;  /* CSI/0 transmit/receive data reg */

__sfr __at (Z180_IO_BASE+0x0C) TMDR0L;  /* Timer data register,    channel 0L */
__sfr __at (Z180_IO_BASE+0x0D) TMDR0H;  /* Timer data register,    channel 0H */
__sfr __at (Z180_IO_BASE+0x0E) RLDR0L;  /* Timer reload register,  channel 0L */
__sfr __at (Z180_IO_BASE+0x0F) RLDR0H;  /* Timer reload register,  channel 0H */
__sfr __at (Z180_IO_BASE+0x10) TCR   ;  /* Timer control register */
__sfr __at (Z180_IO_BASE+0x14) TMDR1L;  /* Timer data register,    channel 1L */
__sfr __at (Z180_IO_BASE+0x15) TMDR1H;  /* Timer data register,    channel 1H */
__sfr __at (Z180_IO_BASE+0x16) RLDR1L;  /* Timer reload register,  channel 1L */
__sfr __at (Z180_IO_BASE+0x17) RLDR1H;  /* Timer reload register,  channel 1H */
__sfr __at (Z180_IO_BASE+0x18) FRC   ;  /* Timer Free running counter */

__sfr __at (Z180_IO_BASE+0x20) SAR0L ;  /* DMA source address reg, channel 0L */
__sfr __at (Z180_IO_BASE+0x21) SAR0H ;  /* DMA source address reg, channel 0H */
__sfr __at (Z180_IO_BASE+0x22) SAR0B ;  /* DMA source address reg, channel 0B */
__sfr __at (Z180_IO_BASE+0x23) DAR0L ;  /* DMA dest address reg,   channel 0L */
__sfr __at (Z180_IO_BASE+0x24) DAR0H ;  /* DMA dest address reg,   channel 0H */
__sfr __at (Z180_IO_BASE+0x25) DAR0B ;  /* DMA dest address reg,   channel 0B */
__sfr __at (Z180_IO_BASE+0x26) BCR0L ;  /* DMA byte count reg,     channel 0L */
__sfr __at (Z180_IO_BASE+0x27) BCR0H ;  /* DMA byte count reg,     channel 0H */
__sfr __at (Z180_IO_BASE+0x28) MAR1L ;  /* DMA memory address reg, channel 1L */
__sfr __at (Z180_IO_BASE+0x29) MAR1H ;  /* DMA memory address reg, channel 1H */
__sfr __at (Z180_IO_BASE+0x2A) MAR1B ;  /* DMA memory address reg, channel 1B */
__sfr __at (Z180_IO_BASE+0x2B) IAR1L ;  /* DMA I/O address reg,    channel 1L */
__sfr __at (Z180_IO_BASE+0x2C) IAR1H ;  /* DMA I/O address reg,    channel 1H */
__sfr __at (Z180_IO_BASE+0x2E) BCR1L ;  /* DMA byte count reg,     channel 1L */
__sfr __at (Z180_IO_BASE+0x2F) BCR1H ;  /* DMA byte count reg,     channel 1H */
__sfr __at (Z180_IO_BASE+0x30) DSTAT ;  /* DMA status register */
__sfr __at (Z180_IO_BASE+0x31) DMODE ;  /* DMA mode register */
__sfr __at (Z180_IO_BASE+0x32) DCNTL ;  /* DMA/WAIT control register */

__sfr __at (Z180_IO_BASE+0x33) IL    ;  /* Interrupt vector low register */
__sfr __at (Z180_IO_BASE+0x34) ITC   ;  /* INT/TRAP control register */

__sfr __at (Z180_IO_BASE+0x36) RCR   ;  /* Refresh control register */

__sfr __at (Z180_IO_BASE+0x38) CBR   ;  /* MMU common base register */
__sfr __at (Z180_IO_BASE+0x39) BBR   ;  /* MMU bank base register */
__sfr __at (Z180_IO_BASE+0x3A) CBAR  ;  /* MMU common/bank area register */

__sfr __at (Z180_IO_BASE+0x3E) OMCR  ;  /* Operation mode control register */

__sfr __at               0x3F  ICR   ;  /* I/O base control register - does not move */

/*
 *---------------------------------------------------------------------------
 *      Interrupt vectors (offsets) for Z180/HD64180 internal interrupts
 */
#define INT1_VECTOR     0x00    /* external /INT1 */
#define INT2_VECTOR     0x02    /* external /INT2 */
#define PRT0_VECTOR     0x04    /* PRT channel 0 */
#define PRT1_VECTOR     0x06    /* PRT channel 1 */
#define DMA0_VECTOR     0x08    /* DMA channel 0 */ /* ???? */
#define DMA1_VECTOR     0x0A    /* DMA Channel 1 */
#define CSIO_VECTOR     0x0C    /* Clocked serial I/O */
#define ASCI0_VECTOR    0x0E    /* Async channel 0 */
#define ASCI1_VECTOR    0x10    /* Async channel 1 */
#define INCAP_VECTOR    0x12    /* input capture */
#define OUTCMP_VECTOR   0x14    /* output compare */
#define TIMOV_VECTOR    0x16    /* timer overflow */
/*
 *---------------------------------------------------------------------------
 */
#endif  /* __Z180_H__ */
