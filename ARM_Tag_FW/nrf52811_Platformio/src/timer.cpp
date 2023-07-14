#include <Arduino.h>
#include "main.h"
#include "timer.h"

#define LF_FREQUENCY 32768UL
#define SECONDS(x) ((uint32_t)((LF_FREQUENCY * (float)((float)x / (float)1000)) + 0.5))
uint32_t wakeUpSeconds = 500;

bool RTC0_running = 0;

void initRTC0(uint32_t ms)
{
    if (RTC0_running == 0)
    {
        RTC0_running = 1;
        NVIC_ClearPendingIRQ(RTC0_IRQn);
        NVIC_SetPriority(RTC0_IRQn, 15);
        NVIC_EnableIRQ(RTC0_IRQn);
        NRF_RTC0->PRESCALER = 0;
        NRF_RTC0->INTENSET = RTC_EVTENSET_COMPARE0_Enabled << RTC_EVTENSET_COMPARE0_Pos;
        NRF_RTC0->EVTENSET = RTC_INTENSET_COMPARE0_Enabled << RTC_INTENSET_COMPARE0_Pos;
    }
    NVIC_DisableIRQ(RTC0_IRQn);
    wakeUpSeconds = ms;
    NRF_RTC0->CC[0] = NRF_RTC0->COUNTER + SECONDS(wakeUpSeconds);
    NVIC_EnableIRQ(RTC0_IRQn);
    NRF_RTC0->TASKS_START = 1;
}

#ifdef __cplusplus
extern "C"
{
#endif
    void RTC0_IRQHandler(void)
    {
        volatile uint32_t dummy;
        if (NRF_RTC0->EVENTS_COMPARE[0] == 1)
        {
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
