#if !defined(LITTLE_FOOT_PRINT)

#include "Arduino_G.h"

/**************************************************************************/
/*!
   @brief    Instatiate a GFX context for graphics! Can only be done by a superclass
   @param    w   Display width, in pixels
   @param    h   Display height, in pixels
*/
/**************************************************************************/
Arduino_G::Arduino_G(int16_t w, int16_t h) : WIDTH(w), HEIGHT(h)
{
}

// utility functions
bool gfx_draw_bitmap_to_framebuffer(
    uint16_t *from_bitmap, int16_t bitmap_w, int16_t bitmap_h,
    uint16_t *framebuffer, int16_t x, int16_t y, int16_t framebuffer_w, int16_t framebuffer_h)
{
  int16_t max_X = framebuffer_w - 1;
  int16_t max_Y = framebuffer_h - 1;
  if (
      ((x + bitmap_w - 1) < 0) || // Outside left
      ((y + bitmap_h - 1) < 0) || // Outside top
      (x > max_X) ||              // Outside right
      (y > max_Y)                 // Outside bottom
  )
  {
    return false;
  }
  else
  {
    int16_t xskip = 0;
    if ((y + bitmap_h - 1) > max_Y)
    {
      bitmap_h -= (y + bitmap_h - 1) - max_Y;
    }
    if (y < 0)
    {
      from_bitmap -= y * bitmap_w;
      bitmap_h += y;
      y = 0;
    }
    if ((x + bitmap_w - 1) > max_X)
    {
      xskip = (x + bitmap_w - 1) - max_X;
      bitmap_w -= xskip;
    }
    if (x < 0)
    {
      from_bitmap -= x;
      xskip -= x;
      bitmap_w += x;
      x = 0;
    }

    uint16_t *row = framebuffer;
    row += y * framebuffer_w; // shift framebuffer to y offset
    row += x;                 // shift framebuffer to x offset
    if (((framebuffer_w & 1) == 0) && ((xskip & 1) == 0) && ((bitmap_w & 1) == 0))
    {
      uint32_t *row2 = (uint32_t *)row;
      uint32_t *from_bitmap2 = (uint32_t *)from_bitmap;
      int16_t framebuffer_w2 = framebuffer_w >> 1;
      int16_t xskip2 = xskip >> 1;
      int16_t w2 = bitmap_w >> 1;

      int16_t j = bitmap_h;
      while (j--)
      {
        for (int16_t i = 0; i < w2; ++i)
        {
          row2[i] = *from_bitmap2++;
        }
        from_bitmap2 += xskip2;
        row2 += framebuffer_w2;
      }
    }
    else
    {
      int16_t j = bitmap_h;
      while (j--)
      {
        for (int i = 0; i < bitmap_w; ++i)
        {
          row[i] = *from_bitmap++;
        }
        from_bitmap += xskip;
        row += framebuffer_w;
      }
    }
    return true;
  }
}

bool gfx_draw_bitmap_to_framebuffer_rotate_1(
    uint16_t *from_bitmap, int16_t bitmap_w, int16_t bitmap_h,
    uint16_t *framebuffer, int16_t x, int16_t y, int16_t framebuffer_w, int16_t framebuffer_h)
{
  int16_t max_X = framebuffer_w - 1;
  int16_t max_Y = framebuffer_h - 1;
  if (
      ((x + bitmap_w - 1) < 0) || // Outside left
      ((y + bitmap_h - 1) < 0) || // Outside top
      (x > max_X) ||              // Outside right
      (y > max_Y)                 // Outside bottom
  )
  {
    return false;
  }
  else
  {
    int16_t xskip = 0;
    if ((y + bitmap_h - 1) > max_Y)
    {
      bitmap_h -= (y + bitmap_h - 1) - max_Y;
    }
    if (y < 0)
    {
      from_bitmap -= y * bitmap_w;
      bitmap_h += y;
      y = 0;
    }
    if ((x + bitmap_w - 1) > max_X)
    {
      xskip = (x + bitmap_w - 1) - max_X;
      bitmap_w -= xskip;
    }
    if (x < 0)
    {
      from_bitmap -= x;
      xskip -= x;
      bitmap_w += x;
      x = 0;
    }

    uint16_t *p;
    int16_t i;
    for (int16_t j = 0; j < bitmap_h; j++)
    {
      p = framebuffer;
      p += (x * framebuffer_h);     // shift framebuffer to y offset
      p += (framebuffer_h - y - j); // shift framebuffer to x offset

      i = bitmap_w;
      while (i--)
      {
        *p = *from_bitmap++;
        p += framebuffer_h;
      }
      from_bitmap += xskip;
    }
    return true;
  }
}

