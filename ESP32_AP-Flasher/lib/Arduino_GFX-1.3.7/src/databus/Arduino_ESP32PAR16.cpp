#include "Arduino_ESP32PAR16.h"

#if defined(ESP32) && (CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3)

Arduino_ESP32PAR16::Arduino_ESP32PAR16(
    int8_t dc, int8_t cs, int8_t wr, int8_t rd,
    int8_t d0, int8_t d1, int8_t d2, int8_t d3, int8_t d4, int8_t d5, int8_t d6, int8_t d7,
    int8_t d8, int8_t d9, int8_t d10, int8_t d11, int8_t d12, int8_t d13, int8_t d14, int8_t d15)
    : _dc(dc), _cs(cs), _wr(wr), _rd(rd),
      _d0(d0), _d1(d1), _d2(d2), _d3(d3), _d4(d4), _d5(d5), _d6(d6), _d7(d7),
      _d8(d8), _d9(d9), _d10(d10), _d11(d11), _d12(d12), _d13(d13), _d14(d14), _d15(d15)
{
}

bool Arduino_ESP32PAR16::begin(int32_t speed, int8_t dataMode)
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
  _data1ClrMask = 0;
  _data2ClrMask = 0;
  if (_d0 >= 32)
  {
    _data2ClrMask |= digitalPinToBitMask(_d0);
  }
  else
  {
    _data1ClrMask |= digitalPinToBitMask(_d0);
  }
  if (_d1 >= 32)
  {
    _data2ClrMask |= digitalPinToBitMask(_d1);
  }
  else
  {
    _data1ClrMask |= digitalPinToBitMask(_d1);
  }
  if (_d2 >= 32)
  {
    _data2ClrMask |= digitalPinToBitMask(_d2);
  }
  else
  {
    _data1ClrMask |= digitalPinToBitMask(_d2);
  }
  if (_d3 >= 32)
  {
    _data2ClrMask |= digitalPinToBitMask(_d3);
  }
  else
  {
    _data1ClrMask |= digitalPinToBitMask(_d3);
  }
  if (_d4 >= 32)
  {
    _data2ClrMask |= digitalPinToBitMask(_d4);
  }
  else
  {
    _data1ClrMask |= digitalPinToBitMask(_d4);
  }
  if (_d5 >= 32)
  {
    _data2ClrMask |= digitalPinToBitMask(_d5);
  }
  else
  {
    _data1ClrMask |= digitalPinToBitMask(_d5);
  }
  if (_d6 >= 32)
  {
    _data2ClrMask |= digitalPinToBitMask(_d6);
  }
  else
  {
    _data1ClrMask |= digitalPinToBitMask(_d6);
  }
  if (_d7 >= 32)
  {
    _data2ClrMask |= digitalPinToBitMask(_d7);
  }
  else
  {
    _data1ClrMask |= digitalPinToBitMask(_d7);
  }
  if (_d8 >= 32)
  {
    _data2ClrMask |= digitalPinToBitMask(_d8);
  }
  else
  {
    _data1ClrMask |= digitalPinToBitMask(_d8);
  }
  if (_d9 >= 32)
  {
    _data2ClrMask |= digitalPinToBitMask(_d9);
  }
  else
  {
    _data1ClrMask |= digitalPinToBitMask(_d9);
  }
  if (_d10 >= 32)
  {
    _data2ClrMask |= digitalPinToBitMask(_d10);
  }
  else
  {
    _data1ClrMask |= digitalPinToBitMask(_d10);
  }
  if (_d11 >= 32)
  {
    _data2ClrMask |= digitalPinToBitMask(_d11);
  }
  else
  {
    _data1ClrMask |= digitalPinToBitMask(_d11);
  }
  if (_d12 >= 32)
  {
    _data2ClrMask |= digitalPinToBitMask(_d12);
  }
  else
  {
    _data1ClrMask |= digitalPinToBitMask(_d12);
  }
  if (_d13 >= 32)
  {
    _data2ClrMask |= digitalPinToBitMask(_d13);
  }
  else
  {
    _data1ClrMask |= digitalPinToBitMask(_d13);
  }
  if (_d14 >= 32)
  {
    _data2ClrMask |= digitalPinToBitMask(_d14);
  }
  else
  {
    _data1ClrMask |= digitalPinToBitMask(_d14);
  }
  if (_d15 >= 32)
  {
    _data2ClrMask |= digitalPinToBitMask(_d15);
  }
  else
  {
    _data1ClrMask |= digitalPinToBitMask(_d15);
  }

  for (int32_t c = 0; c < 256; c++)
  {
    _xset_mask1_lo[c] = 0;
    _xset_mask2_lo[c] = 0;
    if (c & 0x01)
    {
      if (_d0 >= 32)
      {
        _xset_mask2_lo[c] |= digitalPinToBitMask(_d0);
      }
      else
      {
        _xset_mask1_lo[c] |= digitalPinToBitMask(_d0);
      }
    }
    if (c & 0x02)
    {
      if (_d1 >= 32)
      {
        _xset_mask2_lo[c] |= digitalPinToBitMask(_d1);
      }
      else
      {
        _xset_mask1_lo[c] |= digitalPinToBitMask(_d1);
      }
    }
    if (c & 0x04)
    {
      if (_d2 >= 32)
      {
        _xset_mask2_lo[c] |= digitalPinToBitMask(_d2);
      }
      else
      {
        _xset_mask1_lo[c] |= digitalPinToBitMask(_d2);
      }
    }
    if (c & 0x08)
    {
      if (_d3 >= 32)
      {
        _xset_mask2_lo[c] |= digitalPinToBitMask(_d3);
      }
      else
      {
        _xset_mask1_lo[c] |= digitalPinToBitMask(_d3);
      }
    }
    if (c & 0x10)
    {
      if (_d4 >= 32)
      {
        _xset_mask2_lo[c] |= digitalPinToBitMask(_d4);
      }
      else
      {
        _xset_mask1_lo[c] |= digitalPinToBitMask(_d4);
      }
    }
    if (c & 0x20)
    {
      if (_d5 >= 32)
      {
        _xset_mask2_lo[c] |= digitalPinToBitMask(_d5);
      }
      else
      {
        _xset_mask1_lo[c] |= digitalPinToBitMask(_d5);
      }
    }
    if (c & 0x40)
    {
      if (_d6 >= 32)
      {
        _xset_mask2_lo[c] |= digitalPinToBitMask(_d6);
      }
      else
      {
        _xset_mask1_lo[c] |= digitalPinToBitMask(_d6);
      }
    }
    if (c & 0x80)
    {
      if (_d7 >= 32)
      {
        _xset_mask2_lo[c] |= digitalPinToBitMask(_d7);
      }
      else
      {
        _xset_mask1_lo[c] |= digitalPinToBitMask(_d7);
      }
    }
  }
  for (int32_t c = 0; c < 256; c++)
  {
    _xset_mask1_hi[c] = 0;
    _xset_mask2_hi[c] = 0;
    if (c & 0x01)
    {
      if (_d8 >= 32)
      {
        _xset_mask2_hi[c] |= digitalPinToBitMask(_d8);
      }
      else
      {
        _xset_mask1_hi[c] |= digitalPinToBitMask(_d8);
      }
    }
    if (c & 0x02)
    {
      if (_d9 >= 32)
      {
        _xset_mask2_hi[c] |= digitalPinToBitMask(_d9);
      }
      else
      {
        _xset_mask1_hi[c] |= digitalPinToBitMask(_d9);
      }
    }
    if (c & 0x04)
    {
      if (_d10 >= 32)
      {
        _xset_mask2_hi[c] |= digitalPinToBitMask(_d10);
      }
      else
      {
        _xset_mask1_hi[c] |= digitalPinToBitMask(_d10);
      }
    }
    if (c & 0x08)
    {
      if (_d11 >= 32)
      {
        _xset_mask2_hi[c] |= digitalPinToBitMask(_d11);
      }
      else
      {
        _xset_mask1_hi[c] |= digitalPinToBitMask(_d11);
      }
    }
    if (c & 0x10)
    {
      if (_d12 >= 32)
      {
        _xset_mask2_hi[c] |= digitalPinToBitMask(_d12);
      }
      else
      {
        _xset_mask1_hi[c] |= digitalPinToBitMask(_d12);
      }
    }
    if (c & 0x20)
    {
      if (_d13 >= 32)
      {
        _xset_mask2_hi[c] |= digitalPinToBitMask(_d13);
      }
      else
      {
        _xset_mask1_hi[c] |= digitalPinToBitMask(_d13);
      }
    }
    if (c & 0x40)
    {
      if (_d14 >= 32)
      {
        _xset_mask2_hi[c] |= digitalPinToBitMask(_d14);
      }
      else
      {
        _xset_mask1_hi[c] |= digitalPinToBitMask(_d14);
      }
    }
    if (c & 0x80)
    {
      if (_d15 >= 32)
      {
        _xset_mask2_hi[c] |= digitalPinToBitMask(_d15);
      }
      else
      {
        _xset_mask1_hi[c] |= digitalPinToBitMask(_d15);
      }
    }
  }
  GPIO.out_w1tc = _data1ClrMask;
  GPIO.out1_w1tc.val = _data2ClrMask;

  return true;
}

