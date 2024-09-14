/*******************************************************************************
 * Multiple Animated GIF Image Viewer
 * This is a simple Multiple Animated GIF image viewer example
 * Image Source: https://www.geocities.ws/finalfantasyfive/ff5animations.html
 * optimized with ezgif.com
 *
 * Setup steps:
 * 1. Change your LCD parameters in Arduino_GFX setting
 * 2. Upload Animated GIF file
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
 ******************************************************************************/
#define GIF_FILENAME1 "/jobs.gif"
#define GIF_FILENAME2 "/archer.gif"
#define GIF_FILENAME3 "/white.gif"
#define GIF_FILENAME4 "/lancer.gif"

/*******************************************************************************
 * Start of Arduino_GFX setting
 ******************************************************************************/
#include <Arduino_GFX_Library.h>

/* all display share same SPI Data Bus with individual CS, RST pins connected to MCU RST pin */
Arduino_DataBus *bus1 = new Arduino_HWSPI(DF_GFX_DC, 21 /* CS */);
Arduino_GFX *gfx1 = new Arduino_SSD1331(bus1, GFX_NOT_DEFINED /* RST */, 0 /* rotation */);

Arduino_DataBus *bus2 = new Arduino_HWSPI(DF_GFX_DC, 32 /* CS */);
Arduino_GFX *gfx2 = new Arduino_ST7735(bus2, GFX_NOT_DEFINED /* RST */, 3 /* rotation */, true /* IPS */, 80 /* width */, 160 /* height */, 26 /* col offset 1 */, 1 /* row offset 1 */, 26 /* col offset 2 */, 1 /* row offset 2 */);

Arduino_DataBus *bus3 = new Arduino_HWSPI(DF_GFX_DC, 22 /* CS */);
Arduino_GFX *gfx3 = new Arduino_ST7789(bus3, GFX_NOT_DEFINED /* RST */, 0 /* rotation */, true /* IPS */, 240 /* width */, 240 /* height */, 0 /* col offset 1 */, 0 /* row offset 1 */, 0 /* col offset 2 */, 80 /* row offset 2 */);

Arduino_DataBus *bus4 = new Arduino_HWSPI(DF_GFX_DC, 5 /* CS */);
Arduino_GFX *gfx4 = new Arduino_ILI9341(bus4, GFX_NOT_DEFINED /* RST */, 0 /* rotation */, false /* IPS */);
/*******************************************************************************
 * End of Arduino_GFX setting
 ******************************************************************************/

#if defined(TARGET_RP2040)
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
static GifClass gifClass1;
static GifClass gifClass2;
static GifClass gifClass3;
static GifClass gifClass4;

void setup()
{
  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  // while(!Serial);
  Serial.println("Arduino_GFX library Multiple Device Animated GIF Test!");

#ifdef GFX_EXTRA_PRE_INIT
  GFX_EXTRA_PRE_INIT();
#endif

  // Init all displays

  if (!gfx1->begin())
  {
    Serial.println("gfx1->begin() failed!");
  }
  gfx1->fillScreen(RED);
  delay(200);

  if (!gfx2->begin())
  {
    Serial.println("gfx2->begin() failed!");
  }
  gfx2->fillScreen(YELLOW);
  delay(200);

  if (!gfx3->begin())
  {
    Serial.println("gfx3->begin() failed!");
  }
  gfx3->fillScreen(GREEN);
  delay(200);

  if (!gfx4->begin())
  {
    Serial.println("gfx4->begin() failed!");
  }
  gfx4->fillScreen(BLUE);
  delay(200);

#if defined(TARGET_RP2040)
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
    gfx4->println(F("ERROR: File System Mount Failed!"));
    exit(0);
  }
}

