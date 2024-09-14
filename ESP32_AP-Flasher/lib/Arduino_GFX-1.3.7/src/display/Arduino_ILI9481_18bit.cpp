/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 */
#include "Arduino_ILI9481_18bit.h"

Arduino_ILI9481_18bit::Arduino_ILI9481_18bit(Arduino_DataBus *bus, int8_t rst, uint8_t r, bool ips)
    : Arduino_TFT_18bit(bus, rst, r, ips, ILI9481_TFTWIDTH, ILI9481_TFTHEIGHT, 0, 0, 0, 0)
{
}

bool Arduino_ILI9481_18bit::begin(int32_t speed)
{
#if defined(ESP8266) || defined(ESP32)
  if (speed == GFX_NOT_DEFINED)
  {
    speed = 12000000UL;
  }
// Teensy 4.x
#elif defined(__IMXRT1052__) || defined(__IMXRT1062__)
  if (speed == GFX_NOT_DEFINED)
  {
    speed = 12000000UL;
  }
#endif

  return Arduino_TFT::begin(speed);
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Arduino_ILI9481_18bit::tftInit()
{
  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(ILI9481_RST_DELAY);
    digitalWrite(_rst, HIGH);
    delay(ILI9481_RST_DELAY);
  }
  else
  {
    // Software Rest
    _bus->sendCommand(ILI9481_SWRESET);
    delay(ILI9481_RST_DELAY);
  }

  _bus->sendCommand(ILI9481_SLPOUT);
  delay(280);

  _bus->sendCommand(0xd0); // Power_Setting
  _bus->sendData(0x07);    // 07  VC[2:0] Sets the ratio factor of Vci to generate the reference voltages Vci1
  _bus->sendData(0x44);    // 41  BT[2:0] Sets the Step up factor and output voltage level from the reference voltages Vci1
  _bus->sendData(0x1E);    // 1f  17   1C  VRH[3:0]: Sets the factor to generate VREG1OUT from VCILVL
  delay(220);

  _bus->sendCommand(0xd1); // VCOM Control
  _bus->sendData(0x00);    // 00
  _bus->sendData(0x0C);    // 1A   VCM [6:0] is used to set factor to generate VCOMH voltage from the reference voltage VREG1OUT  15    09
  _bus->sendData(0x1A);    // 1F   VDV[4:0] is used to set the VCOM alternating amplitude in the range of VREG1OUT x 0.70 to VREG1OUT   1F   18

  _bus->sendCommand(0xC5); // Frame Rate
  _bus->sendData(0x03);    // 03   02

  _bus->sendCommand(0xd2); // Power_Setting for Normal Mode
  _bus->sendData(0x01);    // 01
  _bus->sendData(0x11);    // 11

  _bus->sendCommand(0xE4); //
  _bus->sendData(0xa0);
  _bus->sendCommand(0xf3);
  _bus->sendData(0x00);
  _bus->sendData(0x2a);

  // 1  OK
  _bus->sendCommand(0xc8);
  _bus->sendData(0x00);
  _bus->sendData(0x26);
  _bus->sendData(0x21);
  _bus->sendData(0x00);
  _bus->sendData(0x00);
  _bus->sendData(0x1f);
  _bus->sendData(0x65);
  _bus->sendData(0x23);
  _bus->sendData(0x77);
  _bus->sendData(0x00);
  _bus->sendData(0x0f);
  _bus->sendData(0x00);
  // GAMMA SETTING

  _bus->sendCommand(0xC0); // Panel Driving Setting
  _bus->sendData(0x00);    // 1//00  REV  SM  GS
  _bus->sendData(0x3B);    // 2//NL[5:0]: Sets the number of lines to drive the LCD at an interval of 8 lines.
  _bus->sendData(0x00);    // 3//SCN[6:0]
  _bus->sendData(0x02);    // 4//PTV: Sets the Vcom output in non-display area drive period
  _bus->sendData(0x11);    // 5//NDL: Sets the source output level in non-display area.  PTG: Sets the scan mode in non-display area.

  _bus->sendCommand(0xc6); // Interface Control
  _bus->sendData(0x83);
  // GAMMA SETTING

  _bus->sendCommand(0xf0); //?
  _bus->sendData(0x01);

  _bus->sendCommand(0xE4); //?
  _bus->sendData(0xa0);

  _bus->sendCommand(ILI9481_PIXFMT);
  _bus->sendData(0x66);

  _bus->sendCommand(0xb4); // Display Mode and Frame Memory Write Mode Setting
  _bus->sendData(0x02);
  _bus->sendData(0x00); //
  _bus->sendData(0x00);
  _bus->sendData(0x01);

  delay(280);

  _bus->sendCommand(0x29);

  invertDisplay(false);
}

void Arduino_ILI9481_18bit::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  if ((x != _currentX) || (w != _currentW))
  {
    int16_t x_start = x + _xStart, x_end = x + w - 1 + _xStart;

    _bus->writeCommand(ILI9481_CASET); // Column addr set
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

    _bus->writeCommand(ILI9481_PASET); // Row addr set
    _bus->write(y_start >> 8);
    _bus->write(y_start & 0xFF); // YSTART
    _bus->write(y_end >> 8);
    _bus->write(y_end & 0xFF); // YEND

    _currentY = y;
    _currentH = h;
  }

  _bus->writeCommand(ILI9481_RAMWR); // write to RAM
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_ILI9481_18bit::setRotation(uint8_t r)
{
  Arduino_TFT::setRotation(r);
  switch (_rotation)
  {
  case 1:
    r = (ILI9481_MADCTL_MV | ILI9481_MADCTL_BGR | ILI9481_MADCTL_HF | ILI9481_MADCTL_VF);
    break;
  case 2:
    r = (ILI9481_MADCTL_BGR | ILI9481_MADCTL_HF);
    break;
  case 3:
    r = (ILI9481_MADCTL_MV | ILI9481_MADCTL_BGR);
    break;
  default: // case 0:
    r = (ILI9481_MADCTL_BGR | ILI9481_MADCTL_VF);
    break;
  }
  _bus->beginWrite();
  _bus->writeCommand(ILI9481_MADCTL);
  _bus->write(r);
  _bus->endWrite();
}

void Arduino_ILI9481_18bit::invertDisplay(bool i)
{
  _bus->sendCommand((_ips ^ i) ? ILI9481_INVON : ILI9481_INVOFF);
}

void Arduino_ILI9481_18bit::displayOn(void)
{
  _bus->sendCommand(ILI9481_SLPOUT);
  delay(ILI9481_SLPOUT_DELAY);
}

void Arduino_ILI9481_18bit::displayOff(void)
{
  _bus->sendCommand(ILI9481_SLPIN);
  delay(ILI9481_SLPIN_DELAY);
}
