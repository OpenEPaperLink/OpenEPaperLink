/*******************************************************************************
 * Touch Calibration tool for helping other example touchscreen support
 *
 * For resistive touchscreen, you can follow the display hint for calibration.
 * And then copy the serial output result to touch.h.
 *
 * For capacitive touchscreen, it should no need to calibrate.
 * This tool just help you check the connection and orientation.
 *
 * Resistive touchscreen libraries
 * XPT2046: https://github.com/PaulStoffregen/XPT2046_Touchscreen.git
 * 
 * Capacitive touchscreen libraries
 * TouchLib: https://github.com/mmMicky/TouchLib.git
 ******************************************************************************/

/*******************************************************************************
 * Please config the touch panel in touch.h
 ******************************************************************************/
#include "touch.h"

/*******************************************************************************
 * Start of Arduino_GFX setting
 *
 * Arduino_GFX try to find the settings depends on selected board in Arduino IDE
 * Or you can define the display dev kit not in the board list
 * Defalult pin list for non display dev kit:
 * Arduino Nano, Micro and more: CS:  9, DC:  8, RST:  7, BL:  6, SCK: 13, MOSI: 11, MISO: 12
 * ESP32 various dev board     : CS:  5, DC: 27, RST: 33, BL: 22, SCK: 18, MOSI: 23, MISO: nil
 * ESP32-C3 various dev board  : CS:  7, DC:  2, RST:  1, BL:  3, SCK:  4, MOSI:  6, MISO: nil
 * ESP32-S2 various dev board  : CS: 34, DC: 38, RST: 33, BL: 21, SCK: 36, MOSI: 35, MISO: nil
 * ESP32-S3 various dev board  : CS: 40, DC: 41, RST: 42, BL: 48, SCK: 36, MOSI: 35, MISO: nil
 * ESP8266 various dev board   : CS: 15, DC:  4, RST:  2, BL:  5, SCK: 14, MOSI: 13, MISO: 12
 * Raspberry Pi Pico dev board : CS: 17, DC: 27, RST: 26, BL: 28, SCK: 18, MOSI: 19, MISO: 16
 * RTL8720 BW16 old patch core : CS: 18, DC: 17, RST:  2, BL: 23, SCK: 19, MOSI: 21, MISO: 20
 * RTL8720_BW16 Official core  : CS:  9, DC:  8, RST:  6, BL:  3, SCK: 10, MOSI: 12, MISO: 11
 * RTL8722 dev board           : CS: 18, DC: 17, RST: 22, BL: 23, SCK: 13, MOSI: 11, MISO: 12
 * RTL8722_mini dev board      : CS: 12, DC: 14, RST: 15, BL: 13, SCK: 11, MOSI:  9, MISO: 10
 * Seeeduino XIAO dev board    : CS:  3, DC:  2, RST:  1, BL:  0, SCK:  8, MOSI: 10, MISO:  9
 * Teensy 4.1 dev board        : CS: 39, DC: 41, RST: 40, BL: 22, SCK: 13, MOSI: 11, MISO: 12
 ******************************************************************************/
#include <Arduino_GFX_Library.h>

#define GFX_BL DF_GFX_BL // default backlight pin, you may replace DF_GFX_BL to actual backlight pin

/* More dev device declaration: https://github.com/moononournation/Arduino_GFX/wiki/Dev-Device-Declaration */
#if defined(DISPLAY_DEV_KIT)
Arduino_GFX *gfx = create_default_Arduino_GFX();
#else /* !defined(DISPLAY_DEV_KIT) */

/* More data bus class: https://github.com/moononournation/Arduino_GFX/wiki/Data-Bus-Class */
Arduino_DataBus *bus = create_default_Arduino_DataBus();

/* More display class: https://github.com/moononournation/Arduino_GFX/wiki/Display-Class */
Arduino_GFX *gfx = new Arduino_ILI9341(bus, DF_GFX_RST, 0 /* rotation */, false /* IPS */);

#endif /* !defined(DISPLAY_DEV_KIT) */
/*******************************************************************************
 * End of Arduino_GFX setting
 ******************************************************************************/

int16_t w = -1, h = -1;
int16_t point_x[4] = {-1};
int16_t point_y[4] = {-1};
int16_t current_point = -1, next_point = 0;
int16_t touched_x[4] = {-1};
int16_t touched_y[4] = {-1};

