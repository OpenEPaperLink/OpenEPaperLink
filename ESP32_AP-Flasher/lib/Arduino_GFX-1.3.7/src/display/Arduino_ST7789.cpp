/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 * https://github.com/ananevilya/Arduino-ST7789-Library.git
 */
#include "Arduino_ST7789.h"
#include "SPI.h"

Arduino_ST7789::Arduino_ST7789(
    Arduino_DataBus *bus, int8_t rst, uint8_t r,
    bool ips, int16_t w, int16_t h,
    uint8_t col_offset1, uint8_t row_offset1, uint8_t col_offset2, uint8_t row_offset2)
    : Arduino_TFT(bus, rst, r, ips, w, h, col_offset1, row_offset1, col_offset2, row_offset2)
{
}

bool Arduino_ST7789::begin(int32_t speed)
{
#if defined(ESP32) || defined(ARDUINO_ARCH_NRF52840)
  _override_datamode = SPI_MODE3;
#elif defined(ESP8266)
  _override_datamode = SPI_MODE2;
#endif

  return Arduino_TFT::begin(speed);
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_ST7789::setRotation(uint8_t r)
{
  Arduino_TFT::setRotation(r);
  switch (_rotation)
  {
  case 1:
    r = ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB;
    break;
  case 2:
    r = ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB;
    break;
  case 3:
    r = ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB;
    break;
  default: // case 0:
    r = ST7789_MADCTL_RGB;
    break;
  }
  _bus->beginWrite();
  _bus->writeCommand(ST7789_MADCTL);
  _bus->write(r);
  _bus->endWrite();
}

void Arduino_ST7789::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  if ((x != _currentX) || (w != _currentW))
  {
    _currentX = x;
    _currentW = w;
    x += _xStart;
    _bus->writeC8D16D16(ST7789_CASET, x, x + w - 1);
  }

  if ((y != _currentY) || (h != _currentH))
  {
    _currentY = y;
    _currentH = h;
    y += _yStart;
    _bus->writeC8D16D16(ST7789_RASET, y, y + h - 1);
  }

  _bus->writeCommand(ST7789_RAMWR); // write to RAM
}

void Arduino_ST7789::invertDisplay(bool i)
{
  _bus->sendCommand((_ips ^ i) ? ST7789_INVON : ST7789_INVOFF);
}

void Arduino_ST7789::displayOn(void)
{
  _bus->sendCommand(ST7789_SLPOUT);
  delay(ST7789_SLPOUT_DELAY);
}

void Arduino_ST7789::displayOff(void)
{
  _bus->sendCommand(ST7789_SLPIN);
  delay(ST7789_SLPIN_DELAY);
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Arduino_ST7789::tftInit()
{
  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(ST7789_RST_DELAY);
    digitalWrite(_rst, HIGH);
    delay(ST7789_RST_DELAY);
  }
  // else
  // {
  // Software Rest
  _bus->sendCommand(ST7789_SWRESET);
  delay(ST7789_RST_DELAY);
  // }

  _bus->batchOperation(st7789_init_operations, sizeof(st7789_init_operations));

  invertDisplay(false);
}
