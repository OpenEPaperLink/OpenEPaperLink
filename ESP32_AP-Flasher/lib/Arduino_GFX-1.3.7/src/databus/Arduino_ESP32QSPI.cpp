#include "Arduino_ESP32QSPI.h"

#if defined(ESP32)

/**
 * @brief Arduino_ESP32QSPI
 *
 */
Arduino_ESP32QSPI::Arduino_ESP32QSPI(
    int8_t cs, int8_t sck, int8_t mosi, int8_t miso, int8_t quadwp, int8_t quadhd, bool is_shared_interface /* = false */)
    : _cs(cs), _sck(sck), _mosi(mosi), _miso(miso), _quadwp(quadwp), _quadhd(quadhd), _is_shared_interface(is_shared_interface)
{
}

/**
 * @brief begin
 *
 * @param speed
 * @param dataMode
 * @return true
 * @return false
 */
bool Arduino_ESP32QSPI::begin(int32_t speed, int8_t dataMode)
{
  // set SPI parameters
  _speed = (speed == GFX_NOT_DEFINED) ? QSPI_FREQUENCY : speed;
  _dataMode = (dataMode == GFX_NOT_DEFINED) ? QSPI_SPI_MODE : dataMode;

  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, HIGH); // disable chip select
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
      .quadwp_io_num = _quadwp,
      .quadhd_io_num = _quadhd,
      .max_transfer_sz = (SPI_MAX_PIXELS_AT_ONCE * 16) + 8,
      .flags = SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_GPIO_PINS,
  };
  esp_err_t ret = spi_bus_initialize(QSPI_SPI_HOST, &buscfg, QSPI_DMA_CHANNEL);
  if (ret != ESP_OK)
  {
    ESP_ERROR_CHECK(ret);
    return false;
  }

  spi_device_interface_config_t devcfg = {
      .command_bits = 8,
      .address_bits = 24,
      .mode = _dataMode,
      .clock_speed_hz = _speed,
      .spics_io_num = -1, // avoid use system CS control
      .flags = SPI_DEVICE_HALFDUPLEX,
      .queue_size = 1,
  };
  ret = spi_bus_add_device(QSPI_SPI_HOST, &devcfg, &_handle);
  if (ret != ESP_OK)
  {
    ESP_ERROR_CHECK(ret);
    return false;
  }

  if (!_is_shared_interface)
  {
    spi_device_acquire_bus(_handle, portMAX_DELAY);
  }

  memset(&_spi_tran_ext, 0, sizeof(_spi_tran_ext));
  _spi_tran = (spi_transaction_t *)&_spi_tran_ext;

  return true;
}

/**
 * @brief beginWrite
 *
 */
void Arduino_ESP32QSPI::beginWrite()
{
  if (_is_shared_interface)
  {
    spi_device_acquire_bus(_handle, portMAX_DELAY);
  }
}

/**
 * @brief endWrite
 *
 */
void Arduino_ESP32QSPI::endWrite()
{
  if (_is_shared_interface)
  {
    spi_device_acquire_bus(_handle, portMAX_DELAY);
  }
}

/**
 * @brief writeCommand
 *
 * @param c
 */
void Arduino_ESP32QSPI::writeCommand(uint8_t c)
{
  CS_LOW();
  _spi_tran_ext.base.flags = SPI_TRANS_MULTILINE_CMD | SPI_TRANS_MULTILINE_ADDR;
  _spi_tran_ext.base.cmd = 0x02;
  _spi_tran_ext.base.addr = ((uint32_t)c) << 8;
  _spi_tran_ext.base.tx_buffer = NULL;
  _spi_tran_ext.base.length = 0;
  POLL_START();
  POLL_END();
  CS_HIGH();
}

/**
 * @brief writeCommand16
 *
 * @param c
 */
void Arduino_ESP32QSPI::writeCommand16(uint16_t c)
{
  CS_LOW();
  _spi_tran_ext.base.flags = SPI_TRANS_MULTILINE_CMD | SPI_TRANS_MULTILINE_ADDR;
  _spi_tran_ext.base.cmd = 0x02;
  _spi_tran_ext.base.addr = c;
  _spi_tran_ext.base.tx_buffer = NULL;
  _spi_tran_ext.base.length = 0;
  POLL_START();
  POLL_END();
  CS_HIGH();
}

/**
 * @brief write
 *
 * @param d
 */
void Arduino_ESP32QSPI::write(uint8_t d)
{
  CS_LOW();
  _spi_tran_ext.base.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_MODE_QIO;
  _spi_tran_ext.base.cmd = 0x32;
  _spi_tran_ext.base.addr = 0x003C00;
  _spi_tran_ext.base.tx_data[0] = d;
  _spi_tran_ext.base.length = 8;
  POLL_START();
  POLL_END();
  CS_HIGH();
}

/**
 * @brief write16
 *
 * @param d
 */
void Arduino_ESP32QSPI::write16(uint16_t d)
{
  CS_LOW();
  _spi_tran_ext.base.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_MODE_QIO;
  _spi_tran_ext.base.cmd = 0x32;
  _spi_tran_ext.base.addr = 0x003C00;
  _spi_tran_ext.base.tx_data[0] = d >> 8;
  _spi_tran_ext.base.tx_data[1] = d;
  _spi_tran_ext.base.length = 16;
  POLL_START();
  POLL_END();
  CS_HIGH();
}

