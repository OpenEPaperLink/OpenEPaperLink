#define __packed
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "asmUtil.h"
#include "comms.h"  // for mLastLqi and mLastRSSI
#include "eeprom.h"
#include "i2c.h"
#include "i2cdevices.h"
#include "powermgt.h"
#include "printf.h"
#include "proto.h"
#include "radio.h"
#include "screen.h"
#include "settings.h"
#include "syncedproto.h"
#include "timer.h"
#include "userinterface.h"
#include "wdt.h"

// #define DEBUG_MODE

uint8_t __xdata capabilities = 0;

void displayLoop() {
    powerUp(INIT_BASE | INIT_UART);

    pr("Splash screen\n");
    powerUp(INIT_EPD);
    showSplashScreen();
    timerDelay(TIMER_TICKS_PER_SECOND * 4);

    pr("Update screen\n");
    powerUp(INIT_EPD);
    showApplyUpdate();
    timerDelay(TIMER_TICKS_PER_SECOND * 4);

    wdt60s();

    pr("Scanning screen - ");
    powerUp(INIT_EPD);
    showScanningWindow();
    timerDelay(TIMER_TICKS_PER_SECOND * 8);
    for (uint8_t i = 0; i < 5; i++) {
        for (uint8_t c = 0; c < 16; c++) {
            addScanResult(11 + c, 2 * i + 60 + c);
        }
        pr("redraw... ");
        draw();
    }
    pr("\n");
    timerDelay(TIMER_TICKS_PER_SECOND * 4);

    wdt30s();

    pr("AP Found\n");
    powerUp(INIT_EPD);
    showAPFound();
    timerDelay(TIMER_TICKS_PER_SECOND * 4);

    wdt30s();

    pr("AP NOT Found\n");
    powerUp(INIT_EPD);
    showNoAP();
    timerDelay(TIMER_TICKS_PER_SECOND * 4);

    wdt30s();

    pr("Longterm sleep screen\n");
    powerUp(INIT_EPD);
    showLongTermSleep();
    timerDelay(TIMER_TICKS_PER_SECOND * 4);

    wdt30s();

    pr("NO EEPROM\n");
    powerUp(INIT_EPD);
    showNoEEPROM();
    timerDelay(TIMER_TICKS_PER_SECOND * 4);

    wdt30s();

    pr("NO EEPROM\n");
    powerUp(INIT_EPD);
    showNoMAC();
    timerDelay(TIMER_TICKS_PER_SECOND * 4);
    wdtDeviceReset();
}

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

void main() {
    // displayLoop();  // remove me
    setupPortsInitial();
    powerUp(INIT_BASE | INIT_UART);

    if (RESET & 0x01) {
        wakeUpReason = WAKEUP_REASON_WDT_RESET;
        pr("WDT reset!\n");
    } else {
        wakeUpReason = WAKEUP_REASON_FIRSTBOOT;
    }

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
            powerDown(INIT_EPD | INIT_UART | INIT_EEPROM);
            doSleep(-1);
            wdtDeviceReset();
        }
    }

    pr("BOOTED>  %d.%d.%d%s", fwVersion / 100, (fwVersion % 100) / 10, (fwVersion % 10), fwVersionSuffix);

#ifdef HAS_BUTTON
    capabilities |= CAPABILITY_HAS_WAKE_BUTTON;
