/*-------------------------------------------------------------------------
   crt0.c - SDCC pic16 port runtime start code

   Copyright (C) 2004, Vangelis Rokas <vrokas at otenet.gr>

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

/*
 * based on Microchip MPLAB-C18 startup files
 */

extern int stack_end;
extern int TBLPTRU;

/* external reference to the user's main routine */
extern void main (void);

void _entry (void) __naked __interrupt 0;
void _startup (void) __naked;

/* Access bank selector. */
#define a 0


/*
 * entry function, placed at interrupt vector 0 (RESET)
 */
void
_entry (void) __naked __interrupt 0
{
  __asm
    goto    __startup
  __endasm;
}

void
_startup (void) __naked
{
  __asm
    ; Initialize the stack pointer
    lfsr    1, _stack_end
    lfsr    2, _stack_end

    ; 1st silicon does not do this on POR
    clrf    _TBLPTRU, a

    ; Initialize the flash memory access configuration.
    ; This is harmless for non-flash devices, so we do it on all parts.
    bsf     0xa6, 7, a      ; EECON1.EEPGD = 1, TBLPTR accesses program memory
    bcf     0xa6, 6, a      ; EECON1.CFGS  = 0, TBLPTR accesses program memory
  __endasm;

  /* Call the main routine. */
  main ();

  __asm
lockup:
    ; Returning from main will lock up.
    bra     lockup
  __endasm;
}
