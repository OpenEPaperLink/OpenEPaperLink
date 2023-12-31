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
#include <stdlib.h>

uint8_t __xdata* drawBuffer;

void drawImageAtAddress(uint32_t addr, uint8_t lut) {
    drawBuffer = malloc(512);
    if (!drawBuffer) {
        pr("malloc during draw failed..\n");
        return;
    }
    struct EepromImageHeader* __xdata eih = (struct EepromImageHeader*)drawBuffer;
    eepromRead(addr, drawBuffer, sizeof(struct EepromImageHeader));
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
                if (c % 512 == 0) {
                    epdDeselect();
                    eepromRead(addr + sizeof(struct EepromImageHeader) + c, drawBuffer, 512);
                    epdSelect();
                }
                epdSend(drawBuffer[c % 512]);
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
                if (c % 512 == 0) {
                    epdDeselect();
                    eepromRead(addr + sizeof(struct EepromImageHeader) + c, drawBuffer, 512);
                    epdSelect();
                }
                epdSend(drawBuffer[c % 512]);
            }
            epdDeselect();
            endWriteFramebuffer();

            beginWriteFramebuffer(EPD_COLOR_RED);
            epdSelect();
            for (uint16_t c = 0; c < (SCREEN_HEIGHT * (SCREEN_WIDTH / 8)); c++) {
                if (c % 512 == 0) {
                    epdDeselect();
                    eepromRead(addr + sizeof(struct EepromImageHeader) + (SCREEN_HEIGHT * (SCREEN_WIDTH / 8)) + c, drawBuffer, 512);
                    epdSelect();
                }
                epdSend(drawBuffer[c % 512]);
            }
            epdDeselect();
            endWriteFramebuffer();
            break;
        default:  // prevent drawing from an unknown file image type
            pr("Image with type 0x%02X was requested, but we don't know what to do with that currently...\n", eih->dataType);
            free(drawBuffer);
            return;
    }
    free(drawBuffer);
    addOverlay();
    drawWithSleep();
}