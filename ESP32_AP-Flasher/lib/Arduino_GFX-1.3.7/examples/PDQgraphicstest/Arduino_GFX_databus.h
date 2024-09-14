// General software SPI
// Arduino_DataBus *bus = new Arduino_SWSPI(TFT_DC, TFT_CS, 18 /* SCK */, 23 /* MOSI */, GFX_NOT_DEFINED /* MISO */);

// hardware SPI
#if defined(ARDUINO_ARCH_NRF52840)
// Arduino_DataBus *bus = new Arduino_mbedSPI(TFT_DC, TFT_CS);
Arduino_DataBus *bus = new Arduino_NRFXSPI(TFT_DC, TFT_CS, 13 /* SCK */, 11 /* MOSI */, 12 /* MISO */);
#elif defined(TARGET_RP2040)
Arduino_DataBus *bus = new Arduino_RPiPicoSPI(TFT_DC /* DC */, TFT_CS /* CS */, 18 /* SCK */, 19 /* MOSI */, 16 /* MISO */, spi0 /* spi */);
#elif defined(ESP32) && (CONFIG_IDF_TARGET_ESP32)
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, 18 /* SCK */, 23 /* MOSI */, GFX_NOT_DEFINED /* MISO */, VSPI /* spi_num */);
#elif defined(ESP32) && (CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3)
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, 36 /* SCK */, 35 /* MOSI */, GFX_NOT_DEFINED /* MISO */, HSPI /* spi_num */);
#elif defined(ESP32) && (CONFIG_IDF_TARGET_ESP32C3)
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, 4 /* SCK */, 6 /* MOSI */, GFX_NOT_DEFINED /* MISO */, FSPI /* spi_num */);
#elif defined(ESP8266)
Arduino_DataBus *bus = new Arduino_ESP8266SPI(TFT_DC, TFT_CS);
#else
// General hardware SPI
Arduino_DataBus *bus = new Arduino_HWSPI(TFT_DC, TFT_CS);
#endif

// General Software parallel 8-bit
// Arduino_DataBus *bus = new Arduino_SWPAR8(TFT_DC, TFT_CS, 25 /* WR */, 32 /* RD */, 23 /* D0 */, 19 /* D1 */, 18 /* D2 */, 26 /* D3 */, 21 /* D4 */, 4 /* D5 */, 0 /* D6 */, 2 /* D7 */);

// General Software parallel 16-bit
// Arduino_DataBus *bus = new Arduino_SWPAR16(32 /* DC */, GFX_NOT_DEFINED /* CS */, 21 /* WR */, GFX_NOT_DEFINED /* RD */, 19 /* D0 */, 23 /* D1 */, 18 /* D2 */, 5 /* D3 */, 17 /* D4 */, 16 /* D5 */, 25 /* D6 */, 26 /* D7 */, 27 /* D8 */, 14 /* D9 */, 12 /* D10 */, 13 /* D11 */, 15 /* D12 */, 2 /* D13 */, 0 /* D14 */, 4 /* D15 */);

// AVR parallel 8-bit
// Arduino Pro Micro port 2(PB): 17, 15, 16, 14, 8, 9, 10, 11
// Arduino_DataBus *bus = new Arduino_AVRPAR8(4 /* DC */, 5 /* CS */, 18 /* WR */, 19 /* RD */, 2 /* PORT */);

// AVR parallel 16-bit
// Arduino MEGA 2560
// port 3(PC): 37, 36, 35, 34, 33, 32, 31, 30
// port 1(PA): 22, 23, 24, 25, 26, 27, 28, 29
// Arduino_DataBus *bus = new Arduino_AVRPAR16(38 /* DC */, 40 /* CS */, 39 /* WR */, 43 /* RD */, 3 /* PORT LOW */, 1 /* PORT HIGH */);

