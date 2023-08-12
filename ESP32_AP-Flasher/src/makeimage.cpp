#include <Arduino.h>
#include <FS.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include <makeimage.h>
#include <web.h>

#include "leds.h"
#include "storage.h"
#include "util.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);

bool spr_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
    spr.pushImage(x, y, w, h, bitmap);
    return 1;
}

void jpg2buffer(String filein, String fileout, imgParam &imageParams) {
    Storage.begin();
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
    int32_t r_diff = gamma8[c1.r] + e1.r - gamma8[c2.r];
    int32_t g_diff = gamma8[c1.g] + e1.g - gamma8[c2.g];
    int32_t b_diff = gamma8[c1.b] + e1.b - gamma8[c2.b];
    return 22 * r_diff * r_diff + 50 * g_diff * g_diff + 20 * b_diff * b_diff;
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
    if (imageParams.grayLut) {
        Color newColor = {160, 160, 160};
        palette.push_back(newColor);
        Serial.println("rendering with gray");
    }
    int num_colors = palette.size();
    if (imageParams.bufferbpp == 1) num_colors = 2;
    Color color;
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
            uint16_t byteIndex = (y * bufw + x) / 8;

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
                    buffer[byteIndex] |= (1 << bitIndex);
                    imageParams.hasRed = true;
                    break;
            }

            if (imageParams.dither) {
                Error error = {
                    color.r + error_bufferold[x].r - palette[best_color_index].r,
                    color.g + error_bufferold[x].g - palette[best_color_index].g,
                    color.b + error_bufferold[x].b - palette[best_color_index].b};

                // Burkes Dithering
                error_buffernew[x].r += error.r / 4.0f;
                error_buffernew[x].g += error.g / 4.0f;
                error_buffernew[x].b += error.b / 4.0f;
                if (x > 0) {
                    error_buffernew[x - 1].r += error.r / 8.0f;
                    error_buffernew[x - 1].g += error.g / 8.0f;
                    error_buffernew[x - 1].b += error.b / 8.0f;
                }
                if (x > 1) {
                    error_buffernew[x - 2].r += error.r / 16.0f;
                    error_buffernew[x - 2].g += error.g / 16.0f;
                    error_buffernew[x - 2].b += error.b / 16.0f;
                }
                error_buffernew[x + 1].r += error.r / 8.0f;
                error_buffernew[x + 1].g += error.g / 8.0f;
                error_buffernew[x + 1].b += error.b / 8.0f;

                error_bufferold[x + 1].r += error.r / 4.0f;
                error_bufferold[x + 1].g += error.g / 4.0f;
                error_bufferold[x + 1].b += error.b / 4.0f;

                error_buffernew[x + 2].r += error.r / 16.0f;
                error_buffernew[x + 2].g += error.g / 16.0f;
                error_buffernew[x + 2].b += error.b / 16.0f;

                error_bufferold[x + 2].r += error.r / 8.0f;
                error_bufferold[x + 2].g += error.g / 8.0f;
                error_bufferold[x + 2].b += error.b / 8.0f;
            }
        }
        memcpy(error_bufferold, error_buffernew, bufw * sizeof(Error));
    }

    delete[] error_buffernew;
    delete[] error_bufferold;

    return;
}

void spr2buffer(TFT_eSprite &spr, String &fileout, imgParam &imageParams) {
    long t = millis();
    Storage.begin();

#ifdef YELLOW_IPS_AP
    if (fileout == "direct") {
        tft.setRotation(3);
        spr.pushSprite(0, 0);
        return;
    }
#endif

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
#endif
            if (!buffer) {
                Serial.println("Failed to allocate buffer");
                util::printLargestFreeBlock();
                return;
            }
            spr2color(spr, imageParams, buffer, buffer_size, false);
            f_out.write(buffer, buffer_size);

            if (imageParams.hasRed && imageParams.bpp > 1) {
                spr2color(spr, imageParams, buffer, buffer_size, true);
                f_out.write(buffer, buffer_size);
            }
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
