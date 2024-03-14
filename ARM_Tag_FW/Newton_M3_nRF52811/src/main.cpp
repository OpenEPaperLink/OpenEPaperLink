#include <Arduino.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <SoftWire.h>
#include <AsyncDelay.h>

#include "settings.h"
#include "comms.h"
#include "drawing.h"
#include "powermgt.h"
#include "../../../oepl-proto.h"
#include "../../../oepl-definitions.h"
#include "syncedproto.h"
#include "hal.h"
#include "userinterface.h"
#include "wdt.h"
#include "../hal/Newton_M3_nRF52811/tagtype_db.h"

uint8_t getFirstWakeUpReason();

#define TAG_MODE_CHANSEARCH 0
#define TAG_MODE_ASSOCIATED 1

uint8_t currentTagMode = TAG_MODE_CHANSEARCH;

uint8_t slideShowCurrentImg = 0;
uint8_t slideShowRefreshCount = 1;

SoftWire sw(NFC_I2C_SDA, NFC_I2C_SCL);

extern "C" int _write(int file, char *ptr, int len) {
    (void)file; /* Not used, avoid warning */
    Serial.write(ptr, len);
    return len;
}

uint8_t getFirstWakeUpReason() {
    return WAKEUP_REASON_FIRSTBOOT;
}

uint8_t channelSelect(uint8_t rounds) {  // returns 0 if no accesspoints were found
    powerUp(INIT_RADIO);
    uint8_t result[16];
    memset(result, 0, sizeof(result));

    for (uint8_t i = 0; i < rounds; i++) {
        for (uint8_t c = 0; c < sizeof(channelList); c++) {
            if (detectAP(channelList[c])) {
                if (mLastLqi > result[c]) result[c] = mLastLqi;
                if (rounds > 2) printf("Channel: %d - LQI: %d RSSI %d\n", channelList[c], mLastLqi, mLastRSSI);
            }
        }
    }
    powerDown(INIT_RADIO);
    uint8_t highestLqi = 0;
    uint8_t highestSlot = 0;
    for (uint8_t c = 0; c < sizeof(result); c++) {
        if (result[c] > highestLqi) {
            highestSlot = channelList[c];
            highestLqi = result[c];
        }
    }

    mLastLqi = highestLqi;
    return highestSlot;
}

bool displayCustomImage(uint8_t imagetype) {
    powerUp(INIT_EEPROM);
    uint8_t slot = findSlotDataTypeArg(imagetype << 3);
    if (slot != 0xFF) {
        // found a slot for gpio button 1

        uint8_t lut = getEepromImageDataArgument(slot);
        lut &= 0x03;
        powerUp(INIT_EPD);
        drawImageFromEeprom(slot, lut);
        powerDown(INIT_EPD | INIT_EEPROM);
        return true;
    } else {
        powerDown(INIT_EEPROM);
    }
    return false;
}

