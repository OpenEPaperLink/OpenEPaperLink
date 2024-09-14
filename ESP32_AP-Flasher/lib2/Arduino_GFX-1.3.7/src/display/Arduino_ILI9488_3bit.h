#include "Arduino_DataBus.h"
#if !defined(LITTLE_FOOT_PRINT)

#ifndef _ARDUINO_ILI9488_3BIT_H_
#define _ARDUINO_ILI9488_3BIT_H_

#include <Arduino.h>
#include <Print.h>
#include "Arduino_ILI9488.h"

class Arduino_ILI9488_3bit : public Arduino_G
{
public:
  Arduino_ILI9488_3bit(Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0, bool ips = false);

  bool begin(int32_t speed = GFX_NOT_DEFINED) override;
  void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg) override;
  void drawIndexedBitmap(int16_t x, int16_t y, uint8_t *bitmap, uint16_t *color_index, int16_t w, int16_t h, int16_t x_skip = 0) override;
  void draw3bitRGBBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h) override;
  void draw16bitRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h) override;
  void draw24bitRGBBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h) override;

  void invertDisplay(bool);
  void displayOn();
  void displayOff();

protected:
  void writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h);

  Arduino_DataBus *_bus;
  int8_t _rst;
  uint8_t _rotation;
  bool _ips;

private:
};

#endif // _ARDUINO_ILI9488_3BIT_H_

#endif // !defined(LITTLE_FOOT_PRINT)
