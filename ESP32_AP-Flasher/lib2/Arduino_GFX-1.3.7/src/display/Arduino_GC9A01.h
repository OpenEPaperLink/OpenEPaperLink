#ifndef _ARDUINO_GC9A01_H_
#define _ARDUINO_GC9A01_H_

#include <Arduino.h>
#include <Print.h>
#include "./Arduino_GFX.h"
#include "../Arduino_TFT.h"

#define GC9A01_TFTWIDTH 240
#define GC9A01_TFTHEIGHT 240

#define GC9A01_RST_DELAY 120    ///< delay ms wait for reset finish
#define GC9A01_SLPIN_DELAY 120  ///< delay ms wait for sleep in finish
#define GC9A01_SLPOUT_DELAY 120 ///< delay ms wait for sleep out finish

#define GC9A01_NOP 0x00
#define GC9A01_SWRESET 0x01
#define GC9A01_RDDID 0x04
#define GC9A01_RDDST 0x09

#define GC9A01_SLPIN 0x10
#define GC9A01_SLPOUT 0x11
#define GC9A01_PTLON 0x12
#define GC9A01_NORON 0x13

#define GC9A01_INVOFF 0x20
#define GC9A01_INVON 0x21
#define GC9A01_DISPOFF 0x28
#define GC9A01_DISPON 0x29

#define GC9A01_CASET 0x2A
#define GC9A01_RASET 0x2B
#define GC9A01_RAMWR 0x2C
#define GC9A01_RAMRD 0x2E

#define GC9A01_PTLAR 0x30
#define GC9A01_COLMOD 0x3A
#define GC9A01_MADCTL 0x36

#define GC9A01_MADCTL_MY 0x80
#define GC9A01_MADCTL_MX 0x40
#define GC9A01_MADCTL_MV 0x20
#define GC9A01_MADCTL_ML 0x10
#define GC9A01_MADCTL_RGB 0x00

#define GC9A01_RDID1 0xDA
#define GC9A01_RDID2 0xDB
#define GC9A01_RDID3 0xDC
#define GC9A01_RDID4 0xDD

