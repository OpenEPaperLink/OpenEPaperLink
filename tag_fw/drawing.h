#ifndef _DRAWING_H_
#define _DRAWING_H_

#include <stdint.h>

#define DRAWING_MIN_BITMAP_SIZE		(128)		//minimum size we'll consider

void set_offline(__bit state);
#pragma callee_saves drawImageAtAddress
void drawImageAtAddress(uint32_t addr, uint8_t lut);


#endif
