/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 * https://github.com/adafruit/Adafruit_ILI9331.git
 */
#ifndef _ARDUINO_ILI9331_H_
#define _ARDUINO_ILI9331_H_

#include <Arduino.h>
#include <Print.h>
#include "../Arduino_GFX.h"
#include "../Arduino_TFT.h"

#define ILI9331_TFTWIDTH 240  /// ILI9331 max TFT width
#define ILI9331_TFTHEIGHT 320 /// ILI9331 max TFT height

#define ILI9331_RST_DELAY 150

#define ILI9331_DRVCODER 0x00   /// Driver Code Read
#define ILI9331_DRVOUTCTL 0x01  /// Driver Output Control 1
#define ILI9331_DRIVWVCTL 0x02  /// LCD Driving Control
#define ILI9331_ENTRY_MODE 0x03 /// Entry Mode
#define ILI9331_DC1 0x07        /// Display Control 1
#define ILI9331_DC2 0x08        /// Display Control 2
#define ILI9331_DC3 0x09        /// Display Control 3
#define ILI9331_DC4 0x0A        /// Display Control 4
#define ILI9331_RGBDIC1 0x0C    /// RGB Display Interface Control 1
#define ILI9331_FMARK 0x0D      /// Frame Marker Position
#define ILI9331_RGDIC2 0x0F     /// RGB Display Interface Control 2

#define ILI9331_PWCTL1 0x10 /// Power Control 1
#define ILI9331_PWCTL2 0x11 /// Power Control 2
#define ILI9331_PWCTL3 0x12 /// Power Control 3
#define ILI9331_PWCTL4 0x13 /// Power Control 4

#define ILI9331_MC 0x20 /// GRAM Horizontal Address Set
#define ILI9331_MP 0x21 /// GRAM Vertical Address Set
#define ILI9331_MW 0x22 /// Write Data to GRAM

#define ILI9331_PWCTL7 0x29 /// Power Control 7
#define ILI9331_FRCR 0x2B   /// Frame Rate and Color Control

#define ILI9331_HSA 0x50 /// Horizontal RAM Address Position
#define ILI9331_HSE 0x51 /// Horizontal RAM Address Position
#define ILI9331_VSA 0x52 /// Vertical RAM Address Position
#define ILI9331_VSE 0x53 /// Vertical RAM Address Position

#define ILI9331_GSC1 0x60 /// Gate Scan Control
#define ILI9331_GSC2 0x61 /// Gate Scan Control
#define ILI9331_GSC3 0x6A /// Gate Scan Control

#define ILI9331_PANCLT1 0x90 /// Panel Interface Control 1
#define ILI9331_PANCLT2 0x92 /// Panel Interface Control 2
#define ILI9331_PANCLT4 0x95 /// Panel Interface Control 4
#define ILI9331_PANCLT5 0x97 /// Panel Interface Control 5

#define ILI9331_WBRI 0xB1     /// Write Display Brightness Value
#define ILI9331_RBRI 0xB2     /// Read Display Brightness Value
#define ILI9331_WBRICTRL 0xB3 /// Write CTRL Display Value
#define ILI9331_RBRICTRL 0xB4 /// Read CTRL Display Value
#define ILI9331_WCABC 0xB5    /// Write Content Adaptive Brightness Control Value
#define ILI9331_RCABC 0xB6    /// Read Content Adaptive Brightness Control Value
#define ILI9331_WCABCMIN 0xBE /// Write CABC Minimum Brightness
#define ILI9331_RCABCMIN 0xBF /// Read CABC Minimum Brightness
#define ILI9331_CABCCTL1 0xC8 /// CABC Control 1
#define ILI9331_CABCCTL2 0xC9 /// CABC Control 2
#define ILI9331_CABCCTL3 0xCA /// CABC Control 3
#define ILI9331_CABCCTL4 0xCB /// CABC Control 4
#define ILI9331_CABCCTL5 0xCC /// CABC Control 5
#define ILI9331_CABCCTL6 0xCD /// CABC Control 6
#define ILI9331_CABCCTL7 0xCD /// CABC Control 7

