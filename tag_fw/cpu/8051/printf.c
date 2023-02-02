#include "printf.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#include "board.h"
#include "epd.h"
#include "uart.h"
#include "zbs243.h"

typedef void (*StrFormatOutputFunc)(uint32_t param /* low byte is data, bits 24..31 is char */) __reentrant;

static __idata __at(0x00) unsigned char R0;
static __idata __at(0x01) unsigned char R1;
static __idata __at(0x02) unsigned char R2;
static __idata __at(0x03) unsigned char R3;
static __idata __at(0x04) unsigned char R4;
static __idata __at(0x05) unsigned char R5;
static __idata __at(0x06) unsigned char R6;
static __idata __at(0x07) unsigned char R7;

static uint8_t __xdata mCvtBuf[18];

// callback must be reentrant and callee_saves
#pragma callee_saves prvPrintFormat
void prvPrintFormat(StrFormatOutputFunc formatF, uint16_t formatD, const char __code *fmt, va_list vl) __reentrant __naked {
    // formatF is in DPTR
    // sp[0..-1] is return addr
    // sp[-2..-3] is formatD
    // sp[-4..-5] is fmt
    // sp[-6] is vl

    __asm__(
        "	push  _R7						\n"
        "	push  DPH						\n"  // push formatF
        "	push  DPL						\n"
        "	mov   _R7, sp					\n"  // save place on stack where we stashed it so we can call it easily
        "	push  _R4						\n"
        "	push  _R3						\n"
        "	push  _R2						\n"
        "	push  _R1						\n"
        "	push  _R0						\n"

        "	mov   A, #-12					\n"
        "	add   A, sp						\n"
        "	mov   R0, A						\n"
        // R0 now points to pushed params, for large values, we see high bytes first
        //  to get next byte, we need to DECEREMENT R0

        "	mov   DPH, @R0					\n"
        "	dec   R0						\n"
        "	mov   DPL, @R0					\n"
        "	dec   R0						\n"
        "	mov   _R0, @R0					\n"
        "	dec   R0						\n"

        // now format string is in DPTR, and R0 points to the top byte of whatever was in the first param

        // main loop: get a byte of the format string
        "00001$:							\n"
        "	clr   A							\n"
        "	movc  A, @A + DPTR				\n"
        "	inc   DPTR						\n"
        // if zero, we're done
        "	jz    00098$					\n"
        // if not '%', print it
        "	cjne  A, #'%', 00097$			\n"

        // we got a percent sign - init state for format processing
        "	mov   R4, #0					\n"  // bit flags:
                                                                     //	0x01 = '*' = pointer provided instead of value (integers only)
                                                                     //	0x02 = '0' = zero-pad (for numbers only)
                                                                     //	0x04       = have pad-to length
                                                                     //	0x08       = long
                                                                     //   0x10       = long long
                                                                     //	0x20       = signed print requested. also: need to print a negative (used to reuse hex printing for decimal once converted to bcd)
        "	mov   R2, #0					\n"  // padLen

        // loop for format string ingestion
        "00002$:							\n"
        "	clr   A							\n"
        "	movc  A, @A + DPTR				\n"
        "	inc   DPTR						\n"
        // if zero, we're done
        "	jz    00098$					\n"
        // check for percent sign
        "	cjne  A, #'%', 00003$			\n"
        // fallthrough to print it and go read next non-format byte
        // print a char in A, go read next format byte
        "00097$:							\n"
        "	lcall 00060$					\n"
        "	sjmp  00001$					\n"

        // exit label - placed for easy jumping to
        "00098$:							\n"
        "	pop   _R0						\n"
        "	pop   _R1						\n"
        "	pop   _R2						\n"
        "	pop   _R3						\n"
        "	pop   _R4						\n"
        "	pop   DPL						\n"
        "	pop   DPH						\n"
        "	pop   _R7						\n"
        "	ret								\n"

        // continue to process format string - handle %c
        "00003$:							\n"
        "	cjne  A, #'c', 00004$			\n"
        "	dec   R0						\n"  // param is pushed as int (16 bits)
        "	mov   A, @R0					\n"
        "	dec   R0						\n"
        "	sjmp  00097$					\n"  // print and go read next non-format byte

        // continue to process format string - handle %m
        "00004$:							\n"
        "	mov   R3, A						\n"
        "	orl   A, #0x20					\n"
        "	cjne  A, #'m', 00008$			\n"

        // sort out which hexch charset to use
        "	mov   A, R3						\n"
        "	anl   A, #0x20					\n"
        "	rr    A							\n"
        "	mov   R1, A						\n"

        // go, do
        "	push  DPH						\n"
        "	push  DPL						\n"
        "	lcall 00090$					\n"  // read the short (__xdata) pointer - >DPTR
        "	mov   R4, #8					\n"  // byteSel
        "00005$:							\n"
        "	push  DPH						\n"
        "	push  DPL						\n"
        "	mov   A, R4						\n"
        "	dec   A							\n"
        "	add   A, DPL					\n"
        "	mov   DPL, A					\n"
        "	mov   A, DPH					\n"
        "	addc  A, #0						\n"
        "	mov   DPH, A					\n"
        "	movx  A, @DPTR					\n"
        "	mov   R2, A						\n"
        "	swap  A							\n"
        "	mov   R3, #2					\n"
        "00006$:							\n"
        "	anl   A, #0x0f					\n"
        "	add   A, R1						\n"
        "	mov   DPTR, #00099$				\n"
        "	movc  A, @A + DPTR				\n"
        "	lcall 00060$					\n"
        "	mov   A, R2						\n"
        "	djnz  R3, 00006$				\n"
        "	pop   DPL						\n"
        "	pop   DPH						\n"
        "	djnz  R4, 00007$				\n"
        // done with mac addr

        "00055$:							\n"
        "	pop   DPL						\n"
        "	pop   DPH						\n"
        "	sjmp  00001$					\n"
        // print colon and contimue mac addr printing
        "00007$:							\n"
        "	mov   A, #':'					\n"
        "	lcall 00060$					\n"
        "	sjmp  00005$					\n"

        // continue to process format string - handle '*'
        "00008$:							\n"
        "	mov   A, R3						\n"
        "	cjne  A, #'*', 00009$			\n"
        "	cjne  R2, #0, 00097$			\n"          // only valid when no length/padding has been specified yet, else invalid specifier
        "	mov   A, #0x01					\n"  //"pointer mode"
        "00010$:							\n"
        "	orl   A, R4						\n"
        "	mov   R4, A						\n"
        "	sjmp  00002$					\n"  // get next format specifier now

        // continue to process format string - handle '0'
        "00009$:							\n"
        "	cjne  A, #'0', 00011$			\n"
        "	cjne  R2, #0, 00011$			\n"          // setting "zero pad" is only valid when pad length is zero
        "	mov   A, #0x06					\n"  //"have pad length" | "zero-pad"
        "	sjmp  00010$					\n"  // orr A into R4, get next format specifier now

        // continue to process format string - handle '1'...'9'
        "00011$:							\n"
        "	mov   R3, A						\n"
        "	add   A, #-'0'					\n"
        "	jnc   00012$					\n"  // now 0..9 are valid
        "	add   A, #-10					\n"
        "	jc    00012$					\n"
        "	add   A, #10					\n"  // get it back into 1..9 range
        "	mov   R3, A						\n"
        "	mov   A, #10					\n"
        "	mov   B, R2						\n"
        "	mul   AB						\n"
        "	add   A, R3						\n"
        "	mov   R2, A						\n"
        "	mov   A, #0x04					\n"  //"have pad length"
        "	sjmp  00010$					\n"  // orr A into R4, get next format specifier now

        // continue to process format string - handle 'l'
        "00012$:							\n"
        "	cjne  R3, #'l', 00014$			\n"
        "	mov   A, R4						\n"
        "	anl   A, #0x08					\n"
        "	jz    00013$					\n"  // no "long" yet? set that
        // have long - set long log
        "	mov   A, #0x10					\n"  //"long long"
        "	sjmp  00010$					\n"  // orr A into R4, get next format specifier now
        // first 'l' - set long
        "00013$:							\n"
        "	mov   A, #0x08					\n"  //"long"
        "	sjmp  00010$					\n"  // orr A into R4, get next format specifier now

        // continue to process format string - handle 's'
        "00014$:							\n"
        "	cjne  R3, #'s', 00025$			\n"
        "	mov   A, R4						\n"
        "	anl   A, #0x08					\n"
        "	push  DPH						\n"
        "	push  DPL						\n"
        "	jnz   00015$					\n"
        "	lcall 00091$					\n"  // get and resolve generic pointer into DPTR
        "	sjmp  00016$					\n"
        "00015$:							\n"  // get short pointer into DPTR, record that it is to XRAM
        "	clr   PSW.5						\n"
        "	clr   PSW.1						\n"
        "	lcall 00090$					\n"
        "00016$:							\n"  // pointer to string now in DPTR
        // we have the string pointer in {DPTR,PSW}, let's see if we have padding to do
        "	mov   A, R4						\n"
        "	anl   A, #0x04					\n"
        "	jnz   00018$					\n"
        // print string with no length restrictions
        "00017$:							\n"
        "	lcall 00095$					\n"
        "	jz    00055$					\n"
        "	lcall 00060$					\n"
        "	sjmp  00017$					\n"

        // print string with length restrictions and/or padding
        "00018$:							\n"
        "	cjne  R2, #0, 00019$			\n"  // verify reqested len was not zero
        "	sjmp  00055$					\n"

        "00019$:							\n"
        "	lcall 00095$					\n"
        "	jz    00020$					\n"
        "	lcall 00060$					\n"
        "	djnz  R2, 00019$				\n"
        // we get here if we ran out of allowable bytes - we're done then
        "	ljmp  00055$					\n"

        // just a trampoline for range issues
        "00035$:							\n"
        "	ljmp  00036$					\n"

        // we need to pad with spaces
        "00020$:							\n"
        "	mov   A, #' '					\n"
        "	lcall 00060$					\n"
        "	djnz  R2, 00020$				\n"
        "	ljmp  00055$					\n"

        // continue to process format string - handle 'x'/'X'
        "00025$:							\n"
        "	mov   A, R3						\n"
        "	orl   A, #0x20					\n"
        "	cjne  A, #'x', 00035$			\n"
        "	push  DPH						\n"
        "	push  DPL						\n"
        "	lcall 00080$					\n"  // get pointer to the number in DPTR, length in bytes in B
        // save it

        "00070$:							\n"
        "	push  DPH						\n"
        "	push  DPL						\n"

        // sort out how long it would be if printed, first get a pointer to the highest
        "	mov   A, B						\n"
        "	rl    A							\n"
        "	mov   R1, A						\n"
        "	rr    A							\n"
        "	add   A, #0xff					\n"
        "	add   A, DPL					\n"
        "	mov   DPL, A					\n"
        "	mov   A, DPH					\n"
        "	addc  A, #0x00					\n"
        "	mov   DPH, A					\n"
        "00026$:							\n"
        "	lcall 00079$					\n"
        "	anl   A, #0xf0					\n"
        "	jnz   00028$					\n"
        "	dec   R1						\n"
        "	lcall 00079$					\n"
        "	jnz   00028$					\n"
        "	dec   R1						\n"
        // dec DPTR
        "	dec   DPL						\n"
        "	mov   A, DPL					\n"
        "	cjne  A, #0xff, 00027$			\n"
        "	dec   DPH						\n"
        "00027$:							\n"
        "	djnz  B, 00026$					\n"

        // we now know how many digits the number is (in R1), except that it has "0" if the number if zero, we cannot have that
        "00028$:							\n"
        "	cjne  R1, #0, 00029$			\n"
        "	inc   R1						\n"
        "00029$:							\n"  // we now finally have the full length of the digits

        // if the number is negative (happens when we're printing decimals)
        //  the length of it is one more, also in case of zero-padding, we need to print the minus sign here now
        "	mov   A, R4						\n"
        "	anl   A, #0x20					\n"
        "	jz    00051$					\n"
        "	inc   R1						\n"  // the length is one more
        "	mov   A, R4						\n"
        "	anl   A, #02					\n"  // if zero-padding, the negative comes now
        "	jz    00051$					\n"
        "	mov   A, #'-'					\n"
        "	lcall 00060$					\n"
        "00051$:							\n"

        // sort out if we need padding at all and if there is space
        "	mov   A, R4						\n"
        "	anl   A, #0x04					\n"
        "	jz    00031$					\n"  // no padding requested
        // padding was requested len is in R2
        "	mov   A, R2						\n"
        "	clr   C							\n"
        "	subb  A, R1						\n"
        "	jc    00031$					\n"  // pad-to len < number_len -> no padding needed
        "	jz    00031$					\n"  // pad-to len == number_len -> no padding needed
        "	mov   R2, A						\n"

        // sort out which character to use -> DPL
        "	mov   A, R4						\n"  // fancy way to create space/zero as needed
        "	anl   A, #0x02					\n"
        "	swap  A							\n"
        "	rr    A							\n"
        "	add   A, #0x20					\n"
        "	mov   DPL, A					\n"

        // pad!
        "00030$:							\n"
        "	mov   A, DPL					\n"
        "	lcall 00060$					\n"
        "	djnz  R2, 00030$				\n"
        "00031$:							\n"

        // if the number is negative (happens when we're printing decimals)
        //  we made the length of it is one more, which we need to undo
        //  also in case of space-padding, we need to print the minus sign here now
        "	mov   A, R4						\n"
        "	anl   A, #0x20					\n"
        "	jz    00052$					\n"
        "	dec   R1						\n"  // the length is one less than we had increased it to
        "	mov   A, R4						\n"
        "	anl   A, #02					\n"  // if space-padding, the negative comes now
        "	jnz   00052$					\n"
        "	mov   A, #'-'					\n"
        "	lcall 00060$					\n"
        "00052$:							\n"

        // time to print the number itself
        // sort out which hexch charset to use -> R2
        "	mov   A, R3						\n"
        "	anl   A, #0x20					\n"
        "	rr    A							\n"
        "	mov   R2, A						\n"
        // re-get the number pointer
        "	pop   DPL						\n"
        "	pop   DPH						\n"
        // currently DPTR points to the number low byte, R1 is now many digits we expect to print, R2 is the charset selection, R4 and R3 are free
        // let's calculate how many bytes we expect to process -> R4
        "	mov   A, R1						\n"
        "	inc   A							\n"
        "	clr   C							\n"
        "	rrc   A							\n"
        "	mov   R4, A						\n"
        // let's repoint DPTR  to the first byte we'll print in (remember we print 2 digits per byte)
        "	dec   A							\n"
        "	add   A, DPL					\n"
        "	mov   DPL, A					\n"
        "	mov   A, DPH					\n"
        "	addc  A, #0x00					\n"
        "	mov   DPH, A					\n"

        // decide if we need to print just a nibble of the high byte or the whole thing. Free up R1
        "	mov   A, R1						\n"
        "	anl   A, #0x01					\n"
        "	jz    00032$					\n"

        // we're printing just the low nibble of the first byte - set up for it
        "	lcall 00079$					\n"
        "	mov   R1, #1					\n"
        "	sjmp  00033$					\n"

        // print loop
        "00032$:							\n"
        "	lcall 00079$					\n"
        "	mov   R1, #2					\n"
        "	mov   R3, A						\n"
        "	swap  A							\n"
        "00033$:							\n"
        "	anl   A, #0x0f					\n"
        "	add   A, R2						\n"
        "	push  DPH						\n"
        "	push  DPL						\n"
        "	mov   DPTR, #00099$				\n"
        "	movc  A, @A + DPTR				\n"
        "	pop   DPL						\n"
        "	pop   DPH						\n"
        "	lcall 00060$					\n"
        "	mov   A, R3						\n"
        "	djnz  R1, 00033$				\n"

        // dec DPTR
        "	dec   DPL						\n"
        "	mov   A, DPL					\n"
        "	cjne  A, #0xff, 00034$			\n"
        "	dec   DPH						\n"
        "00034$:							\n"
        "	djnz  R4, 00032$				\n"

        // done!
        "	ljmp  00055$					\n"

        // continue to process format string - handle 'd'
        "00036$:							\n"
        "	cjne  R3, #'d', 00037$			\n"
        "	mov   A, #0x20					\n"
        "	orl   A, R4						\n"
        "	mov   R4, A						\n"
        "	sjmp  00040$					\n"

        // continue to process format string - handle 'u'
        "00037$:							\n"
        "	cjne  R3, #'u', 00038$			\n"
        "	sjmp  00040$					\n"

        // no more format strings exist that we can handle - bail
        "00038$:							\n"
        "	ljmp  00001$					\n"

        // handle decimal printing
        "00040$:							\n"
        "	push  DPH						\n"
        "	push  DPL						\n"
        "	lcall 00080$					\n"  // get pointer to the number in DPTR, length in bytes in B
        "	push  B							\n"

        // copy the number to the double-dabble storage at proper offset (0 for u64, 4 for u32, 6 for u16)
        // we do this so that the dabble area always starts at the same place...
        "	mov   A, #8						\n"
        "	clr   C							\n"
        "	subb  A, B						\n"
        "	add   A, #_mCvtBuf				\n"
        "	mov   R1, A						\n"
        "	clr   A							\n"
        "	addc  A, #(_mCvtBuf >> 8)		\n"
        "	mov   R3, A						\n"
        "00041$:							\n"
        "	lcall 00079$					\n"
        "	inc   DPTR						\n"
        "	lcall 00086$					\n"
        "	movx  @DPTR, A					\n"
        "	inc   DPTR						\n"
        "	lcall 00086$					\n"
        "	djnz  B, 00041$					\n"
        // leave DPTR pointing to dabble storage, past the number
        "	lcall 00086$					\n"

        // we now have the top byte of the number in A, good time to check for negatives, if needed
        "	mov   B, A						\n"
        "	mov   A, R4						\n"
        "	anl   A, #0x20					\n"
        "	jz    00050$					\n"  // unsigned printing requested
        "	mov   A, B						\n"
        "	anl   A, #0x80					\n"
        "	jnz   00043$					\n"  // is negative - we need to invert, 0x20 bit in R1 stays
        // positive - 0x20 bit in R1 needs to go
        "	mov   A, R4						\n"
        "	anl   A, #~0x20					\n"
        "	mov   R4, A						\n"
        "	sjmp  00050$					\n"

        // we need to negate the number
        //  but first we need a pointer to it, and its size
        "00043$:							\n"
        "	pop   B							\n"
        "	push  B							\n"
        "	mov   A, #8						\n"
        "	clr   C							\n"
        "	subb  A, B						\n"
        "	add   A, #_mCvtBuf				\n"
        "	mov   DPL, A					\n"
        "	clr   A							\n"
        "	addc  A, #(_mCvtBuf >> 8)		\n"
        "	mov   DPH, A					\n"

        // ok, now we are ready to negate it
        "	clr   C							\n"
        "00049$:							\n"
        "	movx  A, @DPTR					\n"
        "	mov   R1, A						\n"
        "	clr   A							\n"
        "	subb  A, R1						\n"
        "	movx  @DPTR, A					\n"
        "	inc   DPTR						\n"
        "	djnz  B, 00049$					\n"

        // zero out the rest of the storage (10 bytes)
        "00050$:							\n"
        "	mov   B, #10					\n"
        "	clr   A							\n"
        "00042$:							\n"
        "	movx  @DPTR, A					\n"
        "	inc   DPTR						\n"
        "	djnz  B, 00042$					\n"

        // calculate number of dabble steps
        "	pop   A							\n"
        "	swap  A							\n"
        "	rr    A							\n"
        "	mov   R3, A						\n"

        // do the thing
        "00044$:							\n"

        // dabble (10 iters for simplicity)
        "	mov   DPTR, #(_mCvtBuf + 8)		\n"
        "	mov   B, #10					\n"
        "00046$:							\n"
        "	movx  A, @DPTR					\n"
        "	mov   R1, A						\n"
        "	anl   A, #0x0f					\n"
        "	add   A,#-0x05					\n"
        "	mov   A, R1						\n"
        "	jnc   00047$					\n"
        "	add   A, #0x03					\n"
        "00047$:							\n"
        "	mov   R1, A						\n"
        "	anl   A, #0xf0					\n"
        "	add   A,#-0x50					\n"
        "	mov   A, R1						\n"
        "	jnc   00048$					\n"
        "	add   A, #0x30					\n"
        "00048$:							\n"
        "	movx  @DPTR, A					\n"
        "	inc   DPTR						\n"
        "	djnz  B, 00046$					\n"

        // double (18 iters for simplicity)
        "	mov   DPTR, #_mCvtBuf			\n"
        "	clr   C							\n"
        "	mov   B, #18					\n"
        "00045$:							\n"
        "	movx  A, @DPTR					\n"
        "	rlc   A							\n"
        "	movx  @DPTR, A					\n"
        "	inc   DPTR						\n"
        "	djnz  B, 00045$					\n"

        "	djnz  R3, 00044$				\n"

        // dabbling is done, print it now using hex routine
        "	mov   DPTR, #(_mCvtBuf + 8)		\n"
        "	mov   B, #10					\n"
        "	clr   PSW.5						\n"  // it is now for sure in XRAM
        "	ljmp  00070$					\n"

        // read short pointer from param stack
        "00090$:							\n"
        "	mov   DPH, @R0					\n"
        "00093$:							\n"
        "	dec   R0						\n"
        "	mov   DPL, @R0					\n"
        "	dec   R0						\n"
        "	ret								\n"

        // read and increment pointer of the type provided by 00091$ (in {DPTR,PSW}) into A. clobber nothing
        "00095$:							\n"
        "	jb    PSW.5, 00066$ 			\n"
        "	jb    PSW.1, 00067$ 			\n"
        // XRAM
        "	movx  A, @DPTR					\n"
        "	inc   DPTR						\n"
        "	ret								\n"
        // CODE
        "00066$:							\n"
        "	clr   A							\n"
        "	movc  A, @A+DPTR				\n"
        "	inc   DPTR						\n"
        "	ret								\n"
        // IRAM
        "00067$:							\n"
        "	mov   DPH, R0					\n"
        "	mov   R0, DPL					\n"
        "	mov   A, @R0					\n"
        "	mov   R0, DPH					\n"
        "	inc   DPL						\n"
        "	ret								\n"

        // resolve generic pointer on param stack to an pointer in DPTR and flags in PSW.5 and PSW.1
        // PSW.5 will be 0 and PSW.1 will be 0 for XRAM (PDATA goes here too)
        // PSW.5 will be 1 and PSW.1 will be 0 for CODE
        // PSW.5 will be 0 and PSW.1 will be 1 for IRAM
        "00091$:							\n"
        "	clr   PSW.5						\n"
        "	clr   PSW.1						\n"
        "	mov   A, @R0					\n"
        "	dec   R0						\n"
        "	jz    00090$					\n"  // 0x00: pointer type: xdata
        "	xrl   A, #0x80					\n"
        "	jz    00094$					\n"  // 0x80: pointer type: code
        "	xrl   A, #0xc0					\n"
        "	jz    00092$					\n"  // 0x40: pointer type: idata
        // pdata
        "	mov   DPH, _XPAGE				\n"
        "	sjmp  00093$					\n"
        // idata
        "00092$:							\n"
        "	setb  PSW.1						\n"
        "	sjmp  00093$					\n"
        // code
        "00094$:							\n"
        "	setb  PSW.5						\n"
        "	sjmp  00090$					\n"

        // read the pointer of the type that 00080$ returns (in DPTR) into A. clobber nothing
        "00079$:							\n"
        "	jnb   PSW.5, 00078$				\n"
        "	push  _R0						\n"
        "	mov   R0, DPL					\n"
        "	mov   A, @R0					\n"
        "	pop   _R0						\n"
        "	ret								\n"
        "00078$:							\n"
        "	movx  A, @DPTR					\n"
        "	ret								\n"

        // get pointer to a number, might be pushed or might be pointed to, size might vary. return pointer to number's LOW byte in DPTR
        "00080$:							\n"
        "	mov   A, R4						\n"
        "	anl   A, #0x01					\n"
        "	jnz   00083$					\n"
        // param is itself on stack - now we care about size, but either way, PSW.5 will be 1
        "	setb  PSW.5						\n"
        "	mov   B, #0						\n"
        "	mov   A, R4						\n"
        "	anl   A, #0x18					\n"
        "	jz    00081$					\n"
        "	anl   A, #0x10					\n"
        "	jz    00082$					\n"
        // long long (8 bytes)				\n"
        "	setb  B.2						\n"
        "	dec   R0						\n"
        "	dec   R0						\n"
        "	dec   R0						\n"
        "	dec   R0						\n"
        // long (4 bytes)
        "00082$:							\n"
        "	setb  B.1						\n"
        "	dec   R0						\n"
        "	dec   R0						\n"
        // int (2 bytes)						\n"
        "00081$:							\n"
        "	setb  B.0						\n"
        "	dec   R0						\n"
        "	mov   DPL, R0					\n"
        "	dec   R0						\n"
        "	inc   B							\n"
        "	ret								\n"
        // pointer it on stack itself, number is in xram, but we still need to provide the length
        "00083$:							\n"
        "	clr   PSW.5						\n"  // mark as "in xram"
        "	mov   A, R4						\n"
        "	anl   A, #0x18					\n"
        "	jz    00084$					\n"
        "	anl   A, #0x10					\n"
        "	jz    00085$					\n"
        // long long
        "	mov   B, #8						\n"
        "	ljmp  00090$					\n"
        // long
        "00085$:							\n"
        "	mov   B, #4						\n"
        "	ljmp  00090$					\n"
        // int
        "00084$:							\n"
        "	mov   B, #2						\n"
        "	ljmp  00090$					\n"

        // swap R3:R1 <-> DPH:DPL
        "00086$:							\n"
        "	xch   A, DPH					\n"
        "	xch   A, R3						\n"
        "	xch   A, DPH					\n"
        "	xch   A, DPL					\n"
        "	xch   A, R1						\n"
        "	xch   A, DPL					\n"
        "	ret								\n"

        /* putchar func
                called via call. char is in A, R7 has pointer to stack as needed
                can clobber B, CANNOT clobber DPTR
                a mess because...8051
        */
        "00060$:							\n"
        "	push  DPH						\n"
        "	push  DPL						\n"
        "	push  _R1						\n"
        "	push  _R0						\n"
        "	mov   _R0, R7					\n"
        "	mov   DPL, @R0					\n"
        "	dec   R0						\n"
        "	mov   DPH, @R0					\n"  // DPTR is now func ptr
        "	dec   R0						\n"
        "	dec   R0						\n"
        "	dec   R0						\n"
        "	dec   R0						\n"
        "	mov   _R1, @R0					\n"
        "	dec   R0						\n"
        "	mov   _R0, @R0					\n"  // R1:R0 is now "formatD"
        "	lcall 00061$					\n"  // to set ret addr
        "	pop   _R0						\n"
        "	pop   _R1						\n"
        "	pop   DPL						\n"
        "	pop   DPH						\n"
        "	ret								\n"
        "00061$:							\n"
        "	push  DPL						\n"
        "	push  DPH						\n"
        "	mov   DPL, _R0					\n"
        "	mov   DPH, _R1					\n"
        "	ret								\n"

        "00099$:							\n"
        "	.ascii \"01234567\"				\n"
        "	.ascii \"89ABCDEF\"				\n"
        "	.ascii \"01234567\"				\n"
        "	.ascii \"89abcdef\"				\n");
    (void)fmt;
    (void)vl;
    (void)formatF;
    (void)formatD;
}

