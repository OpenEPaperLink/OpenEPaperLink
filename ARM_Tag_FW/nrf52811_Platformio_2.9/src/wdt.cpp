
#include "wdt.h"

void watchdog_enable(int timeout)
{
    NRF_WDT->CONFIG = NRF_WDT->CONFIG = (WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos) | (WDT_CONFIG_SLEEP_Pause << WDT_CONFIG_SLEEP_Pos);
    NRF_WDT->CRV = (32768 * timeout) / 1000;
    NRF_WDT->RREN |= WDT_RREN_RR0_Msk;
    NRF_WDT->TASKS_START = 1;
    NRF_WDT->RR[0] = WDT_RR_RR_Reload;
}
