#ifdef ARDUINO_ARCH_STM32

#ifndef _ARDUINO_STM32PAR8_H_
#define _ARDUINO_STM32PAR8_H_

#include "Arduino_DataBus.h"

class Arduino_STM32PAR8 : public Arduino_DataBus
{
public:
  Arduino_STM32PAR8(int8_t dc, int8_t cs, int8_t wr, int8_t rd, GPIO_TypeDef* port); // Constructor

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
  GPIO_TypeDef* _port;
  GPIO_TypeDef* _dcPORT; 
  GPIO_TypeDef* _csPORT;
  GPIO_TypeDef* _wrPORT;
  GPIO_TypeDef* _rdPORT;

  ARDUINOGFX_PORT_t _dcPinMaskSet;   ///< Bitmask for data/command SET (OR)
  ARDUINOGFX_PORT_t _csPinMaskSet;   ///< Bitmask for data/command SET (OR)
  PORTreg_t _wrPort;                 ///< PORT register for data/command
  ARDUINOGFX_PORT_t _wrPinMaskSet;   ///< Bitmask for data/command SET (OR)
  ARDUINOGFX_PORT_t _wrPinMaskClr;   ///< Bitmask for data/command CLEAR (AND)
  ARDUINOGFX_PORT_t _rdPinMaskSet;   ///< Bitmask for data/command SET (OR)
};

#endif // _ARDUINO_STM32PAR8_H_

#endif // #ifdef ARDUINO_ARCH_STM32
