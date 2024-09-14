#ifndef _FASTLED_GFX_H_
#define _FASTLED_GFX_H_

#include <FastLED.h>
#include <Arduino_GFX.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define NEOPIXEL_PIN 1

#define NEOPIXEL_WIDTH 30
#define NEOPIXEL_HEIGHT 8
#define NUMPIXELS (NEOPIXEL_WIDTH * NEOPIXEL_HEIGHT)
#define NEOPIXEL_BRIGHTNESS 3 // 1-255

#define LED_TYPE WS2811
#define COLOR_ORDER GRB

CRGB leds[NUMPIXELS];

class FastLED_GFX : public Arduino_GFX
{
public:
  FastLED_GFX() : Arduino_GFX(NEOPIXEL_WIDTH, NEOPIXEL_HEIGHT)
  {
    wrap = false;
  }

  bool begin(int32_t speed = GFX_NOT_DEFINED) override
  {
    FastLED.addLeds<LED_TYPE, NEOPIXEL_PIN, COLOR_ORDER>(leds, NUMPIXELS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(NEOPIXEL_BRIGHTNESS);

    return true;
  }

  void writePixelPreclipped(int16_t x, int16_t y, uint16_t color) override
  {
    // select you matrix mode
    int32_t i = (x * NEOPIXEL_HEIGHT) + y; // vertical strip start from left top
    // int32_t i = (x * NEOPIXEL_HEIGHT) + ((x % 2) ? y : (NEOPIXEL_HEIGHT - y - 1)); // vertical zigzag strip start from left top
    // int32_t i = (x * NEOPIXEL_HEIGHT) + (NEOPIXEL_HEIGHT - y - 1);                 // vertical strip start from left bottom
    // int32_t i = (x * NEOPIXEL_HEIGHT) + ((x % 2) ? (NEOPIXEL_HEIGHT - y - 1) : y); // vertical zigzag strip start from left bottom
    // int32_t i = x + (y * NEOPIXEL_WIDTH);                                          // horizontal strip start from left top
    // int32_t i = ((y % 2) ? x : (NEOPIXEL_WIDTH - x - 1)) + (y * NEOPIXEL_WIDTH);   // horizontal zigzag strip start from left top
    // int32_t i = (NEOPIXEL_WIDTH - x - 1) + (y * NEOPIXEL_WIDTH);                   // horizontal strip start from right top
    // int32_t i = ((y % 2) ? (NEOPIXEL_WIDTH - x - 1) : x) + (y * NEOPIXEL_WIDTH);   // horizontal zigzag strip start from right top

    leds[i] = RGB16TO24(color);
  }

  void endWrite(void) override
  {
    FastLED.show();
  }
};

#endif // _FASTLED_GFX_H_
