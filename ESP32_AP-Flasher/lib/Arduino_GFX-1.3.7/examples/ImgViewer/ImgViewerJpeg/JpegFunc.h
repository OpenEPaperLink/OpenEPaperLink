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
static File _f;
static int _x, _y, _x_bound, _y_bound;

static void *jpegOpenFile(const char *szFilename, int32_t *pFileSize)
{
    // Serial.println("jpegOpenFile");
#if defined(ARDUINO_ARCH_SAMD) && defined(SEEED_GROVE_UI_WIRELESS)
    _f = SD.open(szFilename, "r");
#elif defined(TARGET_RP2040)
    _f = LittleFS.open(szFilename, "r");
    // _f = SDFS.open(szFilename, "r");
#elif defined(ESP32)
    // _f = FFat.open(szFilename, "r");
    _f = LittleFS.open(szFilename, "r");
    // _f = SPIFFS.open(szFilename, "r");
    // _f = SD.open(szFilename, "r");
    // _f = SD_MMC.open(szFilename, "r");
#elif defined(ESP8266)
    _f = LittleFS.open(szFilename, "r");
    // _f = SD.open(szFilename, "r");
#else
    _f = SD.open(szFilename, FILE_READ);
#endif
    *pFileSize = _f.size();
    return &_f;
}

static void jpegCloseFile(void *pHandle)
{
    // Serial.println("jpegCloseFile");
    File *f = static_cast<File *>(pHandle);
    f->close();
}

static int32_t jpegReadFile(JPEGFILE *pFile, uint8_t *pBuf, int32_t iLen)
{
    // Serial.printf("jpegReadFile, iLen: %d\n", iLen);
    File *f = static_cast<File *>(pFile->fHandle);
    size_t r = f->read(pBuf, iLen);
    return r;
}

static int32_t jpegSeekFile(JPEGFILE *pFile, int32_t iPosition)
{
    // Serial.printf("jpegSeekFile, pFile->iPos: %d, iPosition: %d\n", pFile->iPos, iPosition);
    File *f = static_cast<File *>(pFile->fHandle);
    f->seek(iPosition);
    return iPosition;
}

static void jpegDraw(
    const char *filename, JPEG_DRAW_CALLBACK *jpegDrawCallback, bool useBigEndian,
    int x, int y, int widthLimit, int heightLimit)
{
    _x = x;
    _y = y;
    _x_bound = _x + widthLimit - 1;
    _y_bound = _y + heightLimit - 1;

    _jpeg.open(filename, jpegOpenFile, jpegCloseFile, jpegReadFile, jpegSeekFile, jpegDrawCallback);

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
    _jpeg.decode(x, y, _scale);
    _jpeg.close();
}

#endif // _JPEGFUNC_H_
