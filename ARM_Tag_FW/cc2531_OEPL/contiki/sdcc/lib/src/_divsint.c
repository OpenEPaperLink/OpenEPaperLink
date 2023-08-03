/*-------------------------------------------------------------------------
   _divsint.c :- routine for signed int (16 bit) division. just calls
                 routine for unsigned division after sign adjustment

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
unsigned unsigned _divuint (unsigned x, unsigned y);
#endif

/*   Assembler-functions are provided for:
     mcs51 small
     mcs51 small stack-auto
*/

#if !defined(__SDCC_USE_XSTACK) && !defined(_SDCC_NO_ASM_LIB_FUNCS)
#  if defined(__SDCC_mcs51)
#    if defined(__SDCC_MODEL_SMALL)
#      if defined(__SDCC_STACK_AUTO) && !defined(__SDCC_PARMS_IN_BANK1)
#        define _DIVSINT_ASM_SMALL_AUTO
#      else
#        define _DIVSINT_ASM_SMALL
#      endif
#    endif
#  endif
#endif

#if defined _DIVSINT_ASM_SMALL

static void
_divsint_dummy (void) __naked
{
	__asm

	#define xl	dpl
	#define xh	dph

	.globl __divsint

	// _divsint_PARM_2 shares the same memory with _divuint_PARM_2
	// and is defined in _divuint.c
#if defined(__SDCC_PARMS_IN_BANK1)
	#define yl      (b1_0)
	#define yh      (b1_1)
#else
	#define yl      (__divsint_PARM_2)
	#define yh      (__divsint_PARM_2 + 1)
#endif
__divsint:
				; xh in dph
				; yh in (__divsint_PARM_2 + 1)

	clr	F0 		; Flag 0 in PSW
				; available to user for general purpose
	mov	a,xh
	jnb	acc.7,a_not_negative

	setb	F0

	clr	a
	clr	c
	subb	a,xl
	mov	xl,a
	clr	a
	subb	a,xh
	mov	xh,a

a_not_negative:

	mov	a,yh
	jnb	acc.7,b_not_negative

	cpl	F0

	clr	a
	clr	c
	subb	a,yl
	mov	yl,a
	clr	a
	subb	a,yh
	mov	yh,a

b_not_negative:

	lcall	__divuint

	jnb	F0,not_negative

	clr	a
	clr	c
	subb	a,xl
	mov	xl,a
	clr	a
	subb	a,xh
	mov	xh,a

not_negative:
	ret

	__endasm;
}

#elif defined _DIVSINT_ASM_SMALL_AUTO

static void
_divsint_dummy (void) __naked
{
	__asm

	#define xl	dpl
	#define xh	dph

	.globl __divsint

__divsint:

	clr	F0 		; Flag 0 in PSW
				; available to user for general purpose
	mov	a,xh
	jnb	acc.7,a_not_negative

	setb	F0

	clr	a
	clr	c
	subb	a,xl
	mov	xl,a
	clr	a
	subb	a,xh
	mov	xh,a

a_not_negative:

	mov	a,sp
	add	a,#-2		; 2 bytes return address
	mov	r0,a		; r0 points to yh
	mov	a,@r0		; a = yh

	jnb	acc.7,b_not_negative

	cpl	F0

	dec	r0

	clr	a
	clr	c
	subb	a,@r0		; yl
	mov	@r0,a
	clr	a
	inc	r0
	subb	a,@r0		; a = yh

b_not_negative:

	mov	r1,a		; yh
	dec	r0
	mov	a,@r0		; yl
	mov	r0,a

	lcall	__divint

	jnb	F0,not_negative

	clr	a
	clr	c
	subb	a,xl
	mov	xl,a
	clr	a
	subb	a,xh
	mov	xh,a

not_negative:
	ret

	__endasm;
}

#else  // _DIVSINT_ASM_

int
_divsint (int x, int y)
{
  register int r;

  r = (unsigned int)(x < 0 ? -x : x) / (unsigned int)(y < 0 ? -y : y);
  if ((x < 0) ^ (y < 0))
    return -r;
  else
    return r;
}

#endif  // _DIVSINT_ASM_
