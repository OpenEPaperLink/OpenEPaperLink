/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 * https://github.com/adafruit/Adafruit-SSD1351-library.git
 */
#include "Arduino_SSD1351.h"
#include "SPI.h"

Arduino_SSD1351::Arduino_SSD1351(
    Arduino_DataBus *bus, int8_t rst, uint8_t r, int16_t w, int16_t h,
    uint8_t col_offset1, uint8_t row_offset1, uint8_t col_offset2, uint8_t row_offset2)
    : Arduino_TFT(bus, rst, r, false, w, h, col_offset1, row_offset1, col_offset2, row_offset2)
{
}

bool Arduino_SSD1351::begin(int32_t speed)
{
#if defined(ESP8266) || defined(ESP32)
  if (speed == GFX_NOT_DEFINED)
  {
    speed = 16000000UL;
  }
// Teensy 4.x
#elif defined(__IMXRT1052__) || defined(__IMXRT1062__)
  if (speed == GFX_NOT_DEFINED)
  {
    speed = 16000000UL;
  }
#endif
  _override_datamode = SPI_MODE0; // always SPI_MODE0

  return Arduino_TFT::begin(speed);
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Arduino_SSD1351::tftInit()
{
  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(SSD1351_RST_DELAY);
    digitalWrite(_rst, HIGH);
    delay(SSD1351_RST_DELAY);
  }
  else
  {
    // Software Rest
  }

  _bus->sendCommand(SSD1351_COMMANDLOCK); // set command lock
  _bus->sendData(0x12);
  _bus->sendCommand(SSD1351_COMMANDLOCK); // set command lock
  _bus->sendData(0xB1);
  _bus->sendCommand(SSD1351_DISPLAYOFF);    // Display off
  _bus->sendCommand(SSD1351_DISPLAYOFFSET); // 0xA2
  _bus->sendData(0x0);
  _bus->sendCommand(SSD1351_NORMALDISPLAY); // 0xA6
  _bus->sendCommand(SSD1351_DISPLAYON);     // Main screen turn on
}

void Arduino_SSD1351::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  uint8_t cmd;
  if ((x != _currentX) || (w != _currentW))
  {
    cmd = (_rotation & 0x01) ? SSD1351_SETROW : SSD1351_SETCOLUMN;
    uint8_t x_start = x + _xStart, x_end = x + w - 1 + _xStart;

    _bus->writeCommand(cmd); // Column addr set
    _bus->write(x_start);    // XSTART
    _bus->write(x_end);      // XEND

    _currentX = x;
    _currentW = w;
  }
  if ((y != _currentY) || (h != _currentH))
  {
    cmd = (_rotation & 0x01) ? SSD1351_SETCOLUMN : SSD1351_SETROW;
    uint8_t y_start = y + _yStart, y_end = y + h - 1 + _yStart;

    _bus->writeCommand(cmd); // Row addr set
    _bus->write(y_start);    // YSTART
    _bus->write(y_end);      // YEND

    _currentY = y;
    _currentH = h;
  }

  _bus->writeCommand(SSD1351_WRITERAM); // write to RAM
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_SSD1351::setRotation(uint8_t r)
{
  Arduino_TFT::setRotation(r);
  uint8_t startline = (_rotation < 2) ? HEIGHT : 0;
  switch (_rotation)
  {
  case 1:
    r = 0b01110111;
    break;
  case 2:
    r = 0b01100110;
    break;
  case 3:
    r = 0b01100101;
    break;
  default: // case 0:
    r = 0b01110100;
    break;
  }
  _bus->beginWrite();
  _bus->writeCommand(SSD1351_SETREMAP);
  _bus->write(r);
  _bus->writeCommand(SSD1351_STARTLINE);
  _bus->write(startline);
  _bus->endWrite();
}

void Arduino_SSD1351::invertDisplay(bool i)
{
  _bus->sendCommand(i ? SSD1351_INVERTDISPLAY : SSD1351_NORMALDISPLAY);
}

void Arduino_SSD1351::displayOn(void)
{
  _bus->sendCommand(SSD1351_DISPLAYALLON);
}

void Arduino_SSD1351::displayOff(void)
{
  _bus->sendCommand(SSD1351_DISPLAYALLOFF);
}