void loop()
{
#if defined(TARGET_RP2040)
  File gifFile1 = LittleFS.open(GIF_FILENAME1, "r");
  File gifFile2 = LittleFS.open(GIF_FILENAME2, "r");
  File gifFile3 = LittleFS.open(GIF_FILENAME3, "r");
  File gifFile4 = LittleFS.open(GIF_FILENAME4, "r");
#elif defined(ESP32)
  File gifFile1 = LittleFS.open(GIF_FILENAME1, "r");
  File gifFile2 = LittleFS.open(GIF_FILENAME2, "r");
  File gifFile3 = LittleFS.open(GIF_FILENAME3, "r");
  File gifFile4 = LittleFS.open(GIF_FILENAME4, "r");
#elif defined(ESP8266)
  File gifFile1 = LittleFS.open(GIF_FILENAME1, "r");
  File gifFile2 = LittleFS.open(GIF_FILENAME2, "r");
  File gifFile3 = LittleFS.open(GIF_FILENAME3, "r");
  File gifFile4 = LittleFS.open(GIF_FILENAME4, "r");
#else
  File gifFile1 = SD.open(GIF_FILENAME1, FILE_READ);
  File gifFile2 = SD.open(GIF_FILENAME2, FILE_READ);
  File gifFile3 = SD.open(GIF_FILENAME3, FILE_READ);
  File gifFile4 = SD.open(GIF_FILENAME4, FILE_READ);
#endif

  if (!gifFile1 || gifFile1.isDirectory())
  {
    Serial.println(F("ERROR: open gifFile1 Failed!"));
    gfx1->println(F("ERROR: open gifFile1 Failed!"));
  }
  else if (!gifFile2 || gifFile2.isDirectory())
  {
    Serial.println(F("ERROR: open gifFile2 Failed!"));
    gfx2->println(F("ERROR: open gifFile2 Failed!"));
  }
  else if (!gifFile3 || gifFile3.isDirectory())
  {
    Serial.println(F("ERROR: open gifFile3 Failed!"));
    gfx3->println(F("ERROR: open gifFile3 Failed!"));
  }
  else if (!gifFile4 || gifFile4.isDirectory())
  {
    Serial.println(F("ERROR: open gifFile4 Failed!"));
    gfx4->println(F("ERROR: open gifFile4 Failed!"));
  }
  else
  {
    // read GIF file header
    gd_GIF *gif1 = gifClass1.gd_open_gif(&gifFile1);
    gd_GIF *gif2 = gifClass2.gd_open_gif(&gifFile2);
    gd_GIF *gif3 = gifClass3.gd_open_gif(&gifFile3);
    gd_GIF *gif4 = gifClass4.gd_open_gif(&gifFile4);
    if (!gif1)
    {
      Serial.println(F("gd_open_gif(&gifFile1) failed!"));
    }
    else if (!gif2)
    {
      Serial.println(F("gd_open_gif(&gifFile2) failed!"));
    }
    else if (!gif3)
    {
      Serial.println(F("gd_open_gif(&gifFile3) failed!"));
    }
    else if (!gif4)
    {
      Serial.println(F("gd_open_gif(&gifFile4) failed!"));
    }
    else
    {
      uint8_t *buf1 = (uint8_t *)malloc(gif1->width * gif1->height);
      uint8_t *buf2 = (uint8_t *)malloc(gif2->width * gif2->height);
      uint8_t *buf3 = (uint8_t *)malloc(gif3->width * gif3->height);
      uint8_t *buf4 = (uint8_t *)malloc(gif4->width * gif4->height);
      if (!buf1)
      {
        Serial.println(F("buf1 malloc failed!"));
      }
      if (!buf2)
      {
        Serial.println(F("buf2 malloc failed!"));
      }
      if (!buf3)
      {
        Serial.println(F("buf3 malloc failed!"));
      }
      if (!buf4)
      {
        Serial.println(F("buf4 malloc failed!"));
      }
      else
      {
        int16_t x1 = (gfx1->width() - gif1->width) / 2;
        int16_t y1 = (gfx1->height() - gif1->height) / 2;
        int16_t x2 = (gfx2->width() - gif2->width) / 2;
        int16_t y2 = (gfx2->height() - gif2->height) / 2;
        int16_t x3 = (gfx3->width() - gif3->width) / 2;
        int16_t y3 = (gfx3->height() - gif3->height) / 2;
        int16_t x4 = (gfx4->width() - gif4->width) / 2;
        int16_t y4 = (gfx4->height() - gif4->height) / 2;

        Serial.println(F("GIF video start"));
        int32_t res1, res2, res3, res4;
        uint32_t duration = 0, remain = 0;
        while (1)
        {
          res1 = gifClass1.gd_get_frame(gif1, buf1);
          res2 = gifClass2.gd_get_frame(gif2, buf2);
          res3 = gifClass3.gd_get_frame(gif3, buf3);
          res4 = gifClass4.gd_get_frame(gif4, buf4);
          if (res1 < 0)
          {
            Serial.println(F("ERROR: gd_get_frame(gif1, buf1) failed!"));
            break;
          }
          else if (res1 == 0)
          {
            Serial.println(F("rewind gif1"));
            gifClass1.gd_rewind(gif1);
          }
          else
          {
            gfx1->drawIndexedBitmap(x1, y1, buf1, gif1->palette->colors, gif1->width, gif1->height);
          }

          if (res2 < 0)
          {
            Serial.println(F("ERROR: gd_get_frame(gif2, buf2) failed!"));
            break;
          }
          else if (res2 == 0)
          {
            Serial.println(F("rewind gif2"));
            gifClass2.gd_rewind(gif2);
          }
          else
          {
            gfx2->drawIndexedBitmap(x2, y2, buf2, gif2->palette->colors, gif2->width, gif2->height);
          }

          if (res3 < 0)
          {
            Serial.println(F("ERROR: gd_get_frame(gif3, buf3) failed!"));
            break;
          }
          else if (res3 == 0)
          {
            Serial.println(F("rewind gif3"));
            gifClass3.gd_rewind(gif3);
          }
          else
          {
            gfx3->drawIndexedBitmap(x3, y3, buf3, gif3->palette->colors, gif3->width, gif3->height);
          }

          if (res4 < 0)
          {
            Serial.println(F("ERROR: gd_get_frame(gif4, buf4) failed!"));
            break;
          }
          else if (res4 == 0)
          {
            Serial.println(F("rewind gif4"));
            gifClass4.gd_rewind(gif4);
          }
          else
          {
            gfx4->drawIndexedBitmap(x4, y4, buf4, gif4->palette->colors, gif4->width, gif4->height);
          }
        }
        Serial.println(F("GIF video end"));
        Serial.print(F("duration: "));
        Serial.print(duration);
        Serial.print(F(", remain: "));
        Serial.print(remain);
        Serial.print(F(" ("));
        Serial.print(100.0 * remain / duration);
        Serial.println(F("%)"));

        gifClass1.gd_close_gif(gif1);
        gifClass2.gd_close_gif(gif2);
        gifClass3.gd_close_gif(gif3);
        gifClass4.gd_close_gif(gif4);
        free(buf1);
        free(buf2);
        free(buf3);
        free(buf4);
      }
    }
  }
}
