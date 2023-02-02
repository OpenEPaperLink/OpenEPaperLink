#include "contentmanager.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "newproto.h"
#include <MD5Builder.h>
#include <locale.h>
#include <time.h>

#include "commstructs.h"
#include "makeimage.h"
#include "web.h"

void contentRunner() {
    time_t now;
    time(&now);

    for (int16_t c = 0; c < tagDB.size(); c++) {
        tagRecord* taginfo = nullptr;
        taginfo = tagDB.at(c);

        if (now >= taginfo->nextupdate || taginfo->button) {
            uint8_t mac8[8] = {0, 0, 0, 0, 0, 0, 0, 0};
            memcpy(mac8 + 2, taginfo->mac, 6);
            uint8_t src[8];
            *((uint64_t *)src) = swap64(*((uint64_t *)mac8));

            drawNew(src, taginfo->button, taginfo);
            taginfo->button = false;
        }
    }
}

void drawNew(uint8_t mac[8], bool buttonPressed, tagRecord *&taginfo) {
    time_t now;
    time(&now);

    char buffer[64];
    uint8_t src[8];
    *((uint64_t *)src) = swap64(*((uint64_t *)mac));
    sprintf(buffer, "%02X%02X%02X%02X%02X%02X\0", src[2], src[3], src[4], src[5], src[6], src[7]);
    String dst = (String)buffer;

    String filename = "/" + dst + ".bmp";

    struct tm time_info;
    getLocalTime(&time_info);
    time_info.tm_hour = 0;
    time_info.tm_min = 0;
    time_info.tm_sec = 0;
    time_info.tm_mday++;
    time_t midnight = mktime(&time_info);

    DynamicJsonDocument doc(500);
    deserializeJson(doc, taginfo->modeConfigJson);
    JsonObject cfgobj = doc.as<JsonObject>();

    Serial.println("Updating " + dst + " mode " + String(taginfo->contentMode) + " nextupdate " + String(taginfo->nextupdate));
    taginfo->nextupdate = now + 600;

    switch (taginfo->contentMode) {
        case Image:

            filename = cfgobj["filename"].as<String>();
            if (filename && filename !="null" && !cfgobj["#fetched"].as<bool>()) {
                if (prepareDataAvail(&filename, DATATYPE_IMGRAW, mac, cfgobj["timetolive"].as<int>())) {
                    cfgobj["#fetched"] = true;
                } else {
                    wsString("Error accessing " + filename);
                }
                taginfo->nextupdate = 3216153600;
            }
            break;

        case Today:

            Serial.println("heap voor drawDate: " + String(ESP.getFreeHeap()));
            drawDate(filename);
            Serial.println("heap na drawDate: " + String(ESP.getFreeHeap()));
            // updateTagImage(filename, mac, (midnight - now) / 60 - 10);
            updateTagImage(filename, mac, 60);
            taginfo->nextupdate = midnight;
            break;

        case CountDays:

            if (buttonPressed) cfgobj["counter"] = 0;
            Serial.println("heap voor drawnumber: " + String(ESP.getFreeHeap()));
            drawNumber(filename, (int32_t)cfgobj["counter"], (int32_t)cfgobj["thresholdred"]);
            Serial.println("heap na drawnumber: " + String(ESP.getFreeHeap()));
            updateTagImage(filename, mac, 60);
            cfgobj["counter"] = (int32_t)cfgobj["counter"] + 1;
            taginfo->nextupdate = midnight;
            break;

        case CountHours:

            if (buttonPressed) cfgobj["counter"] = 0;
            Serial.println("heap voor drawnumber: " + String(ESP.getFreeHeap()));
            drawNumber(filename, (int32_t)cfgobj["counter"], (int32_t)cfgobj["thresholdred"]);
            Serial.println("heap na drawnumber: " + String(ESP.getFreeHeap()));
            // updateTagImage(&filename, mac, (3600 - now % 3600) / 60);
            // taginfo->nextupdate = now + 3600 - (now % 3600);
            updateTagImage(filename, mac, 3);
            cfgobj["counter"] = (int32_t)cfgobj["counter"] + 1;
            taginfo->nextupdate = now + 300;
            break;

        case Weather:

            // https://open-meteo.com/
            break;

        case Firmware:

            filename = cfgobj["filename"].as<String>();
            if (filename && filename != "null" && !cfgobj["#fetched"].as<bool>()) {
                if (prepareDataAvail(&filename, DATATYPE_UPDATE, mac, cfgobj["timetolive"].as<int>())) {
                    cfgobj["#fetched"] = true;
                } else {
                    wsString("Error accessing " + filename);
                }
                cfgobj["filename"]="";
                taginfo->nextupdate = 3216153600;
                taginfo->contentMode = Image;
            } else {
                taginfo->nextupdate = now + 300;
            }
            break;

        case Memo:
            break;
        case ImageUrl:

            if (getImgURL(filename, cfgobj["url"], (time_t)cfgobj["#fetched"])) {
                updateTagImage(filename, mac, cfgobj["interval"].as<int>());
                cfgobj["#fetched"] = now;
            }
            taginfo->nextupdate = now + 60 * (cfgobj["interval"].as<int>() < 5 ? 5 : cfgobj["interval"].as<int>()) ;
            break;
    }

    taginfo->modeConfigJson = doc.as<String>();
}

