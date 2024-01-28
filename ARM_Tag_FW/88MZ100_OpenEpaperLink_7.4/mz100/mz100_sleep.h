#pragma once

//#include <stdio.h>
#include <stdint.h>
#include "core_cm3.h"           
#include "mz100_gpio.h"
#include "mz100_interrupt.h"
#include "mz100_rtc.h"
#include "mz100_pinmux.h"
#include "mz100_uart.h"
#include "mz100_wdt.h"

void Something_RTC1(int a1);
void Something_RTC2(unsigned int result, int a2);
void sleep_with_with_wakeup(uint32_t sleep_time_ms);