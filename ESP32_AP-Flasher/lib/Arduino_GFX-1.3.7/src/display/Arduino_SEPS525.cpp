/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 */
#include "Arduino_SEPS525.h"
#include "SPI.h"

Arduino_SEPS525::Arduino_SEPS525(
    Arduino_DataBus *bus, int8_t rst, uint8_t r, int16_t w, int16_t h,
    uint8_t col_offset1, uint8_t row_offset1, uint8_t col_offset2, uint8_t row_offset2)
    : Arduino_TFT(bus, rst, r, false, w, h, col_offset1, row_offset1, col_offset2, row_offset2)
{
}

bool Arduino_SEPS525::begin(int32_t speed)
{
  return Arduino_TFT::begin(speed);
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Arduino_SEPS525::tftInit()
{
  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(SEPS525_RST_DELAY);
    digitalWrite(_rst, HIGH);
    delay(SEPS525_RST_DELAY);
  }
  else
  {
    // Software Rest
    _bus->sendCommand(SEPS525_SOFT_RST);
    _bus->sendData(0x01);
    delay(SEPS525_RST_DELAY);
  }

  _bus->sendCommand(SEPS525_REDUCE_CURRENT);
  _bus->sendData(0x01);
  delay(1);

  // normal mode
  _bus->sendCommand(SEPS525_REDUCE_CURRENT);
  _bus->sendData(0x00);
  delay(1);

  // display off
  _bus->sendCommand(SEPS525_DISP_ON_OFF);
  _bus->sendData(0x00);

  // turn on internal oscillator using external resistor
  _bus->sendCommand(SEPS525_OSC_CTL);
  _bus->sendData(0x01);

  // 90 hz frame rate, divider 0
  _bus->sendCommand(SEPS525_CLOCK_DIV);
  _bus->sendData(0x30);

  // duty cycle 127
  _bus->sendCommand(0x28);
  _bus->sendData(0x7f);

  // start on line 0
  _bus->sendCommand(0x29);
  _bus->sendData(0x00);

  // rgb_if
  _bus->sendCommand(SEPS525_RGB_IF);
  _bus->sendData(0x31);

  // driving current r g b (uA)
  _bus->sendCommand(SEPS525_DRIVING_CURRENT_R);
  _bus->sendData(0x45);
  _bus->sendCommand(SEPS525_DRIVING_CURRENT_G);
  _bus->sendData(0x34);
  _bus->sendCommand(SEPS525_DRIVING_CURRENT_B);
  _bus->sendData(0x33);

  // precharge time r g b
  _bus->sendCommand(SEPS525_PRECHARGE_TIME_R);
  _bus->sendData(0x04);
  _bus->sendCommand(SEPS525_PRECHARGE_TIME_G);
  _bus->sendData(0x05);
  _bus->sendCommand(SEPS525_PRECHARGE_TIME_B);
  _bus->sendData(0x05);

  // precharge current r g b (uA)
  _bus->sendCommand(SEPS525_PRECHARGE_CURRENT_R);
  _bus->sendData(0x9d);
  _bus->sendCommand(SEPS525_PRECHARGE_CURRENT_G);
  _bus->sendData(0x8c);
  _bus->sendCommand(SEPS525_PRECHARGE_CURRENT_B);
  _bus->sendData(0x57);

  _bus->sendCommand(SEPS525_IREF);
  _bus->sendData(0x00);

  // display on
  _bus->sendCommand(SEPS525_DISP_ON_OFF);
  _bus->sendData(0x01);
}

void Arduino_SEPS525::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  uint8_t cmd1, cmd2, cmd3;
  if ((x != _currentX) || (w != _currentW))
  {
    int16_t x_start = x + _xStart, x_end = x + w - 1 + _xStart;
    if (_rotation & 0x01) // Portrait
    {
      cmd1 = SEPS525_MY1_ADDR;
      cmd2 = SEPS525_M_AP_Y;
      cmd3 = SEPS525_MY2_ADDR;
    }
    else
    {
      cmd1 = SEPS525_MX1_ADDR;
      cmd2 = SEPS525_M_AP_X;
      cmd3 = SEPS525_MX2_ADDR;
    }
    _bus->writeCommand(cmd1);
    _bus->write16(x_start);
    _bus->writeCommand(cmd2);
    _bus->write16(x_start);
    _bus->writeCommand(cmd3);
    _bus->write16(x_end);

    _currentX = x;
    _currentW = w;
  }
  if ((y != _currentY) || (h != _currentH))
  {
    int16_t y_start = y + _yStart, y_end = y + h - 1 + _yStart;
    if (_rotation & 0x01) // Portrait
    {
      cmd1 = SEPS525_MX1_ADDR;
      cmd2 = SEPS525_M_AP_X;
      cmd3 = SEPS525_MX2_ADDR;
    }
    else
    {
      cmd1 = SEPS525_MY1_ADDR;
      cmd2 = SEPS525_M_AP_Y;
      cmd3 = SEPS525_MY2_ADDR;
    }
    _bus->writeCommand(cmd1);
    _bus->write16(y_start);
    _bus->writeCommand(cmd2);
    _bus->write16(y_start);
    _bus->writeCommand(cmd3);
    _bus->write16(y_end);

    _currentY = y;
    _currentH = h;
  }

  _bus->writeCommand(SEPS525_RAMWR); // write to RAM
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_SEPS525::setRotation(uint8_t r)
{
  Arduino_TFT::setRotation(r);
  switch (_rotation)
  {
  case 1:
    _bus->sendCommand(SEPS525_DISPLAY_MODE_SET);
    _bus->sendData(0x10);
    _bus->sendCommand(SEPS525_MEMORY_WRITE_MODE);
    _bus->sendData(0x67);
    break;
  case 2:
    _bus->sendCommand(SEPS525_DISPLAY_MODE_SET);
    _bus->sendData(0x30);
    _bus->sendCommand(SEPS525_MEMORY_WRITE_MODE);
    _bus->sendData(0x66);
    break;
  case 3:
    _bus->sendCommand(SEPS525_DISPLAY_MODE_SET);
    _bus->sendData(0x20);
    _bus->sendCommand(SEPS525_MEMORY_WRITE_MODE);
    _bus->sendData(0x67);
    break;
  default: // case 0:
    _bus->sendCommand(SEPS525_DISPLAY_MODE_SET);
    _bus->sendData(0x00);
    _bus->sendCommand(SEPS525_MEMORY_WRITE_MODE);
    _bus->sendData(0x66);
    break;
  }
}

void Arduino_SEPS525::invertDisplay(bool i)
{
  // Not Implemented
  UNUSED(i);
}

void Arduino_SEPS525::displayOn(void)
{
  _bus->sendCommand(SEPS525_DISP_ON_OFF);
  _bus->sendData(0x01);
}

void Arduino_SEPS525::displayOff(void)
{
  _bus->sendCommand(SEPS525_DISP_ON_OFF);
  _bus->sendData(0x00);
}
