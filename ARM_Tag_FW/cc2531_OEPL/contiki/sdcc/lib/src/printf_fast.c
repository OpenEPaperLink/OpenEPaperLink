/*-------------------------------------------------------------------------
   printf_fast.c - Fast printf routine for use with sdcc/mcs51

   Copyright (C) 2004, Paul Stoffregen, paul@pjrc.com

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

/******************************************************************/
/**                                                              **/
/**    Major features.  These determine what capabilities your   **/
/**    compiled printf_fast will have.                           **/
/**                                                              **/
/******************************************************************/

// Include support for 32 bit base 10 integers (%ld and %lu).  Without
// this, you won't be able to print 32 bit integers as base 10.  They
// will appear in hexadecimal.
#define LONG

// Include support for floating point numbers (%f).  Don't forget to
// enable LONG above, if you want to print floats greater than
// 65535.997.  You can have 6 good digits after the decimal point,
// or an 8th if a small error is ok.  +/- 2^32 to 1/10^8 isn't the
// full dynamic range of 32 bit floats, but it covers the most
// commonly used range.  Adds about 500-600 bytes of code.
//#define FLOAT

// Include support for minimum field widths (%8d, %20s, %12.5f)
#define FIELD_WIDTH

// Include fast integer conversion.  Without this, a compact but slower
// algorithm is used to convert integers (%d, %u, int part of %f).
// Even the slow algorithm is much faster than a typical C implementation
// based on repetitive division by 10.  If you enable this, you get an
// extremely fast version (only 8 table lookups and 8 adds to convert a
// 32 bit integer), but it costs extra code space for larger lookup
// tables and optimized non-looping code.
#define FAST_INTEGER


/******************************************************************/
/**                                                              **/
/**    Minor tweaks.  These provide small code savings, with     **/
/**    a partial loss of functionality.                          **/
/**                                                              **/
/******************************************************************/


// If you enabled FLOAT, enabling this replaces the normal %f float
// output with a very compact version that always prints 4 fractional
// digits and does not have round off.  Zero will print as "0.0000",
// and 1.999997 will print as "1.9999" (not rounded up to 2).  The
// 4th digit is not accurate (+/- 2).  This simpler version also
// avoids using 5 bytes of internal data memory.  Code size is about
// 240 bytes less.
//#define FLOAT_FIXED4

// If you used FLOAT (not FLOAT_FIXED4), this will remove the smart
// default number of digits code.  When you use "%f" without a field
// width, normally the smart default width code chooses a good number
// of digits based on size of the number.  If you enabled FIELD_WIDTH
// and use a number, like "%.5f", this smart default code is never
// used anyway.  Saves about 40 bytes of code.
//#define FLOAT_DEFAULT_FRAC_DIGITS 6

// If you used FLOAT (not FLOAT_FIXED4) and you do not specify a
// field width, normally trailing zeros are trimmed.  Using this
// removes that feature (saves only a few bytes).
//#define DO_NOT_TRIM_TRAILING_ZEROS

// Omit saving and restoring registers when calling putchar().  If you
// are desparate for a little more code space, this will give you a
// small savings.  You MUST define putchar() with #pragma callee_saves,
// or implement it in assembly and avoid changing the registers.
//#define PUTCHAR_CALLEE_SAVES


/* extern void putchar(char ); */

// Warning: using static/global variables makes these functions NON-reentrant!
// reentrant keyword is only used for parameter passing method

static __bit long_flag, short_flag, print_zero_flag, negative_flag;

#ifdef FIELD_WIDTH
static __bit field_width_flag;
static __bit leading_zero_flag;
static __data unsigned char field_width;
#endif

#ifdef FLOAT
#define __SDCC_FLOAT_LIB
#include <float.h>
static __bit continue_float;
#ifndef FLOAT_FIXED4
static __data unsigned char frac_field_width;
static __data unsigned char float_frac_bcd[4];
// TODO: can float_frac_bcd be overlaid with temps used by trig functions
#endif
#endif

#ifndef FAST_INTEGER
#ifdef LONG
static __data unsigned int i2bcd_tmp;  // slow 32 int conversion needs temp space
#endif
#endif


#ifndef PRINTF_FAST
#define PRINTF_FAST printf_fast
#endif


#if !defined(__SDCC_mcs51) || defined(__SDCC_USE_XSTACK) || defined(_SDCC_NO_ASM_LIB_FUNCS)
// Does printf_fast really work on ds390 and ds400?
// If it does, enable them in the line above
#if defined(__SDCC_USE_XSTACK)
#warning "printf_fast not built, does not support --xstack"
#elif defined(_SDCC_NO_ASM_LIB_FUNCS)
#warning "printf_fast not built, _SDCC_NO_ASM_LIB_FUNCS defined"
#endif
#else // defines are compatible with printf_fast


