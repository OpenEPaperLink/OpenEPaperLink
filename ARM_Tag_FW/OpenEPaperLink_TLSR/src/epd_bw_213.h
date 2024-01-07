#pragma once

uint8_t EPD_BW_213_read_temp(void);
uint8_t EPD_BW_213_Display(unsigned char *image, int size, uint8_t full_or_partial);
void EPD_BW_213_set_sleep(void);