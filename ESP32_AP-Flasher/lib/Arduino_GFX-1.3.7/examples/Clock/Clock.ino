/*
   Arduino Watch Lite Version
   You may find full version at: https://github.com/moononournation/ArduinoWatch
*/

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

#define BACKGROUND BLACK
#define MARK_COLOR WHITE
#define SUBMARK_COLOR DARKGREY // LIGHTGREY
#define HOUR_COLOR WHITE
#define MINUTE_COLOR BLUE // LIGHTGREY
#define SECOND_COLOR RED

#define SIXTIETH 0.016666667
#define TWELFTH 0.08333333
#define SIXTIETH_RADIAN 0.10471976
#define TWELFTH_RADIAN 0.52359878
#define RIGHT_ANGLE_RADIAN 1.5707963

static uint8_t conv2d(const char *p)
{
  uint8_t v = 0;
  return (10 * (*p - '0')) + (*++p - '0');
}

static int16_t w, h, center;
static int16_t hHandLen, mHandLen, sHandLen, markLen;
static float sdeg, mdeg, hdeg;
static int16_t osx = 0, osy = 0, omx = 0, omy = 0, ohx = 0, ohy = 0; // Saved H, M, S x & y coords
static int16_t nsx, nsy, nmx, nmy, nhx, nhy;                         // H, M, S x & y coords
static int16_t xMin, yMin, xMax, yMax;                               // redraw range
static int16_t hh, mm, ss;
static unsigned long targetTime; // next action time

static int16_t *cached_points;
static uint16_t cached_points_idx = 0;
static int16_t *last_cached_point;

