#include "asmUtil.h"
#include "cpu.h"


#pragma callee_saves mathPrvU8bitswap
uint16_t mathPrvU16from2xU8(uint8_t hi, uint8_t lo) __reentrant __naked
{
	__asm__(
		"	mov   DPH, DPL			\n"
		"	pop   B					\n"
		"	pop   A					\n"
		"	pop   DPL				\n"
		"	push  DPL				\n"
		"	push  A					\n"
		"	push  B					\n"
		"	ret						\n"
	);
	(void)hi;
	(void)lo;
}

//a is hi
uint32_t mathPrvU32from4xU8(uint8_t hi, uint8_t midhi, uint8_t midlo, uint8_t lo) __reentrant __naked
{
	__asm__(
		"	push  _R0				\n"
		"	mov   A, #-5			\n"	//point to first pushed arg (last param - lo)
		"	add   A, sp				\n"
		"	mov   R0, A				\n"
		"	mov   A, DPL			\n"
		"	mov   DPL, @R0			\n"
		"	inc   R0				\n"
		"	mov   DPH, @R0			\n"
		"	inc   R0				\n"
		"	mov   B, @R0			\n"
		"	pop   _R0				\n"
		"	ret						\n"
	);
	(void)hi;
	(void)midhi;
	(void)midlo;
	(void)lo;
}


#pragma callee_saves mathPrvU8bitswap
uint8_t mathPrvU8bitswap(uint8_t val) __reentrant __naked
{
	__asm__(
		"	mov   B, #8			\n"
		"00003$:				\n"
		"	xch   A, DPL		\n"
		"	rrc   A				\n"
		"	xch   A, DPL		\n"
		"	rlc   A				\n"
		"	djnz  B, 00003$		\n"
		"	xch   A, DPL		\n"
		"	ret					\n"
	);
	(void)val;
}


#pragma callee_saves mathPrvI16Asr1
int16_t mathPrvI16Asr1(int16_t val) __reentrant __naked
{
	__asm__(
		"	mov A, DPH		\n"
		"	mov C, A.7		\n"
		"	rrc A			\n"
		"	mov DPH, A		\n"
		"	mov A, DPL		\n"
		"	rrc A			\n"
		"	mov DPL, A		\n"
		"	ret				\n"
	);
	(void)val;
}

#pragma callee_saves mathPrvMul8x8
uint16_t mathPrvMul8x8(uint8_t a, uint8_t b) __reentrant __naked
{
	//return expected in DPTR, first param is in DPL, second on stack
	__asm__(
		//grab param into B
		"	pop  DPH		\n\t"
		"	pop  A			\n\t"
		"	pop  B			\n\t"
		"	push B			\n\t"
		"	push A			\n\t"
		"	push DPH		\n\t"
		
		//second param into A
		"	mov  A, DPL		\n\t"
		
		//do the deed
		"	mul  AB			\n\t"
		
		//return results
		"	mov  DPL, A		\n\t"
		"	mov  DPH, B		\n\t"
		
		"	ret				\n\t"
	);
	(void)a;
	(void)b;
}

#pragma callee_saves mathPrvMul16x8
uint32_t mathPrvMul16x8(uint16_t a, uint8_t b) __reentrant __naked
{
	//return expected in A:B:DPTR, first param is in DPTR, second on stack
	__asm__(
		//save r0
		"	push _R0		\n\t"
		
		//get 2nd param into a & r0
		"	mov  A, SP		\n\t"
		"	add  A, #-0x03	\n\t"
		"	mov  R0, A		\n\t"
		"	mov  A, @R0		\n\t"
		"	mov  R0, A		\n\t"
		
		//get first param.lo into B
		"	mov  B, DPL		\n\t"
		
		//mul
		"	mul  AB			\n\t"
		
		//lower result byte is ready!
		"	mov  DPL, A		\n\t"
		
		//save upper in r0, get 2nd param into A
		"	mov  A, R0		\n\t"
		"	mov  R0, B		\n\t"
		
		//get first param.hi into B
		"	mov  B, DPH		\n\t"
		
		//mul
		"	mul  AB			\n\t"
		
		//add in the carry from before
		"	add  A, R0		\n\t"
		
		//produce middle byte
		"	mov  DPH, A		\n\t"
		
		//calc high byte
		"	mov  A, B		\n\t"
		"	addc A, #0		\n\t"
		"	mov  B, A		\n\t"
		
		//set high high byte to 0 (guaranteed)
		"	clr  A			\n\t"
		
		//get out
		"	pop  _R0		\n\t"
		"	ret				\n\t"
	);
	(void)a;
	(void)b;
}

#pragma callee_saves mathPrvMul32x8
uint32_t mathPrvMul32x8(uint32_t a, uint8_t b) __reentrant __naked
{
	//return expected in A:B:DPTR, first param is in DPTR, second on stack
	__asm__(
		//save r0
		"	push _R0		\n\t"
		"	push _R1		\n\t"
		
		//save A and B
		"	push A			\n"
		"	push B			\n"
		
		//get second param into R0
		"	mov  A, #-6		\n"
		"	add  A, sp		\n"
		"	mov  R0, A		\n"
		"	mov  _R0, @R0	\n"
		
		//low
		"	mov  A, DPL		\n"
		"	mov  B,	R0		\n"
		"	mul  AB			\n"
		"	mov  DPL, A		\n"
		"	mov  R1, B		\n"
		
		//mid.lo
		"	mov  A, DPH		\n"
		"	mov  B, R0		\n"
		"	mul  AB			\n"
		"	add  A, R1		\n"
		"	mov  DPH, A		\n"
		"	mov  A, B		\n"
		"	addc A, #0		\n"
		"	mov  R1, A		\n"
		
		//mid.hi
		"	pop  A			\n"
		"	mov  B, R0		\n"
		"	mul  AB			\n"
		"	add  A, R1		\n"
		"	xch  A, B		\n"
		"	addc A, #0		\n"
		"	mov  R1, A		\n"
		
		//hi
		"	pop  A			\n"
		"	push B			\n"
		"	mov  B, R0		\n"
		"	mul  AB			\n"
		"	add  A, R1		\n"
		"	pop  B			\n"
		
		//get out
		"	pop  _R1		\n\t"
		"	pop  _R0		\n\t"
		"	ret				\n\t"
	);
	(void)a;
	(void)b;
}

