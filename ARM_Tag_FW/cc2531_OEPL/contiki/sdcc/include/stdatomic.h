#ifndef __SDCC_STDATOMIC_H
#define __SDCC_STDATOMIC_H 1

typedef struct {unsigned char flag;} atomic_flag;

#if defined(__SDCC_z80) || defined(__SDCC_z180) || defined(__SDCC_ez80_z80) || defined(__SDCC_z80n) || defined(__SDCC_gbz80) || defined(__SDCC_tlcs90) || defined(__SDCC_r2k) || defined(__SDCC_r2ka) || defined(__SDCC_r3ka) || defined(__SDCC_stm8) || defined(__SDCC_hc08) || defined(__SDCC_s08)

#define ATOMIC_FLAG_INIT {1}
_Bool atomic_flag_test_and_set(volatile atomic_flag *object);

void atomic_flag_clear(volatile atomic_flag *object);

#elif defined(__SDCC_mcs51)

#define ATOMIC_FLAG_INIT {0}
_Bool atomic_flag_test_and_set(volatile atomic_flag __idata *object) __nonbanked;

void atomic_flag_clear(volatile atomic_flag __idata *object) __nonbanked;

#elif defined(__SDCC_ds390)

#define ATOMIC_FLAG_INIT {0}
_Bool atomic_flag_test_and_set(volatile atomic_flag __idata *object);

void atomic_flag_clear(volatile atomic_flag __idata *object);

#else

#error Support for atomic_flag not implemented

#endif

#endif

