/*******************************************************************************
 * Sprite GIF Demo
 * This is a sprite demo using a static GIF as a master image
 * Image Source:
 * https://www.freepik.com/free-vector/urban-life-drawing_727890.htm#query=city%20road
 * https://giphy.com/gifs/car-carro-nQaMsylXcTIRNorQLJ
 *
 * Setup steps:
 * 1. Change your LCD parameters in Arduino_GFX setting
 * 2. Upload GIF file
 *   FFat (ESP32):
 *     upload FFat (FatFS) data with ESP32 Sketch Data Upload:
 *     ESP32: https://github.com/lorol/arduino-esp32fs-plugin
 *   LittleFS (ESP32 / ESP8266 / Pico):
 *     upload LittleFS data with ESP8266 LittleFS Data Upload:
 *     ESP32: https://github.com/lorol/arduino-esp32fs-plugin
 *     ESP8266: https://github.com/earlephilhower/arduino-esp8266littlefs-plugin
 *     Pico: https://github.com/earlephilhower/arduino-pico-littlefs-plugin.git
 *   SPIFFS (ESP32):
 *     upload SPIFFS data with ESP32 Sketch Data Upload:
 *     ESP32: https://github.com/lorol/arduino-esp32fs-plugin
 *   SD:
 *     Most Arduino system built-in support SD file system.
 *     Wio Terminal require extra dependant Libraries:
 *     - Seeed_Arduino_FS: https://github.com/Seeed-Studio/Seeed_Arduino_FS.git
 *     - Seeed_Arduino_SFUD: https://github.com/Seeed-Studio/Seeed_Arduino_SFUD.git
 ******************************************************************************/
#define GIF_FILENAME "/city17_240.gif"

/*******************************************************************************
 * Start of Arduino_G FX setting
 *
 * Arduino_GFX try to find the settings depends on selected board in Arduino IDE
 * Or you can define the display dev kit not in the board list
 * Defalult pin list for non display dev kit:
 * Arduino Nano, Micro and more: CS:  9, DC:  8, RST:  7, BL:  6, SCK: 13, MOSI: 11, MISO: 12
 * ESP32 various dev board     : CS:  5, DC: 27, RST: 33, BL: 22, SCK: 18, MOSI: 23, MISO: nil
 * ESP32-C3 various dev board  : CS:  7, DC:  2, RST:  1, BL:  3, SCK:  4, MOSI:  6, MISO: nil
 * ESP32-S2 various dev board  : CS: 34, DC: 38, RST: 33, BL: 21, SCK: 36, MOSI: 35, MISO: nil
 * ESP32-S3 various dev board  : CS: 40, DC: 41, RST: 42, BL: 48, SCK: 36, MOSI: 35, MISO: nil
 * ESP8266 various dev board   : CS: 15, DC:  4, RST:  2, BL:  5, SCK: 14, MOSI: 13, MISO: 12
 * Raspberry Pi Pico dev board : CS: 17, DC: 27, RST: 26, BL: 28, SCK: 18, MOSI: 19, MISO: 16
 * RTL8720 BW16 old patch core : CS: 18, DC: 17, RST:  2, BL: 23, SCK: 19, MOSI: 21, MISO: 20
 * RTL8720_BW16 Official core  : CS:  9, DC:  8, RST:  6, BL:  3, SCK: 10, MOSI: 12, MISO: 11
 * RTL8722 dev board           : CS: 18, DC: 17, RST: 22, BL: 23, SCK: 13, MOSI: 11, MISO: 12
 * RTL8722_mini dev board      : CS: 12, DC: 14, RST: 15, BL: 13, SCK: 11, MOSI:  9, MISO: 10
 * Seeeduino XIAO dev board    : CS:  3, DC:  2, RST:  1, BL:  0, SCK:  8, MOSI: 10, MISO:  9
 * Teensy 4.1 dev board        : CS: 39, DC: 41, RST: 40, BL: 22, SCK: 13, MOSI: 11, MISO: 12
 ******************************************************************************/
