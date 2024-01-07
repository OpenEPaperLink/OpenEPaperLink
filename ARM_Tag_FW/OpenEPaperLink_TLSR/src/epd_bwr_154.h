#pragma once

uint8_t EPD_BWR_154_detect(void);
uint8_t EPD_BWR_154_read_temp(void);
uint8_t EPD_BWR_154_Display(unsigned char *image, int size, uint8_t full_or_partial);
void EPD_BWR_154_set_sleep(void);