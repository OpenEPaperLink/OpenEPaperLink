#include <Arduino.h>
#include <FS.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include <makeimage.h>
#include <web.h>

#include "leds.h"
#include "miniz-oepl.h"
#include "storage.h"
#include "tag_db.h"
#include "util.h"

#ifdef HAS_TFT
#include "ips_display.h"
#endif

#include "g5/Group5.h"
#include "g5/g5enc.inl"

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

    if (imageParams.rotatebuffer % 2) {
        // turn the image 90 or 270
        rotate = (rotate + 3) % 4;
        rotate = (rotate + (imageParams.rotatebuffer - 1)) % 4;
        bufw = spr.height();
        bufh = spr.width();
    } else {
        // rotate 180
        rotate = (rotate + (imageParams.rotatebuffer)) % 4;
    }

    memset(buffer, 0, buffer_size);

    std::vector<Color> palette = imageParams.hwdata.colortable;
    if (imageParams.invert == 1) {
        std::swap(palette[0], palette[1]);
    }
    Color color;
    int num_colors = palette.size();
    if (imageParams.bufferbpp == 1) num_colors = 2;
    Error *error_bufferold = new Error[bufw + 4];
    Error *error_buffernew = new Error[bufw + 4];

    const uint8_t ditherMatrix[4][4] = {
        {0, 9, 2, 10},
        {12, 5, 14, 6},
        {3, 11, 1, 8},
        {15, 7, 13, 4}};
    size_t bitOffset = 0;

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

            if (imageParams.dither == 2) {
                // Ordered dithering
                uint8_t ditherValue = ditherMatrix[y % 4][x % 4] << (imageParams.bpp == 3 ? 2 : 4);
                error_bufferold[x].r = ditherValue - (imageParams.bpp == 3 ? 30 : 120);  // * 256 / 16 - 128 + 8
                error_bufferold[x].g = ditherValue - (imageParams.bpp == 3 ? 30 : 120);
                error_bufferold[x].b = ditherValue - (imageParams.bpp == 3 ? 30 : 120);
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

            if (imageParams.bpp == 3) {
                size_t byteIndex = bitOffset / 8;
                uint8_t bitIndex = bitOffset % 8;

                if (bitIndex + imageParams.bpp <= 8) {
                    buffer[byteIndex] |= best_color_index << (8 - bitIndex - imageParams.bpp);
                } else {
                    uint8_t highPart = best_color_index >> (bitIndex + imageParams.bpp - 8);
                    uint8_t lowPart = best_color_index & ((1 << (bitIndex + imageParams.bpp - 8)) - 1);
                    buffer[byteIndex] |= highPart;
                    buffer[byteIndex + 1] |= lowPart << (8 - (bitIndex + imageParams.bpp - 8));
                }
                bitOffset += imageParams.bpp;
            } else {
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
                        imageParams.hasRed = true;
                        buffer[byteIndex] |= (1 << bitIndex);
                        break;
                }
            }

            if (imageParams.dither == 1) {
                // Burkes Dithering

                Error error = {
                    color.r + error_bufferold[x].r - palette[best_color_index].r,
                    color.g + error_bufferold[x].g - palette[best_color_index].g,
                    color.b + error_bufferold[x].b - palette[best_color_index].b};

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
    memcpy(headerbuf + (imageParams.rotatebuffer % 2 == 1 ? 3 : 1), &bufw, sizeof(uint16_t));
    memcpy(headerbuf + (imageParams.rotatebuffer % 2 == 1 ? 1 : 3), &bufh, sizeof(uint16_t));

    if (imageParams.bpp == 3) {
        totalbytes = buffer_size * 3 + headersize;
        headerbuf[5] = 3;
    } else if (imageParams.hasRed && imageParams.bpp > 1) {
        totalbytes = buffer_size * 2 + headersize;
        headerbuf[5] = 2;
    } else {
        totalbytes = buffer_size + headersize;
        headerbuf[5] = 1;
    }

    return totalbytes;
}

bool initializeCompressor(Miniz::tdefl_compressor *comp, int flags) {
    return Miniz::tdefl_initOEPL(comp, NULL, NULL, flags) == Miniz::TDEFL_STATUS_OKAY;
}

size_t compressAndWrite(Miniz::tdefl_compressor *comp, const void *inbuf, size_t inbytes, void *zlibbuf, size_t outsize, size_t totalbytes, File &f_out, Miniz::tdefl_flush flush) {
    size_t inbytes_compressed = inbytes;
    size_t outbytes_compressed = outsize;

    uint32_t t = millis();
    Miniz::tdefl_compressOEPL(comp, inbuf, &inbytes_compressed, zlibbuf, &outbytes_compressed, flush);
    Serial.printf("zlib: compressed %d into %d bytes in %d ms\r\n", inbytes_compressed, outbytes_compressed, millis() - t);

    f_out.write((const uint8_t *)zlibbuf, outbytes_compressed);
    return outbytes_compressed;
}

