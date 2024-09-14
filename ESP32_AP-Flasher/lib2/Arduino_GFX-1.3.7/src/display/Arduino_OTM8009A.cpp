#include "Arduino_OTM8009A.h"

Arduino_OTM8009A::Arduino_OTM8009A(
    Arduino_DataBus *bus, int8_t rst, uint8_t r,
    bool ips, int16_t w, int16_t h,
    uint8_t col_offset1, uint8_t row_offset1, uint8_t col_offset2, uint8_t row_offset2)
    : Arduino_TFT(bus, rst, r, ips, w, h, col_offset1, row_offset1, col_offset2, row_offset2)
{
}

bool Arduino_OTM8009A::begin(int32_t speed)
{
  return Arduino_TFT::begin(speed);
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_OTM8009A::setRotation(uint8_t r)
{
  Arduino_TFT::setRotation(r);
  switch (_rotation)
  {
  case 3:
    r = (OTM8009A_MADCTL_MY | OTM8009A_MADCTL_MV);
    break;
  case 2:
    r = (OTM8009A_MADCTL_MY | OTM8009A_MADCTL_MX);
    break;
  case 1:
    r = (OTM8009A_MADCTL_MX | OTM8009A_MADCTL_MV);
    break;
  default: // case 0:
    r = 0;
    break;
  }
  _bus->beginWrite();
  _bus->writeCommand16(OTM8009A_MADCTR);
  _bus->write16(r);
  _bus->endWrite();
}

void Arduino_OTM8009A::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  if ((x != _currentX) || (w != _currentW))
  {
    _currentX = x;
    _currentW = w;
    _data16.value = x + _xStart;
    _bus->writeC16D16(OTM8009A_CASET, _data16.msb);
    _bus->writeC16D16(OTM8009A_CASET + 1, _data16.lsb);
    _data16.value += w - 1;
    _bus->writeC16D16(OTM8009A_CASET + 2, _data16.msb);
    _bus->writeC16D16(OTM8009A_CASET + 3, _data16.lsb);
  }

  if ((y != _currentY) || (h != _currentH))
  {
    _currentY = y;
    _currentH = h;
    _data16.value = y + _yStart;
    _bus->writeC16D16(OTM8009A_PASET, _data16.msb);
    _bus->writeC16D16(OTM8009A_PASET + 1, _data16.lsb);
    _data16.value += h - 1;
    _bus->writeC16D16(OTM8009A_PASET + 2, _data16.msb);
    _bus->writeC16D16(OTM8009A_PASET + 3, _data16.lsb);
  }

  _bus->writeCommand16(OTM8009A_RAMWR); // write to RAM
}

void Arduino_OTM8009A::invertDisplay(bool i)
{
  // Not Implemented
  UNUSED(i);
}

void Arduino_OTM8009A::displayOn(void)
{
  // Not Implemented
}

void Arduino_OTM8009A::displayOff(void)
{
  // Not Implemented
}

