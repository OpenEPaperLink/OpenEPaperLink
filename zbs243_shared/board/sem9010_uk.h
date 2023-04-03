#ifndef _SCREEN_SEGMENTED_H_
#define _SCREEN_SEGMENTED_H_

#include <stdint.h>

// data is 13/30 bytes (12 significant), assumes spi & gpios are set up
__bit epdDraw();
__bit epdUpdate();
__bit epdSetup(__bit inverted);


void epdSetPos(uint8_t p);

void writeCharEPD(char c);

void epdWaitRdy();

void epdClear();
void setEPDIcon(uint16_t iconvalue, bool on);
uint8_t is_drawing();
void epdEnable();
void epdDisable();

#define EPD_SIGN_POUND_LARGE 0x01
#define EPD_DIGIT_ONE_LARGE 0x02
#define EPD_PERIOD_LARGE 0x04
#define EPD_SIGN_PENCE_LARGE 0x08

#define EPD_SIGN_POUND_SMALL 0x10
#define EPD_DIGIT_ONE_SMALL 0x20
#define EPD_PERIOD_SMALL 0x40
#define EPD_SIGN_PENCE_SMALL 0x80

#define EPD_BG_COLOR 0x0100
#define EPD_ICON_DIAMOND 0x0200
#define EPD_ICON_ARROW 0x0400
#define EPD_ICON_STAR 0x0800
#define EPD_ICON_CIRCLE 0x1000
#define EPD_ICON_SQUARE 0x2000
#endif
