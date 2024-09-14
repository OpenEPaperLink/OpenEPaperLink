#include "Arduino_ILI9488_18bit.h"

Arduino_ILI9488_18bit::Arduino_ILI9488_18bit(Arduino_DataBus *bus, int8_t rst, uint8_t r, bool ips)
    : Arduino_TFT_18bit(bus, rst, r, ips, ILI9488_TFTWIDTH, ILI9488_TFTHEIGHT, 0, 0, 0, 0)
{
}

bool Arduino_ILI9488_18bit::begin(int32_t speed)
{
  return Arduino_TFT::begin(speed);
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_ILI9488_18bit::setRotation(uint8_t r)
{
  Arduino_TFT::setRotation(r);
  switch (_rotation)
  {
  case 1:
    r = (ILI9488_MADCTL_BGR | ILI9488_MADCTL_MV);
    break;
  case 2:
    r = (ILI9488_MADCTL_BGR | ILI9488_MADCTL_MY);
    break;
  case 3:
    r = (ILI9488_MADCTL_BGR | ILI9488_MADCTL_MV | ILI9488_MADCTL_MX | ILI9488_MADCTL_MY);
    break;
  default: // case 0:
    r = (ILI9488_MADCTL_BGR | ILI9488_MADCTL_MX);
    break;
  }
  _bus->beginWrite();
  _bus->writeC8D8(ILI9488_MADCTL, r);
  _bus->endWrite();
}

void Arduino_ILI9488_18bit::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  if ((x != _currentX) || (w != _currentW))
  {
    _currentX = x;
    _currentW = w;
    x += _xStart;
    _bus->writeC8D16D16(ILI9488_CASET, x, x + w - 1);
  }
  if ((y != _currentY) || (h != _currentH))
  {
    _currentY = y;
    _currentH = h;
    y += _yStart;
    _bus->writeC8D16D16(ILI9488_PASET, y, y + h - 1);
  }

  _bus->writeCommand(ILI9488_RAMWR); // write to RAM
}

void Arduino_ILI9488_18bit::invertDisplay(bool i)
{
  _bus->sendCommand((_ips ^ i) ? ILI9488_INVON : ILI9488_INVOFF);
}

void Arduino_ILI9488_18bit::displayOn(void)
{
  _bus->sendCommand(ILI9488_SLPOUT);
  delay(ILI9488_SLPOUT_DELAY);
}

void Arduino_ILI9488_18bit::displayOff(void)
{
  _bus->sendCommand(ILI9488_SLPIN);
  delay(ILI9488_SLPIN_DELAY);
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Arduino_ILI9488_18bit::tftInit()
{
  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(ILI9488_RST_DELAY);
    digitalWrite(_rst, HIGH);
    delay(ILI9488_RST_DELAY);
  }
  else
  {
    // Software Rest
    _bus->sendCommand(ILI9488_SWRESET);
    delay(ILI9488_RST_DELAY);
  }

  _bus->batchOperation(ili9488_init_operations, sizeof(ili9488_init_operations));

  _bus->beginWrite();
  _bus->writeC8D8(0x3A, 0x66); // Interface Pixel Format, 18 bit
  _bus->endWrite();

  invertDisplay(false);
}