/**
 * @brief writeC8D8
 *
 * @param c
 * @param d
 */
void Arduino_ESP32QSPI::writeC8D8(uint8_t c, uint8_t d)
{
  CS_LOW();
  _spi_tran_ext.base.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_MULTILINE_CMD | SPI_TRANS_MULTILINE_ADDR;
  _spi_tran_ext.base.cmd = 0x02;
  _spi_tran_ext.base.addr = ((uint32_t)c) << 8;
  _spi_tran_ext.base.tx_data[0] = d;
  _spi_tran_ext.base.length = 8;
  POLL_START();
  POLL_END();
  CS_HIGH();
}

/**
 * @brief writeC8D16D16
 *
 * @param c
 * @param d1
 * @param d2
 */
void Arduino_ESP32QSPI::writeC8D16(uint8_t c, uint16_t d)
{
  CS_LOW();
  _spi_tran_ext.base.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_MULTILINE_CMD | SPI_TRANS_MULTILINE_ADDR;
  _spi_tran_ext.base.cmd = 0x02;
  _spi_tran_ext.base.addr = ((uint32_t)c) << 8;
  _spi_tran_ext.base.tx_data[0] = d >> 8;
  _spi_tran_ext.base.tx_data[1] = d;
  _spi_tran_ext.base.length = 16;
  POLL_START();
  POLL_END();
  CS_HIGH();
}

/**
 * @brief writeC8D16D16
 *
 * @param c
 * @param d1
 * @param d2
 */
void Arduino_ESP32QSPI::writeC8D16D16(uint8_t c, uint16_t d1, uint16_t d2)
{
  CS_LOW();
  _spi_tran_ext.base.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_MULTILINE_CMD | SPI_TRANS_MULTILINE_ADDR;
  _spi_tran_ext.base.cmd = 0x02;
  _spi_tran_ext.base.addr = ((uint32_t)c) << 8;
  _spi_tran_ext.base.tx_data[0] = d1 >> 8;
  _spi_tran_ext.base.tx_data[1] = d1;
  _spi_tran_ext.base.tx_data[2] = d2 >> 8;
  _spi_tran_ext.base.tx_data[3] = d2;
  _spi_tran_ext.base.length = 32;
  POLL_START();
  POLL_END();
  CS_HIGH();
}

/**
 * @brief writeRepeat
 *
 * @param p
 * @param len
 */
void Arduino_ESP32QSPI::writeRepeat(uint16_t p, uint32_t len)
{
  bool first_send = true;

  uint16_t bufLen = (len >= SPI_MAX_PIXELS_AT_ONCE) ? SPI_MAX_PIXELS_AT_ONCE : len;
  int16_t xferLen, l;
  uint32_t c32;
  MSB_32_16_16_SET(c32, p, p);

  l = (bufLen + 1) / 2;
  for (uint32_t i = 0; i < l; i++)
  {
    _buffer32[i] = c32;
  }

  CS_LOW();
  // Issue pixels in blocks from temp buffer
  while (len) // While pixels remain
  {
    xferLen = (bufLen <= len) ? bufLen : len; // How many this pass?

    if (first_send)
    {
      _spi_tran_ext.base.flags = SPI_TRANS_MODE_QIO;
      _spi_tran_ext.base.cmd = 0x32;
      _spi_tran_ext.base.addr = 0x003C00;
      first_send = false;
    }
    else
    {
      _spi_tran_ext.base.flags = SPI_TRANS_MODE_QIO | SPI_TRANS_VARIABLE_CMD |
                                 SPI_TRANS_VARIABLE_ADDR | SPI_TRANS_VARIABLE_DUMMY;
    }
    _spi_tran_ext.base.tx_buffer = _buffer16;
    _spi_tran_ext.base.length = xferLen << 4;

    POLL_START();
    POLL_END();

    len -= xferLen;
  }
  CS_HIGH();
}

/**
 * @brief writePixels
 *
 * @param data
 * @param len
 */
void Arduino_ESP32QSPI::writePixels(uint16_t *data, uint32_t len)
{

  CS_LOW();
  uint32_t l, l2;
  uint16_t p1, p2;
  bool first_send = true;
  while (len)
  {
    l = (len > SPI_MAX_PIXELS_AT_ONCE) ? SPI_MAX_PIXELS_AT_ONCE : len;

    if (first_send)
    {
      _spi_tran_ext.base.flags = SPI_TRANS_MODE_QIO;
      _spi_tran_ext.base.cmd = 0x32;
      _spi_tran_ext.base.addr = 0x003C00;
      first_send = false;
    }
    else
    {
      _spi_tran_ext.base.flags = SPI_TRANS_MODE_QIO | SPI_TRANS_VARIABLE_CMD |
                                 SPI_TRANS_VARIABLE_ADDR | SPI_TRANS_VARIABLE_DUMMY;
    }
    l2 = l >> 1;
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

    _spi_tran_ext.base.tx_buffer = _buffer32;
    _spi_tran_ext.base.length = l << 4;

    POLL_START();
    POLL_END();

    len -= l;
  }
  CS_HIGH();
}

