/*
 * start rewrite from:
 * https://github.com/Bodmer/TFT_eSPI.git
 */
#ifdef ESP8266

#ifndef _ARDUINO_ESP8266SPI_H_
#define _ARDUINO_ESP8266SPI_H_

#include "Arduino_DataBus.h"

class Arduino_ESP8266SPI : public Arduino_DataBus
{
public:
  Arduino_ESP8266SPI(int8_t dc, int8_t cs = GFX_NOT_DEFINED); // Constructor

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
  void writePattern(uint8_t *data, uint8_t len, uint32_t repeat) override;

  void writeIndexedPixels(uint8_t *data, uint16_t *idx, uint32_t len) override;
  void writeIndexedPixelsDouble(uint8_t *data, uint16_t *idx, uint32_t len) override;

private:
  INLINE void WRITE(uint8_t d);
  INLINE void WRITE16(uint16_t d);
  INLINE void DC_HIGH(void);
  INLINE void DC_LOW(void);
  INLINE void CS_HIGH(void);
  INLINE void CS_LOW(void);

  int8_t _dc, _cs;

  PORTreg_t _csPort;               ///< PORT register for chip select
  PORTreg_t _dcPort;               ///< PORT register for data/command
  ARDUINOGFX_PORT_t _csPinMaskSet; ///< Bitmask for chip select SET (OR)
  ARDUINOGFX_PORT_t _csPinMaskClr; ///< Bitmask for chip select CLEAR (AND)
  ARDUINOGFX_PORT_t _dcPinMaskSet; ///< Bitmask for data/command SET (OR)
  ARDUINOGFX_PORT_t _dcPinMaskClr; ///< Bitmask for data/command CLEAR (AND)

  uint32_t twoPixelBuf[16];
  volatile uint32_t *spi1Reg32 = (volatile uint32_t *)(0x60000000 + 0x140); // address of SPI1W0
};

#endif // _ARDUINO_ESP8266SPI_H_

#endif // #ifdef ESP8266
