#include "Arduino_ESP32SPIDMA.h"

#if defined(ESP32)

/**
 * @brief Arduino_ESP32SPIDMA
 *
 */
Arduino_ESP32SPIDMA::Arduino_ESP32SPIDMA(
    int8_t dc /* = GFX_NOT_DEFINED */, int8_t cs /* = GFX_NOT_DEFINED */, int8_t sck /* = GFX_NOT_DEFINED */, int8_t mosi /* = GFX_NOT_DEFINED */, int8_t miso /* = GFX_NOT_DEFINED */, uint8_t spi_num /* = VSPI for ESP32, HSPI for S2 & S3, FSPI for C3 */, bool is_shared_interface /* = true */)
    : _dc(dc), _spi_num(spi_num), _is_shared_interface(is_shared_interface)
{
#if CONFIG_IDF_TARGET_ESP32
  if (
      sck == GFX_NOT_DEFINED && miso == GFX_NOT_DEFINED && mosi == GFX_NOT_DEFINED && cs == GFX_NOT_DEFINED)
  {
    _sck = (_spi_num == VSPI) ? SCK : 14;
    _miso = (_spi_num == VSPI) ? MISO : 12;
    _mosi = (_spi_num == VSPI) ? MOSI : 13;
    _cs = (_spi_num == VSPI) ? SS : 15;
  }
  else
  {
    _sck = sck;
    _miso = miso;
    _mosi = mosi;
    _cs = cs;
  }
#else
  if (sck == GFX_NOT_DEFINED && miso == GFX_NOT_DEFINED && mosi == GFX_NOT_DEFINED && cs == GFX_NOT_DEFINED)
  {
    _sck = SCK;
    _miso = MISO;
    _mosi = MOSI;
    _cs = SS;
  }
  else
  {
    _sck = sck;
    _miso = miso;
    _mosi = mosi;
    _cs = cs;
  }
#endif
}

/**
 * @brief begin
 *
 * @param speed
 * @param dataMode
 * @return true
 * @return false
 */
bool Arduino_ESP32SPIDMA::begin(int32_t speed, int8_t dataMode)
{
  // set SPI parameters
  _speed = (speed == GFX_NOT_DEFINED) ? SPI_DEFAULT_FREQ : speed;
  _dataMode = (dataMode == GFX_NOT_DEFINED) ? SPI_MODE0 : dataMode;

  if (!_div)
  {
    _div = spiFrequencyToClockDiv(_speed);
  }

  // set pin mode
  if (_dc != GFX_NOT_DEFINED)
  {
    pinMode(_dc, OUTPUT);
    digitalWrite(_dc, HIGH); // Data mode
  }
  if (_cs != GFX_NOT_DEFINED)
  {
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH); // disable chip select
  }

#if (CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3)
  // set fastIO variables
  if (_dc >= 32)
  {
    _dcPinMask = digitalPinToBitMask(_dc);
    _dcPortSet = (PORTreg_t)&GPIO.out1_w1ts.val;
    _dcPortClr = (PORTreg_t)&GPIO.out1_w1tc.val;
  }
  else
#endif
      if (_dc != GFX_NOT_DEFINED)
  {
    _dcPinMask = digitalPinToBitMask(_dc);
    _dcPortSet = (PORTreg_t)&GPIO.out_w1ts;
    _dcPortClr = (PORTreg_t)&GPIO.out_w1tc;
  }

#if (CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3)
  if (_cs >= 32)
  {
    _csPinMask = digitalPinToBitMask(_cs);
    _csPortSet = (PORTreg_t)&GPIO.out1_w1ts.val;
    _csPortClr = (PORTreg_t)&GPIO.out1_w1tc.val;
  }
  else
