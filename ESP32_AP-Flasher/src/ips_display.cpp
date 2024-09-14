#include <Arduino.h>
#include <FS.h>
#include <WiFi.h>

#include "commstructs.h"
#include "newproto.h"
#include "storage.h"
#include "tag_db.h"

#ifdef HAS_TFT

#include "ips_display.h"

#define YELLOW_SENSE 8  // sense AP hardware
#define TFT_BACKLIGHT 14

TFT_eSPI tft2 = TFT_eSPI();
uint8_t YellowSense = 0;
bool tftLogscreen = true;
bool tftOverride = false;

#ifdef HAS_LILYGO_TPANEL

Arduino_DataBus *bus = new Arduino_XL9535SWSPI(IIC_SDA /* SDA */, IIC_SCL /* SCL */, -1 /* XL PWD */,
                                               XL95X5_CS /* XL CS */, XL95X5_SCLK /* XL SCK */, XL95X5_MOSI /* XL MOSI */);
Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
  -1 /* DE */, LCD_VSYNC /* VSYNC */, LCD_HSYNC /* HSYNC */, LCD_PCLK /* PCLK */,
  LCD_B0 /* B0 */, LCD_B1 /* B1 */, LCD_B2 /* B2 */, LCD_B3 /* B3 */, LCD_B4 /* B4 */,
  LCD_G0 /* G0 */, LCD_G1 /* G1 */, LCD_G2 /* G2 */, LCD_G3 /* G3 */, LCD_G4 /* G4 */, LCD_G5 /* G5 */,
  LCD_R0 /* R0 */, LCD_R1 /* R1 */, LCD_R2 /* R2 */, LCD_R3 /* R3 */, LCD_R4 /* R4 */,
  1 /* hsync_polarity */, 20 /* hsync_front_porch */, 2 /* hsync_pulse_width */, 0 /* hsync_back_porch */,
  1 /* vsync_polarity */, 30 /* vsync_front_porch */, 8 /* vsync_pulse_width */, 1 /* vsync_back_porch */,
  10 /* pclk_active_neg */, 6000000L /* prefer_speed */, false /* useBigEndian */,
  0 /* de_idle_high*/, 0 /* pclk_idle_high */);
Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
  LCD_WIDTH /* width */, LCD_HEIGHT /* height */, rgbpanel, 0 /* rotation */, true /* auto_flush */,
  bus, -1 /* RST */, st7701_type9_init_operations, sizeof(st7701_type9_init_operations));

#endif

void TFTLog(String text) {
	
    #ifdef HAS_LILYGO_TPANEL

	gfx->setTextSize(2);
 
    if (tftLogscreen == false) {
        gfx->fillScreen(BLACK);
        gfx->setCursor(0, 0);
        tftLogscreen = true;
    } 
    if (text.isEmpty()) return;
    gfx->setTextColor(LIGHTGREY);
    if (text.startsWith("!")) {
        gfx->setTextColor(RED);
        text = text.substring(1);
    } else if (text.indexOf("http") != -1) {
        int httpIndex = text.indexOf("http");
        gfx->print(text.substring(0, httpIndex));
        gfx->setTextColor(YELLOW);
        text = text.substring(httpIndex);
    } else if (text.indexOf(":") != -1) {
        int colonIndex = text.indexOf(":");
        gfx->setTextColor(LIGHTGREY);
        gfx->print(text.substring(0, colonIndex + 1));
        gfx->setTextColor(WHITE);
        text = text.substring(colonIndex + 1);
    } else if (text.endsWith("!")) {
        gfx->setTextColor(GREEN);
    }
    gfx->println(text);
	
	#else
	
    if (tftLogscreen == false) {
        tft2.fillScreen(TFT_BLACK);
        tft2.setCursor(0, 0, (tft2.width() == 160 ? 1 : 2));
        tftLogscreen = true;
    } else {
        if (tft2.width() == 160) tft2.setCursor(0, tft2.getCursorY(), 1);
    }
    if (text.isEmpty()) return;
    tft2.setTextColor(TFT_SILVER);
    if (text.startsWith("!")) {
        tft2.setTextColor(TFT_RED);
        text = text.substring(1);
    } else if (text.indexOf("http") != -1) {
        int httpIndex = text.indexOf("http");
        tft2.print(text.substring(0, httpIndex));
        tft2.setTextColor(TFT_YELLOW);
        if (tft2.width() == 160) {
            tft2.setCursor(0, tft2.getCursorY() + 8, 2);
            text = text.substring(httpIndex + 7);
        } else {
            text = text.substring(httpIndex);
        }
    } else if (text.indexOf(":") != -1) {
        int colonIndex = text.indexOf(":");
        tft2.setTextColor(TFT_SILVER);
        tft2.print(text.substring(0, colonIndex + 1));
        tft2.setTextColor(TFT_WHITE);
        if (tft2.width() == 160) tft2.setCursor(0, tft2.getCursorY() + 8, 2);
        text = text.substring(colonIndex + 1);
    } else if (text.endsWith("!")) {
        tft2.setTextColor(TFT_GREEN);
    }
    tft2.println(text);
    #endif
}

