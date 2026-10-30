#include <Arduino.h>

#ifdef HAS_RGB_LED
// One WS2812B led, driven with neopixelWrite(); the color names and values are FastLED's
struct CRGB {
    enum HTMLColorCode : uint32_t {
        Aqua = 0x00FFFF,
        Black = 0x000000,
        Blue = 0x0000FF,
        DarkBlue = 0x00008B,
        Green = 0x008000,
        Orange = 0xFFA500,
        Red = 0xFF0000,
        White = 0xFFFFFF,
        Yellow = 0xFFFF00,
        YellowGreen = 0x9ACD32,
    };
    uint8_t r = 0, g = 0, b = 0;

    CRGB() = default;
    CRGB(uint32_t colorcode) : r(colorcode >> 16), g(colorcode >> 8), b(colorcode) {}
    bool operator==(const CRGB& other) const { return r == other.r && g == other.g && b == other.b; }
    bool operator!=(const CRGB& other) const { return !(*this == other); }
    // scale up or down so the brightest channel becomes 'limit'
    void maximizeBrightness(uint8_t limit = 255) {
        uint8_t brightest = max(r, max(g, b));
        if (brightest == 0) return;
        r = r * limit / brightest;
        g = g * limit / brightest;
        b = b * limit / brightest;
    }
};

// amountOfB 0 gives a, 255 gives b
CRGB blend(const CRGB& a, const CRGB& b, uint8_t amountOfB);
#endif

const uint8_t PROGMEM gamma8[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
    2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5,
    5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10,
    10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
    17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
    25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
    37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
    51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
    69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
    90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
    115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
    144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
    177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
    215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255};
    
void ledTask(void* parameter);
void setBrightness(int brightness);
void updateBrightnessFromConfig();
void ledcSet(uint8_t channel, uint8_t brightness);

#ifdef HAS_RGB_LED
extern CRGB rgbIdleColor;
extern uint16_t rgbIdlePeriod;
void shortBlink(CRGB cname);
void showColorPattern(CRGB colorone, CRGB colortwo, CRGB colorthree);
void rgbIdle();
void addFadeColor(CRGB cname);
#endif

void quickBlink(uint8_t repeat);
void addFadeMono(uint8_t value);
