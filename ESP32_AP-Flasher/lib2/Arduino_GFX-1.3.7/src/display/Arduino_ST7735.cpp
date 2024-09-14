/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 */
#include "Arduino_ST7735.h"
#include "SPI.h"

Arduino_ST7735::Arduino_ST7735(
    Arduino_DataBus *bus, int8_t rst, uint8_t r,
    bool ips, int16_t w, int16_t h,
    uint8_t col_offset1, uint8_t row_offset1, uint8_t col_offset2, uint8_t row_offset2,
    bool bgr)
    : Arduino_TFT(bus, rst, r, ips, w, h, col_offset1, row_offset1, col_offset2, row_offset2), _bgr(bgr)
{
}

bool Arduino_ST7735::begin(int32_t speed)
{
#if defined(ESP8266) || defined(ESP32)
  if (speed == GFX_NOT_DEFINED)
  {
    speed = 27000000UL; // ST7735 Maximum supported speed
  }
// Teensy 4.x
#elif defined(__IMXRT1052__) || defined(__IMXRT1062__)
  if (speed == GFX_NOT_DEFINED)
  {
    speed = 27000000UL; // ST7735 Maximum supported speed
  }
#endif
  _override_datamode = SPI_MODE0; // always use SPI_MODE0

  return Arduino_TFT::begin(speed);
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Arduino_ST7735::tftInit()
{
  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(ST7735_RST_DELAY);
    digitalWrite(_rst, HIGH);
    delay(ST7735_RST_DELAY);
  }
  else
  {
    // Software Rest
    _bus->sendCommand(ST7735_SWRESET); // 1: Software reset
    delay(ST7735_RST_DELAY);
  }

  _bus->batchOperation(st7735_init_operations, sizeof(st7735_init_operations));

  invertDisplay(false);
}

void Arduino_ST7735::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  if ((x != _currentX) || (w != _currentW))
  {
    int16_t x_start = x + _xStart, x_end = x + w - 1 + _xStart;

    _bus->writeCommand(ST7735_CASET); // Column addr set
    _bus->write(x_start >> 8);
    _bus->write(x_start & 0xFF); // XSTART
    _bus->write(x_end >> 8);
    _bus->write(x_end & 0xFF); // XEND

    _currentX = x;
    _currentW = w;
  }
  if ((y != _currentY) || (h != _currentH))
  {
    int16_t y_start = y + _yStart, y_end = y + h - 1 + _yStart;

    _bus->writeCommand(ST7735_RASET); // Row addr set
    _bus->write(y_start >> 8);
    _bus->write(y_start & 0xFF); // YSTART
    _bus->write(y_end >> 8);
    _bus->write(y_end & 0xFF); // YEND

    _currentY = y;
    _currentH = h;
  }

  _bus->writeCommand(ST7735_RAMWR); // write to RAM
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_ST7735::setRotation(uint8_t r)
{
  Arduino_TFT::setRotation(r);
  switch (_rotation)
  {
  case 1:
    r = ST7735_MADCTL_MY | ST7735_MADCTL_MV | (_bgr ? ST7735_MADCTL_BGR : ST7735_MADCTL_RGB);
    break;
  case 2:
    r = (_bgr ? ST7735_MADCTL_BGR : ST7735_MADCTL_RGB);
    break;
  case 3:
    r = ST7735_MADCTL_MX | ST7735_MADCTL_MV | (_bgr ? ST7735_MADCTL_BGR : ST7735_MADCTL_RGB);
    break;
  default: // case 0:
    r = ST7735_MADCTL_MX | ST7735_MADCTL_MY | (_bgr ? ST7735_MADCTL_BGR : ST7735_MADCTL_RGB);
    break;
  }
  _bus->beginWrite();
  _bus->writeCommand(ST7735_MADCTL);
  _bus->write(r);
  _bus->endWrite();
}

void Arduino_ST7735::invertDisplay(bool i)
{
  _bus->sendCommand((_ips ^ i) ? ST7735_INVON : ST7735_INVOFF);
}

void Arduino_ST7735::displayOn(void)
{
  _bus->sendCommand(ST7735_SLPOUT);
  delay(ST7735_SLPOUT_DELAY);
}

void Arduino_ST7735::displayOff(void)
{
  _bus->sendCommand(ST7735_SLPIN);
  delay(ST7735_SLPIN_DELAY);
}
