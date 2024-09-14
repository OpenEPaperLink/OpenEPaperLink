#pragma once

#include "Arduino_DataBus.h"

#if defined(ESP32)
#include <driver/spi_master.h>

#define SPI_MAX_PIXELS_AT_ONCE 1024
#define DMA_CHANNEL SPI_DMA_CH_AUTO

class Arduino_ESP32SPIDMA : public Arduino_DataBus
{
public:
#if CONFIG_IDF_TARGET_ESP32
  Arduino_ESP32SPIDMA(int8_t dc = GFX_NOT_DEFINED, int8_t cs = GFX_NOT_DEFINED, int8_t sck = GFX_NOT_DEFINED, int8_t mosi = GFX_NOT_DEFINED, int8_t miso = GFX_NOT_DEFINED, uint8_t spi_num = VSPI, bool is_shared_interface = false); // Constructor
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
  Arduino_ESP32SPIDMA(int8_t dc = GFX_NOT_DEFINED, int8_t cs = GFX_NOT_DEFINED, int8_t sck = GFX_NOT_DEFINED, int8_t mosi = GFX_NOT_DEFINED, int8_t miso = GFX_NOT_DEFINED, uint8_t spi_num = HSPI, bool is_shared_interface = false); // Constructor
#else
  Arduino_ESP32SPIDMA(int8_t dc = GFX_NOT_DEFINED, int8_t cs = GFX_NOT_DEFINED, int8_t sck = GFX_NOT_DEFINED, int8_t mosi = GFX_NOT_DEFINED, int8_t miso = GFX_NOT_DEFINED, uint8_t spi_num = FSPI, bool is_shared_interface = false); // Constructor
#endif

  bool begin(int32_t speed = GFX_NOT_DEFINED, int8_t dataMode = SPI_MODE0) override;
  void beginWrite() override;
  void endWrite() override;
  void writeCommand(uint8_t) override;
  void writeCommand16(uint16_t) override;
  void write(uint8_t) override;
  void write16(uint16_t) override;

  void writeC8D8(uint8_t c, uint8_t d) override;
  void writeC8D16(uint8_t c, uint16_t d) override;
  void writeC8D16D16(uint8_t c, uint16_t d1, uint16_t d2) override;

  void writeRepeat(uint16_t p, uint32_t len) override;
  void writePixels(uint16_t *data, uint32_t len) override;

  void writeBytes(uint8_t *data, uint32_t len) override;

  void writeIndexedPixels(uint8_t *data, uint16_t *idx, uint32_t len) override;
  void writeIndexedPixelsDouble(uint8_t *data, uint16_t *idx, uint32_t len) override;

protected:
  void flush_data_buf();
  INLINE void WRITE8BIT(uint8_t d);
  INLINE void WRITE9BIT(uint32_t d);
  INLINE void DC_HIGH(void);
  INLINE void DC_LOW(void);
  INLINE void CS_HIGH(void);
  INLINE void CS_LOW(void);
  INLINE void POLL_START();
  INLINE void POLL_END();

private:
  int8_t _dc, _cs;
  int8_t _sck, _mosi, _miso;
  uint8_t _spi_num;
  bool _is_shared_interface;
  uint32_t _div = 0;

  PORTreg_t _dcPortSet; ///< PORT register for data/command SET
  PORTreg_t _dcPortClr; ///< PORT register for data/command CLEAR
  PORTreg_t _csPortSet; ///< PORT register for chip select SET
  PORTreg_t _csPortClr; ///< PORT register for chip select CLEAR
  uint32_t _dcPinMask;  ///< Bitmask for data/command
  uint32_t _csPinMask;  ///< Bitmask for chip select

  spi_device_handle_t _handle;
  spi_transaction_t _spi_tran;
  uint8_t _bitOrder = SPI_MSBFIRST;
  union
  {
    uint8_t _buffer[SPI_MAX_PIXELS_AT_ONCE * 2] = {0};
    uint16_t _buffer16[SPI_MAX_PIXELS_AT_ONCE];
    uint32_t _buffer32[SPI_MAX_PIXELS_AT_ONCE / 2];
  };
  uint16_t _data_buf_bit_idx = 0;
};

#endif // #if defined(ESP32)
