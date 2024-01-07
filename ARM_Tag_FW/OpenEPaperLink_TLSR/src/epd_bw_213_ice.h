#pragma once

uint8_t EPD_BW_213_ice_detect(void);
uint8_t EPD_BW_213_ice_read_temp(void);

uint8_t EPD_BW_213_ice_Display_start(uint8_t full_or_partial);
void EPD_BW_213_ice_Display_byte(uint8_t data);
void EPD_BW_213_ice_Display_buffer(unsigned char *image, int size);
void EPD_BW_213_ice_Display_end();

uint8_t EPD_BW_213_ice_Display(unsigned char *image, int size, uint8_t full_or_partial);
void EPD_BW_213_ice_set_sleep(void);