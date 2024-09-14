#ifdef ARDUINO_ARCH_STM32

#include "Arduino_STM32PAR8.h"

Arduino_STM32PAR8::Arduino_STM32PAR8(int8_t dc, int8_t cs, int8_t wr, int8_t rd, GPIO_TypeDef *port)
    : _dc(dc), _cs(cs), _wr(wr), _rd(rd), _port(port)
{
}

bool Arduino_STM32PAR8::begin(int32_t speed, int8_t dataMode)
{
  UNUSED(speed);
  UNUSED(dataMode);
  set_GPIO_Port_Clock(STM_PORT(_port)); // Enable data port
  pinMode(_dc, OUTPUT);
  digitalWrite(_dc, HIGH); // Data mode
  _dcPORT = digitalPinToPort(_dc);
  _dcPinMaskSet = digitalPinToBitMask(_dc);

  if (_cs != GFX_NOT_DEFINED)
  {
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH); // Disable chip select
    _csPinMaskSet = digitalPinToBitMask(_cs);
    _csPORT = digitalPinToPort(_cs);
  }

  pinMode(_wr, OUTPUT);
  digitalWrite(_wr, HIGH); // Set write strobe high (inactive)
  _wrPort = (PORTreg_t)portOutputRegister(digitalPinToPort(_wr));
  _wrPORT = digitalPinToPort(_wr);
  _wrPinMaskSet = digitalPinToBitMask(_wr);
  _wrPinMaskClr = ~_wrPinMaskSet;

  if (_rd != GFX_NOT_DEFINED)
  {
    pinMode(_rd, OUTPUT);
    digitalWrite(_rd, HIGH); // Disable RD
    _rdPinMaskSet = digitalPinToBitMask(_rd);
  }
  else
  {
    _rdPinMaskSet = 0;
  }

  *(portModeRegister(_port)) = 0x33333333; // Set data port to output at max speed
  _port->BSRR = 0xFF << 16;                // Clear data port

  return true;
}

void Arduino_STM32PAR8::beginWrite()
{
  DC_HIGH();
  CS_LOW();
}

void Arduino_STM32PAR8::endWrite()
{
  CS_HIGH();
}

void Arduino_STM32PAR8::writeCommand(uint8_t c)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();
}

void Arduino_STM32PAR8::writeCommand16(uint16_t c)
{
  DC_LOW();

  _data16.value = c;
  WRITE(_data16.msb);
  WRITE(_data16.lsb);

  DC_HIGH();
}

void Arduino_STM32PAR8::write(uint8_t d)
{
  WRITE(d);
}

void Arduino_STM32PAR8::write16(uint16_t d)
{
  _data16.value = d;
  WRITE(_data16.msb);
  WRITE(_data16.lsb);
}

void Arduino_STM32PAR8::writeRepeat(uint16_t p, uint32_t len)
{
  uint8_t wrMaskBase = *_wrPort & _wrPinMaskClr;
  uint8_t wrMaskSet = wrMaskBase | _wrPinMaskSet;
  uint32_t wrMASKCLR = _wrPinMaskSet << 16;
  _data16.value = p;
  if (_data16.msb == _data16.lsb)
  {
    _port->BSRR = 0xFF << 16;
    _port->BSRR = (_data16.msb) & 0xFF;
    while (len--)
    {
      *_wrPort = wrMaskBase; // For some reason in this case it's faster then BSRR
      *_wrPort = wrMaskSet;
      *_wrPort = wrMaskBase;
      *_wrPort = wrMaskSet;
    }
  }
  else
  {
    while (len--)
    {
      _port->BSRR = 0xFF << 16;
      _port->BSRR = (_data16.msb);
      *_wrPort = wrMaskBase;
      *_wrPort = wrMaskSet;

      _port->BSRR = 0xFF << 16;
      _port->BSRR = (_data16.lsb);
      *_wrPort = wrMaskBase;
      *_wrPort = wrMaskSet;
    }
  }
}

void Arduino_STM32PAR8::writePixels(uint16_t *data, uint32_t len)
{
  while (len--)
  {
    _data16.value = *data++;
    WRITE(_data16.msb);
    WRITE(_data16.lsb);
  }
}

void Arduino_STM32PAR8::writeC8D8(uint8_t c, uint8_t d)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  WRITE(d);
}

void Arduino_STM32PAR8::writeC8D16(uint8_t c, uint16_t d)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  _data16.value = d;
  WRITE(_data16.msb);
  WRITE(_data16.lsb);
}

void Arduino_STM32PAR8::writeC8D16D16(uint8_t c, uint16_t d1, uint16_t d2)
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

void Arduino_STM32PAR8::writeBytes(uint8_t *data, uint32_t len)
{
  while (len--)
  {
    WRITE(*data++);
  }
}

void Arduino_STM32PAR8::writeIndexedPixels(uint8_t *data, uint16_t *idx, uint32_t len)
{
  while (len--)
  {
    _data16.value = idx[*data++];
    WRITE(_data16.msb);
    WRITE(_data16.lsb);
  }
}

void Arduino_STM32PAR8::writeIndexedPixelsDouble(uint8_t *data, uint16_t *idx, uint32_t len)
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

INLINE void Arduino_STM32PAR8::WRITE(uint8_t d)
{
  _port->BSRR = 0xFF << 16;
  _port->BSRR = (d)&0xFF;
  _wrPORT->BSRR = _wrPinMaskSet << 16; // Set WR LOW
  _wrPORT->BSRR = _wrPinMaskSet;       // Set WR HIGH
}

/******** low level bit twiddling **********/

INLINE void Arduino_STM32PAR8::DC_HIGH(void)
{
  _dcPORT->BSRR = _dcPinMaskSet;
}

INLINE void Arduino_STM32PAR8::DC_LOW(void)
{
  _dcPORT->BSRR = _dcPinMaskSet << 16;
}

INLINE void Arduino_STM32PAR8::CS_HIGH(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
    _csPORT->BSRR = _csPinMaskSet;
  }
}

INLINE void Arduino_STM32PAR8::CS_LOW(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
    _csPORT->BSRR = _csPinMaskSet << 16;
  }
}

#endif // #ifdef ARDUINO_ARCH_STM32
