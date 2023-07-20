#pragma once
#include <stdio.h>
#include <stdint.h>
#include <string.h>

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

void init_GPIO_EPD();
void display_send_buffer();

void epd_refresh_and_sleep();

void display_tx_byte(uint8_t data);
void display_send_start(uint8_t inverted);
void display_send_stop();

void init_epd();
void refresh_epd();