#pragma callee_saves mathPrvMul16x16
uint32_t mathPrvMul16x16(uint16_t a, uint16_t b) __reentrant __naked
{
	//return expected in A:B:DPTR, first param is in DPTR, second on stack (low byte was pushed first)
	__asm__(
		//save r0,r1,r2
		"	push _R0		\n\t"
		"	push _R1		\n\t"
		"	push _R2		\n\t"
		
		//get 2nd param into r1:r0
		"	mov  A, SP		\n\t"
		"	add  A, #-0x05	\n\t"
		"	mov  R0, A		\n\t"
		"	mov  A, @R0		\n\t"
		"	mov  R1, A		\n\t"
		"	dec  R0			\n\t"
		"	mov  A, @R0		\n\t"
		"	mov  R0, A		\n\t"
		
		//mul low bytes, save low result byte
		"	mov  B, DPL		\n\t"
		"	mul  AB			\n\t"
		"	push A			\n\t"
		
		//save high byte
		"	mov  R2, B		\n\t"
		
		//mul p2.lo * p1.hi. add in high from the low multiplication. we know this fits in 16 bits!
		"	mov  A, DPH		\n\t"
		"	mov  B, R0		\n\t"
		"	mul  AB			\n\t"
		"	add  A, R2		\n\t"
		"	mov  R0, A		\n\t"	//save intermediate's lo in R0
		"	mov  A, B		\n\t"
		"	addc A, #0		\n\t"
		"	mov  R2, A		\n\t"	//save intermediate's hi in R2
		
		//mul p2.hi * p1.lo, add in intermediate result
		"	mov  A, R1		\n\t"
		"	mov  B, DPL		\n\t"
		"	mul  AB			\n\t"
		"	add  A, R0		\n\t"	//calc intermediate's lo
		"	push A			\n\t"	//push it
		"	mov  A, B		\n\t"
		"	addc A, R2		\n\t"
		"	mov  R2, A		\n\t"	//calc intermediate's hi in R2
		"	mov  A, #0		\n\t"
		"	addc A, #0		\n\t"	//calc high byte so far
		"	mov  R0, A		\n\t"	//store in R0
		
		//mul high bytes
		"	mov  A, R1		\n\t"
		"	mov  B, DPH		\n\t"
		"	mul  AB			\n\t"
		"	add  A, R2		\n\t"
		"	mov  R2, A		\n\t"	//final value for intermediate's high
		"	mov  A, B		\n\t"
		"	addc A, R0		\n\t"	//final high value is ready
		
		//produce the rest of the result bytes
		"	mov  B, R2		\n\t"
		"	pop  DPH		\n\t"
		"	pop  DPL		\n\t"
		
		//return
		"	pop  _R2		\n\t"
		"	pop  _R1		\n\t"
		"	pop  _R0		\n\t"
		"	ret				\n\t"
	);
	(void)a;
	(void)b;
}

//pushes R0..R2, gets second param into r1:r0
#pragma callee_saves u64_start
static void u64_start(void) __reentrant __naked 
{
	__asm__(
		"	pop   A				\n"	//get ret addr
		"	pop   B				\n"	//get ret addr
		
		"	push  _R0			\n"
		"	push  _R1			\n"
		"	push  _R2			\n"
		
		"	push  B				\n"	//re-push ret addr
		"	push  A				\n"
		
		"	mov   a, #0xf8		\n"	// DPTR = pushed_arg
		"	add   a, sp			\n"
		"	mov   R1, a			\n"
		"	mov   a, @R1		\n"
		"	mov   R0, a			\n"
		"	inc   R1			\n"
		"	mov   a, @R1		\n"
		"	mov   R1, a			\n"
		
		"	ret					\n"
	);
}

//jump to this, do not call it
//does not clobber anything
#pragma callee_saves u64_end
static void u64_end(void) __reentrant __naked 
{
	__asm__(
		"	pop   _R2			\n"
		"	pop   _R1			\n"
		"	pop   _R0			\n"
		"	ret					\n"
	);
}

#pragma callee_saves mathPrvSwapDptrR1R0
void mathPrvSwapDptrR1R0(void) __reentrant __naked 
{
	__asm__(
		"	xch   A, R1			\n\t"
		"	xch   A, DPH		\n\t"
		"	xch   A, R1			\n\t"
		"	xch   A, R0			\n\t"
		"	xch   A, DPL		\n\t"
		"	xch   A, R0			\n\t"
		"	ret					\n\t"
	);
}

#pragma callee_saves u64_copy
void u64_copy(uint64_t __xdata *dst, const uint64_t __xdata *src) __reentrant __naked 
{
	__asm__(
		"	lcall _u64_start	\n"	//get second pointer into R1:R0
		
		"	mov   b, #8			\n"	//repeat 8 times:
		"00003$:				\n"
		"	lcall _mathPrvSwapDptrR1R0	\n"
		"	movx  a, @dptr		\n"
		"	inc   dptr			\n"
		"	lcall _mathPrvSwapDptrR1R0	\n"
		"	movx  @dptr, a		\n"
		"	inc   dptr			\n"
		
		"	djnz  b, 00003$		\n"
	
		"	ljmp  _u64_end		\n"
	);
	(void)dst;
	(void)src;
}

