/*
 * Wio WiFi Analyzer
 * Require Wio Terminal.
 * 
 * Libraries:
 * https://github.com/Seeed-Studio/Seeed_Arduino_FS/releases/tag/v2.0.2
 * https://github.com/Seeed-Studio/Seeed_Arduino_SFUD/releases/tag/v2.0.1
 * https://github.com/Seeed-Studio/Seeed_Arduino_mbedtls/archive/d1ca0175e24768120781bf4a43a1fb2c39fce85f.zip
 * https://github.com/Seeed-Studio/Seeed_Arduino_rpcUnified/releases/tag/v2.1.1
 * https://github.com/Seeed-Studio/Seeed_Arduino_rpcWiFi/releases/tag/v1.0.2
 * 
 * Firmware:
 * https://github.com/Seeed-Studio/seeed-ambd-firmware/releases/tag/v2.1.1
 */

#define SCAN_INTERVAL 1000

#include <Arduino_GFX_Library.h>

#define GFX_BL DF_GFX_BL // default backlight pin, you may replace DF_GFX_BL to actual backlight pin

Arduino_GFX *gfx = create_default_Arduino_GFX();

#include "rpcWiFi.h"

int16_t w, h, text_size, banner_height, graph24_baseline, graph50_baseline, graph_baseline, graph_height, channel24_width, channel50_width, channel_width, signal_width;

// RSSI RANGE
#define RSSI_CEILING -40
#define RSSI_FLOOR -100

// Channel legend mapping
uint16_t channel_legend[] = {
    1, 2, 3, 4, 5, 6, 7,      //  1,  2,  3,  4,  5,  6,  7,
    8, 9, 10, 11, 12, 13, 14, //  8,  9, 10, 11, 12, 13, 14,
    32, 0, 0, 0, 40, 0, 0,    // 32, 34, 36, 38, 40, 42, 44,
    0, 48, 0, 0, 0, 56, 0,    // 46, 48, 50, 52, 54, 56, 58,
    0, 0, 64, 0, 0, 0,        // 60, 62, 64, 68,N/A, 96,
    100, 0, 0, 0, 108, 0, 0,  //100,102,104,106,108,110,112,
    0, 116, 0, 0, 0, 124, 0,  //114,116,118,120,122,124,126,
    0, 0, 132, 0, 0, 0, 140,  //128,N/A,132,134,136,138,140,
    0, 0, 0, 149, 0, 0, 0,    //142,144,N/A,149,151,153,155,
    157, 0, 0, 0, 165, 0, 0,  //157,159,161,163,165,167,169,
    0, 173};                  //171,173

// Channel color mapping
uint16_t channel_color[] = {
    RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, MAGENTA,
    RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, MAGENTA,
    RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, MAGENTA,
    RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, MAGENTA,
    RED, ORANGE, YELLOW, GREEN, WHITE, MAGENTA,
    RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, MAGENTA,
    RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, MAGENTA,
    RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, MAGENTA,
    RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, MAGENTA,
    RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, MAGENTA,
    RED, ORANGE};

uint8_t scan_count = 0;

uint16_t channelIdx(int channel)
{
  if (channel <= 14) // 2.4 GHz, channel 1-14
  {
    return channel - 1;
  }
  if (channel <= 64) // 5 GHz, channel 32 - 64
  {
    return 14 + ((channel - 32) / 2);
  }
  if (channel == 68)
  {
    return 31;
  }
  if (channel == 96)
  {
    return 33;
  }
  if (channel <= 144)
  {
    return 34 + ((channel - 100) / 2); // channe;
  }
  return 58 + ((channel - 149) / 2);
}

void setup()
{
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

#ifdef GFX_EXTRA_PRE_INIT
  GFX_EXTRA_PRE_INIT();
#endif

#if defined(LCD_PWR_PIN)
  pinMode(LCD_PWR_PIN, OUTPUT);    // sets the pin as output
  digitalWrite(LCD_PWR_PIN, HIGH); // power on
#endif

#ifdef GFX_BL
    pinMode(GFX_BL, OUTPUT);
    digitalWrite(GFX_BL, HIGH);
#endif

  // Init Display
  if (!gfx->begin())
  {
    Serial.println("gfx->begin() failed!");
  }
  w = gfx->width();
  h = gfx->height();
  text_size = (h < 200) ? 1 : 2;
  banner_height = (text_size * 8) + 4;
  graph_height = ((gfx->height() - banner_height) / 2) - 30;
  graph24_baseline = banner_height + graph_height + 10;
  graph50_baseline = graph24_baseline + graph_height + 30;
  channel24_width = w / 17;
  channel50_width = w / 62;

  // init banner
  gfx->setTextSize(text_size);
  gfx->fillScreen(BLACK);
  gfx->setTextColor(BLUE);
  gfx->setCursor(2, 2);
  gfx->print("Wio");
  gfx->setTextColor(WHITE);
  gfx->print(" WiFi Analyzer");
}

