#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>

#ifdef HAS_LILYGO_TPANEL

#define LV_ATTRIBUTE_TICK_INC IRAM_ATTR
#define TOUCH_MODULES_CST_MUTUAL

// SD
#define SD_CS 38
#define SD_SCLK 36
#define SD_MOSI 35
#define SD_MISO 37

// IIC
#define IIC_SDA 17
#define IIC_SCL 18

// ESP32H2
#define ESP32H2_TX 48
#define ESP32H2_RX 47

//  #define T_Panel_V1_0_RS485
 #define T_Panel_V1_2_RS485
// #define T_Panel_V1_2_CAN

#if defined T_Panel_V1_0_RS485
#define RS485_TX 15
#define RS485_RX 16
#endif

#if defined T_Panel_V1_2_RS485
#define RS485_TX 16
#define RS485_RX 15
#endif

#if defined T_Panel_V1_2_CAN
#define CAN_TX 16
#define CAN_RX 15
#endif

// YDP395BT001-V2
#define LCD_WIDTH 480
#define LCD_HEIGHT 480
#define LCD_VSYNC 40
#define LCD_HSYNC 39
#define LCD_PCLK 41
#define LCD_B0 1
#define LCD_B1 2
#define LCD_B2 3
#define LCD_B3 4
#define LCD_B4 5
#define LCD_G0 6
#define LCD_G1 7
#define LCD_G2 8
#define LCD_G3 9
#define LCD_G4 10
#define LCD_G5 11
#define LCD_R0 12
#define LCD_R1 13
#define LCD_R2 42
#define LCD_R3 46
#define LCD_R4 45
#define LCD_BL 14

// CST3240
#define CST3240_ADDRESS 0x5A
#define TOUCH_SDA 17
#define TOUCH_SCL 18
#define TOUCH_INT 21
#define TOUCH_RST 4

// XL95x5
#define XL95X5_CS 17
#define XL95X5_SCLK 15
#define XL95X5_MOSI 16
#define XL95X5_TOUCH_RST 4
#define XL95X5_RS485_CON 7
#define XL95X5_LCD_RST 5
#define XL95X5_ESP32H2_IO12 1
#define XL95X5_ESP32H2_IO4 2
#define XL95X5_ESP32H2_IO5 3

// ESP32H2
#define ESP32H2_EN 34
#define ESP32H2_BOOT 33

#include "Arduino_GFX_Library.h"

extern Arduino_RGB_Display *gfx;

#endif

#ifdef HAS_4inch_TPANEL

#define LV_ATTRIBUTE_TICK_INC IRAM_ATTR
#define TOUCH_MODULES_CST_MUTUAL

// esp32-4848S040
#define LCD_WIDTH 480
#define LCD_HEIGHT 480
#define LCD_VSYNC 17
#define LCD_HSYNC 16
#define LCD_PCLK 21
#define LCD_R0 0
#define LCD_R1 11
#define LCD_R2 12
#define LCD_R3 13
#define LCD_R4 14
#define LCD_G0 8
#define LCD_G1 20
#define LCD_G2 3
#define LCD_G3 46
#define LCD_G4 9
#define LCD_G5 10
#define LCD_B0 15
#define LCD_B1 4
#define LCD_B2 5
#define LCD_B3 6
#define LCD_B4 7
#define LCD_BL 38
#define LCD_DE 18

#define SPI_LCD_CS 39
#define SPI_LCD_SCLK 48
#define SPI_LCD_MOSI 47

#include "Arduino_GFX_Library.h"

extern Arduino_RGB_Display *gfx;

#endif

#ifdef HAS_TFT

extern TFT_eSPI tft2;
extern uint8_t YellowSense;
extern bool tftOverride;

void TFTLog(String text);
void sendAvail(uint8_t wakeupReason);

#endif