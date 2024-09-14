#ifndef _ARDUINO_ILI9342_H_
#define _ARDUINO_ILI9342_H_

#include <Arduino.h>
#include <Print.h>
#include "../Arduino_GFX.h"
#include "../Arduino_TFT.h"

#define ILI9342_TFTWIDTH 320  ///< ILI9342 max TFT width
#define ILI9342_TFTHEIGHT 240 ///< ILI9342 max TFT height

#define ILI9342_RST_DELAY 150    ///< delay ms wait for reset finish
#define ILI9342_SLPIN_DELAY 150  ///< delay ms wait for sleep in finish
#define ILI9342_SLPOUT_DELAY 150 ///< delay ms wait for sleep out finish

#define ILI9342_NOP 0x00     ///< No-op register
#define ILI9342_SWRESET 0x01 ///< Software reset register
#define ILI9342_RDDID 0x04   ///< Read display identification information
#define ILI9342_RDDST 0x09   ///< Read Display Status

#define ILI9342_SLPIN 0x10  ///< Enter Sleep Mode
#define ILI9342_SLPOUT 0x11 ///< Sleep Out
#define ILI9342_PTLON 0x12  ///< Partial Mode ON
#define ILI9342_NORON 0x13  ///< Normal Display Mode ON

#define ILI9342_RDMODE 0x0A     ///< Read Display Power Mode
#define ILI9342_RDMADCTL 0x0B   ///< Read Display MADCTL
#define ILI9342_RDPIXFMT 0x0C   ///< Read Display Pixel Format
#define ILI9342_RDIMGFMT 0x0D   ///< Read Display Image Format
#define ILI9342_RDSELFDIAG 0x0F ///< Read Display Self-Diagnostic Result

#define ILI9342_INVOFF 0x20   ///< Display Inversion OFF
#define ILI9342_INVON 0x21    ///< Display Inversion ON
#define ILI9342_GAMMASET 0x26 ///< Gamma Set
#define ILI9342_DISPOFF 0x28  ///< Display OFF
#define ILI9342_DISPON 0x29   ///< Display ON

#define ILI9342_CASET 0x2A ///< Column Address Set
#define ILI9342_PASET 0x2B ///< Page Address Set
#define ILI9342_RAMWR 0x2C ///< Memory Write
#define ILI9342_RAMRD 0x2E ///< Memory Read

#define ILI9342_PTLAR 0x30    ///< Partial Area
#define ILI9342_VSCRDEF 0x33  ///< Vertical Scrolling Definition
#define ILI9342_MADCTL 0x36   ///< Memory Access Control
#define ILI9342_VSCRSADD 0x37 ///< Vertical Scrolling Start Address
#define ILI9342_PIXFMT 0x3A   ///< COLMOD: Pixel Format Set

#define ILI9342_FRMCTR1 0xB1 ///< Frame Rate Control (In Normal Mode/Full Colors)
#define ILI9342_FRMCTR2 0xB2 ///< Frame Rate Control (In Idle Mode/8 colors)
#define ILI9342_FRMCTR3 0xB3 ///< Frame Rate control (In Partial Mode/Full Colors)
#define ILI9342_INVCTR 0xB4  ///< Display Inversion Control
#define ILI9342_DFUNCTR 0xB6 ///< Display Function Control

#define ILI9342_PWCTR1 0xC0 ///< Power Control 1
#define ILI9342_PWCTR2 0xC1 ///< Power Control 2
#define ILI9342_PWCTR3 0xC2 ///< Power Control 3
#define ILI9342_PWCTR4 0xC3 ///< Power Control 4
#define ILI9342_PWCTR5 0xC4 ///< Power Control 5
#define ILI9342_VMCTR1 0xC5 ///< VCOM Control 1
#define ILI9342_VMCTR2 0xC7 ///< VCOM Control 2

#define ILI9342_RDID1 0xDA ///< Read ID 1
#define ILI9342_RDID2 0xDB ///< Read ID 2
#define ILI9342_RDID3 0xDC ///< Read ID 3
#define ILI9342_RDID4 0xDD ///< Read ID 4

#define ILI9342_GMCTRP1 0xE0 ///< Positive Gamma Correction
#define ILI9342_GMCTRN1 0xE1 ///< Negative Gamma Correction
#define ILI9342_PWCTR6 0xFC

#define ILI9342_MADCTL_MY 0x80  ///< Bottom to top
#define ILI9342_MADCTL_MX 0x40  ///< Right to left
#define ILI9342_MADCTL_MV 0x20  ///< Reverse Mode
#define ILI9342_MADCTL_ML 0x10  ///< LCD refresh Bottom to top
#define ILI9342_MADCTL_RGB 0x00 ///< Red-Green-Blue pixel order
#define ILI9342_MADCTL_BGR 0x08 ///< Blue-Green-Red pixel order
#define ILI9342_MADCTL_MH 0x04  ///< LCD refresh right to left

static const uint8_t ILI9342_init_operations[] = {
    BEGIN_WRITE,
    WRITE_C8_D8, ILI9342_PWCTR1, 0x23,        // Power control VRH[5:0]
    WRITE_C8_D8, ILI9342_PWCTR2, 0x10,        // Power control SAP[2:0];BT[3:0]
    WRITE_C8_D16, ILI9342_VMCTR1, 0x3e, 0x28, // VCM control
    WRITE_C8_D8, ILI9342_VMCTR2, 0x86,        // VCM control2
    WRITE_C8_D8, ILI9342_VSCRSADD, 0x00,      // Vertical scroll zero
    WRITE_C8_D8, ILI9342_PIXFMT, 0x55,
    WRITE_C8_D16, ILI9342_FRMCTR1, 0x00, 0x18,

    WRITE_COMMAND_8, ILI9342_DFUNCTR, // Display Function Control
    WRITE_BYTES, 3, 0x08, 0x82, 0x27,

    WRITE_COMMAND_8, ILI9342_SLPOUT, // Exit Sleep
    END_WRITE,

    DELAY, ILI9342_SLPOUT_DELAY,

    BEGIN_WRITE,
    WRITE_COMMAND_8, ILI9342_DISPON, // Display on
    END_WRITE};

class Arduino_ILI9342 : public Arduino_TFT
{
public:
  Arduino_ILI9342(Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0, bool ips = false);

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