void loop()
{
  uint8_t ap_count_list[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int32_t peak_list[] = {RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR, RSSI_FLOOR};
  int16_t peak_id_list[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
  int32_t channel;
  uint16_t idx;
  int32_t rssi;
  String ssid;
  uint16_t color;
  int16_t height, offset, text_width;

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks(false /* async */, true /* show_hidden */, true /* passive */, 500 /* max_ms_per_chan */);

  // clear old graph
  gfx->fillRect(0, banner_height, w, h - banner_height, BLACK);
  gfx->setTextSize(1);

  if (n == 0)
  {
    gfx->setTextColor(WHITE);
    gfx->setCursor(0, banner_height);
    gfx->println("No networks found");
  }
  else
  {
    for (int i = 0; i < n; i++)
    {
      channel = WiFi.channel(i);
      idx = channelIdx(channel);
      rssi = WiFi.RSSI(i);

      // channel peak stat
      if (peak_list[idx] < rssi)
      {
        peak_list[idx] = rssi;
        peak_id_list[idx] = i;
      }

      ap_count_list[idx]++;
    }

    // plot found WiFi info
    for (int i = 0; i < n; i++)
    {
      channel = WiFi.channel(i);
      idx = channelIdx(channel);
      rssi = WiFi.RSSI(i);
      color = channel_color[idx];
      height = constrain(map(rssi, RSSI_FLOOR, RSSI_CEILING, 1, graph_height), 1, graph_height);
      if (idx < 14)
      {
        graph_baseline = graph24_baseline;
        channel_width = channel24_width;
        signal_width = channel24_width * 2;
        offset = (idx + 2) * channel24_width;
      }
      else
      {
        graph_baseline = graph50_baseline;
        channel_width = channel50_width;
        signal_width = channel50_width * 2;
        offset = (idx - 14 + 2) * channel50_width;
      }

      // trim rssi with RSSI_FLOOR
      if (rssi < RSSI_FLOOR)
      {
        rssi = RSSI_FLOOR;
      }

      // plot chart
      // gfx->drawLine(offset, graph_baseline - height, offset - signal_width, graph_baseline + 1, color);
      // gfx->drawLine(offset, graph_baseline - height, offset + signal_width, graph_baseline + 1, color);
      gfx->startWrite();
      gfx->drawEllipseHelper(offset, graph_baseline + 1, signal_width, height, 0b0011, color);
      gfx->endWrite();

      if (i == peak_id_list[idx])
      {
        // Print SSID, signal strengh and if not encrypted
        String ssid = WiFi.SSID(i);
        if (ssid.length() == 0)
        {
          ssid = WiFi.BSSIDstr(i);
        }
        text_width = (ssid.length() + 6) * 6;
        if (text_width > w)
        {
          offset = 0;
        }
        else
        {
          if ((offset + text_width) > w)
          {
            offset = w - text_width;
          }
        }
        gfx->setTextColor(color);
        gfx->setCursor(offset, graph_baseline - 10 - height);
        gfx->print(ssid);
        gfx->print('(');
        gfx->print(rssi);
        gfx->print(')');
        if (WiFi.encryptionType(i) == WIFI_AUTH_OPEN)
        {
          gfx->print('*');
        }
      }
    }
  }

  // print WiFi found
  gfx->setTextColor(WHITE);
  gfx->setCursor(2, banner_height);
  gfx->print(n);
  gfx->print(" networks");

  // draw 2.4 GHz graph base axle
  gfx->drawFastHLine(0, graph24_baseline, 320, WHITE);
  for (idx = 0; idx < 14; idx++)
  {
    channel = channel_legend[idx];
    offset = (idx + 2) * channel24_width;
    if (channel > 0)
    {
      gfx->setTextColor(channel_color[idx]);
      gfx->setCursor(offset - ((channel < 10) ? 3 : 6), graph24_baseline + 2);
      gfx->print(channel);
    }
    if (ap_count_list[idx] > 0)
    {
      gfx->setTextColor(DARKGREY);
      gfx->setCursor(offset - ((ap_count_list[idx] < 10) ? 3 : 6), graph24_baseline + 8 + 2);
      gfx->print(ap_count_list[idx]);
    }
  }

  // draw 5 GHz graph base axle
  gfx->drawFastHLine(0, graph50_baseline, 320, WHITE);
  for (idx = 14; idx < 71; idx++)
  {
    channel = channel_legend[idx];
    offset = (idx - 14 + 2) * channel50_width;
    if (channel > 0)
    {
      gfx->setTextColor(channel_color[idx]);
      gfx->setCursor(offset - ((channel < 100) ? 6 : 9), graph50_baseline + 2);
      gfx->print(channel);
    }
    if (ap_count_list[idx] > 0)
    {
      gfx->setTextColor(DARKGREY);
      gfx->setCursor(offset - ((ap_count_list[idx] < 10) ? 3 : 6), graph50_baseline + 8 + 2);
      gfx->print(ap_count_list[idx]);
    }
  }

  // Wait a bit before scanning again
  delay(SCAN_INTERVAL);

#if defined(SCAN_COUNT_SLEEP)
  //POWER SAVING
  if (++scan_count >= SCAN_COUNT_SLEEP)
  {
#if defined(LCD_PWR_PIN)
    pinMode(LCD_PWR_PIN, INPUT); // disable pin
#endif
#if defined(GFX_BL)
    pinMode(GFX_BL, INPUT); // disable pin
#endif
  }
#endif // defined(SCAN_COUNT_SLEEP)
}
