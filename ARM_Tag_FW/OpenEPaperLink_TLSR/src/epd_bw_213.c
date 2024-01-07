#include <stdint.h>
#include "tl_common.h"
#include "main.h"
#include "epd.h"
#include "epd_spi.h"
#include "epd_bw_213.h"

// UC8151C or similar EPD Controller

#define lut_bw_213_refresh_time 10
uint8_t lut_bw_213_20_part[] =
    {
        0x20, 0x00, lut_bw_213_refresh_time, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
uint8_t lut_bw_213_22_part[] =
    {
        0x22, 0x80, lut_bw_213_refresh_time, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
uint8_t lut_bw_213_23_part[] =
    {
        0x23, 0x40, lut_bw_213_refresh_time, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };


 uint8_t EPD_BW_213_read_temp(void)
{
    uint8_t epd_temperature = 0 ;
    EPD_WriteCmd(0x04);

    // check BUSY pin
    EPD_CheckStatus(100);

    EPD_WriteCmd(0x40);
    epd_temperature = EPD_SPI_read();
    EPD_SPI_read();

    // power off
    EPD_WriteCmd(0x02);

    // deep sleep
    EPD_WriteCmd(0x07);
    EPD_WriteData(0xa5);

    return epd_temperature;
}

 uint8_t EPD_BW_213_Display(unsigned char *image, int size, uint8_t full_or_partial)
{
    uint8_t epd_temperature = 0 ;
    
    // Booster soft start
    EPD_WriteCmd(0x06);
    EPD_WriteData(0x17);
    EPD_WriteData(0x17);
    EPD_WriteData(0x17);
    // power on
    EPD_WriteCmd(0x04);

    // check BUSY pin
    EPD_CheckStatus(100);

    EPD_WriteCmd(0x40);
    epd_temperature = EPD_SPI_read();
    EPD_SPI_read();

    // panel setting
    EPD_WriteCmd(0x00);
    if (full_or_partial)
        EPD_WriteData(0b00011111);
    else
        EPD_WriteData(0b00111111);
    EPD_WriteData(0x0f);

    // resolution setting
    EPD_WriteCmd(0x61);
    EPD_WriteData(0x80);
    EPD_WriteData(0x01);
    EPD_WriteData(0x28);

    // Vcom and data interval setting
    EPD_WriteCmd(0X50);
    EPD_WriteData(0x97);

    if (!full_or_partial)
    {
        EPD_send_lut(lut_bw_213_20_part, sizeof(lut_bw_213_20_part));
        EPD_send_empty_lut(0x21, 260);
        EPD_send_lut(lut_bw_213_22_part, sizeof(lut_bw_213_22_part));
        EPD_send_lut(lut_bw_213_23_part, sizeof(lut_bw_213_23_part));
        EPD_send_empty_lut(0x24, 260);

        EPD_WriteCmd(0x10);
        int i;
        for (i = 0; i < size; i++)
        {
            EPD_WriteData(~image[i]);
        }
    }
    // load image data to EPD
    EPD_LoadImage(image, size, 0x13);

    // trigger display refresh
    EPD_WriteCmd(0x12);

    return epd_temperature;
}

 void EPD_BW_213_set_sleep(void)
{
    // Vcom and data interval setting
    EPD_WriteCmd(0x50);
    EPD_WriteData(0xf7);

    // power off
    EPD_WriteCmd(0x02);

    // deep sleep
    EPD_WriteCmd(0x07);
    EPD_WriteData(0xa5);

}