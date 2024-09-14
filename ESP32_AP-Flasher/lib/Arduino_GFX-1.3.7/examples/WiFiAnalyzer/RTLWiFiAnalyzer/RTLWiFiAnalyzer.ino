/*******************************************************************************
 * Rtl WiFi Analyzer
 * For RTL872x only.
 * 
 * Add realtek ameba core support to Arduino IDE:
 * https://github.com/ambiot/ambd_arduino
 * 
 * Old patch realtek ameba core variant.cpp to RTL8720DN pinout:
 * https://github.com/mikey60/BW16-RTL8720DN-Module-Arduino
 * 
 * Defalult pin list for non display dev kit:
 * RTL8720 BW16 old patch core : CS: 18, DC: 17, RST:  2, BL: 23
 * RTL8720_BW16 Official core  : CS:  9, DC:  8, RST:  6, BL:  3
 * RTL8722 dev board           : CS: 18, DC: 17, RST: 22, BL: 23
 * RTL8722_mini dev board      : CS: 12, DC: 14, RST: 15, BL: 13
 ******************************************************************************/

#define SCAN_INTERVAL 3000

#include <lwip_netconf.h>
#include <wifi_conf.h>
#include <wifi_constants.h>
#include <wifi_structures.h>
#include <wl_definitions.h>
#include <wl_types.h>

#include <Arduino_GFX_Library.h>

#define GFX_BL DF_GFX_BL // default backlight pin, you may replace DF_GFX_BL to actual backlight pin

Arduino_DataBus *bus = create_default_Arduino_DataBus();
/* More display class: https://github.com/moononournation/Arduino_GFX/wiki/Display-Class */
Arduino_GFX *gfx = new Arduino_ILI9341(bus, DF_GFX_RST, 3 /* rotation */, false /* IPS */);

static int16_t w, h, text_size, banner_height, graph24_baseline, graph50_baseline, graph_baseline, graph_height, channel24_width, channel50_width, channel_width, signal_width;

// RSSI RANGE
#define RSSI_CEILING -40
#define RSSI_FLOOR -100

// Channel legend mapping
static uint16_t channel_legend[] = {
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
static uint16_t channel_color[] = {
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

static uint16_t channelIdx(int channel)
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
  if (channel <= 144) // channel 98 - 144
  {
    return 34 + ((channel - 100) / 2);
  }
  // channel 149 - 177
  return 58 + ((channel - 149) / 2);
}

static uint8_t _networkCount;
static char _networkSsid[WL_NETWORKS_LIST_MAXNUM][WL_SSID_MAX_LENGTH];
static int32_t _networkRssi[WL_NETWORKS_LIST_MAXNUM];
static uint32_t _networkEncr[WL_NETWORKS_LIST_MAXNUM];
static uint8_t _networkChannel[WL_NETWORKS_LIST_MAXNUM];
static char _networkMac[WL_NETWORKS_LIST_MAXNUM][18];

static rtw_result_t wifidrv_scan_result_handler(rtw_scan_handler_result_t *malloced_scan_result)
{
  rtw_scan_result_t *record;

  if (malloced_scan_result->scan_complete != RTW_TRUE)
  {
    record = &malloced_scan_result->ap_details;
    record->SSID.val[record->SSID.len] = 0; /* Ensure the SSID is null terminated */

    if (_networkCount < WL_NETWORKS_LIST_MAXNUM)
    {
      strcpy(_networkSsid[_networkCount], (char *)record->SSID.val);
      _networkRssi[_networkCount] = record->signal_strength;
      _networkEncr[_networkCount] = record->security;
      _networkChannel[_networkCount] = record->channel;
      sprintf(_networkMac[_networkCount], "%02X:%02X:%02X:%02X:%02X:%02X",
        record->BSSID.octet[0], record->BSSID.octet[1], record->BSSID.octet[2],
        record->BSSID.octet[3], record->BSSID.octet[4], record->BSSID.octet[5]);

      _networkCount++;
    }
  }

  return RTW_SUCCESS;
}

static int8_t scanNetworks()
{
  uint8_t attempts = 10;

  _networkCount = 0;
  if (wifi_scan_networks(wifidrv_scan_result_handler, NULL) != RTW_SUCCESS)
  {
    return WL_FAILURE;
  }

  do
  {
    delay(SCAN_INTERVAL);
  } while ((_networkCount == 0) && (--attempts > 0));
  return _networkCount;
}

void setup()
{
  LwIP_Init();
  wifi_on(RTW_MODE_STA);

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

  // direct draw banner to output display
  gfx->setTextSize(text_size);
  gfx->fillScreen(BLACK);
  gfx->setTextColor(GREEN);
  gfx->setCursor(2, 2);
  gfx->print("RTL");
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

  int n = scanNetworks();

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
      channel = _networkChannel[i];
      idx = channelIdx(channel);
      rssi = _networkRssi[i];

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
      channel = _networkChannel[i];
      idx = channelIdx(channel);
      rssi = _networkRssi[i];
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
      gfx->startWrite();
      gfx->drawEllipseHelper(offset, graph_baseline + 1, signal_width, height, 0b0011, color);
      gfx->endWrite();

      if (i == peak_id_list[idx])
      {
        // Print SSID, signal strengh and if not encrypted
        String ssid = _networkSsid[i];
        if (ssid.length() == 0)
        {
          ssid = _networkMac[i];
        }
        text_width = (ssid.length() + 6) * 6;
        if (text_width > w)
        {
          offset = 0;
        }
        else
        {
          offset -= signal_width;
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
        if (_networkEncr[i] == RTW_SECURITY_OPEN)
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
      gfx->setTextColor(LIGHTGREY);
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
}
