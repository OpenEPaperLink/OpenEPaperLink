/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 */
#ifndef _ARDUINO_ST7796_H_
#define _ARDUINO_ST7796_H_

#include <Arduino.h>
#include <Print.h>
#include "../Arduino_GFX.h"
#include "../Arduino_TFT.h"

#define ST7796_TFTWIDTH 320
#define ST7796_TFTHEIGHT 480

#define ST7796_RST_DELAY 120    ///< delay ms wait for reset finish
#define ST7796_SLPIN_DELAY 120  ///< delay ms wait for sleep in finish
#define ST7796_SLPOUT_DELAY 120 ///< delay ms wait for sleep out finish

#define ST7796_NOP 0x00
#define ST7796_SWRESET 0x01
#define ST7796_RDDID 0x04
#define ST7796_RDDST 0x09

#define ST7796_SLPIN 0x10
#define ST7796_SLPOUT 0x11
#define ST7796_PTLON 0x12
#define ST7796_NORON 0x13

#define ST7796_INVOFF 0x20
#define ST7796_INVON 0x21
#define ST7796_DISPOFF 0x28
#define ST7796_DISPON 0x29

#define ST7796_CASET 0x2A
#define ST7796_RASET 0x2B
#define ST7796_RAMWR 0x2C
#define ST7796_RAMRD 0x2E

#define ST7796_PTLAR 0x30
#define ST7796_COLMOD 0x3A
#define ST7796_MADCTL 0x36

#define ST7796_MADCTL_MY 0x80
#define ST7796_MADCTL_MX 0x40
#define ST7796_MADCTL_MV 0x20
#define ST7796_MADCTL_ML 0x10
#define ST7796_MADCTL_RGB 0x00
#define ST7796_MADCTL_BGR 0x08
#define ST7796_MADCTL_MH 0x04

#define ST7796_RDID1 0xDA
#define ST7796_RDID2 0xDB
#define ST7796_RDID3 0xDC
#define ST7796_RDID4 0xDD

static const uint8_t st7796_init_operations[] = {
    BEGIN_WRITE,
    WRITE_C8_D8, ST7796_COLMOD, 0x55, // 0x66,

    WRITE_C8_D8, 0xF0, 0xC3, // Command Set Control
    WRITE_C8_D8, 0xF0, 0x96,

    WRITE_C8_D8, 0xB4, 0x01,

    WRITE_COMMAND_8, 0xB6,
    WRITE_BYTES, 3, 0x80, 0x22, 0x3B,

    WRITE_COMMAND_8, 0xE8,
    WRITE_BYTES, 8,
    0x40, 0x8A, 0x00, 0x00, 0x29,
    0x19, 0xA5, 0x33,

    WRITE_C8_D8, 0xC1, 0x06,
    WRITE_C8_D8, 0xC2, 0xA7,
    WRITE_C8_D8, 0xC5, 0x18,

    WRITE_COMMAND_8, 0xE0,
    WRITE_BYTES, 14,
    0xF0, 0x09, 0x0B, 0x06, 0x04,
    0x15, 0x2F, 0x54, 0x42, 0x3C,
    0x17, 0x14, 0x18, 0x1B,

    WRITE_COMMAND_8, 0xE1,
    WRITE_BYTES, 14,
    0xE0, 0x09, 0x0B, 0x06, 0x04,
    0x03, 0x2B, 0x43, 0x42, 0x3B,
    0x16, 0x14, 0x17, 0x1B,

    WRITE_C8_D8, 0xF0, 0x3C,
    WRITE_C8_D8, 0xF0, 0x69,
    WRITE_COMMAND_8, ST7796_SLPOUT,
    END_WRITE,

    DELAY, ST7796_SLPOUT_DELAY,

    BEGIN_WRITE,
    WRITE_COMMAND_8, 0x38,
    WRITE_COMMAND_8, ST7796_DISPON,
    END_WRITE,

    DELAY, ST7796_SLPOUT_DELAY};

class Arduino_ST7796 : public Arduino_TFT
{
public:
  Arduino_ST7796(
      Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0,
      bool ips = false, int16_t w = ST7796_TFTWIDTH, int16_t h = ST7796_TFTHEIGHT,
      uint8_t col_offset1 = 0, uint8_t row_offset1 = 0, uint8_t col_offset2 = 0, uint8_t row_offset2 = 0);

  bool begin(int32_t speed = GFX_NOT_DEFINED) override;

  void setRotation(uint8_t r) override;

  void writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h) override;

  void invertDisplay(bool) override;
  void displayOn() override;
  void displayOff() override;

protected:
  void tftInit() override;

private:
};

#endif