void setup(void)
{
  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  // while(!Serial);
  Serial.println("Touch Calibration");

#ifdef GFX_EXTRA_PRE_INIT
  GFX_EXTRA_PRE_INIT();
#endif

  Serial.println("Init display");
  // Init Display
  if (!gfx->begin())
  {
    Serial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(BLACK);

#ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#endif

  // Init touch device
  w = gfx->width();
  h = gfx->height();
  touch_init(w, h, gfx->getRotation());

  // Top left
  point_x[0] = w / 8;
  point_y[0] = h / 8;
  // Top right
  point_x[1] = point_x[0] * 7;
  point_y[1] = point_y[0];
  // Bottom left
  point_x[2] = point_x[0];
  point_y[2] = point_y[0] * 7;
  // Bottom right
  point_x[3] = point_x[1];
  point_y[3] = point_y[2];

  gfx->setCursor(0, 0);
  gfx->setTextColor(RED);
  gfx->setTextSize(2);
  gfx->println("Touch Calibration");
}

void loop()
{
  if (current_point != next_point)
  {
    current_point = next_point;

    gfx->drawLine(
        point_x[current_point] - 5,
        point_y[current_point] - 5,
        point_x[current_point] + 5,
        point_y[current_point] + 5,
        RED);
    gfx->drawLine(
        point_x[current_point] + 5,
        point_y[current_point] - 5,
        point_x[current_point] - 5,
        point_y[current_point] + 5,
        RED);
  }

  if (touch_touched())
  {
    int32_t total_x = touch_raw_x, total_y = touch_raw_y;
    int count = 1;
    while (touch_touched())
    {
      total_x += touch_raw_x;
      total_y += touch_raw_y;
      count++;
      // Serial.printf("touch_raw_x: %d, touch_raw_y: %d, count: %d\n", touch_raw_x, touch_raw_y, count);
    }
    touched_x[current_point] = total_x / count;
    touched_y[current_point] = total_y / count;
    // Serial.printf("touched_x: %d, touched_y: %d\n", touched_x[current_point], touched_y[current_point]);

    if (current_point == 3)
    {
      Serial.printf("touched_x[0]: %d, touched_y[0]: %d\n", touched_x[0], touched_y[0]);
      Serial.printf("touched_x[1]: %d, touched_y[1]: %d\n", touched_x[1], touched_y[1]);
      Serial.printf("touched_x[2]: %d, touched_y[2]: %d\n", touched_x[2], touched_y[2]);
      Serial.printf("touched_x[3]: %d, touched_y[3]: %d\n", touched_x[3], touched_y[3]);
      uint16_t delta_x = (touched_x[0] > touched_x[1]) ? (touched_x[0] - touched_x[1]) : (touched_x[1] - touched_x[0]);
      uint16_t delta_y = (touched_y[0] > touched_y[1]) ? (touched_y[0] - touched_y[1]) : (touched_y[1] - touched_y[0]);

      if (delta_x > delta_y)
      {
        touch_swap_xy = false;
        touch_map_x1 = (touched_x[0] + touched_x[2]) / 2;
        touch_map_x2 = (touched_x[1] + touched_x[3]) / 2;
        touch_map_y1 = (touched_y[0] + touched_y[1]) / 2;
        touch_map_y2 = (touched_y[2] + touched_y[3]) / 2;
      }
      else
      {
        touch_swap_xy = true;
        touch_map_x1 = (touched_y[0] + touched_y[2]) / 2;
        touch_map_x2 = (touched_y[1] + touched_y[3]) / 2;
        touch_map_y1 = (touched_x[0] + touched_x[1]) / 2;
        touch_map_y2 = (touched_x[2] + touched_x[3]) / 2;
      }

      if (touch_map_x1 > touch_map_x2)
      {
        delta_x = (touch_map_x1 - touch_map_x2) / 6;
        touch_map_x1 += delta_x;
        touch_map_x2 -= delta_x;
      }
      else
      {
        delta_x = (touch_map_x2 - touch_map_x1) / 6;
        touch_map_x1 -= delta_x;
        touch_map_x2 += delta_x;
      }

      if (touch_map_y1 > touch_map_y2)
      {
        delta_y = (touch_map_y1 - touch_map_y2) / 6;
        touch_map_y1 += delta_y;
        touch_map_y2 -= delta_y;
      }
      else
      {
        delta_y = (touch_map_y2 - touch_map_y1) / 6;
        touch_map_y1 -= delta_y;
        touch_map_y2 += delta_y;
      }

      Serial.printf("bool touch_swap_xy = %s;\n", touch_swap_xy ? "true" : "false");
      Serial.printf("int16_t touch_map_x1 = %d;\n", touch_map_x1);
      Serial.printf("int16_t touch_map_x2 = %d;\n", touch_map_x2);
      Serial.printf("int16_t touch_map_y1 = %d;\n", touch_map_y1);
      Serial.printf("int16_t touch_map_y2 = %d;\n", touch_map_y2);

      gfx->setCursor(0, point_y[0] + 10);
      gfx->setTextColor(WHITE);
      gfx->setTextSize(1);
      gfx->printf("bool touch_swap_xy = %s;\n", touch_swap_xy ? "true" : "false");
      gfx->printf("int16_t touch_map_x1 = %d;\n", touch_map_x1);
      gfx->printf("int16_t touch_map_x2 = %d;\n", touch_map_x2);
      gfx->printf("int16_t touch_map_y1 = %d;\n", touch_map_y1);
      gfx->printf("int16_t touch_map_y2 = %d;\n", touch_map_y2);

      // wait next touch to continue
      while (!touch_touched())
        ;

      gfx->fillScreen(BLACK);
      gfx->setCursor(0, 0);
      gfx->setTextColor(RED);
      gfx->setTextSize(2);
      gfx->println("Touch Calibration");
    }

    next_point = (current_point == 3) ? 0 : (current_point + 1);
  }

  delay(100);
}
