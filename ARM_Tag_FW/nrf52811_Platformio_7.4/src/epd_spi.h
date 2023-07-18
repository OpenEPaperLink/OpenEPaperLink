#pragma once

void spi_write(uint8_t data);
void epd_cmd(uint8_t data);
void epd_data(uint8_t data);
void waitBusy();