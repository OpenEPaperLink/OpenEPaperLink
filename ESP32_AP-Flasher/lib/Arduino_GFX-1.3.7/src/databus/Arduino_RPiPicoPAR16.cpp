#if defined(TARGET_RP2040)

#include "Arduino_RPiPicoPAR16.h"

Arduino_RPiPicoPAR16::Arduino_RPiPicoPAR16(int8_t dc, int8_t cs, int8_t wr, int8_t rd)
    : _dc(dc), _cs(cs), _wr(wr), _rd(rd)
{
}

bool Arduino_RPiPicoPAR16::begin(int32_t speed, int8_t dataMode)
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
  _dataClrMask = 0xFFFF | _wrPinMask;

  if (_rd != GFX_NOT_DEFINED)
  {
    pinMode(_rd, OUTPUT);
    digitalWrite(_rd, HIGH);
    _rdPinMask = digitalPinToBitMask(_rd);
  }
  else
  {
    _rdPinMask = 0;
  }

  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(15, OUTPUT);
  sio_hw->gpio_clr = 0xFFFF;

  return true;
}

void Arduino_RPiPicoPAR16::beginWrite()
{
  DC_HIGH();
  CS_LOW();
}

void Arduino_RPiPicoPAR16::endWrite()
{
  CS_HIGH();
}

void Arduino_RPiPicoPAR16::writeCommand(uint8_t c)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();
}

void Arduino_RPiPicoPAR16::writeCommand16(uint16_t c)
{
  DC_LOW();

  WRITE16(c);

  DC_HIGH();
}

void Arduino_RPiPicoPAR16::write(uint8_t d)
{
  WRITE(d);
}

void Arduino_RPiPicoPAR16::write16(uint16_t d)
{
  WRITE16(d);
}

void Arduino_RPiPicoPAR16::writeRepeat(uint16_t p, uint32_t len)
{
  while (len--)
  {
    sio_hw->gpio_clr = _dataClrMask;
    sio_hw->gpio_set = p;
    sio_hw->gpio_set = _wrPinMask;
  }
}

void Arduino_RPiPicoPAR16::writePixels(uint16_t *data, uint32_t len)
{
  while (len--)
  {
    sio_hw->gpio_clr = _dataClrMask;
    sio_hw->gpio_set = *data++;
    sio_hw->gpio_set = _wrPinMask;
  }
}

void Arduino_RPiPicoPAR16::writeC8D8(uint8_t c, uint8_t d)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  WRITE(d);
}

void Arduino_RPiPicoPAR16::writeC8D16(uint8_t c, uint16_t d)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  WRITE16(d);
}

void Arduino_RPiPicoPAR16::writeC8D16D16(uint8_t c, uint16_t d1, uint16_t d2)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  WRITE16(d1);
  WRITE16(d2);
}

void Arduino_RPiPicoPAR16::writeC8D16D16Split(uint8_t c, uint16_t d1, uint16_t d2)
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

void Arduino_RPiPicoPAR16::writeBytes(uint8_t *data, uint32_t len)
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

void Arduino_RPiPicoPAR16::writeIndexedPixels(uint8_t *data, uint16_t *idx, uint32_t len)
{
  while (len--)
  {
    sio_hw->gpio_clr = _dataClrMask;
    sio_hw->gpio_set = idx[*data++];
    sio_hw->gpio_set = _wrPinMask;
  }
}

void Arduino_RPiPicoPAR16::writeIndexedPixelsDouble(uint8_t *data, uint16_t *idx, uint32_t len)
{
  while (len--)
  {
    sio_hw->gpio_clr = _dataClrMask;
    sio_hw->gpio_set = idx[*data++];
    sio_hw->gpio_set = _wrPinMask;
    sio_hw->gpio_clr = _wrPinMask;
    sio_hw->gpio_set = _wrPinMask;
  }
}

INLINE void Arduino_RPiPicoPAR16::WRITE(uint8_t d)
{
  sio_hw->gpio_clr = _dataClrMask;
  sio_hw->gpio_set = d;
  sio_hw->gpio_set = _wrPinMask;
}

INLINE void Arduino_RPiPicoPAR16::WRITE16(uint16_t d)
{
  sio_hw->gpio_clr = _dataClrMask;
  sio_hw->gpio_set = d;
  sio_hw->gpio_set = _wrPinMask;
}

/******** low level bit twiddling **********/

INLINE void Arduino_RPiPicoPAR16::DC_HIGH(void)
{
  sio_hw->gpio_set = _dcPinMask;
}

INLINE void Arduino_RPiPicoPAR16::DC_LOW(void)
{
  sio_hw->gpio_clr = _dcPinMask;
}

INLINE void Arduino_RPiPicoPAR16::CS_HIGH(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
    sio_hw->gpio_set = _csPinMask;
  }
}

INLINE void Arduino_RPiPicoPAR16::CS_LOW(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
    sio_hw->gpio_clr = _csPinMask;
  }
}

#endif // #if defined(TARGET_RP2040)
