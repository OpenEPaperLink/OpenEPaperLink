#include "drawing.h"

#include <Arduino.h>
#include <stdbool.h>

#include "board.h"
#include "eeprom.h"
#include "proto.h"
#include "timer.h"
#include "screen.h"
#include "userinterface.h" // for addIcons

#define COMPRESSION_BITPACKED_3x5_to_7 0x62700357 // 3 pixels of 5 possible colors in 7 bits
#define COMPRESSION_BITPACKED_5x3_to_8 0x62700538 // 5 pixels of 3 possible colors in 8 bits
#define COMPRESSION_BITPACKED_3x6_to_8 0x62700368 // 3 pixels of 6 possible colors in 8 bits

struct BitmapFileHeader
{
    uint8_t sig[2];
    uint32_t fileSz;
    uint8_t rfu[4];
    uint32_t dataOfst;
    uint32_t headerSz; // 40
    int32_t width;
    int32_t height;
    uint16_t colorplanes; // must be one
    uint16_t bpp;
    uint32_t compression;
    uint32_t dataLen; // may be 0
    uint32_t pixelsPerMeterX;
    uint32_t pixelsPerMeterY;
    uint32_t numColors; // if zero, assume 2^bpp
    uint32_t numImportantColors;
};

struct BitmapClutEntry
{
    uint8_t b, g, r, x;
};

struct BitmapDrawInfo
{
    // dimensions
    uint16_t w, h, effectiveW, effectiveH, stride /* 0 -> 1, 5 - >7, 255 -> 256 */;
    uint8_t numColorsM1;

    // data start
    uint32_t dataAddr;

    // compression state
    uint8_t packetPixelDivVal;
    uint8_t packetNumPixels;
    uint8_t packetBitSz;
    uint8_t packetBitMask; // derived from the above

    // flags
    uint8_t bpp : 4;
    uint8_t bottomUp : 1;
};

uint8_t mPassNo = 0;

static const uint8_t mColorMap[][6] = {
    // colors are: B, DG, G, LG, W, R
    // phase 0 (LUTS: B:W:R:G, purpose: BWR, prepare greys)
    {1, 1, 1, 1, 0, 0}, // lo plane (B)

    {0, 0, 0, 0, 0, 1} // hi plane (R)
};

static uint8_t mClutMap[256];
static struct BitmapDrawInfo mDrawInfo;

