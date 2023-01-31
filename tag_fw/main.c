#define __packed
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "asmUtil.h"
#include "eeprom.h"
#include "epd.h"
#include "powermgt.h"
#include "printf.h"
#include "proto.h"
#include "radio.h"
#include "comms.h" // for mLastLqi and mLastRSSI
#include "syncedproto.h"
#include "timer.h"
#include "userinterface.h"
#include "wdt.h"

void mainProtocolLoop(void) {
    clockingAndIntsInit();
    timerInit();
    boardInit();

    if (!boardGetOwnMac(mSelfMac)) {
        pr("failed to get MAC. Aborting\n");
        while (1)
            ;
    } else {
        /*
        for (uint8_t c = 0; c < 8; c++) {
            mSelfMac[c] = c + 5;
        }
        */
        // really... if I do the call below, it'll cost me 8 bytes IRAM. Not the kind of 'optimization' I ever dreamed of doing
        // pr("MAC>%02X%02X%02X%02X%02X%02X%02X%02X\n", mSelfMac[0], mSelfMac[1], mSelfMac[2], mSelfMac[3], mSelfMac[4], mSelfMac[5], mSelfMac[6], mSelfMac[7]);
        pr("MAC>%02X%02X", mSelfMac[0], mSelfMac[1]);
        pr("%02X%02X", mSelfMac[2], mSelfMac[3]);
        pr("%02X%02X", mSelfMac[4], mSelfMac[5]);
        pr("%02X%02X\n", mSelfMac[6], mSelfMac[7]);
    }

    irqsOn();
    boardInitStage2();

    pr("BOOTED> (UI 0.03-1)\n\n");

    if (!eepromInit()) {
        pr("failed to init eeprom\n");
        while (1)
            ;
    } else {
        initializeProto();
    }

    // initialize Powers-saving-attempt-array with the default value;
    initPowerSaving();

    // show the splashscreen
    showSplashScreen();

    epdEnterSleep();
    eepromDeepPowerDown();
    initRadio();

    for (uint8_t c = 25; c > 10; c--) {
        if (probeChannel(c)) {
            pr("Channel: %d - LQI: %d RSSI %d\n", c, mLastLqi, mLastRSSI);
        } else {
            pr("Channel %d - nothing.\n", c);
        }
    }

    P1CHSTA &= ~(1 << 0);

    while (1) {
        radioRxEnable(true, true);

        struct AvailDataInfo *__xdata avail = getAvailDataInfo();
        if (avail == NULL) {
            // no data :(
            nextCheckInFromAP = 0;  // let the power-saving algorithm determine the next sleep period
        } else {
            nextCheckInFromAP = avail->nextCheckIn;
            // got some data from the AP!
            if (avail->dataType != DATATYPE_NOUPDATE) {
                // data transfer
                if (doDataDownload(avail)) {
                    // succesful transfer, next wake time is determined by the NextCheckin;
                } else {
                    // failed transfer, let the algorithm determine next sleep interval (not the AP)
                    nextCheckInFromAP = 0;
                }
            } else {
                // no data transfer, just sleep.
            }
        }

        // if the AP told us to sleep for a specific period, do so.
        if (nextCheckInFromAP) {
            doSleep(nextCheckInFromAP * 60000UL);
        } else {
            doSleep(getNextSleep() * 1000UL);
        }
    }
}

void main(void) {
    mainProtocolLoop();
}
