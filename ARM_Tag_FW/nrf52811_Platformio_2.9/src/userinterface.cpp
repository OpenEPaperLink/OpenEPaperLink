

#include "userinterface.h"

#include <stdbool.h>
#include <string.h>

#include "ssd1619.h"
#include "main.h"
#include "bitmaps.h"
#include "board.h"
#include "comms.h"
#include "font.h"
#include "lut.h"
#include "powermgt.h"
#include "proto.h"
#include "screen.h"
#include "settings.h"
#include "sleep.h"
#include "syncedproto.h"  // for APmac / Channel
#include "timer.h"

// extern uint8_t  mSelfMac[8];
// extern uint8_t  currentChannel;
// extern uint8_t  APmac[];
// extern uint16_t  batteryVoltage;

const uint8_t  fwVersion = FW_VERSION;
const char  fwVersionSuffix[] = FW_VERSION_SUFFIX;

extern uint8_t  capabilities;

bool  lowBatteryShown = false;
bool  noAPShown = false;

void addCapabilities() {
    if (capabilities) epdpr("Options: ");
    if (capabilities & CAPABILITY_HAS_NFC) {
        epdpr("-NFC");
        if (capabilities & CAPABILITY_NFC_WAKE) {
            epdpr("+WAKE");
        } else {
            epdpr(" ");
        }
    }
    if (capabilities & CAPABILITY_HAS_WAKE_BUTTON) {
        epdpr("-WAKE BUTTON");
    }
}

void addOverlay() {
    if (currentChannel == 0) {
#if (SCREEN_WIDTH == 152)
        loadRawBitmap((uint8_t *)ant, SCREEN_WIDTH - 16, 0, EPD_COLOR_BLACK);
        loadRawBitmap((uint8_t *)cross, SCREEN_WIDTH - 8, 7, EPD_COLOR_RED);
#elif (SCREEN_WIDTH == 168)
        loadRawBitmap((uint8_t *)ant, 0, 0, EPD_COLOR_BLACK);
        loadRawBitmap((uint8_t *)cross, 8, 0, EPD_COLOR_RED);
#elif (SCREEN_WIDTH == 128)
        loadRawBitmap((uint8_t *)ant, 0, 0, EPD_COLOR_BLACK);
        loadRawBitmap((uint8_t *)cross, 8, 0, EPD_COLOR_RED);
#elif (SCREEN_WIDTH == 400)
        loadRawBitmap((uint8_t *)ant, SCREEN_WIDTH - 24, 6, EPD_COLOR_BLACK);
        loadRawBitmap((uint8_t *)cross, SCREEN_WIDTH - 16, 13, EPD_COLOR_RED);
#endif
        noAPShown = true;
    } else {
        noAPShown = false;
    }

    if (batteryVoltage != 2600) {
#if (SCREEN_WIDTH == 152)
        loadRawBitmap((uint8_t *)battery, SCREEN_WIDTH - 16, SCREEN_HEIGHT - 10, EPD_COLOR_BLACK);
#elif (SCREEN_WIDTH == 400)
        loadRawBitmap((uint8_t *)battery, SCREEN_WIDTH - 24, SCREEN_HEIGHT - 16, EPD_COLOR_BLACK);
#elif (SCREEN_WIDTH == 128)
        loadRawBitmap((uint8_t *)battery, 112, 0, EPD_COLOR_BLACK);
#elif (SCREEN_WIDTH == 168)
        loadRawBitmap((uint8_t *)battery, 112, 0, EPD_COLOR_BLACK);
#endif
        lowBatteryShown = true;
    } else {
        lowBatteryShown = false;
    }
}


