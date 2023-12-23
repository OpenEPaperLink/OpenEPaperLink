

#include "userinterface.h"

#include <stdbool.h>
#include <string.h>

#include "bitmaps.h"
#include "comms.h"

#include "lut.h"
#include "powermgt.h"

#include "settings.h"
#include "syncedproto.h"  // for APmac / Channel
#include "hal.h"
#include "drawing.h"
#include "font.h"

#ifdef EPD_DRAW_DIRECTION_RIGHT
#define UI_SCREEN_WIDTH SCREEN_HEIGHT
#define UI_SCREEN_HEIGHT SCREEN_WIDTH
#else
#define UI_SCREEN_WIDTH SCREEN_WIDTH
#define UI_SCREEN_HEIGHT SCREEN_HEIGHT
#endif

const uint8_t fwVersion = FW_VERSION;
const char fwVersionSuffix[] = FW_VERSION_SUFFIX;

bool lowBatteryShown = false;
bool noAPShown = false;

void addOverlay() {
    if (currentChannel == 0) {
        drawMask(UI_SCREEN_WIDTH - 27, 5, 22, 22, COLOR_BLACK);
        drawMask(UI_SCREEN_WIDTH - 27, 5, 22, 22, COLOR_RED);
        drawRoundedRectangle(UI_SCREEN_WIDTH - 28, 4, 24, 24, COLOR_RED);
        addBufferedImage(UI_SCREEN_WIDTH - 24, 8, COLOR_BLACK, rotation::ROTATE_0, ant, DRAW_NORMAL);
        addBufferedImage(UI_SCREEN_WIDTH - 16, 15, COLOR_RED, rotation::ROTATE_0, cross, DRAW_NORMAL);
        noAPShown = true;
    } else {
        noAPShown = false;
    }

    if (lowBattery) {
        drawMask(UI_SCREEN_WIDTH - 27, UI_SCREEN_HEIGHT - 26, 22, 22, COLOR_BLACK);
        drawMask(UI_SCREEN_WIDTH - 27, UI_SCREEN_HEIGHT - 26, 22, 22, COLOR_RED);
        drawRoundedRectangle(UI_SCREEN_WIDTH - 28, UI_SCREEN_HEIGHT - 26, 24, 24, COLOR_RED);
        addBufferedImage(UI_SCREEN_WIDTH - 24, UI_SCREEN_HEIGHT - 19, COLOR_BLACK, rotation::ROTATE_0, battery, DRAW_NORMAL);

        lowBatteryShown = true;
    } else {
        lowBatteryShown = false;
    }
}

void afterFlashScreenSaver() {
    selectLUT(EPD_LUT_DEFAULT);
    draw();
}

