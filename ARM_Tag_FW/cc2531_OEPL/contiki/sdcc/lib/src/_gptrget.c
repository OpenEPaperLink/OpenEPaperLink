/*-------------------------------------------------------------------------
   _gptrget.c - get value for a generic pointer

   Copyright (c) 1999, Sandeep Dutta . sandeep.dutta@usa.net

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

/* not all devices use P2 to page pdata memory, therefore __XPAGE was
   introduced. On some targets __XPAGE itself is a paged SFR so it is
   not safe for all platforms to set this. Furthermore some targets
   can be configured to behave differently for movx @dptr vs. movx @Ri
   (don't drive high byte of address bus for movx @Ri only) */
#define USE_PDATA_PAGING_REGISTER 0

__sbit __at (0xF7) B_7;
__sbit __at (0xF6) B_6;
__sbit __at (0xF5) B_5;

/* the  return value is expected to be in acc, and not in the standard
 * location dpl. Therefore we choose return type void here: */
#if defined DSDCC_MODEL_HUGE
void
_gptrget (char *gptr) __naked
{
/* This is the banked version with pointers up to 23 bits.
   B cannot be trashed */

    gptr; /* hush the compiler */

    __asm
    ;
    ;   depending on the pointer type acc. to SDCCsymt.h
    ;
        jb      _B_7,codeptr$        ; >0x80 code       ; 3
        jnb     _B_6,xdataptr$       ; <0x40 far        ; 3

        mov     dph,r0 ; save r0 independant of regbank ; 2
        mov     r0,dpl ; use only low order address     ; 2

        jb      _B_5,pdataptr$       ; >0x60 pdata      ; 3
    ;
    ;   Pointer to data space
    ;
        mov     a,@r0                                   ; 1
 dataptrrestore$:
        mov     r0,dph ; restore r0                     ; 2
        mov     dph,#0 ; restore dph                    ; 2
        ret                                             ; 1
    ;
    ;   pointer to external stack or pdata
    ;
 pdataptr$:
        movx    a,@r0                                   ; 1
        sjmp    dataptrrestore$                         ; 2
    ;
    ;   pointer to code area
    ;
 codeptr$:
    ; implementation for SiLabs C8051F12x
        mov     a,b                                     ; 2
        anl     a,#0x03                                 ; 2
        swap    a                                       ; 1
        push    _PSBANK                                 ; 2
        anl     _PSBANK,#0x0F                           ; 3
        orl     _PSBANK,a                               ; 2

        clr     a                                       ; 1
        movc    a,@a+dptr                               ; 1
        pop     _PSBANK                                 ; 2
        ret                                             ; 1
    ;
    ;   pointer to xternal data
    ;
 xdataptr$:
    ; implementation for xram a16-a21 tied to P3
        mov     _P3,b                                   ; 3

        movx    a,@dptr                                 ; 1
        ret                                             ; 1
                                                        ;===
                                                        ;44 bytes
     __endasm;
}

#elif defined DSDCC_MODEL_MEDIUM

void
_gptrget (char *gptr) __naked
{
/* This is the non-banked version with pointers up to 15 bits.
   Assumes B is free to be used */

    gptr; /* hush the compiler */

    __asm
    ;
    ;   depending on the pointer type acc. to SDCCsymt.h
    ;
        mov     b,dph                                   ; 3
        jb      _B_7,codeptr$        ; >0x80 code       ; 3
        jnb     _B_6,xdataptr$       ; <0x40 far        ; 3

        mov     b,r0   ; save r0 independant of regbank ; 2
        mov     r0,dpl ; use only low order address     ; 2

        jb      _B_5,pdataptr$       ; >0x60 pdata      ; 3
    ;
    ;   Pointer to data space
    ;
        mov     a,@r0                                   ; 1
        mov     r0,b   ; restore r0                     ; 2
        ret                                             ; 1
    ;
    ;   pointer to xternal stack or pdata
    ;
 pdataptr$:
        movx    a,@r0                                   ; 1
        mov     r0,b   ; restore r0                     ; 2
        ret                                             ; 1
    ;
    ;   pointer to code area, max 15 bits
    ;
 codeptr$:
    ; 0x8000 <= dptr <= 0xFFFF
    ; no need to AND dph and restore from B if hardware wraps code memory
        anl     dph,#0x7F                               ; 3
        clr     a                                       ; 1
        movc    a,@a+dptr                               ; 1
        mov     dph,b                                   ; 3
        ret                                             ; 1
    ;
    ;   pointer to xternal data, max 14 bits
    ;
 xdataptr$:
    ; 0 <= dptr <= 0x3FFF
        movx    a,@dptr                                 ; 1
        ret                                             ; 1
                                                        ;===
                                                        ;35 bytes
     __endasm;
}

