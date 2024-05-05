#include "powermgt.h"

#include "main.h"
#include "drivers/drv_pm.h"
#include "tl_common.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "zigbee.h"
#include "eeprom.h"
#include "board.h"
#include "screen.h"
#include "wdt.h"
#include "syncedproto.h"
#include "led.h"

uint16_t dataReqAttemptArr[POWER_SAVING_SMOOTHING] = {0}; // Holds the amount of attempts required per data_req/check-in
uint8_t dataReqAttemptArrayIndex = 0;
uint8_t dataReqLastAttempt = 0;
uint16_t nextCheckInFromAP = 0;
RAM uint8_t wakeUpReason = WAKEUP_REASON_FIRSTBOOT;
uint8_t scanAttempts = 0;

int8_t temperature = 0;
uint16_t batteryVoltage = 0;
bool lowBattery = false;
uint16_t longDataReqCounter = 0;
uint16_t voltageCheckCounter = 0;

uint8_t capabilities = 0;

RAM uint64_t time_ms = 0;
RAM uint32_t time_overflow = 0;

uint32_t getMillis()
{
    return pm_get_32k_tick() / 32; //(float)32.768;
}

void initPowerSaving(const uint16_t initialValue)
{
    for (uint8_t c = 0; c < POWER_SAVING_SMOOTHING; c++)
    {
        dataReqAttemptArr[c] = initialValue;
    }
}

void doSleepGpio(uint32_t t, GPIO_PinTypeDef pin)
{
    set_led_color(0); // Always turn of the LED before sleep for security reasons
    printf("Sleeping for: %u ms\r\n", (unsigned int)t);
    // WaitMs(2000);
    //  return;
    uint32_t r = drv_disable_irq();
    //pm_wakeup_pad_cfg(pin, !drv_gpio_read(pin), 1);
    drv_pm_longSleep(PM_SLEEP_MODE_SUSPEND, PM_WAKEUP_SRC_TIMER | PM_WAKEUP_SRC_PAD, t);
    //pm_wakeup_pad_cfg(pin, !drv_gpio_read(pin), 0);
    drv_restore_irq(r);
    uart_ndma_clear_tx_index(); // UART will be garbled otherwise
}

void doSleep(uint32_t t)
{
    uint8_t randomizer = drv_u32Rand() & 0xff;
    t += randomizer;
    set_led_color(0); // Always turn of the LED before sleep for security reasons
    printf("Sleeping for: %u ms\r\n", (unsigned int)t);
    // WaitMs(2000);
    //  return;
    uint32_t r = drv_disable_irq();
    pm_wakeup_pad_cfg(NFC_IRQ, PM_WAKEUP_LEVEL_LOW, 1);
    drv_pm_longSleep(PM_SLEEP_MODE_DEEP_WITH_RETENTION, PM_WAKEUP_SRC_TIMER | PM_WAKEUP_SRC_PAD, t);
    pm_wakeup_pad_cfg(NFC_IRQ, PM_WAKEUP_LEVEL_LOW, 0);
    drv_restore_irq(r);
    uart_ndma_clear_tx_index(); // UART will be garbled otherwise
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