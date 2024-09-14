#include "../Arduino_DataBus.h"
#if !defined(LITTLE_FOOT_PRINT)

#ifndef _ARDUINO_CANVAS_INDEXED_H_
#define _ARDUINO_CANVAS_INDEXED_H_

#include "../Arduino_GFX.h"

#define COLOR_IDX_SIZE 256

class Arduino_Canvas_Indexed : public Arduino_GFX
{
public:
  Arduino_Canvas_Indexed(int16_t w, int16_t h, Arduino_G *output, int16_t output_x = 0, int16_t output_y = 0, uint8_t mask_level = 0);
  ~Arduino_Canvas_Indexed();

  bool begin(int32_t speed = GFX_NOT_DEFINED) override;
  void writePixelPreclipped(int16_t x, int16_t y, uint16_t color) override;
  void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) override;
  void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) override;
  void drawIndexedBitmap(int16_t x, int16_t y, uint8_t *bitmap, uint16_t *color_index, int16_t w, int16_t h, int16_t x_skip = 0) override;
  void drawIndexedBitmap(int16_t x, int16_t y, uint8_t *bitmap, uint16_t *color_index, uint8_t chroma_key, int16_t w, int16_t h, int16_t x_skip = 0) override;
  void flush(void) override;

  uint8_t *getFramebuffer();
  uint16_t *getColorIndex();
  void setDirectUseColorIndex(bool isEnable);

  uint8_t get_color_index(uint16_t color);
  uint16_t get_index_color(uint8_t idx);
  void raise_mask_level();

protected:
  uint8_t *_framebuffer = nullptr;
  Arduino_G *_output = nullptr;
  int16_t _output_x, _output_y;
  uint16_t _color_index[COLOR_IDX_SIZE];
  uint8_t _indexed_size = 0;
  bool _isDirectUseColorIndex = false;

  uint8_t _current_mask_level;
  uint16_t _color_mask;
#define MAXMASKLEVEL 3
  uint16_t mask_level_list[MAXMASKLEVEL] = {
      0b1111111111111111, // 16-bit, 65536 colors
      0b1111011110011110, // 12-bit, 4096 colors
      0b1100011000011000  // 7-bit, 128 colors
  };

private:
};

#endif // _ARDUINO_CANVAS_INDEXED_H_

#endif // !defined(LITTLE_FOOT_PRINT)
