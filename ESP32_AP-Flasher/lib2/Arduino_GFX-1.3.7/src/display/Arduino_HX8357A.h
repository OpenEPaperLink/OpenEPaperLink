#ifndef _ARDUINO_HX8357A_H_
#define _ARDUINO_HX8357A_H_

#include <Arduino.h>
#include <Print.h>
#include "../Arduino_GFX.h"
#include "../Arduino_TFT.h"

#define HX8357A_TFTWIDTH 320  // HX8357A max TFT width
#define HX8357A_TFTHEIGHT 480 // HX8357A max TFT height

#define HX8357A_RST_DELAY 1200 // delay ms wait for reset finish

#define HX8357A_HIMAX_ID 0x00                         // Himax ID
#define HX8357A_DISPLAY_MODE_CONTROL 0x01             // Display Mode control
#define HX8357A_COLUMN_ADDRESS_START_2 0x02           // Column address start 2
#define HX8357A_COLUMN_ADDRESS_START_1 0x03           // Column address start 1
#define HX8357A_COLUMN_ADDRESS_END_2 0x04             // Column address end 2
#define HX8357A_COLUMN_ADDRESS_END_1 0x05             // Column address end 1
#define HX8357A_ROW_ADDRESS_START_2 0x06              // Row address start 2
#define HX8357A_ROW_ADDRESS_START_1 0x07              // Row address start 1
#define HX8357A_ROW_ADDRESS_END_2 0x08                // Row address end 2
#define HX8357A_ROW_ADDRESS_END_1 0x09                // Row address end 1
#define HX8357A_PARTIAL_ASREA_START_ROW_2 0x0A        // Partial area start row 2
#define HX8357A_PARTIAL_AREA_START_ROW_1 0x0B         // Partial area start row 1
#define HX8357A_PARTIAL_AREA_END_ROW_2 0x0C           // Partial area end row 2
#define HX8357A_PARTIAL_AREA_END_ROW_1 0x0D           // Partial area end row 1
#define HX8357A_VERTICAL_SCROLL_TOP_FIXED_AREA_2 0x0E // Vertical Scroll Top fixed area 2
#define HX8357A_VERTICAL_SCROLL_TOP_FIXED_AREA_1 0x0F // Vertical Scroll Top fixed area 1
#define HX8357A_VERTICAL_SCROLL_HEIGHT_AREA_2 0x10    // Vertical Scroll height area 2
#define HX8357A_VERTICAL_SCROLL_HEIGHT_AREA_1 0x11    // Vertical Scroll height area 1
#define HX8357A_VERTICAL_SCROLL_BUTTON_AREA_2 0x12    // Vertical Scroll Button area 2
#define HX8357A_VERTICAL_SCROLL_BUTTON_AREA_1 0x13    // Vertical Scroll Button area 1
#define HX8357A_VERTICAL_SCROLL_START_ADDRESS_2 0x14  // Vertical Scroll Start address 2
#define HX8357A_VERTICAL_SCROLL_START_ADDRESS_1 0x15  // Vertical Scroll Start address 1
#define HX8357A_MEMORY_ACCESS_CONTROL 0x16            // Memory Access control
#define HX8357A_COLMOD 0x17                           // COLMOD
#define HX8357A_OSC_CONTROL_1 0x18                    // OSC Control 1
#define HX8357A_OSC_CONTROL_2 0x19                    // OSC Control 2
#define HX8357A_POWER_CONTROL_6 0x1A                  // Power Control 6
#define HX8357A_POWER_CONTROL_5 0x1B                  // Power Control 5
#define HX8357A_POWER_CONTROL_4 0x1C                  // Power Control 4
#define HX8357A_POWER_CONTROL_3 0x1D                  // Power Control 3
#define HX8357A_POWER_CONTROL_2 0x1E                  // Power Control 2
#define HX8357A_POWER_CONTROL_1 0x1F                  // Power Control 1
#define HX8357A_SRAM_CONTROL 0x22                     // SRAM Control
#define HX8357A_VCOM_CONTROL_1 0x23                   // VCOM Control 1
#define HX8357A_VCOM_CONTROL_2 0x24                   // VCOM Control 2
#define HX8357A_VCOM_CONTROL_3 0x25                   // VCOM Control 3
#define HX8357A_DISPLAY_CONTROL_1 0x26                // Display Control 1
#define HX8357A_DISPLAY_CONTROL_2 0x27                // Display Control 2
#define HX8357A_DISPLAY_CONTROL_3 0x28                // Display Control 3
#define HX8357A_FRAME_RATE_CONTROL_1 0x29             // Frame Rate control 1
#define HX8357A_FRAME_RATE_CONTROL_2 0x2A             // Frame Rate Control 2
#define HX8357A_FRAME_RATE_CONTROL_3 0x2B             // Frame Rate Control 3
#define HX8357A_FRAME_RATE_CONTROL_4 0x2C             // Frame Rate Control 4
#define HX8357A_CYCLE_CONTROL_2 0x2D                  // Cycle Control 2
#define HX8357A_CYCLE_CONTROL_3 0x2E                  // Cycle Control 3
#define HX8357A_DISPLAY_INVERSION 0x2F                // Display inversion
#define HX8357A_RGB_INTERFACE_CONTROL_1 0x31          // RGB interface control 1
#define HX8357A_RGB_INTERFACE_CONTROL_2 0x32          // RGB interface control 2
#define HX8357A_RGB_INTERFACE_CONTROL_3 0x33          // RGB interface control 3
#define HX8357A_RGB_INTERFACE_CONTROL_4 0x34          // RGB interface control 4
#define HX8357A_PANEL_CHARACTERIC 0x36                // Panel Characteric
#define HX8357A_OTP_CONTROL_1 0x38                    // OTP Control 1
#define HX8357A_OTP_CONTROL_2 0x39                    // OTP Control 2
#define HX8357A_OTP_CONTROL_3 0x3A                    // OTP Control 3
#define HX8357A_OTP_CONTROL_4 0x3B                    // OTP Control 4
#define HX8357A_CABC_CONTROL_1 0x3C                   // CABC Control 1
#define HX8357A_CABC_CONTROL_2 0x3D                   // CABC Control 2
#define HX8357A_CABC_CONTROL_3 0x3E                   // CABC Control 3
#define HX8357A_CABC_CONTROL_4 0x3F                   // CABC Control 4
#define HX8357A_R1_CONTROL_1 0x40                     // r1 Control (1)
#define HX8357A_R1_CONTROL_2 0x41                     // r1 Control (2)
#define HX8357A_R1_CONTROL_3 0x42                     // r1 Control (3)
#define HX8357A_R1_CONTROL_4 0x43                     // r1 Control (4)
#define HX8357A_R1_CONTROL_5 0x44                     // r1 Control (5)
#define HX8357A_R1_CONTROL_6 0x45                     // r1 Control (6)
#define HX8357A_R1_CONTROL_7 0x46                     // r1 Control (7)
#define HX8357A_R1_CONTROL_8 0x47                     // r1 Control (8)
#define HX8357A_R1_CONTROL_9 0x48                     // r1 Control (9)
#define HX8357A_R1_CONTROL_10 0x49                    // r1 Control (10)
#define HX8357A_R1_CONTROL_11 0x4A                    // r1 Control (11)
#define HX8357A_R1_CONTROL_12 0x4B                    // r1 Control (12)
#define HX8357A_R1_CONTROL_13 0x4C                    // r1 Control (13)
#define HX8357A_R1_CONTROL_18 0x50                    // r1 Control (18)
#define HX8357A_R1_CONTROL_19 0x51                    // r1 Control (19)
#define HX8357A_R1_CONTROL_20 0x52                    // r1 Control (20)
#define HX8357A_R1_CONTROL_21 0x53                    // r1 Control (21)
#define HX8357A_R1_CONTROL_22 0x54                    // r1 Control (22)
#define HX8357A_R1_CONTROL_23 0x55                    // r1 Control (23)
#define HX8357A_R1_CONTROL_24 0x56                    // r1 Control (24)
#define HX8357A_R1_CONTROL_25 0x57                    // r1 Control (25)
#define HX8357A_R1_CONTROL_26 0x58                    // r1 Control (26)
#define HX8357A_R1_CONTROL_27 0x59                    // r1 Control (27)
#define HX8357A_R1_CONTROL_28 0x5A                    // r1 Control (28)
#define HX8357A_R1_CONTROL_29 0x5B                    // r1 Control (29)
#define HX8357A_R1_CONTROL_30 0x5C                    // r1 Control (30)
#define HX8357A_R1_CONTROL_35 0x5D                    // r1 Control (35)
#define HX8357A_TE_CONTROL 0x60                       // TE Control
#define HX8357A_ID1 0x61                              // ID1
#define HX8357A_ID2 0x62                              // ID2
#define HX8357A_ID3 0x63                              // ID3
#define HX8357A_ID4 0x64                              // ID4
#define HX8357A_MDDI_CONTROL_4 0x68                   // MDDI Control 4
#define HX8357A_MDDI_CONTROL_5 0x69                   // MDDI Control 5
#define HX8357A_GPIO_CONTROL_1 0x6B                   // GPIO Control 1
#define HX8357A_GPIO_CONTROL_2 0x6C                   // GPIO Control 2
#define HX8357A_GPIO_CONTROL_3 0x6D                   // GPIO Control 3
#define HX8357A_GPIO_CONTROL_4 0x6E                   // GPIO Control 4
#define HX8357A_GPIO_CONTROL_5 0x6F                   // GPIO Control 5
#define HX8357A_SUB_PANEL_CONTROL_1 0x70              // SUB_PANEL Control 1
#define HX8357A_SUB_PANEL_CONTROL_2 0x71              // SUB_PANEL Control 2
#define HX8357A_SUB_PANEL_CONTROL_3 0x72              // SUB_PANEL Control 3
#define HX8357A_SUB_PANEL_CONTROL_4 0x73              // SUB_PANEL Control 4
#define HX8357A_COLUMN_ADDRESS_COUNTER_2 0x80         // Column address counter 2
#define HX8357A_COLUMN_ADDRESS_COUNTER_1 0x81         // Column address counter 1
#define HX8357A_ROW_ADDRESS_COUNTER_2 0x82            // Row address counter 2
#define HX8357A_ROW_ADDRESS_COUNTER_1 0x83            // Row address counter 1
#define HX8357A_SET_VREF 0xE2                         // Set VREF
#define HX8357A_POWER_SAVING_COUNTER_1 0xE4           // Power saving counter 1
#define HX8357A_POWER_SAVING_COUNTER_2 0xE5           // Power saving counter 2
#define HX8357A_POWER_SAVING_COUNTER_3 0xE6           // Power saving counter 3
#define HX8357A_POWER_SAVING_COUNTER_4 0xE7           // Power saving counter 4
#define HX8357A_OPON_CONTROL_1 0xE8                   // OPON Control 1
#define HX8357A_OPON_CONTROL_2 0xE9                   // OPON Control 2
#define HX8357A_CENON_CONTROL 0xF2                    // GENON Control
#define HX8357A_SET_SPI_RRAD_INDEX 0xFD               // Set SPI Rrad Index
#define HX8357A_GET_SPI_INDEX_DATA 0xFE               // Get SPI Index data
#define HX8357A_PAGE_SELECT 0xFF                      // Page select

