#ifndef _ASM_UTIL_H_
#define _ASM_UTIL_H_


#include <stdint.h>
#include "cpu.h"



//SDCC may have uint64_t support, but it is so shitty, we're better off not using it
//sdcc is brain dead when compiling multiplication, so we write our own asm code to make it better...le sigh...
//SDCC also has issues managing xdata memory ops, we write our own

#pragma callee_saves u64_copy
#pragma callee_saves u64_isLt
#pragma callee_saves u64_isEq
#pragma callee_saves u64_sub
#pragma callee_saves u64_add
#pragma callee_saves u64_inc
#pragma callee_saves u64_dec

#pragma callee_saves xMemSet
#pragma callee_saves xMemEqual
#pragma callee_saves xMemEqual4
#pragma callee_saves xMemCopy
#pragma callee_saves xMemCopyShort
#pragma callee_saves xMemCopy8
#pragma callee_saves xStrLen

#pragma callee_saves rngGen

#pragma callee_saves mathPrvMul8x8
#pragma callee_saves mathPrvMul16x8
#pragma callee_saves mathPrvMul16x16
#pragma callee_saves mathPrvMul32x8
#pragma callee_saves mathPrvDiv32x8
#pragma callee_saves mathPrvDiv32x16
#pragma callee_saves mathPrvMod32x16
#pragma callee_saves mathPrvDiv16x8
#pragma callee_saves mathPrvMod16x8
#pragma callee_saves mathPrvCopyPostinc

#pragma callee_saves mathPrvI16Asr1

#pragma callee_saves mathPrvU8bitswap

#pragma callee_saves charsPrvDerefAndIncGenericPtr		//return *(*generic_charPtr)++


//it saddens me that i need these....but i do
#pragma callee_saves mathPrvU16from2xU8
#pragma callee_saves mathPrvU32from4xU8
#pragma callee_saves u32minusU16
#pragma callee_saves u32plusU16
#pragma callee_saves u32Nonzero
#pragma callee_saves i32Negative


__bit u32minusU16(uint32_t __xdata *u32, uint16_t u16) __reentrant;		//sets carry
__bit u32plusU16(uint32_t __xdata *u32, uint16_t u16) __reentrant;		//sets carry
uint8_t u32Nonzero(uint32_t __xdata *u32) __reentrant;
__bit i32Negative(uint32_t __xdata *u32) __reentrant;


void u64_copy(uint64_t __xdata *dst, const uint64_t __xdata *src) __reentrant;
void u64_copyFromCode(uint64_t __xdata *dst, const uint64_t __code *src) __reentrant;
__bit u64_isLt(const uint64_t __xdata *lhs, const uint64_t __xdata *rhs) __reentrant;
__bit u64_isEq(const uint64_t __xdata *lhs, const uint64_t __xdata *rhs) __reentrant;
void u64_sub(uint64_t __xdata *lhs, const uint64_t __xdata *rhs) __reentrant;
void u64_add(uint64_t __xdata *lhs, const uint64_t __xdata *rhs) __reentrant;
void u64_and(uint64_t __xdata *lhs, const uint64_t __xdata *rhs) __reentrant;
void u64_inc(uint64_t __xdata *dst) __reentrant;
void u64_dec(uint64_t __xdata *dst) __reentrant;

#define U64FMT			"%04x%04x%04x%04x"
#define U64CVT(v)		((uint16_t __xdata*)&v)[3], ((uint16_t __xdata*)&v)[2], ((uint16_t __xdata*)&v)[1], ((uint16_t __xdata*)&v)[0]

int16_t mathPrvI16Asr1(int16_t val) __reentrant;

uint16_t mathPrvMul8x8(uint8_t a, uint8_t b) __reentrant;
uint32_t mathPrvMul16x8(uint16_t a, uint8_t b) __reentrant;
uint32_t mathPrvMul16x16(uint16_t a, uint16_t b) __reentrant;
uint32_t mathPrvMul32x8(uint32_t a, uint8_t b) __reentrant;
uint32_t mathPrvDiv32x8(uint32_t num, uint8_t denom) __reentrant;
uint32_t mathPrvDiv32x16(uint32_t num, uint16_t denom) __reentrant;
uint16_t mathPrvMod32x16(uint32_t num, uint16_t denom) __reentrant;
uint16_t mathPrvDiv16x8(uint16_t num, uint8_t denom) __reentrant;
uint8_t mathPrvMod16x8(uint16_t num, uint8_t denom) __reentrant;

uint8_t mathPrvU8bitswap(uint8_t val) __reentrant;

uint16_t mathPrvU16from2xU8(uint8_t hi, uint8_t lo) __reentrant;
uint32_t mathPrvU32from4xU8(uint8_t hi, uint8_t midhi, uint8_t midlo, uint8_t lo) __reentrant;

char charsPrvDerefAndIncGenericPtr(const char * __xdata* __xdata str);

void xMemSet(void __xdata* mem, uint8_t val, uint16_t num) __reentrant;
__bit xMemEqual(const void __xdata* memA, const void __xdata* memB, uint8_t num) __reentrant;
__bit xMemEqual4(const void __xdata* memA, const void __xdata* memB) __reentrant;
void xMemCopy(void __xdata* dst, const void __xdata* src, uint16_t num) __reentrant;
void xMemCopyShort(void __xdata* dst, const void __xdata* src, uint8_t num) __reentrant;
uint16_t xStrLen(const char __xdata *str) __reentrant;

#define  xMemCopy8(_dst, _src) u64_copy((uint64_t __xdata*)(_dst), (const uint64_t __xdata*)(_src))


void mathPrvCopyPostinc(uint32_t __xdata *dst, uint32_t __xdata *src) __reentrant;		//*dst = (*src)++

//private
void mathPrvSwapDptrR1R0(void) __reentrant;



#endif
