#pragma once
#include <Arduino.h>

int8_t startHFCLK(void);
uint8_t isHFCLKstable(void);

void boardGetOwnMac(uint8_t *mac);