void TagAssociated() {
    // associated
    bool fastNextCheckin = false;
    uint32_t fastNextCheckinDelay = WAKE_SECOND_CHECKIN_DELAY;
    struct AvailDataInfo *avail;
    static bool buttonCheckOut = false;  // send another full request if the previous was a trigger reason (buttons, nfc)
    // Is there any reason why we should do a long (full) get data request (including reason, status)?
    if ((longDataReqCounter > LONG_DATAREQ_INTERVAL) || wakeUpReason != WAKEUP_REASON_TIMED || buttonCheckOut) {
        // check if we should do a voltage measurement (those are pretty expensive)
        if (voltageCheckCounter == VOLTAGE_CHECK_INTERVAL) {
            powerUp(INIT_VOLTREADING);
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
                uint8_t lut = getEepromImageDataArgument(curImgSlot) & 0x03;
                drawImageFromEeprom(curImgSlot, lut);
                powerDown(INIT_EEPROM | INIT_EPD);
            } else {
                powerUp(INIT_EPD);
                if (!displayCustomImage(CUSTOM_IMAGE_APFOUND)) showAPFound();
                powerDown(INIT_EPD);
            }
        }

        powerUp(INIT_RADIO);
        avail = getAvailDataInfo();
        powerDown(INIT_RADIO);

        switch (wakeUpReason) {
            case WAKEUP_REASON_BUTTON1:
                if (displayCustomImage(CUSTOM_IMAGE_BUTTON1)) fastNextCheckinDelay = 0;
                fastNextCheckin = true;
                break;
            case WAKEUP_REASON_BUTTON2:
                if (displayCustomImage(CUSTOM_IMAGE_BUTTON2)) fastNextCheckinDelay = 0;
                fastNextCheckin = true;
                break;
            case WAKEUP_REASON_GPIO:
                if (displayCustomImage(CUSTOM_IMAGE_GPIO)) fastNextCheckinDelay = 0;
                fastNextCheckin = true;
                break;
            case WAKEUP_REASON_RF:
                if (displayCustomImage(CUSTOM_IMAGE_RF_WAKE)) fastNextCheckinDelay = 0;
                fastNextCheckin = true;
                break;
            case WAKEUP_REASON_NFC:
                if (displayCustomImage(CUSTOM_IMAGE_NFC_WAKE)) fastNextCheckinDelay = 0;
                fastNextCheckin = true;
                break;
        }

        if (avail != NULL) {
            // we got some data!
            longDataReqCounter = 0;
            if (buttonCheckOut == true) {
                buttonCheckOut = false;
            }

            // since we've had succesful contact, and communicated the wakeup reason succesfully, we can now reset to the 'normal' status
            if ((wakeUpReason == WAKEUP_REASON_GPIO) | (wakeUpReason == WAKEUP_REASON_BUTTON1) | (wakeUpReason == WAKEUP_REASON_BUTTON2) | (wakeUpReason == WAKEUP_REASON_NFC) | (wakeUpReason == CUSTOM_IMAGE_RF_WAKE)) {
                buttonCheckOut = true;
            }
            wakeUpReason = WAKEUP_REASON_TIMED;
        }
        if (tagSettings.enableTagRoaming) {
            uint8_t roamChannel = channelSelect(1);
            if (roamChannel) currentChannel = roamChannel;
        }
    } else {
        powerUp(INIT_RADIO);

#ifdef ENABLE_RETURN_DATA
        // example code to send data back to the AP. Up to 90 bytes can be sent in one packet
        uint8_t blaat[2] = {0xAB, 0xBA};
        sendTagReturnData(blaat, 2, 0x55);
#endif

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

    if (fastNextCheckin) {
        // do a fast check-in next
        fastNextCheckin = false;
        if (fastNextCheckinDelay) {
            doSleep(fastNextCheckinDelay);
        } else {
            doSleep(100UL);
        }
    } else {
        if (nextCheckInFromAP & 0x7FFF) {
            // if the AP told us to sleep for a specific period, do so.
            if (nextCheckInFromAP & 0x8000) {
                doSleep((nextCheckInFromAP & 0x7FFF) * 1000UL);
            } else {
                doSleep(nextCheckInFromAP * 60000UL);
            }
        } else {
            // sleep determined by algorithm
            doSleep(getNextSleep() * 1000UL);
        }
    }
}

