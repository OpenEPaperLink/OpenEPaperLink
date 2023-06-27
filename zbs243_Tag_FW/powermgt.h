#ifndef _POWERMGT_H_
#define _POWERMGT_H_
#include <stdint.h>

#define DETECT_P1_0_NOTHING 0
#define DETECT_P1_0_BUTTON 1
#define DETECT_P1_0_JIG 2

#define INIT_EPD_VOLTREADING 0x80
#define INIT_RADIO 0x40
#define INIT_I2C 0x20
#define INIT_UART 0x10
#define INIT_EPD 0x08
#define INIT_EEPROM 0x04
#define INIT_TEMPREADING 0x02
#define INIT_BASE 0x01

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

#define LONG_DATAREQ_INTERVAL 300     // How often (in seconds, approximately) the tag should do a long datareq (including temperature)
#define VOLTAGE_CHECK_INTERVAL 288    // How often the tag should do a battery voltage check (multiplied by LONG_DATAREQ_INTERVAL)
#define BATTERY_VOLTAGE_MINIMUM 2450  // 2600 or below is the best we can do on the EPD

// power saving when no AP's were found (scanning every X)
#define VOLTAGEREADING_DURING_SCAN_INTERVAL 2  // how often we should read voltages; this is done every scan attempt in interval bracket 3
#define INTERVAL_1_TIME 3600UL                 // Try every hour
#define INTERVAL_1_ATTEMPTS 24                 // for 24 attempts (an entire day)
#define INTERVAL_2_TIME 7200UL                 // Try every 2 hours
#define INTERVAL_2_ATTEMPTS 12                 // for 12 attempts (an additional day)
#define INTERVAL_3_TIME 86400UL                // Finally, try every day

extern uint8_t checkButtonOrJig();

extern void setupPortsInitial();

extern void powerUp(const uint8_t parts);
extern void powerDown(const uint8_t parts);

extern void initAfterWake();
extern void doSleep(const uint32_t __xdata t);

void doVoltageReading();

extern void addAverageValue();
extern uint16_t getNextSleep();

extern uint32_t getNextScanSleep(const bool increment);
extern void initPowerSaving(const uint16_t initialValue);

extern uint8_t __xdata wakeUpReason;

extern uint8_t __xdata capabilities;

extern uint16_t __xdata nextCheckInFromAP;
extern uint8_t __xdata dataReqLastAttempt;
extern int8_t __xdata temperature;
extern uint16_t __xdata batteryVoltage;
extern bool __xdata lowBattery;
extern uint8_t __xdata scanAttempts;
extern uint16_t __xdata longDataReqCounter;
extern uint16_t __xdata voltageCheckCounter;

#endif