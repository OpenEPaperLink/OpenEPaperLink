#ifndef __GROUP5__
#define __GROUP5__
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#ifdef __AVR__
#include <avr/pgmspace.h>
#endif
//
// Group5 1-bit image compression library
// Written by Larry Bank
// Copyright (c) 2024 BitBank Software, Inc.
//
// Use of this software is governed by the Business Source License
// included in the file ./LICENSE.
//
// As of the Change Date specified in that file, in accordance with
// the Business Source License, use of this software will be governed
// by the Apache License, Version 2.0, included in the file
// ./APL.txt.
//
// The name "Group5" is derived from the CCITT Group4 standard
// This code is based on a lot of the good ideas from CCITT T.6
// for FAX image compression, but modified to work in a very
// constrained environment. The Huffman tables for horizontal
// mode have been replaced with a simple 2-bit flag followed by
// short or long counts of a fixed length. The short codes are
// always 3 bits (run lengths 0-7) and the long codes are the
// number of bits needed to encode the width of the image.
// For example, if a 320 pixel wide image is being compressed,
// the longest horizontal run needed is 320, which requires 9
// bits to encode. The 2 prefix bits have the following meaning:
// 00 = short, short (3+3 bits)
// 01 = short, long (3+N bits)
// 10 = long, short (N+3 bits)
// 11 = long, long (N+N bits)
// The rest of the code works identically to Group4 2D FAX
//
// Caution - this is the maximum number of color changes per line
// The default value is set low to work embedded systems with little RAM
// for font compression, this is plenty since each line of a character should have
// a maximum of 7 color changes
// You can define this in your compiler macros to override the default vlaue
//


#define MAX_IMAGE_FLIPS 640

#ifndef MAX_IMAGE_FLIPS
#ifdef __AVR__
#define MAX_IMAGE_FLIPS 32
#else
#define MAX_IMAGE_FLIPS 512
#endif // __AVR__
#endif
// Horizontal prefix bits
enum {
    HORIZ_SHORT_SHORT=0,
    HORIZ_SHORT_LONG,
    HORIZ_LONG_SHORT,
    HORIZ_LONG_LONG
};

// Return code for encoder and decoder
enum {
    G5_SUCCESS = 0,
    G5_INVALID_PARAMETER,
    G5_DECODE_ERROR,
    G5_UNSUPPORTED_FEATURE,
    G5_ENCODE_COMPLETE,
    G5_DECODE_COMPLETE,
    G5_NOT_INITIALIZED,
    G5_DATA_OVERFLOW,
    G5_MAX_FLIPS_EXCEEDED
};
//
// Decoder state
//
typedef struct g5_dec_image_tag
{
    int iWidth, iHeight; // image size
    int iError;
    int y; // last y value drawn
    int iVLCSize;
    int iHLen; // length of 'long' horizontal codes for this image
    int iPitch; // width in bytes of output buffer
    uint32_t u32Accum; // fractional scaling accumulator
    uint32_t ulBitOff, ulBits; // vlc decode variables
    uint8_t *pSrc, *pBuf; // starting & current buffer pointer
    int16_t *pCur, *pRef; // current state of current vs reference flips
    int16_t CurFlips[MAX_IMAGE_FLIPS];
    int16_t RefFlips[MAX_IMAGE_FLIPS];
} G5DECIMAGE;

// Due to unaligned memory causing an exception, we have to do these macros the slow way
#ifdef __AVR__
// assume PROGMEM as the source of data
inline uint32_t TIFFMOTOLONG(uint8_t *p)
{
  uint32_t u32 = pgm_read_dword(p);
  return __builtin_bswap32(u32);
}
#else
#define TIFFMOTOLONG(p) (((uint32_t)(*p)<<24UL) + ((uint32_t)(*(p+1))<<16UL) + ((uint32_t)(*(p+2))<<8UL) + (uint32_t)(*(p+3)))
#endif // __AVR__

#define TOP_BIT 0x80000000
#define MAX_VALUE 0xffffffff
// Must be a 32-bit target processor
#define REGISTER_WIDTH 32
#define BIGUINT uint32_t

//
// G5 Encoder
//

typedef struct pil_buffered_bits
{
unsigned char *pBuf; // buffer pointer
uint32_t ulBits; // buffered bits
uint32_t ulBitOff; // current bit offset
uint32_t ulDataSize; // available data
} BUFFERED_BITS;

//
// Encoder state
//
typedef struct g5_enc_image_tag
{
    int iWidth, iHeight; // image size
    int iError;
    int y; // last y encoded
    int iOutSize;
    int iDataSize; // generated output size
    uint8_t *pOutBuf;
    int16_t *pCur, *pRef; // pointers to swap current and reference lines
    BUFFERED_BITS bb;
    int16_t CurFlips[MAX_IMAGE_FLIPS];
    int16_t RefFlips[MAX_IMAGE_FLIPS];
} G5ENCIMAGE;

// 16-bit marker at the start of a BB_FONT file
// (BitBank FontFile)
#define BB_FONT_MARKER 0xBBFF
// 16-bit marker at the start of a BB_BITMAP file
// (BitBank BitmapFile)
#define BB_BITMAP_MARKER 0xBBBF

// Font info per character (glyph)
typedef struct {
  uint16_t bitmapOffset; // Offset to compressed bitmap data for this glyph (starting from the end of the BB_GLYPH[] array)
  uint8_t width;         // bitmap width in pixels
  uint8_t xAdvance;      // total width in pixels (bitmap + padding)
  uint16_t height;        // bitmap height in pixels
  int16_t xOffset;        // left padding to upper left corner
  int16_t yOffset;        // padding from baseline to upper left corner (usually negative)
} BB_GLYPH;

// This structure is stored at the beginning of a BB_FONT file
typedef struct {
  uint16_t u16Marker; // 16-bit Marker defining a BB_FONT file
  uint16_t first;      // first char (ASCII value)
  uint16_t last;       // last char (ASCII value)
  uint16_t height;    // total height of font
  uint32_t rotation; // store this as 32-bits to not have a struct packing problem
  BB_GLYPH glyphs[];  // Array of glyphs (one for each char)
} BB_FONT;

// This structure defines the start of a compressed bitmap file
typedef struct {
    uint16_t u16Marker; // 16-bit marker defining a BB_BITMAP file
    uint16_t width;
    uint16_t height;
    uint16_t size; // compressed data size (not including this 8-byte header)
} BB_BITMAP;

#ifdef __cplusplus
//
// The G5 classes wrap portable C code which does the actual work
//
class G5ENCODER
{
  public:
    int init(int iWidth, int iHeight, uint8_t *pOut, int iOutSize);
    int encodeLine(uint8_t *pPixels);
    int size();

  private:
    G5ENCIMAGE _g5enc;
};
class G5DECODER
{
  public:
    int init(int iWidth, int iHeight, uint8_t *pData, int iDataSize);
    int decodeLine(uint8_t *pOut);

  private:
    G5DECIMAGE _g5dec;
};
#endif // __cplusplus

#endif // __GROUP5__
