#include "../Arduino_DataBus.h"
#if !defined(LITTLE_FOOT_PRINT)

#include "../Arduino_GFX.h"
#include "Arduino_Canvas_Mono.h"

Arduino_Canvas_Mono::Arduino_Canvas_Mono(int16_t w, int16_t h, Arduino_G *output, int16_t output_x, int16_t output_y)
    : Arduino_GFX(w, h), _output(output), _output_x(output_x), _output_y(output_y)
{
}

Arduino_Canvas_Mono::~Arduino_Canvas_Mono()
{
  if (_framebuffer)
  {
    free(_framebuffer);
  }
}

bool Arduino_Canvas_Mono::begin(int32_t speed)
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
    size_t s = (_width + 7) / 8 * _height;
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

void Arduino_Canvas_Mono::writePixelPreclipped(int16_t x, int16_t y, uint16_t color)
{
  int16_t w = (_width + 7) / 8;
  int32_t pos = y * w + x / 8;
  if (color & 0b1000010000010000)
  {
    _framebuffer[pos] |= 0x80 >> (x & 7);
  }
  else
  {
    _framebuffer[pos] &= ~(0x80 >> (x & 7));
  }
}

void Arduino_Canvas_Mono::flush()
{
  _output->drawBitmap(_output_x, _output_y, _framebuffer, _width, _height, WHITE, BLACK);
}

uint8_t *Arduino_Canvas_Mono::getFramebuffer()
{
  return _framebuffer;
}

#endif // !defined(LITTLE_FOOT_PRINT)
