/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 */
#ifndef _ARDUINO_ILI9225_H_
#define _ARDUINO_ILI9225_H_

#include <Arduino.h>
#include <Print.h>
#include "../Arduino_GFX.h"
#include "../Arduino_TFT.h"

#define ILI9225_TFTWIDTH 176  ///< ILI9225 max TFT width
#define ILI9225_TFTHEIGHT 220 ///< ILI9225 max TFT height

#define ILI9225_RST_DELAY 150    ///< delay ms wait for reset finish
#define ILI9225_SLPIN_DELAY 150  ///< delay ms wait for sleep in finish
#define ILI9225_SLPOUT_DELAY 150 ///< delay ms wait for sleep out finish

#define ILI9225_DRIVER_OUTPUT_CTRL 0x01      // Driver Output Control
#define ILI9225_LCD_AC_DRIVING_CTRL 0x02     // LCD AC Driving Control
#define ILI9225_ENTRY_MODE 0x03              // Entry Mode
#define ILI9225_DISP_CTRL1 0x07              // Display Control 1
#define ILI9225_BLANK_PERIOD_CTRL1 0x08      // Blank Period Control
#define ILI9225_FRAME_CYCLE_CTRL 0x0B        // Frame Cycle Control
#define ILI9225_INTERFACE_CTRL 0x0C          // Interface Control
#define ILI9225_OSC_CTRL 0x0F                // Osc Control
#define ILI9225_POWER_CTRL1 0x10             // Power Control 1
#define ILI9225_POWER_CTRL2 0x11             // Power Control 2
#define ILI9225_POWER_CTRL3 0x12             // Power Control 3
#define ILI9225_POWER_CTRL4 0x13             // Power Control 4
#define ILI9225_POWER_CTRL5 0x14             // Power Control 5
#define ILI9225_VCI_RECYCLING 0x15           // VCI Recycling
#define ILI9225_RAM_ADDR_SET1 0x20           // Horizontal GRAM Address Set
#define ILI9225_RAM_ADDR_SET2 0x21           // Vertical GRAM Address Set
#define ILI9225_GRAM_DATA_REG 0x22           // GRAM Data Register
#define ILI9225_GATE_SCAN_CTRL 0x30          // Gate Scan Control Register
#define ILI9225_VERTICAL_SCROLL_CTRL1 0x31   // Vertical Scroll Control 1 Register
#define ILI9225_VERTICAL_SCROLL_CTRL2 0x32   // Vertical Scroll Control 2 Register
#define ILI9225_VERTICAL_SCROLL_CTRL3 0x33   // Vertical Scroll Control 3 Register
#define ILI9225_PARTIAL_DRIVING_POS1 0x34    // Partial Driving Position 1 Register
#define ILI9225_PARTIAL_DRIVING_POS2 0x35    // Partial Driving Position 2 Register
#define ILI9225_HORIZONTAL_WINDOW_ADDR1 0x36 // Horizontal Address Start Position
#define ILI9225_HORIZONTAL_WINDOW_ADDR2 0x37 // Horizontal Address End Position
#define ILI9225_VERTICAL_WINDOW_ADDR1 0x38   // Vertical Address Start Position
#define ILI9225_VERTICAL_WINDOW_ADDR2 0x39   // Vertical Address End Position
#define ILI9225_GAMMA_CTRL1 0x50             // Gamma Control 1
#define ILI9225_GAMMA_CTRL2 0x51             // Gamma Control 2
#define ILI9225_GAMMA_CTRL3 0x52             // Gamma Control 3
#define ILI9225_GAMMA_CTRL4 0x53             // Gamma Control 4
#define ILI9225_GAMMA_CTRL5 0x54             // Gamma Control 5
#define ILI9225_GAMMA_CTRL6 0x55             // Gamma Control 6
#define ILI9225_GAMMA_CTRL7 0x56             // Gamma Control 7
#define ILI9225_GAMMA_CTRL8 0x57             // Gamma Control 8
#define ILI9225_GAMMA_CTRL9 0x58             // Gamma Control 9
#define ILI9225_GAMMA_CTRL10 0x59            // Gamma Control 10

