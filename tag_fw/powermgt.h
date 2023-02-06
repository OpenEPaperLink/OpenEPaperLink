#ifndef _POWERMGT_H_
#define _POWERMGT_H_
#include <stdint.h>

#define WAKEUP_REASON_TIMED 0
#define WAKEUP_REASON_BOOTUP 1
#define WAKEUP_REASON_GPIO 2
#define WAKEUP_REASON_NFC 3

#define INIT_EPD_VOLTREADING 0x80
#define INIT_RADIO 0x40
#define INIT_GPIO 0x20
#define INIT_UART 0x10
#define INIT_EPD 0x08
#define INIT_EEPROM 0x04
#define INIT_TEMPREADING 0x02
#define INIT_BASE 0x01

#define wdt10s()                    \
    do {                            \
        wdtSetResetVal(0xFFF68A1F); \
    } while (0)

#define wdt30s()                    \
    do {                            \
        wdtSetResetVal(0xFFE39E5F); \
    } while (0)

#define wdt60s()                    \
    do {                            \
        wdtSetResetVal(0xFFC73CBF); \
    } while (0)

// power saving algorithm
#define INTERVAL_BASE 40              // interval (in seconds) (when 1 packet is sent/received) for target current (7.2µA)
#define INTERVAL_AT_MAX_ATTEMPTS 600  // interval (in seconds) (at max attempts) for target average current
#define DATA_REQ_RX_WINDOW_SIZE 5UL   // How many milliseconds we should wait for a packet during the data_request.
                                      // If the AP holds a long list of data for tags, it may need a little more time to lookup the mac address
#define DATA_REQ_MAX_ATTEMPTS 14      // How many attempts (at most) we should do to get something back from the AP
#define POWER_SAVING_SMOOTHING 8      // How many samples we should use to smooth the data request interval
#define MINIMUM_INTERVAL 45           // IMPORTANT: Minimum interval for check-in; this determines overal battery life!
#define MAXIMUM_PING_ATTEMPTS 20      // How many attempts to discover an AP the tag should do
#define PING_REPLY_WINDOW 2UL

#define LONG_DATAREQ_INTERVAL 300   // How often (in seconds, approximately) the tag should do a long datareq (including temperature)
#define VOLTAGE_CHECK_INTERVAL 288  // How often the tag should do a battery voltage check (multiplied by LONG_DATAREQ_INTERVAL)

// power saving when no AP's were found (scanning every X)
#define INTERVAL_1_TIME 1800UL   // Try every half hour
#define INTERVAL_1_ATTEMPTS 48   // for 48 attempts (an entire day)
#define INTERVAL_2_TIME 3600UL   // Try every hour
#define INTERVAL_2_ATTEMPTS 24   // for 24 attempts (an additional day)
#define INTERVAL_3_TIME 86400UL  // Try every day

extern void powerUp(uint8_t parts);
extern void powerDown(uint8_t parts);

extern void initAfterWake();
extern void doSleep(uint32_t __xdata t);

extern void addAverageValue();
extern uint16_t getNextSleep();

extern uint32_t getNextScanSleep(bool increment);
extern void initPowerSaving(uint16_t initialValue);

extern uint8_t __xdata wakeUpReason;

extern uint16_t __xdata nextCheckInFromAP;
extern uint8_t __xdata dataReqLastAttempt;
extern int8_t __xdata temperature;
extern uint16_t __xdata batteryVoltage;
extern uint8_t __xdata scanAttempts;
extern uint16_t __xdata longDataReqCounter;
extern uint16_t __xdata voltageCheckCounter;

#endif