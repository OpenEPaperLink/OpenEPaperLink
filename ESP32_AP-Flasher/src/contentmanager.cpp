#include "contentmanager.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <MD5Builder.h>
#include <locale.h>
#include <rssClass.h>
#include <time.h>
#include <LittleFS.h>

#include "U8g2_for_TFT_eSPI.h"
#include "commstructs.h"
#include "makeimage.h"
#include "newproto.h"
#include "qrcode.h"
#include "tag_db.h"
#include "settings.h"
#include "web.h"
#include "language.h"

#define PAL_BLACK 0
#define PAL_WHITE 9
#define PAL_RED 2

enum contentModes {
    Image,
    Today,
    CountDays,
    CountHours,
    Weather,
    Firmware,
    Memo,
    ImageUrl,
    Forecast,
    RSSFeed,
    QRcode,
    Calendar,
    RemoteAP,
    SegStatic,
};

void contentRunner() {
    time_t now;
    time(&now);

    for (int16_t c = 0; c < tagDB.size(); c++) {
        tagRecord *taginfo = nullptr;
        taginfo = tagDB.at(c);

        if (taginfo->RSSI && (now >= taginfo->nextupdate || taginfo->wakeupReason == WAKEUP_REASON_GPIO || taginfo->wakeupReason == WAKEUP_REASON_NFC)) {
            drawNew(taginfo->mac, (taginfo->wakeupReason == WAKEUP_REASON_GPIO), taginfo);
            taginfo->wakeupReason = 0;
        }

        if (taginfo->expectedNextCheckin > now - 10 && taginfo->expectedNextCheckin < now + 30 && taginfo->pendingIdle == 0 && taginfo->pending == false) {
            uint16_t minutesUntilNextUpdate = 0;
            minutesUntilNextUpdate = (taginfo->nextupdate - now) / 60;
            if (minutesUntilNextUpdate > config.maxsleep) minutesUntilNextUpdate = config.maxsleep;
            if (minutesUntilNextUpdate > 1 && (wsClientCount() == 0 || config.stopsleep == 0)) {
                taginfo->pendingIdle = minutesUntilNextUpdate;
                if (taginfo->isExternal == false) prepareIdleReq(taginfo->mac, minutesUntilNextUpdate);
            }
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);  // add a small delay to allow other threads to run
    }
}

void drawNew(uint8_t mac[8], bool buttonPressed, tagRecord *&taginfo) {
    time_t now;
    time(&now);

    char hexmac[17];
    mac2hex(mac, hexmac);
    String filename = "/" + String(hexmac) + ".raw";

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
    char buffer[64];

    wsLog("Updating " + String(hexmac));
    taginfo->nextupdate = now + 60;

    imgParam imageParams;
    imageParams.hasRed = false;
    imageParams.dataType = DATATYPE_IMG_RAW_1BPP;
    imageParams.dither = true;

    imageParams.invert = false;
    imageParams.symbols = 0;

    switch (taginfo->contentMode) {
        case Image:

            if (cfgobj["filename"].as<String>() && cfgobj["filename"].as<String>() != "null" && !cfgobj["#fetched"].as<bool>()) {
                if (cfgobj["dither"] && cfgobj["dither"] == "0") imageParams.dither = false;
                jpg2buffer(cfgobj["filename"].as<String>(), filename, imageParams);
                if (imageParams.hasRed) imageParams.dataType = DATATYPE_IMG_RAW_2BPP;
                if (prepareDataAvail(&filename, imageParams.dataType, mac, cfgobj["timetolive"].as<int>())) {
                    cfgobj["#fetched"] = true;
                    if (cfgobj["delete"].as<String>()) LittleFS.remove("/"+cfgobj["filename"].as<String>());
                } else {
                    wsErr("Error accessing " + filename);
                }
            }
            taginfo->nextupdate = 3216153600;
            break;

        case Today:

            drawDate(filename, taginfo, imageParams);
            taginfo->nextupdate = midnight;
            updateTagImage(filename, mac, (midnight - now) / 60 - 10, taginfo, imageParams);
            break;

        case CountDays:

            if (buttonPressed) cfgobj["counter"] = 0;
            drawNumber(filename, (int32_t)cfgobj["counter"], (int32_t)cfgobj["thresholdred"], taginfo, imageParams);
            taginfo->nextupdate = midnight;
            updateTagImage(filename, mac, (buttonPressed ? 0 : 15), taginfo, imageParams);
            cfgobj["counter"] = (int32_t)cfgobj["counter"] + 1;
            break;

        case CountHours:

            if (buttonPressed) cfgobj["counter"] = 0;
            drawNumber(filename, (int32_t)cfgobj["counter"], (int32_t)cfgobj["thresholdred"], taginfo, imageParams);
            taginfo->nextupdate = now + 3600;
            updateTagImage(filename, mac, (buttonPressed ? 0 : 5), taginfo, imageParams);
            cfgobj["counter"] = (int32_t)cfgobj["counter"] + 1;
            break;

        case Weather:

            // https://open-meteo.com/
            // https://geocoding-api.open-meteo.com/v1/search?name=eindhoven
            // https://api.open-meteo.com/v1/forecast?latitude=52.52&longitude=13.41&current_weather=true
            // https://github.com/erikflowers/weather-icons

            drawWeather(filename, cfgobj, taginfo, imageParams);
            taginfo->nextupdate = now + 3600;
            updateTagImage(filename, mac, 15, taginfo, imageParams);
            break;

        case Forecast:

            drawForecast(filename, cfgobj, taginfo, imageParams);
            taginfo->nextupdate = now + 3 * 3600;
            updateTagImage(filename, mac, 15, taginfo, imageParams);
            break;

        case Firmware:

            filename = cfgobj["filename"].as<String>();
            if (filename && filename != "null" && !cfgobj["#fetched"].as<bool>()) {
                if (prepareDataAvail(&filename, DATATYPE_FW_UPDATE, mac, cfgobj["timetolive"].as<int>())) {
                    cfgobj["#fetched"] = true;
                } else {
                    wsErr("Error accessing " + filename);
                }
                cfgobj["filename"] = "";
                taginfo->nextupdate = 3216153600;
                taginfo->contentMode = Image;
            } else {
                taginfo->nextupdate = now + 300;
            }
            break;

        case Memo:

            drawIdentify(filename, taginfo, imageParams);
            taginfo->nextupdate = now + 12 * 3600;
            updateTagImage(filename, mac, 0, taginfo, imageParams);
            break;

        case ImageUrl:

            if (getImgURL(filename, cfgobj["url"], (time_t)cfgobj["#fetched"], imageParams, String(hexmac))) {
                taginfo->nextupdate = now + 60 * (cfgobj["interval"].as<int>() < 5 ? 5 : cfgobj["interval"].as<int>());
                updateTagImage(filename, mac, cfgobj["interval"].as<int>(), taginfo, imageParams);
                cfgobj["#fetched"] = now;
            } else {
                taginfo->nextupdate = now + 300;
            }
            break;

        case RSSFeed:

            if (getRssFeed(filename, cfgobj["url"], cfgobj["title"], taginfo, imageParams)) {
                taginfo->nextupdate = now + 60 * (cfgobj["interval"].as<int>() < 5 ? 5 : cfgobj["interval"].as<int>());
                updateTagImage(filename, mac, cfgobj["interval"].as<int>(), taginfo, imageParams);
            } else {
                taginfo->nextupdate = now + 300;
            }
            break;

        case QRcode:

            drawQR(filename, cfgobj["qr-content"], cfgobj["title"], taginfo, imageParams);
            taginfo->nextupdate = now + 12 * 3600;
            updateTagImage(filename, mac, 0, taginfo, imageParams);
            break;

        case Calendar:

            if (getCalFeed(filename, cfgobj["apps_script_url"], cfgobj["title"], taginfo, imageParams)) {
                taginfo->nextupdate = now + 60 * (cfgobj["interval"].as<int>() < 5 ? 5 : cfgobj["interval"].as<int>());
                updateTagImage(filename, mac, cfgobj["interval"].as<int>(), taginfo, imageParams);
            } else {
                taginfo->nextupdate = now + 300;
            }
            break;

        case RemoteAP:

            taginfo->nextupdate = 3216153600;
            break;

        case SegStatic:

            sprintf(buffer, "%-4.4s%-2.2s%-4.4s", cfgobj["line1"].as<const char *>(), cfgobj["line2"].as<const char *>(), cfgobj["line3"].as<const char *>());
            taginfo->nextupdate = 3216153600;
            sendAPSegmentedData(mac, (String)buffer, 0x0000, false, (taginfo->isExternal == false));
            break;

        case 14:  // NFC URL

            taginfo->nextupdate = 3216153600;
            memset(taginfo->md5, 0, 16 * sizeof(uint8_t));
            memset(taginfo->md5pending, 0, 16 * sizeof(uint8_t));
            prepareNFCReq(mac, cfgobj["url"].as<const char *>());
            break;
    }

    taginfo->modeConfigJson = doc.as<String>();
}