void PRINTF_FAST(__code const char *fmt, ...) __reentrant
{
	fmt;	/* suppress unreferenced variable warning */

	__asm

printf_begin:
	mov	a, _bp		// r0 will point to va_args (stack)
	add	a, #253
	mov	r0, a		// r0 points to MSB of fmt
	mov	dph, @r0
	dec	r0
	mov	dpl, @r0	// dptr has address of fmt
	dec	r0

printf_main_loop:
	clr	a
	movc	a, @a+dptr	// get next byte of fmt string
	inc	dptr
	//cjne	a, #'%', printf_normal
	cjne	a, #37, printf_normal

printf_format:
	clr	_long_flag
	clr	_short_flag
	clr	_print_zero_flag
	clr	_negative_flag
#ifdef FIELD_WIDTH
	clr	_field_width_flag
	clr	_leading_zero_flag
	mov	r1, #_field_width
	mov	@r1, #0
#endif
#ifdef FLOAT
	clr	_continue_float
#endif

printf_format_loop:
	clr	a
	movc	a, @a+dptr	// get next byte of data format
	inc	dptr

	/* parse and consume the field width digits, even if */
	/* we don't build the code to make use of them */
	add	a, #198
	jc	printf_nondigit1
	add	a, #10
	jnc	printf_nondigit2
#ifdef FIELD_WIDTH
printf_digit:
	jnz	printf_digit_2
	cjne	a, _field_width, printf_digit_2
	setb	_leading_zero_flag
printf_digit_2:
	setb	_field_width_flag
	mov	r2, a
	mov	a, @r1
	mov	b, #10
	mul	ab
	add	a, r2
	mov	@r1, a
#endif
	sjmp	printf_format_loop
printf_nondigit1:
	add	a, #10
printf_nondigit2:
	add	a, #48

printf_format_l:
	//cjne	a, #'l', printf_format_h
	cjne	a, #108, printf_format_h
	setb	_long_flag
	sjmp	printf_format_loop

printf_format_h:
	//cjne	a, #'h', printf_format_s
	cjne	a, #104, printf_format_s
	setb	_short_flag
	sjmp	printf_format_loop

printf_format_s:
	//cjne	a, #'s', printf_format_d
	cjne	a, #115, printf_format_d
	ljmp	printf_string

printf_format_d:
	//cjne	a, #'d', printf_format_u
	cjne	a, #100, printf_format_u
	lcall	printf_get_int
	ljmp	printf_int

printf_format_u:
	//cjne	a, #'u', printf_format_c
	cjne	a, #117, printf_format_c
	lcall	printf_get_int
	ljmp	printf_uint

printf_format_c:
	//cjne	a, #'c', printf_format_x
	cjne	a, #99, printf_format_x
	dec	r0
	mov	a, @r0		// Acc has the character to print
	dec	r0
	sjmp	printf_char

printf_format_x:
	//cjne	a, #'x', printf_format_f
	cjne	a, #120, printf_format_f
	ljmp	printf_hex

printf_format_f:
#ifdef FLOAT
	//cjne	a, #'f', printf_format_dot
	cjne	a, #102, printf_format_dot
	ljmp	print_float
#endif

printf_format_dot:
	//cjne	a, #'.', printf_normal
	cjne	a, #46, printf_normal
#ifdef FLOAT
#ifdef FLOAT_FIXED4
	mov	r1, #ar3	// parse frac field, but discard if FIXED4
#else
	mov	r1, #_frac_field_width
	mov	@r1, #0
#endif
#endif
	sjmp	printf_format_loop

printf_normal:
	jz	printf_eot
printf_char:
	lcall	printf_putchar
	ljmp	printf_main_loop

printf_eot:
	ljmp	printf_end


	/* print a string... just grab each byte with __gptrget */
	/* the user much pass a 24 bit generic pointer */

printf_string:
	push	dph		// save addr in fmt onto stack
	push	dpl
	mov	b, @r0		// b has type of address (generic *)
	dec	r0
	mov	dph, @r0
	dec	r0
	mov	dpl, @r0	// dptr has address of user's string
	dec	r0

#ifdef FIELD_WIDTH
	jnb	_field_width_flag, printf_str_loop
	clr	_leading_zero_flag	// never leading zeros for strings
	push	dpl
	push	dph
printf_str_fw_loop:
	lcall	__gptrget
	jz	printf_str_space
	inc	dptr
	dec	_field_width
	mov	a, _field_width
	jnz	printf_str_fw_loop
printf_str_space:
	lcall	printf_space
	pop	dph
	pop	dpl
#endif // FIELD_WIDTH

printf_str_loop:
	lcall	__gptrget
	jz	printf_str_done
	inc	dptr
	lcall	printf_putchar
	sjmp	printf_str_loop
printf_str_done:
	pop	dpl		// restore addr withing fmt
	pop	dph
	ljmp	printf_main_loop


	/* printing in hex is easy because sdcc pushes the LSB first */

printf_hex:
	lcall	printf_hex8
	jb	_short_flag, printf_hex_end
	lcall	printf_hex8
	jnb	_long_flag, printf_hex_end
	lcall	printf_hex8
	lcall	printf_hex8
printf_hex_end:
	lcall	printf_zero
	ljmp	printf_main_loop
printf_hex8:
	mov	a, @r0
	lcall	printf_phex_msn
	mov	a, @r0
	dec	r0
	ljmp	printf_phex_lsn


#ifndef LONG
printf_ld_in_hex:
	//mov	a, #'0'
	mov	a, #48
	lcall	printf_putchar
	//mov	a, #'x'
	mov	a, #120
	lcall	printf_putchar
	mov	a, r0
	add	a, #4
	mov	r0, a
	sjmp	printf_hex
#endif


	/* printing an integer is not so easy.  For a signed int */
	/* check if it is negative and print the minus sign and */
	/* invert it to a positive integer */

printf_int:
	mov	a, r5
	jnb	acc.7, printf_uint	/* check if negative */
	setb	_negative_flag
	mov	a, r1			/* invert integer */
	cpl	a
	add	a, #1
	mov	r1, a
	jb	_short_flag, printf_uint
	mov	a, r2
	cpl	a
	addc	a, #0
	mov	r2, a
	jnb	_long_flag, printf_uint
	mov	a, r3
	cpl	a
	addc	a, #0
	mov	r3, a
	mov	a, r4
	cpl	a
	addc	a, #0
	mov	r4, a


	/* printing integers is a lot of work... because it takes so */
	/* long, the first thing to do is make sure we're doing as */
	/* little work as possible, then convert the binary int to */
	/* packed BCD, and finally print each digit of the BCD number */

printf_uint:

	jb	_short_flag, printf_uint_ck8
	jnb	_long_flag, printf_uint_ck16
printf_uint_ck32:
	/* it's a 32 bit int... but if the upper 16 bits are zero */
	/* we can treat it like a 16 bit integer and convert much faster */
#ifdef LONG
	mov	a, r3
	jnz	printf_uint_begin
	mov	a, r4
	jnz	printf_uint_begin
#else
	mov	a, r3
	jnz	printf_ld_in_hex	// print long integer as hex
	mov	a, r4			// rather than just the low 16 bits
	jnz	printf_ld_in_hex
#endif
	clr	_long_flag
printf_uint_ck16:
	/* it's a 16 bit int... but if the upper 8 bits are zero */
	/* we can treat it like a 8 bit integer and convert much faster */
	mov	a, r2
	jnz	printf_uint_begin
	setb	_short_flag
printf_uint_ck8:
	/* it's an 8 bit int... if it's zero, it's a lot faster to just */
	/* print the digit zero and skip all the hard work! */
	mov	a, r1
	jnz	printf_uint_begin
#ifdef FLOAT
	/* never use the "just print zero" shortcut if we're printing */
	/* the integer part of a float  (fixes bug 1255403)  */
	jb	_continue_float, printf_uint_begin
#endif
#ifdef FIELD_WIDTH
	jnb	_field_width_flag, printf_uint_zero
	mov	a, _field_width
	jz	printf_uint_zero
	dec	_field_width
	lcall	printf_space
#endif
printf_uint_zero:
	//mov	a, #'0'
	mov	a, #48
	lcall	printf_putchar
	ljmp	printf_main_loop

printf_uint_begin:
	push	dpl
	push	dph
	lcall	printf_int2bcd		// bcd number in r3/r2/r7/r6/r5
printf_uint_2:

#ifdef FIELD_WIDTH
	jnb	_field_width_flag, printf_uifw_end
#ifdef LONG
printf_uifw_32:
	mov	r1, #10
	jnb	_long_flag, printf_uifw_16
	mov	a, r3
	anl	a, #0xF0
	jnz	printf_uifw_sub
	dec	r1
	mov	a, r3
	anl	a, #0x0F
	jnz	printf_uifw_sub
	dec	r1
	mov	a, r2
	anl	a, #0xF0
	jnz	printf_uifw_sub
	dec	r1
	mov	a, r2
	anl	a, #0x0F
	jnz	printf_uifw_sub
	dec	r1
	mov	a, r7
	anl	a, #0xF0
	jnz	printf_uifw_sub
#endif // LONG
printf_uifw_16:
	mov	r1, #5
	jb	_short_flag, printf_uifw_8
	mov	a, r7
	anl	a, #0x0F
	jnz	printf_uifw_sub
	dec	r1
	mov	a, r6
	anl	a, #0xF0
	jnz	printf_uifw_sub
printf_uifw_8:
	mov	r1, #3
	mov	a, r6
	anl	a, #0x0F
	jnz	printf_uifw_sub
	dec	r1
	mov	a, r5
	anl	a, #0xF0
	jnz	printf_uifw_sub
	dec	r1
printf_uifw_sub:
	//r1 has the number of digits for the number
	mov	a, _field_width
	mov	c, _negative_flag
	subb	a, r1
	jc	printf_uifw_end
	mov	_field_width, a

#ifndef PUTCHAR_CALLEE_SAVES
#ifdef LONG
	push	ar3
	push	ar2
#endif
	push	ar7
	push	ar6
	push	ar5
#endif
	lcall	printf_space
#ifndef PUTCHAR_CALLEE_SAVES
	pop	ar5
	pop	ar6
	pop	ar7
#ifdef LONG
	pop	ar2
	pop	ar3
#endif
#endif


printf_uifw_end:
#endif // FIELD_WIDTH


printf_uint_doit:
	jnb	_negative_flag, printf_uint_pos
#ifdef PUTCHAR_CALLEE_SAVES
	//mov	a, #'-'
	mov	a, #45
	lcall	printf_putchar
#else
#ifdef LONG
	push	ar3
	push	ar2
#endif
	push	ar7
	push	ar6
	push	ar5
	//mov	a, #'-'
	mov	a, #45
	lcall	printf_putchar
	pop	ar5
	pop	ar6
	pop	ar7
#ifdef LONG
	pop	ar2
	pop	ar3
#endif
#endif // PUTCHAR_CALLEE_SAVES

printf_uint_pos:
	jb	_short_flag, printf_uint8
#ifdef LONG
	jnb	_long_flag, printf_uint16
printf_uint32:
	push	ar5
	push	ar6
	push	ar7
	mov	dpl, r2
	mov	a, r3
	mov	dph, a
	lcall	printf_phex_msn
	mov	a, dph
	lcall	printf_phex_lsn
	mov	a, dpl
	lcall	printf_phex_msn
	mov	a, dpl
	lcall	printf_phex_lsn
	pop	acc
	mov	dpl, a
	lcall	printf_phex_msn
	mov	a, dpl
	pop	dph
	pop	dpl
	sjmp	printf_uint16a
#endif // LONG

printf_uint16:
	mov	dpl, r5
	mov	dph, r6
	mov	a, r7
printf_uint16a:
	lcall	printf_phex_lsn
	mov	a, dph
	lcall	printf_phex_msn
	mov	a, dph
	sjmp	printf_uint8a

printf_uint8:
	mov	dpl, r5
	mov	a, r6
printf_uint8a:
	lcall	printf_phex_lsn
	mov	a, dpl
	lcall	printf_phex_msn
	mov	a, dpl
	lcall	printf_phex_lsn
	lcall	printf_zero
	pop	dph
	pop	dpl
#ifdef FLOAT
	jnb	_continue_float, 0002$
	ret
0002$:
#endif
	ljmp	printf_main_loop


#ifdef FLOAT
#ifdef FLOAT_FIXED4
	// Print a float the easy way.  First, extract the integer part and
	// use the integer printing code.  Then extract the fractional part,
	// convert each bit to 4 digit BCD, and print the BCD sum.  Absolutely
	// no field width control, always 4 digits printed past the decimal
	// point.  No round off.  1.9999987 prints as 1.9999, not 2.0000.
print_float:
#ifdef FIELD_WIDTH
	jnb	_field_width_flag, print_float_begin
	mov	a, _field_width
	add	a, #251
	mov	_field_width, a
	jc	print_float_begin
	mov	_field_width, #0
#endif
print_float_begin:
	push	ar0		// keep r0 safe, will need it again
	lcall	printf_get_float
	clr	c
	mov	a, #158			// check for large float we can't print
	subb	a, r7
	jnc	print_float_size_ok
printf_float_too_big:
	// TODO: should print some sort of overflow error??
	pop	ar0
	ljmp	printf_format_loop
print_float_size_ok:
	push	dpl
	lcall	fs_rshift_a
	pop	dpl
	setb	_continue_float
#ifndef LONG
	mov	a, r3
	orl	a, r4
	jnz	printf_float_too_big
#endif
	lcall	printf_uint		// print the integer portion
	//mov	a, #'.'
	mov	a, #0x2E
	lcall	printf_putchar
	// now that the integer part is printed, we need to refetch the
	// float from the va_args and extract the fractional part
	pop	ar0
	lcall	printf_get_float
	push	ar0
	push	dpl
	push	dph
	mov	a, r7
	cjne	a, #126, print_float_frac_lshift
	sjmp	print_float_frac // input between 0.5 to 0.9999
print_float_frac_lshift:
	jc	print_float_frac_rshift
	//Acc (exponent) is greater than 126 (input >= 1.0)
	add	a, #130
	mov	r5, a
print_float_lshift_loop:
	clr	c
	mov	a, r2
	rlc	a
	mov	r2, a
	mov	a, r3
	rlc	a
	mov	r3, a
	mov	a, r4
	rlc	a
	mov	r4, a
	djnz	r5, print_float_lshift_loop
	sjmp	print_float_frac
print_float_frac_rshift:
	//Acc (exponent) is less than 126 (input < 0.5)
	cpl	a
	add	a, #127
	lcall	fs_rshift_a
print_float_frac:
	// now we've got the fractional part, so now is the time to
	// convert to BCD... just convert each bit to BCD using a
	// lookup table and BCD sum them together
	mov	r7, #14
	clr	a
	mov	r6, a
	mov	r5, a
	mov	dptr, #_frac2bcd	// FLOAT_FIXED4 version (14 entries)
print_float_frac_loop:
	mov	a, r3
	rlc	a
	mov	r3, a
	mov	a, r4
	rlc	a
	mov	r4, a
	jnc	print_float_frac_skip
	clr	a
	movc	a, @a+dptr
	add	a, r5
	da	a
	mov	r5, a
	mov	a, #1
	movc	a, @a+dptr
	addc	a, r6
	da	a
	mov	r6, a
print_float_frac_skip:
	inc	dptr
	inc	dptr
	djnz	r7, print_float_frac_loop
	// the BCD sum is in dptr, so all we've got to do is output
	// all 4 digits.  No trailing zero suppression, no nice round
	// off (impossible to change the integer part since we already
	// printed it).
	mov	dph, r6
	mov	dpl, r5
	setb	_print_zero_flag
	mov	a, dph
	lcall	printf_phex_msn
	mov	a, dph
	lcall	printf_phex_lsn
	mov	a, dpl
	lcall	printf_phex_msn
	mov	a, dpl
	lcall	printf_phex_lsn
	pop	dph
	pop	dpl
	pop	ar0
	ljmp	printf_main_loop

#else // not FLOAT_FIXED4


print_float:
	// Print a float the not-as-easy way, with a configurable number of
	// fractional digits (up to 8) and proper round-off (up to 7 digits).
	// First, extract the fractional part, convert to BCD, and then add
	// the scaled round-off.  Store the rounded fractional digits and
	// their carry.  Then extract the integer portion, increment it if
	// the rounding caused a carry.  Use the integer printing to output
	// the integer, and then output the stored fractional digits.  This
	// approach requires 5 bytes of internal RAM to store the 8 fractional
	// digits and the number of them we'll actually print.  This code is
	// a couple hundred bytes larger and a bit slower than the FIXED4
	// version, but it gives very nice results.
print_float_1:
#ifdef FIELD_WIDTH
	jnb	_field_width_flag, print_float_default_width
	// The caller specified exact field width, so use it.  Need to
	// convert the whole float digits into the integer portion only.
	mov	a, _field_width
	setb	c
	subb	a, _frac_field_width
	mov	_field_width, a
	jnc	print_float_begin
	mov	_field_width, #0
	sjmp	print_float_begin
#endif
print_float_default_width:
	// The caller didn't specify field width (or FIELD_WIDTH is
	// not defined so it's ignored).  We've still got to know
	// how many fractional digits are going to print, so we can
	// round off properly.
#ifdef FLOAT_DEFAULT_FRAC_DIGITS
	mov	_frac_field_width, #FLOAT_DEFAULT_FRAC_DIGITS
#else
	// default fractional field width (between 0 to 7)
	// attempt to scale the default number of fractional digits
	// based on the magnitude of the float
	mov	a, @r0
	anl	a, #0x7F	// ignore sign bit
	mov	r2, a		// r2 is first byte of float
	dec	r0
	mov	ar3, @r0	// r3 is second byte of float
	inc	r0
	mov	r6, dpl
	mov	r7, dph
	mov	dptr, #_float_range_table
	mov	r5, #7
print_float_default_loop:
	clr	a
	movc	a, @a+dptr
	add	a, r3
	inc	dptr
	clr	a
	movc	a, @a+dptr
	addc	a, r2
	jnc	print_float_default_done
	inc	dptr
	djnz	r5, print_float_default_loop
print_float_default_done:
	mov	_frac_field_width, r5
	mov	dpl, r6
	mov	dph, r7
#endif // not FLOAT_DEFAULT_FRAC_DIGITS

print_float_begin:
	push	ar0			// keep r0 safe, will need it again
	lcall	printf_get_float
	push	dpl
	push	dph
	mov	a, r7
	cjne	a, #126, print_float_frac_lshift
	sjmp	print_float_frac	// input between 0.5 to 0.9999

print_float_frac_lshift:
	jc	print_float_frac_rshift
	//Acc (exponent) is greater than 126 (input >= 1.0)
	add	a, #130
	mov	r5, a
print_float_lshift_loop:
	clr	c
	mov	a, r2
	rlc	a
	mov	r2, a
	mov	a, r3
	rlc	a
	mov	r3, a
	mov	a, r4
	rlc	a
	mov	r4, a
	djnz	r5, print_float_lshift_loop
	sjmp	print_float_frac
print_float_frac_rshift:
	//Acc (exponent) is less than 126 (input < 0.5)
	cpl	a
	add	a, #127
	lcall	fs_rshift_a
print_float_frac:
	// Convert the fraction in r4/r3/r2/r1 into 8 BCD digits in r0/r7/r6/r5
	mov	b, #27
	clr	a
	mov	r0, a
	mov	r7, a
	mov	r6, a
	mov	r5, a
	mov	dptr, #_frac2bcd	// FLOAT version (27 entries)
print_float_frac_loop:
	mov	a, r1
	rlc	a
	mov	r1, a
	mov	a, r2
	rlc	a
	mov	r2, a
	mov	a, r3
	rlc	a
	mov	r3, a
	mov	a, r4
	rlc	a
	mov	r4, a
	jnc	print_float_frac_skip
	clr	a
	movc	a, @a+dptr
	add	a, r5
	da	a
	mov	r5, a
	mov	a, #1
	movc	a, @a+dptr
	addc	a, r6
	da	a
	mov	r6, a
	mov	a, #2
	movc	a, @a+dptr
	addc	a, r7
	da	a
	mov	r7, a
	mov	a, #3
	movc	a, @a+dptr
	addc	a, r0
	da	a
	mov	r0, a
print_float_frac_skip:
	inc	dptr
	inc	dptr
	inc	dptr
	inc	dptr
	djnz	b, print_float_frac_loop
print_float_frac_roundoff:
	// Now it's time to round-off the BCD digits to the desired precision.
	clr	a
	mov	r4, #0x50		// r4/r3/r2/r1 = 0.5 (bcd rounding)
	mov	r3, a
	mov	r2, a
	mov	r1, a
	mov	a, _frac_field_width
	rl	a
	rl	a
	anl	a, #0xFC
	mov	dph, r0			// fs_rshift_a will overwrite r0 & dpl
	lcall	fs_rshift_a		// divide r4/r3/r2/r1 by 10^frac_field_width
	mov	a, r5
	add	a, r1			// add rounding to fractional part
	da	a
	mov	_float_frac_bcd+3, a	// and store it for later use
	mov	a, r6
	addc	a, r2
	da	a
	mov	_float_frac_bcd+2, a
	mov	a, r7
	addc	a, r3
	da	a
	mov	_float_frac_bcd+1, a
	mov	a, dph
	addc	a, r4
	da	a
	mov	_float_frac_bcd+0, a
	mov	sign_b, c		// keep fractional carry in sign_b
	pop	dph
	pop	dpl
print_float_int:
	// Time to work on the integer portion... fetch the float again, check
	// size (exponent), scale to integer, add the fraction's carry, and
	// let the integer printing code do all the work.
	pop	ar0
	lcall	printf_get_float
	push	ar0
	clr	c
	mov	a, #158			// check for large float we can't print
	subb	a, r7
	jnc	print_float_size_ok
printf_float_too_big:
	// TODO: should print some sort of overflow error??
	pop	ar0
	ljmp	printf_format_loop
print_float_size_ok:
	push	dpl
	lcall	fs_rshift_a
	pop	dpl
	jnb	sign_b, print_float_do_int
	// if we get here, the fractional round off caused the
	// integer part to increment.  Add 1 for a proper result
	mov	a, r1
	add	a, #1
	mov	r1, a
	clr	a
	addc	a, r2
	mov	r2, a
#ifdef LONG
	clr	a
	addc	a, r3
	mov	r3, a
	clr	a
	addc	a, r4
	mov	r4, a
#endif
	jc	printf_float_too_big
print_float_do_int:
#ifndef LONG
	mov	a, r3
	orl	a, r4
	jnz	printf_float_too_big
#endif
	setb	_continue_float
	lcall	printf_uint		// print the integer portion


print_float_frac_width:
	// Now all we have to do is output the fractional digits that
	// were previous computed and stored in memory.
#ifdef FIELD_WIDTH
	jb	_field_width_flag, print_float_do_frac
#endif
#ifndef DO_NOT_TRIM_TRAILING_ZEROS 
	// if the user did not explicitly set a
	// field width, trim off trailing zeros
print_float_frac_trim:
	mov	a, _frac_field_width
	jz	print_float_do_frac
	lcall	get_float_frac_digit
	jnz	print_float_do_frac
	djnz	_frac_field_width, print_float_frac_trim
#endif

print_float_do_frac:
	mov	a, _frac_field_width
	jz	print_float_done
	//mov	a, #'.'
	mov	a, #0x2E
	lcall	printf_putchar
	mov	r0, #0
	setb	_print_zero_flag
print_float_do_frac_loop:
	inc	r0
	mov	a, r0
	lcall	get_float_frac_digit
	lcall	printf_phex_lsn
	mov	a, r0
	cjne	a, _frac_field_width, print_float_do_frac_loop

print_float_done:
	pop	ar0
	ljmp	printf_main_loop


	// acc=1 for tenths, acc=2 for hundredths, etc
get_float_frac_digit:
	dec	a
	clr	c
	rrc	a
	mov	psw.5, c
	add	a, #_float_frac_bcd
	mov	r1, a
	mov	a, @r1
	jb	psw.5, get_float_frac_digit_done
	swap	a
get_float_frac_digit_done:
	anl	a, #15
	ret

#endif // end of normal FLOAT code (not FLOAT_FIXED4)


// These helper functions are used, regardless of which type of
// FLOAT code is used.

#if 0
pm2_print_float:
	 mov	a, r7
	 lcall	pm2_entry_phex
	 mov	a, #0x20
	 lcall	pm2_entry_cout
	 lcall	_print_r4321
	 mov	a, #0x20
	 lcall	pm2_entry_cout
	 ret
#endif

	// Fetch a float from the va_args and put it into
	// r7(exp) r4/r3/r2(mant) and also clear r1 and preset
	// the flags
printf_get_float:
	mov	a, @r0
	dec	r0
	mov	r1, a
	mov	a, @r0
	dec	r0
	mov	r4, a
	rlc	a
	mov	a, r1
	rlc	a
	mov	_negative_flag, c
	mov	r7, a
	jz	printf_get_float_2
	orl	ar4, #0x80
printf_get_float_2:
	mov	a, @r0
	dec	r0
	mov	r3, a
	mov	a, @r0
	dec	r0
	mov	r2, a
	mov	r1, #0
	clr	_short_flag
	setb	_long_flag
	ret
#endif // FLOAT


	/* read an integer into r1/r2/r3/r4, and msb into r5 */
printf_get_int:
	mov	a, @r0
	mov	r1, a
	mov	r5, a
	dec	r0
	jb	_short_flag, printf_get_done
	mov	r2, ar1
	mov	a, @r0
	mov	r1, a
	dec	r0
	jnb	_long_flag, printf_get_done
	mov	r4, ar2
	mov	r3, ar1
	mov	a, @r0
	mov	r2, a
	dec	r0
	mov	a, @r0
	mov	r1, a
	dec	r0
printf_get_done:
	ret


#ifdef FAST_INTEGER

	/* convert binary number in r4/r3/r2/r1 into bcd packed number
	 * in r3/r2/r7/r6/r5.  The input number is destroyed in the
	 * process, to avoid needing extra memory for the result (and
	 * r1 gets used for temporary storage).  dptr is overwritten,
	 * but r0 is not changed.
	 */

printf_int2bcd:
	mov	a, r1
	mov	b, #100
	div	ab
	mov	r6, a
	mov	a, #10
	xch	a, b
	div	ab
	swap	a
	orl	a, b
	mov	r5, a

	jnb	_short_flag, printf_i2bcd_16	// if 8 bit int, we're done
	ret

printf_i2bcd_16:
	mov	a, r2
	anl	a, #0x0F
	mov	r1, a
	mov	dptr, #_int2bcd_2
	movc	a, @a+dptr
	add	a, r5
	da	a
	mov	r5, a
	mov	a, r1
	orl	a, #16
	movc	a, @a+dptr
	addc	a, r6
	da	a
	mov	r6, a

	mov	a, r2
	swap	a
	anl	a, #0x0F
	mov	r1, a
	mov	dptr, #_int2bcd_3
	movc	a, @a+dptr
	add	a, r5
	da	a
	mov	r5, a
	mov	a, r1
	orl	a, #16
	movc	a, @a+dptr
	addc	a, r6
	da	a
	mov	r6, a
	mov	a, r1
	orl	a, #32
	movc	a, @a+dptr
	addc	a, #0
	da	a
	mov	r7, a

	jb	_long_flag, printf_i2bcd_32	// if 16 bit int, we're done
	ret

printf_i2bcd_32:

#ifdef LONG
	mov	a, r3
	anl	a, #0x0F
	mov	r1, a
	mov	dptr, #_int2bcd_4
	movc	a, @a+dptr
	add	a, r5
	da	a
	mov	r5, a
	mov	a, r1
	orl	a, #16
	movc	a, @a+dptr
	addc	a, r6
	da	a
	mov	r6, a
	mov	a, r1
	orl	a, #32
	movc	a, @a+dptr
	addc	a, r7
	da	a
	mov	r7, a
	clr	a
	addc	a, #0
	mov	r2, a

	mov	a, r3
	swap	a
	anl	a, #0x0F
	mov	r1, a
	mov	dptr, #_int2bcd_5
	movc	a, @a+dptr
	add	a, r5
	da	a
	mov	r5, a
	mov	a, r1
	orl	a, #16
	movc	a, @a+dptr
	addc	a, r6
	da	a
	mov	r6, a
	mov	a, r1
	orl	a, #32
	movc	a, @a+dptr
	addc	a, r7
	da	a
	mov	r7, a
	mov	a, r1
	orl	a, #48
	movc	a, @a+dptr
	addc	a, r2
	da	a
	mov	r2, a

	mov	a, r4
	anl	a, #0x0F
	mov	r1, a
	mov	dptr, #_int2bcd_6
	mov	r3, #0
	lcall	printf_bcd_add10	// saves 27 bytes, costs 5 cycles

	mov	a, r4
	swap	a
	anl	a, #0x0F
	mov	r1, a
	mov	dptr, #_int2bcd_7
printf_bcd_add10:
	movc	a, @a+dptr
	add	a, r5
	da	a
	mov	r5, a
	mov	a, r1
	orl	a, #16
	movc	a, @a+dptr
	addc	a, r6
	da	a
	mov	r6, a
	mov	a, r1
	orl	a, #32
	movc	a, @a+dptr
	addc	a, r7
	da	a
	mov	r7, a
	mov	a, r1
	orl	a, #48
	movc	a, @a+dptr
	addc	a, r2
	da	a
	mov	r2, a
	mov	a, r1
	orl	a, #64
	movc	a, @a+dptr
	addc	a, r3
	da	a
	mov	r3, a
#endif // LONG
	ret


#else // not FAST_INTEGER

	/* convert binary number in r4/r3/r2/r1 into bcd packed number
	 * in r3/r2/r7/r6/r5.  The input number is destroyed in the
	 * process, to avoid needing extra memory for the result (and
	 * r1 gets used for temporary storage).  dptr is overwritten,
	 * but r0 is not changed.
	 */

#ifdef LONG

printf_int2bcd:
	mov	a, #8
	jb	_short_flag, printf_int2bcd_begin
	mov	a, #16
	jnb	_long_flag, printf_int2bcd_begin
	mov	a, #32
printf_int2bcd_begin:
	mov	b, a
	clr	a
	mov	r5, a
	mov	r6, a
	mov	r7, a
	mov	(_i2bcd_tmp + 0), a
	mov	(_i2bcd_tmp + 1), a
	mov	dptr, #_int2bcd
printf_i2bcd_loop:
	mov	a, r4
	rrc	a
	mov	r4, a
	mov	a, r3
	rrc	a
	mov	r3, a
	mov	a, r2
	rrc	a
	mov	r2, a
	mov	a, r1
	rrc	a
	mov	r1, a
	jnc	print_i2bcd_skip
	clr	a
	movc	a, @a+dptr
	add	a, r5
	da	a
	mov	r5, a
	mov	a, #1
	movc	a, @a+dptr
	addc	a, r6
	da	a
	mov	r6, a
	mov	a, #2
	movc	a, @a+dptr
	addc	a, r7
	da	a
	mov	r7, a
	mov	a, #3
	movc	a, @a+dptr
	addc	a, (_i2bcd_tmp + 0)
	da	a
	mov	(_i2bcd_tmp + 0), a
	mov	a, #4
	movc	a, @a+dptr
	addc	a, (_i2bcd_tmp + 1)
	da	a
	mov	(_i2bcd_tmp + 1), a
print_i2bcd_skip:
	inc	dptr
	inc	dptr
	inc	dptr
	inc	dptr
	inc	dptr
	djnz	b, printf_i2bcd_loop
	mov	r2, (_i2bcd_tmp + 0)
	mov	r3, (_i2bcd_tmp + 1)
	ret

#else //  not LONG

printf_int2bcd:
	mov	a, #8
	jb	_short_flag, printf_int2bcd_begin
	mov	a, #16
printf_int2bcd_begin:
	mov	b, a
	clr	a
	mov	r5, a
	mov	r6, a
	mov	r7, a
	mov	dptr, #_int2bcd
printf_i2bcd_loop:
	mov	a, r2
	rrc	a
	mov	r2, a
	mov	a, r1
	rrc	a
	mov	r1, a
	jnc	printf_i2bcd_add_skip
	clr	a
	movc	a, @a+dptr
	add	a, r5
	da	a
	mov	r5, a
	mov	a, #1
	movc	a, @a+dptr
	addc	a, r6
	da	a
	mov	r6, a
	mov	a, #2
	movc	a, @a+dptr
	addc	a, r7
	da	a
	mov	r7, a
printf_i2bcd_add_skip:
	inc	dptr
	inc	dptr
	inc	dptr
	djnz	b, printf_i2bcd_loop
	ret

#endif // not LONG


#endif // not FAST_INTEGER


#ifdef FIELD_WIDTH
printf_space_loop:
	//mov	a, #' '
	mov	a, #32
	jnb	_leading_zero_flag, printf_space_output
	//mov	a, #'0'
	mov	a, #48
printf_space_output:
	lcall	printf_putchar
	dec	_field_width
printf_space:
	mov	a, _field_width
	jnz	printf_space_loop
	ret
#endif

	/* print a hex digit, either upper 4 bit (msn) or lower 4 bits (lsn) */

printf_phex_msn:
	swap	a
printf_phex_lsn:
	anl	a, #15
	jnz	printf_phex_ok
	jnb	_print_zero_flag, printf_ret
printf_phex_ok:
	setb	_print_zero_flag
	add	a, #0x90
	da	a
	addc    a, #0x40
	da	a
printf_putchar:
#ifdef PUTCHAR_CALLEE_SAVES
	push	dph
	push	dpl
	mov	dpl, a
	lcall	_putchar
	pop	dpl
	pop	dph
#else
	push	dph
	push	dpl
	push	ar0
	mov	dpl, a
	lcall	_putchar
	pop	ar0
	pop	dpl
	pop	dph
#endif
printf_ret:
	ret

	/* print a zero if all the calls to print the digits ended up */
	/* being leading zeros */

printf_zero:
        jb	_print_zero_flag, printf_ret
        //mov	a, #'0'
        mov	a, #48
        ljmp	printf_putchar
  
printf_end:
	__endasm;
}


