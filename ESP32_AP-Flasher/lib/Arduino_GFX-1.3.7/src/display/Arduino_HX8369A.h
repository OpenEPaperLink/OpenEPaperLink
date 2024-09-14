#ifndef _ARDUINO_HX8369A_H_
#define _ARDUINO_HX8369A_H_

#include <Arduino.h>
#include <Print.h>
#include "../Arduino_GFX.h"
#include "../Arduino_TFT.h"

#define HX8369A_TFTWIDTH 480  // HX8369A max TFT width
#define HX8369A_TFTHEIGHT 864 // HX8369A max TFT height

#define HX8369A_RST_DELAY 150 // delay ms wait for reset finish

#define HX8369A_NOP 0x00
#define HX8369A_SWRESET 0x01
#define HX8369A_RDNUMPE 0x05 // Read number of the parity errors
#define HX8369A_GET_RED_CHANNEL 0x06
#define HX8369A_GET_GREEN_CHANNEL 0x07
#define HX8369A_GET_BLUE_CHANNEL 0x08
#define HX8369A_GET_POWER_MODE 0x0A
#define HX8369A_READ_DISPLAY_MADCTL 0x0B
#define HX8369A_GET_PIXEL_FORMAT 0x0C
#define HX8369A_GET_DISPLAY_MODE 0x0D
#define HX8369A_GET_SIGNAL_MODE 0x0E
#define HX8369A_GET_DIAGNOSTIC_RESULT 0x0F
#define HX8369A_ENTER_SLEEP_MODE 0x10
#define HX8369A_EXIT_SLEEP_MODE 0x11
#define HX8369A_ENTER_PARTIAL_MODE 0x12
#define HX8369A_ENTER_NORMAL_MODE 0x13
#define HX8369A_EXIT_INVERSION_MODE 0x20
#define HX8369A_ENTER_INVERSION_MODE 0x21
#define HX8369A_SET_GAMMA_CURVE 0x26
#define HX8369A_SET_DISPLAY_OFF 0x28
#define HX8369A_SET_DISPLAY_ON 0x29
#define HX8369A_SET_CLUMN_ADDRESS 0x2A
#define HX8369A_SET_PAGE_ADDRESS 0x2B
#define HX8369A_WRITE_MEMORY_START 0x2C
#define HX8369A_COLOUR_SET 0x2D
#define HX8369A_RAED_MEMORY_START 0x2E
#define HX8369A_SET_PARTIAL_AREA 0x30
#define HX8369A_SET_SCROLL_AREA 0x33
#define HX8369A_TEARING_EFFECT_LINE_OFF 0x34
#define HX8369A_SET_TEAR_ON 0x35
#define HX8369A_SET_ADDRESS_MODE 0x36
#define HX8369A_SET_SCROLL_START 0x37
#define HX8369A_IDLE_MODE_OFF 0x38
#define HX8369A_ENTER_IDLE_MODE 0x39
#define HX8369A_SET_PIXEL_FORMAT 0x3A
#define HX8369A_WRITE_MEMORY_CONTIUNE 0x3C
#define HX8369A_RAED_MEMORY_CONTINUE 0x3E
#define HX8369A_SET_TEAR_SCAN_LINES 0x44H
#define HX8369A_GET_THE_CURRENT_SCANLINE 0x45
#define HX8369A_WRITE_DISPLAY_BRIGHTNESS 0x51
#define HX8369A_READ_DISPLAY_BRIGHTNESS_VALUE 0x52
#define HX8369A_WRITE_CTRL_DISPLAY 0x53
#define HX8369A_READ_CTRL_VALUE_DISPLAY 0x54
#define HX8369A_WRITE_CONTENT_ADAPTIVE_BRIGHTNESS_CONTROL 0x55
#define HX8369A_READ_CONTENT_ADAPTIVE_BRIGHTNESS_CONTROL 0x56
#define HX8369A_WRITE_CABC_MINIMUM_BRIGHTNESS 0x5E
#define HX8369A_READ_CABC_MINIMUM_BRIGHTNESS 0x5F
#define HX8369A_READ_AUTOMATIC_BRIGHTNESS_CONTROL_SELF_DIAGNOSTIC_RESULT 0x68
#define HX8369A_READ_DDB_START 0xA1
#define HX8369A_READ_DDB_CONTINUE 0xA8
#define HX8369A_READ_ID1 0xDA
#define HX8369A_READ_ID2 0xDB
#define HX8369A_READ_ID3 0xDC
#define HX8369A_SETOSC 0xB0   // Set internal oscillator
#define HX8369A_SETPOWER 0xB1 // Set power
#define HX8369A_SETDISP 0xB2  // Set display related register
#define HX8369A_SETRGBIF 0xB3 // Set RGB interface related register
#define HX8369A_SETCYC 0xB4   // Set display waveform cycle
#define HX8369A_SETVCOM 0xB6  // Set VCOM voltage
#define HX8369A_SETEXTC 0xB9  // Set extension command
#define HX8369A_SETMIPI 0xBA
#define HX8369A_SETOTP 0xBB    // Set OTP
#define HX8369A_SETDGCLUT 0xC1 // Set DGC LUT
#define HX8369A_SETID 0xC3     // Set ID
#define HX8369A_SETPANEL 0xCC
#define HX8369A_SETGIP 0xD5
#define HX8369A_SETTPSNR 0xD8
#define HX8369A_SETGAMMA 0xE0 // Set gamma curve related setting
#define HX8369A_SETOTPKEY 0xE9
#define HX8369A_GETHXID 0xF4
#define HX8369A_SETCNCD_GETCNCD 0xFD
#define HX8369A_SET_SPI_READ_INDEX 0xFE
#define HX8369A_GETSPIREAD 0xFF // Read command data