bool updateTagImage(String &filename, uint8_t *dst, uint16_t nextCheckin, tagRecord *&taginfo, imgParam &imageParams) {
    if (taginfo->hwType == SOLUM_SEG_UK) {
        sendAPSegmentedData(dst, (String)imageParams.segments, imageParams.symbols, imageParams.invert, (taginfo->isExternal == false));
    } else {
        if (imageParams.hasRed) imageParams.dataType = DATATYPE_IMG_RAW_2BPP;
        prepareDataAvail(&filename, imageParams.dataType, dst, nextCheckin);
    }
    return true;
}

void drawString(TFT_eSprite &spr, String content, uint16_t posx, uint16_t posy, String font, byte align, uint16_t color) {
    // drawString(spr,"test",100,10,"bahnschrift30",TC_DATUM,PAL_RED);
    spr.setTextDatum(align);
    if (font != "") spr.loadFont(font, LittleFS);
    spr.setTextColor(color, PAL_WHITE);
    spr.drawString(content, posx, posy);
    if (font != "") spr.unloadFont();
}

void initSprite(TFT_eSprite &spr, int w, int h) {
    spr.setColorDepth(4);  // 4 bits per pixel, uses indexed color
    spr.createSprite(w, h);
    uint16_t cmap[16];
    cmap[PAL_BLACK] = TFT_BLACK;
    cmap[PAL_RED] = TFT_RED;
    cmap[PAL_WHITE] = TFT_WHITE;
    spr.createPalette(cmap, 16);
    if (spr.getPointer() == nullptr) {
        wsErr("Failed to create sprite");
    }
    spr.fillSprite(PAL_WHITE);
}

