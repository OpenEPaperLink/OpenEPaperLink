#if defined(TARGET_RP2040)

#ifndef _ARDUINO_RPIPICOPAR8_H_
#define _ARDUINO_RPIPICOPAR8_H_

#include "Arduino_DataBus.h"

class Arduino_RPiPicoPAR8 : public Arduino_DataBus
{
public:
  Arduino_RPiPicoPAR8(int8_t dc, int8_t cs, int8_t wr, int8_t rd); // Constructor

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

  uint32_t _dcPinMask;  ///< Bitmask
  uint32_t _csPinMask;  ///< Bitmask
  uint32_t _wrPinMask;  ///< Bitmask
  uint32_t _rdPinMask;  ///< Bitmask
  uint32_t _dataClrMask;
};

#endif // _ARDUINO_RPIPICOPAR8_H_

#endif // #if defined(TARGET_RP2040)
