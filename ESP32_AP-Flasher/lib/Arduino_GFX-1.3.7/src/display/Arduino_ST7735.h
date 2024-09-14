/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 */
#ifndef _ARDUINO_ST7735_H_
#define _ARDUINO_ST7735_H_

#include <Arduino.h>
#include <Print.h>
#include "../Arduino_GFX.h"
#include "../Arduino_TFT.h"

#define ST7735_TFTWIDTH 128
#define ST7735_TFTHEIGHT 160

#define ST7735_RST_DELAY 50     ///< delay ms wait for reset finish
#define ST7735_SLPIN_DELAY 120  ///< delay ms wait for sleep in finish
#define ST7735_SLPOUT_DELAY 120 ///< delay ms wait for sleep out finish

#define ST7735_NOP 0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID 0x04
#define ST7735_RDDST 0x09

#define ST7735_SLPIN 0x10
#define ST7735_SLPOUT 0x11
#define ST7735_PTLON 0x12
#define ST7735_NORON 0x13

#define ST7735_INVOFF 0x20
#define ST7735_INVON 0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON 0x29

#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_RAMWR 0x2C
#define ST7735_RAMRD 0x2E

#define ST7735_PTLAR 0x30
#define ST7735_COLMOD 0x3A
#define ST7735_MADCTL 0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR 0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1 0xC0
#define ST7735_PWCTR2 0xC1
#define ST7735_PWCTR3 0xC2
#define ST7735_PWCTR4 0xC3
#define ST7735_PWCTR5 0xC4
#define ST7735_VMCTR1 0xC5

#define ST7735_RDID1 0xDA
#define ST7735_RDID2 0xDB
#define ST7735_RDID3 0xDC
#define ST7735_RDID4 0xDD

#define ST7735_PWCTR6 0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

#define ST7735_MADCTL_MY 0x80
#define ST7735_MADCTL_MX 0x40
#define ST7735_MADCTL_MV 0x20
#define ST7735_MADCTL_ML 0x10
#define ST7735_MADCTL_RGB 0x00
#define ST7735_MADCTL_MH 0x04
#define ST7735_MADCTL_BGR 0x08

static const uint8_t st7735_init_operations[] = {
    BEGIN_WRITE,
    WRITE_COMMAND_8, ST7735_SLPOUT, // 2: Out of sleep mode, no args, w/delay
    END_WRITE,

    DELAY, ST7735_SLPOUT_DELAY,

    BEGIN_WRITE,
    WRITE_C8_D8, ST7735_COLMOD, 0x05, // 3: Set color mode, 16-bit color

    WRITE_COMMAND_8, ST7735_GMCTRP1, // Gamma Adjustments (pos. polarity), 16 args:
    WRITE_BYTES, 16,
    0x09, 0x16, 0x09, 0x20, // (Not entirely necessary, but provides
    0x21, 0x1B, 0x13, 0x19, //  accurate colors)
    0x17, 0x15, 0x1E, 0x2B,
    0x04, 0x05, 0x02, 0x0E,

    WRITE_COMMAND_8, ST7735_GMCTRN1, // Gamma Adjustments (neg. polarity), 16 args:
    WRITE_BYTES, 16,
    0x0B, 0x14, 0x08, 0x1E, // (Not entirely necessary, but provides
    0x22, 0x1D, 0x18, 0x1E, //  accurate colors)
    0x1B, 0x1A, 0x24, 0x2B,
    0x06, 0x06, 0x02, 0x0F,
    END_WRITE,

    DELAY, 10,

    BEGIN_WRITE,
    WRITE_COMMAND_8, ST7735_NORON, // 5: Normal display on, no args, w/delay
    END_WRITE,

    DELAY, 10,

    BEGIN_WRITE,
    WRITE_COMMAND_8, ST7735_DISPON, // 6: Main screen turn on, no args, w/delay
    END_WRITE};

class Arduino_ST7735 : public Arduino_TFT
{
public:
  Arduino_ST7735(
      Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0,
      bool ips = false, int16_t w = ST7735_TFTWIDTH, int16_t h = ST7735_TFTHEIGHT,
      uint8_t col_offset1 = 0, uint8_t row_offset1 = 0, uint8_t col_offset2 = 0, uint8_t row_offset2 = 0,
      bool bgr = true);

  bool begin(int32_t speed = GFX_NOT_DEFINED) override;
  void writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h) override;
  void setRotation(uint8_t r) override;
  void invertDisplay(bool) override;
  void displayOn() override;
  void displayOff() override;

protected:
  void tftInit() override;
  bool _bgr;

private:
};

#endif