void u64_copyFromCode(uint64_t __xdata *dst, const uint64_t __code *src) __reentrant __naked 
{
	__asm__(
		"	lcall _u64_start	\n"	//get second pointer into R1:R0
		
		"	mov   b, #8			\n"	//repeat 8 times:
		"00003$:				\n"
		"	lcall _mathPrvSwapDptrR1R0	\n"
		"	clr   a				\n"
		"	movc  a, @a+dptr	\n"
		"	inc   dptr			\n"
		"	lcall _mathPrvSwapDptrR1R0	\n"
		"	movx  @dptr, a		\n"
		"	inc   dptr			\n"
		
		"	djnz  b, 00003$		\n"
	
		"	ljmp  _u64_end		\n"
	);
	(void)dst;
	(void)src;
}

#pragma callee_saves u64_add
void u64_add(uint64_t __xdata *lhsP, const uint64_t __xdata *rhsP) __reentrant __naked
{
	__asm__(
		"	lcall _u64_start	\n"	//get second pointer into R1:R0
		"	clr  C				\n"
		
		"	mov   b, #8			\n"	//repeat 8 times:
		"00003$:				\n"
		"	lcall _mathPrvSwapDptrR1R0	\n"
		"	movx  a, @dptr		\n"
		"	inc   dptr			\n"
		"	mov   _R2, a		\n"
		"	lcall _mathPrvSwapDptrR1R0	\n"
		"	movx  a, @dptr		\n"
		"	addc  a, R2			\n"
		"	movx  @dptr, a		\n"
		"	inc   dptr			\n"
		
		"	djnz  b, 00003$		\n"
	
		"	ljmp  _u64_end		\n"
	);
	(void)lhsP;
	(void)rhsP;	
}

#pragma callee_saves u64_and
void u64_and(uint64_t __xdata *lhsP, const uint64_t __xdata *rhsP) __reentrant __naked
{
	__asm__(
		"	lcall _u64_start	\n"	//get second pointer into R1:R0
		
		"	mov   b, #8			\n"	//repeat 8 times:
		"00003$:				\n"
		"	lcall _mathPrvSwapDptrR1R0	\n"
		"	movx  a, @dptr		\n"
		"	inc   dptr			\n"
		"	mov   _R2, a		\n"
		"	lcall _mathPrvSwapDptrR1R0	\n"
		"	movx  a, @dptr		\n"
		"	anl   a, R2			\n"
		"	movx  @dptr, a		\n"
		"	inc   dptr			\n"
		
		"	djnz  b, 00003$		\n"
	
		"	ljmp  _u64_end		\n"
	);
	(void)lhsP;
	(void)rhsP;	
}

#pragma callee_saves u64_sub
void u64_sub(uint64_t __xdata *lhsP, const uint64_t __xdata *rhsP) __reentrant __naked
{
	__asm__(
		"	lcall _u64_start	\n"	//get second pointer into R1:R0
		"	clr  C				\n"
		
		"	mov   b, #8			\n"	//repeat 8 times:
		"00003$:				\n"
		"	lcall _mathPrvSwapDptrR1R0	\n"
		"	movx  a, @dptr		\n"
		"	inc   dptr			\n"
		"	mov   _R2, a		\n"
		"	lcall _mathPrvSwapDptrR1R0	\n"
		"	movx  a, @dptr		\n"
		"	subb  a, R2			\n"
		"	movx  @dptr, a		\n"
		"	inc   dptr			\n"
		
		"	djnz  b, 00003$		\n"
	
		"	ljmp  _u64_end		\n"
	);
	(void)lhsP;
	(void)rhsP;	
}

#pragma callee_saves u64_isLt
__bit u64_isLt(const uint64_t __xdata *lhsP, const uint64_t __xdata *rhsP) __reentrant __naked
{
	__asm__(
		"	lcall _u64_start	\n"	//get second pointer into R1:R0
		"	clr  C				\n"
		
		"	mov   b, #8			\n"	//repeat 8 times:
		"00003$:				\n"
		"	lcall _mathPrvSwapDptrR1R0	\n"
		"	movx  a, @dptr		\n"
		"	inc   dptr			\n"
		"	mov   _R2, a		\n"
		"	lcall _mathPrvSwapDptrR1R0	\n"
		"	movx  a, @dptr		\n"
		"	inc   dptr			\n"
		"	subb  a, R2			\n"
		
		"	djnz  b, 00003$		\n"
	
		"	ljmp  _u64_end		\n"
	);
	(void)lhsP;
	(void)rhsP;	
}

#pragma callee_saves u64_isEq
__bit u64_isEq(const uint64_t __xdata *lhsP, const uint64_t __xdata *rhsP) __reentrant __naked
{
	__asm__(
		"	lcall _u64_start	\n"	//get second pointer into R1:R0
		
		"	mov   b, #8			\n"	//repeat 8 times:
		"00003$:				\n"
		"	lcall _mathPrvSwapDptrR1R0	\n"
		"	movx  a, @dptr		\n"
		"	inc   dptr			\n"
		"	mov   _R2, a		\n"
		"	lcall _mathPrvSwapDptrR1R0	\n"
		"	movx  a, @dptr		\n"
		"	inc   dptr			\n"
		"	xrl   a, R2			\n"
		"	jnz   00004$		\n"
		"	djnz  b, 00003$		\n"

		"	setb  C				\n"
		"	ljmp  _u64_end		\n"
		
		"00004$:				\n"
		"	clr   C				\n"
		"	ljmp  _u64_end		\n"
	);
	(void)lhsP;
	(void)rhsP;	
}

//c is set, A is what to add to each limb
#pragma callee_saves u64_incdec
static void u64_incdec(uint64_t __xdata *dst) __reentrant __naked
{
	__asm__(
		"	xch   A, R0			\n"
		"	push  A				\n"
		"	mov   b, #8			\n"	//repeat 8 times:
		"00003$:				\n"
		"	movx  a, @dptr		\n"
		"	addc  a, R0			\n"
		"	movx  @dptr, a		\n"
		"	inc   dptr			\n"
		"	djnz  b, 00003$		\n"
		"	pop   _R0			\n"
		"	ret					\n"
	);
	(void)dst;	
}

