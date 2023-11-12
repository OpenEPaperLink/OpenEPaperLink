#include <Arduino.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "drawing.h"
#include "settings.h"
#include "hal.h"
#include "wdt.h"

#define CMD_PANEL_SETTING 0x00
#define CMD_POWER_SETTING 0x01
#define CMD_POWER_OFF 0x02
#define CMD_POWER_OFF_SEQUENCE 0x03
#define CMD_POWER_ON 0x04
#define CMD_POWER_ON_MEASURE 0x05
#define CMD_BOOSTER_SOFT_START 0x06
#define CMD_DEEP_SLEEP 0x07
#define CMD_DISPLAY_START_TRANSMISSION_DTM1 0x10
#define CMD_DATA_STOP 0x11
#define CMD_DISPLAY_REFRESH 0x12
#define CMD_DISPLAY_START_TRANSMISSION_DTM2 0x13
#define CMD_PLL_CONTROL 0x30
#define CMD_TEMPERATURE_CALIB 0x40
#define CMD_TEMPERATURE_SELECT 0x41
#define CMD_TEMPERATURE_WRITE 0x42
#define CMD_TEMPERATURE_READ 0x43
#define CMD_VCOM_INTERVAL 0x50
#define CMD_LOWER_POWER_DETECT 0x51
#define CMD_TCON_SETTING 0x60
#define CMD_RESOLUTION_SETING 0x61
#define CMD_REVISION 0x70
#define CMD_STATUS 0x71
#define CMD_AUTO_MEASUREMENT_VCOM 0x80
#define CMD_READ_VCOM 0x81
#define CMD_VCOM_DC_SETTING 0x82
#define CMD_PARTIAL_WINDOW 0x90
#define CMD_PARTIAL_IN 0x91
#define CMD_PARTIAL_OUT 0x92
#define CMD_PROGRAM_MODE 0xA0
#define CMD_ACTIVE_PROGRAM 0xA1
#define CMD_READ_OTP 0xA2
#define CMD_CASCADE_SET 0xE0
#define CMD_POWER_SAVING 0xE3
#define CMD_FORCE_TEMPERATURE 0xE5

static bool isInited = false;

void epdEnterSleep() {
    epdWrite(CMD_VCOM_INTERVAL, 1, 0x17);
    delay(10);
    epdWrite(CMD_VCOM_DC_SETTING, 1, 0x00);
    delay(10);
    epdWrite(CMD_POWER_OFF, 0);
    delay(10);
    epdWrite(CMD_DEEP_SLEEP, 1, 0xA5);
    delay(10);
    isInited = false;
}

void epdSetup() {
    epdReset();
    epdWrite(CMD_PANEL_SETTING, 1, 0x0F);
    epdWrite(CMD_VCOM_INTERVAL, 2, 0x30, 0x07);
    epdWrite(CMD_RESOLUTION_SETING, 4, SCREEN_WIDTH >> 8, SCREEN_WIDTH & 0xFF, SCREEN_HEIGHT >> 8, SCREEN_HEIGHT & 0xFF);
}

void selectLUT(uint8_t lut) {
    // implement alternative LUTs here. Currently just reset the watchdog to two minutes,
    // to ensure it doesn't reset during the much longer bootup procedure
    lut += 1;  // make the compiler a happy camper
    wdt120s();
    return;
}

void epdWriteDisplayData() {
    uint32_t start = millis();
    // this display expects two entire framebuffers worth of data to be written, one for b/w and one for red
    uint8_t *buf[2] = {0, 0};  // this will hold pointers to odd/even data lines
    for (uint8_t c = 0; c < 2; c++) {
        if (c == 0) epd_cmd(CMD_DISPLAY_START_TRANSMISSION_DTM1);
        if (c == 1) epd_cmd(CMD_DISPLAY_START_TRANSMISSION_DTM2);
        markData();
        epdSelect();
        for (uint16_t curY = 0; curY < SCREEN_HEIGHT; curY += 2) {
            // Get 'even' screen line
            buf[0] = (uint8_t *)calloc(SCREEN_WIDTH / 8, 1);
            drawItem::renderDrawLine(buf[0], curY, c);

            // on the first pass, the second (buf[1]) buffer is unused, so we don't have to wait for it to flush to the display / free it
            if (buf[1]) {
                // wait for 'odd' display line to finish writing to the screen
                epdSPIWait();
                free(buf[1]);
            }

            // start transfer of even data line to the screen
            epdSPIAsyncWrite(buf[0], (SCREEN_WIDTH / 8));

            // Get 'odd' screen display line
            buf[1] = (uint8_t *)calloc(SCREEN_WIDTH / 8, 1);
            drawItem::renderDrawLine(buf[1], curY + 1, c);

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
    printf("draw took %lu ms\n", millis() - start);
}

void draw() {
    drawNoWait();
    epdWaitRdy();
}
void drawNoWait() {
    epdWriteDisplayData();
    epdWrite(CMD_POWER_ON, 0);
    epdWaitRdy();
    epdWrite(CMD_DISPLAY_REFRESH, 0);
}
void epdWaitRdy() {
    epdBusyWaitRising(120000);
}