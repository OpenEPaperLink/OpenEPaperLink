/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 */
#ifndef _ARDUINO_HX8357B_H_
#define _ARDUINO_HX8357B_H_

#include <Arduino.h>
#include <Print.h>
#include "../Arduino_GFX.h"
#include "../Arduino_TFT.h"

#define HX8357B_TFTWIDTH 320  ///< HX8357B max TFT width
#define HX8357B_TFTHEIGHT 480 ///< HX8357B max TFT height

#define HX8357B_RST_DELAY 120    ///< delay ms wait for reset finish
#define HX8357B_SLPIN_DELAY 120  ///< delay ms wait for sleep in finish
#define HX8357B_SLPOUT_DELAY 120 ///< delay ms wait for sleep out finish

#define HX8357B_NOP 0x00                               // 6.2.1 NOP
#define HX8357B_SOFTWARE_RESET 0x01                    // 6.2.2 Software Reset (01h)
#define HX8357B_GET_RED_CHANNEL 0x06                   // 6.2.3 Get_red_channel (06h)
#define HX8357B_GET_GREEN_CHANNEL 0x07                 // 6.2.4 Get_green_channel (07h)
#define HX8357B_GET_BLUE_CHANNEL 0x08                  // 6.2.5 Get_blue_channel (08h)
#define HX8357B_GET_POWER_MODE 0x0A                    // 6.2.6 Get_power_mode (0Ah)
#define HX8357B_READ_DISPLAY_MADCTL 0x0B               // 6.2.7 Read Display MADCTL (0Bh)
#define HX8357B_GET_PIXEL_FORMAT 0x0C                  // 6.2.8 Get_pixel_format (0Ch)
#define HX8357B_GET_DISPLAY_MODE 0x0D                  // 6.2.9 Get_display_mode (0Dh)
#define HX8357B_GET_SIGNAL_MODE 0x0E                   // 6.2.10 Get_signal_mode (0Eh)
#define HX8357B_GET_DIAGNOSTIC_RESULT 0x0F             // 6.2.11 Get_diagnostic_result (0Fh)
#define HX8357B_ENTER_SLEEP_MODE 0x10                  // 6.2.12 Enter_sleep_mode (10h)
#define HX8357B_EXIT_SLEEP_MODE 0x11                   // 6.2.13 Exit_sleep_mode (11h)
#define HX8357B_ENTER_PARTIAL_MODE 0x12                // 6.2.14 Enter_partial_mode (12h)
#define HX8357B_ENTER_NORMAL_MODE 0x13                 // 6.2.15 Enter_normal_mode (13h)
#define HX8357B_EXIT_INVERSION_MODE 0x20               // 6.2.16 Exit_inversion_mode (20h)
#define HX8357B_ENTER_INVERSION_MODE 0x21              // 6.2.17 Enter_inversion_mode (21h)
#define HX8357B_SET_DISPLAY_OFF 0x28                   // 6.2.18 Set_display_off (28h)
#define HX8357B_SET_DISPLAY_ON 0x29                    // 6.2.19 Set_display_on (29h)
#define HX8357B_SET_COLUMN_ADDRESS 0x2A                // 6.2.20 Set_column_address (2Ah)
#define HX8357B_SET_PAGE_ADDRESS 0x2B                  // 6.2.21 Set_page_address (2Bh)
#define HX8357B_WRITE_MEMORY_START 0x2C                // 6.2.22 Write_memory_start (2Ch)
#define HX8357B_READ_MEMORY_START 0x2E                 // 6.2.23 Read_memory_start (2Eh)
#define HX8357B_SET_PARTIAL_AREA 0x30                  // 6.2.24 Set_partial_area (30h)
#define HX8357B_SET_SCROLL_AREA 0x33                   // 6.2.25 Set_scroll_area (33h)
#define HX8357B_SET_TEAR_OFF 0x34                      // 6.2.26 Set_tear_off (34h)
#define HX8357B_SET_TEAR_ON 0x35                       // 6.2.27 Set_tear_on (35h)
#define HX8357B_SET_ADDRESS_MODE 0x36                  // 6.2.28 Set_address_mode (36h)
#define HX8357B_SET_SCROLL_START 0x37                  // 6.2.29 Set_scroll_start (37h)
#define HX8357B_EXIT_IDLE_MODE 0x38                    // 6.2.30 Exit_idle_mode (38h)
#define HX8357B_ENTER_IDLE_MODE 0x39                   // 6.2.31 Enter_Idle_mode (39h)
#define HX8357B_SET_PIXEL_FORMAT 0x3A                  // 6.2.32 Set_pixel_format (3Ah)
#define HX8357B_WRITE_MEMORY_CONTINUE 0x3C             // 6.2.33 Write_memory_contiune (3Ch)
#define HX8357B_READ_MEMORY_CONTINUE 0x3E              // 6.2.34 Read_memory_continue (3Eh)
#define HX8357B_SET_TEAR_SCAN_LINES 0x44               // 6.2.35 Set tear scan lines(44h)
#define HX8357B_GET_SCAN_LINES 0x45                    // 6.2.36 Get scan lines(45h)
#define HX8357B_READ_DDB_START 0xA1                    // 6.2.37 Read_DDB_start (A1h)
#define HX8357B_WRITE_DISPLAY_BRIGHTNESS 0x51          // 6.2.38 Write Display Brightness (51h)
#define HX8357B_READ_DISPLAY_BRIGHTNESS 0x52           // 6.2.39 Read Display Brightness Value (52h)
#define HX8357B_WRITE_CTRL_DISPLAY 0x53                // 6.2.40 Write CTRL Display (53h)
#define HX8357B_READ_CTRL_DISPLAY 0x54                 // 6.2.41 Read CTRL Value Display (54h)
#define HX8357B_WRITE_CONTENT_ADAPTIVE_BRIGHTNESS 0x55 // 6.2.42 Write Content Adaptive Brightness Control (55h)
#define HX8357B_READ_CONTENT_ADAPTIVE_BRIGHTNESS 0x56  // 6.2.43 Read Content Adaptive Brightness Control (56h)
#define HX8357B_WRITE_CABC_MINIMUM_BRIGHTNESS 0x5E     // 6.2.44 Write CABC minimum brightness (5Eh)
#define HX8357B_READ_CABC_MINIMUM_BRIGHTNESS 0x5F      // 6.2.45 Read CABC minimum brightness (5Fh)
#define HX8357B_READ_AUTO_BRIGHTNESS_SD_RESULT 0x68    // 6.2.46 Read Automatic Brightness Control Self-Diagnostic Result (68h)
#define HX8357B_SET_EXTENDED_COMMAND_SET 0xB0          // 6.2.47 Set extended command set (B0h)
#define HX8357B_SET_DEEP_STANDBY_MODE 0xB1             // 6.2.48 Set Deep Standby mode (B1h)
#define HX8357B_SET_GRAM_ACCESS_INTERFACE 0xB3         // 6.2.49 Set GRAM access and Interface (B3h)
#define HX8357B_SET_DISPLAY_MODE 0xB4                  // 6.2.50 Set Display mode (B4h)
#define HX8357B_GET_DEVICE_ID 0xBF                     // 6.2.51 Get Device ID (BFh)
#define HX8357B_SET_PANEL_DRIVING 0xC0                 // 6.2.52 Set Panel Driving (C0h)
#define HX8357B_SET_DISPLAY_TIMING_NORMAL 0xC1         // 6.2.53 Set display timing for Normal mode (C1h)
#define HX8357B_SET_DISPLAY_TIMING_PARTIAL 0xC2        // 6.2.54 Set display timing for Partial mode (C2h)
#define HX8357B_SET_DISPLAY_TIMING_IDLE 0xC3           // 6.2.55 Set display timing for Idle mode (C3h)
#define HX8357B_SET_DISPLAY_FRAME 0xC5                 // 6.2.56 Set display frame (C5h)
#define HX8357B_SET_RGB_INTERFACE 0xC6                 // 6.2.57 Set RGB Interface (C6h)
#define HX8357B_SET_GAMMA 0xC8                         // 6.2.58 Set Gamma (C8h)
#define HX8357B_SET_POWER 0xD0                         // 6.2.59 Set Power (D0h)
#define HX8357B_SET_VCOM 0xD1                          // 6.2.60 Set VCOM (D1h)
#define HX8357B_SET_POWER_NORMAL 0xD2                  // 6.2.61 Set Power for Normal mode (D2h)
#define HX8357B_SET_POWER_PARTIAL 0xD3                 // 6.2.62 Set Power for Partial mode (D3h)
#define HX8357B_SET_POWER_IDLE 0xD4                    // 6.2.63 Set Power for Idle mode (D4h)
#define HX8357B_SET_ID 0xE0                            // 6.2.64 Set ID (E0h)
#define HX8357B_SET_OTP_SETTING 0xE2                   // 6.2.65 Set OTP Related Setting (E2h)
#define HX8357B_SETOPKEY 0xE3                          // 6.2.66 SETOTPKEY (E3h)
#define HX8357B_SETCABC 0xE4                           // 6.2.67 SETCABC(E4h)
#define HX8357B_SET_PANEL_RELATED 0xE9                 // 6.2.68 Set Panel related (E9h)
#define HX8357B_SET_EP_FUNCTION 0xEE                   // 6.2.69 Set EQ function (EEh)

