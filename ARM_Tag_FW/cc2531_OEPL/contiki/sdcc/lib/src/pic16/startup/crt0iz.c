/*-------------------------------------------------------------------------
   crt0iz.c - SDCC pic16 port runtime start code with initialisation and
              clear RAM

   Copyright (C) 2004, Vangelis Rokas <vrokas at otenet.gr>
   Copyright (C) 2012, Molnár Károly <molnarkaroly@users.sf.net>

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
-------------------------------------------------------------------------

  $Id: crt0iz.c 9317 2015-09-13 08:19:01Z spth $
*/

/*
 * based on Microchip MPLAB-C18 startup files
 */

extern int stack_end;
extern int TBLPTRU;
extern int TBLPTRH;
extern int TBLPTRL;
extern int FSR0L;
extern int FSR0H;
extern int TABLAT;
extern int POSTINC0;
extern int POSTDEC0;

#if 1
/* Global variable for forcing gplink to add _cinit section. */
char __uflags = 0;
#endif

/* External reference to the user's main routine. */
extern void main (void);

void _entry (void) __naked __interrupt 0;
void _startup (void) __naked;

/* Access bank selector. */
#define a 0


/*
 * Entry function, placed at interrupt vector 0 (RESET).
 */
void
_entry (void) __naked __interrupt 0
{
  __asm
    goto    __startup
  __endasm;
}

/* The cinit table will be filled by the linker. */
extern __code struct
  {
    unsigned short num_init;
    struct
      {
        unsigned long from;
        unsigned long to;
        unsigned long size;
      } entries[1];
  } cinit;

#define TBLRDPOSTINC	tblrd*+

#define src_ptr		0x00		/* 0x00 0x01 0x02*/
#define byte_count	0x03		/* 0x03 0x04 */
#define entry_count	0x05		/* 0x05 0x06 */
#define entry_ptr	0x07		/* 0x07 0x08 0x09 */

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

  /* cleanup the RAM */
    ; Load FSR0 with top of RAM.
    setf    _FSR0L, a
    movlw   0x0e
    movwf   _FSR0H, a

    ; Place 0xff at address 0x00 as a sentinel.
    setf    0x00, a

clear_loop:
    clrf    _POSTDEC0, a
    movf    0x00, w, a
    bnz     clear_loop

  /* Initialize global and/or static variables. */

  /*
   * Access registers 0x00 - 0x09 are not saved in this code.
   */
    ; TBLPTR = &cinit
    movlw   low(_cinit)
    movwf   _TBLPTRL, a
    movlw   high(_cinit)
    movwf   _TBLPTRH, a
    movlw   upper(_cinit)
    movwf   _TBLPTRU, a

    ; entry_count = cinit.num_init
    TBLRDPOSTINC
    movff   _TABLAT, entry_count

    TBLRDPOSTINC
    movff   _TABLAT, (entry_count + 1)

    ; while (entry_count)

    bra	    entry_loop_dec

entry_loop:

    ; Count down so we only have to look up the data in _cinit once.

    ; At this point we know that TBLPTR points to the top of the current
    ; entry in _cinit, so we can just start reading the from, to, and
    ; size values.

    ; Read the source address low.

    ; src_ptr = entry_ptr->from; 

    TBLRDPOSTINC
    movff   _TABLAT, src_ptr

    ; source address high
    TBLRDPOSTINC
    movff   _TABLAT, (src_ptr + 1)

    ; source address upper
    TBLRDPOSTINC
    movff   _TABLAT, (src_ptr + 2)

    ; Skip a byte since it is stored as a 32bit int.
    TBLRDPOSTINC

    ; Read the destination address directly into FSR0
    ; destination address low.

    ; FSR0 = (unsigned short)entry_ptr->to; 

    TBLRDPOSTINC
    movff   _TABLAT, _FSR0L

    ; destination address high
    TBLRDPOSTINC
    movff   _TABLAT, _FSR0H

    ; Skip two bytes since it is stored as a 32bit int.
    TBLRDPOSTINC
    TBLRDPOSTINC

    ; Read the size of data to transfer to destination address.

    ; byte_count = (unsigned short)entry_ptr->size; 

    TBLRDPOSTINC
    movff   _TABLAT, byte_count

    TBLRDPOSTINC
    movff   _TABLAT, (byte_count + 1)

    ; Skip two bytes since it is stored as a 32bit int.
    TBLRDPOSTINC
    TBLRDPOSTINC

    ; src_ptr = entry_ptr->from;
    ; FSR0 = (unsigned short)entry_ptr->to;
    ; byte_count = (unsigned short)entry_ptr->size;

    ; The table pointer now points to the next entry. Save it
    ; off since we will be using the table pointer to do the copying
    ; for the entry.

    ; entry_ptr = TBLPTR
    movff   _TBLPTRL, entry_ptr
    movff   _TBLPTRH, (entry_ptr + 1)
    movff   _TBLPTRU, (entry_ptr + 2)

    ; Now assign the source address to the table pointer.
    ; TBLPTR = src_ptr
    movff   src_ptr, _TBLPTRL
    movff   (src_ptr + 1), _TBLPTRH
    movff   (src_ptr + 2), _TBLPTRU
    bra	    copy_loop_dec

copy_loop:
    TBLRDPOSTINC
    movff   _TABLAT, _POSTINC0

copy_loop_dec:
    ; while (--byte_count);

    ; Decrement and test the byte counter.
    ; The cycle ends when the value of counter reaches the -1.
    decf    byte_count, f, a
    bc      copy_loop
    decf    (byte_count + 1), f, a
    bc      copy_loop

    ; Restore the table pointer for the next entry.
    ; TBLPTR = entry_ptr
    movff   entry_ptr, _TBLPTRL
    movff   (entry_ptr + 1), _TBLPTRH
    movff   (entry_ptr + 2), _TBLPTRU

entry_loop_dec:
    ; while (--entry_count);

    ; Decrement and test the entry counter.
    ; The cycle ends when the value of counter reaches the -1.
    decf    entry_count, f, a
    bc      entry_loop
    decf    (entry_count + 1), f, a
    bc      entry_loop
  __endasm;

  /* Call the main routine. */
  main ();

  __asm
lockup:
    ; Returning from main will lock up.
    bra     lockup
  __endasm;
}
