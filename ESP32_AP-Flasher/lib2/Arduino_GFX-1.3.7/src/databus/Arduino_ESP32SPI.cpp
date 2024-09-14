/*
 * start rewrite from:
 * https://github.com/espressif/arduino-esp32.git
 */
#include "Arduino_ESP32SPI.h"

#if defined(ESP32)

struct spi_struct_t
{
  spi_dev_t *dev;
#if !CONFIG_DISABLE_HAL_LOCKS
  xSemaphoreHandle lock;
#endif
  uint8_t num;
};

#if CONFIG_DISABLE_HAL_LOCKS
#define SPI_MUTEX_LOCK()
#define SPI_MUTEX_UNLOCK()

static spi_t _spi_bus_array[] = {
#if CONFIG_IDF_TARGET_ESP32S2
    {(volatile spi_dev_t *)(DR_REG_SPI1_BASE), 0},
    {(volatile spi_dev_t *)(DR_REG_SPI2_BASE), 1},
    {(volatile spi_dev_t *)(DR_REG_SPI3_BASE), 2}
#elif CONFIG_IDF_TARGET_ESP32S3
    {(volatile spi_dev_t *)(DR_REG_SPI2_BASE), 0},
    {(volatile spi_dev_t *)(DR_REG_SPI3_BASE), 1}
#else
    {(volatile spi_dev_t *)(DR_REG_SPI0_BASE), 0},
    {(volatile spi_dev_t *)(DR_REG_SPI1_BASE), 1},
    {(volatile spi_dev_t *)(DR_REG_SPI2_BASE), 2},
    {(volatile spi_dev_t *)(DR_REG_SPI3_BASE), 3}
#endif
};
#else // !CONFIG_DISABLE_HAL_LOCKS
#define SPI_MUTEX_LOCK() \
  do                     \
  {                      \
  } while (xSemaphoreTake(_spi->lock, portMAX_DELAY) != pdPASS)
#define SPI_MUTEX_UNLOCK() xSemaphoreGive(_spi->lock)

static spi_t _spi_bus_array[] = {
#if CONFIG_IDF_TARGET_ESP32S2
    {(volatile spi_dev_t *)(DR_REG_SPI1_BASE), NULL, 0},
    {(volatile spi_dev_t *)(DR_REG_SPI2_BASE), NULL, 1},
    {(volatile spi_dev_t *)(DR_REG_SPI3_BASE), NULL, 2}
#elif CONFIG_IDF_TARGET_ESP32S3
    {(volatile spi_dev_t *)(DR_REG_SPI2_BASE), NULL, 0},
    {(volatile spi_dev_t *)(DR_REG_SPI3_BASE), NULL, 1}
#elif CONFIG_IDF_TARGET_ESP32C3
    {(volatile spi_dev_t *)(&GPSPI2), NULL, FSPI}
#else
    {(volatile spi_dev_t *)(DR_REG_SPI0_BASE), NULL, 0},
    {(volatile spi_dev_t *)(DR_REG_SPI1_BASE), NULL, 1},
    {(volatile spi_dev_t *)(DR_REG_SPI2_BASE), NULL, 2},
    {(volatile spi_dev_t *)(DR_REG_SPI3_BASE), NULL, 3}
#endif
};
#endif // CONFIG_DISABLE_HAL_LOCKS

/**
 * @brief Arduino_ESP32SPI
 *
 */
