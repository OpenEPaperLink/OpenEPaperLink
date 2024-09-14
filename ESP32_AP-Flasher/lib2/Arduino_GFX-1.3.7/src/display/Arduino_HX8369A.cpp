#include "Arduino_HX8369A.h"

Arduino_HX8369A::Arduino_HX8369A(
    Arduino_DataBus *bus, int8_t rst, uint8_t r,
    bool ips, int16_t w, int16_t h,
    uint8_t col_offset1, uint8_t row_offset1, uint8_t col_offset2, uint8_t row_offset2)
    : Arduino_TFT(bus, rst, r, ips, w, h, col_offset1, row_offset1, col_offset2, row_offset2)

{
}

bool Arduino_HX8369A::begin(int32_t speed)
{
  return Arduino_TFT::begin(speed);
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_HX8369A::setRotation(uint8_t r)
{
  Arduino_TFT::setRotation(r);
  switch (_rotation)
  {
  case 7:
    r = (HX8369A_MADCTL_MV);
    break;
  case 6:
    r = (HX8369A_MADCTL_MY);
    break;
  case 5:
    r = (HX8369A_MADCTL_MY | HX8369A_MADCTL_MX | HX8369A_MADCTL_MV);
    break;
  case 4:
    r = (HX8369A_MADCTL_MX);
    break;
  case 3:
    r = (HX8369A_MADCTL_MY | HX8369A_MADCTL_MV);
    break;
  case 2:
    r = (HX8369A_MADCTL_MY | HX8369A_MADCTL_MX);
    break;
  case 1:
    r = (HX8369A_MADCTL_MX | HX8369A_MADCTL_MV);
    break;
  default: // case 0:
    r = 0;
    break;
  }
  _bus->beginWrite();
  _bus->writeC8D8(HX8369A_SET_ADDRESS_MODE, r);
  _bus->endWrite();
}

void Arduino_HX8369A::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  if ((x != _currentX) || (w != _currentW))
  {
    _currentX = x;
    _currentW = w;
    x += _xStart;
    _bus->writeC8D16D16Split(HX8369A_SET_CLUMN_ADDRESS, x, x + w - 1);
  }

  if ((y != _currentY) || (h != _currentH))
  {
    _currentY = y;
    _currentH = h;
    y += _yStart;
    _bus->writeC8D16D16Split(HX8369A_SET_PAGE_ADDRESS, y, y + h - 1);
  }

  _bus->writeCommand(HX8369A_WRITE_MEMORY_START);
}

void Arduino_HX8369A::invertDisplay(bool i)
{
  _bus->sendCommand((_ips ^ i) ? HX8369A_ENTER_INVERSION_MODE : HX8369A_EXIT_INVERSION_MODE);
}

void Arduino_HX8369A::displayOn(void)
{
  _bus->sendCommand(HX8369A_EXIT_SLEEP_MODE);
  delay(120);
  _bus->sendCommand(HX8369A_SET_DISPLAY_ON);
}

void Arduino_HX8369A::displayOff(void)
{
  _bus->sendCommand(HX8369A_SET_DISPLAY_OFF);
  delay(120);
  _bus->sendCommand(HX8369A_ENTER_SLEEP_MODE);
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Arduino_HX8369A::tftInit()
{
  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(HX8369A_RST_DELAY);
    digitalWrite(_rst, HIGH);
    delay(HX8369A_RST_DELAY);
  }
  else
  {
    _bus->sendCommand(HX8369A_SWRESET);
    delay(HX8369A_RST_DELAY);
  }

  _bus->batchOperation(hx8369a_init_operations_part1, sizeof(hx8369a_init_operations_part1));
  for (int i = 0; i <= 63; i++)
  {
    _bus->write(i * 8);
  }

  for (int i = 0; i <= 63; i++)
  {
    _bus->write(i * 4);
  }

  for (int i = 0; i <= 63; i++)
  {
    _bus->write(i * 8);
  }
  _bus->batchOperation(hx8369a_init_operations_part2, sizeof(hx8369a_init_operations_part2));

  invertDisplay(false);
}
