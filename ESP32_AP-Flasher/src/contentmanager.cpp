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
#include "truetype.h"
#include "util.h"
#include "web.h"

// https://csvjson.com/json_beautifier

void contentRunner() {
    if (config.runStatus == RUNSTATUS_STOP) return;

    time_t now;
    time(&now);

    for (tagRecord *taginfo : tagDB) {
        if (taginfo->RSSI && (now >= taginfo->nextupdate || taginfo->wakeupReason == WAKEUP_REASON_GPIO || taginfo->wakeupReason == WAKEUP_REASON_NFC) && config.runStatus == RUNSTATUS_RUN && Storage.freeSpace() > 31000) {
            drawNew(taginfo->mac, (taginfo->wakeupReason == WAKEUP_REASON_GPIO), taginfo);
            taginfo->wakeupReason = 0;
        }

        if (taginfo->expectedNextCheckin > now - 10 && taginfo->expectedNextCheckin < now + 30 && taginfo->pendingIdle == 0 && taginfo->pending == false) {
            uint16_t minutesUntilNextUpdate = (taginfo->nextupdate - now) / 60;
            if (minutesUntilNextUpdate > config.maxsleep) {
                minutesUntilNextUpdate = config.maxsleep;
            }
            if (minutesUntilNextUpdate > 1 && (wsClientCount() == 0 || config.stopsleep == 0)) {
                taginfo->pendingIdle = minutesUntilNextUpdate;
                if (taginfo->isExternal == false) {
                    prepareIdleReq(taginfo->mac, minutesUntilNextUpdate);
                }
            }
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);  // add a small delay to allow other threads to run
    }
}

void checkVars() {
    DynamicJsonDocument cfgobj(500);
    for (tagRecord *tag : tagDB) {
        if (tag->contentMode == 19) {
            deserializeJson(cfgobj, tag->modeConfigJson);
            if (cfgobj["filename"]) {
                const String jsonfile = cfgobj["filename"].as<String>();
                File file = contentFS->open(jsonfile, "r");
                if (file) {
                    const size_t fileSize = file.size();
                    std::unique_ptr<char[]> fileContent(new char[fileSize + 1]);
                    file.readBytes(fileContent.get(), fileSize);
                    file.close();
                    fileContent[fileSize] = '\0';
                    const char *contentPtr = fileContent.get();
                    for (const auto &entry : varDB) {
                        if (entry.second.changed && strstr(contentPtr, entry.first.c_str()) != nullptr) {
                            Serial.println("updating " + jsonfile + " because of var " + entry.first.c_str());
                            tag->nextupdate = 0;
                        }
                    }
                }
                file.close();
            }
        }
        if (tag->contentMode == 21) {
            if (varDB["ap_tagcount"].changed || varDB["ap_ip"].changed || varDB["ap_ch"].changed) {
                tag->nextupdate = 0;
            }
        }
    }
    for (const auto &entry : varDB) {
        if (entry.second.changed) varDB[entry.first].changed = false;
    }
}

/// @brief Draw a counter
/// @param mac Destination mac
/// @param buttonPressed Was the button pressed (true) or not (false)
/// @param taginfo Tag information
/// @param cfgobj Tag config as json object
/// @param filename Filename
/// @param imageParams Image parameters
/// @param nextupdate Next counter update
/// @param nextCheckin Next tag checkin
void drawCounter(const uint8_t mac[8], const bool buttonPressed, tagRecord *&taginfo, JsonObject &cfgobj, String &filename, imgParam &imageParams, const uint32_t nextupdate, const uint16_t nextCheckin) {
    int32_t counter = cfgobj["counter"].as<int32_t>();
    if (buttonPressed) {
        counter = 0;
    }
    drawNumber(filename, counter, (int32_t)cfgobj["thresholdred"], taginfo, imageParams);
    taginfo->nextupdate = nextupdate;
    updateTagImage(filename, mac, (buttonPressed ? 0 : nextCheckin), taginfo, imageParams);
    cfgobj["counter"] = counter + 1;
}