void Arduino_ESP32PAR16::beginWrite()
{
  DC_HIGH();
  CS_LOW();
}

void Arduino_ESP32PAR16::endWrite()
{
  CS_HIGH();
}

void Arduino_ESP32PAR16::writeCommand(uint8_t c)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();
}

void Arduino_ESP32PAR16::writeCommand16(uint16_t c)
{
  DC_LOW();

  WRITE16(c);

  DC_HIGH();
}

void Arduino_ESP32PAR16::write(uint8_t d)
{
  WRITE(d);
}

void Arduino_ESP32PAR16::write16(uint16_t d)
{
  WRITE16(d);
}

void Arduino_ESP32PAR16::writeRepeat(uint16_t p, uint32_t len)
{
  _data16.value = p;
  uint32_t d1 = _xset_mask1_hi[_data16.msb] | _xset_mask1_lo[_data16.lsb];
  uint32_t d2 = _xset_mask2_hi[_data16.msb] | _xset_mask2_lo[_data16.lsb];
  GPIO.out_w1tc = _data1ClrMask;
  GPIO.out1_w1tc.val = _data2ClrMask;
  GPIO.out_w1ts = d1;
  GPIO.out1_w1ts.val = d2;
  while (len--)
  {
    *_wrPortClr = _wrPinMask;
    *_wrPortSet = _wrPinMask;
  }
}

