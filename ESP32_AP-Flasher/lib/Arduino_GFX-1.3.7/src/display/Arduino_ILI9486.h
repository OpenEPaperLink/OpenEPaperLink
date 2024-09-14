/*
 * start rewrite from:
 * https://github.com/nopnop2002/esp-idf-parallel-tft
 */
#ifndef _ARDUINO_ILI9486_H_
#define _ARDUINO_ILI9486_H_

#include <Arduino.h>
#include <Print.h>
#include "../Arduino_GFX.h"
#include "../Arduino_TFT.h"

#define ILI9486_TFTWIDTH 320  ///< ILI9486 max TFT width
#define ILI9486_TFTHEIGHT 480 ///< ILI9486 max TFT height

#define ILI9486_RST_DELAY 120    ///< delay ms wait for reset finish
#define ILI9486_SLPIN_DELAY 120  ///< delay ms wait for sleep in finish
#define ILI9486_SLPOUT_DELAY 150 ///< delay ms wait for sleep out finish

#define ILI9486_NOP 0x00     ///< No-op register
#define ILI9486_SWRESET 0x01 ///< Software reset register
#define ILI9486_RDDID 0x04   ///< Read display identification information
#define ILI9486_RDDST 0x09   ///< Read Display Status

#define ILI9486_SLPIN 0x10  ///< Enter Sleep Mode
#define ILI9486_SLPOUT 0x11 ///< Sleep Out
#define ILI9486_PTLON 0x12  ///< Partial Mode ON
#define ILI9486_NORON 0x13  ///< Normal Display Mode ON

#define ILI9486_RDMODE 0x0A     ///< Read Display Power Mode
#define ILI9486_RDMADCTL 0x0B   ///< Read Display MADCTL
#define ILI9486_RDPIXFMT 0x0C   ///< Read Display Pixel Format
#define ILI9486_RDIMGFMT 0x0D   ///< Read Display Image Format
#define ILI9486_RDSELFDIAG 0x0F ///< Read Display Self-Diagnostic Result

#define ILI9486_INVOFF 0x20   ///< Display Inversion OFF
#define ILI9486_INVON 0x21    ///< Display Inversion ON
#define ILI9486_GAMMASET 0x26 ///< Gamma Set
#define ILI9486_DISPOFF 0x28  ///< Display OFF
#define ILI9486_DISPON 0x29   ///< Display ON

#define ILI9486_CASET 0x2A ///< Column Address Set
#define ILI9486_PASET 0x2B ///< Page Address Set
#define ILI9486_RAMWR 0x2C ///< Memory Write
#define ILI9486_RAMRD 0x2E ///< Memory Read

#define ILI9486_MADCTL 0x36 ///< Memory Access Control
#define ILI9486_PIXFMT 0x3A ///< COLMOD: Pixel Format Set

#define ILI9486_GMCTRP1 0xE0 ///< Positive Gamma Correction
#define ILI9486_GMCTRN1 0xE1 ///< Negative Gamma Correction

#define ILI9486_MADCTL_MY 0x80  ///< Bottom to top
#define ILI9486_MADCTL_MX 0x40  ///< Right to left
#define ILI9486_MADCTL_MV 0x20  ///< Reverse Mode
#define ILI9486_MADCTL_ML 0x10  ///< LCD refresh Bottom to top
#define ILI9486_MADCTL_RGB 0x00 ///< Red-Green-Blue pixel order
#define ILI9486_MADCTL_BGR 0x08 ///< Blue-Green-Red pixel order
#define ILI9486_MADCTL_MH 0x04  ///< LCD refresh right to left
#define ILI9486_MADCTL_SS 0x02
#define ILI9486_MADCTL_GS 0x01

static const uint8_t ili9486_init_operations[] = {
    BEGIN_WRITE,
    WRITE_COMMAND_8, ILI9486_SWRESET,
    END_WRITE,

    DELAY, ILI9486_RST_DELAY,

    BEGIN_WRITE,
    WRITE_COMMAND_8, ILI9486_SLPOUT,
    END_WRITE,

    DELAY, ILI9486_SLPOUT_DELAY,

    BEGIN_WRITE,
    WRITE_C8_D8, ILI9486_PIXFMT, 0x55, // 16 bit colour interface
    WRITE_C8_D8, 0xC2, 0x44,
    WRITE_COMMAND_8, 0xC5,
    WRITE_BYTES, 4, 0x00, 0x00, 0x00, 0x00,
    WRITE_COMMAND_8, 0xE0,
    WRITE_BYTES, 15,
    0x0F, 0x1F, 0x1C, 0x0C, 0x0F,
    0x08, 0x48, 0x98, 0x37, 0x0A,
    0x13, 0x04, 0x11, 0x0D, 0x00,
    WRITE_COMMAND_8, 0xE1,
    WRITE_BYTES, 15,
    0x0F, 0x32, 0x2E, 0x0B, 0x0D,
    0x05, 0x47, 0x75, 0x37, 0x06,
    0x10, 0x03, 0x24, 0x20, 0x00,
    WRITE_C8_D8, 0x36, 0x48,
    WRITE_COMMAND_8, 0x29, // display on
    END_WRITE,
    DELAY, ILI9486_SLPOUT_DELAY};

class Arduino_ILI9486 : public Arduino_TFT
{
public:
  Arduino_ILI9486(Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0, bool ips = false);

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
