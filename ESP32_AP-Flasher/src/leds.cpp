#include <Arduino.h>

#ifdef HAS_RGB_LED
#define FASTLED_INTERNAL
#include <FastLED.h>
#endif

#include "settings.h"
#include "tag_db.h"

QueueHandle_t ledQueue;
int maxledbrightness = 255;

#ifdef HAS_RGB_LED
QueueHandle_t rgbLedQueue;

struct ledInstructionRGB {
    CRGB ledColor;
    uint16_t fadeTime;
    uint16_t length;
    bool reQueue = false;
};

CRGB leds[1];
volatile bool rgbQueueFlush = false;
#endif

struct ledInstruction {
    uint16_t value;
    uint16_t fadeTime;
    uint16_t length;
    bool reQueue = false;
};

const uint8_t PROGMEM gamma8[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
    2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5,
    5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10,
    10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
    17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
    25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
    37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
    51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
    69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
    90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
    115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
    144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
    177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
    215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255};

const uint16_t gamma12[256] = {
    0, 2, 4, 6, 8, 10, 12, 13, 15, 17,
    19, 21, 23, 25, 27, 29, 31, 33, 35, 36,
    38, 37, 39, 41, 43, 45, 47, 49, 52, 54,
    56, 59, 61, 64, 66, 69, 72, 74, 77, 80,
    83, 87, 90, 93, 96, 100, 103, 107, 111, 115,
    118, 122, 126, 131, 135, 139, 143, 148, 153, 157,
    162, 167, 172, 177, 182, 187, 193, 198, 204, 209,
    215, 221, 227, 233, 239, 246, 252, 259, 265, 272,
    279, 286, 293, 300, 307, 315, 322, 330, 338, 346,
    354, 362, 370, 379, 387, 396, 405, 414, 423, 432,
    442, 451, 461, 470, 480, 490, 500, 511, 521, 532,
    542, 553, 564, 575, 587, 598, 610, 622, 633, 645,
    658, 670, 683, 695, 708, 721, 734, 747, 761, 774,
    788, 802, 816, 830, 845, 859, 874, 889, 904, 919,
    935, 950, 966, 982, 998, 1015, 1031, 1048, 1065, 1082,
    1099, 1116, 1134, 1151, 1169, 1187, 1206, 1224, 1243, 1262,
    1281, 1300, 1319, 1339, 1359, 1379, 1399, 1419, 1440, 1461,
    1482, 1503, 1524, 1546, 1568, 1590, 1612, 1634, 1657, 1680,
    1703, 1726, 1749, 1773, 1797, 1821, 1845, 1870, 1895, 1920,
    1945, 1970, 1996, 2022, 2048, 2074, 2100, 2127, 2154, 2181,
    2209, 2236, 2264, 2292, 2321, 2349, 2378, 2407, 2436, 2466,
    2495, 2525, 2556, 2586, 2617, 2648, 2679, 2710, 2742, 2774,
    2806, 2838, 2871, 2904, 2937, 2970, 3004, 3038, 3072, 3107,
    3141, 3176, 3211, 3247, 3283, 3319, 3355, 3391, 3428, 3465,
    3502, 3540, 3578, 3616, 3654, 3693, 3732, 3771, 3810, 3850,
    3890, 3930, 3971, 4013, 4054, 4095};

#ifdef HAS_RGB_LED

void addToRGBQueue(struct ledInstructionRGB* rgb, bool requeue) {
    rgb->reQueue = requeue;
    if (!rgbLedQueue) {
        delete rgb;
        return;
    }
    BaseType_t queuestatus = xQueueSend(rgbLedQueue, &rgb, 0);
    if (queuestatus == pdFALSE) {
        delete rgb;
    }
}

void addFadeColor(CRGB cname) {
    struct ledInstructionRGB* rgb = new struct ledInstructionRGB;
    rgb->ledColor = cname;
    rgb->fadeTime = 750;
    rgb->length = 0;
    addToRGBQueue(rgb, false);
}

