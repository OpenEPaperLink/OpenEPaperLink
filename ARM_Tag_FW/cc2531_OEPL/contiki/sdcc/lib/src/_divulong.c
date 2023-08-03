/*-------------------------------------------------------------------------
   _divulong.c - routine for division of 32 bit unsigned long

   Copyright (C) 1999, Jean-Louis Vern <jlvern AT gmail.com>

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

/*   Assembler-functions are provided for:
     mcs51 small
     mcs51 small stack-auto
*/

#include <stdbool.h>

#if !defined(__SDCC_USE_XSTACK) && !defined(_SDCC_NO_ASM_LIB_FUNCS)
#  if defined(__SDCC_mcs51)
#    if defined(__SDCC_MODEL_SMALL)
#      if defined(__SDCC_STACK_AUTO) && !defined(__SDCC_PARMS_IN_BANK1)
#        define _DIVULONG_ASM_SMALL_AUTO
#      else
#        define _DIVULONG_ASM_SMALL
#      endif
#    endif
#  endif
#endif

#if defined _DIVULONG_ASM_SMALL

static void
_divlong_dummy (void) __naked
{
	__asm

	.globl __divulong

__divulong:

	#define count   r2

	#define x0	dpl
	#define x1	dph
	#define x2	b
	#define x3	r3

	#define reste0	r4
	#define reste1	r5
	#define reste2	r6
	#define reste3	r7
#if !defined(__SDCC_PARMS_IN_BANK1)

#if defined(__SDCC_NOOVERLAY)
	.area DSEG    (DATA)
#else
	.area OSEG    (OVR,DATA)
#endif

	.globl __divulong_PARM_2
	.globl __divslong_PARM_2

__divulong_PARM_2:
__divslong_PARM_2:
	.ds	4

	.area CSEG    (CODE)

	#define y0      (__divulong_PARM_2)
	#define y1      (__divulong_PARM_2 + 1)
	#define y2      (__divulong_PARM_2 + 2)
	#define y3      (__divulong_PARM_2 + 3)
#else
	#define y0      (b1_0)
	#define y1      (b1_1)
	#define y2      (b1_2)
	#define y3      (b1_3)
#endif // !__SDCC_PARMS_IN_BANK1
				; parameter x comes in a, b, dph, dpl
	mov	x3,a		; save parameter x3

	mov	count,#32
	clr	a
	mov	reste0,a
	mov	reste1,a
	mov	reste2,a
	mov	reste3,a

	; optimization  loop in lp0 until the first bit is shifted into rest

lp0:	mov	a,x0		; x <<= 1
	add	a,x0
	mov	x0,a
	mov	a,x1
	rlc	a
	mov	x1,a
	mov	a,x2
	rlc	a
	mov	x2,a
	mov	a,x3
	rlc	a
	mov	x3,a

	jc	in_lp
	djnz	count,lp0

	sjmp	exit

loop:	mov	a,x0		; x <<= 1
	add	a,x0
	mov	x0,a
	mov	a,x1
	rlc	a
	mov	x1,a
	mov	a,x2
	rlc	a
	mov	x2,a
	mov	a,x3
	rlc	a
	mov	x3,a

in_lp:	mov	a,reste0	; reste <<= 1
	rlc	a		; feed in carry
	mov	reste0,a
	mov	a,reste1
	rlc	a
	mov	reste1,a
	mov	a,reste2
	rlc	a
	mov	reste2,a
	mov	a,reste3
	rlc	a
	mov	reste3,a

	mov	a,reste0	; reste - y
	subb	a,y0		; carry is always clear here, because
					; reste <<= 1 never overflows
	mov	a,reste1
	subb	a,y1
	mov	a,reste2
	subb	a,y2
	mov	a,reste3
	subb	a,y3

	jc	minus		; reste >= y?

				; -> yes;  reste -= y;
	mov	a,reste0
	subb	a,y0		; carry is always clear here (jc)
	mov	reste0,a
	mov	a,reste1
	subb	a,y1
	mov	reste1,a
	mov	a,reste2
	subb	a,y2
	mov	reste2,a
	mov	a,reste3
	subb	a,y3
	mov	reste3,a

	orl	x0,#1

minus:	djnz	count,loop	; -> no

exit:	mov	a,x3		; prepare the return value
	ret

	__endasm;
}

