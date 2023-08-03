/*-------------------------------------------------------------------------
   _divslong.c - routine for division of 32 bit long

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


#include <sdcc-lib.h>

#if _SDCC_MANGLES_SUPPORT_FUNS
unsigned long _divulong (unsigned long x, unsigned long y);
#endif

/*   Assembler-functions are provided for:
     mcs51 small
     mcs51 small stack-auto
*/

#if !defined(__SDCC_USE_XSTACK) && !defined(_SDCC_NO_ASM_LIB_FUNCS)
#  if defined(__SDCC_mcs51)
#    if defined(__SDCC_MODEL_SMALL)
#      if defined(__SDCC_STACK_AUTO) && !defined(__SDCC_PARMS_IN_BANK1)
#        define _DIVSLONG_ASM_SMALL_AUTO
#      else
#        define _DIVSLONG_ASM_SMALL
#      endif
#    endif
#  endif
#endif

#if defined _DIVSLONG_ASM_SMALL

static void
_divslong_dummy (void) __naked
{
	__asm

	#define x0	dpl
	#define x1	dph
	#define x2	b
	#define x3	r3

	.globl __divslong

	// _divslong_PARM_2 shares the same memory with _divulong_PARM_2
	// and is defined in _divulong.c
#if defined(__SDCC_PARMS_IN_BANK1)
	#define y0      (b1_0)
	#define y1      (b1_1)
	#define y2      (b1_2)
	#define y3      (b1_3)
#else
	#define y0      (__divslong_PARM_2)
	#define y1      (__divslong_PARM_2 + 1)
	#define y2      (__divslong_PARM_2 + 2)
	#define y3      (__divslong_PARM_2 + 3)
#endif
__divslong:
				; x3 in acc
				; y3 in (__divslong_PARM_2 + 3)
	mov	x3,a		; save x3

	clr	F0 		; Flag 0 in PSW
				; available to user for general purpose
	jnb	acc.7,a_not_negative

	setb	F0

	clr	a
	clr	c
	subb	a,x0
	mov	x0,a
	clr	a
	subb	a,x1
	mov	x1,a
	clr	a
	subb	a,x2
	mov	x2,a
	clr	a
	subb	a,x3
	mov	x3,a

a_not_negative:

	mov	a,y3
	jnb	acc.7,b_not_negative

	cpl	F0

	clr	a
	clr	c
	subb	a,y0
	mov	y0,a
	clr	a
	subb	a,y1
	mov	y1,a
	clr	a
	subb	a,y2
	mov	y2,a
	clr	a
	subb	a,y3
	mov	y3,a

b_not_negative:

	mov	a,x3		; restore x3 in acc

	lcall	__divulong

	jnb	F0,not_negative

	mov	x3,a		; save x3

	clr	a
	clr	c
	subb	a,x0
	mov	x0,a
	clr	a
	subb	a,x1
	mov	x1,a
	clr	a
	subb	a,x2
	mov	x2,a
	clr	a
	subb	a,x3	; x3 ends in acc

not_negative:
	ret

	__endasm;
}

#elif defined _DIVSLONG_ASM_SMALL_AUTO

static void
_divslong_dummy (void) __naked
{
	__asm

	#define x0	dpl
	#define x1	dph
	#define x2	b
	#define x3	r3

	.globl __divslong

__divslong:

				; x3 in acc
	mov	x3,a		; save x3

	clr	F0 		; Flag 0 in PSW
				; available to user for general purpose
	jnb	acc.7,a_not_negative

	setb	F0

	clr	a
	clr	c
	subb	a,x0
	mov	x0,a
	clr	a
	subb	a,x1
	mov	x1,a
	clr	a
	subb	a,x2
	mov	x2,a
	clr	a
	subb	a,x3
	mov	x3,a

a_not_negative:

	mov	a,sp
	add	a,#-2		; 2 bytes return address
	mov	r0,a		; r0 points to y3
	mov	a,@r0		; y3

	jnb	acc.7,b_not_negative

	cpl	F0

	dec	r0
	dec	r0
	dec	r0

	clr	a
	clr	c
	subb	a,@r0		; y0
	mov	@r0,a
	clr	a
	inc	r0
	subb	a,@r0		; y1
	mov	@r0,a
	clr	a
	inc	r0
	subb	a,@r0		; y2
	mov	@r0,a
	clr	a
	inc	r0
	subb	a,@r0		; y3
	mov	@r0,a

b_not_negative:
	dec	r0
	dec	r0
	dec	r0		; r0 points to y0

	lcall	__divlong

	jnb	F0,not_negative

	mov	x3,a		; save x3

	clr	a
	clr	c
	subb	a,x0
	mov	x0,a
	clr	a
	subb	a,x1
	mov	x1,a
	clr	a
	subb	a,x2
	mov	x2,a
	clr	a
	subb	a,x3		; x3 ends in acc

not_negative:
	ret

	__endasm;
}

#else // _DIVSLONG_ASM

long
_divslong (long x, long y)
{
  long r;

  r = (unsigned long)(x < 0 ? -x : x) / (unsigned long)(y < 0 ? -y : y);
  if ((x < 0) ^ (y < 0))
    return -r;
  else
    return r;
}

#endif // _DIVSLONG_ASM
