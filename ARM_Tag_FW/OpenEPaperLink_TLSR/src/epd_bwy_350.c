#include <stdint.h>
#include "tl_common.h"
#include "main.h"
#include "epd.h"
#include "epd_spi.h"
#include "epd_bwy_350.h"

// UC8151C or similar EPD Controller

enum PSR_FLAGS
{
    RES_96x230 = 0b00000000,
    RES_96x252 = 0b01000000,
    RES_128x296 = 0b10000000,
    RES_160x296 = 0b11000000,

    LUT_OTP = 0b00000000,
    LUT_REG = 0b00100000,

    FORMAT_BWR = 0b00000000,
    FORMAT_BW = 0b00010000,

    SCAN_DOWN = 0b00000000,
    SCAN_UP = 0b00001000,

    SHIFT_LEFT = 0b00000000,
    SHIFT_RIGHT = 0b00000100,

    BOOSTER_OFF = 0b00000000,
    BOOSTER_ON = 0b00000010,

    RESET_SOFT = 0b00000000,
    RESET_NONE = 0b00000001
};

#define lut_bwy_350_refresh_time 10
uint8_t lut_bwy_350_20_part[] =
    {
        0x20, 0x00, lut_bwy_350_refresh_time, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t lut_bwy_350_22_part[] =
    {
        0x22, 0x80, lut_bwy_350_refresh_time, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t lut_bwy_350_23_part[] =
    {
        0x23, 0x40, lut_bwy_350_refresh_time, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

uint8_t EPD_BWY_350_detect(void)
{
    // SW Reset
    /*EPD_WriteCmd(0x12);
    WaitMs(10);

    EPD_WriteCmd(0x2F);
    if (EPD_SPI_read() != 0x01)
        return 0;
    return 1;*/
    return 0;
}

uint8_t EPD_BWY_350_read_temp(void)
{
    uint8_t epd_temperature = 0;
    EPD_WriteCmd(0x04);

    WaitMs(1);

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

#define scan_direction (SCAN_UP | RES_160x296 | FORMAT_BWR | BOOSTER_ON | RESET_NONE | LUT_OTP | SHIFT_RIGHT)

uint8_t EPD_BWY_350_Display_start(uint8_t full_or_partial)
{
    uint8_t epd_temperature = 0;

    // power on
    EPD_WriteCmd(0x04);

    WaitMs(1);

    /*EPD_WriteCmd(0X4D);
    EPD_WriteData(0x55);
    EPD_WriteCmd(0XF3);
    EPD_WriteData(0x0A);
    EPD_WriteCmd(0X50);
    EPD_WriteData(0x57);*/

    EPD_WriteCmd(0x00);
    EPD_WriteData(scan_direction);
    EPD_WriteData(0x0f);

    EPD_WriteCmd(0x40);
    epd_temperature = EPD_SPI_read();
    EPD_SPI_read();

    EPD_WriteCmd(0x10);

    return epd_temperature;
}
void EPD_BWY_350_Display_byte(uint8_t data)
{
    EPD_WriteData(data);
}
void EPD_BWY_350_Display_buffer(unsigned char *image, int size)
{
    for (int i = 0; i < size; i++)
    {
        EPD_WriteData(image[i]);
    }
}

void EPD_BWY_350_Display_color_change()
{
    EPD_WriteCmd(0x13);
}

void EPD_BWY_350_Display_end()
{
    EPD_WriteCmd(0x12);
}

uint8_t EPD_BWY_350_Display(unsigned char *image, int size, uint8_t full_or_partial)
{
    uint8_t epd_temperature = 0;

    // power on
    EPD_WriteCmd(0x04);

    WaitMs(1);

    /*EPD_WriteCmd(0X4D);
    EPD_WriteData(0x55);
    EPD_WriteCmd(0XF3);
    EPD_WriteData(0x0A);
    EPD_WriteCmd(0X50);
    EPD_WriteData(0x57);*/
    EPD_WriteCmd(0x00);
    EPD_WriteData(scan_direction); //| LUT_REG);
    EPD_WriteData(0x0f);

    EPD_WriteCmd(0x40);
    epd_temperature = EPD_SPI_read();
    EPD_SPI_read();

    /*EPD_send_lut(lut_bwy_350_20_part, sizeof(lut_bwy_350_20_part));
    EPD_send_empty_lut(0x21, 260);
    EPD_send_lut(lut_bwy_350_22_part, sizeof(lut_bwy_350_22_part));
    EPD_send_lut(lut_bwy_350_23_part, sizeof(lut_bwy_350_23_part));
    EPD_send_empty_lut(0x24, 260);*/

    //////////////////////// This parts clears the full screen
    EPD_WriteCmd(0x10);
    int i;
    for (i = 0; i < 8832; i++)
    {
        EPD_WriteData(0);
    }

    EPD_WriteCmd(0x13);
    for (i = 0; i < 8832; i++)
    {
        EPD_WriteData(0);
    }
    //////////////////////// This parts clears the full screen

    EPD_WriteCmd(0x10);
    for (i = 0; i < size; i++)
    {
        EPD_WriteData(~image[i]);
    }
    // load image data to EPD
    // EPD_LoadImage(image, size, 0x13);

    // EPD_WriteCmd(0XB6);
    // EPD_WriteData(0x12);
    //  trigger display refresh
    EPD_WriteCmd(0x12);

    return epd_temperature;
}

void EPD_BWY_350_set_sleep(void)
{
    // Vcom and data interval setting
    // EPD_WriteCmd(0x50);
    // EPD_WriteData(0xf7);

    // power off
    EPD_WriteCmd(0x02);

    // deep sleep
    EPD_WriteCmd(0x07);
    EPD_WriteData(0xa5);
}