void Arduino_OTM8009A::WriteRegM(uint16_t adr, uint16_t len, uint8_t dat[])
{
  for (uint16_t i = 0; i < len; i++)
  {
    _bus->writeCommand16(adr++);
    _bus->write16(dat[i]);
  }
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Arduino_OTM8009A::tftInit()
{
  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(OTM8009A_RST_DELAY);
    digitalWrite(_rst, HIGH);
    delay(OTM8009A_RST_DELAY);
  }
  else
  {
    // Software Rest
  }

  //************* OTM8009A**********//
  _bus->beginWrite();
  // 3.97inch OTM8009 Init 20190116
  /* Enter CMD2 */
  uint8_t ini01[] = {0x80, 0x09, 0x01, 0x01};
  WriteRegM(0xFF00, sizeof(ini01), ini01);
  /* Enter Orise Command2 */
  uint8_t ini02[] = {0x80, 0x09};
  WriteRegM(0xFF80, sizeof(ini02), ini02);

  /* Command not documented */
  _bus->writeCommand16(0xf5b6);
  _bus->write16(0x06); //??

  /* Source Driver Precharge Control */
  uint8_t ini03[] = {0x30, 0x83};
  WriteRegM(0xC480, sizeof(ini03), ini03);

  /* Command not documented: 0xC48A */
  _bus->writeCommand16(0xC48A);
  _bus->write16(0x40);

  /* Source Driver Timing Setting */
  _bus->writeCommand16(0xC0A2 + 1);
  _bus->write16(0x1B);

  /* Command not documented */
  _bus->writeCommand16(0xc0ba); //--> (0xc0b4); // column inversion //  2013.12.16 modify
  _bus->write16(0x50);

  /* Oscillator Adjustment for Idle/Normal mode */
  _bus->writeCommand16(0xC181);
  _bus->write16(0x66); /* 65Hz */

  /* RGB Video Mode Setting */
  _bus->writeCommand16(0xC1A1);
  _bus->write16(0x0E);

  /* Power Control Setting 1 */
  _bus->writeCommand16(0xC580 + 2);
  _bus->write16(0x83);

  /* Power Control Setting 2 for Normal Mode */
  uint8_t ini04[] = {0x96, 0x2B, 0x01, 0x33, 0x34};
  WriteRegM(0xC590, sizeof(ini04), ini04);

  /* Power Control Setting 4 for DC Voltage */
  _bus->writeCommand16(0xC5B0 + 1);
  _bus->write16(0xa9);

  /* GOA VST Setting */
  uint8_t ini05[] = {0x86, 0x01, 0x00, 0x85, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  WriteRegM(0xCE80, sizeof(ini05), ini05);

  /* GOA CLKA1 Setting */
  uint8_t ini06[] = {0x18, 0x04, 0x03, 0x21, 0x00, 0x00, 0x00};
  WriteRegM(0xCEA0, sizeof(ini06), ini06);

  /* GOA CLKA2 Setting */
  uint8_t ini07[] = {0x18, 0x03, 0x03, 0x22, 0x00, 0x00, 0x00};
  WriteRegM(0xCEA7, sizeof(ini07), ini07);

  /* GOA CLKA3 Setting */
  uint8_t ini08[] = {0x18, 0x02, 0x03, 0x23, 0x00, 0x00, 0x00};
  WriteRegM(0xCEB0, sizeof(ini08), ini08);

  /* GOA CLKA4 Setting */
  uint8_t ini09[] = {0x18, 0x01, 0x03, 0x24, 0x00, 0x00, 0x00};
  WriteRegM(0xCEB7, sizeof(ini09), ini09);

  /* GOA ECLK Setting */
  uint8_t ini10[] = {0x01, 0x01, 0x20, 0x20, 0x00, 0x00};
  WriteRegM(0xCFC0, sizeof(ini10), ini10);

  /* GOA Other Options 1 */
  _bus->writeCommand16(0xCFC6); // cfc7[7:0] : 00, vstmask, vendmask, 00, dir1, dir2 (0=VGL, 1=VGH)
  _bus->write16(0x01);

  /* GOA Signal Toggle Option Setting */
  uint8_t ini11[] = {0x00, 0x00, 0x00};
  WriteRegM(0xCFC7, sizeof(ini11), ini11); // cfc8[7:0] : reg_goa_gnd_opt, reg_goa_dpgm_tail_set, reg_goa_f_gating_en, reg_goa_f_odd_gating, toggle_mod1, 2, 3, 4

  /* Command not documented: 0xCFD0 */
  _bus->writeCommand16(0xCFD0); // cfd1[7:0] : 0000000, reg_goa_frame_odd_high
  _bus->write16(0x00);

  /* Panel Control Setting 5 */
  uint8_t ini12[] = {0x00, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  WriteRegM(0xCBC0, sizeof(ini12), ini12); // cbc[7:0] : enmode H-byte of sig  (pwrof_0, pwrof_1, norm, pwron_4 )

  /* Panel Control Setting 6 */
  uint8_t ini13[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00};
  WriteRegM(0xCBD0, sizeof(ini13), ini13); // cbd1[7:0] : enmode H-byte of sig16 (pwrof_0, pwrof_1, norm, pwron_4 )

  /* Panel Control Setting 7 */
  uint8_t ini14[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  WriteRegM(0xCBE0, sizeof(ini14), ini14); // cbe1[7:0] : enmode H-byte of sig31 (pwrof_0, pwrof_1, norm, pwron_4 )

  /* Panel U2D Setting 1 */
  // cc8x
  uint8_t ini15[] = {0x00, 0x26, 0x09, 0x0B, 0x01, 0x25, 0x00, 0x00, 0x00, 0x00};
  WriteRegM(0xCC80, sizeof(ini15), ini15); // cc81[7:0] : reg setting for signal01 selection with u2d mode

  /* Panel U2D Setting 2 */
  // cc9x
  uint8_t ini16[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x26, 0x0A, 0x0C, 0x02};
  WriteRegM(0xCC90, sizeof(ini16), ini16); // cc91[7:0] : reg setting for signal11 selection with u2d mode

  /* Panel U2D Setting 3 */
  // ccax
  uint8_t ini17[] = {0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  WriteRegM(0xCCA0, sizeof(ini17), ini17); // cca1[7:0] : reg setting for signal26 selection with u2d mode

  /* Command not documented: 0x3A00 */
  _bus->writeCommand16(0x3A00); // ccaa[7:0] : reg setting for signal35 selection with u2d mode
  _bus->write16(0x55);          // 0x55

  _bus->endWrite();

  /* Sleep out */
  _bus->sendCommand16(0x1100);
  delay(100);

  /* Display on */
  _bus->sendCommand16(0x2900);
  delay(50);
}
