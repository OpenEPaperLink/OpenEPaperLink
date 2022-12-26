#ifndef _SOCi_H_
#define _SOCi_H_

#define PDATA 
#include "zbs243.h"

#include <stdint.h>


#pragma callee_saves clockingAndIntsInit
void clockingAndIntsInit(void);

#pragma callee_saves rndGen8
uint8_t rndGen8(void);

#pragma callee_saves rndGen32
uint32_t rndGen32(void);

#pragma callee_saves rndSeed
void rndSeed(uint8_t seedA, uint8_t seedB);

#pragma callee_saves selfUpdate
void selfUpdate(void);


void TEMP_ISR(void) __interrupt (10);


#endif
