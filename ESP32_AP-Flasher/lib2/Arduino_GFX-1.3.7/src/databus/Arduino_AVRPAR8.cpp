#ifdef __AVR__

#include "Arduino_AVRPAR8.h"

Arduino_AVRPAR8::Arduino_AVRPAR8(int8_t dc, int8_t cs, int8_t wr, int8_t rd, uint8_t port)
    : _dc(dc), _cs(cs), _wr(wr), _rd(rd), _port(port)
{
}

bool Arduino_AVRPAR8::begin(int32_t speed, int8_t dataMode)
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
    digitalWrite(_cs, HIGH); // Disable chip select
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
    digitalWrite(_rd, HIGH); // Disable RD
    _rdPort = (PORTreg_t)portOutputRegister(digitalPinToPort(_rd));
    _rdPinMaskSet = digitalPinToBitMask(_rd);
  }
  else
  {
    _rdPort = _dcPort;
    _rdPinMaskSet = 0;
  }
  _rdPinMaskClr = ~_rdPinMaskSet;

  *(portModeRegister(_port)) = 0xFF;
  _dataPort = portOutputRegister(_port);
  *_dataPort = 0xFF;

  return true;
}

void Arduino_AVRPAR8::beginWrite()
{
  DC_HIGH();
  CS_LOW();
}

void Arduino_AVRPAR8::endWrite()
{
  CS_HIGH();
}

void Arduino_AVRPAR8::writeCommand(uint8_t c)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();
}

void Arduino_AVRPAR8::writeCommand16(uint16_t c)
{
  DC_LOW();

  _data16.value = c;
  WRITE(_data16.msb);
  WRITE(_data16.lsb);

  DC_HIGH();
}

void Arduino_AVRPAR8::write(uint8_t d)
{
  WRITE(d);
}

void Arduino_AVRPAR8::write16(uint16_t d)
{
  _data16.value = d;
  WRITE(_data16.msb);
  WRITE(_data16.lsb);
}

void Arduino_AVRPAR8::writeRepeat(uint16_t p, uint32_t len)
{
  uint8_t wrMaskBase = *_wrPort & _wrPinMaskClr;
  uint8_t wrMaskSet = wrMaskBase | _wrPinMaskSet;
  _data16.value = p;
  if (_data16.msb == _data16.lsb)
  {
    *_dataPort = _data16.msb;
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
    while (len--)
    {
      *_dataPort = _data16.msb;
      *_wrPort = wrMaskBase;
      *_wrPort = wrMaskSet;

      *_dataPort = _data16.lsb;
      *_wrPort = wrMaskBase;
      *_wrPort = wrMaskSet;
    }
  }
}

void Arduino_AVRPAR8::writePixels(uint16_t *data, uint32_t len)
{
  while (len--)
  {
    _data16.value = *data++;
    WRITE(_data16.msb);
    WRITE(_data16.lsb);
  }
}

void Arduino_AVRPAR8::writeC8D8(uint8_t c, uint8_t d)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  WRITE(d);
}

void Arduino_AVRPAR8::writeC8D16(uint8_t c, uint16_t d)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  _data16.value = d;
  WRITE(_data16.msb);
  WRITE(_data16.lsb);
}

void Arduino_AVRPAR8::writeC8D16D16(uint8_t c, uint16_t d1, uint16_t d2)
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

INLINE void Arduino_AVRPAR8::WRITE(uint8_t d)
{
  uint8_t wrMaskBase = *_wrPort & _wrPinMaskClr;
  *_dataPort = d;
  *_wrPort = wrMaskBase;
  *_wrPort = wrMaskBase | _wrPinMaskSet;
}

/******** low level bit twiddling **********/

INLINE void Arduino_AVRPAR8::DC_HIGH(void)
{
  *_dcPort |= _dcPinMaskSet;
}

INLINE void Arduino_AVRPAR8::DC_LOW(void)
{
  *_dcPort &= _dcPinMaskClr;
}

INLINE void Arduino_AVRPAR8::CS_HIGH(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
    *_csPort |= _csPinMaskSet;
  }
}

INLINE void Arduino_AVRPAR8::CS_LOW(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
    *_csPort &= _csPinMaskClr;
  }
}

#endif // #ifdef __AVR__
