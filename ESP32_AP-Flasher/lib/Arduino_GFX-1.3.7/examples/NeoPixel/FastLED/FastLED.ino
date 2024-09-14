/*******************************************************************************
 * FastLED GFX Example
 * This is a simple GFX example for FastLED
 *
 * Dependent libraries:
 * FastLED: https://github.com/FastLED/FastLED.git
 ******************************************************************************/
#include <Arduino_GFX_Library.h>

// all settings in header file
#include "FastLED_GFX.h"
Arduino_GFX *gfx = new FastLED_GFX();
int16_t x;
uint16_t w, tw;

void setup(void)
{
  // Init Display
  if (!gfx->begin())
  {
    Serial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(BLACK);

  x = 0;
  gfx->setCursor(x, 0);
  gfx->setTextColor(RED);
  gfx->println("Hello World!");

  int16_t x1, y1;
  uint16_t h;
  gfx->getTextBounds("Hello World!", 0, 0, &x1, &y1, &w, &h);
  tw = w;
  w = gfx->width();

  delay(1000); // 1 seconds
}

void loop()
{
  x--;
  if (x < (-tw))
  {
    x = w - 1;
  }
  gfx->setCursor(x, 0);
  gfx->setTextColor(random(0xffff), BLACK);
  gfx->println("Hello World!");

  delay(100); // 0.1 second
}