#endif
      if (_cs != GFX_NOT_DEFINED)
  {
    _csPinMask = digitalPinToBitMask(_cs);
    _csPortSet = (PORTreg_t)&GPIO.out_w1ts;
    _csPortClr = (PORTreg_t)&GPIO.out_w1tc;
  }

  spi_bus_config_t buscfg = {
      .mosi_io_num = _mosi,
      .miso_io_num = _miso,
      .sclk_io_num = _sck,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
      .max_transfer_sz = (SPI_MAX_PIXELS_AT_ONCE * 16) + 8,
  };
  esp_err_t ret = spi_bus_initialize((spi_host_device_t)_spi_num, &buscfg, DMA_CHANNEL);
  if (ret != ESP_OK)
  {
    ESP_ERROR_CHECK(ret);
    return false;
  }

  spi_device_interface_config_t devcfg = {
      .command_bits = 0,
      .address_bits = 0,
      .dummy_bits = 0,
      .mode = (uint8_t)_dataMode,
      .duty_cycle_pos = 128,
      .cs_ena_pretrans = 0,
      .cs_ena_posttrans = 0,
      .clock_speed_hz = _speed,
      .input_delay_ns = 0,
      .spics_io_num = -1, // avoid use system CS control
      .flags = (_miso < 0) ? (uint32_t)SPI_DEVICE_NO_DUMMY : 0,
      .queue_size = 1,
  };
  ret = spi_bus_add_device((spi_host_device_t)_spi_num, &devcfg, &_handle);
  if (ret != ESP_OK)
  {
    ESP_ERROR_CHECK(ret);
    return false;
  }

  if (!_is_shared_interface)
  {
    spi_device_acquire_bus(_handle, portMAX_DELAY);
  }

  memset(&_spi_tran, 0, sizeof(_spi_tran));

  return true;
}

/**
 * @brief beginWrite
 *
 */
void Arduino_ESP32SPIDMA::beginWrite()
{
  _data_buf_bit_idx = 0;
  _buffer[0] = 0;

  if (_is_shared_interface)
  {
    spi_device_acquire_bus(_handle, portMAX_DELAY);
  }

  if (_dc != GFX_NOT_DEFINED)
  {
    DC_HIGH();
  }
  CS_LOW();
}

/**
 * @brief endWrite
 *
 */
void Arduino_ESP32SPIDMA::endWrite()
{
  if (_data_buf_bit_idx > 0)
  {
    flush_data_buf();
  }

  if (_is_shared_interface)
  {
    spi_device_release_bus(_handle);
  }

  CS_HIGH();
}

/**
 * @brief writeCommand
 *
 * @param c
 */
void Arduino_ESP32SPIDMA::writeCommand(uint8_t c)
{
  if (_dc == GFX_NOT_DEFINED) // 9-bit SPI
  {
    WRITE9BIT(c);
  }
  else
  {
    if (_data_buf_bit_idx > 0)
    {
      flush_data_buf();
    }

    DC_LOW();

    _spi_tran.length = 8;
    _spi_tran.tx_data[0] = c;
    _spi_tran.flags = SPI_TRANS_USE_TXDATA;

    POLL_START();
    POLL_END();

    DC_HIGH();
  }
}

/**
 * @brief writeCommand16
 *
 * @param c
 */
void Arduino_ESP32SPIDMA::writeCommand16(uint16_t c)
{
  if (_dc == GFX_NOT_DEFINED) // 9-bit SPI
  {
    _data16.value = c;
    WRITE9BIT(_data16.msb);
    WRITE9BIT(_data16.lsb);
  }
  else
  {
    if (_data_buf_bit_idx > 0)
    {
      flush_data_buf();
    }

    DC_LOW();

    _spi_tran.length = 16;
    MSB_16_SET(_spi_tran.tx_data[0], c);
    _spi_tran.flags = SPI_TRANS_USE_TXDATA;

    POLL_START();
    POLL_END();

    DC_HIGH();
  }
}

/**
 * @brief write
 *
 * @param d
 */
void Arduino_ESP32SPIDMA::write(uint8_t d)
{
  if (_dc == GFX_NOT_DEFINED) // 9-bit SPI
  {
    WRITE9BIT(0x100 | d);
  }
  else
  {
    WRITE8BIT(d);
  }
}