void drawNew(const uint8_t mac[8], const bool buttonPressed, tagRecord *&taginfo) {
    time_t now;
    time(&now);

    const HwType hwdata = getHwType(taginfo->hwType);
    if (hwdata.bpp == 0) {
        taginfo->nextupdate = now + 600;
        wsErr("No definition found for tag type " + String(taginfo->hwType));
        return;
    }

    uint8_t wifimac[8];
    WiFi.macAddress(wifimac);
    memset(&wifimac[6], 0, 2);

    const bool isAp = memcmp(mac, wifimac, 8) == 0;
    if ((taginfo->wakeupReason == WAKEUP_REASON_FIRSTBOOT || taginfo->wakeupReason == WAKEUP_REASON_WDT_RESET) && taginfo->contentMode == 0 && isAp) {
        taginfo->contentMode = 21;
        taginfo->nextupdate = 0;
    }

    char hexmac[17];
    mac2hex(mac, hexmac);
    String filename = "/" + String(hexmac) + ".raw";
#ifdef YELLOW_IPS_AP
    if (isAp) {
        filename = "direct";
    }
#endif

    struct tm time_info;
    getLocalTime(&time_info);
    time_info.tm_hour = time_info.tm_min = time_info.tm_sec = 0;
    time_info.tm_mday++;
    const time_t midnight = mktime(&time_info);

    DynamicJsonDocument doc(500);
    deserializeJson(doc, taginfo->modeConfigJson);
    JsonObject cfgobj = doc.as<JsonObject>();
    char buffer[64];

    wsLog("Updating " + String(hexmac));
    taginfo->nextupdate = now + 60;

    imgParam imageParams;

    imageParams.width = hwdata.width;
    imageParams.height = hwdata.height;
    imageParams.bpp = hwdata.bpp;
    imageParams.rotatebuffer = hwdata.rotatebuffer;

    imageParams.hasRed = false;
    imageParams.dataType = DATATYPE_IMG_RAW_1BPP;
    imageParams.dither = false;
    if (taginfo->hasCustomLUT && taginfo->lut != 1) imageParams.grayLut = true;

    imageParams.invert = false;
    imageParams.symbols = 0;
    imageParams.rotate = taginfo->rotate;

    switch (taginfo->contentMode) {
        case 0:  // Image
        {
            const String configFilename = cfgobj["filename"].as<String>();
            if (!configFilename && configFilename != "null" && !cfgobj["#fetched"].as<bool>()) {
                imageParams.dither = cfgobj["dither"] && cfgobj["dither"] == "1";
                jpg2buffer(configFilename, filename, imageParams);
                if (imageParams.hasRed) {
                    imageParams.dataType = DATATYPE_IMG_RAW_2BPP;
                }
                if (prepareDataAvail(&filename, imageParams.dataType, mac, cfgobj["timetolive"].as<int>())) {
                    cfgobj["#fetched"] = true;
                    if (cfgobj["delete"].as<String>() == "1") {
                        contentFS->remove("/" + configFilename);
                    }
                } else {
                    wsErr("Error accessing " + filename);
                }
            }
            taginfo->nextupdate = 3216153600;
        } break;

        case 1:  // Today

            drawDate(filename, taginfo, imageParams);
            taginfo->nextupdate = midnight;
            updateTagImage(filename, mac, (midnight - now) / 60 - 10, taginfo, imageParams);
            break;

        case 2:  // CountDays
            drawCounter(mac, buttonPressed, taginfo, cfgobj, filename, imageParams, midnight, 15);
            break;

        case 3:  // CountHours
            drawCounter(mac, buttonPressed, taginfo, cfgobj, filename, imageParams, now + 3600, 5);
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
            const int httpcode = getImgURL(filename, cfgobj["url"], (time_t)cfgobj["#fetched"], imageParams, String(hexmac));
            const int interval = cfgobj["interval"].as<int>();
            if (httpcode == 200) {
                taginfo->nextupdate = now + 60 * (interval < 3 ? 15 : interval);
                updateTagImage(filename, mac, interval, taginfo, imageParams);
                cfgobj["#fetched"] = now;
            } else if (httpcode == 304) {
                taginfo->nextupdate = now + 60 * (interval < 3 ? 15 : interval);
            } else {
                taginfo->nextupdate = now + 300;
            }
            break;
        }

        case 9:  // RSSFeed

            if (getRssFeed(filename, cfgobj["url"], cfgobj["title"], taginfo, imageParams)) {
                const int interval = cfgobj["interval"].as<int>();
                taginfo->nextupdate = now + 60 * (interval < 3 ? 60 : interval);
                updateTagImage(filename, mac, interval, taginfo, imageParams);
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
                const int interval = cfgobj["interval"].as<int>();
                taginfo->nextupdate = now + 60 * (interval < 3 ? 15 : interval);
                updateTagImage(filename, mac, interval, taginfo, imageParams);
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
            const uint8_t refresh = drawBuienradar(filename, cfgobj, taginfo, imageParams);
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
            const String configFilename = cfgobj["filename"].as<String>();
            if (configFilename) {
                const int result = getJsonTemplateFile(filename, configFilename, taginfo, imageParams);
                if (result) {
                    updateTagImage(filename, mac, cfgobj["interval"].as<int>(), taginfo, imageParams);
                } else {
                    wsErr("error opening file " + configFilename);
                }
                taginfo->nextupdate = 3216153600;
            } else {
                const int httpcode = getJsonTemplateUrl(filename, cfgobj["url"], (time_t)cfgobj["#fetched"], String(hexmac), taginfo, imageParams);
                const int interval = cfgobj["interval"].as<int>();
                if (httpcode == 200) {
                    taginfo->nextupdate = now + 60 * (interval < 3 ? 15 : interval);
                    updateTagImage(filename, mac, interval, taginfo, imageParams);
                    cfgobj["#fetched"] = now;
                } else if (httpcode == 304) {
                    taginfo->nextupdate = now + 60 * (interval < 3 ? 15 : interval);
                } else {
                    taginfo->nextupdate = now + 600;
                }
            }
            break;
        }

        case 20:  // display a copy
            break;

        case 21:  // ap info
            drawAPinfo(filename, cfgobj, taginfo, imageParams);
            updateTagImage(filename, mac, 0, taginfo, imageParams);
            taginfo->nextupdate = 3216153600;
            break;
    }

    taginfo->modeConfigJson = doc.as<String>();
}

