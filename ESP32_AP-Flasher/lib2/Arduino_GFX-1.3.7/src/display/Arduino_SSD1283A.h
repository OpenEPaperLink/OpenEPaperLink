/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 * https://github.com/gitcnd/LCDWIKI_SPI.git
 */
#ifndef _ARDUINO_SSD1283A_H_
#define _ARDUINO_SSD1283A_H_

#include <Arduino.h>
#include <Print.h>
#include "../Arduino_GFX.h"
#include "../Arduino_TFT.h"

#define SSD1283A_TFTWIDTH 130  ///< SSD1283A max TFT width
#define SSD1283A_TFTHEIGHT 130 ///< SSD1283A max TFT height

#define SSD1283A_RST_DELAY 120

// http://www.lcdwiki.com/res/MSP1601/SSD1283A%20Datasheet.pdf
#define SSD1283A_OSCILLATOR 0x00
#define SSD1283A_DRIVER_OUTPUT_CONTROL 0x01
#define SSD1283A_LCD_DRIVING_WAVEFORM_CONTROL 0x02
#define SSD1283A_ENTRY_MODE 0x03
#define SSD1283A_COMPARE_REGISTER_1 0x04
#define SSD1283A_COMPARE_REGISTER_2 0x05
#define SSD1283A_DISPLAY_CONTROL 0x07
#define SSD1283A_FRAME_CYCLE_CONTROL 0x0B
#define SSD1283A_POWER_CONTROL_1 0x10
#define SSD1283A_POWER_CONTROL_2 0x11
#define SSD1283A_POWER_CONTROL_3 0x12
#define SSD1283A_POWER_CONTROL_4 0x13
#define SSD1283A_HORIZONTAL_PORCH 0x16
#define SSD1283A_VERTICAL_PORCH 0x17
#define SSD1283A_POWER_CONTROL_5 0x1E
#define SSD1283A_POWER_CONTROL_6 0x1F
#define SSD1283A_RAM_ADDRESS_SET 0x21
#define SSD1283A_WRITE_DATA_TO_GRAM 0x22
#define SSD1283A_READ_DATA_TO_GRAM 0x22
#define SSD1283A_RAM_WRITE_DATA_MASK_1 0x23
#define SSD1283A_RAM_WRITE_DATA_MASK_2 0x24
#define SSD1283A_VCOM_OTP_1 0x28
#define SSD1283A_VCOM_OTP_2 0x29
#define SSD1283A_GAMMA_CONTROL_01 0x30
#define SSD1283A_GAMMA_CONTROL_02 0x31
#define SSD1283A_GAMMA_CONTROL_03 0x32
#define SSD1283A_GAMMA_CONTROL_04 0x33
#define SSD1283A_GAMMA_CONTROL_05 0x34
#define SSD1283A_GAMMA_CONTROL_06 0x35
#define SSD1283A_GAMMA_CONTROL_07 0x36
#define SSD1283A_GAMMA_CONTROL_08 0x37
#define SSD1283A_GAMMA_CONTROL_09 0x38
#define SSD1283A_GAMMA_CONTROL_10 0x39
#define SSD1283A_GATE_SCAN_POSITION 0x40
#define SSD1283A_VERTICAL_SCROLL_CONTROL 0x41
#define SSD1283A_1ST_SCREEN_DRIVING_POSITION 0x42
#define SSD1283A_2ND_SCREEN_DRIVING_POSITION 0x43
#define SSD1283A_HORIZONTAL_RAM_ADDRESS_POSITION 0x44
#define SSD1283A_VERTICAL_RAM_ADDRESS_POSITION 0x45
#define SSD1283A_FURTHER_BIAS_CURRENT_SETTING 0x27
#define SSD1283A_OSCILLATOR_FREQUENCY 0x2C

class Arduino_SSD1283A : public Arduino_TFT
{
public:
  Arduino_SSD1283A(
      Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0,
      int16_t w = SSD1283A_TFTWIDTH, int16_t h = SSD1283A_TFTHEIGHT,
      uint8_t col_offset1 = 2, uint8_t row_offset1 = 2, uint8_t col_offset2 = 2, uint8_t row_offset2 = 2);

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
