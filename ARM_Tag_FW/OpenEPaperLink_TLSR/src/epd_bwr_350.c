#include <stdint.h>
#include "tl_common.h"
#include "main.h"
#include "epd.h"
#include "epd_spi.h"
#include "epd_bwr_350.h"

// SSD1675 mixed with SSD1680 EPD Controller

#define BWR_350_Len 50
uint8_t LUT_bwr_350_part[] = {

    0x01, 0x10, 0x18, 0x01, 0x32, 0x02, 0x01, 0x10, 0x08, 0x01, 0x03, 0x02,
    0x01, 0x01, 0x01, 0x0A, 0x05, 0x0A, 0x0F, 0x01, 0x01, 0x01, 0x14, 0x0F,
    0x02, 0x02, 0x01, 0x14, 0x01, 0x02, 0x02, 0x04, 0x02, 0x14, 0x12, 0x01,
    0x0A, 0x0F, 0x01, 0x02, 0x01, 0x01, 0x07, 0x05, 0x00, 0x22, 0x02, 0x01,
    0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x50, 0x18, 0x01,
    0x72, 0x02, 0x01, 0x10, 0x88, 0x01, 0x43,
    0x02, 0x01, 0x01, 0x01, 0x4A, 0x85, 0x4A,
    0x8F, 0x01, 0x01, 0x01, 0x94, 0x8F, 0x82,
    0x42, 0x01, 0x14, 0x01, 0x82, 0x42, 0x84,
    0x42, 0x14, 0x12, 0x01, 0x4A, 0x4F, 0x01,
    0x02, 0x01, 0x01, 0x07, 0x85, 0x00, 0xE2,
    0x02, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x50, 0x58, 0x81,
    0x72, 0x02, 0x01, 0x10, 0x88, 0x01, 0x43,
    0x42, 0x01, 0x01, 0x01, 0x4A, 0x85, 0x4A,
    0x8F, 0x01, 0x01, 0x01, 0x94, 0x8F, 0x82,
    0x42, 0x01, 0x14, 0x01, 0x82, 0x42, 0x84,
    0x42, 0x14, 0x12, 0x01, 0x4A, 0x4F, 0x01,
    0x02, 0x01, 0x01, 0x07, 0x85, 0x00, 0x22,
    0x02, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x50, 0x58, 0x01,
    0x72, 0x02, 0x01, 0x10, 0x88, 0x81, 0x43,
    0x02, 0x01, 0x01, 0x01, 0x4A, 0x85, 0x4A,
    0x8F, 0x01, 0x01, 0x01, 0x94, 0x8F, 0x82,
    0x42, 0x01, 0x14, 0x01, 0x82, 0x42, 0x84,
    0x42, 0x14, 0x12, 0x01, 0x4A, 0x4F, 0x01,
    0x82, 0x01, 0x01, 0x07, 0x05, 0x00, 0x22,
    0x42, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01,
    0x02, 0x00, 0x00,
    0x22, 0x17, 0x41, 0xA8, 0x32, 0x50};

