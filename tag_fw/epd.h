#ifndef _JSCREEN_H_
#define _JSCREEN_H_

#include <stdbool.h>
#include <stdint.h>

#define epdSend spiTXByte
#define EPD_DIRECTION_X false
#define EPD_DIRECTION_Y true
#define EPD_SIZE_SINGLE false
#define EPD_SIZE_DOUBLE true
#define EPD_COLOR_RED true
#define EPD_COLOR_BLACK false
#define EPD_LOAD_CUSTOM_LUT true
#define EPD_LOAD_OTP_LUT false
#define EPD_MODE_NORMAL 0x00
#define EPD_MODE_INVERT 0x08
#define EPD_MODE_IGNORE 0x04

#define epdSelect() \
    do {            \
        P1_7 = 0;   \
    } while (0)

#define epdDeselect() \
    do {              \
        P1_7 = 1;     \
    } while (0)

void epdSetup();
void epdEnterSleep();

void setWindowX(uint16_t start, uint16_t end);
void setWindowY(uint16_t start, uint16_t end);
void setPosXY(uint16_t x, uint16_t y);
void setColorMode(uint8_t red, uint8_t bw) ;
void fillWindowWithPattern(bool color);
void clearWindow(bool color);
void clearScreen();
void draw();
void drawNoWait();
void epdWaitRdy();
void drawLineHorizontal(bool color, uint16_t x1, uint16_t x2, uint16_t y);
void drawLineVertical(bool color, uint16_t x, uint16_t y1, uint16_t y2);

void beginFullscreenImage();
void beginWriteFramebuffer(bool color);
void endWriteFramebuffer();
void loadRawBitmap(uint8_t* bmp, uint16_t x, uint16_t y, bool color);
void printBarcode(const uint8_t* string, uint16_t x, uint16_t y);

void selectLUT(uint8_t lut);

void ByteDecode(uint8_t byte);

void epdPrintBegin(uint16_t x, uint16_t y, bool direction, bool fontsize, bool red);
void epdPrintEnd();

void beginFullscreenImage();
void beginWriteFramebuffer(bool color);

void lutTest();

// for printf.c
void writeCharEPD(uint8_t c);

#endif