static uint32_t drawPrvParseHeader(uint32_t addr) // return clut addr or zero on error
{
    struct BitmapFileHeader bmph;
    uint16_t packetsPerRow;

    addr += sizeof(struct EepromImageHeader);
    eepromRead(addr, &bmph, sizeof(bmph));

    if (bmph.sig[0] != 'B' || bmph.sig[1] != 'M')
        goto fail;

    if (bmph.colorplanes != 1)
        goto fail;

    if ((&bmph.headerSz - 40)) // < 40
        goto fail;

    if (bmph.bpp > 8)
        goto fail;

    mDrawInfo.bpp = bmph.bpp;

    if (!(&bmph.headerSz - 257)) // >= 257
        goto fail;

    if ((&bmph.numColors))
        mDrawInfo.numColorsM1 = (uint8_t)bmph.numColors - (uint8_t)1;
    else
        mDrawInfo.numColorsM1 = (uint8_t)((uint8_t)1 << (uint8_t)mDrawInfo.bpp) - (uint8_t)1;

    if (!(&bmph.height))
        goto fail;

    if ((&bmph.width - 1) || !(&bmph.width - 0xffff))
        goto fail;
    mDrawInfo.w = bmph.width;

    if ((&bmph.height) < 0)
    {
        if ((&bmph.height + 0xffff)) // carries if val too negative
            goto fail;
        mDrawInfo.h = -bmph.height;
        mDrawInfo.bottomUp = false;
    }
    else
    {
        if (!(&bmph.headerSz - 0xffff)) // no carry if val too big
            goto fail;
        mDrawInfo.h = bmph.height;
        mDrawInfo.bottomUp = true;
    }

    if (bmph.compression)
    {
        printf("compression is not supported ;(");
        goto fail;
    }

    mDrawInfo.packetPixelDivVal = 0;
    mDrawInfo.packetNumPixels = 1;
    if (mDrawInfo.bpp > 1)
    {
        mDrawInfo.packetBitSz = 2;
    }
    else
    {
        mDrawInfo.packetBitSz = 1; // mDrawInfo.bpp;
    }

    // mDrawInfo.stride = mathPrvDiv32x8(mathPrvMul16x8((mDrawInfo.w + mDrawInfo.packetNumPixels - 1), mDrawInfo.packetBitSz) + 31, 32) * 4UL;
    // mDrawInfo.packetBitMask = (uint8_t)(((uint8_t)1) << (uint8_t)mDrawInfo.packetBitSz) - (uint8_t)1;

    packetsPerRow = (mDrawInfo.w + mDrawInfo.packetNumPixels - 1) / (mDrawInfo.packetNumPixels);
    mDrawInfo.stride = (((packetsPerRow * mDrawInfo.packetBitSz) + 31) / 32) * 4UL;
    mDrawInfo.packetBitMask = (uint8_t)(((uint8_t)1) << (uint8_t)mDrawInfo.packetBitSz) - (uint8_t)1;

    // calc effective size
    mDrawInfo.effectiveH = (mDrawInfo.h > SCREEN_HEIGHT) ? SCREEN_HEIGHT : mDrawInfo.h;
    mDrawInfo.effectiveW = (mDrawInfo.w > SCREEN_WIDTH) ? SCREEN_WIDTH : mDrawInfo.w;

    // calc addrs
    mDrawInfo.dataAddr = addr + bmph.dataOfst;
    return addr + bmph.dataOfst - sizeof(struct BitmapClutEntry) * (1 + mDrawInfo.numColorsM1);

fail:
    printf("Tried to parse the bmp header, didn't work...");
    return 0;
}

static void drawPrvLoadAndMapClut(uint32_t clutAddr)
{
    struct BitmapClutEntry clut;
    uint8_t i;

    // convert clut to our understanding of color
    i = 0;
    do
    {
        uint8_t entry;

        eepromRead(clutAddr, &clut, sizeof(clut));
        clutAddr += sizeof(struct BitmapClutEntry);

        if (SCREEN_EXTRA_COLOR_INDEX >= 0 && clut.r == 0xff && (clut.g == 0xff || clut.g == 0) && clut.b == 0) // yellow/red
            entry = SCREEN_EXTRA_COLOR_INDEX;
        else
        {
            uint16_t intensity = 0;

            intensity += (0x37 * clut.r);
            intensity += (0xB7 * clut.g);
            intensity += (0x12 * clut.b);
            // adds up to 0xff00 -> fix it
            intensity += (uint8_t)(intensity >> 8);

            entry = (intensity * SCREEN_NUM_GREYS) >> 16;
            entry += SCREEN_FIRST_GREY_IDX;
        }
        // printf("mapped clut %u (%d %d %d) -> %d\n", i, clut.r, clut.g, clut.b, entry);
        mClutMap[i] = entry;
    } while (i++ != mDrawInfo.numColorsM1);

    // replicate clut down if not a full 256-entry clut
    if (mDrawInfo.bpp != 8)
    {
        uint8_t num = (uint8_t)((uint8_t)1 << (uint8_t)mDrawInfo.bpp);

        // we can use the fact that our memcpy always copies forward
        memcpy(mClutMap + num, mClutMap, (uint8_t)256 - (uint8_t)num);
    }
}