/**
 * @brief write16
 *
 * @param d
 */
void Arduino_ESP32SPIDMA::write16(uint16_t d)
{
  _data16.value = d;
  if (_dc == GFX_NOT_DEFINED) // 9-bit SPI
  {
    WRITE9BIT(0x100 | _data16.msb);
    WRITE9BIT(0x100 | _data16.lsb);
  }
  else
  {
    WRITE8BIT(_data16.msb);
    WRITE8BIT(_data16.lsb);
  }
}

/**
 * @brief writeC8D8
 *
 * @param c
 * @param d
 */
void Arduino_ESP32SPIDMA::writeC8D8(uint8_t c, uint8_t d)
{
  if (_dc == GFX_NOT_DEFINED) // 9-bit SPI
  {
    WRITE9BIT(c);
    WRITE9BIT(0x100 | d);
  }
  else
  {
    if (_data_buf_bit_idx > 0)
    {
      flush_data_buf();
    }

    DC_LOW();

    _spi_tran.length = 8;
    _spi_tran.tx_data[0] = c;
    _spi_tran.flags = SPI_TRANS_USE_TXDATA;

    POLL_START();
    POLL_END();

    DC_HIGH();

    _spi_tran.length = 8;
    _spi_tran.tx_data[0] = d;
    _spi_tran.flags = SPI_TRANS_USE_TXDATA;

    POLL_START();
    POLL_END();
  }
}

/**
 * @brief writeC8D16
 *
 * @param c
 * @param d
 */
void Arduino_ESP32SPIDMA::writeC8D16(uint8_t c, uint16_t d)
{
  if (_dc == GFX_NOT_DEFINED) // 9-bit SPI
  {
    WRITE9BIT(c);
    _data16.value = d;
    WRITE9BIT(0x100 | _data16.msb);
    WRITE9BIT(0x100 | _data16.lsb);
  }
  else
  {
    if (_data_buf_bit_idx > 0)
    {
      flush_data_buf();
    }

    DC_LOW();

    _spi_tran.length = 8;
    _spi_tran.tx_data[0] = c;
    _spi_tran.flags = SPI_TRANS_USE_TXDATA;

    POLL_START();
    POLL_END();

    DC_HIGH();

    _spi_tran.length = 16;
    _spi_tran.tx_data[0] = (d >> 8);
    _spi_tran.tx_data[1] = (d & 0xff);
    _spi_tran.flags = SPI_TRANS_USE_TXDATA;

    POLL_START();
    POLL_END();
  }
}

/**
 * @brief writeC8D16D16
 *
 * @param c
 * @param d1
 * @param d2
 */
void Arduino_ESP32SPIDMA::writeC8D16D16(uint8_t c, uint16_t d1, uint16_t d2)
{
  if (_dc == GFX_NOT_DEFINED) // 9-bit SPI
  {
    WRITE9BIT(c);
    _data16.value = d1;
    WRITE9BIT(0x100 | _data16.msb);
    WRITE9BIT(0x100 | _data16.lsb);
    _data16.value = d2;
    WRITE9BIT(0x100 | _data16.msb);
    WRITE9BIT(0x100 | _data16.lsb);
  }
  else
  {
    if (_data_buf_bit_idx > 0)
    {
      flush_data_buf();
    }

    DC_LOW();

    _spi_tran.length = 8;
    _spi_tran.tx_data[0] = c;
    _spi_tran.flags = SPI_TRANS_USE_TXDATA;

    POLL_START();
    POLL_END();

    DC_HIGH();

    _spi_tran.length = 32;
    _spi_tran.tx_data[0] = (d1 >> 8);
    _spi_tran.tx_data[1] = (d1 & 0xff);
    _spi_tran.tx_data[2] = (d2 >> 8);
    _spi_tran.tx_data[3] = (d2 & 0xff);
    _spi_tran.flags = SPI_TRANS_USE_TXDATA;

    POLL_START();
    POLL_END();
  }
}

