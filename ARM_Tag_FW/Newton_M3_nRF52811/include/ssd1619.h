#ifndef _JSCREEN_H_
#define _JSCREEN_H_

#include <stdbool.h>
#include <stdint.h>

#define EPD_SSD1619

#define epdSend spi_write

#define EPD_LUT_DEFAULT 0
#define EPD_LUT_NO_REPEATS 1
#define EPD_LUT_FAST_NO_REDS 2
#define EPD_LUT_FAST 3
#define EPD_LUT_OTA 0x10

void epdSetup();
void epdEnterSleep();

extern uint8_t dispLutSize;
extern uint8_t customLUT[];

void draw();
void drawNoWait();
void drawWithSleep();
void epdWaitRdy();

void selectLUT(uint8_t lut);

#endif