#pragma callee_saves u64_inc
void u64_inc(uint64_t __xdata *dst) __reentrant __naked
{
	__asm__(
		"	setb  C				\n"
		"	clr   A				\n"
		"	ljmp  _u64_incdec	\n"
	);
	(void)dst;	
}

#pragma callee_saves u64_dec
void u64_dec(uint64_t __xdata *dst) __reentrant __naked
{
	__asm__(
		"	clr   C				\n"
		"	mov   A, #0xFF		\n"
		"	ljmp  _u64_incdec	\n"
	);
	(void)dst;
}


#pragma callee_saves xMemSet
void xMemSet(void __xdata* mem, uint8_t val, uint16_t num) __reentrant __naked
{
	__asm__(
		"	push  _R0			\n"
		"	push  _R1			\n"
		
		"	mov   A, #-6		\n"
		"	add   A, sp			\n"
		"	mov   R0, A			\n"
		"	mov   A, @R0		\n"	//num.lo
		"	mov   B, A			\n"
		"	inc   R0			\n"
		"	mov   _R1, @R0		\n"	//num.hi
		"	orl   A, R1			\n"	//zero check
		"	jz    00002$		\n"	//num is R1:B
		"	inc   R0			\n"
		"	mov   _R0, @R0		\n"	//val
		"00003$:				\n"
		"	mov   A, R0			\n"
		"	movx  @DPTR, A		\n"
		"	inc   DPTR			\n"
		"	mov   A, #0xff		\n"
		"	add   A, B			\n"
		"	mov   B, A			\n"
		"	mov   A, #0xff		\n"
		"	addc  A, R1			\n"
		"	mov   _R1, A		\n"
		"	orl   A, B			\n"
		"	jnz   00003$		\n"
		"00002$:				\n"
		"	pop   _R1			\n"
		"	pop   _R0			\n"
		"	ret					\n"
	);
	
	(void)mem;
	(void)val;
	(void)num;
}

#pragma callee_saves xMemEqual
__bit xMemEqual(const void __xdata* memA, const void __xdata* memB, uint8_t num) __reentrant __naked
{
	__asm__(
		"	push  _R0				\n"
		"	push  _R1				\n"
		"	push  _R2				\n"
		
		"	mov   a, #-7			\n"
		"	add   a, sp				\n"
		"	mov   R1, a				\n"
		"	mov   A, @R1			\n"
		"	setb  C					\n"	//equal if len is zero
		"	jz    00004$			\n"
		"	mov   B, A				\n"
		"	inc   R1				\n"
		"	mov   _R0, @R1			\n"
		"	inc   R1				\n"
		"	mov   _R1, @R1			\n"
		"	clr   C					\n"
		"00003$:					\n"
		"	movx  a, @dptr			\n"
		"	mov   _R2, a			\n"
		"	inc   dptr				\n"
		"	lcall _mathPrvSwapDptrR1R0		\n"
		"	movx  a, @dptr			\n"
		"	inc   dptr				\n"
		"	lcall _mathPrvSwapDptrR1R0		\n"
		"	xrl   a, R2				\n"
		"	jnz   00004$			\n"
		"	djnz  b, 00003$			\n"
		"	setb  C					\n"
		"00004$:					\n"
		"	pop   _R2				\n"
		"	pop   _R1				\n"
		"	pop   _R0				\n"
		"	ret						\n"
	);
	
	(void)memA;
	(void)memB;
	(void)num;
}

#pragma callee_saves xMemCopy
void xMemCopy(void __xdata* dst, const void __xdata* src, uint16_t num) __reentrant __naked
{
	__asm__(
		"	push  _R0				\n"
		"	push  _R1				\n"
		"	push  _R2				\n"
		
		"	mov   A, #-8			\n"
		"	add   A, sp				\n"
		"	mov   R1, A				\n"
		"	mov   A, @R1			\n"
		"	mov   B, A				\n"
		"	inc   R1				\n"
		"	mov   _R2, @R1			\n"		//R2:B is length
		"	orl   A, R2				\n"
		"	jz    00004$			\n"		//handle zero length
		"	inc   R1				\n"
		"	mov   _R0, @R1			\n"
		"	inc   R1				\n"
		"	mov   _R1, @R1			\n"		//R1:R0 is src
		"00003$:					\n"
		"	lcall _mathPrvSwapDptrR1R0		\n"
		"	movx  A, @DPTR			\n"
		"	inc   DPTR				\n"
		"	lcall _mathPrvSwapDptrR1R0		\n"
		"	movx  @DPTR, A			\n"
		"	inc   DPTR				\n"
		"	mov   A, #0xff			\n"
		"	add   A, B				\n"
		"	mov   B, A				\n"
		"	mov   A, #0xff			\n"
		"	addc  A, R2				\n"
		"	mov   _R2, A			\n"
		"	orl   A, B				\n"
		"	jnz   00003$			\n"
		"00004$:					\n"
		"	pop   _R2				\n"
		"	pop   _R1				\n"
		"	pop   _R0				\n"
		"	ret						\n"
	);

	(void)dst;
	(void)src;
	(void)num;
}

#pragma callee_saves xMemCopyShort
void xMemCopyShort(void __xdata* dst, const void __xdata* src, uint8_t num) __reentrant __naked
{
	__asm__(
		"	push  _R0				\n"
		"	push  _R1				\n"
		"	push  _R2				\n"
		
		"	mov   a, #-7			\n"
		"	add   a, sp				\n"
		"	mov   R1, a				\n"
		"	mov   A, @R1			\n"
		"	jz    00004$			\n"
		"	mov   B, A				\n"
		"	inc   R1				\n"
		"	mov   _R0, @R1			\n"
		"	inc   R1				\n"
		"	mov   _R1, @R1			\n"
		"00003$:					\n"
		"	lcall _mathPrvSwapDptrR1R0		\n"
		"	movx  a, @dptr			\n"
		"	inc   dptr				\n"
		"	lcall _mathPrvSwapDptrR1R0		\n"
		"	movx  @dptr, a			\n"
		"	inc   dptr				\n"
		"	djnz  b, 00003$			\n"
		"00004$:					\n"
		"	pop   _R2				\n"
		"	pop   _R1				\n"
		"	pop   _R0				\n"
		"	ret						\n"
	);
	
	(void)dst;
	(void)src;
	(void)num;
}

