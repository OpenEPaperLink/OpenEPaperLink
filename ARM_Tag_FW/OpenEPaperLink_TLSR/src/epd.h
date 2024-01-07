#pragma once

#define epd_height 200
#define epd_width 200
#define epd_buffer_size ((epd_height / 8) * epd_width)

void set_EPD_model(uint8_t model_nr);
void init_epd(void);
uint8_t EPD_read_temp(void);

void EPD_Display_start(uint8_t full_or_partial);
void EPD_Display_byte(uint8_t data);
void EPD_Display_buffer(unsigned char *image, int size);
void EPD_Display_color_change();
void EPD_Display_end();

void EPD_Display(unsigned char *image, int size, uint8_t full_or_partial);
void epd_display_this();
void epd_display(char *str1, uint16_t battery_mv, char *ownMacStr, uint8_t full_or_partial);
void epd_set_sleep(void);
uint8_t epd_state_handler(void);
void epd_display_char(uint8_t data);
void epd_clear(void);