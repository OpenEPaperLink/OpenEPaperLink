#include <Arduino.h>
#include <FS.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include <makeimage.h>
#include <web.h>

#include "leds.h"
#include "colorquant.h"
#include "miniz-oepl.h"
#include "storage.h"
#include "tag_db.h"
#include "util.h"

#include <functional>

#ifdef HAS_TFT
#include "ips_display.h"
#endif

#include "commstructs.h"
#ifndef SAVE_SPACE
#include "g5/Group5.h"
#include "g5/g5enc.inl"
#endif

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);

bool spr_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
    spr.pushImage(x, y, w, h, bitmap);
    return 1;
}

/// @brief Decode a jpg of w x h pixels with draw() into a sprite and convert that to fileout
static void jpg2buffer(uint16_t w, uint16_t h, const std::function<void()> &draw, String &fileout, imgParam &imageParams) {
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
        draw();

        spr2buffer(spr, fileout, imageParams);
        spr.deleteSprite();
    }
}

static void setupJpgDecoder() {
    TJpgDec.setSwapBytes(true);
    TJpgDec.setJpgScale(1);
    TJpgDec.setCallback(spr_output);
}

void jpg2buffer(String filein, String fileout, imgParam &imageParams) {
    setupJpgDecoder();
    if (filein.c_str()[0] != '/') {
        filein = "/" + filein;
    }
    uint16_t w = 0, h = 0;
    TJpgDec.getFsJpgSize(&w, &h, filein, *contentFS);
    jpg2buffer(w, h, [&filein]() { TJpgDec.drawFsJpg(0, 0, filein, *contentFS); }, fileout, imageParams);
}

void jpg2buffer(const uint8_t *jpg, size_t len, String fileout, imgParam &imageParams) {
    setupJpgDecoder();
    uint16_t w = 0, h = 0;
    TJpgDec.getJpgSize(&w, &h, jpg, len);
    jpg2buffer(w, h, [jpg, len]() { TJpgDec.drawJpg(0, 0, jpg, len); }, fileout, imageParams);
}

/// @brief Convert the sprite to the tag's palette: bit planes (bpp 1/2) or packed indices (bpp 3/4/packed 2-bit)
/// @param plane0 black plane, or the packed buffer; null to skip
/// @param plane1 red plane (bpp 2), filled in the same pass; null to skip
static void spr2color(TFT_eSprite &spr, imgParam &imageParams, uint8_t *plane0, uint8_t *plane1, size_t buffer_size) {
    QuantParams params;
    params.rotate = imageParams.rotate;
    params.bufw = spr.width();
    params.bufh = spr.height();
    if (imageParams.rotatebuffer % 2) {
        // turn the image 90 or 270
        params.rotate = (params.rotate + 3) % 4;
        params.rotate = (params.rotate + (imageParams.rotatebuffer - 1)) % 4;
        params.bufw = spr.height();
        params.bufh = spr.width();
    } else {
        // rotate 180
        params.rotate = (params.rotate + imageParams.rotatebuffer) % 4;
    }

    if (plane0) memset(plane0, 0, buffer_size);
    if (plane1) memset(plane1, 0, buffer_size);

    const std::vector<Color> &colortable = imageParams.hwdata.colortable;
    std::vector<QuantColor> palette(colortable.size());
    for (size_t i = 0; i < colortable.size(); i++) palette[i] = {colortable[i].r, colortable[i].g, colortable[i].b};
    if (imageParams.invert == 1) std::swap(palette[0], palette[1]);
    params.palette = palette.data();
    params.paletteSize = palette.size();
    params.numColors = imageParams.bufferbpp == 1 ? 2 : palette.size();
    params.bpp = imageParams.bpp;
    params.packedTwoBit = BPP_PACKED_2BIT;
    params.dither = imageParams.dither;

    if (quantizeImage(params, [&spr](int32_t x, int32_t y) { return spr.readPixel(x, y); }, plane0, plane1)) {
        imageParams.hasRed = true;
    }
}