static const uint8_t ili9331_init_operations[] = {
    BEGIN_WRITE,
    //************* Start Initial Sequence **********//
    WRITE_C16_D16, 0x00, 0xE7, 0x10, 0x14, // Not sure about this.
                                           // This register is not in datasheet and display inits without it,
                                           // but it mentioned in official code snippet.
    WRITE_C16_D16, 0x00, 0x01, 0x01, 0x00, // set SS and SM bit
    WRITE_C16_D16, 0x00, 0x02, 0x02, 0x00, // set 1 line inversion
    WRITE_C16_D16, 0x00, 0x03, 0x10, 0x30, // set GRAM write direction and BGR=1.
    WRITE_C16_D16, 0x00, 0x08, 0x02, 0x07, // set the back porch and front porch
    WRITE_C16_D16, 0x00, 0x09, 0x00, 0x00, // set non-display area refresh cycle ISC[3:0]
    WRITE_C16_D16, 0x00, 0x0A, 0x00, 0x00, // FMARK function
    WRITE_C16_D16, 0x00, 0x0C, 0x00, 0x00, // RGB interface setting
    WRITE_C16_D16, 0x00, 0x0D, 0x00, 0x00, // Frame marker Position
    WRITE_C16_D16, 0x00, 0x0F, 0x00, 0x00, // RGB interface polarity
    //*************Power On sequence ****************//
    WRITE_C16_D16, 0x00, 0x10, 0x00, 0x00, // SAP, BT[3:0], AP, DSTB, SLP, STB
    WRITE_C16_D16, 0x00, 0x11, 0x00, 0x01, // DC1[2:0], DC0[2:0], VC[2:0]
    WRITE_C16_D16, 0x00, 0x12, 0x00, 0x00, // VREG1OUT voltage
    WRITE_C16_D16, 0x00, 0x13, 0x00, 0x00, // VDV[4:0] for VCOM amplitude
    DELAY, 200,                            // Dis-charge capacitor power voltage
    WRITE_C16_D16, 0x00, 0x10, 0x16, 0x90, // SAP=1, BT=6, APE=1, AP=1, DSTB=0, SLP=0, STB=0
    WRITE_C16_D16, 0x00, 0x11, 0x00, 0x01, // DC1=2, DC0=2, VC=7
    DELAY, 50,                             // wait_ms 50ms
    WRITE_C16_D16, 0x00, 0x12, 0x00, 0x0C, // Internal reference voltage= Vci; VCIRE=1, PON=0, VRH=5
    DELAY, 50,                             // wait_ms 50ms
    WRITE_C16_D16, 0x00, 0x13, 0x07, 0x00, // VDV=28 for VCOM amplitude
    WRITE_C16_D16, 0x00, 0x29, 0x00, 0x05, // VCM=10 for VCOMH
    WRITE_C16_D16, 0x00, 0x2B, 0x00, 0x0D, // Set Frame Rate
    DELAY, 50,                             // wait_ms 50ms
    WRITE_C16_D16, 0x00, 0x20, 0x00, 0x00, // GRAM horizontal Address
    WRITE_C16_D16, 0x00, 0x21, 0x00, 0x00, // GRAM Vertical Address
    // ----------- Adjust the Gamma Curve ----------//
    WRITE_C16_D16, 0x00, 0x30, 0x00, 0x00,
    WRITE_C16_D16, 0x00, 0x31, 0x02, 0x07,
    WRITE_C16_D16, 0x00, 0x32, 0x00, 0x00,
    WRITE_C16_D16, 0x00, 0x35, 0x00, 0x07,
    WRITE_C16_D16, 0x00, 0x36, 0x05, 0x08,
    WRITE_C16_D16, 0x00, 0x37, 0x07, 0x07,
    WRITE_C16_D16, 0x00, 0x38, 0x00, 0x05,
    WRITE_C16_D16, 0x00, 0x39, 0x07, 0x07,
    WRITE_C16_D16, 0x00, 0x3C, 0x02, 0x02,
    WRITE_C16_D16, 0x00, 0x3D, 0x0A, 0x09,
    //------------------ Set GRAM area ---------------//
    WRITE_C16_D16, 0x00, 0x50, 0x00, 0x00, // Horizontal GRAM Start Address
    WRITE_C16_D16, 0x00, 0x51, 0x00, 0xEF, // Horizontal GRAM End Address
    WRITE_C16_D16, 0x00, 0x52, 0x00, 0x00, // Vertical GRAM Start Address
    WRITE_C16_D16, 0x00, 0x53, 0x01, 0x3F, // Vertical GRAM Start Address
    WRITE_C16_D16, 0x00, 0x60, 0xA7, 0x00, // Gate Scan Line GS=0 [0xA700]
    WRITE_C16_D16, 0x00, 0x61, 0x00, 0x01, // NDL,VLE, REV
    WRITE_C16_D16, 0x00, 0x6A, 0x00, 0x00, // set scrolling line
    //-------------- Partial Display Control ---------//
    WRITE_C16_D16, 0x00, 0x80, 0x00, 0x00,
    WRITE_C16_D16, 0x00, 0x81, 0x00, 0x00,
    WRITE_C16_D16, 0x00, 0x82, 0x00, 0x00,
    WRITE_C16_D16, 0x00, 0x83, 0x00, 0x00,
    WRITE_C16_D16, 0x00, 0x84, 0x00, 0x00,
    WRITE_C16_D16, 0x00, 0x85, 0x00, 0x00,
    //-------------- Panel Control -------------------//
    WRITE_C16_D16, 0x00, 0x90, 0x00, 0x10,
    WRITE_C16_D16, 0x00, 0x92, 0x06, 0x00,
    WRITE_C16_D16, 0x00, 0x07, 0x01, 0x33,
    //-------------- Backlight Control ---------------//
    WRITE_C16_D16, 0x00, 0xB3, 0x00, 0x24, // Enable backlight
    WRITE_C16_D16, 0x00, 0xB1, 0x00, 0xFF, // Set maximum brightness
    END_WRITE};

class Arduino_ILI9331 : public Arduino_TFT
{
public:
  Arduino_ILI9331(Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0, bool ips = false);

  bool begin(int32_t speed = GFX_NOT_DEFINED) override;

  void setRotation(uint8_t r) override;

  void writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h) override;

  void invertDisplay(bool) override;
  void displayOn() override;
  void displayOff() override;

protected:
  void tftInit() override;

private:
  uint16_t _MC, _MP, _SC, _EC, _SP, _EP;
};

#endif