#ifdef FAST_INTEGER
/*
 * #! /usr/bin/perl
 * for ($d=0; $d < 8; $d++) {
 * 	$n = 16 ** $d;
 * 	for ($p=0; $p < 5; $p++) {
 * 		last unless (((16 ** $d) * 15) / (10 ** ($p * 2))) % 100;
 * 		printf "code unsigned char int2bcd_%d_%d[15] = {", $d, $p;
 * 		for ($i=0; $i < 16; $i++) {
 * 			printf "0x%02d",
 * 			   (((16 ** $d) * $i) / (10 ** ($p * 2))) % 100;
 * 			print ", " if $i < 15;
 * 		}
 * 		print "};\n";
 * 	}
 * }
 */

#if 0
static __code unsigned char int2bcd_0[] = {
0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15};

static __code unsigned char int2bcd_1[] = {
0x00, 0x16, 0x32, 0x48, 0x64, 0x80, 0x96, 0x12,
0x28, 0x44, 0x60, 0x76, 0x92, 0x08, 0x24, 0x40,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02};
#endif

static __code unsigned char int2bcd_2[] = {
0x00, 0x56, 0x12, 0x68, 0x24, 0x80, 0x36, 0x92,
0x48, 0x04, 0x60, 0x16, 0x72, 0x28, 0x84, 0x40,
0x00, 0x02, 0x05, 0x07, 0x10, 0x12, 0x15, 0x17,
0x20, 0x23, 0x25, 0x28, 0x30, 0x33, 0x35, 0x38};