int32_t findId(uint8_t mac[8]) {
    for (uint32_t c = 0; c < tagDB.size(); c++) {
        tagRecord* tag = tagDB.at(c);
        if (memcmp(tag->mac, mac, 8) == 0 && tag->version == 0) {
            return c;
        }
    }
    return -1;
}

void sendAvail(uint8_t wakeupReason) {
    struct espAvailDataReq eadr = {0};
    uint8_t mac[6];
    WiFi.macAddress(mac);
    memcpy(&eadr.src, mac, 6);
    eadr.adr.lastPacketRSSI = WiFi.RSSI();
    eadr.adr.currentChannel = config.channel;
    #ifdef HAS_LILYGO_TPANEL
    eadr.adr.hwType = 0xE2;
	#else
    eadr.adr.hwType = (tft2.width() == 160 ? 0xE1 : 0xE0);
    #endif
    eadr.adr.wakeupReason = wakeupReason;
    eadr.adr.capabilities = 0;
    eadr.adr.tagSoftwareVersion = 0;
    eadr.adr.customMode = 0;
    processDataReq(&eadr, true);
}

void yellow_ap_display_init(void) {
	
    #ifdef HAS_LILYGO_TPANEL
    
  	tftLogscreen = true;

    pinMode(LCD_BL, OUTPUT);
    digitalWrite(LCD_BL, HIGH);

    Wire.begin(IIC_SDA, IIC_SCL);

    gfx->begin();
    gfx->fillScreen(BLACK);
    
	#else
    pinMode(YELLOW_SENSE, INPUT_PULLDOWN);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    if (digitalRead(YELLOW_SENSE) == HIGH) YellowSense = 1;
    pinMode(TFT_BACKLIGHT, OUTPUT);
    digitalWrite(TFT_BACKLIGHT, LOW);

    tft2.init();
    #ifdef ST7735_NANO_TLSR
    YellowSense = 0;
    tft2.setRotation(1);
    #else
    tft2.setRotation(YellowSense == 1 ? 1 : 3);
    #endif
    tft2.fillScreen(TFT_BLACK);
    tft2.setCursor(12, 0, (tft2.width() == 160 ? 1 : 2));
    tft2.setTextColor(TFT_WHITE);
    tftLogscreen = true;

    ledcSetup(6, 5000, 8);
    ledcAttachPin(TFT_BACKLIGHT, 6);
    if (tft2.width() == 160) {
        GPIO.func_out_sel_cfg[TFT_BACKLIGHT].inv_sel = 1;
    }
    ledcWrite(6, config.tft);
    #endif
}

void yellow_ap_display_loop(void) {
    static bool first_run = 0;
    static time_t last_checkin = 0;
    static time_t last_update = 0;

    if (millis() - last_checkin >= 60000) {
        sendAvail(0);
        last_checkin = millis();
        tftLogscreen = false;
    }
    if (first_run == 0) {
        sendAvail(0xFC);
        first_run = 1;
    }
    if (millis() - last_update >= 3000) {
        uint8_t wifimac[8];
        WiFi.macAddress(wifimac);
        memset(&wifimac[6], 0, 2);
        tagRecord* tag = tagRecord::findByMAC(wifimac);
        if (tag == nullptr) {
            last_update = millis();
            return;
        }
        if (tag->pendingCount > 0 && tftOverride == false) {
            String filename = tag->filename;
            fs::File file = contentFS->open(filename);
            if (!file) {
                Serial.print("No current file. Canceling request\r\n");
                prepareCancelPending(tag->mac);
                return;
            }

            TFT_eSprite spr = TFT_eSprite(&tft2);
            spr.setColorDepth(16);
            if (tag->len == tft2.width() * tft2.height()) spr.setColorDepth(8);
            spr.createSprite(tft2.width(), tft2.height());
            void* spriteData = spr.getPointer();
            size_t bytesRead = file.readBytes((char*)spriteData, spr.width() * spr.height() * 2);
            file.close();
            
            #ifdef HAS_LILYGO_TPANEL
 			long dy = spr.height();
			long dx = spr.width();
			
            uint16_t* data = static_cast<uint16_t*>(const_cast<void*>(spriteData));
			
		    for (int16_t j = 0; j < dy; j++)
		    {
		      for (int16_t i = 0; i < dx; i++)
		      {
		        uint16_t color = *data;
		        color = color<<8 | color>>8;
                *data = color;
		        data++;
		      }
		    }
			gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)spriteData, dx, dy);
			#else            
            spr.pushSprite(0, 0);
            #endif
            
            tftLogscreen = false;

            struct espXferComplete xfc = {0};
            memcpy(xfc.src, tag->mac, 8);
            processXferComplete(&xfc, true);
        }
        last_update = millis();
    }
}

#endif