#pragma callee_saves prPrvPutchar
static void prPrvPutchar(uint32_t data) __reentrant {
    char ch = data >> 24;
    if (ch == '\n')
        uartTx('\r');
    uartTx(ch);
}
#pragma callee_saves epdPutchar
static void epdPutchar(uint32_t data) __reentrant {
    char ch = data >> 24;
    writeCharEPD(ch);
}

void pr(const char __code *fmt, ...) __reentrant {
    va_list vl;
    va_start(vl, fmt);
    prvPrintFormat(prPrvPutchar, 0, fmt, vl);
    va_end(vl);
}

void epdpr(const char __code *fmt, ...) __reentrant {
    va_list vl;
    va_start(vl, fmt);
    prvPrintFormat(epdPutchar, 0, fmt, vl);
    va_end(vl);
}

#pragma callee_saves prPrvPutS
static void prPrvPutS(uint32_t data) __reentrant {
    char __xdata *__idata *strPP = (char __xdata *__idata *)data;
    char ch = data >> 24;

    *(*strPP)++ = ch;
}

void spr(char __xdata *out, const char __code *fmt, ...) __reentrant {
    char __xdata *outStart = out;

    va_list vl;

    va_start(vl, fmt);
    prvPrintFormat(prPrvPutS, (uint16_t)&out, fmt, vl);
    va_end(vl);

    *out = 0;
}