static void drawPrvDecodeImageOnce(void)
{
    uint8_t rowBuf[SCREEN_WIDTH];
    uint16_t er, c;
    if (mDrawInfo.bottomUp)
        er = mDrawInfo.effectiveH - 1;
    else
        er = 0;
    while (1)
    { // we account differently for loop gets compiled worse
        uint8_t inIdx = 0, bitpoolInUsed = 0, bitpoolIn = 0;
        uint16_t nBytesOut = 0;

#if SCREEN_TX_BPP == 4
        uint8_t txPrev = 0;
        boolean emit = false;
#else
        uint8_t bitpoolOutUsedUsed = 0;
        uint16_t bitpoolOut = 0;
#endif
        // get a row
        epdDeselect();
        eepromRead((er * mDrawInfo.stride) + mDrawInfo.dataAddr, rowBuf, mDrawInfo.stride);
        epdSelect();
        // convert to our format
        c = mDrawInfo.effectiveW;
        do
        {
            // uartTx('.');
            uint8_t packet, packetIdx, packetMembers = mDrawInfo.packetNumPixels;

            if (bitpoolInUsed >= mDrawInfo.packetBitSz)
            {
                bitpoolInUsed -= mDrawInfo.packetBitSz;
                packet = bitpoolIn >> bitpoolInUsed;
            }
            else
            {
                uint8_t packetBitSz = mDrawInfo.packetBitSz;
                uint8_t t = rowBuf[inIdx++];

                packet = (bitpoolIn << (packetBitSz - bitpoolInUsed)) | (t >> (8 - (packetBitSz - bitpoolInUsed)));
                bitpoolInUsed += 8 - packetBitSz;

                bitpoolIn = t;
            }
            packet &= mDrawInfo.packetBitMask;

            // val is now a packet - unpack it
            if (packetMembers > c)
                packetMembers = c;

            for (packetIdx = 0; packetIdx < packetMembers; packetIdx++)
            {
                uint8_t val;

                // extract
                if (mDrawInfo.packetPixelDivVal)
                {
                    val = packet % mDrawInfo.packetPixelDivVal;
                    packet /= mDrawInfo.packetPixelDivVal;
                }
                else
                    val = packet;

                // map
                val = mClutMap[val];

// get bits out
#if SCREEN_TX_BPP == 4

                if (emit)
                {
                    emit = false;
                    ByteDecode(txPrev | val);
                    nBytesOut++;
                    txPrev = 0;
                }
                else
                {
                    emit = true;
                    txPrev = val << 4;
                }

#else
                bitpoolOut <<= SCREEN_TX_BPP;
                bitpoolOut |= val;
                bitpoolOutUsedUsed += SCREEN_TX_BPP;
                if (bitpoolOutUsedUsed >= 8)
                {
                    ByteDecode(bitpoolOut >> (bitpoolOutUsedUsed -= 8));
                    bitpoolOut &= (1 << bitpoolOutUsedUsed) - 1;
                    nBytesOut++;
                }
#endif
            }
            c -= packetMembers;
        } while (c);

#if SCREEN_TX_BPP == 4

        if (emit)
        {
            ByteDecode(txPrev);
            nBytesOut++;
        }

#else

        if (bitpoolOutUsedUsed)
        {
            ByteDecode(bitpoolOut);
            nBytesOut++;
        }

#endif

        // if we did not produce enough bytes, do so
        nBytesOut = ((long)SCREEN_WIDTH * SCREEN_TX_BPP + 7) / 8 - nBytesOut;
        while (nBytesOut--)
            ByteDecode(SCREEN_BYTE_FILL);

        // update row
        if (mDrawInfo.bottomUp)
        {
            if (er)
                er--;
            else
                break;
        }
        else
        {
            er++;
            if (er == mDrawInfo.effectiveH)
                break;
        }
    }

    // fill the rest of the screen
    for (er = mDrawInfo.effectiveH - SCREEN_HEIGHT; er; er--)
    {
        for (c = ((long)SCREEN_WIDTH * SCREEN_TX_BPP + 7) / 8; c; c--)
        {
            ByteDecode(SCREEN_BYTE_FILL);
        }
    }
}

static uint8_t prev, step = 0;

