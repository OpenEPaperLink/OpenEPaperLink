#ifdef __AVR__

#ifndef _ARDUINO_AVRPAR8_H_
#define _ARDUINO_AVRPAR8_H_

#include "Arduino_DataBus.h"

class Arduino_AVRPAR8 : public Arduino_DataBus
{
public:
  Arduino_AVRPAR8(int8_t dc, int8_t cs, int8_t wr, int8_t rd, uint8_t port); // Constructor

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

protected:
private:
  INLINE void WRITE(uint8_t d);
  INLINE void DC_HIGH(void);
  INLINE void DC_LOW(void);
  INLINE void CS_HIGH(void);
  INLINE void CS_LOW(void);

  int8_t _dc, _cs, _wr, _rd;
  uint8_t _port;

  PORTreg_t _dcPort;                 ///< PORT register for data/command
  ARDUINOGFX_PORT_t _dcPinMaskSet;   ///< Bitmask for data/command SET (OR)
  ARDUINOGFX_PORT_t _dcPinMaskClr;   ///< Bitmask for data/command CLEAR (AND)

  PORTreg_t _csPort;                 ///< PORT register for data/command
  ARDUINOGFX_PORT_t _csPinMaskSet;   ///< Bitmask for data/command SET (OR)
  ARDUINOGFX_PORT_t _csPinMaskClr;   ///< Bitmask for data/command CLEAR (AND)

  PORTreg_t _wrPort;                 ///< PORT register for data/command
  ARDUINOGFX_PORT_t _wrPinMaskSet;   ///< Bitmask for data/command SET (OR)
  ARDUINOGFX_PORT_t _wrPinMaskClr;   ///< Bitmask for data/command CLEAR (AND)

  PORTreg_t _rdPort;                 ///< PORT register for data/command
  ARDUINOGFX_PORT_t _rdPinMaskSet;   ///< Bitmask for data/command SET (OR)
  ARDUINOGFX_PORT_t _rdPinMaskClr;   ///< Bitmask for data/command CLEAR (AND)

  PORTreg_t _dataPort;                 ///< PORT register for data/command
};

#endif // _ARDUINO_AVRPAR8_H_

#endif // #ifdef __AVR__
