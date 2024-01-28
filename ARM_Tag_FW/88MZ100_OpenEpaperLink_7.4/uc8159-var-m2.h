#pragma once
// #include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "epd_interface.h"

class uc8159 : public epdInterface {
   public:
    ~uc8159();
    void epdSetup();
    void epdEnterSleep();
    void draw();
    void drawNoWait();
    void epdWaitRdy();
    void selectLUT(uint8_t lut);

   protected:
    void initPartTwo();
    uint8_t drawLut;
    void epdWriteDisplayData();
    void interleaveColorToBuffer(uint8_t *dst, uint8_t b, uint8_t r);
    void eepromReadBlock(char a1, uint16_t readaddress, uint8_t *target, uint16_t length);
    void epdReset();
    void loadLUTSfromEEPROM(uint8_t bracket, bool doRed);
    uint8_t *loadLUT(uint8_t index, uint8_t bracket);
    void loadTempVSH(uint8_t bracket);
    void loadTempVCOMDC(uint8_t bracket);
    void loadFrameRatePLL(uint8_t bracket);
    uint8_t getTempBracket();
    uint8_t EPDtempBracket = 0;
    uint8_t vshc;
    uint8_t vslc;
};
