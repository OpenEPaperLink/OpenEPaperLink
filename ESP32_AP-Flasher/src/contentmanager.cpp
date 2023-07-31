#include "contentmanager.h"

// possibility to turn off, to save space if needed
#define CONTENT_QR
#define CONTENT_RSS
#define CONTENT_CAL
#define CONTENT_BUIENRADAR

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <MD5Builder.h>
#include <locale.h>
#ifdef CONTENT_RSS
#include <rssClass.h>
#endif
#include <time.h>

#include <map>

#include "storage.h"

#if defined CONTENT_RSS || defined CONTENT_CAL
#include "U8g2_for_TFT_eSPI.h"
#endif
#include "commstructs.h"
#include "makeimage.h"
#include "newproto.h"
#ifdef CONTENT_QR
#include "qrcode.h"
#endif
#include "language.h"
#include "settings.h"
#include "tag_db.h"
#include "web.h"
#include "truetype.h"

#define PAL_BLACK TFT_BLACK
#define PAL_WHITE TFT_WHITE
#define PAL_RED TFT_RED

#define TEMPLATE "/content_template.json"
// https://csvjson.com/json_beautifier

struct HwType {
    uint8_t basetype;
    uint16_t width;
    uint16_t height;
};

std::map<int, HwType> hwdata = {
    {0, {0, 152, 152}},
    {1, {1, 296, 128}},
    {2, {2, 400, 300}},
    {3, {2, 168, 384}},
    {4, {2, 800, 480}},
    {5, {2, 640, 384}},
    {17, {1, 296, 128}},
    {0x30, {0, 200, 200}},  //"1.6\" BWR 200x200px"
    {0x38, {0, 200, 200}},  //"1.6\" BWY 200x200px"
    {0x31, {1, 296, 160}},  //"2.2\" BWR 296x160px"
    {0x39, {1, 296, 160}},  //"2.2\" BWY 296x160px"
    {0x32, {1, 360, 184}},  //"2.6\" BWR 360x184px"
    {0x3A, {1, 360, 184}},  //"2.6\" BWY 360x184px"
    {0x33, {1, 384, 168}},     //"2.9\" BWR 384x168px"
    {0x3B, {1, 384, 168}},     //"2.9\" BWY 384x168px"
    {0x34, {2, 400, 300}},     //"4.2\" BWR 400x300px"
    {0x3C, {2, 400, 300}},     //"4.2\" BWY 400x300px"
    {0x35, {2, 600, 448}},     //"6.0\" BWR 600x448px"
    {0x3D, {2, 600, 448}},     //"6.0\" BWY 600x448px"
    {0x36, {2, 880, 528}},     //"7.5\" BWR 880x528px"
    {0x3E, {2, 880, 528}},     //"7.5\" BWY 880x528px"
    {0x37, {2, 640, 960}},     //"11.6\" BWR 640x960px"
    {0x3F, {2, 640, 960}},     //"11.6\" BWY 640x960px"
};

