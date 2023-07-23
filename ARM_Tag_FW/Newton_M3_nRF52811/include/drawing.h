#ifndef _DRAWING_H_
#define _DRAWING_H_

#include <Arduino.h>
#include <stdint.h>

#define DRAWING_MIN_BITMAP_SIZE		(128)		//minimum size we'll consider

void set_offline(boolean state);
void drawImageAtAddress(uint32_t addr, uint8_t lut);
void drawImageFromBuffer(uint8_t* buffer, const uint8_t lut);

#endif
