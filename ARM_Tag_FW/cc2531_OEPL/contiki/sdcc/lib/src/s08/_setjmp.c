/*-------------------------------------------------------------------------
   setjmp.c - source file for ANSI routines setjmp & longjmp

   Copyright (C) 2016, Maarten Brock. sourceforge.brock AT dse.nl

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
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

#include <sdcc-lib.h>
#define __SDCC_HIDE_LONGJMP
#include <setjmp.h>

static void dummy (void) __naked
{
        __asm

;--------------------------------------------------------
; overlayable items in ram
;--------------------------------------------------------
        .area   OSEG    (PAG, OVR)
___setjmp_buf:
        .ds 2
        .area   OSEG    (PAG, OVR)
_longjmp_buf:
        .ds 2
;--------------------------------------------------------
; extended address mode data
;--------------------------------------------------------
        .area XSEG
_longjmp_PARM_2:
        .ds 2
;--------------------------------------------------------
; code
;--------------------------------------------------------
        .area CSEG    (CODE)
;------------------------------------------------------------
;Allocation info for local variables in function '__setjmp'
;------------------------------------------------------------
;buf                       Allocated with name '___setjmp_buf'
;------------------------------------------------------------
;_setjmp.c:205: int __setjmp (jmp_buf buf)
;       -----------------------------------------
;        function __setjmp
;       -----------------------------------------
;       Stack space usage: 1 bytes.
        .globl ___setjmp
___setjmp:
        stx	(___setjmp_buf + 0)		; msb(buf)
        sta	(___setjmp_buf + 1)		; lsb(buf)

        ; save stack pointer
        tsx
        pshh
        txa
        ldhx	(___setjmp_buf)
        sta	1,x
        pula
        sta	0,x

        ; save return address
        lda	1,s
        sta	2,x
        lda	2,s
        sta	3,x

        ; return 0
        clra
        tax
        rts

;------------------------------------------------------------
;Allocation info for local variables in function 'longjmp'
;------------------------------------------------------------
;rv                        Allocated with name '_longjmp_PARM_2'
;buf                       Allocated with name '_longjmp_buf'
;------------------------------------------------------------
;_setjmp.c:216: int longjmp (jmp_buf buf, int rv)
;       -----------------------------------------
;        function longjmp
;       -----------------------------------------
;       Stack space usage: 1 bytes.
        .globl _longjmp
        .globl _longjmp_PARM_2
_longjmp:
        stx	(_longjmp_buf + 0)		; msb(buf)
        sta	(_longjmp_buf + 1)		; lsb(buf)

        ; restore stack pointer
        ldhx	(_longjmp_buf)
        lda	0,x
        psha
        ldx	1,x
        pulh
        txs

        ; set return address
        ldhx	(_longjmp_buf)
        lda	2,x
        sta	1,s
        lda	3,x
        sta	2,s

;_setjmp.c:224: return rv ? rv : 1;
        ldx     (_longjmp_PARM_2 + 0)
        txa
        ora     (_longjmp_PARM_2 + 1)
        beq     0001$
        lda     (_longjmp_PARM_2 + 1)
        rts
0001$:
        lda     #0x01
        rts

        __endasm;
}
