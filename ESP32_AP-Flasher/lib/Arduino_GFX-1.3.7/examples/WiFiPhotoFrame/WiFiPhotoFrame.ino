/*******************************************************************************
 * WiFi Photo Frame
 * This is a simple IoT photo frame sample
 * Please find more details at instructables:
 * https://www.instructables.com/id/Face-Aware-OSD-Photo-Frame/
 *
 * Dependent libraries:
 * JPEGDEC: https://github.com/bitbank2/JPEGDEC.git
 *
 * Raspberry Pi Pico W dependent libraries:
 * HttpClient: https://github.com/moononournation/HttpClient.git
 *
 * Setup steps:
 * 1. Fill your own SSID_NAME, SSID_PASSWORD, HTTP_HOST, HTTP_PORT and HTTP_PATH_TEMPLATE
 * 2. Change your LCD parameters in Arduino_GFX setting
 ******************************************************************************/

/* WiFi settings */
const char *SSID_NAME = "YourAP";
const char *SSID_PASSWORD = "PleaseInputYourPasswordHere";

const char *HTTP_HOST = "192.168.12.34";                /* Your HTTP photo server host name */
const uint16_t HTTP_PORT = 5000;                        /* Your HTTP photo server port */
const char *HTTP_PATH_TEMPLATE = "/OSDPhoto?w=%d&h=%d"; /* Your HTTP photo server URL path template */

const uint16_t HTTP_TIMEOUT = 30000; // in ms, wait a while for server processing

/*******************************************************************************
 * Start of Arduino_GFX setting
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
Arduino_GFX *gfx = new Arduino_ILI9341(bus, DF_GFX_RST, 0 /* rotation */, false /* IPS */);

#endif /* !defined(DISPLAY_DEV_KIT) */
/*******************************************************************************
 * End of Arduino_GFX setting
 ******************************************************************************/

#if defined(ESP32)
#include <esp_task_wdt.h>
#include <WiFi.h>
#include <HTTPClient.h>
WiFiClient client;
HTTPClient http;
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
WiFiClient client;
HTTPClient http;
#elif defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#include <WiFiClient.h>
#include <HttpClient.h>
WiFiClient client;
HttpClient http(client);
#elif defined(RTL8722DM)
#include <WiFi.h>
#include <WiFiClient.h>
#include <HttpClient.h>
WiFiClient client;
HttpClient http(client);
#endif

#include "JpegFunc.h"

// pixel drawing callback
static int jpegDrawCallback(JPEGDRAW *pDraw)
{
  // Serial.printf("Draw pos = %d,%d. size = %d x %d\n", pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
  gfx->draw16bitRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}

static unsigned long next_show_millis = 0;

char http_path[1024];

void setup()
{
  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  // while(!Serial);
  Serial.println("WiFi Photo Frame");

#ifdef GFX_EXTRA_PRE_INIT
  GFX_EXTRA_PRE_INIT();
#endif

  Serial.println("Init display");
  if (!gfx->begin())
  {
    Serial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(BLACK);

#ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#endif

  Serial.println("Init WiFi");
  gfx->println("Init WiFi");
#if defined(ESP32) || defined(ESP8266)
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID_NAME, SSID_PASSWORD);
#elif defined(ARDUINO_RASPBERRY_PI_PICO_W)
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID_NAME, SSID_PASSWORD);
#elif defined(RTL8722DM)
  WiFi.begin((char *)SSID_NAME, (char *)SSID_PASSWORD);
#endif
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    gfx->print(".");
  }
  Serial.println(" CONNECTED");
  gfx->println(" CONNECTED");

  Serial.println("Init HTTP path");
  gfx->println("Init HTTP path");
  // setup http_path query value with LCD dimension
  sprintf(http_path, HTTP_PATH_TEMPLATE, gfx->width(), gfx->height());
  Serial.println(http_path);
  gfx->println(http_path);

