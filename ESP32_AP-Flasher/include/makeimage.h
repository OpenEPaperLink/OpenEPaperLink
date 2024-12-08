#include <Arduino.h>
#include <TFT_eSPI.h>

#pragma once

#include "tag_db.h"

extern TFT_eSPI tft;

#define SHORTLUT_DISABLED 0
#define SHORTLUT_ONLY_BLACK 1
#define SHORTLUT_ALLOWED 2

struct imgParam {
    HwType hwdata;

    bool hasRed;
    uint8_t dataType;
    uint8_t dither;
    // bool grayLut = false;
    uint8_t bufferbpp = 8;
    uint8_t rotate = 0;
    uint16_t highlightColor = 2;

    uint16_t width;
    uint16_t height;
    uint8_t rotatebuffer;
    uint8_t bpp;

    char segments[12];
    uint16_t symbols;
    uint8_t invert;

    uint8_t lut;
    uint8_t shortlut;

    bool preload;
    uint8_t preloadtype;
    uint8_t preloadlut;

    uint8_t zlib;
    uint8_t g5;
};

void spr2buffer(TFT_eSprite &spr, String &fileout, imgParam &imageParams);
void jpg2buffer(String filein, String fileout, imgParam &imageParams);