bool updateTagImage(String &filename, uint8_t *dst, uint16_t nextCheckin) {
    prepareDataAvail(&filename, DATATYPE_IMGRAW, dst, nextCheckin);
    return true;
}

void drawDate(String &filename) {

    TFT_eSPI tft = TFT_eSPI();
    TFT_eSprite spr = TFT_eSprite(&tft);
    time_t now;
    time(&now);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    String Dag[] = {"zondag","maandag","dinsdag", "woensdag", "donderdag", "vrijdag", "zaterdag"};
    String Maand[] = {"januari", "februari", "maart", "april", "mei", "juni","juli", "augustus", "september", "oktober", "november", "december"};
    int weekday_number = timeinfo.tm_wday;
    int month_number = timeinfo.tm_mon;

    LittleFS.begin();
    long w = 296, h = 128;  // mag staand of liggend
    spr.createSprite(w, h);
    if (spr.getPointer() == nullptr) {
        Serial.println("Failed to create sprite in drawDate");
    }
    spr.setColorDepth(8);
    spr.fillSprite(TFT_WHITE);
    spr.setTextDatum(TC_DATUM);
    spr.loadFont("calibrib62", LittleFS);
    spr.setTextColor(TFT_RED, TFT_WHITE);
    spr.drawString(Dag[timeinfo.tm_wday], w / 2, 10);
    spr.loadFont("calibrib50", LittleFS);
    spr.setTextColor(TFT_BLACK, TFT_WHITE);
    spr.drawString(String(timeinfo.tm_mday) + " " + Maand[timeinfo.tm_mon], w / 2, 73);
    spr.unloadFont();

    spr2grays(spr, w, h, filename);

    spr.deleteSprite();
}

void drawNumber(String &filename, int32_t count, int32_t thresholdred) {
    TFT_eSPI tft = TFT_eSPI();
    TFT_eSprite spr = TFT_eSprite(&tft);

    LittleFS.begin();
    long w = 296, h = 128;
    spr.createSprite(w, h);
    if (spr.getPointer() == nullptr) {
        Serial.println("Failed to create sprite in drawNumber");
    }
    spr.setColorDepth(8);
    spr.fillSprite(TFT_WHITE);
    spr.setTextDatum(MC_DATUM);
    if (count > thresholdred) {
        spr.setTextColor(TFT_RED, TFT_WHITE);
    } else {
        spr.setTextColor(TFT_BLACK, TFT_WHITE);
    }
    String font = "numbers1-2";
    if (count>999) font="numbers2-2";
    if (count>9999) font="numbers3-2";
    spr.loadFont(font, LittleFS);
    spr.drawString(String(count), w/2, h/2+10);
    spr.unloadFont();

    spr2grays(spr, w, h, filename);

    spr.deleteSprite();
}

bool getImgURL(String &filename, String URL, time_t fetched) {
    // https://images.klari.net/kat-bw29.jpg

    LittleFS.begin();
 
    Serial.println("get external " + URL);
    HTTPClient http;
    http.begin(URL);
    http.addHeader("If-Modified-Since", formatHttpDate(fetched));
    http.setTimeout(5000);  //timeout in ms
    int httpCode = http.GET();
    if (httpCode == 200) {
        File f = LittleFS.open(filename, "w");
        if (f) {
            http.writeToStream(&f);
            f.close();
            jpg2grays(filename, filename);
        }
    } else {
        Serial.println("http " + String(httpCode));
    }
    http.end();
    return (httpCode == 200);
}

char *formatHttpDate(time_t t) {
    static char buf[40];
    struct tm *timeinfo;
    timeinfo = gmtime(&t);
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
    return buf;
}
