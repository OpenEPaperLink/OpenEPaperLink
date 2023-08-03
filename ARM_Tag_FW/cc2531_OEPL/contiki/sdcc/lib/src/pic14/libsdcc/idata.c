/*-------------------------------------------------------------------------
   idata.c - startup code evaluating gputils' cinit structure

   Copyright (C) 2007, Raphael Neider <rneider AT web.de>

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
 * This code fragment copies initialized data from ROM to their
 * assigned RAM locations. The required cinit structure is created
 * by gputils' linker and comprises initial values of all linked in
 * modules.
 */

/*
 * We call the user's main() after initialization is done.
 */
extern void main(void);

/*
 * Force generation of _cinit symbol.
 */
static char force_cinit = 0;

/*
 * This struct describes one initialized variable.
 */
typedef struct {
    unsigned src;   // source address of data in CODE space
    unsigned dst;   // destination address of values in DATA space
    unsigned size;  // number of bytes to copy from `src' to `dst'
} cinit_t;

/*
 * This structure provides the number and position of the above
 * structs. to initialize all variables in the .hex file.
 */
extern __code struct {
    unsigned	records;    // number of entries in this file
    cinit_t	entry[];    // intialization descriptor
} cinit;

/*
 * Iterate over all records and copy values from ROM to RAM.
 */
void
_sdcc_gsinit_startup(void)
{
    unsigned num, size;
    __code cinit_t *cptr;
    __code char *src;
    __data char *dst;

    num = cinit.records;
    cptr = &cinit.entry[0];

    // iterate over all cinit entries
    while (num--) {
	size = cptr->size;
	src = (__code char *) cptr->src;
	dst = (__data char *) cptr->dst;

	// copy data byte-wise from ROM to RAM
	while (size--) {
	    *dst = *src;
	    src++;
	    dst++;
	} // while

	// XXX: might need to clear the watchdog timer here...
	cptr++;
    } // while

    // call main after initialization
    __asm
        EXTERN _main
	PAGESEL _main
	GOTO _main
    __endasm;
}
