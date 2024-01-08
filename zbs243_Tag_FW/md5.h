#ifndef MD5_H
#define MD5_H

#include <stdio.h>
#include <stdint.h>
//#include <string.h>
//#include <stdlib.h>

extern uint64_t __xdata ctxsize;       // Size of input in bytes
extern uint32_t __xdata ctxbuffer[4];  // Current accumulation of hash
extern uint8_t __xdata ctxinput[64];   // Input to be used in the next step
extern uint8_t __xdata ctxdigest[16];  // Result of algorithm

void md5Init();
void md5Update(uint8_t *input, size_t input_len) __reentrant;
void md5Finalize();
void md5Step(uint32_t *input);

#endif
