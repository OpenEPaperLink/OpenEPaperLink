#pragma once
#include <stdbool.h>

void init_led();
void led_set(int nr, bool state);
void led_flash(int nr);
