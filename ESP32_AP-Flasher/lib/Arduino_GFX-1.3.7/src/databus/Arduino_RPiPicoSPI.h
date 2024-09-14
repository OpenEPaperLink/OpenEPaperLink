#if defined(TARGET_RP2040)

#ifndef _ARDUINO_RPIPICOSPI_H_
#define _ARDUINO_RPIPICOSPI_H_

#include <api/HardwareSPI.h>
#include <hardware/spi.h>

#include "Arduino_DataBus.h"

#define SPI_MAX_PIXELS_AT_ONCE 32

class Arduino_RPiPicoSPI : public Arduino_DataBus
{
public:
  Arduino_RPiPicoSPI(int8_t dc = GFX_NOT_DEFINED, int8_t cs = GFX_NOT_DEFINED, int8_t sck = PIN_SPI0_SCK, int8_t mosi = PIN_SPI0_MOSI, int8_t miso = PIN_SPI0_MISO, spi_inst_t *spi = spi0); // Constructor

  bool begin(int32_t speed = GFX_NOT_DEFINED, int8_t dataMode = GFX_NOT_DEFINED) override;
  void beginWrite() override;
  void endWrite() override;
  void writeCommand(uint8_t) override;
  void writeCommand16(uint16_t) override;
  void write(uint8_t) override;
  void write16(uint16_t) override;
  void writeRepeat(uint16_t p, uint32_t len) override;
  void writePixels(uint16_t *data, uint32_t len) override;

  void writeC8D8(uint8_t c, uint8_t d) override;
  void writeC8D16(uint8_t c, uint16_t d) override;
  void writeC8D16D16(uint8_t c, uint16_t d1, uint16_t d2) override;
  void writeBytes(uint8_t *data, uint32_t len) override;

protected:

private:
  INLINE void WRITE(uint8_t d);
  INLINE void WRITE16(uint16_t d);
  INLINE void WRITEBUF(uint8_t *buf, size_t count);
  INLINE void DC_HIGH(void);
  INLINE void DC_LOW(void);
  INLINE void CS_HIGH(void);
  INLINE void CS_LOW(void);

  int8_t _dc, _cs;
  int8_t _sck, _mosi, _miso;
  int32_t _speed;

  PORTreg_t _dcPortSet; ///< PORT register for data/command SET
  PORTreg_t _dcPortClr; ///< PORT register for data/command CLEAR
  PORTreg_t _csPortSet; ///< PORT register for chip select SET
  PORTreg_t _csPortClr; ///< PORT register for chip select CLEAR
  uint32_t _dcPinMask;  ///< Bitmask for data/command
  uint32_t _csPinMask;  ///< Bitmask for chip select

  spi_inst_t *_spi;
  SPISettings _spis;
  BitOrder _bitOrder = MSBFIRST;

  union
  {
    uint8_t _buffer[SPI_MAX_PIXELS_AT_ONCE * 2] = {0};
    uint16_t _buffer16[SPI_MAX_PIXELS_AT_ONCE];
    uint32_t _buffer32[SPI_MAX_PIXELS_AT_ONCE / 2];
  };
};

#endif // _ARDUINO_RPIPICOSPI_H_
#endif // #if defined(TARGET_RP2040)
