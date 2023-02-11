#include "drawing.h"

#include <stdbool.h>

#include "asmUtil.h"
#include "board.h"
#include "cpu.h"
#include "eeprom.h"
#include "epd.h"
#include "printf.h"
#include "screen.h"
#include "timer.h"

#define COMPRESSION_BITPACKED_3x5_to_7 0x62700357  // 3 pixels of 5 possible colors in 7 bits
#define COMPRESSION_BITPACKED_5x3_to_8 0x62700538  // 5 pixels of 3 possible colors in 8 bits
#define COMPRESSION_BITPACKED_3x6_to_8 0x62700368  // 3 pixels of 6 possible colors in 8 bits

struct BitmapFileHeader {
    uint8_t sig[2];
    uint32_t fileSz;
    uint8_t rfu[4];
    uint32_t dataOfst;
    uint32_t headerSz;  // 40
    int32_t width;
    int32_t height;
    uint16_t colorplanes;  // must be one
    uint16_t bpp;
    uint32_t compression;
    uint32_t dataLen;  // may be 0
    uint32_t pixelsPerMeterX;
    uint32_t pixelsPerMeterY;
    uint32_t numColors;  // if zero, assume 2^bpp
    uint32_t numImportantColors;
};

struct BitmapClutEntry {
    uint8_t b, g, r, x;
};

struct BitmapDrawInfo {
    // dimensions
    uint16_t w, h, effectiveW, effectiveH, stride /* 0 -> 1, 5 - >7, 255 -> 256 */;
    uint8_t numColorsM1;

    // data start
    uint32_t dataAddr;

    // compression state
    uint8_t packetPixelDivVal;
    uint8_t packetNumPixels;
    uint8_t packetBitSz;
    uint8_t packetBitMask;  // derived from the above

    // flags
    uint8_t bpp : 4;
    uint8_t bottomUp : 1;
};

uint8_t __xdata mPassNo = 0;

static const uint8_t __code mColorMap[][6] = {
    // colors are: B, DG, G, LG, W, R
    // phase 0 (LUTS: B:W:R:G, purpose: BWR, prepare greys)
    {1, 1, 1, 1, 0, 0},  // lo plane (B)

    {0, 0, 0, 0, 0, 1}  // hi plane (R)
};

static uint8_t __xdata mClutMap[256];
static struct BitmapDrawInfo __xdata mDrawInfo;

#pragma callee_saves drawPrvParseHeader
static uint32_t drawPrvParseHeader(uint32_t addr)  // return clut addr or zero on error
{
    struct BitmapFileHeader __xdata bmph;
    uint16_t __xdata packetsPerRow;

    addr += sizeof(struct EepromImageHeader);
    eepromRead(addr, &bmph, sizeof(bmph));

    if (bmph.sig[0] != 'B' || bmph.sig[1] != 'M')
        goto fail;

    if (bmph.colorplanes != 1)
        goto fail;

    if (u32minusU16(&bmph.headerSz, 40))  // < 40
        goto fail;

    if (bmph.bpp > 8)
        goto fail;

    mDrawInfo.bpp = bmph.bpp;

    if (!u32minusU16(&bmph.headerSz, 257))  // >= 257
        goto fail;

    if (u32Nonzero(&bmph.numColors))
        mDrawInfo.numColorsM1 = (uint8_t)bmph.numColors - (uint8_t)1;
    else
        mDrawInfo.numColorsM1 = (uint8_t)((uint8_t)1 << (uint8_t)mDrawInfo.bpp) - (uint8_t)1;

    if (!u32Nonzero(&bmph.height))
        goto fail;

    if (u32minusU16(&bmph.width, 1) || !u32minusU16(&bmph.width, 0xffff))
        goto fail;
    mDrawInfo.w = bmph.width;

    if (i32Negative(&bmph.height)) {
        if (u32plusU16(&bmph.height, 0xffff))  // carries if val too negative
            goto fail;
        mDrawInfo.h = -bmph.height;
        mDrawInfo.bottomUp = false;
    } else {
        if (!u32minusU16(&bmph.headerSz, 0xffff))  // no carry if val too big
            goto fail;
        mDrawInfo.h = bmph.height;
        mDrawInfo.bottomUp = true;
    }

    if (bmph.compression) {
        pr("compression is not supported ;(");
        goto fail;
    }

    mDrawInfo.packetPixelDivVal = 0;
    mDrawInfo.packetNumPixels = 1;
    if (mDrawInfo.bpp > 1) {
        mDrawInfo.packetBitSz = 2;
    } else {
        mDrawInfo.packetBitSz = 1;  // mDrawInfo.bpp;
    }

    // mDrawInfo.stride = mathPrvDiv32x8(mathPrvMul16x8((mDrawInfo.w + mDrawInfo.packetNumPixels - 1), mDrawInfo.packetBitSz) + 31, 32) * 4UL;
    // mDrawInfo.packetBitMask = (uint8_t)(((uint8_t)1) << (uint8_t)mDrawInfo.packetBitSz) - (uint8_t)1;

    packetsPerRow = mathPrvDiv16x8(mDrawInfo.w + mDrawInfo.packetNumPixels - 1, mDrawInfo.packetNumPixels);
    mDrawInfo.stride = mathPrvDiv32x8(mathPrvMul16x8(packetsPerRow, mDrawInfo.packetBitSz) + 31, 32) * 4UL;
    mDrawInfo.packetBitMask = (uint8_t)(((uint8_t)1) << (uint8_t)mDrawInfo.packetBitSz) - (uint8_t)1;

    // calc effective size
    mDrawInfo.effectiveH = (mDrawInfo.h > SCREEN_HEIGHT) ? SCREEN_HEIGHT : mDrawInfo.h;
    mDrawInfo.effectiveW = (mDrawInfo.w > SCREEN_WIDTH) ? SCREEN_WIDTH : mDrawInfo.w;

    // calc addrs
    mDrawInfo.dataAddr = addr + bmph.dataOfst;
    return addr + bmph.dataOfst - sizeof(struct BitmapClutEntry) * (1 + mDrawInfo.numColorsM1);

// seriously, fuck SDCC
fail:
    pr("Tried to parse the bmp header, didn't work...");
    return 0;
}

