#ifndef _ARDUINO_SWPAR16_H_
#define _ARDUINO_SWPAR16_H_

#include "Arduino_DataBus.h"

class Arduino_SWPAR16 : public Arduino_DataBus
{
public:
  Arduino_SWPAR16(
      int8_t dc, int8_t cs, int8_t wr, int8_t rd,
      int8_t d0, int8_t d1, int8_t d2, int8_t d3, int8_t d4, int8_t d5, int8_t d6, int8_t d7,
      int8_t d8, int8_t d9, int8_t d10, int8_t d11, int8_t d12, int8_t d13, int8_t d14, int8_t d15); // Constructor

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
  void writeC8D8(uint8_t c, uint8_t d) override;
  void writeC8D16(uint8_t c, uint16_t d) override;
  void writeC8D16D16(uint8_t c, uint16_t d1, uint16_t d2) override;
  void writeC8D16D16Split(uint8_t c, uint16_t d1, uint16_t d2) override;
  void writeBytes(uint8_t *data, uint32_t len) override;

  void writeIndexedPixels(uint8_t *data, uint16_t *idx, uint32_t len) override;
  void writeIndexedPixelsDouble(uint8_t *data, uint16_t *idx, uint32_t len) override;
#endif // !defined(LITTLE_FOOT_PRINT)

protected:
private:
  INLINE void WRITE(uint8_t d);
  INLINE void WRITE16(uint16_t d);
  INLINE void WRITEREPEAT(uint16_t p, uint32_t len);
  INLINE void DC_HIGH(void);
  INLINE void DC_LOW(void);
  INLINE void CS_HIGH(void);
  INLINE void CS_LOW(void);
  INLINE void WR_HIGH(void);
  INLINE void WR_LOW(void);
  INLINE void RD_HIGH(void);
  INLINE void RD_LOW(void);
  INLINE void D0_HIGH(void);
  INLINE void D0_LOW(void);
  INLINE void D1_HIGH(void);
  INLINE void D1_LOW(void);
  INLINE void D2_HIGH(void);
  INLINE void D2_LOW(void);
  INLINE void D3_HIGH(void);
  INLINE void D3_LOW(void);
  INLINE void D4_HIGH(void);
  INLINE void D4_LOW(void);
  INLINE void D5_HIGH(void);
  INLINE void D5_LOW(void);
  INLINE void D6_HIGH(void);
  INLINE void D6_LOW(void);
  INLINE void D7_HIGH(void);
  INLINE void D7_LOW(void);
  INLINE void D8_HIGH(void);
  INLINE void D8_LOW(void);
  INLINE void D9_HIGH(void);
  INLINE void D9_LOW(void);
  INLINE void D10_HIGH(void);
  INLINE void D10_LOW(void);
  INLINE void D11_HIGH(void);
  INLINE void D11_LOW(void);
  INLINE void D12_HIGH(void);
  INLINE void D12_LOW(void);
  INLINE void D13_HIGH(void);
  INLINE void D13_LOW(void);
  INLINE void D14_HIGH(void);
  INLINE void D14_LOW(void);
  INLINE void D15_HIGH(void);
  INLINE void D15_LOW(void);

  int8_t _dc, _cs, _wr, _rd;
  int8_t _d0, _d1, _d2, _d3, _d4, _d5, _d6, _d7;
  int8_t _d8, _d9, _d10, _d11, _d12, _d13, _d14, _d15;

  // CLASS INSTANCE VARIABLES --------------------------------------------

