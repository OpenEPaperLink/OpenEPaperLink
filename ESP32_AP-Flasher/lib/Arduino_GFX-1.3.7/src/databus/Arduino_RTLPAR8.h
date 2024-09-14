#ifdef RTL8722DM

#ifndef _ARDUINO_RTLPAR8_H_
#define _ARDUINO_RTLPAR8_H_

#include "Arduino_DataBus.h"

class Arduino_RTLPAR8 : public Arduino_DataBus
{
public:
  Arduino_RTLPAR8(
      int8_t dc, int8_t cs, int8_t wr, int8_t rd,
      int8_t d0, int8_t d1, int8_t d2, int8_t d3, int8_t d4, int8_t d5, int8_t d6, int8_t d7); // Constructor

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

  void writeIndexedPixels(uint8_t *data, uint16_t *idx, uint32_t len) override;
  void writeIndexedPixelsDouble(uint8_t *data, uint16_t *idx, uint32_t len) override;

protected:
private:
  INLINE void WRITE(uint8_t d);
  INLINE void DC_HIGH(void);
  INLINE void DC_LOW(void);
  INLINE void CS_HIGH(void);
  INLINE void CS_LOW(void);

  int8_t _dc, _cs, _wr, _rd;
  int8_t _d0, _d1, _d2, _d3, _d4, _d5, _d6, _d7;

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
  ARDUINOGFX_PORT_t _dataPinMaskClr;   ///< Bitmask for data/command CLEAR (AND)

  // Lookup table for ESP32 parallel bus interface uses 1kbyte RAM,
  uint32_t _xset_mask[256];
};

#endif // _ARDUINO_RTLPAR8_H_

#endif // #ifdef RTL8722DM
