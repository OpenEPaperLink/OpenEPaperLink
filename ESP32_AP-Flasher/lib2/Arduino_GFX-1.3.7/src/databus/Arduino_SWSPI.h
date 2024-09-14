/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 */
#ifndef _ARDUINO_SWSPI_H_
#define _ARDUINO_SWSPI_H_

#include "Arduino_DataBus.h"

class Arduino_SWSPI : public Arduino_DataBus
{
public:
  Arduino_SWSPI(int8_t dc, int8_t cs, int8_t _sck, int8_t _mosi, int8_t _miso = GFX_NOT_DEFINED); // Constructor

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
#endif // !defined(LITTLE_FOOT_PRINT)

private:
  INLINE void WRITE9BITCOMMAND(uint8_t c);
  INLINE void WRITE9BITDATA(uint8_t d);
  INLINE void WRITE(uint8_t d);
  INLINE void WRITE16(uint16_t d);
  INLINE void WRITE9BITREPEAT(uint16_t p, uint32_t len);
  INLINE void WRITEREPEAT(uint16_t p, uint32_t len);
  INLINE void DC_HIGH(void);
  INLINE void DC_LOW(void);
  INLINE void CS_HIGH(void);
  INLINE void CS_LOW(void);
  INLINE void SPI_MOSI_HIGH(void);
  INLINE void SPI_MOSI_LOW(void);
  INLINE void SPI_SCK_HIGH(void);
  INLINE void SPI_SCK_LOW(void);
  INLINE bool SPI_MISO_READ(void);

  int8_t _dc, _cs;
  int8_t _sck, _mosi, _miso;

  // CLASS INSTANCE VARIABLES --------------------------------------------

  // Here be dragons! There's a big union of three structures here --
  // one each for hardware SPI, software (bitbang) SPI, and parallel
  // interfaces. This is to save some memory, since a display's connection
  // will be only one of these. The order of some things is a little weird
  // in an attempt to get values to align and pack better in RAM.

#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
  PORTreg_t _csPortSet;   ///< PORT register for chip select SET
  PORTreg_t _csPortClr;   ///< PORT register for chip select CLEAR
  PORTreg_t _dcPortSet;   ///< PORT register for data/command SET
  PORTreg_t _dcPortClr;   ///< PORT register for data/command CLEAR
  PORTreg_t _mosiPortSet; ///< PORT register for MOSI SET
  PORTreg_t _mosiPortClr; ///< PORT register for MOSI CLEAR
  PORTreg_t _sckPortSet;  ///< PORT register for SCK SET
  PORTreg_t _sckPortClr;  ///< PORT register for SCK CLEAR
#if !defined(KINETISK)
  ARDUINOGFX_PORT_t _csPinMask;   ///< Bitmask for chip select
  ARDUINOGFX_PORT_t _dcPinMask;   ///< Bitmask for data/command
  ARDUINOGFX_PORT_t _mosiPinMask; ///< Bitmask for MOSI
  ARDUINOGFX_PORT_t _sckPinMask;  ///< Bitmask for SCK
#endif                            // !KINETISK
#else                             // !HAS_PORT_SET_CLR
  PORTreg_t _mosiPort;               ///< PORT register for MOSI
  PORTreg_t _sckPort;                ///< PORT register for SCK
  PORTreg_t _csPort;                 ///< PORT register for chip select
  PORTreg_t _dcPort;                 ///< PORT register for data/command
  ARDUINOGFX_PORT_t _csPinMaskSet;   ///< Bitmask for chip select SET (OR)
  ARDUINOGFX_PORT_t _csPinMaskClr;   ///< Bitmask for chip select CLEAR (AND)
  ARDUINOGFX_PORT_t _dcPinMaskSet;   ///< Bitmask for data/command SET (OR)
  ARDUINOGFX_PORT_t _dcPinMaskClr;   ///< Bitmask for data/command CLEAR (AND)
  ARDUINOGFX_PORT_t _mosiPinMaskSet; ///< Bitmask for MOSI SET (OR)
  ARDUINOGFX_PORT_t _mosiPinMaskClr; ///< Bitmask for MOSI CLEAR (AND)
  ARDUINOGFX_PORT_t _sckPinMaskSet;  ///< Bitmask for SCK SET (OR bitmask)
  ARDUINOGFX_PORT_t _sckPinMaskClr;  ///< Bitmask for SCK CLEAR (AND)
#endif                            // HAS_PORT_SET_CLR
  PORTreg_t _misoPort;            ///< PORT (PIN) register for MISO
#if !defined(KINETISK)
  ARDUINOGFX_PORT_t _misoPinMask; ///< Bitmask for MISO
#endif                            // !KINETISK
#endif                            // defined(USE_FAST_PINIO)
};

#endif // _ARDUINO_SWSPI_H_
