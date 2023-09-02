#pragma once
//#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define DISPLAY_WIDTH (640)
#define DISPLAY_HEIGHT (384)
#define DISPLAY_WIDTH_MM (164)
#define DISPLAY_HEIGHT_MM (97)

#define MAGNIFY1 3
#define MAGNIFY2 2
#define MAGNIFY3 1
#define BACK_COLOR 3
#define FORE_COLOR_1 0
#define FORE_COLOR_2 4
#define FORE_COLOR_3 0

#define EPD_LUT_DEFAULT 0
#define EPD_LUT_NO_REPEATS 1
#define EPD_LUT_FAST_NO_REDS 2
#define EPD_LUT_FAST 3

#define EPD_DIRECTION_X false
#define EPD_DIRECTION_Y true
#define EPD_SIZE_SINGLE false
#define EPD_SIZE_DOUBLE true
#define EPD_COLOR_RED true
#define EPD_COLOR_BLACK false

void init_GPIO_EPD();
void display_send_buffer();

void epd_refresh_and_sleep(uint8_t lut);

void display_tx_byte(uint8_t data);
void display_send_start(uint8_t inverted);
void display_send_stop();

void setDisplayWindow(uint16_t x, uint16_t y, uint16_t xe, uint16_t ye);

void init_epd();
void refresh_epd();

void lutBeginTX(uint8_t reg);
void lutEndTX();

void epd_pin_enable(int a1);


void fillWindow(uint16_t x, uint16_t y, uint16_t xe, uint16_t ye, uint8_t color);
void epdPrintf(uint16_t x, uint16_t y, bool color, const char* c, ...);

void interleaveColor(uint8_t b, uint8_t r);
void interleaveBW(uint8_t);

void drawImg(uint16_t x, uint16_t y, const uint8_t *img);