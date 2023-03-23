#include <Arduino.h>
#include "powermgt.h"
#include "settings.h"

// On the OpenEPaperLink board, there is no in-rush current limiting. The tags that can be connected to the board can have significant capacity, which,
// when drained, will cause the 3v3 rail to sag enough to reset the ESP32. This is obviously not great. To prevent this from happening, we ramp up/down the
// voltage with PWM. Ramping down really is unnecessary, as the board has a resistor to dump the charge into.
void rampTagPower(uint8_t pin, bool up) {
    if (up) {
        ledcSetup(0, 152000, 8);  // 141251 okay // 101251 okay
        ledcWrite(0, 254);
        vTaskDelay(10 / portTICK_PERIOD_MS);
        ledcAttachPin(pin, 0);
        pinMode(pin, OUTPUT);
        for (uint8_t c = 254; c != 0xFF; c--) {
            ledcWrite(0, c);
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
        digitalWrite(pin, LOW);
        ledcDetachPin(pin);
        digitalWrite(pin, LOW);
    } else {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, HIGH);
    }
}