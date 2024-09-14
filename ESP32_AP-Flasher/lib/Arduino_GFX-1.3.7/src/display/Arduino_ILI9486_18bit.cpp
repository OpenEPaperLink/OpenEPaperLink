/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 */
#include "Arduino_ILI9486_18bit.h"

Arduino_ILI9486_18bit::Arduino_ILI9486_18bit(Arduino_DataBus *bus, int8_t rst, uint8_t r, bool ips)
    : Arduino_TFT_18bit(bus, rst, r, ips, ILI9486_TFTWIDTH, ILI9486_TFTHEIGHT, 0, 0, 0, 0)
{
}

bool Arduino_ILI9486_18bit::begin(int32_t speed)
{
  return Arduino_TFT::begin(speed);
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Arduino_ILI9486_18bit::tftInit()
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
  else
  {
    // Software Rest
    _bus->sendCommand(ILI9486_SWRESET);
    delay(ILI9486_RST_DELAY);
  }

  _bus->sendCommand(ILI9486_SLPOUT); // Exit Sleep
  delay(ILI9486_SLPIN_DELAY);

  _bus->sendCommand(0x3A);
  _bus->sendData(0x66); // 18-bit color

  _bus->sendCommand(0xC2);
  _bus->sendData(0x44);

  _bus->sendCommand(0xC5);
  _bus->sendData(0x00);
  _bus->sendData(0x00);
  _bus->sendData(0x00);
  _bus->sendData(0x00);

  _bus->sendCommand(0xE0);
  _bus->sendData(0x0F);
  _bus->sendData(0x1F);
  _bus->sendData(0x1C);
  _bus->sendData(0x0C);
  _bus->sendData(0x0F);
  _bus->sendData(0x08);
  _bus->sendData(0x48);
  _bus->sendData(0x98);
  _bus->sendData(0x37);
  _bus->sendData(0x0A);
  _bus->sendData(0x13);
  _bus->sendData(0x04);
  _bus->sendData(0x11);
  _bus->sendData(0x0D);
  _bus->sendData(0x00);

  _bus->sendCommand(0xE1);
  _bus->sendData(0x0F);
  _bus->sendData(0x32);
  _bus->sendData(0x2E);
  _bus->sendData(0x0B);
  _bus->sendData(0x0D);
  _bus->sendData(0x05);
  _bus->sendData(0x47);
  _bus->sendData(0x75);
  _bus->sendData(0x37);
  _bus->sendData(0x06);
  _bus->sendData(0x10);
  _bus->sendData(0x03);
  _bus->sendData(0x24);
  _bus->sendData(0x20);
  _bus->sendData(0x00);

  _bus->sendCommand(ILI9486_DISPON); // Display on
  delay(25);

  invertDisplay(false);
}

void Arduino_ILI9486_18bit::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  if ((x != _currentX) || (w != _currentW))
  {
    int16_t x_start = x + _xStart, x_end = x + w - 1 + _xStart;

    _bus->writeCommand(ILI9486_CASET); // Column addr set
    _bus->write(x_start >> 8);
    _bus->write(x_start & 0xFF); // XSTART
    _bus->write(x_end >> 8);
    _bus->write(x_end & 0xFF); // XEND

    _currentX = x;
    _currentW = w;
  }
  if ((y != _currentY) || (h != _currentH))
  {
    int16_t y_start = y + _yStart, y_end = y + h - 1 + _yStart;

    _bus->writeCommand(ILI9486_PASET); // Row addr set
    _bus->write(y_start >> 8);
    _bus->write(y_start & 0xFF); // YSTART
    _bus->write(y_end >> 8);
    _bus->write(y_end & 0xFF); // YEND

    _currentY = y;
    _currentH = h;
  }

  _bus->writeCommand(ILI9486_RAMWR); // write to RAM
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_ILI9486_18bit::setRotation(uint8_t r)
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
  _bus->writeCommand(ILI9486_MADCTL);
  _bus->write(r);
  _bus->endWrite();
}

void Arduino_ILI9486_18bit::invertDisplay(bool i)
{
  _bus->sendCommand(i ? ILI9486_INVON : ILI9486_INVOFF);
}

void Arduino_ILI9486_18bit::displayOn(void)
{
  _bus->sendCommand(ILI9486_SLPOUT);
  delay(ILI9486_SLPOUT_DELAY);
}

void Arduino_ILI9486_18bit::displayOff(void)
{
  _bus->sendCommand(ILI9486_SLPIN);
  delay(ILI9486_SLPIN_DELAY);
}
