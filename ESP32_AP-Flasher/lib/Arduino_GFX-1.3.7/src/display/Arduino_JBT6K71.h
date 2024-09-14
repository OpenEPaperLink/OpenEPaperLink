/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 * https://github.com/gitcnd/LCDWIKI_SPI.git
 */
#ifndef _ARDUINO_JBT6K71_H_
#define _ARDUINO_JBT6K71_H_

#include <Arduino.h>
#include <Print.h>
#include "../Arduino_GFX.h"
#include "../Arduino_TFT.h"

#define JBT6K71_TFTWIDTH 240  ///< JBT6K71 max TFT width
#define JBT6K71_TFTHEIGHT 320 ///< JBT6K71 max TFT height

#define JBT6K71_RST_DELAY 150

static const uint8_t jbt6k71_init_operations[] = {
    BEGIN_WRITE,
    WRITE_COMMAND_16, 0x00, 0x00, // exiting from deep standby mode
    END_WRITE,

    DELAY, 10, // spec 1ms

    BEGIN_WRITE,
    WRITE_COMMAND_16, 0x00, 0x00,
    END_WRITE,

    DELAY, 10, // spec 1ms

    BEGIN_WRITE,
    WRITE_COMMAND_16, 0x00, 0x00,
    END_WRITE,

    DELAY, 10, // spec 1ms

    BEGIN_WRITE,
    WRITE_C16_D16, 0x00, 0x1d, // mode setting
    0x00, 0x05,                // exit standby
    END_WRITE,

    DELAY, 100, // spec 1ms

    BEGIN_WRITE,
    WRITE_C16_D16, 0x00, 0x00, // oscillation setting
    0x00, 0x01,                // set to on
    END_WRITE,

    DELAY, 100, // spec 1ms

    // Display control
    BEGIN_WRITE,
    WRITE_C16_D16, 0x00, 0x02, // LCD driver AC control
    0x02, 0x00,                // line inversion

    WRITE_C16_D16, 0x00, 0x0d, // FR period adjustment setting
    0x00, 0x11,                // Ffr=60Hz optimized
    END_WRITE,

    DELAY, 100, // spec 1ms

    // LTPS control settings
    BEGIN_WRITE,
    WRITE_C16_D16, 0x00, 0x12, // LTPS control setting 1
    0x03, 0x03,

    WRITE_C16_D16, 0x00, 0x13, // LTPS control setting 2
    0x01, 0x02,

    WRITE_C16_D16, 0x00, 0x1c, // Amplifier capability setting
    0x00, 0x00,                // Maximum

    // Power settings
    WRITE_C16_D16, 0x01, 0x02, // Power supply control (1)
    0x00, 0xf6,                // VCOMD Output voltage: 1.4V(Initial), VCS output voltage: 4.5V, VGM output voltage: 4.3V
    END_WRITE,

    DELAY, 250, // uint8_t max value 255
    DELAY, 250,

    BEGIN_WRITE,
    WRITE_C16_D16, 0x01, 0x03, // Power Supply Control (2)
    0x00, 0x07,                // Boosting clock mode: Dual mode, XVDD output voltage: 5.4V
    END_WRITE,

    DELAY, 100,

    BEGIN_WRITE,
    WRITE_C16_D16, 0x01, 0x05, // Power supply control (4)
    0x01, 0x11,                // Mask period (DCEW1/DCEW2): 1.0 clock, DCCLK frequency for external regulate circuit: 1H, DCCLK frequency for XVDD regulate circuit: 1/2H, DCCLK frequency for AVDD regulate circuit: 1H
    END_WRITE,

    DELAY, 100,

    // Gray scale settings (gamma c
    BEGIN_WRITE,
    WRITE_C16_D16, 0x03, 0x00, 0x02, 0x00, // chan
    WRITE_C16_D16, 0x03, 0x01, 0x00, 0x02, //
    WRITE_C16_D16, 0x03, 0x02, 0x00, 0x00,
    WRITE_C16_D16, 0x03, 0x03, 0x03, 0x00, //
    WRITE_C16_D16, 0x03, 0x04, 0x07, 0x00,
    WRITE_C16_D16, 0x03, 0x05, 0x00, 0x70, //
    WRITE_C16_D16, 0x04, 0x02, 0x00, 0x00, // First screen start, 0
    WRITE_C16_D16, 0x04, 0x03, 0x01, 0x3f, // First screen end, 319
    WRITE_C16_D16, 0x01, 0x00, 0xC0, 0x10, // Display Control
    END_WRITE,

    DELAY, 250, // uint8_t max value 255
    DELAY, 250,

    BEGIN_WRITE,
    WRITE_C16_D16, 0x01, 0x01, 0x00, 0x01, // Auto sequence Control, AUTO
    WRITE_C16_D16, 0x01, 0x00, 0xF7, 0xFE, // Display Control
    END_WRITE,

    DELAY, 250, // uint8_t max value 255
    DELAY, 250,
    DELAY, 250};

class Arduino_JBT6K71 : public Arduino_TFT
{
public:
  Arduino_JBT6K71(
      Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0,
      bool ips = false, int16_t w = JBT6K71_TFTWIDTH, int16_t h = JBT6K71_TFTHEIGHT,
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
