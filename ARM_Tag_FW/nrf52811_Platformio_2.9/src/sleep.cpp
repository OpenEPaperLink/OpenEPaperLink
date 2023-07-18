#include <Arduino.h>
#include "main.h"
#include "sleep.h"
#include "timer.h"
#include "zigbee.h"

uint32_t sleepMsEntry = 0;
void sleepForMs(uint32_t ms)
{
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
    while (millis() - sleepMsEntry < ms)
    {
        __WFE();
        __SEV();
        __WFE();
    }
    Serial.begin(115200);
}
