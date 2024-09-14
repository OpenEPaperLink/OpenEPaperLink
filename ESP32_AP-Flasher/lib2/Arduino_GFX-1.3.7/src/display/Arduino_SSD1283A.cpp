/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 * https://github.com/gitcnd/LCDWIKI_SPI.git
 */
#include "Arduino_SSD1283A.h"
#include "SPI.h"

Arduino_SSD1283A::Arduino_SSD1283A(
    Arduino_DataBus *bus, int8_t rst, uint8_t r, int16_t w, int16_t h,
    uint8_t col_offset1, uint8_t row_offset1, uint8_t col_offset2, uint8_t row_offset2)
    : Arduino_TFT(bus, rst, r, false, w, h, col_offset1, row_offset1, col_offset2, row_offset2)
{
}

bool Arduino_SSD1283A::begin(int32_t speed)
{
#if defined(ESP8266) || defined(ESP32)
  if (speed == GFX_NOT_DEFINED)
  {
    speed = 27000000UL;
  }
// Teensy 4.x
#elif defined(__IMXRT1052__) || defined(__IMXRT1062__)
  if (speed == GFX_NOT_DEFINED)
  {
    speed = 27000000UL;
  }
#endif

  return Arduino_TFT::begin(speed);
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Arduino_SSD1283A::tftInit()
{
  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(SSD1283A_RST_DELAY);
    digitalWrite(_rst, HIGH);
    delay(SSD1283A_RST_DELAY);
  }
  else
  {
    // Software Rest
  }

  // Initialization Sequence
  _bus->beginWrite();
  _bus->writeCommand16(SSD1283A_POWER_CONTROL_1);
  _bus->write16(0x2F8E);
  _bus->writeCommand16(SSD1283A_POWER_CONTROL_2);
  _bus->write16(0x000C);
  _bus->writeCommand16(SSD1283A_DISPLAY_CONTROL);
  _bus->write16(0x0021);
  _bus->writeCommand16(SSD1283A_VCOM_OTP_1);
  _bus->write16(0x0006);
  _bus->writeCommand16(SSD1283A_VCOM_OTP_1);
  _bus->write16(0x0005);
  _bus->writeCommand16(SSD1283A_FURTHER_BIAS_CURRENT_SETTING);
  _bus->write16(0x057F);
  _bus->writeCommand16(SSD1283A_VCOM_OTP_2);
  _bus->write16(0x89A1);
  _bus->writeCommand16(SSD1283A_OSCILLATOR);
  _bus->write16(0x0001);
  _bus->endWrite();
  delay(100);
  _bus->beginWrite();
  _bus->writeCommand16(SSD1283A_VCOM_OTP_2);
  _bus->write16(0x80B0);
  _bus->endWrite();
  delay(30);
  _bus->beginWrite();
  _bus->writeCommand16(SSD1283A_VCOM_OTP_2);
  _bus->write16(0xFFFE);
  _bus->writeCommand16(SSD1283A_DISPLAY_CONTROL);
  _bus->write16(0x0223);
  _bus->endWrite();
  delay(30);
  _bus->beginWrite();
  _bus->writeCommand16(SSD1283A_DISPLAY_CONTROL);
  _bus->write16(0x0233);
  _bus->writeCommand16(SSD1283A_DRIVER_OUTPUT_CONTROL);
  _bus->write16(0x2183);
  _bus->writeCommand16(SSD1283A_ENTRY_MODE);
  _bus->write16(0x6830);
  _bus->writeCommand16(0x2F);
  _bus->write16(0xFFFF);
  _bus->writeCommand16(SSD1283A_OSCILLATOR_FREQUENCY);
  _bus->write16(0x8000);
  _bus->writeCommand16(SSD1283A_FURTHER_BIAS_CURRENT_SETTING);
  _bus->write16(0x0570);
  _bus->writeCommand16(SSD1283A_LCD_DRIVING_WAVEFORM_CONTROL);
  _bus->write16(0x0300);
  _bus->writeCommand16(SSD1283A_FRAME_CYCLE_CONTROL);
  _bus->write16(0x580C);
  _bus->writeCommand16(SSD1283A_POWER_CONTROL_3);
  _bus->write16(0x0609);
  _bus->writeCommand16(SSD1283A_POWER_CONTROL_4);
  _bus->write16(0x3100);
  _bus->endWrite();
}

void Arduino_SSD1283A::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  uint8_t v1 = 0, v2 = 0, v3 = 0, h1 = 0, h2 = 0, h3 = 0;

  // TODO: it works, but should have better way
  switch (_rotation)
  {
  case 1:
    v1 = x + w - 1 + _xStart;
    v2 = x + _xStart;
    v3 = v2;
    h1 = SSD1283A_TFTWIDTH - y - 1 + _yStart;
    h2 = SSD1283A_TFTWIDTH - y - h + _yStart;
    h3 = h1;
    break;
  case 2:
    v1 = SSD1283A_TFTWIDTH - y - 1 + _yStart;
    v2 = SSD1283A_TFTWIDTH - y - h + _yStart;
    v3 = v1;
    h1 = SSD1283A_TFTHEIGHT - x - 1 + _xStart;
    h2 = SSD1283A_TFTHEIGHT - x - w + _xStart;
    h3 = h1;
    break;
  case 3:
    v1 = SSD1283A_TFTHEIGHT - x - 1 + _xStart;
    v2 = SSD1283A_TFTHEIGHT - x - w + _xStart;
    v3 = v1;
    h1 = y + h - 1 + _yStart;
    h2 = y + _yStart;
    h3 = h2;
    break;
  default: // case 0:
    v1 = y + h - 1 + _yStart;
    v2 = y + _yStart;
    v3 = v2;
    h1 = x + w - 1 + _xStart;
    h2 = x + _xStart;
    h3 = h2;
    break;
  }
  _bus->writeCommand(SSD1283A_HORIZONTAL_RAM_ADDRESS_POSITION);
  _bus->write(h1);
  _bus->write(h2);

  _bus->writeCommand(SSD1283A_VERTICAL_RAM_ADDRESS_POSITION);
  _bus->write(v1);
  _bus->write(v2);

  _bus->writeCommand(SSD1283A_RAM_ADDRESS_SET);
  _bus->write(v3);
  _bus->write(h3);

  _bus->writeCommand(SSD1283A_WRITE_DATA_TO_GRAM);
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_SSD1283A::setRotation(uint8_t r)
{
  Arduino_TFT::setRotation(r);
  _bus->beginWrite();
  switch (_rotation)
  {
  case 1:
    _bus->writeCommand16(SSD1283A_ENTRY_MODE);
    _bus->write16(0x6828);
    break;
  case 2:
    _bus->writeCommand(SSD1283A_ENTRY_MODE);
    _bus->write16(0x6800);
    break;
  case 3:
    _bus->writeCommand(SSD1283A_ENTRY_MODE);
    _bus->write16(0x6818);
    break;
  default: // case 0:
    _bus->writeCommand16(SSD1283A_ENTRY_MODE);
    _bus->write16(0x6830);
    break;
  }
  _bus->endWrite();
}

void Arduino_SSD1283A::invertDisplay(bool i)
{
  // Not Implemented
  UNUSED(i);
}

void Arduino_SSD1283A::displayOn(void)
{
  // Not Implemented
}

void Arduino_SSD1283A::displayOff(void)
{
  // Not Implemented
}