void drawDate(String &filename, tagRecord *&taginfo, imgParam &imageParams) {
    time_t now;
    time(&now);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);

    int weekday_number = timeinfo.tm_wday;
    int month_number = timeinfo.tm_mon;
    int year_number = timeinfo.tm_year + 1900;

    if (taginfo->hwType == SOLUM_SEG_UK) {
        sprintf(imageParams.segments, "%2d%2d%-2.2s%04d", timeinfo.tm_mday, month_number + 1, languageDays[getCurrentLanguage()][timeinfo.tm_wday], year_number);
        imageParams.symbols = 0x04;
        return;
    }

    TFT_eSPI tft = TFT_eSPI();
    TFT_eSprite spr = TFT_eSprite(&tft);
    LittleFS.begin();

    if (taginfo->hwType == SOLUM_29_SSD1619 || taginfo->hwType == SOLUM_29_UC8151) {
        initSprite(spr, 296, 128);
        if (getCurrentLanguage() == 0 && timeinfo.tm_wday == 3) {
            drawString(spr, languageDays[getCurrentLanguage()][timeinfo.tm_wday], 296 / 2, 10, "fonts/calibrib50", TC_DATUM, PAL_RED);
        } else {
            drawString(spr, languageDays[getCurrentLanguage()][timeinfo.tm_wday], 296 / 2, 10, "fonts/calibrib62", TC_DATUM, PAL_RED);
        }
        drawString(spr, String(timeinfo.tm_mday) + " " + languageMonth[getCurrentLanguage()][timeinfo.tm_mon], 296 / 2, 73, "fonts/calibrib50", TC_DATUM);

    } else if (taginfo->hwType == SOLUM_154_SSD1619) {
        initSprite(spr, 152, 152);
        drawString(spr, languageDays[getCurrentLanguage()][timeinfo.tm_wday], 152 / 2, 10, "fonts/calibrib30", TC_DATUM);
        drawString(spr, String(languageMonth[getCurrentLanguage()][timeinfo.tm_mon]), 152 / 2, 120, "fonts/calibrib30", TC_DATUM);
        drawString(spr, String(timeinfo.tm_mday), 152 / 2, 42, "fonts/numbers2-1", TC_DATUM, PAL_RED);

    } else if (taginfo->hwType == SOLUM_42_SSD1619) {
        initSprite(spr, 400, 300);
        drawString(spr, languageDays[getCurrentLanguage()][timeinfo.tm_wday], 400 / 2, 30, "fonts/calibrib62", TC_DATUM, PAL_RED);
        drawString(spr, String(timeinfo.tm_mday) + " " + languageMonth[getCurrentLanguage()][timeinfo.tm_mon], 400 / 2, 113, "fonts/calibrib50", TC_DATUM);
    }

    spr2buffer(spr, filename, imageParams);
    spr.deleteSprite();
}

void drawNumber(String &filename, int32_t count, int32_t thresholdred, tagRecord *&taginfo, imgParam &imageParams) {

    if (taginfo->hwType == SOLUM_SEG_UK) {
        imageParams.symbols = 0x00;
        if (count > 19999) {
            sprintf(imageParams.segments, "over  flow");
        } else {
            if (count > 9999) {
                imageParams.symbols = 0x02;
                if (taginfo->contentMode == CountHours) {
                    sprintf(imageParams.segments, "%04d  hour", count - 10000);
                } else {
                    sprintf(imageParams.segments, "%04d  days", count - 10000);
                }
            } else {
                if (taginfo->contentMode == CountHours) {
                    sprintf(imageParams.segments, "%4d  hour", count);
                } else {
                    sprintf(imageParams.segments, "%4d  days", count);
                }
            }
        }
        return;
    }

    TFT_eSPI tft = TFT_eSPI();
    TFT_eSprite spr = TFT_eSprite(&tft);
    LittleFS.begin();

    if (taginfo->hwType == SOLUM_29_SSD1619 || taginfo->hwType == SOLUM_29_UC8151) {
        initSprite(spr, 296, 128);
        spr.setTextDatum(MC_DATUM);
        if (count > thresholdred) {
            spr.setTextColor(PAL_RED, PAL_WHITE);
        } else {
            spr.setTextColor(PAL_BLACK, PAL_WHITE);
        }
        String font = "fonts/numbers1-2";
        if (count > 999) font = "fonts/numbers2-2";
        if (count > 9999) font = "fonts/numbers3-2";
        spr.loadFont(font, LittleFS);
        spr.drawString(String(count), 296 / 2, 128 / 2 + 10);
        spr.unloadFont();

    } else if (taginfo->hwType == SOLUM_154_SSD1619) {
        initSprite(spr, 152, 152);
        spr.setTextDatum(MC_DATUM);
        if (count > thresholdred) {
            spr.setTextColor(PAL_RED, PAL_WHITE);
        } else {
            spr.setTextColor(PAL_BLACK, PAL_WHITE);
        }
        String font = "fonts/numbers1-1";
        if (count > 99) font = "fonts/numbers2-1";
        if (count > 999) font = "fonts/numbers3-1";
        spr.loadFont(font, LittleFS);
        spr.drawString(String(count), 152 / 2, 152 / 2 + 7);
        spr.unloadFont();

    } else if (taginfo->hwType == SOLUM_42_SSD1619) {
        initSprite(spr, 400, 300);
        spr.setTextDatum(MC_DATUM);
        if (count > thresholdred) {
            spr.setTextColor(PAL_RED, PAL_WHITE);
        } else {
            spr.setTextColor(PAL_BLACK, PAL_WHITE);
        }
        String font = "fonts/numbers1-2";
        if (count > 999) font = "fonts/numbers2-2";
        if (count > 9999) font = "fonts/numbers3-2";
        spr.loadFont(font, LittleFS);
        spr.drawString(String(count), 400 / 2, 300 / 2 + 7);
        spr.unloadFont();
    }

    spr2buffer(spr, filename, imageParams);
    spr.deleteSprite();
}

