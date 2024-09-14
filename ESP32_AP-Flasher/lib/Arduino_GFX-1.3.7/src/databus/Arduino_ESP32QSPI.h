#pragma once

#include "Arduino_DataBus.h"

#if defined(ESP32)
#include <driver/spi_master.h>

#define SPI_MAX_PIXELS_AT_ONCE 1024
#define QSPI_FREQUENCY 80000000
#define QSPI_SPI_MODE SPI_MODE0
#define QSPI_SPI_HOST SPI2_HOST
#define QSPI_DMA_CHANNEL SPI_DMA_CH_AUTO

class Arduino_ESP32QSPI : public Arduino_DataBus
{
public:
  Arduino_ESP32QSPI(
    int8_t cs, int8_t sck, int8_t mosi, int8_t miso, int8_t quadwp, int8_t quadhd, bool is_shared_interface = false); // Constructor

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

  void writeRepeat(uint16_t p, uint32_t len) override;
  void writePixels(uint16_t *data, uint32_t len) override;

  void writeBytes(uint8_t *data, uint32_t len) override;

  void writeIndexedPixels(uint8_t *data, uint16_t *idx, uint32_t len) override;
  void writeIndexedPixelsDouble(uint8_t *data, uint16_t *idx, uint32_t len) override;

protected:
private:
  INLINE void CS_HIGH(void);
  INLINE void CS_LOW(void);
  INLINE void POLL_START();
  INLINE void POLL_END();

  int8_t _cs, _sck, _mosi, _miso, _quadwp, _quadhd;
  bool _is_shared_interface;

  PORTreg_t _csPortSet; ///< PORT register for chip select SET
  PORTreg_t _csPortClr; ///< PORT register for chip select CLEAR
  uint32_t _csPinMask;  ///< Bitmask for chip select

  spi_device_handle_t _handle;
  spi_transaction_ext_t _spi_tran_ext;
  spi_transaction_t *_spi_tran;
  union
  {
    uint8_t _buffer[SPI_MAX_PIXELS_AT_ONCE * 2] = {0};
    uint16_t _buffer16[SPI_MAX_PIXELS_AT_ONCE];
    uint32_t _buffer32[SPI_MAX_PIXELS_AT_ONCE / 2];
  };
};

#endif // #if defined(ESP32)
