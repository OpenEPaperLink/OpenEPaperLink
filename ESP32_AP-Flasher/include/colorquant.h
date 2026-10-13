#pragma once

// Conversion of an RGB565 image to the colours of a tag, used by spr2color() in makeimage.cpp.
// Free of Arduino types.

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>

struct QuantColor {
    uint8_t r, g, b;
};

struct QuantParams {
    uint16_t bufw, bufh;        // output size (sprite size, swapped for a 90/270 degree buffer)
    uint8_t rotate;             // 0..3, how the sprite is read
    uint8_t bpp;                // 1/2: bit planes, 3/4: packed indices, packedTwoBit: packed 2-bit indices
    uint8_t packedTwoBit;       // the bpp value that means packed 2-bit indices (BPP_PACKED_2BIT)
    uint8_t dither;             // 0 none, 1 Burkes error diffusion, 2 ordered
    uint8_t numColors;          // palette entries dither 0/1 may choose from
    const QuantColor *palette;  // dither 2 always uses all of them
    uint8_t paletteSize;
};

namespace colorquant {

struct Error {
    int32_t r, g, b;
};

// Weighted squared distance 3*dr^2 + 5.47*dg^2 + 1.53*db^2, rounded down like the previous double version
// (equal distances decide which colour wins). Computed times 100 in integers: the ESP32 has no double FPU.
// A grey pixel never maps to a coloured palette entry (UINT32_MAX).
inline uint32_t distance(int r, int g, int b, bool pixelGrey, const QuantColor &p, bool paletteColoured) {
    if (pixelGrey && paletteColoured) return UINT32_MAX;
    const int dr = r - p.r, dg = g - p.g, db = b - p.b;
    return (300u * (uint32_t)(dr * dr) + 547u * (uint32_t)(dg * dg) + 153u * (uint32_t)(db * db)) / 100;
}

inline bool isGrey(int r, int g, int b) { return abs(r - g) < 20 && abs(b - g) < 20; }
inline bool isColoured(const QuantColor &c) { return abs(c.r - c.g) > 20 || abs(c.b - c.g) > 20; }

// Ordered dithering: the two closest palette colours, and how to mix them (0 = only c1 .. 4 = only c2)
struct OrderedMix {
    uint8_t c1, c2, level;
};

inline OrderedMix orderedMix(int r, int g, int b, const QuantParams &p, const bool *coloured) {
    const bool grey = isGrey(r, g, b);
    int closest = -1, second = -1;
    uint32_t closestDist = UINT32_MAX, secondDist = UINT32_MAX;
    bool closestSet = false, secondSet = false;
    for (int i = 0; i < p.paletteSize; i++) {
        const uint32_t d = distance(r, g, b, grey, p.palette[i], coloured[i]);
        if (!closestSet || d < closestDist) {
            second = closest;
            secondDist = closestDist;
            secondSet = closestSet;
            closest = i;
            closestDist = d;
            closestSet = true;
        } else if (!secondSet || d < secondDist) {
            second = i;
            secondDist = d;
            secondSet = true;
        }
    }
    if (closest != -1 && second != -1) {
        const QuantColor &a = p.palette[second], &c = p.palette[closest];
        if (((a.r << 16) | (a.g << 8) | a.b) > ((c.r << 16) | (c.g << 8) | c.b)) {
            std::swap(closest, second);
            std::swap(closestDist, secondDist);
        }
    }
    // In float against double thresholds, as before: pure colours land exactly on a threshold (red on a
    // black/white palette: 0.7), where float rounding decides. Once per colour, so the cost doesn't matter.
    const float d1 = closestDist, d2 = secondDist;
    const float weight = d1 / (d1 + d2);
    uint8_t level;
    if (weight <= 0.03) {
        level = 0;
    } else if (weight < 0.30) {
        level = 1;
    } else if (weight < 0.70) {
        level = 2;
    } else if (weight < 0.97) {
        level = 3;
    } else {
        level = 4;
    }
    return {(uint8_t)closest, (uint8_t)second, level};
}

}  // namespace colorquant