static const uint8_t gc9a01_init_operations[] = {
    BEGIN_WRITE,

    WRITE_COMMAND_8, 0xEF,
    WRITE_C8_D8, 0xEB, 0x14,

    WRITE_COMMAND_8, 0xFE,
    WRITE_COMMAND_8, 0xEF,

    WRITE_C8_D8, 0xEB, 0x14,

    WRITE_C8_D8, 0x84, 0x40,

    WRITE_C8_D8, 0x85, 0xFF,

    WRITE_C8_D8, 0x86, 0xFF,

    WRITE_C8_D8, 0x87, 0xFF,

    WRITE_C8_D8, 0x88, 0x0A,

    WRITE_C8_D8, 0x89, 0x21,

    WRITE_C8_D8, 0x8A, 0x00,

    WRITE_C8_D8, 0x8B, 0x80,

    WRITE_C8_D8, 0x8C, 0x01,

    WRITE_C8_D8, 0x8D, 0x01,

    WRITE_C8_D8, 0x8E, 0xFF,

    WRITE_C8_D8, 0x8F, 0xFF,

    WRITE_C8_D16, 0xB6, 0x00, 0x20,

    WRITE_C8_D8, 0x3A, 0x05,

    WRITE_COMMAND_8, 0x90,
    WRITE_BYTES, 4, 0x08, 0x08, 0x08, 0x08,

    WRITE_C8_D8, 0xBD, 0x06,

    WRITE_C8_D8, 0xBC, 0x00,

    WRITE_COMMAND_8, 0xFF,
    WRITE_BYTES, 3, 0x60, 0x01, 0x04,

    WRITE_C8_D8, 0xC3, 0x13,
    WRITE_C8_D8, 0xC4, 0x13,

    WRITE_C8_D8, 0xC9, 0x22,

    WRITE_C8_D8, 0xBE, 0x11,

    WRITE_C8_D16, 0xE1, 0x10, 0x0E,

    WRITE_COMMAND_8, 0xDF,
    WRITE_BYTES, 3, 0x21, 0x0c, 0x02,

    WRITE_COMMAND_8, 0xF0,
    WRITE_BYTES, 6,
    0x45, 0x09, 0x08, 0x08, 0x26,
    0x2A,

    WRITE_COMMAND_8, 0xF1,
    WRITE_BYTES, 6,
    0x43, 0x70, 0x72, 0x36, 0x37,
    0x6F,

    WRITE_COMMAND_8, 0xF2,
    WRITE_BYTES, 6,
    0x45, 0x09, 0x08, 0x08, 0x26,
    0x2A,

    WRITE_COMMAND_8, 0xF3,
    WRITE_BYTES, 6,
    0x43, 0x70, 0x72, 0x36, 0x37,
    0x6F,

    WRITE_C8_D16, 0xED, 0x1B, 0x0B,

    WRITE_C8_D8, 0xAE, 0x77,

    WRITE_C8_D8, 0xCD, 0x63,

    WRITE_COMMAND_8, 0x70,
    WRITE_BYTES, 9,
    0x07, 0x07, 0x04, 0x0E, 0x0F,
    0x09, 0x07, 0x08, 0x03,

    WRITE_C8_D8, 0xE8, 0x34,

    WRITE_COMMAND_8, 0x62,
    WRITE_BYTES, 12,
    0x18, 0x0D, 0x71, 0xED, 0x70,
    0x70, 0x18, 0x0F, 0x71, 0xEF,
    0x70, 0x70,

    WRITE_COMMAND_8, 0x63,
    WRITE_BYTES, 12,
    0x18, 0x11, 0x71, 0xF1, 0x70,
    0x70, 0x18, 0x13, 0x71, 0xF3,
    0x70, 0x70,

    WRITE_COMMAND_8, 0x64,
    WRITE_BYTES, 7,
    0x28, 0x29, 0xF1, 0x01, 0xF1,
    0x00, 0x07,

    WRITE_COMMAND_8, 0x66,
    WRITE_BYTES, 10,
    0x3C, 0x00, 0xCD, 0x67, 0x45,
    0x45, 0x10, 0x00, 0x00, 0x00,

    WRITE_COMMAND_8, 0x67,
    WRITE_BYTES, 10,
    0x00, 0x3C, 0x00, 0x00, 0x00,
    0x01, 0x54, 0x10, 0x32, 0x98,

    WRITE_COMMAND_8, 0x74,
    WRITE_BYTES, 7,
    0x10, 0x85, 0x80, 0x00, 0x00,
    0x4E, 0x00,

    WRITE_C8_D16, 0x98, 0x3e, 0x07,

    WRITE_COMMAND_8, 0x35,

    WRITE_COMMAND_8, GC9A01_SLPOUT,
    END_WRITE,

    DELAY, GC9A01_SLPOUT_DELAY,

    BEGIN_WRITE,
    WRITE_COMMAND_8, GC9A01_DISPON, // Display on
    END_WRITE,

    DELAY, 20};

class Arduino_GC9A01 : public Arduino_TFT
{
public:
  Arduino_GC9A01(
      Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0,
      bool ips = false, int16_t w = GC9A01_TFTWIDTH, int16_t h = GC9A01_TFTHEIGHT,
      uint8_t col_offset1 = 0, uint8_t row_offset1 = 0, uint8_t col_offset2 = 0, uint8_t row_offset2 = 0);

  bool begin(int32_t speed = GFX_NOT_DEFINED) override;
  void writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h) override;
  void setRotation(uint8_t r) override;
  void invertDisplay(bool) override;
  void displayOn() override;
  void displayOff() override;

protected:
  void tftInit() override;

private:
};

#endif
