#include <Arduino.h>
#include <FS.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include <makeimage.h>
#include <web.h>

#include "leds.h"
#include "miniz-oepl.h"
#include "storage.h"
#include "util.h"

#ifdef HAS_TFT
#include "ips_display.h"
#endif

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);

bool spr_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
    spr.pushImage(x, y, w, h, bitmap);
    return 1;
}

void jpg2buffer(String filein, String fileout, imgParam &imageParams) {
    TJpgDec.setSwapBytes(true);
    TJpgDec.setJpgScale(1);
    TJpgDec.setCallback(spr_output);
    uint16_t w = 0, h = 0;
    if (filein.c_str()[0] != '/') {
        filein = "/" + filein;
    }
    TJpgDec.getFsJpgSize(&w, &h, filein, *contentFS);
    if (w == 0 && h == 0) {
        wsErr("invalid jpg");
        return;
    }
    Serial.println("jpeg conversion " + String(w) + "x" + String(h));

#ifdef BOARD_HAS_PSRAM
    spr.setColorDepth(16);
#else
    spr.setColorDepth(8);
#endif
    spr.createSprite(w, h);
    if (spr.getPointer() == nullptr) {
        wsErr("low on memory. Fallback to 1bpp");
        util::printLargestFreeBlock();
        spr.setColorDepth(1);
        spr.setBitmapColor(TFT_WHITE, TFT_BLACK);
        imageParams.bufferbpp = 1;
        spr.createSprite(w, h);
    }
    if (spr.getPointer() == nullptr) {
        wsErr("Failed to create sprite in jpg2buffer");
    } else {
        spr.fillSprite(TFT_WHITE);
        TJpgDec.drawFsJpg(0, 0, filein, *contentFS);

        spr2buffer(spr, fileout, imageParams);
        spr.deleteSprite();
    }
}

struct Color {
    uint8_t r, g, b;
    Color() : r(0), g(0), b(0) {}
    Color(uint16_t value_) : r((value_ >> 8) & 0xF8 | (value_ >> 13) & 0x07), g((value_ >> 3) & 0xFC | (value_ >> 9) & 0x03), b((value_ << 3) & 0xF8 | (value_ >> 2) & 0x07) {}
    Color(uint8_t r_, uint8_t g_, uint8_t b_) : r(r_), g(g_), b(b_) {}
};

struct Error {
    int32_t r;
    int32_t g;
    int32_t b;
};

uint32_t colorDistance(Color &c1, Color &c2, Error &e1) {
    e1.r = constrain(e1.r, -255, 255);
    e1.g = constrain(e1.g, -255, 255);
    e1.b = constrain(e1.b, -255, 255);
    int32_t r_diff = c1.r + e1.r - c2.r;
    int32_t g_diff = c1.g + e1.g - c2.g;
    int32_t b_diff = c1.b + e1.b - c2.b;
    return 3 * r_diff * r_diff + 6 * g_diff * g_diff + b_diff * b_diff;
}

