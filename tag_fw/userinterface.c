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

void showSplashScreen() {
    epdSetup();
#if (SCREEN_WIDTH == 152)
    selectLUT(1);
    clearScreen();
    setColorMode(EPD_MODE_IGNORE, EPD_MODE_INVERT);
    drawLineHorizontal(EPD_COLOR_BLACK, 33, 1);
    epdPrintBegin(0, 0, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    pr("Booting!");
    epdPrintEnd();
    draw();
    timerDelay(1333000);
#endif
}


void showApplyUpdate(){
    epdSetup();
    selectLUT(1);
    clearScreen();
    setColorMode(EPD_MODE_IGNORE, EPD_MODE_NORMAL);
    epdPrintBegin(8, 60, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
    pr("Updating!");
    epdPrintEnd();
    drawNoWait();
}