// ESP32 parallel 8-bit
// Arduino_DataBus *bus = new Arduino_ESP32PAR8(TFT_DC, TFT_CS, 25 /* WR */, 32 /* RD */, 23 /* D0 */, 19 /* D1 */, 18 /* D2 */, 26 /* D3 */, 21 /* D4 */, 4 /* D5 */, 0 /* D6 */, 2 /* D7 */);

// ESP32 parallel 16-bit
// Almost all GPIO 0-31 used up for 16-bit and WR, disable PSRAM to gain 16 and 17 but still no GPIOs remain for CS and RD.
// CS connect to GND (enable); RD connect to Vcc (disable).
// Arduino_DataBus *bus = new Arduino_ESP32PAR16(
//     32 /* DC */, GFX_NOT_DEFINED /* CS */, 21 /* WR */, GFX_NOT_DEFINED /* RD */,
//     19 /* D0 */, 23 /* D1 */, 18 /* D2 */, 5 /* D3 */, 17 /* D4 */, 16 /* D5 */, 25 /* D6 */, 26 /* D7 */,
//     27 /* D8 */, 14 /* D9 */, 12 /* D10 */, 13 /* D11 */, 15 /* D12 */, 2 /* D13 */, 0 /* D14 */, 4 /* D15 */);

// ESP32S2 parallel 8-bit
// Display D0-D7 connect to GPIO 0-7
// Arduino_DataBus *bus = new Arduino_ESP32S2PAR8(TFT_DC, TFT_CS, 16 /* WR */, 17 /* RD */);

// ESP32S2 parallel 16-bit
// Display D0-D15 connect to GPIO 0-15
// Arduino_DataBus *bus = new Arduino_ESP32S2PAR16(TFT_DC, TFT_CS, 16 /* WR */, 17 /* RD */);

// ESP32S3 i80 LCD parallel 8-bit
// Arduino_DataBus *bus = new Arduino_ESP32LCD8(
//     TFT_DC, TFT_CS, 16 /* WR */, 17 /* RD */,
//     0 /* D0 */, 1 /* D1 */, 2 /* D2 */, 3 /* D3 */, 4 /* D4 */, 5 /* D5 */, 6 /* D6 */, 7 /* D7 */);

// ESP32S3 i80 LCD parallel 16-bit
// Arduino_DataBus *bus = new Arduino_ESP32LCD16(
//     TFT_DC, TFT_CS, 16 /* WR */, 17 /* RD */,
//     0 /* D0 */, 1 /* D1 */, 2 /* D2 */, 3 /* D3 */, 4 /* D4 */, 5 /* D5 */, 6 /* D6 */, 7 /* D7 */,
//     8 /* D8 */, 9 /* D9 */, 10 /* D10 */, 11 /* D11 */, 12 /* D12 */, 13 /* D13 */, 14 /* D14 */, 15 /* D15 */);

// Raspberry Pi Pico parallel 8-bit
// Display D0-D7 connect to GPIO 0-7
// Arduino_DataBus *bus = new Arduino_RPiPicoPAR8(TFT_DC, TFT_CS, 18 /* WR */, 19 /* RD */);

// Raspberry Pi Pico parallel 16-bit
// Display D0-D15 connect to GPIO 0-15
// Arduino_DataBus *bus = new Arduino_RPiPicoPAR16(TFT_DC, TFT_CS, 18 /* WR */, 19 /* RD */);

// RTL8722 parallel 8-bit
// Reduce GPIO usage: CS connect to GND (enable); RD connect to Vcc (disable); No Backlight pins.
// Arduino_DataBus *bus = new Arduino_RTLPAR8(0 /* DC */, GFX_NOT_DEFINED /* CS */, 1 /* WR */, GFX_NOT_DEFINED /* RD */, 18 /* D0 */, 22 /* D1 */, 17 /* D2 */, 20 /* D3 */, 19 /* D4 */, 23 /* D5 */, 21 /* D6 */, 16 /* D7 */);