static const uint8_t ili9225_init_operations[] = {
    BEGIN_WRITE,
    WRITE_C8_D16, ILI9225_LCD_AC_DRIVING_CTRL, 0x01, 0x00,
    WRITE_C8_D16, ILI9225_BLANK_PERIOD_CTRL1, 0x08, 0x08, // set BP and FP
    WRITE_C8_D16, ILI9225_FRAME_CYCLE_CTRL, 0x11, 0x00,   // frame cycle
    WRITE_C8_D16, ILI9225_INTERFACE_CTRL, 0x00, 0x00,     // RGB interface setting R0Ch=0x0110 for RGB 18Bit and R0Ch=0111for RGB16Bit
    WRITE_C8_D16, ILI9225_OSC_CTRL, 0x14, 0x01,           // Set frame rate----0801
    WRITE_C8_D16, ILI9225_VCI_RECYCLING, 0x00, 0x00,      // set system interface
    END_WRITE,

    DELAY, 50,

    //*************Power On sequence ****************//
    BEGIN_WRITE,
    WRITE_C8_D16, ILI9225_POWER_CTRL1, 0x08, 0x00, // Set SAP,DSTB,STB----0A00
    WRITE_C8_D16, ILI9225_POWER_CTRL2, 0x1F, 0x3F, // Set APON,PON,AON,VCI1EN,VC----1038
    END_WRITE,

    DELAY, 50,

    BEGIN_WRITE,
    WRITE_C8_D16, ILI9225_POWER_CTRL3, 0x01, 0x21, // Internal reference voltage= Vci;----1121
    WRITE_C8_D16, ILI9225_POWER_CTRL4, 0x00, 0x6F, // Set GVDD----0066
    WRITE_C8_D16, ILI9225_POWER_CTRL5, 0x43, 0x49, // Set VCOMH/VCOML voltage----5F60
    //-------------- Set GRAM area -----------------//
    WRITE_C8_D16, ILI9225_GATE_SCAN_CTRL, 0x00, 0x00,
    WRITE_C8_D16, ILI9225_VERTICAL_SCROLL_CTRL1, 0x00, 0xDB,
    WRITE_C8_D16, ILI9225_VERTICAL_SCROLL_CTRL2, 0x00, 0x00,
    WRITE_C8_D16, ILI9225_VERTICAL_SCROLL_CTRL3, 0x00, 0x00,
    WRITE_C8_D16, ILI9225_PARTIAL_DRIVING_POS1, 0x00, 0xDB,
    WRITE_C8_D16, ILI9225_PARTIAL_DRIVING_POS2, 0x00, 0x00,
    // ----------- Adjust the Gamma Curve ----------//
    WRITE_C8_D16, ILI9225_GAMMA_CTRL1, 0x00, 0x01,  // 0x0400
    WRITE_C8_D16, ILI9225_GAMMA_CTRL2, 0x20, 0x0B,  // 0x060B
    WRITE_C8_D16, ILI9225_GAMMA_CTRL3, 0x00, 0x00,  // 0x0C0A
    WRITE_C8_D16, ILI9225_GAMMA_CTRL4, 0x04, 0x04,  // 0x0105
    WRITE_C8_D16, ILI9225_GAMMA_CTRL5, 0x0C, 0x0C,  // 0x0A0C
    WRITE_C8_D16, ILI9225_GAMMA_CTRL6, 0x00, 0x0C,  // 0x0B06
    WRITE_C8_D16, ILI9225_GAMMA_CTRL7, 0x01, 0x01,  // 0x0004
    WRITE_C8_D16, ILI9225_GAMMA_CTRL8, 0x04, 0x00,  // 0x0501
    WRITE_C8_D16, ILI9225_GAMMA_CTRL9, 0x11, 0x08,  // 0x0E00
    WRITE_C8_D16, ILI9225_GAMMA_CTRL10, 0x05, 0x0C, // 0x000E
    END_WRITE,

    DELAY, 50,

    BEGIN_WRITE,
    WRITE_C8_D16, ILI9225_DISP_CTRL1, 0x10, 0x17,
    END_WRITE};

class Arduino_ILI9225 : public Arduino_TFT
{
public:
  Arduino_ILI9225(Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0);

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
