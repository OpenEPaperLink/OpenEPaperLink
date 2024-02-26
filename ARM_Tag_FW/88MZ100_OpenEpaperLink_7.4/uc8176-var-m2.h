#pragma once
// #include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "epd_interface.h"

class uc8176 : public epdInterface {
   public:
    ~uc8176();
    void epdSetup();
    void epdEnterSleep();
    void draw();
    void drawNoWait();
    void epdWaitRdy();
    void selectLUT(uint8_t lut);

   protected:
    uint8_t drawLut;
    void epdWriteDisplayData();
    void epdReset();
};
