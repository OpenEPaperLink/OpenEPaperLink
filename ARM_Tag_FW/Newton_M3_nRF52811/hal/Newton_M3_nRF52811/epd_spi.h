#pragma once
#include <stdint.h>

void spi_write(uint8_t data);
void epd_cmd(uint8_t data);
void epd_data(uint8_t data);
void waitBusy();
void epdBusyWaitFalling(uint32_t timeout);
void epdBusyWaitRising(uint32_t timeout);