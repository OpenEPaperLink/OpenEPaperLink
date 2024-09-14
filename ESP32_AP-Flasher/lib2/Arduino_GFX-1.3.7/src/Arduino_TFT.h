/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 */
#ifndef _ARDUINO_TFT_H_
#define _ARDUINO_TFT_H_

#include "Arduino_DataBus.h"
#include "Arduino_GFX.h"

class Arduino_TFT : public Arduino_GFX
{
public:
  Arduino_TFT(Arduino_DataBus *bus, int8_t rst, uint8_t r, bool ips, int16_t w, int16_t h, uint8_t col_offset1, uint8_t row_offset1, uint8_t col_offset2, uint8_t row_offset2);

  // This SHOULD be defined by the subclass:
  void setRotation(uint8_t r) override;

  // This MUST be defined by the subclass:
  // and also protected function: tftInit()
  virtual void writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h) = 0;

  bool begin(int32_t speed = GFX_NOT_DEFINED);
  void startWrite(void) override;
  void endWrite(void) override;
  void writePixelPreclipped(int16_t x, int16_t y, uint16_t color) override;
  void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) override;
  void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) override;
  void writeFillRectPreclipped(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override;

  virtual void writeRepeat(uint16_t color, uint32_t len);

  void setAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h);
  virtual void writeColor(uint16_t color);

// TFT optimization code, too big for ATMEL family
#if !defined(LITTLE_FOOT_PRINT)
  virtual void writePixels(uint16_t *data, uint32_t size);
  virtual void writeIndexedPixels(uint8_t *bitmap, uint16_t *color_index, uint32_t len);
  virtual void writeIndexedPixelsDouble(uint8_t *bitmap, uint16_t *color_index, uint32_t len);

  void writeBytes(uint8_t *data, uint32_t size);
  void pushColor(uint16_t color);

  void writeSlashLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) override;
  void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color, uint16_t bg) override;
  void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg) override;
  void drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h) override;
  void drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h) override;
  void drawIndexedBitmap(int16_t x, int16_t y, uint8_t *bitmap, uint16_t *color_index, int16_t w, int16_t h, int16_t x_skip = 0) override;
  void draw16bitRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, uint8_t *mask, int16_t w, int16_t h) override;
  void draw16bitRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h) override;
  void draw16bitRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h) override;
  void draw16bitBeRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h) override;
  void draw24bitRGBBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h) override;
  void draw24bitRGBBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h) override;
  void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg) override;
#endif // !defined(LITTLE_FOOT_PRINT)

protected:
  virtual void tftInit() = 0;

  Arduino_DataBus *_bus;
  int8_t _rst;
  bool _ips;
  uint8_t COL_OFFSET1, ROW_OFFSET1;
  uint8_t COL_OFFSET2, ROW_OFFSET2;
  uint8_t _xStart, _yStart;
  int16_t _currentX, _currentY;
  uint16_t _currentW, _currentH;
  int8_t _override_datamode = GFX_NOT_DEFINED;

private:
};

#endif
