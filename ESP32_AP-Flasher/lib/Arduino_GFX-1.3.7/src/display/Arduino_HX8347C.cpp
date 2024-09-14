/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 */
#include "Arduino_HX8347C.h"

Arduino_HX8347C::Arduino_HX8347C(
    Arduino_DataBus *bus, int8_t rst, uint8_t r,
    bool ips, int16_t w, int16_t h,
    uint8_t col_offset1, uint8_t row_offset1, uint8_t col_offset2, uint8_t row_offset2)
    : Arduino_TFT(bus, rst, r, ips, w, h, col_offset1, row_offset1, col_offset2, row_offset2)
{
  _invert = ips;
}

bool Arduino_HX8347C::begin(int32_t speed)
{
  return Arduino_TFT::begin(speed);
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Arduino_HX8347C::tftInit()
{
  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(HX8347C_RST_DELAY);
    digitalWrite(_rst, HIGH);
    delay(HX8347C_RST_DELAY);
  }
  else
  {
    // Software Rest
  }

  // LCD Init For 3.0inch LCD Panel with HX8347C.
  // Power Voltage Setting
  _bus->sendCommand(0x1A);
  _bus->sendData(0x02); // BT
  _bus->sendCommand(0x1B);
  _bus->sendData(0x8B); // VRH
  //****VCOM offset**///
  _bus->sendCommand(0x23);
  _bus->sendData(0x00); // SEL_VCM
  _bus->sendCommand(0x24);
  _bus->sendData(0x5D); // VCM
  _bus->sendCommand(0x25);
  _bus->sendData(0x15); // VDV
  _bus->sendCommand(0x2D);
  _bus->sendData(0x01); // NOW[2:0]=001
  //****OPON**//
  _bus->sendCommand(0xE8);
  _bus->sendData(0x60);
  // Power on Setting
  _bus->sendCommand(0x18);
  _bus->sendData(0x04); // Frame rate 72Hz
  _bus->sendCommand(0x19);
  _bus->sendData(0x01); // OSC_EN='1', start Osc
  _bus->sendCommand(0x01);
  _bus->sendData(0x00); // DP_STB='0', out deep sleep
  _bus->sendCommand(0x1F);
  _bus->sendData(0x88); // STB=0
  delay(5);
  _bus->sendCommand(0x1F);
  _bus->sendData(0x80); // DK=0
  delay(5);
  _bus->sendCommand(0x1F);
  _bus->sendData(0x90); // PON=1
  delay(5);
  _bus->sendCommand(0x1F);
  _bus->sendData(0xD0); // VCOMG=1
  delay(5);

  // 262k/65k color selection
  _bus->sendCommand(0x17);
  _bus->sendData(0x05); // default 0x06 262k color // 0x05 65k color
  // SET PANEL
  _bus->sendCommand(0x29);
  _bus->sendData(0x31); // 400 lines
  _bus->sendCommand(0x71);
  _bus->sendData(0x1A); // RTN
  // Gamma 2.2 Setting
  _bus->sendCommand(0x40);
  _bus->sendData(0x00);
  _bus->sendCommand(0x41);
  _bus->sendData(0x77);
  _bus->sendCommand(0x42);
  _bus->sendData(0x77);
  _bus->sendCommand(0x43);
  _bus->sendData(0x00);
  _bus->sendCommand(0x44);
  _bus->sendData(0x04);
  _bus->sendCommand(0x45);
  _bus->sendData(0x00);
  _bus->sendCommand(0x46);
  _bus->sendData(0x00);
  _bus->sendCommand(0x47);
  _bus->sendData(0x00);
  _bus->sendCommand(0x48);
  _bus->sendData(0x77);
  _bus->sendCommand(0x49);
  _bus->sendData(0x00);
  _bus->sendCommand(0x4A);
  _bus->sendData(0x00);
  _bus->sendCommand(0x4B);
  _bus->sendData(0x08);
  _bus->sendCommand(0x4C);
  _bus->sendData(0x00);
  _bus->sendCommand(0x4D);
  _bus->sendData(0x00);
  _bus->sendCommand(0x4E);
  _bus->sendData(0x00);
  // Set DGC
  _bus->sendCommand(0xFF);
  _bus->sendData(0x01); // Page1
  _bus->sendCommand(0x00);
  _bus->sendData(0x01); // DGC_EN
  _bus->sendCommand(0x01);
  _bus->sendData(0x00);
  _bus->sendCommand(0x02);
  _bus->sendData(0x06);
  _bus->sendCommand(0x03);
  _bus->sendData(0x0C);
  _bus->sendCommand(0x04);
  _bus->sendData(0x12);
  _bus->sendCommand(0x05);
  _bus->sendData(0x16);
  _bus->sendCommand(0x06);
  _bus->sendData(0x1C);
  _bus->sendCommand(0x07);
  _bus->sendData(0x23);
  _bus->sendCommand(0x08);
  _bus->sendData(0x2E);
  _bus->sendCommand(0x09);
  _bus->sendData(0x36);
  _bus->sendCommand(0x0A);
  _bus->sendData(0x3F);
  _bus->sendCommand(0x0B);
  _bus->sendData(0x47);
  _bus->sendCommand(0x0C);
  _bus->sendData(0x50);
  _bus->sendCommand(0x0D);
  _bus->sendData(0x57);
  _bus->sendCommand(0x0E);
  _bus->sendData(0x5F);
  _bus->sendCommand(0x0F);
  _bus->sendData(0x67);
  _bus->sendCommand(0x10);
  _bus->sendData(0x6F);
  _bus->sendCommand(0x11);
  _bus->sendData(0x76);
  _bus->sendCommand(0x12);
  _bus->sendData(0x7F);
  _bus->sendCommand(0x13);
  _bus->sendData(0x87);
  _bus->sendCommand(0x14);
  _bus->sendData(0x90);
  _bus->sendCommand(0x15);
  _bus->sendData(0x99);
  _bus->sendCommand(0x16);
  _bus->sendData(0xA3);
  _bus->sendCommand(0x17);
  _bus->sendData(0xAD);
  _bus->sendCommand(0x18);
  _bus->sendData(0xB4);
  _bus->sendCommand(0x19);
  _bus->sendData(0xBB);
  _bus->sendCommand(0x1A);
  _bus->sendData(0xC4);
  _bus->sendCommand(0x1B);
  _bus->sendData(0xCE);
  _bus->sendCommand(0x1C);
  _bus->sendData(0xD9);
  _bus->sendCommand(0x1D);
  _bus->sendData(0xE3);
  _bus->sendCommand(0x1E);
  _bus->sendData(0xEC);
  _bus->sendCommand(0x1F);
  _bus->sendData(0xF3);
  _bus->sendCommand(0x20);
  _bus->sendData(0xF7);
  _bus->sendCommand(0x21);
  _bus->sendData(0xFC);
  _bus->sendCommand(0x22);
  _bus->sendData(0x00);
  _bus->sendCommand(0x23);
  _bus->sendData(0x06);
  _bus->sendCommand(0x24);
  _bus->sendData(0x0C);
  _bus->sendCommand(0x25);
  _bus->sendData(0x12);
  _bus->sendCommand(0x26);
  _bus->sendData(0x16);
  _bus->sendCommand(0x27);
  _bus->sendData(0x1C);
  _bus->sendCommand(0x28);
  _bus->sendData(0x23);
  _bus->sendCommand(0x29);
  _bus->sendData(0x2E);
  _bus->sendCommand(0x2A);
  _bus->sendData(0x36);
  _bus->sendCommand(0x2B);
  _bus->sendData(0x3F);
  _bus->sendCommand(0x2C);
  _bus->sendData(0x47);
  _bus->sendCommand(0x2D);
  _bus->sendData(0x50);
  _bus->sendCommand(0x2E);
  _bus->sendData(0x57);
  _bus->sendCommand(0x2F);
  _bus->sendData(0x5F);
  _bus->sendCommand(0x30);
  _bus->sendData(0x67);
  _bus->sendCommand(0x31);
  _bus->sendData(0x6F);
  _bus->sendCommand(0x32);
  _bus->sendData(0x76);
  _bus->sendCommand(0x33);
  _bus->sendData(0x7F);
  _bus->sendCommand(0x34);
  _bus->sendData(0x87);
  _bus->sendCommand(0x35);
  _bus->sendData(0x90);
  _bus->sendCommand(0x36);
  _bus->sendData(0x99);
  _bus->sendCommand(0x37);
  _bus->sendData(0xA3);
  _bus->sendCommand(0x38);
  _bus->sendData(0xAD);
  _bus->sendCommand(0x39);
  _bus->sendData(0xB4);
  _bus->sendCommand(0x3A);
  _bus->sendData(0xBB);
  _bus->sendCommand(0x3B);
  _bus->sendData(0xC4);
  _bus->sendCommand(0x3C);
  _bus->sendData(0xCE);
  _bus->sendCommand(0x3D);
  _bus->sendData(0xD9);
  _bus->sendCommand(0x3E);
  _bus->sendData(0xE3);
  _bus->sendCommand(0x3F);
  _bus->sendData(0xEC);
  _bus->sendCommand(0x40);
  _bus->sendData(0xF3);
  _bus->sendCommand(0x41);
  _bus->sendData(0xF7);
  _bus->sendCommand(0x42);
  _bus->sendData(0xFC);
  _bus->sendCommand(0x43);
  _bus->sendData(0x00);
  _bus->sendCommand(0x44);
  _bus->sendData(0x06);
  _bus->sendCommand(0x45);
  _bus->sendData(0x0C);
  _bus->sendCommand(0x46);
  _bus->sendData(0x12);
  _bus->sendCommand(0x47);
  _bus->sendData(0x16);
  _bus->sendCommand(0x48);
  _bus->sendData(0x1C);
  _bus->sendCommand(0x49);
  _bus->sendData(0x23);
  _bus->sendCommand(0x4A);
  _bus->sendData(0x2E);
  _bus->sendCommand(0x4B);
  _bus->sendData(0x36);
  _bus->sendCommand(0x4C);
  _bus->sendData(0x3F);
  _bus->sendCommand(0x4D);
  _bus->sendData(0x47);
  _bus->sendCommand(0x4E);
  _bus->sendData(0x50);
  _bus->sendCommand(0x4F);
  _bus->sendData(0x57);
  _bus->sendCommand(0x50);
  _bus->sendData(0x5F);
  _bus->sendCommand(0x51);
  _bus->sendData(0x67);
  _bus->sendCommand(0x52);
  _bus->sendData(0x6F);
  _bus->sendCommand(0x53);
  _bus->sendData(0x76);
  _bus->sendCommand(0x54);
  _bus->sendData(0x7F);
  _bus->sendCommand(0x55);
  _bus->sendData(0x87);
  _bus->sendCommand(0x56);
  _bus->sendData(0x90);
  _bus->sendCommand(0x57);
  _bus->sendData(0x99);
  _bus->sendCommand(0x58);
  _bus->sendData(0xA3);
  _bus->sendCommand(0x59);
  _bus->sendData(0xAD);
  _bus->sendCommand(0x5A);
  _bus->sendData(0xB4);
  _bus->sendCommand(0x5B);
  _bus->sendData(0xBB);
  _bus->sendCommand(0x5C);
  _bus->sendData(0xC4);
  _bus->sendCommand(0x5D);
  _bus->sendData(0xCE);
  _bus->sendCommand(0x5E);
  _bus->sendData(0xD9);
  _bus->sendCommand(0x5F);
  _bus->sendData(0xE3);
  _bus->sendCommand(0x60);
  _bus->sendData(0xEC);
  _bus->sendCommand(0x61);
  _bus->sendData(0xF3);
  _bus->sendCommand(0x62);
  _bus->sendData(0xF7);
  _bus->sendCommand(0x63);
  _bus->sendData(0xFC);
  _bus->sendCommand(0xFF);
  _bus->sendData(0x00); // Page0
  // Display ON Setting
  _bus->sendCommand(0x28);
  _bus->sendData(0x38); // GON=1, DTE=1, D=10
  delay(40);
  _bus->sendCommand(0x28);
  _bus->sendData(0x3C);    // GON=1, DTE=1, D=11
  _bus->sendCommand(0x22); // Start GRAM write
}

