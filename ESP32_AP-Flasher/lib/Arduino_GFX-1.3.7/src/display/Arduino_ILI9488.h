/*
 * start rewrite from:
 * https://github.com/nopnop2002/esp-idf-parallel-tft
 */
#ifndef _ARDUINO_ILI9488_H_
#define _ARDUINO_ILI9488_H_

#include <Arduino.h>
#include <Print.h>
#include "../Arduino_GFX.h"
#include "../Arduino_TFT.h"

#define ILI9488_TFTWIDTH 320  ///< ILI9488 max TFT width
#define ILI9488_TFTHEIGHT 480 ///< ILI9488 max TFT height

#define ILI9488_RST_DELAY 150    ///< delay ms wait for reset finish
#define ILI9488_SLPIN_DELAY 150  ///< delay ms wait for sleep in finish
#define ILI9488_SLPOUT_DELAY 150 ///< delay ms wait for sleep out finish

// Generic commands used by ILI9488_eSPI.cpp
#define ILI9488_NOP 0x00
#define ILI9488_SWRESET 0x01

#define ILI9488_SLPIN 0x10
#define ILI9488_SLPOUT 0x11

#define ILI9488_INVOFF 0x20
#define ILI9488_INVON 0x21

#define ILI9488_DISPOFF 0x28
#define ILI9488_DISPON 0x29

#define ILI9488_CASET 0x2A
#define ILI9488_PASET 0x2B
#define ILI9488_RAMWR 0x2C

#define ILI9488_RAMRD 0x2E

#define ILI9488_MADCTL 0x36

#define ILI9488_MADCTL_MY 0x80
#define ILI9488_MADCTL_MX 0x40
#define ILI9488_MADCTL_MV 0x20
#define ILI9488_MADCTL_ML 0x10
#define ILI9488_MADCTL_RGB 0x00
#define ILI9488_MADCTL_BGR 0x08
#define ILI9488_MADCTL_MH 0x04
#define ILI9488_MADCTL_SS 0x02
#define ILI9488_MADCTL_GS 0x01

static const uint8_t ili9488_init_operations[] = {
    BEGIN_WRITE,

    WRITE_COMMAND_8, 0xE0,
    WRITE_BYTES, 15,
    0x00, 0x03, 0x09, 0x08,
    0x16, 0x0A, 0x3F, 0x78,
    0x4C, 0x09, 0x0A, 0x08,
    0x16, 0x1A, 0x0F,

    WRITE_COMMAND_8, 0xE1,
    WRITE_BYTES, 15,
    0x00, 0x16, 0x19, 0x03,
    0x0F, 0x05, 0x32, 0x45,
    0x46, 0x04, 0x0E, 0x0D,
    0x35, 0x37, 0x0F,

    WRITE_C8_D16, 0XC0, // Power Control 1
    0x17,               // Vreg1out
    0x15,               // Verg2out

    WRITE_C8_D8, 0xC1, // Power Control 2
    0x41,              // VGH,VGL

    WRITE_COMMAND_8, 0xC5, // Power Control 3
    WRITE_BYTES, 3,
    0x00,
    0x12, // Vcom
    0x80,

    WRITE_C8_D8, 0xB0, 0x80, // Interface Mode Control, SDO NOT USE
    WRITE_C8_D8, 0xB1, 0xA0, // Frame rate, 60Hz
    WRITE_C8_D8, 0xB4, 0x02, // Display Inversion Control, 2-dot

    WRITE_C8_D16, 0xB6, // Display Function Control  RGB/MCU Interface Control
    0x02,               // MCU
    0x02,               // Source,Gate scan dieection

    WRITE_C8_D8, 0xE9, 0x00, // Set Image Function, disable 24 bit data

    WRITE_COMMAND_8, 0xF7,                  // Adjust Control
    WRITE_BYTES, 4, 0xA9, 0x51, 0x2C, 0x82, // D7 stream, loose

    WRITE_COMMAND_8, ILI9488_SLPOUT, // Sleep Out
    END_WRITE,

    DELAY, ILI9488_SLPOUT_DELAY,

    BEGIN_WRITE,
    WRITE_COMMAND_8, ILI9488_DISPON, // Display on
    END_WRITE};

class Arduino_ILI9488 : public Arduino_TFT
{
public:
  Arduino_ILI9488(Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0, bool ips = false);

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

#endif // #ifndef _ARDUINO_ILI9488_H_
