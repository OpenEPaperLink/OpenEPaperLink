#ifndef _ARDUINO_ILI9806_H_
#define _ARDUINO_ILI9806_H_

#include <Arduino.h>
#include <Print.h>
#include "../Arduino_GFX.h"
#include "../Arduino_TFT.h"

#define ILI9806_TFTWIDTH 480  ///< ILI9806 max TFT width
#define ILI9806_TFTHEIGHT 854 ///< ILI9806 max TFT height

#define ILI9806_RST_DELAY 120    ///< delay ms wait for reset finish
#define ILI9806_SLPIN_DELAY 120  ///< delay ms wait for sleep in finish
#define ILI9806_SLPOUT_DELAY 120 ///< delay ms wait for sleep out finish

#define ILI9806_NOP 0x00     ///< No-op register
#define ILI9806_SWRESET 0x01 ///< Software reset register
#define ILI9806_RDDID 0x04   ///< Read display identification information
#define ILI9806_RDDST 0x09   ///< Read Display Status

#define ILI9806_SLPIN 0x10  ///< Enter Sleep Mode
#define ILI9806_SLPOUT 0x11 ///< Sleep Out
#define ILI9806_PTLON 0x12  ///< Partial Mode ON
#define ILI9806_NORON 0x13  ///< Normal Display Mode ON

#define ILI9806_RDMODE 0x0A     ///< Read Display Power Mode
#define ILI9806_RDMADCTL 0x0B   ///< Read Display MADCTL
#define ILI9806_RDPIXFMT 0x0C   ///< Read Display Pixel Format
#define ILI9806_RDIMGFMT 0x0D   ///< Read Display Image Format
#define ILI9806_RDSELFDIAG 0x0F ///< Read Display Self-Diagnostic Result

#define ILI9806_INVOFF 0x20   ///< Display Inversion OFF
#define ILI9806_INVON 0x21    ///< Display Inversion ON
#define ILI9806_GAMMASET 0x26 ///< Gamma Set
#define ILI9806_DISPOFF 0x28  ///< Display OFF
#define ILI9806_DISPON 0x29   ///< Display ON

#define ILI9806_CASET 0x2A ///< Column Address Set
#define ILI9806_PASET 0x2B ///< Page Address Set
#define ILI9806_RAMWR 0x2C ///< Memory Write
#define ILI9806_RAMRD 0x2E ///< Memory Read

#define ILI9806_PTLAR 0x30    ///< Partial Area
#define ILI9806_VSCRDEF 0x33  ///< Vertical Scrolling Definition
#define ILI9806_MADCTL 0x36   ///< Memory Access Control
#define ILI9806_VSCRSADD 0x37 ///< Vertical Scrolling Start Address
#define ILI9806_PIXFMT 0x3A   ///< COLMOD: Pixel Format Set

#define ILI9806_FRMCTR1 0xB1 ///< Frame Rate Control (In Normal Mode/Full Colors)
#define ILI9806_FRMCTR2 0xB2 ///< Frame Rate Control (In Idle Mode/8 colors)
#define ILI9806_FRMCTR3 0xB3 ///< Frame Rate control (In Partial Mode/Full Colors)
#define ILI9806_INVCTR 0xB4  ///< Display Inversion Control
#define ILI9806_DFUNCTR 0xB6 ///< Display Function Control

#define ILI9806_PWCTR1 0xC0 ///< Power Control 1
#define ILI9806_PWCTR2 0xC1 ///< Power Control 2
#define ILI9806_PWCTR3 0xC2 ///< Power Control 3
#define ILI9806_PWCTR4 0xC3 ///< Power Control 4
#define ILI9806_PWCTR5 0xC4 ///< Power Control 5
#define ILI9806_VMCTR1 0xC5 ///< VCOM Control 1
#define ILI9806_VMCTR2 0xC7 ///< VCOM Control 2

#define ILI9806_RDID1 0xDA ///< Read ID 1
#define ILI9806_RDID2 0xDB ///< Read ID 2
#define ILI9806_RDID3 0xDC ///< Read ID 3
#define ILI9806_RDID4 0xDD ///< Read ID 4

#define ILI9806_GMCTRP1 0xE0 ///< Positive Gamma Correction
#define ILI9806_GMCTRN1 0xE1 ///< Negative Gamma Correction
#define ILI9806_PWCTR6 0xFC

#define ILI9806_MADCTL_MY 0x80  ///< Bottom to top
#define ILI9806_MADCTL_MX 0x40  ///< Right to left
#define ILI9806_MADCTL_MV 0x20  ///< Reverse Mode
#define ILI9806_MADCTL_ML 0x10  ///< LCD refresh Bottom to top
#define ILI9806_MADCTL_RGB 0x00 ///< Red-Green-Blue pixel order
#define ILI9806_MADCTL_BGR 0x08 ///< Blue-Green-Red pixel order
#define ILI9806_MADCTL_MH 0x04  ///< LCD refresh right to left