void rewriteHeader(File &f_out) {
    // https://www.rfc-editor.org/rfc/rfc1950
    const uint8_t cmf = 0x48;  // 4096
    // const uint8_t cmf = 0x58; // 8192
    uint8_t flg, flevel = 3;
    uint16_t header = cmf << 8 | (flevel << 6);
    header += 31 - (header % 31);
    flg = header & 0xFF;
    f_out.seek(4);
    f_out.write(cmf);
    f_out.write(flg);
}

uint8_t *g5Compress(uint16_t width, uint16_t height, uint8_t *buffer, uint16_t buffersize, uint16_t &outBufferSize) {
    G5ENCIMAGE g5enc;
    int rc;
    uint8_t *outbuffer = (uint8_t *)ps_malloc(buffersize + 16384);
    if (outbuffer == NULL) {
        Serial.println("Failed to allocate the output buffer for the G5 encoder");
        return nullptr;
    }

    printf("G5 Compress: %d x %d\n", width, height);

    rc = g5_encode_init(&g5enc, width, height, outbuffer, buffersize + 16384);
    for (int y = 0; y < height && rc == G5_SUCCESS; y++) {
        rc = g5_encode_encodeLine(&g5enc, buffer);
        buffer += (width / 8);
    }
    if (rc == G5_ENCODE_COMPLETE) {
        printf("Encode succeeded!\n");
        outBufferSize = g5_encode_getOutSize(&g5enc);
    } else {
        printf("Encode failed! rc=%d\n", rc);
        free(outbuffer);
        return nullptr;
    }
    return outbuffer;
}

void spr2buffer(TFT_eSprite &spr, String &fileout, imgParam &imageParams) {
    long t = millis();

#ifdef HAS_TFT
    extern uint8_t YellowSense;
    if (fileout == "direct") {
        if (tftOverride == false) {
            TFT_eSprite spr2 = TFT_eSprite(&tft2);
#ifdef ST7735_NANO_TLSR
            tft2.setRotation(1);
#else
            tft2.setRotation(YellowSense == 1 ? 1 : 3);
#endif
            spr2.createSprite(spr.width(), spr.height());
            spr2.setColorDepth(spr.getColorDepth());

            void *spriteData = spr.getPointer();
            void *spriteData2 = spr2.getPointer();
            size_t dataSize = spr.width() * spr.height() * (spr.getColorDepth() / 8);
            memcpy(spriteData2, spriteData, dataSize);

#ifdef HAS_LILYGO_TPANEL
            if (spr.getColorDepth() == 16) {
                long dy = spr.height();
                long dx = spr.width();

                uint16_t *data = static_cast<uint16_t *>(const_cast<void *>(spriteData2));

                gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)spriteData2, dx, dy);
                spr2.deleteSprite();
            }
#else
            spr2.pushSprite(0, 0);
#endif
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
            imageParams.g5 = 0;
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

                // 768 = compression level 9, 1500 = unofficial level 10
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

                free(zlibbuf);
                free(comp);

                rewriteHeader(f_out);
            } else if (imageParams.g5) {
                // handling for G5-compressed image data
                uint16_t height = imageParams.height;  // spr.height();
                uint16_t width = imageParams.width;
                spr.width();
                // uint8_t *buffer = (uint8_t *)ps_malloc(buffer_size);
                if (imageParams.hasRed && imageParams.bpp > 1) {
                    uint8_t *newbuffer = (uint8_t *)ps_realloc(buffer, 2 * buffer_size);
                    if (newbuffer == NULL) {
                        Serial.println("Failed to allocate larger buffer for 2bpp G5");
                        free(buffer);
                        f_out.close();
                        xSemaphoreGive(fsMutex);
                        return;
                    }
                    buffer = newbuffer;
                    spr2color(spr, imageParams, buffer + buffer_size, buffer_size, true);
                    buffer_size *= 2;
                    if (imageParams.rotatebuffer % 2) {
                        width *= 2;
                    } else {
                        height *= 2;
                    }
                }
                uint16_t outbufferSize = 0;
                uint8_t *outBuffer;
                if (imageParams.rotatebuffer % 2) {
                    outBuffer = g5Compress(height, width, buffer, buffer_size, outbufferSize);
                } else {
                    outBuffer = g5Compress(width, height, buffer, buffer_size, outbufferSize);
                }
                if (outBuffer == NULL) {
                    Serial.println("Failed to compress G5");
                } else {
                    printf("Compressed %d to %d bytes\n", buffer_size, outbufferSize);
                    f_out.write(outBuffer, outbufferSize);
                    free(outBuffer);
                }
            } else {
                f_out.write(buffer, buffer_size);
                if (imageParams.hasRed && imageParams.bpp > 1) {
                    spr2color(spr, imageParams, buffer, buffer_size, true);
                    f_out.write(buffer, buffer_size);
                }
            }

            free(buffer);
        } break;

        case 3: {
            long bufw = spr.width(), bufh = spr.height();
            size_t buffer_size = (bufw * bufh) / 8 * 3;
            uint8_t *buffer = (uint8_t *)ps_malloc(buffer_size);
            if (!buffer) {
                Serial.println("Failed to allocate buffer");
                util::printLargestFreeBlock();
                f_out.close();
                xSemaphoreGive(fsMutex);
                return;
            }
            spr2color(spr, imageParams, buffer, buffer_size, false);
            f_out.write(buffer, buffer_size);

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
