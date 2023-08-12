#include "drawing.h"

#include <stdbool.h>
#include <stdbool.h>
#include "printf.h"

#include "board.h"
#include "eeprom.h"
#include "proto.h"
#include "timer.h"
#include "screen.h"
#include "util.h"
#include "epd.h"


struct BitmapClutEntry
{
    uint8_t b, g, r, x;
};


uint8_t mPassNo = 0;

static uint8_t mClutMap[256];
static uint8_t mClutMapRed[256];

void drawImageAtAddress(uint32_t addr, uint8_t lut)
{
    struct EepromImageHeader *eih = (struct EepromImageHeader *)mClutMap;
    eepromRead(addr, mClutMap, sizeof(struct EepromImageHeader));
    switch (eih->dataType)
    {
    case DATATYPE_IMG_RAW_1BPP:
        printf("Doing raw 1bpp\n");

        init_epd();
        display_send_start(1);
        for (uint32_t c = 0; c < (SCREEN_HEIGHT * (SCREEN_WIDTH / 8)); c++)
        {
            if (c % 256 == 0)
            {
                eepromRead(addr + sizeof(struct EepromImageHeader) + c, mClutMap, 256);
            }

            uint8_t curr_byte = mClutMap[c % 256]; // this one holds 8 pixel, we will translate them to 4 sendings

            for (int pixI = 0; pixI < 4; pixI++)
            {
                uint8_t pixel1 = (curr_byte & 0x80) ? 0 : 3;
                curr_byte <<= 1;
                uint8_t pixel2 = (curr_byte & 0x80) ? 0 : 3;
                curr_byte <<= 1;
                display_tx_byte((pixel1 << 4) | pixel2);
            }
        }
        display_send_stop();
        epd_refresh_and_sleep();
        break;
    case DATATYPE_IMG_RAW_2BPP:
        printf("Doing raw 2bpp\n");
        init_epd();
        display_send_start(1);
        for (uint32_t c = 0; c < (SCREEN_HEIGHT * (SCREEN_WIDTH / 8)); c++)
        {
            if (c % 256 == 0)
            {
                eepromRead(addr + sizeof(struct EepromImageHeader) + c, mClutMap, 256);
                eepromRead(addr + sizeof(struct EepromImageHeader) + (SCREEN_HEIGHT * (SCREEN_WIDTH / 8)) + c, mClutMapRed, 256);
            }

            uint8_t curr_byte = mClutMap[c % 256];       // this one holds 8 pixel, we will translate them to 4 sendings
            uint8_t curr_byteRed = mClutMapRed[c % 256]; // this one holds 8 pixel, we will translate them to 4 sendings

            for (int pixI = 0; pixI < 4; pixI++)
            {
                uint8_t pixel1 = (curr_byte & 0x80) ? 0 : 3;
                pixel1 = (curr_byteRed & 0x80) ? 4 : pixel1;
                curr_byte <<= 1;
                curr_byteRed <<= 1;
                uint8_t pixel2 = (curr_byte & 0x80) ? 0 : 3;
                pixel2 = (curr_byteRed & 0x80) ? 4 : pixel1;
                curr_byte <<= 1;
                curr_byteRed <<= 1;
                display_tx_byte((pixel1 << 4) | pixel2);
            }
        }
        display_send_stop();
        epd_refresh_and_sleep();
        break;
    default: // prevent drawing from an unknown file image type
        printf("Image with type 0x%02X was requested, but we don't know what to do with that currently...\n", eih->dataType);
        return;
    }
    // addOverlay();
    // drawWithSleep();
}