/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 */
#ifndef _ARDUINO_SEPS525_H_
#define _ARDUINO_SEPS525_H_

#include <Arduino.h>
#include <Print.h>
#include "../Arduino_GFX.h"
#include "../Arduino_TFT.h"

#define SEPS525_TFTWIDTH 160  ///< SEPS525 max TFT width
#define SEPS525_TFTHEIGHT 128 ///< SEPS525 max TFT height

#define SEPS525_RST_DELAY 1

#define SEPS525_INDEX 0x00
#define SEPS525_STATUS_RD 0x01
#define SEPS525_OSC_CTL 0x02
#define SEPS525_IREF 0x80
#define SEPS525_CLOCK_DIV 0x03
#define SEPS525_REDUCE_CURRENT 0x04
#define SEPS525_SOFT_RST 0x05
#define SEPS525_DISP_ON_OFF 0x06
#define SEPS525_PRECHARGE_TIME_R 0x08
#define SEPS525_PRECHARGE_TIME_G 0x09
#define SEPS525_PRECHARGE_TIME_B 0x0A
#define SEPS525_PRECHARGE_CURRENT_R 0x0B
#define SEPS525_PRECHARGE_CURRENT_G 0x0C
#define SEPS525_PRECHARGE_CURRENT_B 0x0D
#define SEPS525_DRIVING_CURRENT_R 0x10
#define SEPS525_DRIVING_CURRENT_G 0x11
#define SEPS525_DRIVING_CURRENT_B 0x12
#define SEPS525_DISPLAY_MODE_SET 0x13
#define SEPS525_RGB_IF 0x14
#define SEPS525_RGB_POL 0x15
#define SEPS525_MEMORY_WRITE_MODE 0x16
#define SEPS525_MX1_ADDR 0x17
#define SEPS525_MX2_ADDR 0x18
#define SEPS525_MY1_ADDR 0x19
#define SEPS525_MY2_ADDR 0x1a
#define SEPS525_M_AP_X 0x20
#define SEPS525_M_AP_Y 0x21
#define SEPS525_RAMWR 0x22

class Arduino_SEPS525 : public Arduino_TFT
{
public:
  Arduino_SEPS525(
      Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0,
      int16_t w = SEPS525_TFTWIDTH, int16_t h = SEPS525_TFTHEIGHT,
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
