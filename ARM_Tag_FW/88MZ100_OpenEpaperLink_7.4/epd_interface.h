#pragma once
// #include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

class epdInterface {
   public:
    virtual ~epdInterface() = 0;
    virtual void epdSetup() = 0;
    virtual void epdEnterSleep() = 0;
    virtual void draw() = 0;
    virtual void drawNoWait() = 0;
    virtual void epdWaitRdy() = 0;
    virtual void selectLUT(uint8_t lut) = 0;
    uint8_t controllerType = 0;
    uint16_t Xres;
    uint16_t Yres;
    uint16_t effectiveXRes;
    uint16_t effectiveYRes;
    uint16_t XOffset = 0;
    uint16_t YOffset = 0;
    uint8_t bpp = 0;
    bool drawDirectionRight = false;
    bool epdMirrorV = false;
    bool epdMirrorH = false;
};

struct tagSpecs {
    uint8_t buttonCount = 0;
    bool hasNFC = false;
    bool hasLED = false;
    uint16_t macSuffix = 0x0000;
    uint8_t OEPLtype = 0;
    uint8_t solumType = 0;
    uint32_t imageSize = 0;
} __attribute__((packed));

extern __attribute__((section(".aonshadow"))) tagSpecs tag;

//__attribute__((section(".aonshadow")))
extern epdInterface *epd;

void epdSetup();
void epdEnterSleep();

void draw();
void drawNoWait();
void drawWithSleep();
void epdWaitRdy();

#define EPD_LUT_DEFAULT 0
#define EPD_LUT_NO_REPEATS 1
#define EPD_LUT_FAST_NO_REDS 2
#define EPD_LUT_FAST 3

void selectLUT(uint8_t lut);

void initEPDGPIO();

void busyWaitUntil(bool high, uint32_t timeout);
void softSPIWriteByte(char byteOut);
uint8_t softSPIReadByte();

void enableHardSPI(bool enable);
void epdWrite(uint8_t reg, uint8_t len, ...);
void epdBlockWrite(uint8_t reg, uint8_t *buffer, uint16_t len);
void epdBlockWrite(uint8_t *buffer, uint16_t len);