Arduino_ESP32SPI::Arduino_ESP32SPI(
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

static void _on_apb_change(void *arg, apb_change_ev_t ev_type, uint32_t old_apb, uint32_t new_apb)
{
  spi_t *_spi = (spi_t *)arg;
  if (ev_type == APB_BEFORE_CHANGE)
  {
    SPI_MUTEX_LOCK();
    while (_spi->dev->cmd.usr)
      ;
  }
  else
  {
    _spi->dev->clock.val = spiFrequencyToClockDiv(old_apb / ((_spi->dev->clock.clkdiv_pre + 1) * (_spi->dev->clock.clkcnt_n + 1)));
    SPI_MUTEX_UNLOCK();
  }
}

static void spiInitBus(spi_t *spi)
{
#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32
  spi->dev->slave.trans_done = 0;
#endif
  spi->dev->slave.val = 0;
#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3
  spi->dev->misc.val = 0;
#else
  spi->dev->pin.val = 0;
#endif
  spi->dev->user.val = 0;
  spi->dev->user1.val = 0;
  spi->dev->ctrl.val = 0;
#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32
  spi->dev->ctrl1.val = 0;
  spi->dev->ctrl2.val = 0;
#else
  spi->dev->clk_gate.val = 0;
  spi->dev->dma_conf.val = 0;
  spi->dev->dma_conf.rx_afifo_rst = 1;
  spi->dev->dma_conf.buf_afifo_rst = 1;
#endif
  spi->dev->clock.val = 0;
}

/**
 * @brief begin
 *
 * @param speed
 * @param dataMode
 * @return true
 * @return false
 */
bool Arduino_ESP32SPI::begin(int32_t speed, int8_t dataMode)
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

  // SPI.begin(_sck, _miso, _mosi);
  // _spi = spiStartBus(_spi_num, _div, SPI_MODE0, SPI_MSBFIRST);
  _spi = &_spi_bus_array[_spi_num];

#if !CONFIG_DISABLE_HAL_LOCKS
  if (_spi->lock == NULL)
  {
    _spi->lock = xSemaphoreCreateMutex();
  }
#endif

#if CONFIG_IDF_TARGET_ESP32S2
  if (_spi_num == FSPI)
  {
    DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_SPI2_CLK_EN);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI2_RST);
  }
  else if (_spi_num == HSPI)
  {
    DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_SPI3_CLK_EN);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI3_RST);
  }
  else
  {
    DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_SPI01_CLK_EN);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI01_RST);
  }
#elif CONFIG_IDF_TARGET_ESP32S3
  if (_spi_num == FSPI)
  {
    periph_module_reset(PERIPH_SPI2_MODULE);
    periph_module_enable(PERIPH_SPI2_MODULE);
  }
  else if (_spi_num == HSPI)
  {
    periph_module_reset(PERIPH_SPI3_MODULE);
    periph_module_enable(PERIPH_SPI3_MODULE);
  }
#elif CONFIG_IDF_TARGET_ESP32
  if (_spi_num == HSPI)
  {
    DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_SPI2_CLK_EN);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI2_RST);
  }
  else if (_spi_num == VSPI)
  {
    DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_SPI3_CLK_EN);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI3_RST);
  }
  else
  {
    DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_SPI01_CLK_EN);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI01_RST);
  }
#elif CONFIG_IDF_TARGET_ESP32C3
  periph_module_reset(PERIPH_SPI2_MODULE);
  periph_module_enable(PERIPH_SPI2_MODULE);
#endif

  SPI_MUTEX_LOCK();
  spiInitBus(_spi);
#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
  _spi->dev->clk_gate.clk_en = 1;
  _spi->dev->clk_gate.mst_clk_sel = 1;
  _spi->dev->clk_gate.mst_clk_active = 1;
  _spi->dev->dma_conf.tx_seg_trans_clr_en = 1;
  _spi->dev->dma_conf.rx_seg_trans_clr_en = 1;
  _spi->dev->dma_conf.dma_seg_trans_en = 0;
#endif
  _spi->dev->user.usr_mosi = 1;
  if (_miso == GFX_NOT_DEFINED)
  {
    _spi->dev->user.usr_miso = 0;
    _spi->dev->user.doutdin = 0;
  }
  else
  {
    _spi->dev->user.usr_miso = 1;
    _spi->dev->user.doutdin = 1;
  }

  for (uint8_t i = 0; i < 16; i++)
  {
    _spi->dev->data_buf[i] = 0x00000000;
  }
  SPI_MUTEX_UNLOCK();

  spiSetDataMode(_spi, _dataMode);
  spiSetBitOrder(_spi, _bitOrder);
  spiSetClockDiv(_spi, _div);

  addApbChangeCallback(_spi, _on_apb_change);

  spiAttachSCK(_spi, _sck);

  if (_miso != GFX_NOT_DEFINED)
  {
    spiAttachMISO(_spi, _miso);
  }

  spiAttachMOSI(_spi, _mosi);

  if (!_is_shared_interface)
  {
    spiTransaction(_spi, _div, _dataMode, _bitOrder);
  }

  return true;
}

/**
 * @brief beginWrite
 *
 */
