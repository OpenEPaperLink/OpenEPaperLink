/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 */
#include "Arduino_HWSPI.h"

#if defined(SPI_HAS_TRANSACTION)
#define SPI_BEGIN_TRANSACTION() _spi->beginTransaction(mySPISettings)
#define SPI_END_TRANSACTION() _spi->endTransaction()
#else
#define SPI_BEGIN_TRANSACTION() \
  {                             \
  }
#define SPI_END_TRANSACTION() \
  {                           \
  }
#endif

#if defined(SPI_HAS_TRANSACTION)
static SPISettings mySPISettings;
#elif defined(__AVR__) || defined(CORE_TEENSY)
static uint8_t SPCRbackup;
static uint8_t mySPCR;
#endif

#if defined(ESP32)
Arduino_HWSPI::Arduino_HWSPI(int8_t dc, int8_t cs /* = GFX_NOT_DEFINED */, int8_t sck /* = GFX_NOT_DEFINED */, int8_t mosi /* = GFX_NOT_DEFINED */, int8_t miso /* = GFX_NOT_DEFINED */, SPIClass *spi, bool is_shared_interface /* = true */)
    : _dc(dc), _cs(cs), _sck(sck), _mosi(mosi), _miso(miso), _spi(spi), _is_shared_interface(is_shared_interface)
{
#else
Arduino_HWSPI::Arduino_HWSPI(int8_t dc, int8_t cs /* = GFX_NOT_DEFINED */, SPIClass *spi, bool is_shared_interface /* = true */)
    : _dc(dc), _cs(cs), _spi(spi), _is_shared_interface(is_shared_interface)
{
#endif
}

bool Arduino_HWSPI::begin(int32_t speed, int8_t dataMode)
{
  _speed = (speed == GFX_NOT_DEFINED) ? SPI_DEFAULT_FREQ : speed;
  _dataMode = dataMode;

  pinMode(_dc, OUTPUT);
  digitalWrite(_dc, HIGH); // Data mode
  if (_cs != GFX_NOT_DEFINED)
  {
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH); // Deselect
  }

#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(ARDUINO_ARCH_NRF52840)
  uint32_t pin = digitalPinToPinName((pin_size_t)_dc);
  NRF_GPIO_Type *reg = nrf_gpio_pin_port_decode(&pin);
  _dcPortSet = &reg->OUTSET;
  _dcPortClr = &reg->OUTCLR;
  _dcPinMask = 1UL << pin;
  if (_cs != GFX_NOT_DEFINED)
  {
    pin = digitalPinToPinName((pin_size_t)_cs);
    reg = nrf_gpio_pin_port_decode(&pin);
    _csPortSet = &reg->OUTSET;
    _csPortClr = &reg->OUTCLR;
    _csPinMask = 1UL << pin;
  }
#elif defined(TARGET_RP2040)
  _dcPinMask = digitalPinToBitMask(_dc);
  _dcPortSet = (PORTreg_t)&sio_hw->gpio_set;
  _dcPortClr = (PORTreg_t)&sio_hw->gpio_clr;
  if (_cs != GFX_NOT_DEFINED)
  {
    _csPinMask = digitalPinToBitMask(_cs);
    _csPortSet = (PORTreg_t)&sio_hw->gpio_set;
    _csPortClr = (PORTreg_t)&sio_hw->gpio_clr;
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
#elif defined(ESP32)
  _dcPinMask = digitalPinToBitMask(_dc);
  if (_dc >= 32)
  {
    _dcPortSet = (PORTreg_t)&GPIO.out1_w1ts.val;
    _dcPortClr = (PORTreg_t)&GPIO.out1_w1tc.val;
  }
  else
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
#elif defined(CORE_TEENSY)
#if !defined(KINETISK)
  _dcPinMask = digitalPinToBitMask(_dc);
#endif
  _dcPortSet = portSetRegister(_dc);
  _dcPortClr = portClearRegister(_dc);
  if (_cs != GFX_NOT_DEFINED)
  {
#if !defined(KINETISK)
    _csPinMask = digitalPinToBitMask(_cs);
#endif
    _csPortSet = portSetRegister(_cs);
    _csPortClr = portClearRegister(_cs);
  }
#else  // !CORE_TEENSY
  _dcPinMask = digitalPinToBitMask(_dc);
  _dcPortSet = &(PORT->Group[g_APinDescription[_dc].ulPort].OUTSET.reg);
  _dcPortClr = &(PORT->Group[g_APinDescription[_dc].ulPort].OUTCLR.reg);
  if (_cs != GFX_NOT_DEFINED)
  {
    _csPinMask = digitalPinToBitMask(_cs);
    _csPortSet = &(PORT->Group[g_APinDescription[_cs].ulPort].OUTSET.reg);
    _csPortClr = &(PORT->Group[g_APinDescription[_cs].ulPort].OUTCLR.reg);
  }
#endif // end !CORE_TEENSY
#else  // !HAS_PORT_SET_CLR
  _dcPort = (PORTreg_t)portOutputRegister(digitalPinToPort(_dc));
  _dcPinMaskSet = digitalPinToBitMask(_dc);
  _dcPinMaskClr = ~_dcPinMaskSet;
  if (_cs != GFX_NOT_DEFINED)
  {
    _csPort = (PORTreg_t)portOutputRegister(digitalPinToPort(_cs));
    _csPinMaskSet = digitalPinToBitMask(_cs);
  }
  _csPinMaskClr = ~_csPinMaskSet;
#endif // !HAS_PORT_SET_CLR
#endif // USE_FAST_PINIO

#if defined(ESP32)
  _spi->begin(_sck, _miso, _mosi);
  if (_dataMode == GFX_NOT_DEFINED)
  {
    _dataMode = SPI_MODE0;
  }
  mySPISettings = SPISettings(_speed, MSBFIRST, _dataMode);
#elif defined(ESP8266)
  _spi->begin();
  if (_dataMode == GFX_NOT_DEFINED)
  {
    _dataMode = SPI_MODE0;
  }
  mySPISettings = SPISettings(_speed, MSBFIRST, _dataMode);
// Teensy 4.x
#elif defined(__IMXRT1052__) || defined(__IMXRT1062__)
  _spi->begin();
  if (_dataMode == GFX_NOT_DEFINED)
  {
    _dataMode = SPI_MODE0;
  }
  mySPISettings = SPISettings(_speed, MSBFIRST, _dataMode);
#elif defined(SPI_HAS_TRANSACTION)
  _spi->begin();
  if (_dataMode == GFX_NOT_DEFINED)
  {
    _dataMode = SPI_MODE2;
  }
  mySPISettings = SPISettings(_speed, MSBFIRST, _dataMode);
#elif defined(__AVR__) || defined(CORE_TEENSY)
  SPCRbackup = SPCR;
  _spi->begin();
  _spi->setClockDivider(SPI_CLOCK_DIV2);
  if (_dataMode == GFX_NOT_DEFINED)
  {
    _dataMode = SPI_MODE2;
  }
  _spi->setDataMode(_dataMode);
  mySPCR = SPCR;     // save our preferred state
  SPCR = SPCRbackup; // then restore
#elif defined(__SAM3X8E__)
  _spi->begin();
  _spi->setClockDivider(21); // 4MHz
  if (_dataMode == GFX_NOT_DEFINED)
  {
    _dataMode = SPI_MODE2;
  }
  _spi->setDataMode(_dataMode);
#elif defined(__arm__)
  if (_dataMode == GFX_NOT_DEFINED)
  {
    _dataMode = SPI_MODE2;
  }
#endif

  return true;
}

void Arduino_HWSPI::beginWrite()
{
  if (_is_shared_interface)
  {
    SPI_BEGIN_TRANSACTION();
  }

  DC_HIGH();
  CS_LOW();
}

void Arduino_HWSPI::endWrite()
{
  CS_HIGH();

  if (_is_shared_interface)
  {
    SPI_END_TRANSACTION();
  }
}

void Arduino_HWSPI::writeCommand(uint8_t c)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();
}

