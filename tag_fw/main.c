#define __packed
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "asmUtil.h"
#include "comms.h"  // for mLastLqi and mLastRSSI
#include "eeprom.h"
#include "epd.h"
#include "powermgt.h"
#include "printf.h"
#include "proto.h"
#include "radio.h"
#include "syncedproto.h"
#include "timer.h"
#include "userinterface.h"
#include "wdt.h"

//#define DEBUG_MODE



uint8_t showChannelSelect() {
    uint8_t __xdata result[16];
    memset(result, 0, sizeof(result));
    showScanningWindow();
    for (uint8_t i = 0; i < 8; i++) {
        for (uint8_t c = 11; c < 27; c++) {
            if (detectAP(c)) {
                if (mLastLqi > result[c - 11]) result[c - 11] = mLastLqi;
                pr("Channel: %d - LQI: %d RSSI %d\n", c, mLastLqi, mLastRSSI);
            }
        }
        epdWaitRdy();
        for (uint8_t c = 0; c < 16; c++) {
            addScanResult(11 + c, result[c]);
        }
        drawNoWait();
    }
    uint8_t __xdata highestLqi = 0;
    uint8_t __xdata highestSlot = 0;
    for (uint8_t c = 0; c < sizeof(result); c++) {
        if (result[c] > highestLqi) {
            highestSlot = c + 11;
            highestLqi = result[c];
        }
    }
    epdWaitRdy();
    mLastLqi = highestLqi;
    return highestSlot;
}
void mainProtocolLoop(void) {
    clockingAndIntsInit();
    timerInit();
    boardInit();

    if (!boardGetOwnMac(mSelfMac)) {
        pr("failed to get MAC. Aborting\n");
        while (1)
            ;
    } else {
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
    eepromDeepPowerDown();
    // initialize Powers-saving-attempt-array with the default value;
    initPowerSaving();
#ifndef DEBUG_MODE
    // show the splashscreen
        getExtraData();
        
    showSplashScreen();

    eepromDeepPowerDown();
    initRadio();

    currentChannel = showChannelSelect();
    if (currentChannel == 0) {
        // couldn't find an AP :()
        showNoAP();
    } else {
        radioSetChannel(currentChannel);
        // Found an AP.
        showAPFound();
    }
#endif
#ifdef DEBUG_MODE
    initRadio();
    currentChannel = 11;
#endif
    epdEnterSleep();

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