void spr2color(TFT_eSprite &spr, imgParam &imageParams, uint8_t *buffer, size_t buffer_size, bool is_red) {
    uint8_t rotate = imageParams.rotate;
    long bufw = spr.width(), bufh = spr.height();
    if (imageParams.rotatebuffer) {
        rotate = (rotate + 3) % 4;
        bufw = spr.height();
        bufh = spr.width();
    }

    memset(buffer, 0, buffer_size);

    std::vector<Color> palette = {
        {255, 255, 255},  // White
        {0, 0, 0},        // Black
        {255, 0, 0}       // Red
    };
    if (imageParams.invert == 1) {
        std::swap(palette[0], palette[1]);
    }
    Color color;
    if (imageParams.dither == 2) {
        color = {128, 128, 128};
        palette.push_back(color);
        color = {211, 211, 211};
        palette.push_back(color);
        color = {255, 192, 203};
        palette.push_back(color);
    } else if (imageParams.grayLut) {
        color = {160, 160, 160};
        palette.push_back(color);
        Serial.println("rendering with gray");
    }
    int num_colors = palette.size();
    if (imageParams.bufferbpp == 1) num_colors = 2;
    Error *error_bufferold = new Error[bufw + 4];
    Error *error_buffernew = new Error[bufw + 4];

    memset(error_bufferold, 0, bufw * sizeof(Error));
    for (uint16_t y = 0; y < bufh; y++) {
        memset(error_buffernew, 0, bufw * sizeof(Error));
        for (uint16_t x = 0; x < bufw; x++) {
            switch (rotate) {
                case 0:
                    color = Color(spr.readPixel(x, y));
                    break;
                case 1:
                    color = Color(spr.readPixel(y, bufw - 1 - x));
                    break;
                case 2:
                    color = Color(spr.readPixel(bufw - 1 - x, bufh - 1 - y));
                    break;
                case 3:
                    color = Color(spr.readPixel(bufh - 1 - y, x));
                    break;
            }

            int best_color_index = 0;
            uint32_t best_color_distance = colorDistance(color, palette[0], error_bufferold[x]);
            for (int i = 1; i < num_colors; i++) {
                if (best_color_distance == 0) break;
                uint32_t distance = colorDistance(color, palette[i], error_bufferold[x]);
                if (distance < best_color_distance) {
                    best_color_distance = distance;
                    best_color_index = i;
                }
            }

            uint8_t bitIndex = 7 - (x % 8);
            uint32_t byteIndex = (y * bufw + x) / 8;

            // this looks a bit ugly, but it's performing better than shorter notations
            switch (best_color_index) {
                case 1:
                    if (!is_red)
                        buffer[byteIndex] |= (1 << bitIndex);
                    break;
                case 2:
                    imageParams.hasRed = true;
                    if (is_red)
                        buffer[byteIndex] |= (1 << bitIndex);
                    break;
                case 3:
                    if (imageParams.grayLut) {
                        buffer[byteIndex] |= (1 << bitIndex);
                        imageParams.hasRed = true;
                    } else {
                        if (!is_red && (x + y) % 2) buffer[byteIndex] |= (1 << bitIndex);
                    }
                    break;
                case 4:
                    if (!is_red && ((x + y / 2) % 2 == 0) && (y % 2 == 0)) buffer[byteIndex] |= (1 << bitIndex);
                    break;
                case 5:
                    if (is_red && (x + y) % 2) buffer[byteIndex] |= (1 << bitIndex);
                    imageParams.hasRed = true;
                    break;
            }

            if (imageParams.dither == 1) {
                Error error = {
                    color.r + error_bufferold[x].r - palette[best_color_index].r,
                    color.g + error_bufferold[x].g - palette[best_color_index].g,
                    color.b + error_bufferold[x].b - palette[best_color_index].b};

                // Burkes Dithering
                error_buffernew[x].r += error.r >> 2;
                error_buffernew[x].g += error.g >> 2;
                error_buffernew[x].b += error.b >> 2;
                if (x > 0) {
                    error_buffernew[x - 1].r += error.r >> 3;
                    error_buffernew[x - 1].g += error.g >> 3;
                    error_buffernew[x - 1].b += error.b >> 3;
                }
                if (x > 1) {
                    error_buffernew[x - 2].r += error.r >> 4;
                    error_buffernew[x - 2].g += error.g >> 4;
                    error_buffernew[x - 2].b += error.b >> 4;
                }
                error_buffernew[x + 1].r += error.r >> 3;
                error_buffernew[x + 1].g += error.g >> 3;
                error_buffernew[x + 1].b += error.b >> 3;

                error_bufferold[x + 1].r += error.r >> 2;
                error_bufferold[x + 1].g += error.g >> 2;
                error_bufferold[x + 1].b += error.b >> 2;

                error_buffernew[x + 2].r += error.r >> 4;
                error_buffernew[x + 2].g += error.g >> 4;
                error_buffernew[x + 2].b += error.b >> 4;

                error_bufferold[x + 2].r += error.r >> 3;
                error_bufferold[x + 2].g += error.g >> 3;
                error_bufferold[x + 2].b += error.b >> 3;
            }
        }
        memcpy(error_bufferold, error_buffernew, bufw * sizeof(Error));
    }

    delete[] error_buffernew;
    delete[] error_bufferold;

    return;
}

size_t prepareHeader(uint8_t headerbuf[], uint16_t bufw, uint16_t bufh, imgParam imageParams, size_t buffer_size) {
    size_t totalbytes;
    uint8_t headersize = 6;

    headerbuf[0] = headersize;
    memcpy(headerbuf + (imageParams.rotatebuffer == 1 ? 3 : 1), &bufw, sizeof(uint16_t));
    memcpy(headerbuf + (imageParams.rotatebuffer == 1 ? 1 : 3), &bufh, sizeof(uint16_t));

    if (imageParams.hasRed && imageParams.bpp > 1) {
        totalbytes = buffer_size * 2 + headersize;
        headerbuf[5] = 2;
    } else {
        totalbytes = buffer_size + headersize;
        headerbuf[5] = 1;
    }

    return totalbytes;
}

bool initializeCompressor(Miniz::tdefl_compressor *comp, int flags) {
    return Miniz::tdefl_init(comp, NULL, NULL, flags) == Miniz::TDEFL_STATUS_OKAY;
}

size_t compressAndWrite(Miniz::tdefl_compressor *comp, const void *inbuf, size_t inbytes, void *zlibbuf, size_t outsize, size_t totalbytes, File &f_out, Miniz::tdefl_flush flush) {
    size_t inbytes_compressed = inbytes;
    size_t outbytes_compressed = outsize;

    uint32_t t = millis();
    tdefl_compress(comp, inbuf, &inbytes_compressed, zlibbuf, &outbytes_compressed, flush);
    Serial.printf("zlib: compressed %d into %d bytes in %d ms\n", inbytes_compressed, outbytes_compressed, millis()-t);

    f_out.write((const uint8_t *)zlibbuf, outbytes_compressed);
    return outbytes_compressed;
}

