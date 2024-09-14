/*******************************************************************************
 * LVGL Hello NeoPixel demo
 * This is a LVGL Hello World with NeoPixel matrix and Unicode text display
 *
 * Dependent libraries:
 * LVGL: https://github.com/lvgl/lvgl.git
 * Adafruit_NeoPixel: https://github.com/adafruit/Adafruit_NeoPixel.git
 * 
 * Font:
 * https://github.com/Warren2060/Chill-Bitmap.git
 *
 * LVGL Configuration file:
 * Copy your_arduino_path/libraries/lvgl/lv_conf_template.h
 * to your_arduino_path/libraries/lv_conf.h
 *
 * In lv_conf.h around line 15, enable config file:
 * #if 1 // Set it to "1" to enable content
 *
 * Then find and set:
 * #define LV_COLOR_DEPTH     16
 * #define LV_TICK_CUSTOM     1
 * #define LV_COLOR_16_SWAP   0
 ******************************************************************************/
#include <lvgl.h>
#include <Arduino_GFX_Library.h>

// all settings in header file
#include "Adafruit_NeoPixel_GFX.h"
Arduino_GFX *gfx = new Adafruit_NeoPixel_GFX();
int16_t x;
uint16_t w, tw;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;
static lv_disp_drv_t disp_drv;

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);

  lv_disp_flush_ready(disp);
}

void setup()
{
  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  // while(!Serial);
  Serial.println("LVGL Hello NeoPixel Demo");

#ifdef GFX_EXTRA_PRE_INIT
  GFX_EXTRA_PRE_INIT();
#endif

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

  lv_init();

#ifdef ESP32
  disp_draw_buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * NEOPIXEL_WIDTH * NEOPIXEL_HEIGHT, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
#else
  disp_draw_buf = (lv_color_t *)malloc(sizeof(lv_color_t) * NEOPIXEL_WIDTH * NEOPIXEL_HEIGHT);
#endif
  if (!disp_draw_buf)
  {
    Serial.println("LVGL disp_draw_buf allocate failed!");
  }
  else
  {
    lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, NEOPIXEL_WIDTH * NEOPIXEL_HEIGHT);

    /* Initialize the display */
    lv_disp_drv_init(&disp_drv);
    /* Change the following line to your display resolution */
    disp_drv.hor_res = NEOPIXEL_WIDTH;
    disp_drv.ver_res = NEOPIXEL_HEIGHT;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    /* Initialize the (dummy) input device driver */
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);

    /* Set black background */
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
    /* Create label */
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_width(label, NEOPIXEL_WIDTH);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT );
    lv_label_set_text(label, "LVGL Hello NeoPixel! 《陈亮手痕定律》 点阵屏测试");
    /* Set font */
    LV_FONT_DECLARE(ui_font_Chill7);
    lv_obj_set_style_text_font(label, &ui_font_Chill7, LV_PART_MAIN| LV_STATE_DEFAULT);
    /* Set scrolling */
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_anim_speed(label, 10, LV_STATE_DEFAULT);

    Serial.println("Setup done");
  }
}

void loop()
{
  lv_timer_handler(); /* let the GUI do its work */
  delay(5);
}