#define HX8369A_MADCTL_MY 0x80
#define HX8369A_MADCTL_MX 0x40
#define HX8369A_MADCTL_MV 0x20
#define HX8369A_MADCTL_ML 0x20
#define HX8369A_MADCTL_BGR 0x08
#define HX8369A_MADCTL_SS 0x04

static const uint8_t hx8369a_init_operations_part1[] = {
    BEGIN_WRITE,
    WRITE_COMMAND_8, HX8369A_SETEXTC,
    WRITE_BYTES, 3, 0xFF, 0x83, 0x69,

    WRITE_COMMAND_8, HX8369A_SETPOWER, // Set Power
    WRITE_BYTES, 19,
    0x01, 0x00, 0x34, 0x07,
    0x00, 0x01, 0x0F, 0x2A,
    0x32, 0x3F, 0x3F, 0x07,
    0x10, 0x01, 0xE6, 0xE6,
    0xE6, 0xE6, 0xE6,

    WRITE_COMMAND_8, HX8369A_SETDISP,
    WRITE_BYTES, 15,
    0x00, 0x00, 0x03, 0x03,
    0x70, 0x00, 0xFF, 0x00,
    0x00, 0x00, 0x00, 0x03,
    0x03, 0x00, 0x01,

    WRITE_COMMAND_8, HX8369A_SETCYC,
    WRITE_BYTES, 5, 0x00, 0x0C, 0xA0, 0x0E, 0x06,

    WRITE_C8_D16, HX8369A_SETVCOM, 0x2C, 0x2C,

    WRITE_COMMAND_8, HX8369A_SETGIP,
    WRITE_BYTES, 26,
    0x00, 0x05, 0x03, 0x00,
    0x01, 0x09, 0x10, 0x80,
    0x37, 0x37, 0x20, 0x31,
    0x46, 0x8A, 0x57, 0x9B,
    0x20, 0x31, 0x46, 0x8A,
    0x57, 0x9B, 0x07, 0x0F,
    0x02, 0x00,

    WRITE_COMMAND_8, HX8369A_SETGAMMA,
    WRITE_BYTES, 34,
    0x00, 0x08, 0x0D, 0x2D,
    0x34, 0x3F, 0x19, 0x38,
    0x09, 0x0E, 0x0E, 0x12,
    0x14, 0x12, 0x14, 0x13,
    0x19, 0x00, 0x08, 0x0D,
    0x2D, 0x34, 0x3F, 0x19,
    0x38, 0x09, 0x0E, 0x0E,
    0x12, 0x14, 0x12, 0x14,
    0x13, 0x19,

    WRITE_COMMAND_8, HX8369A_SETDGCLUT,
    WRITE_BYTES, 127,
    0x01, 0x02, 0x08, 0x12,
    0x1A, 0x22, 0x2A, 0x31,
    0x36, 0x3F, 0x48, 0x51,
    0x58, 0x60, 0x68, 0x70,
    0x78, 0x80, 0x88, 0x90,
    0x98, 0xA0, 0xA7, 0xAF,
    0xB6, 0xBE, 0xC7, 0xCE,
    0xD6, 0xDE, 0xE6, 0xEF,
    0xF5, 0xFB, 0xFC, 0xFE,
    0x8C, 0xA4, 0x19, 0xEC,
    0x1B, 0x4C, 0x40, 0x02,
    0x08, 0x12, 0x1A, 0x22,
    0x2A, 0x31, 0x36, 0x3F,
    0x48, 0x51, 0x58, 0x60,
    0x68, 0x70, 0x78, 0x80,
    0x88, 0x90, 0x98, 0xA0,
    0xA7, 0xAF, 0xB6, 0xBE,
    0xC7, 0xCE, 0xD6, 0xDE,
    0xE6, 0xEF, 0xF5, 0xFB,
    0xFC, 0xFE, 0x8C, 0xA4,
    0x19, 0xEC, 0x1B, 0x4C,
    0x40, 0x02, 0x08, 0x12,
    0x1A, 0x22, 0x2A, 0x31,
    0x36, 0x3F, 0x48, 0x51,
    0x58, 0x60, 0x68, 0x70,
    0x78, 0x80, 0x88, 0x90,
    0x98, 0xA0, 0xA7, 0xAF,
    0xB6, 0xBE, 0xC7, 0xCE,
    0xD6, 0xDE, 0xE6, 0xEF,
    0xF5, 0xFB, 0xFC, 0xFE,
    0x8C, 0xA4, 0x19, 0xEC,
    0x1B, 0x4C, 0x40,

    WRITE_COMMAND_8, HX8369A_COLOUR_SET};

static const uint8_t hx8369a_init_operations_part2[] = {
    WRITE_C8_D8, HX8369A_SET_TEAR_ON, 0x00,

    WRITE_C8_D8, HX8369A_SET_PIXEL_FORMAT, 0x55,

    WRITE_COMMAND_8, HX8369A_EXIT_SLEEP_MODE,
    END_WRITE,

    DELAY, 120,

    BEGIN_WRITE,
    WRITE_COMMAND_8, HX8369A_SET_DISPLAY_ON, // Display On
    END_WRITE};

class Arduino_HX8369A : public Arduino_TFT
{
public:
  Arduino_HX8369A(
      Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0, bool ips = false,
      int16_t w = HX8369A_TFTWIDTH, int16_t h = HX8369A_TFTHEIGHT,
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
