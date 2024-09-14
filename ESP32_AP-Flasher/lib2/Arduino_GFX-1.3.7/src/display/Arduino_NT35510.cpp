/*
 * start rewrite from:
 * https://github.com/hi631/LCD_NT35510-MRB3971.git
 */
#include "Arduino_NT35510.h"

Arduino_NT35510::Arduino_NT35510(
    Arduino_DataBus *bus, int8_t rst, uint8_t r,
    bool ips, int16_t w, int16_t h,
    uint8_t col_offset1, uint8_t row_offset1, uint8_t col_offset2, uint8_t row_offset2)
    : Arduino_TFT(bus, rst, r, ips, w, h, col_offset1, row_offset1, col_offset2, row_offset2)
{
}

bool Arduino_NT35510::begin(int32_t speed)
{
  return Arduino_TFT::begin(speed);
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_NT35510::setRotation(uint8_t r)
{
  Arduino_TFT::setRotation(r);
  switch (_rotation)
  {
  case 3:
    r = (NT35510_MADCTL_MY | NT35510_MADCTL_MV);
    break;
  case 2:
    r = (NT35510_MADCTL_MY | NT35510_MADCTL_MX);
    break;
  case 1:
    r = (NT35510_MADCTL_MX | NT35510_MADCTL_MV);
    break;
  default: // case 0:
    r = 0;
    break;
  }
  _bus->beginWrite();
  _bus->writeCommand16(NT35510_MADCTR);
  _bus->write16(r);
  _bus->endWrite();
}

void Arduino_NT35510::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  if ((x != _currentX) || (w != _currentW))
  {
    _currentX = x;
    _currentW = w;
    _data16.value = x + _xStart;
    _bus->writeC16D16(NT35510_CASET, _data16.msb);
    _bus->writeC16D16(NT35510_CASET + 1, _data16.lsb);
    _data16.value += w - 1;
    _bus->writeC16D16(NT35510_CASET + 2, _data16.msb);
    _bus->writeC16D16(NT35510_CASET + 3, _data16.lsb);
  }

  if ((y != _currentY) || (h != _currentH))
  {
    _currentY = y;
    _currentH = h;
    _data16.value = y + _yStart;
    _bus->writeC16D16(NT35510_PASET, _data16.msb);
    _bus->writeC16D16(NT35510_PASET + 1, _data16.lsb);
    _data16.value += h - 1;
    _bus->writeC16D16(NT35510_PASET + 2, _data16.msb);
    _bus->writeC16D16(NT35510_PASET + 3, _data16.lsb);
  }

  _bus->writeCommand16(NT35510_RAMWR); // write to RAM
}

void Arduino_NT35510::invertDisplay(bool i)
{
  // Not Implemented
  UNUSED(i);
}

void Arduino_NT35510::displayOn(void)
{
  // Not Implemented
}

void Arduino_NT35510::displayOff(void)
{
  // Not Implemented
}