void Arduino_HX8347C::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  if ((x != _currentX) || (w != _currentW))
  {
    int16_t x_start = x + _xStart, x_end = x + w - 1 + _xStart;
    _bus->writeCommand(0x02);
    _bus->write(x_start >> 8);
    _bus->writeCommand(0x03);
    _bus->write(x_start & 0xFF);
    _bus->writeCommand(0x04);
    _bus->write(x_end >> 8);
    _bus->writeCommand(0x05);
    _bus->write(x_end & 0xFF);

    _currentX = x;
    _currentW = w;
  }
  if ((y != _currentY) || (h != _currentH))
  {
    int16_t y_start = y + _yStart, y_end = y + h - 1 + _yStart;
    _bus->writeCommand(0x06);
    _bus->write(y_start >> 8);
    _bus->writeCommand(0x07);
    _bus->write(y_start & 0xFF);
    _bus->writeCommand(0x08);
    _bus->write(y_end >> 8);
    _bus->writeCommand(0x09);
    _bus->write(y_end & 0xFF);

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
void Arduino_HX8347C::setRotation(uint8_t r)
{
  Arduino_TFT::setRotation(r);
  switch (_rotation)
  {
  case 1:
    _bus->sendCommand(0x36);
    _bus->sendData(_invert ? 0x13 : 0x03);
    _bus->sendCommand(0x16);
    _bus->sendData(0x60);
    break;
  case 2:
    _bus->sendCommand(0x36);
    _bus->sendData(_invert ? 0x13 : 0x03);
    _bus->sendCommand(0x16);
    _bus->sendData(0x00);
    break;
  case 3:
    _bus->sendCommand(0x36);
    _bus->sendData(_invert ? 0x17 : 0x07);
    _bus->sendCommand(0x16);
    _bus->sendData(0x20);
    break;
  default: // case 0:
    _bus->sendCommand(0x36);
    _bus->sendData(_invert ? 0x17 : 0x07);
    _bus->sendCommand(0x16);
    _bus->sendData(0x40);
    break;
  }
}

void Arduino_HX8347C::invertDisplay(bool i)
{
  _invert = _ips ^ i;
  setRotation(_rotation); // set invert by calling setRotation()
}

void Arduino_HX8347C::displayOn(void)
{
  _bus->sendCommand(0x28);
  _bus->sendData(0x3C); // GON=1, DTE=1, D=11
}

void Arduino_HX8347C::displayOff(void)
{
  _bus->sendCommand(0x28);
  _bus->sendData(0x34); // GON=1, DTE=1, D=01
  delay(40);
}
