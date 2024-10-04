#include "../Arduino_DataBus.h"

#if defined(ESP32) && (CONFIG_IDF_TARGET_ESP32S3)

#ifndef _ARDUINO_RGB_DISPLAY_H_
#define _ARDUINO_RGB_DISPLAY_H_

#include "../Arduino_GFX.h"
#include "../databus/Arduino_ESP32RGBPanel.h"


class Arduino_RGB_Display : public Arduino_GFX
{
public:
    Arduino_RGB_Display(
        int16_t w, int16_t h, Arduino_ESP32RGBPanel *rgbpanel, uint8_t r = 0, bool auto_flush = true,
        Arduino_DataBus *bus = NULL, int8_t rst = GFX_NOT_DEFINED, const uint8_t *init_operations = NULL, size_t init_operations_len = GFX_NOT_DEFINED);

    bool begin(int32_t speed = GFX_NOT_DEFINED) override;
    void writePixelPreclipped(int16_t x, int16_t y, uint16_t color) override;
    void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) override;
    void writeFastVLineCore(int16_t x, int16_t y, int16_t h, uint16_t color);
    void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) override;
    void writeFastHLineCore(int16_t x, int16_t y, int16_t w, uint16_t color);
    void writeFillRectPreclipped(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override;
    void drawIndexedBitmap(int16_t x, int16_t y, uint8_t *bitmap, uint16_t *color_index, int16_t w, int16_t h, int16_t x_skip = 0) override;
    void draw16bitRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h) override;
    void draw16bitBeRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h) override;
    void flush(void) override;

    uint16_t *getFramebuffer();

    void XL_pinMode(uint8_t pin, uint8_t mode);
    void XL_digitalWrite(uint8_t pin, uint8_t val);
    int XL_digitalRead(uint8_t pin);

protected:
    uint16_t *_framebuffer;
    size_t _framebuffer_size;
    Arduino_ESP32RGBPanel *_rgbpanel;
    bool _auto_flush;
    Arduino_DataBus *_bus;
    int8_t _rst;
    const uint8_t *_init_operations;
    size_t _init_operations_len;
    int16_t MAX_X, MAX_Y;

private:
};

#endif // _ARDUINO_RGB_DISPLAY_H_

#endif // #if defined(ESP32) && (CONFIG_IDF_TARGET_ESP32S3)
