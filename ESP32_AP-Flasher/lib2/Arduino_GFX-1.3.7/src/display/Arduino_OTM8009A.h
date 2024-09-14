#ifndef _ARDUINO_OTM8009A_H_
#define _ARDUINO_OTM8009A_H_

#include <Arduino.h>
#include <Print.h>
#include "../Arduino_GFX.h"
#include "../Arduino_TFT.h"

#define OTM8009A_TFTWIDTH 480  ///< OTM8009A max TFT width
#define OTM8009A_TFTHEIGHT 800 ///< OTM8009A max TFT height

#define OTM8009A_RST_DELAY 500    ///< delay ms wait for reset finish
#define OTM8009A_SLPIN_DELAY 120  ///< delay ms wait for sleep in finish
#define OTM8009A_SLPOUT_DELAY 120 ///< delay ms wait for sleep out finish

#define OTM8009A_NOP 0x0000
#define OTM8009A_SWRESET 0x0100
#define OTM8009A_RDNUMED 0x0500
#define OTM8009A_RDDPM 0x0A00
#define OTM8009A_RDDMADCTR 0x0B00
#define OTM8009A_RDDCOLMOD 0x0C00
#define OTM8009A_RDDIM 0x0D00
#define OTM8009A_RDDSM 0x0E00
#define OTM8009A_RDDSDR 0x0F00

#define OTM8009A_SLPIN 0x1000
#define OTM8009A_SLPOUT 0x1100
#define OTM8009A_PTLON 0x1200
#define OTM8009A_NORON 0x1300

#define OTM8009A_INVOFF 0x2000
#define OTM8009A_INVON 0x2100
#define OTM8009A_ALLPOFF 0x2200
#define OTM8009A_ALLPON 0x2300
#define OTM8009A_GAMSET 0x2600
#define OTM8009A_DISPOFF 0x2800
#define OTM8009A_DISPON 0x2900
#define OTM8009A_CASET 0x2A00
#define OTM8009A_PASET 0x2B00
#define OTM8009A_RAMWR 0x2C00
#define OTM8009A_RAMRD 0x2E00

#define OTM8009A_PTLAR 0x3000
#define OTM8009A_TEOFF 0x3400
#define OTM8009A_TEON 0x3500
#define OTM8009A_MADCTR 0x3600
#define OTM8009A_IDMOFF 0x3800
#define OTM8009A_IDMON 0x3900
#define OTM8009A_COLMOD 0x3A00
#define OTM8009A_RAMWRCNT 0x3C00
#define OTM8009A_RAMRDCNT 0x3E00

#define OTM8009A_WRTESCN 0x4400
#define OTM8009A_RDSCNL 0x4500

#define OTM8009A_WRDISBV 0x5100
#define OTM8009A_RDDISBV 0x5200
#define OTM8009A_WRCTRLD 0x5300
#define OTM8009A_RDCTRLD 0x5400
#define OTM8009A_WRCABC 0x5500
#define OTM8009A_RDCABC 0x5600
#define OTM8009A_WRCABCMB 0x5E00
#define OTM8009A_RDCABCMB 0x5F00

#define OTM8009A_RDPWMSDR 0x6800

#define OTM8009A_RDBWLB 0x7000
#define OTM8009A_RDBKX 0x7100
#define OTM8009A_RDBKY 0x7200
#define OTM8009A_RDWX 0x7300
#define OTM8009A_RDWY 0x7400
#define OTM8009A_RDRGLB 0x7500
#define OTM8009A_RDRX 0x7600
#define OTM8009A_RDRY 0x7700
#define OTM8009A_RDGX 0x7800
#define OTM8009A_RDGY 0x7900
#define OTM8009A_RDBALB 0x7A00
#define OTM8009A_RDBX 0x7B00
#define OTM8009A_RDBY 0x7C00
#define OTM8009A_RDAX 0x7D00
#define OTM8009A_RDAY 0x7E00

#define OTM8009A_RDDDBSTR 0xA100
#define OTM8009A_RDDDBCNT 0xA800
#define OTM8009A_RDFCS 0xAA00
#define OTM8009A_RDCCS 0xAF00

#define OTM8009A_RDID1 0xDA00
#define OTM8009A_RDID2 0xDB00
#define OTM8009A_RDID3 0xDC00

#define OTM8009A_MADCTL_MY 0x80
#define OTM8009A_MADCTL_MX 0x40
#define OTM8009A_MADCTL_MV 0x20

class Arduino_OTM8009A : public Arduino_TFT
{
public:
  Arduino_OTM8009A(
      Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0,
      bool ips = false, int16_t w = OTM8009A_TFTWIDTH, int16_t h = OTM8009A_TFTHEIGHT,
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
