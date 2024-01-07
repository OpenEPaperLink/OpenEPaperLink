
#include "wdt.h"
#include "../tl_common.h"

void watchdog_enable(int timeout)
{
    wd_stop();
    drv_wd_setInterval(timeout);
    drv_wd_start();
    drv_wd_clear();
}
