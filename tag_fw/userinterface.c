#include "userinterface.h"

#include <stdbool.h>
#include <string.h>

#include "asmUtil.h"
#include "board.h"
#include "cpu.h"
#include "epd.h"
#include "font.h"
#include "lut.h"
#include "printf.h"
#include "screen.h"
#include "sleep.h"
#include "spi.h"
#include "timer.h"

extern uint8_t mSelfMac[];

void showSplashScreen() {
    epdSetup();
    lutTest();
#if (SCREEN_WIDTH == 152) // 1.54"
    selectLUT(1);
    clearScreen();
    setColorMode(EPD_MODE_NORMAL, EPD_MODE_INVERT);
    drawLineHorizontal(EPD_COLOR_BLACK, 33, 1);
    epdPrintBegin(0, 0, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    pr("Booting!");
    epdPrintEnd();
    draw();
    timerDelay(1333000);
#endif
#if (SCREEN_WIDTH == 128) // 2.9"
    selectLUT(1);
    clearScreen();
    setColorMode(EPD_MODE_NORMAL, EPD_MODE_INVERT);
    epdPrintBegin(128, 0, EPD_DIRECTION_Y, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    pr("Booting!");
    epdPrintEnd();
    epdPrintBegin(16, 10, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_RED);
    pr("MAC: %02X:%02X", mSelfMac[7], mSelfMac[6]);
    pr(":%02X:%02X", mSelfMac[5], mSelfMac[4]);
    pr(":%02X:%02X", mSelfMac[3], mSelfMac[2]);
    pr(":%02X:%02X", mSelfMac[1], mSelfMac[0]);
    epdPrintEnd();
    draw();
    timerDelay(1333000);
#endif
#if (SCREEN_WIDTH == 400) // 2.9"
    selectLUT(1);
    clearScreen();
    setColorMode(EPD_MODE_NORMAL, EPD_MODE_INVERT);
    epdPrintBegin(0, 0, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    pr("Booting!");
    epdPrintEnd();
    epdPrintBegin(16, 284, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_RED);
    pr("MAC: %02X:%02X", mSelfMac[7], mSelfMac[6]);
    pr(":%02X:%02X", mSelfMac[5], mSelfMac[4]);
    pr(":%02X:%02X", mSelfMac[3], mSelfMac[2]);
    pr(":%02X:%02X", mSelfMac[1], mSelfMac[0]);
    epdPrintEnd();
    draw();
    timerDelay(1333000);
#endif

}

void showApplyUpdate() {
    epdSetup();
    selectLUT(1);
    clearScreen();
    setColorMode(EPD_MODE_IGNORE, EPD_MODE_NORMAL);
    epdPrintBegin(8, 60, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    pr("Updating!");
    epdPrintEnd();
    drawNoWait();
}