size_t prepareHeader(uint8_t headerbuf[], uint16_t bufw, uint16_t bufh, imgParam imageParams, size_t buffer_size) {
    size_t totalbytes;
    uint8_t headersize = 6;

    headerbuf[0] = headersize;
    memcpy(headerbuf + (imageParams.rotatebuffer % 2 == 1 ? 3 : 1), &bufw, sizeof(uint16_t));
    memcpy(headerbuf + (imageParams.rotatebuffer % 2 == 1 ? 1 : 3), &bufh, sizeof(uint16_t));

    if (imageParams.bpp == 3 || imageParams.bpp == 4) {
        totalbytes = buffer_size * imageParams.bpp + headersize;
        headerbuf[5] = imageParams.bpp;
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

size_t compressAndWrite(Miniz::tdefl_compressor *comp, const void *inbuf, size_t inbytes, void *zlibbuf, size_t outsize, size_t totalbytes, LockedFile &f_out, Miniz::tdefl_flush flush) {
    size_t inbytes_compressed = inbytes;
    size_t outbytes_compressed = outsize;

    uint32_t t = millis();
    Miniz::tdefl_compressOEPL(comp, inbuf, &inbytes_compressed, zlibbuf, &outbytes_compressed, flush);
    Serial.printf("zlib: compressed %d into %d bytes in %d ms\r\n", inbytes_compressed, outbytes_compressed, millis() - t);

    f_out.write((const uint8_t *)zlibbuf, outbytes_compressed);
    return outbytes_compressed;
}

void rewriteHeader(LockedFile &f_out) {
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

#ifndef SAVE_SPACE
uint8_t *g5Compress(uint16_t width, uint16_t height, uint8_t *buffer, uint16_t buffersize, uint16_t &outBufferSize) {
    G5ENCIMAGE g5enc;
    int rc;
    uint8_t *outbuffer = (uint8_t *)ps_malloc(buffersize+16384);
    if (outbuffer == NULL) {
        Serial.println("Failed to allocate the output buffer for the G5 encoder");
        return nullptr;
    }

    rc = g5_encode_init(&g5enc, width, height, outbuffer, buffersize);
    for (int y = 0; y < height && rc == G5_SUCCESS; y++) {
        rc = g5_encode_encodeLine(&g5enc, buffer);
        buffer += (width / 8);
        if (rc != G5_SUCCESS) break;
    }
    if (rc == G5_ENCODE_COMPLETE) {
        outBufferSize = g5_encode_getOutSize(&g5enc);
    } else {
        printf("Encode failed! rc=%d\n", rc);
        free(outbuffer);
        return nullptr;
    }
    return outbuffer;
}
#endif

// The "ts_option" is a bitmapped variable with a default value of 1 
// which is black on white, long format @ bottom right.
// 
// b2, b1, b0: 
//    0 - no timestamp
//    1 - bottom right
//    2 - top right
//    3 - bottom left
//    4 - top left
//    5 -> 7 reserved
// b3:
//    0 - long format (year-month-day hr:min)
//    1 - short format (month-day hr:min)
// b4:
//    0 - black on white
//    1 - white on black
// b5 -> b7: reserved
//
void doTimestamp(TFT_eSprite *spr, uint8_t ts_option) {
    time_t now = time(nullptr);
    struct tm *timeinfo = localtime(&now);
    char buffer[20];
    strftime(buffer, sizeof(buffer),
             (ts_option & 0x8) ? "%m-%d %H:%M" : "%Y-%m-%d %H:%M",timeinfo);
    int ts_chars = strlen(buffer);

    uint16_t char_color;
    uint16_t bg_color;

    if(ts_option & 0x10) {
       char_color = TFT_WHITE;
       bg_color= TFT_BLACK;
    }
    else {
       char_color = TFT_BLACK;
       bg_color = TFT_WHITE;
    }
    
    ts_option = (ts_option & 0x3) - 1;

    int32_t ts_x = (ts_option & 2) ? 1 : spr->width() - ts_chars * 6 - 2;
    int32_t ts_y = (ts_option & 1) ? 1 : spr->height() - 10;

    spr->drawRect(ts_x - 1, ts_y - 1, ts_chars * 6 + 1, 9, bg_color);
    spr->setTextColor(char_color, bg_color);
    spr->setCursor(ts_x,ts_y);
    spr->print(buffer);
}

void spr2buffer(TFT_eSprite &spr, String &fileout, imgParam &imageParams) {
    long t = millis();

    if (imageParams.ts_option) doTimestamp(&spr,imageParams.ts_option);
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

#if defined HAS_LILYGO_TPANEL || defined HAS_4inch_TPANEL
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

    // fsMutex is taken per write, not during the (slow) conversion and compression
    LockedFile f_out(fileout, "w");

    switch (imageParams.bpp) {
        case 1:
        case 2: {
            long bufw = spr.width(), bufh = spr.height();
            size_t buffer_size = ((bufw * bufh) + 7) / 8;  // round up: not all dimensions are multiples of 8
            // black and red plane next to each other, filled in one pass; with too little memory, the red
            // plane is made afterwards in the same buffer (a second conversion pass)
#ifdef BOARD_HAS_PSRAM
            uint8_t *buffer = imageParams.bpp == 2 ? (uint8_t *)ps_malloc(2 * buffer_size) : nullptr;
            uint8_t *redPlane = buffer ? buffer + buffer_size : nullptr;
            if (!buffer) buffer = (uint8_t *)ps_malloc(buffer_size);
#else
            uint8_t *buffer = imageParams.bpp == 2 ? (uint8_t *)malloc(2 * buffer_size) : nullptr;
            uint8_t *redPlane = buffer ? buffer + buffer_size : nullptr;
            if (!buffer) buffer = (uint8_t *)malloc(buffer_size);
            imageParams.zlib = 0;
            imageParams.g5 = 0;
#endif
            if (!buffer) {
                Serial.println("Failed to allocate buffer");
                util::printLargestFreeBlock();
                f_out.close();
                return;
            }
            spr2color(spr, imageParams, buffer, redPlane, buffer_size);
            // the red plane, converting it now into buffer if it wasn't made in the first pass
            auto getRedPlane = [&]() {
                if (redPlane) return redPlane;
                spr2color(spr, imageParams, nullptr, buffer, buffer_size);
                return buffer;
            };

            if (imageParams.zlib) {
                Miniz::tdefl_compressor *comp;
                comp = (Miniz::tdefl_compressor *)malloc(sizeof(Miniz::tdefl_compressor));

                uint8_t headerbuf[6];
                size_t totalbytes = prepareHeader(headerbuf, bufw, bufh, imageParams, buffer_size);
                char *zlibbuf = (char *)malloc(totalbytes * 1.3);

                f_out.write(reinterpret_cast<uint8_t *>(&totalbytes), sizeof(uint32_t));

                // 768 = compression level 9, 1500 = unofficial level 10
                if (comp == NULL || zlibbuf == NULL || totalbytes == 0 || !initializeCompressor(comp, Miniz::TDEFL_WRITE_ZLIB_HEADER | 1500)) {
                    Serial.println("Failed to initialize compressor or allocate memory for zlib, falling back to raw");
                    if (zlibbuf != NULL) free(zlibbuf);
                    if (comp != NULL) free(comp);
                    imageParams.zlib = 0;
                    imageParams.g5 = 0;
                    f_out.seek(0);
                    f_out.write(buffer, buffer_size);
                    if (imageParams.hasRed && imageParams.bpp > 1) {
                        f_out.write(getRedPlane(), buffer_size);
                    }
                    free(buffer);
                    break;
                }

                size_t bufferstart = compressAndWrite(comp, headerbuf, sizeof(headerbuf), zlibbuf, buffer_size, totalbytes, f_out, Miniz::TDEFL_NO_FLUSH);
                compressAndWrite(comp, buffer, buffer_size, zlibbuf + bufferstart, buffer_size, buffer_size, f_out, (headerbuf[5] == 2 ? Miniz::TDEFL_SYNC_FLUSH : Miniz::TDEFL_FINISH));

                if (headerbuf[5] == 2) {
                    compressAndWrite(comp, getRedPlane(), buffer_size, zlibbuf, buffer_size, buffer_size, f_out, Miniz::TDEFL_FINISH);
                }

                free(zlibbuf);
                free(comp);

                rewriteHeader(f_out);
#ifndef SAVE_SPACE
            } else if (imageParams.g5) {
                // handling for G5-compressed image data

                uint8_t headerbuf[6];
                prepareHeader(headerbuf, bufw, bufh, imageParams, buffer_size);
                f_out.write(headerbuf, sizeof(headerbuf));

                uint16_t height = imageParams.height;  // spr.height();
                uint16_t width = imageParams.width;
                spr.width();
                if (imageParams.hasRed && imageParams.bpp > 1) {
                    if (!redPlane) {
                        uint8_t *newbuffer = (uint8_t *)ps_realloc(buffer, 2 * buffer_size);
                        if (newbuffer == NULL) {
                            Serial.println("Failed to allocate larger buffer for 2bpp G5");
                            free(buffer);
                            f_out.close();
                            return;
                        }
                        buffer = newbuffer;
                        spr2color(spr, imageParams, nullptr, buffer + buffer_size, buffer_size);
                    }
                    buffer_size *= 2;
                    // double the height, to do two layers sequentially
                    if (imageParams.rotatebuffer % 2) {
                        width *= 2;
                    } else {
                        height *= 2;
                    }
                }
                uint16_t outbufferSize = 0;
                uint8_t *outBuffer;
                bool compressionSuccessful = true;
                if (imageParams.rotatebuffer % 2) {
                    outBuffer = g5Compress(height, width, buffer, buffer_size, outbufferSize);
                } else {
                    outBuffer = g5Compress(width, height, buffer, buffer_size, outbufferSize);
                }
                if (outBuffer == NULL) {
                    Serial.println("Failed to compress G5");
                    compressionSuccessful = false;
                } else {
                    printf("Compressed %d to %d bytes\n", buffer_size, outbufferSize);
                    if (outbufferSize > buffer_size) {
                        printf("That wasn't very useful, falling back to raw\n");
                        compressionSuccessful = false;
                    } else {
                        f_out.write(outBuffer, outbufferSize);
                    }
                    free(outBuffer);
                }
                if (!compressionSuccessful) {
                    // if we failed to compress the image, or the resulting image was larger than a raw file, fallback
                    imageParams.g5 = false;
                    if (imageParams.hasRed && imageParams.bpp > 1) {
                        imageParams.dataType = DATATYPE_IMG_RAW_2BPP;
                    } else {
                        imageParams.dataType = DATATYPE_IMG_RAW_1BPP;
                    }
                    f_out.seek(0);
                    f_out.write(buffer, buffer_size);
                }
#endif
            } else {
                f_out.write(buffer, buffer_size);
                if (imageParams.hasRed && imageParams.bpp > 1) {
                    f_out.write(getRedPlane(), buffer_size);
                }
            }

            free(buffer);
        } break;

        case 3:
        case 4:
        case BPP_PACKED_2BIT: {
            long bufw = spr.width(), bufh = spr.height();
            const uint8_t packedBpp = imageParams.bpp == BPP_PACKED_2BIT ? 2 : imageParams.bpp;
            size_t buffer_size = ((bufw * bufh * packedBpp) + 7) / 8;
            uint8_t *buffer = (uint8_t *)ps_malloc(buffer_size);
            if (!buffer) {
                Serial.println("Failed to allocate buffer");
                util::printLargestFreeBlock();
                f_out.close();
                return;
            }
            spr2color(spr, imageParams, buffer, nullptr, buffer_size);
            f_out.write(buffer, buffer_size);

            free(buffer);
        } break;

        case 16: {
            size_t spriteDataSize = (spr.getColorDepth() == 1) ? (spr.width() * spr.height() / 8) : ((spr.getColorDepth() == 8) ? (spr.width() * spr.height()) : ((spr.getColorDepth() == 16) ? (spr.width() * spr.height() * 2) : 0));
            f_out.write((const uint8_t *)spr.getPointer(), spriteDataSize);
        } break;
    }

    f_out.close();
    Serial.println("finished writing buffer " + String(millis() - t) + "ms");
}
