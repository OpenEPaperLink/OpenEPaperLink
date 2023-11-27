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

#define CMD_PANEL_SETTING 0x00
#define CMD_POWER_SETTING 0x01
#define CMD_POWER_OFF 0x02
#define CMD_POWER_OFF_SEQUENCE 0x03
#define CMD_POWER_ON 0x04
#define CMD_BOOSTER_SOFT_START 0x06
#define CMD_DEEP_SLEEP 0x07
#define CMD_DISPLAY_START_TRANSMISSION_DTM1 0x10
#define CMD_DATA_STOP 0x11
#define CMD_DISPLAY_REFRESH 0x12
#define CMD_DISPLAY_IMAGE_PROCESS 0x13
#define CMD_VCOM_LUT_C 0x20
#define CMD_LUT_B 0x21
#define CMD_LUT_W 0x22
#define CMD_LUT_G1 0x23
#define CMD_LUT_G2 0x24
#define CMD_LUT_R0 0x25
#define CMD_LUT_R1 0x26
#define CMD_LUT_R2 0x27
#define CMD_LUT_R3 0x28
#define CMD_LUT_XON 0x29
#define CMD_PLL_CONTROL 0x30
#define CMD_TEMPERATURE_DOREADING 0x40
#define CMD_TEMPERATURE_SELECT 0x41
#define CMD_TEMPERATURE_WRITE 0x42
#define CMD_TEMPERATURE_READ 0x43
#define CMD_VCOM_INTERVAL 0x50
#define CMD_LOWER_POWER_DETECT 0x51
#define CMD_TCON_SETTING 0x60
#define CMD_RESOLUTION_SETING 0x61
#define CMD_SPI_FLASH_CONTROL 0x65
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
#define CMD_EPD_EEPROM_SLEEP 0xB9
#define CMD_EPD_EEPROM_WAKE 0xAB
#define CMD_CASCADE_SET 0xE0
#define CMD_POWER_SAVING 0xE3
#define CMD_FORCE_TEMPERATURE 0xE5
#define CMD_LOAD_FLASH_LUT 0xE5

#define epdEepromSelect()           \
    do {                            \
        digitalWrite(EPD_HLT, LOW); \
    } while (0)

#define epdEepromDeselect()          \
    do {                             \
        digitalWrite(EPD_HLT, HIGH); \
    } while (0)

void dump(const uint8_t *a, const uint16_t l);

static void epdEepromRead(uint16_t addr, uint8_t *data, uint16_t len) {
    // return;
    epdWrite(CMD_SPI_FLASH_CONTROL, 1, 0x01);
    delay(1);
    epdEepromSelect();
    spi_write(0x03);  // EEPROM READ;
    spi_write(0x00);
    spi_write(addr >> 8);
    spi_write(addr & 0xFF);
    epdSPIReadBlock(data, len);
    epdEepromDeselect();
    delay(1);
    epdWrite(CMD_SPI_FLASH_CONTROL, 1, 0x00);
}
uint8_t getTempBracket() {
    uint8_t temptable[10];
    epdEepromRead(25002, temptable, 10);
    epdWrite(CMD_TEMPERATURE_DOREADING, 0);
    epdBusyWaitRising(1500);
    epdHardSPI(false);
    int8_t temp = spi3_read();
    temp <<= 1;
    temp |= (spi3_read() >> 7);

    uint8_t bracket = 0;
    for (int i = 0; i < 9; i++) {
        if ((((char)temp - (uint8_t)temptable[i]) & 0x80) != 0) {
            bracket = i;
            break;
        }
    }
    epdHardSPI(true);
    return bracket;
}
static void loadFrameRatePLL(uint8_t bracket) {
    uint8_t pllvalue;
    uint8_t plltable[10];
    epdEepromRead(0x6410, plltable, 10);
    pllvalue = plltable[bracket];
    if (!pllvalue) pllvalue = 0x3C;  // check if there's a valid pll value; if not; load preset
    epdWrite(CMD_PLL_CONTROL, 1, pllvalue);
}
static void loadTempVCOMDC(uint8_t bracket) {
    uint8_t vcomvalue;
    uint8_t vcomtable[10];
    epdEepromRead(25049, vcomtable, 10);
    vcomvalue = vcomtable[bracket];
    if (!vcomvalue) {
        // if we couldn't find the vcom table, then it's a fixed value sitting at 0x6400
        epdEepromRead(0x6400, vcomtable, 10);
        if (vcomtable[0])
            vcomvalue = vcomtable[0];
        else
            vcomvalue = 0x1E;  // check if there's a valid vcomvalue; if not; load preset
    }
    epdWrite(CMD_VCOM_DC_SETTING, 1, vcomvalue);
}

