/*
 * start rewrite from:
 * https://github.com/Bodmer/TFT_eSPI.git
 */
#ifdef ESP8266

#include <SPI.h>
#include "Arduino_ESP8266SPI.h"

#define WAIT_SPI_NOT_BUSY while (SPI1CMD & SPIBUSY)

Arduino_ESP8266SPI::Arduino_ESP8266SPI(int8_t dc, int8_t cs /* = GFX_NOT_DEFINED */)
    : _dc(dc), _cs(cs)
{
}

bool Arduino_ESP8266SPI::begin(int32_t speed, int8_t dataMode)
{
  _speed = (speed == GFX_NOT_DEFINED) ? SPI_DEFAULT_FREQ : speed;
  _dataMode = (dataMode == GFX_NOT_DEFINED) ? SPI_MODE0 : dataMode;

  pinMode(_dc, OUTPUT);
  digitalWrite(_dc, HIGH); // Data mode
  if (_cs != GFX_NOT_DEFINED)
  {
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH); // Deselect
  }

  _dcPort = (PORTreg_t)portOutputRegister(digitalPinToPort(_dc));
  _dcPinMaskSet = digitalPinToBitMask(_dc);
  if (_cs != GFX_NOT_DEFINED)
  {
    _csPort = (PORTreg_t)portOutputRegister(digitalPinToPort(_cs));
    _csPinMaskSet = digitalPinToBitMask(_cs);
  }
  _csPinMaskClr = ~_csPinMaskSet;
  _dcPinMaskClr = ~_dcPinMaskSet;

  SPI.begin();
  if (_dataMode == GFX_NOT_DEFINED)
  {
    _dataMode = SPI_MODE0;
  }
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(_dataMode);
  SPI.setFrequency(_speed);

  return true;
}

void Arduino_ESP8266SPI::beginWrite()
{
  DC_HIGH();

  CS_LOW();
}

void Arduino_ESP8266SPI::endWrite()
{
  CS_HIGH();
}

void Arduino_ESP8266SPI::writeCommand(uint8_t c)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();
}

void Arduino_ESP8266SPI::writeCommand16(uint16_t c)
{
  DC_LOW();

  WRITE16(c);

  DC_HIGH();
}

void Arduino_ESP8266SPI::write(uint8_t d)
{
  WRITE(d);
}

void Arduino_ESP8266SPI::write16(uint16_t d)
{
  WRITE16(d);
}

void Arduino_ESP8266SPI::writeRepeat(uint16_t p, uint32_t len)
{
  _data16.value = p;
  static uint8_t temp[2];
  temp[0] = _data16.msb;
  temp[1] = _data16.lsb;
  SPI.writePattern((uint8_t *)temp, 2, len);
}

void Arduino_ESP8266SPI::writePixels(uint16_t *data, uint32_t len)
{
  uint8_t *data8 = (uint8_t *)data;
  uint8_t i;

  if (len > 31)
  {
    SPI1U1 = (511 << SPILMOSI);

    while (len > 31)
    {
      i = 0;
      while (i < 16)
      {
        MSB_32_8_ARRAY_SET(twoPixelBuf[i++], data8);
        data8 += 4;
      }

      len -= 32;

      // ESP8266 wait time here at 40MHz SPI is ~5.45us
      WAIT_SPI_NOT_BUSY;
      SPI1W0 = twoPixelBuf[0];
      SPI1W1 = twoPixelBuf[1];
      SPI1W2 = twoPixelBuf[2];
      SPI1W3 = twoPixelBuf[3];
      SPI1W4 = twoPixelBuf[4];
      SPI1W5 = twoPixelBuf[5];
      SPI1W6 = twoPixelBuf[6];
      SPI1W7 = twoPixelBuf[7];
      SPI1W8 = twoPixelBuf[8];
      SPI1W9 = twoPixelBuf[9];
      SPI1W10 = twoPixelBuf[10];
      SPI1W11 = twoPixelBuf[11];
      SPI1W12 = twoPixelBuf[12];
      SPI1W13 = twoPixelBuf[13];
      SPI1W14 = twoPixelBuf[14];
      SPI1W15 = twoPixelBuf[15];
      SPI1CMD |= SPIBUSY;
    }
  }

  if (len)
  {
    uint32_t bits = ((len << 4) - 1); // bits left to shift - 1
    i = 0;
    len = (len + 1) >> 1;

    WAIT_SPI_NOT_BUSY;
    SPI1U1 = (bits << SPILMOSI);
    while (len--)
    {
      MSB_32_8_ARRAY_SET(spi1Reg32[i++], data8);
      data8 += 4;
    }
    SPI1CMD |= SPIBUSY;
  }

  WAIT_SPI_NOT_BUSY;
}

void Arduino_ESP8266SPI::writeC8D8(uint8_t c, uint8_t d)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  WRITE(d);
}

void Arduino_ESP8266SPI::writeC8D16(uint8_t c, uint16_t d)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  WRITE16(d);
}

void Arduino_ESP8266SPI::writeC8D16D16(uint8_t c, uint16_t d1, uint16_t d2)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  uint32_t d;
  MSB_32_16_16_SET(d, d1, d2);

  SPI1U1 = (31 << SPILMOSI);
  SPI1W0 = d;
  SPI1CMD |= SPIBUSY;
  WAIT_SPI_NOT_BUSY;
}

void Arduino_ESP8266SPI::writeBytes(uint8_t *data, uint32_t len)
{
  SPI.writeBytes(data, len);
}

