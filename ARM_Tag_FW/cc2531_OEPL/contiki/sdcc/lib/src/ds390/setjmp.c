/*-------------------------------------------------------------------------
   setjmp.c - source file for ANSI routines setjmp & longjmp

   Copyright (C) 1999, Sandeep Dutta . sandeep.dutta@usa.net

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

#include <ds80c390.h>
#include <sdcc-lib.h>
#define __SDCC_HIDE_LONGJMP
#include <setjmp.h>

int __setjmp (jmp_buf buf)
{
    unsigned char sp, esp;
    unsigned long lsp;

    /* registers would have been saved on the
       stack anyway so we need to save SP
       and the return address */
    __critical {
        sp = SP;
        esp = ESP;
    }
    lsp = sp;
    lsp |= (unsigned int)(esp << 8);
    lsp |= 0x400000;
    *buf++ = lsp;
    *buf++ = lsp >> 8;
    *buf++ = *((unsigned char __xdata *) lsp - 0);
    *buf++ = *((unsigned char __xdata *) lsp - 1);
    *buf++ = *((unsigned char __xdata *) lsp - 2);
    return 0;
}

int longjmp (jmp_buf buf, int rv)
{
    unsigned long lsp;

    lsp = *buf++;
    lsp |= (unsigned int)(*buf++ << 8);
    lsp |= 0x400000;
    *((unsigned char __xdata *) lsp - 0) = *buf++;
    *((unsigned char __xdata *) lsp - 1) = *buf++;
    *((unsigned char __xdata *) lsp - 2) = *buf++;
    __critical {
        SP = lsp;
        ESP = lsp >> 8;
    }
    return rv ? rv : 1;
}
