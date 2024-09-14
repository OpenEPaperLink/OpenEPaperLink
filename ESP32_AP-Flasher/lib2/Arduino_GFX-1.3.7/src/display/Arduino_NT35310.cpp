/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 * https://github.com/daumemo/IPS_LCD_NT35310_FT6236_Arduino_eSPI_Test
 */
#include "Arduino_NT35310.h"

Arduino_NT35310::Arduino_NT35310(
    Arduino_DataBus *bus, int8_t rst, uint8_t r,
    bool ips, int16_t w, int16_t h,
    uint8_t col_offset1, uint8_t row_offset1, uint8_t col_offset2, uint8_t row_offset2)
    : Arduino_TFT(bus, rst, r, ips, w, h, col_offset1, row_offset1, col_offset2, row_offset2)
{
}

bool Arduino_NT35310::begin(int32_t speed)
{
  return Arduino_TFT::begin(speed);
}

void Arduino_NT35310::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  if ((x != _currentX) || (w != _currentW))
  {
    _bus->writeC8D16D16(NT35310_SET_HORIZONTAL_ADDRESS, x + _xStart, x + w - 1 + _xStart);

    _currentX = x;
    _currentW = w;
  }
  if ((y != _currentY) || (h != _currentH))
  {
    _bus->writeC8D16D16(NT35310_SET_VERTICAL_ADDRESS, y + _yStart, y + h - 1 + _yStart);

    _currentY = y;
    _currentH = h;
  }

  _bus->writeCommand(NT35310_WRITE_MEMORY_START); // write to RAM
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_NT35310::setRotation(uint8_t r)
{
  Arduino_TFT::setRotation(r);
  switch (_rotation)
  {
  case 3:
    r = NT35310_MADCTL_MY | NT35310_MADCTL_MV | NT35310_MADCTL_RGB;
    break;
  case 2:
    r = NT35310_MADCTL_MY | NT35310_MADCTL_MX | NT35310_MADCTL_RGB;
    break;
  case 1:
    r = NT35310_MADCTL_MX | NT35310_MADCTL_MV | NT35310_MADCTL_RGB;
    break;
  default: // case 0:
    r = NT35310_MADCTL_RGB;
    break;
  }
  _bus->beginWrite();
  _bus->writeCommand(NT35310_SET_ADDRESS_MODE);
  _bus->write(r);
  _bus->endWrite();
}

void Arduino_NT35310::invertDisplay(bool i)
{
  _bus->sendCommand((_ips ^ i) ? NT35310_ENTER_INVERT_MODE : NT35310_EXIT_INVERT_MODE);
}

void Arduino_NT35310::displayOn(void)
{
  _bus->sendCommand(NT35310_EXIT_SLEEP_MODE);
  delay(NT35310_SLPOUT_DELAY);
}

void Arduino_NT35310::displayOff(void)
{
  _bus->sendCommand(NT35310_ENTER_SLEEP_MODE);
  delay(NT35310_SLPIN_DELAY);
}

void Arduino_NT35310::tftInit()
{
  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(NT35310_RST_DELAY);
    digitalWrite(_rst, HIGH);
    delay(NT35310_RST_DELAY);
  }
  else
  {
    // Software Rest
    _bus->sendCommand(NT35310_SOFT_RESET);
    delay(NT35310_RST_DELAY);
  }

  _bus->batchOperation(nt35310_init_operations, sizeof(nt35310_init_operations));

  invertDisplay(false);
}
