/*-------------------------------------------------------------------------
;  gptr_cmp.asm - C run-time: compare two generic pointers
;
;  Copyright (C) 2011, Maarten Brock
;
;  This library is free software; you can redistribute it and/or modify it
;  under the terms of the GNU General Public License as published by the
;  Free Software Foundation; either version 2, or (at your option) any
;  later version.
;
;  This library is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
;  GNU General Public License for more details.
;
;  You should have received a copy of the GNU General Public License 
;  along with this library; see the file COPYING. If not, write to the
;  Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
;   MA 02110-1301, USA.
;
;  As a special exception, if you link this library with other files,
;  some of which are compiled with SDCC, to produce an executable,
;  this library does not by itself cause the resulting executable to
;  be covered by the GNU General Public License. This exception does
;  not however invalidate any other reasons why the executable file
;  might be covered by the GNU General Public License.
;------------------------------------------------------------------------*/

#ifdef __SDCC_MODEL_FLAT24

static void dummy(void) __naked
{
	__asm
	.area HOME    (CODE)

; compares two generic pointers.
; if p1 < p2  return NZ and C
; if p1 == p2 return  Z and NC
; if p1 > p2  return NZ and NC
; if both are a NULL pointer (yes, we have several) also return Z and NC

___gptr_cmp::
    mov  a,sp
    add  a,#0xFA
    mov  dpl1,a
    mov  a,esp
    anl  a,#0x03
    addc a,#0xFF
    mov  dph1,a
    mov  dpx1,#0x40
    inc  dps        ;select dptr1
    movx a,@dptr
    mov  ap,a
    inc  dptr
    movx a,@dptr
    orl  ap,a
    inc  dptr
    movx a,@dptr
    orl  a,ap
    clr  c
    jnz  00001$
    mov  a,dpl
    orl  a,dph
    orl  a,dpx
; if both are NULL, return Z and NC
    jz   00002$
00001$:
    orl  dps,#0x80  ;decrement dptr1
    inc  dptr
    inc  dptr
    anl  dps,#0x7F  ;increment dptr1
    movx a,@dptr
    subb a,dpl
    mov  ap,a
    inc  dptr
    movx a,@dptr
    subb a,dph
    orl  ap,a
    inc  dptr
    movx a,@dptr
    subb a,dpx
    orl  ap,a
    inc  dptr
    movx a,@dptr
    subb a,b
    orl  a,ap
; p2 < p1, return NZ and C
; p2 = p1, return Z and NC
; p2 > p1, return NZ and NC
00002$:
    dec  dps        ;select dptr0
    ret

	__endasm;
}

#endif

