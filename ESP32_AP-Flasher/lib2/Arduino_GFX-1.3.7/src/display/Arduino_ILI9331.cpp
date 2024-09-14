/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 * https://github.com/adafruit/Adafruit_ILI9331.git
 */
#include "Arduino_ILI9331.h"
#include "SPI.h"

Arduino_ILI9331::Arduino_ILI9331(Arduino_DataBus *bus, int8_t rst, uint8_t r, bool ips)
    : Arduino_TFT(bus, rst, r, ips, ILI9331_TFTWIDTH, ILI9331_TFTHEIGHT, 0, 0, 0, 0)
{
}

bool Arduino_ILI9331::begin(int32_t speed)
{
  return Arduino_TFT::begin(speed);
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_ILI9331::setRotation(uint8_t r)
{
  uint16_t gs, ss, org;
  Arduino_TFT::setRotation(r);
  switch (_rotation)
  {
  case 1:
    gs = 0x2700;
    ss = 0;
    org = 0x1038;
    break;
  case 2:
    gs = 0xA700;
    ss = 0;
    org = 0x1030;
    break;
  case 3:
    gs = 0xA700;
    ss = 0x100;
    org = 0x1038;
    break;
  default: // case 0:
    gs = 0x2700;
    ss = 0x100;
    org = 0x1030;
    break;
  }

  _MC = 0x20, _MP = 0x21, _SC = 0x50, _EC = 0x51, _SP = 0x52, _EP = 0x53;

  if ((_rotation & 1))
  {
    uint16_t x;
    x = _MC, _MC = _MP, _MP = x;
    x = _SC, _SC = _SP, _SP = x;
    x = _EC, _EC = _EP, _EP = x;
  }

  _bus->beginWrite();
  _bus->writeC16D16(ILI9331_GSC1, gs);        // Set the direction of scan by the gate driver
  _bus->writeC16D16(ILI9331_DRVOUTCTL, ss);   // Select the shift direction of outputs from the source driver
  _bus->writeC16D16(ILI9331_ENTRY_MODE, org); // Set GRAM write direction
  _bus->writeCommand16(ILI9331_MW);           // Write to GRAM
  _bus->endWrite();
}

void Arduino_ILI9331::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  if ((x != _currentX) || (w != _currentW))
  {
    _currentX = x;
    _currentW = w;
    x += _xStart;
    _bus->writeC16D16(_MC, x);
    _bus->writeC16D16(_SC, x);
    _bus->writeC16D16(_EC, x + w - 1);
  }

  if ((y != _currentY) || (h != _currentH))
  {
    _currentY = y;
    _currentH = h;
    y += _yStart;
    _bus->writeC16D16(_MP, y);
    _bus->writeC16D16(_SP, y);
    _bus->writeC16D16(_EP, y + h - 1);
  }

  _bus->writeCommand16(ILI9331_MW);
}

void Arduino_ILI9331::invertDisplay(bool i)
{
  _bus->beginWrite();
  _bus->writeC16D16(ILI9331_GSC2, _ips != i);
  _bus->endWrite();
}

void Arduino_ILI9331::displayOn(void)
{
  _bus->beginWrite();
  _bus->writeC16D16(ILI9331_PWCTL1, 0x1690);   // Standby mode OFF
  _bus->writeC16D16(ILI9331_WBRICTRL, 0x0024); // Enable backlight
  _bus->writeC16D16(ILI9331_WBRI, 0x00FF);     // Set maximum brightness
  _bus->endWrite();
  delay(100);
}

void Arduino_ILI9331::displayOff(void)
{
  _bus->beginWrite();
  _bus->writeC16D16(ILI9331_PWCTL1, 0x1691);   // Standby mode OFF
  _bus->writeC16D16(ILI9331_WBRICTRL, 0x0020); // Disable backlight
  _bus->endWrite();
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Arduino_ILI9331::tftInit()
{
  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(ILI9331_RST_DELAY);
    digitalWrite(_rst, HIGH);
    delay(ILI9331_RST_DELAY);
  }

  _bus->batchOperation(ili9331_init_operations, sizeof(ili9331_init_operations));

  invertDisplay(false);
}
