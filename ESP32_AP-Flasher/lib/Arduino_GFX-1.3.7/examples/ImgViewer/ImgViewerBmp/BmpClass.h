/*******************************************************************************
 * BMP Class
 * 
 * Rewrite from: https://github.com/Jaycar-Electronics/Arduino-Picture-Frame.git
 ******************************************************************************/
#ifndef _BMPCLASS_H_
#define _BMPCLASS_H_

/* Wio Terminal */
#if defined(ARDUINO_ARCH_SAMD) && defined(SEEED_GROVE_UI_WIRELESS)
#include <Seeed_FS.h>
#elif defined(ESP32) || defined(ESP8266)
#include <FS.h>
#else
#include <SD.h>
#endif

typedef void(BMP_DRAW_CALLBACK)(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h);

class BmpClass
{
public:
    void draw(
        File *f, BMP_DRAW_CALLBACK *bmpDrawCallback, bool useBigEndian,
        int16_t x, int16_t y, int16_t widthLimit, int16_t heightLimit)
    {
        _bmpDrawCallback = bmpDrawCallback;
        _useBigEndian = useBigEndian;
        _heightLimit = heightLimit;

        int16_t u, v;
        uint32_t xend;

        getbmpparms(f);

        //validate bitmap
        if ((bmtype == 19778) && (bmwidth > 0) && (bmheight > 0) && (bmbpp > 0))
        {
            //centre image
            u = (widthLimit - bmwidth) / 2;
            v = (heightLimit - bmheight) / 2;
            u = (u < 0) ? x : x + u;
            v = (v < 0) ? y : y + v;
            xend = (bmwidth > widthLimit) ? widthLimit : bmwidth;

            bmpRow = (uint16_t *)malloc(xend * 2);
            if (!bmpRow)
            {
                Serial.println(F("bmpRow malloc failed."));
            }
            if (bmbpp < 9)
            {
                bmplt = (uint16_t *)malloc(bmpltsize * 2);
                if (!bmplt)
                {
                    Serial.println(F("bmplt malloc failed."));
                }
                bmloadplt(f); //load palette if palettized
                drawbmpal(f, u, v, xend);
                free(bmplt);
            }
            else if (bmbpp == 16)
            {
                // TODO: bpp 16 should have 3 pixel types
                drawbmRgb565(f, u, v, xend);
            }
            else
            {
                drawbmtrue(f, u, v, xend);
            }
            free(bmpRow);
        }
    }

private:
    void bmloadplt(File *f)
    {
        byte r, g, b;
        if (bmpltsize == 0)
        {
            bmpltsize = 1 << bmbpp; //load default palette size
        }
        f->seek(54); //palette position in type 0x28 bitmaps
        for (int16_t i = 0; i < bmpltsize; i++)
        {
            b = f->read();
            g = f->read();
            r = f->read();
            f->read(); //dummy byte
            bmplt[i] = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
        }
    }

    void drawbmpal(File *f, int16_t u, int16_t v, uint32_t xend)
    {
        byte bmbitmask;
        int16_t i, ystart, bmppb, p, d;
        int16_t x, y;
        uint16_t c;
        bmbpl = ((bmbpp * bmwidth + 31) / 32) * 4; //bytes per line
        bmppb = 8 / bmbpp;                         //pixels/byte
        bmbitmask = ((1 << bmbpp) - 1);            //mask for each pixel
        ystart = 0;
        if (bmheight > _heightLimit)
        {
            ystart = bmheight - _heightLimit; //don't draw if it's outside screen
        }
        for (y = ystart; y < bmheight; y++)
        {                                   //invert in calculation (y=0 is bottom)
            f->seek(bmdataptr + y * bmbpl); //seek to start of line
            x = 0;
            p = 0;
            while (x < xend)
            {
                if (p < 1)
                {
                    d = f->read();
                    p = bmppb;
                }
                d = d << bmbpp;
                c = bmplt[(bmbitmask & (d >> 8))];
                bmpRow[x] = (_useBigEndian) ? ((c >> 8) | (c << 8)) : c;

                p--;
                x++;
            }
            _bmpDrawCallback(u, v + bmheight - 1 - y, bmpRow, xend, 1);
        }
    }

