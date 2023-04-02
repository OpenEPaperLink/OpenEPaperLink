#include <Arduino.h>
#include <TFT_eSPI.h>

#pragma once

struct imgParam {
    bool hasRed;
    uint8_t dataType;
    bool dither;
};

void spr2buffer(TFT_eSprite &spr, String &fileout, imgParam &imageParams);
void jpg2buffer(String filein, String fileout, imgParam &imageParams);
