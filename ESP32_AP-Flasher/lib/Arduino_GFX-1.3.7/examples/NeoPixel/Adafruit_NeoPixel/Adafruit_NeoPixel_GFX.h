#ifndef _ADAFRUIT_NEOPIXEL_GFX_H_
#define _ADAFRUIT_NEOPIXEL_GFX_H_

#include <Adafruit_NeoPixel.h>
#include <Arduino_GFX.h>

#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define NEOPIXEL_PIN 1 // On Trinket or Gemma, suggest changing this to 1

#define NEOPIXEL_WIDTH 30
#define NEOPIXEL_HEIGHT 8
#define NUMPIXELS (NEOPIXEL_WIDTH * NEOPIXEL_HEIGHT)
#define NEOPIXEL_BRIGHTNESS 3 // 1-255

// Pixel type flags, add together as needed:
// NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
// NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
// NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
// NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
// NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
#define NEOPIXEL_TYPE (NEO_GRB + NEO_KHZ800)

class Adafruit_NeoPixel_GFX : public Arduino_GFX
{
public:
  Adafruit_NeoPixel_GFX() : Arduino_GFX(NEOPIXEL_WIDTH, NEOPIXEL_HEIGHT)
  {
    wrap = false;
  }

  bool begin(int32_t speed = GFX_NOT_DEFINED) override
  {
    // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
    // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
#endif
    // END of Trinket-specific code.
    _pixels = new Adafruit_NeoPixel(NUMPIXELS, NEOPIXEL_PIN, NEOPIXEL_TYPE);
    _pixels->begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
    _pixels->show();  // Turn OFF all pixels ASAP
    _pixels->setBrightness(NEOPIXEL_BRIGHTNESS);

    return true;
  }

  void writePixelPreclipped(int16_t x, int16_t y, uint16_t color) override
  {
    // select you matrix mode
    int32_t i = (x * NEOPIXEL_HEIGHT) + y;                                         // vertical strip start from left top
    // int32_t i = (x * NEOPIXEL_HEIGHT) + ((x % 2) ? y : (NEOPIXEL_HEIGHT - y - 1)); // vertical zigzag strip start from left top
    // int32_t i = (x * NEOPIXEL_HEIGHT) + (NEOPIXEL_HEIGHT - y - 1);                 // vertical strip start from left bottom
    // int32_t i = (x * NEOPIXEL_HEIGHT) + ((x % 2) ? (NEOPIXEL_HEIGHT - y - 1) : y); // vertical zigzag strip start from left bottom
    // int32_t i = x + (y * NEOPIXEL_WIDTH);                                          // horizontal strip start from left top
    // int32_t i = ((y % 2) ? x : (NEOPIXEL_WIDTH - x - 1)) + (y * NEOPIXEL_WIDTH);   // horizontal zigzag strip start from left top
    // int32_t i = (NEOPIXEL_WIDTH - x - 1) + (y * NEOPIXEL_WIDTH);                   // horizontal strip start from right top
    // int32_t i = ((y % 2) ? (NEOPIXEL_WIDTH - x - 1) : x) + (y * NEOPIXEL_WIDTH);   // horizontal zigzag strip start from right top

    _pixels->setPixelColor(i, RGB16TO24(color));
  }

  void endWrite(void) override
  {
    _pixels->show();
  }

  Adafruit_NeoPixel *_pixels;
};

#endif // _ADAFRUIT_NEOPIXEL_GFX_H_
