/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 */
#include "Arduino_SWSPI.h"

Arduino_SWSPI::Arduino_SWSPI(int8_t dc, int8_t cs, int8_t sck, int8_t mosi, int8_t miso /* = GFX_NOT_DEFINED */)
    : _dc(dc), _cs(cs), _sck(sck), _mosi(mosi), _miso(miso)
{
}

bool Arduino_SWSPI::begin(int32_t speed, int8_t dataMode)
{
  UNUSED(speed);
  UNUSED(dataMode);

  if (_dc != GFX_NOT_DEFINED)
  {
    pinMode(_dc, OUTPUT);
    digitalWrite(_dc, HIGH); // Data mode
  }
  if (_cs != GFX_NOT_DEFINED)
  {
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH); // Deselect
  }
  pinMode(_sck, OUTPUT);
  digitalWrite(_sck, LOW);
  pinMode(_mosi, OUTPUT);
  digitalWrite(_mosi, LOW);
  if (_miso != GFX_NOT_DEFINED)
  {
    pinMode(_miso, INPUT);
  }

#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(ARDUINO_ARCH_NRF52840)
  uint32_t pin;
  NRF_GPIO_Type *reg;
  if (_dc != GFX_NOT_DEFINED)
  {
    pin = digitalPinToPinName((pin_size_t)_dc);
    reg = nrf_gpio_pin_port_decode(&pin);
    _dcPinMask = 1UL << pin;
    _dcPortSet = &reg->OUTSET;
    _dcPortClr = &reg->OUTCLR;
  }
  if (_cs != GFX_NOT_DEFINED)
  {
    pin = digitalPinToPinName((pin_size_t)_cs);
    NRF_GPIO_Type *reg = nrf_gpio_pin_port_decode(&pin);
    _csPinMask = 1UL << pin;
    _csPortSet = &reg->OUTSET;
    _csPortClr = &reg->OUTCLR;
  }
  pin = digitalPinToPinName((pin_size_t)_sck);
  reg = nrf_gpio_pin_port_decode(&pin);
  _sckPinMask = 1UL << pin;
  _sckPortSet = &reg->OUTSET;
  _sckPortClr = &reg->OUTCLR;
  pin = digitalPinToPinName((pin_size_t)_mosi);
  reg = nrf_gpio_pin_port_decode(&pin);
  _mosiPinMask = 1UL << pin;
  _mosiPortSet = &reg->OUTSET;
  _mosiPortClr = &reg->OUTCLR;
  if (_miso != GFX_NOT_DEFINED)
  {
    pin = digitalPinToPinName((pin_size_t)_miso);
    reg = nrf_gpio_pin_port_decode(&pin);
    _misoPinMask = 1UL << pin;
    _misoPort = &reg->IN;
  }
#elif defined(TARGET_RP2040)
  if (_dc != GFX_NOT_DEFINED)
  {
    _dcPinMask = digitalPinToBitMask(_dc);
    _dcPortSet = (PORTreg_t)&sio_hw->gpio_set;
    _dcPortClr = (PORTreg_t)&sio_hw->gpio_clr;
  }
  if (_cs != GFX_NOT_DEFINED)
  {
    _csPinMask = digitalPinToBitMask(_cs);
    _csPortSet = (PORTreg_t)&sio_hw->gpio_set;
    _csPortClr = (PORTreg_t)&sio_hw->gpio_clr;
  }
  _sckPinMask = digitalPinToBitMask(_sck);
  _sckPortSet = (PORTreg_t)&sio_hw->gpio_set;
  _sckPortClr = (PORTreg_t)&sio_hw->gpio_clr;
  _mosiPinMask = digitalPinToBitMask(_mosi);
  _mosiPortSet = (PORTreg_t)&sio_hw->gpio_set;
  _mosiPortClr = (PORTreg_t)&sio_hw->gpio_clr;
  if (_miso != GFX_NOT_DEFINED)
  {
    _misoPinMask = digitalPinToBitMask(_miso);
    _misoPort = (PORTreg_t)portInputRegister(digitalPinToPort(_miso));
  }
