/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 * https://github.com/ananevilya/Arduino-ST7789-Library.git
 */
#ifndef _ARDUINO_ST7789_H_
#define _ARDUINO_ST7789_H_

#include <Arduino.h>
#include <Print.h>
#include "../Arduino_GFX.h"
#include "../Arduino_TFT.h"

#define ST7789_TFTWIDTH 240
#define ST7789_TFTHEIGHT 320

#define ST7789_RST_DELAY 120    ///< delay ms wait for reset finish
#define ST7789_SLPIN_DELAY 120  ///< delay ms wait for sleep in finish
#define ST7789_SLPOUT_DELAY 120 ///< delay ms wait for sleep out finish

#define ST7789_NOP 0x00
#define ST7789_SWRESET 0x01
#define ST7789_RDDID 0x04
#define ST7789_RDDST 0x09

#define ST7789_SLPIN 0x10
#define ST7789_SLPOUT 0x11
#define ST7789_PTLON 0x12
#define ST7789_NORON 0x13

#define ST7789_INVOFF 0x20
#define ST7789_INVON 0x21
#define ST7789_DISPOFF 0x28
#define ST7789_DISPON 0x29

#define ST7789_CASET 0x2A
#define ST7789_RASET 0x2B
#define ST7789_RAMWR 0x2C
#define ST7789_RAMRD 0x2E

#define ST7789_PTLAR 0x30
#define ST7789_COLMOD 0x3A
#define ST7789_MADCTL 0x36

#define ST7789_MADCTL_MY 0x80
#define ST7789_MADCTL_MX 0x40
#define ST7789_MADCTL_MV 0x20
#define ST7789_MADCTL_ML 0x10
#define ST7789_MADCTL_RGB 0x00

#define ST7789_RDID1 0xDA
#define ST7789_RDID2 0xDB
#define ST7789_RDID3 0xDC
#define ST7789_RDID4 0xDD

static const uint8_t st7789_init_operations[] = {
    BEGIN_WRITE,
    WRITE_COMMAND_8, ST7789_SLPOUT, // 2: Out of sleep mode, no args, w/delay
    END_WRITE,

    DELAY, ST7789_SLPOUT_DELAY,

    BEGIN_WRITE,
    WRITE_C8_D8, ST7789_COLMOD, 0x55, // 3: Set color mode, 16-bit color
    WRITE_C8_D8, 0x36, 0x00,

    WRITE_COMMAND_8, 0xB0,
    WRITE_BYTES, 2, 0x00, 0xF0, // 0xF0 MSB first, 0xF8 LSB first

    WRITE_COMMAND_8, 0xB2,
    WRITE_BYTES, 5, 0x0C, 0x0C, 0x00, 0x33, 0x33,

    WRITE_C8_D8, 0xB7, 0x35,
    WRITE_C8_D8, 0xBB, 0x19,
    WRITE_C8_D8, 0xC0, 0x2C,
    WRITE_C8_D8, 0xC2, 0x01,
    WRITE_C8_D8, 0xC3, 0x12,
    WRITE_C8_D8, 0xC4, 0x20,
    WRITE_C8_D8, 0xC6, 0x0F,

    WRITE_C8_D16, 0xD0, 0xA4, 0xA1,

    WRITE_COMMAND_8, 0xE0,
    WRITE_BYTES, 14,
    0b11110000, // V63P3, V63P2, V63P1, V63P0,  V0P3,  V0P2,  V0P1,  V0P0
    0b00001001, //     0,     0,  V1P5,  V1P4,  V1P3,  V1P2,  V1P1,  V1P0
    0b00010011, //     0,     0,  V2P5,  V2P4,  V2P3,  V2P2,  V2P1,  V2P0
    0b00010010, //     0,     0,     0,  V4P4,  V4P3,  V4P2,  V4P1,  V4P0
    0b00010010, //     0,     0,     0,  V6P4,  V6P3,  V6P2,  V6P1,  V6P0
    0b00101011, //     0,     0,  J0P1,  J0P0, V13P3, V13P2, V13P1, V13P0
    0b00111100, //     0, V20P6, V20P5, V20P4, V20P3, V20P2, V20P1, V20P0
    0b01000100, //     0, V36P2, V36P1, V36P0,     0, V27P2, V27P1, V27P0
    0b01001011, //     0, V43P6, V43P5, V43P4, V43P3, V43P2, V43P1, V43P0
    0b00011011, //     0,     0,  J1P1,  J1P0, V50P3, V50P2, V50P1, V50P0
    0b00011000, //     0,     0,     0, V57P4, V57P3, V57P2, V57P1, V57P0
    0b00010111, //     0,     0,     0, V59P4, V59P3, V59P2, V59P1, V59P0
    0b00011101, //     0,     0, V61P5, V61P4, V61P3, V61P2, V61P1, V61P0
    0b00100001, //     0,     0, V62P5, V62P4, V62P3, V62P2, V62P1, V62P0

    WRITE_COMMAND_8, 0XE1,
    WRITE_BYTES, 14,
    0b11110000, // V63P3, V63P2, V63P1, V63P0,  V0P3,  V0P2,  V0P1,  V0P0
    0b00001001, //     0,     0,  V1P5,  V1P4,  V1P3,  V1P2,  V1P1,  V1P0
    0b00010011, //     0,     0,  V2P5,  V2P4,  V2P3,  V2P2,  V2P1,  V2P0
    0b00001100, //     0,     0,     0,  V4N4,  V4N3,  V4N2,  V4N1,  V4N0
    0b00001101, //     0,     0,     0,  V6N4,  V6N3,  V6N2,  V6N1,  V6N0
    0b00100111, //     0,     0,  J0N1,  J0N0, V13N3, V13N2, V13N1, V13N0
    0b00111011, //     0, V20N6, V20N5, V20N4, V20N3, V20N2, V20N1, V20N0
    0b01000100, //     0, V36N2, V36N1, V36N0,     0, V27N2, V27N1, V27N0
    0b01001101, //     0, V43N6, V43N5, V43N4, V43N3, V43N2, V43N1, V43N0
    0b00001011, //     0,     0,  J1N1,  J1N0, V50N3, V50N2, V50N1, V50N0
    0b00010111, //     0,     0,     0, V57N4, V57N3, V57N2, V57N1, V57N0
    0b00010111, //     0,     0,     0, V59N4, V59N3, V59N2, V59N1, V59N0
    0b00011101, //     0,     0, V61N5, V61N4, V61N3, V61N2, V61N1, V61N0
    0b00100001, //     0,     0, V62N5, V62N4, V62N3, V62N2, V62N1, V62N0

    WRITE_COMMAND_8, ST7789_NORON, // 4: Normal display on, no args, w/delay
    END_WRITE,

    DELAY, 10,

    BEGIN_WRITE,
    WRITE_COMMAND_8, ST7789_DISPON, // 5: Main screen turn on, no args, w/delay
    END_WRITE};

class Arduino_ST7789 : public Arduino_TFT
{
public:
  Arduino_ST7789(
      Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0,
      bool ips = false, int16_t w = ST7789_TFTWIDTH, int16_t h = ST7789_TFTHEIGHT,
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
