#ifndef _JSCREEN_H_
#define _JSCREEN_H_

#include <stdbool.h>
#include <stdint.h>

#include "main.h"
#include "epd_spi.h"

#define EPD_SSD1619

#define epdSend spi_write
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

#define EPD_LUT_DEFAULT     0
#define EPD_LUT_NO_REPEATS  1
#define EPD_LUT_FAST_NO_REDS  2
#define EPD_LUT_FAST 3


#define epdSelect()                \
    do                             \
    {                              \
        digitalWrite(EPD_CS, LOW); \
    } while (0)

#define epdDeselect()               \
    do                              \
    {                               \
        digitalWrite(EPD_CS, HIGH); \
    } while (0)

void epdSetup();
void epdEnterSleep();
uint16_t epdGetBattery();
void epdConfigGPIO(bool setup);

void epdSetup();
void epdEnterSleep();
uint16_t epdGetBattery();
void epdConfigGPIO(bool setup);

extern bool  epdGPIOActive;

void setColorMode(uint8_t red, uint8_t bw) ;
void clearWindow(bool color);
void clearScreen();
void draw();
void drawNoWait();
void drawWithSleep();
void epdWaitRdy();

void beginFullscreenImage();
void beginWriteFramebuffer(bool color);
void endWriteFramebuffer();
void loadRawBitmap(uint8_t* bmp, uint16_t x, uint16_t y, bool color) ;
void printBarcode(const uint8_t* string, uint16_t x, uint16_t y);

void selectLUT(uint8_t lut);

void ByteDecode(uint8_t byte);

void epdPrintBegin(uint16_t x, uint16_t y, bool direction, bool fontsize, bool red);
void epdPrintEnd();

void beginFullscreenImage();
void beginWriteFramebuffer(bool color);

void lutTest();

void epdTest();

// for printf.c
void writeCharEPD(uint8_t c);
void epdpr(const char  *c, ...);

#endif