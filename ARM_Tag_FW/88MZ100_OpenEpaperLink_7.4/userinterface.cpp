#include "userinterface.h"

#include <stdbool.h>
#include <string.h>

#include "board.h"
#include "comms.h"
#include "epd_interface.h"
#include "powermgt.h"
#include "mz100/printf.h"
#include "proto.h"
#include "screen.h"
#include "settings.h"
#include "oepl-protocol.h"  // for APmac / Channel
#include "mz100/timer.h"

#include "drawing.h"

const uint16_t fwVersion = FW_VERSION;
const char fwVersionSuffix[] = FW_VERSION_SUFFIX;

extern uint8_t capabilities;

bool __attribute__((section(".aonshadow"))) lowBatteryShown;
bool __attribute__((section(".aonshadow"))) noAPShown;

void addOverlay() {
    if ((currentChannel == 0) && (tagSettings.enableNoRFSymbol)) {
        fs->init();
        drawMask((epd->Xres - 48) - 5, 8, 48, 42, COLOR_BLACK);
        drawMask((epd->Xres - 48) - 5, 8, 48, 42, COLOR_RED);
        addCompressedFSImage((epd->Xres - 48) - 5, 8, rotation::ROTATE_0, (char *)"norf.bin.z");
        noAPShown = true;
    } else {
        noAPShown = false;
    }

    if ((lowBattery) && (tagSettings.enableLowBatSymbol)) {
        fs->init();
        drawMask((epd->Xres - 48) - 5, (epd->Yres - 26) - 7, 48, 26, COLOR_BLACK);
        drawMask((epd->Xres - 48) - 5, (epd->Yres - 26) - 7, 48, 26, COLOR_RED);
        addCompressedFSImage((epd->Xres - 48) - 5, (epd->Yres - 26) - 7, rotation::ROTATE_0, (char *)"lowbat.bin.z");
        lowBatteryShown = true;
    } else {
        lowBatteryShown = false;
    }

#ifdef DEBUG_BUILD
    fs->init();
    fontrender fr((char *)"font/FreeSansBold18pt7b");
    drawMask(15, epd->Yres - 45, 129, 33, COLOR_BLACK);
    drawMask(15, epd->Yres - 45, 129, 33, COLOR_RED);
    drawRoundedRectangle(15, epd->Yres - 45, 129, 33, COLOR_RED);
    fr.epdPrintf(17, epd->Yres - 42, COLOR_RED, rotation::ROTATE_0, "DEBUG");
#endif
}

