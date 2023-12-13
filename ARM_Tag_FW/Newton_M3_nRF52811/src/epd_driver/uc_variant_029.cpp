#include <Arduino.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hal.h"
#include "lut.h"
#include "settings.h"
#include "wdt.h"
#include "drawing.h"

#define EPD_CMD_POWER_OFF 0x02
#define EPD_CMD_POWER_ON 0x04
#define EPD_CMD_BOOSTER_SOFT_START 0x06
#define EPD_CMD_DEEP_SLEEP 0x07
#define EPD_CMD_DISPLAY_START_TRANSMISSION_DTM1 0x10
#define EPD_CMD_DISPLAY_REFRESH 0x12
#define EPD_CMD_DISPLAY_START_TRANSMISSION_DTM2 0x13
#define EPD_CMD_VCOM_INTERVAL 0x50
#define EPD_CMD_RESOLUTION_SETTING 0x61
#define EPD_CMD_UNKNOWN 0xF8

void epdEnterSleep() {
    epd_cmd(EPD_CMD_POWER_OFF);
    epdBusyWaitRising(50000);
    epdWrite(EPD_CMD_DEEP_SLEEP, 1, 0xA5);
    delay(200);
    delay(1);
}

void epdSetup() {
    epdReset();
    epdWrite(0x4D, 1, 0x55);
    epdWrite(0xF3, 1, 0x0A);
    epdWrite(0x31, 1, 0x00);
    epdWrite(0x06, 3, 0xE5, 0x35, 0x3C);
    epdWrite(0x50, 1, 0x57);
    epdWrite(0x00, 2, 0x03 | 0x04, 0x09);
}

void epdWriteDisplayData() {
    // send a dummy byte. Don't ask me why, it's what she likes. She'll sometimes display garbage on the b/w framebuffer if she doesn't get the dummy byte.
    epd_data(0x00);

    // this display expects two entire framebuffers worth of data to be written, one for b/w and one for red
    uint8_t* buf[2] = {0, 0};  // this will hold pointers to odd/even data lines
    for (uint8_t c = 0; c < 2; c++) {
        if (c == 0) epd_cmd(EPD_CMD_DISPLAY_START_TRANSMISSION_DTM1);
        if (c == 1) epd_cmd(EPD_CMD_DISPLAY_START_TRANSMISSION_DTM2);
        markData();
        epdSelect();
        for (uint16_t curY = 0; curY < SCREEN_HEIGHT; curY += 2) {
            // Get 'even' screen line
            buf[0] = (uint8_t*)calloc(SCREEN_WIDTH / 8, 1);
            drawItem::renderDrawLine(buf[0], curY, c);
            if (c == 0) {
                for (uint8_t c = 0; c < SCREEN_WIDTH / 8; c++) {
                    buf[0][c] = ~buf[0][c];
                }
            }

            // on the first pass, the second (buf[1]) buffer is unused, so we don't have to wait for it to flush to the display / free it
            if (buf[1]) {
                // wait for 'odd' display line to finish writing to the screen
                epdSPIWait();
                free(buf[1]);
            }

            // start transfer of even data line to the screen
            epdSPIAsyncWrite(buf[0], (SCREEN_WIDTH / 8));

            // Get 'odd' screen display line
            buf[1] = (uint8_t*)calloc(SCREEN_WIDTH / 8, 1);
            drawItem::renderDrawLine(buf[1], curY + 1, c);
            if (c == 0) {
                for (uint8_t c = 0; c < SCREEN_WIDTH / 8; c++) {
                    buf[1][c] = ~buf[1][c];
                }
            }

            // wait until the 'even' data has finished writing
            epdSPIWait();
            free(buf[0]);

            // start transfer of the 'odd' data line
            epdSPIAsyncWrite(buf[1], (SCREEN_WIDTH / 8));
        }
        // check if this was the first pass. If it was, we'll need to wait until the last display line finished writing
        if (c == 0) {
            epdSPIWait();
            epdDeselect();
            free(buf[1]);
            buf[1] = nullptr;
        }
    }
    // flush the draw list, make sure items don't appear on subsequent screens
    drawItem::flushDrawItems();

    // wait until the last line of display has finished writing and clean our stuff up
    epdSPIWait();
    epdDeselect();
    if (buf[1]) free(buf[1]);
}

void selectLUT(uint8_t lut) {
    // implement alternative LUTs here. Currently just reset the watchdog to two minutes,
    // to ensure it doesn't reset during the much longer bootup procedure
    lut += 1;  // make the compiler a happy camper
    wdt120s();
    return;
}

void draw() {
    drawNoWait();
    epdBusyWaitRising(50000);
}

void drawNoWait() {
    epdWriteDisplayData();
    epd_cmd(EPD_CMD_POWER_ON);
    epdBusyWaitRising(200);
    epd_cmd(EPD_CMD_DISPLAY_REFRESH);
}

void epdWaitRdy() {
    epdBusyWaitRising(50000);
}