void Arduino_HWSPI::writeCommand16(uint16_t c)
{
  DC_LOW();

#if defined(LITTLE_FOOT_PRINT)
  _data16.value = c;
  WRITE(_data16.msb);
  WRITE(_data16.lsb);
#else  // !defined(LITTLE_FOOT_PRINT)
  WRITE16(c);
#endif // !defined(LITTLE_FOOT_PRINT)

  DC_HIGH();
}

void Arduino_HWSPI::write(uint8_t d)
{
  WRITE(d);
}

void Arduino_HWSPI::write16(uint16_t d)
{
#if defined(LITTLE_FOOT_PRINT)
  _data16.value = d;
  WRITE(_data16.msb);
  WRITE(_data16.lsb);
#else  // !defined(LITTLE_FOOT_PRINT)
  WRITE16(d);
#endif // !defined(LITTLE_FOOT_PRINT)
}

void Arduino_HWSPI::writeRepeat(uint16_t p, uint32_t len)
{
#if defined(LITTLE_FOOT_PRINT)
  _data16.value = p;
  while (len--)
  {
    WRITE(_data16.msb);
    WRITE(_data16.lsb);
  }
#elif defined(ESP8266) || defined(CONFIG_ARCH_CHIP_CXD56XX)
  MSB_16_SET(p, p);
  uint32_t xferLen = (len < SPI_MAX_PIXELS_AT_ONCE) ? len : SPI_MAX_PIXELS_AT_ONCE;
  for (uint32_t i = 0; i < xferLen; i++)
  {
    _buffer.v16[i] = p;
  }

  while (len)
  {
    xferLen = (len < SPI_MAX_PIXELS_AT_ONCE) ? len : SPI_MAX_PIXELS_AT_ONCE;
    len -= xferLen;

    xferLen += xferLen;
    WRITEBUF(_buffer.v8, xferLen);
  }
#else  // other arch
  MSB_16_SET(p, p);
  uint32_t xferLen;

  while (len)
  {
    xferLen = (len < SPI_MAX_PIXELS_AT_ONCE) ? len : SPI_MAX_PIXELS_AT_ONCE;
    for (uint32_t i = 0; i < xferLen; i++)
    {
      _buffer.v16[i] = p;
    }
    len -= xferLen;

    xferLen += xferLen;
    WRITEBUF(_buffer.v8, xferLen);
  }
#endif // other arch
}

