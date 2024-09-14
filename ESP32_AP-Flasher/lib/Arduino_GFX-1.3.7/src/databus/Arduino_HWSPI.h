/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 */
#ifndef _ARDUINO_HWSPI_H_
#define _ARDUINO_HWSPI_H_

#include <SPI.h>
#include "Arduino_DataBus.h"

#if !defined(LITTLE_FOOT_PRINT)
#define SPI_MAX_PIXELS_AT_ONCE 32
#endif

// HARDWARE CONFIG ---------------------------------------------------------

class Arduino_HWSPI : public Arduino_DataBus
{
public:
#if defined(ESP32)
  Arduino_HWSPI(int8_t dc, int8_t cs = GFX_NOT_DEFINED, int8_t sck = GFX_NOT_DEFINED, int8_t mosi = GFX_NOT_DEFINED, int8_t miso = GFX_NOT_DEFINED, SPIClass *spi = &SPI, bool is_shared_interface = true); // Constructor
#elif defined(ARDUINO_ARCH_SAMD) && defined(SEEED_GROVE_UI_WIRELESS)
  Arduino_HWSPI(int8_t dc, int8_t cs = GFX_NOT_DEFINED, SPIClass *spi = &LCD_SPI, bool is_shared_interface = true); // Constructor
#elif defined(RTL8722DM) && defined(BOARD_RTL8722DM)
  Arduino_HWSPI(int8_t dc, int8_t cs = GFX_NOT_DEFINED, SPIClass *spi = &SPI1, bool is_shared_interface = true); // Constructor
#else
  Arduino_HWSPI(int8_t dc, int8_t cs = GFX_NOT_DEFINED, SPIClass *spi = &SPI, bool is_shared_interface = true); // Constructor
#endif

  bool begin(int32_t speed = GFX_NOT_DEFINED, int8_t dataMode = GFX_NOT_DEFINED) override;
  void beginWrite() override;
  void endWrite() override;
  void writeCommand(uint8_t) override;
  void writeCommand16(uint16_t) override;
  void write(uint8_t) override;
  void write16(uint16_t) override;
  void writeRepeat(uint16_t p, uint32_t len) override;
  void writePixels(uint16_t *data, uint32_t len) override;

#if !defined(LITTLE_FOOT_PRINT)
  void writeBytes(uint8_t *data, uint32_t len) override;
  void writePattern(uint8_t *data, uint8_t len, uint32_t repeat) override;
#endif // !defined(LITTLE_FOOT_PRINT)

private:
  INLINE void WRITE(uint8_t d);
#if !defined(LITTLE_FOOT_PRINT)
  INLINE void WRITE16(uint16_t d);
  INLINE void WRITEBUF(uint8_t *buf, size_t count);
#endif // !defined(LITTLE_FOOT_PRINT)
  INLINE void DC_HIGH(void);
  INLINE void DC_LOW(void);
  INLINE void CS_HIGH(void);
  INLINE void CS_LOW(void);


  int8_t _dc, _cs;
#if defined(ESP32)
  int8_t _sck, _mosi, _miso;
#endif
  SPIClass *_spi;
  bool _is_shared_interface;

  // CLASS INSTANCE VARIABLES --------------------------------------------

  // Here be dragons! There's a big union of three structures here --
  // one each for hardware SPI, software (bitbang) SPI, and parallel
  // interfaces. This is to save some memory, since a display's connection
  // will be only one of these. The order of some things is a little weird
  // in an attempt to get values to align and pack better in RAM.

#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
  PORTreg_t _csPortSet; ///< PORT register for chip select SET
  PORTreg_t _csPortClr; ///< PORT register for chip select CLEAR
  PORTreg_t _dcPortSet; ///< PORT register for data/command SET
  PORTreg_t _dcPortClr; ///< PORT register for data/command CLEAR
#if !defined(KINETISK)
  ARDUINOGFX_PORT_t _csPinMask; ///< Bitmask for chip select
  ARDUINOGFX_PORT_t _dcPinMask; ///< Bitmask for data/command
#endif                          // !KINETISK
#else                           // !HAS_PORT_SET_CLR
  PORTreg_t _csPort;               ///< PORT register for chip select
  PORTreg_t _dcPort;               ///< PORT register for data/command
  ARDUINOGFX_PORT_t _csPinMaskSet; ///< Bitmask for chip select SET (OR)
  ARDUINOGFX_PORT_t _csPinMaskClr; ///< Bitmask for chip select CLEAR (AND)
  ARDUINOGFX_PORT_t _dcPinMaskSet; ///< Bitmask for data/command SET (OR)
  ARDUINOGFX_PORT_t _dcPinMaskClr; ///< Bitmask for data/command CLEAR (AND)
#endif                          // HAS_PORT_SET_CLR
#endif                          // !defined(USE_FAST_PINIO)

#if !defined(LITTLE_FOOT_PRINT)
  union
  {
    uint8_t v8[SPI_MAX_PIXELS_AT_ONCE * 2] = {0};
    uint16_t v16[SPI_MAX_PIXELS_AT_ONCE];
  } _buffer;
#endif // !defined(LITTLE_FOOT_PRINT)
};

#endif // _ARDUINO_HWSPI_H_
