#include <Arduino.h>
#include <FS.h>
#include <WiFi.h>

#include "commstructs.h"
#include "newproto.h"
#include "storage.h"
#include "tag_db.h"

#ifdef YELLOW_IPS_AP

#include "ips_display.h"

#define YELLOW_SENSE 8  // sense AP hardware
#define TFT_BACKLIGHT 14

TFT_eSPI tft2 = TFT_eSPI();
int32_t tftid = -1;
uint8_t YellowSense = 0;
bool tftLogscreen = true;

void TFTLog(String text) {
    if (tftLogscreen == false) {
        tft2.fillScreen(TFT_BLACK);
        tft2.setCursor(0, 5, 2);
        tftLogscreen = true;
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
        text = text.substring(httpIndex);
    } else if (text.indexOf(":") != -1) {
        int colonIndex = text.indexOf(":");
        tft2.setTextColor(TFT_SILVER);
        tft2.print(text.substring(0, colonIndex + 1));
        tft2.setTextColor(TFT_WHITE);
        text = text.substring(colonIndex + 1);
    } else if (text.endsWith("!")) {
        tft2.setTextColor(TFT_GREEN);
    }
    tft2.println(text);
}

int32_t findId(uint8_t mac[8]) {
    for (uint32_t c = 0; c < tagDB.size(); c++) {
        tagRecord* tag = tagDB.at(c);
        if (memcmp(tag->mac, mac, 8) == 0) {
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
    eadr.adr.hwType = 0xE0;
    eadr.adr.wakeupReason = wakeupReason;
    eadr.adr.capabilities = 0;
    eadr.adr.tagSoftwareVersion = 0;
    eadr.adr.customMode = 0;
    processDataReq(&eadr, true);
    if (wakeupReason) tftid = findId(eadr.src);
}

void yellow_ap_display_init(void) {

    pinMode(YELLOW_SENSE, INPUT_PULLDOWN);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    if (digitalRead(YELLOW_SENSE) == HIGH) YellowSense = 1;

    pinMode(TFT_BACKLIGHT, OUTPUT);
    digitalWrite(TFT_BACKLIGHT, HIGH);

    ledcSetup(6, 5000, 8);
    ledcAttachPin(TFT_BACKLIGHT, 6);
    ledcWrite(6, config.tft);

    tft2.init();
    tft2.setRotation(YellowSense == 1 ? 1 : 3);

    tft2.fillScreen(TFT_BLACK);
    tft2.setCursor(10, 5, 2);
    tft2.setTextColor(TFT_WHITE);
    tft2.println("*** Initialising... ***");
    tftLogscreen = true;
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
    if (millis() - last_update >= 1000) {
        tagRecord* tag = tagDB.at(tftid);
        if (tag->pending) {
            String filename = tag->filename;
            fs::File file = contentFS->open(filename);
            if (!file) {
                Serial.print("No current file. Canceling request\n");
                prepareCancelPending(tag->mac);
                return;
            }

            TFT_eSprite spr = TFT_eSprite(&tft2);
            if (tag->len == tft2.width() * tft2.height() * 2) spr.setColorDepth(16);
            if (tag->len == tft2.width() * tft2.height()) spr.setColorDepth(8);
            spr.createSprite(tft2.width(), tft2.height());
            void* spriteData = spr.getPointer();
            size_t bytesRead = file.readBytes((char*)spriteData, spr.width() * spr.height() * 2);
            file.close();
            spr.pushSprite(0, 0);
            tftLogscreen = false;

            struct espXferComplete xfc = {0};
            memcpy(xfc.src, tag->mac, 8);
            processXferComplete(&xfc, true);
        }
        last_update = millis();
    }
}

#endif