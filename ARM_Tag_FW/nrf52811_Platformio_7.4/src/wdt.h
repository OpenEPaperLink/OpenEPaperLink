
#pragma once

#include "Arduino.h"

void watchdog_enable(int timeout);

#define wdt10s()                \
    do                          \
    {                           \
        watchdog_enable(10000); \
    } while (0)

#define wdt30s()                \
    do                          \
    {                           \
        watchdog_enable(30000); \
    } while (0)

#define wdt60s()                \
    do                          \
    {                           \
        watchdog_enable(60000); \
    } while (0)

#define wdt120s()                \
    do                           \
    {                            \
        watchdog_enable(120000); \
    } while (0)
