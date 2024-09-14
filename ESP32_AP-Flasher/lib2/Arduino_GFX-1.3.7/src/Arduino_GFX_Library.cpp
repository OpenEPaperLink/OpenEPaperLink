#include "Arduino_GFX_Library.h"

Arduino_DataBus *create_default_Arduino_DataBus()
{
#if defined(ARDUINO_ARCH_NRF52840)
  return new Arduino_NRFXSPI(DF_GFX_DC, DF_GFX_CS, DF_GFX_SCK, DF_GFX_MOSI, DF_GFX_MISO);
#elif defined(TARGET_RP2040)
  return new Arduino_RPiPicoSPI(DF_GFX_DC, DF_GFX_CS, DF_GFX_SCK, DF_GFX_MOSI, DF_GFX_MISO, DF_GFX_SPI);
#elif defined(ESP32)
  return new Arduino_ESP32SPI(DF_GFX_DC, DF_GFX_CS, DF_GFX_SCK, DF_GFX_MOSI, DF_GFX_MISO);
#elif defined(ESP8266)
  return new Arduino_ESP8266SPI(DF_GFX_DC, DF_GFX_CS);
#else
  return new Arduino_HWSPI(DF_GFX_DC, DF_GFX_CS);
#endif
}

Arduino_GFX *create_default_Arduino_GFX()
{
  Arduino_DataBus *bus = create_default_Arduino_DataBus();
#if defined(WIO_TERMINAL)
  return new Arduino_ILI9341(bus, DF_GFX_RST, 1 /* rotation */);
#elif defined(ESP32_S3_BOX)
  return new Arduino_ILI9342(bus, DF_GFX_RST, 0 /* rotation */);
#elif defined(M5STACK_CORE)
  return new Arduino_ILI9342(bus, DF_GFX_RST, 2 /* rotation */);
#elif defined(M5STACK_ATOMS3)
  return new Arduino_GC9107(bus, 34 /* RST */, 0 /* rotation */, true /* IPS */);
#elif defined(ODROID_GO)
  return new Arduino_ILI9341(bus, DF_GFX_RST, 3 /* rotation */);
#elif defined(TTGO_T_WATCH)
  return new Arduino_ST7789(bus, DF_GFX_RST, 0 /* rotation */, true /* IPS */, 240, 240, 0, 80);
#elif defined(WAVESHARE_RP2040_LCD_1_28)
  return new Arduino_ST7789(bus, DF_GFX_RST, 0 /* rotation */, true /* IPS */, 240, 240, 0, 80);
#else
  return new Arduino_GC9A01(bus, DF_GFX_RST, 0 /* rotation */, true /* IPS */);
#endif
}
