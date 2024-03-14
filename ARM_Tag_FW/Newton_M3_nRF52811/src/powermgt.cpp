#include "powermgt.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "eeprom.h"
#include "syncedproto.h"
#include "hal.h"
#include "userinterface.h"
#include "wdt.h"
#include "epd_driver/epd_interface.h"

uint16_t dataReqAttemptArr[POWER_SAVING_SMOOTHING] = {0};  // Holds the amount of attempts required per data_req/check-in
uint8_t dataReqAttemptArrayIndex = 0;
uint8_t dataReqLastAttempt = 0;
uint16_t nextCheckInFromAP = 0;
uint8_t wakeUpReason = 0;
uint8_t scanAttempts = 0;

uint16_t longDataReqCounter = 0;
uint16_t voltageCheckCounter = 0;

uint8_t capabilities = 0;

bool spiActive = false;
bool uartActive = false;
bool eepromActive = false;
bool i2cActive = false;
extern int8_t adcSampleTemperature(void);  // in degrees C

uint8_t checkButtonOrJig() {
    return DETECT_P1_0_NOTHING;
}

void button1wake() {
    wakeUpReason = WAKEUP_REASON_BUTTON1;
    resettimer();
}

void button2wake() {
    wakeUpReason = WAKEUP_REASON_BUTTON2;
    resettimer();
}

void nfcwake() {
    wakeUpReason = WAKEUP_REASON_NFC;
    resettimer();
}

void setupPortsInitial() {
    if (tag.ledInverted) {
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_BLUE, LOW);
    } else {
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_GREEN, HIGH);
        digitalWrite(LED_BLUE, HIGH);
    }
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    switch (tag.boardType) {
        case NRF_BOARDTYPE_REGULAR:
            pinMode(BUTTON1, INPUT_PULLUP);
            pinMode(BUTTON2, INPUT_PULLUP);
            attachInterrupt(digitalPinToInterrupt(BUTTON1), button1wake, FALLING);
            attachInterrupt(digitalPinToInterrupt(BUTTON2), button2wake, FALLING);
            break;
        case NRF_BOARDTYPE_PEGHOOK:
            pinMode(PEGHOOK_BUTTON, INPUT_PULLUP);
            attachInterrupt(digitalPinToInterrupt(PEGHOOK_BUTTON), button1wake, FALLING);
            break;
    }

    pinMode(NFC_POWER, INPUT_PULLDOWN);
    pinMode(NFC_IRQ, INPUT_PULLDOWN);

    pinMode(EPD_POWER, OUTPUT);
    digitalWrite(EPD_POWER, LOW);

    pinMode(FLASH_MISO, INPUT);
    pinMode(FLASH_CLK, OUTPUT);
    pinMode(FLASH_MOSI, OUTPUT);
    digitalWrite(FLASH_CS, HIGH);
    pinMode(FLASH_CS, OUTPUT);

    // pinMode(EPD_VPP, OUTPUT);
    // digitalWrite(EPD_VPP, HIGH);
    // pinMode(EPD_HLT, OUTPUT);
    // digitalWrite(EPD_HLT, HIGH);

    attachInterrupt(digitalPinToInterrupt(NFC_IRQ), nfcwake, RISING);
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
    return;
    if (setup == uartActive)
        return;
    uartActive = setup;
    if (setup) {
        NRF_UART0->ENABLE = 4;
    } else {
        NRF_UART0->ENABLE = 0;
    }
}

static void configEEPROM(const bool setup) {
    if (setup == eepromActive)
        return;
    if (setup) {
        setupEepromHWSPI(true);
        if (!eepromInit()) {
            printf("Eeprom init error\r\n");
            powerDown(INIT_RADIO);
            powerUp(INIT_EPD);
            showNoEEPROM();
            powerDown(INIT_EEPROM | INIT_EPD);
            doSleep(-1);
            NVIC_SystemReset();
        }
    } else {
        setupEepromHWSPI(false);
    }
    eepromActive = setup;
}

static void configI2C(const bool setup) {
    if (setup == i2cActive)
        return;
    if (setup) {
        pinMode(NFC_I2C_SCL, OUTPUT);
        pinMode(NFC_I2C_SDA, OUTPUT);
    } else {
        pinMode(NFC_I2C_SDA, INPUT);
        pinMode(NFC_I2C_SCL, INPUT);
    }
    i2cActive = setup;
}

void powerUp(const uint8_t parts) {
    if (parts & INIT_BASE) {
        // clockingAndIntsInit();
        // timerInit();
        // irqsOn();
        wdt30s();
    }

    if (parts & INIT_EPD) {
        configSPI(true);
        epdConfigGPIO(true);
        epdSetup();
    }

    if (parts & INIT_VOLTREADING) {
        getVoltage();
        if (batteryVoltage < BATTERY_VOLTAGE_MINIMUM) {
            lowBattery = true;
        } else {
            lowBattery = false;
        }
    }

    if (parts & INIT_UART) {
        configUART(true);
    }

    if (parts & INIT_EEPROM) {
        configSPI(true);
        configEEPROM(true);
    }

    if (parts & INIT_TEMPREADING) {
        getTemperature();
    }

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
    if (parts & INIT_I2C) {
        configI2C(true);
    }
}

void powerDown(const uint8_t parts) {
    if (parts & INIT_UART) {
        configUART(false);
    }
    if (parts & INIT_RADIO) {
        radioRxEnable(false);
    }
    if (parts & INIT_EEPROM) {
        eepromDeepPowerDown();
        eepromPrvDeselect();
        configEEPROM(false);
    }
    if (parts & INIT_EPD) {
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

void doSleep(const uint32_t t) {
    // printf("Sleeping for: %lu ms\r\n", t);
    sleepForMs(t);
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