void Arduino_ESP32PAR16::writePixels(uint16_t *data, uint32_t len)
{
  while (len--)
  {
    WRITE16(*data++);
  }
}

void Arduino_ESP32PAR16::writeC8D8(uint8_t c, uint8_t d)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  WRITE(d);
}

void Arduino_ESP32PAR16::writeC8D16(uint8_t c, uint16_t d)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  WRITE16(d);
}

void Arduino_ESP32PAR16::writeC8D16D16(uint8_t c, uint16_t d1, uint16_t d2)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  WRITE16(d1);
  WRITE16(d2);
}

void Arduino_ESP32PAR16::writeC8D16D16Split(uint8_t c, uint16_t d1, uint16_t d2)
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

void Arduino_ESP32PAR16::writeBytes(uint8_t *data, uint32_t len)
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

void Arduino_ESP32PAR16::writeIndexedPixels(uint8_t *data, uint16_t *idx, uint32_t len)
{
  while (len--)
  {
    WRITE16(idx[*data++]);
  }
}

void Arduino_ESP32PAR16::writeIndexedPixelsDouble(uint8_t *data, uint16_t *idx, uint32_t len)
{
  while (len--)
  {
    _data16.value = idx[*data++];
    GPIO.out_w1tc = _data1ClrMask;
    GPIO.out1_w1tc.val = _data2ClrMask;
    GPIO.out_w1ts = _xset_mask1_hi[_data16.msb] | _xset_mask1_lo[_data16.lsb];
    GPIO.out1_w1ts.val = _xset_mask2_hi[_data16.msb] | _xset_mask2_lo[_data16.lsb];
    *_wrPortClr = _wrPinMask;
    *_wrPortSet = _wrPinMask;
    *_wrPortClr = _wrPinMask;
    *_wrPortSet = _wrPinMask;
  }
}

INLINE void Arduino_ESP32PAR16::WRITE(uint8_t d)
{
  GPIO.out_w1tc = _data1ClrMask;
  GPIO.out1_w1tc.val = _data2ClrMask;
  GPIO.out_w1ts = _xset_mask1_lo[d];
  GPIO.out1_w1ts.val = _xset_mask2_lo[d];
  *_wrPortClr = _wrPinMask;
  *_wrPortSet = _wrPinMask;
}

INLINE void Arduino_ESP32PAR16::WRITE16(uint16_t d)
{
  _data16.value = d;
  uint32_t d1 = _xset_mask1_hi[_data16.msb] | _xset_mask1_lo[_data16.lsb];
  uint32_t d2 = _xset_mask2_hi[_data16.msb] | _xset_mask2_lo[_data16.lsb];
  GPIO.out_w1tc = _data1ClrMask;
  GPIO.out1_w1tc.val = _data2ClrMask;
  GPIO.out_w1ts = d1;
  GPIO.out1_w1ts.val = d2;
  *_wrPortClr = _wrPinMask;
  *_wrPortSet = _wrPinMask;
}

/******** low level bit twiddling **********/

INLINE void Arduino_ESP32PAR16::DC_HIGH(void)
{
  *_dcPortSet = _dcPinMask;
}

INLINE void Arduino_ESP32PAR16::DC_LOW(void)
{
  *_dcPortClr = _dcPinMask;
}

INLINE void Arduino_ESP32PAR16::CS_HIGH(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
    *_csPortSet = _csPinMask;
  }
}

INLINE void Arduino_ESP32PAR16::CS_LOW(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
    *_csPortClr = _csPinMask;
  }
}

#endif // #if defined(ESP32) && (CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3)