#elif defined(ESP32) && (CONFIG_IDF_TARGET_ESP32C3)
  _dcPinMask = digitalPinToBitMask(_dc);
  _dcPortSet = (PORTreg_t)&GPIO.out_w1ts;
  _dcPortClr = (PORTreg_t)&GPIO.out_w1tc;
  if (_cs != GFX_NOT_DEFINED)
  {
    _csPinMask = digitalPinToBitMask(_cs);
    _csPortSet = (PORTreg_t)&GPIO.out_w1ts;
    _csPortClr = (PORTreg_t)&GPIO.out_w1tc;
  }
  _sckPinMask = digitalPinToBitMask(_sck);
  _sckPortSet = (PORTreg_t)&GPIO.out_w1ts;
  _sckPortClr = (PORTreg_t)&GPIO.out_w1tc;
  _mosiPinMask = digitalPinToBitMask(_mosi);
  _mosiPortSet = (PORTreg_t)&GPIO.out_w1ts;
  _mosiPortClr = (PORTreg_t)&GPIO.out_w1tc;
  if (_miso != GFX_NOT_DEFINED)
  {
    _misoPinMask = digitalPinToBitMask(_miso);
    _misoPort = (PORTreg_t)GPIO_IN_REG;
  }
#elif defined(ESP32)
  _dcPinMask = digitalPinToBitMask(_dc);
  if (_dc >= 32)
  {
    _dcPortSet = (PORTreg_t)&GPIO.out1_w1ts.val;
    _dcPortClr = (PORTreg_t)&GPIO.out1_w1tc.val;
  }
  else if (_dc != GFX_NOT_DEFINED)
  {
    _dcPortSet = (PORTreg_t)&GPIO.out_w1ts;
    _dcPortClr = (PORTreg_t)&GPIO.out_w1tc;
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
  _sckPinMask = digitalPinToBitMask(_sck);
  _mosiPinMask = digitalPinToBitMask(_mosi);
  if (_sck >= 32)
  {
    _sckPortSet = (PORTreg_t)&GPIO.out1_w1ts.val;
    _sckPortClr = (PORTreg_t)&GPIO.out1_w1tc.val;
  }
  else
  {
    _sckPortSet = (PORTreg_t)&GPIO.out_w1ts;
    _sckPortClr = (PORTreg_t)&GPIO.out_w1tc;
  }
  if (_mosi >= 32)
  {
    _mosiPortSet = (PORTreg_t)&GPIO.out1_w1ts.val;
    _mosiPortClr = (PORTreg_t)&GPIO.out1_w1tc.val;
  }
  else
  {
    _mosiPortSet = (PORTreg_t)&GPIO.out_w1ts;
    _mosiPortClr = (PORTreg_t)&GPIO.out_w1tc;
  }
  if (_miso != GFX_NOT_DEFINED)
  {
    _misoPinMask = digitalPinToBitMask(_miso);
    _misoPort = (PORTreg_t)portInputRegister(digitalPinToPort(_miso));
  }
#elif defined(CORE_TEENSY)
  if (_dc != GFX_NOT_DEFINED)
  {
#if !defined(KINETISK)
    _dcPinMask = digitalPinToBitMask(_dc);
#endif
    _dcPortSet = portSetRegister(_dc);
    _dcPortClr = portClearRegister(_dc);
  }
  if (_cs != GFX_NOT_DEFINED)
  {
#if !defined(KINETISK)
    _csPinMask = digitalPinToBitMask(_cs);
#endif
    _csPortSet = portSetRegister(_cs);
    _csPortClr = portClearRegister(_cs);
  }
#if !defined(KINETISK)
  _sckPinMask = digitalPinToBitMask(_sck);
#endif
  _sckPortSet = portSetRegister(_sck);
  _sckPortClr = portClearRegister(_sck);
#if !defined(KINETISK)
  _mosiPinMask = digitalPinToBitMask(_mosi);
#endif
  _mosiPortSet = portSetRegister(_mosi);
  _mosiPortClr = portClearRegister(_mosi);
  if (_miso != GFX_NOT_DEFINED)
  {
#if !defined(KINETISK)
    _misoPinMask = digitalPinToBitMask(_miso);
#endif
    _misoPort = (PORTreg_t)portInputRegister(digitalPinToPort(_miso));
  }
#else  // !CORE_TEENSY
  if (_dc != GFX_NOT_DEFINED)
  {
    _dcPinMask = digitalPinToBitMask(_dc);
    _dcPortSet = &(PORT->Group[g_APinDescription[_dc].ulPort].OUTSET.reg);
    _dcPortClr = &(PORT->Group[g_APinDescription[_dc].ulPort].OUTCLR.reg);
  }
  if (_cs != GFX_NOT_DEFINED)
  {
    _csPinMask = digitalPinToBitMask(_cs);
    _csPortSet = &(PORT->Group[g_APinDescription[_cs].ulPort].OUTSET.reg);
    _csPortClr = &(PORT->Group[g_APinDescription[_cs].ulPort].OUTCLR.reg);
  }
  _sckPinMask = digitalPinToBitMask(_sck);
  _sckPortSet = &(PORT->Group[g_APinDescription[_sck].ulPort].OUTSET.reg);
  _sckPortClr = &(PORT->Group[g_APinDescription[_sck].ulPort].OUTCLR.reg);
  _mosiPinMask = digitalPinToBitMask(_mosi);
  _mosiPortSet = &(PORT->Group[g_APinDescription[_mosi].ulPort].OUTSET.reg);
  _mosiPortClr = &(PORT->Group[g_APinDescription[_mosi].ulPort].OUTCLR.reg);
  if (_miso != GFX_NOT_DEFINED)
  {
    _misoPinMask = digitalPinToBitMask(_miso);
    _misoPort = (PORTreg_t)portInputRegister(digitalPinToPort(_miso));
  }
#endif // end !CORE_TEENSY
#else  // !HAS_PORT_SET_CLR
  if (_dc != GFX_NOT_DEFINED)
  {
    _dcPort = (PORTreg_t)portOutputRegister(digitalPinToPort(_dc));
    _dcPinMaskSet = digitalPinToBitMask(_dc);
    _dcPinMaskClr = ~_dcPinMaskSet;
  }
  if (_cs != GFX_NOT_DEFINED)
  {
    _csPort = (PORTreg_t)portOutputRegister(digitalPinToPort(_cs));
    _csPinMaskSet = digitalPinToBitMask(_cs);
    _csPinMaskClr = ~_csPinMaskSet;
  }
  _sckPort = (PORTreg_t)portOutputRegister(digitalPinToPort(_sck));
  _sckPinMaskSet = digitalPinToBitMask(_sck);
  _sckPinMaskClr = ~_sckPinMaskSet;
  _mosiPort = (PORTreg_t)portOutputRegister(digitalPinToPort(_mosi));
  _mosiPinMaskSet = digitalPinToBitMask(_mosi);
  _mosiPinMaskClr = ~_mosiPinMaskSet;
  if (_miso != GFX_NOT_DEFINED)
  {
    _misoPort = (PORTreg_t)portInputRegister(digitalPinToPort(_miso));
    _misoPinMask = digitalPinToBitMask(_miso);
  }
#endif // !HAS_PORT_SET_CLR
#endif // USE_FAST_PINIO

  return true;
}

