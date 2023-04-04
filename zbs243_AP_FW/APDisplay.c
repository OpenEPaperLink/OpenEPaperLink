#define __packed
#include "board.h"

#if (HAS_SCREEN == 1)
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "asmUtil.h"
#include "comms.h"
#include "cpu.h"
#include "printf.h"
#include "proto.h"
#include "radio.h"
#include "screen.h"
#include "timer.h"
#include "uart.h"
#include "wdt.h"

// #if (HAS_SCREEN == 1)

uint8_t updateCount = 1;

void epdInitialize() {
    epdEnable();
    pr("Setting up EPD\n");
    if (!epdSetup(false)) pr("EPD setup failed\n");
    epdSetPos(0);
    epdpr("boot");
    setEPDIcon(EPD_ICON_DIAMOND, true);
    epdDraw();
    while (is_drawing()) {
    };
    timerDelay(1333);
}

extern uint8_t __xdata curChannel;

extern uint8_t __xdata curPendingData;
extern uint8_t __xdata curNoUpdate;

void epdShowRun() {
    epdClear();
    epdSetPos(0);
    epdpr("run");
    setEPDIcon(EPD_ICON_ARROW | EPD_SIGN_PENCE_SMALL, true);

    epdSetPos(4);
    epdpr("%d", curChannel);

    epdSetPos(6);
    epdpr("%d", curNoUpdate);

    epdSetPos(8);
    epdpr("%d", curPendingData);

    if (epdUpdate()) {
        updateCount++;
        if (updateCount == 10) {
            while (is_drawing()) {
            };
            epdSetup(true);
        } else if (updateCount == 20) {
            while (is_drawing()) {
            };
            epdSetup(false);
            updateCount = 0;
        }
    }
}

#endif