void drawWeather(String &filename, JsonObject &cfgobj, tagRecord *&taginfo, imgParam &imageParams) {

    wsLog("get weather");

    getLocation(cfgobj);
    HTTPClient http;

    String lat = cfgobj["#lat"];
    String lon = cfgobj["#lon"];
    String tz = cfgobj["#tz"];
    http.begin("https://api.open-meteo.com/v1/forecast?latitude=" + lat + "&longitude=" + lon + "&current_weather=true&windspeed_unit=ms&timezone=" + tz);
    http.setTimeout(5000);
    int httpCode = http.GET();
    Serial.printf("Got code %d for this OpenMeteo\n", httpCode);

    if (httpCode == 200) {
        StaticJsonDocument<200> filter;
        filter["current_weather"]["temperature"] = true;
        filter["current_weather"]["windspeed"] = true;
        filter["current_weather"]["winddirection"] = true;
        filter["current_weather"]["weathercode"] = true;
        filter["current_weather"]["is_day"] = true;

        StaticJsonDocument<1000> doc;
        DeserializationError error = deserializeJson(doc, http.getString(), DeserializationOption::Filter(filter));
        if (error) {
            Serial.println(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
        }

        auto temperature = doc["current_weather"]["temperature"].as<double>();
        auto windspeed = doc["current_weather"]["windspeed"].as<int>();
        auto winddirection = doc["current_weather"]["winddirection"].as<int>();
        uint8_t weathercode = doc["current_weather"]["weathercode"].as<int>();
        uint8_t isday = doc["current_weather"]["is_day"].as<int>();
        if (weathercode > 40) weathercode -= 40;
        int wind = windSpeedToBeaufort(windspeed);

        doc.clear();

        if (taginfo->hwType == SOLUM_SEG_UK) {
            String weatherText[] = {"sun", "sun", "sun", "CLDY", "CLDY", "FOG", "", "", "FOG", "", "",
                                    "DRZL", "", "DRZL", "", "DRZL", "ice", "ice", "", "", "",
                                    "rain", "", "rain", "", "rain", "ice", "ice", "", "", "",
                                    "SNOW", "", "SNOW", "", "SNOW", "", "SNOW", "", "", "rain",
                                    "rain", "rain", "", "", "SNOW", "SNOW", "", "", "", "",
                                    "", "", "", "", "STRM", "HAIL", "", "", "HAIL"};
            if (temperature < -9.9) {
                sprintf(imageParams.segments, "%3d^%2d%-4.4s", static_cast<int>(temperature), wind, weatherText[weathercode].c_str());
                imageParams.symbols = 0x00;
            } else {
                sprintf(imageParams.segments, "%3d^%2d%-4.4s", static_cast<int>(temperature * 10), wind, weatherText[weathercode].c_str());
                imageParams.symbols = 0x04;
            }
            http.end();
            return;
        }

        String weatherIcons[] = {"\uf00d", "\uf00c", "\uf002", "\uf013", "\uf013", "\uf014", "-", "-", "\uf014", "-", "-",
                                 "\uf01a", "-", "\uf01a", "-", "\uf01a", "\uf017", "\uf017", "-", "-", "-",
                                 "\uf019", "-", "\uf019", "-", "\uf019", "\uf015", "\uf015", "-", "-", "-",
                                 "\uf01b", "-", "\uf01b", "-", "\uf01b", "-", "\uf076", "-", "-", "\uf01a",
                                 "\uf01a", "\uf01a", "-", "-", "\uf064", "\uf064", "-", "-", "-", "-",
                                 "-", "-", "-", "-", "\uf01e", "\uf01d", "-", "-", "\uf01e"};
        if (isday == 0) {
            weatherIcons[0] = "\uf02e";
            weatherIcons[1] = "\uf083";
            weatherIcons[2] = "\uf086";
        }

        TFT_eSPI tft = TFT_eSPI();
        TFT_eSprite spr = TFT_eSprite(&tft);
        LittleFS.begin();
        tft.setTextWrap(false, false);

        if (taginfo->hwType == SOLUM_29_SSD1619 || taginfo->hwType == SOLUM_29_UC8151) {
            initSprite(spr, 296, 128);

            drawString(spr, cfgobj["location"], 5, 5, "fonts/bahnschrift30");
            drawString(spr, String(wind), 280, 5, "fonts/bahnschrift30", TR_DATUM, (wind > 4 ? PAL_RED : PAL_BLACK));

            char tmpOutput[5];
            dtostrf(temperature, 2, 1, tmpOutput);
            drawString(spr, String(tmpOutput), 5, 65, "fonts/bahnschrift70", TL_DATUM, (temperature < 0 ? PAL_RED : PAL_BLACK));

            spr.loadFont("fonts/weathericons70", LittleFS);
            if (weathercode == 55 || weathercode == 65 || weathercode == 75 || weathercode == 82 || weathercode == 86 || weathercode == 95 || weathercode == 99) {
                spr.setTextColor(PAL_RED, PAL_WHITE);
            } else {
                spr.setTextColor(PAL_BLACK, PAL_WHITE);
            }

            spr.setCursor(185, 32);
            spr.printToSprite(weatherIcons[weathercode]);
            spr.unloadFont();

            spr.loadFont("fonts/weathericons30", LittleFS);
            spr.setTextColor(PAL_BLACK, PAL_WHITE);
            spr.setCursor(235, -3);
            spr.printToSprite(windDirectionIcon(winddirection));
            if (weathercode > 10) {
                spr.setTextColor(PAL_RED, PAL_WHITE);
                spr.setCursor(190, 0);
                spr.printToSprite("\uf084");
            }
            spr.unloadFont();

        } else if (taginfo->hwType == SOLUM_154_SSD1619) {
            initSprite(spr, 152, 152);
            spr.setTextDatum(TL_DATUM);

            spr.setTextFont(2);
            spr.setTextColor(PAL_BLACK, PAL_WHITE);
            spr.drawString(cfgobj["location"].as<String>(), 10, 130);

            char tmpOutput[5];
            dtostrf(temperature, 2, 1, tmpOutput);
            drawString(spr, String(tmpOutput), 10, 10, "fonts/bahnschrift30", TL_DATUM, (temperature < 0 ? PAL_RED : PAL_BLACK));

            spr.loadFont("fonts/weathericons78", LittleFS);
            if (weathercode == 55 || weathercode == 65 || weathercode == 75 || weathercode == 82 || weathercode == 86 || weathercode == 95 || weathercode == 99) {
                spr.setTextColor(PAL_RED, PAL_WHITE);
            } else {
                spr.setTextColor(PAL_BLACK, PAL_WHITE);
            }

            spr.setCursor(33, 33);
            spr.printToSprite(weatherIcons[weathercode]);
            spr.unloadFont();

            drawString(spr, String(wind), 140, 10, "fonts/bahnschrift30", TR_DATUM, (wind > 4 ? PAL_RED : PAL_BLACK));

            spr.loadFont("fonts/weathericons30", LittleFS);
            spr.setTextColor(PAL_BLACK, PAL_WHITE);
            spr.setCursor(100, -2);
            spr.printToSprite(windDirectionIcon(winddirection));
            if (weathercode > 10) {
                spr.setTextColor(PAL_RED, PAL_WHITE);
                spr.setCursor(115, 110);
                spr.printToSprite("\uf084");
            }
            spr.unloadFont();

        } else if (taginfo->hwType == SOLUM_42_SSD1619) {
            initSprite(spr, 400, 300);

            drawString(spr, cfgobj["location"], 10, 10, "fonts/bahnschrift30");
            drawString(spr, String(wind), 280, 10, "fonts/bahnschrift30", TR_DATUM, (wind > 4 ? PAL_RED : PAL_BLACK));

            char tmpOutput[5];
            dtostrf(temperature, 2, 1, tmpOutput);
            drawString(spr, String(tmpOutput), 5, 65, "fonts/bahnschrift70", TL_DATUM, (temperature < 0 ? PAL_RED : PAL_BLACK));

            spr.loadFont("fonts/weathericons70", LittleFS);
            if (weathercode == 55 || weathercode == 65 || weathercode == 75 || weathercode == 82 || weathercode == 86 || weathercode == 95 || weathercode == 99) {
                spr.setTextColor(PAL_RED, PAL_WHITE);
            } else {
                spr.setTextColor(PAL_BLACK, PAL_WHITE);
            }

            spr.setCursor(185, 32);
            spr.printToSprite(weatherIcons[weathercode]);
            spr.unloadFont();

            spr.loadFont("fonts/weathericons30", LittleFS);
            spr.setTextColor(PAL_BLACK, PAL_WHITE);
            spr.setCursor(235, -3);
            spr.printToSprite(windDirectionIcon(winddirection));
            if (weathercode > 10) {
                spr.setTextColor(PAL_RED, PAL_WHITE);
                spr.setCursor(190, 0);
                spr.printToSprite("\uf084");
            }
            spr.unloadFont();
        }

        spr2buffer(spr, filename, imageParams);
        spr.deleteSprite();
    }
    http.end();
}

void drawForecast(String &filename, JsonObject &cfgobj, tagRecord *&taginfo, imgParam &imageParams) {
    TFT_eSPI tft = TFT_eSPI();
    TFT_eSprite spr = TFT_eSprite(&tft);

    wsLog("get weather");
    getLocation(cfgobj);
    HTTPClient http;

    String lat = cfgobj["#lat"];
    String lon = cfgobj["#lon"];
    String tz = cfgobj["#tz"];

    http.begin("https://api.open-meteo.com/v1/forecast?latitude=" + lat + "&longitude=" + lon + "&daily=weathercode,temperature_2m_max,temperature_2m_min,precipitation_sum,windspeed_10m_max,winddirection_10m_dominant&windspeed_unit=ms&timeformat=unixtime&timezone=" + tz);

    http.setTimeout(5000);
    int httpCode = http.GET();

    if (httpCode == 200) {
        StaticJsonDocument<500> filter;
        filter["daily"]["time"][0] = true;
        filter["daily"]["weathercode"][0] = true;
        filter["daily"]["temperature_2m_max"][0] = true;
        filter["daily"]["temperature_2m_min"][0] = true;
        filter["daily"]["precipitation_sum"][0] = true;
        filter["daily"]["windspeed_10m_max"][0] = true;
        filter["daily"]["winddirection_10m_dominant"][0] = true;

        DynamicJsonDocument doc(2000);
        DeserializationError error = deserializeJson(doc, http.getString(), DeserializationOption::Filter(filter));
        if (error) {
            Serial.println(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
        }

        String weatherIcons[] = {"\uf00d", "\uf00c", "\uf002", "\uf013", "\uf013", "\uf014", "-", "-", "\uf014", "-", "-",
                                 "\uf01a", "-", "\uf01a", "-", "\uf01a", "\uf017", "\uf017", "-", "-", "-",
                                 "\uf019", "-", "\uf019", "-", "\uf019", "\uf015", "\uf015", "-", "-", "-",
                                 "\uf01b", "-", "\uf01b", "-", "\uf01b", "-", "\uf076", "-", "-", "\uf01a",
                                 "\uf01a", "\uf01a", "-", "-", "\uf064", "\uf064", "-", "-", "-", "-",
                                 "-", "-", "-", "-", "\uf01e", "\uf01d", "-", "-", "\uf01e"};

        LittleFS.begin();
        tft.setTextWrap(false, false);

        if (taginfo->hwType == SOLUM_29_SSD1619 || taginfo->hwType == SOLUM_29_UC8151) {
            initSprite(spr, 296, 128);

            spr.setTextFont(2);
            spr.setTextColor(PAL_BLACK, PAL_WHITE);
            spr.drawString(cfgobj["location"].as<String>(), 5, 0);

            for (uint8_t dag = 0; dag < 5; dag++) {
                time_t weatherday = doc["daily"]["time"][dag].as<time_t>();
                struct tm *datum = localtime(&weatherday);
                drawString(spr, String(languageDaysShort[getCurrentLanguage()][datum->tm_wday]), dag * 59 + 30, 18, "fonts/twbold20", TC_DATUM, PAL_BLACK);

                uint8_t weathercode = doc["daily"]["weathercode"][dag].as<int>();
                if (weathercode > 40) weathercode -= 40;

                spr.loadFont("fonts/weathericons30", LittleFS);
                if (weathercode == 55 || weathercode == 65 || weathercode == 75 || weathercode == 82 || weathercode == 86 || weathercode == 95 || weathercode == 99) {
                    spr.setTextColor(PAL_RED, PAL_WHITE);
                } else {
                    spr.setTextColor(PAL_BLACK, PAL_WHITE);
                }
                spr.setTextDatum(TL_DATUM);
                spr.setCursor(12 + dag * 59, 58);
                spr.printToSprite(weatherIcons[weathercode]);

                spr.setTextColor(PAL_BLACK, PAL_WHITE);
                spr.setCursor(17 + dag * 59, 27);
                spr.printToSprite(windDirectionIcon(doc["daily"]["winddirection_10m_dominant"][dag]));
                spr.unloadFont();

                int8_t tmin = round(doc["daily"]["temperature_2m_min"][dag].as<double>());
                int8_t tmax = round(doc["daily"]["temperature_2m_max"][dag].as<double>());
                uint8_t wind = windSpeedToBeaufort(doc["daily"]["windspeed_10m_max"][dag].as<double>());

                spr.loadFont("fonts/GillSC20", LittleFS);
                drawString(spr, String(tmin) + " ", dag * 59 + 30, 108, "", TR_DATUM, (tmin < 0 ? PAL_RED : PAL_BLACK));
                drawString(spr, String(" ") + String(tmax), dag * 59 + 30, 108, "", TL_DATUM, (tmax < 0 ? PAL_RED : PAL_BLACK));
                drawString(spr, String(" ") + String(wind), dag * 59 + 30, 43, "", TL_DATUM, (wind > 5 ? PAL_RED : PAL_BLACK));
                spr.unloadFont();
                if (dag > 0) {
                    for (int i = 20; i < 128; i += 3) {
                        spr.drawPixel(dag * 59, i, PAL_BLACK);
                    }
                }
            }

        } else if (taginfo->hwType == SOLUM_42_SSD1619) {
            initSprite(spr, 400, 300);
            spr.setTextFont(2);
            spr.setTextColor(PAL_BLACK, PAL_WHITE);
            spr.drawString(cfgobj["location"].as<String>(), 5, 0);

            for (uint8_t dag = 0; dag < 5; dag++) {
                time_t weatherday = doc["daily"]["time"][dag].as<time_t>();
                struct tm *datum = localtime(&weatherday);
                drawString(spr, String(languageDaysShort[getCurrentLanguage()][datum->tm_wday]), dag * 59 + 30, 18, "fonts/twbold20", TC_DATUM, PAL_BLACK);

                uint8_t weathercode = doc["daily"]["weathercode"][dag].as<int>();
                if (weathercode > 40) weathercode -= 40;

                spr.loadFont("fonts/weathericons30", LittleFS);
                if (weathercode == 55 || weathercode == 65 || weathercode == 75 || weathercode == 82 || weathercode == 86 || weathercode == 95 || weathercode == 99) {
                    spr.setTextColor(PAL_RED, PAL_WHITE);
                } else {
                    spr.setTextColor(PAL_BLACK, PAL_WHITE);
                }
                spr.setTextDatum(TL_DATUM);
                spr.setCursor(12 + dag * 59, 58);
                spr.printToSprite(weatherIcons[weathercode]);

                spr.setTextColor(PAL_BLACK, PAL_WHITE);
                spr.setCursor(17 + dag * 59, 27);
                spr.printToSprite(windDirectionIcon(doc["daily"]["winddirection_10m_dominant"][dag]));
                spr.unloadFont();

                int8_t tmin = round(doc["daily"]["temperature_2m_min"][dag].as<double>());
                int8_t tmax = round(doc["daily"]["temperature_2m_max"][dag].as<double>());
                uint8_t wind = windSpeedToBeaufort(doc["daily"]["windspeed_10m_max"][dag].as<double>());

                spr.loadFont("fonts/GillSC20", LittleFS);
                drawString(spr, String(tmin) + " ", dag * 59 + 30, 108, "", TR_DATUM, (tmin < 0 ? PAL_RED : PAL_BLACK));
                drawString(spr, String(" ") + String(tmax), dag * 59 + 30, 108, "", TL_DATUM, (tmax < 0 ? PAL_RED : PAL_BLACK));
                drawString(spr, String(" ") + String(wind), dag * 59 + 30, 43, "", TL_DATUM, (wind > 5 ? PAL_RED : PAL_BLACK));
                spr.unloadFont();
                if (dag > 0) {
                    for (int i = 20; i < 128; i += 3) {
                        spr.drawPixel(dag * 59, i, PAL_BLACK);
                    }
                }
            }
        }
        spr2buffer(spr, filename, imageParams);
        spr.deleteSprite();
    }
    http.end();
}

void drawIdentify(String &filename, tagRecord *&taginfo, imgParam &imageParams) {
    TFT_eSPI tft = TFT_eSPI();
    TFT_eSprite spr = TFT_eSprite(&tft);
    LittleFS.begin();

    char hexmac[17];
    mac2hex(taginfo->mac, hexmac);
    if (taginfo->hwType == SOLUM_29_SSD1619 || taginfo->hwType == SOLUM_29_UC8151) {
        initSprite(spr, 296, 128);
        drawString(spr, taginfo->alias, 10, 10, "fonts/bahnschrift20");
        drawString(spr, String(hexmac), 10, 50, "fonts/bahnschrift20", TL_DATUM, PAL_RED);

    } else if (taginfo->hwType == SOLUM_154_SSD1619) {
        initSprite(spr, 152, 152);
        drawString(spr, taginfo->alias, 5, 5, "fonts/bahnschrift20");
        drawString(spr, String(hexmac), 10, 50, "fonts/bahnschrift20", TL_DATUM, PAL_RED);

    } else if (taginfo->hwType == SOLUM_42_SSD1619) {
        initSprite(spr, 400, 300);
        drawString(spr, taginfo->alias, 20, 20, "fonts/bahnschrift20");
        drawString(spr, String(hexmac), 20, 70, "fonts/bahnschrift20", TL_DATUM, PAL_RED);
    }

    spr2buffer(spr, filename, imageParams);
    spr.deleteSprite();
}

bool getImgURL(String &filename, String URL, time_t fetched, imgParam &imageParams, String MAC) {
    // https://images.klari.net/kat-bw29.jpg

    LittleFS.begin();

    Serial.println("get external " + URL);
    HTTPClient http;
    http.begin(URL);
    http.addHeader("If-Modified-Since", formatHttpDate(fetched));
    http.addHeader("X-ESL-MAC", MAC);
    http.setTimeout(5000);  // timeout in ms
    int httpCode = http.GET();
    if (httpCode == 200) {
        File f = LittleFS.open("/temp/temp.jpg", "w");
        if (f) {
            http.writeToStream(&f);
            f.close();
            jpg2buffer("/temp/temp.jpg", filename, imageParams);
        }
    } else {
        if (httpCode != 304) {
            wsErr("http " + URL + " " + String(httpCode));
        } else {
            wsLog("http " + URL + " " + String(httpCode));
        }
    }
    http.end();
    return (httpCode == 200 || httpCode == 304);
}

rssClass reader;

bool getRssFeed(String &filename, String URL, String title, tagRecord *&taginfo, imgParam &imageParams) {
    // https://github.com/garretlab/shoddyxml2

    // http://feeds.feedburner.com/tweakers/nieuws
    // https://www.nu.nl/rss/Algemeen

    Serial.println("RSS feed");
    struct tm timeInfo;
    char header[32];
    getLocalTime(&timeInfo);
    sprintf(header, "%02d-%02d-%04d %02d:%02d", timeInfo.tm_mday, timeInfo.tm_mon + 1, timeInfo.tm_year + 1900, timeInfo.tm_hour, timeInfo.tm_min);

    const char *url = URL.c_str();
    const char *tag = "title";
    const int rssArticleSize = 128;

    TFT_eSPI tft = TFT_eSPI();
    TFT_eSprite spr = TFT_eSprite(&tft);
    U8g2_for_TFT_eSPI u8f;
    u8f.begin(spr);

    if (taginfo->hwType == SOLUM_29_SSD1619 || taginfo->hwType == SOLUM_29_UC8151) {
        initSprite(spr, 296, 128);
        if (title == "" || title == "null") title = "RSS feed";
        drawString(spr, title, 5, 3, "fonts/bahnschrift20", TL_DATUM, PAL_RED);

        u8f.setFont(u8g2_font_glasstown_nbp_tr);
        u8f.setFontMode(0);
        u8f.setFontDirection(0);
        u8f.setForegroundColor(PAL_BLACK);
        u8f.setBackgroundColor(PAL_WHITE);
        u8f.setCursor(220, 20);
        u8f.print(header);

        // u8g2_font_nine_by_five_nbp_tr
        // u8g2_font_7x14_tr
        // u8g2_font_crox1h_tr
        // u8g2_font_miranda_nbp_tr
        u8f.setFont(u8g2_font_glasstown_nbp_tr);

        int n = reader.getArticles(url, tag, rssArticleSize, 8);
        for (int i = 0; i < n; i++) {
            u8f.setCursor(5, 34 + i * 13);
            u8f.print(reader.itemData[i]);
        }
    } else if (taginfo->hwType == SOLUM_42_SSD1619) {
        initSprite(spr, 400, 300);
        if (title == "" || title == "null") title = "RSS feed";
        drawString(spr, title, 5, 5, "fonts/bahnschrift20", TL_DATUM, PAL_RED);

        u8f.setFont(u8g2_font_glasstown_nbp_tr);
        u8f.setFontMode(0);
        u8f.setFontDirection(0);
        u8f.setForegroundColor(PAL_BLACK);
        u8f.setBackgroundColor(PAL_WHITE);
        u8f.setCursor(220, 20);
        u8f.print(header);

        u8f.setFont(u8g2_font_glasstown_nbp_tr);

        int n = reader.getArticles(url, tag, rssArticleSize, 10);
        for (int i = 0; i < n; i++) {
            u8f.setCursor(5, 34 + i * 14);
            u8f.print(reader.itemData[i]);
        }
    }

    spr2buffer(spr, filename, imageParams);
    spr.deleteSprite();

    return true;
}

char *epoch_to_display(time_t utc) {
    static char display[6];
    struct tm local_tm;
    localtime_r(&utc, &local_tm);
    time_t now;
    time(&now);
    struct tm now_tm;
    localtime_r(&now, &now_tm);
    if (local_tm.tm_year < now_tm.tm_year ||
        (local_tm.tm_year == now_tm.tm_year && local_tm.tm_mon < now_tm.tm_mon) ||
        (local_tm.tm_year == now_tm.tm_year && local_tm.tm_mon == now_tm.tm_mon && local_tm.tm_mday < now_tm.tm_mday) ||
        (local_tm.tm_hour == 0 && local_tm.tm_min == 0)) {
        strftime(display, sizeof(display), "%d-%m", &local_tm);
    } else {
        strftime(display, sizeof(display), "%H:%M", &local_tm);
    }
    return display;
}

bool getCalFeed(String &filename, String URL, String title, tagRecord *&taginfo, imgParam &imageParams) {
    // google apps scripts method to retrieve calendar
    // see /data/calendar.txt for description

    wsLog("get calendar");

    time_t now;
    time(&now);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    static char dateString[40];
    strftime(dateString, sizeof(dateString), " - %d.%m.%Y", &timeinfo);

    HTTPClient http;
    http.begin(URL);
    http.setTimeout(10000);  // timeout in ms
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();
    if (httpCode != 200) {
        wsErr("http error " + String(httpCode));
        return false;
    }

    DynamicJsonDocument doc(5000);
    DeserializationError error = deserializeJson(doc, http.getString());
    if (error) {
        wsErr(error.c_str());
    }
    http.end();

    TFT_eSPI tft = TFT_eSPI();
    TFT_eSprite spr = TFT_eSprite(&tft);
    U8g2_for_TFT_eSPI u8f;
    u8f.begin(spr);

    if (taginfo->hwType == SOLUM_29_SSD1619 || taginfo->hwType == SOLUM_29_UC8151) {
        initSprite(spr, 296, 128);
        if (title == "" || title == "null") title = "Calendar";

        u8f.setFont(u8g2_font_t0_22b_tr);  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
        u8f.setFontMode(0);
        u8f.setFontDirection(0);
        u8f.setForegroundColor(PAL_BLACK);
        u8f.setBackgroundColor(PAL_WHITE);
        u8f.setCursor(5, 16);
        u8f.print(title);
        u8f.print(dateString);

        int n = doc.size();
        if (n > 7) n = 7;
        for (int i = 0; i < n; i++) {
            JsonObject obj = doc[i];
            String eventtitle = obj["title"];
            String startz = obj["start"];
            time_t starttime = obj["start"];
            time_t endtime = obj["end"];
            if (starttime < now && endtime > now) {
                u8f.setFont(u8g2_font_t0_14b_tr);
                u8f.setForegroundColor(PAL_WHITE);
                u8f.setBackgroundColor(PAL_RED);
                spr.fillRect(0, i * 15 + 21, 296, 14, PAL_RED);
            } else {
                u8f.setFont(u8g2_font_t0_14_tr);
                u8f.setForegroundColor(PAL_BLACK);
                u8f.setBackgroundColor(PAL_WHITE);
            }
            u8f.setCursor(5, 32 + i * 15);
            u8f.print(epoch_to_display(obj["start"]));
            u8f.setCursor(50, 32 + i * 15);
            u8f.print(eventtitle);
        }
    } else if (taginfo->hwType == SOLUM_42_SSD1619) {
        initSprite(spr, 400, 300);
        if (title == "" || title == "null") title = "Calendar";

        u8f.setFont(u8g2_font_t0_22b_tr);
        u8f.setFontMode(0);
        u8f.setFontDirection(0);
        u8f.setForegroundColor(PAL_BLACK);
        u8f.setBackgroundColor(PAL_WHITE);
        u8f.setCursor(5, 16);
        u8f.print(title);
        u8f.print(dateString);

        int n = doc.size();
        if (n > 8) n = 8;
        for (int i = 0; i < n; i++) {
            JsonObject obj = doc[i];
            String eventtitle = obj["title"];
            String startz = obj["start"];
            time_t starttime = obj["start"];
            time_t endtime = obj["end"];
            if (starttime < now && endtime > now) {
                u8f.setFont(u8g2_font_t0_14b_tr);
                u8f.setForegroundColor(PAL_WHITE);
                u8f.setBackgroundColor(PAL_RED);
                spr.fillRect(0, i * 15 + 21, 296, 14, PAL_RED);
            } else {
                u8f.setFont(u8g2_font_t0_14_tr);
                u8f.setForegroundColor(PAL_BLACK);
                u8f.setBackgroundColor(PAL_WHITE);
            }
            u8f.setCursor(5, 32 + i * 15);
            u8f.print(epoch_to_display(obj["start"]));
            u8f.setCursor(50, 32 + i * 15);
            u8f.print(eventtitle);
        }
    }

    spr2buffer(spr, filename, imageParams);
    spr.deleteSprite();

    return true;
}

void drawQR(String &filename, String qrcontent, String title, tagRecord *&taginfo, imgParam &imageParams) {
    TFT_eSPI tft = TFT_eSPI();
    TFT_eSprite spr = TFT_eSprite(&tft);
    LittleFS.begin();

    const char *text = qrcontent.c_str();
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(2)];
    // https://github.com/ricmoo/QRCode
    qrcode_initText(&qrcode, qrcodeData, 2, ECC_MEDIUM, text);
    int size = qrcode.size;
    int xpos = 0, ypos = 0, dotsize = 1;

    if (taginfo->hwType == SOLUM_29_SSD1619 || taginfo->hwType == SOLUM_29_UC8151) {
        initSprite(spr, 296, 128);
        drawString(spr, title, 10, 5, "fonts/bahnschrift20");
        dotsize = int((128 - 25) / size);
        xpos = 149 - dotsize * size / 2;
        ypos = 25;
    } else if (taginfo->hwType == SOLUM_154_SSD1619) {
        initSprite(spr, 152, 152);
        spr.setTextFont(2);
        spr.setTextColor(PAL_BLACK, PAL_WHITE);
        spr.drawString(title, 10, 5);
        dotsize = int((152 - 20) / size);
        xpos = 76 - dotsize * size / 2;
        ypos = 20;
    } else if (taginfo->hwType == SOLUM_42_SSD1619) {
        initSprite(spr, 400, 300);
        drawString(spr, title, 10, 10, "fonts/bahnschrift20");
        dotsize = int((300 - 30) / size);
        xpos = 200 - dotsize * size / 2;
        ypos = 30;
    }

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if (qrcode_getModule(&qrcode, x, y)) {
                spr.fillRect(xpos + x * dotsize, ypos + y * dotsize, dotsize, dotsize, PAL_BLACK);
            }
        }
    }

    spr2buffer(spr, filename, imageParams);
    spr.deleteSprite();
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
    String directions[] = {"\uf044", "\uf043", "\uf048", "\uf087", "\uf058", "\uf057", "\uf04d", "\uf088"};
    int index = (degrees + 22) / 45;
    if (index >= 8) {
        index = 0;
    }
    return directions[index];
}

void getLocation(JsonObject &cfgobj) {
    HTTPClient http;
    StaticJsonDocument<1000> doc;

    String lat = cfgobj["#lat"];
    String lon = cfgobj["#lon"];
    String tz = cfgobj["#tz"];

    if (lat == "null" || lon == "null") {
        http.begin("https://geocoding-api.open-meteo.com/v1/search?name=" + urlEncode(cfgobj["location"]) + "&count=1");
        http.setTimeout(5000);
        int httpCode = http.GET();
        Serial.printf("Got code %d for this location\n", httpCode);
        Serial.print(http.errorToString(httpCode));

        if (httpCode == 200) {
            DeserializationError error = deserializeJson(doc, http.getStream());
            http.end();
            lat = doc["results"][0]["latitude"].as<String>();
            lon = doc["results"][0]["longitude"].as<String>();
            tz = doc["results"][0]["timezone"].as<String>();
            cfgobj["#lat"] = lat;
            cfgobj["#lon"] = lon;
            cfgobj["#tz"] = tz;
        }
    }
}