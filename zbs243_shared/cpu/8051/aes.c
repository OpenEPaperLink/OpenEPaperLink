#include "asmUtil.h"
#include "aes.h"
#include "cpu.h"



static uint8_t __xdata mAesKeySchedule[176];

static void aesSbox(void) __naked
{
	__asm__(
		"	.db 0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76	\n"
		"	.db 0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0	\n"
		"	.db 0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15	\n"
		"	.db 0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75	\n"
		"	.db 0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84	\n"
		"	.db 0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf	\n"
		"	.db 0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8	\n"
		"	.db 0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2	\n"
		"	.db 0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73	\n"
		"	.db 0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb	\n"
		"	.db 0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79	\n"
		"	.db 0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08	\n"
		"	.db 0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a	\n"
		"	.db 0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e	\n"
		"	.db 0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf	\n"
		"	.db 0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16	\n"
	);
}

void aesEnc(uint8_t __xdata *data) __reentrant __naked
{
	__asm__(
		"	push  _R7				\n"
		"	push  _R6				\n"
		"	push  _R5				\n"
		"	push  _R4				\n"
		"	push  _R3				\n"
		"	push  _R2				\n"
		"	push  _R1				\n"
		"	push  _R0				\n"
		
		//get key schedule pointer
		"	mov   R0, #_mAesKeySchedule			\n"
		"	mov   R1, #_mAesKeySchedule >> 8	\n"
		
		"	lcall 00094$			\n"	//AddRoundKey
		
		//10 rounds
		"	mov   R2, #10			\n"
		"00002$:					\n"
		
		"	mov   R4, DPH			\n"
		"	mov   R3, DPL			\n"
		
		//SubBytes
		"	mov   R6, #0			\n"
		"00003$:					\n"
		"	movx  A, @DPTR			\n"
		"	mov   R5, DPL			\n"
		"	mov   R7, DPH			\n"
		"	mov   DPTR, #_aesSbox	\n"
		"	movc  A, @A+DPTR		\n"
		"	mov   DPL, R5			\n"
		"	mov   DPH, R7			\n"
		"	movx  @DPTR, A			\n"
		"	inc   DPTR				\n"
		"	inc   R6				\n"
		"	cjne  R6, #16, 00003$	\n"
		
		//ShiftRows using swaps
		"	mov   R6, #0			\n"
		"00004$:					\n"
		"	mov   DPTR, #00098$		\n"
		"	mov   A, R6				\n"
		"	movc  A, @A + DPTR		\n"
		"	anl   A, #0x0f			\n"
		"	add   A, R3				\n"
		"	mov   DPL, A			\n"
		"	clr   A					\n"
		"	addc  A, R4				\n"
		"	mov   DPH, A			\n"
		"	movx  A, @DPTR			\n"
		"	mov   R7, A				\n"
		
		"	mov   DPTR, #00098$		\n"
		"	mov   A, R6				\n"
		"	movc  A, @A + DPTR		\n"
		"	swap  A					\n"
		"	anl   A, #0x0f			\n"
		"	add   A, R3				\n"
		"	mov   DPL, A			\n"
		"	clr   A					\n"
		"	addc  A, R4				\n"
		"	mov   DPH, A			\n"
		"	movx  A, @DPTR			\n"
		"	xch   A, R7				\n"
		"	movx  @DPTR, A			\n"
		
		"	mov   DPTR, #00098$		\n"
		"	mov   A, R6				\n"
		"	movc  A, @A + DPTR		\n"
		"	anl   A, #0x0f			\n"
		"	add   A, R3				\n"
		"	mov   DPL, A			\n"
		"	clr   A					\n"
		"	addc  A, R4				\n"
		"	mov   DPH, A			\n"
		"	mov   A, R7				\n"
		"	movx  @DPTR, A			\n"
		"	inc   R6				\n"
		"	cjne  R6, #8, 00004$	\n"

		//MixColumns (Except for final round)
		"	cjne  R2, #1, 00005$	\n"
		"	ljmp  00007$			\n"
		"00005$:					\n"
		
		"	mov   B, #4				\n"
		"	mov   DPH, R4			\n"
		"	mov   DPL, R3			\n"
		
		"00006$:					\n"
		//calc [3] row
		"	mov   R6, DPH			\n"
		"	mov   R7, DPL			\n"
		"	movx  A, @DPTR			\n"
		"	inc   DPTR				\n"
		"	mov   R5, A				\n"
		"	lcall 00097$			\n"
		"	xrl   _R5, A			\n"
		"	movx  A, @DPTR			\n"
		"	inc   DPTR				\n"
		"	xrl   _R5, A			\n"
		"	movx  A, @DPTR			\n"
		"	inc   DPTR				\n"
		"	xrl   _R5, A			\n"
		"	movx  A, @DPTR			\n"
		"	lcall 00097$			\n"
		"	xrl   A, R5				\n"
		"	push  A					\n"
		
		//calc [2] row
		"	mov   DPH, R6			\n"
		"	mov   DPL, R7			\n"
		"	movx  A, @DPTR			\n"
		"	inc   DPTR				\n"
		"	mov   R5, A				\n"
		"	movx  A, @DPTR			\n"
		"	inc   DPTR				\n"
		"	xrl   _R5, A			\n"
		"	movx  A, @DPTR			\n"
		"	inc   DPTR				\n"
		"	lcall 00097$			\n"
		"	xrl   _R5, A			\n"
		"	movx  A, @DPTR			\n"
		"	xrl   _R5, A			\n"
		"	lcall 00097$			\n"
		"	xrl   A, R5				\n"
		"	push  A					\n"
		
		//calc [1] row
		"	mov   DPH, R6			\n"
		"	mov   DPL, R7			\n"
		"	movx  A, @DPTR			\n"
		"	inc   DPTR				\n"
		"	mov   R5, A				\n"
		"	movx  A, @DPTR			\n"
		"	inc   DPTR				\n"
		"	lcall 00097$			\n"
		"	xrl   _R5, A			\n"
		"	movx  A, @DPTR			\n"
		"	inc   DPTR				\n"
		"	xrl   _R5, A			\n"
		"	lcall 00097$			\n"
		"	xrl   _R5, A			\n"
		"	movx  A, @DPTR			\n"
		"	xrl   A, R5				\n"
		"	push  A					\n"
		
		//cals [0] row
		"	mov   DPH, R6			\n"
		"	mov   DPL, R7			\n"
		"	movx  A, @DPTR			\n"
		"	inc   DPTR				\n"
		"	lcall 00097$			\n"
		"	mov   R5, A				\n"
		"	movx  A, @DPTR			\n"
		"	inc   DPTR				\n"
		"	xrl   _R5, A			\n"
		"	lcall 00097$			\n"
		"	xrl   _R5, A			\n"
		"	movx  A, @DPTR			\n"
		"	inc   DPTR				\n"
		"	xrl   _R5, A			\n"
		"	movx  A, @DPTR			\n"
		"	xrl   A, R5				\n"
		
		//write them
		"	mov   DPH, R6			\n"
		"	mov   DPL, R7			\n"
		"	movx  @DPTR, A			\n"
		"	inc   DPTR				\n"
		"	pop   A					\n"
		"	movx  @DPTR, A			\n"
		"	inc   DPTR				\n"
		"	pop   A					\n"
		"	movx  @DPTR, A			\n"
		"	inc   DPTR				\n"
		"	pop   A					\n"
		"	movx  @DPTR, A			\n"
		"	inc   DPTR				\n"
		
		"	dec   B					\n"
		"	mov   A, B				\n"
		"	jz    00007$			\n"
		"	ljmp  00006$			\n"
		
		"00007$:					\n"
		
		"	mov   DPH, R4			\n"
		"	mov   DPL, R3			\n"
		
		"	lcall 00094$			\n"
		
		"	dec   R2				\n"
		"	mov   A, R2				\n"
		"	jz    00008$			\n"
		"	ljmp  00002$			\n"
		"00008$:					\n"
		
		//pop off regs
		"	pop   _R0				\n"
		"	pop   _R1				\n"
		"	pop   _R2				\n"
		"	pop   _R3				\n"
		"	pop   _R4				\n"
		"	pop   _R5				\n"
		"	pop   _R6				\n"
		"	pop   _R7				\n"
		
		"	ret						\n"
		
		//AddRoundKey()
		"00094$:						\n"
		"	mov   R4, DPH				\n"
		"	mov   R3, DPL				\n"
		"	mov   R7, #16				\n"
		"00093$:						\n"
		"	lcall _mathPrvSwapDptrR1R0	\n"
		"	movx  A, @DPTR				\n"
		"	inc   DPTR					\n"
		"	mov   B, A					\n"
		"	lcall _mathPrvSwapDptrR1R0	\n"
		"	movx  A, @DPTR				\n"
		"	xrl   A, B					\n"
		"	movx  @DPTR, A				\n"
		"	inc   DPTR					\n"
		"	djnz  R7, 00093$			\n"
		"	mov   DPH, R4				\n"
		"	mov   DPL, R3				\n"
		"	ret							\n"
		
		//inc DPTR 4x
		"00095$:					\n"
		"	inc   DPTR				\n"
		"	inc   DPTR				\n"
		"	inc   DPTR				\n"
		"	inc   DPTR				\n"
		"	ret						\n"
		
		//galois multiply by 2 of A
		"00097$:					\n"
		"	add   A, ACC			\n"
		"	jnc   00096$			\n"
		"	xrl   A, #0x1b			\n"
		"00096$:					\n"
		"	ret						\n"
		
		
		//ShiftRows swaps
		"00098$:						\n"
		"	.db 0x15, 0x59, 0x9d		\n"
		"	.db 0x2a, 0x6e				\n"
		"	.db 0xfb, 0xb7, 0x73		\n"
	);
	
	(void)data;
}