void Arduino_SWSPI::beginWrite()
{
  if (_dc != GFX_NOT_DEFINED)
  {
    DC_HIGH();
  }
  CS_LOW();
}

void Arduino_SWSPI::endWrite()
{
  CS_HIGH();
}

void Arduino_SWSPI::writeCommand(uint8_t c)
{
  if (_dc == GFX_NOT_DEFINED) // 9-bit SPI
  {
    WRITE9BITCOMMAND(c);
  }
  else
  {
    DC_LOW();
    WRITE(c);
    DC_HIGH();
  }
}

void Arduino_SWSPI::writeCommand16(uint16_t c)
{
  if (_dc == GFX_NOT_DEFINED) // 9-bit SPI
  {
    _data16.value = c;
    WRITE9BITCOMMAND(_data16.msb);
    WRITE9BITCOMMAND(_data16.lsb);
  }
  else
  {
    DC_LOW();
    WRITE16(c);
    DC_HIGH();
  }
}

void Arduino_SWSPI::write(uint8_t d)
{
  if (_dc == GFX_NOT_DEFINED) // 9-bit SPI
  {
    WRITE9BITDATA(d);
  }
  else
  {
    WRITE(d);
  }
}

void Arduino_SWSPI::write16(uint16_t d)
{
  if (_dc == GFX_NOT_DEFINED) // 9-bit SPI
  {
    _data16.value = d;
    WRITE9BITDATA(_data16.msb);
    WRITE9BITDATA(_data16.lsb);
  }
  else
  {
    WRITE16(d);
  }
}