void rewriteHeader(File &f_out) {
    // https://www.rfc-editor.org/rfc/rfc1950
    const uint8_t cmf = 0x48;
    uint8_t flg, flevel = 3;
    uint16_t header = cmf << 8 | (flevel << 6);
    header += 31 - (header % 31);
    flg = header & 0xFF;
    f_out.seek(4);
    f_out.write(cmf);
    f_out.write(flg);
}


void spr2buffer(TFT_eSprite &spr, String &fileout, imgParam &imageParams) {
    long t = millis();

#ifdef HAS_TFT
    extern uint8_t YellowSense;
    if (fileout == "direct") {
        if (tftOverride == false) {
            TFT_eSprite spr2 = TFT_eSprite(&tft2);
            tft2.setRotation(YellowSense == 1 ? 1 : 3);
            spr2.createSprite(spr.width(), spr.height());
            spr2.setColorDepth(spr.getColorDepth());

            void *spriteData = spr.getPointer();
            void *spriteData2 = spr2.getPointer();
            size_t dataSize = spr.width() * spr.height() * (spr.getColorDepth() / 8);
            memcpy(spriteData2, spriteData, dataSize);

            spr2.pushSprite(0, 0);
        }
        return;
    }
#endif

    xSemaphoreTake(fsMutex, portMAX_DELAY);
    fs::File f_out = contentFS->open(fileout, "w");

    switch (imageParams.bpp) {
        case 1:
        case 2: {
            long bufw = spr.width(), bufh = spr.height();
            size_t buffer_size = (bufw * bufh) / 8;
#ifdef BOARD_HAS_PSRAM
            uint8_t *buffer = (uint8_t *)ps_malloc(buffer_size);
#else
            uint8_t *buffer = (uint8_t *)malloc(buffer_size);
            imageParams.zlib = 0;
#endif
            if (!buffer) {
                Serial.println("Failed to allocate buffer");
                util::printLargestFreeBlock();
                f_out.close();
                xSemaphoreGive(fsMutex);
                return;
            }
            spr2color(spr, imageParams, buffer, buffer_size, false);

            if (imageParams.zlib) {
                Miniz::tdefl_compressor *comp;
                comp = (Miniz::tdefl_compressor *)malloc(sizeof(Miniz::tdefl_compressor));

                uint8_t headerbuf[6];
                size_t totalbytes = prepareHeader(headerbuf, bufw, bufh, imageParams, buffer_size);
                char *zlibbuf = (char *)malloc(totalbytes * 1.3);

                f_out.write(reinterpret_cast<uint8_t *>(&totalbytes), sizeof(uint32_t));

                if (comp == NULL || zlibbuf == NULL || totalbytes == 0 || !initializeCompressor(comp, Miniz::TDEFL_WRITE_ZLIB_HEADER | 1500)) {
                    Serial.println("Failed to initialize compressor or allocate memory for zlib");
                    if (zlibbuf != NULL) free(zlibbuf);
                    if (comp != NULL) free(comp);
                    break;
                }

                size_t bufferstart = compressAndWrite(comp, headerbuf, sizeof(headerbuf), zlibbuf, buffer_size, totalbytes, f_out, Miniz::TDEFL_NO_FLUSH);
                compressAndWrite(comp, buffer, buffer_size, zlibbuf + bufferstart, buffer_size, buffer_size, f_out, (headerbuf[5] == 2 ? Miniz::TDEFL_SYNC_FLUSH : Miniz::TDEFL_FINISH));

                if (headerbuf[5] == 2) {
                    spr2color(spr, imageParams, buffer, buffer_size, true);
                    compressAndWrite(comp, buffer, buffer_size, zlibbuf, buffer_size, buffer_size, f_out, Miniz::TDEFL_FINISH);
                }

                rewriteHeader(f_out);

                free(zlibbuf);
                free(comp);
            } else {
                f_out.write(buffer, buffer_size);
                if (imageParams.hasRed && imageParams.bpp > 1) {
                    spr2color(spr, imageParams, buffer, buffer_size, true);
                    f_out.write(buffer, buffer_size);
                }
            }

            free(buffer);
        } break;

        case 16: {
            size_t spriteDataSize = (spr.getColorDepth() == 1) ? (spr.width() * spr.height() / 8) : ((spr.getColorDepth() == 8) ? (spr.width() * spr.height()) : ((spr.getColorDepth() == 16) ? (spr.width() * spr.height() * 2) : 0));
            f_out.write((const uint8_t *)spr.getPointer(), spriteDataSize);
        } break;
    }

    f_out.close();
    xSemaphoreGive(fsMutex);
    Serial.println("finished writing buffer " + String(millis() - t) + "ms");
}