    // draw 16-bit colour (RGB565) bitmap
    void drawbmRgb565(File *f, int16_t u, int16_t v, uint32_t xend)
    {
        int16_t i, ystart;
        uint32_t x, y;
        byte lo, hi;
        bmbpl = ((bmbpp * bmwidth + 31) / 32) * 4; //bytes per line, due to 32bit chunks
        ystart = 0;
        if (bmheight > _heightLimit)
        {
            ystart = bmheight - _heightLimit; //don't draw if it's outside screen
        }
        Serial.println(xend);
        for (y = ystart; y < bmheight; y++)
        {                                   //invert in calculation (y=0 is bottom)
            f->seek(bmdataptr + (y * bmbpl)); //seek at start of line
            for (x = 0; x < xend; x++)
            {
                lo = f->read();
                hi = f->read();
                if (_useBigEndian)
                {
                    bmpRow[x] = hi | lo << 8;
                }
                else
                {
                    bmpRow[x] = lo | hi << 8;
                }
            }
            _bmpDrawCallback(u, v + bmheight - 1 - y, bmpRow, xend, 1);
        }
    }

    // draw true colour bitmap at (u,v) handles 24/32 not 16bpp yet
    void drawbmtrue(File *f, int16_t u, int16_t v, uint32_t xend)
    {
        int16_t i, ystart;
        uint32_t x, y;
        byte r, g, b;
        bmbpl = ((bmbpp * bmwidth + 31) / 32) * 4; //bytes per line, due to 32bit chunks
        ystart = 0;
        if (bmheight > _heightLimit)
        {
            ystart = bmheight - _heightLimit; //don't draw if it's outside screen
        }
        for (y = ystart; y < bmheight; y++)
        {                                   //invert in calculation (y=0 is bottom)
            f->seek(bmdataptr + y * bmbpl); //seek at start of line
            for (x = 0; x < xend; x++)
            {
                b = f->read();
                g = f->read();
                r = f->read();
                if (bmbpp == 32)
                {
                    f->read(); //dummy byte for 32bit
                }
                bmpRow[x] = (_useBigEndian) ? ((r & 0xf8) | (g >> 5) | ((g & 0x1c) << 11) | ((b & 0xf8) << 5)) : (((r & 0xf8) << 8) | ((g & 0xfc) << 3) | (b >> 3));
            }
            _bmpDrawCallback(u, v + bmheight - 1 - y, bmpRow, xend, 1);
        }
    }

    void getbmpparms(File *f)
    {               //load into globals as ints-some parameters are 32 bit, but we can't handle this size anyway
        byte h[48]; //header is 54 bytes typically, but we don't need it all
        int16_t i;
        f->seek(0); //set start of file
        for (i = 0; i < 48; i++)
        {
            h[i] = f->read(); //read header
        }
        bmtype = h[0] + (h[1] << 8);      //offset 0 'BM'
        bmdataptr = h[10] + (h[11] << 8); //offset 0xA pointer to image data
        bmhdrsize = h[14] + (h[15] << 8); //dib header size (0x28 is usual)
        //files may vary here, if !=28, unsupported type, put default values
        bmwidth = 0;
        bmheight = 0;
        bmbpp = 0;
        bmpltsize = 0;
        if ((bmhdrsize == 0x28) || (bmhdrsize == 0x38))
        {
            bmwidth = h[18] + (h[19] << 8);   //width
            bmheight = h[22] + (h[23] << 8);  //height
            bmbpp = h[28] + (h[29] << 8);     //bits per pixel
            bmpltsize = h[46] + (h[47] << 8); //palette size
        }
        // Serial.printf("bmtype: %d, bmhdrsize: %d, bmwidth: %d, bmheight: %d, bmbpp: %d\n", bmtype, bmhdrsize, bmwidth, bmheight, bmbpp);
    }

    byte isbmp(char n[])
    { //check if bmp extension
        int16_t k;
        k = strlen(n);
        if (k < 5)
        {
            return 0; //name not long enough
        }
        if (n[k - 1] != 'P')
        {
            return 0;
        }
        if (n[k - 2] != 'M')
        {
            return 0;
        }
        if (n[k - 3] != 'B')
        {
            return 0;
        }
        if (n[k - 4] != '.')
        {
            return 0;
        }
        return 1; //passes all tests
    }

    BMP_DRAW_CALLBACK *_bmpDrawCallback;
    bool _useBigEndian;
    int16_t _heightLimit;

    uint16_t bmtype, bmdataptr;                              //from header
    uint32_t bmhdrsize, bmwidth, bmheight, bmbpp, bmpltsize; //from DIB Header
    uint16_t bmbpl;                                          //bytes per line- derived
    uint16_t *bmplt;                                        //palette- stored encoded for LCD
    uint16_t *bmpRow;
};

#endif // _BMPCLASS_H_
