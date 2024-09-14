/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 */
#ifndef _ARDUINO_ILI9481_18BIT_H_
#define _ARDUINO_ILI9481_18BIT_H_

#include <Arduino.h>
#include <Print.h>
#include "../Arduino_GFX.h"
#include "../Arduino_TFT_18bit.h"

#define ILI9481_TFTWIDTH 320  ///< ILI9481 max TFT width
#define ILI9481_TFTHEIGHT 480 ///< ILI9481 max TFT height

#define ILI9481_RST_DELAY 150    ///< delay ms wait for reset finish
#define ILI9481_SLPIN_DELAY 150  ///< delay ms wait for sleep in finish
#define ILI9481_SLPOUT_DELAY 150 ///< delay ms wait for sleep out finish

#define ILI9481_NOP 0x00     ///< No-op register
#define ILI9481_SWRESET 0x01 ///< Software reset register

#define ILI9481_SLPIN 0x10  ///< Enter Sleep Mode
#define ILI9481_SLPOUT 0x11 ///< Sleep Out
#define ILI9481_NORON 0x13  ///< Normal Display Mode ON

#define ILI9481_INVOFF 0x20  ///< Display Inversion OFF
#define ILI9481_INVON 0x21   ///< Display Inversion ON
#define ILI9481_DISPOFF 0x28 ///< Display OFF
#define ILI9481_DISPON 0x29  ///< Display ON

#define ILI9481_CASET 0x2A ///< Column Address Set
#define ILI9481_PASET 0x2B ///< Page Address Set
#define ILI9481_RAMWR 0x2C ///< Memory Write
#define ILI9481_RAMRD 0x2E ///< Memory Read

#define ILI9481_MADCTL 0x36 ///< Memory Access Control
#define ILI9481_PIXFMT 0x3A ///< COLMOD: Pixel Format Set

#define ILI9481_MADCTL_MY 0x80  ///< Bottom to top
#define ILI9481_MADCTL_MX 0x40  ///< Right to left
#define ILI9481_MADCTL_MV 0x20  ///< Reverse Mode
#define ILI9481_MADCTL_ML 0x10  ///< LCD refresh Bottom to top
#define ILI9481_MADCTL_RGB 0x00 ///< Red-Green-Blue pixel order
#define ILI9481_MADCTL_BGR 0x08 ///< Blue-Green-Red pixel order
#define ILI9481_MADCTL_HF 0x02
#define ILI9481_MADCTL_VF 0x01

class Arduino_ILI9481_18bit : public Arduino_TFT_18bit
{
public:
  Arduino_ILI9481_18bit(Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0, bool ips = false);

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