void Arduino_ESP32SPI::beginWrite()
{
  _data_buf_bit_idx = 0;
  _buffer[0] = 0;

  if (_is_shared_interface)
  {
    spiTransaction(_spi, _div, _dataMode, _bitOrder);
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
void Arduino_ESP32SPI::endWrite()
{
  if (_data_buf_bit_idx > 0)
  {
    flush_data_buf();
  }

  if (_is_shared_interface)
  {
    spiEndTransaction(_spi);
  }

  CS_HIGH();
}

/**
 * @brief writeCommand
 *
 * @param c
 */
void Arduino_ESP32SPI::writeCommand(uint8_t c)
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

    _spi->dev->data_buf[0] = c;
    POLL(7);

    DC_HIGH();
  }
}

/**
 * @brief writeCommand16
 *
 * @param c
 */
void Arduino_ESP32SPI::writeCommand16(uint16_t c)
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

    MSB_16_SET(_spi->dev->data_buf[0], c);
    POLL(15);

    DC_HIGH();
  }
}

/**
 * @brief write
 *
 * @param d
 */
void Arduino_ESP32SPI::write(uint8_t d)
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
void Arduino_ESP32SPI::write16(uint16_t d)
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
void Arduino_ESP32SPI::writeC8D8(uint8_t c, uint8_t d)
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

    _spi->dev->data_buf[0] = c;
    POLL(7);

    DC_HIGH();

    _spi->dev->data_buf[0] = d;
    POLL(7);
  }
}

/**
 * @brief writeC8D16
 *
 * @param c
 * @param d
 */
void Arduino_ESP32SPI::writeC8D16(uint8_t c, uint16_t d)
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

    _spi->dev->data_buf[0] = c;
    POLL(7);

    DC_HIGH();

    MSB_16_SET(_spi->dev->data_buf[0], d);
    POLL(15);
  }
}

/**
 * @brief writeC8D16D16
 *
 * @param c
 * @param d1
 * @param d2
 */
void Arduino_ESP32SPI::writeC8D16D16(uint8_t c, uint16_t d1, uint16_t d2)
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

    _spi->dev->data_buf[0] = c;
    POLL(7);

    DC_HIGH();

    MSB_32_16_16_SET(_spi->dev->data_buf[0], d1, d2);
    POLL(31);
  }
}

/**
 * @brief writeRepeat
 *
 * @param p
 * @param len
 */
