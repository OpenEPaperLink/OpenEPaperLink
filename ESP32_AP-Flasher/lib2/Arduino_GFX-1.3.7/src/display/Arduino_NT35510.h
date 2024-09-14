/*
 * start rewrite from:
 * https://github.com/hi631/LCD_NT35510-MRB3971.git
 */
#ifndef _ARDUINO_NT35510_H_
#define _ARDUINO_NT35510_H_

#include <Arduino.h>
#include <Print.h>
#include "../Arduino_GFX.h"
#include "../Arduino_TFT.h"

#define NT35510_TFTWIDTH 480  ///< NT35510 max TFT width
#define NT35510_TFTHEIGHT 800 ///< NT35510 max TFT height

#define NT35510_RST_DELAY 500    ///< delay ms wait for reset finish
#define NT35510_SLPIN_DELAY 120  ///< delay ms wait for sleep in finish
#define NT35510_SLPOUT_DELAY 120 ///< delay ms wait for sleep out finish

#define NT35510_NOP 0x0000
#define NT35510_SWRESET 0x0100
#define NT35510_RDNUMED 0x0500
#define NT35510_RDDPM 0x0A00
#define NT35510_RDDMADCTR 0x0B00
#define NT35510_RDDCOLMOD 0x0C00
#define NT35510_RDDIM 0x0D00
#define NT35510_RDDSM 0x0E00
#define NT35510_RDDSDR 0x0F00

#define NT35510_SLPIN 0x1000
#define NT35510_SLPOUT 0x1100
#define NT35510_PTLON 0x1200
#define NT35510_NORON 0x1300

#define NT35510_INVOFF 0x2000
#define NT35510_INVON 0x2100
#define NT35510_ALLPOFF 0x2200
#define NT35510_ALLPON 0x2300
#define NT35510_GAMSET 0x2600
#define NT35510_DISPOFF 0x2800
#define NT35510_DISPON 0x2900
#define NT35510_CASET 0x2A00
#define NT35510_PASET 0x2B00
#define NT35510_RAMWR 0x2C00
#define NT35510_RAMRD 0x2E00

#define NT35510_PTLAR 0x3000
#define NT35510_TEOFF 0x3400
#define NT35510_TEON 0x3500
#define NT35510_MADCTR 0x3600
#define NT35510_IDMOFF 0x3800
#define NT35510_IDMON 0x3900
#define NT35510_COLMOD 0x3A00
#define NT35510_RAMWRCNT 0x3C00
#define NT35510_RAMRDCNT 0x3E00

#define NT35510_WRTESCN 0x4400
#define NT35510_RDSCNL 0x4500

#define NT35510_WRDISBV 0x5100
#define NT35510_RDDISBV 0x5200
#define NT35510_WRCTRLD 0x5300
#define NT35510_RDCTRLD 0x5400
#define NT35510_WRCABC 0x5500
#define NT35510_RDCABC 0x5600
#define NT35510_WRCABCMB 0x5E00
#define NT35510_RDCABCMB 0x5F00

#define NT35510_RDPWMSDR 0x6800

#define NT35510_RDBWLB 0x7000
#define NT35510_RDBKX 0x7100
#define NT35510_RDBKY 0x7200
#define NT35510_RDWX 0x7300
#define NT35510_RDWY 0x7400
#define NT35510_RDRGLB 0x7500
#define NT35510_RDRX 0x7600
#define NT35510_RDRY 0x7700
#define NT35510_RDGX 0x7800
#define NT35510_RDGY 0x7900
#define NT35510_RDBALB 0x7A00
#define NT35510_RDBX 0x7B00
#define NT35510_RDBY 0x7C00
#define NT35510_RDAX 0x7D00
#define NT35510_RDAY 0x7E00

#define NT35510_RDDDBSTR 0xA100
#define NT35510_RDDDBCNT 0xA800
#define NT35510_RDFCS 0xAA00
#define NT35510_RDCCS 0xAF00

#define NT35510_RDID1 0xDA00
#define NT35510_RDID2 0xDB00
#define NT35510_RDID3 0xDC00

#define NT35510_MADCTL_MY 0x80
#define NT35510_MADCTL_MX 0x40
#define NT35510_MADCTL_MV 0x20

class Arduino_NT35510 : public Arduino_TFT
{
public:
  Arduino_NT35510(
      Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0,
      bool ips = false, int16_t w = NT35510_TFTWIDTH, int16_t h = NT35510_TFTHEIGHT,
      uint8_t col_offset1 = 0, uint8_t row_offset1 = 0, uint8_t col_offset2 = 0, uint8_t row_offset2 = 0);

  bool begin(int32_t speed = GFX_NOT_DEFINED) override;

  void setRotation(uint8_t r) override;

  void writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h) override;

  void invertDisplay(bool) override;
  void displayOn() override;
  void displayOff() override;

protected:
  void WriteRegM(uint16_t adr, uint16_t len, uint8_t dat[]);
  void tftInit() override;

private:
};

#endif
