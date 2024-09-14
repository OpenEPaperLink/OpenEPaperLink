/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 * https://github.com/lcdwiki/LCDWIKI_SPI
 */
#include "Arduino_ILI9225.h"

Arduino_ILI9225::Arduino_ILI9225(Arduino_DataBus *bus, int8_t rst, uint8_t r)
    : Arduino_TFT(bus, rst, r, false, ILI9225_TFTWIDTH, ILI9225_TFTHEIGHT, 0, 0, 0, 0)
{
}

bool Arduino_ILI9225::begin(int32_t speed)
{
  return Arduino_TFT::begin(speed);
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_ILI9225::setRotation(uint8_t r)
{
  Arduino_TFT::setRotation(r);
  _bus->beginWrite();
  switch (_rotation)
  {
  case 3:
    _bus->writeC8D16(ILI9225_DRIVER_OUTPUT_CTRL, 0x031C);
    _bus->writeC8D16(ILI9225_ENTRY_MODE, 0x1038);
    break;
  case 2:
    _bus->writeC8D16(ILI9225_DRIVER_OUTPUT_CTRL, 0x021C);
    _bus->writeC8D16(ILI9225_ENTRY_MODE, 0x1030);
    break;
  case 1:
    _bus->writeC8D16(ILI9225_DRIVER_OUTPUT_CTRL, 0x001C);
    _bus->writeC8D16(ILI9225_ENTRY_MODE, 0x1038);
    break;
  default: // case 0:
    _bus->writeC8D16(ILI9225_DRIVER_OUTPUT_CTRL, 0x011C);
    _bus->writeC8D16(ILI9225_ENTRY_MODE, 0x1030);
    break;
  }
  _bus->endWrite();
}

void Arduino_ILI9225::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  uint8_t cmd1, cmd2, cmd3;

  if ((x != _currentX) || (w != _currentW))
  {
    _currentX = x;
    _currentW = w;
    x += _xStart;
    if (_rotation & 0x01) // Landscape
    {
      cmd1 = ILI9225_VERTICAL_WINDOW_ADDR2;
      cmd2 = ILI9225_VERTICAL_WINDOW_ADDR1;
      cmd3 = ILI9225_RAM_ADDR_SET2;
    }
    else
    {
      cmd1 = ILI9225_HORIZONTAL_WINDOW_ADDR2;
      cmd2 = ILI9225_HORIZONTAL_WINDOW_ADDR1;
      cmd3 = ILI9225_RAM_ADDR_SET1;
    }
    _bus->writeC8D16(cmd1, x);
    _bus->writeC8D16(cmd2, x + w - 1);
    _bus->writeC8D16(cmd3, x);
  }
  if ((y != _currentY) || (h != _currentH))
  {
    _currentY = y;
    _currentH = h;
    y += _yStart;
    if (_rotation & 0x01) // Landscape
    {
      cmd1 = ILI9225_HORIZONTAL_WINDOW_ADDR2;
      cmd2 = ILI9225_HORIZONTAL_WINDOW_ADDR1;
      cmd3 = ILI9225_RAM_ADDR_SET1;
    }
    else
    {
      cmd1 = ILI9225_VERTICAL_WINDOW_ADDR2;
      cmd2 = ILI9225_VERTICAL_WINDOW_ADDR1;
      cmd3 = ILI9225_RAM_ADDR_SET2;
    }
    _bus->writeC8D16(cmd1, y);
    _bus->writeC8D16(cmd2, y + h - 1);
    _bus->writeC8D16(cmd3, y);
  }

  _bus->writeCommand(ILI9225_GRAM_DATA_REG); // write to RAM
}

void Arduino_ILI9225::invertDisplay(bool i)
{
  // Not Implemented
  UNUSED(i);
}

void Arduino_ILI9225::displayOn(void)
{
  _bus->sendCommand(ILI9225_POWER_CTRL1);
  _bus->sendData16(0x0800); // Set SAP,DSTB,STB
}

void Arduino_ILI9225::displayOff(void)
{
  _bus->sendCommand(ILI9225_POWER_CTRL1);
  _bus->sendData16(0x0801); // Set SAP,DSTB,STB
}

void Arduino_ILI9225::tftInit()
{
  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(ILI9225_RST_DELAY);
    digitalWrite(_rst, HIGH);
    delay(ILI9225_RST_DELAY);
  }
  else
  {
    // Software Rest
  }

  _bus->batchOperation(ili9225_init_operations, sizeof(ili9225_init_operations));
}
