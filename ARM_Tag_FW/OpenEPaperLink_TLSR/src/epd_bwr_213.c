#include <stdint.h>
#include "tl_common.h"
#include "main.h"
#include "epd.h"
#include "epd_spi.h"
#include "epd_bwr_213.h"

// SSD1675 mixed with SSD1680 EPD Controller

#define BWR_213_Len 50
uint8_t LUT_bwr_213_part[] = {

0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

BWR_213_Len, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 
0x00, 0x00, 0x00, 

};

#define EPD_BWR_213_test_pattern 0xA5
 uint8_t EPD_BWR_213_detect(void)
{
    // SW Reset
    EPD_WriteCmd(0x12);
    WaitMs(10);

    EPD_WriteCmd(0x32);
    int i;
    for (i = 0; i < 153; i++)// This model has a 159 bytes LUT storage so we test for that
    {
        EPD_WriteData(EPD_BWR_213_test_pattern);
    }
    EPD_WriteCmd(0x33);
    for (i = 0; i < 153; i++)
    {
        if(EPD_SPI_read() != EPD_BWR_213_test_pattern)
            return 0;
    }
    return 1;
}

 uint8_t EPD_BWR_213_read_temp(void)
{
    uint8_t epd_temperature = 0 ;
    
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

 uint8_t EPD_BWR_213_Display(unsigned char *image, int size, uint8_t full_or_partial)
{    
    uint8_t epd_temperature = 0 ;
    
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

    EPD_WriteCmd(0x26);// RED Color TODO make something out of it :)
    int i;
    for (i = 0; i < size; i++)
    {
        EPD_WriteData(0x00);
    }

    if (!full_or_partial)
    {
        EPD_WriteCmd(0x32);
        for (i = 0; i < sizeof(LUT_bwr_213_part); i++)
        {
            EPD_WriteData(LUT_bwr_213_part[i]);
        }
    }
    
    // Display update control
    EPD_WriteCmd(0x22);
    EPD_WriteData(0xC7);
    
    // Master Activation
    EPD_WriteCmd(0x20);

    return epd_temperature;
}

 void EPD_BWR_213_set_sleep(void)
{
    // deep sleep
    EPD_WriteCmd(0x10);
    EPD_WriteData(0x01);

}