#elif defined _DIVULONG_ASM_SMALL_AUTO

static void
_divlong_dummy (void) __naked
{
	__asm

	.globl __divulong

__divulong:

	#define count   r2

	#define x0	dpl
	#define x1	dph
	#define x2	b
	#define x3	r3

	#define reste0	r4
	#define reste1	r5
	#define reste2	r6
	#define reste3	r7

	.globl __divlong	; entry point for __divslong

	#define y0	r1

				; parameter x comes in a, b, dph, dpl
	mov	x3,a		; save parameter x3

	mov	a,sp
	add	a,#-2-3		; 2 bytes return address, 3 bytes param y
	mov	r0,a		; r0 points to y0

__divlong:			; entry point for __divslong

	mov	a,@r0		; load y0
	mov	y0,a
	inc	r0		; r0 points to y1

	mov	count,#32
	clr	a
	mov	reste0,a
	mov	reste1,a
	mov	reste2,a
	mov	reste3,a

	; optimization  loop in lp0 until the first bit is shifted into rest

lp0:	mov	a,x0		; x <<= 1
	add	a,x0
	mov	x0,a
	mov	a,x1
	rlc	a
	mov	x1,a
	mov	a,x2
	rlc	a
	mov	x2,a
	mov	a,x3
	rlc	a
	mov	x3,a

	jc	in_lp
	djnz	count,lp0

	sjmp	exit

loop:	mov	a,x0		; x <<= 1
	add	a,x0
	mov	x0,a
	mov	a,x1
	rlc	a
	mov	x1,a
	mov	a,x2
	rlc	a
	mov	x2,a
	mov	a,x3
	rlc	a
	mov	x3,a

in_lp:	mov	a,reste0	; reste <<= 1
	rlc	a		; feed in carry
	mov	reste0,a
	mov	a,reste1
	rlc	a
	mov	reste1,a
	mov	a,reste2
	rlc	a
	mov	reste2,a
	mov	a,reste3
	rlc	a
	mov	reste3,a

	mov	a,reste0	; reste - y
	subb	a,y0		; carry is always clear here, because
				; reste <<= 1 never overflows
	mov	a,reste1
	subb	a,@r0		; y1
	mov	a,reste2
	inc	r0
	subb	a,@r0		; y2
	mov	a,reste3
	inc	r0
	subb	a,@r0		; y3
	dec	r0
	dec	r0

	jc	minus		; reste >= y?

				; -> yes;  reste -= y;
	mov	a,reste0
	subb	a,y0		; carry is always clear here (jc)
	mov	reste0,a
	mov	a,reste1
	subb	a,@r0		; y1
	mov	reste1,a
	mov	a,reste2
	inc	r0
	subb	a,@r0		; y2
	mov	reste2,a
	mov	a,reste3
	inc	r0
	subb	a,@r0		; y3
	mov	reste3,a
	dec	r0
	dec	r0

	orl	x0,#1

minus:	djnz	count,loop	; -> no

exit:	mov	a,x3		; prepare the return value
	ret

	__endasm;
}

#else // _DIVULONG_ASM

#define MSB_SET(x) ((x >> (8*sizeof(x)-1)) & 1)

unsigned long
_divulong (unsigned long x, unsigned long y)
{
  unsigned long reste = 0L;
  unsigned char count = 32;
  bool c;

  do
  {
    // reste: x <- 0;
    c = MSB_SET(x);
    x <<= 1;
    reste <<= 1;
    if (c)
      reste |= 1L;

    if (reste >= y)
    {
      reste -= y;
      // x <- (result = 1)
      x |= 1L;
    }
  }
  while (--count);
  return x;
}

#endif // _DIVULONG_ASM