void Arduino_HWSPI::writePixels(uint16_t *data, uint32_t len)
{
#if defined(LITTLE_FOOT_PRINT)
  while (len--)
  {
    _data16.value = *data++;
    WRITE(_data16.msb);
    WRITE(_data16.lsb);
  }
#else  // !defined(LITTLE_FOOT_PRINT)
  uint32_t xferLen;
  uint8_t *b;
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
    b = _buffer.v8;
    for (uint32_t i = 0; i < xferLen; i++)
    {
      t.val = *data++;
      *b++ = t.msb;
      *b++ = t.lsb;
    }
    len -= xferLen;

    xferLen += xferLen; // uint16_t to uint8_t, double length
    WRITEBUF(_buffer.v8, xferLen);
  }
#endif // !defined(LITTLE_FOOT_PRINT)
}

#if !defined(LITTLE_FOOT_PRINT)
void Arduino_HWSPI::writeBytes(uint8_t *data, uint32_t len)
{
  WRITEBUF(data, len);
}

void Arduino_HWSPI::writePattern(uint8_t *data, uint8_t len, uint32_t repeat)
{
#if defined(ESP8266) || defined(ESP32)
  _spi->writePattern(data, len, repeat);
#else  // !(defined(ESP8266) || defined(ESP32))
  while (repeat--)
  {
    WRITEBUF(data, len);
  }
#endif // !(defined(ESP8266) || defined(ESP32))
}
#endif // !defined(LITTLE_FOOT_PRINT)

INLINE void Arduino_HWSPI::WRITE(uint8_t d)
{
#if defined(SPI_HAS_TRANSACTION)
  _spi->transfer(d);
#elif defined(__AVR__) || defined(CORE_TEENSY)
  SPCRbackup = SPCR;
  SPCR = mySPCR;
  _spi->transfer(d);
  SPCR = SPCRbackup;
#elif defined(__arm__)
  _spi->setClockDivider(21); // 4MHz
  _spi->setDataMode(_dataMode);
  _spi->transfer(d);
#endif
}

#if !defined(LITTLE_FOOT_PRINT)

INLINE void Arduino_HWSPI::WRITE16(uint16_t d)
{
#if defined(ESP8266) || defined(ESP32)
  _spi->write16(d);
#elif defined(SPI_HAS_TRANSACTION)
  _spi->transfer16(d);
#elif defined(__AVR__) || defined(CORE_TEENSY)
  SPCRbackup = SPCR;
  SPCR = mySPCR;
  _spi->transfer16(d);
  SPCR = SPCRbackup;
#elif defined(__arm__)
  _spi->setClockDivider(21); // 4MHz
  _spi->setDataMode(_dataMode);
  _spi->transfer16(d);
#else
  _spi->transfer16(d);
#endif
}

INLINE void Arduino_HWSPI::WRITEBUF(uint8_t *buf, size_t count)
{
#if defined(ESP8266) || defined(ESP32)
  _spi->writeBytes(buf, count);
#elif defined(CONFIG_ARCH_CHIP_CXD56XX)
  _spi->send(buf, count);
#else  // other arch.
  _spi->transfer(buf, count);
#endif // other arch.
}

#endif // !defined(LITTLE_FOOT_PRINT)

/******** low level bit twiddling **********/

INLINE void Arduino_HWSPI::DC_HIGH(void)
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

INLINE void Arduino_HWSPI::DC_LOW(void)
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

INLINE void Arduino_HWSPI::CS_HIGH(void)
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

INLINE void Arduino_HWSPI::CS_LOW(void)
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