void Arduino_NT35510::WriteRegM(uint16_t adr, uint16_t len, uint8_t dat[])
{
  for (uint16_t i = 0; i < len; i++)
  {
    _bus->writeCommand16(adr++);
    _bus->write16(dat[i]);
  }
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Arduino_NT35510::tftInit()
{
  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(NT35510_RST_DELAY);
    digitalWrite(_rst, HIGH);
    delay(NT35510_RST_DELAY);
  }
  else
  {
    // Software Rest
  }

  //************* NT35510**********//
  _bus->beginWrite();
  uint8_t ini01[] = {0x55, 0xAA, 0x52, 0x08, 0x01};
  WriteRegM(0xF000, sizeof(ini01), ini01);
  uint8_t ini03[] = {0x34, 0x34, 0x34};
  WriteRegM(0xB600, sizeof(ini03), ini03);
  uint8_t ini02[] = {0x0D, 0x0D, 0x0D};
  WriteRegM(0xB000, sizeof(ini02), ini02); // AVDD Set AVDD 5.2V
  uint8_t ini05[] = {0x34, 0x34, 0x34};
  WriteRegM(0xB700, sizeof(ini05), ini05); // AVEE ratio
  uint8_t ini04[] = {0x0D, 0x0D, 0x0D};
  WriteRegM(0xB100, sizeof(ini04), ini04); // AVEE  -5.2V
  uint8_t ini07[] = {0x24, 0x24, 0x24};
  WriteRegM(0xB800, sizeof(ini07), ini07); // VCL ratio
  uint8_t ini10[] = {0x34, 0x34, 0x34};
  WriteRegM(0xB900, sizeof(ini10), ini10); // VGH  ratio
  uint8_t ini09[] = {0x0F, 0x0F, 0x0F};
  WriteRegM(0xB300, sizeof(ini09), ini09);
  uint8_t ini14[] = {0x24, 0x24, 0x24};
  WriteRegM(0xBA00, sizeof(ini14), ini14); // VGLX  ratio
  uint8_t ini12[] = {0x08, 0x08};
  WriteRegM(0xB500, sizeof(ini12), ini12);
  uint8_t ini15[] = {0x00, 0x78, 0x00};
  WriteRegM(0xBC00, sizeof(ini15), ini15); // VGMP/VGSP 4.5V/0V
  uint8_t ini16[] = {0x00, 0x78, 0x00};
  WriteRegM(0xBD00, sizeof(ini16), ini16); // VGMN/VGSN -4.5V/0V
  uint8_t ini17[] = {0x00, 0x89};
  WriteRegM(0xBE00, sizeof(ini17), ini17); // VCOM  -1.325V
  // Gamma Setting
  uint8_t ini20[] = {
      0x00, 0x2D, 0x00, 0x2E, 0x00, 0x32, 0x00, 0x44, 0x00, 0x53, 0x00, 0x88, 0x00, 0xB6, 0x00, 0xF3, 0x01, 0x22, 0x01, 0x64,
      0x01, 0x92, 0x01, 0xD4, 0x02, 0x07, 0x02, 0x08, 0x02, 0x34, 0x02, 0x5F, 0x02, 0x78, 0x02, 0x94, 0x02, 0xA6, 0x02, 0xBB,
      0x02, 0xCA, 0x02, 0xDB, 0x02, 0xE8, 0x02, 0xF9, 0x03, 0x1F, 0x03, 0x7F};
  WriteRegM(0xD100, sizeof(ini20), ini20);
  WriteRegM(0xD400, sizeof(ini20), ini20); // R+ R-
  WriteRegM(0xD200, sizeof(ini20), ini20);
  WriteRegM(0xD500, sizeof(ini20), ini20); // G+ G-
  WriteRegM(0xD300, sizeof(ini20), ini20);
  WriteRegM(0xD600, sizeof(ini20), ini20); // B+ B-
  //
  uint8_t ini21[] = {0x55, 0xAA, 0x52, 0x08, 0x00};
  WriteRegM(0xF000, sizeof(ini21), ini21); // #Enable Page0
  uint8_t ini22[] = {0x08, 0x05, 0x02, 0x05, 0x02};
  WriteRegM(0xB000, sizeof(ini22), ini22); // # RGB I/F Setting
  _bus->writeCommand16(0xB600);
  _bus->write16(0x08);
  _bus->writeCommand16(0xB500);
  _bus->write16(0x50); // ## SDT: //0x6b ?? 480x854  0x50 ?? 480x800
  uint8_t ini24[] = {0x00, 0x00};
  WriteRegM(0xB700, sizeof(ini24), ini24); // ## Gate EQ:
  uint8_t ini25[] = {0x01, 0x05, 0x05, 0x05};
  WriteRegM(0xB800, sizeof(ini25), ini25); // ## Source EQ:
  uint8_t ini26[] = {0x00, 0x00, 0x00};
  WriteRegM(0xBC00, sizeof(ini26), ini26); // # Inversion: Column inversion (NVT)
  uint8_t ini27[] = {0x03, 0x00, 0x00};
  WriteRegM(0xCC00, sizeof(ini27), ini27); // # BOE's Setting(default)
  uint8_t ini28[] = {0x01, 0x84, 0x07, 0x31, 0x00, 0x01};
  WriteRegM(0xBD00, sizeof(ini28), ini28); // # Display Timing:
  //
  uint8_t ini30[] = {0xAA, 0x55, 0x25, 0x01};
  WriteRegM(0xFF00, sizeof(ini30), ini30);
  _bus->writeCommand16(NT35510_TEON);
  _bus->write16(0x00);
  _bus->writeCommand16(NT35510_COLMOD);
  _bus->write16(0x55); // 0x55=16bit Mode
  _bus->writeCommand16(NT35510_SLPOUT);
  _bus->endWrite();

  delay(NT35510_SLPOUT_DELAY);

  _bus->sendCommand16(NT35510_DISPON);

  delay(NT35510_SLPOUT_DELAY);
}