static __code unsigned char int2bcd_3[] = {
0x00, 0x96, 0x92, 0x88, 0x84, 0x80, 0x76, 0x72,
0x68, 0x64, 0x60, 0x56, 0x52, 0x48, 0x44, 0x40,
0x00, 0x40, 0x81, 0x22, 0x63, 0x04, 0x45, 0x86,
0x27, 0x68, 0x09, 0x50, 0x91, 0x32, 0x73, 0x14,
0x00, 0x00, 0x00, 0x01, 0x01, 0x02, 0x02, 0x02,
0x03, 0x03, 0x04, 0x04, 0x04, 0x05, 0x05, 0x06};

#ifdef LONG
static __code unsigned char int2bcd_4[] = {
0x00, 0x36, 0x72, 0x08, 0x44, 0x80, 0x16, 0x52,
0x88, 0x24, 0x60, 0x96, 0x32, 0x68, 0x04, 0x40,
0x00, 0x55, 0x10, 0x66, 0x21, 0x76, 0x32, 0x87,
0x42, 0x98, 0x53, 0x08, 0x64, 0x19, 0x75, 0x30,
0x00, 0x06, 0x13, 0x19, 0x26, 0x32, 0x39, 0x45,
0x52, 0x58, 0x65, 0x72, 0x78, 0x85, 0x91, 0x98};