/**
 * @brief writeRepeat
 *
 * @param p
 * @param len
 */
void Arduino_ESP32SPIDMA::writeRepeat(uint16_t p, uint32_t len)
{
  if (_data_buf_bit_idx > 0)
  {
    flush_data_buf();
  }

  if (_dc == GFX_NOT_DEFINED) // 9-bit SPI
  {
    _data16.value = p;
    uint32_t hi = 0x100 | _data16.msb;
    uint32_t lo = 0x100 | _data16.lsb;
    uint16_t idx;
    uint8_t shift;
    uint32_t l;
    uint16_t bufLen = (len <= 28) ? len : 28;
    int16_t xferLen;
    for (uint32_t t = 0; t < bufLen; t++)
    {
      idx = _data_buf_bit_idx >> 3;
      shift = (_data_buf_bit_idx % 8);
      if (shift)
      {
        _buffer[idx++] |= hi >> (shift + 1);
        _buffer[idx] = hi << (7 - shift);
      }
      else
      {
        _buffer[idx++] = hi >> 1;
        _buffer[idx] = hi << 7;
      }
      _data_buf_bit_idx += 9;

      idx = _data_buf_bit_idx >> 3;
      shift = (_data_buf_bit_idx % 8);
      if (shift)
      {
        _buffer[idx++] |= lo >> (shift + 1);
        _buffer[idx] = lo << (7 - shift);
      }
      else
      {
        _buffer[idx++] = lo >> 1;
        _buffer[idx] = lo << 7;
      }
      _data_buf_bit_idx += 9;
    }

    // Issue pixels in blocks from temp buffer
    while (len) // While pixels remain
    {
      xferLen = (bufLen < len) ? bufLen : len; // How many this pass?
      _data_buf_bit_idx = xferLen * 18;

      _spi_tran.tx_buffer = _buffer32;
      _spi_tran.length = _data_buf_bit_idx;
      _spi_tran.flags = 0;

      POLL_START();
      POLL_END();

      len -= xferLen;
    }
  }
  else // 8-bit SPI
  {
    uint16_t bufLen = (len >= SPI_MAX_PIXELS_AT_ONCE) ? SPI_MAX_PIXELS_AT_ONCE : len;
    int16_t xferLen, l;
    uint32_t c32;
    MSB_32_16_16_SET(c32, p, p);

    l = (bufLen + 1) / 2;
    for (uint32_t i = 0; i < l; i++)
    {
      _buffer32[i] = c32;
    }

    // Issue pixels in blocks from temp buffer
    while (len) // While pixels remain
    {
      xferLen = (bufLen <= len) ? bufLen : len; // How many this pass?

      _spi_tran.tx_buffer = _buffer32;
      _spi_tran.length = xferLen << 4;
      _spi_tran.flags = 0;

      POLL_START();
      POLL_END();

      len -= xferLen;
    }
  }

  _data_buf_bit_idx = 0;
}

/**
 * @brief writePixels
 *
 * @param data
 * @param len
 */
void Arduino_ESP32SPIDMA::writePixels(uint16_t *data, uint32_t len)
{
  if (_dc == GFX_NOT_DEFINED) // 9-bit SPI
  {
    while (len--)
    {
      write16(*data++);
    }
  }
  else // 8-bit SPI
  {
    if (_data_buf_bit_idx > 0)
    {
      flush_data_buf();
    }

    uint32_t l, l2;
    uint16_t p1, p2;
    while (len)
    {
      l = (len > SPI_MAX_PIXELS_AT_ONCE) ? SPI_MAX_PIXELS_AT_ONCE : len;
      l2 = (l + 1) >> 1;
      for (uint32_t i = 0; i < l2; ++i)
      {
        p1 = *data++;
        p2 = *data++;
        MSB_32_16_16_SET(_buffer32[i], p1, p2);
      }
      if (l & 1)
      {
        p1 = *data++;
        MSB_16_SET(_buffer16[l - 1], p1);
      }

      _spi_tran.tx_buffer = _buffer32;
      _spi_tran.length = l << 4;
      _spi_tran.flags = 0;

      POLL_START();
      POLL_END();

      len -= l;
    }
  }
}

