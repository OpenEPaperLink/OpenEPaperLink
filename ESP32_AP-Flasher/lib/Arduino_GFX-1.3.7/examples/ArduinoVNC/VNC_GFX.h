#ifndef _VNC_GFX_H_
#define _VNC_GFX_H_

#include "Arduino_GFX_Library.h"
#include "VNC.h"
#include "VNC_config.h"

#ifdef ESP32
// #define SEPARATE_DRAW_TASK
#endif

#ifdef SEPARATE_DRAW_TASK

#define NUMBER_OF_DRAW_BUFFER 64

typedef struct
{
  xQueueHandle xqh;
  Arduino_GFX *gfx;
} ParamDrawTask;

typedef struct
{
  int16_t x;
  int16_t y;
  int16_t w;
  int16_t h;
  bool isBitmap;
  uint16_t *buf;
} DrawData;

static xQueueHandle _xqh;
static ParamDrawTask _pDrawTask;
static TaskHandle_t _draw_task_handle;

static DrawData drawDatas[NUMBER_OF_DRAW_BUFFER];
static int draw_queue_cnt = 0;

static void queueDrawTask(uint32_t x, uint32_t y, uint32_t w, uint32_t h, bool isBitmap, uint16_t *d)
{
  log_i("queueDrawTask start.");
  DrawData *dd = &drawDatas[draw_queue_cnt % NUMBER_OF_DRAW_BUFFER];
  dd->x = x;
  dd->y = y;
  dd->w = w;
  dd->h = h;
  dd->isBitmap = isBitmap;
  log_i("copy data start.");
  uint16_t *p = &dd->buf[0];
  if (isBitmap)
  {
    log_i("copy bitmap.");
    int i = w * h;
    while (i--)
    {
      *p++ = *d++;
    }
  }
  else
  {
    *p = *d;
  }
  log_i("copy data end.");

  ++draw_queue_cnt;

  log_i("xQueueSend start.");
  xQueueSend(_xqh, &dd, portMAX_DELAY);
  log_i("xQueueSend end.");

  log_i("queueDrawTask end.");
}

static void draw_task(void *arg)
{
  DrawData *dd;
  log_i("draw_task start.");
  while (xQueueReceive(_xqh, &dd, portMAX_DELAY))
  {
    log_i("draw_task work start: x: %d, y: %d, w: %d, h: %d.", dd->x, dd->y, dd->w, dd->h);
    if (dd->isBitmap)
    {
      gfx->draw16bitBeRGBBitmap(dd->x, dd->y, &dd->buf[0], dd->w, dd->h);
    }
    else
    {
      gfx->fillRect(dd->x, dd->y, dd->w, dd->h, dd->buf[0]);
    }
    log_i("draw_task work end.");
  }
  vQueueDelete(_xqh);
  log_i("draw_task end.");
  vTaskDelete(NULL);
}

void draw_task_setup()
{
  _xqh = xQueueCreate(NUMBER_OF_DRAW_BUFFER, sizeof(DrawData));
  _pDrawTask.xqh = _xqh;
  _pDrawTask.gfx = gfx;

  log_i("xTaskCreatePinnedToCore start");
  xTaskCreatePinnedToCore(
      (TaskFunction_t)draw_task,
      (const char *const)"Draw Task",
      (const uint32_t)1600,
      (void *const)&_pDrawTask,
      (UBaseType_t)configMAX_PRIORITIES - 1,
      (TaskHandle_t *const)&_draw_task_handle,
      (const BaseType_t)0);
  log_i("xTaskCreatePinnedToCore end");

  for (int i = 0; i < NUMBER_OF_DRAW_BUFFER; i++)
  {
    if (!drawDatas[i].buf)
    {
      drawDatas[i].buf = (uint16_t *)malloc(FB_SIZE * 2);
    }
    if (drawDatas[i].buf)
    {
      log_i("#%d draw buffer allocated.", i);
    }
    else
    {
      log_e("#%d draw buffer allocat failed.", i);
    }
  }
}

#endif

class VNC_GFX : public VNCdisplay
{
public:
  VNC_GFX(Arduino_GFX *gfx)
  {
    _gfx = gfx;
  }

  bool hasCopyRect(void)
  {
    return false;
  }

  uint32_t getHeight(void)
  {
    return _gfx->height();
  }
  uint32_t getWidth(void)
  {
    return _gfx->width();
  }

  void draw_area(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t *data)
  {
// DEBUG_VNC("draw_area(%d, %d, %d, %d, data)\n", x, y, w, h);
#ifdef SEPARATE_DRAW_TASK
    queueDrawTask(x, y, w, h, true, (uint16_t *)data);
#else
    // _gfx->draw16bitBeRGBBitmap(x, y, (uint16_t *)data, w, h);
    _gfx->draw16bitRGBBitmap(x, y, (uint16_t *)data, w, h);
#endif
  }

  void draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint16_t color)
  {
    // DEBUG_VNC("draw_rect(%d, %d, %d, %d, color)\n", x, y, w, h);
    // MSB_16_SET(color, color);
#ifdef SEPARATE_DRAW_TASK
    queueDrawTask(x, y, w, h, false, &color);
#else
    _gfx->fillRect(x, y, w, h, color);
#endif
  }

  void vnc_options_override(dfb_vnc_options *opt)
  {
    // opt->client.bigendian = 1;
  }

private:
  Arduino_GFX *_gfx;
};

#endif /* _VNC_GFX_H_ */
