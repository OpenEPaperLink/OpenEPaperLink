/*******************************************************************************
 * Arduino VNC
 * This is a simple VNC sample
 *
 * Dependent libraries:
 * ArduinoVNC: https://github.com/moononournation/arduinoVNC.git
 *
 * Touch libraries:
 * FT6X36: https://github.com/strange-v/FT6X36.git
 * GT911: https://github.com/TAMCTec/gt911-arduino.git
 * XPT2046: https://github.com/PaulStoffregen/XPT2046_Touchscreen.git
 *
 * Setup steps:
 * 1. Fill your own SSID_NAME, SSID_PASSWORD, VNC_IP, VNC_PORT and VNC_PASSWORD
 * 2. Change your LCD parameters in Arduino_GFX setting
 ******************************************************************************/

/* WiFi settings */
const char *SSID_NAME = "YourAP";
const char *SSID_PASSWORD = "PleaseInputYourPasswordHere";

const char *VNC_IP = "192.168.12.34";
const uint16_t VNC_PORT = 5901;
const char *VNC_PASSWORD = "PleaseInputYourPasswordHere";

/*******************************************************************************
 * Please config the touch panel in touch.h
 ******************************************************************************/
#include "touch.h"

/*******************************************************************************
 * Please config the optional keyboard in keyboard.h
 ******************************************************************************/
#include "keyboard.h"

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
Arduino_GFX *gfx = new Arduino_ILI9341(bus, DF_GFX_RST, 3 /* rotation */, false /* IPS */);

#endif /* !defined(DISPLAY_DEV_KIT) */
/*******************************************************************************
 * End of Arduino_GFX setting
 ******************************************************************************/

#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#elif defined(RTL8722DM)
#include <WiFi.h>
#endif

#include "VNC_GFX.h"
#include <VNC.h>

VNC_GFX *vnc_gfx = new VNC_GFX(gfx);
arduinoVNC vnc = arduinoVNC(vnc_gfx);

void TFTnoWifi(void)
{
  gfx->fillScreen(BLACK);
  gfx->setCursor(0, ((gfx->height() / 2) - (5 * 8)));
  gfx->setTextColor(RED);
  gfx->setTextSize(5);
  gfx->println("NO WIFI!");
  gfx->setTextSize(2);
  gfx->println();
}

void TFTnoVNC(void)
{
  gfx->fillScreen(BLACK);
  gfx->setCursor(0, ((gfx->height() / 2) - (4 * 8)));
  gfx->setTextColor(GREEN);
  gfx->setTextSize(4);
  gfx->println("connect VNC");
  gfx->setTextSize(2);
  gfx->println();
  gfx->print(VNC_IP);
  gfx->print(":");
  gfx->println(VNC_PORT);
}

void handle_touch()
{
  if (touch_has_signal())
  {
    if (touch_touched())
    {
      vnc.mouseEvent(touch_last_x, touch_last_y, 0b001);
    }
    else if (touch_released())
    {
      vnc.mouseEvent(touch_last_x, touch_last_y, 0b000);
    }
  }
}

void handle_keyboard()
{
  int key = keyboard_get_key();
  if (key > 0)
  {
    // Serial.println(key);
    switch (key)
    {
    case 8:
      key = 0xff08; // BackSpace
      break;
    case 9:
      key = 0xff09; // Tab
      break;
    case 13:
      key = 0xff0d; // Return or Enter
      break;
    case 27:
      key = 0xff1b; // Escape
      break;
    case 180:
      key = 0xff51; // Left
      break;
    case 181:
      key = 0xff52; // Up
      break;
    case 182:
      key = 0xff54; // Down
      break;
    case 183:
      key = 0xff53; // Right
      break;
    }
    vnc.keyEvent(key, 0b001);
    vnc.keyEvent(key, 0b000);
  }
}

void setup(void)
{
  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  // while(!Serial);
  Serial.println("Arduino VNC");

#ifdef GFX_EXTRA_PRE_INIT
  GFX_EXTRA_PRE_INIT();
#endif

  // Init keyboard device
  keyboard_init();

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

  // Init touch device
  touch_init(gfx->width(), gfx->height(), gfx->getRotation());

  TFTnoWifi();

  Serial.println("Init WiFi");
  gfx->println("Init WiFi");
#if defined(ESP32)
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID_NAME, SSID_PASSWORD);
#elif defined(ESP8266)
  // disable sleep mode for better data rate
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
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
  Serial.println("IP address: ");
  gfx->println("IP address: ");
  Serial.println(WiFi.localIP());
  gfx->println(WiFi.localIP());
  TFTnoVNC();

  Serial.println(F("[SETUP] VNC..."));

#ifdef SEPARATE_DRAW_TASK
  draw_task_setup();
#endif

  vnc.begin(VNC_IP, VNC_PORT);
  vnc.setPassword(VNC_PASSWORD); // optional
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    vnc.reconnect();
    TFTnoWifi();
    delay(100);
  }
  else
  {
    if (vnc.connected())
    {
      handle_touch();
      handle_keyboard();
    }
    vnc.loop();
    if (!vnc.connected())
    {
      TFTnoVNC();
      // some delay to not flood the server
      delay(5000);
    }
  }
}
