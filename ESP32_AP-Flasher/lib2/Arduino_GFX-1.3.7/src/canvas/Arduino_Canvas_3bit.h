#include "../Arduino_DataBus.h"
#if !defined(LITTLE_FOOT_PRINT)

#ifndef _ARDUINO_CANVAS_3BIT_H_
#define _ARDUINO_CANVAS_3BIT_H_

#include "../Arduino_GFX.h"

class Arduino_Canvas_3bit : public Arduino_GFX
{
public:
  Arduino_Canvas_3bit(int16_t w, int16_t h, Arduino_G *output, int16_t output_x = 0, int16_t output_y = 0);
  ~Arduino_Canvas_3bit();

  bool begin(int32_t speed = GFX_NOT_DEFINED) override;
  void writePixelPreclipped(int16_t x, int16_t y, uint16_t color) override;
  void flush(void) override;

  uint8_t *getFramebuffer();

protected:
  uint8_t *_framebuffer = nullptr;
  Arduino_G *_output = nullptr;
  int16_t _output_x, _output_y;

private:
};

#endif // _ARDUINO_CANVAS_3BIT_H_

#endif // !defined(LITTLE_FOOT_PRINT)
