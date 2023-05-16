#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include <makeimage.h>
#include <web.h>

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);

bool spr_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
    spr.pushImage(x, y, w, h, bitmap);
    return 1;
}

void jpg2buffer(String filein, String fileout, imgParam &imageParams) {
    LittleFS.begin();
    TJpgDec.setSwapBytes(true);
    TJpgDec.setJpgScale(1);
    TJpgDec.setCallback(spr_output);
    uint16_t w = 0, h = 0;
    if (filein.c_str()[0] != '/') {
        filein = "/" + filein;
    }
    TJpgDec.getFsJpgSize(&w, &h, filein, LittleFS);
    if (w==0 && h==0) {
        wsErr("invalid jpg");
        return;
    }
    Serial.println("jpeg conversion " + String(w) + "x" + String(h));

    spr.setColorDepth(8);
    spr.createSprite(w, h);
    if (spr.getPointer() == nullptr) {
        //no heap space for 8bpp, fallback to 1bpp
        wsErr("fallback to 1bpp");
        spr.setColorDepth(1);
        spr.createSprite(w, h);
    }
    if (spr.getPointer() == nullptr) {
        wsErr("Failed to create sprite in jpg2buffer");
    } else {
        spr.fillSprite(TFT_WHITE);
        TJpgDec.drawFsJpg(0, 0, filein, LittleFS);

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
    float r;
    float g;
    float b;
};

// Gamma brightness lookup table <https://victornpb.github.io/gamma-table-generator>
// gamma = 1.50 steps = 256 range = 0-255
const uint8_t gamma_lut[256] PROGMEM = {
     0,   0,   0,   0,   1,   1,   1,   1,   1,   2,   2,   2,   3,   3,   3,   4,
     4,   4,   5,   5,   6,   6,   6,   7,   7,   8,   8,   9,   9,  10,  10,  11,
    11,  12,  12,  13,  14,  14,  15,  15,  16,  16,  17,  18,  18,  19,  20,  20,
    21,  21,  22,  23,  23,  24,  25,  26,  26,  27,  28,  28,  29,  30,  31,  31,
    32,  33,  34,  34,  35,  36,  37,  37,  38,  39,  40,  41,  41,  42,  43,  44,
    45,  46,  46,  47,  48,  49,  50,  51,  52,  53,  53,  54,  55,  56,  57,  58,
    59,  60,  61,  62,  63,  64,  65,  65,  66,  67,  68,  69,  70,  71,  72,  73,
    74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  88,  89,  90,
    91,  92,  93,  94,  95,  96,  97,  98,  99, 100, 102, 103, 104, 105, 106, 107,
    108, 109, 110, 112, 113, 114, 115, 116, 117, 119, 120, 121, 122, 123, 124, 126,
    127, 128, 129, 130, 132, 133, 134, 135, 136, 138, 139, 140, 141, 142, 144, 145,
    146, 147, 149, 150, 151, 152, 154, 155, 156, 158, 159, 160, 161, 163, 164, 165,
    167, 168, 169, 171, 172, 173, 174, 176, 177, 178, 180, 181, 182, 184, 185, 187,
    188, 189, 191, 192, 193, 195, 196, 197, 199, 200, 202, 203, 204, 206, 207, 209,
    210, 211, 213, 214, 216, 217, 218, 220, 221, 223, 224, 226, 227, 228, 230, 231,
    233, 234, 236, 237, 239, 240, 242, 243, 245, 246, 248, 249, 251, 252, 254, 255,
};

uint32_t colorDistance(const Color &c1, const Color &c2, const Error &e1) {
    float r_diff = gamma_lut[c1.r] + e1.r - gamma_lut[c2.r];
    float g_diff = gamma_lut[c1.g] + e1.g - gamma_lut[c2.g];
    float b_diff = gamma_lut[c1.b] + e1.b - gamma_lut[c2.b];
    return round(r_diff * r_diff + g_diff * g_diff + b_diff * b_diff);
}

void spr2buffer(TFT_eSprite &spr, String &fileout, imgParam &imageParams) {
    long t = millis();
    LittleFS.begin();

    fs::File f_out = LittleFS.open(fileout, "w");

    bool dither = true, rotated = false;
    long bufw = spr.width(), bufh = spr.height();

    if (bufw > bufh && bufw!=400 && bufh!=300) {
        rotated = true;
        bufw = spr.height();
        bufh = spr.width();
    }

    int bufferSize = (bufw * bufh) / 8;
    uint8_t *blackBuffer = new uint8_t[bufferSize];
    uint8_t *redBuffer = new uint8_t[bufferSize];
    memset(blackBuffer, 0, bufferSize);
    memset(redBuffer, 0, bufferSize);

    std::vector<Color> palette = {
        {255, 255, 255},  // White
        {0, 0, 0},        // Black
        {255, 0, 0}       // Red
    };
    int num_colors = palette.size();
    Color color;
    Error *error_bufferold = new Error[bufw];
    Error *error_buffernew = new Error[bufw];

    memset(error_bufferold, 0, bufw * sizeof(Error));
    for (uint16_t y = 0; y < bufh; y++) {
        memset(error_buffernew, 0, bufw * sizeof(Error));
        for (uint16_t x = 0; x < bufw; x++) {
            if (rotated) {
                color = Color(spr.readPixel(bufh - 1 - y, x));
            } else {
                color = Color(spr.readPixel(x, y));
            }

            int best_color_index = 0;
            uint32_t best_color_distance = colorDistance(color, palette[0], error_bufferold[x]);
            for (int i = 1; i < num_colors; i++) {
                uint32_t distance = colorDistance(color, palette[i], error_bufferold[x]);
                if (distance < best_color_distance) {
                    best_color_distance = distance;
                    best_color_index = i;
                }
            }

            uint16_t bitIndex = 7 - (x % 8);
            uint16_t byteIndex = (y * bufw + x) / 8;
            if (best_color_index & 1) {
                blackBuffer[byteIndex] |= (1 << bitIndex);
            } else if (best_color_index & 2) {
                imageParams.hasRed = true;
                redBuffer[byteIndex] |= (1 << bitIndex);
            }

            if (imageParams.dither) {
                Error error = {
                    ((float)color.r + error_bufferold[x].r - palette[best_color_index].r) / 16.0f,
                    ((float)color.g + error_bufferold[x].g - palette[best_color_index].g) / 16.0f,
                    ((float)color.b + error_bufferold[x].b - palette[best_color_index].b) / 16.0f};

                error_buffernew[x].r += error.r * 5.0f;
                error_buffernew[x].g += error.g * 5.0f;
                error_buffernew[x].b += error.b * 5.0f;
                if (x > 0) {
                    error_buffernew[x - 1].r += error.r * 3.0f;
                    error_buffernew[x - 1].g += error.g * 3.0f;
                    error_buffernew[x - 1].b += error.b * 3.0f;
                }
                if (x < bufw - 1) {
                    error_buffernew[x + 1].r += error.r * 1.0f;
                    error_buffernew[x + 1].g += error.g * 1.0f;
                    error_buffernew[x + 1].b += error.b * 1.0f;
                    error_bufferold[x + 1].r += error.r * 7.0f;
                    error_bufferold[x + 1].g += error.g * 7.0f;
                    error_bufferold[x + 1].b += error.b * 7.0f;
                }
            }
        }
        memcpy(error_bufferold, error_buffernew, bufw * sizeof(Error));
    }
    delete[] error_buffernew;
    delete[] error_bufferold;

    f_out.write(blackBuffer, bufferSize);
    if (imageParams.hasRed) f_out.write(redBuffer, bufferSize);

    delete[] blackBuffer;
    delete[] redBuffer;

    f_out.close();
    Serial.println("finished writing buffer " + String(millis() - t) + "ms");
}