#define HX8357B_MADCTL_MY 0x80  ///< Bottom to top
#define HX8357B_MADCTL_MX 0x40  ///< Right to left
#define HX8357B_MADCTL_MV 0x20  ///< Reverse Mode
#define HX8357B_MADCTL_ML 0x10  ///< LCD refresh Bottom to top
#define HX8357B_MADCTL_RGB 0x00 ///< Red-Green-Blue pixel order
#define HX8357B_MADCTL_BGR 0x08 ///< Blue-Green-Red pixel order
#define HX8357B_MADCTL_MH 0x04  ///< LCD refresh right to left

static const uint8_t hx8357b_init_operations[] = {
    BEGIN_WRITE,
    WRITE_COMMAND_8, HX8357B_EXIT_SLEEP_MODE,
    END_WRITE,

    DELAY, HX8357B_SLPOUT_DELAY,

    BEGIN_WRITE,
    WRITE_COMMAND_8, HX8357B_SET_EP_FUNCTION,
    WRITE_BYTES, 4, 0x02, 0x01, 0x02, 0x01,

    WRITE_C8_D8, HX8357B_SET_DISPLAY_MODE, 0x00, // Set RM, DM

    WRITE_COMMAND_8, HX8357B_SET_PANEL_DRIVING,
    WRITE_BYTES, 5,
    0x10, // REV SM GS
    0x3B, // NL[5:0]
    0x00, // SCN[6:0]
    0x02, // NDL 0 PTS[2:0]
    0x11, // PTG ISC[3:0]

    WRITE_C8_D8, HX8357B_SET_DISPLAY_TIMING_NORMAL, 0x10, // ne inversion

    WRITE_COMMAND_8, HX8357B_SET_GAMMA,
    WRITE_BYTES, 12,
    0x00, // KP1,KP0
    0x32, // KP3,KP2
    0x36, // KP5,KP4
    0x45, // RP1,RP0
    0x06, // VRP0  01
    0x16, // VRP1
    0x37, // KN1,KN0
    0x75, // KN3,KN2
    0x77, // KN5,KN4
    0x54, // RN1,RN0
    0x0c, // VRN0
    0x00, // VRN1  01

    WRITE_COMMAND_8, HX8357B_SET_POWER,
    WRITE_BYTES, 3,
    0x44, // DDVDH :5.28
    0x42, // BT VGH:15.84    VGL:-7.92
    0x06, // VREG1  4.625V

    WRITE_C8_D16, HX8357B_SET_VCOM, 0x43, 0x16,         // VCOMH
    WRITE_C8_D16, HX8357B_SET_POWER_NORMAL, 0x04, 0x22, // 12
    WRITE_C8_D16, HX8357B_SET_POWER_PARTIAL, 0x04, 0x12,
    WRITE_C8_D16, HX8357B_SET_POWER_IDLE, 0x07, 0x12,
    WRITE_C8_D8, HX8357B_SET_PANEL_RELATED, 0x00,

    WRITE_C8_D8, HX8357B_SET_DISPLAY_FRAME, 0x08, // 61.51Hz

    WRITE_C8_D8, HX8357B_SET_PIXEL_FORMAT, 0X55,
    END_WRITE,

    DELAY, HX8357B_SLPOUT_DELAY,

    BEGIN_WRITE,
    WRITE_COMMAND_8, HX8357B_SET_DISPLAY_ON, // Display On
    END_WRITE,

    DELAY, 50};

class Arduino_HX8357B : public Arduino_TFT
{
public:
  Arduino_HX8357B(Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0, bool ips = false);

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
