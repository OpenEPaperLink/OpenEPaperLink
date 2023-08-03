/*-------------------------------------------------------------------------
   _gptrgetc.c - get value for a generic pointer (used with --xram-movc)

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


/* the  return value is expected to be in acc, and not in the standard
 * location dpl. Therefore we choose return type void here: */

#if 1

void
_gptrgetc (char *gptr) __naked
{
/* This is the new version with pointers up to 16 bits.
   B cannot be trashed */

    gptr; /* hush the compiler */

    __asm
    ;   save values passed
    ;
    ;   depending on the pointer type acc. to SDCCsymt.h
    ;
    	jb		_B_7,codeptr$        ; >0x80 code		; 3
    	jnb		_B_6,xdataptr$       ; <0x40 far		; 3

        mov     dph,r0 ; save r0 independant of regbank	; 2
        mov     r0,dpl ; use only low order address		; 2

    	jb		_B_5,pdataptr$       ; >0x60 pdata		; 3
    ;
    ;   Pointer to data space
    ;
        mov     a,@r0									; 1
        mov     r0,dph ; restore r0						; 2
        mov     dph,#0 ; restore dph					; 2
        ret												; 1
    ;
    ;   pointer to external stack or pdata
    ;
 pdataptr$:
        movx    a,@r0									; 1
        mov     r0,dph ; restore r0						; 2
        mov     dph,#0 ; restore dph					; 2
        ret												; 1
;
;   pointer to xternal data
;   pointer to code area
;
 codeptr$:
 xdataptr$:
        clr     a										; 1
        movc    a,@a+dptr								; 1
        ret												; 1
        												;===
        												;28 bytes
     __endasm;
}

#else

void
_gptrgetc (char *gptr) __naked
{
    gptr; /* hush the compiler */

    __asm
    ;   save values passed
	xch    a,r0
	push   acc
    ;
    ;   depending on the pointer type acc. to SDCCsymt.h
    ;
        mov     a,b
        jz      00001$	; 0 near
	dec     a
	jz      00003$	; 1 far
        dec     a
        jz      00003$	; 2 code
	dec     a
	jz      00004$  ; 3 pdata
	dec     a	; 4 skip generic pointer
	dec     a
	jz      00001$	; 5 idata
    ;
    ;   any other value for type
    ;   return xFF
	mov     a,#0xff
	ret
    ;
    ;   Pointer to data space
    ;
 00001$:
	mov     r0,dpl     ; use only low order address
	mov     a,@r0
        ret
;
;   pointer to xternal data
;   pointer to code area
;
 00003$:
	; clr     a  is already 0
        movc    a,@a+dptr
        ret
;
;   pointer to xternal stack
;
 00004$:
	mov     r0,dpl
        movx    a,@r0
;
;   restore and return
;
        mov     r0,a
        pop     acc
        xch     a,r0
        ret
     __endasm;

}
#endif
