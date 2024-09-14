#ifdef RTL8722DM

#include "Arduino_DataBus.h"
#include "Arduino_RTLPAR8.h"

Arduino_RTLPAR8::Arduino_RTLPAR8(
    int8_t dc, int8_t cs, int8_t wr, int8_t rd,
    int8_t d0, int8_t d1, int8_t d2, int8_t d3, int8_t d4, int8_t d5, int8_t d6, int8_t d7)
    : _dc(dc), _cs(cs), _wr(wr), _rd(rd),
      _d0(d0), _d1(d1), _d2(d2), _d3(d3), _d4(d4), _d5(d5), _d6(d6), _d7(d7)
{
}

bool Arduino_RTLPAR8::begin(int32_t speed, int8_t dataMode)
{
  UNUSED(speed);
  UNUSED(dataMode);

  pinMode(_dc, OUTPUT);
  digitalWrite(_dc, HIGH); // Data mode
  _dcPort = (PORTreg_t)portOutputRegister(digitalPinToPort(_dc));
  _dcPinMaskSet = digitalPinToBitMask(_dc);
  _dcPinMaskClr = ~_dcPinMaskSet;

  if (_cs != GFX_NOT_DEFINED)
  {
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH); // disable chip select
    _csPort = (PORTreg_t)portOutputRegister(digitalPinToPort(_cs));
    _csPinMaskSet = digitalPinToBitMask(_cs);
  }
  _csPinMaskClr = ~_csPinMaskSet;

  pinMode(_wr, OUTPUT);
  digitalWrite(_wr, HIGH); // Set write strobe high (inactive)
  _wrPort = (PORTreg_t)portOutputRegister(digitalPinToPort(_wr));
  _wrPinMaskSet = digitalPinToBitMask(_wr);
  _wrPinMaskClr = ~_wrPinMaskSet;

  if (_rd != GFX_NOT_DEFINED)
  {
    pinMode(_rd, OUTPUT);
    digitalWrite(_rd, HIGH);
    _rdPort = (PORTreg_t)portOutputRegister(digitalPinToPort(_rd));
    _rdPinMaskSet = digitalPinToBitMask(_rd);
  }
  else
  {
    _rdPort = _dcPort;
    _rdPinMaskSet = 0;
  }
  _rdPinMaskClr = ~_rdPinMaskSet;

  // TODO: check pin in same port
  pinMode(_d0, OUTPUT);
  digitalWrite(_d0, HIGH);
  pinMode(_d1, OUTPUT);
  digitalWrite(_d1, HIGH);
  pinMode(_d2, OUTPUT);
  digitalWrite(_d2, HIGH);
  pinMode(_d3, OUTPUT);
  digitalWrite(_d3, HIGH);
  pinMode(_d4, OUTPUT);
  digitalWrite(_d4, HIGH);
  pinMode(_d5, OUTPUT);
  digitalWrite(_d5, HIGH);
  pinMode(_d6, OUTPUT);
  digitalWrite(_d6, HIGH);
  pinMode(_d7, OUTPUT);
  digitalWrite(_d7, HIGH);

  // INIT 8-bit mask
  _dataPort = (PORTreg_t)portOutputRegister(digitalPinToPort(_d0));
  for (int32_t c = 0; c < 256; c++)
  {
    _xset_mask[c] = 0;
    if (c & 0x01)
    {
      _xset_mask[c] |= digitalPinToBitMask(_d0);
    }
    if (c & 0x02)
    {
      _xset_mask[c] |= digitalPinToBitMask(_d1);
    }
    if (c & 0x04)
    {
      _xset_mask[c] |= digitalPinToBitMask(_d2);
    }
    if (c & 0x08)
    {
      _xset_mask[c] |= digitalPinToBitMask(_d3);
    }
    if (c & 0x10)
    {
      _xset_mask[c] |= digitalPinToBitMask(_d4);
    }
    if (c & 0x20)
    {
      _xset_mask[c] |= digitalPinToBitMask(_d5);
    }
    if (c & 0x40)
    {
      _xset_mask[c] |= digitalPinToBitMask(_d6);
    }
    if (c & 0x80)
    {
      _xset_mask[c] |= digitalPinToBitMask(_d7);
    }
  }
  _dataPinMaskClr = ~_xset_mask[255];

  return true;
}

void Arduino_RTLPAR8::beginWrite()
{
  DC_HIGH();
  CS_LOW();
}

void Arduino_RTLPAR8::endWrite()
{
  CS_HIGH();
}

void Arduino_RTLPAR8::writeCommand(uint8_t c)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();
}

void Arduino_RTLPAR8::writeCommand16(uint16_t c)
{
  DC_LOW();

  _data16.value = c;
  WRITE(_data16.msb);
  WRITE(_data16.lsb);

  DC_HIGH();
}

void Arduino_RTLPAR8::write(uint8_t d)
{
  WRITE(d);
}

void Arduino_RTLPAR8::write16(uint16_t d)
{
  _data16.value = d;
  WRITE(_data16.msb);
  WRITE(_data16.lsb);
}

