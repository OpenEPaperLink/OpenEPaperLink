/*-------------------------------------------------------------------------
   memcpyx.c

   Copyright (C) 2003, Kevin Vigor

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

#include <string.h>

// FIXME: can optimize even further on '400 with auto-increment/auto-toggle.

void __xdata * memcpyx (
	void __xdata * dst,
	void __xdata * src,
	int count
	) __naked
{
    /* Shut compiler up about unused parameters. */
    dst; src; count;
    
__asm
    ; Destination is in DPTR. Save it on stack so we can return it at end.
    
    push dpx
    push dph
    push dpl
    
    mov  dps, #0x1        ; Alternate DPTR.
    
    ; count  is in _memcpyx_PARM_3
    mov  dptr, #_memcpyx_PARM_3
    movx a, @dptr
    inc dptr
    mov r2, a
    movx a, @dptr
    mov r3, a

    ; src is in _memcpyx_PARM_2
    mov  dptr, #_memcpyx_PARM_2
    movx a, @dptr
    inc  dptr
    push acc
    movx a, @dptr
    inc  dptr
    push acc
    movx a, @dptr
    mov  dpx1, a
    pop  dph1
    pop  dpl1

    mov	dps, #0x21	; Current DPTR is alt DPTR, toggle after each op.
    
    ; src is in alt DPTR, dst is in normal DPTR, count is in r2/r3.
    
    ; If we have zero bytes to copy, quick out.
    mov	 a, r2
    orl  a, r3
    jz   _memcpy_done

    ; increment r3 if r2 != 0 (makes djnz end-of-loop sequence possible).
    inc r3
    cjne r2, #0x0, _memcpyx_loopTop
    dec r3

_memcpyx_loopTop:

    movx a, @dptr
    movx @dptr, a
    inc dptr
    inc dptr

    djnz r2, _memcpyx_loopTop
    djnz r3, _memcpyx_loopTop
    
_memcpy_done:

    mov dps, #0x0

    pop dpl
    pop dph
    pop dpx
    ret
__endasm;    
    
}