void afterFlashScreenSaver() {
    // selectLUT(EPD_LUT_DEFAULT);
    // clearScreen();

#if (SCREEN_WIDTH == 400)  // 4.2"
    epdPrintBegin(3, 3, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("OpenEPaperLink");
    epdPrintEnd();
#endif
    // drawWithSleep();
}

void showSplashScreen() {
    fs->init();
    epdSetup();
    addCompressedFSImage((640 - 365) - 20, (384 - 237) / 2, rotation::ROTATE_0, (char *)"tbird2.bin.z");
    fontrender fr((char *)"font/FreeSansBold24pt7b");
    fr.epdPrintf(5, 5, 0, rotation::ROTATE_0, (char *)"OpenEPaperLink");
    fr.setFont((char *)"font/FreeSansBold18pt7b");

    if (tagProfile.bpp == 2) {
        fr.epdPrintf(15, 70, 1, rotation::ROTATE_0, "M2 7.5\" BWR");
    } else {
        fr.epdPrintf(20, 70, 1, rotation::ROTATE_0, "M2 7.5\" BW");
    }
    fr.epdPrintf(10, 350, 0, rotation::ROTATE_0, "Tag MAC: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
    addQR(80, 160, 3, 3, "https://openepaperlink.eu/tag/0/%02X/%02X%02X%02X%02X%02X%02X%02X%02X/", tag.OEPLtype, mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
    fr.setFont((char *)"font/FreeSans9pt7b");
    fr.epdPrintf(622, 290, 0, rotation::ROTATE_270, "v%04X%s", FW_VERSION, FW_VERSION_SUFFIX);
    selectLUT(1);
    draw();
    epdEnterSleep();
}

#define FW_VERSION 0x0027          // version number (max 2.5.5 :) )
#define FW_VERSION_SUFFIX "-zlib"  // suffix, like -RC1 or whatever.

void showApplyUpdate() {
    epdSetup();
    fs->init();

    fontrender fr((char *)"font/FreeSansBold24pt7b");
    fr.epdPrintf(5, 5, 0, rotation::ROTATE_0, (char *)"Updating......");
    fr.setFont((char *)"font/FreeSansBold18pt7b");
    fr.epdPrintf(40, 140, 1, rotation::ROTATE_0, "This shouldn't take too long!");
    // epdPrintf(90, 170, 1, "Performing update... This shouldn't take too long!");
    addCompressedFSImage((640 - 551) / 2, 384 - 198, rotation::ROTATE_0, (char *)"jet.bin.z");
    addOverlay();
    drawNoWait();
}

void showAPFound() {
    fs->init();
    epdSetup();

    // addCompressedFSImage(228, 24, rotation::ROTATE_0, (char *)"img/sadpanda.z");

    fontrender fr((char *)"font/FreeSansBold24pt7b");
    fr.epdPrintf(5, 5, 0, rotation::ROTATE_0, (char *)"OpenEPaperLink");
    fr.setFont((char *)"font/FreeSansBold18pt7b");
    fr.epdPrintf(20, 70, 0, rotation::ROTATE_0, "AP Found at channel %d", currentChannel);
    fr.epdPrintf(20, 110, 0, rotation::ROTATE_0, "AP MAC: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", APmac[7], APmac[6], APmac[5], APmac[4], APmac[3], APmac[2], APmac[1], APmac[0]);
    fr.epdPrintf(10, 315, 0, rotation::ROTATE_0, "Battery: %d.%dV", batteryVoltage / 1000, batteryVoltage % 1000);
    fr.epdPrintf(10, 350, 0, rotation::ROTATE_0, "Tag MAC: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
    fr.setFont((char *)"font/FreeSans9pt7b");
    fr.epdPrintf(622, 290, 0, rotation::ROTATE_270, "v%04X%s", FW_VERSION, FW_VERSION_SUFFIX);
    addOverlay();

    if (lowBatteryShown) {
        selectLUT(0);
    } else {
        selectLUT(1);
    }
    draw();
    epdEnterSleep();
}

void showNoAP() {
    fs->init();
    epdSetup();
    selectLUT(0);
    addCompressedFSImage(640 - 415, 384 - 198, rotation::ROTATE_0, (char *)"sadpanda.bin.z");
    fontrender fr((char *)"font/FreeSansBold24pt7b");
    fr.epdPrintf(5, 5, 0, rotation::ROTATE_0, (char *)"No AP Found!");
    fr.setFont((char *)"font/FreeSansBold18pt7b");
    fr.epdPrintf(20, 70, 0, rotation::ROTATE_0, "We'll try again later");
    // epdPrintf(10, 10, 1, "OpenEPaperLink ");
    // epdPrintf(10, 40, 1, "No AP found... We'll try again in a little while though!");

    drawMask(10, 360, 300, 16, COLOR_BLACK);
    drawMask(10, 360, 300, 16, COLOR_RED);
    fr.setFont((char *)"font/FreeSans9pt7b");
    fr.epdPrintf(10, 360, 0, rotation::ROTATE_0, "Tag MAC: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
    addOverlay();
    draw();
    epdEnterSleep();
}

void showLongTermSleep() {
    // selectLUT(EPD_LUT_NO_REPEATS);
    // clearScreen();

    // epdPrintBegin(2, SCREEN_HEIGHT - 16, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    // epdpr("zZ");
    // epdPrintEnd();

    addOverlay();
    // drawWithSleep();
}
void showNoEEPROM() {
    // selectLUT(EPD_LUT_NO_REPEATS);
    // clearScreen();
#if (SCREEN_WIDTH == 400)  // 4.2"
    epdPrintBegin(50, 3, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("EEPROM FAILED :(");
    epdPrintEnd();
    loadRawBitmap(failed, 176, 126, EPD_COLOR_RED);
    epdPrintBegin(100, 284, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("Sleeping forever :'(");
    epdPrintEnd();
#endif
    // drawWithSleep();
}

void showNoMAC() {
    // selectLUT(EPD_LUT_NO_REPEATS);
    // clearScreen();
#if (SCREEN_WIDTH == 400)  // 4.2"
    epdPrintBegin(100, 3, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    epdpr("NO MAC SET :(");
    epdPrintEnd();
    loadRawBitmap(failed, 176, 126, EPD_COLOR_RED);
    epdPrintBegin(100, 284, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
    epdpr("Sleeping forever :'(");
    epdPrintEnd();
#endif
    // drawWithSleep();
}