void Arduino_RTLPAR8::writeRepeat(uint16_t p, uint32_t len)
{
  uint32_t dataMaskBase = *_dataPort & _dataPinMaskClr;
  uint32_t wrMaskBase = *_wrPort & _wrPinMaskClr;
  uint32_t wrMaskSet = wrMaskBase | _wrPinMaskSet;
  _data16.value = p;
  if (_data16.msb == _data16.lsb)
  {
    *_dataPort = dataMaskBase | _xset_mask[_data16.msb];
    while (len--)
    {
      *_wrPort = wrMaskBase;
      *_wrPort = wrMaskSet;
      *_wrPort = wrMaskBase;
      *_wrPort = wrMaskSet;
    }
  }
  else
  {
    uint32_t hiMask = _xset_mask[_data16.msb];
    uint32_t loMask = _xset_mask[_data16.lsb];
    while (len--)
    {
      *_dataPort = dataMaskBase | hiMask;
      *_wrPort = wrMaskBase;
      *_wrPort = wrMaskSet;

      *_dataPort = dataMaskBase | loMask;
      *_wrPort = wrMaskBase;
      *_wrPort = wrMaskSet;
    }
  }
}

void Arduino_RTLPAR8::writePixels(uint16_t *data, uint32_t len)
{
  uint32_t dataMaskBase = *_dataPort & _dataPinMaskClr;
  uint32_t wrMaskBase = *_wrPort & _wrPinMaskClr;
  uint32_t wrMaskSet = wrMaskBase | _wrPinMaskSet;
  while (len--)
  {
    _data16.value = *(data++);
    uint32_t hiMask = _xset_mask[_data16.msb];
    uint32_t loMask = _xset_mask[_data16.lsb];
    *_dataPort = dataMaskBase | hiMask;
    *_wrPort = wrMaskBase;
    *_wrPort = wrMaskSet;

    *_dataPort = dataMaskBase | loMask;
    *_wrPort = wrMaskBase;
    *_wrPort = wrMaskSet;
  }
}

void Arduino_RTLPAR8::writeC8D8(uint8_t c, uint8_t d)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  WRITE(d);
}

void Arduino_RTLPAR8::writeC8D16(uint8_t c, uint16_t d)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  _data16.value = d;
  WRITE(_data16.msb);
  WRITE(_data16.lsb);
}

void Arduino_RTLPAR8::writeC8D16D16(uint8_t c, uint16_t d1, uint16_t d2)
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

void Arduino_RTLPAR8::writeBytes(uint8_t *data, uint32_t len)
{
  while (len--)
  {
    WRITE(*data++);
  }
}

void Arduino_RTLPAR8::writeIndexedPixels(uint8_t *data, uint16_t *idx, uint32_t len)
{
  uint32_t dataMaskBase = *_dataPort & _dataPinMaskClr;
  uint32_t wrMaskBase = *_wrPort & _wrPinMaskClr;
  uint32_t wrMaskSet = wrMaskBase | _wrPinMaskSet;
  while (len--)
  {
    _data16.value = idx[*data++];

    *_dataPort = dataMaskBase | _xset_mask[_data16.msb];
    *_wrPort = wrMaskBase;
    *_wrPort = wrMaskSet;

    *_dataPort = dataMaskBase | _xset_mask[_data16.lsb];
    *_wrPort = wrMaskBase;
    *_wrPort = wrMaskSet;
  }
}

void Arduino_RTLPAR8::writeIndexedPixelsDouble(uint8_t *data, uint16_t *idx, uint32_t len)
{
  uint32_t dataMaskBase = *_dataPort & _dataPinMaskClr;
  uint32_t wrMaskBase = *_wrPort & _wrPinMaskClr;
  uint32_t wrMaskSet = wrMaskBase | _wrPinMaskSet;
  while (len--)
  {
    _data16.value = idx[*data++];
    uint32_t hiMask = _xset_mask[_data16.msb];
    uint32_t loMask = _xset_mask[_data16.lsb];

    *_dataPort = dataMaskBase | hiMask;
    *_wrPort = wrMaskBase;
    *_wrPort = wrMaskSet;

    *_dataPort = dataMaskBase | loMask;
    *_wrPort = wrMaskBase;
    *_wrPort = wrMaskSet;

    *_dataPort = dataMaskBase | hiMask;
    *_wrPort = wrMaskBase;
    *_wrPort = wrMaskSet;

    *_dataPort = dataMaskBase | loMask;
    *_wrPort = wrMaskBase;
    *_wrPort = wrMaskSet;
  }
}

INLINE void Arduino_RTLPAR8::WRITE(uint8_t d)
{
  uint32_t dataMaskBase = *_dataPort & _dataPinMaskClr;
  uint32_t wrMaskBase = *_wrPort & _wrPinMaskClr;
  *_dataPort = dataMaskBase | _xset_mask[d];

  *_wrPort = wrMaskBase;
  *_wrPort = wrMaskBase | _wrPinMaskSet;
}

/******** low level bit twiddling **********/

INLINE void Arduino_RTLPAR8::DC_HIGH(void)
{
  *_dcPort |= _dcPinMaskSet;
}

INLINE void Arduino_RTLPAR8::DC_LOW(void)
{
  *_dcPort &= _dcPinMaskClr;
}

INLINE void Arduino_RTLPAR8::CS_HIGH(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
    *_csPort |= _csPinMaskSet;
  }
}

INLINE void Arduino_RTLPAR8::CS_LOW(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
    *_csPort &= _csPinMaskClr;
  }
}

#endif // #ifdef RTL8722DM
