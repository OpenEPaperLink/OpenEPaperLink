#include "Arduino_DataBus.h"
#if !defined(LITTLE_FOOT_PRINT)

#include "Arduino_ILI9488_3bit.h"

Arduino_ILI9488_3bit::Arduino_ILI9488_3bit(Arduino_DataBus *bus, int8_t rst, uint8_t r, bool ips)
    : Arduino_G(ILI9488_TFTWIDTH, ILI9488_TFTHEIGHT), _bus(bus), _rst(rst), _rotation(r), _ips(ips)
{
}

bool Arduino_ILI9488_3bit::begin(int32_t speed)
{
  if (!_bus->begin(speed))
  {
    return false;
  }

  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(ILI9488_RST_DELAY);
    digitalWrite(_rst, HIGH);
    delay(ILI9488_RST_DELAY);
  }
  else
  {
    // Software Rest
    _bus->sendCommand(ILI9488_SWRESET);
    delay(ILI9488_RST_DELAY);
  }

  _bus->batchOperation(ili9488_init_operations, sizeof(ili9488_init_operations));

  _bus->beginWrite();
  _bus->writeC8D8(0x3A, 0x01); // Interface Pixel Format, 3 bit
  _bus->endWrite();

  if (_ips)
  {
    _bus->sendCommand(ILI9488_INVON);
  }
  else
  {
    _bus->sendCommand(ILI9488_INVOFF);
  }

  uint16_t r;
  // setRotation
  switch (_rotation)
  {
  case 1:
    r = (ILI9488_MADCTL_BGR | ILI9488_MADCTL_MV);
    break;
  case 2:
    r = (ILI9488_MADCTL_BGR | ILI9488_MADCTL_MY);
    break;
  case 3:
    r = (ILI9488_MADCTL_BGR | ILI9488_MADCTL_MV | ILI9488_MADCTL_MX | ILI9488_MADCTL_MY);
    break;
  default: // case 0:
    r = (ILI9488_MADCTL_BGR | ILI9488_MADCTL_MX);
    break;
  }
  _bus->beginWrite();
  _bus->writeC8D8(ILI9488_MADCTL, r);
  _bus->endWrite();

  return true;
}

void Arduino_ILI9488_3bit::drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg)
{
  printf("Not Implemented drawBitmap()");
  UNUSED(x);
  UNUSED(y);
  UNUSED(bitmap);
  UNUSED(w);
  UNUSED(h);
  UNUSED(color);
  UNUSED(bg);
}

void Arduino_ILI9488_3bit::drawIndexedBitmap(int16_t x, int16_t y, uint8_t *bitmap, uint16_t *color_index, int16_t w, int16_t h, int16_t x_skip)
{
  printf("Not Implemented drawIndexedBitmap()");
  UNUSED(x);
  UNUSED(y);
  UNUSED(bitmap);
  UNUSED(color_index);
  UNUSED(w);
  UNUSED(h);
  UNUSED(x_skip);
}

void Arduino_ILI9488_3bit::draw3bitRGBBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h)
{
  UNUSED(x);
  UNUSED(y);
  _bus->beginWrite();
  writeAddrWindow(0, 0, w, h);
  _bus->writeBytes(bitmap, w * h / 2);
  _bus->endWrite();
}

void Arduino_ILI9488_3bit::draw16bitRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h)
{
  printf("Not Implemented draw16bitRGBBitmap()");
  UNUSED(x);
  UNUSED(y);
  UNUSED(bitmap);
  UNUSED(w);
  UNUSED(h);
}

void Arduino_ILI9488_3bit::draw24bitRGBBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h)
{
  printf("Not Implemented draw24bitRGBBitmap()");
  UNUSED(x);
  UNUSED(y);
  UNUSED(bitmap);
  UNUSED(w);
  UNUSED(h);
}

void Arduino_ILI9488_3bit::invertDisplay(bool i)
{
  _bus->sendCommand(i ? ILI9488_INVON : ILI9488_INVOFF);
}

void Arduino_ILI9488_3bit::displayOn(void)
{
  _bus->sendCommand(ILI9488_SLPOUT);
  delay(ILI9488_SLPOUT_DELAY);
}

void Arduino_ILI9488_3bit::displayOff(void)
{
  _bus->sendCommand(ILI9488_SLPIN);
  delay(ILI9488_SLPIN_DELAY);
}

void Arduino_ILI9488_3bit::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
  _bus->writeC8D16D16(ILI9488_CASET, x, x + w - 1);
  _bus->writeC8D16D16(ILI9488_PASET, y, y + h - 1);
  _bus->writeCommand(ILI9488_RAMWR);
}

#endif // !defined(LITTLE_FOOT_PRINT)
