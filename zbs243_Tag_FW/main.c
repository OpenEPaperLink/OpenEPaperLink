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

static const uint64_t __code __at(0x008b) mVersionRom = 0x1000011300000000ull;

#define TAG_MODE_CHANSEARCH 0
#define TAG_MODE_ASSOCIATED 1
uint8_t currentTagMode = TAG_MODE_CHANSEARCH;

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
    uint8_t __xdata result[sizeof(channelList)];
    memset(result, 0, sizeof(result));
    showScanningWindow();
    drawNoWait();
    powerUp(INIT_RADIO);
    for (uint8_t i = 0; i < 4; i++) {
        for (uint8_t c = 0; c < sizeof(channelList); c++) {
            if (detectAP(channelList[c])) {
                if (mLastLqi > result[c]) result[c] = mLastLqi;
                pr("Channel: %d - LQI: %d RSSI %d\n", channelList[c], mLastLqi, mLastRSSI);
            }
        }
    }

    uint8_t __xdata highestLqi = 0;
    uint8_t __xdata highestSlot = 0;
    for (uint8_t c = 0; c < sizeof(result); c++) {
        if (result[c] > highestLqi) {
            highestSlot = channelList[c];
            highestLqi = result[c];
        }
    }
    powerDown(INIT_RADIO);
    epdWaitRdy();
    mLastLqi = highestLqi;
    return highestSlot;
}
uint8_t channelSelect() {  // returns 0 if no accesspoints were found
    powerUp(INIT_RADIO);
    uint8_t __xdata result[16];
    memset(result, 0, sizeof(result));

    for (uint8_t i = 0; i < 2; i++) {
        for (uint8_t c = 0; c < sizeof(channelList); c++) {
            if (detectAP(channelList[c])) {
                if (mLastLqi > result[c]) result[c] = mLastLqi;
            }
        }
    }
    powerDown(INIT_RADIO);
    uint8_t __xdata highestLqi = 0;
    uint8_t __xdata highestSlot = 0;
    for (uint8_t c = 0; c < sizeof(result); c++) {
        if (result[c] > highestLqi) {
            highestSlot = channelList[c];
            highestLqi = result[c];
        }
    }

    mLastLqi = highestLqi;
    return highestSlot;
}

void validateMacAddress() {
    // check if the mac contains at least some non-0xFF values
    for (uint8_t __xdata c = 0; c < 8; c++) {
        if (mSelfMac[c] != 0xFF) goto macIsValid;
    }
    // invalid mac address. Display warning screen and sleep forever
    pr("Mac can't be all FF's.\n");
    powerUp(INIT_EPD);
    showNoMAC();
    powerDown(INIT_EPD | INIT_UART | INIT_EEPROM);
    doSleep(-1);
    wdtDeviceReset();
macIsValid:
    return;
}
uint8_t getFirstWakeUpReason() {
    if (RESET & 0x01) {
        pr("WDT reset!\n");
        return WAKEUP_REASON_WDT_RESET;
    }
    return WAKEUP_REASON_FIRSTBOOT;
}
void checkI2C() {
    powerUp(INIT_I2C);
//  i2cBusScan();
    if (i2cCheckDevice(0x55)) {
        powerDown(INIT_I2C);
        // found something!
        capabilities |= CAPABILITY_HAS_NFC;
        if (supportsNFCWake()) {
            pr("NFC: NFC Wake Supported\n");
            capabilities |= CAPABILITY_NFC_WAKE;
        }
    } else {
        pr("I2C: No devices found");
        // didn't find a NFC chip on the expected ID
        powerDown(INIT_I2C);
    }
}

void detectButtonOrJig() {
    switch (checkButtonOrJig()) {
        case DETECT_P1_0_BUTTON:
            capabilities |= CAPABILITY_HAS_WAKE_BUTTON;
            break;
        case DETECT_P1_0_JIG:
            wdt120s();
            // show the screensaver, full LUT (minimal text to prevent image burn-in)
            powerUp(INIT_EPD);
            afterFlashScreenSaver();
            while (1)
                ;
            break;
        case DETECT_P1_0_NOTHING:
            break;
        default:
            break;
    }
}

