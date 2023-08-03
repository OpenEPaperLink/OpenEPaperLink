/*-------------------------------------------------------------------------
   eeprom16_gptrput1.c - write 1 byte value to EEPROM via a generic pointer

   Copyright (C) 2012 Raphael Neider <rneider AT web.de>

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

/* write address is expected to be in WREG:PRODL:FSR0L while
 * write value is in TBLPTRH:TBLPTRL:PRODH:[stack] */

extern int EEADR;
extern int EEADRH;
extern int EECON1;
extern int EEDATA;
extern int FSR0H;
extern int FSR0L;
extern int INTCON;
extern int PREINC1;
extern int PRODL;
extern int __eeprom16_write;

void
__eeprom16_gptrput1(void) __naked
{
    __asm
        MOVFF   _INTCON, _FSR0H     ; save previous interupt state
        BCF     _INTCON, 7, 0       ; GIE = 0: disable interrupts

        BCF     _EECON1, 7, 0       ; EEPGD = 0: access EEPROM, not program memory
        BCF     _EECON1, 6, 0       ; CFGS = 0: access EEPROM, not config words
        BSF     _EECON1, 2, 0       ; WREN = 1: enable write access

        MOVFF   _FSR0L, _EEADR      ; address first byte
        MOVFF   _PRODL, _EEADRH     ; high address bits

        MOVFF   _PREINC1, _EEDATA   ; load first byte
        CALL    ___eeprom16_write   ; write and address next byte

        BCF     _EECON1, 2, 0       ; WREN = 0: disable write access

        BTFSC   _FSR0H, 7, 0        ; check previous interrupt state
        BSF     _INTCON, 7, 0       ; conditionally re-enable interrupts

        RETURN
    __endasm;
}
