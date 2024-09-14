#include "Arduino_DataBus.h"

#if defined(ESP32) && (CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3)

#ifndef _ARDUINO_ESP32PAR16_H_
#define _ARDUINO_ESP32PAR16_H_

class Arduino_ESP32PAR16 : public Arduino_DataBus
{
public:
  Arduino_ESP32PAR16(
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

  void writeC8D8(uint8_t c, uint8_t d) override;
  void writeC8D16(uint8_t c, uint16_t d) override;
  void writeC8D16D16(uint8_t c, uint16_t d1, uint16_t d2) override;
  void writeC8D16D16Split(uint8_t c, uint16_t d1, uint16_t d2) override;
  void writeBytes(uint8_t *data, uint32_t len) override;

  void writeIndexedPixels(uint8_t *data, uint16_t *idx, uint32_t len) override;
  void writeIndexedPixelsDouble(uint8_t *data, uint16_t *idx, uint32_t len) override;

protected:
private:
  INLINE void WRITE(uint8_t d);
  INLINE void WRITE16(uint16_t d);
  INLINE void DC_HIGH(void);
  INLINE void DC_LOW(void);
  INLINE void CS_HIGH(void);
  INLINE void CS_LOW(void);

  int8_t _dc, _cs, _wr, _rd;
  int8_t _d0, _d1, _d2, _d3, _d4, _d5, _d6, _d7;
  int8_t _d8, _d9, _d10, _d11, _d12, _d13, _d14, _d15;

  PORTreg_t _dcPortSet; ///< PORT register SET
  PORTreg_t _dcPortClr; ///< PORT register CLEAR
  uint32_t _dcPinMask;  ///< Bitmask

  PORTreg_t _csPortSet; ///< PORT register SET
  PORTreg_t _csPortClr; ///< PORT register CLEAR
  uint32_t _csPinMask;  ///< Bitmask

  PORTreg_t _wrPortSet; ///< PORT register SET
  PORTreg_t _wrPortClr; ///< PORT register CLEAR
  uint32_t _wrPinMask;  ///< Bitmask

  uint32_t _data1ClrMask;
  uint32_t _data2ClrMask;
  // Lookup table for ESP32 parallel bus interface uses 4kbyte RAM
  uint32_t _xset_mask1_lo[256];
  uint32_t _xset_mask1_hi[256];
  uint32_t _xset_mask2_lo[256];
  uint32_t _xset_mask2_hi[256];
};

#endif // _ARDUINO_ESP32PAR16_H_

#endif // #if defined(ESP32) && (CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3)