void aesSetKey(const uint8_t __xdata *key) __reentrant __naked
{
	__asm__(
		"	push  _R5						\n"
		"	push  _R4						\n"
		"	push  _R3						\n"
		"	push  _R2						\n"
		"	push  _R1						\n"
		"	push  _R0						\n"
		
		//move key pointer into R1:R0
		"	mov   _R1, DPH					\n"
		"	mov   _R0, DPL					\n"
		
		// save keyschedule ptr (and thus get pointer to [0])
		"	mov  DPTR, #_mAesKeySchedule	\n"
		"	push _DPH						\n"
		"	push _DPL						\n"
		
		//copy key into state (and thus get pointer to [16] into DPTR)
		"	mov   B, #16				\n"
		"00001$:						\n"
		"	lcall _mathPrvSwapDptrR1R0	\n"
		"	movx  A, @DPTR				\n"
		"	inc   DPTR					\n"
		"	lcall _mathPrvSwapDptrR1R0	\n"
		"	movx  @DPTR, A				\n"
		"	inc   DPTR					\n"
		"	djnz  B, 00001$				\n"
		
		//get pointer to [0] into R1:R0
		"	pop   _R0					\n"
		"	pop   _R1					\n"
		
		//the loop
		"	mov   B, #0					\n"
		"00002$:						\n"
		"	mov   A, #-4				\n"	//point DPTR to [12]
		"	add   A, DPL				\n"
		"	mov   DPL, A				\n"
		"	mov   A, #0xff				\n"
		"	addc  A, DPH				\n"
		"	mov   DPH, A				\n"
		"	mov   A, #3					\n"
		"	anl   A, B					\n"
		"	jz    00003$				\n"
		
		//the normal 3/4 rounds
		"	movx  A, @DPTR				\n"	//[12]
		"	inc   DPTR					\n"
		"	mov   R2, A					\n"
		"	movx  A, @DPTR				\n"	//[13]
		"	inc   DPTR					\n"
		"	mov   R3, A					\n"
		"	movx  A, @DPTR				\n"	//[14]
		"	inc   DPTR					\n"
		"	mov   R4, A					\n"
		"	movx  A, @DPTR				\n"	//[15]
		"	inc   DPTR					\n"
		"	mov   R5, A					\n"
		"	sjmp  00004$				\n"
		
		//the weird 1/4 round
		"00003$:						\n"
		"	movx  A, @DPTR				\n"	//[12]
		"	inc   DPTR					\n"
		"	push  DPL					\n"
		"	push  DPH					\n"
		"	mov   DPTR, #_aesSbox		\n"
		"	movc  A, @A+DPTR			\n"
		"	mov   _R5, A				\n"
		"	pop   DPH					\n"
		"	pop   DPL					\n"
		
		"	movx  A, @DPTR				\n"	//[13]
		"	inc   DPTR					\n"
		"	push  DPL					\n"
		"	push  DPH					\n"
		"	mov   DPTR, #_aesSbox		\n"
		"	movc  A, @A+DPTR			\n"
		"	mov   _R2, A				\n"
		"	mov   DPTR, #00098$			\n"
		"	mov   A, B					\n"
		"	rr    A						\n"
		"	rr    A						\n"
		"	movc  A, @A+DPTR			\n"
		"	xrl   _R2, A				\n"
		"	pop   DPH					\n"
		"	pop   DPL					\n"
		
		"	movx  A, @DPTR				\n"	//[14]
		"	inc   DPTR					\n"
		"	push  DPL					\n"
		"	push  DPH					\n"
		"	mov   DPTR, #_aesSbox		\n"
		"	movc  A, @A+DPTR			\n"
		"	mov   _R3, A				\n"
		"	pop   DPH					\n"
		"	pop   DPL					\n"
		
		"	movx  A, @DPTR				\n"	//[15]
		"	inc   DPTR					\n"
		"	push  DPL					\n"
		"	push  DPH					\n"
		"	mov   DPTR, #_aesSbox		\n"
		"	movc  A, @A+DPTR			\n"
		"	mov   _R4, A				\n"
		"	pop   DPH					\n"
		"	pop   DPL					\n"
		
		"00004$:						\n"
		"	lcall _mathPrvSwapDptrR1R0	\n"
		"	movx  A, @DPTR				\n"	//[0]
		"	inc   DPTR					\n"
		"	xrl   _R2, A				\n"
		"	movx  A, @DPTR				\n"	//[1]
		"	inc   DPTR					\n"
		"	xrl   _R3, A				\n"
		"	movx  A, @DPTR				\n"	//[2]
		"	inc   DPTR					\n"
		"	xrl   _R4, A				\n"
		"	movx  A, @DPTR				\n"	//[3]
		"	inc   DPTR					\n"
		"	xrl   _R5, A				\n"
		"	lcall _mathPrvSwapDptrR1R0	\n"
		"	mov   A, R2					\n"
		"	movx  @DPTR, A				\n"	//[16]
		"	inc   DPTR					\n"
		"	mov   A, R3					\n"
		"	movx  @DPTR, A				\n"	//[17]
		"	inc   DPTR					\n"
		"	mov   A, R4					\n"
		"	movx  @DPTR, A				\n"	//[18]
		"	inc   DPTR					\n"
		"	mov   A, R5					\n"
		"	movx  @DPTR, A				\n"	//[19]
		"	inc   DPTR					\n"
		
		"	inc   B						\n"
		"	mov   A, #-40				\n"
		"	add   A, B					\n"
		"	jz    00005$				\n"
		"	ljmp  00002$				\n"
		
		"00005$:						\n"
		"	pop   _R0					\n"
		"	pop   _R1					\n"
		"	pop   _R2					\n"
		"	pop   _R3					\n"
		"	pop   _R4					\n"
		"	pop   _R5					\n"
		"	ret							\n"		
		
		//round constants
		"00098$:						\n"
		"	.db 0x01,0x02,0x04,0x08,0x10\n"
		"	.db 0x20,0x40,0x80,0x1b,0x36\n"
	);
	
	(void)key;
}