#include <Arduino_GFX_Library.h>

#define GFX_BL DF_GFX_BL // default backlight pin, you may replace DF_GFX_BL to actual backlight pin

/* More dev device declaration: https://github.com/moononournation/Arduino_GFX/wiki/Dev-Device-Declaration */
#if defined(DISPLAY_DEV_KIT)
Arduino_GFX *gfx = create_default_Arduino_GFX();
#else /* !defined(DISPLAY_DEV_KIT) */

/* More data bus class: https://github.com/moononournation/Arduino_GFX/wiki/Data-Bus-Class */
Arduino_DataBus *bus = create_default_Arduino_DataBus();

/* More display class: https://github.com/moononournation/Arduino_GFX/wiki/Display-Class */
Arduino_GFX *gfx = new Arduino_ILI9341(bus, DF_GFX_RST, 3 /* rotation */, false /* IPS */);

#endif /* !defined(DISPLAY_DEV_KIT) */
Arduino_Canvas_Indexed *canvasGfx = new Arduino_Canvas_Indexed(320 /* width */, 240 /* height */, gfx);
/*******************************************************************************
 * End of Arduino_GFX setting
 ******************************************************************************/

/* Wio Terminal */
#if defined(ARDUINO_ARCH_SAMD) && defined(SEEED_GROVE_UI_WIRELESS)
#include <Seeed_FS.h>
#include <SD/Seeed_SD.h>
#elif defined(TARGET_RP2040)
#include <LittleFS.h>
#include <SD.h>
#elif defined(ESP32)
#include <FFat.h>
#include <LittleFS.h>
#include <SPIFFS.h>
#include <SD.h>
#elif defined(ESP8266)
#include <LittleFS.h>
#include <SD.h>
#else
#include <SD.h>
#endif

#include "GifClass.h"
static GifClass gifClass;

uint8_t *spriteMaster;
bool spriteInitiated = false;

#include "IndexedSprite.h"
IndexedSprite *background;
IndexedSprite *road;
IndexedSprite *cars;
IndexedSprite *birds;
IndexedSprite *sun;
IndexedSprite *clouds;
IndexedSprite *mpv;

int frame = 0;
int fpsSnapShot = 0;
unsigned long nextSnap = 0;