void Arduino_ESP8266SPI::writePattern(uint8_t *data, uint8_t len, uint32_t repeat)
{
  SPI.writePattern(data, len, repeat);
}

void Arduino_ESP8266SPI::writeIndexedPixels(uint8_t *data, uint16_t *idx, uint32_t len)
{
  uint16_t p1, p2;
  uint8_t i;

  if (len > 31)
  {
    SPI1U1 = (511 << SPILMOSI);

    while (len > 31)
    {
      i = 0;
      while (i < 16)
      {
        p1 = idx[*data++];
        p2 = idx[*data++];
        MSB_32_16_16_SET(twoPixelBuf[i++], p1, p2);
      }

      len -= 32;

      // ESP8266 wait time here at 40MHz SPI is ~5.45us
      WAIT_SPI_NOT_BUSY;
      SPI1W0 = twoPixelBuf[0];
      SPI1W1 = twoPixelBuf[1];
      SPI1W2 = twoPixelBuf[2];
      SPI1W3 = twoPixelBuf[3];
      SPI1W4 = twoPixelBuf[4];
      SPI1W5 = twoPixelBuf[5];
      SPI1W6 = twoPixelBuf[6];
      SPI1W7 = twoPixelBuf[7];
      SPI1W8 = twoPixelBuf[8];
      SPI1W9 = twoPixelBuf[9];
      SPI1W10 = twoPixelBuf[10];
      SPI1W11 = twoPixelBuf[11];
      SPI1W12 = twoPixelBuf[12];
      SPI1W13 = twoPixelBuf[13];
      SPI1W14 = twoPixelBuf[14];
      SPI1W15 = twoPixelBuf[15];
      SPI1CMD |= SPIBUSY;
    }
  }

  if (len)
  {
    uint32_t bits = ((len << 4) - 1); // bits left to shift - 1
    i = 0;
    len = (len + 1) >> 1;

    WAIT_SPI_NOT_BUSY;
    SPI1U1 = (bits << SPILMOSI);
    while (len--)
    {
      p1 = idx[*data++];
      p2 = idx[*data++];
      MSB_32_16_16_SET(spi1Reg32[i++], p1, p2);
    }
    SPI1CMD |= SPIBUSY;
  }

  WAIT_SPI_NOT_BUSY;
}

void Arduino_ESP8266SPI::writeIndexedPixelsDouble(uint8_t *data, uint16_t *idx, uint32_t len)
{
  uint16_t p;
  uint8_t i;

  if (len > 15)
  {
    SPI1U1 = (511 << SPILMOSI);
    while (len > 15)
    {
      i = 0;
      while (i < 16)
      {
        p = idx[*data++];
        MSB_32_16_16_SET(twoPixelBuf[i++], p, p);
      }

      len -= 16;

      // ESP8266 wait time here at 40MHz SPI is ~5.45us
      WAIT_SPI_NOT_BUSY;
      SPI1W0 = twoPixelBuf[0];
      SPI1W1 = twoPixelBuf[1];
      SPI1W2 = twoPixelBuf[2];
      SPI1W3 = twoPixelBuf[3];
      SPI1W4 = twoPixelBuf[4];
      SPI1W5 = twoPixelBuf[5];
      SPI1W6 = twoPixelBuf[6];
      SPI1W7 = twoPixelBuf[7];
      SPI1W8 = twoPixelBuf[8];
      SPI1W9 = twoPixelBuf[9];
      SPI1W10 = twoPixelBuf[10];
      SPI1W11 = twoPixelBuf[11];
      SPI1W12 = twoPixelBuf[12];
      SPI1W13 = twoPixelBuf[13];
      SPI1W14 = twoPixelBuf[14];
      SPI1W15 = twoPixelBuf[15];
      SPI1CMD |= SPIBUSY;
    }
  }

  if (len)
  {
    uint32_t bits = ((len << 5) - 1); // bits left to shift - 1
    i = 0;

    WAIT_SPI_NOT_BUSY;
    SPI1U1 = (bits << SPILMOSI);
    while (len--)
    {
      p = idx[*data++];
      MSB_32_16_16_SET(spi1Reg32[i++], p, p);
    }
    SPI1CMD |= SPIBUSY;
  }

  WAIT_SPI_NOT_BUSY;
}

INLINE void Arduino_ESP8266SPI::WRITE(uint8_t d)
{
  SPI1U1 = (7 << SPILMOSI);
  SPI1W0 = d;
  SPI1CMD |= SPIBUSY;
  WAIT_SPI_NOT_BUSY;
}

INLINE void Arduino_ESP8266SPI::WRITE16(uint16_t d)
{
  MSB_16_SET(d, d);

  SPI1U1 = (15 << SPILMOSI);
  SPI1W0 = d;
  SPI1CMD |= SPIBUSY;
  WAIT_SPI_NOT_BUSY;
}

/******** low level bit twiddling **********/

INLINE void Arduino_ESP8266SPI::DC_HIGH(void)
{
  *_dcPort |= _dcPinMaskSet;
}

INLINE void Arduino_ESP8266SPI::DC_LOW(void)
{
  *_dcPort &= _dcPinMaskClr;
}

INLINE void Arduino_ESP8266SPI::CS_HIGH(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
    *_csPort |= _csPinMaskSet;
  }
}

INLINE void Arduino_ESP8266SPI::CS_LOW(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
    *_csPort &= _csPinMaskClr;
  }
}

#endif // #ifdef ESP8266
