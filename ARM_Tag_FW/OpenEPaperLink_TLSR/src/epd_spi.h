#pragma once

#define EPD_POWER_ON() gpio_write(EPD_ENABLE, 0)

#define EPD_POWER_OFF() gpio_write(EPD_ENABLE, 1)

#define EPD_ENABLE_WRITE_CMD() gpio_write(EPD_DC, 0)
#define EPD_ENABLE_WRITE_DATA() gpio_write(EPD_DC, 1)

#define EPD_IS_BUSY() (!gpio_read(EPD_BUSY))


void EPD_init(void);
void EPD_SPI_Write(unsigned char value);
uint8_t EPD_SPI_read(void);
void EPD_WriteCmd(unsigned char cmd);
void EPD_WriteData(unsigned char data);
void EPD_CheckStatus(int max_ms);
void EPD_CheckStatus_inverted(int max_ms);
void EPD_send_lut(uint8_t lut[], int len);
void EPD_send_empty_lut(uint8_t lut, int len);
void EPD_LoadImage(unsigned char *image, int size, uint8_t cmd);