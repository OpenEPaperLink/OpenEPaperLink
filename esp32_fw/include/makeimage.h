#include <Arduino.h>
#include <TFT_eSPI.h>

#pragma once

struct BitmapFileHeader {
	uint8_t sig[2];
	uint32_t fileSz;
	uint8_t rfu[4];
	uint32_t dataOfst;
	uint32_t headerSz;			//40
	int32_t width;
	int32_t height;
	uint16_t colorplanes;		//must be one
	uint16_t bpp;
	uint32_t compression;
	uint32_t dataLen;			//may be 0
	uint32_t pixelsPerMeterX;
	uint32_t pixelsPerMeterY;
	uint32_t numColors;			//if zero, assume 2^bpp
	uint32_t numImportantColors;
	
} __attribute__((packed));

enum EinkClut {
	EinkClutTwoBlacks = 0,
	EinkClutTwoBlacksAndRed,
	EinkClutFourBlacks,
	EinkClutThreeBlacksAndRed,
};

void spr2grays(TFT_eSprite &spr, long w, long h, String &fileout);
void spr2buffer(TFT_eSprite &spr, String &fileout);
void jpg2buffer(String filein, String fileout);
void bmp2grays(String filein, String fileout);

