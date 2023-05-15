#include <Arduino.h>

#ifdef HAS_RGB_LED
#include <FastLED.h>
#endif

void ledTask(void* parameter);
void setBrightness(int brightness);
void updateBrightnessFromConfig();

#ifdef HAS_RGB_LED
void shortBlink(CRGB cname);
void showColorPattern(CRGB colorone, CRGB colortwo, CRGB colorthree);
void rgbIdle();
void addFadeColor(CRGB cname);
#endif