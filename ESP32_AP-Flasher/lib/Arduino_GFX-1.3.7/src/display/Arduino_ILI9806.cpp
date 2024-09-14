#include "Arduino_ILI9806.h"
#include "SPI.h"

Arduino_ILI9806::Arduino_ILI9806(Arduino_DataBus *bus, int8_t rst, uint8_t r, bool ips)
    : Arduino_TFT(bus, rst, r, ips, ILI9806_TFTWIDTH, ILI9806_TFTHEIGHT, 0, 0, 0, 0)
{
}

bool Arduino_ILI9806::begin(int32_t speed)
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
void Arduino_ILI9806::setRotation(uint8_t r)
{
  Arduino_TFT::setRotation(r);
  switch (_rotation)
  {
  case 3:
    r = (ILI9806_MADCTL_MY | ILI9806_MADCTL_MV | ILI9806_MADCTL_RGB);
    break;
  case 2:
    r = (ILI9806_MADCTL_MX | ILI9806_MADCTL_MY | ILI9806_MADCTL_RGB);
    break;
  case 1:
    r = (ILI9806_MADCTL_MX | ILI9806_MADCTL_MV | ILI9806_MADCTL_RGB);
    break;
  default: // case 0:
    r = ILI9806_MADCTL_RGB;
    break;
  }
  _bus->beginWrite();
  _bus->writeC8D8(ILI9806_MADCTL, r);
  _bus->endWrite();
}

void Arduino_ILI9806::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  if ((x != _currentX) || (w != _currentW))
  {
    _currentX = x;
    _currentW = w;
    x += _xStart;
    _bus->writeC8D16D16Split(ILI9806_CASET, x, x + w - 1);
  }

  if ((y != _currentY) || (h != _currentH))
  {
    _currentY = y;
    _currentH = h;
    y += _yStart;
    _bus->writeC8D16D16Split(ILI9806_PASET, y, y + h - 1);
  }

  _bus->writeCommand(ILI9806_RAMWR); // write to RAM
}

void Arduino_ILI9806::invertDisplay(bool i)
{
  _bus->sendCommand((_ips ^ i) ? ILI9806_INVON : ILI9806_INVOFF);
}

void Arduino_ILI9806::displayOn(void)
{
  _bus->sendCommand(ILI9806_SLPOUT);
  delay(ILI9806_SLPOUT_DELAY);
  _bus->sendCommand(ILI9806_DISPON);
}

void Arduino_ILI9806::displayOff(void)
{
  _bus->sendCommand(ILI9806_DISPOFF);
  delay(10);
  _bus->sendCommand(ILI9806_SLPIN);
  delay(ILI9806_SLPIN_DELAY);
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Arduino_ILI9806::tftInit()
{
  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(1);
    digitalWrite(_rst, LOW);
    delay(10);
    digitalWrite(_rst, HIGH);
    delay(ILI9806_RST_DELAY);
  }
  else
  {
    // Software Rest
    _bus->sendCommand(ILI9806_SWRESET);
    delay(ILI9806_RST_DELAY);
  }

  _bus->batchOperation(ili9806_init_operations, sizeof(ili9806_init_operations));

  invertDisplay(false);
}
