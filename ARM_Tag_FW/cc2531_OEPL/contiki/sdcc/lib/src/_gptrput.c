/*-------------------------------------------------------------------------
   _gptrput.c - put value for a generic pointer

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

#if defined DSDCC_MODEL_HUGE
void
_gptrput (char *gptr, char c) __naked
{
/* This is the banked version with pointers up to 22 bits.
   B cannot be trashed */

    gptr; c; /* hush the compiler */

    __asm
    ;
    ;   depending on the pointer type according to SDCCsymt.h
    ;
        jb      _B_7,codeptr$        ; >0x80 code       ; 3
        jnb     _B_6,xdataptr$       ; <0x40 far        ; 3

        mov     dph,r0 ; save r0 independant of regbank ; 2
        mov     r0,dpl ; use only low order address     ; 2

        jb      _B_5,pdataptr$       ; >0x60 pdata      ; 3
    ;
    ;   store into near/idata space
    ;
        mov     @r0,a                                   ; 1
 dataptrrestore$:
        mov     r0,dph ; restore r0                     ; 2
        mov     dph,#0 ; restore dph                    ; 2
        ret                                             ; 1
    ;
    ;   cannot store into code space, lock up
    ;
 codeptr$:
        sjmp    .                                       ; 2
    ;
    ;   store into external stack/pdata space
    ;
 pdataptr$:
        movx    @r0,a                                   ; 1
        sjmp    dataptrrestore$                         ; 2
    ;
    ;   store into far space
    ;
 xdataptr$:
        mov     _P3,b                                   ; 3

        movx    @dptr,a                                 ; 1
        ret                                             ; 1

                                                        ;===
                                                        ;29 bytes
    __endasm;
}

#elif defined DSDCC_MODEL_MEDIUM

void
_gptrput (char *gptr, char c) __naked
{
/* This is the non-banked version with pointers up to 14 bits.
   Assumes B is free to be used */

    gptr; c; /* hush the compiler */

    __asm
    ;
    ;   depending on the pointer type according to SDCCsymt.h
    ;
        mov     b,dph                                   ; 3
        jb      _B_7,codeptr$        ; >0x80 code       ; 3
        jnb     _B_6,xdataptr$       ; <0x40 far        ; 3

        mov     b,r0   ; save r0 independant of regbank ; 2
        mov     r0,dpl ; use only low order address     ; 2

        jb      _B_5,pdataptr$       ; >0x60 pdata      ; 3
    ;
    ;   store into near/idata space
    ;
        mov     @r0,a                                   ; 1
 dataptrrestore$:
        mov     r0,b   ; restore r0                     ; 2
        ret                                             ; 1
    ;
    ;   cannot store into code space, lock up
    ;
 codeptr$:
        sjmp    .                                       ; 2
    ;
    ;   store into external stack/pdata space
    ;
 pdataptr$:
        movx    @r0,a                                   ; 1
        sjmp    dataptrrestore$                         ; 2
    ;
    ;   store into far space, max 14 bits
    ;
 xdataptr$:
    ; 0 <= dptr <= 0x3FFF
        movx    @dptr,a                                 ; 1
        ret                                             ; 1
                                                        ;===
                                                        ;27 bytes
    __endasm;
}

#else

void
_gptrput (char *gptr, char c) __naked
{
/* This is the new version with pointers up to 16 bits.
   B cannot be trashed */

    gptr; c; /* hush the compiler */

    __asm
    ;
    ;   depending on the pointer type according to SDCCsymt.h
    ;
        jb      _B_7,codeptr$        ; >0x80 code       ; 3
        jnb     _B_6,xdataptr$       ; <0x40 far        ; 3

        mov     dph,r0 ; save r0 independant of regbank ; 2
        mov     r0,dpl ; use only low order address     ; 2

        jb      _B_5,pdataptr$       ; >0x60 pdata      ; 3
    ;
    ;   store into near/idata space
    ;
        mov     @r0,a                                   ; 1
 dataptrrestore$:
        mov     r0,dph ; restore r0                     ; 2
        mov     dph,#0 ; restore dph                    ; 2
        ret                                             ; 1
    ;
    ;   cannot store into code space, lock up
    ;
 codeptr$:
        sjmp    .                                       ; 2
    ;
    ;   store into external stack/pdata space
    ;
 pdataptr$:
        movx    @r0,a                                   ; 1
        sjmp    dataptrrestore$                         ; 2
    ;
    ;   store into far space
    ;
 xdataptr$:
        movx    @dptr,a                                 ; 1
        ret                                             ; 1

                                                        ;===
                                                        ;26 bytes
    __endasm;
}

#endif

#ifdef __SDCC_ds390

void
_gptrputWord (int *gptr, int w) __naked
{
/* The ds390 can write two bytes in one go.
   B cannot be trashed */

    gptr; w; /* hush the compiler */

    __asm
    ;
    ;   depending on the pointer type acc. to SDCCsymt.h
    ;
        jb      _B_7,codeptr_w$       ; >0x80 code
        jnb     _B_6,xdataptr_w$      ; <0x40 far

        mov     dph,r0 ; save r0 independant of regbank
        mov     r0,dpl ; use only low order address

        jb      _B_5,pdataptr_w$      ; >0x60 pdata
;
;       store into near space
;
        mov     @r0,acc1
        inc     r0
        mov     @r0,a
 dataptrrestore_w$:
        mov     dpl,r0
        mov     r0,dph ; restore r0
        mov     dph,#0 ; restore dph
        ret
    ;
    ;   cannot store into code space, lock up
    ;
 codeptr_w$:
        sjmp    .
;
;       store into xstack space
;
 pdataptr_w$:
        xch     a,acc1
        movx    @r0,a
        inc     r0
        xch     a,acc1
        movx    @r0, a
        sjmp    dataptrrestore_w$
;
;       store into far space
;
 xdataptr_w$:
        xch     a,acc1
        movx    @dptr,a
        inc     dptr
        xch     a,acc1
        movx    @dptr,a
        ret
    __endasm;
}

#endif
