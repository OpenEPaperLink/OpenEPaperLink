#include "powermgt.h"

#include <Arduino.h>

#include "settings.h"

#ifdef OPENEPAPERLINK_PCB
#include "soc/rtc_cntl_reg.h"
#include "soc/soc.h"
#endif

void simpleAPPower(uint8_t* pin, uint8_t pincount, bool state) {
    for (uint8_t c = 0; c < pincount; c++) {
        pinMode(pin[c], INPUT);
    }
    for (uint8_t c = 0; c < pincount; c++) {
        digitalWrite(pin[c], state);
    }
    for (uint8_t c = 0; c < pincount; c++) {
        pinMode(pin[c], OUTPUT);
    }
}

void powerControl(bool powerState, uint8_t* pin, uint8_t pincount) {
    if (pincount == 0) return;
    if (pin == nullptr) return;

    pincount = 4;
    simpleAPPower(pin, pincount, powerState);

}