#else

void
_gptrget (char *gptr) __naked
{
/* This is the new version with pointers up to 16 bits.
   B cannot be trashed */

    gptr; /* hush the compiler */

    __asm
    ;
    ;   depending on the pointer type acc. to SDCCsymt.h
    ;
        jb      _B_7,codeptr$        ; >0x80 code       ; 3
        jnb     _B_6,xdataptr$       ; <0x40 far        ; 3

        mov     dph,r0 ; save r0 independant of regbank ; 2
        mov     r0,dpl ; use only low order address     ; 2

        jb      _B_5,pdataptr$       ; >0x60 pdata      ; 3
    ;
    ;   Pointer to data space
    ;
        mov     a,@r0                                   ; 1
 dataptrrestore$:
        mov     r0,dph ; restore r0                     ; 2
        mov     dph,#0 ; restore dph                    ; 2
        ret                                             ; 1
    ;
    ;   pointer to xternal stack or pdata
    ;
 pdataptr$:
        movx    a,@r0                                   ; 1
        sjmp    dataptrrestore$                         ; 2
    ;
    ;   pointer to code area, max 16 bits
    ;
 codeptr$:
        clr     a                                       ; 1
        movc    a,@a+dptr                               ; 1
        ret                                             ; 1
    ;
    ;   pointer to xternal data, max 16 bits
    ;
 xdataptr$:
        movx    a,@dptr                                 ; 1
        ret                                             ; 1
                                                        ;===
                                                        ;27 bytes
     __endasm;
}

#endif

#ifdef __SDCC_ds390
/* the  return value is expected to be in acc/acc1, and not in the standard
 * location dpl/dph. Therefore we choose return type void here: */

void
_gptrgetWord (unsigned *gptr)
{
    gptr; /* hush the compiler */

    __asm
    ;
    ;   depending on the pointer type acc. to SDCCsymt.h
    ;
        jb      _B_7,00003$           ; >0x80 code
        jnb     _B_6,00002$           ; <0x40 far

        mov     dph,r0 ; save r0 independant of regbank
        mov     r0,dpl ; use only low order address

        jb      _B_5,00004$           ; >0x60 pdata
    ;
    ;   Pointer to data space
    ;
        mov     acc1,@r0
        inc     r0
        mov     a,@r0
        inc     dpl
        sjmp    00005$
    ;
    ;   pointer to xternal data
    ;
 00002$:
        movx    a,@dptr
        mov     acc1,a
        inc     dptr
        movx    a,@dptr
        sjmp    00006$
;
;   pointer to code area
;
 00003$:
        clr     a
        movc    a,@a+dptr
        mov     acc1,a
        clr     a
        inc     dptr
        movc    a,@a+dptr
        sjmp    00006$
;
;   pointer to xternal stack
;
 00004$:
        movx    a,@r0
        mov     acc1,a
        inc     r0
        movx    a,@r0
        inc     dpl
;
;   restore and return
;
 00005$:
        mov     r0,dph ; restore r0
        mov     dph,#0 ; restore dph
 00006$:
        xch     a,acc1
    __endasm;

}

#endif