/**
 * @brief writeBytes
 *
 * @param data
 * @param len
 */
void Arduino_ESP32SPIDMA::writeBytes(uint8_t *data, uint32_t len)
{
  if (_dc == GFX_NOT_DEFINED) // 9-bit SPI
  {
    while (len--)
    {
      write(*data++);
    }
  }
  else // 8-bit SPI
  {
    if (esp_ptr_dma_capable(data))
    {
      if (_data_buf_bit_idx > 0)
      {
        flush_data_buf();
      }

      uint32_t l;
      while (len)
      {
        l = (len >= (SPI_MAX_PIXELS_AT_ONCE << 1)) ? (SPI_MAX_PIXELS_AT_ONCE << 1) : len;

        _spi_tran.tx_buffer = data;
        _spi_tran.length = l << 3;
        _spi_tran.flags = 0;

        POLL_START();
        POLL_END();

        len -= l;
        data += l;
      }
    }
    else
    {
      if (_data_buf_bit_idx > 0)
      {
        flush_data_buf();
      }

      uint32_t l, l4;
      uint32_t *p;
      while (len)
      {
        l = (len > (SPI_MAX_PIXELS_AT_ONCE << 1)) ? (SPI_MAX_PIXELS_AT_ONCE << 1) : len;
        l4 = (l + 3) >> 2;
        p = (uint32_t *)data;
        for (uint32_t i = 0; i < l4; ++i)
        {
          _buffer32[i] = *p++;
        }

        _spi_tran.tx_buffer = _buffer32;
        _spi_tran.length = l << 3;
        _spi_tran.flags = 0;

        POLL_START();
        POLL_END();

        len -= l;
        data += l;
      }
    }
  }
}

/**
 * @brief writeIndexedPixels
 *
 * @param data
 * @param idx
 * @param len
 */
void Arduino_ESP32SPIDMA::writeIndexedPixels(uint8_t *data, uint16_t *idx, uint32_t len)
{
  if (_dc == GFX_NOT_DEFINED) // 9-bit SPI
  {
    while (len--)
    {
      write16(idx[*data++]);
    }
  }
  else // 8-bit SPI
  {
    if (_data_buf_bit_idx > 0)
    {
      flush_data_buf();
    }

    uint32_t l, l2;
    uint16_t p1, p2;
    while (len)
    {
      l = (len > SPI_MAX_PIXELS_AT_ONCE) ? SPI_MAX_PIXELS_AT_ONCE : len;
      l2 = l >> 1;
      for (uint32_t i = 0; i < l2; ++i)
      {
        p1 = idx[*data++];
        p2 = idx[*data++];
        MSB_32_16_16_SET(_buffer32[i], p1, p2);
      }
      if (l & 1)
      {
        p1 = idx[*data++];
        MSB_16_SET(_buffer16[l - 1], p1);
      }

      _spi_tran.tx_buffer = _buffer32;
      _spi_tran.length = l << 4;
      _spi_tran.flags = 0;

      POLL_START();
      POLL_END();

      len -= l;
    }
  }
}

/**
 * @brief writeIndexedPixelsDouble
 *
 * @param data
 * @param idx
 * @param len
 */
