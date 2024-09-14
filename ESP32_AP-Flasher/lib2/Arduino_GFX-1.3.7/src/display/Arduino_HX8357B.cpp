#include "Arduino_HX8357B.h"

Arduino_HX8357B::Arduino_HX8357B(Arduino_DataBus *bus, int8_t rst, uint8_t r, bool ips /* = false */)
    : Arduino_TFT(bus, rst, r, ips, HX8357B_TFTWIDTH, HX8357B_TFTHEIGHT, 0, 0, 0, 0)
{
}

bool Arduino_HX8357B::begin(int32_t speed)
{
  return Arduino_TFT::begin(speed);
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_HX8357B::setRotation(uint8_t r)
{
  Arduino_TFT::setRotation(r);
  switch (_rotation)
  {
  case 3:
    r = (HX8357B_MADCTL_MX | HX8357B_MADCTL_MY | HX8357B_MADCTL_MV | HX8357B_MADCTL_BGR);
    break;
  case 2:
    r = (HX8357B_MADCTL_MY | HX8357B_MADCTL_BGR);
    break;
  case 1:
    r = (HX8357B_MADCTL_MV | HX8357B_MADCTL_BGR);
    break;
  default: // case 0:
    r = (HX8357B_MADCTL_MX | HX8357B_MADCTL_BGR);
    break;
  }
  _bus->beginWrite();
  _bus->writeC8D8(HX8357B_SET_ADDRESS_MODE, r);
  _bus->endWrite();
}

void Arduino_HX8357B::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  if ((x != _currentX) || (w != _currentW))
  {
    _currentX = x;
    _currentW = w;
    x += _xStart;
    _bus->writeC8D16D16(HX8357B_SET_COLUMN_ADDRESS, x, x + w - 1);
  }

  if ((y != _currentY) || (h != _currentH))
  {
    _currentY = y;
    _currentH = h;
    y += _yStart;
    _bus->writeC8D16D16(HX8357B_SET_PAGE_ADDRESS, y, y + h - 1);
  }

  _bus->writeCommand(HX8357B_WRITE_MEMORY_START); // write to RAM
}

void Arduino_HX8357B::invertDisplay(bool i)
{
  _bus->sendCommand((_ips ^ i) ? HX8357B_ENTER_INVERSION_MODE : HX8357B_EXIT_INVERSION_MODE);
}

void Arduino_HX8357B::displayOn(void)
{
  _bus->sendCommand(HX8357B_EXIT_SLEEP_MODE);
  delay(HX8357B_SLPOUT_DELAY);
}

void Arduino_HX8357B::displayOff(void)
{
  _bus->sendCommand(HX8357B_ENTER_SLEEP_MODE);
  delay(HX8357B_SLPIN_DELAY);
}

void Arduino_HX8357B::tftInit()
{
  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(HX8357B_RST_DELAY);
    digitalWrite(_rst, HIGH);
    delay(HX8357B_RST_DELAY);
  }
  else
  {
    // Software Rest
    _bus->sendCommand(HX8357B_SOFTWARE_RESET);
    delay(HX8357B_RST_DELAY);
  }

  _bus->batchOperation(hx8357b_init_operations, sizeof(hx8357b_init_operations));

  invertDisplay(false);
}