void shortBlink(CRGB cname) {
    struct ledInstructionRGB* rgb = new struct ledInstructionRGB;
    rgb->ledColor = CRGB::Black;
    rgb->fadeTime = 0;
    rgb->length = 3;
    addToRGBQueue(rgb, false);
    rgb = new struct ledInstructionRGB;
    rgb->ledColor = cname;
    rgb->ledColor.maximizeBrightness(0x80);
    rgb->fadeTime = 0;
    rgb->length = 10;
    addToRGBQueue(rgb, false);
    rgb = new struct ledInstructionRGB;
    rgb->ledColor = CRGB::Black;
    rgb->fadeTime = 0;
    rgb->length = 3;
    addToRGBQueue(rgb, false);
}

void flushRGBQueue() {
    rgbQueueFlush = true;
}

void rgbIdle() {
    flushRGBQueue();
}

void showColorPattern(CRGB colorone, CRGB colortwo, CRGB colorthree) {
    struct ledInstructionRGB* rgb = new struct ledInstructionRGB;
    rgb->ledColor = CRGB::Black;
    rgb->fadeTime = 0;
    rgb->length = 600;
    addToRGBQueue(rgb, true);
    rgb = new struct ledInstructionRGB;
    rgb->ledColor = colorone;
    rgb->fadeTime = 0;
    rgb->length = 120;
    addToRGBQueue(rgb, true);
    rgb = new struct ledInstructionRGB;
    rgb->ledColor = CRGB::Black;
    rgb->fadeTime = 0;
    rgb->length = 200;
    addToRGBQueue(rgb, true);
    rgb = new struct ledInstructionRGB;
    rgb->ledColor = colortwo;
    rgb->fadeTime = 0;
    rgb->length = 120;
    addToRGBQueue(rgb, true);
    rgb = new struct ledInstructionRGB;
    rgb->ledColor = CRGB::Black;
    rgb->fadeTime = 0;
    rgb->length = 200;
    addToRGBQueue(rgb, true);
    rgb = new struct ledInstructionRGB;
    rgb->ledColor = colorthree;
    rgb->fadeTime = 0;
    rgb->length = 120;
    addToRGBQueue(rgb, true);
}

void showRGB() {
    FastLED.show();
}

volatile CRGB rgbIdleColor = CRGB::Green;
volatile uint16_t rgbIdlePeriod = 800;

void rgbIdleStep() {
    static bool dirUp = true;
    static uint16_t step = 0;

    if (dirUp) {
        // up
        step++;
        if (step == rgbIdlePeriod) {
            dirUp = false;
        }
    } else {
        // down
        step--;
        if (step == 0) {
            dirUp = true;
        }
    }
    CRGB newvalue = blend(CRGB::Black, (const CRGB&)rgbIdleColor, gamma8[map(step, 0, rgbIdlePeriod, 0, 255)]);
    if (newvalue != leds[0]) {
        leds[0] = newvalue;
        showRGB();
    }
}
#endif

void addToMonoQueue(struct ledInstruction* mono) {
    BaseType_t queuestatus = xQueueSend(ledQueue, &mono, 0);
    if (queuestatus == pdFALSE) {
        delete mono;
    }
}

void addFadeMono(uint8_t value) {
    struct ledInstruction* mono = new struct ledInstruction;
    mono->value = value;
    mono->fadeTime = 750;
    mono->length = 0;
    addToMonoQueue(mono);
}

void showMono(uint8_t brightness) {
    ledcWrite(7, gamma12[brightness]);
}
volatile uint16_t monoIdlePeriod = 900;

uint8_t monoValue = 0;

void monoIdleStep() {
    static bool dirUp = true;
    static uint16_t step = 0;
    if (dirUp) {
        // up
        step++;
        if (step == monoIdlePeriod) {
            dirUp = false;
        }
    } else {
        // down
        step--;
        if (step == 0) {
            dirUp = true;
        }
    }
    uint8_t newvalue = map(step, 0, monoIdlePeriod, 0, 255);
    if (newvalue != monoValue) {
        monoValue = newvalue;
        showMono(newvalue);
    }
}

void setBrightness(int brightness) {
    maxledbrightness = brightness;
#ifdef HAS_RGB_LED
    FastLED.setBrightness(maxledbrightness);
#endif
}

