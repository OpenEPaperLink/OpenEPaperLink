/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 */
#include "Arduino_DataBus.h"
#include "Arduino_GFX.h"
#include "Arduino_TFT.h"
#include "font/glcdfont.h"

Arduino_TFT::Arduino_TFT(
    Arduino_DataBus *bus, int8_t rst, uint8_t r,
    bool ips, int16_t w, int16_t h,
    uint8_t col_offset1, uint8_t row_offset1, uint8_t col_offset2, uint8_t row_offset2)
    : Arduino_GFX(w, h), _bus(bus), _rst(rst), _ips(ips),
      COL_OFFSET1(col_offset1), ROW_OFFSET1(row_offset1),
      COL_OFFSET2(col_offset2), ROW_OFFSET2(row_offset2)
{
  _rotation = r;
}

bool Arduino_TFT::begin(int32_t speed)
{
  if (_override_datamode != GFX_NOT_DEFINED)
  {
    if (!_bus->begin(speed, _override_datamode))
    {
      return false;
    }
  }
  else
  {
    if (!_bus->begin(speed))
    {
      return false;
    }
  }

  tftInit();
  setRotation(_rotation); // apply the setting rotation to the display
  setAddrWindow(0, 0, _width, _height);

  return true;
}

void Arduino_TFT::startWrite()
{
  _bus->beginWrite();
}

void Arduino_TFT::writePixelPreclipped(int16_t x, int16_t y, uint16_t color)
{
  writeAddrWindow(x, y, 1, 1);
  _bus->write16(color);
}

void Arduino_TFT::writeRepeat(uint16_t color, uint32_t len)
{
  _bus->writeRepeat(color, len);
}

void Arduino_TFT::writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
  if (_ordered_in_range(x, 0, _max_x) && h)
  { // X on screen, nonzero height
    if (h < 0)
    {             // If negative height...
      y += h + 1; //   Move Y to top edge
      h = -h;     //   Use positive height
    }
    if (y <= _max_y)
    { // Not off bottom
      int16_t y2 = y + h - 1;
      if (y2 >= 0)
      { // Not off top
        // Line partly or fully overlaps screen
        if (y < 0)
        {
          y = 0;
          h = y2 + 1;
        } // Clip top
        if (y2 > _max_y)
        {
          h = _max_y - y + 1;
        } // Clip bottom
        writeFillRectPreclipped(x, y, 1, h, color);
      }
    }
  }
}

void Arduino_TFT::writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  if (_ordered_in_range(y, 0, _max_y) && w)
  { // Y on screen, nonzero width
    if (w < 0)
    {             // If negative width...
      x += w + 1; //   Move X to left edge
      w = -w;     //   Use positive width
    }
    if (x <= _max_x)
    { // Not off right
      int16_t x2 = x + w - 1;
      if (x2 >= 0)
      { // Not off left
        // Line partly or fully overlaps screen
        if (x < 0)
        {
          x = 0;
          w = x2 + 1;
        } // Clip left
        if (x2 > _max_x)
        {
          w = _max_x - x + 1;
        } // Clip right
        writeFillRectPreclipped(x, y, w, 1, color);
      }
    }
  }
}

void Arduino_TFT::writeFillRectPreclipped(
    int16_t x, int16_t y,
    int16_t w, int16_t h, uint16_t color)
{
#ifdef ESP8266
  yield();
#endif
  writeAddrWindow(x, y, w, h);
  writeRepeat(color, (uint32_t)w * h);
}

void Arduino_TFT::endWrite()
{
  _bus->endWrite();
}

void Arduino_TFT::setAddrWindow(int16_t x0, int16_t y0, uint16_t w,
                                uint16_t h)
{
  startWrite();

  writeAddrWindow(x0, y0, w, h);

  endWrite();
}

