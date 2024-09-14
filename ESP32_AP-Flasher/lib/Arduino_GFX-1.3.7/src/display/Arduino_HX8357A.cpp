#include "Arduino_HX8357A.h"

Arduino_HX8357A::Arduino_HX8357A(Arduino_DataBus *bus, int8_t rst, uint8_t r, bool ips)
    : Arduino_TFT(bus, rst, r, ips, HX8357A_TFTWIDTH, HX8357A_TFTHEIGHT, 0, 0, 0, 0)
{
}

bool Arduino_HX8357A::begin(int32_t speed)
{
  return Arduino_TFT::begin(speed);
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_HX8357A::setRotation(uint8_t r)
{
  Arduino_TFT::setRotation(r);
  switch (_rotation)
  {
  case 3:
    // TODO: not working
    r = (HX8357A_MADCTL_MY | HX8357A_MADCTL_MV | HX8357A_MADCTL_BGR);
    break;
  case 2:
    r = (HX8357A_MADCTL_MX | HX8357A_MADCTL_BGR);
    break;
  case 1:
    // TODO: not working
    r = (HX8357A_MADCTL_MX | HX8357A_MADCTL_MV | HX8357A_MADCTL_BGR);
    break;
  default: // case 0:
    r = (HX8357A_MADCTL_MY | HX8357A_MADCTL_BGR);
    break;
  }
  _bus->beginWrite();
  _bus->writeC8D8(HX8357A_MEMORY_ACCESS_CONTROL, r);
  _bus->endWrite();
}

void Arduino_HX8357A::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  if ((x != _currentX) || (w != _currentW))
  {
    _currentX = x;
    _currentW = w;
    x += _xStart;
    _data16.value = x;
    _bus->writeC8D8(HX8357A_COLUMN_ADDRESS_COUNTER_2, _data16.msb);
    _bus->writeC8D8(HX8357A_COLUMN_ADDRESS_COUNTER_1, _data16.lsb);
    _bus->writeC8D8(HX8357A_COLUMN_ADDRESS_START_2, _data16.msb);
    _bus->writeC8D8(HX8357A_COLUMN_ADDRESS_START_1, _data16.lsb);
    _data16.value = x + w - 1;
    _bus->writeC8D8(HX8357A_COLUMN_ADDRESS_END_2, _data16.msb);
    _bus->writeC8D8(HX8357A_COLUMN_ADDRESS_END_1, _data16.lsb);
  }

  if ((y != _currentY) || (h != _currentH))
  {
    _currentY = y;
    _currentH = h;
    y += _yStart;
    _data16.value = y;
    _bus->writeC8D8(HX8357A_ROW_ADDRESS_COUNTER_2, _data16.msb);
    _bus->writeC8D8(HX8357A_ROW_ADDRESS_COUNTER_1, _data16.lsb);
    _bus->writeC8D8(HX8357A_ROW_ADDRESS_START_2, _data16.msb);
    _bus->writeC8D8(HX8357A_ROW_ADDRESS_START_1, _data16.lsb);
    _data16.value = y + h - 1;
    _bus->writeC8D8(HX8357A_ROW_ADDRESS_END_2, _data16.msb);
    _bus->writeC8D8(HX8357A_ROW_ADDRESS_END_1, _data16.lsb);
  }

  _bus->writeCommand(HX8357A_SRAM_CONTROL);
}

void Arduino_HX8357A::invertDisplay(bool i)
{
  _bus->beginWrite();
  _bus->writeC8D8(HX8357A_DISPLAY_MODE_CONTROL, (_ips ^ i) ? HX8357A_INV_ON : HX8357A_INV_OFF);
  _bus->endWrite();
}

void Arduino_HX8357A::displayOn(void)
{
  _bus->beginWrite();
  _bus->writeC8D8(HX8357A_OSC_CONTROL_2, 0x01); // OSC_EN=1
  _bus->endWrite();
  delay(5);
  _bus->beginWrite();
  _bus->writeC8D8(HX8357A_POWER_CONTROL_1, 0x88);      // PON=0, DK=1
  _bus->writeC8D8(HX8357A_DISPLAY_MODE_CONTROL, 0x00); // DP_STB=00
  _bus->writeC8D8(HX8357A_POWER_CONTROL_4, 0x03);      // AP=011
  _bus->writeC8D8(HX8357A_POWER_CONTROL_1, 0x80);      // DK=0
  _bus->endWrite();
  delay(3);
  _bus->beginWrite();
  _bus->writeC8D8(HX8357A_POWER_CONTROL_1, 0x90); // PON=1
  _bus->endWrite();
  delay(3);
  _bus->beginWrite();
  _bus->writeC8D8(HX8357A_POWER_CONTROL_1, 0xD0);   // VCOMG=1
  _bus->writeC8D8(HX8357A_DISPLAY_CONTROL_3, 0x3C); // GON=1, DTE=1, D[1:0]=11
  _bus->endWrite();
}

void Arduino_HX8357A::displayOff(void)
{
  _bus->beginWrite();
  _bus->writeC8D8(HX8357A_DISPLAY_CONTROL_3, 0x34); // GON=1, DTE=1, D[1:0]=01
  _bus->writeC8D8(HX8357A_POWER_CONTROL_1, 0x90);   // VCOMG=0
  _bus->endWrite();
  delay(1);
  _bus->beginWrite();
  _bus->writeC8D8(HX8357A_POWER_CONTROL_1, 0x88); // PON=0, DK=1
  _bus->writeC8D8(HX8357A_POWER_CONTROL_4, 0x00); // AP=000
  _bus->writeC8D8(HX8357A_POWER_CONTROL_1, 0x89); // STB=1
  _bus->endWrite();
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Arduino_HX8357A::tftInit()
{
  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(HX8357A_RST_DELAY);
    digitalWrite(_rst, HIGH);
    delay(HX8357A_RST_DELAY);
  }
  else
  {
    // Software Rest
  }

  _bus->batchOperation(hx8357a_init_operations, sizeof(hx8357a_init_operations));

  invertDisplay(false);
}