void updateBrightnessFromConfig() {
    if (APconfig["ledbrightness"].as<int>() != 0) {
        int newbrightness = APconfig["ledbrightness"].as<int>();
        if (newbrightness < 0) newbrightness = 0;
        if (newbrightness != maxledbrightness) {
            setBrightness(newbrightness);
        }
    }
}

void ledTask(void* parameter) {
#ifdef HAS_RGB_LED
    FastLED.addLeds<WS2812B, FLASHER_RGB_LED, GRB>(leds, 1);  // GRB ordering is typical
    leds[0] = CRGB::Blue;
    showRGB();
    rgbLedQueue = xQueueCreate(30, sizeof(struct ledInstructionRGB*));

    struct ledInstructionRGB* rgb = nullptr;
    // open with a nice RGB crossfade
    addFadeColor(CRGB::Red);
    addFadeColor(CRGB::Green);
    addFadeColor(CRGB::Blue);
    addFadeColor(CRGB::Red);
    addFadeColor(CRGB::Green);
    addFadeColor(CRGB::Blue);
    CRGB oldColor = CRGB::Black;
    uint16_t rgbInstructionFadeTime = 0;
#endif

    ledQueue = xQueueCreate(30, sizeof(struct ledInstruction*));

    digitalWrite(FLASHER_LED, HIGH);
    pinMode(FLASHER_LED, OUTPUT);
    ledcSetup(7, 9500, 12);  // 141251 okay // 101251 okay
    ledcAttachPin(FLASHER_LED, 7);

    struct ledInstruction* monoled = nullptr;

    addFadeMono(255);
    addFadeMono(127);
    addFadeMono(255);
    addFadeMono(0);

    uint8_t oldBrightness = 0;

    uint16_t monoInstructionFadeTime = 0;

    while (1) {
#ifdef HAS_RGB_LED
        // handle RGB led instructions
        if (rgb == nullptr) {
            // fetch a led instruction
            BaseType_t q = xQueueReceive(rgbLedQueue, &rgb, 1);
            if (q == pdTRUE) {
                if (rgb->reQueue && !rgbQueueFlush) {
                    // requeue this instruction at the end of the queue, caveman style.
                    struct ledInstructionRGB* requeue = new ledInstructionRGB;
                    requeue->fadeTime = rgb->fadeTime;
                    requeue->ledColor = rgb->ledColor;
                    requeue->length = rgb->length;
                    addToRGBQueue(requeue, true);
                }

                if (rgbQueueFlush) {
                    delete rgb;
                    rgb = nullptr;
                } else {
                    rgbInstructionFadeTime = rgb->fadeTime;
                    if (rgb->fadeTime <= 1) {
                        leds[0] = rgb->ledColor;
                        showRGB();
                    }
                }
            } else {
                rgbQueueFlush = false;
                // no commands, run idle led task
                rgbIdleStep();
            }
        } else {
            // process instruction
            if (rgb->fadeTime) {
                rgb->fadeTime--;
                leds[0] = blend(rgb->ledColor, oldColor, map(rgb->fadeTime, 0, rgbInstructionFadeTime, 0, 255));
                showRGB();
            } else if (rgb->length) {
                rgb->length--;
            } else {
                oldColor = rgb->ledColor;
                delete rgb;
                rgb = nullptr;
            }
        }
#endif
        // handle flasher LED (single color)
        if (monoled == nullptr) {
            BaseType_t q = xQueueReceive(ledQueue, &monoled, 1);
            if (q == pdTRUE) {
                monoInstructionFadeTime = monoled->fadeTime;
                if (monoled->fadeTime <= 1) {
                    showMono(gamma12[monoled->value]);
                }
            } else {
                monoIdleStep();
            }
        } else {
            if (monoled->fadeTime) {
                monoled->fadeTime--;
                showMono(map(monoled->fadeTime, 0, monoInstructionFadeTime, monoled->value, oldBrightness));
            } else if (monoled->length) {
                monoled->length--;
            } else {
                oldBrightness = monoled->value;
                delete monoled;
                monoled = nullptr;
            }
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}