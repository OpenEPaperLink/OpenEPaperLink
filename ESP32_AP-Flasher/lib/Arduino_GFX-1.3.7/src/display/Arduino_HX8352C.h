/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 */
#ifndef _ARDUINO_HX8352C_H_
#define _ARDUINO_HX8352C_H_

#include <Arduino.h>
#include <Print.h>
#include "../Arduino_GFX.h"
#include "../Arduino_TFT.h"

#define HX8352C_TFTWIDTH 240  ///< HX8352C max TFT width
#define HX8352C_TFTHEIGHT 400 ///< HX8352C max TFT height

#define HX8352C_RST_DELAY 120

class Arduino_HX8352C : public Arduino_TFT
{
public:
  Arduino_HX8352C(
      Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0,
      bool ips = false, int16_t w = HX8352C_TFTWIDTH, int16_t h = HX8352C_TFTHEIGHT,
      uint8_t col_offset1 = 0, uint8_t row_offset1 = 0, uint8_t col_offset2 = 0, uint8_t row_offset2 = 0);

  bool begin(int32_t speed = GFX_NOT_DEFINED) override;
  void writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h) override;
  void setRotation(uint8_t r) override;
  void invertDisplay(bool) override;
  void displayOn() override;
  void displayOff() override;

protected:
  void tftInit() override;
  bool _invert = false;

private:
};

#endif