#define EPD_BWR_350_test_pattern 0xA5
 uint8_t EPD_BWR_350_detect(void)
{
    // SW Reset
    EPD_WriteCmd(0x12);
    WaitMs(10);

    EPD_WriteCmd(0x32);
    int i;
    for (i = 0; i < 153; i++) // This model has a 159 bytes LUT storage so we test for that
    {
        EPD_WriteData(EPD_BWR_350_test_pattern);
    }
    EPD_WriteCmd(0x33);
    for (i = 0; i < 153; i++)
    {
        if (EPD_SPI_read() != EPD_BWR_350_test_pattern)
            return 0;
    }
    return 1;
}

 uint8_t EPD_BWR_350_read_temp(void)
{
    uint8_t epd_temperature = 0;

    // SW Reset
    EPD_WriteCmd(0x12);

    EPD_CheckStatus_inverted(100);

    // Set Analog Block control
    EPD_WriteCmd(0x74);
    EPD_WriteData(0x54);
    // Set Digital Block control
    EPD_WriteCmd(0x7E);
    EPD_WriteData(0x3B);

    // Booster soft start
    EPD_WriteCmd(0x0C);
    EPD_WriteData(0x8B);
    EPD_WriteData(0x9C);
    EPD_WriteData(0x96);
    EPD_WriteData(0x0F);

    // Driver output control
    EPD_WriteCmd(0x01);
    EPD_WriteData(0x28);
    EPD_WriteData(0x01);
    EPD_WriteData(0x01);

    // Data entry mode setting
    EPD_WriteCmd(0x11);
    EPD_WriteData(0x01);

    // Set RAM X- Address Start/End
    EPD_WriteCmd(0x44);
    EPD_WriteData(0x00);
    EPD_WriteData(0x0F);

    // Set RAM Y- Address Start/End
    EPD_WriteCmd(0x45);
    EPD_WriteData(0x28);
    EPD_WriteData(0x01);
    EPD_WriteData(0x2E);
    EPD_WriteData(0x00);

    // Border waveform control
    EPD_WriteCmd(0x3C);
    EPD_WriteData(0x05);

    // Display update control
    EPD_WriteCmd(0x21);
    EPD_WriteData(0x00);
    EPD_WriteData(0x80);

    // Temperature sensor control
    EPD_WriteCmd(0x18);
    EPD_WriteData(0x80);

    // Display update control
    EPD_WriteCmd(0x22);
    EPD_WriteData(0xB1);

    // Master Activation
    EPD_WriteCmd(0x20);

    EPD_CheckStatus_inverted(100);

    // Temperature sensor read from register
    EPD_WriteCmd(0x1B);
    epd_temperature = EPD_SPI_read();
    EPD_SPI_read();

    WaitMs(5);

    // deep sleep
    EPD_WriteCmd(0x10);
    EPD_WriteData(0x01);

    return epd_temperature;
}

 uint8_t EPD_BWR_350_Display_start(uint8_t full_or_partial)
{
    uint8_t epd_temperature = 0;

    // SW Reset
    EPD_WriteCmd(0x12);

    EPD_CheckStatus_inverted(100);
    EPD_WriteCmd(0x3c);
    EPD_WriteData(0x05);
    EPD_WriteCmd(0x18);
    EPD_WriteData(0x80);
    EPD_WriteCmd(0x22);
    EPD_WriteData(0xb1);

    EPD_WriteCmd(0x20);

    EPD_CheckStatus_inverted(100);

    EPD_WriteCmd(0x1B);
    epd_temperature = EPD_SPI_read();
    EPD_SPI_read();

    WaitMs(5);

    EPD_WriteCmd(0x21);
    EPD_WriteData(0b00001000);
    EPD_WriteData(0x00);

    EPD_WriteCmd(0x11);
    EPD_WriteData(0x03);
    // Set RAM X- Address Start/End
    EPD_WriteCmd(0x44);
    EPD_WriteData(0x01);
    EPD_WriteData(0x17);

    // Set RAM Y- Address Start/End
    EPD_WriteCmd(0x45);
    EPD_WriteData(0x00);
    EPD_WriteData(0x00);
    EPD_WriteData(0x7f);
    EPD_WriteData(0x01);
    
    // Set RAM X address
    EPD_WriteCmd(0x4E);
    EPD_WriteData(0x01);

    // Set RAM Y address
    EPD_WriteCmd(0x4F);
    EPD_WriteData(0x00);
    EPD_WriteData(0x00);

    EPD_WriteCmd(0x24);

    return epd_temperature;
}
 void EPD_BWR_350_Display_byte(uint8_t data)
{
    EPD_WriteData(data);
}
 void EPD_BWR_350_Display_buffer(unsigned char *image, int size)
{
    for (int i = 0; i < size; i++)
    {
        EPD_WriteData(image[i]);
    }
}
 void EPD_BWR_350_Display_end()
{
    // Display update control
    EPD_WriteCmd(0x22);
    EPD_WriteData(0xC7);

    // Master Activation
    EPD_WriteCmd(0x20);
}
 void EPD_BWR_350_Display_color_change()
{
    // Set RAM X address
    EPD_WriteCmd(0x4E);
    EPD_WriteData(0x01);

    // Set RAM Y address
    EPD_WriteCmd(0x4F);
    EPD_WriteData(0x00);
    EPD_WriteData(0x00);

    EPD_WriteCmd(0x26);
}

 uint8_t EPD_BWR_350_Display(unsigned char *image, int size, uint8_t full_or_partial)
{
    uint8_t epd_temperature = 0;

    // SW Reset
    EPD_WriteCmd(0x12);

    EPD_CheckStatus_inverted(100);

    // Set Analog Block control
    EPD_WriteCmd(0x74);
    EPD_WriteData(0x54);
    // Set Digital Block control
    EPD_WriteCmd(0x7E);
    EPD_WriteData(0x3B);

    // Booster soft start
    EPD_WriteCmd(0x0C);
    EPD_WriteData(0x8B);
    EPD_WriteData(0x9C);
    EPD_WriteData(0x96);
    EPD_WriteData(0x0F);

    // Driver output control
    EPD_WriteCmd(0x01);
    EPD_WriteData(0x28);
    EPD_WriteData(0x01);
    EPD_WriteData(0x01);

    // Data entry mode setting
    EPD_WriteCmd(0x11);
    EPD_WriteData(0x01);

    // Set RAM X- Address Start/End
    EPD_WriteCmd(0x44);
    EPD_WriteData(0x00);
    EPD_WriteData(0x0F);

    // Set RAM Y- Address Start/End
    EPD_WriteCmd(0x45);
    EPD_WriteData(0x28);
    EPD_WriteData(0x01);
    EPD_WriteData(0x2E);
    EPD_WriteData(0x00);

    // Border waveform control
    EPD_WriteCmd(0x3C);
    EPD_WriteData(0x05);

    // Display update control
    EPD_WriteCmd(0x21);
    EPD_WriteData(0x00);
    EPD_WriteData(0x80);

    // Temperature sensor control
    EPD_WriteCmd(0x18);
    EPD_WriteData(0x80);

    // Display update control
    EPD_WriteCmd(0x22);
    EPD_WriteData(0xB1);

    // Master Activation
    EPD_WriteCmd(0x20);

    EPD_CheckStatus_inverted(100);

    // Temperature sensor read from register
    EPD_WriteCmd(0x1B);
    epd_temperature = EPD_SPI_read();
    EPD_SPI_read();

    WaitMs(5);

    // Set RAM X address
    EPD_WriteCmd(0x4E);
    EPD_WriteData(0x00);

    // Set RAM Y address
    EPD_WriteCmd(0x4F);
    EPD_WriteData(0x28);
    EPD_WriteData(0x01);

    EPD_LoadImage(image, size, 0x24);

    // Set RAM X address
    EPD_WriteCmd(0x4E);
    EPD_WriteData(0x00);

    // Set RAM Y address
    EPD_WriteCmd(0x4F);
    EPD_WriteData(0x28);
    EPD_WriteData(0x01);

    EPD_WriteCmd(0x26); // RED Color TODO make something out of it :)
    int i;
    for (i = 0; i < size; i++)
    {
        EPD_WriteData(0x00);
    }

    if (!full_or_partial)
    {
        EPD_WriteCmd(0x32);
        for (i = 0; i < sizeof(LUT_bwr_350_part); i++)
        {
            EPD_WriteData(LUT_bwr_350_part[i]);
        }
    }

    // Display update control
    EPD_WriteCmd(0x22);
    EPD_WriteData(0xC7);

    // Master Activation
    EPD_WriteCmd(0x20);

    return epd_temperature;
}

 void EPD_BWR_350_set_sleep(void)
{
    // deep sleep
    EPD_WriteCmd(0x10);
    EPD_WriteData(0x01);
}