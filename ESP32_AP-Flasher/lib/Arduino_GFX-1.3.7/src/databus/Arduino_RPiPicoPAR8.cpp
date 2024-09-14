#if defined(TARGET_RP2040)

#include "Arduino_RPiPicoPAR8.h"

Arduino_RPiPicoPAR8::Arduino_RPiPicoPAR8(int8_t dc, int8_t cs, int8_t wr, int8_t rd)
    : _dc(dc), _cs(cs), _wr(wr), _rd(rd)
{
}

bool Arduino_RPiPicoPAR8::begin(int32_t speed, int8_t dataMode)
{
  pinMode(_dc, OUTPUT);
  digitalWrite(_dc, HIGH); // Data mode
  _dcPinMask = digitalPinToBitMask(_dc);
  if (_cs != GFX_NOT_DEFINED)
  {
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH); // disable chip select
    _csPinMask = digitalPinToBitMask(_cs);
  }
  pinMode(_wr, OUTPUT);
  digitalWrite(_wr, HIGH); // Set write strobe high (inactive)
  _wrPinMask = digitalPinToBitMask(_wr);
  _dataClrMask = 0xFF | _wrPinMask;
  if (_rd != GFX_NOT_DEFINED)
  {
    pinMode(_rd, OUTPUT);
    digitalWrite(_rd, HIGH);
    _rdPinMask = digitalPinToBitMask(_rd);
  }

  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  sio_hw->gpio_clr = 0xFF;

  return true;
}

void Arduino_RPiPicoPAR8::beginWrite()
{
  DC_HIGH();
  CS_LOW();
}

void Arduino_RPiPicoPAR8::endWrite()
{
  CS_HIGH();
}

void Arduino_RPiPicoPAR8::writeCommand(uint8_t c)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();
}

void Arduino_RPiPicoPAR8::writeCommand16(uint16_t c)
{
  DC_LOW();

  _data16.value = c;
  WRITE(_data16.msb);
  WRITE(_data16.lsb);

  DC_HIGH();
}

void Arduino_RPiPicoPAR8::write(uint8_t d)
{
  WRITE(d);
}

void Arduino_RPiPicoPAR8::write16(uint16_t d)
{
  _data16.value = d;
  WRITE(_data16.msb);
  WRITE(_data16.lsb);
}

void Arduino_RPiPicoPAR8::writeRepeat(uint16_t p, uint32_t len)
{
  _data16.value = p;
  if (_data16.msb == _data16.lsb)
  {
    sio_hw->gpio_clr = 0xFF;
    sio_hw->gpio_set = _data16.msb;
    while (len--)
    {
      sio_hw->gpio_clr = _wrPinMask;
      sio_hw->gpio_set = _wrPinMask;
      sio_hw->gpio_clr = _wrPinMask;
      sio_hw->gpio_set = _wrPinMask;
    }
  }
  else
  {
    while (len--)
    {
      sio_hw->gpio_clr = _dataClrMask;
      sio_hw->gpio_set = _data16.msb;
      sio_hw->gpio_set = _wrPinMask;

      sio_hw->gpio_clr = _dataClrMask;
      sio_hw->gpio_set = _data16.lsb;
      sio_hw->gpio_set = _wrPinMask;
    }
  }
}

void Arduino_RPiPicoPAR8::writePixels(uint16_t *data, uint32_t len)
{
  while (len--)
  {
    _data16.value = *data++;
    WRITE(_data16.msb);
    WRITE(_data16.lsb);
  }
}

void Arduino_RPiPicoPAR8::writeC8D8(uint8_t c, uint8_t d)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  WRITE(d);
}

void Arduino_RPiPicoPAR8::writeC8D16(uint8_t c, uint16_t d)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  _data16.value = d;
  WRITE(_data16.msb);
  WRITE(_data16.lsb);
}

void Arduino_RPiPicoPAR8::writeC8D16D16(uint8_t c, uint16_t d1, uint16_t d2)
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

void Arduino_RPiPicoPAR8::writeBytes(uint8_t *data, uint32_t len)
{
  while (len--)
  {
    WRITE(*data++);
  }
}

void Arduino_RPiPicoPAR8::writeIndexedPixels(uint8_t *data, uint16_t *idx, uint32_t len)
{
  while (len--)
  {
    _data16.value = idx[*data++];
    WRITE(_data16.msb);
    WRITE(_data16.lsb);
  }
}

void Arduino_RPiPicoPAR8::writeIndexedPixelsDouble(uint8_t *data, uint16_t *idx, uint32_t len)
{
  while (len--)
  {
    _data16.value = idx[*data++];
    WRITE(_data16.msb);
    WRITE(_data16.lsb);
    WRITE(_data16.msb);
    WRITE(_data16.lsb);
  }
}

INLINE void Arduino_RPiPicoPAR8::WRITE(uint8_t d)
{
  sio_hw->gpio_clr = _dataClrMask;
  sio_hw->gpio_set = d;
  sio_hw->gpio_set = _wrPinMask;
}

/******** low level bit twiddling **********/

INLINE void Arduino_RPiPicoPAR8::DC_HIGH(void)
{
  sio_hw->gpio_set = _dcPinMask;
}

INLINE void Arduino_RPiPicoPAR8::DC_LOW(void)
{
  sio_hw->gpio_clr = _dcPinMask;
}

INLINE void Arduino_RPiPicoPAR8::CS_HIGH(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
    sio_hw->gpio_set = _csPinMask;
  }
}

INLINE void Arduino_RPiPicoPAR8::CS_LOW(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
    sio_hw->gpio_clr = _csPinMask;
  }
}

#endif // #if defined(TARGET_RP2040)
