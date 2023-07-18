#include "powermgt.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "sleep.h"
#include "zigbee.h"
#include "eeprom.h"
#include "board.h"
#include "screen.h"
#include "uc8179.h"
#include "userinterface.h"
#include "wdt.h"
#include "syncedproto.h"

uint16_t dataReqAttemptArr[POWER_SAVING_SMOOTHING] = {0}; // Holds the amount of attempts required per data_req/check-in
uint8_t dataReqAttemptArrayIndex = 0;
uint8_t dataReqLastAttempt = 0;
uint16_t nextCheckInFromAP = 0;
uint8_t wakeUpReason = 0;
uint8_t scanAttempts = 0;

int8_t temperature = 0;
uint16_t batteryVoltage = 0;
bool lowBattery = false;
uint16_t longDataReqCounter = 0;
uint16_t voltageCheckCounter = 0;

uint8_t capabilities = 0;

bool spiActive = false;
bool uartActive = false;
bool eepromActive = false;
bool i2cActive = false;
extern int8_t adcSampleTemperature(void); // in degrees C

uint8_t checkButtonOrJig()
{
    return DETECT_P1_0_NOTHING;
}

void setupPortsInitial()
{
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, HIGH);
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    pinMode(BUTTON1, INPUT_PULLUP);
    pinMode(BUTTON2, INPUT_PULLUP);

    pinMode(FLASH_MISO, INPUT);
    pinMode(FLASH_CLK, OUTPUT);
    pinMode(FLASH_MOSI, OUTPUT);
    digitalWrite(FLASH_CS, HIGH);
    pinMode(FLASH_CS, OUTPUT);
}

void initPowerSaving(const uint16_t initialValue)
{
    for (uint8_t c = 0; c < POWER_SAVING_SMOOTHING; c++)
    {
        dataReqAttemptArr[c] = initialValue;
    }
}

static void configSPI(const bool setup)
{

    spiActive = setup;
}

static void configUART(const bool setup)
{
    /* if (setup == uartActive)
         return;
     uartActive = setup;
     if (setup)
         Serial.begin(115200);
     else
         Serial.end();*/
}

static void configEEPROM(const bool setup)
{
    if (setup == eepromActive)
        return;
    if (setup)
    {
        if (!eepromInit())
        {
            printf("Eeprom init error\r\n");
            powerDown(INIT_RADIO);
            powerUp(INIT_EPD);
            showNoEEPROM();
            powerDown(INIT_EEPROM | INIT_EPD);
            doSleep(-1);
            NVIC_SystemReset();
        }
    }
    else
    {
        digitalWrite(FLASH_CS, HIGH);
    }
}

static void configI2C(const bool setup)
{
    if (setup == i2cActive)
        return;
    if (setup)
    {
        /*P1DIR &= ~(1 << 6);
        P1_6 = 1;
        P1FUNC |= (1 << 4) | (1 << 5);
        P1PULL |= (1 << 4) | (1 << 5);
        i2cInit();
        i2cCheckDevice(0x50); */
        // first transaction after init fails, this makes sure everything is ready for the first transaction
    }
    else
    {
        /*P1DIR |= (1 << 6);
        P1_6 = 0;
        P1FUNC &= ~((1 << 4) | (1 << 5));
        P1PULL &= ~((1 << 4) | (1 << 5));
        CLKEN &= ~0x10;
        IEN1 &= ~4;*/
    }
    i2cActive = setup;
}

void powerUp(const uint8_t parts)
{
    if (parts & INIT_BASE)
    {
        // clockingAndIntsInit();
        // timerInit();
        // irqsOn();
        wdt30s();
    }

    if (parts & INIT_EPD)
    {
        configSPI(true);
        epdConfigGPIO(true);
        epdSetup();
    }

    if (parts & INIT_EPD_VOLTREADING)
    {
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

    if (parts & INIT_UART)
    {
        configUART(true);
    }

    if (parts & INIT_EEPROM)
    {
        configSPI(true);
        configEEPROM(true);
    }

    if (parts & INIT_TEMPREADING)
    {
        // temperature = adcSampleTemperature();
    }

    if (parts & INIT_RADIO)
    {
        radioInit();
        // radioRxFilterCfg(mSelfMac, 0x10000, PROTO_PAN_ID);
        // radioSetTxPower(10);
        if (currentChannel >= 11 && currentChannel <= 27)
        {
            radioSetChannel(currentChannel);
        }
        else
        {
            radioSetChannel(RADIO_FIRST_CHANNEL);
        }
    }
    if (parts & INIT_I2C)
    {
        configI2C(true);
    }
}

void powerDown(const uint8_t parts)
{
    if (parts & INIT_UART)
    {
        configUART(false);
    }
    if (parts & INIT_RADIO)
    {
        radioRxEnable(false);
    }
    if (parts & INIT_EEPROM)
    {
        eepromDeepPowerDown();
        eepromPrvDeselect();
        configEEPROM(false);
    }
    if (parts & INIT_EPD)
    {
        epdConfigGPIO(true);
        epdEnterSleep();
        epdConfigGPIO(false);
    }
    if (!eepromActive && !epdGPIOActive)
    {
        configSPI(false);
    }
    if (parts & INIT_I2C)
    {
        configI2C(false);
    }
}

void doSleep(const uint32_t t)
{
    printf("Sleeping for: %d ms\r\n", t);
    sleepForMs(t);
}

uint32_t getNextScanSleep(const bool increment)
{
    if (increment)
    {
        if (scanAttempts < 255)
            scanAttempts++;
    }

    if (scanAttempts < INTERVAL_1_ATTEMPTS)
    {
        return INTERVAL_1_TIME;
    }
    else if (scanAttempts < (INTERVAL_1_ATTEMPTS + INTERVAL_2_ATTEMPTS))
    {
        return INTERVAL_2_TIME;
    }
    else
    {
        return INTERVAL_3_TIME;
    }
}

void addAverageValue()
{
    uint16_t curval = INTERVAL_AT_MAX_ATTEMPTS - INTERVAL_BASE;
    curval *= dataReqLastAttempt;
    curval /= DATA_REQ_MAX_ATTEMPTS;
    curval += INTERVAL_BASE;
    dataReqAttemptArr[dataReqAttemptArrayIndex % POWER_SAVING_SMOOTHING] = curval;
    dataReqAttemptArrayIndex++;
}

uint16_t getNextSleep()
{
    uint16_t avg = 0;
    for (uint8_t c = 0; c < POWER_SAVING_SMOOTHING; c++)
    {
        avg += dataReqAttemptArr[c];
    }
    avg /= POWER_SAVING_SMOOTHING;
    return avg;
}