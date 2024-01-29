#ifndef _DRAWING_H_
#define _DRAWING_H_

#include <stdint.h>
#include "oepl_fs.h"

#define COLOR_RED 1
#define COLOR_BLACK 0
#define COLOR_DUAL 2

#define IMAGE_OR 1
#define IMAGE_REPLACE 0

#define DRAW_INVERTED 1
#define DRAW_NORMAL 0

#define FILENAME_LENGTH 32

typedef struct __attribute__ ((packed)) {
    uint16_t bitmapOffset;  ///< Pointer into GFXfont->bitmap
    uint8_t width;          ///< Bitmap dimensions in pixels
    uint8_t height;         ///< Bitmap dimensions in pixels
    uint8_t xAdvance;       ///< Distance to advance cursor (x axis)
    int8_t xOffset;         ///< X dist from cursor pos to UL corner
    int8_t yOffset;         ///< Y dist from cursor pos to UL corner
}  GFXglyph;

enum rotation {
    ROTATE_0,
    ROTATE_90,
    ROTATE_180,
    ROTATE_270
};

typedef struct __attribute__ ((packed)) {
    uint16_t first;
    uint16_t last;
    uint8_t yAdvance;
    char glyphFile[FILENAME_LENGTH];
    char bitmapFile[FILENAME_LENGTH];
} GFXFontOEPL;


extern "C"{
    void drawImageAtAddress(uint32_t addr, uint8_t lut);
}

void addBufferedImage(uint16_t x, uint16_t y, bool color, enum rotation ro, const uint8_t *image, bool mask);
// void addFlashImage(uint16_t x, uint16_t y, bool color, enum rotation ro, const uint8_t *image);
void addQR(uint16_t x, uint16_t y, uint8_t version, uint8_t scale, const char *c, ...);
void drawRoundedRectangle(uint16_t xpos, uint16_t ypos, uint16_t width, uint16_t height, bool color);
void drawMask(uint16_t xpos, uint16_t ypos, uint16_t width, uint16_t height, bool color);
void addFSImage(uint16_t x, uint16_t y, uint8_t color, enum rotation ro, char *name);
void addCompressedFSImage(uint16_t x, uint16_t y, enum rotation ro, char *name);

class drawItem {
   public:
    drawItem();
    ~drawItem();
    void setRotation(enum rotation ro);
    void addItem(uint8_t *data, uint16_t width, uint16_t height);
    bool addToList();

    static void shiftBytesRight(uint8_t *data, uint8_t shift, uint8_t len);
    static void renderDrawLine(uint8_t *line, uint16_t number, uint8_t c);
    static void flushDrawItems();

    // these are also used for rotated screens
    static void reverseBytes(uint8_t *src, uint8_t src_len);
    static uint8_t bitReverse(uint8_t byte);

    enum drawType {
        DRAW_FONT,
        DRAW_BUFFERED_1BPP,
        DRAW_MASK,
        DRAW_EEPROM_1BPP,
        DRAW_EEPROM_2BPP,
        DRAW_COMPRESSED,
        DRAW_OEPLFS_1BPP,
        DRAW_OEPLFS_2BPP
    } type;

    int16_t xpos;
    int16_t ypos;

    enum rotation rotate = ROTATE_0;

    uint8_t color = 0;

    bool direction = false;

    bool mirrorH = false;
    bool mirrorV = false;
    uint16_t width;
    uint16_t height;

    uint8_t imageHeaderOffset = 0;

    // if this is true, clean up the reference (free memory).
    bool cleanUp = true;

   protected:
    void copyWithByteShift(uint8_t *dst, uint8_t *src, uint8_t src_len, uint8_t offset);

    void getDrawLine(uint8_t *line, uint16_t number, uint8_t c);
    void getXLine(uint8_t *line, uint16_t yPos, uint8_t color);
    void getYLine(uint8_t *line, uint16_t xPos, uint8_t color);
    uint8_t widthBytes = 0;
    uint8_t *buffer = nullptr;
};

class fontrender {
   public:
    void epdPrintf(uint16_t x, uint16_t y, bool color, enum rotation ro, const char *c, ...);
    fontrender(char* name);
    ~fontrender();
    void setFont(char* name);

   protected:
    GFXFontOEPL gfxFont;
    uint16_t bufferByteWidth = 0;
    uint8_t *fb = nullptr;
    uint16_t Xpixels;
    uint8_t drawChar(int32_t x, int32_t y, uint16_t c, uint8_t size);
    uint8_t getCharWidth(uint16_t c);
    void drawFastHLine(uint16_t x, uint16_t y, uint16_t w);
    void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    GFXglyph getGlyph(uint16_t c);
    OEPLFile *glyphFile = nullptr;
    OEPLFile *bitmapFile = nullptr;
};

#endif
