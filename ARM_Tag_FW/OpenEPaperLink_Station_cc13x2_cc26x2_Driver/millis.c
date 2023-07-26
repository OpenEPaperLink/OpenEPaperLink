#include "millis.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ti/drivers/Timer.h>
#include "ti_drivers_config.h"

Timer_Handle timer0;
Timer_Params params;
volatile uint64_t timeOverflows = 0;
void timerCallback(Timer_Handle myHandle, int_fast16_t status)
{
    timeOverflows++;
}

uint32_t getMillis()
{
    uint64_t ticks = ((uint64_t)((uint64_t)timeOverflows << (uint64_t)32) | (uint64_t)Timer_getCount(timer0));
    return (uint64_t)((uint64_t)ticks/(uint64_t)48000);
}

void millisInit()
{
    Timer_init();
    Timer_Params_init(&params);
    params.period        = 0xffffffff;
    params.periodUnits   = Timer_PERIOD_COUNTS;
    params.timerMode     = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = timerCallback;

    timer0 = Timer_open(CONFIG_TIMER_0, &params);
    if (timer0 != NULL)
    {
        Timer_start(timer0);
    }
}