static __code unsigned char int2bcd_5[] = {
0x00, 0x76, 0x52, 0x28, 0x04, 0x80, 0x56, 0x32,
0x08, 0x84, 0x60, 0x36, 0x12, 0x88, 0x64, 0x40,
0x00, 0x85, 0x71, 0x57, 0x43, 0x28, 0x14, 0x00,
0x86, 0x71, 0x57, 0x43, 0x29, 0x14, 0x00, 0x86,
0x00, 0x04, 0x09, 0x14, 0x19, 0x24, 0x29, 0x34,
0x38, 0x43, 0x48, 0x53, 0x58, 0x63, 0x68, 0x72,
0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15};

static __code unsigned char int2bcd_6[] = {
0x00, 0x16, 0x32, 0x48, 0x64, 0x80, 0x96, 0x12,
0x28, 0x44, 0x60, 0x76, 0x92, 0x08, 0x24, 0x40,
0x00, 0x72, 0x44, 0x16, 0x88, 0x60, 0x32, 0x05,
0x77, 0x49, 0x21, 0x93, 0x65, 0x38, 0x10, 0x82,
0x00, 0x77, 0x55, 0x33, 0x10, 0x88, 0x66, 0x44,
0x21, 0x99, 0x77, 0x54, 0x32, 0x10, 0x88, 0x65,
0x00, 0x16, 0x33, 0x50, 0x67, 0x83, 0x00, 0x17,
0x34, 0x50, 0x67, 0x84, 0x01, 0x18, 0x34, 0x51,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02};

