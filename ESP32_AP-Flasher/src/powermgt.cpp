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
#ifdef POWER_HIGH_SIDE_DRIVER
        digitalWrite(pin[c], !state);
#else
        digitalWrite(pin[c], state);
#endif
    }
    for (uint8_t c = 0; c < pincount; c++) {
        pinMode(pin[c], OUTPUT);
    }
}

// On the OpenEPaperLink board, there is no in-rush current limiting. The tags that can be connected to the board can have significant capacity, which,
// when drained if the board applies power, will cause the 3v3 rail to sag enough to reset the ESP32. This is obviously not great. To prevent this from happening,
// we ramp up/down the voltage with PWM. Ramping down really is unnecessary, as the board has a resistor to dump the charge into.
void rampTagPower(uint8_t* pin, bool up) {
#ifdef OPENEPAPERLINK_PCB
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
#endif
    if (up) {
        ledcSetup(0, 152000, 8);  // 141251 okay // 101251 okay
        ledcWrite(0, 254);
        vTaskDelay(1 / portTICK_PERIOD_MS);
        ledcAttachPin(pin[0], 0);
        pinMode(pin[0], OUTPUT);
        vTaskDelay(10 / portTICK_PERIOD_MS);
        for (uint8_t c = 254; c != 0xFF; c--) {
            ledcWrite(0, c);
            if (c > 250) {
                vTaskDelay(2 / portTICK_PERIOD_MS);
            } else {
                delayMicroseconds(100);
            }
        }
        digitalWrite(pin[0], LOW);
        ledcDetachPin(pin[0]);
        digitalWrite(pin[0], LOW);
    } else {
        ledcSetup(0, 152000, 8);  // 141251 okay // 101251 okay
        ledcWrite(0, 0);
        vTaskDelay(1 / portTICK_PERIOD_MS);
        ledcAttachPin(pin[0], 0);
        pinMode(pin[0], OUTPUT);
        vTaskDelay(10 / portTICK_PERIOD_MS);
        for (uint8_t c = 0; c < 0xFF; c++) {
            ledcWrite(0, c);
            if (c > 250) {
                vTaskDelay(2 / portTICK_PERIOD_MS);
            } else {
                delayMicroseconds(100);
            }
        }
        digitalWrite(pin[0], HIGH);
        ledcDetachPin(pin[0]);
        digitalWrite(pin[0], HIGH);
    }
#ifdef OPENEPAPERLINK_PCB
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 1);
#endif
}

void powerControl(bool powerState, uint8_t* pin, uint8_t pincount) {
    if (pincount == 0) return;
    if (pin == nullptr) return;

#ifdef POWER_RAMPING
    if (powerState == true) {
#ifdef POWER_HIGH_SIDE_DRIVER
        rampTagPower(pin, true);
#else
        rampTagPower(pin, false);
#endif
    } else {
        pinMode(pin[0], OUTPUT);
#ifdef POWER_HIGH_SIDE_DRIVER
        digitalWrite(pin[0], HIGH);
#else
        digitalWrite(pin[0], LOW);
#endif
    }
#else
    simpleAPPower(pin, pincount, false);
    delay(500);
    simpleAPPower(pin, pincount, true);
#endif
}