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
#include "i2c.h"
#include "i2cdevices.h"
#include "printf.h"
#include "proto.h"
#include "radio.h"
#include "screen.h"
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

bool __xdata spiActive = false;
bool __xdata uartActive = false;
bool __xdata eepromActive = false;
bool __xdata i2cActive = false;
extern int8_t adcSampleTemperature(void);  // in degrees C

void setupPortsInitial() {
    P0INTEN = 0;
    P1INTEN = 0;
    P2INTEN = 0;
    P0FUNC = 0;
    P1FUNC = 0;
    P2FUNC = 0;
    P0DIR = 0xFF;
    P1DIR = 0xFF;
    P2DIR = 0xFF;
    P0PULL = 0x00;
    P1PULL = 0x00;
    P2PULL = 0x00;
}

void initPowerSaving(const uint16_t initialValue) {
    for (uint8_t c = 0; c < POWER_SAVING_SMOOTHING; c++) {
        dataReqAttemptArr[c] = initialValue;
    }
}

static void configSPI(const bool setup) {
    if (setup == spiActive) return;
    if (setup) {
        P0FUNC |= (1 << 0) | (1 << 1) | (1 << 2);
        P0DIR |= (1 << 2);                // MISO as input
        P0DIR &= ~((1 << 0) | (1 << 1));  // CLK and MOSI as output
        P0PULL |= (1 << 2);
        spiInit();
        wdtOn();
    } else {
        P0FUNC &= ~((1 << 0) | (1 << 1) | (1 << 2));
        P0DIR |= (1 << 0) | (1 << 1) | (1 << 2);
        P0PULL &= ~(1 << 2);
        uint8_t bcp;
        CLKEN &= ~(0x08);
        bcp = CFGPAGE;
        CFGPAGE = 4;
        SPIENA &= ~(0x81);
        CFGPAGE = bcp;
    }
    spiActive = setup;
}

static void configUART(const bool setup) {
    if (uartActive == setup) return;
    if (setup) {
        P0FUNC |= (1 << 6);
        P0DIR &= ~(1 << 6);
        uartInit();
    } else {
        P0DIR |= (1 << 6);
        P0FUNC &= ~(1 << 6);
        CLKEN &= ~(0x20);
    }
    uartActive = setup;
}

static void configEEPROM(const bool setup) {
    if (setup == eepromActive) return;
    if (setup) {
        P1FUNC &= ~(1 << 1);
        P1DIR &= ~(1 << 1);
        if (!eepromInit()) {
            powerDown(INIT_RADIO);
            powerUp(INIT_EPD);
            showNoEEPROM();
            powerDown(INIT_EEPROM | INIT_EPD);
            doSleep(-1);
            wdtDeviceReset();
        }
    } else {
        P1DIR |= (1 << 1);
    }
    setup == eepromActive;  // wtf, this does nothing.
}

static void configI2C(const bool setup) {
    if (setup == i2cActive) return;
    if (setup) {
        P1DIR &= ~(1 << 6);
        P1_6 = 1;
        P1FUNC |= (1 << 4) | (1 << 5);
        P1PULL |= (1 << 4) | (1 << 5);
        i2cInit();
        i2cCheckDevice(0x50); // first transaction after init fails, this makes sure everything is ready for the first transaction
    } else {
        P1DIR |= (1 << 6);
        P1_6 = 0;
        P1FUNC &= ~((1 << 4) | (1 << 5));
        P1PULL &= ~((1 << 4) | (1 << 5));
        CLKEN &= ~0x10;
        IEN1 &= ~4;
    }
    i2cActive = setup;
}

void powerUp(const uint8_t parts) {
    if (parts & INIT_BASE) {
        clockingAndIntsInit();
        timerInit();
        irqsOn();
        wdtOn();
    }

    if (parts & INIT_EPD) {
        configSPI(true);
        epdConfigGPIO(true);
        epdSetup();
    }

    if (parts & INIT_EPD_VOLTREADING) {
        epdConfigGPIO(true);
        configSPI(true);
        batteryVoltage = epdGetBattery();
        if (batteryVoltage < BATTERY_VOLTAGE_MINIMUM) {
            lowBattery = true;
        } else {
            lowBattery = false;
        }
        configSPI(false);
        epdConfigGPIO(false);
    }

    if (parts & INIT_UART) {
        configUART(true);
    }

    if (parts & INIT_EEPROM) {
        configSPI(true);
        configEEPROM(true);
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
    if (parts & INIT_I2C) {
        configI2C(true);
    }
}

void powerDown(const uint8_t parts) {
    if (parts & INIT_UART) {
        configUART(false);
    }
    if (parts & INIT_RADIO) {  // warning; this also touches some stuff about the EEPROM, apparently. Re-init EEPROM afterwards
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
        configEEPROM(false);
    }
    if (parts & INIT_EPD) {
        epdConfigGPIO(true);
        epdEnterSleep();
        epdConfigGPIO(false);
    }
    if (!eepromActive && !epdGPIOActive) {
        configSPI(false);
    }
    if (parts & INIT_I2C) {
        configI2C(false);
    }
}

void doSleep(const uint32_t __xdata t) {
    // if (t > 1000) pr("s=%lu\n ", t / 1000);
    // powerPortsDownForSleep();

    // set up pins for spi(0.0,0.1,0.2), UART (0.6)
    // setup 1.1(eeprom_nCS), 1.2(eink_BS1), 1.7(eink_nCS)
    // setup 2.0(eink_nRST), 2.1(eink_BUSY), 2.2(eink_D/nC)

    P0FUNC = 0;
    P1FUNC = 0;
    P2FUNC = 0;

    P0DIR = 1;
    P0 = 0;
    P0PULL = 1;

    P1DIR = 0x86;
    P1PULL = 0x86;

    P2DIR = 7;
    P2 = 0;
    P2PULL = 5;

    spiActive = false;
    uartActive = false;
    eepromActive = false;

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
#ifdef HAS_BUTTON
    P1INTEN = 0;
    if (P1CHSTA && (1 << 0)) {
        wakeUpReason = WAKEUP_REASON_GPIO;
        P1CHSTA &= ~(1 << 0);
    }
#endif
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