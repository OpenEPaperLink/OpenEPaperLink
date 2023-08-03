/*-------------------------------------------------------------------------
   gptrput1.c - put 1 byte value at generic pointer

   Copyright (C) 1999, Sandeep Dutta . sandeep.dutta@usa.net
   Adopted for pic16 port by Vangelis Rokas, 2004 <vrokas AT otenet.gr>

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
 
extern int FSR0H;
extern int POSTINC0;
extern int PREINC1;
extern int PRODL;
extern int WREG;
extern int __eeprom_gptrput1;

void _gptrput1(void) __naked
{
  __asm
    /* decode generic pointer MSB (in WREG) bits 6 and 7:
     * 00 -> code (unimplemented)
     * 01 -> EEPROM
     * 10 -> data
     * 11 -> data
     *
     * address: (WREG, PRODL, FSR0L)
     * value: (TBLPTRH, TBLPTRL, PRODH, STACK1[+1])
     */
    btfss	_WREG, 7
    bra		_lab_01_
    
    /* data pointer  */
    /* FSR0L is already set up */
    movff	_PRODL, _FSR0H
    
    movff	_PREINC1, _POSTINC0
    
    return
    

_lab_01_:
    /* code or eeprom */
    btfsc	_WREG, 6
    goto        ___eeprom_gptrput1

    /* code pointer, cannot write code pointers */
    return

  __endasm;
}
