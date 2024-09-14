/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 * https://github.com/adafruit/Adafruit_ILI9342.git
 */
#include "Arduino_ILI9342.h"
#include "SPI.h"

Arduino_ILI9342::Arduino_ILI9342(Arduino_DataBus *bus, int8_t rst, uint8_t r, bool ips)
    : Arduino_TFT(bus, rst, r, ips, ILI9342_TFTWIDTH, ILI9342_TFTHEIGHT, 0, 0, 0, 0)
{
}

bool Arduino_ILI9342::begin(int32_t speed)
{
  _override_datamode = SPI_MODE0; // always use SPI_MODE0

  return Arduino_TFT::begin(speed);
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_ILI9342::setRotation(uint8_t r)
{
  Arduino_TFT::setRotation(r);
  switch (_rotation)
  {
  case 1:
    r = (ILI9342_MADCTL_MV | ILI9342_MADCTL_MY | ILI9342_MADCTL_BGR);
    break;
  case 2:
    r = (ILI9342_MADCTL_BGR);
    break;
  case 3:
    r = (ILI9342_MADCTL_MV | ILI9342_MADCTL_MX | ILI9342_MADCTL_BGR);
    break;
  default: // case 0:
    r = (ILI9342_MADCTL_MX | ILI9342_MADCTL_MY | ILI9342_MADCTL_BGR);
    break;
  }
  _bus->beginWrite();
  _bus->writeC8D8(ILI9342_MADCTL, r);
  _bus->endWrite();
}

void Arduino_ILI9342::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  if ((x != _currentX) || (w != _currentW))
  {
    _currentX = x;
    _currentW = w;
    x += _xStart;
    _bus->writeC8D16D16(ILI9342_CASET, x, x + w - 1);
  }

  if ((y != _currentY) || (h != _currentH))
  {
    _currentY = y;
    _currentH = h;
    y += _yStart;
    _bus->writeC8D16D16(ILI9342_PASET, y, y + h - 1);
  }

  _bus->writeCommand(ILI9342_RAMWR); // write to RAM
}

void Arduino_ILI9342::invertDisplay(bool i)
{
  _bus->sendCommand((_ips ^ i) ? ILI9342_INVON : ILI9342_INVOFF);
}

void Arduino_ILI9342::displayOn(void)
{
  _bus->sendCommand(ILI9342_SLPOUT);
  delay(ILI9342_SLPOUT_DELAY);
}

void Arduino_ILI9342::displayOff(void)
{
  _bus->sendCommand(ILI9342_SLPIN);
  delay(ILI9342_SLPIN_DELAY);
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Arduino_ILI9342::tftInit()
{
  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(ILI9342_RST_DELAY);
    digitalWrite(_rst, HIGH);
    delay(ILI9342_RST_DELAY);
  }
  else
  {
    // Software Rest
    _bus->sendCommand(ILI9342_SWRESET);
    delay(ILI9342_RST_DELAY);
  }

  _bus->batchOperation(ILI9342_init_operations, sizeof(ILI9342_init_operations));

  invertDisplay(false);
}
