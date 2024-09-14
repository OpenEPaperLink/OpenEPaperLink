#include "Arduino_NV3041A.h"
#include "SPI.h"

Arduino_NV3041A::Arduino_NV3041A(
    Arduino_DataBus *bus, int8_t rst, uint8_t r,
    bool ips, int16_t w, int16_t h,
    uint8_t col_offset1, uint8_t row_offset1, uint8_t col_offset2, uint8_t row_offset2)
    : Arduino_TFT(bus, rst, r, ips, w, h, col_offset1, row_offset1, col_offset2, row_offset2)
{
}

bool Arduino_NV3041A::begin(int32_t speed)
{
  return Arduino_TFT::begin(speed);
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_NV3041A::setRotation(uint8_t r)
{
  Arduino_TFT::setRotation(r);
  switch (_rotation)
  {
  case 1:
    r = NV3041A_MADCTL_MY | NV3041A_MADCTL_MV | NV3041A_MADCTL_RGB;
    break;
  case 2:
    r = NV3041A_MADCTL_RGB;
    break;
  case 3:
    r = NV3041A_MADCTL_MX | NV3041A_MADCTL_MV | NV3041A_MADCTL_RGB;
    break;
  default: // case 0:
    r = NV3041A_MADCTL_MX | NV3041A_MADCTL_MY | NV3041A_MADCTL_RGB;
    break;
  }
  _bus->beginWrite();
  _bus->writeCommand(NV3041A_MADCTL);
  _bus->write(r);
  _bus->endWrite();
}

void Arduino_NV3041A::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  if ((x != _currentX) || (w != _currentW))
  {
    _currentX = x;
    _currentW = w;
    x += _xStart;
    _bus->writeC8D16D16Split(NV3041A_CASET, x, x + w - 1);
  }

  if ((y != _currentY) || (h != _currentH))
  {
    _currentY = y;
    _currentH = h;
    y += _yStart;
    _bus->writeC8D16D16Split(NV3041A_RASET, y, y + h - 1);
  }

  _bus->writeCommand(NV3041A_RAMWR); // write to RAM
}

void Arduino_NV3041A::invertDisplay(bool i)
{
  _bus->sendCommand((_ips ^ i) ? NV3041A_INVON : NV3041A_INVOFF);
}

void Arduino_NV3041A::displayOn(void)
{
  _bus->sendCommand(NV3041A_SLPOUT);
  delay(NV3041A_SLPOUT_DELAY);
}

void Arduino_NV3041A::displayOff(void)
{
  _bus->sendCommand(NV3041A_SLPIN);
  delay(NV3041A_SLPIN_DELAY);
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Arduino_NV3041A::tftInit()
{
  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(NV3041A_RST_DELAY);
    digitalWrite(_rst, HIGH);
    delay(NV3041A_RST_DELAY);
  }
  else
  {
    // Software Rest
    _bus->sendCommand(NV3041A_SWRESET);
    delay(NV3041A_RST_DELAY);
  }

  _bus->batchOperation(nv3041a_init_operations, sizeof(nv3041a_init_operations));

  invertDisplay(false);
}
