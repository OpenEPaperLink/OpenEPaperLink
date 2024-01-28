#include "userinterface.h"

#include <stdbool.h>
#include <string.h>

#include "bitmaps.h"
#include "board.h"
#include "comms.h"
#include "epd_interface.h"
#include "powermgt.h"
#include "mz100/printf.h"
#include "proto.h"
#include "screen.h"
#include "settings.h"
#include "syncedproto.h"  // for APmac / Channel
#include "mz100/timer.h"

#include "drawing.h"

const uint16_t fwVersion = FW_VERSION;
const char fwVersionSuffix[] = FW_VERSION_SUFFIX;

extern uint8_t capabilities;

bool __attribute__((section(".aonshadow"))) lowBatteryShown = false;
bool __attribute__((section(".aonshadow"))) noAPShown = false;

void addOverlay() {
    if ((currentChannel == 0) && (tagSettings.enableNoRFSymbol)) {
        // drawImg(0, 3, norf);
        noAPShown = true;
    } else {
        noAPShown = false;
    }
    if ((batteryVoltage < tagSettings.batLowVoltage) && (tagSettings.enableLowBatSymbol)) {
        //  drawImg(0, 366, batlow);
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
    // addCompressedFSImage(300, 80,rotation::ROTATE_0, (char *)"img_tbird.bin.z");

    // addCompressedFSImage(228, 24, rotation::ROTATE_0, (char *)"img/sadpanda.z");
    // addCompressedFSImage(0, 0, rotation::ROTATE_0, (char *)"forecast75.raw.z");

    fontrender fr((char *)"font/FreeSansBold24pt7b");
    fr.epdPrintf(5, 5, 0, rotation::ROTATE_0, (char *)"OpenEPaperLink");
    fr.setFont((char *)"font/FreeSansBold18pt7b");
    fr.epdPrintf(20, 70, 1, rotation::ROTATE_0, "M2 7.5\"");
    fr.epdPrintf(10, 350, 0, rotation::ROTATE_0, "Tag MAC: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
    selectLUT(1);
    draw();
    epdEnterSleep();
}

void showApplyUpdate() {
    epdSetup();
    fontrender fr((char *)"font/FreeSansBold24pt7b");
    fr.epdPrintf(5, 5, 0, rotation::ROTATE_0, (char *)"Updating...");
    fr.setFont((char *)"font/FreeSansBold18pt7b");
    fr.epdPrintf(40, 160, 1, rotation::ROTATE_0, "This shouldn't take too long!");
    // epdPrintf(90, 170, 1, "Performing update... This shouldn't take too long!");

    addOverlay();
    drawNoWait();
}

void showAPFound() {
    fs->init();
    epdSetup();
    fontrender fr((char *)"font/FreeSansBold24pt7b");
    fr.epdPrintf(5, 5, 0, rotation::ROTATE_0, (char *)"OpenEPaperLink");
    fr.setFont((char *)"font/FreeSansBold18pt7b");
    fr.epdPrintf(20, 70, 0, rotation::ROTATE_0, "AP Found at channel %d", currentChannel);
    fr.epdPrintf(20, 110, 0, rotation::ROTATE_0, "AP MAC: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", APmac[7], APmac[6], APmac[5], APmac[4], APmac[3], APmac[2], APmac[1], APmac[0]);
    fr.epdPrintf(10, 315, 0, rotation::ROTATE_0, "Battery: %d.%dV", batteryVoltage / 1000, batteryVoltage % 1000);
    fr.epdPrintf(10, 350, 0, rotation::ROTATE_0, "Tag MAC: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
    selectLUT(1);
    draw();
    epdEnterSleep();
}

void showNoAP() {
    fs->init();

    epdSetup();
    addCompressedFSImage(228, 24, rotation::ROTATE_0, (char *)"img/sadpanda.z");
    // addCompressedFSImage(0, 0, rotation::ROTATE_0, (char *)"forecast75.raw.z");

    fontrender fr((char *)"font/FreeSansBold24pt7b");
    fr.epdPrintf(5, 5, 0, rotation::ROTATE_0, (char *)"No AP Found!");
    fr.setFont((char *)"font/FreeSansBold18pt7b");
    fr.epdPrintf(20, 70, 1, rotation::ROTATE_0, "We'll try again later\"");
    // epdPrintf(10, 10, 1, "OpenEPaperLink ");
    // epdPrintf(10, 40, 1, "No AP found... We'll try again in a little while though!");
    // epdPrintf(10, 350, 1, "Tag MAC: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
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
