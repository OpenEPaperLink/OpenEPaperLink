/*******************************************************************************
 * U8g2 RSS Reader
 * This is a simple RSS Reader sample with UTF-8 support
 *
 *  * Raspberry Pi Pico W dependent libraries:
 * HttpClient: https://github.com/moononournation/HttpClient.git
 *
 * Setup steps:
 * 1. Fill your own SSID_NAME, SSID_PASSWORD, RSS_HOST, RSS_PORT, RSS_PATH
 * 2. Change your LCD parameters in Arduino_GFX setting
 ******************************************************************************/

/* WiFi settings */
#define SSID_NAME "YourAP"
#define SSID_PASSWORD "PleaseInputYourPasswordHere"

#define RSS_HOST "rss.weather.gov.hk"
#define RSS_PORT 80
#define RSS_PATH "/rss/LocalWeatherForecast_uc.xml"

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
#include <U8g2lib.h>
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

void setup(void)
{
  Serial.begin(115200);
  // while (!Serial);
  // Serial.setDebugOutput(true);
  Serial.println("U8g2 RSS Reader");

#ifdef GFX_EXTRA_PRE_INIT
  GFX_EXTRA_PRE_INIT();
#endif

  Serial.println("Init display");
  if (!gfx->begin())
  {
    Serial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(BLACK);
  gfx->setUTF8Print(true); // enable UTF8 support for the Arduino print() function

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

  /* U8g2 font list: https://github.com/olikraus/u8g2/wiki/fntlistall */
  /* U8g2 Unifont list: https://github.com/olikraus/u8g2/wiki/fntgrpunifont */
  gfx->setFont(u8g2_font_unifont_t_chinese4);
  gfx->setTextColor(WHITE);
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.printf("[HTTP] begin...\n");
#if defined(ESP32) || defined(ESP8266)
    http.begin(client, RSS_HOST, RSS_PORT, RSS_PATH);
    int httpCode = http.GET();
#elif defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(RTL8722DM)
    http.get(RSS_HOST, RSS_PORT, RSS_PATH);
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
          // get XML string
          String xml = http.readString();
          // update hour
          int key_idx = xml.indexOf("<item>");
          key_idx = xml.indexOf("<title>", key_idx + 6);
          int val_start_idx = key_idx + 7;
          int val_end_idx = xml.indexOf('<', val_start_idx);
          int update_hour = xml.substring(val_start_idx + 35, val_start_idx + 37).toInt();
          String title = xml.substring(val_start_idx, val_end_idx);
          Serial.println(title);

          gfx->fillScreen(BLACK);
          gfx->setCursor(0, 16);

          // gfx->setTextSize(2);
          gfx->setTextColor(GREEN);
          gfx->println(title);
          gfx->println();

          // description
          key_idx = xml.indexOf("<description><![CDATA[", val_end_idx);
          val_start_idx = key_idx + 22;
          val_end_idx = xml.indexOf("]]></description>", val_start_idx);
          String description = xml.substring(val_start_idx, val_end_idx);
          description.trim();
          Serial.println(description);

          // gfx->setTextSize(1);
          gfx->setTextColor(WHITE);
          val_start_idx = 0;
          while (val_start_idx < description.length())
          {
            val_end_idx = description.indexOf("<br/>", val_start_idx);
            if (val_end_idx < 0)
            {
              val_end_idx = description.length();
            }
            String paragraph = description.substring(val_start_idx, val_end_idx);
            paragraph.trim();
            gfx->println(paragraph);
            val_start_idx = val_end_idx + 5;
          }
        }
      }
#if defined(ESP32) || defined(ESP8266)
      http.end();
#elif defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(RTL8722DM)
      http.stop();
#endif

      delay(60 * 60 * 1000);
    }
  }

  delay(1000);
}
