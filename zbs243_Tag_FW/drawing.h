#ifndef _DRAWING_H_
#define _DRAWING_H_

#include <stdint.h>

//void set_offline(__bit state);
#pragma callee_saves drawImageAtAddress
extern void drawImageAtAddress(uint32_t addr, uint8_t lut) __reentrant;

#endif
