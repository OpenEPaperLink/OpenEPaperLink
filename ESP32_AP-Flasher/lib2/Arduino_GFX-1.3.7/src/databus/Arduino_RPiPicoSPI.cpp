#if defined(TARGET_RP2040)

#include "Arduino_DataBus.h"
#include "Arduino_RPiPicoSPI.h"

Arduino_RPiPicoSPI::Arduino_RPiPicoSPI(int8_t dc /* = GFX_NOT_DEFINED */, int8_t cs /* = GFX_NOT_DEFINED */, int8_t sck /* = PIN_SPI0_SCK */, int8_t mosi /* = PIN_SPI0_MOSI */, int8_t miso /* = PIN_SPI0_MISO */, spi_inst_t *spi /* = spi0 */)
    : _dc(dc), _cs(cs), _sck(sck), _mosi(mosi), _miso(miso), _spi(spi)
{
}

bool Arduino_RPiPicoSPI::begin(int32_t speed /* = 0 */, int8_t dataMode /* = GFX_NOT_DEFINED */)
{
  // set SPI parameters
  _speed = (speed == GFX_NOT_DEFINED) ? SPI_DEFAULT_FREQ : speed;
  _dataMode = (dataMode != GFX_NOT_DEFINED) ? dataMode : SPI_MODE0;
  _spis = SPISettings(_speed, _bitOrder, _dataMode);

  // set pin mode
  pinMode(_dc, OUTPUT);
  digitalWrite(_dc, HIGH); // Data mode
  if (_cs != GFX_NOT_DEFINED)
  {
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH); // disable chip select
  }

  gpio_set_function(_miso, GPIO_FUNC_SPI);
  gpio_set_function(_sck, GPIO_FUNC_SPI);
  gpio_set_function(_mosi, GPIO_FUNC_SPI);

  // set fastIO variables
  _dcPinMask = digitalPinToBitMask(_dc);
  _dcPortSet = (PORTreg_t)&sio_hw->gpio_set;
  _dcPortClr = (PORTreg_t)&sio_hw->gpio_clr;

  if (_cs != GFX_NOT_DEFINED)
  {
    _csPinMask = digitalPinToBitMask(_cs);
    _csPortSet = (PORTreg_t)&sio_hw->gpio_set;
    _csPortClr = (PORTreg_t)&sio_hw->gpio_clr;
  }

  spi_init(_spi, _spis.getClockFreq());
  spi_set_format(_spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

  return true;
}

void Arduino_RPiPicoSPI::beginWrite()
{
  DC_HIGH();
  CS_LOW();
}

void Arduino_RPiPicoSPI::endWrite()
{
  CS_HIGH();
}

void Arduino_RPiPicoSPI::writeCommand(uint8_t c)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();
}

void Arduino_RPiPicoSPI::writeCommand16(uint16_t c)
{
  DC_LOW();

  WRITE16(c);

  DC_HIGH();
}

void Arduino_RPiPicoSPI::write(uint8_t d)
{
  WRITE(d);
}

void Arduino_RPiPicoSPI::write16(uint16_t d)
{
  WRITE16(d);
}

void Arduino_RPiPicoSPI::writeRepeat(uint16_t p, uint32_t len)
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

void Arduino_RPiPicoSPI::writePixels(uint16_t *data, uint32_t len)
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

void Arduino_RPiPicoSPI::writeC8D8(uint8_t c, uint8_t d)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  WRITE(d);
}

void Arduino_RPiPicoSPI::writeC8D16(uint8_t c, uint16_t d)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  WRITE16(d);
}

void Arduino_RPiPicoSPI::writeC8D16D16(uint8_t c, uint16_t d1, uint16_t d2)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  WRITE16(d1);
  WRITE16(d2);
}

void Arduino_RPiPicoSPI::writeBytes(uint8_t *data, uint32_t len)
{
  WRITEBUF(data, len);
}

INLINE void Arduino_RPiPicoSPI::WRITE(uint8_t d)
{
  spi_write_blocking(_spi, (const uint8_t *)&d, 1);
}

INLINE void Arduino_RPiPicoSPI::WRITE16(uint16_t d)
{
  MSB_16_SET(d, d);
  spi_write_blocking(_spi, (const uint8_t *)&d, 2);
}

INLINE void Arduino_RPiPicoSPI::WRITEBUF(uint8_t *buf, size_t count)
{
  spi_write_blocking(_spi, (const uint8_t *)buf, count);
}

/******** low level bit twiddling **********/

INLINE void Arduino_RPiPicoSPI::DC_HIGH(void)
{
  *_dcPortSet = _dcPinMask;
}

INLINE void Arduino_RPiPicoSPI::DC_LOW(void)
{
  *_dcPortClr = _dcPinMask;
}

INLINE void Arduino_RPiPicoSPI::CS_HIGH(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
    *_csPortSet = _csPinMask;
  }
}

INLINE void Arduino_RPiPicoSPI::CS_LOW(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
    *_csPortClr = _csPinMask;
  }
}

#endif // #if defined(TARGET_RP2040)
