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
#include "settings.h"
#include "syncedproto.h"
#include "timer.h"
#include "userinterface.h"
#include "wdt.h"

// #define DEBUG_MODE
static bool __xdata attemptFirstContact = true;

uint8_t showChannelSelect() {  // returns 0 if no accesspoints were found
    uint8_t __xdata result[16];
    memset(result, 0, sizeof(result));
    showScanningWindow();
    powerUp(INIT_RADIO);
    for (uint8_t i = 0; i < 5; i++) {
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
    powerDown(INIT_RADIO);
    epdWaitRdy();
    mLastLqi = highestLqi;
    return highestSlot;
}
uint8_t channelSelect() {  // returns 0 if no accesspoints were found
    uint8_t __xdata result[16];
    memset(result, 0, sizeof(result));
    for (uint8_t i = 0; i < 2; i++) {
        for (uint8_t c = 11; c < 27; c++) {
            if (detectAP(c)) {
                if (mLastLqi > result[c - 11]) result[c - 11] = mLastLqi;
            }
        }
    }
    uint8_t __xdata highestLqi = 0;
    uint8_t __xdata highestSlot = 0;
    for (uint8_t c = 0; c < sizeof(result); c++) {
        if (result[c] > highestLqi) {
            highestSlot = c + 11;
            highestLqi = result[c];
        }
    }

    mLastLqi = highestLqi;
    return highestSlot;
}

void mainProtocolLoop(void) {
    powerUp(INIT_BASE | INIT_UART | INIT_GPIO);
    wdt10s();
    boardGetOwnMac(mSelfMac);

    {
        bool __xdata macSet = false;
        for (uint8_t __xdata c = 0; c < 8; c++) {
            if (mSelfMac[c] != 0xFF) {
                macSet = true;
                break;
            }
        }

        if (!macSet) {
            pr("Mac can't be all FF's.\n");
            powerUp(INIT_EPD);
            showNoMAC();
            powerDown(INIT_EPD | INIT_GPIO | INIT_EEPROM);
            doSleep(-1);
            wdtDeviceReset();
        }
    }

    pr("BOOTED>  %d.%d.%d%s", fwVersion / 100, (fwVersion % 100) / 10, (fwVersion % 10), fwVersionSuffix);

    pr("MAC>%02X%02X", mSelfMac[0], mSelfMac[1]);
    pr("%02X%02X", mSelfMac[2], mSelfMac[3]);
    pr("%02X%02X", mSelfMac[4], mSelfMac[5]);
    pr("%02X%02X\n", mSelfMac[6], mSelfMac[7]);

    powerUp(INIT_EPD_VOLTREADING | INIT_TEMPREADING | INIT_EEPROM);

    // get the highest slot number, number of slots
    initializeProto();
    powerDown(INIT_EEPROM);

    // show the splashscreen
    powerUp(INIT_EPD);
    showSplashScreen();

    wdt30s();
    powerUp(INIT_EPD);
    currentChannel = showChannelSelect();

    powerUp(INIT_GPIO | INIT_EPD);
    if (currentChannel) {
        showAPFound();
        initPowerSaving(INTERVAL_BASE);
        powerDown(INIT_EPD | INIT_GPIO);
        doSleep(5000UL);
    } else {
        showNoAP();
        initPowerSaving(INTERVAL_AT_MAX_ATTEMPTS);
        powerDown(INIT_EPD | INIT_GPIO);
        doSleep(120000UL);
    }

    while (1) {
        if (currentChannel) {
            // associated

            struct AvailDataInfo *__xdata avail;
            if ((longDataReqCounter > LONG_DATAREQ_INTERVAL) || attemptFirstContact || wakeUpReason != WAKEUP_REASON_TIMED) {
                if (attemptFirstContact)
                    wakeUpReason = WAKEUP_REASON_BOOTUP;
                
                
                if (voltageCheckCounter == VOLTAGE_CHECK_INTERVAL) {
                    powerUp(INIT_BASE | INIT_TEMPREADING | INIT_EPD_VOLTREADING | INIT_RADIO);
                    voltageCheckCounter = 0;
                } else {
                    powerUp(INIT_BASE | INIT_TEMPREADING | INIT_RADIO);
                }
                voltageCheckCounter++;

                avail = getAvailDataInfo();
                if (avail != NULL) {
                    longDataReqCounter = 0;
                    attemptFirstContact = false;
                }
            } else {
                powerUp(INIT_BASE | INIT_RADIO);  //| INIT_GPIO | INIT_UART
                avail = getShortAvailDataInfo();
            }
            powerDown(INIT_RADIO);

            addAverageValue();

            if (avail == NULL) {
                // no data :(
                nextCheckInFromAP = 0;  // let the power-saving algorithm determine the next sleep period
            } else {
                nextCheckInFromAP = avail->nextCheckIn;
                // got some data from the AP!
                if (avail->dataType != DATATYPE_NOUPDATE) {
                    // data transfer
                    wdt10s();
                    powerUp(INIT_GPIO | INIT_UART);
                    if (doDataDownload(avail)) {
                        // succesful transfer, next wake time is determined by the NextCheckin;
                    } else {
                        // failed transfer, let the algorithm determine next sleep interval (not the AP)
                        nextCheckInFromAP = 0;
                    }
                    powerUp(INIT_GPIO);
                    powerDown(INIT_EEPROM | INIT_RADIO);
                } else {
                    // no data transfer, just sleep.
                }
            }

            powerDown(INIT_GPIO);

            uint16_t nextCheckin = getNextSleep();
            longDataReqCounter += nextCheckin;
            if (nextCheckin == INTERVAL_AT_MAX_ATTEMPTS) {
                // disconnected, obviously...
                currentChannel = 0;
            }

            // if the AP told us to sleep for a specific period, do so.
            if (nextCheckInFromAP) {
                doSleep(nextCheckInFromAP * 60000UL);
            } else {
                doSleep(getNextSleep() * 1000UL);
            }

        } else {
            // not associated
            powerUp(INIT_BASE | INIT_RADIO);  // || INIT_GPIO | INIT_UART

            // try to find a working channel
            powerUp(INIT_RADIO);
            wdt30s();
            currentChannel = channelSelect();
            powerDown(INIT_RADIO | INIT_GPIO);

            // did we find a working channel?
            if (currentChannel) {
                // now associated!
                scanAttempts = 0;
                attemptFirstContact = true;
                initPowerSaving(INTERVAL_BASE);
                doSleep(getNextSleep() * 1000UL);

            } else {
                // still not associated
                doSleep(getNextScanSleep(true) * 1000UL);
            }
        }
    }
}

void main(void) {
    mainProtocolLoop();
}