#pragma callee_saves xStrLen
uint16_t xStrLen(const char __xdata *str) __reentrant __naked
{
	__asm__(
		"	push  DPH				\n"
		"	push  DPL				\n"
		"00003$:					\n"
		"	movx  a, @dptr			\n"
		"	jz    00002$			\n"
		"	inc   dptr				\n"
		"	sjmp  00003$			\n"
		"00002$:					\n"
		"	clr   C					\n"
		"	mov   A, DPL			\n"
		"	pop   B					\n"
		"	subb  A, B				\n"
		"	mov   DPL, A			\n"
		"	mov   A, DPH			\n"
		"	pop   B					\n"
		"	subb  A, B				\n"
		"	mov   DPH, A			\n"
		"	ret						\n"
	);
	
	(void)str;
}

#pragma callee_saves static mathPrvDivMod32x16
static uint32_t mathPrvDivMod32x16(uint32_t num, uint16_t denom) __reentrant __naked
{
	//PSW.5 determines if to produce quotient (0) or remainder(1)
	
	__asm__(
		"	push  _R3				\n"
		"	push  _R2				\n"
		"	push  _R1				\n"
		"	push  _R0				\n"
		"	push  A					\n"
		"	push  B					\n"
		"	push  DPH				\n"
		"	push  DPL				\n"
		"	mov   R1, sp			\n"	//R1: point to bottom byte of numerator
		
		//get denom -> B:R0
		"	mov   A, #-10			\n"
		"	add   A, sp				\n"
		"	mov   R0, A				\n"	//R0 = &denom.hi
		"	mov   B, @R0			\n"	//B = denom.hi
		"	dec   R0				\n"
		"	mov   _R0, @R0			\n"	//R0 = denom.lo
		
		//shift it off (in B:R0), record how many iters we'll need (in DPL), generate proper top 8 bits for result mask (in R3:R2)
		"	mov   _R2, #1			\n"
		"	mov   _R3, #0			\n"
		"	mov   DPL, #17			\n"
		"00002$:					\n"
		"	mov   C, B.7			\n"
		"	jc    00003$			\n"
		
		"	clr   C					\n"
		"	mov   A, R0				\n"
		"	rlc   A					\n"
		"	mov   R0, A				\n"
		"	mov   A, B				\n"
		"	rlc   A					\n"
		"	mov   B, A				\n"
		
		"	clr   C					\n"
		"	mov   A, R2				\n"
		"	rlc   A					\n"
		"	mov   R2, A				\n"
		"	mov   A, R3				\n"
		"	rlc   A					\n"
		"	mov   R3, A				\n"
		
		"	inc   DPL				\n"
		"	sjmp  00002$			\n"
		"00003$:					\n"
		
		"	clr   A					\n"
		
		//push result_mask to stack
		"	push  _R3				\n"
		"	push  _R2				\n"
		"	push  A					\n"
		"	push  A					\n"
		"	mov   R2, sp			\n"	//R2: point to bottom byte of result_mask
		
		//push denom.shifted to stack
		"	push  B					\n"
		"	push  _R0				\n"
		"	push  A					\n"
		"	push  A					\n"
		"	mov   R0, sp			\n"	//R0: point to bottom byte of denom.shifted
		
		//push result to stack
		"	push  A					\n"
		"	push  A					\n"
		"	push  A					\n"
		"	push  A					\n"
		"	mov   R3, sp			\n"	//R3: point to bottom byte of result
		
		//loop-divide
		"00088$:					\n"
		
		//	check if denom.shifted >= num
		"	push  _R0				\n"
		"	push  _R1				\n"
		"	clr   C					\n"
		"	mov   DPH, #4			\n"
		"00001$:					\n"
		"	mov   B, @R0			\n"
		"	dec   R0				\n"
		"	mov   A, @R1			\n"
		"	dec   R1				\n"
		"	subb  A, B				\n"
		"	djnz  DPH, 00001$		\n"
		"	pop   _R1				\n"
		"	pop   _R0				\n"
		"	jc    00099$			\n"	//no? skip this round
		
		//subtract
		"	push  _R0				\n"
		"	push  _R1				\n"
		"	clr   C					\n"
		"	mov   DPH, #4			\n"
		"00004$:					\n"
		"	mov   B, @R0			\n"
		"	dec   R0				\n"
		"	mov   A, @R1			\n"
		"	subb  A, B				\n"
		"	mov   @R1, A			\n"
		"	dec   R1				\n"
		"	djnz  DPH, 00004$		\n"
		//keep r0 & r1 pushed
		
		//set bit in result
		//r0 & r1 still pushed
		"	mov   _R0, _R2			\n"
		"	mov   _R1, _R3			\n"
		"	mov   DPH, #4			\n"
		"00005$:					\n"
		"	mov   B, @R0			\n"
		"	dec   R0				\n"
		"	mov   A, @R1			\n"
		"	orl   A, B				\n"
		"	mov   @R1, A			\n"
		"	dec   R1				\n"
		"	djnz  DPH, 00005$		\n"
		"	pop   _R1				\n"
		"	pop   _R0				\n"
		
		//set up next iteration
		"00099$:					\n"
		
		//	shift denom right one (pointer ends up where needed)
		"	mov   A, #0xfc			\n"
		"	add   A, R0				\n"
		"	mov   R0, A				\n"
		"	clr   C					\n"
		"	mov   DPH, #4			\n"
		"00006$:					\n"
		"	inc   R0				\n"
		"	mov   A, @R0			\n"
		"	rrc   A					\n"
		"	mov   @R0, A			\n"
		"	djnz  DPH, 00006$		\n"
		
		//	shift result mask right one
		"	push  _R0				\n"
		"	mov   A, #0xfc			\n"
		"	add   A, R2				\n"
		"	mov   R0, A				\n"
		"	clr   C					\n"
		"	mov   DPH, #4			\n"
		"00007$:					\n"
		"	inc   R0				\n"
		"	mov   A, @R0			\n"
		"	rrc   A					\n"
		"	mov   @R0, A			\n"
		"	djnz  DPH, 00007$		\n"
		"	pop   _R0				\n"
		
		//check on loop limit
		"	djnz  DPL, 00088$		\n"
		
		//we're done. undo the terrible things we've done to the stack and get the result
		
		//quotient return?
		"	jb    PSW.5, 00009$		\n"
		//return quotient
		"	pop   DPL				\n"
		"	pop   DPH				\n"
		"	pop   B					\n"
		"	pop   _R0				\n"
		"00009$:					\n"
		
		//now clear up the rest
		"	mov   A, #-12			\n"
		"	add   A, sp				\n"
		"	mov   sp, A				\n"
		
		//remainder return?
		"	jnb    PSW.5, 00010$	\n"
		//return quotient
		"	pop   DPL				\n"
		"	pop   DPH				\n"
		"	pop   B					\n"
		"	pop   _R0				\n"
		"00010$:					\n"
		
		"	mov   A, R0				\n"
		
		//pop off regs
		"	pop   _R0				\n"
		"	pop   _R1				\n"
		"	pop   _R2				\n"
		"	pop   _R3				\n"
		
		"	ret						\n"
		
	);
	
	(void)num;
	(void)denom;
}

