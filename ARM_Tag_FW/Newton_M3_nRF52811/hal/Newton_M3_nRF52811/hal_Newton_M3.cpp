#include <Arduino.h>

#include "hal.h"
#include "wdt.h"

int8_t startHFCLK(void) {
    if (!isHFCLKstable()) {
        NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
        NRF_CLOCK->TASKS_HFCLKSTART = 1U;

        while (!isHFCLKstable())
            ;

        return 0;
    } else {
        return -1;
    }
}

uint8_t isHFCLKstable(void) {
    uint32_t stable = (uint32_t)(CLOCK_HFCLKSTAT_STATE_Running << CLOCK_HFCLKSTAT_STATE_Pos) | (CLOCK_HFCLKSTAT_SRC_Xtal << CLOCK_HFCLKSTAT_SRC_Pos);

    if (NRF_CLOCK->HFCLKSTAT == stable) {
        NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;

        return 1;
    } else {
        return 0;
    }
}

void boardGetOwnMac(uint8_t *mac) {
    mac[0] = MAC_ID_1;
    mac[1] = MAC_ID_0;
    mac[2] = (NRF_UICR->CUSTOMER[0]) & 0xFF;
    mac[3] = (NRF_UICR->CUSTOMER[0] >> 8) & 0xFF;
    mac[4] = (NRF_UICR->CUSTOMER[0] >> 16) & 0xFF;
    mac[5] = (NRF_UICR->CUSTOMER[0] >> 24);
    mac[6] = 0;
    mac[7] = 0;
    uint8_t cksum = 0;
    for (uint8_t c = 0; c < 8; c++) {
        cksum ^= mac[c];
        cksum ^= mac[c] >> 4;
    }
    mac[0] += cksum & 0x0F;
}

void watchdog_enable(int timeout) {
    NRF_WDT->CONFIG = NRF_WDT->CONFIG = (WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos) | (WDT_CONFIG_SLEEP_Pause << WDT_CONFIG_SLEEP_Pos);
    NRF_WDT->CRV = (32768 * timeout) / 1000;
    NRF_WDT->RREN |= WDT_RREN_RR0_Msk;
    NRF_WDT->TASKS_START = 1;
    NRF_WDT->RR[0] = WDT_RR_RR_Reload;
}

uint32_t sleepMsEntry = 0;
void sleepForMs(uint32_t ms) {
    // Turn everything off for minimal deep sleep current
    radioRxEnable(0);
    NRF_CLOCK->TASKS_HFCLKSTOP = 1U;
    while ((NRF_CLOCK->HFCLKSTAT & 0x10001) == 0x10001)
        ;
    Serial.end();
    yield();
    // here we set the rtc to sleep for the time we want
    initRTC0(ms);
    sleepMsEntry = millis();
    while (millis() - sleepMsEntry < ms) {
        __WFE();
        __SEV();
        __WFE();
    }
    Serial.begin(115200);
}

#define LF_FREQUENCY 32768UL
#define SECONDS(x) ((uint32_t)((LF_FREQUENCY * (float)((float)x / (float)1000)) + 0.5))
uint32_t wakeUpSeconds = 500;

void initRTC0(uint32_t ms) {
    NVIC_DisableIRQ(RTC0_IRQn);
    NVIC_ClearPendingIRQ(RTC0_IRQn);
    NVIC_SetPriority(RTC0_IRQn, 15);
    NRF_RTC0->PRESCALER = 0;
    NRF_RTC0->INTENSET = RTC_EVTENSET_COMPARE0_Enabled << RTC_EVTENSET_COMPARE0_Pos;
    NRF_RTC0->EVTENSET = RTC_INTENSET_COMPARE0_Enabled << RTC_INTENSET_COMPARE0_Pos;
    wakeUpSeconds = ms;
    NRF_RTC0->CC[0] = NRF_RTC0->COUNTER + SECONDS(wakeUpSeconds);
    NVIC_EnableIRQ(RTC0_IRQn);
    NRF_RTC0->TASKS_START = 1;
}

#ifdef __cplusplus
extern "C" {
#endif
void RTC0_IRQHandler(void) {
    volatile uint32_t dummy;
    if (NRF_RTC0->EVENTS_COMPARE[0] == 1) {
        NRF_RTC0->EVENTS_COMPARE[0] = 0;
        NRF_RTC0->CC[0] = NRF_RTC0->COUNTER + SECONDS(wakeUpSeconds);
        dummy = NRF_RTC0->EVENTS_COMPARE[0];
        dummy;
        NRF_RTC0->TASKS_STOP = 1;
    }
}
#ifdef __cplusplus
}
#endif