void ByteDecode(uint8_t byte)
{
    prev <<= 2;
    prev |= (mColorMap[mPassNo][byte >> 4] << 1) | mColorMap[mPassNo][byte & 0x0f];
    if (++step == 4)
    {
        step = 0;
        epdSend(prev);
    }
}
#if (SCREEN_WIDTH == 152)
void drawImageFromBuffer(uint8_t *buffer, const uint8_t lut)
{
    printf("Doing raw 1bpp\n");
    epdSetup();
    if (lut)
        selectLUT(lut);
    beginFullscreenImage();
    clearScreen();
    beginWriteFramebuffer(EPD_COLOR_BLACK);
    epdSelect();
    for (uint16_t c = 0; c < (SCREEN_HEIGHT * (SCREEN_WIDTH / 8)); c++)
    {
        epdSend(buffer[c]);
    }
    epdDeselect();
    endWriteFramebuffer();
    addOverlay();
    drawWithSleep();
}
#endif
void drawImageAtAddress(uint32_t addr, uint8_t lut)
{
    struct EepromImageHeader *eih = (struct EepromImageHeader *)mClutMap;
    eepromRead(addr, mClutMap, sizeof(struct EepromImageHeader));
    switch (eih->dataType)
    {
    case DATATYPE_IMG_RAW_1BPP:
        printf("Doing raw 1bpp\n");
        epdSetup();
        if (lut)
            selectLUT(lut);
        beginFullscreenImage();
        clearScreen();
        beginWriteFramebuffer(EPD_COLOR_BLACK);
        epdSelect();
        for (uint16_t c = 0; c < (SCREEN_HEIGHT * (SCREEN_WIDTH / 8)); c++)
        {
            if (c % 256 == 0)
            {
                epdDeselect();
                eepromRead(addr + sizeof(struct EepromImageHeader) + c, mClutMap, 256);
                epdSelect();
            }
            epdSend(mClutMap[c % 256]);
        }
        epdDeselect();
        endWriteFramebuffer();
        break;
    case DATATYPE_IMG_RAW_2BPP:
        printf("Doing raw 2bpp\n");
        epdSetup();
        if (lut)
            selectLUT(lut);
        beginFullscreenImage();
        beginWriteFramebuffer(EPD_COLOR_BLACK);
        epdSelect();
        for (uint16_t c = 0; c < (SCREEN_HEIGHT * (SCREEN_WIDTH / 8)); c++)
        {
            if (c % 256 == 0)
            {
                epdDeselect();
                eepromRead(addr + sizeof(struct EepromImageHeader) + c, mClutMap, 256);
                epdSelect();
            }
            epdSend(mClutMap[c % 256]);
        }
        epdDeselect();
        endWriteFramebuffer();

        beginWriteFramebuffer(EPD_COLOR_RED);
        epdSelect();
        for (uint16_t c = 0; c < (SCREEN_HEIGHT * (SCREEN_WIDTH / 8)); c++)
        {
            if (c % 256 == 0)
            {
                epdDeselect();
                eepromRead(addr + sizeof(struct EepromImageHeader) + (SCREEN_HEIGHT * (SCREEN_WIDTH / 8)) + c, mClutMap, 256);
                epdSelect();
            }
            epdSend(mClutMap[c % 256]);
        }
        epdDeselect();
        endWriteFramebuffer();
        break;
    case DATATYPE_IMG_BMP:;
        uint32_t clutAddr;
        printf("sending BMP to EPD - ");
        clutAddr = drawPrvParseHeader(addr);
        if (!clutAddr)
            return;
        drawPrvLoadAndMapClut(clutAddr);

        epdSetup();
        if (lut)
            selectLUT(lut);
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

        printf(" complete.\n");
        break;
    default: // prevent drawing from an unknown file image type
        printf("Image with type 0x%02X was requested, but we don't know what to do with that currently...\n", eih->dataType);
        return;
    }
    addOverlay();
    drawWithSleep();
}