#include "Arduino_ESP32S2PAR16Q.h"

#if defined(ESP32) && (CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3)

Arduino_ESP32S2PAR16Q::Arduino_ESP32S2PAR16Q(int8_t dc, int8_t cs, int8_t wr, int8_t rd)
    : _dc(dc), _cs(cs), _wr(wr), _rd(rd)
{
}

bool Arduino_ESP32S2PAR16Q::begin(int32_t speed, int8_t dataMode)
{
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

  // INIT 16-bit mask
  _dataClrMask = (1 << _wr) | 0xFFFF;
  _dataPortSet = (PORTreg_t)&GPIO.out_w1ts;
  _dataPortClr = (PORTreg_t)&GPIO.out_w1tc;
  *_dataPortClr = _dataClrMask;

  return true;
}

void Arduino_ESP32S2PAR16Q::beginWrite()
{
  DC_HIGH();
  CS_LOW();
}

void Arduino_ESP32S2PAR16Q::endWrite()
{
  CS_HIGH();
}

void Arduino_ESP32S2PAR16Q::writeCommand(uint8_t c)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();
}

void Arduino_ESP32S2PAR16Q::writeCommand16(uint16_t c)
{
  DC_LOW();

  WRITE16(c);

  DC_HIGH();
}

void Arduino_ESP32S2PAR16Q::write(uint8_t d)
{
  WRITE(d);
}

void Arduino_ESP32S2PAR16Q::write16(uint16_t d)
{
  WRITE16(d);
}

void Arduino_ESP32S2PAR16Q::writeRepeat(uint16_t p, uint32_t len)
{
  *_dataPortClr = _dataClrMask;
  *_dataPortSet = p;
  while (len--)
  {
    *_wrPortClr = _wrPinMask;
    *_wrPortSet = _wrPinMask;
  }
}

void Arduino_ESP32S2PAR16Q::writePixels(uint16_t *data, uint32_t len)
{
  while (len--)
  {
    WRITE16(*data++);
  }
}

void Arduino_ESP32S2PAR16Q::writeC8D8(uint8_t c, uint8_t d)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  WRITE(d);
}

void Arduino_ESP32S2PAR16Q::writeC8D16(uint8_t c, uint16_t d)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  WRITE16(d);
}

void Arduino_ESP32S2PAR16Q::writeC8D16D16(uint8_t c, uint16_t d1, uint16_t d2)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  WRITE16(d1);
  WRITE16(d2);
}

void Arduino_ESP32S2PAR16Q::writeC8D16D16Split(uint8_t c, uint16_t d1, uint16_t d2)
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

void Arduino_ESP32S2PAR16Q::writeBytes(uint8_t *data, uint32_t len)
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

void Arduino_ESP32S2PAR16Q::writeIndexedPixels(uint8_t *data, uint16_t *idx, uint32_t len)
{
  while (len--)
  {
    WRITE16(idx[*data++]);
  }
}

void Arduino_ESP32S2PAR16Q::writeIndexedPixelsDouble(uint8_t *data, uint16_t *idx, uint32_t len)
{
  while (len--)
  {
    *_dataPortClr = _dataClrMask;
    *_dataPortSet = idx[*data++];
    *_wrPortSet = _wrPinMask;
    *_wrPortClr = _wrPinMask;
    *_wrPortSet = _wrPinMask;
  }
}

INLINE void Arduino_ESP32S2PAR16Q::WRITE(uint8_t d)
{
  *_dataPortClr = _dataClrMask;
  *_dataPortSet = d;
  *_wrPortSet = _wrPinMask;
}

INLINE void Arduino_ESP32S2PAR16Q::WRITE16(uint16_t d)
{
  *_dataPortClr = _dataClrMask;
  *_dataPortSet = d;
  *_wrPortSet = _wrPinMask;
}

/******** low level bit twiddling **********/

INLINE void Arduino_ESP32S2PAR16Q::DC_HIGH(void)
{
  *_dcPortSet = _dcPinMask;
}

INLINE void Arduino_ESP32S2PAR16Q::DC_LOW(void)
{
  *_dcPortClr = _dcPinMask;
}

INLINE void Arduino_ESP32S2PAR16Q::CS_HIGH(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
    *_csPortSet = _csPinMask;
  }
}

INLINE void Arduino_ESP32S2PAR16Q::CS_LOW(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
    *_csPortClr = _csPinMask;
  }
}

#endif // #if defined(ESP32) && (CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3)
