#include "powermgt.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
// #include <stdio.h>
#include <string.h>

#include "board.h"
#include "eeprom.h"
#include "main.h"
#include "mz100_sleep.h"
#include "printf.h"
#include "screen.h"
#include "syncedproto.h"
#include "util.h"
#include "zigbee.h"

__attribute__((section(".aonshadow"))) uint16_t dataReqAttemptArr[POWER_SAVING_SMOOTHING] = {0};  // Holds the amount of attempts required per data_req/check-in
__attribute__((section(".aonshadow"))) uint8_t dataReqAttemptArrayIndex = 0;
__attribute__((section(".aonshadow"))) uint8_t dataReqLastAttempt = 0;
__attribute__((section(".aonshadow"))) uint16_t nextCheckInFromAP = 0;
__attribute__((section(".aonshadow"))) uint8_t wakeUpReason = 0;
__attribute__((section(".aonshadow"))) uint8_t scanAttempts = 0;

__attribute__((section(".aonshadow"))) int8_t temperature = 0;
__attribute__((section(".aonshadow"))) uint16_t batteryVoltage = 0;
__attribute__((section(".aonshadow"))) bool lowBattery = false;
__attribute__((section(".aonshadow"))) uint16_t longDataReqCounter = 0;
__attribute__((section(".aonshadow"))) uint16_t voltageCheckCounter = 0;

__attribute__((section(".aonshadow"))) uint8_t capabilities = 0;

bool spiActive = false;
bool uartActive = false;
bool eepromActive = false;
bool i2cActive = false;

extern int8_t adcSampleTemperature(void);  // in degrees C

uint8_t checkButtonOrJig() {
    return DETECT_P1_0_NOTHING;
}

void setupPortsInitial() {
}

uint16_t doVoltageReading() {
    batteryVoltage = (uint16_t)measureBattery();
    if (batteryVoltage < BATTERY_VOLTAGE_MINIMUM) {
        lowBattery = true;
    } else {
        lowBattery = false;
    }
    return batteryVoltage;
}

void initPowerSaving(const uint16_t initialValue) {
    for (uint8_t c = 0; c < POWER_SAVING_SMOOTHING; c++) {
        dataReqAttemptArr[c] = initialValue;
    }
}

static void configSPI(const bool setup) {
    spiActive = setup;
}

static void configUART(const bool setup) {
    /* if (setup == uartActive)
         return;
     uartActive = setup;
     if (setup)
         Serial.begin(115200);
     else
         Serial.end();*/
}

static void configEEPROM(const bool setup) {
}

static void configI2C(const bool setup) {
}

void powerUp(const uint8_t parts) {
    // printf("Power up: %d\r\n", parts);
    if (parts & INIT_RADIO) {
        radioInit();
        // radioRxFilterCfg(mSelfMac, 0x10000, PROTO_PAN_ID);
        // radioSetTxPower(10);
        if (currentChannel >= 11 && currentChannel <= 27) {
            radioSetChannel(currentChannel);
        } else {
            radioSetChannel(RADIO_FIRST_CHANNEL);
        }
    }
    if (parts & INIT_UART) {
        configUART(true);
    }
    if (parts & INIT_EPD) {
        configSPI(true);
    }
    if (parts & INIT_EEPROM) {
        configEEPROM(true);
    }
    if (parts & INIT_I2C) {
        configI2C(true);
    }
}

void powerDown(const uint8_t parts) {
    // printf("Power down: %d\r\n", parts);
}

void doSleep(const uint32_t t) {
    printf("Sleeping for: %d ms\r\n", t);
    // sleepForMs(t);
    delay(t);
}

uint32_t getNextScanSleep(const bool increment) {
    if (increment) {
        if (scanAttempts < 255)
            scanAttempts++;
    }

    if (scanAttempts < INTERVAL_1_ATTEMPTS) {
        return INTERVAL_1_TIME;
    } else if (scanAttempts < (INTERVAL_1_ATTEMPTS + INTERVAL_2_ATTEMPTS)) {
        return INTERVAL_2_TIME;
    } else {
        return INTERVAL_3_TIME;
    }
}

void addAverageValue() {
    uint16_t curval = INTERVAL_AT_MAX_ATTEMPTS - INTERVAL_BASE;
    curval *= dataReqLastAttempt;
    curval /= DATA_REQ_MAX_ATTEMPTS;
    curval += INTERVAL_BASE;
    dataReqAttemptArr[dataReqAttemptArrayIndex % POWER_SAVING_SMOOTHING] = curval;
    dataReqAttemptArrayIndex++;
}

uint16_t getNextSleep() {
    uint16_t avg = 0;
    for (uint8_t c = 0; c < POWER_SAVING_SMOOTHING; c++) {
        avg += dataReqAttemptArr[c];
    }
    avg /= POWER_SAVING_SMOOTHING;
    return avg;
}