  // Here be dragons! There's a big union of three structures here --
  // one each for hardware SPI, software (bitbang) SPI, and parallel
  // interfaces. This is to save some memory, since a display's connection
  // will be only one of these. The order of some things is a little weird
  // in an attempt to get values to align and pack better in RAM.

#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
  PORTreg_t _dcPortSet;
  PORTreg_t _dcPortClr;
  PORTreg_t _csPortSet;
  PORTreg_t _csPortClr;
  PORTreg_t _wrPortSet;
  PORTreg_t _wrPortClr;
  PORTreg_t _rdPortSet;
  PORTreg_t _rdPortClr;
  PORTreg_t _d0PortSet;
  PORTreg_t _d0PortClr;
  PORTreg_t _d1PortSet;
  PORTreg_t _d1PortClr;
  PORTreg_t _d2PortSet;
  PORTreg_t _d2PortClr;
  PORTreg_t _d3PortSet;
  PORTreg_t _d3PortClr;
  PORTreg_t _d4PortSet;
  PORTreg_t _d4PortClr;
  PORTreg_t _d5PortSet;
  PORTreg_t _d5PortClr;
  PORTreg_t _d6PortSet;
  PORTreg_t _d6PortClr;
  PORTreg_t _d7PortSet;
  PORTreg_t _d7PortClr;
  PORTreg_t _d8PortSet;
  PORTreg_t _d8PortClr;
  PORTreg_t _d9PortSet;
  PORTreg_t _d9PortClr;
  PORTreg_t _d10PortSet;
  PORTreg_t _d10PortClr;
  PORTreg_t _d11PortSet;
  PORTreg_t _d11PortClr;
  PORTreg_t _d12PortSet;
  PORTreg_t _d12PortClr;
  PORTreg_t _d13PortSet;
  PORTreg_t _d13PortClr;
  PORTreg_t _d14PortSet;
  PORTreg_t _d14PortClr;
  PORTreg_t _d15PortSet;
  PORTreg_t _d15PortClr;
#if !defined(KINETISK)
  ARDUINOGFX_PORT_t _dcPinMask;
  ARDUINOGFX_PORT_t _csPinMask;
  ARDUINOGFX_PORT_t _wrPinMask;
  ARDUINOGFX_PORT_t _rdPinMask;
  ARDUINOGFX_PORT_t _d0PinMask;
  ARDUINOGFX_PORT_t _d1PinMask;
  ARDUINOGFX_PORT_t _d2PinMask;
  ARDUINOGFX_PORT_t _d3PinMask;
  ARDUINOGFX_PORT_t _d4PinMask;
  ARDUINOGFX_PORT_t _d5PinMask;
  ARDUINOGFX_PORT_t _d6PinMask;
  ARDUINOGFX_PORT_t _d7PinMask;
  ARDUINOGFX_PORT_t _d8PinMask;
  ARDUINOGFX_PORT_t _d9PinMask;
  ARDUINOGFX_PORT_t _d10PinMask;
  ARDUINOGFX_PORT_t _d11PinMask;
  ARDUINOGFX_PORT_t _d12PinMask;
  ARDUINOGFX_PORT_t _d13PinMask;
  ARDUINOGFX_PORT_t _d14PinMask;
  ARDUINOGFX_PORT_t _d15PinMask;
#endif                            // !KINETISK
#else                             // !HAS_PORT_SET_CLR
  PORTreg_t _dcPort;
  PORTreg_t _csPort;
  PORTreg_t _wrPort;
  PORTreg_t _rdPort;
  PORTreg_t _d0Port;
  PORTreg_t _d1Port;
  PORTreg_t _d2Port;
  PORTreg_t _d3Port;
  PORTreg_t _d4Port;
  PORTreg_t _d5Port;
  PORTreg_t _d6Port;
  PORTreg_t _d7Port;
  PORTreg_t _d8Port;
  PORTreg_t _d9Port;
  PORTreg_t _d10Port;
  PORTreg_t _d11Port;
  PORTreg_t _d12Port;
  PORTreg_t _d13Port;
  PORTreg_t _d14Port;
  PORTreg_t _d15Port;
  ARDUINOGFX_PORT_t _dcPinMaskSet;
  ARDUINOGFX_PORT_t _dcPinMaskClr;
  ARDUINOGFX_PORT_t _csPinMaskSet;
  ARDUINOGFX_PORT_t _csPinMaskClr;
  ARDUINOGFX_PORT_t _wrPinMaskSet;
  ARDUINOGFX_PORT_t _wrPinMaskClr;
  ARDUINOGFX_PORT_t _rdPinMaskSet;
  ARDUINOGFX_PORT_t _rdPinMaskClr;
  ARDUINOGFX_PORT_t _d0PinMaskSet;
  ARDUINOGFX_PORT_t _d0PinMaskClr;
  ARDUINOGFX_PORT_t _d1PinMaskSet;
  ARDUINOGFX_PORT_t _d1PinMaskClr;
  ARDUINOGFX_PORT_t _d2PinMaskSet;
  ARDUINOGFX_PORT_t _d2PinMaskClr;
  ARDUINOGFX_PORT_t _d3PinMaskSet;
  ARDUINOGFX_PORT_t _d3PinMaskClr;
  ARDUINOGFX_PORT_t _d4PinMaskSet;
  ARDUINOGFX_PORT_t _d4PinMaskClr;
  ARDUINOGFX_PORT_t _d5PinMaskSet;
  ARDUINOGFX_PORT_t _d5PinMaskClr;
  ARDUINOGFX_PORT_t _d6PinMaskSet;
  ARDUINOGFX_PORT_t _d6PinMaskClr;
  ARDUINOGFX_PORT_t _d7PinMaskSet;
  ARDUINOGFX_PORT_t _d7PinMaskClr;
  ARDUINOGFX_PORT_t _d8PinMaskSet;
  ARDUINOGFX_PORT_t _d8PinMaskClr;
  ARDUINOGFX_PORT_t _d9PinMaskSet;
  ARDUINOGFX_PORT_t _d9PinMaskClr;
  ARDUINOGFX_PORT_t _d10PinMaskSet;
  ARDUINOGFX_PORT_t _d10PinMaskClr;
  ARDUINOGFX_PORT_t _d11PinMaskSet;
  ARDUINOGFX_PORT_t _d11PinMaskClr;
  ARDUINOGFX_PORT_t _d12PinMaskSet;
  ARDUINOGFX_PORT_t _d12PinMaskClr;
  ARDUINOGFX_PORT_t _d13PinMaskSet;
  ARDUINOGFX_PORT_t _d13PinMaskClr;
  ARDUINOGFX_PORT_t _d14PinMaskSet;
  ARDUINOGFX_PORT_t _d14PinMaskClr;
  ARDUINOGFX_PORT_t _d15PinMaskSet;
  ARDUINOGFX_PORT_t _d15PinMaskClr;
#endif                            // HAS_PORT_SET_CLR
#endif                            // defined(USE_FAST_PINIO)
};

#endif // _ARDUINO_SWPAR16_H_
