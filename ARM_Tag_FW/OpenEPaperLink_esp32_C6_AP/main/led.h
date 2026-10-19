#pragma once
#include <stdint.h>

void init_led();
void led_flash(int nr);
void led_set_brightness(uint8_t brightness);  // 0 = off, 255 = full
