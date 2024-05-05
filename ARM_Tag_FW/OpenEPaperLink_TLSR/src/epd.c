#include <stdint.h>
#include "tl_common.h"
#include "main.h"
#include "epd.h"
#include "epd_spi.h"
#include "eeprom.h"
#include "epd_bw_213.h"
#include "epd_bwr_213.h"
#include "epd_bwr_350.h"
#include "epd_bwy_350.h"
#include "epd_bw_213_ice.h"
#include "epd_bwr_154.h"
#include "powermgt.h"

#include "OneBitDisplay.h"
extern const uint8_t ucMirror[];
#include "Roboto_Black_80.h"
#include "font.h"
#include "font_60.h"
#include "font16.h"
#include "font30.h"

RAM uint8_t epd_model = 0; // 0 = Undetected, 1 = BW213, 2 = BWR213, 3 = BWR154, 4 = BW213ICE, 5 = BWR350
const char *epd_model_string[] = {"NC", "BW213", "BWR213", "BWR154", "213ICE", "BWR350", "BWY350"};
RAM uint8_t epd_update_state = 0;

const char *BLE_conn_string[] = {"", "B"};
RAM uint8_t epd_temperature_is_read = 0;
RAM uint8_t epd_temperature = 0;

uint8_t *epd_buffer = (uint8_t *)0x84C000;
uint8_t *epd_temp = (uint8_t *)(0x84C000 + epd_buffer_size);
OBDISP obd;                        // virtual display structure

// With this we can force a display if it wasnt detected correctly
void set_EPD_model(uint8_t model_nr)
{
    epd_model = model_nr;
}