/**
 * @brief writeBytes
 *
 * @param data
 * @param len
 */
void Arduino_ESP32QSPI::writeBytes(uint8_t *data, uint32_t len)
{
  CS_LOW();
  uint32_t l;
  bool first_send = true;
  while (len)
  {
    l = (len >= (SPI_MAX_PIXELS_AT_ONCE << 1)) ? (SPI_MAX_PIXELS_AT_ONCE << 1) : len;

    if (first_send)
    {
      _spi_tran_ext.base.flags = SPI_TRANS_MODE_QIO;
      _spi_tran_ext.base.cmd = 0x32;
      _spi_tran_ext.base.addr = 0x003C00;
      first_send = false;
    }
    else
    {
      _spi_tran_ext.base.flags = SPI_TRANS_MODE_QIO | SPI_TRANS_VARIABLE_CMD |
                                 SPI_TRANS_VARIABLE_ADDR | SPI_TRANS_VARIABLE_DUMMY;
    }

    _spi_tran_ext.base.tx_buffer = data;
    _spi_tran_ext.base.length = l << 3;

    POLL_START();
    POLL_END();

    len -= l;
    data += l;
  }
  CS_HIGH();
}

/**
 * @brief writeIndexedPixels
 *
 * @param data
 * @param idx
 * @param len
 */
void Arduino_ESP32QSPI::writeIndexedPixels(uint8_t *data, uint16_t *idx, uint32_t len)
{
  CS_LOW();
  uint32_t l, l2;
  uint16_t p1, p2;
  bool first_send = true;
  while (len)
  {
    l = (len > SPI_MAX_PIXELS_AT_ONCE) ? SPI_MAX_PIXELS_AT_ONCE : len;

    if (first_send)
    {
      _spi_tran_ext.base.flags = SPI_TRANS_MODE_QIO;
      _spi_tran_ext.base.cmd = 0x32;
      _spi_tran_ext.base.addr = 0x003C00;
      first_send = false;
    }
    else
    {
      _spi_tran_ext.base.flags = SPI_TRANS_MODE_QIO | SPI_TRANS_VARIABLE_CMD |
                                 SPI_TRANS_VARIABLE_ADDR | SPI_TRANS_VARIABLE_DUMMY;
    }
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

    _spi_tran_ext.base.tx_buffer = _buffer32;
    _spi_tran_ext.base.length = l << 4;

    POLL_START();
    POLL_END();

    len -= l;
  }
  CS_HIGH();
}

/**
 * @brief writeIndexedPixelsDouble
 *
 * @param data
 * @param idx
 * @param len
 */
void Arduino_ESP32QSPI::writeIndexedPixelsDouble(uint8_t *data, uint16_t *idx, uint32_t len)
{
  CS_LOW();
  uint32_t l;
  uint16_t p;
  bool first_send = true;
  while (len)
  {
    l = (len > (SPI_MAX_PIXELS_AT_ONCE >> 1)) ? (SPI_MAX_PIXELS_AT_ONCE >> 1) : len;

    if (first_send)
    {
      _spi_tran_ext.base.flags = SPI_TRANS_MODE_QIO;
      _spi_tran_ext.base.cmd = 0x32;
      _spi_tran_ext.base.addr = 0x003C00;
      first_send = false;
    }
    else
    {
      _spi_tran_ext.base.flags = SPI_TRANS_MODE_QIO | SPI_TRANS_VARIABLE_CMD |
                                 SPI_TRANS_VARIABLE_ADDR | SPI_TRANS_VARIABLE_DUMMY;
    }
    for (uint32_t i = 0; i < l; ++i)
    {
      p = idx[*data++];
      MSB_32_16_16_SET(_buffer32[i], p, p);
    }

    _spi_tran_ext.base.tx_buffer = _buffer32;
    _spi_tran_ext.base.length = l << 5;

    POLL_START();
    POLL_END();

    len -= l;
  }
  CS_HIGH();
}

/******** low level bit twiddling **********/

/**
 * @brief CS_HIGH
 *
 * @return INLINE
 */
INLINE void Arduino_ESP32QSPI::CS_HIGH(void)
{
  *_csPortSet = _csPinMask;
}

/**
 * @brief CS_LOW
 *
 * @return INLINE
 */
INLINE void Arduino_ESP32QSPI::CS_LOW(void)
{
  *_csPortClr = _csPinMask;
}

/**
 * @brief POLL_START
 *
 * @return INLINE
 */
INLINE void Arduino_ESP32QSPI::POLL_START()
{
  esp_err_t ret = spi_device_polling_start(_handle, _spi_tran, portMAX_DELAY);
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
INLINE void Arduino_ESP32QSPI::POLL_END()
{
  esp_err_t ret = spi_device_polling_end(_handle, portMAX_DELAY);
  // if (ret != ESP_OK)
  // {
  //   log_e("spi_device_polling_end error: %d", ret);
  // }
}

#endif // #if defined(ESP32)
