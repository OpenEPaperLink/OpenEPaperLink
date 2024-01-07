#pragma once

uint8_t EPD_BWY_350_detect(void);
uint8_t EPD_BWY_350_read_temp(void);

uint8_t EPD_BWY_350_Display_start(uint8_t full_or_partial);
void EPD_BWY_350_Display_byte(uint8_t data);
void EPD_BWY_350_Display_buffer(unsigned char *image, int size);
void EPD_BWY_350_Display_end();
void EPD_BWY_350_Display_color_change();

uint8_t EPD_BWY_350_Display(unsigned char *image, int size, uint8_t full_or_partial);
void EPD_BWY_350_set_sleep(void);