#include "Arduino_GC9107.h"
#include "SPI.h"

Arduino_GC9107::Arduino_GC9107(
    Arduino_DataBus *bus, int8_t rst, uint8_t r,
    bool ips, int16_t w, int16_t h,
    uint8_t col_offset1, uint8_t row_offset1, uint8_t col_offset2, uint8_t row_offset2)
    : Arduino_TFT(bus, rst, r, ips, w, h, col_offset1, row_offset1, col_offset2, row_offset2)
{
}

bool Arduino_GC9107::begin(int32_t speed)
{
  _override_datamode = SPI_MODE0; // always use SPI_MODE0

  return Arduino_TFT::begin(speed);
}

void Arduino_GC9107::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  if ((x != _currentX) || (w != _currentW) || (y != _currentY) || (h != _currentH))
  {
    _bus->writeC8D16D16(GC9107_CASET, x + _xStart, x + w - 1 + _xStart);
    _bus->writeC8D16D16(GC9107_RASET, y + _yStart, y + h - 1 + _yStart);

    _currentX = x;
    _currentY = y;
    _currentW = w;
    _currentH = h;
  }

  _bus->writeCommand(GC9107_RAMWR); // write to RAM
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_GC9107::setRotation(uint8_t r)
{
  Arduino_TFT::setRotation(r);
  switch (_rotation)
  {
  case 1:
    r = GC9107_MADCTL_MY | GC9107_MADCTL_MV | GC9107_MADCTL_BGR;
    break;
  case 2:
    r = GC9107_MADCTL_BGR;
    break;
  case 3:
    r = GC9107_MADCTL_MX | GC9107_MADCTL_MV | GC9107_MADCTL_BGR;
    break;
  default: // case 0:
    r = GC9107_MADCTL_MY | GC9107_MADCTL_MX | GC9107_MADCTL_BGR;
    break;
  }
  _bus->beginWrite();
  _bus->writeCommand(GC9107_MADCTL);
  _bus->write(r);
  _bus->endWrite();
}

void Arduino_GC9107::invertDisplay(bool i)
{
  _bus->sendCommand(_ips ? (i ? GC9107_INVOFF : GC9107_INVON) : (i ? GC9107_INVON : GC9107_INVOFF));
}

void Arduino_GC9107::displayOn(void)
{
  _bus->sendCommand(GC9107_SLPOUT);
  delay(GC9107_SLPOUT_DELAY);
}

void Arduino_GC9107::displayOff(void)
{
  _bus->sendCommand(GC9107_SLPIN);
  delay(GC9107_SLPIN_DELAY);
}

void Arduino_GC9107::tftInit()
{
  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(GC9107_RST_DELAY);
    digitalWrite(_rst, HIGH);
    delay(GC9107_RST_DELAY);
  }
  else
  {
    // Software Rest
  }

  _bus->batchOperation(GC9107_init_operations, sizeof(GC9107_init_operations));

  invertDisplay(false);
}