#pragma callee_saves static mathPrvDiv32x16
uint32_t mathPrvDiv32x16(uint32_t num, uint16_t denom) __reentrant __naked
{
	__asm__(
		"	clr   PSW.5					\n"
		"	ljmp  _mathPrvDivMod32x16	\n"
	);
	
	(void)num;
	(void)denom;
}

#pragma callee_saves static mathPrvDiv32x16
uint16_t mathPrvMod32x16(uint32_t num, uint16_t denom) __reentrant __naked
{
	__asm__(
		"	setb  PSW.5					\n"
		"	ljmp  _mathPrvDivMod32x16	\n"
	);
	
	(void)num;
	(void)denom;
}

#pragma callee_saves mathPrvDiv32x8
uint32_t mathPrvDiv32x8(uint32_t num, uint8_t denom) __reentrant __naked
{
	__asm__(
		"	push  _R3				\n"
		"	push  _R2				\n"
		"	push  _R1				\n"
		"	push  _R0				\n"
		"	push  A					\n"
		"	push  B					\n"
		"	push  DPH				\n"
		"	push  DPL				\n"
		"	mov   R1, sp			\n"	//R1: point to bottom byte of numerator
		
		//get denom -> B
		"	mov   A, #0xf6			\n"
		"	add   A, sp				\n"
		"	mov   R0, A				\n"
		"	mov   B, @R0			\n"
		
		//shift it off (in B), record how many iters we'll need (in DPL), generate proper top 8 bits for result mask (in A)
		"	mov   A, #1				\n"
		"	mov   DPL, #25			\n"
		"00002$:					\n"
		"	mov   C, B.7			\n"
		"	jc    00003$			\n"
		"	rl    A					\n"
		"	xch   A, B				\n"
		"	rl    A					\n"
		"	xch   A, B				\n"
		"	inc   DPL				\n"
		"	sjmp  00002$			\n"
		"00003$:					\n"
		
		//push result_mask to stack
		"	push  A					\n"
		"	clr   A					\n"
		"	push  A					\n"
		"	push  A					\n"
		"	push  A					\n"
		"	mov   R2, sp			\n"	//R2: point to bottom byte of result_mask
		
		//push denom.shifted to stack
		"	push  B					\n"
		"	push  A					\n"
		"	push  A					\n"
		"	push  A					\n"
		"	mov   R0, sp			\n"	//R0: point to bottom byte of denom.shifted
		
		//push result to stack
		"	push  A					\n"
		"	push  A					\n"
		"	push  A					\n"
		"	push  A					\n"
		"	mov   R3, sp			\n"	//R3: point to bottom byte of result
		
		//loop-divide
		"00088$:					\n"
		
		//	check if denom.shifted >= num
		"	push  _R0				\n"
		"	push  _R1				\n"
		"	clr   C					\n"
		"	mov   DPH, #4			\n"
		"00001$:					\n"
		"	mov   B, @R0			\n"
		"	dec   R0				\n"
		"	mov   A, @R1			\n"
		"	dec   R1				\n"
		"	subb  A, B				\n"
		"	djnz  DPH, 00001$		\n"
		"	pop   _R1				\n"
		"	pop   _R0				\n"
		"	jc    00099$			\n"	//no? skip this round
		
		//subtract
		"	push  _R0				\n"
		"	push  _R1				\n"
		"	clr   C					\n"
		"	mov   DPH, #4			\n"
		"00004$:					\n"
		"	mov   B, @R0			\n"
		"	dec   R0				\n"
		"	mov   A, @R1			\n"
		"	subb  A, B				\n"
		"	mov   @R1, A			\n"
		"	dec   R1				\n"
		"	djnz  DPH, 00004$		\n"
		//keep r0 & r1 pushed
		
		//set bit in result
		//r0 & r1 still pushed
		"	mov   _R0, _R2			\n"
		"	mov   _R1, _R3			\n"
		"	mov   DPH, #4			\n"
		"00005$:					\n"
		"	mov   B, @R0			\n"
		"	dec   R0				\n"
		"	mov   A, @R1			\n"
		"	orl   A, B				\n"
		"	mov   @R1, A			\n"
		"	dec   R1				\n"
		"	djnz  DPH, 00005$		\n"
		"	pop   _R1				\n"
		"	pop   _R0				\n"
		
		//set up next iteration
		"00099$:					\n"
		
		//	shift denom right one (pointer ends up where needed)
		"	mov   A, #0xfc			\n"
		"	add   A, R0				\n"
		"	mov   R0, A				\n"
		"	clr   C					\n"
		"	mov   DPH, #4			\n"
		"00006$:					\n"
		"	inc   R0				\n"
		"	mov   A, @R0			\n"
		"	rrc   A					\n"
		"	mov   @R0, A			\n"
		"	djnz  DPH, 00006$		\n"
		
		//	shift result mask right one
		"	push  _R0				\n"
		"	mov   A, #0xfc			\n"
		"	add   A, R2				\n"
		"	mov   R0, A				\n"
		"	clr   C					\n"
		"	mov   DPH, #4			\n"
		"00007$:					\n"
		"	inc   R0				\n"
		"	mov   A, @R0			\n"
		"	rrc   A					\n"
		"	mov   @R0, A			\n"
		"	djnz  DPH, 00007$		\n"
		"	pop   _R0				\n"
		
		//check on loop limit
		"	djnz  DPL, 00088$		\n"
		
		//we're done. undo the terrible things we've done to the stack
		
		//first, get the result
		"	pop   DPL				\n"
		"	pop   DPH				\n"
		"	pop   B					\n"
		"	pop   _R0				\n"
		
		//now clear up the rest
		"	mov   A, #-12			\n"
		"	add   A, sp				\n"
		"	mov   sp, A				\n"
		
		"	mov   A, R0				\n"
		
		//pop off regs
		"	pop   _R0				\n"
		"	pop   _R1				\n"
		"	pop   _R2				\n"
		"	pop   _R3				\n"
		
		"	ret						\n"
	);
	
	(void)num;
	(void)denom;
}

