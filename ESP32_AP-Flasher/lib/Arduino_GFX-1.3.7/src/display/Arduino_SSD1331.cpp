/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 */
#include "Arduino_SSD1331.h"
#include "SPI.h"

Arduino_SSD1331::Arduino_SSD1331(
    Arduino_DataBus *bus, int8_t rst, uint8_t r, int16_t w, int16_t h,
    uint8_t col_offset1, uint8_t row_offset1, uint8_t col_offset2, uint8_t row_offset2)
    : Arduino_TFT(bus, rst, r, false, w, h, col_offset1, row_offset1, col_offset2, row_offset2)
{
}

bool Arduino_SSD1331::begin(int32_t speed)
{
  return Arduino_TFT::begin(speed);
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Arduino_SSD1331::tftInit()
{
  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(SSD1331_RST_DELAY);
    digitalWrite(_rst, HIGH);
    delay(SSD1331_RST_DELAY);
  }
  else
  {
    // Software Rest
  }

  // Initialization Sequence
  _bus->sendCommand(SSD1331_DISPLAYOFF); // 0xAE
  _bus->sendCommand(SSD1331_STARTLINE);  // 0xA1
  _bus->sendCommand(0x0);
  _bus->sendCommand(SSD1331_DISPLAYOFFSET); // 0xA2
  _bus->sendCommand(0x0);
  _bus->sendCommand(SSD1331_NORMALDISPLAY); // 0xA4
  _bus->sendCommand(SSD1331_SETMULTIPLEX);  // 0xA8
  _bus->sendCommand(0x3F);                  // 0x3F 1/64 duty
  _bus->sendCommand(SSD1331_SETMASTER);     // 0xAD
  _bus->sendCommand(0x8E);
  _bus->sendCommand(SSD1331_POWERMODE); // 0xB0
  _bus->sendCommand(0x0B);
  _bus->sendCommand(SSD1331_PRECHARGE); // 0xB1
  _bus->sendCommand(0x31);
  _bus->sendCommand(SSD1331_CLOCKDIV);   // 0xB3
  _bus->sendCommand(0xF0);               // 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
  _bus->sendCommand(SSD1331_PRECHARGEA); // 0x8A
  _bus->sendCommand(0x64);
  _bus->sendCommand(SSD1331_PRECHARGEB); // 0x8B
  _bus->sendCommand(0x78);
  _bus->sendCommand(SSD1331_PRECHARGEA); // 0x8C
  _bus->sendCommand(0x64);
  _bus->sendCommand(SSD1331_PRECHARGELEVEL); // 0xBB
  _bus->sendCommand(0x3A);
  _bus->sendCommand(SSD1331_VCOMH); // 0xBE
  _bus->sendCommand(0x3E);
  _bus->sendCommand(SSD1331_MASTERCURRENT); // 0x87
  _bus->sendCommand(0x06);
  _bus->sendCommand(SSD1331_CONTRASTA); // 0x81
  _bus->sendCommand(0x91);
  _bus->sendCommand(SSD1331_CONTRASTB); // 0x82
  _bus->sendCommand(0x50);
  _bus->sendCommand(SSD1331_CONTRASTC); // 0x83
  _bus->sendCommand(0x7D);
  _bus->sendCommand(SSD1331_DISPLAYON); //--turn on oled panel
}

void Arduino_SSD1331::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  if ((x != _currentX) || (w != _currentW))
  {
    uint8_t cmd = (_rotation & 0x01) ? SSD1331_SETROW : SSD1331_SETCOLUMN;
    uint8_t x_start = x + _xStart, x_end = x + w - 1 + _xStart;

    _bus->writeCommand(cmd);     // Column addr set
    _bus->writeCommand(x_start); // XSTART
    _bus->writeCommand(x_end);   // XEND

    _currentX = x;
    _currentW = w;
  }
  if ((y != _currentY) || (h != _currentH))
  {
    uint8_t cmd = (_rotation & 0x01) ? SSD1331_SETCOLUMN : SSD1331_SETROW;
    uint8_t y_start = y + _yStart, y_end = y + h - 1 + _yStart;

    _bus->writeCommand(cmd);     // Row addr set
    _bus->writeCommand(y_start); // YSTART
    _bus->writeCommand(y_end);   // YEND

    _currentY = y;
    _currentH = h;
  }
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_SSD1331::setRotation(uint8_t r)
{
  Arduino_TFT::setRotation(r);
  switch (_rotation)
  {
  case 1:
    r = 0b01110001;
    break;
  case 2:
    r = 0b01100000;
    break;
  case 3:
    r = 0b01100011;
    break;
  default: // case 0:
    r = 0b01110010;
    break;
  }
  _bus->beginWrite();
  _bus->writeCommand(SSD1331_SETREMAP);
  _bus->writeCommand(r);
  _bus->endWrite();
}

void Arduino_SSD1331::invertDisplay(bool i)
{
  _bus->sendCommand(i ? SSD1331_INVERTDISPLAY : SSD1331_NORMALDISPLAY);
}

void Arduino_SSD1331::displayOn(void)
{
  _bus->sendCommand(SSD1331_DISPLAYALLON);
}

void Arduino_SSD1331::displayOff(void)
{
  _bus->sendCommand(SSD1331_DISPLAYALLOFF);
}