/// @brief Convert an image to palette indices, written as bit planes or packed indices.
/// @param readPixel (x, y) -> RGB565 of the source image
/// @param plane0 bit planes: black plane (index 1 or 3); packed formats: the packed buffer. May be null.
/// @param plane1 bit planes: red plane (index 2 or 3), or null. Both planes are filled in the same pass.
/// @return true if a pixel got palette index 2 or 3 (bit planes and packed 2-bit only)
template <typename ReadPixel>
bool quantizeImage(const QuantParams &p, ReadPixel readPixel, uint8_t *plane0, uint8_t *plane1) {
    using namespace colorquant;
    const long bufw = p.bufw, bufh = p.bufh;
    bool hasRed = false;

    bool coloured[256];
    for (int i = 0; i < p.paletteSize; i++) coloured[i] = isColoured(p.palette[i]);

    Error *errOld = nullptr, *errNew = nullptr;
    if (p.dither == 1) {
        errOld = new Error[bufw + 4]();
        errNew = new Error[bufw + 4]();
    }

    // ordered dithering only depends on the colour: remember the last one (large areas share a colour)
    int32_t cachedRgb = -1;
    OrderedMix cachedMix = {0, 0, 0};

    size_t bitOffset = 0;
    for (long y = 0; y < bufh; y++) {
        if (errNew) memset(errNew, 0, (bufw + 4) * sizeof(Error));
        for (long x = 0; x < bufw; x++) {
            uint16_t v = 0;
            switch (p.rotate) {
                case 0:
                    v = readPixel(x, y);
                    break;
                case 1:
                    v = readPixel(y, bufw - 1 - x);
                    break;
                case 2:
                    v = readPixel(bufw - 1 - x, bufh - 1 - y);
                    break;
                case 3:
                    v = readPixel(bufh - 1 - y, x);
                    break;
            }
            // RGB565 to RGB888, like Color(uint16_t)
            const int r = ((v >> 8) & 0xF8) | ((v >> 13) & 0x07);
            const int g = ((v >> 3) & 0xFC) | ((v >> 9) & 0x03);
            const int b = ((v << 3) & 0xF8) | ((v >> 2) & 0x07);

            int best = 0;
            if (p.dither == 2) {
                if ((int32_t)v != cachedRgb) {
                    cachedMix = orderedMix(r, g, b, p, coloured);
                    cachedRgb = v;
                }
                const bool checker = (x + y) % 2;
                const bool sparse = (y % 2) && ((y / 2 + x) % 2);
                switch (cachedMix.level) {
                    case 0: best = cachedMix.c1; break;
                    case 1: best = sparse ? cachedMix.c2 : cachedMix.c1; break;
                    case 2: best = checker ? cachedMix.c2 : cachedMix.c1; break;
                    case 3: best = sparse ? cachedMix.c1 : cachedMix.c2; break;
                    default: best = cachedMix.c2; break;
                }
            } else if (p.dither <= 1) {
                const bool grey = isGrey(r, g, b);
                const int er = errOld ? r + errOld[x].r : r;
                const int eg = errOld ? g + errOld[x].g : g;
                const int eb = errOld ? b + errOld[x].b : b;
                uint32_t bestDist = distance(er, eg, eb, grey, p.palette[0], coloured[0]);
                for (int i = 1; i < p.numColors; i++) {
                    if (bestDist == 0) break;
                    const uint32_t d = distance(er, eg, eb, grey, p.palette[i], coloured[i]);
                    if (d < bestDist) {
                        bestDist = d;
                        best = i;
                    }
                }
            }

            if (p.bpp == p.packedTwoBit) {
                // packed palette indices, four pixels per byte, MSB first
                if (plane0) plane0[bitOffset / 8] |= best << (6 - bitOffset % 8);
                bitOffset += 2;
                if (best >= 2) hasRed = true;
            } else if (p.bpp == 3 || p.bpp == 4) {
                if (plane0) {
                    const size_t byteIndex = bitOffset / 8;
                    const uint8_t bitIndex = bitOffset % 8;
                    if (bitIndex + p.bpp <= 8) {
                        plane0[byteIndex] |= best << (8 - bitIndex - p.bpp);
                    } else {
                        const uint8_t highPart = best >> (bitIndex + p.bpp - 8);
                        const uint8_t lowPart = best & ((1 << (bitIndex + p.bpp - 8)) - 1);
                        plane0[byteIndex] |= highPart;
                        plane0[byteIndex + 1] |= lowPart << (8 - (bitIndex + p.bpp - 8));
                    }
                }
                bitOffset += p.bpp;
            } else {
                const uint8_t mask = 1 << (7 - (x % 8));
                const uint32_t byteIndex = (y * bufw + x) / 8;
                if (best >= 2) hasRed = true;
                if (plane0 && (best == 1 || best == 3)) plane0[byteIndex] |= mask;
                if (plane1 && (best == 2 || best == 3)) plane1[byteIndex] |= mask;
            }

            if (errOld) {
                // Burkes
                const QuantColor &pc = p.palette[best];
                Error e = {r + errOld[x].r - pc.r, g + errOld[x].g - pc.g, b + errOld[x].b - pc.b};
                const int32_t m = std::max(abs(e.r), std::max(abs(e.g), abs(e.b)));
                if (m > 255) {
                    const float scale = 255.0f / m;
                    e.r *= scale;
                    e.g *= scale;
                    e.b *= scale;
                }

                errNew[x].r += e.r / 4;
                errNew[x].g += e.g / 4;
                errNew[x].b += e.b / 4;
                if (x > 0) {
                    errNew[x - 1].r += e.r / 8;
                    errNew[x - 1].g += e.g / 8;
                    errNew[x - 1].b += e.b / 8;
                }
                if (x > 1) {
                    errNew[x - 2].r += e.r / 16;
                    errNew[x - 2].g += e.g / 16;
                    errNew[x - 2].b += e.b / 16;
                }
                errNew[x + 1].r += e.r / 8;
                errNew[x + 1].g += e.g / 8;
                errNew[x + 1].b += e.b / 8;
                errOld[x + 1].r += e.r / 4;
                errOld[x + 1].g += e.g / 4;
                errOld[x + 1].b += e.b / 4;
                errNew[x + 2].r += e.r / 16;
                errNew[x + 2].g += e.g / 16;
                errNew[x + 2].b += e.b / 16;
                errOld[x + 2].r += e.r / 8;
                errOld[x + 2].g += e.g / 8;
                errOld[x + 2].b += e.b / 8;
            }
        }
        if (errOld) memcpy(errOld, errNew, bufw * sizeof(Error));
    }

    delete[] errNew;
    delete[] errOld;
    return hasRed;
}