void Arduino_ESP32SPIDMA::writeIndexedPixelsDouble(uint8_t *data, uint16_t *idx, uint32_t len)
{
  if (_dc == GFX_NOT_DEFINED) // 9-bit SPI
  {
    uint16_t hi, lo;
    while (len--)
    {
      _data16.value = idx[*data++];
      hi = 0x100 | _data16.msb;
      lo = 0x100 | _data16.lsb;
      WRITE9BIT(hi);
      WRITE9BIT(lo);
      WRITE9BIT(hi);
      WRITE9BIT(lo);
    }
  }
  else // 8-bit SPI
  {
    if (_data_buf_bit_idx > 0)
    {
      flush_data_buf();
    }

    uint32_t l;
    uint16_t p;
    while (len)
    {
      l = (len > (SPI_MAX_PIXELS_AT_ONCE >> 1)) ? (SPI_MAX_PIXELS_AT_ONCE >> 1) : len;
      for (uint32_t i = 0; i < l; ++i)
      {
        p = idx[*data++];
        MSB_32_16_16_SET(_buffer32[i], p, p);
      }

      _spi_tran.tx_buffer = _buffer32;
      _spi_tran.length = l << 5;
      _spi_tran.flags = 0;

      POLL_START();
      POLL_END();

      len -= l;
    }
  }
}

/**
 * @brief flush_data_buf
 *
 */
void Arduino_ESP32SPIDMA::flush_data_buf()
{
  _spi_tran.tx_buffer = _buffer32;
  _spi_tran.length = _data_buf_bit_idx;
  _spi_tran.flags = 0;

  POLL_START();
  POLL_END();

  _data_buf_bit_idx = 0;
}

/**
 * @brief WRITE8BIT
 *
 * @param d
 * @return INLINE
 */
INLINE void Arduino_ESP32SPIDMA::WRITE8BIT(uint8_t d)
{
  uint16_t idx = _data_buf_bit_idx >> 3;
  _buffer[idx] = d;
  _data_buf_bit_idx += 8;
  if (_data_buf_bit_idx >= (SPI_MAX_PIXELS_AT_ONCE << 4))
  {
    flush_data_buf();
  }
}

/**
 * @brief WRITE9BIT
 *
 * @param d
 * @return INLINE
 */
INLINE void Arduino_ESP32SPIDMA::WRITE9BIT(uint32_t d)
{
  uint16_t idx = _data_buf_bit_idx >> 3;
  uint8_t shift = (_data_buf_bit_idx % 8);
  if (shift)
  {
    _buffer[idx++] |= d >> (shift + 1);
    _buffer[idx] = d << (7 - shift);
  }
  else
  {
    _buffer[idx++] = d >> 1;
    _buffer[idx] = d << 7;
  }
  _data_buf_bit_idx += 9;
  if (_data_buf_bit_idx >= 504) // 56 bytes * 9 bits
  {
    flush_data_buf();
  }
}

/******** low level bit twiddling **********/

/**
 * @brief DC_HIGH
 *
 * @return INLINE
 */
INLINE void Arduino_ESP32SPIDMA::DC_HIGH(void)
{
  *_dcPortSet = _dcPinMask;
}

/**
 * @brief DC_LOW
 *
 * @return INLINE
 */
INLINE void Arduino_ESP32SPIDMA::DC_LOW(void)
{
  *_dcPortClr = _dcPinMask;
}

/**
 * @brief CS_HIGH
 *
 * @return INLINE
 */
INLINE void Arduino_ESP32SPIDMA::CS_HIGH(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
    *_csPortSet = _csPinMask;
  }
}

/**
 * @brief CS_LOW
 *
 * @return INLINE
 */
INLINE void Arduino_ESP32SPIDMA::CS_LOW(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
    *_csPortClr = _csPinMask;
  }
}

/**
 * @brief POLL_START
 *
 * @return INLINE
 */
INLINE void Arduino_ESP32SPIDMA::POLL_START()
{
  esp_err_t ret = spi_device_polling_start(_handle, &_spi_tran, portMAX_DELAY);
  // if (ret != ESP_OK)
  // {
  //   log_e("spi_device_polling_start error: %d", ret);
  // }
}

/**
 * @brief POLL_END
 *
 * @return INLINE
 */
INLINE void Arduino_ESP32SPIDMA::POLL_END()
{
  esp_err_t ret = spi_device_polling_end(_handle, portMAX_DELAY);
  // if (ret != ESP_OK)
  // {
  //   log_e("spi_device_polling_end error: %d", ret);
  // }
}

#endif // #if defined(ESP32)