void setup(void)
{
#ifdef GFX_EXTRA_PRE_INIT
  GFX_EXTRA_PRE_INIT();
#endif

  // Init Display
  if (!gfx->begin())
  {
    Serial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(BACKGROUND);

#ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#endif

  // init LCD constant
  w = gfx->width();
  h = gfx->height();
  if (w < h)
  {
    center = w / 2;
  }
  else
  {
    center = h / 2;
  }
  hHandLen = center * 3 / 8;
  mHandLen = center * 2 / 3;
  sHandLen = center * 5 / 6;
  markLen = sHandLen / 6;
  cached_points = (int16_t *)malloc((hHandLen + 1 + mHandLen + 1 + sHandLen + 1) * 2 * 2);

  // Draw 60 clock marks
  draw_round_clock_mark(
      // draw_square_clock_mark(
      center - markLen, center,
      center - (markLen * 2 / 3), center,
      center - (markLen / 2), center);

  hh = conv2d(__TIME__);
  mm = conv2d(__TIME__ + 3);
  ss = conv2d(__TIME__ + 6);

  targetTime = ((millis() / 1000) + 1) * 1000;
}

void loop()
{
  unsigned long cur_millis = millis();
  if (cur_millis >= targetTime)
  {
    targetTime += 1000;
    ss++; // Advance second
    if (ss == 60)
    {
      ss = 0;
      mm++; // Advance minute
      if (mm > 59)
      {
        mm = 0;
        hh++; // Advance hour
        if (hh > 23)
        {
          hh = 0;
        }
      }
    }
  }

  // Pre-compute hand degrees, x & y coords for a fast screen update
  sdeg = SIXTIETH_RADIAN * ((0.001 * (cur_millis % 1000)) + ss); // 0-59 (includes millis)
  nsx = cos(sdeg - RIGHT_ANGLE_RADIAN) * sHandLen + center;
  nsy = sin(sdeg - RIGHT_ANGLE_RADIAN) * sHandLen + center;
  if ((nsx != osx) || (nsy != osy))
  {
    mdeg = (SIXTIETH * sdeg) + (SIXTIETH_RADIAN * mm); // 0-59 (includes seconds)
    hdeg = (TWELFTH * mdeg) + (TWELFTH_RADIAN * hh);   // 0-11 (includes minutes)
    mdeg -= RIGHT_ANGLE_RADIAN;
    hdeg -= RIGHT_ANGLE_RADIAN;
    nmx = cos(mdeg) * mHandLen + center;
    nmy = sin(mdeg) * mHandLen + center;
    nhx = cos(hdeg) * hHandLen + center;
    nhy = sin(hdeg) * hHandLen + center;

    // redraw hands
    redraw_hands_cached_draw_and_erase();

    ohx = nhx;
    ohy = nhy;
    omx = nmx;
    omy = nmy;
    osx = nsx;
    osy = nsy;

    delay(1);
  }
}

void draw_round_clock_mark(int16_t innerR1, int16_t outerR1, int16_t innerR2, int16_t outerR2, int16_t innerR3, int16_t outerR3)
{
  float x, y;
  int16_t x0, x1, y0, y1, innerR, outerR;
  uint16_t c;

  for (uint8_t i = 0; i < 60; i++)
  {
    if ((i % 15) == 0)
    {
      innerR = innerR1;
      outerR = outerR1;
      c = MARK_COLOR;
    }
    else if ((i % 5) == 0)
    {
      innerR = innerR2;
      outerR = outerR2;
      c = MARK_COLOR;
    }
    else
    {
      innerR = innerR3;
      outerR = outerR3;
      c = SUBMARK_COLOR;
    }

    mdeg = (SIXTIETH_RADIAN * i) - RIGHT_ANGLE_RADIAN;
    x = cos(mdeg);
    y = sin(mdeg);
    x0 = x * outerR + center;
    y0 = y * outerR + center;
    x1 = x * innerR + center;
    y1 = y * innerR + center;

    gfx->drawLine(x0, y0, x1, y1, c);
  }
}

void draw_square_clock_mark(int16_t innerR1, int16_t outerR1, int16_t innerR2, int16_t outerR2, int16_t innerR3, int16_t outerR3)
{
  float x, y;
  int16_t x0, x1, y0, y1, innerR, outerR;
  uint16_t c;

  for (uint8_t i = 0; i < 60; i++)
  {
    if ((i % 15) == 0)
    {
      innerR = innerR1;
      outerR = outerR1;
      c = MARK_COLOR;
    }
    else if ((i % 5) == 0)
    {
      innerR = innerR2;
      outerR = outerR2;
      c = MARK_COLOR;
    }
    else
    {
      innerR = innerR3;
      outerR = outerR3;
      c = SUBMARK_COLOR;
    }

    if ((i >= 53) || (i < 8))
    {
      x = tan(SIXTIETH_RADIAN * i);
      x0 = center + (x * outerR);
      y0 = center + (1 - outerR);
      x1 = center + (x * innerR);
      y1 = center + (1 - innerR);
    }
    else if (i < 23)
    {
      y = tan((SIXTIETH_RADIAN * i) - RIGHT_ANGLE_RADIAN);
      x0 = center + (outerR);
      y0 = center + (y * outerR);
      x1 = center + (innerR);
      y1 = center + (y * innerR);
    }
    else if (i < 38)
    {
      x = tan(SIXTIETH_RADIAN * i);
      x0 = center - (x * outerR);
      y0 = center + (outerR);
      x1 = center - (x * innerR);
      y1 = center + (innerR);
    }
    else if (i < 53)
    {
      y = tan((SIXTIETH_RADIAN * i) - RIGHT_ANGLE_RADIAN);
      x0 = center + (1 - outerR);
      y0 = center - (y * outerR);
      x1 = center + (1 - innerR);
      y1 = center - (y * innerR);
    }
    gfx->drawLine(x0, y0, x1, y1, c);
  }
}

void redraw_hands_cached_draw_and_erase()
{
  gfx->startWrite();
  draw_and_erase_cached_line(center, center, nsx, nsy, SECOND_COLOR, cached_points, sHandLen + 1, false, false);
  draw_and_erase_cached_line(center, center, nhx, nhy, HOUR_COLOR, cached_points + ((sHandLen + 1) * 2), hHandLen + 1, true, false);
  draw_and_erase_cached_line(center, center, nmx, nmy, MINUTE_COLOR, cached_points + ((sHandLen + 1 + hHandLen + 1) * 2), mHandLen + 1, true, true);
  gfx->endWrite();
}

void draw_and_erase_cached_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t color, int16_t *cache, int16_t cache_len, bool cross_check_second, bool cross_check_hour)
{
#if defined(ESP8266)
  yield();
#endif
  bool steep = _diff(y1, y0) > _diff(x1, x0);
  if (steep)
  {
    _swap_int16_t(x0, y0);
    _swap_int16_t(x1, y1);
  }

  int16_t dx, dy;
  dx = _diff(x1, x0);
  dy = _diff(y1, y0);

  int16_t err = dx / 2;
  int8_t xstep = (x0 < x1) ? 1 : -1;
  int8_t ystep = (y0 < y1) ? 1 : -1;
  x1 += xstep;
  int16_t x, y, ox, oy;
  for (uint16_t i = 0; i <= dx; i++)
  {
    if (steep)
    {
      x = y0;
      y = x0;
    }
    else
    {
      x = x0;
      y = y0;
    }
    ox = *(cache + (i * 2));
    oy = *(cache + (i * 2) + 1);
    if ((x == ox) && (y == oy))
    {
      if (cross_check_second || cross_check_hour)
      {
        write_cache_pixel(x, y, color, cross_check_second, cross_check_hour);
      }
    }
    else
    {
      write_cache_pixel(x, y, color, cross_check_second, cross_check_hour);
      if ((ox > 0) || (oy > 0))
      {
        write_cache_pixel(ox, oy, BACKGROUND, cross_check_second, cross_check_hour);
      }
      *(cache + (i * 2)) = x;
      *(cache + (i * 2) + 1) = y;
    }
    if (err < dy)
    {
      y0 += ystep;
      err += dx;
    }
    err -= dy;
    x0 += xstep;
  }
  for (uint16_t i = dx + 1; i < cache_len; i++)
  {
    ox = *(cache + (i * 2));
    oy = *(cache + (i * 2) + 1);
    if ((ox > 0) || (oy > 0))
    {
      write_cache_pixel(ox, oy, BACKGROUND, cross_check_second, cross_check_hour);
    }
    *(cache + (i * 2)) = 0;
    *(cache + (i * 2) + 1) = 0;
  }
}

void write_cache_pixel(int16_t x, int16_t y, int16_t color, bool cross_check_second, bool cross_check_hour)
{
  int16_t *cache = cached_points;
  if (cross_check_second)
  {
    for (uint16_t i = 0; i <= sHandLen; i++)
    {
      if ((x == *(cache++)) && (y == *(cache)))
      {
        return;
      }
      cache++;
    }
  }
  if (cross_check_hour)
  {
    cache = cached_points + ((sHandLen + 1) * 2);
    for (uint16_t i = 0; i <= hHandLen; i++)
    {
      if ((x == *(cache++)) && (y == *(cache)))
      {
        return;
      }
      cache++;
    }
  }
  gfx->writePixel(x, y, color);
}
