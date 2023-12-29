#include "drawing.h"

#include <stdbool.h>

#include "asmUtil.h"
#include "board.h"
#include "cpu.h"
#include "eeprom.h"
#include "screen.h"
#include "printf.h"
#include "../oepl-definitions.h"
#include "screen.h"
#include "timer.h"
#include "userinterface.h"  // for addIcons

extern uint8_t blockbuffer[];

#if (SCREEN_WIDTH == 152)
void drawImageFromBuffer(uint8_t* buffer, const uint8_t lut) {
    pr("Doing raw 1bpp\n");
    epdSetup();
    if (lut) selectLUT(lut);
    beginFullscreenImage();
    clearScreen();
    beginWriteFramebuffer(EPD_COLOR_BLACK);
    epdSelect();
    for (uint16_t c = 0; c < (SCREEN_HEIGHT * (SCREEN_WIDTH / 8)); c++) {
        epdSend(buffer[c]);
    }
    epdDeselect();
    endWriteFramebuffer();
    addOverlay();
    drawWithSleep();
}
#endif
void drawImageAtAddress(uint32_t addr, uint8_t lut) {
    struct EepromImageHeader* __xdata eih = (struct EepromImageHeader*)blockbuffer;
    eepromRead(addr, blockbuffer, sizeof(struct EepromImageHeader));
    switch (eih->dataType) {
        case DATATYPE_IMG_RAW_1BPP:
            pr("Doing raw 1bpp\n");
            epdSetup();
            if (lut) selectLUT(lut);
            beginFullscreenImage();
            clearScreen();
            beginWriteFramebuffer(EPD_COLOR_BLACK);
            epdSelect();
            for (uint16_t c = 0; c < (SCREEN_HEIGHT * (SCREEN_WIDTH / 8)); c++) {
                if (c % 256 == 0) {
                    epdDeselect();
                    eepromRead(addr + sizeof(struct EepromImageHeader) + c, blockbuffer, 256);
                    epdSelect();
                }
                epdSend(blockbuffer[c % 256]);
            }
            epdDeselect();
            endWriteFramebuffer();
            break;
        case DATATYPE_IMG_RAW_2BPP:
            pr("Doing raw 2bpp\n");
            epdSetup();
            if (lut) selectLUT(lut);
            beginFullscreenImage();
            beginWriteFramebuffer(EPD_COLOR_BLACK);
            epdSelect();
            for (uint16_t c = 0; c < (SCREEN_HEIGHT * (SCREEN_WIDTH / 8)); c++) {
                if (c % 256 == 0) {
                    epdDeselect();
                    eepromRead(addr + sizeof(struct EepromImageHeader) + c, blockbuffer, 256);
                    epdSelect();
                }
                epdSend(blockbuffer[c % 256]);
            }
            epdDeselect();
            endWriteFramebuffer();

            beginWriteFramebuffer(EPD_COLOR_RED);
            epdSelect();
            for (uint16_t c = 0; c < (SCREEN_HEIGHT * (SCREEN_WIDTH / 8)); c++) {
                if (c % 256 == 0) {
                    epdDeselect();
                    eepromRead(addr + sizeof(struct EepromImageHeader) + (SCREEN_HEIGHT * (SCREEN_WIDTH / 8)) + c, blockbuffer, 256);
                    epdSelect();
                }
                epdSend(blockbuffer[c % 256]);
            }
            epdDeselect();
            endWriteFramebuffer();
            break;
        default:  // prevent drawing from an unknown file image type
            pr("Image with type 0x%02X was requested, but we don't know what to do with that currently...\n", eih->dataType);
            return;
    }
    addOverlay();
    drawWithSleep();
}