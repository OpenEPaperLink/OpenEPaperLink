/*-------------------------------------------------------------------------
   _modsint.c - routine for signed int (16 bit) modulus

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
unsigned unsigned _moduint (unsigned a, unsigned b);
#endif

/*   Assembler-functions are provided for:
     mcs51 small
     mcs51 small stack-auto
*/

#if !defined(__SDCC_USE_XSTACK) && !defined(_SDCC_NO_ASM_LIB_FUNCS)
#  if defined(__SDCC_mcs51)
#    if defined(__SDCC_MODEL_SMALL)
#      if defined(__SDCC_STACK_AUTO) && !defined(__SDCC_PARMS_IN_BANK1)
#        define _MODSINT_ASM_SMALL_AUTO
#      else
#        define _MODSINT_ASM_SMALL
#      endif
#    endif
#  endif
#endif

#if defined _MODSINT_ASM_SMALL

static void
_modsint_dummy (void) __naked
{
	__asm

	#define a0	dpl
	#define a1	dph

	.globl __modsint
#if defined(__SDCC_PARMS_IN_BANK1)
	#define b0      (b1_0)
	#define b1      (b1_1)
#else
	// _modsint_PARM_2 shares the same memory with _moduint_PARM_2
	// and is defined in _moduint.c
	#define b0      (__modsint_PARM_2)
	#define b1      (__modsint_PARM_2 + 1)
#endif
__modsint:
				; a1 in dph
				; b1 in (__modsint_PARM_2 + 1)

	clr	F0 		; Flag 0 in PSW
				; available to user for general purpose
	mov	a,a1
	jnb	acc.7,a_not_negative

	setb	F0

	clr	a
	clr	c
	subb	a,a0
	mov	a0,a
	clr	a
	subb	a,a1
	mov	a1,a

a_not_negative:

	mov	a,b1
	jnb	acc.7,b_not_negative

	clr	a
	clr	c
	subb	a,b0
	mov	b0,a
	clr	a
	subb	a,b1
	mov	b1,a

b_not_negative:

	lcall	__moduint

	jnb	F0,not_negative

	clr	a
	clr	c
	subb	a,a0
	mov	a0,a
	clr	a
	subb	a,a1
	mov	a1,a

not_negative:
	ret

	__endasm;
}

#elif defined _MODSINT_ASM_SMALL_AUTO

static void
_modsint_dummy (void) __naked
{
	__asm

	#define a0	dpl
	#define a1	dph

	ar0 = 0			; BUG register set is not considered
	ar1 = 1

	.globl __modsint

__modsint:

	clr	F0 		; Flag 0 in PSW
				; available to user for general purpose
	mov	a,a1
	jnb	acc.7,a_not_negative

	setb	F0

	clr	a
	clr	c
	subb	a,a0
	mov	a0,a
	clr	a
	subb	a,a1
	mov	a1,a

a_not_negative:

	mov	a,sp
	add	a,#-2		; 2 bytes return address
	mov	r0,a		; r0 points to b1
	mov	a,@r0		; b1

	jnb	acc.7,b_not_negative

	dec	r0

	clr	a
	clr	c
	subb	a,@r0		; b0
	mov	@r0,a
	clr	a
	inc	r0
	subb	a,@r0		; b1
	mov	@r0,a

b_not_negative:

	mov	ar1,@r0		; b1
	dec	r0
	mov	ar0,@r0		; b0

	lcall	__modint

	jnb	F0,not_negative

	clr	a
	clr	c
	subb	a,a0
	mov	a0,a
	clr	a
	subb	a,a1
	mov	a1,a

not_negative:
	ret

	__endasm;
}

#else  // _MODSINT_ASM_

int _modsint (int a, int b)
{
  register int r;

  r = (unsigned)(a < 0 ? -a : a) % (unsigned)(b < 0 ? -b : b);

  if (a < 0)
    return -r;
  else
    return r;
}

#endif  // _MODSINT_ASM_
