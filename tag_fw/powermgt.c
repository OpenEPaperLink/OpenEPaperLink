#include "powermgt.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "asmUtil.h"
#include "board.h"
#include "comms.h"
#include "cpu.h"
#include "drawing.h"
#include "eeprom.h"
#include "epd.h"
#include "i2c.h"
#include "printf.h"
#include "proto.h"
#include "radio.h"
#include "settings.h"
#include "sleep.h"
#include "syncedproto.h"
#include "timer.h"
#include "userinterface.h"
#include "wdt.h"

uint16_t __xdata dataReqAttemptArr[POWER_SAVING_SMOOTHING] = {0};  // Holds the amount of attempts required per data_req/check-in
uint8_t __xdata dataReqAttemptArrayIndex = 0;
uint8_t __xdata dataReqLastAttempt = 0;
uint16_t __xdata nextCheckInFromAP = 0;
uint8_t __xdata wakeUpReason = 0;

void initPowerSaving() {
    for (uint8_t c = 0; c < POWER_SAVING_SMOOTHING; c++) {
        dataReqAttemptArr[c] = INTERVAL_BASE;
    }
}

// init/sleep
void initAfterWake() {
    clockingAndIntsInit();
    timerInit();
    // partialInit();
    boardInit();
    epdEnterSleep();
    irqsOn();
    boardInitStage2();
    initRadio();
}
void doSleep(uint32_t __xdata t) {
    if (t > 1000) pr("s=%lu\n ", t / 1000);
    powerPortsDownForSleep();

#ifdef HAS_BUTTON
    // Button setup on TEST pin 1.0 (input pullup)
    P1FUNC &= ~(1 << 0);
    P1DIR |= (1 << 0);
    P1PULL |= (1 << 0);
    P1LVLSEL |= (1 << 0);
    P1INTEN = (1 << 0);
    P1CHSTA &= ~(1 << 0);
#endif

    // sleepy
    sleepForMsec(t);
    wakeUpReason = WAKEUP_REASON_TIMED;

#ifdef HAS_BUTTON
    P1INTEN = 0;
    if (P1CHSTA && (1 << 0)) {
        wakeUpReason = WAKEUP_REASON_GPIO;
        pr("button pressed\n");
        P1CHSTA &= ~(1 << 0);
    }
#endif

    initAfterWake();
}
uint16_t getNextSleep() {
    uint16_t __xdata curval = INTERVAL_AT_MAX_ATTEMPTS - INTERVAL_BASE;
    curval *= dataReqLastAttempt;
    curval /= DATA_REQ_MAX_ATTEMPTS;
    curval += INTERVAL_BASE;
    dataReqAttemptArr[dataReqAttemptArrayIndex % POWER_SAVING_SMOOTHING] = curval;
    dataReqAttemptArrayIndex++;

    uint16_t avg = 0;
    bool noNetwork = true;
    for (uint8_t c = 0; c < POWER_SAVING_SMOOTHING; c++) {
        avg += dataReqAttemptArr[c];
        if (dataReqAttemptArr[c] != INTERVAL_AT_MAX_ATTEMPTS) {
            noNetwork = false;
        }
    }
    if (noNetwork == true) return INTERVAL_NO_SIGNAL;
    avg /= POWER_SAVING_SMOOTHING;
    return avg;
}