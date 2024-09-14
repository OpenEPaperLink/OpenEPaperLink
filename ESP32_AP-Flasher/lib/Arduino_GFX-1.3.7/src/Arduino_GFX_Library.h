#ifndef _ARDUINO_GFX_LIBRARIES_H_
#define _ARDUINO_GFX_LIBRARIES_H_

#include "Arduino_DataBus.h"
#include "databus/Arduino_AVRPAR8.h"
#include "databus/Arduino_AVRPAR16.h"
#include "databus/Arduino_ESP32LCD8.h"
#include "databus/Arduino_ESP32LCD16.h"
#include "databus/Arduino_ESP32PAR8.h"
#include "databus/Arduino_ESP32PAR8Q.h"
#include "databus/Arduino_ESP32PAR8QQ.h"
#include "databus/Arduino_ESP32PAR8QQQ.h"
#include "databus/Arduino_ESP32PAR16.h"
#include "databus/Arduino_ESP32PAR16Q.h"
#include "databus/Arduino_ESP32PAR16QQ.h"
#include "databus/Arduino_ESP32QSPI.h"
#include "databus/Arduino_ESP32RGBPanel.h"
#include "databus/Arduino_ESP32S2PAR8.h"
#include "databus/Arduino_ESP32S2PAR8Q.h"
#include "databus/Arduino_ESP32S2PAR16.h"
#include "databus/Arduino_ESP32S2PAR16Q.h"
#include "databus/Arduino_ESP32SPI.h"
#include "databus/Arduino_ESP32SPIDMA.h"
#include "databus/Arduino_ESP8266SPI.h"
#include "databus/Arduino_HWSPI.h"
#include "databus/Arduino_mbedSPI.h"
#include "databus/Arduino_NRFXSPI.h"
#include "databus/Arduino_RPiPicoPAR8.h"
#include "databus/Arduino_RPiPicoPAR16.h"
#include "databus/Arduino_RPiPicoSPI.h"
#include "databus/Arduino_RTLPAR8.h"
#include "databus/Arduino_STM32PAR8.h"
#include "databus/Arduino_SWPAR8.h"
#include "databus/Arduino_SWPAR16.h"
#include "databus/Arduino_SWSPI.h"
#include "databus/Arduino_XL9535SWSPI.h"

#include "Arduino_GFX.h" // Core graphics library
#if !defined(LITTLE_FOOT_PRINT)
#include "canvas/Arduino_Canvas.h"
#include "canvas/Arduino_Canvas_Indexed.h"
#include "canvas/Arduino_Canvas_3bit.h"
#include "canvas/Arduino_Canvas_Mono.h"
#include "display/Arduino_ILI9488_3bit.h"
#endif // !defined(LITTLE_FOOT_PRINT)

#include "display/Arduino_GC9106.h"
#include "display/Arduino_GC9107.h"
#include "display/Arduino_GC9A01.h"
#include "display/Arduino_HX8347C.h"
#include "display/Arduino_HX8347D.h"
#include "display/Arduino_HX8352C.h"
#include "display/Arduino_HX8357A.h"
#include "display/Arduino_HX8357B.h"
#include "display/Arduino_HX8369A.h"
#include "display/Arduino_ILI9225.h"
#include "display/Arduino_ILI9331.h"
#include "display/Arduino_ILI9341.h"
#include "display/Arduino_ILI9342.h"
#include "display/Arduino_ILI9481_18bit.h"
#include "display/Arduino_ILI9486.h"
#include "display/Arduino_ILI9486_18bit.h"
#include "display/Arduino_ILI9488.h"
#include "display/Arduino_ILI9488_18bit.h"
#include "display/Arduino_ILI9806.h"
#include "display/Arduino_JBT6K71.h"
#include "display/Arduino_NT35310.h"
#include "display/Arduino_NT35510.h"
#include "display/Arduino_NT39125.h"
#include "display/Arduino_NV3041A.h"
#include "display/Arduino_OTM8009A.h"
#include "display/Arduino_R61529.h"
#include "display/Arduino_RM67162.h"
#include "display/Arduino_RGB_Display.h"
#include "display/Arduino_SEPS525.h"
#include "display/Arduino_SSD1283A.h"
#include "display/Arduino_SSD1331.h"
#include "display/Arduino_SSD1351.h"
#include "display/Arduino_ST7735.h"
#include "display/Arduino_ST7789.h"
#include "display/Arduino_ST7796.h"

