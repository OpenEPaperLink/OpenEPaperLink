class IndexedSprite
{
public:
  IndexedSprite(int16_t x, uint16_t y, uint8_t *bitmap, uint16_t *palette, int16_t w, int16_t h, int16_t x_skip, bool loop, size_t frames, int16_t speed_divider)
      : _x(x), _y(y), _bitmap(bitmap), _palette(palette), _w(w), _h(h), _x_skip(x_skip), _loop(loop), _frames(frames), _speed_divider(speed_divider)
  {
  }
  IndexedSprite(int16_t x, uint16_t y, uint8_t *bitmap, uint16_t *palette, int16_t w, int16_t h, int16_t x_skip, bool loop, size_t frames, int16_t speed_divider, uint8_t chroma_key)
      : _x(x), _y(y), _bitmap(bitmap), _palette(palette), _w(w), _h(h), _x_skip(x_skip), _loop(loop), _frames(frames), _speed_divider(speed_divider), _chroma_key(chroma_key)
  {
    _has_chroma_key = true;
  }

  void h_scroll(int16_t v)
  {
    h_scroll(v, _w);
  }

  void h_scroll(int16_t v, int16_t bound)
  {
    _curr_sub_scroll += v;
    while (_curr_sub_scroll > _speed_divider)
    {
      ++_x;
      _curr_sub_scroll -= _speed_divider;
    }
    while (_curr_sub_scroll < -_speed_divider)
    {
      --_x;
      _curr_sub_scroll += _speed_divider;
    }
    if (_x < -(_w))
    {
      _x = bound;
    }
    else if (_x > bound)
    {
      _x = -(_w);
    }
  }

  void next_frame()
  {
    _curr_sub_frame++;
    if (_curr_sub_frame > _speed_divider)
    {
      ++_curr_frame;
      _curr_sub_frame -= _speed_divider;
    }
    if (_curr_frame >= _frames)
    {
      _curr_frame = 0;
    }
  }

  void draw(Arduino_GFX *gfx)
  {
    if (_has_chroma_key)
    {
      gfx->drawIndexedBitmap(_x, _y, _bitmap + (_curr_frame * _w), _palette, _chroma_key, _w, _h, _x_skip);
      if (_loop)
      {
        gfx->drawIndexedBitmap((_x < 0) ? (_x + _w) : (_x - _w), _y, _bitmap + (_curr_frame * _w), _palette, _chroma_key, _w, _h, _x_skip);
      }
    }
    else
    {
      gfx->drawIndexedBitmap(_x, _y, _bitmap + (_curr_frame * _w), _palette, _w, _h, _x_skip);
      if (_loop)
      {
        gfx->drawIndexedBitmap((_x < 0) ? (_x + _w) : (_x - _w), _y, _bitmap + (_curr_frame * _w), _palette, _w, _h);
      }
    }
  }

private:
  int16_t _x;
  int16_t _y;
  uint16_t *_palette;
  uint8_t *_bitmap;
  int16_t _w;
  int16_t _h;
  int16_t _x_skip;
  bool _loop;
  uint8_t _chroma_key;
  bool _has_chroma_key = false;
  size_t _frames;
  int16_t _speed_divider;
  size_t _curr_frame = 0;
  int16_t _curr_sub_scroll = 0;
  int16_t _curr_sub_frame = 0;
};