bool gfx_draw_bitmap_to_framebuffer_rotate_2(
    uint16_t *from_bitmap, int16_t bitmap_w, int16_t bitmap_h,
    uint16_t *framebuffer, int16_t x, int16_t y, int16_t framebuffer_w, int16_t framebuffer_h)
{
  int16_t max_X = framebuffer_w - 1;
  int16_t max_Y = framebuffer_h - 1;
  if (
      ((x + bitmap_w - 1) < 0) || // Outside left
      ((y + bitmap_h - 1) < 0) || // Outside top
      (x > max_X) ||              // Outside right
      (y > max_Y)                 // Outside bottom
  )
  {
    return false;
  }
  else
  {
    int16_t xskip = 0;
    if ((y + bitmap_h - 1) > max_Y)
    {
      bitmap_h -= (y + bitmap_h - 1) - max_Y;
    }
    if (y < 0)
    {
      from_bitmap -= y * bitmap_w;
      bitmap_h += y;
      y = 0;
    }
    if ((x + bitmap_w - 1) > max_X)
    {
      xskip = (x + bitmap_w - 1) - max_X;
      bitmap_w -= xskip;
    }
    if (x < 0)
    {
      from_bitmap -= x;
      xskip -= x;
      bitmap_w += x;
      x = 0;
    }

    uint16_t *row = framebuffer;
    row += (max_Y - y) * framebuffer_w;  // shift framebuffer to y offset
    row += framebuffer_w - x - bitmap_w; // shift framebuffer to x offset
    int16_t i;
    int16_t j = bitmap_h;
    while (j--)
    {
      i = bitmap_w;
      while (i--)
      {
        row[i] = *from_bitmap++;
      }
      from_bitmap += xskip;
      row -= framebuffer_w;
    }
    return true;
  }
}

bool gfx_draw_bitmap_to_framebuffer_rotate_3(
    uint16_t *from_bitmap, int16_t bitmap_w, int16_t bitmap_h,
    uint16_t *framebuffer, int16_t x, int16_t y, int16_t framebuffer_w, int16_t framebuffer_h)
{
  int16_t max_X = framebuffer_w - 1;
  int16_t max_Y = framebuffer_h - 1;
  if (
      ((x + bitmap_w - 1) < 0) || // Outside left
      ((y + bitmap_h - 1) < 0) || // Outside top
      (x > max_X) ||              // Outside right
      (y > max_Y)                 // Outside bottom
  )
  {
    return false;
  }
  else
  {
    int16_t xskip = 0;
    if ((y + bitmap_h - 1) > max_Y)
    {
      bitmap_h -= (y + bitmap_h - 1) - max_Y;
    }
    if (y < 0)
    {
      from_bitmap -= y * bitmap_w;
      bitmap_h += y;
      y = 0;
    }
    if ((x + bitmap_w - 1) > max_X)
    {
      xskip = (x + bitmap_w - 1) - max_X;
      bitmap_w -= xskip;
    }
    if (x < 0)
    {
      from_bitmap -= x;
      xskip -= x;
      bitmap_w += x;
      x = 0;
    }

    uint16_t *p;
    int16_t i;
    for (int16_t j = 0; j < bitmap_h; j++)
    {
      p = framebuffer;
      p += ((max_X - x) * framebuffer_h); // shift framebuffer to y offset
      p += y + j;                         // shift framebuffer to x offset

      i = bitmap_w;
      while (i--)
      {
        *p = *from_bitmap++;
        p -= framebuffer_h;
      }
      from_bitmap += xskip;
    }
    return true;
  }
}

#endif // !defined(LITTLE_FOOT_PRINT)