#pragma callee_saves drawPrvLoadAndMapClut
static void drawPrvLoadAndMapClut(uint32_t clutAddr) {
    struct BitmapClutEntry __xdata clut;
    uint8_t __xdata i;

    // convert clut to our understanding of color
    i = 0;
    do {
        uint8_t __xdata entry;

        eepromRead(clutAddr, &clut, sizeof(clut));
        clutAddr += sizeof(struct BitmapClutEntry);

        if (SCREEN_EXTRA_COLOR_INDEX >= 0 && clut.r == 0xff && (clut.g == 0xff || clut.g == 0) && clut.b == 0)  // yellow/red
            entry = SCREEN_EXTRA_COLOR_INDEX;
        else {
            uint16_t intensity = 0;

            intensity += mathPrvMul8x8(0x37, clut.r);
            intensity += mathPrvMul8x8(0xB7, clut.g);
            intensity += mathPrvMul8x8(0x12, clut.b);
            // adds up to 0xff00 -> fix it
            intensity += (uint8_t)(intensity >> 8);

            entry = mathPrvMul16x8(intensity, SCREEN_NUM_GREYS) >> 16;
            entry += SCREEN_FIRST_GREY_IDX;
        }
        // pr("mapped clut %u (%d %d %d) -> %d\n", i, clut.r, clut.g, clut.b, entry);
        mClutMap[i] = entry;
    } while (i++ != mDrawInfo.numColorsM1);

    // replicate clut down if not a full 256-entry clut
    if (mDrawInfo.bpp != 8) {
        uint8_t num = (uint8_t)((uint8_t)1 << (uint8_t)mDrawInfo.bpp);

        // we can use the fact that our memcpy always copies forward
        xMemCopyShort(mClutMap + num, mClutMap, (uint8_t)256 - (uint8_t)num);
    }
}

