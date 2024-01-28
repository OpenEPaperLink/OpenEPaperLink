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

void drawImageAtAddress(uint32_t addr, uint8_t lut) __reentrant {
    drawBuffer = malloc(512);
    if (!drawBuffer) {
#ifdef DEBUGDRAWING
        pr("DRAW: malloc during draw failed..\n");
#endif
        return;
    }
    static struct EepromImageHeader* __xdata eih;
    eih = (struct EepromImageHeader*)drawBuffer;
    eepromRead(addr, drawBuffer, sizeof(struct EepromImageHeader));
    switch (eih->dataType) {
        case DATATYPE_IMG_RAW_1BPP:
#ifdef DEBUGDRAWING
            pr("DRAW: Raw 1bpp with lut %d\n", lut);
#endif
            // epdSetup();
            if (lut) selectLUT(lut);
            beginFullscreenImage();
            clearScreen();

            beginWriteFramebuffer(EPD_COLOR_BLACK);
            #ifdef EPD_BYTEWISE_CS
            for (uint16_t c = 0; c < (SCREEN_HEIGHT * (SCREEN_WIDTH / 8)); c++) {
                if (c % 512 == 0) {
                    eepromRead(addr + sizeof(struct EepromImageHeader) + c, drawBuffer, 512);
                }
                epdSelect();
                epdSend(drawBuffer[c % 512]);
                epdDeselect();
            }
            #else
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
            #endif
            endWriteFramebuffer();

            break;
        case DATATYPE_IMG_RAW_2BPP:
#ifdef DEBUGDRAWING
            pr("DRAW: Raw 2bpp with lut %d\n", lut);
#endif
            if (lut) selectLUT(lut);
            beginFullscreenImage();


            beginWriteFramebuffer(EPD_COLOR_BLACK);
            #ifdef EPD_BYTEWISE_CS
            for (uint16_t c = 0; c < (SCREEN_HEIGHT * (SCREEN_WIDTH / 8)); c++) {
                if (c % 512 == 0) {
                    eepromRead(addr + sizeof(struct EepromImageHeader) + c, drawBuffer, 512);
                }
                epdSelect();
                epdSend(drawBuffer[c % 512]);
                epdDeselect();
            }
            #else
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
            #endif
            endWriteFramebuffer();

            beginWriteFramebuffer(EPD_COLOR_RED);
            #ifdef EPD_BYTEWISE_CS
            for (uint16_t c = 0; c < (SCREEN_HEIGHT * (SCREEN_WIDTH / 8)); c++) {
                if (c % 512 == 0) {
                    eepromRead(addr + sizeof(struct EepromImageHeader) + (SCREEN_HEIGHT * (SCREEN_WIDTH / 8)) + c, drawBuffer, 512);
                }
                epdSelect();
                epdSend(drawBuffer[c % 512]);
                epdDeselect();
            }
            #else
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
            #endif
            endWriteFramebuffer();
            break;
        default:  // prevent drawing from an unknown file image type
#ifdef DEBUGDRAWING
            pr("DRAW: Image with type 0x%02X was requested, but we don't know what to do with that currently...\n", eih->dataType);
#endif
            free(drawBuffer);
            return;
    }
    free(drawBuffer);
    addOverlay();
    drawWithSleep();
}