static __code unsigned char int2bcd_7[] = {
0x00, 0x56, 0x12, 0x68, 0x24, 0x80, 0x36, 0x92,
0x48, 0x04, 0x60, 0x16, 0x72, 0x28, 0x84, 0x40,
0x00, 0x54, 0x09, 0x63, 0x18, 0x72, 0x27, 0x81,
0x36, 0x91, 0x45, 0x00, 0x54, 0x09, 0x63, 0x18,
0x00, 0x43, 0x87, 0x30, 0x74, 0x17, 0x61, 0x04,
0x48, 0x91, 0x35, 0x79, 0x22, 0x66, 0x09, 0x53,
0x00, 0x68, 0x36, 0x05, 0x73, 0x42, 0x10, 0x79,
0x47, 0x15, 0x84, 0x52, 0x21, 0x89, 0x58, 0x26,
0x00, 0x02, 0x05, 0x08, 0x10, 0x13, 0x16, 0x18,
0x21, 0x24, 0x26, 0x29, 0x32, 0x34, 0x37, 0x40};
#endif // LONG

#else // not FAST_INTEGER

/*
 * #! /usr/bin/perl
 * print "__code unsigned char int2bcd[] = {\n";
 * for ($i=0, $n=1; $i<32; $i++, $n*=2) {
 * 	$r = sprintf "%010u", $n;
 * 	$r =~ /([0-9][0-9])([0-9][0-9])([0-9][0-9])([0-9][0-9])([0-9][0-9])/;
 * 	printf "0x%02d, 0x%02d, 0x%02d, 0x%02d, 0x%02d", $5, $4, $3, $2, $1;
 * 	print ',' if $i < 31;
 * 	printf "\t\t// %10u\n", $n;
 * }
 * print "}\n__code unsigned char int2bcd[] = {\n";
 * for ($i=0, $n=1; $i<16; $i++, $n*=2) {
 * 	$r = sprintf "%06u", $n;
 * 	$r =~ /([0-9][0-9])([0-9][0-9])([0-9][0-9])/;
 * 	printf "0x%02d, 0x%02d, 0x%02d", $3, $2, $1;
 * 	print ',' if $i < 15;
 * 	printf "\t\t// %10u\n", $n;
 * }
 * print "};\n";
*/

