#include "powermgt.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "asmUtil.h"
#include "board.h"
#include "comms.h"
#include "cpu.h"
#include "drawing.h"
#include "eeprom.h"
#include "epd.h"
#include "i2c.h"
#include "printf.h"
#include "proto.h"
#include "radio.h"
#include "settings.h"
#include "sleep.h"
#include "syncedproto.h"
#include "timer.h"
#include "uart.h"  // for initUart
#include "userinterface.h"
#include "wdt.h"

uint16_t __xdata dataReqAttemptArr[POWER_SAVING_SMOOTHING] = {0};  // Holds the amount of attempts required per data_req/check-in
uint8_t __xdata dataReqAttemptArrayIndex = 0;
uint8_t __xdata dataReqLastAttempt = 0;
uint16_t __xdata nextCheckInFromAP = 0;
uint8_t __xdata wakeUpReason = 0;
uint8_t __xdata scanAttempts = 0;

int8_t __xdata temperature = 0;
uint16_t __xdata batteryVoltage = 0;
bool __xdata lowBattery = false;
uint16_t __xdata longDataReqCounter = 0;
uint16_t __xdata voltageCheckCounter = 0;

extern int8_t adcSampleTemperature(void);  // in degrees C

void initPowerSaving(uint16_t initialValue) {
    for (uint8_t c = 0; c < POWER_SAVING_SMOOTHING; c++) {
        dataReqAttemptArr[c] = initialValue;
    }
}

void powerUp(uint8_t parts) {
    if (parts & INIT_BASE) {
        clockingAndIntsInit();
        timerInit();
        irqsOn();
        wdtOn();
        wdt10s();
    }

    if (parts & INIT_GPIO)
        boardInit();

    if (parts & INIT_EPD)
        epdSetup();

    if ((parts & INIT_BASE) && !(parts & INIT_EPD_VOLTREADING) && !(parts & INIT_EPD)) {
        if (!(parts & INIT_GPIO))
            boardInit();
        epdEnterSleep();  // this required fixing! halp halp fix me
    }

    if (parts & INIT_EPD_VOLTREADING) {
        if (!(parts & INIT_GPIO))
            boardInit();
        batteryVoltage = epdGetBattery();
        if(batteryVoltage<BATTERY_VOLTAGE_MINIMUM){
            lowBattery = true;
        } else {
            lowBattery = false;
        }
    }

    if (parts & INIT_UART) {
        uartInit();
    }

    if (parts & INIT_EEPROM) {
        if (!eepromInit()) {
            powerDown(INIT_RADIO);
            powerUp(INIT_EPD);
            showNoEEPROM();
            powerDown(INIT_EEPROM | INIT_EPD | INIT_GPIO);
            doSleep(-1);
            wdtDeviceReset();
        }
    }

    if (parts & INIT_TEMPREADING) {
        temperature = adcSampleTemperature();
    }

    if (parts & INIT_RADIO) {
        radioInit();
        radioRxFilterCfg(mSelfMac, 0x10000, PROTO_PAN_ID);
        radioSetTxPower(10);
        if (currentChannel >= 11 && currentChannel <= 25) {
            radioSetChannel(currentChannel);
        } else {
            radioSetChannel(RADIO_FIRST_CHANNEL);
        }
    }
}

void powerDown(uint8_t parts) {
    if (parts & INIT_RADIO) {
        radioRxEnable(false, true);
        RADIO_IRQ4_pending = 0;
        UNK_C1 &= ~0x81;
        TCON &= ~0x20;
        uint8_t __xdata cfgPg = CFGPAGE;
        CFGPAGE = 4;
        RADIO_command = 0xCA;
        RADIO_command = 0xC5;
        CFGPAGE = cfgPg;
    }
    if (parts & INIT_EEPROM) {
        eepromDeepPowerDown();
        eepromPrvDeselect();
    }
    if (parts & INIT_EPD)
        epdEnterSleep();

    if (parts & INIT_GPIO)
        powerPortsDownForSleep();
}

void doSleep(uint32_t __xdata t) {
    // if (t > 1000) pr("s=%lu\n ", t / 1000);
    // powerPortsDownForSleep();

#ifdef HAS_BUTTON
    // Button setup on TEST pin 1.0 (input pullup)
    P1FUNC &= ~(1 << 0);
    P1DIR |= (1 << 0);
    P1PULL |= (1 << 0);
    P1LVLSEL |= (1 << 0);
    P1INTEN = (1 << 0);
    P1CHSTA &= ~(1 << 0);
#endif

    // sleepy
    sleepForMsec(t);
    wakeUpReason = WAKEUP_REASON_TIMED;
#ifdef HAS_BUTTON
    P1INTEN = 0;
    if (P1CHSTA && (1 << 0)) {
        wakeUpReason = WAKEUP_REASON_GPIO;
        pr("button pressed\n");
        P1CHSTA &= ~(1 << 0);
    }
#endif
}

uint32_t getNextScanSleep(bool increment) {
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
    uint16_t __xdata curval = INTERVAL_AT_MAX_ATTEMPTS - INTERVAL_BASE;
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