#if defined(ESP32)
  Serial.println("Init ESP32 WDT");
  gfx->println("Init ESP32 WDT");
  // set WDT timeout a little bit longer than HTTP timeout
  esp_task_wdt_init((HTTP_TIMEOUT / 1000) + 1, true);
  enableLoopWDT();
#endif
}

void loop()
{
  if (millis() < next_show_millis)
  {
    // Serial.println("Wait for next minute...");
    delay(1000);
  }
  else
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      int jpeg_result = 0;

      Serial.printf("[HTTP] begin...\n");
      http.setTimeout(HTTP_TIMEOUT);
#if defined(ESP32) || defined(ESP8266)
      http.begin(client, HTTP_HOST, HTTP_PORT, http_path);
      int httpCode = http.GET();
#elif defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(RTL8722DM)
      http.get(HTTP_HOST, HTTP_PORT, http_path);
      int httpCode = http.responseStatusCode();
      http.skipResponseHeaders();
#endif

      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      gfx->printf("[HTTP] GET... code: %d\n", httpCode);
      // HTTP header has been send and Server response header has been handled
      if (httpCode <= 0)
      {
#if defined(ESP32) || defined(ESP8266)
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
#endif
      }
      else
      {
        if (httpCode != 200)
        {
          Serial.printf("[HTTP] Not OK!\n");
          gfx->printf("[HTTP] Not OK!\n");
          delay(5000);
        }
        else
        {
// get lenght of document(is - 1 when Server sends no Content - Length header)
#if defined(ESP32) || defined(ESP8266)
          int len = http.getSize();
#elif defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(RTL8722DM)
          int len = http.contentLength();
#endif
          Serial.printf("[HTTP] size: %d\n", len);
          gfx->printf("[HTTP] size: %d\n", len);

          if (len <= 0)
          {
            Serial.printf("[HTTP] Unknow content size: %d\n", len);
            gfx->printf("[HTTP] Unknow content size: %d\n", len);
          }
          else
          {
            unsigned long start = millis();

            uint8_t *buf = (uint8_t *)malloc(len);
            if (buf)
            {
#if defined(ESP32) || defined(ESP8266)
              static WiFiClient *http_stream = http.getStreamPtr();
              jpeg_result = jpegOpenHttpStreamWithBuffer(http_stream, buf, len, jpegDrawCallback);
#elif defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(RTL8722DM)
              jpeg_result = jpegOpenHttpStreamWithBuffer(&client, buf, len, jpegDrawCallback);
#endif
              if (jpeg_result)
              {
                jpeg_result = jpegDraw(false /* useBigEndian */,
                                       0 /* x */, 0 /* y */, gfx->width() /* widthLimit */, gfx->height() /* heightLimit */);
              }
              free(buf);
            }
            else
            {
// get tcp stream
#if defined(ESP32) || defined(ESP8266)
              static WiFiClient *http_stream = http.getStreamPtr();
              jpeg_result = jpegOpenHttpStream(http_stream, len, jpegDrawCallback);
#elif defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(RTL8722DM)
              jpeg_result = jpegOpenHttpStream(&client, len, jpegDrawCallback);
#endif
              if (jpeg_result)
              {
                jpeg_result = jpegDraw(false /* useBigEndian */,
                                       0 /* x */, 0 /* y */, gfx->width() /* widthLimit */, gfx->height() /* heightLimit */);
              }
            }
            Serial.printf("Time used: %lu\n", millis() - start);
          }
        }
      }
#if defined(ESP32) || defined(ESP8266)
      http.end();
#elif defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(RTL8722DM)
      http.stop();
#endif

      if (jpeg_result)
      {
        next_show_millis = ((millis() / 60000L) + 1) * 60000L; // next minute
      }
    }
  }

#if defined(ESP32)
  // notify WDT still working
  feedLoopWDT();
#elif defined(ESP8266)
  yield();
#elif defined(ARDUINO_RASPBERRY_PI_PICO_W)
  yield();
#endif
}
