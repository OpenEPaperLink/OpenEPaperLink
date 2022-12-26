#define __packed
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "adc.h"
#include "asmUtil.h"
#include "board.h"
#include "chars.h"
#include "comms.h"
#include "cpu.h"
#include "drawing.h"
#include "eeprom.h"
#include "printf.h"
#include "proto.h"
#include "radio.h"
#include "screen.h"
#include "sleep.h"
#include "timer.h"
#include "wdt.h"
#include "syncedproto.h"


uint16_t __xdata battery_voltage = 0;
int8_t __xdata mCurTemperature;


void getVolt(void) {
    if (battery_voltage == 0)
        battery_voltage = adcSampleBattery();
}

void main(void){
    mainProtocolLoop();
}
