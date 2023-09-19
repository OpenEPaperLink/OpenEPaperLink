#ifndef _JSCREEN_H_
#define _JSCREEN_H_

#include <stdbool.h>
#include <stdint.h>

#define EPD_LUT_DEFAULT     0
#define EPD_LUT_NO_REPEATS  1
#define EPD_LUT_FAST_NO_REDS  2
#define EPD_LUT_FAST 3

void epdSetup();
void epdEnterSleep();


void draw();
void drawNoWait();

void epdWaitRdy();

void selectLUT(uint8_t lut);

#endif