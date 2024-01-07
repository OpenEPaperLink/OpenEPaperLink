#pragma once

uint8_t EPD_BWR_213_detect(void);
uint8_t EPD_BWR_213_read_temp(void);
uint8_t EPD_BWR_213_Display(unsigned char *image, int size, uint8_t full_or_partial);
void EPD_BWR_213_set_sleep(void);