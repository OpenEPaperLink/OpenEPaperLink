/*
 * start rewrite from:
 * https://github.com/arduino/ArduinoCore-mbed/blob/master/libraries/SPI/SPI.cpp
 */
#ifdef ARDUINO_ARCH_NRF52840

#include "Arduino_mbedSPI.h"

Arduino_mbedSPI::Arduino_mbedSPI(int8_t dc, int8_t cs /* = GFX_NOT_DEFINED */)
    : _dc(dc), _cs(cs)
{
}

bool Arduino_mbedSPI::begin(int32_t speed, int8_t dataMode)
{
  _speed = (speed == GFX_NOT_DEFINED) ? SPI_DEFAULT_FREQ : speed;
  _dataMode = (dataMode == GFX_NOT_DEFINED) ? SPI_MODE2 : dataMode;

  uint32_t pin = digitalPinToPinName((pin_size_t)_dc);
  NRF_GPIO_Type *reg = nrf_gpio_pin_port_decode(&pin);
  nrf_gpio_cfg_output(pin);
  _dcPortSet = &reg->OUTSET;
  _dcPortClr = &reg->OUTCLR;
  _dcPinMask = 1UL << pin;
  if (_cs != GFX_NOT_DEFINED)
  {
    pin = digitalPinToPinName((pin_size_t)_cs);
    reg = nrf_gpio_pin_port_decode(&pin);
    nrf_gpio_cfg_output(pin);
    _csPortSet = &reg->OUTSET;
    _csPortClr = &reg->OUTCLR;
    _csPinMask = 1UL << pin;
  }

  if (_dataMode == GFX_NOT_DEFINED)
  {
    _dataMode = SPI_MODE0;
  }
  _dev = new mbed::SPI((PinName)SPI_MOSI, (PinName)SPI_MISO, (PinName)SPI_SCK);

  return true;
}

void Arduino_mbedSPI::beginWrite()
{
  _dev->lock();
  _dev->format(8, _dataMode);
  _dev->frequency(_speed);

  DC_HIGH();

  CS_LOW();
}

void Arduino_mbedSPI::endWrite()
{
  CS_HIGH();
  _dev->unlock();
}

void Arduino_mbedSPI::writeCommand(uint8_t c)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();
}

void Arduino_mbedSPI::writeCommand16(uint16_t c)
{
  DC_LOW();

  WRITE16(c);

  DC_HIGH();
}

void Arduino_mbedSPI::write(uint8_t d)
{
  WRITE(d);
}

void Arduino_mbedSPI::write16(uint16_t d)
{
  WRITE16(d);
}

void Arduino_mbedSPI::writeRepeat(uint16_t p, uint32_t len)
{
  MSB_16_SET(p, p);
  uint32_t bufLen = (len < SPI_MAX_PIXELS_AT_ONCE) ? len : SPI_MAX_PIXELS_AT_ONCE;
  uint32_t xferLen;
  for (uint32_t i = 0; i < bufLen; i++)
  {
    _buffer16[i] = p;
  }

  while (len)
  {
    xferLen = (bufLen < len) ? bufLen : len;
    WRITEBUF(_buffer, xferLen * 2);
    len -= xferLen;
  }
}

void Arduino_mbedSPI::writePixels(uint16_t *data, uint32_t len)
{
  uint32_t xferLen;
  uint8_t *p;
  union
  {
    uint16_t val;
    struct
    {
      uint8_t lsb;
      uint8_t msb;
    };
  } t;
  while (len)
  {
    xferLen = (len < SPI_MAX_PIXELS_AT_ONCE) ? len : SPI_MAX_PIXELS_AT_ONCE;
    p = _buffer;
    for (uint32_t i = 0; i < xferLen; i++)
    {
      t.val = *data++;
      *p++ = t.msb;
      *p++ = t.lsb;
    }
    len -= xferLen;

    xferLen += xferLen; // uint16_t to uint8_t, double length
    WRITEBUF(_buffer, xferLen);
  }
}

void Arduino_mbedSPI::writeC8D8(uint8_t c, uint8_t d)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  WRITE(d);
}

void Arduino_mbedSPI::writeC8D16(uint8_t c, uint16_t d)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  WRITE16(d);
}

void Arduino_mbedSPI::writeC8D16D16(uint8_t c, uint16_t d1, uint16_t d2)
{
  DC_LOW();

  _dev->write((const char *)&c, 1, NULL, 0);

  DC_HIGH();

  uint32_t d;
  MSB_32_16_16_SET(d, d1, d2);
  _dev->write((const char *)&d, 4, NULL, 0);
}

void Arduino_mbedSPI::writeBytes(uint8_t *data, uint32_t len)
{
  WRITEBUF(data, len);
}

INLINE void Arduino_mbedSPI::WRITE(uint8_t d)
{
  _dev->write((const char *)&d, 1, NULL, 0);
}

INLINE void Arduino_mbedSPI::WRITE16(uint16_t d)
{
  MSB_16_SET(d, d);
  _dev->write((const char *)&d, 2, NULL, 0);
}

INLINE void Arduino_mbedSPI::WRITEBUF(uint8_t *buf, size_t count)
{
  _dev->write((const char *)buf, count, NULL, 0);
}

/******** low level bit twiddling **********/

INLINE void Arduino_mbedSPI::DC_HIGH(void)
{
  *_dcPortSet = _dcPinMask;
}

INLINE void Arduino_mbedSPI::DC_LOW(void)
{
  *_dcPortClr = _dcPinMask;
}

INLINE void Arduino_mbedSPI::CS_HIGH(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
    *_csPortSet = _csPinMask;
  }
}

INLINE void Arduino_mbedSPI::CS_LOW(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
    *_csPortClr = _csPinMask;
  }
}

#endif // #ifdef ARDUINO_ARCH_NRF52840
