#ifdef YELLOW_IPS_AP
#include <Arduino.h>
#include <FS.h>
#include <TFT_eSPI.h>
#include <WiFi.h>

#include "commstructs.h"
#include "newproto.h"
#include "storage.h"
#include "tag_db.h"

TFT_eSPI tft2 = TFT_eSPI();
bool first_run = 0;
time_t last_update = 0;
time_t last_checkin = 0;
int32_t tftid = -1;

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
    eadr.adr.currentChannel = WiFi.channel();
    eadr.adr.hwType = 0xE0;
    eadr.adr.wakeupReason = wakeupReason;
    eadr.adr.capabilities = 0;
    eadr.adr.tagSoftwareVersion = 0;
    eadr.adr.customMode = 0;
    processDataReq(&eadr, true);
    if (wakeupReason) tftid = findId(eadr.src);
}

void yellow_ap_display_init(void) {
    tft2.init();
    tft2.setRotation(3);

    tft2.fillScreen(TFT_BLACK);
    tft2.setCursor(0, 0, 2);
    tft2.setTextColor(TFT_WHITE);
    tft2.println(" Init\n");
}

void yellow_ap_display_loop(void) {
    if (millis() - last_checkin >= 60000) {
        sendAvail(0);
        last_checkin = millis();
    }
    if (millis() - last_update >= 1000) {
        if (first_run == 0) {
            sendAvail(0xFC);
            first_run = 1;
        }

        // if ((uint32_t)WiFi.localIP() == (uint32_t)0) {}

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

            struct espXferComplete xfc = {0};
            memcpy(xfc.src, tag->mac, 8);
            processXferComplete(&xfc, true);
        }
        last_update = millis();
    }
}

#endif