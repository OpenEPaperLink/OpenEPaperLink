/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 * https://github.com/gitcnd/LCDWIKI_SPI.git
 */
#include "Arduino_JBT6K71.h"
#include "SPI.h"

Arduino_JBT6K71::Arduino_JBT6K71(
    Arduino_DataBus *bus, int8_t rst, uint8_t r, bool ips, int16_t w, int16_t h,
    uint8_t col_offset1, uint8_t row_offset1, uint8_t col_offset2, uint8_t row_offset2)
    : Arduino_TFT(bus, rst, r, ips, w, h, col_offset1, row_offset1, col_offset2, row_offset2)
{
}

bool Arduino_JBT6K71::begin(int32_t speed)
{
  return Arduino_TFT::begin(speed);
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Arduino_JBT6K71::tftInit()
{
  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(JBT6K71_RST_DELAY);
    digitalWrite(_rst, HIGH);
    delay(JBT6K71_RST_DELAY);
  }
  else
  {
    // Software Rest
  }

  _bus->batchOperation(jbt6k71_init_operations, sizeof(jbt6k71_init_operations));

  invertDisplay(false);
}

void Arduino_JBT6K71::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  uint16_t cmd1, cmd2, cmd3;

  if ((x != _currentX) || (w != _currentW))
  {
    int16_t x_start, x_end, x_pos;

    if (_rotation & 0x01) // Landscape
    {
      cmd1 = 0x0408;
      cmd2 = 0x0409;
      cmd3 = 0x0201;
    }
    else
    {
      cmd1 = 0x0406;
      cmd2 = 0x0407;
      cmd3 = 0x0200;
    }
    if (_rotation == 1)
    {
      x_start = JBT6K71_TFTHEIGHT - x - w - _xStart;
      x_end = JBT6K71_TFTHEIGHT - x - 1 - _xStart;
      x_pos = x_end;
    }
    else
    {
      x_start = x + _xStart;
      x_end = x + w - 1 + _xStart;
      x_pos = x_start;
    }
    _bus->writeCommand16(cmd1);
    _bus->write16(x_start);
    _bus->writeCommand16(cmd2);
    _bus->write16(x_end);
    _bus->writeCommand16(cmd3);
    _bus->write16(x_pos);

    _currentX = x;
    _currentW = w;
  }
  if ((y != _currentY) || (h != _currentH))
  {
    int16_t y_start, y_end, y_pos;

    if (_rotation & 0x01) // Portrait
    {
      cmd1 = 0x0406;
      cmd2 = 0x0407;
      cmd3 = 0x0200;
    }
    else
    {
      cmd1 = 0x0408;
      cmd2 = 0x0409;
      cmd3 = 0x0201;
    }
    if (_rotation == 0)
    {
      y_start = JBT6K71_TFTHEIGHT - y - h - _yStart;
      y_end = JBT6K71_TFTHEIGHT - y - 1 - _yStart;
      y_pos = y_end;
    }
    else
    {
      y_start = y + _yStart;
      y_end = y + h - 1 + _yStart;
      y_pos = y_start;
    }
    _bus->writeCommand16(cmd1);
    _bus->write16(y_start);
    _bus->writeCommand16(cmd2);
    _bus->write16(y_end);
    _bus->writeCommand16(cmd3);
    _bus->write16(y_pos);

    _currentY = y;
    _currentH = h;
  }

  _bus->writeCommand16(0x0202); // write to RAM
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_JBT6K71::setRotation(uint8_t r)
{
  Arduino_TFT::setRotation(r);
  uint16_t output_control, entry_mode;
  switch (_rotation)
  {
  case 1:
    output_control = 0x0127; // SS=1
    entry_mode = 0x0018;
    break;
  case 2:
    output_control = 0x0127; // SS=1
    entry_mode = 0x0030;
    break;
  case 3:
    output_control = 0x0027; // SS=0
    entry_mode = 0x0038;
    break;
  default:                   // case 0:
    output_control = 0x0027; // SS=0
    entry_mode = 0x0010;     // ID=01
    break;
  }
  _bus->beginWrite();
  _bus->writeCommand16(0x0001); // Driver output control
  _bus->write16(output_control);
  _bus->writeCommand16(0x0003); // Entry mode
  _bus->write16(entry_mode);
  _bus->endWrite();
}

void Arduino_JBT6K71::invertDisplay(bool i)
{
  _bus->beginWrite();
  _bus->writeCommand16(0x0007); // Display mode
  _bus->write16((_ips ^ i) ? 0x4004 : 0x4000);
  _bus->endWrite();
}

void Arduino_JBT6K71::displayOn(void)
{
  // Not Implemented
}

void Arduino_JBT6K71::displayOff(void)
{
  // Not Implemented
}