void contentRunner() {
    if (config.runStatus == RUNSTATUS_STOP) return;

    time_t now;
    time(&now);

    // xSemaphoreTake(tagDBOwner, portMAX_DELAY);
    for (int16_t c = 0; c < tagDB.size(); c++) {
        tagRecord *taginfo = nullptr;
        taginfo = tagDB.at(c);

        if (taginfo->RSSI && (now >= taginfo->nextupdate || taginfo->wakeupReason == WAKEUP_REASON_GPIO || taginfo->wakeupReason == WAKEUP_REASON_NFC) && config.runStatus == RUNSTATUS_RUN && Storage.freeSpace() > 31000) {
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
    // xSemaphoreGive(tagDBOwner);
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
    imageParams.dither = false;
    if (taginfo->hasCustomLUT && taginfo->lut != 1) imageParams.grayLut = true;

    imageParams.invert = false;
    imageParams.symbols = 0;
    imageParams.rotate = taginfo->rotate;

    switch (taginfo->contentMode) {
        case 0:  // Image

            if (cfgobj["filename"].as<String>() && cfgobj["filename"].as<String>() != "null" && !cfgobj["#fetched"].as<bool>()) {
                if (cfgobj["dither"] && cfgobj["dither"] == "1") imageParams.dither = true;
                jpg2buffer(cfgobj["filename"].as<String>(), filename, imageParams);
                if (imageParams.hasRed) imageParams.dataType = DATATYPE_IMG_RAW_2BPP;
                if (prepareDataAvail(&filename, imageParams.dataType, mac, cfgobj["timetolive"].as<int>())) {
                    cfgobj["#fetched"] = true;
                    if (cfgobj["delete"].as<String>() == "1") contentFS->remove("/" + cfgobj["filename"].as<String>());
                } else {
                    wsErr("Error accessing " + filename);
                }
            }
            taginfo->nextupdate = 3216153600;
            break;

        case 1:  // Today

            drawDate(filename, taginfo, imageParams);
            taginfo->nextupdate = midnight;
            updateTagImage(filename, mac, (midnight - now) / 60 - 10, taginfo, imageParams);
            break;

        case 2:  // CountDays

            if (buttonPressed) cfgobj["counter"] = 0;
            drawNumber(filename, (int32_t)cfgobj["counter"], (int32_t)cfgobj["thresholdred"], taginfo, imageParams);
            taginfo->nextupdate = midnight;
            updateTagImage(filename, mac, (buttonPressed ? 0 : 15), taginfo, imageParams);
            cfgobj["counter"] = (int32_t)cfgobj["counter"] + 1;
            break;

        case 3:  // CountHours

            if (buttonPressed) cfgobj["counter"] = 0;
            drawNumber(filename, (int32_t)cfgobj["counter"], (int32_t)cfgobj["thresholdred"], taginfo, imageParams);
            taginfo->nextupdate = now + 3600;
            updateTagImage(filename, mac, (buttonPressed ? 0 : 5), taginfo, imageParams);
            cfgobj["counter"] = (int32_t)cfgobj["counter"] + 1;
            break;

        case 4:  // Weather

            // https://open-meteo.com/
            // https://geocoding-api.open-meteo.com/v1/search?name=eindhoven
            // https://api.open-meteo.com/v1/forecast?latitude=52.52&longitude=13.41&current_weather=true
            // https://github.com/erikflowers/weather-icons

            drawWeather(filename, cfgobj, taginfo, imageParams);
            taginfo->nextupdate = now + 1800;
            updateTagImage(filename, mac, 15, taginfo, imageParams);
            break;

        case 8:  // Forecast

            drawForecast(filename, cfgobj, taginfo, imageParams);
            taginfo->nextupdate = now + 3600;
            updateTagImage(filename, mac, 15, taginfo, imageParams);
            break;

        case 5:  // Firmware

            filename = cfgobj["filename"].as<String>();
            if (filename && filename != "null" && !cfgobj["#fetched"].as<bool>()) {
                if (prepareDataAvail(&filename, DATATYPE_FW_UPDATE, mac, cfgobj["timetolive"].as<int>())) {
                    cfgobj["#fetched"] = true;
                } else {
                    wsErr("Error accessing " + filename);
                }
                cfgobj["filename"] = "";
                taginfo->nextupdate = 3216153600;
                taginfo->contentMode = 0;
            } else {
                taginfo->nextupdate = now + 300;
            }
            break;

        case 7:  // ImageUrl

        {
            int httpcode = getImgURL(filename, cfgobj["url"], (time_t)cfgobj["#fetched"], imageParams, String(hexmac));
            if (httpcode == 200) {
                taginfo->nextupdate = now + 60 * (cfgobj["interval"].as<int>() < 3 ? 15 : cfgobj["interval"].as<int>());
                updateTagImage(filename, mac, cfgobj["interval"].as<int>(), taginfo, imageParams);
                cfgobj["#fetched"] = now;
            } else if (httpcode == 304) {
                taginfo->nextupdate = now + 60 * (cfgobj["interval"].as<int>() < 3 ? 15 : cfgobj["interval"].as<int>());
            } else {
                taginfo->nextupdate = now + 300;
            }
            break;
        }

        case 9:  // RSSFeed

            if (getRssFeed(filename, cfgobj["url"], cfgobj["title"], taginfo, imageParams)) {
                taginfo->nextupdate = now + 60 * (cfgobj["interval"].as<int>() < 3 ? 60 : cfgobj["interval"].as<int>());
                updateTagImage(filename, mac, cfgobj["interval"].as<int>(), taginfo, imageParams);
            } else {
                taginfo->nextupdate = now + 300;
            }
            break;

        case 10:  // QRcode:

            drawQR(filename, cfgobj["qr-content"], cfgobj["title"], taginfo, imageParams);
            taginfo->nextupdate = now + 12 * 3600;
            updateTagImage(filename, mac, 0, taginfo, imageParams);
            break;

        case 11:  // Calendar:

            if (getCalFeed(filename, cfgobj["apps_script_url"], cfgobj["title"], taginfo, imageParams)) {
                taginfo->nextupdate = now + 60 * (cfgobj["interval"].as<int>() < 3 ? 15 : cfgobj["interval"].as<int>());
                updateTagImage(filename, mac, cfgobj["interval"].as<int>(), taginfo, imageParams);
            } else {
                taginfo->nextupdate = now + 300;
            }
            break;

        case 12:  // RemoteAP

            taginfo->nextupdate = 3216153600;
            break;

        case 13:  // SegStatic

            sprintf(buffer, "%-4.4s%-2.2s%-4.4s", cfgobj["line1"].as<const char *>(), cfgobj["line2"].as<const char *>(), cfgobj["line3"].as<const char *>());
            taginfo->nextupdate = 3216153600;
            sendAPSegmentedData(mac, (String)buffer, 0x0000, false, (taginfo->isExternal == false));
            break;

        case 14:  // NFC URL

            taginfo->nextupdate = 3216153600;
            prepareNFCReq(mac, cfgobj["url"].as<const char *>());
            break;

        case 15:  // send gray LUT

            taginfo->nextupdate = 3216153600;
            prepareLUTreq(mac, cfgobj["bytes"]);
            taginfo->hasCustomLUT = true;
            break;

        case 16:  // buienradar

            {
                uint8_t refresh = drawBuienradar(filename, cfgobj, taginfo, imageParams);
                taginfo->nextupdate = now + refresh * 60;
                updateTagImage(filename, mac, refresh, taginfo, imageParams);
                break;
            }

        case 17:  // tag command
            sendTagCommand(mac, cfgobj["cmd"].as<int>(), (taginfo->isExternal == false));
            cfgobj["filename"] = "";
            taginfo->nextupdate = 3216153600;
            taginfo->contentMode = 0;
            break;

        case 18:  // tag config
            prepareConfigFile(mac, cfgobj);
            cfgobj["filename"] = "";
            taginfo->nextupdate = 3216153600;
            taginfo->contentMode = 0;
            break;

        case 19:  // json template
        {
            DynamicJsonDocument doc(5000);
            if (cfgobj["filename"]) {
                File file = contentFS->open("/" + String(hexmac) + ".json", "r");
                if (file) {
                    DeserializationError error = deserializeJson(doc, file);
                    if (error) {
                        wsErr(error.c_str());
                    } else {
                        taginfo->nextupdate = 3216153600;
                        drawJsonTemplate(doc, filename, taginfo, imageParams);
                        updateTagImage(filename, mac, 0, taginfo, imageParams);
                    }
                } else {
                    wsErr("json file not found");
                }
            } else {
                int httpcode = getJsonTemplate(cfgobj["url"], doc, (time_t)cfgobj["#fetched"], String(hexmac));
                if (httpcode == 200) {
                    taginfo->nextupdate = now + 60 * (cfgobj["interval"].as<int>() < 3 ? 15 : cfgobj["interval"].as<int>());
                    drawJsonTemplate(doc, filename, taginfo, imageParams);
                    updateTagImage(filename, mac, cfgobj["interval"].as<int>(), taginfo, imageParams);
                    cfgobj["#fetched"] = now;
                } else if (httpcode == 304) {
                    taginfo->nextupdate = now + 60 * (cfgobj["interval"].as<int>() < 3 ? 15 : cfgobj["interval"].as<int>());
                } else {
                    taginfo->nextupdate = now + 300;
                }
            }
            break;
        }
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

void drawString(TFT_eSprite &spr, String content, int16_t posx, int16_t posy, String font, byte align, uint16_t color, uint16_t size) {
    // drawString(spr,"test",100,10,"bahnschrift30",TC_DATUM,PAL_RED);
    if (font != "" && !font.startsWith("fonts/") && !font.startsWith("/fonts/")) {

        // u8g2 font
        U8g2_for_TFT_eSPI u8f;
        u8f.begin(spr);
        setU8G2Font(font, u8f);
        u8f.setForegroundColor(color);
        u8f.setBackgroundColor(PAL_WHITE);
        u8f.setCursor(posx, posy);
        u8f.print(content);

    } else if (size > 0) {

        // truetype
        time_t t = millis();
        truetypeClass truetype = truetypeClass();
        void *framebuffer = spr.getPointer();
        truetype.setFramebuffer(spr.width(), spr.height(), spr.getColorDepth(), static_cast<uint8_t *>(framebuffer));
        File fontFile = contentFS->open(font, "r");
        if (!truetype.setTtfFile(fontFile)) {
            Serial.println("read ttf failed");
            return;
        }

        truetype.setCharacterSize(size);
        truetype.setCharacterSpacing(0);
        if (align == TC_DATUM) {
            posx -= truetype.getStringWidth(content) / 2;
        }
        if (align == TR_DATUM) {
            posx -= truetype.getStringWidth(content);
        }
        truetype.setTextBoundary(posx, spr.width(), spr.height());
        truetype.setTextColor(spr.color16to8(color), spr.color16to8(color));
        truetype.textDraw(posx, posy, content);
        truetype.end();
        Serial.println("text: '" + content + "' " + String(millis() - t) + "ms"); 

    } else {

        // vlw bitmap font
        spr.setTextDatum(align);
        if (font != "") spr.loadFont(font, *contentFS);
        spr.setTextColor(color, PAL_WHITE);
        spr.drawString(content, posx, posy);
        if (font != "") spr.unloadFont();

    }
}

void initSprite(TFT_eSprite &spr, int w, int h, imgParam &imageParams) {
    spr.setColorDepth(8);
    spr.createSprite(w, h);
    if (spr.getPointer() == nullptr) {
        wsErr("low on memory. Fallback to 1bpp");
        Serial.println("Maximum Continuous Heap Space: " + String(heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT)));
        spr.setColorDepth(1);
        spr.setBitmapColor(TFT_WHITE, TFT_BLACK);
        imageParams.bpp = 1;
        spr.createSprite(w, h);
    }
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

    StaticJsonDocument<512> loc;
    getTemplate(loc, TEMPLATE, 1, hwdata[taginfo->hwType].basetype);

    initSprite(spr, hwdata[taginfo->hwType].width, hwdata[taginfo->hwType].height, imageParams);

    if (loc["date"]) {
        drawString(spr, languageDays[getCurrentLanguage()][timeinfo.tm_wday], loc["weekday"][0], loc["weekday"][1], loc["weekday"][2], TC_DATUM, PAL_RED);
        drawString(spr, String(timeinfo.tm_mday) + " " + languageMonth[getCurrentLanguage()][timeinfo.tm_mon], loc["date"][0], loc["date"][1], loc["date"][2], TC_DATUM);
    } else {
        drawString(spr, languageDays[getCurrentLanguage()][timeinfo.tm_wday], loc["weekday"][0], loc["weekday"][1], loc["weekday"][2], TC_DATUM, PAL_BLACK);
        drawString(spr, String(languageMonth[getCurrentLanguage()][timeinfo.tm_mon]), loc["month"][0], loc["month"][1], loc["month"][2], TC_DATUM);
        drawString(spr, String(timeinfo.tm_mday), loc["day"][0], loc["day"][1], loc["day"][2], TC_DATUM, PAL_RED);
    }

    spr2buffer(spr, filename, imageParams);
    spr.deleteSprite();
}

void drawNumber(String &filename, int32_t count, int32_t thresholdred, tagRecord *&taginfo, imgParam &imageParams) {
    if (taginfo->hwType == SOLUM_SEG_UK) {
        imageParams.symbols = 0x00;
        if (count > 19999) {
            sprintf(imageParams.segments, "over  flow");
            return;
        } else if (count > 9999) {
            imageParams.symbols = 0x02;
            sprintf(imageParams.segments, "%04d", count - 10000);
        } else {
            sprintf(imageParams.segments, "%4d", count);
        }
        if (taginfo->contentMode == 3) {
            strcat(imageParams.segments, "  hour");
        } else {
            strcat(imageParams.segments, "  days");
        }
        return;
    }

    TFT_eSPI tft = TFT_eSPI();
    TFT_eSprite spr = TFT_eSprite(&tft);

    StaticJsonDocument<512> loc;
    getTemplate(loc, TEMPLATE, 2, hwdata[taginfo->hwType].basetype);

    initSprite(spr, hwdata[taginfo->hwType].width, hwdata[taginfo->hwType].height, imageParams);
    spr.setTextDatum(MC_DATUM);
    if (count > thresholdred) {
        spr.setTextColor(PAL_RED, PAL_WHITE);
    } else {
        spr.setTextColor(PAL_BLACK, PAL_WHITE);
    }
    String font = loc["fonts"][0].as<String>();
    if (count > 99) font = loc["fonts"][1].as<String>();
    if (count > 999) font = loc["fonts"][2].as<String>();
    if (count > 9999) font = loc["fonts"][3].as<String>();
    spr.loadFont(font, *contentFS);
    spr.drawString(String(count), loc["xy"][0].as<uint16_t>(), loc["xy"][1].as<uint16_t>());
    spr.unloadFont();

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
        StaticJsonDocument<1000> doc;
        DeserializationError error = deserializeJson(doc, http.getString());
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

        StaticJsonDocument<512> loc;
        getTemplate(loc, TEMPLATE, 4, hwdata[taginfo->hwType].basetype);

        String weatherIcons[] = {"\uf00d", "\uf00c", "\uf002", "\uf013", "\uf013", "\uf014", "", "", "\uf014", "", "",
                                 "\uf01a", "", "\uf01a", "", "\uf01a", "\uf017", "\uf017", "", "", "",
                                 "\uf019", "", "\uf019", "", "\uf019", "\uf015", "\uf015", "", "", "",
                                 "\uf01b", "", "\uf01b", "", "\uf01b", "", "\uf076", "", "", "\uf01a",
                                 "\uf01a", "\uf01a", "", "", "\uf064", "\uf064", "", "", "", "",
                                 "", "", "", "", "\uf01e", "\uf01d", "", "", "\uf01e"};
        if (isday == 0) {
            weatherIcons[0] = "\uf02e";
            weatherIcons[1] = "\uf083";
            weatherIcons[2] = "\uf086";
        }

        TFT_eSPI tft = TFT_eSPI();
        TFT_eSprite spr = TFT_eSprite(&tft);
        tft.setTextWrap(false, false);

        initSprite(spr, hwdata[taginfo->hwType].width, hwdata[taginfo->hwType].height, imageParams);
        drawString(spr, cfgobj["location"], loc["location"][0], loc["location"][1], loc["location"][2]);
        drawString(spr, String(wind), loc["wind"][0], loc["wind"][1], loc["wind"][2], TR_DATUM, (wind > 4 ? PAL_RED : PAL_BLACK));

        char tmpOutput[5];
        dtostrf(temperature, 2, 1, tmpOutput);
        drawString(spr, String(tmpOutput), loc["temp"][0], loc["temp"][1], loc["temp"][2], TL_DATUM, (temperature < 0 ? PAL_RED : PAL_BLACK));

        int iconcolor = PAL_BLACK;
        if (weathercode == 55 || weathercode == 65 || weathercode == 75 || weathercode == 82 || weathercode == 86 || weathercode == 95 || weathercode == 96 || weathercode == 99) {
            iconcolor = PAL_RED;
        }
        drawString(spr, weatherIcons[weathercode], loc["icon"][0], loc["icon"][1], "/fonts/weathericons.ttf", loc["icon"][3], iconcolor, loc["icon"][2]);
        drawString(spr, windDirectionIcon(winddirection), loc["dir"][0], loc["dir"][1], "/fonts/weathericons.ttf", TC_DATUM, PAL_BLACK, loc["dir"][2]);
        if (weathercode > 10) {
            drawString(spr, "\uf084", loc["umbrella"][0], loc["umbrella"][1], "/fonts/weathericons.ttf", TC_DATUM, PAL_RED, loc["umbrella"][2]);
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
        DynamicJsonDocument doc(2000);
        DeserializationError error = deserializeJson(doc, http.getString());
        if (error) {
            Serial.println(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
        }

        String weatherIcons[] = {"\uf00d", "\uf00c", "\uf002", "\uf013", "\uf013", "\uf014", "", "", "\uf014", "", "",
                                 "\uf01a", "", "\uf01a", "", "\uf01a", "\uf017", "\uf017", "", "", "",
                                 "\uf019", "", "\uf019", "", "\uf019", "\uf015", "\uf015", "", "", "",
                                 "\uf01b", "", "\uf01b", "", "\uf01b", "", "\uf076", "", "", "\uf01a",
                                 "\uf01a", "\uf01a", "", "", "\uf064", "\uf064", "", "", "", "",
                                 "", "", "", "", "\uf01e", "\uf01d", "", "", "\uf01e"};

        tft.setTextWrap(false, false);

        StaticJsonDocument<512> loc;
        getTemplate(loc, TEMPLATE, 8, hwdata[taginfo->hwType].basetype);
        initSprite(spr, hwdata[taginfo->hwType].width, hwdata[taginfo->hwType].height, imageParams);

        drawString(spr, cfgobj["location"], loc["location"][0], loc["location"][1], loc["location"][2], TL_DATUM, PAL_BLACK);
        for (uint8_t dag = 0; dag < loc["column"][0]; dag++) {
            time_t weatherday = doc["daily"]["time"][dag].as<time_t>();
            struct tm *datum = localtime(&weatherday);

            drawString(spr, String(languageDaysShort[getCurrentLanguage()][datum->tm_wday]), dag * loc["column"][1].as<int>() + loc["day"][0].as<int>(), loc["day"][1], loc["day"][2], TC_DATUM, PAL_BLACK);

            uint8_t weathercode = doc["daily"]["weathercode"][dag].as<int>();
            if (weathercode > 40) weathercode -= 40;
            
            int iconcolor = PAL_BLACK;
            if (weathercode == 55 || weathercode == 65 || weathercode == 75 || weathercode == 82 || weathercode == 86 || weathercode == 95 || weathercode == 96 || weathercode == 99) {
                iconcolor = PAL_RED;
            }
            drawString(spr, weatherIcons[weathercode], loc["icon"][0].as<int>() + dag * loc["column"][1].as<int>(), loc["icon"][1], "/fonts/weathericons.ttf", TC_DATUM, iconcolor, loc["icon"][2]);

            drawString(spr, windDirectionIcon(doc["daily"]["winddirection_10m_dominant"][dag]), loc["wind"][0].as<int>() + dag * loc["column"][1].as<int>(), loc["wind"][1], "/fonts/weathericons.ttf", TC_DATUM, PAL_BLACK, loc["icon"][2]);

            int8_t tmin = round(doc["daily"]["temperature_2m_min"][dag].as<double>());
            int8_t tmax = round(doc["daily"]["temperature_2m_max"][dag].as<double>());
            uint8_t wind = windSpeedToBeaufort(doc["daily"]["windspeed_10m_max"][dag].as<double>());

            spr.loadFont(loc["day"][2], *contentFS);

            if (loc["rain"]) {
                int8_t rain = round(doc["daily"]["precipitation_sum"][dag].as<double>());
                if (rain > 0) {
                    drawString(spr, String(rain) + "mm", dag * loc["column"][1].as<int>() + loc["rain"][0].as<int>(), loc["rain"][1], "", TC_DATUM, (rain > 10 ? PAL_RED : PAL_BLACK));
                }
            }

            drawString(spr, String(tmin) + " ", dag * loc["column"][1].as<int>() + loc["day"][0].as<int>(), loc["day"][4], "", TR_DATUM, (tmin < 0 ? PAL_RED : PAL_BLACK));
            drawString(spr, String(" ") + String(tmax), dag * loc["column"][1].as<int>() + loc["day"][0].as<int>(), loc["day"][4], "", TL_DATUM, (tmax < 0 ? PAL_RED : PAL_BLACK));
            drawString(spr, String(" ") + String(wind), dag * loc["column"][1].as<int>() + loc["day"][0].as<int>(), loc["day"][3], "", TL_DATUM, (wind > 5 ? PAL_RED : PAL_BLACK));
            spr.unloadFont();
            if (dag > 0) {
                for (int i = loc["line"][0]; i < loc["line"][1]; i += 3) {
                    spr.drawPixel(dag * loc["column"][1].as<int>(), i, PAL_BLACK);
                }
            }
        }

        spr2buffer(spr, filename, imageParams);
        spr.deleteSprite();
    }
    http.end();
}

int getImgURL(String &filename, String URL, time_t fetched, imgParam &imageParams, String MAC) {
    // https://images.klari.net/kat-bw29.jpg

    Storage.begin();

    Serial.println("get external " + URL);
    HTTPClient http;
    http.begin(URL);
    http.addHeader("If-Modified-Since", formatHttpDate(fetched));
    http.addHeader("X-ESL-MAC", MAC);
    http.setTimeout(5000);  // timeout in ms
    int httpCode = http.GET();
    if (httpCode == 200) {
        File f = contentFS->open("/temp/temp.jpg", "w");
        if (f) {
            http.writeToStream(&f);
            f.close();
            jpg2buffer("/temp/temp.jpg", filename, imageParams);
        }
    } else {
        if (httpCode != 304) {
            wsErr("http " + URL + " " + String(httpCode));
        } else {
            Serial.println("http 304, image is not changed " + URL);
        }
    }
    http.end();
    return httpCode;
}

#ifdef CONTENT_RSS
rssClass reader;
#endif

bool getRssFeed(String &filename, String URL, String title, tagRecord *&taginfo, imgParam &imageParams) {
#ifdef CONTENT_RSS
    // https://github.com/garretlab/shoddyxml2

    // http://feeds.feedburner.com/tweakers/nieuws
    // https://www.nu.nl/rss/Algemeen

    Serial.println("RSS feed");
    const char *url = URL.c_str();
    const char *tag = "title";
    const int rssArticleSize = 128;

    TFT_eSPI tft = TFT_eSPI();
    TFT_eSprite spr = TFT_eSprite(&tft);
    U8g2_for_TFT_eSPI u8f;
    u8f.begin(spr);

    StaticJsonDocument<512> loc;
    getTemplate(loc, TEMPLATE, 9, hwdata[taginfo->hwType].basetype);
    initSprite(spr, hwdata[taginfo->hwType].width, hwdata[taginfo->hwType].height, imageParams);

    if (title == "" || title == "null") title = "RSS feed";
    drawString(spr, title, loc["title"][0], loc["title"][1], loc["title"][2], TL_DATUM, PAL_BLACK);

    setU8G2Font(loc["font"], u8f);
    u8f.setFontMode(0);
    u8f.setFontDirection(0);
    u8f.setForegroundColor(PAL_BLACK);
    u8f.setBackgroundColor(PAL_WHITE);

    int n = reader.getArticles(url, tag, rssArticleSize, loc["items"]);
    for (int i = 0; i < n; i++) {
        u8f.setCursor(loc["line"][0], loc["line"][1].as<int>() + i * loc["line"][2].as<int>());
        u8f.print(reader.itemData[i]);
    }

    spr2buffer(spr, filename, imageParams);
    spr.deleteSprite();
#endif

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
#ifdef CONTENT_CAL
    // google apps scripts method to retrieve calendar
    // see /data/calendar.txt for description

    wsLog("get calendar");

    time_t now;
    time(&now);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    static char dateString[40];
    strftime(dateString, sizeof(dateString), "%d.%m.%Y", &timeinfo);

    HTTPClient http;
    http.begin(URL);
    http.setTimeout(10000);
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

    StaticJsonDocument<512> loc;
    getTemplate(loc, TEMPLATE, 11, hwdata[taginfo->hwType].basetype);
    initSprite(spr, hwdata[taginfo->hwType].width, hwdata[taginfo->hwType].height, imageParams);

    if (title == "" || title == "null") title = "Calendar";
    drawString(spr, title, loc["title"][0], loc["title"][1], loc["title"][2], TL_DATUM, PAL_BLACK);
    drawString(spr, dateString, loc["date"][0], loc["date"][1], loc["title"][2], TR_DATUM, PAL_BLACK);

    u8f.setFontMode(0);
    u8f.setFontDirection(0);
    int n = doc.size();
    if (n > loc["items"]) n = loc["items"];
    for (int i = 0; i < n; i++) {
        JsonObject obj = doc[i];
        String eventtitle = obj["title"];
        time_t starttime = obj["start"];
        time_t endtime = obj["end"];
        setU8G2Font(loc["line"][3], u8f);
        if (starttime <= now && endtime > now) {
            u8f.setForegroundColor(PAL_WHITE);
            u8f.setBackgroundColor(PAL_RED);
            spr.fillRect(loc["red"][0], loc["red"][1].as<int>() + i * loc["line"][2].as<int>(), loc["red"][2], loc["red"][3], PAL_RED);
        } else {
            u8f.setForegroundColor(PAL_BLACK);
            u8f.setBackgroundColor(PAL_WHITE);
        }
        u8f.setCursor(loc["line"][0], loc["line"][1].as<int>() + i * loc["line"][2].as<int>());
        if (starttime > 0) u8f.print(epoch_to_display(obj["start"]));
        u8f.setCursor(loc["line"][4], loc["line"][1].as<int>() + i * loc["line"][2].as<int>());
        u8f.print(eventtitle);
    }

    spr2buffer(spr, filename, imageParams);
    spr.deleteSprite();
#endif
    return true;
}

void drawQR(String &filename, String qrcontent, String title, tagRecord *&taginfo, imgParam &imageParams) {
#ifdef CONTENT_QR
    TFT_eSPI tft = TFT_eSPI();
    TFT_eSprite spr = TFT_eSprite(&tft);
    Storage.begin();

    const char *text = qrcontent.c_str();
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(2)];
    // https://github.com/ricmoo/QRCode
    qrcode_initText(&qrcode, qrcodeData, 2, ECC_MEDIUM, text);
    int size = qrcode.size;
    int xpos = 0, ypos = 0, dotsize = 1;

    StaticJsonDocument<512> loc;
    getTemplate(loc, TEMPLATE, 10, hwdata[taginfo->hwType].basetype);
    initSprite(spr, hwdata[taginfo->hwType].width, hwdata[taginfo->hwType].height, imageParams);
    drawString(spr, title, loc["title"][0], loc["title"][1], loc["title"][2]);

    dotsize = int((hwdata[taginfo->hwType].height - loc["pos"][1].as<int>()) / size);
    xpos = loc["pos"][0].as<int>() - dotsize * size / 2;
    ypos = loc["pos"][1];

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if (qrcode_getModule(&qrcode, x, y)) {
                spr.fillRect(xpos + x * dotsize, ypos + y * dotsize, dotsize, dotsize, PAL_BLACK);
            }
        }
    }

    spr2buffer(spr, filename, imageParams);
    spr.deleteSprite();
#endif
}

uint8_t drawBuienradar(String &filename, JsonObject &cfgobj, tagRecord *&taginfo, imgParam &imageParams) {
    uint8_t refresh = 60;
#ifdef CONTENT_BUIENRADAR
    wsLog("get weather");

    getLocation(cfgobj);
    HTTPClient http;

    String lat = cfgobj["#lat"];
    String lon = cfgobj["#lon"];
    http.begin("https://gps.buienradar.nl/getrr.php?lat=" + lat + "&lon=" + lon);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    http.setTimeout(5000);
    int httpCode = http.GET();
    Serial.printf("Got code %d for Buienradar\n", httpCode);

    if (httpCode == 200) {
        TFT_eSPI tft = TFT_eSPI();
        TFT_eSprite spr = TFT_eSprite(&tft);
        U8g2_for_TFT_eSPI u8f;
        u8f.begin(spr);

        StaticJsonDocument<512> loc;
        getTemplate(loc, TEMPLATE, 16, hwdata[taginfo->hwType].basetype);
        initSprite(spr, hwdata[taginfo->hwType].width, hwdata[taginfo->hwType].height, imageParams);

        tft.setTextWrap(false, false);

        String response = http.getString();

        drawString(spr, cfgobj["location"], loc["location"][0], loc["location"][1], loc["location"][2]);

        for (int i = 0; i < 295; i += 4) {
            spr.drawPixel(i, 110, PAL_BLACK);
            spr.drawPixel(i, 91, PAL_BLACK);
            spr.drawPixel(i, 82, PAL_BLACK);
            spr.drawPixel(i, 72, PAL_BLACK);
            spr.drawPixel(i, 62, PAL_BLACK);
            spr.drawPixel(i, 56, PAL_BLACK);
            spr.drawPixel(i, 52, PAL_BLACK);
        }

        setU8G2Font(loc["title"][2], u8f);
        u8f.setFontMode(0);
        u8f.setFontDirection(0);
        u8f.setForegroundColor(PAL_BLACK);
        u8f.setBackgroundColor(PAL_WHITE);
        u8f.setCursor(loc["title"][0], loc["title"][1]);
        u8f.print("Buienradar");

        for (int i = 0; i < 24; i++) {
            int startPos = i * 11;
            uint8_t value = response.substring(startPos, startPos + 3).toInt();
            String timestring = response.substring(startPos + 4, startPos + 9);
            int minutes = timestring.substring(3).toInt();
            if (value < 70) value = 70;
            if (value > 180) value = 180;
            if (value > 70 && i < 12) refresh = 5;
            if (value > 70 && refresh > 5) refresh = 15;

            spr.fillRect(i * loc["cols"][2].as<int>() + loc["bars"][0].as<int>(), loc["bars"][1].as<int>() - (value - 70), loc["bars"][2], (value - 70), (value > 130 ? PAL_RED : PAL_BLACK));

            if (minutes % 15 == 0) {
                drawString(spr, timestring, i * loc["cols"][2].as<int>() + loc["cols"][0].as<int>(), loc["cols"][1], loc["cols"][3]);
            }
        }

        spr2buffer(spr, filename, imageParams);
        spr.deleteSprite();
    }
    http.end();
#endif
    return refresh;
}

int getJsonTemplate(String URL, JsonDocument &jsondoc, time_t fetched, String MAC) {
    Serial.println("get external " + URL);
    HTTPClient http;
    http.useHTTP10(true);
    http.begin(URL);
    http.addHeader("If-Modified-Since", formatHttpDate(fetched));
    http.addHeader("X-ESL-MAC", MAC);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    http.setTimeout(5000);
    int httpCode = http.GET();
    if (httpCode == 200) {
        DeserializationError error = deserializeJson(jsondoc, http.getStream());
        if (error) {
            wsErr("json error " + String(error.c_str()));
            return 0;
        }
    } else {
        if (httpCode != 304) {
            wsErr("http " + URL + " status " + String(httpCode));
        } else {
            Serial.println("http 304, image is not changed " + URL);
        }
    }
    http.end();
    return httpCode;
}

void drawJsonTemplate(JsonDocument &doc, String filename, tagRecord *&taginfo, imgParam &imageParams) {
    TFT_eSPI tft = TFT_eSPI();
    TFT_eSprite spr = TFT_eSprite(&tft);
    initSprite(spr, hwdata[taginfo->hwType].width, hwdata[taginfo->hwType].height, imageParams);

    for (const JsonVariant &element : doc.as<JsonArray>()) {
        drawElement(element.as<JsonObject>(), spr);
    }
    spr2buffer(spr, filename, imageParams);
    spr.deleteSprite();
}

void drawElement(const JsonObject &element, TFT_eSprite &spr) {
    if (element.containsKey("text")) {
        const JsonArray &textArray = element["text"];
        uint16_t color = textArray[4] | 1;
        uint16_t align = textArray[5] | 0;
        uint16_t size = textArray[6] | 0;
        drawString(spr, textArray[2], textArray[0].as<int>(), textArray[1].as<int>(), textArray[3], align, getColor(color), size);
    } else if (element.containsKey("box")) {
        const JsonArray &boxArray = element["box"];
        uint16_t color = boxArray[4] | 1;
        spr.fillRect(boxArray[0].as<int>(), boxArray[1].as<int>(), boxArray[2].as<int>(), boxArray[3].as<int>(), getColor(color));
    } else if (element.containsKey("line")) {
        const JsonArray &lineArray = element["line"];
        uint16_t color = lineArray[4] | 1;
        spr.drawLine(lineArray[0].as<int>(), lineArray[1].as<int>(), lineArray[2].as<int>(), lineArray[3].as<int>(), getColor(color));
    } else if (element.containsKey("triangle")) {
        const JsonArray &lineArray = element["triangle"];
        uint16_t color = lineArray[6] | 1;
        spr.fillTriangle(lineArray[0].as<int>(), lineArray[1].as<int>(), lineArray[2].as<int>(), lineArray[3].as<int>(), lineArray[4].as<int>(), lineArray[5].as<int>(), getColor(color));
    }
}

uint16_t getColor(uint8_t color) {
    switch (color) {
        case 0:
            return PAL_WHITE;
        case 1:
            return PAL_BLACK;
        case 2:
            return PAL_RED;
    }
    return PAL_WHITE;
}

char *formatHttpDate(time_t t) {
    static char buf[40];
    struct tm *timeinfo;
    timeinfo = localtime(&t);           // Get the local time
    time_t utcTime = mktime(timeinfo);  // Convert to UTC
    timeinfo = gmtime(&utcTime);
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

void prepareNFCReq(uint8_t *dst, const char *url) {
    uint8_t *data;
    size_t len = strlen(url);
    data = new uint8_t[len + 8];

    // TLV
    data[0] = 0x03;  // NDEF message (TLV type)
    data[1] = 4 + len + 1;
    // ndef record
    data[2] = 0xD1;
    data[3] = 0x01;     // well known record type
    data[4] = len + 1;  // payload length
    data[5] = 0x55;     // payload type (URI record)
    data[6] = 0x00;     // URI identifier code (no prepending)

    memcpy(data + 7, reinterpret_cast<const uint8_t *>(url), len);
    len = 7 + len;
    data[len] = 0xFE;
    len = 1 + len;
    prepareDataAvail(data, len, DATATYPE_NFC_RAW_CONTENT, dst);
}

void prepareLUTreq(uint8_t *dst, String input) {
    const char *delimiters = ", \t";
    const int maxValues = 76;
    uint8_t waveform[maxValues];
    char *ptr = strtok(const_cast<char *>(input.c_str()), delimiters);
    int i = 0;
    while (ptr != nullptr && i < maxValues) {
        waveform[i++] = static_cast<uint8_t>(strtol(ptr, nullptr, 16));
        ptr = strtok(nullptr, delimiters);
    }
    Serial.println(String(i) + " bytes found");
    size_t waveformLen = sizeof(waveform);
    prepareDataAvail(waveform, waveformLen, DATATYPE_CUSTOM_LUT_OTA, dst);
}

void prepareConfigFile(uint8_t *dst, JsonObject config) {
    struct tagsettings tagSettings;
    tagSettings.settingsVer = 1;
    tagSettings.enableFastBoot = config["fastboot"].as<int>();
    tagSettings.enableRFWake = config["rfwake"].as<int>();
    tagSettings.enableTagRoaming = config["tagroaming"].as<int>();
    tagSettings.enableScanForAPAfterTimeout = config["tagscanontimeout"].as<int>();
    tagSettings.enableLowBatSymbol = config["showlowbat"].as<int>();
    tagSettings.enableNoRFSymbol = config["shownorf"].as<int>();
    tagSettings.customMode = 0;
    tagSettings.fastBootCapabilities = 0;
    tagSettings.minimumCheckInTime = 1;
    tagSettings.fixedChannel = config["fixedchannel"].as<int>();
    tagSettings.batLowVoltage = config["lowvoltage"].as<int>();
    prepareDataAvail((uint8_t *)&tagSettings, sizeof(tagSettings), 0xA8, dst);
}

void getTemplate(JsonDocument &json, const char *filePath, uint8_t id, uint8_t hwtype) {
    File jsonFile = contentFS->open(filePath, "r");
    if (!jsonFile) {
        Serial.println("Failed to open content template file " + String(filePath));
        return;
    }

    StaticJsonDocument<50> filter;
    filter[String(id)][String(hwtype)] = true;

    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, jsonFile, DeserializationOption::Filter(filter));
    jsonFile.close();

    if (error) {
        Serial.println("json error in getTemplate:");
        Serial.println(error.c_str());
        return;
    }

    json.set(doc[String(id)][String(hwtype)]);
}

void setU8G2Font(const String &title, U8g2_for_TFT_eSPI &u8f) {
    if (title == "glasstown_nbp_tf") u8f.setFont(u8g2_font_glasstown_nbp_tf);
    if (title == "7x14_tf") u8f.setFont(u8g2_font_7x14_tf);
    if (title == "t0_14b_tf") u8f.setFont(u8g2_font_t0_14b_tf);
}

void showIpAddress(String dst) {
    uint8_t mac[8];
    if (hex2mac(dst, mac)) {
        tagRecord *taginfo = nullptr;
        taginfo = tagRecord::findByMAC(mac);
        if (taginfo != nullptr) {
            String json = String("[");
            json += String("{\"text\": [0,5,\"OpenEPaperLink\",\"fonts/bahnschrift20\",2]}");
            json += String(",");
            json += String("{\"text\": [0,25,\"MAC:\",\"fonts/bahnschrift20\",1]}");
            json += String(",");
            json += String("{\"text\": [10,55,\"") + dst + String("\",\"glasstown_nbp_tf\",1]}");

            if ((uint32_t)WiFi.localIP() == (uint32_t)0) {
                json += String(",");
                json += String("{\"text\": [0,65,\"Connect to my \",\"fonts/bahnschrift20\",1]}");
                json += String(",");
                json += String("{\"text\": [0,85,\"WiFi, browse to:\",\"fonts/bahnschrift20\",1]}");
                json += String(",");
                json += String("{\"text\": [0,105,\"192.168.4.1/setup\",\"fonts/bahnschrift20\",1]}");
                json += String(",");
                json += String("{\"text\": [0,125,\"to configure me\",\"fonts/bahnschrift20\",1]}");
            } else {
                json += String(",");
                json += String("{\"text\": [0,65,\"IP:\",\"fonts/bahnschrift20\",1]}");
                json += String(",");
                json += String("{\"text\": [0,85,\"") + WiFi.localIP().toString() + String("\",\"fonts/bahnschrift20\",1]}");
            }

            json += String("]");
            File file = LittleFS.open("/" + dst + ".json", "w");
            if (!file) {
                Serial.print("Failed to create file\n");
                return;
            }
            file.print(json);
            file.close();
            taginfo->modeConfigJson = "{\"filename\":\"" + dst + ".json\"}";
            taginfo->contentMode = 19;
            taginfo->nextupdate = 0;
        }
    }
}