bool updateTagImage(String &filename, const uint8_t *dst, uint16_t nextCheckin, tagRecord *&taginfo, imgParam &imageParams) {
    if (taginfo->hwType == SOLUM_SEG_UK) {
        sendAPSegmentedData(dst, (String)imageParams.segments, imageParams.symbols, imageParams.invert, (taginfo->isExternal == false));
    } else {
        if (imageParams.hasRed) imageParams.dataType = DATATYPE_IMG_RAW_2BPP;
        prepareDataAvail(&filename, imageParams.dataType, dst, nextCheckin);
    }
    return true;
}

uint8_t processFontPath(String &font) {
    if (font == "") return 3;
    if (font == "glasstown_nbp_tf") return 1;
    if (font == "7x14_tf") return 1;
    if (font == "t0_14b_tf") return 1;
    if (font.indexOf('/') == -1) font = "/fonts/" + font;
    if (!font.startsWith("/")) font = "/" + font;
    if (font.endsWith(".vlw")) font = font.substring(0, font.length() - 4);
    if (font.endsWith(".ttf")) return 2;
    return 3;
}

void replaceVariables(String &format) {
    size_t startIndex = 0;
    size_t openBraceIndex, closeBraceIndex;

    while ((openBraceIndex = format.indexOf('{', startIndex)) != -1 &&
           (closeBraceIndex = format.indexOf('}', openBraceIndex + 1)) != -1) {
        const std::string variableName = format.substring(openBraceIndex + 1, closeBraceIndex).c_str();
        const std::string varKey = "{" + variableName + "}";
        auto var = varDB.find(variableName);
        if (var != varDB.end()) {
            format.replace(varKey.c_str(), var->second.value);
        }
        startIndex = closeBraceIndex + 1;
    }
}

void drawString(TFT_eSprite &spr, String content, int16_t posx, int16_t posy, String font, byte align, uint16_t color, uint16_t size, uint16_t bgcolor) {
    // drawString(spr,"test",100,10,"bahnschrift30",TC_DATUM,TFT_RED);
    replaceVariables(content);
    switch (processFontPath(font)) {
        case 1: {
            // u8g2 font
            U8g2_for_TFT_eSPI u8f;
            u8f.begin(spr);
            setU8G2Font(font, u8f);
            u8f.setForegroundColor(color);
            u8f.setBackgroundColor(bgcolor);
            if (align == TC_DATUM) {
                posx -= u8f.getUTF8Width(content.c_str()) / 2;
            }
            if (align == TR_DATUM) {
                posx -= u8f.getUTF8Width(content.c_str());
            }
            u8f.setCursor(posx, posy);
            u8f.print(content);
        } break;
        case 2: {
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
            // Serial.println("text: '" + content + "' " + String(millis() - t) + "ms");
        } break;
        case 3: {
            // vlw bitmap font
            spr.setTextDatum(align);
            if (font != "") spr.loadFont(font.substring(1), *contentFS);
            spr.setTextColor(color, bgcolor);
            spr.drawString(content, posx, posy);
            if (font != "") spr.unloadFont();
        }
    }
}

void initSprite(TFT_eSprite &spr, int w, int h, imgParam &imageParams) {
    spr.setColorDepth(8);
    spr.createSprite(w, h);
    spr.setRotation(3);
    if (spr.getPointer() == nullptr) {
        wsErr("low on memory. Fallback to 1bpp");
        util::printLargestFreeBlock();
        spr.setColorDepth(1);
        spr.setBitmapColor(TFT_WHITE, TFT_BLACK);
        imageParams.bufferbpp = 1;
        spr.createSprite(w, h);
    }
    if (spr.getPointer() == nullptr) {
        wsErr("Failed to create sprite");
    }
    spr.fillSprite(TFT_WHITE);
}

