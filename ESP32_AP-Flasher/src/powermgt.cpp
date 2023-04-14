#include "powermgt.h"

#include <Arduino.h>

#include "settings.h"

#ifdef OPENEPAPERLINK_PCB
#include "soc/rtc_cntl_reg.h"
#include "soc/soc.h"
#endif

#ifdef SIMPLE_AP
void simpleAPPower(bool state) {
    pinMode(FLASHER_AP_POWER, INPUT);
    pinMode(FLASHER_AP_POWER2, INPUT);
    digitalWrite(FLASHER_AP_POWER, state);
    digitalWrite(FLASHER_AP_POWER2, state);
    pinMode(FLASHER_AP_POWER, OUTPUT);
    pinMode(FLASHER_AP_POWER2, OUTPUT);
}
#endif

// On the OpenEPaperLink board, there is no in-rush current limiting. The tags that can be connected to the board can have significant capacity, which,
// when drained if the board applies power, will cause the 3v3 rail to sag enough to reset the ESP32. This is obviously not great. To prevent this from happening,
// we ramp up/down the voltage with PWM. Ramping down really is unnecessary, as the board has a resistor to dump the charge into.
void rampTagPower(uint8_t pin, bool up) {
#ifdef OPENEPAPERLINK_PCB
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
    if (up) {
        ledcSetup(0, 152000, 8);  // 141251 okay // 101251 okay
        ledcWrite(0, 254);
        vTaskDelay(1 / portTICK_PERIOD_MS);
        ledcAttachPin(pin, 0);
        pinMode(pin, OUTPUT);
        vTaskDelay(10 / portTICK_PERIOD_MS);
        for (uint8_t c = 254; c != 0xFF; c--) {
            ledcWrite(0, c);
            if (c > 250) {
                vTaskDelay(2 / portTICK_PERIOD_MS);
            } else {
                delayMicroseconds(100);
            }
        }
        digitalWrite(pin, LOW);
        ledcDetachPin(pin);
        digitalWrite(pin, LOW);
    } else {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, HIGH);
    }
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 1);
#endif
#ifdef SIMPLE_AP
    simpleAPPower(false);
    delay(500);
    simpleAPPower(up);
#endif
}