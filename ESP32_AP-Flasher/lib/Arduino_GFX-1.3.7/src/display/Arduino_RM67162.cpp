#include "Arduino_RM67162.h"
#include "SPI.h"

Arduino_RM67162::Arduino_RM67162(Arduino_DataBus *bus, int8_t rst, uint8_t r, bool ips)
    : Arduino_TFT(bus, rst, r, ips, RM67162_TFTWIDTH, RM67162_TFTHEIGHT, 0, 0, 0, 0)
{
}

bool Arduino_RM67162::begin(int32_t speed)
{
  return Arduino_TFT::begin(speed);
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_RM67162::setRotation(uint8_t r)
{
  Arduino_TFT::setRotation(r);
  switch (_rotation)
  {
  case 1:
    r = RM67162_MADCTL_MX | RM67162_MADCTL_MV | RM67162_MADCTL_RGB;
    break;
  case 2:
    r = RM67162_MADCTL_MX | RM67162_MADCTL_MY | RM67162_MADCTL_RGB;
    break;
  case 3:
    r = RM67162_MADCTL_MV | RM67162_MADCTL_MY | RM67162_MADCTL_RGB;
    break;
  default: // case 0:
    r = RM67162_MADCTL_RGB;
    break;
  }
  _bus->beginWrite();
  _bus->writeC8D8(RM67162_MADCTL, r);
  _bus->endWrite();
}

void Arduino_RM67162::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  if ((x != _currentX) || (w != _currentW))
  {
    _currentX = x;
    _currentW = w;
    x += _xStart;
    _bus->writeC8D16D16(RM67162_CASET, x, x + w - 1);
  }

  if ((y != _currentY) || (h != _currentH))
  {
    _currentY = y;
    _currentH = h;
    y += _yStart;
    _bus->writeC8D16D16(RM67162_PASET, y, y + h - 1);
  }

  _bus->writeCommand(RM67162_RAMWR); // write to RAM
}

void Arduino_RM67162::invertDisplay(bool i)
{
  _bus->sendCommand((_ips ^ i) ? RM67162_INVON : RM67162_INVOFF);
}

void Arduino_RM67162::displayOn(void)
{
  _bus->sendCommand(RM67162_SLPOUT);
  delay(RM67162_SLPOUT_DELAY);
}

void Arduino_RM67162::displayOff(void)
{
  _bus->sendCommand(RM67162_SLPIN);
  delay(RM67162_SLPIN_DELAY);
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Arduino_RM67162::tftInit()
{
  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(RM67162_RST_DELAY);
    digitalWrite(_rst, HIGH);
    delay(RM67162_RST_DELAY);
  }
  else
  {
    // Software Rest
    _bus->sendCommand(RM67162_SWRESET);
    delay(RM67162_RST_DELAY);
  }

  _bus->sendCommand(RM67162_SLPOUT); // Sleep Out
  delay(RM67162_SLPOUT_DELAY);
  _bus->beginWrite();
  _bus->writeC8D8(RM67162_PIXFMT, 0x55);     // Interface Pixel Format 16bit/pixel
  _bus->writeCommand(RM67162_DISPON);        // Display on
  _bus->writeC8D8(RM67162_BRIGHTNESS, 0xD0); // Write Display Brightness   MAX_VAL=0XFF
  _bus->endWrite();

  invertDisplay(false);
}