#ifdef LONG
static __code unsigned char int2bcd[] = {
0x01, 0x00, 0x00, 0x00, 0x00,		//          1
0x02, 0x00, 0x00, 0x00, 0x00,		//          2
0x04, 0x00, 0x00, 0x00, 0x00,		//          4
0x08, 0x00, 0x00, 0x00, 0x00,		//          8
0x16, 0x00, 0x00, 0x00, 0x00,		//         16
0x32, 0x00, 0x00, 0x00, 0x00,		//         32
0x64, 0x00, 0x00, 0x00, 0x00,		//         64
0x28, 0x01, 0x00, 0x00, 0x00,		//        128
0x56, 0x02, 0x00, 0x00, 0x00,		//        256
0x12, 0x05, 0x00, 0x00, 0x00,		//        512
0x24, 0x10, 0x00, 0x00, 0x00,		//       1024
0x48, 0x20, 0x00, 0x00, 0x00,		//       2048
0x96, 0x40, 0x00, 0x00, 0x00,		//       4096
0x92, 0x81, 0x00, 0x00, 0x00,		//       8192
0x84, 0x63, 0x01, 0x00, 0x00,		//      16384
0x68, 0x27, 0x03, 0x00, 0x00,		//      32768
0x36, 0x55, 0x06, 0x00, 0x00,		//      65536
0x72, 0x10, 0x13, 0x00, 0x00,		//     131072
0x44, 0x21, 0x26, 0x00, 0x00,		//     262144
0x88, 0x42, 0x52, 0x00, 0x00,		//     524288
0x76, 0x85, 0x04, 0x01, 0x00,		//    1048576
0x52, 0x71, 0x09, 0x02, 0x00,		//    2097152
0x04, 0x43, 0x19, 0x04, 0x00,		//    4194304
0x08, 0x86, 0x38, 0x08, 0x00,		//    8388608
0x16, 0x72, 0x77, 0x16, 0x00,		//   16777216
0x32, 0x44, 0x55, 0x33, 0x00,		//   33554432
0x64, 0x88, 0x10, 0x67, 0x00,		//   67108864
0x28, 0x77, 0x21, 0x34, 0x01,		//  134217728
0x56, 0x54, 0x43, 0x68, 0x02,		//  268435456
0x12, 0x09, 0x87, 0x36, 0x05,		//  536870912
0x24, 0x18, 0x74, 0x73, 0x10,		// 1073741824
0x48, 0x36, 0x48, 0x47, 0x21		// 2147483648
};
#else // not LONG
static __code unsigned char int2bcd[] = {
0x01, 0x00, 0x00,		//          1
0x02, 0x00, 0x00,		//          2
0x04, 0x00, 0x00,		//          4
0x08, 0x00, 0x00,		//          8
0x16, 0x00, 0x00,		//         16
0x32, 0x00, 0x00,		//         32
0x64, 0x00, 0x00,		//         64
0x28, 0x01, 0x00,		//        128
0x56, 0x02, 0x00,		//        256
0x12, 0x05, 0x00,		//        512
0x24, 0x10, 0x00,		//       1024
0x48, 0x20, 0x00,		//       2048
0x96, 0x40, 0x00,		//       4096
0x92, 0x81, 0x00,		//       8192
0x84, 0x63, 0x01,		//      16384
0x68, 0x27, 0x03		//      32768
};
#endif // not LONG