void TagChanSearch() {
    // not associated
    if (((scanAttempts != 0) && (scanAttempts % VOLTAGEREADING_DURING_SCAN_INTERVAL == 0)) || (scanAttempts > (INTERVAL_1_ATTEMPTS + INTERVAL_2_ATTEMPTS))) {
        powerUp(INIT_VOLTREADING);
    }

    // try to find a working channel
    currentChannel = channelSelect(2);

    // Check if we should redraw the screen with icons, info screen or screensaver
    if ((!currentChannel && !noAPShown && tagSettings.enableNoRFSymbol) ||
        (lowBattery && !lowBatteryShown && tagSettings.enableLowBatSymbol) ||
        (scanAttempts == (INTERVAL_1_ATTEMPTS + INTERVAL_2_ATTEMPTS - 1))) {
        powerUp(INIT_EPD);
        wdt60s();
        if (curImgSlot != 0xFF) {
            if (!displayCustomImage(CUSTOM_IMAGE_LOST_CONNECTION)) {
                powerUp(INIT_EEPROM);
                uint8_t lut = getEepromImageDataArgument(curImgSlot) & 0x03;
                drawImageFromEeprom(curImgSlot, lut);
                powerDown(INIT_EEPROM);
            }
        } else if ((scanAttempts >= (INTERVAL_1_ATTEMPTS + INTERVAL_2_ATTEMPTS - 1))) {
            if (!displayCustomImage(CUSTOM_IMAGE_LONGTERMSLEEP)) showLongTermSleep();
        } else {
            if (!displayCustomImage(CUSTOM_IMAGE_LOST_CONNECTION)) showNoAP();
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

void TagSlideShow() {
    currentChannel = 11;  // suppress the no-rf image thing
    displayCustomImage(CUSTOM_IMAGE_SPLASHSCREEN);

    // do a short channel search
    currentChannel = channelSelect(2);

    printf("Slideshow mode ch: %d\n", currentChannel);

    // if we did find an AP, check in once
    if (currentChannel) {
        powerUp(INIT_VOLTREADING);
        struct AvailDataInfo *avail;
        powerUp(INIT_RADIO);
        avail = getAvailDataInfo();

        if (avail != NULL) {
            processAvailDataInfo(avail);
        }
    }
    powerDown(INIT_RADIO);

    // suppress the no-rf image
    currentChannel = 11;

    while (1) {
        powerUp(INIT_UART);
        wdt60s();
        powerUp(INIT_EEPROM);
        uint8_t img = findNextSlideshowImage(slideShowCurrentImg);
        if (img != slideShowCurrentImg) {
            slideShowCurrentImg = img;
            uint8_t lut = getEepromImageDataArgument(img) & 0x03;
            powerUp(INIT_EPD);
            if (SLIDESHOW_FORCE_FULL_REFRESH_EVERY) {
                slideShowRefreshCount++;
            }
            if ((slideShowRefreshCount == SLIDESHOW_FORCE_FULL_REFRESH_EVERY) || (lut == 0)) {
                slideShowRefreshCount = 1;
                lut = 0;
            }
            drawImageFromEeprom(img, lut);
            powerDown(INIT_EPD | INIT_EEPROM);
        } else {
            // same image, so don't update the screen; this only happens when there's exactly one slideshow image
            powerDown(INIT_EEPROM);
        }

        switch (tagSettings.customMode) {
            case TAG_CUSTOM_SLIDESHOW_FAST:
                doSleep(1000UL * SLIDESHOW_INTERVAL_FAST);
                break;
            case TAG_CUSTOM_SLIDESHOW_MEDIUM:
                doSleep(1000UL * SLIDESHOW_INTERVAL_MEDIUM);
                break;
            case TAG_CUSTOM_SLIDESHOW_SLOW:
                doSleep(1000UL * SLIDESHOW_INTERVAL_SLOW);
                break;
            case TAG_CUSTOM_SLIDESHOW_GLACIAL:
                doSleep(1000UL * SLIDESHOW_INTERVAL_GLACIAL);
                break;
        }
        printf("wake...\n");
    }
}

void executeCommand(uint8_t cmd) {
    printf("executing command %d \n", cmd);
    delay(20);
    switch (cmd) {
        case CMD_DO_REBOOT:
            NVIC_SystemReset();
            break;
        case CMD_DO_RESET_SETTINGS:
            powerUp(INIT_EEPROM);
            loadDefaultSettings();
            writeSettings();
            powerDown(INIT_EEPROM);
            break;
        case CMD_DO_SCAN:
            currentChannel = channelSelect(4);
            break;
        case CMD_DO_DEEPSLEEP:
            powerUp(INIT_EPD);
            afterFlashScreenSaver();
            powerDown(INIT_EPD | INIT_UART);
            while (1) {
                doSleep(-1);
            }
            break;
        case CMD_ERASE_EEPROM_IMAGES:
            powerUp(INIT_EEPROM);
            eraseImageBlocks();
            powerDown(INIT_EEPROM);
            break;
        case CMD_ENTER_SLIDESHOW_FAST:
            powerUp(INIT_EEPROM);
            if (findSlotDataTypeArg(CUSTOM_IMAGE_SLIDESHOW << 3) == 0xFF) {
                powerDown(INIT_EEPROM);
                return;
            }
            tagSettings.customMode = TAG_CUSTOM_SLIDESHOW_FAST;
            writeSettings();
            powerDown(INIT_EEPROM);
            NVIC_SystemReset();
            break;
        case CMD_ENTER_SLIDESHOW_MEDIUM:
            powerUp(INIT_EEPROM);
            if (findSlotDataTypeArg(CUSTOM_IMAGE_SLIDESHOW << 3) == 0xFF) {
                powerDown(INIT_EEPROM);
                return;
            }
            tagSettings.customMode = TAG_CUSTOM_SLIDESHOW_MEDIUM;
            writeSettings();
            powerDown(INIT_EEPROM);
            NVIC_SystemReset();
            break;
        case CMD_ENTER_SLIDESHOW_SLOW:
            powerUp(INIT_EEPROM);
            if (findSlotDataTypeArg(CUSTOM_IMAGE_SLIDESHOW << 3) == 0xFF) {
                powerDown(INIT_EEPROM);
                return;
            }
            tagSettings.customMode = TAG_CUSTOM_SLIDESHOW_SLOW;
            writeSettings();
            powerDown(INIT_EEPROM);
            NVIC_SystemReset();
            break;
        case CMD_ENTER_SLIDESHOW_GLACIAL:
            powerUp(INIT_EEPROM);
            if (findSlotDataTypeArg(CUSTOM_IMAGE_SLIDESHOW << 3) == 0xFF) {
                powerDown(INIT_EEPROM);
                return;
            }
            tagSettings.customMode = TAG_CUSTOM_SLIDESHOW_GLACIAL;
            writeSettings();
            powerDown(INIT_EEPROM);
            NVIC_SystemReset();
            break;
        case CMD_ENTER_NORMAL_MODE:
            tagSettings.customMode = TAG_CUSTOM_MODE_NONE;
            powerUp(INIT_EEPROM);
            writeSettings();
            powerDown(INIT_EEPROM);
            NVIC_SystemReset();
            break;
    }
}

void mainLoop() {
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

void setup() {
    setupBatteryVoltage();
    setupTemperature();
    Serial.begin(115200);
}

void loop() {
    powerUp(INIT_BASE | INIT_UART);
    printf("BOOTED> %04X-%s\n", fwVersion, fwVersionSuffix);

    wakeUpReason = getFirstWakeUpReason();
    identifyTagInfo();
    setupPortsInitial();
    boardGetOwnMac(mSelfMac);

    // do something if the mac isn't valid

    boardGetOwnMac(mSelfMac);
    printf("MAC>%02X%02X", mSelfMac[0], mSelfMac[1]);
    printf("%02X%02X", mSelfMac[2], mSelfMac[3]);
    printf("%02X%02X", mSelfMac[4], mSelfMac[5]);
    printf("%02X%02X\n", mSelfMac[6], mSelfMac[7]);

    // capabilities/options
    capabilities |= CAPABILITY_NFC_WAKE;
    if (tag.buttonCount) capabilities |= CAPABILITY_HAS_WAKE_BUTTON;
    if (tag.hasLED) capabilities |= CAPABILITY_HAS_LED;
    if (tag.hasNFC) {
        capabilities |= CAPABILITY_HAS_NFC;
        capabilities |= CAPABILITY_NFC_WAKE;
    }

    powerUp(INIT_EEPROM);
    loadSettings();
    initializeProto();
    invalidateSettingsEEPROM();

    powerDown(INIT_EEPROM);

    powerUp(INIT_VOLTREADING);
    powerUp(INIT_TEMPREADING);

    if (tagSettings.enableFastBoot) {
        // fast boot
        if (tagSettings.fixedChannel) {
            // don't scan, as this tag is bound to a fixed channel
            currentChannel = tagSettings.fixedChannel;
        } else {
            // do a quick scan
            wdt30s();
            currentChannel = channelSelect(2);
        }

        if (currentChannel) {
            currentTagMode = TAG_MODE_ASSOCIATED;
        } else {
            currentTagMode = TAG_MODE_CHANSEARCH;
        }
    } else {
        // normal boot

        // show the splashscreen
        powerUp(INIT_EPD);
        currentChannel = 1;
        if (!displayCustomImage(CUSTOM_IMAGE_SPLASHSCREEN)) showSplashScreen();
        currentChannel = 0;
        powerDown(INIT_EPD);

        if (tagSettings.fixedChannel) {
            currentChannel = tagSettings.fixedChannel;
        } else {
            currentChannel = channelSelect(4);
        }
    }

    wdt30s();
    powerUp(INIT_EPD);
    if (currentChannel) {
        if (!displayCustomImage(CUSTOM_IMAGE_APFOUND)) showAPFound();
        powerDown(INIT_EPD);
        initPowerSaving(INTERVAL_BASE);
        currentTagMode = TAG_MODE_ASSOCIATED;

        powerUp(INIT_EEPROM);
        writeSettings();
        powerDown(INIT_EEPROM);

        doSleep(5000UL);
    } else {
        if (!displayCustomImage(CUSTOM_IMAGE_NOAPFOUND)) showNoAP();
        powerDown(INIT_EPD);
        initPowerSaving(INTERVAL_AT_MAX_ATTEMPTS);
        currentTagMode = TAG_MODE_CHANSEARCH;

        powerUp(INIT_EEPROM);
        writeSettings();
        powerDown(INIT_EEPROM);

        doSleep(120000UL);
    }

    mainLoop();
    NVIC_SystemReset();
}