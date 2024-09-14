/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 */
#ifndef _ARDUINO_SSD1331_H_
#define _ARDUINO_SSD1331_H_

#include <Arduino.h>
#include <Print.h>
#include "../Arduino_GFX.h"
#include "../Arduino_TFT.h"

#define SSD1331_TFTWIDTH 96  ///< SSD1331 max TFT width
#define SSD1331_TFTHEIGHT 64 ///< SSD1331 max TFT height

#define SSD1331_RST_DELAY 120

#define SSD1331_DRAWLINE 0x21
#define SSD1331_DRAWRECT 0x22
#define SSD1331_FILL 0x26
#define SSD1331_SETCOLUMN 0x15
#define SSD1331_SETROW 0x75
#define SSD1331_CONTRASTA 0x81
#define SSD1331_CONTRASTB 0x82
#define SSD1331_CONTRASTC 0x83
#define SSD1331_MASTERCURRENT 0x87
#define SSD1331_SETREMAP 0xA0
#define SSD1331_STARTLINE 0xA1
#define SSD1331_DISPLAYOFFSET 0xA2
#define SSD1331_NORMALDISPLAY 0xA4
#define SSD1331_DISPLAYALLON 0xA5
#define SSD1331_DISPLAYALLOFF 0xA6
#define SSD1331_INVERTDISPLAY 0xA7
#define SSD1331_SETMULTIPLEX 0xA8
#define SSD1331_SETMASTER 0xAD
#define SSD1331_DISPLAYOFF 0xAE
#define SSD1331_DISPLAYON 0xAF
#define SSD1331_POWERMODE 0xB0
#define SSD1331_PRECHARGE 0xB1
#define SSD1331_CLOCKDIV 0xB3
#define SSD1331_PRECHARGEA 0x8A
#define SSD1331_PRECHARGEB 0x8B
#define SSD1331_PRECHARGEC 0x8C
#define SSD1331_PRECHARGELEVEL 0xBB
#define SSD1331_VCOMH 0xBE

class Arduino_SSD1331 : public Arduino_TFT
{
public:
  Arduino_SSD1331(
      Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0,
      int16_t w = SSD1331_TFTWIDTH, int16_t h = SSD1331_TFTHEIGHT,
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