static const uint8_t ili9806_init_operations[] = {
    BEGIN_WRITE,
    WRITE_COMMAND_8, 0xFF, // EXTC Command Set enable register
    WRITE_BYTES, 3, 0xFF, 0x98, 0x06,

    WRITE_C8_D8, 0xBA, 0x60, // SPI Interface Setting

    WRITE_COMMAND_8, 0xBC, // GIP 1
    WRITE_BYTES, 21,
    0x01, 0x10, 0x00, 0x00, 0x01,
    0x01, 0x0B, 0x11, 0x32, 0x10,
    0x00, 0x00, 0x01, 0x01, 0x01,
    0x01, 0x50, 0x52, 0x01, 0x00,
    0x40,

    WRITE_COMMAND_8, 0xBD, // GIP 2
    WRITE_BYTES, 8,
    0x01, 0x23, 0x45, 0x67, 0x01,
    0x23, 0x45, 0x67,

    WRITE_COMMAND_8, 0xBE, // GIP 3
    WRITE_BYTES, 9,
    0x00, 0x21, 0xAB, 0x60, 0x22,
    0x22, 0x22, 0x22, 0x22,

    WRITE_C8_D8, 0xC7, 0x30, // VCOM Control

    WRITE_COMMAND_8, 0xED, // EN_volt_reg
    WRITE_BYTES, 3, 0x7F, 0x0F, 0x00,

    WRITE_COMMAND_8, 0xC0,            // Power Control 1
    WRITE_BYTES, 3, 0x03, 0x0B, 0x0C, // 0A VGH VGL

    WRITE_COMMAND_8, 0xFD, // External Power Selection Set
    WRITE_BYTES, 2, 0x0A, 0x00,

    WRITE_C8_D8, 0xFC, 0x08, // LVGL

    WRITE_COMMAND_8, 0xDF, // Engineering Setting
    WRITE_BYTES, 6,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x20,

    WRITE_C8_D8, 0xF3, 0x74, // DVDD Voltage Setting

    WRITE_COMMAND_8, 0xB4, // Display Inversion Control
    WRITE_BYTES, 3, 0x00, 0x00, 0x00,

    WRITE_COMMAND_8, 0xB5, // Blanking Porch Control
    WRITE_BYTES, 2, 0x08, 0x15,

    WRITE_C8_D8, 0xF7, 0x81, // 480x854

    WRITE_COMMAND_8, 0xB1, // Frame Rate Control
    WRITE_BYTES, 3, 0x00, 0x13, 0x13,

    WRITE_COMMAND_8, 0xF2, // Panel Timing Control
    WRITE_BYTES, 4, 0x80, 0x04, 0x40, 0x28,

    WRITE_COMMAND_8, 0xC1, // Power Control 2
    WRITE_BYTES, 3,
    0x17,
    0x71, // VGMP
    0x71, // VGMN

    WRITE_COMMAND_8, 0xE0, // P_Gamma
    WRITE_BYTES, 16,
    0x00, // P1
    0x13, // P2
    0x1A, // P3
    0x0C, // P4
    0x0E, // P5
    0x0B, // P6
    0x07, // P7
    0x05, // P8
    0x05, // P9
    0x0A, // P10
    0x0F, // P11
    0x0F, // P12
    0x0E, // P13
    0x1C, // P14
    0x16, // P15
    0x00, // P16

    WRITE_COMMAND_8, 0xE1, // N_Gamma
    WRITE_BYTES, 16,
    0x00, // P1
    0x13, // P2
    0x1A, // P3
    0x0C, // P4
    0x0E, // P5
    0x0B, // P6
    0x07, // P7
    0x05, // P8
    0x05, // P9
    0x0A, // P10
    0x0F, // P11
    0x0F, // P12
    0x0E, // P13
    0x1C, // P14
    0x16, // P15
    0x00, // P16

    WRITE_C8_D8, 0x3A, 0x55, // 55-16BIT,66-18BIT,77-24BIT

    WRITE_COMMAND_8, 0x11,
    END_WRITE,

    DELAY, ILI9806_SLPOUT_DELAY,

    BEGIN_WRITE,
    WRITE_COMMAND_8, 0x29,
    END_WRITE,

    DELAY, 25,

    BEGIN_WRITE,
    WRITE_COMMAND_8, 0x2C,
    END_WRITE};

class Arduino_ILI9806 : public Arduino_TFT
{
public:
  Arduino_ILI9806(Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0, bool ips = false);

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
