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

#include "radio.h"
#include "screen.h"
#include "settings.h"
#include "syncedproto.h"
#include "timer.h"
#include "userinterface.h"
#include "wdt.h"

#include "flash.h"

#include "uart.h"

#include "../oepl-definitions.h"
#include "../oepl-proto.h"

// #define DEBUG_MODE

// static const uint64_t __code __at(0x008b) mVersionRom = 0x1000011300000000ull;

#define TAG_MODE_CHANSEARCH 0
#define TAG_MODE_ASSOCIATED 1

uint8_t currentTagMode = TAG_MODE_CHANSEARCH;

uint8_t __xdata slideShowCurrentImg = 0;
uint8_t __xdata slideShowRefreshCount = 1;

extern uint8_t *__idata blockp;
extern uint8_t blockbuffer[];

uint8_t *rebootP;
#ifdef ENABLE_EEPROM_LOADER
extern bool __idata serialBypassActive;
bool __xdata serialActive = false;
void processSerial(uint8_t lastchar) {
    static uint8_t __xdata cmdbuffer[4];
    // shift characters in
    for (uint8_t c = 0; c < 3; c++) {
        cmdbuffer[c] = cmdbuffer[c + 1];
    }
    cmdbuffer[3] = lastchar;

    if (strncmp(cmdbuffer + 1, ">D>", 3) == 0) {
        wdt120s();
        blockp = blockbuffer;
        serialBypassActive = true;
        pr("ACK>\n");
        while (serialBypassActive)
            ;
        if (validateBlockData()) {
            pr("ACK>\n");
        } else {
            pr("NOK>\n");
        }
    }

    if (strncmp(cmdbuffer + 1, "<D<", 3) == 0) {
        wdt120s();
        pr("ACK>");
        for (uint16_t c = 0; c < 4100; c++) {
            uartTx(blockbuffer[c]);
            timerDelay(TIMER_TICKS_PER_MS / 400);  // 30 okay // 50 kinda okay // 80 ook okay?
        }
        pr("blaat");
    }

    if (strncmp(cmdbuffer, "STE", 3) == 0) {  // store block to offset
        if (!eepromErase(4096UL * cmdbuffer[3], 4096 / EEPROM_ERZ_SECTOR_SZ)) {
            pr("NOK>\n");
            return;
        }
        if (eepromWrite(4096UL * cmdbuffer[3], blockbuffer + 4, 4096))
            pr("ACK>\n");
        else
            pr("NOK>\n");
    }
    if (strncmp(cmdbuffer, "LDE", 3) == 0) {  // load block from offset
        eepromRead(4096UL * cmdbuffer[3], blockbuffer + 4, 4096);
        uint16_t *header = blockbuffer;
        *header = 4096;
        uint16_t *sum = blockbuffer + 2;
        *sum = 0;
        for (uint16_t c = 4; c < 4100; c++) {
            *sum += blockbuffer[c];
        }
        pr("ACK>\n");
    }
}
void serialTerminal() {
    serialActive = true;
    while (serialActive) {
        while (uartBytesAvail()) {
            processSerial(uartRx());
        }
    }
}
#endif

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

#ifdef WRITE_MAC_FROM_FLASH
void writeInfoPageWithMac() {
    uint8_t *settemp = blockbuffer + 2048;
    flashRead(FLASH_INFOPAGE_ADDR, (void *)blockbuffer, 1024);
    flashRead(0xFC06, (void *)(settemp + 8), 4);
    settemp[7] = 0x00;
    settemp[6] = 0x00;
    settemp[5] = settemp[8];
    settemp[4] = settemp[9];
    settemp[3] = settemp[10];
    settemp[2] = settemp[11];
#if (HW_TYPE == SOLUM_29_SSD1619)
    settemp[1] = 0x3B;
    settemp[0] = 0x10;
#endif
#if (HW_TYPE == SOLUM_M2_BWR_29_UC8151)
    settemp[1] = 0x3B;
    settemp[0] = 0x30;
#endif
#if (HW_TYPE == SOLUM_154_SSD1619)
    settemp[1] = 0x34;
    settemp[0] = 0x10;
#endif
#if (HW_TYPE == SOLUM_42_SSD1619)
    settemp[1] = 0x48;
    settemp[0] = 0x10;
#endif
#if (HW_TYPE == SOLUM_M2_BW_29_LOWTEMP)
    settemp[1] = 0x2D;
    settemp[0] = 0x10;
#endif
    uint8_t cksum = 0;
    for (uint8_t c = 0; c < 8; c++) {
        cksum ^= settemp[c];
        cksum ^= settemp[c] >> 4;
    }
    settemp[0] += cksum & 0x0F;

    memcpy((void *)(blockbuffer + 0x0010), (void *)settemp, 8);

    flashErase(FLASH_INFOPAGE_ADDR + 1);
    flashWrite(FLASH_INFOPAGE_ADDR, (void *)blockbuffer, 1024, false);
}
#endif

