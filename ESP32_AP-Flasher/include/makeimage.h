#include <Arduino.h>
#include <TFT_eSPI.h>

#pragma once

extern TFT_eSPI tft;

#define SHORTLUT_DISABLED 0
#define SHORTLUT_ONLY_BLACK 1
#define SHORTLUT_ALLOWED 2

struct imgParam {
    bool hasRed;
    uint8_t dataType;
    bool dither;
    bool grayLut = false;
    uint8_t bufferbpp = 8;
    uint8_t rotate = 0;

    uint16_t width;
    uint16_t height;
    uint8_t rotatebuffer;
    uint8_t bpp;

    char segments[12];
    uint16_t symbols;
    bool invert;

    uint8_t lut;
    uint8_t shortlut;

    bool preload;
    uint8_t preloadtype;
    uint8_t preloadlut;
};

void spr2buffer(TFT_eSprite &spr, String &fileout, imgParam &imageParams);
void jpg2buffer(String filein, String fileout, imgParam &imageParams);