#define HX8357A_MADCTL_MY 0x80  // PAGE ADDRESS ORDER
#define HX8357A_MADCTL_MX 0x40  // COLUMN ADDRESS ORDER
#define HX8357A_MADCTL_MV 0x20  // PAGE/COLUMN SELECTION
#define HX8357A_MADCTL_BGR 0x08 // RGB-BGR ORDER
#define HX8357A_MADCTL_SS 0x02  // SOURCE OUTPUT ORDER
#define HX8357A_MADCTL_GS 0x01  // GATE OUTPUT ORDER
#define HX8357A_INV_ON 0x20     // INV_ON enable
#define HX8357A_INV_OFF 0x00    // INV_ON disable

static const uint8_t hx8357a_init_operations[] = {
    BEGIN_WRITE,
    WRITE_C8_D8, HX8357A_PAGE_SELECT, 0x00, // Command page 0
    END_WRITE,
    DELAY, 15,
    BEGIN_WRITE,
    WRITE_C8_D8, HX8357A_POWER_CONTROL_6, 0x04, // VGH VGL VCL  DDVDH
    WRITE_C8_D8, HX8357A_POWER_CONTROL_5, 0x1C,
    // Power Settng
    WRITE_C8_D8, HX8357A_VCOM_CONTROL_1, 0x94, // Set VCOM offset, VMF=0x52
    WRITE_C8_D8, HX8357A_VCOM_CONTROL_2, 0x69, // Set VCOMH voltage, VHH=0x64
    WRITE_C8_D8, HX8357A_VCOM_CONTROL_3, 0x63, // Set VCOML voltage, VML=0x71
    WRITE_C8_D8, HX8357A_OSC_CONTROL_2, 0x01,
    END_WRITE,
    DELAY, 10,
    BEGIN_WRITE,
    WRITE_C8_D8, HX8357A_POWER_CONTROL_6, 0x00,      // VGH VGL VCL  DDVDH
    WRITE_C8_D8, HX8357A_POWER_CONTROL_1, 0x8A,      //
    WRITE_C8_D8, HX8357A_DISPLAY_MODE_CONTROL, 0x00, //
    WRITE_C8_D8, HX8357A_POWER_CONTROL_4, 0x05,      //
    WRITE_C8_D8, HX8357A_POWER_CONTROL_1, 0x82,      //
    END_WRITE,
    DELAY, 10,
    BEGIN_WRITE,
    WRITE_C8_D8, HX8357A_POWER_CONTROL_1, 0x92, //
    END_WRITE,
    DELAY, 10,
    BEGIN_WRITE,
    WRITE_C8_D8, HX8357A_POWER_CONTROL_1, 0xD4, //

    WRITE_C8_D8, HX8357A_COLMOD, 0x55,          // 16-bit/pixel
    WRITE_C8_D8, HX8357A_OSC_CONTROL_1, 0x21,   // Fosc=130%*5.2MHZ   21
    WRITE_C8_D8, HX8357A_POWER_CONTROL_3, 0x00, // FS0[1:0]=01, Set the operating frequency of the step-up circuit 1
    WRITE_C8_D8, HX8357A_POWER_CONTROL_2, 0x00,
    WRITE_C8_D8, HX8357A_DISPLAY_CONTROL_1, 0x33,
    WRITE_C8_D8, HX8357A_DISPLAY_CONTROL_2, 0x01,
    WRITE_C8_D8, HX8357A_FRAME_RATE_CONTROL_1, 0x00,
    WRITE_C8_D8, HX8357A_FRAME_RATE_CONTROL_2, 0x00,
    WRITE_C8_D8, HX8357A_FRAME_RATE_CONTROL_3, 0x01, // 0A
    WRITE_C8_D8, HX8357A_FRAME_RATE_CONTROL_4, 0x0A,
    WRITE_C8_D8, HX8357A_CYCLE_CONTROL_2, 0x20,
    WRITE_C8_D8, HX8357A_CYCLE_CONTROL_3, 0xA3,
    WRITE_C8_D8, HX8357A_DISPLAY_INVERSION, 0x00, // 00 100416

    WRITE_C8_D8, HX8357A_RGB_INTERFACE_CONTROL_1, 0x00, // RGB MODE1  0X03=RGB MODE2
    WRITE_C8_D8, HX8357A_RGB_INTERFACE_CONTROL_2, 0x00,
    WRITE_C8_D8, HX8357A_RGB_INTERFACE_CONTROL_3, 0x08,
    WRITE_C8_D8, HX8357A_RGB_INTERFACE_CONTROL_4, 0x08,
    WRITE_C8_D8, HX8357A_PANEL_CHARACTERIC, 0x02, // REV

    // Gamma
    WRITE_C8_D8, HX8357A_R1_CONTROL_1, 0x01,
    WRITE_C8_D8, HX8357A_R1_CONTROL_2, 0x0F,
    WRITE_C8_D8, HX8357A_R1_CONTROL_3, 0x0F,
    WRITE_C8_D8, HX8357A_R1_CONTROL_4, 0x26,
    WRITE_C8_D8, HX8357A_R1_CONTROL_5, 0x2C,
    WRITE_C8_D8, HX8357A_R1_CONTROL_6, 0x3C,
    WRITE_C8_D8, HX8357A_R1_CONTROL_7, 0x0B,
    WRITE_C8_D8, HX8357A_R1_CONTROL_8, 0x5F,
    WRITE_C8_D8, HX8357A_R1_CONTROL_9, 0x00,
    WRITE_C8_D8, HX8357A_R1_CONTROL_10, 0x06,
    WRITE_C8_D8, HX8357A_R1_CONTROL_11, 0x09,
    WRITE_C8_D8, HX8357A_R1_CONTROL_12, 0x0E,
    WRITE_C8_D8, HX8357A_R1_CONTROL_13, 0x16,

    WRITE_C8_D8, HX8357A_R1_CONTROL_18, 0x01,
    WRITE_C8_D8, HX8357A_R1_CONTROL_19, 0x1D,
    WRITE_C8_D8, HX8357A_R1_CONTROL_20, 0x21,
    WRITE_C8_D8, HX8357A_R1_CONTROL_21, 0x3A,
    WRITE_C8_D8, HX8357A_R1_CONTROL_22, 0x39,
    WRITE_C8_D8, HX8357A_R1_CONTROL_23, 0x3F,
    WRITE_C8_D8, HX8357A_R1_CONTROL_24, 0x2D,
    WRITE_C8_D8, HX8357A_R1_CONTROL_25, 0x7F,
    WRITE_C8_D8, HX8357A_R1_CONTROL_26, 0x02,
    WRITE_C8_D8, HX8357A_R1_CONTROL_27, 0x15,
    WRITE_C8_D8, HX8357A_R1_CONTROL_28, 0x1B,
    WRITE_C8_D8, HX8357A_R1_CONTROL_29, 0x1B,
    WRITE_C8_D8, HX8357A_R1_CONTROL_30, 0x1A,
    WRITE_C8_D8, HX8357A_R1_CONTROL_35, 0x55,

    // #if 1

    WRITE_C8_D8, 0xff, 0x01,
    WRITE_C8_D8, 0x00, 0x01,
    WRITE_C8_D8, 0x01, 0x00,
    WRITE_C8_D8, 0x02, 0x01,
    WRITE_C8_D8, 0x03, 0x03,
    WRITE_C8_D8, 0x04, 0x05,
    WRITE_C8_D8, 0x05, 0x06,
    WRITE_C8_D8, 0x06, 0x08,
    WRITE_C8_D8, 0x07, 0x0C,
    WRITE_C8_D8, 0x08, 0x0E,
    WRITE_C8_D8, 0x09, 0x11,
    WRITE_C8_D8, 0x0A, 0x12,
    WRITE_C8_D8, 0x0B, 0x14,
    WRITE_C8_D8, 0x0C, 0x1B,
    WRITE_C8_D8, 0x0D, 0x23,
    WRITE_C8_D8, 0x0E, 0x29,
    WRITE_C8_D8, 0x0F, 0x2F,
    WRITE_C8_D8, 0x10, 0x34,
    WRITE_C8_D8, 0x11, 0x39,
    WRITE_C8_D8, 0x12, 0x3E,
    WRITE_C8_D8, 0x13, 0x43,
    WRITE_C8_D8, 0x14, 0x48,
    WRITE_C8_D8, 0x15, 0x4C,
    WRITE_C8_D8, 0x16, 0x51,
    WRITE_C8_D8, 0x17, 0x55,
    WRITE_C8_D8, 0x18, 0x59,
    WRITE_C8_D8, 0x19, 0x5D,
    WRITE_C8_D8, 0x1A, 0x60,
    WRITE_C8_D8, 0x1B, 0x64,
    WRITE_C8_D8, 0x1C, 0x68,
    WRITE_C8_D8, 0x1D, 0x6C,
    WRITE_C8_D8, 0x1E, 0x70,
    WRITE_C8_D8, 0x1F, 0x73,
    WRITE_C8_D8, 0x20, 0x77,
    WRITE_C8_D8, 0x21, 0x7B,
    WRITE_C8_D8, 0x22, 0x7F,
    WRITE_C8_D8, 0x23, 0x83,
    WRITE_C8_D8, 0x24, 0x87,
    WRITE_C8_D8, 0x25, 0x8A,
    WRITE_C8_D8, 0x26, 0x8E,
    WRITE_C8_D8, 0x27, 0x92,
    WRITE_C8_D8, 0x28, 0x96,
    WRITE_C8_D8, 0x29, 0x9A,
    WRITE_C8_D8, 0x2A, 0x9F,
    WRITE_C8_D8, 0x2B, 0xA3,
    WRITE_C8_D8, 0x2C, 0xA7,
    WRITE_C8_D8, 0x2D, 0xAC,
    WRITE_C8_D8, 0x2E, 0xAF,
    WRITE_C8_D8, 0x2F, 0xB3,
    WRITE_C8_D8, 0x30, 0xB7,
    WRITE_C8_D8, 0x31, 0xBA,
    WRITE_C8_D8, 0x32, 0xBE,
    WRITE_C8_D8, 0x33, 0xC3,
    WRITE_C8_D8, 0x34, 0xC7,
    WRITE_C8_D8, 0x35, 0xCC,
    WRITE_C8_D8, 0x36, 0xD1,
    WRITE_C8_D8, 0x37, 0xD7,
    WRITE_C8_D8, 0x38, 0xDD,
    WRITE_C8_D8, 0x39, 0xE3,
    WRITE_C8_D8, 0x3A, 0xE8,
    WRITE_C8_D8, 0x3B, 0xEA,
    WRITE_C8_D8, 0x3C, 0xEC,
    WRITE_C8_D8, 0x3D, 0xEF,
    WRITE_C8_D8, 0x3E, 0xF3,
    WRITE_C8_D8, 0x3F, 0xF6,
    WRITE_C8_D8, 0x40, 0xFE,
    WRITE_C8_D8, 0x41, 0x00,
    WRITE_C8_D8, 0x42, 0x01,
    WRITE_C8_D8, 0x43, 0x03,
    WRITE_C8_D8, 0x44, 0x05,
    WRITE_C8_D8, 0x45, 0x06,
    WRITE_C8_D8, 0x46, 0x08,
    WRITE_C8_D8, 0x47, 0x0C,
    WRITE_C8_D8, 0x48, 0x0E,
    WRITE_C8_D8, 0x49, 0x11,
    WRITE_C8_D8, 0x4A, 0x12,
    WRITE_C8_D8, 0x4B, 0x14,
    WRITE_C8_D8, 0x4C, 0x1B,
    WRITE_C8_D8, 0x4D, 0x23,
    WRITE_C8_D8, 0x4E, 0x29,
    WRITE_C8_D8, 0x4F, 0x2F,
    WRITE_C8_D8, 0x50, 0x34,
    WRITE_C8_D8, 0x51, 0x39,
    WRITE_C8_D8, 0x52, 0x3E,
    WRITE_C8_D8, 0x53, 0x43,
    WRITE_C8_D8, 0x54, 0x48,
    WRITE_C8_D8, 0x55, 0x4C,
    WRITE_C8_D8, 0x56, 0x51,
    WRITE_C8_D8, 0x57, 0x55,
    WRITE_C8_D8, 0x58, 0x59,
    WRITE_C8_D8, 0x59, 0x5D,
    WRITE_C8_D8, 0x5A, 0x60,
    WRITE_C8_D8, 0x5B, 0x64,
    WRITE_C8_D8, 0x5C, 0x68,
    WRITE_C8_D8, 0x5D, 0x6C,
    WRITE_C8_D8, 0x5E, 0x70,
    WRITE_C8_D8, 0x5F, 0x73,
    WRITE_C8_D8, 0x60, 0x77,
    WRITE_C8_D8, 0x61, 0x7B,
    WRITE_C8_D8, 0x62, 0x7F,
    WRITE_C8_D8, 0x63, 0x83,
    WRITE_C8_D8, 0x64, 0x87,
    WRITE_C8_D8, 0x65, 0x8A,
    WRITE_C8_D8, 0x66, 0x8E,
    WRITE_C8_D8, 0x67, 0x92,
    WRITE_C8_D8, 0x68, 0x96,
    WRITE_C8_D8, 0x69, 0x9A,
    WRITE_C8_D8, 0x6A, 0x9F,
    WRITE_C8_D8, 0x6B, 0xA3,
    WRITE_C8_D8, 0x6C, 0xA7,
    WRITE_C8_D8, 0x6D, 0xAC,
    WRITE_C8_D8, 0x6E, 0xAF,
    WRITE_C8_D8, 0x6F, 0xB3,
    WRITE_C8_D8, 0x70, 0xB7,
    WRITE_C8_D8, 0x71, 0xBA,
    WRITE_C8_D8, 0x72, 0xBE,
    WRITE_C8_D8, 0x73, 0xC3,
    WRITE_C8_D8, 0x74, 0xC7,
    WRITE_C8_D8, 0x75, 0xCC,
    WRITE_C8_D8, 0x76, 0xD1,
    WRITE_C8_D8, 0x77, 0xD7,
    WRITE_C8_D8, 0x78, 0xDD,
    WRITE_C8_D8, 0x79, 0xE3,
    WRITE_C8_D8, 0x7A, 0xE8,
    WRITE_C8_D8, 0x7B, 0xEA,
    WRITE_C8_D8, 0x7C, 0xEC,
    WRITE_C8_D8, 0x7D, 0xEF,
    WRITE_C8_D8, 0x7E, 0xF3,
    WRITE_C8_D8, 0x7F, 0xF6,
    WRITE_C8_D8, 0x80, 0xFE,
    WRITE_C8_D8, 0x81, 0x00,
    WRITE_C8_D8, 0x82, 0x01,
    WRITE_C8_D8, 0x83, 0x03,
    WRITE_C8_D8, 0x84, 0x05,
    WRITE_C8_D8, 0x85, 0x06,
    WRITE_C8_D8, 0x86, 0x08,
    WRITE_C8_D8, 0x87, 0x0C,
    WRITE_C8_D8, 0x88, 0x0E,
    WRITE_C8_D8, 0x89, 0x11,
    WRITE_C8_D8, 0x8A, 0x12,
    WRITE_C8_D8, 0x8B, 0x14,
    WRITE_C8_D8, 0x8C, 0x1B,
    WRITE_C8_D8, 0x8D, 0x23,
    WRITE_C8_D8, 0x8E, 0x29,
    WRITE_C8_D8, 0x8F, 0x2F,
    WRITE_C8_D8, 0x90, 0x34,
    WRITE_C8_D8, 0x91, 0x39,
    WRITE_C8_D8, 0x92, 0x3E,
    WRITE_C8_D8, 0x93, 0x43,
    WRITE_C8_D8, 0x94, 0x48,
    WRITE_C8_D8, 0x95, 0x4C,
    WRITE_C8_D8, 0x96, 0x51,
    WRITE_C8_D8, 0x97, 0x55,
    WRITE_C8_D8, 0x98, 0x59,
    WRITE_C8_D8, 0x99, 0x5D,
    WRITE_C8_D8, 0x9A, 0x60,
    WRITE_C8_D8, 0x9B, 0x64,
    WRITE_C8_D8, 0x9C, 0x68,
    WRITE_C8_D8, 0x9D, 0x6C,
    WRITE_C8_D8, 0x9E, 0x70,
    WRITE_C8_D8, 0x9F, 0x73,
    WRITE_C8_D8, 0xA0, 0x77,
    WRITE_C8_D8, 0xA1, 0x7B,
    WRITE_C8_D8, 0xA2, 0x7F,
    WRITE_C8_D8, 0xA3, 0x83,
    WRITE_C8_D8, 0xA4, 0x87,
    WRITE_C8_D8, 0xA5, 0x8A,
    WRITE_C8_D8, 0xA6, 0x8E,
    WRITE_C8_D8, 0xA7, 0x92,
    WRITE_C8_D8, 0xA8, 0x96,
    WRITE_C8_D8, 0xA9, 0x9A,
    WRITE_C8_D8, 0xAA, 0x9F,
    WRITE_C8_D8, 0xAB, 0xA3,
    WRITE_C8_D8, 0xAC, 0xA7,
    WRITE_C8_D8, 0xAD, 0xAC,
    WRITE_C8_D8, 0xAE, 0xAF,
    WRITE_C8_D8, 0xAF, 0xB3,
    WRITE_C8_D8, 0xB0, 0xB7,
    WRITE_C8_D8, 0xB1, 0xBA,
    WRITE_C8_D8, 0xB2, 0xBE,
    WRITE_C8_D8, 0xB3, 0xC3,
    WRITE_C8_D8, 0xB4, 0xC7,
    WRITE_C8_D8, 0xB5, 0xCC,
    WRITE_C8_D8, 0xB6, 0xD1,
    WRITE_C8_D8, 0xB7, 0xD7,
    WRITE_C8_D8, 0xB8, 0xDD,
    WRITE_C8_D8, 0xB9, 0xE3,
    WRITE_C8_D8, 0xBA, 0xE8,
    WRITE_C8_D8, 0xBB, 0xEA,
    WRITE_C8_D8, 0xBC, 0xEC,
    WRITE_C8_D8, 0xBD, 0xEF,
    WRITE_C8_D8, 0xBE, 0xF3,
    WRITE_C8_D8, 0xBF, 0xF6,
    WRITE_C8_D8, 0xC0, 0xFE,

    WRITE_C8_D8, 0xff, 0x00,
    WRITE_C8_D8, HX8357A_TE_CONTROL, 0x08,
    WRITE_C8_D8, HX8357A_CENON_CONTROL, 0x00,
    WRITE_C8_D8, HX8357A_POWER_SAVING_COUNTER_1, 0x1F, // EQVCI_M1=0x00
    WRITE_C8_D8, HX8357A_POWER_SAVING_COUNTER_2, 0x1F, // EQGND_M1=0x1C
    WRITE_C8_D8, HX8357A_POWER_SAVING_COUNTER_3, 0x20, // EQVCI_M0=0x1C
    WRITE_C8_D8, HX8357A_POWER_SAVING_COUNTER_4, 0x00, // EQGND_M0=0x1C
    WRITE_C8_D8, HX8357A_OPON_CONTROL_1, 0xD1,
    WRITE_C8_D8, HX8357A_OPON_CONTROL_2, 0xC0,
    WRITE_C8_D8, HX8357A_DISPLAY_CONTROL_3, 0x38,
    END_WRITE,
    DELAY, 80,
    BEGIN_WRITE,
    WRITE_C8_D8, HX8357A_DISPLAY_CONTROL_3, 0x3C, // GON=1, DTE=1, D[1:0]=11
    END_WRITE,
    DELAY, 100};

class Arduino_HX8357A : public Arduino_TFT
{
public:
  Arduino_HX8357A(Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0, bool ips = false);

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