void Arduino_ESP32SPI::writeRepeat(uint16_t p, uint32_t len)
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

    if (_miso == GFX_NOT_DEFINED)
    {
      l = (_data_buf_bit_idx + 31) / 32;
      for (uint32_t i = 0; i < l; i++)
      {
        _spi->dev->data_buf[i] = _buffer32[i];
      }
    }

    // Issue pixels in blocks from temp buffer
    while (len) // While pixels remain
    {
      xferLen = (bufLen < len) ? bufLen : len; // How many this pass?
      _data_buf_bit_idx = xferLen * 18;

      if (_miso != GFX_NOT_DEFINED)
      {
        l = (_data_buf_bit_idx + 31) / 32;
        for (uint32_t i = 0; i < l; i++)
        {
          _spi->dev->data_buf[i] = _buffer32[i];
        }
      }
      POLL(_data_buf_bit_idx - 1);

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
    if (_miso == GFX_NOT_DEFINED)
    {
      l = (bufLen + 1) >> 1;
      for (uint32_t i = 0; i < l; i++)
      {
        _spi->dev->data_buf[i] = c32;
      }
    }

    // Issue pixels in blocks from temp buffer
    while (len) // While pixels remain
    {
      xferLen = (bufLen <= len) ? bufLen : len; // How many this pass?

      if (_miso != GFX_NOT_DEFINED)
      {
        l = (xferLen + 1) >> 1;
        for (uint32_t i = 0; i < l; i++)
        {
          _spi->dev->data_buf[i] = c32;
        }
      }
      POLL((xferLen << 4) - 1);

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
void Arduino_ESP32SPI::writePixels(uint16_t *data, uint32_t len)
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
    uint32_t *p32 = (uint32_t *)_spi->dev->data_buf;
    while (len)
    {
      l = (len > SPI_MAX_PIXELS_AT_ONCE) ? SPI_MAX_PIXELS_AT_ONCE : len;
      l2 = (l + 1) >> 1;
      for (uint32_t i = 0; i < l2; ++i)
      {
        p1 = *data++;
        p2 = *data++;
        MSB_32_16_16_SET(p32[i], p1, p2);
      }
      POLL((l << 4) - 1);

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
void Arduino_ESP32SPI::writeBytes(uint8_t *data, uint32_t len)
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
    if (_data_buf_bit_idx > 0)
    {
      flush_data_buf();
    }

    uint32_t l, l4;
    uint32_t *p;
    uint32_t *buf32 = (uint32_t *)_spi->dev->data_buf;
    while (len)
    {
      l = (len > (SPI_MAX_PIXELS_AT_ONCE << 1)) ? (SPI_MAX_PIXELS_AT_ONCE << 1) : len;
      l4 = (l + 3) >> 2;
      p = (uint32_t *)data;
      for (uint32_t i = 0; i < l4; ++i)
      {
        buf32[i] = *p++;
      }
      POLL((l << 3) - 1);

      len -= l;
      data += l;
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
void Arduino_ESP32SPI::writeIndexedPixels(uint8_t *data, uint16_t *idx, uint32_t len)
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
        MSB_32_16_16_SET(_spi->dev->data_buf[i], p1, p2);
      }
      if (l & 1)
      {
        p1 = idx[*data++];
        MSB_16_SET(_spi->dev->data_buf[l - 1], p1);
      }
      POLL((l << 4) - 1);

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
void Arduino_ESP32SPI::writeIndexedPixelsDouble(uint8_t *data, uint16_t *idx, uint32_t len)
{
  uint16_t p;
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
        MSB_32_16_16_SET(_spi->dev->data_buf[i], p, p);
      }
      POLL((l << 5) - 1);

      len -= l;
    }
  }
}

/**
 * @brief flush_data_buf
 *
 */
void Arduino_ESP32SPI::flush_data_buf()
{
  uint32_t len = (_data_buf_bit_idx + 31) / 32;
  for (uint32_t i = 0; i < len; i++)
  {
    _spi->dev->data_buf[i] = _buffer32[i];
  }
  POLL(_data_buf_bit_idx - 1);

  _data_buf_bit_idx = 0;
}

/**
 * @brief WRITE8BIT
 *
 * @param d
 * @return INLINE
 */
INLINE void Arduino_ESP32SPI::WRITE8BIT(uint8_t d)
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
INLINE void Arduino_ESP32SPI::WRITE9BIT(uint32_t d)
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
INLINE void Arduino_ESP32SPI::DC_HIGH(void)
{
  *_dcPortSet = _dcPinMask;
}

/**
 * @brief DC_LOW
 *
 * @return INLINE
 */
INLINE void Arduino_ESP32SPI::DC_LOW(void)
{
  *_dcPortClr = _dcPinMask;
}

/**
 * @brief CS_HIGH
 *
 * @return INLINE
 */
INLINE void Arduino_ESP32SPI::CS_HIGH(void)
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
INLINE void Arduino_ESP32SPI::CS_LOW(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
    *_csPortClr = _csPinMask;
  }
}

/**
 * @brief POLL
 *
 * @return INLINE
 */
INLINE void Arduino_ESP32SPI::POLL(uint32_t len)
{
  #if (CONFIG_IDF_TARGET_ESP32)
_spi->dev->mosi_dlen.usr_mosi_dbitlen = len;
#elif (CONFIG_IDF_TARGET_ESP32S2)
_spi->dev->mosi_dlen.usr_mosi_bit_len = len;
#elif (CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3)
_spi->dev->ms_dlen.ms_data_bitlen = len;
#endif

#if (CONFIG_IDF_TARGET_ESP32)
_spi->dev->miso_dlen.usr_miso_dbitlen = 0;
#elif (CONFIG_IDF_TARGET_ESP32S2)
_spi->dev->miso_dlen.usr_miso_bit_len = 0;
#endif

#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
  _spi->dev->cmd.update = 1;
  while (_spi->dev->cmd.update)
    ;
#endif
  _spi->dev->cmd.usr = 1;
  while (_spi->dev->cmd.usr)
    ;
}

#endif // #if defined(ESP32)
