#include "drawing.h"

#include <stdbool.h>
#include "tl_common.h"
#include "board.h"
#include "eeprom.h"
#include "proto.h"
#include "screen.h"
#include "epd.h"

static uint8_t mClutMap[256];
void drawImageAtAddress(uint32_t addr, uint8_t lut)
{
    struct EepromImageHeader *eih = (struct EepromImageHeader *)mClutMap;
    eepromRead(addr, mClutMap, sizeof(struct EepromImageHeader));
    switch (eih->dataType)
    {
    case DATATYPE_IMG_RAW_1BPP:
        printf("Doing raw 1bpp\r\n");
        EPD_Display_start(1);
        for (uint32_t c = 0; c < (SCREEN_HEIGHT * (SCREEN_WIDTH / 8)); c++)
        {
            if (c % 256 == 0)
            {
                eepromRead(addr + sizeof(struct EepromImageHeader) + c, mClutMap, 256);
            }
            EPD_Display_byte(mClutMap[c % 256]);
        }
        EPD_Display_color_change();
        for (uint32_t c = 0; c < (SCREEN_HEIGHT * (SCREEN_WIDTH / 8)); c++)
        {
            EPD_Display_byte(0x00);
        }
        EPD_Display_end();
        break;
    case DATATYPE_IMG_RAW_2BPP:
        printf("Doing raw 2bpp\r\n");
        EPD_Display_start(1);
        for (uint32_t c = 0; c < (SCREEN_HEIGHT * (SCREEN_WIDTH / 8)); c++)
        {
            if (c % 256 == 0)
            {
                eepromRead(addr + sizeof(struct EepromImageHeader) + c, mClutMap, 256);
            }
            EPD_Display_byte(mClutMap[c % 256]);
        }
        EPD_Display_color_change();
        for (uint32_t c = 0; c < (SCREEN_HEIGHT * (SCREEN_WIDTH / 8)); c++)
        {
            if (c % 256 == 0)
            {
                eepromRead(addr + sizeof(struct EepromImageHeader) + (SCREEN_HEIGHT * (SCREEN_WIDTH / 8)) + c, mClutMap, 256);
            }
            EPD_Display_byte(mClutMap[c % 256]);
        }
        EPD_Display_end();
        break;
    case DATATYPE_IMG_BMP:;
        printf("sending BMP to EPD - ");

        printf(" complete.\r\n");
        break;
    default: // prevent drawing from an unknown file image type
        printf("Image with type 0x%02X was requested, but we don't know what to do with that currently...\r\n", eih->dataType);
        return;
    }
}