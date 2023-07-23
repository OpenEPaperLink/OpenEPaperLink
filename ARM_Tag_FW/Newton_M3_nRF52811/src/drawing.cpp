#include "drawing.h"

#include <Arduino.h>
#include <stdbool.h>

#include "eeprom.h"
#include "proto.h"
#include "userinterface.h"  // for addIcons

#include "hal.h"



#if (SCREEN_WIDTH == 152)
void drawImageFromBuffer(uint8_t *buffer, const uint8_t lut) {
    printf("Doing raw 1bpp\n");
    epdSetup();
    if (lut)
        selectLUT(lut);
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

#define EEPROM_XFER_BLOCKSIZE 512 // shouldn't be any less than 256 bytes probably

void drawImageAtAddress(uint32_t addr, uint8_t lut) {
    uint8_t* xferbuffer = (uint8_t*)malloc(EEPROM_XFER_BLOCKSIZE);
    struct EepromImageHeader *eih = (struct EepromImageHeader *)xferbuffer;
    eepromRead(addr, xferbuffer, sizeof(struct EepromImageHeader));
    switch (eih->dataType) {
        case DATATYPE_IMG_RAW_1BPP:
            printf("Doing raw 1bpp\n");
            epdSetup();
            if (lut)
                selectLUT(lut);
            beginFullscreenImage();
            clearScreen();
            beginWriteFramebuffer(EPD_COLOR_BLACK);
            epdSelect();
            for (uint32_t c = 0; c < (SCREEN_HEIGHT * (SCREEN_WIDTH / 8)); c++) {
                if (c % EEPROM_XFER_BLOCKSIZE == 0) {
                    epdDeselect();
                    eepromRead(addr + sizeof(struct EepromImageHeader) + c, xferbuffer, EEPROM_XFER_BLOCKSIZE);
                    epdSelect();
                }
                epdSend(xferbuffer[c % EEPROM_XFER_BLOCKSIZE]);
            }
            epdDeselect();
            endWriteFramebuffer();
            break;
        case DATATYPE_IMG_RAW_2BPP:
            printf("Doing raw 2bpp\n");
            epdSetup();
            if (lut)
                selectLUT(lut);
            beginFullscreenImage();
            beginWriteFramebuffer(EPD_COLOR_BLACK);
            epdSelect();
            for (uint32_t c = 0; c < (SCREEN_HEIGHT * (SCREEN_WIDTH / 8)); c++) {
                if (c % EEPROM_XFER_BLOCKSIZE == 0) {
                    epdDeselect();
                    eepromRead(addr + sizeof(struct EepromImageHeader) + c, xferbuffer, EEPROM_XFER_BLOCKSIZE);
                    epdSelect();
                }
                epdSend(xferbuffer[c % EEPROM_XFER_BLOCKSIZE]);
            }
            epdDeselect();
            endWriteFramebuffer();

            beginWriteFramebuffer(EPD_COLOR_RED);
            epdSelect();
            for (uint32_t c = 0; c < (SCREEN_HEIGHT * (SCREEN_WIDTH / 8)); c++) {
                if (c % EEPROM_XFER_BLOCKSIZE == 0) {
                    epdDeselect();
                    eepromRead(addr + sizeof(struct EepromImageHeader) + (SCREEN_HEIGHT * (SCREEN_WIDTH / 8)) + c, xferbuffer, EEPROM_XFER_BLOCKSIZE);
                    epdSelect();
                }
                epdSend(xferbuffer[c % EEPROM_XFER_BLOCKSIZE]);
            }
            epdDeselect();
            endWriteFramebuffer();
            break;
        default:  // prevent drawing from an unknown file image type
            printf("Image with type 0x%02X was requested, but we don't know what to do with that currently...\n", eih->dataType);
            free(xferbuffer);
            return;
    }
    free(xferbuffer);
    addOverlay();
    drawWithSleep();
}