#endif
    powerUp(INIT_I2C);
    if (i2cCheckDevice(0x55)){
        capabilities |= CAPABILITY_HAS_NFC;
    }
    powerDown(INIT_I2C);

    pr("MAC>%02X%02X", mSelfMac[0], mSelfMac[1]);
    pr("%02X%02X", mSelfMac[2], mSelfMac[3]);
    pr("%02X%02X", mSelfMac[4], mSelfMac[5]);
    pr("%02X%02X\n", mSelfMac[6], mSelfMac[7]);

    powerUp(INIT_RADIO);  // load down the battery using the radio to get a good voltage reading
    powerUp(INIT_EPD_VOLTREADING | INIT_TEMPREADING);
    powerDown(INIT_RADIO);

    powerUp(INIT_EEPROM);
    // get the highest slot number, number of slots
    initializeProto();
    powerDown(INIT_EEPROM);

    // show the splashscreen
    powerUp(INIT_EPD);
    showSplashScreen();

    powerUp(INIT_EPD);
    wdt30s();
    currentChannel = showChannelSelect();

    wdt10s();

    if (currentChannel) {
        showAPFound();
        initPowerSaving(INTERVAL_BASE);
        powerDown(INIT_EPD | INIT_UART);
        doSleep(5000UL);
    } else {
        showNoAP();
        initPowerSaving(INTERVAL_AT_MAX_ATTEMPTS);
        powerDown(INIT_EPD | INIT_UART);
        doSleep(120000UL);
    }

    while (1) {
        powerUp(INIT_UART);
        wdt10s();
        if (currentChannel) {
            // associated

            struct AvailDataInfo *__xdata avail;
            // Is there any reason why we should do a long (full) get data request (including reason, status)?
            if ((longDataReqCounter > LONG_DATAREQ_INTERVAL) || wakeUpReason != WAKEUP_REASON_TIMED) {
                // check if we should do a voltage measurement (those are pretty expensive)
                if (voltageCheckCounter == VOLTAGE_CHECK_INTERVAL) {
                    powerUp(INIT_RADIO);  // load down the battery using the radio to get a good reading
                    powerUp(INIT_TEMPREADING | INIT_EPD_VOLTREADING);
                    powerDown(INIT_RADIO);
                    voltageCheckCounter = 0;
                } else {
                    powerUp(INIT_TEMPREADING);
                }
                voltageCheckCounter++;

                // check if the battery level is below minimum, and force a redraw of the screen
                if ((lowBattery && !lowBatteryShown) || (noAPShown)) {
                    // Check if we were already displaying an image
                    if (curImgSlot != 0xFF) {
                        powerUp(INIT_EEPROM | INIT_EPD);
                        wdt60s();
                        drawImageFromEeprom(curImgSlot);
                        powerDown(INIT_EEPROM | INIT_EPD);
                    } else {
                        powerUp(INIT_EPD);
                        showAPFound();
                        powerDown(INIT_EPD);
                    }
                }
                powerUp(INIT_RADIO);
                avail = getAvailDataInfo();
                powerDown(INIT_RADIO);

                if (avail != NULL) {
                    // we got some data!
                    longDataReqCounter = 0;
                    // since we've had succesful contact, and communicated the wakeup reason succesfully, we can now reset to the 'normal' status
                    wakeUpReason = WAKEUP_REASON_TIMED;
                }
            } else {
                powerUp(INIT_RADIO);
                avail = getShortAvailDataInfo();
                powerDown(INIT_RADIO);
            }

            addAverageValue();

            if (avail == NULL) {
                // no data :(
                nextCheckInFromAP = 0;  // let the power-saving algorithm determine the next sleep period
            } else {
                nextCheckInFromAP = avail->nextCheckIn;
                // got some data from the AP!
                if (avail->dataType != DATATYPE_NOUPDATE) {
                    // data transfer
                    if (processAvailDataInfo(avail)) {
                        // succesful transfer, next wake time is determined by the NextCheckin;
                    } else {
                        // failed transfer, let the algorithm determine next sleep interval (not the AP)
                        nextCheckInFromAP = 0;
                    }
                } else {
                    // no data transfer, just sleep.
                }
            }

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
            if (((scanAttempts != 0) && (scanAttempts % VOLTAGEREADING_DURING_SCAN_INTERVAL == 0)) || (scanAttempts > (INTERVAL_1_ATTEMPTS + INTERVAL_2_ATTEMPTS))) {
                powerUp(INIT_RADIO);  // load down the battery using the radio to get a good reading
                powerUp(INIT_EPD_VOLTREADING);
                powerDown(INIT_RADIO);
            }
            // try to find a working channel
            powerUp(INIT_RADIO);
            currentChannel = channelSelect();
            powerDown(INIT_RADIO);

            if ((!currentChannel && !noAPShown) || (lowBattery && !lowBatteryShown) || (scanAttempts == (INTERVAL_1_ATTEMPTS + INTERVAL_2_ATTEMPTS - 1))) {
                powerUp(INIT_EPD);
                wdt60s();
                if (curImgSlot != 0xFF) {
                    powerUp(INIT_EEPROM);
                    drawImageFromEeprom(curImgSlot);
                    powerDown(INIT_EEPROM);
                } else if ((scanAttempts >= (INTERVAL_1_ATTEMPTS + INTERVAL_2_ATTEMPTS - 1))) {
                    showLongTermSleep();
                } else {
                    showNoAP();
                }
                powerDown(INIT_EPD);
            }

            // did we find a working channel?
            if (currentChannel) {
                // now associated!
                scanAttempts = 0;
                wakeUpReason = WAKEUP_REASON_NETWORK_SCAN;
                initPowerSaving(INTERVAL_BASE);
                doSleep(getNextSleep() * 1000UL);

            } else {
                // still not associated
                doSleep(getNextScanSleep(true) * 1000UL);
            }
        }
    }
}
