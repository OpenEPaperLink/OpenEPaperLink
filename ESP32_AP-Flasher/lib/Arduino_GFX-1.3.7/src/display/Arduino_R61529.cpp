/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 * https://github.com/daumemo/IPS_LCD_R61529_FT6236_Arduino_eSPI_Test
 */
#include "Arduino_R61529.h"

Arduino_R61529::Arduino_R61529(Arduino_DataBus *bus, int8_t rst, uint8_t r, bool ips)
    : Arduino_TFT(bus, rst, r, ips, R61529_TFTWIDTH, R61529_TFTHEIGHT, 0, 0, 0, 0)
{
}

bool Arduino_R61529::begin(int32_t speed)
{
  return Arduino_TFT::begin(speed);
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Arduino_R61529::tftInit()
{
  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(R61529_RST_DELAY);
    digitalWrite(_rst, HIGH);
    delay(R61529_RST_DELAY);
  }
  else
  {
    // Software Rest
    _bus->sendCommand(R61529_SWRESET);
    delay(R61529_RST_DELAY);
  }

  _bus->batchOperation(r61529_init_operations, sizeof(r61529_init_operations));
}

void Arduino_R61529::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  if ((x != _currentX) || (w != _currentW))
  {
    _bus->writeC8D16D16Split(R61529_CASET, x + _xStart, x + w - 1 + _xStart);

    _currentX = x;
    _currentW = w;
  }
  if ((y != _currentY) || (h != _currentH))
  {
    _bus->writeC8D16D16Split(R61529_PASET, y + _yStart, y + h - 1 + _yStart);

    _currentY = y;
    _currentH = h;
  }

  _bus->writeCommand(R61529_RAMWR); // write to RAM
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_R61529::setRotation(uint8_t r)
{
  Arduino_TFT::setRotation(r);
  switch (_rotation)
  {
  case 1:
    r = R61529_MADCTL_MV | R61529_MADCTL_MX | R61529_MADCTL_RGB;
    break;
  case 2:
    r = R61529_MADCTL_RGB | R61529_MADCTL_GS | R61529_MADCTL_MX;
    break;
  case 3:
    r = R61529_MADCTL_MV | R61529_MADCTL_RGB | R61529_MADCTL_GS;
    break;
  default: // case 0:
    r = R61529_MADCTL_RGB;
    break;
  }
  _bus->beginWrite();
  _bus->writeCommand(R61529_MADCTL);
  _bus->write(r);
  _bus->endWrite();
}

void Arduino_R61529::invertDisplay(bool i)
{
  _bus->sendCommand(i ? R61529_INVON : R61529_INVOFF);
}

void Arduino_R61529::displayOn(void)
{
  _bus->sendCommand(R61529_SLPOUT);
  delay(R61529_SLPOUT_DELAY);
}

void Arduino_R61529::displayOff(void)
{
  _bus->sendCommand(R61529_SLPIN);
  delay(R61529_SLPIN_DELAY);
}
