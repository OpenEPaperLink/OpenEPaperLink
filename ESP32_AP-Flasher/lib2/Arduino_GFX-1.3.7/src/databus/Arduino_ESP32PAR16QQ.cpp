#include "Arduino_ESP32PAR16QQ.h"

#if defined(ESP32) && (CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3)

Arduino_ESP32PAR16QQ::Arduino_ESP32PAR16QQ(
    int8_t dc, int8_t cs, int8_t wr, int8_t rd,
    int8_t d0, int8_t d1, int8_t d2, int8_t d3, int8_t d4, int8_t d5, int8_t d6, int8_t d7,
    int8_t d8, int8_t d9, int8_t d10, int8_t d11, int8_t d12, int8_t d13, int8_t d14, int8_t d15)
    : _dc(dc), _cs(cs), _wr(wr), _rd(rd),
      _d0(d0), _d1(d1), _d2(d2), _d3(d3), _d4(d4), _d5(d5), _d6(d6), _d7(d7),
      _d8(d8), _d9(d9), _d10(d10), _d11(d11), _d12(d12), _d13(d13), _d14(d14), _d15(d15)
{
}

bool Arduino_ESP32PAR16QQ::begin(int32_t speed, int8_t dataMode)
{
  UNUSED(speed);
  UNUSED(dataMode);

  pinMode(_dc, OUTPUT);
  digitalWrite(_dc, HIGH); // Data mode
  if (_dc >= 32)
  {
    _dcPinMask = digitalPinToBitMask(_dc);
    _dcPortSet = (PORTreg_t)&GPIO.out1_w1ts.val;
    _dcPortClr = (PORTreg_t)&GPIO.out1_w1tc.val;
  }
  else
  {
    _dcPinMask = digitalPinToBitMask(_dc);
    _dcPortSet = (PORTreg_t)&GPIO.out_w1ts;
    _dcPortClr = (PORTreg_t)&GPIO.out_w1tc;
  }

  if (_cs != GFX_NOT_DEFINED)
  {
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH); // disable chip select
  }
  if (_cs >= 32)
  {
    _csPinMask = digitalPinToBitMask(_cs);
    _csPortSet = (PORTreg_t)&GPIO.out1_w1ts.val;
    _csPortClr = (PORTreg_t)&GPIO.out1_w1tc.val;
  }
  else if (_cs != GFX_NOT_DEFINED)
  {
    _csPinMask = digitalPinToBitMask(_cs);
    _csPortSet = (PORTreg_t)&GPIO.out_w1ts;
    _csPortClr = (PORTreg_t)&GPIO.out_w1tc;
  }

  pinMode(_wr, OUTPUT);
  digitalWrite(_wr, HIGH); // Set write strobe high (inactive)
  if (_wr >= 32)
  {
    _wrPinMask = digitalPinToBitMask(_wr);
    _wrPortSet = (PORTreg_t)&GPIO.out1_w1ts.val;
    _wrPortClr = (PORTreg_t)&GPIO.out1_w1tc.val;
  }
  else
  {
    _wrPinMask = digitalPinToBitMask(_wr);
    _wrPortSet = (PORTreg_t)&GPIO.out_w1ts;
    _wrPortClr = (PORTreg_t)&GPIO.out_w1tc;
  }

  if (_rd != GFX_NOT_DEFINED)
  {
    pinMode(_rd, OUTPUT);
    digitalWrite(_rd, HIGH);
  }

  // TODO: check pin range 0-31
  pinMode(_d0, OUTPUT);
  pinMode(_d1, OUTPUT);
  pinMode(_d2, OUTPUT);
  pinMode(_d3, OUTPUT);
  pinMode(_d4, OUTPUT);
  pinMode(_d5, OUTPUT);
  pinMode(_d6, OUTPUT);
  pinMode(_d7, OUTPUT);
  pinMode(_d8, OUTPUT);
  pinMode(_d9, OUTPUT);
  pinMode(_d10, OUTPUT);
  pinMode(_d11, OUTPUT);
  pinMode(_d12, OUTPUT);
  pinMode(_d13, OUTPUT);
  pinMode(_d14, OUTPUT);
  pinMode(_d15, OUTPUT);

  // INIT 16-bit mask
  _dataClrMask = (1 << _wr) | (1 << _d0) | (1 << _d1) | (1 << _d2) | (1 << _d3) | (1 << _d4) | (1 << _d5) | (1 << _d6) | (1 << _d7) | (1 << _d8) | (1 << _d9) | (1 << _d10) | (1 << _d11) | (1 << _d12) | (1 << _d13) | (1 << _d14) | (1 << _d15);
  for (int32_t c = 0; c < 256; c++)
  {
    _xset_mask_lo[c] = (1 << _wr);
    if (c & 0x01)
    {
      _xset_mask_lo[c] |= (1 << _d0);
    }
    if (c & 0x02)
    {
      _xset_mask_lo[c] |= (1 << _d1);
    }
    if (c & 0x04)
    {
      _xset_mask_lo[c] |= (1 << _d2);
    }
    if (c & 0x08)
    {
      _xset_mask_lo[c] |= (1 << _d3);
    }
    if (c & 0x10)
    {
      _xset_mask_lo[c] |= (1 << _d4);
    }
    if (c & 0x20)
    {
      _xset_mask_lo[c] |= (1 << _d5);
    }
    if (c & 0x40)
    {
      _xset_mask_lo[c] |= (1 << _d6);
    }
    if (c & 0x80)
    {
      _xset_mask_lo[c] |= (1 << _d7);
    }
  }
  for (int32_t c = 0; c < 256; c++)
  {
    _xset_mask_hi[c] = 0;
    if (c & 0x01)
    {
      _xset_mask_hi[c] |= (1 << _d8);
    }
    if (c & 0x02)
    {
      _xset_mask_hi[c] |= (1 << _d9);
    }
    if (c & 0x04)
    {
      _xset_mask_hi[c] |= (1 << _d10);
    }
    if (c & 0x08)
    {
      _xset_mask_hi[c] |= (1 << _d11);
    }
    if (c & 0x10)
    {
      _xset_mask_hi[c] |= (1 << _d12);
    }
    if (c & 0x20)
    {
      _xset_mask_hi[c] |= (1 << _d13);
    }
    if (c & 0x40)
    {
      _xset_mask_hi[c] |= (1 << _d14);
    }
    if (c & 0x80)
    {
      _xset_mask_hi[c] |= (1 << _d15);
    }
  }
  _dataPortSet = (PORTreg_t)&GPIO.out_w1ts;
  _dataPortClr = (PORTreg_t)&GPIO.out_w1tc;
  *_dataPortClr = _dataClrMask;

  return true;
}