#if defined(ARDUINO_ARCH_SAMD) && defined(SEEED_GROVE_UI_WIRELESS)
#define DISPLAY_DEV_KIT
#define WIO_TERMINAL
#define DF_GFX_CS LCD_SS_PIN
#define DF_GFX_DC LCD_DC
#define DF_GFX_RST GFX_NOT_DEFINED
#define DF_GFX_BL LCD_BACKLIGHT
#elif defined(ARDUINO_ESP32_S3_BOX)
#define DISPLAY_DEV_KIT
#define ESP32_S3_BOX
#define DF_GFX_SCK TFT_CLK
#define DF_GFX_MOSI TFT_MOSI
#define DF_GFX_MISO TFT_MISO
#define DF_GFX_CS TFT_CS
#define DF_GFX_DC TFT_DC
#define DF_GFX_RST TFT_RST
#define DF_GFX_BL TFT_BL
#elif defined(ARDUINO_M5Stack_Core_ESP32) || defined(ARDUINO_M5STACK_FIRE)
#define DISPLAY_DEV_KIT
#define M5STACK_CORE
#define DF_GFX_SCK 18
#define DF_GFX_MOSI 23
#define DF_GFX_MISO 19
#define DF_GFX_CS 14
#define DF_GFX_DC 27
#define DF_GFX_RST 33
#define DF_GFX_BL 32
#elif defined(ARDUINO_M5Stack_ATOMS3)
#define DISPLAY_DEV_KIT
#define M5STACK_ATOMS3
#define DF_GFX_SCK 17
#define DF_GFX_MOSI 21
#define DF_GFX_MISO GFX_NOT_DEFINED
#define DF_GFX_CS 15
#define DF_GFX_DC 33
#define DF_GFX_RST 34
#define DF_GFX_BL 16
#elif defined(ARDUINO_ODROID_ESP32)
#define DISPLAY_DEV_KIT
#define ODROID_GO
#define DF_GFX_SCK 18
#define DF_GFX_MOSI 23
#define DF_GFX_MISO 19
#define DF_GFX_CS 5
#define DF_GFX_DC 21
#define DF_GFX_RST GFX_NOT_DEFINED
#define DF_GFX_BL 14
/* TTGO T-Watch */
#elif defined(ARDUINO_T) || defined(ARDUINO_TWATCH_BASE) || defined(ARDUINO_TWATCH_2020_V1) || defined(ARDUINO_TWATCH_2020_V2)
#define DISPLAY_DEV_KIT
#define TTGO_T_WATCH
#define DF_GFX_SCK 18
#define DF_GFX_MOSI 19
#define DF_GFX_MISO GFX_NOT_DEFINED
#define DF_GFX_CS 5
#define DF_GFX_DC 27
#define DF_GFX_RST GFX_NOT_DEFINED
#define DF_GFX_BL 12
/* Waveshare RP2040-LCD-1.28 */
#elif defined(ARDUINO_WAVESHARE_RP2040_LCD_1_28)
#define DISPLAY_DEV_KIT
#define WAVESHARE_RP2040_LCD_1_28
#define DF_GFX_SCK 10
#define DF_GFX_MOSI 11
#define DF_GFX_MISO 12
#define DF_GFX_CS 9
#define DF_GFX_DC 8
#define DF_GFX_RST 12
#define DF_GFX_BL 25
#define DF_GFX_SPI spi1
#elif defined(ARDUINO_ARCH_NRF52840)
#define DF_GFX_SCK 13
#define DF_GFX_MOSI 11
#define DF_GFX_MISO 12
#define DF_GFX_CS 9
#define DF_GFX_DC 8
#define DF_GFX_RST 7
#define DF_GFX_BL 6
#elif defined(__IMXRT1052__) || defined(__IMXRT1062__)
// PJRC Teensy 4.x
#define DF_GFX_SCK 13
#define DF_GFX_MOSI 11
#define DF_GFX_MISO 12
#define DF_GFX_CS 39 // GFX_NOT_DEFINED for display without CS pin
#define DF_GFX_DC 41
#define DF_GFX_RST 40
#define DF_GFX_BL 22
#elif defined(ARDUINO_BLACKPILL_F411CE)
#define DF_GFX_SCK 5
#define DF_GFX_MOSI 7
#define DF_GFX_MISO 6
#define DF_GFX_CS 4
#define DF_GFX_DC 3
#define DF_GFX_RST 2
#define DF_GFX_BL 1
#elif defined(TARGET_RP2040)
#define DF_GFX_SCK 18
#define DF_GFX_MOSI 19
#define DF_GFX_MISO 16
#define DF_GFX_CS 17
#define DF_GFX_DC 27
#define DF_GFX_RST 26
#define DF_GFX_BL 28
#define DF_GFX_SPI spi0
#elif defined(ESP32) && (CONFIG_IDF_TARGET_ESP32)
#define DF_GFX_SCK 18
#define DF_GFX_MOSI 23
#define DF_GFX_MISO GFX_NOT_DEFINED
#define DF_GFX_CS 5
#define DF_GFX_DC 27
#define DF_GFX_RST 33
#define DF_GFX_BL 22
#elif defined(ESP32) && (CONFIG_IDF_TARGET_ESP32S2)
#define DF_GFX_SCK 36
#define DF_GFX_MOSI 35
#define DF_GFX_MISO GFX_NOT_DEFINED
#define DF_GFX_CS 34
#define DF_GFX_DC 38
#define DF_GFX_RST 33
#define DF_GFX_BL 21
#elif defined(ESP32) && (CONFIG_IDF_TARGET_ESP32S3)
#define DF_GFX_SCK 36
#define DF_GFX_MOSI 35
#define DF_GFX_MISO GFX_NOT_DEFINED
#define DF_GFX_CS 40
#define DF_GFX_DC 41
#define DF_GFX_RST 42
#define DF_GFX_BL 48
#elif defined(ESP32) && (CONFIG_IDF_TARGET_ESP32C3)
#define DF_GFX_SCK 4
#define DF_GFX_MOSI 6
#define DF_GFX_MISO GFX_NOT_DEFINED
#define DF_GFX_CS 7
#define DF_GFX_DC 2
#define DF_GFX_RST 1
#define DF_GFX_BL 3
#elif defined(ESP8266)
#define DF_GFX_SCK 14
#define DF_GFX_MOSI 13
#define DF_GFX_MISO 12
#define DF_GFX_CS 15
#define DF_GFX_DC 4
#define DF_GFX_RST 2
#define DF_GFX_BL 5
#elif defined(RTL8722DM)
#if defined(BOARD_RTL8720DN_BW16)
#define DF_GFX_SCK 10
#define DF_GFX_MOSI 12
#define DF_GFX_MISO 11
#define DF_GFX_CS 9
#define DF_GFX_DC 8
#define DF_GFX_RST 6
#define DF_GFX_BL 3
#elif defined(BOARD_RTL8722DM)
#define DF_GFX_SCK 13
#define DF_GFX_MOSI 11
#define DF_GFX_MISO 12
#define DF_GFX_CS 18
#define DF_GFX_DC 17
#define DF_GFX_RST 22
#define DF_GFX_BL 23
#elif defined(BOARD_RTL8722DM_MINI)
#define DF_GFX_SCK 11
#define DF_GFX_MOSI 9
#define DF_GFX_MISO 10
#define DF_GFX_CS 12
#define DF_GFX_DC 14
#define DF_GFX_RST 15
#define DF_GFX_BL 13
#else // old version
#define DF_GFX_SCK 19
#define DF_GFX_MOSI 21
#define DF_GFX_MISO 20
#define DF_GFX_CS 18 // GFX_NOT_DEFINED for display without CS pin
#define DF_GFX_DC 17
#define DF_GFX_RST 2
#define DF_GFX_BL 23
#endif
#elif defined(SEEED_XIAO_M0)
#define DF_GFX_SCK 8
#define DF_GFX_MOSI 10
#define DF_GFX_MISO 9
#define DF_GFX_CS 3 // GFX_NOT_DEFINED for display without CS pin
#define DF_GFX_DC 2
#define DF_GFX_RST 1
#define DF_GFX_BL 0
#else // default pins for Arduino Nano, Mini, Micro and more
#define DF_GFX_SCK 13
#define DF_GFX_MOSI 11
#define DF_GFX_MISO 12
#define DF_GFX_CS 9
#define DF_GFX_DC 8
#define DF_GFX_RST 7
#define DF_GFX_BL 6
#endif

Arduino_DataBus *create_default_Arduino_DataBus();
Arduino_GFX *create_default_Arduino_GFX();

#endif // _ARDUINO_GFX_LIBRARIES_H_
