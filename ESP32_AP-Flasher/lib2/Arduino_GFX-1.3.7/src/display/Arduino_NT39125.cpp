/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 * https://github.com/daumemo/IPS_LCD_NT39125_FT6236_Arduino_eSPI_Test
 */
#include "Arduino_NT39125.h"

Arduino_NT39125::Arduino_NT39125(
    Arduino_DataBus *bus, int8_t rst, uint8_t r,
    bool ips, int16_t w, int16_t h,
    uint8_t col_offset1, uint8_t row_offset1, uint8_t col_offset2, uint8_t row_offset2)
    : Arduino_TFT(bus, rst, r, ips, w, h, col_offset1, row_offset1, col_offset2, row_offset2)
{
}

bool Arduino_NT39125::begin(int32_t speed)
{
  return Arduino_TFT::begin(speed);
}

void Arduino_NT39125::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  if ((x != _currentX) || (w != _currentW))
  {
    _bus->writeC8D16D16(NT39125_CASET, x + _xStart, x + w - 1 + _xStart);

    _currentX = x;
    _currentW = w;
  }
  if ((y != _currentY) || (h != _currentH))
  {
    _bus->writeC8D16D16(NT39125_RASET, y + _yStart, y + h - 1 + _yStart);

    _currentY = y;
    _currentH = h;
  }

  _bus->writeCommand(NT39125_RAMWR); // write to RAM
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_NT39125::setRotation(uint8_t r)
{
  Arduino_TFT::setRotation(r);
  switch (_rotation)
  {
  case 3:
    r = NT39125_MADCTL_BGR | NT39125_MADCTL_MV | NT39125_MADCTL_MX | NT39125_MADCTL_MY;
    break;
  case 2:
    r = NT39125_MADCTL_BGR | NT39125_MADCTL_MY;
    break;
  case 1:
    r = NT39125_MADCTL_BGR | NT39125_MADCTL_MV;
    break;
  default: // case 0:
    r = NT39125_MADCTL_BGR | NT39125_MADCTL_MX;
    break;
  }
  _bus->beginWrite();
  _bus->writeCommand(NT39125_MADCTL);
  _bus->write(r);
  _bus->endWrite();
}

void Arduino_NT39125::invertDisplay(bool i)
{
  _bus->sendCommand((_ips ^ i) ? NT39125_INVON : NT39125_INVOFF);
}

void Arduino_NT39125::displayOn(void)
{
  _bus->sendCommand(NT39125_SLPOUT);
  delay(NT39125_SLPOUT_DELAY);
}

void Arduino_NT39125::displayOff(void)
{
  _bus->sendCommand(NT39125_SLPIN);
  delay(NT39125_SLPIN_DELAY);
}

void Arduino_NT39125::tftInit()
{
  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(NT39125_RST_DELAY);
    digitalWrite(_rst, HIGH);
    delay(NT39125_RST_DELAY);
  }
  else
  {
    // Software Rest
    _bus->sendCommand(NT39125_SWRESET);
    delay(NT39125_RST_DELAY);
  }

  _bus->batchOperation(nt39125_init_operations, sizeof(nt39125_init_operations));

  invertDisplay(false);
}
