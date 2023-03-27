#include "cpu.h"


struct RngState {
	uint64_t a, b;
	uint32_t c;
};

static struct RngState mState = {0, };

void rndSeed(uint8_t seedA, uint8_t seedB)
{
	mState.a = seedA;
	mState.b = seedB;
}

#pragma callee_saves rndPrvGen32
static uint32_t rndPrvGen32(struct RngState __xdata *state) __naked
{
	//	//xorshift128p with a small midification ( " retval ^= ++ c"  )
	//
	//	state->a ^= state->a << 23;
	//	state->a ^= state->a >> 17;
	//	state->a ^= state->b;
	//	t = state->a;
	//	state->a = state->b;
	//	state->b >>= 26
	//	state->b = t ^ state->b;
	//	state->c++;
	//	
	//	return (state->a + state->b) >> 32 + state->c;
	
	__asm__(

		"	push  _R0							\n"

		//state->a ^= state->a << 23
		"	mov   A, #5							\n"	//point to a[5]
		"	lcall 00091$						\n"
		"	movx  A, @DPTR						\n"
		"	mov   PSW.5, A.0					\n"
		"	mov   B, #5							\n"
		"	mov   A, #-1						\n"
		"00001$:								\n"
		"	lcall 00089$						\n"
		"	movx  A, @DPTR						\n"
		"	mov   C, PSW.5						\n"
		"	rrc   A								\n"
		"	mov   PSW.5, C						\n"
		"	inc   DPTR							\n"
		"	inc   DPTR							\n"
		"	inc   DPTR							\n"
		"	mov   _R0, A						\n"
		"	movx  A, @DPTR						\n"
		"	xrl   A, R0							\n"
		"	movx  @DPTR, A						\n"
		"	mov   A, #-4						\n"
		"	djnz  B, 00001$						\n"
		
		//repoint DPTR to state->a.8
		"	mov   A, #5							\n"
		"	lcall 00091$						\n"
		
		//state->a ^= state->a >> 17
		
		"	clr   PSW.5							\n"
		"	mov   B, #6							\n"
		"00002$:								\n"
		"	mov   A, #-1						\n"
		"	lcall 00089$						\n"
		"	movx  A, @DPTR						\n"
		"	mov   C, PSW.5						\n"
		"	rrc   A								\n"
		"	mov   PSW.5, C						\n"
		"	push  A								\n"
		"	djnz  B, 00002$						\n"
		
		"	mov   A, #-2						\n"
		"	lcall 00089$						\n"
		
		"	mov   B, #6							\n"
		"00003$:								\n"
		"	movx  A, @DPTR						\n"
		"	pop   _R0							\n"
		"	xrl   A, R0							\n"
		"	movx  @DPTR, A						\n"
		"	inc   DPTR							\n"
		"	djnz  B, 00003$						\n"
		
		"	inc   DPTR							\n"	//point to B
		"	inc   DPTR							\n"
		
		//	pushed_t = state->a ^ state->b	//last pushe di nhigh byte
		//	state->a = state->b;
		
		"	mov   B, #8							\n"
		"00004$:								\n"
		"	movx  A, @DPTR						\n"
		"	mov   _R0, A						\n"		//r0 = b[i]
		"	mov   A, #-8						\n"
		"	lcall 00089$						\n"
		"	movx  A, @DPTR						\n"		//a = a[i]
		"	xrl   A, R0							\n"		//a = a[i] ^ b[i]
		"	push  A								\n"
		"	mov   A, R0							\n"
		"	movx  @DPTR, A						\n"
		"	mov   A, #9							\n"
		"	lcall 00091$						\n"
		"	djnz  B, 00004$						\n"
		
		//repoint DPTR to state->b.3
		"	mov   A, #-5						\n"
		"	lcall 00089$						\n"
		
		//state->b >>= 24 (top 3 bytes are garbage)
		"	mov   B, #5							\n"
		"00005$:								\n"
		"	movx  A, @DPTR						\n"
		"	mov   _R0, A						\n"
		"	mov   A, #-3						\n"
		"	lcall 00089$						\n"
		"	mov   A, R0							\n"
		"	movx  @DPTR, A						\n"
		"	mov   A, #4							\n"
		"	lcall 00091$						\n"
		"	djnz  B, 00005$						\n"
		
		//state->b >>= 2
		"	mov   A, #-4						\n"
		"	lcall 00089$						\n"
		
		"	mov   B, #2							\n"
		"00006$:								\n"
		"	mov   _R0, #5						\n"
		"	clr   PSW.5							\n"
		"00007$:								\n"
		"	movx  A, @DPTR						\n"
		"	mov   C, PSW.5						\n"
		"	rrc   A								\n"
		"	mov   PSW.5, C						\n"
		"	movx  @DPTR, A						\n"
		"	mov   A, #-1						\n"
		"	lcall 00089$						\n"
		"	djnz  _R0, 00007$					\n"
		"	mov   A, #5							\n"
		"	lcall 00091$						\n"
		"	djnz  B, 00006$						\n"
		
		//reset DPTR to end of state->b
		"	mov   A, #3							\n"
		"	lcall 00091$						\n"
		
		//state->b = t ^ state->b
		"	mov   B, #3							\n"
		"00008$:								\n"
		"	pop   A								\n"
		"	movx  @DPTR, A						\n"
		"	mov   A, #-1						\n"
		"	lcall 00089$						\n"
		"	djnz  B, 00008$						\n"
		
		"	mov   B, #5							\n"
		"00009$:								\n"
		"	movx  A, @DPTR						\n"
		"	pop   _R0							\n"
		"	xrl   A, R0							\n"
		"	movx  @DPTR, A						\n"
		"	mov   A, #-1						\n"
		"	lcall 00089$						\n"
		"	djnz  B, 00009$						\n"
		
		"	mov   A, #9							\n"
		"	lcall 00091$						\n"
		
		//state->c++
		"	mov   B, #4							\n"
		"	setb  C								\n"
		"00010$:								\n"
		"	movx  A, @DPTR						\n"
		"	addc  A, #0							\n"
		"	movx  @DPTR, A						\n"
		"	inc   DPTR							\n"
		"	djnz  B, 00010$						\n"
		
		"	mov   A, #-16						\n"	//point to top 32 bits of A
		"	lcall 00089$						\n"
		
		// push (state->a + state->b) >> 32 ^ state->c
		"	mov   B, #4							\n"
		"	clr   PSW.5							\n"
		"00011$:								\n"
		"	movx  A, @DPTR						\n"
		"	mov   _R0, A						\n"
		"	mov   A, #8							\n"
		"	lcall 00091$						\n"
		"	movx  A, @DPTR						\n"
		"	mov   C, PSW.5						\n"
		"	addc  A, R0							\n"
		"	mov   PSW.5, C						\n"
		"	mov   _R0, A						\n"
		"	mov   A, #4							\n"
		"	lcall 00091$						\n"
		"	movx  A, @DPTR						\n"
		"	xrl   A, R0							\n"
		"	push  A								\n"
		"	mov   A, #-11						\n"
		"	lcall 00089$						\n"
		"	djnz  B, 00011$						\n"
		
		//pop result (pop it large to small)
		"	pop   A								\n"
		"	pop   B								\n"
		"	pop   DPH							\n"
		"	pop   DPL							\n"
		
		"	pop   _R0							\n"
		"	ret									\n"
		
		//sub from DPTR
		"00089$:								\n"
		"	add   A, DPL						\n"
		"	mov   DPL, A						\n"
		"	mov   A, #0xff						\n"
		"	addc  A, DPH						\n"
		"	mov   DPH, A						\n"
		"	ret									\n"
		
		//add to DPTR
		"00091$:								\n"
		"	add   A, DPL						\n"
		"	mov   DPL, A						\n"
		"	clr   A								\n"
		"	addc  A, DPH						\n"
		"	mov   DPH, A						\n"
		"	ret									\n"
	);
	
	(void)state;
}

uint32_t rndGen32(void) __naked
{
	__asm__(
		"	mov   DPTR, #_mState	\n"
		"	ljmp _rndPrvGen32		\n"
	);
}

uint8_t rndGen8(void) __naked
{
	__asm__(
		"	lcall _rndGen32			\n"
		"	rr    A					\n"
		"	xrl   A, B				\n"
		"	rr    A					\n"
		"	xrl   A, DPH			\n"
		"	rr    A					\n"
		"	xrl   DPL, A			\n"
		"	ret						\n"
	);
}
