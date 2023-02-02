#ifndef _POWERMGT_H_
#define _POWERMGT_H_
#include <stdint.h>


#define WAKEUP_REASON_TIMED     0
#define WAKEUP_REASON_BOOTUP    1
#define WAKEUP_REASON_GPIO      2
#define WAKEUP_REASON_NFC       3


// power saving algorithm
#define INTERVAL_BASE 40              // interval (in seconds) (when 1 packet is sent/received) for target current (7.2µA)
#define INTERVAL_AT_MAX_ATTEMPTS 600  // interval (in seconds) (at max attempts) for target average current
#define INTERVAL_NO_SIGNAL 1800       // interval (in seconds) when no answer for POWER_SAVING_SMOOTHING attempts,
                                      // (INTERVAL_AT_MAX_ATTEMPTS * POWER_SAVING_SMOOTHING) seconds
#define DATA_REQ_RX_WINDOW_SIZE 5UL   // How many milliseconds we should wait for a packet during the data_request.
                                      // If the AP holds a long list of data for tags, it may need a little more time to lookup the mac address
#define DATA_REQ_MAX_ATTEMPTS 14      // How many attempts (at most) we should do to get something back from the AP
#define POWER_SAVING_SMOOTHING 8      // How many samples we should use to smooth the data request interval
#define MINIMUM_INTERVAL 45           // IMPORTANT: Minimum interval for check-in; this determines overal battery life!

#define MAXIMUM_PING_ATTEMPTS 20      // How many attempts to discover an AP the tag should do
#define PING_REPLY_WINDOW 2UL


extern void initAfterWake();
extern void doSleep(uint32_t __xdata t);
extern uint16_t getNextSleep();
extern void initPowerSaving();
extern uint8_t __xdata wakeUpReason;

extern uint16_t __xdata nextCheckInFromAP;
extern uint8_t __xdata dataReqLastAttempt;

#endif