void Arduino_TFT::setRotation(uint8_t r)
{
  Arduino_GFX::setRotation(r);
  switch (_rotation)
  {
  case 5:
  case 3:
    _xStart = ROW_OFFSET2;
    _yStart = COL_OFFSET1;
    break;
  case 6:
  case 2:
    _xStart = COL_OFFSET2;
    _yStart = ROW_OFFSET2;
    break;
  case 7:
  case 1:
    _xStart = ROW_OFFSET1;
    _yStart = COL_OFFSET2;
    break;
  case 4:
  default: // case 0:
    _xStart = COL_OFFSET1;
    _yStart = ROW_OFFSET1;
    break;
  }
  _currentX = 0xFFFF;
  _currentY = 0xFFFF;
  _currentW = 0xFFFF;
  _currentH = 0xFFFF;
}

void Arduino_TFT::writeColor(uint16_t color)
{
  _bus->write16(color);
}

// TFT optimization code, too big for ATMEL family
#if !defined(LITTLE_FOOT_PRINT)

void Arduino_TFT::writeBytes(uint8_t *data, uint32_t len)
{
  _bus->writeBytes(data, len);
}

void Arduino_TFT::writePixels(uint16_t *data, uint32_t len)
{
  _bus->writePixels(data, len);
}

void Arduino_TFT::pushColor(uint16_t color)
{
  _bus->beginWrite();
  writeColor(color);
  _bus->endWrite();
}

void Arduino_TFT::writeSlashLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
  int16_t dx;
  int16_t dy;
  int16_t err;
  int16_t xs;
  int16_t step;
  int16_t len;

  bool steep = _diff(y1, y0) > _diff(x1, x0);
  if (steep)
  {
    _swap_int16_t(x0, y0);
    _swap_int16_t(x1, y1);
  }

  if (x0 > x1)
  {
    _swap_int16_t(x0, x1);
    _swap_int16_t(y0, y1);
  }

  dx = x1 - x0;
  dy = _diff(y1, y0);
  err = dx >> 1;
  xs = x0;
  step = (y0 < y1) ? 1 : -1;
  len = 0;

  while (x0 <= x1)
  {
    x0++;
    len++;
    err -= dy;
    if ((err < 0) || ((x0 > x1) && len))
    {
      if (steep)
      {
        writeFillRectPreclipped(y0, xs, 1, len, color);
      }
      else
      {
        writeFillRectPreclipped(xs, y0, len, 1, color);
      }
      err += dx;
      y0 += step;
      len = 0;
      xs = x0;
    }
  }
}

// TFT tuned BITMAP / XBITMAP / GRAYSCALE / RGB BITMAP FUNCTIONS ---------------------

void Arduino_TFT::writeIndexedPixels(uint8_t *bitmap, uint16_t *color_index, uint32_t len)
{
  _bus->writeIndexedPixels(bitmap, color_index, len);
}

void Arduino_TFT::writeIndexedPixelsDouble(uint8_t *bitmap, uint16_t *color_index, uint32_t len)
{
  _bus->writeIndexedPixelsDouble(bitmap, color_index, len);
}

void Arduino_TFT::drawBitmap(
    int16_t x, int16_t y,
    const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color, uint16_t bg)
{
  if (
      ((x + w - 1) < 0) || // Outside left
      ((y + h - 1) < 0) || // Outside top
      (x > _max_x) ||      // Outside right
      (y > _max_y)         // Outside bottom
  )
  {
    return;
  }
  else if (
      (x < 0) ||                // Clip left
      (y < 0) ||                // Clip top
      ((x + w - 1) > _max_x) || // Clip right
      ((y + h - 1) > _max_y)    // Clip bottom
  )
  {
    Arduino_GFX::drawBitmap(x, y, bitmap, w, h, color, bg);
  }
  else
  {
    int32_t pixels = w * h;
    uint8_t byte = 0;
    uint16_t idx = 0;
    startWrite();
    writeAddrWindow(x, y, w, h);
    for (int32_t i = 0; i < pixels; i++)
    {
      if (i & 7)
      {
        byte <<= 1;
      }
      else
      {
        byte = pgm_read_byte(&bitmap[idx++]);
      }
      _bus->write16((byte & 0x80) ? color : bg);
    }
    endWrite();
  }
}