void Arduino_SWSPI::writeRepeat(uint16_t p, uint32_t len)
{
  if (_dc == GFX_NOT_DEFINED) // 9-bit SPI
  {
// ESP8266 avoid trigger watchdog
#if defined(ESP8266)
    while (len > (ESP8266SAFEBATCHBITSIZE / 9))
    {
      WRITE9BITREPEAT(p, ESP8266SAFEBATCHBITSIZE / 9);
      len -= ESP8266SAFEBATCHBITSIZE / 9;
      yield();
    }
    WRITE9BITREPEAT(p, len);
#else
    WRITE9BITREPEAT(p, len);
#endif
  }
  else
  {
#if defined(ESP8266)
    while (len > (ESP8266SAFEBATCHBITSIZE / 8))
    {
      WRITEREPEAT(p, ESP8266SAFEBATCHBITSIZE / 8);
      len -= ESP8266SAFEBATCHBITSIZE / 8;
      yield();
    }
    WRITEREPEAT(p, len);
#else
    WRITEREPEAT(p, len);
#endif
  }
}

void Arduino_SWSPI::writePixels(uint16_t *data, uint32_t len)
{
  while (len--)
  {
    WRITE16(*data++);
  }
}

#if !defined(LITTLE_FOOT_PRINT)
void Arduino_SWSPI::writeBytes(uint8_t *data, uint32_t len)
{
  while (len--)
  {
    WRITE(*data++);
  }
}
#endif // !defined(LITTLE_FOOT_PRINT)

INLINE void Arduino_SWSPI::WRITE9BITCOMMAND(uint8_t c)
{
  // D/C bit, command
  SPI_MOSI_LOW();
  SPI_SCK_HIGH();
  SPI_SCK_LOW();

  uint8_t bit = 0x80;
  while (bit)
  {
    if (c & bit)
    {
      SPI_MOSI_HIGH();
    }
    else
    {
      SPI_MOSI_LOW();
    }
    SPI_SCK_HIGH();
    bit >>= 1;
    SPI_SCK_LOW();
  }
}

INLINE void Arduino_SWSPI::WRITE9BITDATA(uint8_t d)
{
  // D/C bit, data
  SPI_MOSI_HIGH();
  SPI_SCK_HIGH();
  SPI_SCK_LOW();

  uint8_t bit = 0x80;
  while (bit)
  {
    if (d & bit)
    {
      SPI_MOSI_HIGH();
    }
    else
    {
      SPI_MOSI_LOW();
    }
    SPI_SCK_HIGH();
    bit >>= 1;
    SPI_SCK_LOW();
  }
}

INLINE void Arduino_SWSPI::WRITE(uint8_t d)
{
  uint8_t bit = 0x80;
  while (bit)
  {
    if (d & bit)
    {
      SPI_MOSI_HIGH();
    }
    else
    {
      SPI_MOSI_LOW();
    }
    SPI_SCK_HIGH();
    bit >>= 1;
    SPI_SCK_LOW();
  }
}

INLINE void Arduino_SWSPI::WRITE16(uint16_t d)
{
  uint16_t bit = 0x8000;
  while (bit)
  {
    if (d & bit)
    {
      SPI_MOSI_HIGH();
    }
    else
    {
      SPI_MOSI_LOW();
    }
    SPI_SCK_HIGH();
    bit >>= 1;
    SPI_SCK_LOW();
  }
}

INLINE void Arduino_SWSPI::WRITE9BITREPEAT(uint16_t p, uint32_t len)
{
  if (p == 0xffff) // no need to set MOSI level while filling white
  {
    SPI_MOSI_HIGH();
    len *= 18; // 9-bit * 2
    while (len--)
    {
      SPI_SCK_HIGH();
      SPI_SCK_LOW();
    }
  }
  else
  {
    _data16.value = p;
    while (len--)
    {
      WRITE9BITDATA(_data16.msb);
      WRITE9BITDATA(_data16.lsb);
    }
  }
}

INLINE void Arduino_SWSPI::WRITEREPEAT(uint16_t p, uint32_t len)
{
  if ((p == 0x0000) || (p == 0xffff)) // no need to set MOSI level while filling black or white
  {
    if (p)
    {
      SPI_MOSI_HIGH();
    }
    else
    {
      SPI_MOSI_LOW();
    }
    len *= 16;
    while (len--)
    {
      SPI_SCK_HIGH();
      SPI_SCK_LOW();
    }
  }
  else
  {
    while (len--)
    {
      WRITE16(p);
    }
  }
}

