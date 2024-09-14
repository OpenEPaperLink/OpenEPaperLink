/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 */
#ifndef _ARDUINO_HX8347D_H_
#define _ARDUINO_HX8347D_H_

#include <Arduino.h>
#include <Print.h>
#include "../Arduino_GFX.h"
#include "../Arduino_TFT.h"

#define HX8347D_TFTWIDTH 240  ///< HX8347D max TFT width
#define HX8347D_TFTHEIGHT 320 ///< HX8347D max TFT height

#define HX8347D_RST_DELAY 120

#define HX8347D_DISPLAY_MODE_CONTROL 0x01 // Display Mode control

#define HX8347D_INV_OFF 0x00 // INV_ON disable
#define HX8347D_INV_ON 0x02  // INV_ON enable

static const uint8_t hx8347d_init_operations[] = {
    BEGIN_WRITE,
    WRITE_C8_D8, 0xEA, 0x00,
    WRITE_C8_D8, 0xEB, 0x20,
    WRITE_C8_D8, 0xEC, 0x0C,
    WRITE_C8_D8, 0xED, 0xC4,
    WRITE_C8_D8, 0xE8, 0x38,
    WRITE_C8_D8, 0xE9, 0x10,
    WRITE_C8_D8, 0xF1, 0x01,
    WRITE_C8_D8, 0xF2, 0x10,
    WRITE_C8_D8, 0x40, 0x01,
    WRITE_C8_D8, 0x41, 0x00,
    WRITE_C8_D8, 0x42, 0x00,
    WRITE_C8_D8, 0x43, 0x10,
    WRITE_C8_D8, 0x44, 0x0E,
    WRITE_C8_D8, 0x45, 0x24,
    WRITE_C8_D8, 0x46, 0x04,
    WRITE_C8_D8, 0x47, 0x50,
    WRITE_C8_D8, 0x48, 0x02,
    WRITE_C8_D8, 0x49, 0x13,
    WRITE_C8_D8, 0x4A, 0x19,
    WRITE_C8_D8, 0x4B, 0x19,
    WRITE_C8_D8, 0x4C, 0x16,
    WRITE_C8_D8, 0x50, 0x1B,
    WRITE_C8_D8, 0x51, 0x31,
    WRITE_C8_D8, 0x52, 0x2F,
    WRITE_C8_D8, 0x53, 0x3F,
    WRITE_C8_D8, 0x54, 0x3F,
    WRITE_C8_D8, 0x55, 0x3E,
    WRITE_C8_D8, 0x56, 0x2F,
    WRITE_C8_D8, 0x57, 0x7B,
    WRITE_C8_D8, 0x58, 0x09,
    WRITE_C8_D8, 0x59, 0x06,
    WRITE_C8_D8, 0x5A, 0x06,
    WRITE_C8_D8, 0x5B, 0x0C,
    WRITE_C8_D8, 0x5C, 0x1D,
    WRITE_C8_D8, 0x5D, 0xCC,
    WRITE_C8_D8, 0x1B, 0x1B,
    WRITE_C8_D8, 0x1A, 0x01,
    WRITE_C8_D8, 0x24, 0x2F,
    WRITE_C8_D8, 0x25, 0x57,
    WRITE_C8_D8, 0x23, 0x88,
    WRITE_C8_D8, 0x18, 0x34,
    WRITE_C8_D8, 0x19, 0x01,
    WRITE_C8_D8, 0x1F, 0x88,
    WRITE_C8_D8, 0x1F, 0x80,
    WRITE_C8_D8, 0x1F, 0x90,
    WRITE_C8_D8, 0x1F, 0xD0,
    WRITE_C8_D8, 0x17, 0x05,
    WRITE_C8_D8, 0x28, 0x38,
    WRITE_C8_D8, 0x28, 0x3F,
    WRITE_C8_D8, 0x16, 0x18,
    END_WRITE};

class Arduino_HX8347D : public Arduino_TFT
{
public:
  Arduino_HX8347D(
      Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0,
      bool ips = false, int16_t w = HX8347D_TFTWIDTH, int16_t h = HX8347D_TFTHEIGHT,
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