void afterFlashScreenSaver() {
    selectLUT(EPD_LUT_DEFAULT);
    clearScreen();
    setColorMode(EPD_MODE_NORMAL, EPD_MODE_INVERT);

#if (SCREEN_WIDTH == 152)  // 1.54"
    epdPrintBegin(2, 2, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("OpenEPaperLink");
    epdPrintEnd();
#endif

#if (SCREEN_WIDTH == 168)  //  High-res 2.9"
    epdPrintBegin(0, 295, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("OpenEPaperLink");
    epdPrintEnd();
#endif
#if (SCREEN_WIDTH == 128)  // 2.9"
    epdPrintBegin(0, 295, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("OpenEPaperLink");
    epdPrintEnd();
#endif
#if (SCREEN_WIDTH == 400)  // 4.2"
    epdPrintBegin(3, 3, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("OpenEPaperLink");
    epdPrintEnd();
#endif
    drawWithSleep();
}


void showSplashScreen() {
    selectLUT(EPD_LUT_NO_REPEATS);
    clearScreen();
    setColorMode(EPD_MODE_NORMAL, EPD_MODE_INVERT);

#if (SCREEN_WIDTH == 152)  // 1.54"
    epdPrintBegin(5, 55, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("Starting");
    epdPrintEnd();

    loadRawBitmap((uint8_t *)oepli, 12, 12, EPD_COLOR_BLACK);
    loadRawBitmap((uint8_t *)cloud, 12, 0, EPD_COLOR_RED);

    epdPrintBegin(5, 136, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_RED);
    epdpr("%02X%02X", mSelfMac[7], mSelfMac[6]);
    epdpr("%02X%02X", mSelfMac[5], mSelfMac[4]);
    epdpr("%02X%02X", mSelfMac[3], mSelfMac[2]);
    epdpr("%02X%02X", mSelfMac[1], mSelfMac[0]);
    epdPrintEnd();

    epdPrintBegin(2, 104, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    addCapabilities();
    epdPrintEnd();

    epdPrintBegin(2, 120, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("zbs154v033 %d.%d.%d%s", fwVersion / 100, (fwVersion % 100) / 10, (fwVersion % 10), fwVersionSuffix);
    epdPrintEnd();

#endif

#if (SCREEN_WIDTH == 168)  //  High-res 2.9"

    epdPrintBegin(0, 383, EPD_DIRECTION_Y, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("Starting");
    epdPrintEnd();

    epdPrintBegin(64, 295, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    addCapabilities();
    epdPrintEnd();

    epdPrintBegin(80, 295, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("zbs29v033 %d.%d.%d%s", fwVersion / 100, (fwVersion % 100) / 10, (fwVersion % 10), fwVersionSuffix);
    epdPrintEnd();

    epdPrintBegin(105, 270, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_RED);
    epdpr("MAC: %02X:%02X", mSelfMac[7], mSelfMac[6]);
    epdpr(":%02X:%02X", mSelfMac[5], mSelfMac[4]);
    epdpr(":%02X:%02X", mSelfMac[3], mSelfMac[2]);
    epdpr(":%02X:%02X", mSelfMac[1], mSelfMac[0]);
    epdPrintEnd();

    uint8_t  buffer[17];
    sprintf((char *)buffer, "%02X%02X", mSelfMac[7], mSelfMac[6]);
    sprintf((char *)buffer + 4, "%02X%02X", mSelfMac[5], mSelfMac[4]);
    sprintf((char *)buffer + 8, "%02X%02X", mSelfMac[3], mSelfMac[2]);
    sprintf((char *)buffer + 12, "%02X%02X", mSelfMac[1], mSelfMac[0]);
    printBarcode(buffer, 120, 284);

    loadRawBitmap((uint8_t *)oepli, 0, 12, EPD_COLOR_BLACK);
    loadRawBitmap((uint8_t *)cloud, 0, 0, EPD_COLOR_RED);
    // lutTest();
    //  drawLineVertical(EPD_COLOR_RED, 64, 10, 286);
    //  drawLineVertical(EPD_COLOR_BLACK, 65, 10, 286);

    // timerDelay(TIMER_TICKS_PER_SECOND * 4);
#endif

#if (SCREEN_WIDTH == 128)  // 2.9"

    epdPrintBegin(0, 295, EPD_DIRECTION_Y, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("Starting");
    epdPrintEnd();

    epdPrintBegin(64, 295, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    addCapabilities();
    epdPrintEnd();

    epdPrintBegin(80, 295, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("zbs29v033 %d.%d.%d%s", fwVersion / 100, (fwVersion % 100) / 10, (fwVersion % 10), fwVersionSuffix);
    epdPrintEnd();

    epdPrintBegin(105, 270, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_RED);
    epdpr("MAC: %02X:%02X", mSelfMac[7], mSelfMac[6]);
    epdpr(":%02X:%02X", mSelfMac[5], mSelfMac[4]);
    epdpr(":%02X:%02X", mSelfMac[3], mSelfMac[2]);
    epdpr(":%02X:%02X", mSelfMac[1], mSelfMac[0]);
    epdPrintEnd();

    uint8_t  buffer[17];
    sprintf((char *)buffer, "%02X%02X", mSelfMac[7], mSelfMac[6]);
    sprintf((char *)buffer + 4, "%02X%02X", mSelfMac[5], mSelfMac[4]);
    sprintf((char *)buffer + 8, "%02X%02X", mSelfMac[3], mSelfMac[2]);
    sprintf((char *)buffer + 12, "%02X%02X", mSelfMac[1], mSelfMac[0]);
    printBarcode(buffer, 120, 284);

    loadRawBitmap((uint8_t *)oepli, 0, 12, EPD_COLOR_BLACK);
    loadRawBitmap((uint8_t *)cloud, 0, 0, EPD_COLOR_RED);
    // lutTest();
    //  drawLineVertical(EPD_COLOR_RED, 64, 10, 286);
    //  drawLineVertical(EPD_COLOR_BLACK, 65, 10, 286);

    // timerDelay(TIMER_TICKS_PER_SECOND * 4);
#endif
#if (SCREEN_WIDTH == 400)  // 4.2"
    epdPrintBegin(3, 3, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("Starting");
    epdPrintEnd();

    epdPrintBegin(2, 252, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    addCapabilities();
    epdPrintEnd();

    epdPrintBegin(3, 268, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("zbs42v033 %d.%d.%d%s", fwVersion / 100, (fwVersion % 100) / 10, (fwVersion % 10), fwVersionSuffix);
    epdPrintEnd();
    epdPrintBegin(3, 284, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_RED);
    epdpr("MAC: %02X:%02X", mSelfMac[7], mSelfMac[6]);
    epdpr(":%02X:%02X", mSelfMac[5], mSelfMac[4]);
    epdpr(":%02X:%02X", mSelfMac[3], mSelfMac[2]);
    epdpr(":%02X:%02X", mSelfMac[1], mSelfMac[0]);
    epdPrintEnd();

    loadRawBitmap((uint8_t *)oepli, 136, 22, EPD_COLOR_BLACK);
    loadRawBitmap((uint8_t *)cloud, 136, 10, EPD_COLOR_RED);

    uint8_t  buffer[17];
    sprintf((char *)buffer, "%02X%02X", mSelfMac[7], mSelfMac[6]);
    sprintf((char *)buffer + 4, "%02X%02X", mSelfMac[5], mSelfMac[4]);
    sprintf((char *)buffer + 8, "%02X%02X", mSelfMac[3], mSelfMac[2]);
    sprintf((char *)buffer + 12, "%02X%02X", mSelfMac[1], mSelfMac[0]);
    printBarcode(buffer, 392, 264);
    printBarcode(buffer, 384, 264);

#endif
    drawWithSleep();
}

void showApplyUpdate() {
    setColorMode(EPD_MODE_NORMAL, EPD_MODE_INVERT);
    selectLUT(1);
    clearScreen();
    setColorMode(EPD_MODE_IGNORE, EPD_MODE_NORMAL);

#if (SCREEN_WIDTH == 152)
    epdPrintBegin(12, 60, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
#endif
#if (SCREEN_WIDTH == 168)
    epdPrintBegin(48, 220, EPD_DIRECTION_Y, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
#endif
#if (SCREEN_WIDTH == 128)
    epdPrintBegin(48, 220, EPD_DIRECTION_Y, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
#endif

#if (SCREEN_WIDTH == 400)
    epdPrintBegin(136, 134, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
#endif

    epdpr("Updating!");
    epdPrintEnd();
    drawNoWait();
}

uint8_t  resultcounter = 0;

void showScanningWindow() {
    setColorMode(EPD_MODE_NORMAL, EPD_MODE_INVERT);
    selectLUT(EPD_LUT_FAST_NO_REDS);
    clearScreen();
#if (SCREEN_WIDTH == 168)  //  High-res 2.9"
    epdPrintBegin(2, 374, EPD_DIRECTION_Y, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("Scanning for APs");
    epdPrintEnd();
    // epdPrintBegin(40, 262, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_RED);
    // epdpr("Channel - Quality");
    // epdPrintEnd();
    loadRawBitmap((uint8_t *)receive, 36, 24, EPD_COLOR_BLACK);
#endif
#if (SCREEN_WIDTH == 128)  // 2.9"
    epdPrintBegin(2, 275, EPD_DIRECTION_Y, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("Scanning for APs");
    epdPrintEnd();
    // epdPrintBegin(40, 262, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_RED);
    // epdpr("Channel - Quality");
    // epdPrintEnd();
    loadRawBitmap((uint8_t *)receive, 36, 24, EPD_COLOR_BLACK);
#endif
#if (SCREEN_WIDTH == 152)  // 1.54"
    loadRawBitmap((uint8_t *)receive, 96, 28, EPD_COLOR_BLACK);
    epdPrintBegin(3, 0, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("Scanning...");
    epdPrintEnd();
#endif
#if (SCREEN_WIDTH == 400)  // 4.2"
    epdPrintBegin(2, 2, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("Scanning for APs");
    epdPrintEnd();

    // epdPrintBegin(2, 40, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_RED);
    // epdpr("Channel - Quality");
    // epdPrintEnd();
    loadRawBitmap((uint8_t *)receive, 320, 125, EPD_COLOR_BLACK);
#endif

    draw();
    selectLUT(EPD_LUT_FAST);
    resultcounter = 0;
}

void addScanResult(uint8_t channel, uint8_t lqi) {
    if (channel == 11) resultcounter = 0;
#if (SCREEN_WIDTH == 168)  //  High-res 2.9"
    epdPrintBegin(56 + ((resultcounter % 4) * 16), 282 - (47 * (resultcounter / 4)), EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
#endif
#if (SCREEN_WIDTH == 128)  // 2.9"
    epdPrintBegin(56 + ((resultcounter % 4) * 16), 282 - (47 * (resultcounter / 4)), EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
#endif
#if (SCREEN_WIDTH == 152)  // 1.54"
    epdPrintBegin(4 + (47 * (resultcounter / 8)), 31 + (15 * (resultcounter % 8)), EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
#endif
#if (SCREEN_WIDTH == 400)  // 4.2"
    epdPrintBegin(4 + (47 * (resultcounter / 8)), 58 + (15 * (resultcounter % 8)), EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
#endif
    epdpr("%d-%d", channel, lqi);
    epdPrintEnd();
    resultcounter++;
}

void showAPFound() {
    clearScreen();
    setColorMode(EPD_MODE_NORMAL, EPD_MODE_INVERT);
    selectLUT(1);
#if (SCREEN_WIDTH == 168)
    epdPrintBegin(0, 374, EPD_DIRECTION_Y, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("Waiting for data...");
    epdPrintEnd();
    epdPrintBegin(48, 278, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("Found the following AP:");
    epdPrintEnd();
    epdPrintBegin(64, 293, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("AP MAC: %02X:%02X", APmac[7], APmac[6]);
    epdpr(":%02X:%02X", APmac[5], APmac[4]);
    epdpr(":%02X:%02X", APmac[3], APmac[2]);
    epdpr(":%02X:%02X", APmac[1], APmac[0]);
    epdPrintEnd();
    epdPrintBegin(80, 293, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("Ch: %d RSSI: %d LQI: %d", currentChannel, mLastRSSI, mLastLqi);
    epdPrintEnd();

    epdPrintBegin(103, 258, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("Tag MAC: %02X:%02X", mSelfMac[7], mSelfMac[6]);
    epdpr(":%02X:%02X", mSelfMac[5], mSelfMac[4]);
    epdpr(":%02X:%02X", mSelfMac[3], mSelfMac[2]);
    epdpr(":%02X:%02X", mSelfMac[1], mSelfMac[0]);
    epdPrintEnd();

    uint8_t  buffer[17];
    sprintf((char *)buffer, "%02X%02X", mSelfMac[7], mSelfMac[6]);
    sprintf((char *)buffer + 4, "%02X%02X", mSelfMac[5], mSelfMac[4]);
    sprintf((char *)buffer + 8, "%02X%02X", mSelfMac[3], mSelfMac[2]);
    sprintf((char *)buffer + 12, "%02X%02X", mSelfMac[1], mSelfMac[0]);
    printBarcode(buffer, 120, 253);
    loadRawBitmap((uint8_t *)receive, 36, 14, EPD_COLOR_BLACK);
#endif
#if (SCREEN_WIDTH == 128)
    epdPrintBegin(0, 285, EPD_DIRECTION_Y, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("Waiting for data...");
    epdPrintEnd();
    epdPrintBegin(48, 278, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("Found the following AP:");
    epdPrintEnd();
    epdPrintBegin(64, 293, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("AP MAC: %02X:%02X", APmac[7], APmac[6]);
    epdpr(":%02X:%02X", APmac[5], APmac[4]);
    epdpr(":%02X:%02X", APmac[3], APmac[2]);
    epdpr(":%02X:%02X", APmac[1], APmac[0]);
    epdPrintEnd();
    epdPrintBegin(80, 293, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("Ch: %d RSSI: %d LQI: %d", currentChannel, mLastRSSI, mLastLqi);
    epdPrintEnd();

    epdPrintBegin(103, 258, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("Tag MAC: %02X:%02X", mSelfMac[7], mSelfMac[6]);
    epdpr(":%02X:%02X", mSelfMac[5], mSelfMac[4]);
    epdpr(":%02X:%02X", mSelfMac[3], mSelfMac[2]);
    epdpr(":%02X:%02X", mSelfMac[1], mSelfMac[0]);
    epdPrintEnd();

    uint8_t  buffer[17];
    sprintf((char *)buffer, "%02X%02X", mSelfMac[7], mSelfMac[6]);
    sprintf((char *)buffer + 4, "%02X%02X", mSelfMac[5], mSelfMac[4]);
    sprintf((char *)buffer + 8, "%02X%02X", mSelfMac[3], mSelfMac[2]);
    sprintf((char *)buffer + 12, "%02X%02X", mSelfMac[1], mSelfMac[0]);
    printBarcode(buffer, 120, 253);
    loadRawBitmap((uint8_t *)receive, 36, 14, EPD_COLOR_BLACK);
#endif
#if (SCREEN_WIDTH == 152)  // 1.54"
    epdPrintBegin(25, 0, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("Waiting");
    epdPrintEnd();
    epdPrintBegin(3, 32, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("for data...");
    epdPrintEnd();

    epdPrintBegin(5, 64, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("AP MAC:");
    epdPrintEnd();
    epdPrintBegin(5, 80, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("%02X%02X", APmac[7], APmac[6]);
    epdpr("%02X%02X", APmac[5], APmac[4]);
    epdpr("%02X%02X", APmac[3], APmac[2]);
    epdpr("%02X%02X", APmac[1], APmac[0]);
    epdPrintEnd();

    epdPrintBegin(5, 96, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("Ch:%d rssi:%d lqi:%d", currentChannel, mLastRSSI, mLastLqi);
    epdPrintEnd();

    epdPrintBegin(5, 120, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("Tag MAC:");
    epdPrintEnd();

    epdPrintBegin(5, 136, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_RED);
    epdpr("%02X%02X", mSelfMac[7], mSelfMac[6]);
    epdpr("%02X%02X", mSelfMac[5], mSelfMac[4]);
    epdpr("%02X%02X", mSelfMac[3], mSelfMac[2]);
    epdpr("%02X%02X", mSelfMac[1], mSelfMac[0]);
    epdPrintEnd();
#endif
#if (SCREEN_WIDTH == 400)
    epdPrintBegin(10, 10, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("Waiting for data...");
    epdPrintEnd();
    epdPrintBegin(48, 80, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("Found the following AP:");
    epdPrintEnd();
    epdPrintBegin(48, 96, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("AP MAC: %02X:%02X", APmac[7], APmac[6]);
    epdpr(":%02X:%02X", APmac[5], APmac[4]);
    epdpr(":%02X:%02X", APmac[3], APmac[2]);
    epdpr(":%02X:%02X", APmac[1], APmac[0]);
    epdPrintEnd();
    epdPrintBegin(48, 112, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("Ch: %d RSSI: %d LQI: %d", currentChannel, mLastRSSI, mLastLqi);
    epdPrintEnd();

    epdPrintBegin(366, 258, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("Tag MAC: %02X:%02X", mSelfMac[7], mSelfMac[6]);
    epdpr(":%02X:%02X", mSelfMac[5], mSelfMac[4]);
    epdpr(":%02X:%02X", mSelfMac[3], mSelfMac[2]);
    epdpr(":%02X:%02X", mSelfMac[1], mSelfMac[0]);
    epdPrintEnd();

    uint8_t  buffer[17];
    sprintf((char *)buffer, "%02X%02X", mSelfMac[7], mSelfMac[6]);
    sprintf((char *)buffer + 4, "%02X%02X", mSelfMac[5], mSelfMac[4]);
    sprintf((char *)buffer + 8, "%02X%02X", mSelfMac[3], mSelfMac[2]);
    sprintf((char *)buffer + 12, "%02X%02X", mSelfMac[1], mSelfMac[0]);
    printBarcode(buffer, 392, 253);
    printBarcode(buffer, 384, 253);
    loadRawBitmap((uint8_t *)receive, 100, 170, EPD_COLOR_BLACK);
#endif
    addOverlay();
    drawWithSleep();
}

void showNoAP() {
    selectLUT(EPD_LUT_NO_REPEATS);
    setColorMode(EPD_MODE_NORMAL, EPD_MODE_INVERT);
    clearScreen();
#if (SCREEN_WIDTH == 168)  //  High-res 2,9"
    epdPrintBegin(0, 374, EPD_DIRECTION_Y, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("No AP found :(");
    epdPrintEnd();
    epdPrintBegin(48, 285, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("We'll try again in a");
    epdPrintEnd();
    epdPrintBegin(64, 285, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("little while...");
    epdPrintEnd();
    loadRawBitmap((uint8_t *)receive, 36, 24, EPD_COLOR_BLACK);
    loadRawBitmap((uint8_t *)failed, 42, 26, EPD_COLOR_RED);
#endif
#if (SCREEN_WIDTH == 128)  // 2,9"
    epdPrintBegin(0, 285, EPD_DIRECTION_Y, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("No AP found :(");
    epdPrintEnd();
    epdPrintBegin(48, 285, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("We'll try again in a");
    epdPrintEnd();
    epdPrintBegin(64, 285, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("little while...");
    epdPrintEnd();
    loadRawBitmap((uint8_t *)receive, 36, 24, EPD_COLOR_BLACK);
    loadRawBitmap((uint8_t *)failed, 42, 26, EPD_COLOR_RED);
#endif
#if (SCREEN_WIDTH == 152)  // 1.54"
    epdPrintBegin(40, 0, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("No AP");
    epdPrintEnd();
    epdPrintBegin(22, 32, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("found :(");
    epdPrintEnd();

    epdPrintBegin(8, 76, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("We'll try again in");
    epdPrintEnd();
    epdPrintBegin(25, 92, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("a little while");
    epdPrintEnd();
#endif
#if (SCREEN_WIDTH == 400)  // 4.2"
    epdPrintBegin(10, 10, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("No AP found :(");
    epdPrintEnd();
    epdPrintBegin(10, 274, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("We'll try again in a little while");
    epdPrintEnd();
    loadRawBitmap((uint8_t *)receive, 76, 120, EPD_COLOR_BLACK);
    loadRawBitmap((uint8_t *)failed, 82, 122, EPD_COLOR_RED);
#endif
    addOverlay();
    drawWithSleep();
}

void showLongTermSleep() {
    selectLUT(EPD_LUT_NO_REPEATS);
    setColorMode(EPD_MODE_NORMAL, EPD_MODE_INVERT);
    clearScreen();
#if (SCREEN_WIDTH == 128)  // 2.9"
    epdPrintBegin(0, 295, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("zZ");
    epdPrintEnd();
#else
    epdPrintBegin(2, SCREEN_HEIGHT - 16, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("zZ");
    epdPrintEnd();
#endif
    addOverlay();
    drawWithSleep();
}
void showNoEEPROM() {
    selectLUT(EPD_LUT_NO_REPEATS);
    clearScreen();
    setColorMode(EPD_MODE_NORMAL, EPD_MODE_INVERT);
#if (SCREEN_WIDTH == 168)  //  High-res 2.9"
    epdPrintBegin(0, 374, EPD_DIRECTION_Y, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("EEPROM FAILED :(");
    epdPrintEnd();
    epdPrintBegin(64, 285, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("Sleeping forever :'(");
    epdPrintEnd();
    loadRawBitmap((uint8_t *)failed, 42, 26, EPD_COLOR_RED);
#endif
#if (SCREEN_WIDTH == 128)  // 2.9"
    epdPrintBegin(0, 285, EPD_DIRECTION_Y, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("EEPROM FAILED :(");
    epdPrintEnd();
    epdPrintBegin(64, 285, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("Sleeping forever :'(");
    epdPrintEnd();
    loadRawBitmap((uint8_t *)failed, 42, 26, EPD_COLOR_RED);
#endif
#if (SCREEN_WIDTH == 152)  // 1.54"
    epdPrintBegin(26, 0, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("EEPROM ");
    epdPrintEnd();
    epdPrintBegin(8, 32, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("FAILED :(");
    epdPrintEnd();
    loadRawBitmap((uint8_t *)failed, 60, 72, EPD_COLOR_RED);

    epdPrintBegin(3, 136, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("Sleeping forever :'(");
    epdPrintEnd();
#endif
#if (SCREEN_WIDTH == 400)  // 4.2"
    epdPrintBegin(50, 3, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("EEPROM FAILED :(");
    epdPrintEnd();
    loadRawBitmap((uint8_t *)failed, 176, 126, EPD_COLOR_RED);
    epdPrintBegin(100, 284, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("Sleeping forever :'(");
    epdPrintEnd();
#endif
    drawWithSleep();
}

void showNoMAC() {
    selectLUT(EPD_LUT_NO_REPEATS);
    clearScreen();
    setColorMode(EPD_MODE_NORMAL, EPD_MODE_INVERT);
#if (SCREEN_WIDTH == 168)  //  High-res 2.9"
    epdPrintBegin(0, 374, EPD_DIRECTION_Y, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("NO MAC SET :(");
    epdPrintEnd();
    epdPrintBegin(64, 285, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("Sleeping forever :'(");
    epdPrintEnd();
    loadRawBitmap((uint8_t *)failed, 42, 26, EPD_COLOR_RED);
#endif
#if (SCREEN_WIDTH == 128)  // 2.9"
    epdPrintBegin(0, 285, EPD_DIRECTION_Y, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("NO MAC SET :(");
    epdPrintEnd();
    epdPrintBegin(64, 285, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("Sleeping forever :'(");
    epdPrintEnd();
    loadRawBitmap((uint8_t *)failed, 42, 26, EPD_COLOR_RED);
#endif
#if (SCREEN_WIDTH == 152)  // 1.54"
    epdPrintBegin(20, 0, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("NO MAC");
    epdPrintEnd();
    epdPrintBegin(30, 32, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("SET :(");
    epdPrintEnd();
    loadRawBitmap((uint8_t *)failed, 60, 72, EPD_COLOR_RED);
    epdPrintBegin(3, 136, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("Sleeping forever :'(");
    epdPrintEnd();
#endif
#if (SCREEN_WIDTH == 400)  // 4.2"
    epdPrintBegin(100, 3, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("NO MAC SET :(");
    epdPrintEnd();
    loadRawBitmap((uint8_t *)failed, 176, 126, EPD_COLOR_RED);
    epdPrintBegin(100, 284, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("Sleeping forever :'(");
    epdPrintEnd();
#endif
    drawWithSleep();
}