/******** low level bit twiddling **********/

INLINE void Arduino_SWSPI::DC_HIGH(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *_dcPortSet = 1;
#else  // !KINETISK
  *_dcPortSet = _dcPinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
  *_dcPort |= _dcPinMaskSet;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
  digitalWrite(_dc, HIGH);
#endif // end !USE_FAST_PINIO
}

INLINE void Arduino_SWSPI::DC_LOW(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *_dcPortClr = 1;
#else  // !KINETISK
  *_dcPortClr = _dcPinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
  *_dcPort &= _dcPinMaskClr;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
  digitalWrite(_dc, LOW);
#endif // end !USE_FAST_PINIO
}

INLINE void Arduino_SWSPI::CS_HIGH(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
    *_csPortSet = 1;
#else  // !KINETISK
    *_csPortSet = _csPinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
    *_csPort |= _csPinMaskSet;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
    digitalWrite(_cs, HIGH);
#endif // end !USE_FAST_PINIO
  }
}

INLINE void Arduino_SWSPI::CS_LOW(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
    *_csPortClr = 1;
#else  // !KINETISK
    *_csPortClr = _csPinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
    *_csPort &= _csPinMaskClr;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
    digitalWrite(_cs, LOW);
#endif // end !USE_FAST_PINIO
  }
}

/*!
    @brief  Set the software (bitbang) SPI MOSI line HIGH.
*/
INLINE void Arduino_SWSPI::SPI_MOSI_HIGH(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *_mosiPortSet = 1;
#else // !KINETISK
  *_mosiPortSet = _mosiPinMask;
#endif
#else  // !HAS_PORT_SET_CLR
  *_mosiPort |= _mosiPinMaskSet;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
  digitalWrite(_mosi, HIGH);
#endif // end !USE_FAST_PINIO
}

/*!
    @brief  Set the software (bitbang) SPI MOSI line LOW.
*/
INLINE void Arduino_SWSPI::SPI_MOSI_LOW(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *_mosiPortClr = 1;
#else // !KINETISK
  *_mosiPortClr = _mosiPinMask;
#endif
#else  // !HAS_PORT_SET_CLR
  *_mosiPort &= _mosiPinMaskClr;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
  digitalWrite(_mosi, LOW);
#endif // end !USE_FAST_PINIO
}

/*!
    @brief  Set the software (bitbang) SPI SCK line HIGH.
*/
INLINE void Arduino_SWSPI::SPI_SCK_HIGH(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *_sckPortSet = 1;
#else                                                // !KINETISK
  *_sckPortSet = _sckPinMask;
#if defined(__IMXRT1052__) || defined(__IMXRT1062__) // Teensy 4.x
  for (volatile uint8_t i = 0; i < 1; i++)
    ;
#endif
#endif
#else  // !HAS_PORT_SET_CLR
  *_sckPort |= _sckPinMaskSet;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
  digitalWrite(_sck, HIGH);
#endif // end !USE_FAST_PINIO
}

/*!
    @brief  Set the software (bitbang) SPI SCK line LOW.
*/
INLINE void Arduino_SWSPI::SPI_SCK_LOW(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *_sckPortClr = 1;
#else                                                // !KINETISK
  *_sckPortClr = _sckPinMask;
#if defined(__IMXRT1052__) || defined(__IMXRT1062__) // Teensy 4.x
  for (volatile uint8_t i = 0; i < 1; i++)
    ;
#endif
#endif
#else  // !HAS_PORT_SET_CLR
  *_sckPort &= _sckPinMaskClr;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
  digitalWrite(_sck, LOW);
#endif // end !USE_FAST_PINIO
}

/*!
    @brief   Read the state of the software (bitbang) SPI MISO line.
    @return  true if HIGH, false if LOW.
*/
INLINE bool Arduino_SWSPI::SPI_MISO_READ(void)
{
#if defined(USE_FAST_PINIO)
#if defined(KINETISK)
  return *_misoPort;
#else  // !KINETISK
  return *_misoPort & _misoPinMask;
#endif // end !KINETISK
#else  // !USE_FAST_PINIO
  return digitalRead(_miso);
#endif // end !USE_FAST_PINIO
}