void Arduino_TFT::drawBitmap(
    int16_t x, int16_t y,
    uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg)
{
  if (
      ((x + w - 1) < 0) || // Outside left
      ((y + h - 1) < 0) || // Outside top
      (x > _max_x) ||      // Outside right
      (y > _max_y)         // Outside bottom
  )
  {
    return;
  }
  else if (
      (x < 0) ||                // Clip left
      (y < 0) ||                // Clip top
      ((x + w - 1) > _max_x) || // Clip right
      ((y + h - 1) > _max_y)    // Clip bottom
  )
  {
    Arduino_GFX::drawBitmap(x, y, bitmap, w, h, color, bg);
  }
  else
  {
    int32_t pixels = w * h;
    uint8_t byte = 0;
    startWrite();
    writeAddrWindow(x, y, w, h);
    for (int32_t i = 0; i < pixels; i++)
    {
      if (i & 7)
      {
        byte <<= 1;
      }
      else
      {
        byte = *(bitmap++);
      }
      _bus->write16((byte & 0x80) ? color : bg);
    }
    endWrite();
  }
}

void Arduino_TFT::drawGrayscaleBitmap(
    int16_t x, int16_t y,
    const uint8_t bitmap[], int16_t w, int16_t h)
{
  if (
      ((x + w - 1) < 0) || // Outside left
      ((y + h - 1) < 0) || // Outside top
      (x > _max_x) ||      // Outside right
      (y > _max_y)         // Outside bottom
  )
  {
    return;
  }
  else if (
      (x < 0) ||                // Clip left
      (y < 0) ||                // Clip top
      ((x + w - 1) > _max_x) || // Clip right
      ((y + h - 1) > _max_y)    // Clip bottom
  )
  {
    Arduino_GFX::drawGrayscaleBitmap(x, y, bitmap, w, h);
  }
  else
  {
    uint32_t len = (uint32_t)w * h;
    uint8_t v;
    startWrite();
    writeAddrWindow(x, y, w, h);
    for (uint32_t i = 0; i < len; i++)
    {
      v = (uint8_t)pgm_read_byte(&bitmap[i]);
      _bus->write16(color565(v, v, v));
    }
    endWrite();
  }
}

void Arduino_TFT::drawGrayscaleBitmap(
    int16_t x, int16_t y,
    uint8_t *bitmap, int16_t w, int16_t h)
{
  if (
      ((x + w - 1) < 0) || // Outside left
      ((y + h - 1) < 0) || // Outside top
      (x > _max_x) ||      // Outside right
      (y > _max_y)         // Outside bottom
  )
  {
    return;
  }
  else if (
      (x < 0) ||                // Clip left
      (y < 0) ||                // Clip top
      ((x + w - 1) > _max_x) || // Clip right
      ((y + h - 1) > _max_y)    // Clip bottom
  )
  {
    Arduino_GFX::drawGrayscaleBitmap(x, y, bitmap, w, h);
  }
  else
  {
    uint32_t len = (uint32_t)w * h;
    uint8_t v;
    startWrite();
    writeAddrWindow(x, y, w, h);
    while (len--)
    {
      v = *(bitmap++);
      _bus->write16(color565(v, v, v));
    }
    endWrite();
  }
}