void TagAssociated() {
    // associated
    struct AvailDataInfo *__xdata avail;
    // Is there any reason why we should do a long (full) get data request (including reason, status)?
    if ((longDataReqCounter > LONG_DATAREQ_INTERVAL) || wakeUpReason != WAKEUP_REASON_TIMED) {
        // check if we should do a voltage measurement (those are pretty expensive)
        if (voltageCheckCounter == VOLTAGE_CHECK_INTERVAL) {
            doVoltageReading();
            voltageCheckCounter = 0;
        } else {
            powerUp(INIT_TEMPREADING);
        }
        voltageCheckCounter++;

        // check if the battery level is below minimum, and force a redraw of the screen

        if ((lowBattery && !lowBatteryShown && tagSettings.enableLowBatSymbol) || (noAPShown && tagSettings.enableNoRFSymbol)) {
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
        if (tagSettings.enableTagRoaming) {
            uint8_t roamChannel = channelSelect();
            if (roamChannel) currentChannel = roamChannel;
        }
    } else {
        powerUp(INIT_RADIO);
        avail = getShortAvailDataInfo();
        powerDown(INIT_RADIO);
    }

    addAverageValue();

    if (avail == NULL) {
        // no data :( this means no reply from AP
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
        // We've averaged up to the maximum interval, this means the tag hasn't been in contact with an AP for some time.
        if (tagSettings.enableScanForAPAfterTimeout) {
            currentTagMode = TAG_MODE_CHANSEARCH;
            return;
        }
    }

    // if the AP told us to sleep for a specific period, do so.
    if (nextCheckInFromAP) {
        doSleep(nextCheckInFromAP * 60000UL);
    } else {
        doSleep(getNextSleep() * 1000UL);
    }
}

void TagChanSearch() {
    // not associated
    if (((scanAttempts != 0) && (scanAttempts % VOLTAGEREADING_DURING_SCAN_INTERVAL == 0)) || (scanAttempts > (INTERVAL_1_ATTEMPTS + INTERVAL_2_ATTEMPTS))) {
        doVoltageReading();
    }

    // try to find a working channel
    currentChannel = channelSelect();

    // Check if we should redraw the screen with icons, info screen or screensaver
    if ((!currentChannel && !noAPShown && tagSettings.enableNoRFSymbol) ||
        (lowBattery && !lowBatteryShown && tagSettings.enableLowBatSymbol) ||
        (scanAttempts == (INTERVAL_1_ATTEMPTS + INTERVAL_2_ATTEMPTS - 1))) {
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
        // now associated! set up and bail out of this loop.
        scanAttempts = 0;
        wakeUpReason = WAKEUP_REASON_NETWORK_SCAN;
        initPowerSaving(INTERVAL_BASE);
        doSleep(getNextSleep() * 1000UL);
        currentTagMode = TAG_MODE_ASSOCIATED;
        return;
    } else {
        // still not associated
        doSleep(getNextScanSleep(true) * 1000UL);
    }
}

void executeCommand(uint8_t cmd) {
    switch (cmd) {
        case CMD_DO_REBOOT:
            wdtDeviceReset();
            break;
        case CMD_DO_RESET_SETTINGS:
            loadDefaultSettings();
            writeSettings();
            break;
        case CMD_DO_SCAN:
            currentChannel = channelSelect();
            break;
    }
}

void main() {
    // displayLoop();  // remove me
    setupPortsInitial();
    powerUp(INIT_BASE | INIT_UART);
    pr("BOOTED>  %d.%d.%d%s\n", fwVersion / 100, (fwVersion % 100) / 10, (fwVersion % 10), fwVersionSuffix);

    // Find the reason why we're booting; is this a WDT?
    wakeUpReason = getFirstWakeUpReason();

    // get our own mac address. this is stored in Infopage at offset 0x10-onwards
    boardGetOwnMac(mSelfMac);
    pr("MAC>%02X%02X", mSelfMac[0], mSelfMac[1]);
    pr("%02X%02X", mSelfMac[2], mSelfMac[3]);
    pr("%02X%02X", mSelfMac[4], mSelfMac[5]);
    pr("%02X%02X\n", mSelfMac[6], mSelfMac[7]);

    // load settings from infopage
    loadSettings();

    // get the highest slot number, number of slots
    initializeProto();

    if (tagSettings.enableFastBoot) {
        // Fastboot
        pr("Doing fast boot\n");
        capabilities = tagSettings.fastBootCapabilities;
        if (tagSettings.fixedChannel) {
            currentChannel = tagSettings.fixedChannel;
        } else {
            currentChannel = channelSelect();
        }
    } else {
        // Normal boot/startup

        // validate the mac address; this will display a warning on the screen if the mac address is invalid
        validateMacAddress();

#if (NFC_TYPE == 1)
        // initialize I2C
        checkI2C();
#endif

        // Get a voltage reading on the tag, loading down the battery with the radio
        doVoltageReading();

        // detect button or jig
        detectButtonOrJig();

        // show the splashscreen
        pr("EPD: First powerup\n");
        powerUp(INIT_EPD);
        showSplashScreen();

// we've now displayed something on the screen; for the SSD1619, we are now aware of the lut-size
#ifdef EPD_SSD1619
        capabilities |= CAPABILITY_SUPPORTS_CUSTOM_LUTS;
        if (dispLutSize != 7) {
            capabilities |= CAPABILITY_ALT_LUT_SIZE;
        }
#endif
        tagSettings.fastBootCapabilities = capabilities;

        // now that we've collected all possible capabilities, save it to settings
        writeSettings();

        // scan for channels
        powerUp(INIT_EPD);
        wdt30s();
        if (tagSettings.fixedChannel) {
            currentChannel = tagSettings.fixedChannel;
        } else {
            currentChannel = showChannelSelect();
        }
    }

    // end of the fastboot option split
    wdt10s();
    powerUp(INIT_EPD);
    if (currentChannel) {
        showAPFound();
        initPowerSaving(INTERVAL_BASE);
        powerDown(INIT_EPD | INIT_UART);
        currentTagMode = TAG_MODE_ASSOCIATED;
        doSleep(5000UL);
    } else {
        showNoAP();
        initPowerSaving(INTERVAL_AT_MAX_ATTEMPTS);
        powerDown(INIT_EPD | INIT_UART);
        currentTagMode = TAG_MODE_CHANSEARCH;
        doSleep(120000UL);
    }

    // this is the loop we'll stay in forever, basically.
    while (1) {
        powerUp(INIT_UART);
        wdt10s();
        switch (currentTagMode) {
            case TAG_MODE_ASSOCIATED:
                TagAssociated();
                break;
            case TAG_MODE_CHANSEARCH:
                TagChanSearch();
                break;
        }
    }
}