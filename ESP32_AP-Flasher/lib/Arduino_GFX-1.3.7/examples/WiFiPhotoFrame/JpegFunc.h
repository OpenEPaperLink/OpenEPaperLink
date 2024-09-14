/*******************************************************************************
 * JPEGDEC related function
 *
 * Dependent libraries:
 * JPEGDEC: https://github.com/bitbank2/JPEGDEC.git
 ******************************************************************************/
#ifndef _JPEGFUNC_H_
#define _JPEGFUNC_H_

#include <JPEGDEC.h>

static JPEGDEC _jpeg;
static int _x, _y, _x_bound, _y_bound;

static void jpegCloseHttpStream(void *pHandle)
{
  // printf("jpegCloseHttpStream\n");
  // WiFiClient *http_stream = (WiFiClient *)pHandle;
  // do nothing
}

static int32_t readStream(WiFiClient *http_stream, uint8_t *pBuf, int32_t iLen)
{
  uint8_t wait = 0;
  size_t a = http_stream->available();
  size_t r = 0;
  while ((r < iLen) && (wait < 10))
  {
    if (a)
    {
      wait = 0; // reset wait count once available
      r += http_stream->readBytes(pBuf + r, iLen - r);
      // printf("1st byte: %d, 2nd byte: %d, last byte: %d, iLen: %d, r: %d, wait: %d\n", pBuf[0], pBuf[1], pBuf[r - 1], iLen, r, wait);
    }
    else
    {
      delay(100);
      wait++;
    }
    a = http_stream->available();
  }
  return r;
}

static int32_t jpegReadHttpStream(JPEGFILE *pFile, uint8_t *pBuf, int32_t iLen)
{
  // printf("jpegReadHttpStream, iLen: %d\n", iLen);
  WiFiClient *http_stream = (WiFiClient *)pFile->fHandle;
  return readStream(http_stream, pBuf, iLen);
}

static int32_t jpegSeekHttpStream(JPEGFILE *pFile, int32_t iPosition)
{
  // printf("jpegSeekHttpStream, pFile->iPos: %d, iPosition: %d\n", pFile->iPos, iPosition);
  WiFiClient *http_stream = (WiFiClient *)pFile->fHandle;
  http_stream->readBytes((uint8_t *)nullptr, iPosition - pFile->iPos);
  return iPosition;
}

static int jpegOpenHttpStreamWithBuffer(WiFiClient *http_stream, uint8_t *buf, int32_t dataSize, JPEG_DRAW_CALLBACK *jpegDrawCallback)
{
  int32_t r = readStream(http_stream, buf, dataSize);
  if (r != dataSize)
  {
    return 0;
  }
  return _jpeg.openRAM(buf, dataSize, jpegDrawCallback);
}

static int jpegOpenHttpStream(WiFiClient *http_stream, int32_t dataSize, JPEG_DRAW_CALLBACK *jpegDrawCallback)
{
  return _jpeg.open(http_stream, dataSize, jpegCloseHttpStream, jpegReadHttpStream, jpegSeekHttpStream, jpegDrawCallback);
}

static int jpegDraw(bool useBigEndian,
                    int x, int y, int widthLimit, int heightLimit)
{
  _x = x;
  _y = y;
  _x_bound = _x + widthLimit - 1;
  _y_bound = _y + heightLimit - 1;

  // scale to fit height
  int _scale;
  int iMaxMCUs;
  float ratio = (float)_jpeg.getHeight() / heightLimit;
  if (ratio <= 1)
  {
    _scale = 0;
    iMaxMCUs = widthLimit / 16;
  }
  else if (ratio <= 2)
  {
    _scale = JPEG_SCALE_HALF;
    iMaxMCUs = widthLimit / 8;
  }
  else if (ratio <= 4)
  {
    _scale = JPEG_SCALE_QUARTER;
    iMaxMCUs = widthLimit / 4;
  }
  else
  {
    _scale = JPEG_SCALE_EIGHTH;
    iMaxMCUs = widthLimit / 2;
  }
  _jpeg.setMaxOutputSize(iMaxMCUs);
  if (useBigEndian)
  {
    _jpeg.setPixelType(RGB565_BIG_ENDIAN);
  }
  int decode_result = _jpeg.decode(x, y, _scale);
  _jpeg.close();

  return decode_result;
}

#endif // _JPEGFUNC_H_