void Arduino_TFT::drawIndexedBitmap(
    int16_t x, int16_t y,
    uint8_t *bitmap, uint16_t *color_index, int16_t w, int16_t h, int16_t x_skip)
{
  if (
      ((x + w - 1) < 0) || // Outside left
      ((y + h - 1) < 0) || // Outside top
      (x > _max_x) ||      // Outside right
      (y > _max_y)         // Outside bottom
  )
  {
    return;
  }
  else if (
      (x < 0) ||                // Clip left
      (y < 0) ||                // Clip top
      ((x + w - 1) > _max_x) || // Clip right
      ((y + h - 1) > _max_y)    // Clip bottom
  )
  {
    Arduino_GFX::drawIndexedBitmap(x, y, bitmap, color_index, w, h, x_skip);
  }
  else
  {
    startWrite();
    writeAddrWindow(x, y, w, h);
    if (x_skip == 0) {
      _bus->writeIndexedPixels(bitmap, color_index, h*w);
    } else {
      while (h--)
      {
        _bus->writeIndexedPixels(bitmap, color_index, w);
        bitmap += w + x_skip;
      }
    }
    endWrite();
  }
}

void Arduino_TFT::draw16bitRGBBitmap(int16_t x, int16_t y,
                                     uint16_t *bitmap, uint8_t *mask, int16_t w, int16_t h)
{
  if (
      ((x + w - 1) < 0) || // Outside left
      ((y + h - 1) < 0) || // Outside top
      (x > _max_x) ||      // Outside right
      (y > _max_y)         // Outside bottom
  )
  {
    return;
  }
  else if (
      (x < 0) ||                // Clip left
      (y < 0) ||                // Clip top
      ((x + w - 1) > _max_x) || // Clip right
      ((y + h - 1) > _max_y)    // Clip bottom
  )
  {
    Arduino_GFX::draw16bitRGBBitmap(x, y, bitmap, mask, w, h);
  }
  else
  {
    int32_t offset = 0, maskIdx = 0, len = 0;
    uint8_t byte = 0;
    startWrite();
    for (int16_t j = 0; j < h; j++, y++)
    {
      for (int16_t i = 0; i < w; i++)
      {
        if (i & 7)
        {
          byte <<= 1;
        }
        else
        {
          byte = mask[maskIdx++];
        }
        if (byte & 0x80)
        {
          len++;
        }
        else
        {
          if (len)
          {
            writeAddrWindow(x + i - len, y, len, 1);
            _bus->writePixels(&bitmap[offset - len], len);
            len = 0;
          }
        }
        offset++;
      }
      if (len)
      {
        writeAddrWindow(x + w - 1 - len, y, len, 1);
        _bus->writePixels(&bitmap[offset - len], len);
        len = 0;
      }
    }
    endWrite();
  }
}

void Arduino_TFT::draw16bitRGBBitmap(
    int16_t x, int16_t y,
    const uint16_t bitmap[], int16_t w, int16_t h)
{
  if (
      ((x + w - 1) < 0) || // Outside left
      ((y + h - 1) < 0) || // Outside top
      (x > _max_x) ||      // Outside right
      (y > _max_y)         // Outside bottom
  )
  {
    return;
  }
  else if (
      (x < 0) ||                // Clip left
      (y < 0) ||                // Clip top
      ((x + w - 1) > _max_x) || // Clip right
      ((y + h - 1) > _max_y)    // Clip bottom
  )
  {
    Arduino_GFX::draw16bitRGBBitmap(x, y, bitmap, w, h);
  }
  else
  {
    uint32_t len = (uint32_t)w * h;
    startWrite();
    writeAddrWindow(x, y, w, h);
    for (uint32_t i = 0; i < len; i++)
    {
      _bus->write16(pgm_read_word(&bitmap[i]));
    }
    endWrite();
  }
}

void Arduino_TFT::draw16bitRGBBitmap(
    int16_t x, int16_t y,
    uint16_t *bitmap, int16_t w, int16_t h)
{
  if (
      ((x + w - 1) < 0) || // Outside left
      ((y + h - 1) < 0) || // Outside top
      (x > _max_x) ||      // Outside right
      (y > _max_y)         // Outside bottom
  )
  {
    return;
  }
  else if (
      (x < 0) ||                // Clip left
      (y < 0) ||                // Clip top
      ((x + w - 1) > _max_x) || // Clip right
      ((y + h - 1) > _max_y)    // Clip bottom
  )
  {
    Arduino_GFX::draw16bitRGBBitmap(x, y, bitmap, w, h);
  }
  else
  {
    startWrite();
    writeAddrWindow(x, y, w, h);
    _bus->writePixels(bitmap, (uint32_t)w * h);
    endWrite();
  }
}