void drawDate(String &filename, tagRecord *&taginfo, imgParam &imageParams) {
    time_t now;
    time(&now);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);

    // const int weekday_number = timeinfo.tm_wday;
    const int month_number = timeinfo.tm_mon;
    const int year_number = timeinfo.tm_year + 1900;

    if (taginfo->hwType == SOLUM_SEG_UK) {
        sprintf(imageParams.segments, "%2d%2d%-2.2s%04d", timeinfo.tm_mday, month_number + 1, languageDays[getCurrentLanguage()][timeinfo.tm_wday], year_number);
        imageParams.symbols = 0x04;
        return;
    }

    TFT_eSprite spr = TFT_eSprite(&tft);

    StaticJsonDocument<512> loc;
    getTemplate(loc, 1, taginfo->hwType);

    initSprite(spr, imageParams.width, imageParams.height, imageParams);

    const auto &date = loc["date"];
    const auto &weekday = loc["weekday"];
    if (date) {
        drawString(spr, languageDays[getCurrentLanguage()][timeinfo.tm_wday], weekday[0], weekday[1], weekday[2], TC_DATUM, TFT_RED);
        drawString(spr, String(timeinfo.tm_mday) + " " + languageMonth[getCurrentLanguage()][timeinfo.tm_mon], date[0], date[1], date[2], TC_DATUM);
    } else {
        const auto &month = loc["month"];
        const auto &day = loc["month"];
        drawString(spr, languageDays[getCurrentLanguage()][timeinfo.tm_wday], weekday[0], weekday[1], weekday[2], TC_DATUM, TFT_BLACK);
        drawString(spr, String(languageMonth[getCurrentLanguage()][timeinfo.tm_mon]), month[0], month[1], month[2], TC_DATUM);
        drawString(spr, String(timeinfo.tm_mday), day[0], day[1], day[2], TC_DATUM, TFT_RED);
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

    TFT_eSprite spr = TFT_eSprite(&tft);

    StaticJsonDocument<512> loc;
    getTemplate(loc, 2, taginfo->hwType);

    initSprite(spr, imageParams.width, imageParams.height, imageParams);
    spr.setTextDatum(MC_DATUM);
    if (count > thresholdred) {
        spr.setTextColor(TFT_RED, TFT_WHITE);
    } else {
        spr.setTextColor(TFT_BLACK, TFT_WHITE);
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

/// @brief Get a weather icon
/// @param id Icon identifier/index
/// @param isNight Use night icons (true) or not (false)
/// @return String reference to icon
const String &getWeatherIcon(const uint8_t id, const bool isNight = false) {
    static const String weatherIcons[] = {"\uf00d", "\uf00c", "\uf002", "\uf013", "\uf013", "\uf014", "", "", "\uf014", "", "",
                                          "\uf01a", "", "\uf01a", "", "\uf01a", "\uf017", "\uf017", "", "", "",
                                          "\uf019", "", "\uf019", "", "\uf019", "\uf015", "\uf015", "", "", "",
                                          "\uf01b", "", "\uf01b", "", "\uf01b", "", "\uf076", "", "", "\uf01a",
                                          "\uf01a", "\uf01a", "", "", "\uf064", "\uf064", "", "", "", "",
                                          "", "", "", "", "\uf01e", "\uf01d", "", "", "\uf01e"};
    if (isNight && id <= 3) {
        static const String nightIcons[] = {"\uf02e", "\uf083", "\uf086"};
        return nightIcons[id];
    }
    return weatherIcons[id];
}

void drawWeather(String &filename, JsonObject &cfgobj, tagRecord *&taginfo, imgParam &imageParams) {
    wsLog("get weather");

    getLocation(cfgobj);

    String lat = cfgobj["#lat"];
    String lon = cfgobj["#lon"];
    String tz = cfgobj["#tz"];

    StaticJsonDocument<1000> doc;
    const bool success = util::httpGetJson("https://api.open-meteo.com/v1/forecast?latitude=" + lat + "&longitude=" + lon + "&current_weather=true&windspeed_unit=ms&timezone=" + tz, doc, 5000);
    if (!success) {
        return;
    }

    const auto &currentWeather = doc["current_weather"];
    const double temperature = currentWeather["temperature"].as<double>();
    const int windspeed = currentWeather["windspeed"].as<int>();
    const int winddirection = currentWeather["winddirection"].as<int>();
    const uint8_t isday = currentWeather["is_day"].as<int>();
    uint8_t weathercode = currentWeather["weathercode"].as<int>();
    if (weathercode > 40) weathercode -= 40;
    const int beaufort = windSpeedToBeaufort(windspeed);

    doc.clear();

    if (taginfo->hwType == SOLUM_SEG_UK) {
        static const String weatherText[] = {"sun", "sun", "sun", "CLDY", "CLDY", "FOG", "", "", "FOG", "", "",
                                             "DRZL", "", "DRZL", "", "DRZL", "ice", "ice", "", "", "",
                                             "rain", "", "rain", "", "rain", "ice", "ice", "", "", "",
                                             "SNOW", "", "SNOW", "", "SNOW", "", "SNOW", "", "", "rain",
                                             "rain", "rain", "", "", "SNOW", "SNOW", "", "", "", "",
                                             "", "", "", "", "STRM", "HAIL", "", "", "HAIL"};
        if (temperature < -9.9) {
            sprintf(imageParams.segments, "%3d^%2d%-4.4s", static_cast<int>(temperature), beaufort, weatherText[weathercode].c_str());
            imageParams.symbols = 0x00;
        } else {
            sprintf(imageParams.segments, "%3d^%2d%-4.4s", static_cast<int>(temperature * 10), beaufort, weatherText[weathercode].c_str());
            imageParams.symbols = 0x04;
        }
        return;
    }

    getTemplate(doc, 4, taginfo->hwType);

    TFT_eSprite spr = TFT_eSprite(&tft);
    tft.setTextWrap(false, false);

    initSprite(spr, imageParams.width, imageParams.height, imageParams);
    const auto &location = doc["location"];
    drawString(spr, cfgobj["location"], location[0], location[1], location[2]);
    const auto &wind = doc["wind"];
    drawString(spr, String(beaufort), wind[0], wind[1], wind[2], TR_DATUM, (beaufort > 4 ? TFT_RED : TFT_BLACK));

    char tmpOutput[5];
    dtostrf(temperature, 2, 1, tmpOutput);
    const auto &temp = doc["temp"];
    drawString(spr, String(tmpOutput), temp[0], temp[1], temp[2], TL_DATUM, (temperature < 0 ? TFT_RED : TFT_BLACK));

    int iconcolor = TFT_BLACK;
    if (weathercode == 55 || weathercode == 65 || weathercode == 75 || weathercode == 82 || weathercode == 86 || weathercode == 95 || weathercode == 96 || weathercode == 99) {
        iconcolor = TFT_RED;
    }
    const auto &icon = doc["icon"];
    drawString(spr, getWeatherIcon(weathercode, isday == 0), icon[0], icon[1], "/fonts/weathericons.ttf", icon[3], iconcolor, icon[2]);
    const auto &dir = doc["dir"];
    drawString(spr, windDirectionIcon(winddirection), dir[0], dir[1], "/fonts/weathericons.ttf", TC_DATUM, TFT_BLACK, dir[2]);
    if (weathercode > 10) {
        const auto &umbrella = doc["umbrella"];
        drawString(spr, "\uf084", umbrella[0], umbrella[1], "/fonts/weathericons.ttf", TC_DATUM, TFT_RED, umbrella[2]);
    }

    spr2buffer(spr, filename, imageParams);
    spr.deleteSprite();
}

void drawForecast(String &filename, JsonObject &cfgobj, tagRecord *&taginfo, imgParam &imageParams) {
    TFT_eSprite spr = TFT_eSprite(&tft);

    wsLog("get weather");
    getLocation(cfgobj);

    String lat = cfgobj["#lat"];
    String lon = cfgobj["#lon"];
    String tz = cfgobj["#tz"];

    DynamicJsonDocument doc(2000);
    const bool success = util::httpGetJson("https://api.open-meteo.com/v1/forecast?latitude=" + lat + "&longitude=" + lon + "&daily=weathercode,temperature_2m_max,temperature_2m_min,precipitation_sum,windspeed_10m_max,winddirection_10m_dominant&windspeed_unit=ms&timeformat=unixtime&timezone=" + tz, doc, 5000);
    if (!success) {
        return;
    }

    tft.setTextWrap(false, false);

    StaticJsonDocument<512> loc;
    getTemplate(loc, 8, taginfo->hwType);
    initSprite(spr, imageParams.width, imageParams.height, imageParams);

    const auto &location = loc["location"];
    drawString(spr, cfgobj["location"], location[0], location[1], location[2], TL_DATUM, TFT_BLACK);
    const auto &daily = doc["daily"];
    const auto &column = loc["column"];
    const int column1 = column[1].as<int>();
    const auto &day = loc["day"];
    for (uint8_t dag = 0; dag < column[0]; dag++) {
        const time_t weatherday = daily["time"][dag].as<time_t>();
        const struct tm *datum = localtime(&weatherday);

        drawString(spr, String(languageDaysShort[getCurrentLanguage()][datum->tm_wday]), dag * column1 + day[0].as<int>(), day[1], day[2], TC_DATUM, TFT_BLACK);

        uint8_t weathercode = daily["weathercode"][dag].as<int>();
        if (weathercode > 40) weathercode -= 40;

        const int iconcolor = (weathercode == 55 || weathercode == 65 || weathercode == 75 || weathercode == 82 || weathercode == 86 || weathercode == 95 || weathercode == 96 || weathercode == 99) ? TFT_RED : TFT_BLACK;
        drawString(spr, getWeatherIcon(weathercode), loc["icon"][0].as<int>() + dag * column1, loc["icon"][1], "/fonts/weathericons.ttf", TC_DATUM, iconcolor, loc["icon"][2]);

        drawString(spr, windDirectionIcon(daily["winddirection_10m_dominant"][dag]), loc["wind"][0].as<int>() + dag * column1, loc["wind"][1], "/fonts/weathericons.ttf", TC_DATUM, TFT_BLACK, loc["icon"][2]);

        const int8_t tmin = round(daily["temperature_2m_min"][dag].as<double>());
        const int8_t tmax = round(daily["temperature_2m_max"][dag].as<double>());
        const uint8_t wind = windSpeedToBeaufort(daily["windspeed_10m_max"][dag].as<double>());

        spr.loadFont(day[2], *contentFS);

        if (loc["rain"]) {
            const int8_t rain = round(daily["precipitation_sum"][dag].as<double>());
            if (rain > 0) {
                drawString(spr, String(rain) + "mm", dag * column1 + loc["rain"][0].as<int>(), loc["rain"][1], "", TC_DATUM, (rain > 10 ? TFT_RED : TFT_BLACK));
            }
        }

        drawString(spr, String(tmin) + " ", dag * column1 + day[0].as<int>(), day[4], "", TR_DATUM, (tmin < 0 ? TFT_RED : TFT_BLACK));
        drawString(spr, String(" ") + String(tmax), dag * column1 + day[0].as<int>(), day[4], "", TL_DATUM, (tmax < 0 ? TFT_RED : TFT_BLACK));
        drawString(spr, String(" ") + String(wind), dag * column1 + day[0].as<int>(), day[3], "", TL_DATUM, (wind > 5 ? TFT_RED : TFT_BLACK));
        spr.unloadFont();
        if (dag > 0) {
            for (int i = loc["line"][0]; i < loc["line"][1]; i += 3) {
                spr.drawPixel(dag * column1, i, TFT_BLACK);
            }
        }
    }

    spr2buffer(spr, filename, imageParams);
    spr.deleteSprite();
}

int getImgURL(String &filename, String URL, time_t fetched, imgParam &imageParams, String MAC) {
    // https://images.klari.net/kat-bw29.jpg

    Storage.begin();

    HTTPClient http;
    http.begin(URL);
    http.addHeader("If-Modified-Since", formatHttpDate(fetched));
    http.addHeader("X-ESL-MAC", MAC);
    http.setTimeout(5000);  // timeout in ms
    const int httpCode = http.GET();
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

    const char *url = URL.c_str();
    const char *tag = "title";
    const int rssArticleSize = 128;

    TFT_eSprite spr = TFT_eSprite(&tft);
    U8g2_for_TFT_eSPI u8f;
    u8f.begin(spr);

    StaticJsonDocument<512> loc;
    getTemplate(loc, 9, taginfo->hwType);
    initSprite(spr, imageParams.width, imageParams.height, imageParams);

    if (title == "" || title == "null") title = "RSS feed";
    drawString(spr, title, loc["title"][0], loc["title"][1], loc["title"][2], TL_DATUM, TFT_BLACK);

    setU8G2Font(loc["font"], u8f);
    u8f.setFontMode(0);
    u8f.setFontDirection(0);
    u8f.setForegroundColor(TFT_BLACK);
    u8f.setBackgroundColor(TFT_WHITE);

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
        (local_tm.tm_hour == 0 && local_tm.tm_min == 0) ||
        difftime(utc, now) >= 86400) {
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

    TFT_eSprite spr = TFT_eSprite(&tft);
    U8g2_for_TFT_eSPI u8f;
    u8f.begin(spr);

    StaticJsonDocument<512> loc;
    getTemplate(loc, 11, taginfo->hwType);
    initSprite(spr, imageParams.width, imageParams.height, imageParams);

    if (title == "" || title == "null") title = "Calendar";
    drawString(spr, title, loc["title"][0], loc["title"][1], loc["title"][2], TL_DATUM, TFT_BLACK);
    drawString(spr, dateString, loc["date"][0], loc["date"][1], loc["title"][2], TR_DATUM, TFT_BLACK);

    u8f.setFontMode(0);
    u8f.setFontDirection(0);
    int n = doc.size();
    if (n > loc["items"]) n = loc["items"];
    for (int i = 0; i < n; i++) {
        const JsonObject &obj = doc[i];
        const String eventtitle = obj["title"];
        const time_t starttime = obj["start"];
        const time_t endtime = obj["end"];
        setU8G2Font(loc["line"][3], u8f);
        if (starttime <= now && endtime > now) {
            u8f.setForegroundColor(TFT_WHITE);
            u8f.setBackgroundColor(TFT_RED);
            spr.fillRect(loc["red"][0], loc["red"][1].as<int>() + i * loc["line"][2].as<int>(), loc["red"][2], loc["red"][3], TFT_RED);
        } else {
            u8f.setForegroundColor(TFT_BLACK);
            u8f.setBackgroundColor(TFT_WHITE);
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
    TFT_eSprite spr = TFT_eSprite(&tft);
    Storage.begin();

    const char *text = qrcontent.c_str();
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(2)];
    // https://github.com/ricmoo/QRCode
    qrcode_initText(&qrcode, qrcodeData, 2, ECC_MEDIUM, text);

    StaticJsonDocument<512> loc;
    getTemplate(loc, 10, taginfo->hwType);
    initSprite(spr, imageParams.width, imageParams.height, imageParams);
    drawString(spr, title, loc["title"][0], loc["title"][1], loc["title"][2]);

    const int size = qrcode.size;
    const int dotsize = int((imageParams.height - loc["pos"][1].as<int>()) / size);
    const int xpos = loc["pos"][0].as<int>() - dotsize * size / 2;
    const int ypos = loc["pos"][1];

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if (qrcode_getModule(&qrcode, x, y)) {
                spr.fillRect(xpos + x * dotsize, ypos + y * dotsize, dotsize, dotsize, TFT_BLACK);
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

    if (httpCode == 200) {
        TFT_eSprite spr = TFT_eSprite(&tft);
        U8g2_for_TFT_eSPI u8f;
        u8f.begin(spr);

        StaticJsonDocument<512> loc;
        getTemplate(loc, 16, taginfo->hwType);
        initSprite(spr, imageParams.width, imageParams.height, imageParams);

        tft.setTextWrap(false, false);

        String response = http.getString();

        drawString(spr, cfgobj["location"], loc["location"][0], loc["location"][1], loc["location"][2]);

        for (int i = 0; i < 295; i += 4) {
            int yCoordinates[] = {110, 91, 82, 72, 62, 56, 52};
            for (int y : yCoordinates) {
                spr.drawPixel(i, y, TFT_BLACK);
            }
        }

        drawString(spr, "Buienradar", loc["title"][0], loc["title"][1], loc["title"][2]);

        const auto &bars = loc["bars"];
        const auto &cols = loc["cols"];
        const int cols0 = cols[0].as<int>();
        const int cols1 = cols[1].as<int>();
        const int cols2 = cols[2].as<int>();
        const String cols3 = cols[3].as<String>();
        const int bars0 = bars[0].as<int>();
        const int bars1 = bars[1].as<int>();
        const int bars2 = bars[2].as<int>();
        for (int i = 0; i < 24; i++) {
            const int startPos = i * 11;
            uint8_t value = response.substring(startPos, startPos + 3).toInt();
            const String timestring = response.substring(startPos + 4, startPos + 9);
            const int minutes = timestring.substring(3).toInt();
            if (value < 70) {
                value = 70;
            } else if (value > 180) {
                value = 180;
            }
            if (value > 70) {
                if (i < 12) {
                    refresh = 5;
                } else if (refresh > 5) {
                    refresh = 15;
                }
            }

            spr.fillRect(i * cols2 + bars0, bars1 - (value - 70), bars2, (value - 70), (value > 130 ? TFT_RED : TFT_BLACK));

            if (minutes % 15 == 0) {
                drawString(spr, timestring, i * cols2 + cols0, cols1, cols3);
            }
        }

        spr2buffer(spr, filename, imageParams);
        spr.deleteSprite();
    } else {
        wsErr("Buitenradar http " + String(httpCode));
    }
    http.end();
#endif
    return refresh;
}

void drawAPinfo(String &filename, JsonObject &cfgobj, tagRecord *&taginfo, imgParam &imageParams) {
    if (taginfo->hwType == SOLUM_SEG_UK) {
        imageParams.symbols = 0x00;
        sprintf(imageParams.segments, "");
        return;
    }

    TFT_eSprite spr = TFT_eSprite(&tft);
    StaticJsonDocument<2048> loc;
    getTemplate(loc, 21, taginfo->hwType);

    initSprite(spr, imageParams.width, imageParams.height, imageParams);
    const JsonArray jsonArray = loc.as<JsonArray>();
    for (const JsonVariant &elem : jsonArray) {
        drawElement(elem, spr);
    }

    spr2buffer(spr, filename, imageParams);
    spr.deleteSprite();
}

int getJsonTemplateFile(String &filename, String jsonfile, tagRecord *&taginfo, imgParam &imageParams) {
    if (jsonfile.c_str()[0] != '/') {
        jsonfile = "/" + jsonfile;
    }
    File file = contentFS->open(jsonfile, "r");
    if (file) {
        drawJsonStream(file, filename, taginfo, imageParams);
        file.close();
        // contentFS->remove(jsonfile);
        return 1;
    }
    return 0;
}

int getJsonTemplateUrl(String &filename, String URL, time_t fetched, String MAC, tagRecord *&taginfo, imgParam &imageParams) {
    HTTPClient http;
    http.useHTTP10(true);
    http.begin(URL);
    http.addHeader("If-Modified-Since", formatHttpDate(fetched));
    http.addHeader("X-ESL-MAC", MAC);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    http.setTimeout(5000);
    const int httpCode = http.GET();
    if (httpCode == 200) {
        drawJsonStream(http.getStream(), filename, taginfo, imageParams);
    } else {
        if (httpCode != 304) {
            wsErr("http " + URL + " status " + String(httpCode));
        }
    }
    http.end();
    return httpCode;
}

void drawJsonStream(Stream &stream, String &filename, tagRecord *&taginfo, imgParam &imageParams) {
    TFT_eSprite spr = TFT_eSprite(&tft);
    initSprite(spr, imageParams.width, imageParams.height, imageParams);
    DynamicJsonDocument doc(300);

    if (stream.find("[")) {
        do {
            DeserializationError error = deserializeJson(doc, stream);
            if (error) {
                wsErr("json error " + String(error.c_str()));
                break;
            } else {
                drawElement(doc.as<JsonObject>(), spr);
                doc.clear();
            }
        } while (stream.findUntil(",", "]"));
    }

    spr2buffer(spr, filename, imageParams);
    spr.deleteSprite();
}

void drawElement(const JsonObject &element, TFT_eSprite &spr) {
    if (element.containsKey("text")) {
        const JsonArray &textArray = element["text"];
        const uint16_t align = textArray[5] | 0;
        const uint16_t size = textArray[6] | 0;
        const String bgcolorstr = textArray[7].as<String>();
        const uint16_t bgcolor = (bgcolorstr.length() > 0) ? getColor(bgcolorstr) : TFT_WHITE;
        drawString(spr, textArray[2], textArray[0].as<int>(), textArray[1].as<int>(), textArray[3], align, getColor(textArray[4]), size, bgcolor);
    } else if (element.containsKey("box")) {
        const JsonArray &boxArray = element["box"];
        spr.fillRect(boxArray[0].as<int>(), boxArray[1].as<int>(), boxArray[2].as<int>(), boxArray[3].as<int>(), getColor(boxArray[4]));
    } else if (element.containsKey("line")) {
        const JsonArray &lineArray = element["line"];
        spr.drawLine(lineArray[0].as<int>(), lineArray[1].as<int>(), lineArray[2].as<int>(), lineArray[3].as<int>(), getColor(lineArray[4]));
    } else if (element.containsKey("triangle")) {
        const JsonArray &lineArray = element["triangle"];
        spr.fillTriangle(lineArray[0].as<int>(), lineArray[1].as<int>(), lineArray[2].as<int>(), lineArray[3].as<int>(), lineArray[4].as<int>(), lineArray[5].as<int>(), getColor(lineArray[6]));
    }
}

uint16_t getColor(const String &color) {
    if (color == "0" || color == "white") return TFT_WHITE;
    if (color == "1" || color == "" || color == "black") return TFT_BLACK;
    if (color == "2" || color == "red") return TFT_RED;
    uint16_t r, g, b;
    if (color.length() == 7 && color[0] == '#' &&
        sscanf(color.c_str(), "#%2hx%2hx%2hx", &r, &g, &b) == 3) {
        return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
    }
    return TFT_WHITE;
}

char *formatHttpDate(time_t t) {
    static char buf[40];
    struct tm *timeinfo;
    timeinfo = localtime(&t);                 // Get the local time
    const time_t utcTime = mktime(timeinfo);  // Convert to UTC
    timeinfo = gmtime(&utcTime);
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
    return buf;
}

String urlEncode(const char *msg) {
    static const char *hex = "0123456789ABCDEF";
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

int windSpeedToBeaufort(const float windSpeed) {
    constexpr static const float speeds[] = {0.3, 1.5, 3.3, 5.5, 8, 10.8, 13.9, 17.2, 20.8, 24.5, 28.5, 32.7};
    constexpr static const int numSpeeds = sizeof(speeds) / sizeof(speeds[0]);
    int beaufort = 0;
    for (int i = 0; i < numSpeeds; i++) {
        if (windSpeed >= speeds[i]) {
            beaufort = i + 1;
        }
    }
    return beaufort;
}

String windDirectionIcon(const int degrees) {
    static const String directions[] = {"\uf044", "\uf043", "\uf048", "\uf087", "\uf058", "\uf057", "\uf04d", "\uf088"};
    int index = (degrees + 22) / 45;
    if (index >= 8) {
        index = 0;
    }
    return directions[index];
}

void getLocation(JsonObject &cfgobj) {
    const String lat = cfgobj["#lat"];
    const String lon = cfgobj["#lon"];

    if (lat == "null" || lon == "null") {
        wsLog("get location");
        StaticJsonDocument<1000> doc;
        if (util::httpGetJson("https://geocoding-api.open-meteo.com/v1/search?name=" + urlEncode(cfgobj["location"]) + "&count=1", doc, 5000)) {
            cfgobj["#lat"] = doc["results"][0]["latitude"].as<String>();
            cfgobj["#lon"] = doc["results"][0]["longitude"].as<String>();
            cfgobj["#tz"] = doc["results"][0]["timezone"].as<String>();
        }
    }
}

void prepareNFCReq(const uint8_t *dst, const char *url) {
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

void prepareLUTreq(const uint8_t *dst, const String &input) {
    const char *delimiters = ", \t";
    const int maxValues = 76;
    uint8_t waveform[maxValues];
    char *ptr = strtok(const_cast<char *>(input.c_str()), delimiters);
    int i = 0;
    while (ptr != nullptr && i < maxValues) {
        waveform[i++] = static_cast<uint8_t>(strtol(ptr, nullptr, 16));
        ptr = strtok(nullptr, delimiters);
    }
    const size_t waveformLen = sizeof(waveform);
    prepareDataAvail(waveform, waveformLen, DATATYPE_CUSTOM_LUT_OTA, dst);
}

void prepareConfigFile(const uint8_t *dst, const JsonObject &config) {
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

void getTemplate(JsonDocument &json, const uint8_t id, const uint8_t hwtype) {
    StaticJsonDocument<80> filter;
    StaticJsonDocument<2048> doc;

    const String idstr = String(id);
    const char *templateKey = "template";

    char filename[20];
    snprintf(filename, sizeof(filename), "/tagtypes/%02X.json", hwtype);
    File jsonFile = contentFS->open(filename, "r");

    if (jsonFile) {
        filter[templateKey][idstr] = true;
        filter["usetemplate"] = true;
        const DeserializationError error = deserializeJson(doc, jsonFile, DeserializationOption::Filter(filter));
        jsonFile.close();
        if (!error && doc.containsKey(templateKey) && doc[templateKey].containsKey(idstr)) {
            json.set(doc[templateKey][idstr]);
            return;
        }
        if (!error && doc.containsKey("usetemplate")) {
            getTemplate(json, id, doc["usetemplate"]);
            return;
        }
        Serial.println("json error in " + String(filename));
        Serial.println(error.c_str());
    } else {
        Serial.println("Failed to open " + String(filename));
    }
}

void setU8G2Font(const String &title, U8g2_for_TFT_eSPI &u8f) {
    if (title == "glasstown_nbp_tf") {
        u8f.setFont(u8g2_font_glasstown_nbp_tf);
    } else if (title == "7x14_tf") {
        u8f.setFont(u8g2_font_7x14_tf);
    } else if (title == "t0_14b_tf") {
        u8f.setFont(u8g2_font_t0_14b_tf);
    }
}
