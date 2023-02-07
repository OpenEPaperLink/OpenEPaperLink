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

        if (taginfo->RSSI && (now >= taginfo->nextupdate || taginfo->wakeupReason == WAKEUP_REASON_GPIO)) {
            uint8_t mac8[8] = {0, 0, 0, 0, 0, 0, 0, 0};
            memcpy(mac8 + 2, taginfo->mac, 6);
            uint8_t src[8];
            *((uint64_t *)src) = swap64(*((uint64_t *)mac8));

            drawNew(src, (taginfo->wakeupReason == WAKEUP_REASON_GPIO), taginfo);
            taginfo->wakeupReason = 0;
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

    wsLog("Updating " + dst);
    taginfo->nextupdate = now + 600;
    
    switch (taginfo->contentMode) {
        case Image:

            filename = cfgobj["filename"].as<String>();
            if (filename && filename !="null" && !cfgobj["#fetched"].as<bool>()) {
                if (prepareDataAvail(&filename, DATATYPE_IMGRAW, mac, cfgobj["timetolive"].as<int>())) {
                    cfgobj["#fetched"] = true;
                } else {
                    wsErr("Error accessing " + filename);
                }
                taginfo->nextupdate = 3216153600;
            }
            break;

        case Today:

            drawDate(filename, taginfo);
            // updateTagImage(filename, mac, (midnight - now) / 60 - 10);
            updateTagImage(filename, mac, 600);
            taginfo->nextupdate = midnight;
            break;

        case CountDays:

            if (buttonPressed) cfgobj["counter"] = 0;
            drawNumber(filename, (int32_t)cfgobj["counter"], (int32_t)cfgobj["thresholdred"], taginfo);
            updateTagImage(filename, mac, (buttonPressed?0:600));
            cfgobj["counter"] = (int32_t)cfgobj["counter"] + 1;
            taginfo->nextupdate = midnight;
            break;

        case CountHours:

            if (buttonPressed) cfgobj["counter"] = 0;
            drawNumber(filename, (int32_t)cfgobj["counter"], (int32_t)cfgobj["thresholdred"], taginfo);
            // updateTagImage(&filename, mac, (3600 - now % 3600) / 60);
            // taginfo->nextupdate = now + 3600 - (now % 3600);
            updateTagImage(filename, mac, (buttonPressed?0:600));
            cfgobj["counter"] = (int32_t)cfgobj["counter"] + 1;
            taginfo->nextupdate = now + 3600;
            break;

        case Weather:

            // https://open-meteo.com/
            // https://geocoding-api.open-meteo.com/v1/search?name=eindhoven
            // https://api.open-meteo.com/v1/forecast?latitude=52.52&longitude=13.41&current_weather=true
            // https://github.com/erikflowers/weather-icons

            drawWeather(filename, cfgobj["location"], taginfo);
            updateTagImage(filename, mac, 600);
            taginfo->nextupdate = now + 3600;
            break;

        case Firmware:

            filename = cfgobj["filename"].as<String>();
            if (filename && filename != "null" && !cfgobj["#fetched"].as<bool>()) {
                if (prepareDataAvail(&filename, DATATYPE_UPDATE, mac, cfgobj["timetolive"].as<int>())) {
                    cfgobj["#fetched"] = true;
                } else {
                    wsErr("Error accessing " + filename);
                }
                cfgobj["filename"]="";
                taginfo->nextupdate = 3216153600;
                taginfo->contentMode = Image;
            } else {
                taginfo->nextupdate = now + 300;
            }
            break;

        case Memo:

            drawIdentify(filename, taginfo);
            updateTagImage(filename, mac, 0);
            taginfo->nextupdate = now + 24*3600;
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

void drawString(TFT_eSprite &spr, String content, uint16_t posx, uint16_t posy, String font, byte align,uint16_t color) {
    // drawString(spr,"test",100,10,"bahnschrift30",TC_DATUM,TFT_RED);
    spr.setTextDatum(align);
    spr.loadFont(font, LittleFS);
    spr.setTextColor(color, TFT_WHITE);
    spr.drawString(content, posx, posy);
    spr.unloadFont();
}

void initSprite(TFT_eSprite &spr, int w, int h) {
    spr.setColorDepth(8);
    spr.createSprite(w, h);
    if (spr.getPointer() == nullptr) {
        wsErr("Failed to create sprite in drawNumber");
    }
    spr.fillSprite(TFT_WHITE);
}

void drawDate(String &filename, tagRecord *&taginfo) {

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

    if (taginfo->hwType == SOLUM_29_033) {

        initSprite(spr,296,128);
        drawString(spr, Dag[timeinfo.tm_wday], 296 / 2, 10, "fonts/calibrib62", TC_DATUM, TFT_RED);
        drawString(spr, String(timeinfo.tm_mday) + " " + Maand[timeinfo.tm_mon], 296 / 2, 73, "fonts/calibrib50", TC_DATUM);

    } else if (taginfo->hwType == SOLUM_154_033) {

        initSprite(spr, 154, 154);
        drawString(spr, Dag[timeinfo.tm_wday], 154 / 2, 10, "fonts/calibrib30", TC_DATUM);
        drawString(spr, String(Maand[timeinfo.tm_mon]), 154 / 2, 120, "fonts/calibrib30", TC_DATUM);
        drawString(spr, String(timeinfo.tm_mday), 154 / 2, 42, "fonts/numbers2-1", TC_DATUM, TFT_RED);
    }

    spr2grays(spr, spr.width(), spr.height(), filename);
    spr.deleteSprite();
}

void drawNumber(String &filename, int32_t count, int32_t thresholdred, tagRecord *&taginfo) {

    TFT_eSPI tft = TFT_eSPI();
    TFT_eSprite spr = TFT_eSprite(&tft);
    LittleFS.begin();

    if (taginfo->hwType == SOLUM_29_033) {

        initSprite(spr, 296, 128);
        spr.setTextDatum(MC_DATUM);
        if (count > thresholdred) {
            spr.setTextColor(TFT_RED, TFT_WHITE);
        } else {
            spr.setTextColor(TFT_BLACK, TFT_WHITE);
        }
        String font = "fonts/numbers1-2";
        if (count > 999) font = "fonts/numbers2-2";
        if (count > 9999) font = "fonts/numbers3-2";
        spr.loadFont(font, LittleFS);
        spr.drawString(String(count), 296 / 2, 128 / 2 + 10);
        spr.unloadFont();

    } else if (taginfo->hwType == SOLUM_154_033) {

        initSprite(spr, 154, 154);
        spr.setTextDatum(MC_DATUM);
        if (count > thresholdred) {
            spr.setTextColor(TFT_RED, TFT_WHITE);
        } else {
            spr.setTextColor(TFT_BLACK, TFT_WHITE);
        }
        String font = "fonts/numbers1-1";
        if (count > 99) font = "fonts/numbers2-1";
        if (count > 999) font = "fonts/numbers3-1";
        spr.loadFont(font, LittleFS);
        spr.drawString(String(count), 154 / 2, 154 / 2 + 10);
        spr.unloadFont();

    }

    spr2grays(spr, spr.width(), spr.height(), filename);
    spr.deleteSprite();
}

void drawWeather(String &filename, String location, tagRecord *&taginfo) {

    TFT_eSPI tft = TFT_eSPI();
    TFT_eSprite spr = TFT_eSprite(&tft);

    wsLog("get weather");
    HTTPClient http;
    http.begin("https://geocoding-api.open-meteo.com/v1/search?name=" + urlEncode(location.c_str()) + "&count=1");
    http.setTimeout(5000);  // timeout in ms
    int httpCode = http.GET();
    if (httpCode == 200) {

        StaticJsonDocument<1000> doc;
        DeserializationError error = deserializeJson(doc, http.getStream());
        http.end();

        http.begin("https://api.open-meteo.com/v1/forecast?latitude=" + doc["results"][0]["latitude"].as<String>() + "&longitude=" + doc["results"][0]["longitude"].as<String>() + "&current_weather=true&windspeed_unit=ms&timezone=" + doc["results"][0]["timezone"].as<String>());
        http.setTimeout(5000);  // timeout in ms
        int httpCode = http.GET();

        if (httpCode == 200) {

            StaticJsonDocument<200> filter;
            filter["current_weather"]["temperature"] = true;
            filter["current_weather"]["windspeed"] = true;
            filter["current_weather"]["winddirection"] = true;
            filter["current_weather"]["weathercode"] = true;

            doc.clear();
            DeserializationError error = deserializeJson(doc, http.getString(), DeserializationOption::Filter(filter));
            if (error) {
                Serial.println(F("deserializeJson() failed: "));
                Serial.println(error.c_str());
            }

            auto temperature = doc["current_weather"]["temperature"].as<double>();
            auto windspeed = doc["current_weather"]["windspeed"].as<int>();
            auto winddirection = doc["current_weather"]["winddirection"].as<int>();
            uint8_t weathercode = doc["current_weather"]["weathercode"].as<int>();
            if (weathercode > 40) weathercode -= 40;
            int wind = windSpeedToBeaufort(windspeed);

            String weatherIcons[] = {"\uf00d", "\uf00c", "\uf002", "\uf013", "\uf013", "\uf014", "-", "-", "\uf014", "-", "-",
             "\uf01a", "-", "\uf01a", "-", "\uf01a", "\uf017", "\uf017", "-", "-", "-",
             "\uf019", "-", "\uf019", "-", "\uf019", "\uf015", "\uf015", "-", "-", "-",
             "\uf01b", "-", "\uf01b", "-", "\uf01b", "-", "\uf076", "-", "-", "\uf01a",
             "\uf01a", "\uf01a", "-", "-", "\uf064", "\uf064", "-", "-", "-", "-",
             "-", "-", "-", "-", "\uf01e", "\uf01d", "-", "-", "\uf01e"};
            if (1==0) {   //nacht
                weatherIcons[0] = "\0uf02e";
                weatherIcons[1] = "\0uf083";
                weatherIcons[2] = "\0uf086";
            }

            String windIcons[] = {"\uf0b7", "\uf0b8", "\uf0b9", "\uf0ba", "\uf0bb", "\uf0bc", "\uf0bd", "\uf0be", "\uf0bf", "\uf0c0", "\uf0c1", "\uf0c2", "\uf0c3"};

            doc.clear();

            LittleFS.begin();
            tft.setTextWrap(false,false);

            if (taginfo->hwType == SOLUM_29_033) {

                initSprite(spr, 296, 128);

                drawString(spr, location, 10, 10, "fonts/bahnschrift30");

                char tmpOutput[5];
                dtostrf(temperature, 2, 1, tmpOutput);
                drawString(spr, String(tmpOutput), 5, 65, "fonts/bahnschrift70", TL_DATUM, (temperature < 0 ? TFT_RED : TFT_BLACK));

                spr.loadFont("fonts/weathericons78", LittleFS);
                if (weathercode == 55 || weathercode == 65 || weathercode == 75 || weathercode == 82 || weathercode == 86 || weathercode == 95 || weathercode == 99) {
                    spr.setTextColor(TFT_RED, TFT_WHITE);
                } else {
                    spr.setTextColor(TFT_BLACK, TFT_WHITE);
                }
                spr.setCursor(185, 20);
                spr.printToSprite(weatherIcons[weathercode]);
                spr.unloadFont();

                spr.loadFont("fonts/weathericons30", LittleFS);
                if (wind > 4) {
                    spr.setTextColor(TFT_RED, TFT_WHITE);
                } else {
                    spr.setTextColor(TFT_BLACK, TFT_WHITE);
                }
                spr.setCursor(255, 0);
                spr.printToSprite(windIcons[wind]);

                spr.setTextColor(TFT_BLACK, TFT_WHITE);
                spr.setCursor(230, -5);
                spr.printToSprite(windDirectionIcon(winddirection));
                if (weathercode > 10) {
                    spr.setTextColor(TFT_RED, TFT_WHITE);
                    spr.setCursor(190, 0);
                    spr.printToSprite("\uf084");
                }
                spr.unloadFont();

            } else if (taginfo->hwType == SOLUM_154_033) {

                initSprite(spr, 154, 154);
                spr.setTextDatum(TL_DATUM);

                spr.setTextFont(2);
                spr.setTextColor(TFT_BLACK, TFT_WHITE);
                spr.drawString(location, 10, 130);

                char tmpOutput[5];
                dtostrf(temperature, 2, 1, tmpOutput);
                drawString(spr, String(tmpOutput), 10, 10, "fonts/bahnschrift30", TL_DATUM, (temperature < 0 ? TFT_RED : TFT_BLACK));

                spr.loadFont("fonts/weathericons78", LittleFS);
                if (weathercode == 55 || weathercode == 65 || weathercode == 75 || weathercode == 82 || weathercode == 86 || weathercode == 95 || weathercode == 99) {
                    spr.setTextColor(TFT_RED, TFT_WHITE);
                } else {
                    spr.setTextColor(TFT_BLACK, TFT_WHITE);
                }
                spr.setCursor(35, 25);
                spr.printToSprite(weatherIcons[weathercode]);
                spr.unloadFont();

                spr.loadFont("fonts/weathericons30", LittleFS);
                if (wind > 4) {
                    spr.setTextColor(TFT_RED, TFT_WHITE);
                } else {
                    spr.setTextColor(TFT_BLACK, TFT_WHITE);
                }
                spr.setCursor(115, -5);
                spr.printToSprite(windIcons[wind]);

                spr.setTextColor(TFT_BLACK, TFT_WHITE);
                spr.setCursor(90, -5);
                spr.printToSprite(windDirectionIcon(winddirection));
                if (weathercode > 10) {
                    spr.setTextColor(TFT_RED, TFT_WHITE);
                    spr.setCursor(115, 110);
                    spr.printToSprite("\uf084");
                }
                spr.unloadFont();

            }

            spr2grays(spr, spr.width(), spr.height(), filename);
            spr.deleteSprite();
        }
    }
    http.end();
}

void drawIdentify(String &filename, tagRecord *&taginfo) {

    TFT_eSPI tft = TFT_eSPI();
    TFT_eSprite spr = TFT_eSprite(&tft);
    LittleFS.begin();

    if (taginfo->hwType == SOLUM_29_033) {
        initSprite(spr, 296, 128);
        drawString(spr, taginfo->alias, 10, 10, "fonts/bahnschrift20");
        drawString(spr, mac62hex(taginfo->mac), 10, 50, "fonts/bahnschrift20", TL_DATUM, TFT_RED);

    } else if (taginfo->hwType == SOLUM_154_033) {
        initSprite(spr, 154, 154);
        drawString(spr, taginfo->alias, 5, 5, "fonts/bahnschrift20");
        drawString(spr, mac62hex(taginfo->mac), 10, 50, "fonts/bahnschrift20", TL_DATUM, TFT_RED);
    }

    spr2grays(spr, spr.width(), spr.height(), filename);
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
        if (httpCode!=304) { 
            wsErr("http " + URL + " " + String(httpCode));
        } else {
            wsLog("http " + URL + " " + String(httpCode));
        }
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

String urlEncode(const char *msg) {
    const char *hex = "0123456789ABCDEF";
    String encodedMsg = "";

    while (*msg != '\0') {
        if (
            ('a' <= *msg && *msg <= 'z') || ('A' <= *msg && *msg <= 'Z') || ('0' <= *msg && *msg <= '9') || *msg == '-' || *msg == '_' || *msg == '.' || *msg == '~') {
            encodedMsg += *msg;
        } else {
            encodedMsg += '%';
            encodedMsg += hex[(unsigned char)*msg >> 4];
            encodedMsg += hex[*msg & 0xf];
        }
        msg++;
    }
    return encodedMsg;
}

int windSpeedToBeaufort(float windSpeed) {
    int beaufort = 0;
    float speeds[] = {0.3, 1.5, 3.3, 5.5, 8, 10.8, 13.9, 17.2, 20.8, 24.5, 28.5, 32.7};
    for (int i = 0; i < 12; i++) {
        if (windSpeed >= speeds[i]) {
            beaufort = i + 1;
        }
    }
    return beaufort;
}

String windDirectionIcon(int degrees) {
    String directions[] = {"\uf044", "\uf088", "\uf04d", "\uf057", "\uf058", "\uf087", "\uf048", "\uf043"};
    int index = (degrees + 22) / 45;
    if (index >= 8) {
        index = 0;
    }
    return directions[index];
}

String mac62hex(uint8_t *mac) {
    char buffer[16];
    sprintf(buffer, "%02X%02X%02X%02X%02X%02X\0", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return (String)buffer;
}