void epdEnterSleep() {
    epdWrite(CMD_POWER_OFF, 0);
    epdBusyWaitRising(250);
}
void epdSetup() {
    epdReset();
    digitalWrite(EPD_BS, LOW);

    epdWrite(CMD_PANEL_SETTING, 2, 0xEF, 0x08);  // default = 0xE7-0x08 // 0xEF-0x08  = right-side up
    epdWrite(CMD_POWER_SETTING, 4, 0x37, 0x00, 0x05, 0x05);
    epdWrite(CMD_POWER_OFF_SEQUENCE, 1, 0x00);
    epdWrite(CMD_BOOSTER_SOFT_START, 3, 0xC7, 0xCC, 0x1D);
    epdBusyWaitRising(250);
    epdWrite(CMD_POWER_ON, 0);
    epdBusyWaitRising(250);

    epdWrite(CMD_DISPLAY_IMAGE_PROCESS, 1, 0x00);
    epdWrite(CMD_PLL_CONTROL, 1, 0x3C);
    epdWrite(CMD_TEMPERATURE_SELECT, 1, 0x00);
    epdWrite(CMD_VCOM_INTERVAL, 1, 0x77);
    epdWrite(CMD_TCON_SETTING, 1, 0x22);
    epdWrite(CMD_RESOLUTION_SETING, 4, 0x02, 0x58, 0x01, 0xC0);  // set for 600x448
    epdWrite(CMD_SPI_FLASH_CONTROL, 1, 0x00);
    uint8_t bracket = getTempBracket();
    loadFrameRatePLL(bracket);
    epdBusyWaitRising(250);
    loadTempVCOMDC(bracket);
    epdBusyWaitRising(250);
}

static void interleaveColorToBuffer(uint8_t *dst, uint8_t b, uint8_t r) {
    b ^= 0xFF;
    uint8_t b_out = 0;
    for (int8_t shift = 3; shift >= 0; shift--) {
        b_out = 0;
        if (((b >> 2 * shift) & 0x01) && ((r >> 2 * shift) & 0x01)) {
            b_out |= 0x04;  // 0x30
        } else if ((b >> 2 * shift) & 0x01) {
            b_out |= 0x03;                     // 0x30
        } else if ((r >> 2 * shift) & 0x01) {  // 4 or 5
            b_out |= 0x04;                     // 0x30
        } else {
        }

        if (((b >> 2 * shift) & 0x02) && ((r >> 2 * shift) & 0x02)) {
            b_out |= 0x40;  // 0x30
        } else if ((b >> 2 * shift) & 0x02) {
            b_out |= 0x30;  // 0x30
        } else if ((r >> 2 * shift) & 0x02) {
            b_out |= 0x40;  // 0x30
        } else {
        }
        *dst++ = b_out;
    }
}

void selectLUT(uint8_t lut) {
    // implement alternative LUTs here. Currently just reset the watchdog to two minutes,
    // to ensure it doesn't reset during the much longer bootup procedure
    lut += 1;  // make the compiler a happy camper
    wdt120s();
    return;
}

static void epdWriteDisplayData() {
    uint8_t screenrow_bw[SCREEN_WIDTH / 8];
    uint8_t screenrow_r[SCREEN_WIDTH / 8];
    uint8_t screenrowInterleaved[SCREEN_WIDTH / 8 * 4];

    epd_cmd(CMD_DISPLAY_START_TRANSMISSION_DTM1);
    markData();
    epdSelect();
    for (uint16_t curY = 0; curY < SCREEN_HEIGHT; curY++) {
        memset(screenrow_bw, 0, SCREEN_WIDTH / 8);
        memset(screenrow_r, 0, SCREEN_WIDTH / 8);
        drawItem::renderDrawLine(screenrow_bw, curY, 0);
        drawItem::renderDrawLine(screenrow_r, curY, 1);
        if (curY != 0) {
            epdSPIWait();
            epdDeselect();
            epdSelect();
        }
        for (uint16_t curX = 0; curX < (SCREEN_WIDTH / 8); curX++) {
            interleaveColorToBuffer(screenrowInterleaved + (curX * 4), screenrow_bw[curX], screenrow_r[curX]);
        }
        epdSPIAsyncWrite(screenrowInterleaved, SCREEN_WIDTH / 8 * 4);
    }
    epdSPIWait();

    epdDeselect();
    epd_cmd(CMD_DATA_STOP);

    drawItem::flushDrawItems();
}

void draw() {
    delay(1);
    drawNoWait();
    epdBusyWaitRising(25000);
}
void drawNoWait() {
    epdWriteDisplayData();
    // epdWrite(CMD_LOAD_FLASH_LUT, 1, 0x03);
    epdWrite(CMD_DISPLAY_REFRESH, 0);
}
void drawWithSleep() {
    draw();
}
void epdWaitRdy() {
    epdBusyWaitRising(25000);
}