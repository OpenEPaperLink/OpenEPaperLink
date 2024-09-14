#pragma once

#include "Arduino_DataBus.h"

#if defined(ESP32) && (CONFIG_IDF_TARGET_ESP32S3)

#define LCD_MAX_PIXELS_AT_ONCE 2046
#define USE_DMA_THRESHOLD 6

class Arduino_ESP32LCD8 : public Arduino_DataBus
{
public:
  Arduino_ESP32LCD8(
      int8_t dc, int8_t cs, int8_t wr, int8_t rd,
      int8_t d0, int8_t d1, int8_t d2, int8_t d3, int8_t d4, int8_t d5, int8_t d6, int8_t d7); // Constructor

  bool begin(int32_t speed = GFX_NOT_DEFINED, int8_t dataMode = GFX_NOT_DEFINED) override;
  void beginWrite() override;
  void endWrite() override;
  void writeCommand(uint8_t) override;
  void writeCommand16(uint16_t) override;
  void write(uint8_t) override;
  void write16(uint16_t) override;

  void writeC8D8(uint8_t c, uint8_t d) override;
  void writeC8D16(uint8_t c, uint16_t d) override;
  void writeC8D16D16(uint8_t c, uint16_t d1, uint16_t d2) override;
  void writeC8D16D16Split(uint8_t c, uint16_t d1, uint16_t d2) override;

  void writeRepeat(uint16_t p, uint32_t len) override;
  void writePixels(uint16_t *data, uint32_t len) override;

  void writeBytes(uint8_t *data, uint32_t len) override;

  void writeIndexedPixels(uint8_t *data, uint16_t *idx, uint32_t len) override;
  void writeIndexedPixelsDouble(uint8_t *data, uint16_t *idx, uint32_t len) override;

protected:
private:
  INLINE void CS_HIGH(void);
  INLINE void CS_LOW(void);

  int8_t _dc, _cs, _wr, _rd;
  int8_t _d0, _d1, _d2, _d3, _d4, _d5, _d6, _d7;

  PORTreg_t _csPortSet; ///< PORT register for chip select SET
  PORTreg_t _csPortClr; ///< PORT register for chip select CLEAR
  uint32_t _csPinMask;  ///< Bitmask for chip select

  esp_lcd_i80_bus_handle_t _i80_bus = nullptr;
  dma_descriptor_t *_dmadesc = nullptr;
  gdma_channel_handle_t _dma_chan;

  union
  {
    uint32_t value;
    struct
    {
      uint16_t value16;
      uint16_t value16_2;
    };
    struct
    {
      uint8_t lsb;
      uint8_t msb;
      uint8_t lsb_2;
      uint8_t msb_2;
    };
  } _data32;

  union
  {
    uint8_t _buffer[LCD_MAX_PIXELS_AT_ONCE * 2] = {0};
    uint16_t _buffer16[LCD_MAX_PIXELS_AT_ONCE];
    uint32_t _buffer32[LCD_MAX_PIXELS_AT_ONCE / 2];
  };
};

#endif // #if defined(ESP32) && (CONFIG_IDF_TARGET_ESP32S3)
