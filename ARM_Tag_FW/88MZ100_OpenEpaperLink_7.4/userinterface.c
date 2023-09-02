#include "userinterface.h"

#include <stdbool.h>
#include <string.h>

#include "bitmaps.h"
#include "board.h"
#include "comms.h"
#include "epd.h"
#include "font.h"
#include "powermgt.h"
#include "printf.h"
#include "proto.h"
#include "screen.h"
#include "settings.h"
#include "syncedproto.h"  // for APmac / Channel
#include "timer.h"

const uint16_t fwVersion = FW_VERSION;
const char fwVersionSuffix[] = FW_VERSION_SUFFIX;

extern uint8_t capabilities;

bool __attribute__((section(".aonshadow"))) lowBatteryShown = false;
bool __attribute__((section(".aonshadow"))) noAPShown = false;

void addCapabilities() {
    // if (capabilities) epdpr("Options: ");
    if (capabilities & CAPABILITY_HAS_NFC) {
        // epdpr("-NFC");
        if (capabilities & CAPABILITY_NFC_WAKE) {
            // epdpr("+WAKE");
        } else {
            //   epdpr(" ");
        }
    }
    if (capabilities & CAPABILITY_HAS_WAKE_BUTTON) {
        // epdpr("-WAKE BUTTON");
    }
}

void addOverlay() {
    if ((currentChannel == 0) && (tagSettings.enableNoRFSymbol)) {
        drawImg(0, 3, norf);
        noAPShown = true;
    } else {
        noAPShown = false;
    }
    if ((batteryVoltage < tagSettings.batLowVoltage) && (tagSettings.enableLowBatSymbol)) {
        drawImg(0, 366, batlow);
        lowBatteryShown = true;
    } else {
        lowBatteryShown = false;
    }
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
    init_epd();
    fillWindow(0, 0, 640, 384, 1);
    epdPrintf(10, 10, 1, "OpenEPaperLink starting!");
    addOverlay();
    epd_refresh_and_sleep(1);
}

void showApplyUpdate() {
    init_epd();

    fillWindow(0, 0, 640, 384, 1);

   
    epdPrintf(90, 170, 1, "Performing update... This shouldn't take too long!");


    addOverlay();
    epd_refresh_and_sleep(1);
}

void showAPFound() {
    init_epd();

    fillWindow(0, 0, 640, 384, 1);

    epdPrintf(10, 10, 1, "OpenEPaperLink");
    epdPrintf(10, 40, 1, "AP Found at channel %d", currentChannel);
    epdPrintf(10, 60, 1, "AP MAC: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", APmac[7], APmac[6], APmac[5], APmac[4], APmac[3], APmac[2], APmac[1], APmac[0]);

    epdPrintf(10, 330, 1, "Battery: %d.%dV", batteryVoltage / 1000, batteryVoltage % 1000);
    epdPrintf(10, 350, 1, "Tag MAC: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
    addOverlay();
    epd_refresh_and_sleep(1);
}

void showNoAP() {
    init_epd();
    fillWindow(0, 0, 640, 384, 1);
    epdPrintf(10, 10, 1, "OpenEPaperLink ");
    epdPrintf(10, 40, 1, "No AP found... We'll try again in a little while though!");
    epdPrintf(10, 350, 1, "Tag MAC: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
    addOverlay();
    epd_refresh_and_sleep(1);
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