#pragma callee_saves drawPrvDecodeImageOnce
static void drawPrvDecodeImageOnce(void) {
    uint8_t __xdata rowBuf[SCREEN_WIDTH];
    uint16_t __xdata er, c;
    if (mDrawInfo.bottomUp)
        er = mDrawInfo.effectiveH - 1;
    else
        er = 0;
    while (1) {  // we account differently for loop gets compiled worse
        uint8_t __xdata inIdx = 0, bitpoolInUsed = 0, bitpoolIn = 0;
        uint16_t __xdata nBytesOut = 0;

#if SCREEN_TX_BPP == 4
        uint8_t __xdata txPrev = 0;
        __bit emit = false;
#else
        uint8_t __xdata bitpoolOutUsedUsed = 0;
        uint16_t __xdata bitpoolOut = 0;
#endif
        // get a row
        eepromRead(mathPrvMul16x16(er, mDrawInfo.stride) + mDrawInfo.dataAddr, rowBuf, mDrawInfo.stride);
        // convert to our format
        c = mDrawInfo.effectiveW;
        do {
            // uartTx('.');
            uint8_t packet, packetIdx, packetMembers = mDrawInfo.packetNumPixels;

            if (bitpoolInUsed >= mDrawInfo.packetBitSz) {
                bitpoolInUsed -= mDrawInfo.packetBitSz;
                packet = bitpoolIn >> bitpoolInUsed;
            } else {
                uint8_t __xdata packetBitSz = mDrawInfo.packetBitSz;
                uint8_t __xdata t = rowBuf[inIdx++];

                packet = (bitpoolIn << (packetBitSz - bitpoolInUsed)) | (t >> (8 - (packetBitSz - bitpoolInUsed)));
                bitpoolInUsed += 8 - packetBitSz;

                bitpoolIn = t;
            }
            packet &= mDrawInfo.packetBitMask;

            // val is now a packet - unpack it
            if (packetMembers > c)
                packetMembers = c;

            for (packetIdx = 0; packetIdx < packetMembers; packetIdx++) {
                uint8_t __xdata val;

                // extract
                if (mDrawInfo.packetPixelDivVal) {
                    val = packet % mDrawInfo.packetPixelDivVal;
                    packet /= mDrawInfo.packetPixelDivVal;
                } else
                    val = packet;

                // map
                val = mClutMap[val];

// get bits out
#if SCREEN_TX_BPP == 4

                if (emit) {
                    emit = false;
                    ByteDecode(txPrev | val);
                    nBytesOut++;
                    txPrev = 0;
                } else {
                    emit = true;
                    txPrev = val << 4;
                }

#else
                bitpoolOut <<= SCREEN_TX_BPP;
                bitpoolOut |= val;
                bitpoolOutUsedUsed += SCREEN_TX_BPP;
                if (bitpoolOutUsedUsed >= 8) {
                    ByteDecode(bitpoolOut >> (bitpoolOutUsedUsed -= 8));
                    bitpoolOut &= (1 << bitpoolOutUsedUsed) - 1;
                    nBytesOut++;
                }
#endif
            }
            c -= packetMembers;
        } while (c);

#if SCREEN_TX_BPP == 4

        if (emit) {
            ByteDecode(txPrev);
            nBytesOut++;
        }

#else

        if (bitpoolOutUsedUsed) {
            ByteDecode(bitpoolOut);
            nBytesOut++;
        }

#endif

        // if we did not produce enough bytes, do so
        nBytesOut = ((long)SCREEN_WIDTH * SCREEN_TX_BPP + 7) / 8 - nBytesOut;
        while (nBytesOut--)
            ByteDecode(SCREEN_BYTE_FILL);

        // update row
        if (mDrawInfo.bottomUp) {
            if (er)
                er--;
            else
                break;
        } else {
            er++;
            if (er == mDrawInfo.effectiveH)
                break;
        }
    }

    // fill the rest of the screen
    for (er = mDrawInfo.effectiveH - SCREEN_HEIGHT; er; er--) {
        for (c = ((long)SCREEN_WIDTH * SCREEN_TX_BPP + 7) / 8; c; c--) {
            ByteDecode(SCREEN_BYTE_FILL);
        }
    }
}

static uint8_t __xdata prev, step = 0;

void ByteDecode(uint8_t byte) {
    prev <<= 2;
    prev |= (mColorMap[mPassNo][byte >> 4] << 1) | mColorMap[mPassNo][byte & 0x0f];
    if (++step == 4) {
        step = 0;
        epdSelect();
        epdSend(prev);
        epdDeselect();
    }
}

void drawImageAtAddress(uint32_t addr, uint8_t lut) {
    uint32_t __xdata clutAddr;
    pr("sending to EPD - ");
    clutAddr = drawPrvParseHeader(addr);
    if (!clutAddr)
        return;
    drawPrvLoadAndMapClut(clutAddr);

    epdSetup();
    if(lut)selectLUT(lut);
    mPassNo = 0;
    beginFullscreenImage();
    beginWriteFramebuffer(EPD_COLOR_BLACK);
    prev = 0;
    step = 0;
    drawPrvDecodeImageOnce();
    endWriteFramebuffer();
    mPassNo++;
    beginFullscreenImage();
    beginWriteFramebuffer(EPD_COLOR_RED);
    prev = 0;
    step = 0;
    drawPrvDecodeImageOnce();
    endWriteFramebuffer();

    pr(" complete.\n");

    drawWithSleep();
}