#endif // not FAST_INTEGER


#ifdef FLOAT
#ifndef FLOAT_FIXED4

/*
 * #! /usr/bin/perl
 * for ($i=0, $f=0.5; $i<24; $i++) {
 * 	$r = sprintf "%.8f", $f;
 * 	$r =~ /0\.([0-9][0-9])([0-9][0-9])([0-9][0-9])([0-9][0-9])/;
 * 	printf "0x%02d, 0x%02d, 0x%02d, 0x%02d", $4, $3, $2, $1;
 * 	print ',' if $i < 23;
 * 	$sum += $r;
 * 	printf "\t\t// %.15f  %.8f\n", $f, $sum;
 * 	$f /= 2;
 * }
 */

static __code unsigned char frac2bcd[] = {
0x00, 0x00, 0x00, 0x50,		// 0.500000000000000  0.50000000
0x00, 0x00, 0x00, 0x25,		// 0.250000000000000  0.75000000
0x00, 0x00, 0x50, 0x12,		// 0.125000000000000  0.87500000
0x00, 0x00, 0x25, 0x06,		// 0.062500000000000  0.93750000
0x00, 0x50, 0x12, 0x03,		// 0.031250000000000  0.96875000
0x00, 0x25, 0x56, 0x01,		// 0.015625000000000  0.98437500
0x50, 0x12, 0x78, 0x00,		// 0.007812500000000  0.99218750
0x25, 0x06, 0x39, 0x00,		// 0.003906250000000  0.99609375
0x12, 0x53, 0x19, 0x00,		// 0.001953125000000  0.99804687
0x56, 0x76, 0x09, 0x00,		// 0.000976562500000  0.99902343
0x28, 0x88, 0x04, 0x00,		// 0.000488281250000  0.99951171
0x14, 0x44, 0x02, 0x00,		// 0.000244140625000  0.99975585
0x07, 0x22, 0x01, 0x00,		// 0.000122070312500  0.99987792
0x04, 0x61, 0x00, 0x00,		// 0.000061035156250  0.99993896
0x52, 0x30, 0x00, 0x00,		// 0.000030517578125  0.99996948
0x26, 0x15, 0x00, 0x00,		// 0.000015258789062  0.99998474
0x63, 0x07, 0x00, 0x00,		// 0.000007629394531  0.99999237
0x81, 0x03, 0x00, 0x00,		// 0.000003814697266  0.99999618
0x91, 0x01, 0x00, 0x00,		// 0.000001907348633  0.99999809
0x95, 0x00, 0x00, 0x00,		// 0.000000953674316  0.99999904
0x48, 0x00, 0x00, 0x00,		// 0.000000476837158  0.99999952
0x24, 0x00, 0x00, 0x00,		// 0.000000238418579  0.99999976
0x12, 0x00, 0x00, 0x00,		// 0.000000119209290  0.99999988
0x06, 0x00, 0x00, 0x00,		// 0.000000059604645  0.99999994
0x03, 0x00, 0x00, 0x00,		// 0.000000029802322  0.99999997
0x01, 0x00, 0x00, 0x00,		// 0.000000014901161  0.99999998
0x01, 0x00, 0x00, 0x00		// 0.000000007450581  0.99999999
};

#ifndef FLOAT_DEFAULT_FRAC_DIGITS
// TODO: Perhaps these should be tweaked a bit to take round up
// effects into account... or maybe give more default digits??
// Range		#digits
// 0.0001 - 0.0009999	7
// 0.001 - 0.009999	6	0.001 = 0x3A83126F  3A83
// 0.01 - 0.09999	5	0.01  = 0x3C23D70A  3C23
// 0.1 - 9.9999		4	0.1   = 0x3DCCCCCD, 3DCC
// 10.0 - 99.99		3	10.0  = 0x41200000  4120
// 100.0 - 999.99	2	100.0 = 0x42C80000  42C8
// 1000 - 9999.9	1	1000  = 0x447A0000  447A
// 10000+		0	10000 = 0x461C4000  461C
static __code unsigned int float_range_table[] = {
65536 - 0x3A83,
65536 - 0x3C23,
65536 - 0x3DCC,
65536 - 0x4120,
65536 - 0x42C8,
65536 - 0x447A,
65536 - 0x461C
};
#endif

#else // using FLOAT_FIXED4

/*
* #! /usr/bin/perl
*     for ($i=0, $f=0.5; $i<14; $i++) {
*     $r = sprintf "%.4f", $f;
*     $r =~ /0\.([0-9][0-9])([0-9][0-9])/;
*     printf "0x%02d, 0x%02d", $2, $1;
*     print ',' if $i < 13;
*     $sum += $r;
*     printf "\t\t// %.15f  %.4f\n", $f, $sum;
*     $f /= 2;
* }
*/

static __code unsigned char frac2bcd[] = {
0x00, 0x50,             // 0.500000000000000  0.5000
0x00, 0x25,             // 0.250000000000000  0.7500
0x50, 0x12,             // 0.125000000000000  0.8750
0x25, 0x06,             // 0.062500000000000  0.9375
0x12, 0x03,             // 0.031250000000000  0.9687
0x56, 0x01,             // 0.015625000000000  0.9843
0x78, 0x00,             // 0.007812500000000  0.9921
0x39, 0x00,             // 0.003906250000000  0.9960
0x20, 0x00,             // 0.001953125000000  0.9980
0x10, 0x00,             // 0.000976562500000  0.9990
0x05, 0x00,             // 0.000488281250000  0.9995
0x02, 0x00,             // 0.000244140625000  0.9997
0x01, 0x00,             // 0.000122070312500  0.9998
0x01, 0x00              // 0.000061035156250  0.9999
};

#endif // FLOAT_FIXED4
#endif // FLOAT


#endif // defines compatible with printf_fast