uint8_t channelSelect(uint8_t rounds) {  // returns 0 if no accesspoints were found
    powerUp(INIT_RADIO);
    uint8_t __xdata result[16];
    memset(result, 0, sizeof(result));

    for (uint8_t i = 0; i < rounds; i++) {
        for (uint8_t c = 0; c < sizeof(channelList); c++) {
            if (detectAP(channelList[c])) {
                if (mLastLqi > result[c]) result[c] = mLastLqi;
                if (rounds > 2) pr("Channel: %d - LQI: %d RSSI %d\n", channelList[c], mLastLqi, mLastRSSI);
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
#ifdef ENABLE_EEPROM_LOADER
            // run the eeprom loader interface
            powerUp(INIT_EPD | INIT_EEPROM);
            serialActive = true;
            serialTerminal();
#else
            // show splashscreen
            powerUp(INIT_EPD);
            afterFlashScreenSaver();
            while (1)
                ;
#endif
            break;
        case DETECT_P1_0_NOTHING:
            break;
        default:
            break;
    }
}

void TagAssociated() {
    // associated
    bool fastNextCheckin = false;
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
                uint8_t lut = getEepromImageDataArgument(curImgSlot) & 0x03;
                drawImageFromEeprom(curImgSlot, lut);
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

        switch (wakeUpReason) {
            case WAKEUP_REASON_BUTTON1:
                externalWakeHandler(CUSTOM_IMAGE_BUTTON1);
                fastNextCheckin = true;
                break;
            case WAKEUP_REASON_BUTTON2:
                externalWakeHandler(CUSTOM_IMAGE_BUTTON2);
                fastNextCheckin = true;
                break;
            case WAKEUP_REASON_GPIO:
                externalWakeHandler(CUSTOM_IMAGE_GPIO);
                fastNextCheckin = true;
                break;
            case WAKEUP_REASON_RF:
                externalWakeHandler(CUSTOM_IMAGE_RF_WAKE);
                fastNextCheckin = true;
                break;
            case WAKEUP_REASON_NFC:
                externalWakeHandler(CUSTOM_IMAGE_NFC_WAKE);
                fastNextCheckin = true;
                break;
        }

        if (avail != NULL) {
            // we got some data!
            longDataReqCounter = 0;
            // since we've had succesful contact, and communicated the wakeup reason succesfully, we can now reset to the 'normal' status
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
        uint8_t __xdata blaat[2] = {0xAB, 0xBA};
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
        doSleep(100UL);
    } else {
        if (nextCheckInFromAP) {
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
        doVoltageReading();
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

#ifndef LEAN_VERSION
void TagSlideShow() {
    currentChannel = 11;  // suppress the no-rf image thing
    displayCustomImage(CUSTOM_IMAGE_SPLASHSCREEN);

    // do a short channel search
    currentChannel = channelSelect(2);

    pr("Slideshow mode ch: %d\n", currentChannel);

    // if we did find an AP, check in once
    if (currentChannel) {
        doVoltageReading();
        struct AvailDataInfo *__xdata avail;
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
        tagSettings.enableRFWake = true;
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
        pr("wake...\n");
    }
}

void TagShowWaitRFWake() {
    pr("waiting for RF wake to start slideshow, now showing image\n");
    currentChannel = 11;  // suppress the no-rf image thing
    displayCustomImage(CUSTOM_IMAGE_SLIDESHOW);
    // powerDown(INIT_EEPROM | INIT_EPD);
    tagSettings.enableRFWake = 1;
    while (1) {
        doSleep(-1);
        if (wakeUpReason == WAKEUP_REASON_RF || wakeUpReason == WAKEUP_REASON_BUTTON1) {
            break;
        }
    }
    tagSettings.enableRFWake = 0;
    tagSettings.customMode = TAG_CUSTOM_SLIDESHOW_SLOW;
    powerUp(INIT_EEPROM);
    writeSettings();
    powerDown(INIT_EEPROM);
    wdtDeviceReset();
}
#endif

void executeCommand(uint8_t cmd) {
    switch (cmd) {
        case CMD_DO_REBOOT:
            wdtDeviceReset();
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
#ifndef LEAN_VERSION
        case CMD_ENTER_SLIDESHOW_FAST:
            powerUp(INIT_EEPROM);
            if (findSlotDataTypeArg(CUSTOM_IMAGE_SLIDESHOW << 3) == 0xFF) {
                powerDown(INIT_EEPROM);
                return;
            }
            tagSettings.customMode = TAG_CUSTOM_SLIDESHOW_FAST;
            writeSettings();
            powerDown(INIT_EEPROM);
            wdtDeviceReset();
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
            wdtDeviceReset();
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
            wdtDeviceReset();
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
            wdtDeviceReset();
            break;
        case CMD_ENTER_NORMAL_MODE:
            tagSettings.customMode = TAG_CUSTOM_MODE_NONE;
            powerUp(INIT_EEPROM);
            writeSettings();
            powerDown(INIT_EEPROM);
            wdtDeviceReset();
            break;
        case CMD_ENTER_WAIT_RFWAKE:
            tagSettings.customMode = TAG_CUSTOM_MODE_WAIT_RFWAKE;
            powerUp(INIT_EEPROM);
            writeSettings();
            powerDown(INIT_EEPROM);
            wdtDeviceReset();
            break;
#endif
    }
}

void main() {
    // displayLoop();  // remove me
    setupPortsInitial();
    powerUp(INIT_BASE | INIT_UART);
    pr("BOOTED>  %d.%d.%d%s\n", fwVersion / 100, (fwVersion % 100) / 10, (fwVersion % 10), fwVersionSuffix);

    // Find the reason why we're booting; is this a WDT?
    wakeUpReason = getFirstWakeUpReason();

    // dump(blockbuffer, 1024);
    //  get our own mac address. this is stored in Infopage at offset 0x10-onwards
    boardGetOwnMac(mSelfMac);
    pr("MAC>%02X%02X", mSelfMac[0], mSelfMac[1]);
    pr("%02X%02X", mSelfMac[2], mSelfMac[3]);
    pr("%02X%02X", mSelfMac[4], mSelfMac[5]);
    pr("%02X%02X\n", mSelfMac[6], mSelfMac[7]);

    // do a little sleep, this prevents a partial boot during battery insertion
    doSleep(200UL);
    powerUp(INIT_EEPROM);
    // load settings from infopage
    loadSettings();
    // invalidate the settings, and write them back in a later state
    invalidateSettingsEEPROM();

#ifdef WRITE_MAC_FROM_FLASH
    if (mSelfMac[7] == 0xFF && mSelfMac[6] == 0xFF) {
        wdt10s();
        timerDelay(TIMER_TICKS_PER_SECOND * 2);
        writeInfoPageWithMac();
        for (uint16_t c = 0xE800; c != 0; c += 1024) {
            flashErase(c);
        }
        boardGetOwnMac(mSelfMac);
        powerUp(INIT_UART | INIT_EEPROM | INIT_RADIO);
        wdt120s();
        powerDown(INIT_EEPROM | INIT_RADIO);

        wdt120s();
        powerUp(INIT_EPD);
        afterFlashScreenSaver();
        powerDown(INIT_EPD | INIT_UART);
        while (1) {
            doSleep(-1);
        }
    }
#endif

    // get the highest slot number, number of slots
    initializeProto();
    powerDown(INIT_EEPROM);

    // detect button or jig
    detectButtonOrJig();

#ifndef LEAN_VERSION
    switch (tagSettings.customMode) {
        case TAG_CUSTOM_MODE_WAIT_RFWAKE:
            TagShowWaitRFWake();
            break;
        case TAG_CUSTOM_SLIDESHOW_FAST:
        case TAG_CUSTOM_SLIDESHOW_MEDIUM:
        case TAG_CUSTOM_SLIDESHOW_SLOW:
        case TAG_CUSTOM_SLIDESHOW_GLACIAL:
            TagSlideShow();
            break;
        default:
            break;
    }
#endif

    if (tagSettings.enableFastBoot) {
        // Fastboot
        pr("Doing fast boot\n");
        capabilities = tagSettings.fastBootCapabilities;
        if (tagSettings.fixedChannel) {
            currentChannel = tagSettings.fixedChannel;
        } else {
            currentChannel = channelSelect(2);
        }
    } else {
        // Normal boot/startup
        pr("Normal boot\n");
        // validate the mac address; this will display a warning on the screen if the mac address is invalid
        validateMacAddress();

#if (NFC_TYPE == 1)
        // initialize I2C
        checkI2C();
#endif

        // Get a voltage reading on the tag, loading down the battery with the radio
        doVoltageReading();

        // show the splashscreen
        currentChannel = 11;
        showSplashScreen();
        currentChannel = 0;

// we've now displayed something on the screen; for the SSD1619, we are now aware of the lut-size
#ifdef EPD_SSD1619
        capabilities |= CAPABILITY_SUPPORTS_CUSTOM_LUTS;
        if (dispLutSize != 7) {
            capabilities |= CAPABILITY_ALT_LUT_SIZE;
        }
#endif
        tagSettings.fastBootCapabilities = capabilities;

        // now that we've collected all possible capabilities, save it to settings

        // scan for channels
        wdt30s();
        if (tagSettings.fixedChannel) {
            currentChannel = tagSettings.fixedChannel;
        } else {
            currentChannel = channelSelect(4);
        }
    }
    // end of the fastboot option split

    wdt10s();
    if (currentChannel) {
        showAPFound();

        // write the settings to the eeprom
        powerUp(INIT_EEPROM);
        writeSettings();
        powerDown(INIT_EEPROM);

        initPowerSaving(INTERVAL_BASE);
        currentTagMode = TAG_MODE_ASSOCIATED;
        doSleep(5000UL);
    } else {
        showNoAP();

        // write the settings to the eeprom
        powerUp(INIT_EEPROM);
        writeSettings();
        powerDown(INIT_EEPROM);

        initPowerSaving(INTERVAL_AT_MAX_ATTEMPTS);
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