void Arduino_ESP32PAR16QQ::beginWrite()
{
  DC_HIGH();
  CS_LOW();
}

void Arduino_ESP32PAR16QQ::endWrite()
{
  CS_HIGH();
}

void Arduino_ESP32PAR16QQ::writeCommand(uint8_t c)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();
}

void Arduino_ESP32PAR16QQ::writeCommand16(uint16_t c)
{
  DC_LOW();

  WRITE16(c);

  DC_HIGH();
}

void Arduino_ESP32PAR16QQ::write(uint8_t d)
{
  WRITE(d);
}

void Arduino_ESP32PAR16QQ::write16(uint16_t d)
{
  WRITE16(d);
}

void Arduino_ESP32PAR16QQ::writeRepeat(uint16_t p, uint32_t len)
{
  _data16.value = p;
  uint32_t d = _xset_mask_hi[_data16.msb] | _xset_mask_lo[_data16.lsb];
  while (len--)
  {
    *_dataPortClr = _dataClrMask;
    *_dataPortSet = d;
  }
}

void Arduino_ESP32PAR16QQ::writePixels(uint16_t *data, uint32_t len)
{
  while (len--)
  {
    WRITE16(*data++);
  }
}

void Arduino_ESP32PAR16QQ::writeC8D8(uint8_t c, uint8_t d)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  WRITE(d);
}

void Arduino_ESP32PAR16QQ::writeC8D16(uint8_t c, uint16_t d)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  WRITE16(d);
}

void Arduino_ESP32PAR16QQ::writeC8D16D16(uint8_t c, uint16_t d1, uint16_t d2)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  WRITE16(d1);
  WRITE16(d2);
}

void Arduino_ESP32PAR16QQ::writeC8D16D16Split(uint8_t c, uint16_t d1, uint16_t d2)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  _data16.value = d1;
  WRITE(_data16.msb);
  WRITE(_data16.lsb);

  _data16.value = d2;
  WRITE(_data16.msb);
  WRITE(_data16.lsb);
}

void Arduino_ESP32PAR16QQ::writeBytes(uint8_t *data, uint32_t len)
{
  while (len > 1)
  {
    _data16.msb = *data++;
    _data16.lsb = *data++;
    WRITE16(_data16.value);
    len -= 2;
  }
  if (len)
  {
    WRITE(*data);
  }
}

void Arduino_ESP32PAR16QQ::writeIndexedPixels(uint8_t *data, uint16_t *idx, uint32_t len)
{
  while (len--)
  {
    WRITE16(idx[*data++]);
  }
}

void Arduino_ESP32PAR16QQ::writeIndexedPixelsDouble(uint8_t *data, uint16_t *idx, uint32_t len)
{
  while (len--)
  {
    _data16.value = idx[*data++];
    *_dataPortClr = _dataClrMask;
    *_dataPortSet = _xset_mask_hi[_data16.msb] | _xset_mask_lo[_data16.lsb];
    *_wrPortClr = _wrPinMask;
    *_wrPortSet = _wrPinMask;
  }
}

INLINE void Arduino_ESP32PAR16QQ::WRITE(uint8_t d)
{
  *_dataPortClr = _dataClrMask;
  *_dataPortSet = _xset_mask_lo[d];
}

INLINE void Arduino_ESP32PAR16QQ::WRITE16(uint16_t d)
{
  _data16.value = d;
  *_dataPortClr = _dataClrMask;
  *_dataPortSet = _xset_mask_hi[_data16.msb] | _xset_mask_lo[_data16.lsb];
}

/******** low level bit twiddling **********/

INLINE void Arduino_ESP32PAR16QQ::DC_HIGH(void)
{
  *_dcPortSet = _dcPinMask;
}

INLINE void Arduino_ESP32PAR16QQ::DC_LOW(void)
{
  *_dcPortClr = _dcPinMask;
}

INLINE void Arduino_ESP32PAR16QQ::CS_HIGH(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
    *_csPortSet = _csPinMask;
  }
}

INLINE void Arduino_ESP32PAR16QQ::CS_LOW(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
    *_csPortClr = _csPinMask;
  }
}

#endif // #if defined(ESP32) && (CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3)