#pragma callee_saves mathPrvDivMod16x8
static uint16_t mathPrvDivMod16x8(uint16_t num, uint8_t denom) __reentrant  __naked
{
	__asm__(
		"	push  _R6				\n"
		"	push  _R5				\n"
		"	push  _R4				\n"
		"	push  _R3				\n"
		"	push  _R2				\n"
		"	push  _R1				\n"
		"	push  _R0				\n"
		
		//get denom -> B
		"	mov   A, #-9			\n"
		"	add   A, sp				\n"
		"	mov   R0, A				\n"
		"	mov   B, @R0			\n"
	
		//shift it off (in B), record how many iters we'll need (in R4), generate proper top 8 bits for result mask (in A)
		"	mov   A, #1				\n"
		"	mov   R4, #9			\n"
		"00002$:					\n"
		"	mov   C, B.7			\n"
		"	jc    00003$			\n"
		"	rl    A					\n"
		"	xch   A, B				\n"
		"	rl    A					\n"
		"	xch   A, B				\n"
		"	inc   R4				\n"
		"	sjmp  00002$			\n"
		"00003$:					\n"
		
		//result mask in R3:R2		\n"
		"	mov   R3, A				\n"
		"	clr   A					\n"
		"	mov   R2, A				\n"
		
		//quotient in R1:R0			\n"
		"	mov   R1, B				\n"
		"	mov   R0, A				\n"
		
		//iter count in B			\n"
		"	mov   B,  DPL			\n"
		
		//result in R6:R5
		"	mov   R5, A				\n"
		"	mov   R6, A				\n"
		
		//loop-divide
		"00088$:					\n"
		
		//	check if denom.shifted >= num
		"	clr   C					\n"
		"	mov   A, DPL			\n"
		"	subb  A, R0				\n"
		"	mov   A, DPH			\n"
		"	subb  A, R1				\n"
		"	jc    00099$			\n"	//no? skip this round
		
		//subtract
		"	clr   C					\n"
		"	mov   A, DPL			\n"
		"	subb  A, R0				\n"
		"	mov   DPL, A			\n"
		"	mov   A, DPH			\n"
		"	subb  A, R1				\n"
		"	mov   DPH, A			\n"
		
		//set bit in result
		"	mov   A, R2				\n"
		"	orl   A, R5				\n"
		"	mov   R5, A				\n"
		"	mov   A, R3				\n"
		"	orl   A, R6				\n"
		"	mov   R6, A				\n"
		
		//set up next iteration
		"00099$:					\n"
		
		//	shift denom right one
		"	clr   C					\n"
		"	mov   A, R1				\n"
		"	rrc   A					\n"
		"	mov   R1, A				\n"
		"	mov   A, R0				\n"
		"	rrc   A					\n"
		"	mov   R0, A				\n"
		
		//	shift result mask right one
		"	clr   C					\n"
		"	mov   A, R3				\n"
		"	rrc   A					\n"
		"	mov   R3, A				\n"
		"	mov   A, R2				\n"
		"	rrc   A					\n"
		"	mov   R2, A				\n"
		
		//check on loop limit
		"	djnz  R4, 00088$		\n"
		
		//we're done - produce the result (it is already in DPT Rif we want modulus)
		"	jb    PSW.5, 00098$		\n"
		"	mov   DPL, R5			\n"
		"	mov   DPH, R6			\n"
		"00098$:					\n"
		
		//pop off regs
		"	pop   _R0				\n"
		"	pop   _R1				\n"
		"	pop   _R2				\n"
		"	pop   _R3				\n"
		"	pop   _R4				\n"
		"	pop   _R5				\n"
		"	pop   _R6				\n"
		
		"	ret						\n"
	);
	
	(void)num;
	(void)denom;
}

#pragma callee_saves mathPrvDiv16x8
uint16_t mathPrvDiv16x8(uint16_t num, uint8_t denom) __reentrant __naked
{
	__asm__(
		"	clr   PSW.5					\n"
		"	ljmp  _mathPrvDivMod16x8	\n"
	);
	
	(void)num;
	(void)denom;
}

#pragma callee_saves mathPrvMod16x8
uint8_t mathPrvMod16x8(uint16_t num, uint8_t denom) __reentrant __naked
{
	__asm__(
		"	setb  PSW.5					\n"
		"	ljmp  _mathPrvDivMod16x8	\n"
	);
	
	(void)num;
	(void)denom;
}