// Here we detect what E-Paper display is connected
 void EPD_detect_model(void)
{
    epd_model = 6;
    return;
    EPD_init();
    // system power
    EPD_POWER_ON();

    WaitMs(10);
    // Reset the EPD driver IC
    gpio_write(EPD_RESET, 0);
    WaitMs(10);
    gpio_write(EPD_RESET, 1);
    WaitMs(10);

    // Here we neeed to detect it
    if (EPD_BWR_213_detect())
    {
        epd_model = 2;
    }
    else if (EPD_BWR_154_detect()) // Right now this will never trigger, the 154 is same to 213BWR right now.
    {
        epd_model = 3;
    }
    else if (EPD_BW_213_ice_detect())
    {
        epd_model = 4;
    }
    else
    {
        epd_model = 1;
    }

    EPD_POWER_OFF();
}

 uint8_t EPD_read_temp(void)
{
    if (epd_temperature_is_read)
        return epd_temperature;

    if (!epd_model)
        EPD_detect_model();

    EPD_init();
    // system power
    EPD_POWER_ON();
    WaitMs(5);
    // Reset the EPD driver IC
    gpio_write(EPD_RESET, 0);
    WaitMs(10);
    gpio_write(EPD_RESET, 1);
    WaitMs(10);

    if (epd_model == 1)
        epd_temperature = EPD_BW_213_read_temp();
    else if (epd_model == 2)
        epd_temperature = EPD_BWR_213_read_temp();
    else if (epd_model == 3)
        epd_temperature = EPD_BWR_154_read_temp();
    else if (epd_model == 4)
        epd_temperature = EPD_BW_213_ice_read_temp();
    else if (epd_model == 5)
        epd_temperature = EPD_BWR_350_read_temp();
    else if (epd_model == 6)
        epd_temperature = EPD_BWY_350_read_temp();

    EPD_POWER_OFF();

    epd_temperature_is_read = 1;

    return epd_temperature;
}

 void EPD_Display_start(uint8_t full_or_partial)
{
    if (!epd_model)
        EPD_detect_model();

    EPD_init();
    // system power
    EPD_POWER_ON();
    WaitMs(5);
    // Reset the EPD driver IC
    gpio_write(EPD_RESET, 0);
    WaitMs(20);
    gpio_write(EPD_RESET, 1);
    WaitMs(20);

    /*if (epd_model == 1)
        epd_temperature = EPD_BW_213_Display_start(full_or_partial);
    else if (epd_model == 2)
        epd_temperature = EPD_BWR_213_Display_start(full_or_partial);
    else if (epd_model == 3)
        epd_temperature = EPD_BWR_154_Display_start(full_or_partial);
    else if (epd_model == 4)
        epd_temperature = EPD_BW_213_ice_Display_start(full_or_partial);
    else if (epd_model == 5)
        epd_temperature = EPD_BWR_350_Display_start(full_or_partial);
    else if (epd_model == 6)
        epd_temperature = EPD_BWY_350_Display_start(full_or_partial);*/

    if (epd_model == 4)
        epd_temperature = EPD_BW_213_ice_Display_start(full_or_partial);
    else if (epd_model == 5)
        epd_temperature = EPD_BWR_350_Display_start(full_or_partial);
    else if (epd_model == 6)
        epd_temperature = EPD_BWY_350_Display_start(full_or_partial);
    epd_temperature_is_read = 1;
    epd_update_state = 1;
}

 void EPD_Display_byte(uint8_t data)
{
    /*if (epd_model == 1)
        epd_temperature = EPD_BW_213_Display_byte(data);
    else if (epd_model == 2)
        epd_temperature = EPD_BWR_213_Display_byte(data);
    else if (epd_model == 3)
        epd_temperature = EPD_BWR_154_Display_byte(data);
    else if (epd_model == 4)
        epd_temperature = EPD_BW_213_ice_Display_byte(data);
    else if (epd_model == 5)
        epd_temperature = EPD_BWR_350_Display_byte(data);
    else if (epd_model == 6)
        epd_temperature = EPD_BWY_350_Display_byte(data);*/

    if (epd_model == 4)
        EPD_BW_213_ice_Display_byte(data);
    else if (epd_model == 5)
        EPD_BWR_350_Display_byte(data);
    else if (epd_model == 6)
        EPD_BWY_350_Display_byte(data);
}

 void EPD_Display_buffer(unsigned char *image, int size)
{
    /*if (epd_model == 1)
        epd_temperature = EPD_BW_213_Display_buffer(image, size);
    else if (epd_model == 2)
        epd_temperature = EPD_BWR_213_Display_buffer(image, size);
    else if (epd_model == 3)
        epd_temperature = EPD_BWR_154_Display_buffer(image, size);
    else if (epd_model == 4)
        epd_temperature = EPD_BW_213_ice_Display_buffer(image, size);
    else if (epd_model == 5)
        epd_temperature = EPD_BWR_350_Display_buffer(image, size);
    else if (epd_model == 6)
        epd_temperature = EPD_BWY_350_Display_buffer(image, size);*/

    if (epd_model == 4)
        EPD_BW_213_ice_Display_buffer(image, size);
    else if (epd_model == 5)
        EPD_BWR_350_Display_buffer(image, size);
    else if (epd_model == 6)
        EPD_BWY_350_Display_buffer(image, size);
}

 void EPD_Display_color_change()
{
    if (epd_model == 5)
        EPD_BWR_350_Display_color_change();
    if (epd_model == 6)
        EPD_BWY_350_Display_color_change();
}

 void EPD_Display_end()
{
    /*if (epd_model == 1)
        epd_temperature = EPD_BW_213_Display_end();
    else if (epd_model == 2)
        epd_temperature = EPD_BWR_213_Display_end();
    else if (epd_model == 3)
        epd_temperature = EPD_BWR_154_Display_end();
    else if (epd_model == 4)
        epd_temperature = EPD_BW_213_ice_Display_end();
    else if (epd_model == 5)
        epd_temperature = EPD_BWR_350_Display_end();
    else if (epd_model == 6)
        epd_temperature = EPD_BWY_350_Display_end();*/
    if (epd_model == 4)
        EPD_BW_213_ice_Display_end();
    else if (epd_model == 5)
        EPD_BWR_350_Display_end();
    else if (epd_model == 6)
        EPD_BWY_350_Display_end();
    uint32_t timeout_counter = 60; // 60 Seconds timeout
    while (epd_state_handler())
    {
        doSleepGpio(1000, EPD_BUSY);
        timeout_counter--;
        if (!timeout_counter)
        {
            epd_set_sleep();
            printf("E-Paper Refresh timeout !!!\r\n");
            break;
        }
    }
}

 void EPD_Display(unsigned char *image, int size, uint8_t full_or_partial)
{
    if (!epd_model)
        EPD_detect_model();

    EPD_init();
    // system power
    EPD_POWER_ON();
    WaitMs(5);
    // Reset the EPD driver IC
    gpio_write(EPD_RESET, 0);
    WaitMs(10);
    gpio_write(EPD_RESET, 1);
    WaitMs(10);

    if (epd_model == 1)
        epd_temperature = EPD_BW_213_Display(image, size, full_or_partial);
    else if (epd_model == 2)
        epd_temperature = EPD_BWR_213_Display(image, size, full_or_partial);
    else if (epd_model == 3)
        epd_temperature = EPD_BWR_154_Display(image, size, full_or_partial);
    else if (epd_model == 4)
        epd_temperature = EPD_BW_213_ice_Display(image, size, full_or_partial);
    else if (epd_model == 5)
        epd_temperature = EPD_BWR_350_Display(image, size, full_or_partial);
    else if (epd_model == 6)
        epd_temperature = EPD_BWY_350_Display(image, size, full_or_partial);

    epd_temperature_is_read = 1;
    epd_update_state = 1;
    uint32_t timeout_counter = 60; // 60 Seconds timeout
    while (epd_state_handler())
    {
        doSleepGpio(1000, EPD_BUSY);
        timeout_counter--;
        if (!timeout_counter)
        {
            epd_set_sleep();
            printf("E-Paper Refresh timeout !!!\r\n");
            break;
        }
    }
}

 void epd_set_sleep(void)
{
    if (!epd_model)
        EPD_detect_model();

    if (epd_model == 1)
        EPD_BW_213_set_sleep();
    else if (epd_model == 2)
        EPD_BWR_213_set_sleep();
    else if (epd_model == 3)
        EPD_BWR_154_set_sleep();
    else if (epd_model == 4)
        EPD_BW_213_ice_set_sleep();
    else if (epd_model == 5)
        EPD_BWR_350_set_sleep();
    else if (epd_model == 6)
        EPD_BWY_350_set_sleep();

    EPD_POWER_OFF();
    epd_update_state = 0;
}

 uint8_t epd_state_handler(void)
{
    switch (epd_update_state)
    {
    case 0:
        // Nothing todo
        break;
    case 1: // check if refresh is done and sleep epd if so
        if (epd_model == 1 || epd_model == 6)
        {
            if (!EPD_IS_BUSY())
                epd_set_sleep();
        }
        else
        {
            if (EPD_IS_BUSY())
                epd_set_sleep();
        }
        break;
    }
    return epd_update_state;
}

 void FixBuffer(uint8_t *pSrc, uint8_t *pDst, uint16_t width, uint16_t height)
{
    int x, y;
    uint8_t *s, *d;
    for (y = 0; y < (height / 8); y++)
    { // byte rows
        d = &pDst[y];
        s = &pSrc[y * width];
        for (x = 0; x < width; x++)
        {
            d[x * (height / 8)] = ~ucMirror[s[width - 1 - x]]; // invert and flip
        }                                                      // for x
    }                                                          // for y
}

 void epd_display_this()
{
    // test G4 decoder
    memset(epd_buffer, 0xff, epd_buffer_size); // clear to white
    EPD_Display(epd_buffer, epd_buffer_size, 1);
}

 void epd_display(char *str1, uint16_t battery_mv, char *ownMacStr, uint8_t full_or_partial)
{
    if (epd_update_state)
        return;

    if (!epd_model)
    {
        EPD_detect_model();
    }
    uint16_t resolution_w = 250;
    uint16_t resolution_h = 128; // 122 real pixel, but needed to have a full byte
    if (epd_model == 1)
    {
        resolution_w = 250;
        resolution_h = 128; // 122 real pixel, but needed to have a full byte
    }
    else if (epd_model == 2)
    {
        resolution_w = 250;
        resolution_h = 128; // 122 real pixel, but needed to have a full byte
    }
    else if (epd_model == 3)
    {
        resolution_w = 200;
        resolution_h = 200;
    }
    else if (epd_model == 4)
    {
        resolution_w = 212;
        resolution_h = 104;
    }
    else if (epd_model == 5)
    { // Just as placeholder right now, needs a complete different driving because of RAM limits
        resolution_w = 200;
        resolution_h = 184;
    }
    else if (epd_model == 6)
    { // Just as placeholder right now, needs a complete different driving because of RAM limits
        resolution_w = 210;
        resolution_h = 184;
    }

    obdCreateVirtualDisplay(&obd, resolution_w, resolution_h, epd_temp);
    obdFill(&obd, 0, 0); // fill with white

    char buff[100];
    sprintf(buff, "Compiled:");
    obdWriteString(&obd, 0, 0, 20, (char *)buff, FONT_8x8, 0, 0);
    sprintf(buff, "%s %s", __DATE__, __TIME__);
    obdWriteString(&obd, 0, 0, 21, (char *)buff, FONT_8x8, 0, 0);
    sprintf(buff, "Tag MAC:", ownMacStr);
    // obdWriteStringCustom(&obd, FONT_8x8, 0, 18, (char *)buff, 1);
    obdWriteString(&obd, 0, 0, 17, (char *)buff, FONT_8x8, 0, 0);
    sprintf(buff, "%s", ownMacStr);
    // obdWriteStringCustom(&obd, FONT_8x8, 0, 18, (char *)buff, 1);
    obdWriteString(&obd, 0, 0, 18, (char *)buff, FONT_8x8, 0, 0);
    sprintf(buff, "OpenEpaperLink");
    obdWriteStringCustom(&obd, (GFXfont *)&Dialog_plain_16, 10, 50, (char *)buff, 1);
    sprintf(buff, "%s", str1);
    obdWriteStringCustom(&obd, (GFXfont *)&Special_Elite_Regular_30, 10, 95, (char *)buff, 1);
    sprintf(buff, "Battery %dmV", battery_mv);
    obdWriteStringCustom(&obd, (GFXfont *)&Dialog_plain_16, 10, 120, (char *)buff, 1);
    FixBuffer(epd_temp, epd_buffer, resolution_w, resolution_h);
    EPD_Display(epd_buffer, resolution_w * resolution_h / 8, full_or_partial);
}

 void epd_display_char(uint8_t data)
{
    int i;
    for (i = 0; i < epd_buffer_size; i++)
    {
        epd_buffer[i] = data;
    }
    EPD_Display(epd_buffer, epd_buffer_size, 1);
}

 void epd_clear(void)
{
    memset(epd_buffer, 0x00, epd_buffer_size);
}
