/*
 * start rewrite from:
 * https://github.com/nopnop2002/esp-idf-parallel-tft
 */
#include "Arduino_ILI9486.h"

Arduino_ILI9486::Arduino_ILI9486(Arduino_DataBus *bus, int8_t rst, uint8_t r, bool ips)
    : Arduino_TFT(bus, rst, r, ips, ILI9486_TFTWIDTH, ILI9486_TFTHEIGHT, 0, 0, 0, 0)
{
}

bool Arduino_ILI9486::begin(int32_t speed)
{
  return Arduino_TFT::begin(speed);
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_ILI9486::setRotation(uint8_t r)
{
  Arduino_TFT::setRotation(r);
  switch (_rotation)
  {
  case 1:
    r = (ILI9486_MADCTL_MV | ILI9486_MADCTL_BGR);
    break;
  case 2:
    r = (ILI9486_MADCTL_MY | ILI9486_MADCTL_BGR);
    break;
  case 3:
    r = (ILI9486_MADCTL_MY | ILI9486_MADCTL_MX | ILI9486_MADCTL_MV | ILI9486_MADCTL_BGR);
    break;
  case 4:
    r = (ILI9486_MADCTL_BGR);
    break;
  case 5:
    r = (ILI9486_MADCTL_MY | ILI9486_MADCTL_MV | ILI9486_MADCTL_BGR);
    break;
  case 6:
    r = (ILI9486_MADCTL_MY | ILI9486_MADCTL_MX | ILI9486_MADCTL_BGR);
    break;
  case 7:
    r = (ILI9486_MADCTL_MX | ILI9486_MADCTL_MV | ILI9486_MADCTL_BGR);
    break;
  default: // case 0:
    r = (ILI9486_MADCTL_MX | ILI9486_MADCTL_BGR);
    break;
  }
  _bus->beginWrite();
  _bus->writeC8D8(ILI9486_MADCTL, r);
  _bus->endWrite();
}

void Arduino_ILI9486::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  if ((x != _currentX) || (w != _currentW))
  {
    _currentX = x;
    _currentW = w;
    x += _xStart;
    _bus->writeC8D16D16Split(ILI9486_CASET, x, x + w - 1);
  }
  if ((y != _currentY) || (h != _currentH))
  {
    _currentY = y;
    _currentH = h;
    y += _yStart;
    _bus->writeC8D16D16Split(ILI9486_PASET, y, y + h - 1);
  }

  _bus->writeCommand(ILI9486_RAMWR); // write to RAM
}

void Arduino_ILI9486::invertDisplay(bool i)
{
  _bus->sendCommand((_ips ^ i) ? ILI9486_INVON : ILI9486_INVOFF);
}

void Arduino_ILI9486::displayOn(void)
{
  _bus->sendCommand(ILI9486_SLPOUT);
  delay(ILI9486_SLPOUT_DELAY);
}

void Arduino_ILI9486::displayOff(void)
{
  _bus->sendCommand(ILI9486_SLPIN);
  delay(ILI9486_SLPIN_DELAY);
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Arduino_ILI9486::tftInit()
{
  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(ILI9486_RST_DELAY);
    digitalWrite(_rst, HIGH);
    delay(ILI9486_RST_DELAY);
  }

  _bus->batchOperation(ili9486_init_operations, sizeof(ili9486_init_operations));

  invertDisplay(false);
}
