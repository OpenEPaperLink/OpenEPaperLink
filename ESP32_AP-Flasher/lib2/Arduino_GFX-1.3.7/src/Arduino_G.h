#if !defined(LITTLE_FOOT_PRINT)

#ifndef _ARDUINO_G_H_
#define _ARDUINO_G_H_

#include <Arduino.h>

#include "Arduino_DataBus.h"

/// A generic graphics superclass that can handle all sorts of drawing. At a minimum you can subclass and provide drawPixel(). At a maximum you can do a ton of overriding to optimize. Used for any/all Adafruit displays!
class Arduino_G
{
public:
  Arduino_G(int16_t w, int16_t h); // Constructor

  // This MUST be defined by the subclass:
  virtual bool begin(int32_t speed = GFX_NOT_DEFINED) = 0;

  virtual void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg) = 0;
  virtual void drawIndexedBitmap(int16_t x, int16_t y, uint8_t *bitmap, uint16_t *color_index, int16_t w, int16_t h, int16_t x_skip = 0) = 0;
  virtual void draw3bitRGBBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h) = 0;
  virtual void draw16bitRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h) = 0;
  virtual void draw24bitRGBBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h) = 0;

protected:
  int16_t
      WIDTH,  ///< This is the 'raw' display width - never changes
      HEIGHT; ///< This is the 'raw' display height - never changes
};

#endif // _ARDUINO_G_H_

// utility functions
bool gfx_draw_bitmap_to_framebuffer(
    uint16_t *from_bitmap, int16_t bitmap_w, int16_t bitmap_h,
    uint16_t *framebuffer, int16_t x, int16_t y, int16_t framebuffer_w, int16_t framebuffer_h);

bool gfx_draw_bitmap_to_framebuffer_rotate_1(
    uint16_t *from_bitmap, int16_t bitmap_w, int16_t bitmap_h,
    uint16_t *framebuffer, int16_t x, int16_t y, int16_t framebuffer_w, int16_t framebuffer_h);

bool gfx_draw_bitmap_to_framebuffer_rotate_2(
    uint16_t *from_bitmap, int16_t bitmap_w, int16_t bitmap_h,
    uint16_t *framebuffer, int16_t x, int16_t y, int16_t framebuffer_w, int16_t framebuffer_h);

bool gfx_draw_bitmap_to_framebuffer_rotate_3(
    uint16_t *from_bitmap, int16_t bitmap_w, int16_t bitmap_h,
    uint16_t *framebuffer, int16_t x, int16_t y, int16_t framebuffer_w, int16_t framebuffer_h);

#endif // !defined(LITTLE_FOOT_PRINT)