void Arduino_TFT::draw16bitBeRGBBitmap(
    int16_t x, int16_t y,
    uint16_t *bitmap, int16_t w, int16_t h)
{
  if (
      ((x + w - 1) < 0) || // Outside left
      ((y + h - 1) < 0) || // Outside top
      (x > _max_x) ||      // Outside right
      (y > _max_y)         // Outside bottom
  )
  {
    return;
  }
  else if (
      (x < 0) ||                // Clip left
      (y < 0) ||                // Clip top
      ((x + w - 1) > _max_x) || // Clip right
      ((y + h - 1) > _max_y)    // Clip bottom
  )
  {
    Arduino_GFX::draw16bitBeRGBBitmap(x, y, bitmap, w, h);
  }
  else
  {
    startWrite();
    writeAddrWindow(x, y, w, h);
    _bus->writeBytes((uint8_t *)bitmap, (uint32_t)w * h * 2);
    endWrite();
  }
}

void Arduino_TFT::draw24bitRGBBitmap(
    int16_t x, int16_t y,
    const uint8_t bitmap[], int16_t w, int16_t h)
{
  if (
      ((x + w - 1) < 0) || // Outside left
      ((y + h - 1) < 0) || // Outside top
      (x > _max_x) ||      // Outside right
      (y > _max_y)         // Outside bottom
  )
  {
    return;
  }
  else if (
      (x < 0) ||                // Clip left
      (y < 0) ||                // Clip top
      ((x + w - 1) > _max_x) || // Clip right
      ((y + h - 1) > _max_y)    // Clip bottom
  )
  {
    Arduino_GFX::draw24bitRGBBitmap(x, y, bitmap, w, h);
  }
  else
  {
    uint32_t len = (uint32_t)w * h;
    uint32_t offset = 0;
    startWrite();
    writeAddrWindow(x, y, w, h);
    while (len--)
    {
      _bus->write16(color565(pgm_read_byte(&bitmap[offset]), pgm_read_byte(&bitmap[offset + 1]), pgm_read_byte(&bitmap[offset + 2])));
      offset += 3;
    }
    endWrite();
  }
}

void Arduino_TFT::draw24bitRGBBitmap(
    int16_t x, int16_t y,
    uint8_t *bitmap, int16_t w, int16_t h)
{
  if (
      ((x + w - 1) < 0) || // Outside left
      ((y + h - 1) < 0) || // Outside top
      (x > _max_x) ||      // Outside right
      (y > _max_y)         // Outside bottom
  )
  {
    return;
  }
  else if (
      (x < 0) ||                // Clip left
      (y < 0) ||                // Clip top
      ((x + w - 1) > _max_x) || // Clip right
      ((y + h - 1) > _max_y)    // Clip bottom
  )
  {
    Arduino_GFX::draw24bitRGBBitmap(x, y, bitmap, w, h);
  }
  else
  {
    uint32_t len = (uint32_t)w * h;
    uint32_t offset = 0;
    startWrite();
    writeAddrWindow(x, y, w, h);
    while (len--)
    {
      _bus->write16(color565(bitmap[offset], bitmap[offset + 1], bitmap[offset + 2]));
      offset += 3;
    }
    endWrite();
  }
}

