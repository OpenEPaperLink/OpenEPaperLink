/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 */
#include "Arduino_HX8347D.h"
#include "SPI.h"

Arduino_HX8347D::Arduino_HX8347D(
    Arduino_DataBus *bus, int8_t rst, uint8_t r,
    bool ips, int16_t w, int16_t h,
    uint8_t col_offset1, uint8_t row_offset1, uint8_t col_offset2, uint8_t row_offset2)
    : Arduino_TFT(bus, rst, r, ips, w, h, col_offset1, row_offset1, col_offset2, row_offset2)
{
}

bool Arduino_HX8347D::begin(int32_t speed)
{
#if defined(__AVR__)
  _override_datamode = SPI_MODE0;
#endif

  return Arduino_TFT::begin(speed);
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Arduino_HX8347D::tftInit()
{
  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(HX8347D_RST_DELAY);
    digitalWrite(_rst, HIGH);
    delay(HX8347D_RST_DELAY);
  }
  else
  {
    // Software Rest
  }

  _bus->batchOperation(hx8347d_init_operations, sizeof(hx8347d_init_operations));

  invertDisplay(false);
}

void Arduino_HX8347D::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  if ((x != _currentX) || (w != _currentW))
  {
    int16_t x_start = x + _xStart, x_end = x + w - 1 + _xStart;
    _bus->writeC8D8(0x02, x_start >> 8);
    _bus->writeC8D8(0x03, x_start & 0xFF);
    _bus->writeC8D8(0x04, x_end >> 8);
    _bus->writeC8D8(0x05, x_end & 0xFF);

    _currentX = x;
    _currentW = w;
  }
  if ((y != _currentY) || (h != _currentH))
  {
    int16_t y_start = y + _yStart, y_end = y + h - 1 + _yStart;
    _bus->writeC8D8(0x06, y_start >> 8);
    _bus->writeC8D8(0x07, y_start & 0xFF);
    _bus->writeC8D8(0x08, y_end >> 8);
    _bus->writeC8D8(0x09, y_end & 0xFF);

    _currentY = y;
    _currentH = h;
  }

  _bus->writeCommand(0x22); // write to RAM
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_HX8347D::setRotation(uint8_t r)
{
  Arduino_TFT::setRotation(r);
  _bus->beginWrite();
  switch (_rotation)
  {
  case 1:
    _bus->writeC8D8(0x36, 0x03);
    _bus->writeC8D8(0x16, 0x20);
    break;
  case 2:
    _bus->writeC8D8(0x36, 0x07);
    _bus->writeC8D8(0x16, 0x00);
    break;
  case 3:
    _bus->writeC8D8(0x36, 0x07);
    _bus->writeC8D8(0x16, 0x60);
    break;
  default: // case 0:
    _bus->writeC8D8(0x36, 0x03);
    _bus->writeC8D8(0x16, 0x40);
    break;
  }
  _bus->endWrite();
}

void Arduino_HX8347D::invertDisplay(bool i)
{
  _bus->beginWrite();
  _bus->writeC8D8(HX8347D_DISPLAY_MODE_CONTROL, (_ips ^ i) ? HX8347D_INV_ON : HX8347D_INV_OFF);
  _bus->endWrite();
}

void Arduino_HX8347D::displayOn(void)
{
  _bus->sendCommand(0x28);
  _bus->sendData(0x3C); // GON=1, DTE=1, D=11
}

void Arduino_HX8347D::displayOff(void)
{
  _bus->sendCommand(0x28);
  _bus->sendData(0x34); // GON=1, DTE=1, D=01
  delay(40);
}