void setup()
{
  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  // while(!Serial);
  Serial.println("GIF Sprite Demo");

#ifdef GFX_EXTRA_PRE_INIT
  GFX_EXTRA_PRE_INIT();
#endif

  // Init Display
  if (!canvasGfx->begin())
  {
    Serial.println("canvasGfx->begin() failed!");
  }
  canvasGfx->fillScreen(BLACK);
  canvasGfx->flush();
  canvasGfx->setDirectUseColorIndex(true);

#ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#endif

/* Wio Terminal */
#if defined(ARDUINO_ARCH_SAMD) && defined(SEEED_GROVE_UI_WIRELESS)
  if (!SD.begin(SDCARD_SS_PIN, SDCARD_SPI, 4000000UL))
#elif defined(TARGET_RP2040)
  if (!LittleFS.begin())
  // if (!SD.begin(SS))
#elif defined(ESP32)
  // if (!FFat.begin())
  if (!LittleFS.begin())
  // if (!SPIFFS.begin())
  // if (!SD.begin(SS))
#elif defined(ESP8266)
  if (!LittleFS.begin())
  // if (!SD.begin(SS))
#else
  if (!SD.begin())
#endif
  {
    Serial.println(F("ERROR: File System Mount Failed!"));
    gfx->println(F("ERROR: File System Mount Failed!"));
    exit(0);
  }

/* Wio Terminal */
#if defined(ARDUINO_ARCH_SAMD) && defined(SEEED_GROVE_UI_WIRELESS)
  File gifFile = SD.open(GIF_FILENAME, "r");
#elif defined(TARGET_RP2040)
  File gifFile = LittleFS.open(GIF_FILENAME, "r");
  // File gifFile = SD.open(GIF_FILENAME, "r");
#elif defined(ESP32)
  // File gifFile = FFat.open(GIF_FILENAME, "r");
  File gifFile = LittleFS.open(GIF_FILENAME, "r");
  // File gifFile = SPIFFS.open(GIF_FILENAME, "r");
  // File gifFile = SD.open(GIF_FILENAME, "r");
#elif defined(ESP8266)
  File gifFile = LittleFS.open(GIF_FILENAME, "r");
  // File gifFile = SD.open(GIF_FILENAME, "r");
#else
  File gifFile = SD.open(GIF_FILENAME, FILE_READ);
#endif
  if (!gifFile || gifFile.isDirectory())
  {
    Serial.println(F("ERROR: open gifFile Failed!"));
    gfx->println(F("ERROR: open gifFile Failed!"));
  }
  else
  {
    // read GIF file header
    gd_GIF *gif = gifClass.gd_open_gif(&gifFile);
    if (!gif)
    {
      Serial.println(F("gd_open_gif() failed!"));
    }
    else
    {
      spriteMaster = (uint8_t *)malloc(gif->width * gif->height / 2);
      if (!spriteMaster)
      {
        Serial.println(F("spriteMaster malloc failed!"));
      }
      else
      {
        int32_t res = gifClass.gd_get_frame(gif, spriteMaster);

        if (res > 0)
        {
          // inital palette
          uint16_t *palette = canvasGfx->getColorIndex();
          memcpy(palette, gif->palette->colors, gif->palette->len * 2);

          //IndexedSprite(x, y, *bitmap, *palette, w, h, x_skip, loop, frames, speed_divider, chroma_key)
          background = new IndexedSprite(0, 0, spriteMaster, palette, 405, 180, 0, true, 1, 3);
          road = new IndexedSprite(0, 180, spriteMaster + (180 * 405), palette, 405, 60, 0, true, 1, 1);
          cars = new IndexedSprite(0, 182, spriteMaster + (240 * 405), palette, 405, 11, 0, true, 1, 1, gif->gce.tindex);
          birds = new IndexedSprite(0, 80, spriteMaster + (251 * 405), palette, 51, 32, (405 - 51), false, 4, 4, gif->gce.tindex);
          sun = new IndexedSprite(16, 16, spriteMaster + (251 * 405) + 210, palette, 30, 30, (405 - 30), false, 1, 0, gif->gce.tindex);
          clouds = new IndexedSprite(0, 2, spriteMaster + (283 * 405), palette, 405, 94, 0, true, 1, 2, gif->gce.tindex);
          mpv = new IndexedSprite((canvasGfx->width() - 70) / 2, 182, spriteMaster + (377 * 405), palette, 50, 30, (405 - 50), false, 8, 2, gif->gce.tindex);

          spriteInitiated = true;
        }

        gifClass.gd_close_gif(gif);
      }
    }
  }

  canvasGfx->setTextColor(0xfd, 0x00);
  canvasGfx->setTextSize(1);
}

bool otherFrame = false;
void testingLoop(void)
{
  if (spriteInitiated)
  {
    background->h_scroll(-1);
    background->draw(canvasGfx);

    road->h_scroll(-3);
    road->draw(canvasGfx);

    cars->h_scroll(-6);
    cars->draw(canvasGfx);

    birds->h_scroll(1, 480);
    birds->next_frame();
    birds->draw(canvasGfx);

    sun->draw(canvasGfx);

    clouds->h_scroll(1);
    clouds->draw(canvasGfx);

    mpv->next_frame();
    mpv->draw(canvasGfx);
  }
}

void loop()
{
  testingLoop();

  canvasGfx->setCursor(8, 8);
  canvasGfx->print(fpsSnapShot);

  canvasGfx->flush();

  // calculate FPS
  frame++;
  if (millis() > nextSnap)
  {
    fpsSnapShot = frame;
    frame = 0;
    nextSnap = millis() + 1000;
  }
}