void Arduino_TFT::drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg)
{
  uint16_t block_w;
  uint16_t block_h;

#if !defined(ATTINY_CORE)
  if (gfxFont) // custom font
  {
    // Character is assumed previously filtered by write() to eliminate
    // newlines, returns, non-printable characters, etc.  Calling
    // drawChar() directly with 'bad' characters of font may cause mayhem!
    uint8_t first = pgm_read_byte(&gfxFont->first);
    GFXglyph *glyph = pgm_read_glyph_ptr(gfxFont, c - first);
    uint8_t *bitmap = pgm_read_bitmap_ptr(gfxFont);

    uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
    uint8_t w = pgm_read_byte(&glyph->width),
            h = pgm_read_byte(&glyph->height),
            xAdvance = pgm_read_byte(&glyph->xAdvance),
            yAdvance = pgm_read_byte(&gfxFont->yAdvance),
            baseline = yAdvance * 2 / 3; // TODO: baseline is an arbitrary currently, may be define in font file
    int8_t xo = pgm_read_byte(&glyph->xOffset),
           yo = pgm_read_byte(&glyph->yOffset);
    // urgly workaround for the character not fit in the box
    if ((bg != color)             // have background color
        && ((xo + w) > xAdvance)) // if character draw outside the box
    {
      xo = xAdvance - w; // pad inside the box
    }

    uint8_t xx, yy, bits = 0, bit = 0;
    int16_t xo16 = xo, yo16 = yo;

    if (xAdvance < w)
    {
      xAdvance = w; // Don't know why it exists
    }

    block_w = xAdvance * textsize_x;
    block_h = yAdvance * textsize_y;
    int16_t x1 = (xo < 0) ? (x + xo) : x;
    if (
        (x1 < 0) ||                             // Clip left
        ((y - baseline) < 0) ||                 // Clip top
        ((x1 + block_w - 1) > _max_x) ||        // Clip right
        ((y - baseline + block_h - 1) > _max_y) // Clip bottom
    )
    {
      // partial draw char by parent class
      Arduino_GFX::drawChar(x, y, c, color, bg);
    }
    else
    {
      // NOTE: Different from Adafruit_GFX design, Adruino_GFX also cater background.
      // Since it may introduce many ugly output, it should limited using on mono font only.
      if (xo < 0) // padding X offset to >= 0
      {
        x += xo;
        xo = 0;
      }

      startWrite();
      if (bg != color) // have background color
      {
        writeAddrWindow(x, y - (baseline * textsize_y), block_w, block_h);

        uint16_t line_buf[block_w];
        int8_t i;
        bool draw_dot;
        for (yy = 0; yy < yAdvance; yy++)
        {
          if ((yy < (baseline + yo)) || (yy > (baseline + yo + h - 1)))
          {
            writeRepeat(bg, block_w * textsize_y);
          }
          else
          {
            i = 0;
            for (xx = 0; xx < xAdvance; xx++)
            {
              if ((xx < xo) || (xx > (xo + w - 1)))
              {
                draw_dot = false;
              }
              else
              {
                if (!(bit++ & 7))
                {
                  bits = pgm_read_byte(&bitmap[bo++]);
                }
                draw_dot = bits & 0x80;
                bits <<= 1;
              }

              if (textsize_x == 1)
              {
                line_buf[i++] = draw_dot ? color : bg;
              }
              else
              {
                if (draw_dot)
                {
                  for (int8_t k = 0; k < textsize_x; k++)
                  {
                    line_buf[i++] = (k < (textsize_x - text_pixel_margin)) ? color : bg;
                  }
                }
                else
                {
                  for (int8_t k = 0; k < textsize_x; k++)
                  {
                    line_buf[i++] = bg;
                  }
                }
              }
            }
            if (textsize_y == 1)
            {
              writePixels(line_buf, block_w);
            }
            else
            {
              for (int8_t l = 0; l < textsize_y; l++)
              {
                if (l < (textsize_y - text_pixel_margin))
                {
                  writePixels(line_buf, block_w);
                }
                else
                {
                  writeRepeat(bg, block_w);
                }
              }
            }
          }
        }
      }
      else // (bg == color), no background color
      {
        for (yy = 0; yy < h; yy++)
        {
          for (xx = 0; xx < w; xx++)
          {
            if (!(bit++ & 7))
            {
              bits = pgm_read_byte(&bitmap[bo++]);
            }
            if (bits & 0x80)
            {
              if (textsize_x == 1 && textsize_y == 1)
              {
                writePixelPreclipped(x + xo + xx, y + yo + yy, color);
              }
              else
              {
                writeFillRectPreclipped(x + (xo16 + xx) * textsize_x, y + (yo16 + yy) * textsize_y,
                                        textsize_x - text_pixel_margin, textsize_y - text_pixel_margin, color);
              }
            }
            bits <<= 1;
          }
        }
      }
      endWrite();
    }
  }
  else // not gfxFont
#endif // !defined(ATTINY_CORE)
#if defined(U8G2_FONT_SUPPORT)
      if (u8g2Font)
  {
    Arduino_GFX::drawChar(x, y, c, color, bg);
  }
  else // not u8g2Font
#endif // defined(U8G2_FONT_SUPPORT)
  {
    block_w = 6 * textsize_x;
    block_h = 8 * textsize_y;
    if (
        (x < 0) ||                      // Clip left
        (y < 0) ||                      // Clip top
        ((x + block_w - 1) > _max_x) || // Clip right
        ((y + block_h - 1) > _max_y)    // Clip bottom
    )
    {
      // partial draw char by parent class
      Arduino_GFX::drawChar(x, y, c, color, bg);
    }
    else
    {
      uint8_t col[5];
      for (int8_t i = 0; i < 5; i++)
      {
        col[i] = pgm_read_byte(&font[c * 5 + i]);
      }

      startWrite();
      if (bg != color) // have background color
      {
        writeAddrWindow(x, y, block_w, block_h);

        uint16_t line_buf[block_w];
        if (textsize_x == 1)
        {
          line_buf[5] = bg; // last column always bg
        }
        else
        {
          for (int8_t k = 0; k < textsize_x; k++)
          {
            line_buf[5 * textsize_x + k] = bg;
          }
        }
        uint8_t bit = 1;
        bool draw_dot;

        while (bit)
        {
          for (int8_t i = 0; i < 5; i++)
          {
            draw_dot = col[i] & bit;
            if (textsize_x == 1)
            {
              line_buf[i] = (draw_dot) ? color : bg;
            }
            else
            {
              if (draw_dot)
              {
                for (int8_t k = 0; k < textsize_x; k++)
                {
                  line_buf[i * textsize_x + k] = (k < (textsize_x - text_pixel_margin)) ? color : bg;
                }
              }
              else
              {
                for (int8_t k = 0; k < textsize_x; k++)
                {
                  line_buf[i * textsize_x + k] = bg;
                }
              }
            }
          }
          if (textsize_y == 1)
          {
            writePixels(line_buf, block_w);
          }
          else
          {
            for (int8_t l = 0; l < textsize_y; l++)
            {
              if (l < (textsize_y - text_pixel_margin))
              {
                writePixels(line_buf, block_w);
              }
              else
              {
                writeRepeat(bg, block_w);
              }
            }
          }
          bit <<= 1;
        }
      }
      else // (bg == color), no background color
      {
        for (int8_t i = 0; i < 5; i++)
        { // Char bitmap = 5 columns
          uint8_t line = col[i];
          for (int8_t j = 0; j < 8; j++, line >>= 1)
          {
            if (line & 1)
            {
              if (textsize_x == 1 && textsize_y == 1)
              {
                writePixelPreclipped(x + i, y + j, color);
              }
              else
              {
                writeFillRectPreclipped(x + i * textsize_x, y + j * textsize_y, textsize_x - text_pixel_margin, textsize_y - text_pixel_margin, color);
              }
            }
          }
        }
      }
      endWrite();
    }
  }
}

#endif // !defined(LITTLE_FOOT_PRINT)
