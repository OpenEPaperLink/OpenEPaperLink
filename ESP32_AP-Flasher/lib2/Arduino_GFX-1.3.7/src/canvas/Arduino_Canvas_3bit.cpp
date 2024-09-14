#include "../Arduino_DataBus.h"
#if !defined(LITTLE_FOOT_PRINT)

#include "../Arduino_GFX.h"
#include "Arduino_Canvas_3bit.h"

Arduino_Canvas_3bit::Arduino_Canvas_3bit(int16_t w, int16_t h, Arduino_G *output, int16_t output_x, int16_t output_y)
    : Arduino_GFX(w, h), _output(output), _output_x(output_x), _output_y(output_y)
{
}

Arduino_Canvas_3bit::~Arduino_Canvas_3bit()
{
  if (_framebuffer)
  {
    free(_framebuffer);
  }
}

bool Arduino_Canvas_3bit::begin(int32_t speed)
{
  if (speed != GFX_SKIP_OUTPUT_BEGIN)
  {
    if (!_output->begin(speed))
    {
      return false;
    }
  }

  if (!_framebuffer)
  {
    size_t s = (_width * _height + 1) / 2;
#if defined(ESP32)
    if (psramFound())
    {
      _framebuffer = (uint8_t *)ps_malloc(s);
    }
    else
    {
      _framebuffer = (uint8_t *)malloc(s);
    }
#else
    _framebuffer = (uint8_t *)malloc(s);
#endif
    if (!_framebuffer)
    {
      return false;
    }
  }

  return true;
}

void Arduino_Canvas_3bit::writePixelPreclipped(int16_t x, int16_t y, uint16_t color)
{
  int32_t pos = x + (y * _width);
  int32_t idx = pos >> 1;
  uint8_t c = (((color & 0b1000000000000000) ? 0b100 : 0) |
               ((color & 0b0000010000000000) ? 0b010 : 0) |
               ((color & 0b0000000000010000) ? 0b001 : 0));
  if (pos & 1)
  {
    _framebuffer[idx] = (_framebuffer[idx] & 0b00111000) | c;
  }
  else
  {
    _framebuffer[idx] = (_framebuffer[idx] & 0b00000111) | (c << 3);
  }
}

void Arduino_Canvas_3bit::flush()
{
  _output->draw3bitRGBBitmap(_output_x, _output_y, _framebuffer, _width, _height);
}

uint8_t *Arduino_Canvas_3bit::getFramebuffer()
{
  return _framebuffer;
}

#endif // !defined(LITTLE_FOOT_PRINT)