char charsPrvDerefAndIncGenericPtr(const char * __xdata* __xdata str) __naked
{
	__asm__(
		"	movx A, @DPTR			\n"
		"	push A					\n"
		"	add  A, #1				\n"
		"	movx @DPTR, A			\n"
		"	inc  DPTR				\n"
		"	movx A, @DPTR			\n"
		"	push A					\n"
		"	addc A, #0				\n"
		"	movx @DPTR, A			\n"
		"	inc  DPTR				\n"
		"	movx A, @DPTR			\n"
		"	pop  _DPH				\n"
		"	pop  _DPL				\n"
		"	jz   00001$				\n"		//check for xdata, taken if yes
		"	cjne A, #0x60, 00002$	\n"		//check for pdata, taken if no
		
		//pdata
		"	push _R0				\n"
		"	mov  R0, DPL			\n"
		"	movx A, @R0				\n"
		"	pop  _R0				\n"
		"	mov  DPL, A				\n"
		"	ret						\n"
		
		"00002$:					\n"
		"	jc   00003$				\n"		//check for idata. taken if yes
		
		//code
		"	clr  A					\n"
		"	movc A, @A+DPTR			\n"
		"	mov  DPL, A				\n"
		"	ret						\n"
		
		//xdata
		"00001$:					\n"
		"	movx A, @DPTR			\n"
		"	mov  DPL, A				\n"
		"	ret						\n"
		
		//idata
		"00003$:					\n"
		"	push _R0				\n"
		"	mov  R0, DPL			\n"
		"	mov  A, @R0				\n"
		"	pop  _R0				\n"
		"	mov  DPL, A				\n"
		"	ret						\n"
	);
	
	(void)str;
}

void mathPrvCopyPostinc(uint32_t __xdata *dst, uint32_t __xdata *src) __reentrant __naked
{
	__asm__(
		"	lcall _u64_start			\n"	//get second pointer into R1:R0
		
		"	mov   b, #4					\n"	//repeat 4 times:
		"	setb  C						\n"
		"00003$:						\n"
		"	lcall _mathPrvSwapDptrR1R0	\n"
		"	movx  a, @dptr				\n"
		"	mov   r2, a					\n"
		"	addc  a, #0					\n"
		"	movx  @dptr, a				\n"
		"	mov   a, r2					\n"
		"	inc   dptr					\n"
		"	lcall _mathPrvSwapDptrR1R0	\n"
		"	movx  @dptr, a				\n"
		"	inc   dptr					\n"
		
		"	djnz  b, 00003$				\n"
	
		"	ljmp  _u64_end				\n"
	);
	(void)dst;
	(void)src;
}

__bit xMemEqual4(const void __xdata* memA, const void __xdata* memB) __reentrant __naked
{
	__asm__(
		"	lcall _u64_start			\n"	//get second pointer into R1:R0
		"	mov   b, #4					\n"	//repeat 4 times:
		"	clr   C						\n"
		"00003$:						\n"
		"	lcall _mathPrvSwapDptrR1R0	\n"
		"	movx  a, @dptr				\n"
		"	mov   r2, a					\n"
		"	inc   dptr					\n"
		"	lcall _mathPrvSwapDptrR1R0	\n"
		"	movx  a, @dptr				\n"
		"	inc   dptr					\n"
		"	xrl   a, r2					\n"
		"	jnz   00004$				\n"
		"	djnz  b, 00003$				\n"
		"	setb  C						\n"
		"00004$:						\n"
	
		"	ljmp  _u64_end				\n"
	);
	
	(void)memA;
	(void)memB;
}

__bit u32minusU16(uint32_t __xdata *u32, uint16_t u16) __naked __reentrant
{
	__asm__(
		"	lcall _u64_start		\n"
		"	clr   C					\n"
		"	movx  A, @DPTR			\n"
		"	inc   DPTR				\n"
		"	subb  A, R0				\n"
		"	movx  A, @DPTR			\n"
		"	inc   DPTR				\n"
		"	subb  A, R1				\n"
		"	movx  A, @DPTR			\n"
		"	inc   DPTR				\n"
		"	subb  A, #0				\n"
		"	movx  A, @DPTR			\n"
		"	inc   DPTR				\n"
		"	subb  A, #0				\n"
		"	ljmp  _u64_end			\n"
	);
	(void)u32;
	(void)u16;
}

__bit u32plusU16(uint32_t __xdata *u32, uint16_t u16) __naked __reentrant
{
	__asm__(
		"	lcall _u64_start		\n"
		"	clr   C					\n"
		"	movx  A, @DPTR			\n"
		"	inc   DPTR				\n"
		"	addc  A, R0				\n"
		"	movx  A, @DPTR			\n"
		"	inc   DPTR				\n"
		"	addc  A, R1				\n"
		"	movx  A, @DPTR			\n"
		"	inc   DPTR				\n"
		"	addc  A, #0				\n"
		"	movx  A, @DPTR			\n"
		"	inc   DPTR				\n"
		"	addc  A, #0				\n"
		"	ljmp  _u64_end			\n"
	);
	(void)u32;
	(void)u16;
}

uint8_t u32Nonzero(uint32_t __xdata *u32) __naked __reentrant
{
	__asm__(
		"	mov   B, #4				\n"
		"00001$:					\n"
		"	movx  A, @DPTR			\n"
		"	jnz   00002$			\n"
		"	inc   DPTR				\n"
		"	djnz  B, 00001$			\n"
		"00002$:					\n"
		"	mov   DPL, A			\n"
		"	ret						\n"
	);
	(void)u32;
}

__bit i32Negative(uint32_t __xdata *u32) __naked __reentrant
{
	__asm__(
		"	inc   DPTR				\n"
		"	inc   DPTR				\n"
		"	inc   DPTR				\n"
		"	movx  A, @DPTR			\n"
		"	rlc   A					\n"
		"	ret						\n"
	);
	(void)u32;
}