void showSplashScreen() {
    selectLUT(EPD_LUT_FAST_NO_REDS);
#if (HW_TYPE == SOLUM_M3_BWR_22)
    fontrender fr(&FreeSansBold18pt7b);
    fr.epdPrintf(2, 2, COLOR_BLACK, rotation::ROTATE_0, "OpenEPaperLink");
    fr.setFont(&FreeSans9pt7b);
    fr.epdPrintf(10, 38, COLOR_RED, rotation::ROTATE_0, "Newton M3 2.2\"");
    // fr.setFont(&FreeSans9pt7b);
    fr.epdPrintf(5, UI_SCREEN_HEIGHT - 40, 0, rotation::ROTATE_0, "FW: %04X-%s", fwVersion, fwVersionSuffix);
    fr.epdPrintf(5, UI_SCREEN_HEIGHT - 20, 0, rotation::ROTATE_0, "MAC: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
    addQR(UI_SCREEN_WIDTH - 120, 42, 3, 3, "https://openepaperlink.eu/tag/0/%02X/%02X%02X%02X%02X%02X%02X%02X%02X/", HW_TYPE, mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
#endif
#if (HW_TYPE == SOLUM_M3_BWR_29)
    fontrender fr(&FreeSansBold18pt7b);
    fr.epdPrintf(2, 2, COLOR_BLACK, rotation::ROTATE_0, "OpenEPaperLink");
    fr.setFont(&FreeSans9pt7b);
    fr.epdPrintf(10, 38, 1, rotation::ROTATE_0, "Newton M3 2.9\"");
    // fr.setFont(&FreeSans9pt7b);
    fr.epdPrintf(UI_SCREEN_WIDTH - 17, 0, 0, rotation::ROTATE_270, "FW: %04X-%s", fwVersion, fwVersionSuffix);
    fr.epdPrintf(5, UI_SCREEN_HEIGHT - 20, 0, rotation::ROTATE_0, "MAC: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
    addQR(UI_SCREEN_WIDTH - 120, 42, 3, 3, "https://openepaperlink.eu/tag/0/%02X/%02X%02X%02X%02X%02X%02X%02X%02X/", HW_TYPE, mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
#endif
#if (HW_TYPE == SOLUM_M3_BWR_43)
    fontrender fr(&FreeSansBold24pt7b);
    fr.epdPrintf(7, 7, COLOR_BLACK, rotation::ROTATE_0, "OpenEPaperLink");
    fr.setFont(&FreeSansBold18pt7b);
    fr.epdPrintf(15, 60, COLOR_RED, rotation::ROTATE_0, "Newton M3 4.3\"");
    fr.setFont(&FreeSans9pt7b);
    fr.epdPrintf(UI_SCREEN_WIDTH - 17, 0, 0, rotation::ROTATE_270, "FW: %04X-%s", fwVersion, fwVersionSuffix);
    fr.epdPrintf(10, UI_SCREEN_HEIGHT - 25, 0, rotation::ROTATE_0, "MAC: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
    addQR(UI_SCREEN_WIDTH - 120, 32, 3, 3, "https://openepaperlink.eu/tag/0/%02X/%02X%02X%02X%02X%02X%02X%02X%02X/", HW_TYPE, mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
#endif
#if (HW_TYPE == SOLUM_M3_BWR_60)
    fontrender fr(&FreeSansBold24pt7b);
    fr.epdPrintf(10, 10, COLOR_BLACK, rotation::ROTATE_0, "OpenEPaperLink");
    fr.setFont(&FreeSansBold18pt7b);
    fr.epdPrintf(15, 60, 1, rotation::ROTATE_0, "Newton M3 6.0\"");
    fr.setFont(&FreeSans9pt7b);
    fr.epdPrintf(UI_SCREEN_WIDTH - 17, 310, 0, rotation::ROTATE_270, "FW: %04X-%s", fwVersion, fwVersionSuffix);
    fr.epdPrintf(10, UI_SCREEN_HEIGHT - 25, 0, rotation::ROTATE_0, "MAC: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
    addFlashImage(293, 61, COLOR_BLACK, rotation::ROTATE_0, newton);
    addQR(40, 120, 3, 7, "https://openepaperlink.eu/tag/0/%02X/%02X%02X%02X%02X%02X%02X%02X%02X/", HW_TYPE, mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
#endif
#if (HW_TYPE == SOLUM_M3_BWR_75)
    fontrender fr(&FreeSansBold24pt7b);
    fr.epdPrintf(10, 10, COLOR_BLACK, rotation::ROTATE_0, "OpenEPaperLink");
    fr.setFont(&FreeSansBold18pt7b);
    fr.epdPrintf(15, 60, COLOR_RED, rotation::ROTATE_0, "Newton M3 7.5\"");
    fr.setFont(&FreeSans9pt7b);
    fr.epdPrintf(UI_SCREEN_WIDTH - 17, 310, 0, rotation::ROTATE_270, "FW: %04X-%s", fwVersion, fwVersionSuffix);
    fr.epdPrintf(10, UI_SCREEN_HEIGHT - 25, 0, rotation::ROTATE_0, "MAC: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
    addFlashImage(420, 81, COLOR_BLACK, rotation::ROTATE_0, newton);
    addQR(100, 160, 3, 7, "https://openepaperlink.eu/tag/0/%02X/%02X%02X%02X%02X%02X%02X%02X%02X/", HW_TYPE, mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);

#endif
#if (HW_TYPE == SOLUM_M3_BWR_97)
    fontrender fr(&FreeSansBold24pt7b);
    fr.epdPrintf(10, 10, COLOR_BLACK, rotation::ROTATE_0, "OpenEPaperLink");
    fr.setFont(&FreeSansBold18pt7b);
    fr.epdPrintf(15, 60, COLOR_RED, rotation::ROTATE_0, "Newton M3 9.7\"");
    fr.setFont(&FreeSans9pt7b);
    fr.epdPrintf(UI_SCREEN_WIDTH - 37, 310, 0, rotation::ROTATE_270, "FW: %04X-%s", fwVersion, fwVersionSuffix);
    fr.epdPrintf(10, UI_SCREEN_HEIGHT - 25, 0, rotation::ROTATE_0, "MAC: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
    addFlashImage(220, 420, COLOR_BLACK, rotation::ROTATE_0, newton);
    addQR(260, 160, 3, 7, "https://openepaperlink.eu/tag/0/%02X/%02X%02X%02X%02X%02X%02X%02X%02X/", HW_TYPE, mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);

#endif
    draw();
}

void showApplyUpdate() {
    // setColorMode(EPD_MODE_NORMAL, EPD_MODE_INVERT);
    selectLUT(1);
    drawNoWait();
}

uint8_t resultcounter = 0;

void showScanningWindow() {
    // setColorMode(EPD_MODE_NORMAL, EPD_MODE_INVERT);
    selectLUT(EPD_LUT_FAST_NO_REDS);
    draw();
    selectLUT(EPD_LUT_FAST);
    resultcounter = 0;
}

void showAPFound() {
    selectLUT(EPD_LUT_NO_REPEATS);

#if (HW_TYPE == SOLUM_M3_BWR_22)
    fontrender fr(&FreeSansBold18pt7b);
    fr.epdPrintf(7, 7, COLOR_BLACK, rotation::ROTATE_0, "AP Found");
    fr.setFont(&FreeSans9pt7b);
    fr.epdPrintf(10, 53, COLOR_RED, rotation::ROTATE_0, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", APmac[7], APmac[6], APmac[5], APmac[4], APmac[3], APmac[2], APmac[1], APmac[0]);
    fr.epdPrintf(10, 71, COLOR_RED, rotation::ROTATE_0, "RSSI: %ddBm    LQI: %d", mLastRSSI, mLastLqi);
    fr.epdPrintf(10, 89, COLOR_RED, rotation::ROTATE_0, "Ch %d", currentChannel);
    fr.setFont(&FreeSans9pt7b);
    fr.epdPrintf(10, UI_SCREEN_HEIGHT - 43, 0, rotation::ROTATE_0, "Battery: %d.%dV Temp: %d'C", batteryVoltage / 1000, batteryVoltage % 1000, temperature);
    fr.epdPrintf(10, UI_SCREEN_HEIGHT - 25, 0, rotation::ROTATE_0, "MAC: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
    addQR(UI_SCREEN_WIDTH - 66, 47, 3, 2, "https://openepaperlink.eu/tag/1/%02X/%02X%02X%02X%02X%02X%02X%02X%02X/", HW_TYPE, mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
#endif
#if (HW_TYPE == SOLUM_M3_BWR_29)
    fontrender fr(&FreeSansBold18pt7b);
    fr.epdPrintf(7, 7, COLOR_BLACK, rotation::ROTATE_0, "AP Found");
    fr.setFont(&FreeSans9pt7b);
    fr.epdPrintf(10, 53, COLOR_RED, rotation::ROTATE_0, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", APmac[7], APmac[6], APmac[5], APmac[4], APmac[3], APmac[2], APmac[1], APmac[0]);
    fr.epdPrintf(10, 71, COLOR_RED, rotation::ROTATE_0, "RSSI: %ddBm    LQI: %d", mLastRSSI, mLastLqi);
    fr.epdPrintf(10, 89, COLOR_RED, rotation::ROTATE_0, "Ch %d", currentChannel);
    fr.setFont(&FreeSans9pt7b);
    fr.epdPrintf(10, UI_SCREEN_HEIGHT - 43, 0, rotation::ROTATE_0, "Battery: %d.%dV Temp: %d'C", batteryVoltage / 1000, batteryVoltage % 1000, temperature);
    fr.epdPrintf(10, UI_SCREEN_HEIGHT - 25, 0, rotation::ROTATE_0, "MAC: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
    addQR(UI_SCREEN_WIDTH - 66, 47, 3, 2, "https://openepaperlink.eu/tag/1/%02X/%02X%02X%02X%02X%02X%02X%02X%02X/", HW_TYPE, mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
#endif
#if (HW_TYPE == SOLUM_M3_BWR_43)
    fontrender fr(&FreeSansBold18pt7b);
    fr.epdPrintf(7, 7, COLOR_BLACK, rotation::ROTATE_0, "AP Found - Waiting for data");
    fr.setFont(&FreeSans9pt7b);
    fr.epdPrintf(15, 55, COLOR_RED, rotation::ROTATE_0, "AP: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", APmac[7], APmac[6], APmac[5], APmac[4], APmac[3], APmac[2], APmac[1], APmac[0]);
    fr.epdPrintf(15, 73, COLOR_RED, rotation::ROTATE_0, "RSSI: %ddBm    LQI: %d", mLastRSSI, mLastLqi);
    fr.setFont(&FreeSansBold18pt7b);
    fr.epdPrintf(270, 55, COLOR_RED, rotation::ROTATE_0, "Ch %d", currentChannel);
    fr.setFont(&FreeSans9pt7b);
    fr.epdPrintf(10, UI_SCREEN_HEIGHT - 43, 0, rotation::ROTATE_0, "Battery: %d.%dV Temp: %d'C", batteryVoltage / 1000, batteryVoltage % 1000, temperature);
    fr.epdPrintf(10, UI_SCREEN_HEIGHT - 25, 0, rotation::ROTATE_0, "MAC: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
    addQR(UI_SCREEN_WIDTH - 66, 47, 3, 2, "https://openepaperlink.eu/tag/1/%02X/%02X%02X%02X%02X%02X%02X%02X%02X/", HW_TYPE, mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
#endif
#if (HW_TYPE == SOLUM_M3_BWR_60)
    fontrender fr(&FreeSansBold18pt7b);
    fr.epdPrintf(7, 7, COLOR_BLACK, rotation::ROTATE_0, "AP Found - Waiting for data");
    fr.setFont(&FreeSans9pt7b);
    fr.epdPrintf(15, 55, COLOR_RED, rotation::ROTATE_0, "AP: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", APmac[7], APmac[6], APmac[5], APmac[4], APmac[3], APmac[2], APmac[1], APmac[0]);
    fr.epdPrintf(15, 73, COLOR_RED, rotation::ROTATE_0, "RSSI: %ddBm    LQI: %d", mLastRSSI, mLastLqi);
    fr.setFont(&FreeSansBold18pt7b);
    fr.epdPrintf(270, 55, COLOR_RED, rotation::ROTATE_0, "Ch %d", currentChannel);
    fr.setFont(&FreeSans9pt7b);
    fr.epdPrintf(10, UI_SCREEN_HEIGHT - 43, 0, rotation::ROTATE_0, "Battery: %d.%dV Temp: %d'C", batteryVoltage / 1000, batteryVoltage % 1000, temperature);
    fr.epdPrintf(10, UI_SCREEN_HEIGHT - 25, 0, rotation::ROTATE_0, "MAC: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
    addQR(UI_SCREEN_WIDTH - 66, 47, 3, 2, "https://openepaperlink.eu/tag/1/%02X/%02X%02X%02X%02X%02X%02X%02X%02X/", HW_TYPE, mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
#endif
#if (HW_TYPE == SOLUM_M3_BWR_75)
    fontrender fr(&FreeSansBold18pt7b);
    fr.epdPrintf(7, 7, COLOR_BLACK, rotation::ROTATE_0, "AP Found - Waiting for data");
    fr.setFont(&FreeSans9pt7b);
    fr.epdPrintf(15, 55, COLOR_RED, rotation::ROTATE_0, "AP: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", APmac[7], APmac[6], APmac[5], APmac[4], APmac[3], APmac[2], APmac[1], APmac[0]);
    fr.epdPrintf(15, 73, COLOR_RED, rotation::ROTATE_0, "RSSI: %ddBm    LQI: %d", mLastRSSI, mLastLqi);
    fr.setFont(&FreeSansBold18pt7b);
    fr.epdPrintf(270, 55, COLOR_RED, rotation::ROTATE_0, "Ch %d", currentChannel);
    fr.setFont(&FreeSans9pt7b);
    fr.epdPrintf(10, UI_SCREEN_HEIGHT - 43, 0, rotation::ROTATE_0, "Battery: %d.%dV Temp: %d'C", batteryVoltage / 1000, batteryVoltage % 1000, temperature);
    fr.epdPrintf(10, UI_SCREEN_HEIGHT - 25, 0, rotation::ROTATE_0, "MAC: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
    addQR(UI_SCREEN_WIDTH - 66, 47, 3, 2, "https://openepaperlink.eu/tag/1/%02X/%02X%02X%02X%02X%02X%02X%02X%02X/", HW_TYPE, mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
#endif
#if (HW_TYPE == SOLUM_M3_BWR_97)
    fontrender fr(&FreeSansBold18pt7b);
    fr.epdPrintf(7, 7, COLOR_BLACK, rotation::ROTATE_0, "AP Found - Waiting for data");
    fr.setFont(&FreeSans9pt7b);
    fr.epdPrintf(15, 55, COLOR_RED, rotation::ROTATE_0, "AP: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", APmac[7], APmac[6], APmac[5], APmac[4], APmac[3], APmac[2], APmac[1], APmac[0]);
    fr.epdPrintf(15, 73, COLOR_RED, rotation::ROTATE_0, "RSSI: %ddBm    LQI: %d", mLastRSSI, mLastLqi);
    fr.setFont(&FreeSansBold18pt7b);
    fr.epdPrintf(270, 55, COLOR_RED, rotation::ROTATE_0, "Ch %d", currentChannel);
    fr.setFont(&FreeSans9pt7b);
    fr.epdPrintf(10, UI_SCREEN_HEIGHT - 43, 0, rotation::ROTATE_0, "Battery: %d.%dV Temp: %d'C", batteryVoltage / 1000, batteryVoltage % 1000, temperature);
    fr.epdPrintf(10, UI_SCREEN_HEIGHT - 25, 0, rotation::ROTATE_0, "MAC: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
    addQR(UI_SCREEN_WIDTH - 66, 47, 3, 2, "https://openepaperlink.eu/tag/1/%02X/%02X%02X%02X%02X%02X%02X%02X%02X/", HW_TYPE, mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
#endif
    addOverlay();
    draw();
}
void showNoAP() {
    // selectLUT(EPD_LUT_NO_REPEATS);
#if (HW_TYPE == SOLUM_M3_BWR_22)
    fontrender fr(&FreeSansBold18pt7b);
    fr.epdPrintf(7, 7, COLOR_BLACK, rotation::ROTATE_0, "No AP Found");
    fr.setFont(&FreeSans9pt7b);
    addQR(UI_SCREEN_WIDTH - 66, 47, 3, 2, "https://openepaperlink.eu/tag/1/%02X/%02X%02X%02X%02X%02X%02X%02X%02X/", HW_TYPE, mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
    fr.epdPrintf(10, 69, COLOR_BLACK, rotation::ROTATE_0, "Couldn't find an AP :(");
    fr.epdPrintf(10, 89, COLOR_BLACK, rotation::ROTATE_0, "I'll try again in a little while, but you");
    fr.epdPrintf(10, 109, COLOR_BLACK, rotation::ROTATE_0, "can force a retry now by pressing a button");
#endif
#if (HW_TYPE == SOLUM_M3_BWR_29)
    fontrender fr(&FreeSansBold18pt7b);
    fr.epdPrintf(7, 7, COLOR_BLACK, rotation::ROTATE_0, "No AP Found");
    fr.setFont(&FreeSans9pt7b);
    addQR(UI_SCREEN_WIDTH - 66, 47, 3, 2, "https://openepaperlink.eu/tag/1/%02X/%02X%02X%02X%02X%02X%02X%02X%02X/", HW_TYPE, mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
    fr.epdPrintf(10, 69, COLOR_BLACK, rotation::ROTATE_0, "Couldn't find an AP :(");
    fr.epdPrintf(10, 89, COLOR_BLACK, rotation::ROTATE_0, "I'll try again in a little while, but you");
    fr.epdPrintf(10, 109, COLOR_BLACK, rotation::ROTATE_0, "can force a retry now by pressing a button");
#endif
#if (HW_TYPE == SOLUM_M3_BWR_43)
    fontrender fr(&FreeSansBold18pt7b);
    fr.epdPrintf(7, 7, COLOR_BLACK, rotation::ROTATE_0, "No AP Found          UwU");
    fr.setFont(&FreeSans9pt7b);
    addQR(UI_SCREEN_WIDTH - 66, 47, 3, 2, "https://openepaperlink.eu/tag/1/%02X/%02X%02X%02X%02X%02X%02X%02X%02X/", HW_TYPE, mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
    drawRoundedRectangle(36, 55, 112, 42, COLOR_RED);
    fr.epdPrintf(44, 61, COLOR_BLACK, rotation::ROTATE_0, "NFC WAKE");
    fr.epdPrintf(41, 77, COLOR_BLACK, rotation::ROTATE_0, "SCAN HERE");

    fr.epdPrintf(152, 49, COLOR_BLACK, rotation::ROTATE_0, "Couldn't find an AP :(");
    fr.epdPrintf(152, 69, COLOR_BLACK, rotation::ROTATE_0, "I'll try again in a little while, but you");
    fr.epdPrintf(152, 89, COLOR_BLACK, rotation::ROTATE_0, "can force a retry now by scanning");
    fr.epdPrintf(152, 109, COLOR_BLACK, rotation::ROTATE_0, "the NFC-wake area with your phone");
#endif
#if (HW_TYPE == SOLUM_M3_BWR_60)
    fontrender fr(&FreeSansBold18pt7b);
    fr.epdPrintf(7, 7, COLOR_BLACK, rotation::ROTATE_0, "No AP Found          U_U");
    fr.setFont(&FreeSans9pt7b);
    addQR(UI_SCREEN_WIDTH - 66, 47, 3, 2, "https://openepaperlink.eu/tag/1/%02X/%02X%02X%02X%02X%02X%02X%02X%02X/", HW_TYPE, mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
    fr.epdPrintf(10, 39, COLOR_BLACK, rotation::ROTATE_0, "Couldn't find an AP :(");
    fr.epdPrintf(10, 58, COLOR_BLACK, rotation::ROTATE_0, "I'll try again in a little while, but you");
    fr.epdPrintf(10, 77, COLOR_BLACK, rotation::ROTATE_0, "can force a retry now by pressing a button");
    addFlashImage(0, 96, COLOR_BLACK, rotation::ROTATE_0, pandablack);
    addFlashImage(112, 242, COLOR_RED, rotation::ROTATE_0, pandared);
#endif
#if (HW_TYPE == SOLUM_M3_BWR_75)
    fontrender fr(&FreeSansBold18pt7b);
    fr.epdPrintf(7, 7, COLOR_BLACK, rotation::ROTATE_0, "No AP Found          U_U");
    fr.setFont(&FreeSans9pt7b);
    addQR(UI_SCREEN_WIDTH - 66, 47, 3, 2, "https://openepaperlink.eu/tag/1/%02X/%02X%02X%02X%02X%02X%02X%02X%02X/", HW_TYPE, mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
    fr.epdPrintf(10, 39, COLOR_BLACK, rotation::ROTATE_0, "Couldn't find an AP :(");
    fr.epdPrintf(10, 58, COLOR_BLACK, rotation::ROTATE_0, "I'll try again in a little while, but you");
    fr.epdPrintf(10, 77, COLOR_BLACK, rotation::ROTATE_0, "can force a retry now by pressing a button");
    addFlashImage(200, 128, COLOR_BLACK, rotation::ROTATE_0, pandablack);
    addFlashImage(312, 274, COLOR_RED, rotation::ROTATE_0, pandared);
#endif
#if (HW_TYPE == SOLUM_M3_BWR_97)
    fontrender fr(&FreeSansBold18pt7b);
    fr.epdPrintf(7, 7, COLOR_BLACK, rotation::ROTATE_0, "No AP Found          U_U");
    fr.setFont(&FreeSans9pt7b);
    addQR(UI_SCREEN_WIDTH - 66, 47, 3, 2, "https://openepaperlink.eu/tag/1/%02X/%02X%02X%02X%02X%02X%02X%02X%02X/", HW_TYPE, mSelfMac[7], mSelfMac[6], mSelfMac[5], mSelfMac[4], mSelfMac[3], mSelfMac[2], mSelfMac[1], mSelfMac[0]);
    fr.epdPrintf(10, 39, COLOR_BLACK, rotation::ROTATE_0, "Couldn't find an AP :(");
    fr.epdPrintf(10, 58, COLOR_BLACK, rotation::ROTATE_0, "I'll try again in a little while, but you");
    fr.epdPrintf(10, 77, COLOR_BLACK, rotation::ROTATE_0, "can force a retry now by pressing a button");
    addFlashImage(200, 128, COLOR_BLACK, rotation::ROTATE_0, pandablack);
    addFlashImage(312, 274, COLOR_RED, rotation::ROTATE_0, pandared);
#endif
    addOverlay();
    draw();
    delay(5000);
}

void showLongTermSleep() {
    selectLUT(EPD_LUT_NO_REPEATS);
    // setColorMode(EPD_MODE_NORMAL, EPD_MODE_INVERT);

    addOverlay();
    draw();
}
void showNoEEPROM() {
    selectLUT(EPD_LUT_NO_REPEATS);
    draw();
}

void showNoMAC() {
    selectLUT(EPD_LUT_NO_REPEATS);
    draw();
}