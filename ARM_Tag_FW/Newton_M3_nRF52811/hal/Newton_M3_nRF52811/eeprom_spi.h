#pragma once
#include <stdint.h>

uint8_t eepromSPIByte(uint8_t data);
void eepromSPIBlockRead(uint8_t* dst, uint16_t len);
void eepromSPIBlockWrite(uint8_t* src, uint16_t len);
void setupEepromHWSPI(bool);