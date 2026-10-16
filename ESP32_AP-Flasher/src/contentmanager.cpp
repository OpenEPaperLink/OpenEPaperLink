#include "contentmanager.h"

#include <map>

#include "gzipfile.h"

// possibility to turn off, to save space if needed
#ifndef SAVE_SPACE
#define CONTENT_QR
#define CONTENT_RSS
#define CONTENT_BIGCAL
#define CONTENT_NFCLUT
#define CONTENT_DAYAHEAD
#define CONTENT_TIMESTAMP
#define CONTENT_BUIENRADAR
#define CONTENT_CAL
#define CONTENT_TIME_RAWDATA
#endif
#define CONTENT_TAGCFG

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <MD5Builder.h>
#include <locale.h>
#ifdef CONTENT_RSS
#include <rssClass.h>
#endif
#include <TJpg_Decoder.h>
#include <time.h>

#include <map>

#include "commstructs.h"
#include "makeimage.h"
#include "newproto.h"
#include "od_session.h"
#include "pendingupload.h"
#include "storage.h"
#ifdef CONTENT_QR
#include "QRCodeGenerator.h"
#endif
#include "language.h"
#include "settings.h"
#include "system.h"
#include "tag_db.h"
#include <TinyTTF.h>
#include "util.h"
#include "web.h"

// https://csvjson.com/json_beautifier

static void clearFontCache();

bool needRedraw(uint8_t contentMode, uint8_t wakeupReason) {
    // contentmode 26, timestamp
    if ((wakeupReason == WAKEUP_REASON_BUTTON1 || wakeupReason == WAKEUP_REASON_BUTTON2 || wakeupReason == WAKEUP_REASON_BUTTON3) && contentMode == 26) return true;
    return false;
}

void contentRunner() {
    if (config.runStatus == RUNSTATUS_STOP) return;

    time_t now;
    time(&now);
    uint8_t wifimac[8];
    WiFi.macAddress(wifimac);
    memset(&wifimac[6], 0, 2);

    freeDeletedRecords();
    freeRetiredQueueItems();
    dropStalePendingUploads();
    spillPendingUpload();
    // Storage.freeSpace() walks the whole filesystem: only when a tag needs it, and once per run
    int8_t spaceLeftCache = -1;
    auto spaceLeft = [&spaceLeftCache]() {
        if (spaceLeftCache < 0) spaceLeftCache = Storage.freeSpace() > 31000 ? 1 : 0;
        return spaceLeftCache == 1;
    };
    // iterate over a copy: drawNew() can take seconds and must not hold the tagDB lock
    for (tagRecord *taginfo : tagDBSnapshot()) {

        const bool isAp = memcmp(taginfo->mac, wifimac, 8) == 0;
        const bool due = now >= taginfo->nextupdate || needRedraw(taginfo->contentMode, taginfo->wakeupReason);
        const bool checkinSoon = taginfo->expectedNextCheckin < now + 300 || isAp || (wsClientCount() && config.stopsleep == 1);
        if (taginfo->RSSI && due && config.runStatus == RUNSTATUS_RUN && checkinSoon && spaceLeft() &&
            !util::isSleeping(config.sleepTime1, config.sleepTime2)) {
                drawNew(taginfo->mac, taginfo);
                taginfo->wakeupReason = 0;
        } else if (due && config.runStatus == RUNSTATUS_RUN && now > taginfo->nextupdate + 900 &&
                   !util::isSleeping(config.sleepTime1, config.sleepTime2)) {
            // content is overdue for more than 15 minutes: log why (#579)
            String reason;
            if (!taginfo->RSSI) {
                reason = "no RSSI";
            } else if (!spaceLeft()) {
                reason = "filesystem full";
            } else {
                time_t expected = taginfo->expectedNextCheckin;
                char timeStr[20];
                strftime(timeStr, sizeof(timeStr), "%d-%m %H:%M", localtime(&expected));
                reason = "waiting for check-in expected at " + String(timeStr);
            }
            char hexmac[17];
            mac2hex(taginfo->mac, hexmac);
            logLineRateLimited(String("overdue") + hexmac, String(hexmac) + " content overdue: " + reason);
        }

        if (taginfo->expectedNextCheckin > now - 10 && taginfo->expectedNextCheckin < now + 30 && taginfo->pendingIdle == 0 && taginfo->pendingCount == 0 && !isAp) {
            int32_t minutesUntilNextUpdate = (taginfo->nextupdate - now) / 60;
            if (minutesUntilNextUpdate > config.maxsleep) {
                minutesUntilNextUpdate = config.maxsleep;
            }
            if (util::isSleeping(config.sleepTime1, config.sleepTime2)) {
                struct tm timeinfo;
                getLocalTime(&timeinfo);
                struct tm nextSleepTimeinfo = timeinfo;
                nextSleepTimeinfo.tm_hour = config.sleepTime2;
                nextSleepTimeinfo.tm_min = 0;
                nextSleepTimeinfo.tm_sec = 0;
                time_t nextWakeTime = mktime(&nextSleepTimeinfo);
                if (nextWakeTime < now) nextWakeTime += 24 * 3600;
                minutesUntilNextUpdate = (nextWakeTime - now) / 60 - 2;
            }
            if (minutesUntilNextUpdate > 1 && (wsClientCount() == 0 || config.stopsleep == 0)) {
                taginfo->pendingIdle = minutesUntilNextUpdate * 60;
                taginfo->expectedNextCheckin = now + taginfo->pendingIdle;
                if (taginfo->isExternal == false) {
                    prepareIdleReq(taginfo->mac, minutesUntilNextUpdate);
                }
            }
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);  // add a small delay to allow other threads to run
    }
}

void checkVars() {
    JsonDocument cfgobj;
    for (tagRecord *tag : tagDBSnapshot()) {
        if (tag->contentMode == 19) {
            deserializeJson(cfgobj, tag->modeConfigJson);
            const String jsonfile = cfgobj["filename"].as<String>();
            if (!util::isEmptyOrNull(jsonfile)) {
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
/// @param taginfo Tag information
/// @param cfgobj Tag config as json object
/// @param filename Filename
/// @param imageParams Image parameters
/// @param nextupdate Next counter update
/// @param nextCheckin Next tag checkin
void drawCounter(const uint8_t mac[8], tagRecord *&taginfo, JsonObject &cfgobj, String &filename, imgParam &imageParams, const uint32_t nextupdate, const uint16_t nextCheckin) {
    int32_t counter = cfgobj["counter"].as<int32_t>();
    bool buttonPressed = false;
    if (buttonPressed) {  // fixme: button pressed
        counter = 0;
    }
    drawNumber(filename, counter, (int32_t)cfgobj["thresholdred"], taginfo, imageParams);
    taginfo->nextupdate = nextupdate;
    updateTagImage(filename, mac, (buttonPressed ? 0 : nextCheckin), taginfo, imageParams);
    cfgobj["counter"] = counter + 1;
}

/// @brief Send the image the tag got last (/current/<mac>.raw, kept when preview is on) again
/// @note For image content whose source is gone (/imgupload removes it after the first transfer), on a forced
/// refresh or when the tag rebooted (#567). A tag that cached the image recognizes the md5 and doesn't download it.
static bool resendCurrentImage(const uint8_t mac[8], const tagRecord *taginfo, const char *hexmac, const uint8_t lut, const uint16_t nextCheckin) {
    String rawFilename = "/current/" + String(hexmac) + ".raw";
    if (!contentFS->exists(rawFilename)) {
        wsErr("Can't resend image, " + rawFilename + " not found (preview off?)");
        return false;
    }
    if (taginfo->imageDataType == 0) {
        wsErr("Can't resend image, unknown data type for " + rawFilename);
        return false;
    }
    struct imageDataTypeArgStruct arg = {0};
    arg.lut = lut & 0x03;
    return prepareDataAvail(rawFilename, taginfo->imageDataType, *((uint8_t *)&arg), mac, nextCheckin, true);
}

void drawNew(const uint8_t mac[8], tagRecord *&taginfo) {
    time_t now;
    time(&now);

    const HwType hwdata = getHwType(taginfo->hwType);
    if (hwdata.bpp == 0) {
        taginfo->nextupdate = now + 300;
        Serial.println("No definition found for tag type " + String(taginfo->hwType));
        return;
    }

    uint8_t wifimac[8];
    WiFi.macAddress(wifimac);
    memset(&wifimac[6], 0, 2);

    const bool isAp = memcmp(mac, wifimac, 8) == 0;
    if ((taginfo->wakeupReason == WAKEUP_REASON_FIRSTBOOT || taginfo->wakeupReason == WAKEUP_REASON_WDT_RESET) && taginfo->contentMode == 0) {
        if (isAp) {
            taginfo->contentMode = 21;
            taginfo->nextupdate = 0;
        } else if (contentFS->exists("/tag_defaults.json")) {
            JsonDocument doc;
            fs::File tagDefaults = contentFS->open("/tag_defaults.json", "r");
            DeserializationError err = deserializeJson(doc, tagDefaults);
            if (!err) {
                if (doc["contentMode"].is<uint8_t>()) {
                    taginfo->contentMode = doc["contentMode"];
                }
                if (doc["modecfgjson"].is<String>()) {
                    taginfo->modeConfigJson = doc["modecfgjson"].as<String>();
                }
            }
            tagDefaults.close();
        }
    }

    char hexmac[17];
    mac2hex(mac, hexmac);
    String filename = "/temp/" + String(hexmac) + ".raw";
#ifdef HAS_TFT
    if (isAp) {
        filename = "direct";
    }
#endif

    // an upload can replace the config while this runs: see the static image modes below
    const String processedConfig = taginfo->modeConfigJson;
    JsonDocument doc;
    deserializeJson(doc, processedConfig);
    JsonObject cfgobj = doc.as<JsonObject>();
    char buffer[64];

    wsLog("Updating " + String(hexmac));
    taginfo->nextupdate = now + 60;

    imgParam imageParams;
    imageParams.hwdata = hwdata;
    imageParams.width = hwdata.width;
    imageParams.height = hwdata.height;
    imageParams.bpp = hwdata.bpp;
    imageParams.rotatebuffer = hwdata.rotatebuffer;
    imageParams.shortlut = hwdata.shortlut;
    imageParams.highlightColor = getColor(String(hwdata.highlightColor));

    imageParams.hasRed = false;
    imageParams.dataType = DATATYPE_IMG_RAW_1BPP;
    imageParams.dither = 2;

    imageParams.invert = taginfo->invert;
    imageParams.symbols = 0;
    imageParams.rotate = taginfo->rotate;
    if (hwdata.zlib != 0 && taginfo->tagSoftwareVersion >= hwdata.zlib) {
        imageParams.zlib = 1;
    } else {
        imageParams.zlib = 0;
    }
#ifdef SAVE_SPACE
    imageParams.g5 = 0;
#else
    if (hwdata.g5 != 0 && taginfo->tagSoftwareVersion >= hwdata.g5) {
        imageParams.g5 = 1;
    } else {
        imageParams.g5 = 0;
    }
#endif

#ifdef HAS_BLE_WRITER
    // OpenDisplay: buffer layout and colors from the tag's own config; the BLE upload compresses
    if (bleFamily(mac, taginfo->hwType) == BLE_FAMILY_OPENDISPLAY) odImageParams(mac, imageParams);
#endif

    imageParams.lut = EPD_LUT_NO_REPEATS;
    if (taginfo->lut == 2) imageParams.lut = EPD_LUT_FAST_NO_REDS;
    if (taginfo->lut == 3) imageParams.lut = EPD_LUT_FAST;
    time_t last_midnight = now - now % (24 * 60 * 60) + 3 * 3600;  // somewhere in the middle of the night
    if (imageParams.shortlut == SHORTLUT_DISABLED || taginfo->lastfullupdate < last_midnight || taginfo->lut == 1) {
        imageParams.lut = EPD_LUT_DEFAULT;
        taginfo->lastfullupdate = now;
    }

    int32_t interval = cfgobj["interval"].as<int>() * 60;
    if (interval == -1440 * 60) {
        interval = util::getMidnightTime() - now;
    } else if (interval < 0) {
        interval = -interval;
        unsigned int secondsUntilNext = (interval - (now % interval)) % interval;
        interval = secondsUntilNext;
    } else if (interval < 180)
        interval = 60 * 60;

    imageParams.ts_option = config.showtimestamp;
    if(imageParams.ts_option) {
       JsonDocument loc;
       getTemplate(loc, taginfo->contentMode, taginfo->hwType);

       if(loc["ts_option"].is<int>()) {
       // Overide ts_option if present in template
          imageParams.ts_option = loc["ts_option"];
       }
       else {
          const JsonArray jsonArray = loc.as<JsonArray>();
          for (const JsonVariant &elem : jsonArray) {
             if(elem["ts_option"].is<int>()) {
             // Overide ts_option if present in template
                imageParams.ts_option = elem["ts_option"];
                break;
             }
          }
       }
    }

    switch (taginfo->contentMode) {
        case 0:   // Not configured
        case 22:  // Static image
        case 23:  // Static image (advanced)
        case 24:  // External image
        case 25:  // Home Assistant
        {
            String configFilename = cfgobj["filename"].as<String>();
            if (!util::isEmptyOrNull(configFilename)) {
                if (!configFilename.startsWith("/")) {
                    configFilename = "/" + configFilename;
                }

                // an image from /imgupload waits in PSRAM (pendingupload.h)
                uint8_t *uploadData = nullptr;
                size_t uploadLen = 0, uploadSize = 0;
                const PendingUpload pending = takePendingUpload(mac, configFilename, uploadData, uploadLen, uploadSize);
                if (pending == PendingUpload::Other) {
                    // a newer upload is stored, its config is set right after: convert that one on the next run
                    taginfo->nextupdate = 0;
                    break;
                }

                if (pending == PendingUpload::None && !contentFS->exists(configFilename)) {
                    // the source image is gone: resend the image the tag got last
                    if (!resendCurrentImage(mac, taginfo, hexmac, imageParams.lut, cfgobj["timetolive"].as<int>())) {
                        wsErr("Not found: " + configFilename);
                    }
                    if (taginfo->modeConfigJson == processedConfig) taginfo->nextupdate = 3216153600;
                    break;
                }

                imageParams.dither = cfgobj["dither"];

                imageParams.preload = cfgobj["preload"] && cfgobj["preload"] == "1";
                imageParams.preloadlut = cfgobj["preload_lut"];
                imageParams.preloadtype = cfgobj["preload_type"];

                if (pending == PendingUpload::Taken) {
                    jpg2buffer(uploadData, uploadLen, filename, imageParams);
                    freeUploadBuffer(uploadData, uploadSize);
                } else {
                    jpg2buffer(configFilename, filename, imageParams);
                }

                if (imageParams.hasRed && imageParams.lut == EPD_LUT_NO_REPEATS && imageParams.shortlut == SHORTLUT_ONLY_BLACK) {
                    imageParams.lut = EPD_LUT_DEFAULT;
                }

                if (imageParams.bpp == 3) {
                    imageParams.dataType = DATATYPE_IMG_RAW_3BPP;
                    Serial.println("datatype: DATATYPE_IMG_RAW_3BPP");
                } else if (imageParams.bpp == 4) {
                    imageParams.dataType = DATATYPE_IMG_RAW_4BPP;
                    Serial.println("datatype: DATATYPE_IMG_RAW_4BPP");
                } else if (imageParams.zlib) {
                    imageParams.dataType = DATATYPE_IMG_ZLIB;
                    Serial.println("datatype: DATATYPE_IMG_ZLIB");
                } else if (imageParams.g5) {
                    imageParams.dataType = DATATYPE_IMG_G5;
                    Serial.println("datatype: DATATYPE_IMG_G5");
                } else if (imageParams.hasRed) {
                    imageParams.dataType = DATATYPE_IMG_RAW_2BPP;
                    Serial.println("datatype: DATATYPE_IMG_RAW_2BPP");
                } else {
                    Serial.println("datatype: DATATYPE_IMG_RAW_1BPP");
                }

                struct imageDataTypeArgStruct arg = {0};
                // load parameters in case we do need to preload an image
                if (imageParams.preload) {
                    arg.preloadImage = 1;
                    arg.specialType = imageParams.preloadtype;
                    arg.lut = imageParams.preloadlut;
                } else {
                    arg.lut = imageParams.lut & 0x03;
                }

                if (prepareDataAvail(filename, imageParams.dataType, *((uint8_t *)&arg), mac, cfgobj["timetolive"].as<int>())) {
                    if (pending == PendingUpload::None && cfgobj["delete"].as<String>() == "1") {
                        contentFS->remove("/" + configFilename);
                    }
                } else {
                    wsErr("Error accessing " + filename);
                }
            } else {
                // configfilename is empty. Probably the tag needs to redisplay the image after a reboot.
                Serial.println("Resend static image");
                // fixme: doesn't work yet
                // prepareDataAvail(mac);
            }
            // an image uploaded during the conversion has set nextupdate to 0: keep that, so it gets converted too
            if (taginfo->modeConfigJson == processedConfig) taginfo->nextupdate = 3216153600;
        } break;

        case 1:  // Today

            drawDate(filename, cfgobj, taginfo, imageParams);
            taginfo->nextupdate = util::getMidnightTime();
            updateTagImage(filename, mac, (taginfo->nextupdate - now) / 60 - 10, taginfo, imageParams);
            break;

        case 2:  // CountDays
            drawCounter(mac, taginfo, cfgobj, filename, imageParams, util::getMidnightTime(), 15);
            break;

        case 3:  // CountHours
            drawCounter(mac, taginfo, cfgobj, filename, imageParams, now + 3600, 5);
            break;

        case 4:  // Weather

            // https://open-meteo.com/
            // https://geocoding-api.open-meteo.com/v1/search?name=eindhoven
            // https://api.open-meteo.com/v1/forecast?latitude=52.52&longitude=13.41&current_weather=true
            // https://github.com/erikflowers/weather-icons

            drawWeather(filename, cfgobj, taginfo, imageParams);
            taginfo->nextupdate = now + interval;
            updateTagImage(filename, mac, interval / 60, taginfo, imageParams);
            break;

        case 8:  // Forecast

            drawForecast(filename, cfgobj, taginfo, imageParams);
            taginfo->nextupdate = now + interval;
            updateTagImage(filename, mac, interval / 60, taginfo, imageParams);
            break;

        case 5:  // Firmware

            filename = cfgobj["filename"].as<String>();
            if (!util::isEmptyOrNull(filename) && !cfgobj["#fetched"].as<bool>()) {
                File file = contentFS->open(filename, "r");
                if (file) {
                    if (file.find("<html")) {
                        file.close();
                        wsErr("User error flashing tag firmware: this is a html-file!");
                        cfgobj["#fetched"] = true;
                    } else {
                        file.close();
                        if (prepareDataAvail(filename, DATATYPE_FW_UPDATE, 0, mac, cfgobj["timetolive"].as<int>())) {
                            cfgobj["#fetched"] = true;
                        }
                    }
                }
                taginfo->nextupdate = 3216153600;
            } else {
                taginfo->nextupdate = now + 300;
            }
            break;

        case 7:  // ImageUrl

        {
            const int httpcode = getImgURL(filename, cfgobj["url"], (time_t)cfgobj["#fetched"], imageParams, String(hexmac));
            if (httpcode == 200) {
                taginfo->nextupdate = now + interval;
                updateTagImage(filename, mac, interval / 60, taginfo, imageParams);
                cfgobj["#fetched"] = now;
            } else if (httpcode == 304) {
                taginfo->nextupdate = now + interval;
            } else {
                taginfo->nextupdate = now + 300;
            }
            break;
        }

#ifdef CONTENT_RSS
        case 9:  // RSSFeed

            if (getRssFeed(filename, cfgobj["url"], cfgobj["title"], taginfo, imageParams)) {
                taginfo->nextupdate = now + interval;
                updateTagImage(filename, mac, interval / 60, taginfo, imageParams);
            } else {
                taginfo->nextupdate = now + 300;
            }
            break;
#endif

#ifdef CONTENT_QR
        case 10:  // QRcode:

            drawQR(filename, cfgobj["qr-content"], cfgobj["title"], taginfo, imageParams);
            taginfo->nextupdate = now + 12 * 3600;
            updateTagImage(filename, mac, 0, taginfo, imageParams);
            break;
#endif

#ifdef CONTENT_CAL
        case 11:  // Calendar:

            if (getCalFeed(filename, cfgobj, taginfo, imageParams)) {
                taginfo->nextupdate = now + interval;
                updateTagImage(filename, mac, interval / 60, taginfo, imageParams);
            } else {
                taginfo->nextupdate = now + 300;
            }
            break;
#endif

        case 12:  // RemoteAP

            taginfo->nextupdate = 3216153600;
            break;

        case 13:  // SegStatic

            sprintf(buffer, "%-4.4s%-2.2s%-4.4s", cfgobj["line1"].as<const char *>(), cfgobj["line2"].as<const char *>(), cfgobj["line3"].as<const char *>());
            taginfo->nextupdate = 3216153600;
            sendAPSegmentedData(mac, (String)buffer, 0x0000, false, (taginfo->isExternal == false));
            break;

#ifdef CONTENT_NFCLUT
        case 14:  // NFC URL

            taginfo->nextupdate = 3216153600;
            prepareNFCReq(mac, cfgobj["url"].as<const char *>());
            break;
#endif

#ifdef CONTENT_BUIENRADAR
        case 16:  // buienradar

        {
            const uint8_t refresh = drawBuienradar(filename, cfgobj, taginfo, imageParams);
            taginfo->nextupdate = now + refresh * 60;
            updateTagImage(filename, mac, refresh, taginfo, imageParams);
            break;
        }
#endif

        case 17:  // tag command
        {
            sendTagCommand(mac, cfgobj["cmd"].as<int>(), (taginfo->isExternal == false));
            taginfo->nextupdate = 3216153600;
            break;
        }

#ifdef CONTENT_TAGCFG
        case 18:  // tag config
        {
            prepareConfigFile(mac, cfgobj);
            taginfo->nextupdate = 3216153600;
            break;
        }
#endif

        case 19:  // json template
        {
            const String configFilename = cfgobj["filename"].as<String>();
            if (!util::isEmptyOrNull(configFilename)) {
                String configUrl = cfgobj["url"].as<String>();
                if (!util::isEmptyOrNull(configUrl)) {
                    JsonDocument json;
                    Serial.println("Get json url + file");

                    int index = configUrl.indexOf("{mac}");
                    if (index != -1) {
                        char macStr[17];
                        mac2hex(mac, macStr);
                        configUrl.replace("{mac}", macStr);
                    }

                    if (util::httpGetJson(configUrl, json, 1000)) {
                        taginfo->nextupdate = now + interval;
                        if (getJsonTemplateFileExtractVariables(filename, configFilename, json, taginfo, imageParams)) {
                            updateTagImage(filename, mac, interval / 60, taginfo, imageParams);
                        } else {
                            wsErr("error opening file " + configFilename);
                        }
                    } else {
                        taginfo->nextupdate = now + 600;
                    }

                } else {
                    const bool result = getJsonTemplateFile(filename, configFilename, taginfo, imageParams);
                    if (result) {
                        updateTagImage(filename, mac, interval, taginfo, imageParams);
                    } else {
                        wsErr("error opening file " + configFilename);
                    }
                    taginfo->nextupdate = 3216153600;
                }
            } else {
                const int httpcode = getJsonTemplateUrl(filename, cfgobj["url"], (time_t)cfgobj["#fetched"], String(hexmac), taginfo, imageParams);
                if (httpcode == 200) {
                    taginfo->nextupdate = now + interval;
                    updateTagImage(filename, mac, interval / 60, taginfo, imageParams);
                    cfgobj["#fetched"] = now;
                } else if (httpcode == 304) {
                    taginfo->nextupdate = now + interval;
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

#ifdef CONTENT_TIMESTAMP
        case 26:  // timestamp
            taginfo->nextupdate = 3216153600;
            drawTimestamp(filename, cfgobj, taginfo, imageParams);
            updateTagImage(filename, mac, 0, taginfo, imageParams);
            break;
#endif

#ifdef CONTENT_DAYAHEAD
        case 27:  // Day Ahead:

            if (getDayAheadFeed(filename, cfgobj, taginfo, imageParams)) {
                // Get user-configured update frequency in minutes, default to 15 minutes (matching 15-min data intervals)
                int updateFreqMinutes = 15;
                if (cfgobj["updatefreq"].is<String>()) {
                    String freqStr = cfgobj["updatefreq"].as<String>();
                    freqStr.trim();
                    if (freqStr.length() > 0) {
                        updateFreqMinutes = freqStr.toInt();
                        if (updateFreqMinutes < 1) {
                            wsErr("Invalid update frequency, defaulting to 15 minutes");
                            updateFreqMinutes = 15;
                        }
                    }
                }
                int updateInterval = updateFreqMinutes * 60;  // Convert to seconds

                // Schedule next update (align to interval boundary for consistent timing)
                taginfo->nextupdate = now + (updateInterval - now % updateInterval);
                updateTagImage(filename, mac, 0, taginfo, imageParams);
            } else {
                taginfo->nextupdate = now + 300;  // Retry in 5 minutes on failure
            }
            break;
#endif
        case 28:  // tag command
        {
            uint64_t newmac;
            sscanf(cfgobj["mac"].as<String>().c_str(), "%llx", &newmac);
            sendTagMac(mac, newmac, (taginfo->isExternal == false));
            taginfo->nextupdate = 3216153600;
            break;
        }
#ifdef CONTENT_TIME_RAWDATA
        case 29:  // Time and raw data like strings etc. in the future
        
            taginfo->nextupdate = now + 1800;
            prepareTIME_RAW(mac, now);
            break;
#endif
    }

    // keep a config that an upload or the web interface set while this ran
    if (taginfo->modeConfigJson == processedConfig) taginfo->modeConfigJson = doc.as<String>();
    clearFontCache();
}

bool updateTagImage(String &filename, const uint8_t *dst, uint16_t nextCheckin, tagRecord *&taginfo, imgParam &imageParams) {
    if (taginfo->hwType == SOLUM_SEG_UK) {
        sendAPSegmentedData(dst, (String)imageParams.segments, imageParams.symbols, (imageParams.invert == 1), (taginfo->isExternal == false));
    } else {
        if (imageParams.hasRed && imageParams.lut == EPD_LUT_NO_REPEATS && imageParams.shortlut == SHORTLUT_ONLY_BLACK) {
            imageParams.lut = EPD_LUT_DEFAULT;
        }

        if (imageParams.bpp == 3) {
            imageParams.dataType = DATATYPE_IMG_RAW_3BPP;
            Serial.println("datatype: DATATYPE_IMG_RAW_3BPP");
        } else if (imageParams.bpp == 4) {
            imageParams.dataType = DATATYPE_IMG_RAW_4BPP;
            Serial.println("datatype: DATATYPE_IMG_RAW_4BPP");
        } else if (imageParams.zlib) {
            imageParams.dataType = DATATYPE_IMG_ZLIB;
            Serial.println("datatype: DATATYPE_IMG_ZLIB");
        } else if (imageParams.g5) {
            imageParams.dataType = DATATYPE_IMG_G5;
            Serial.println("datatype: DATATYPE_IMG_G5");
        } else if (imageParams.hasRed) {
            imageParams.dataType = DATATYPE_IMG_RAW_2BPP;
            Serial.println("datatype: DATATYPE_IMG_RAW_2BPP");
        }
        if (nextCheckin > 0x7fff) nextCheckin = 0;
        prepareDataAvail(filename, imageParams.dataType, imageParams.lut, dst, nextCheckin);
    }
    return true;
}

char *formatHttpDate(const time_t t) {
    static char buf[40];
    struct tm *timeinfo;
    timeinfo = localtime(&t);                 // Get the local time
    const time_t utcTime = mktime(timeinfo);  // Convert to UTC
    timeinfo = gmtime(&utcTime);
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
    return buf;
}

String urlEncode(const char *msg) {
    constexpr const char *hex = "0123456789ABCDEF";
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

void getTemplate(JsonDocument &json, const uint8_t id, const uint16_t hwtype) {
    JsonDocument filter;
    JsonDocument doc;

    const String idstr = String(id);
    constexpr const char *templateKey = "template";

    char filename[20];
    snprintf(filename, sizeof(filename), hwtype > 0xFF ? "/tagtypes/%04X.json" : "/tagtypes/%02X.json", hwtype);
    File jsonFile = contentFS->open(filename, "r");

    if (jsonFile) {
        filter[templateKey][idstr] = true;
        filter["usetemplate"] = true;
        const DeserializationError error = deserializeJson(doc, jsonFile, DeserializationOption::Filter(filter));
        jsonFile.close();
        if (!error && doc[templateKey].is<JsonVariant>() && doc[templateKey][idstr].is<JsonVariant>()) {
            json.set(doc[templateKey][idstr]);
            return;
        }
        if (!error && doc["usetemplate"].is<uint16_t>()) {
            getTemplate(json, id, doc["usetemplate"]);
            return;
        }
        if (error) {
            Serial.println("json error in " + String(filename) + ": " + error.c_str());
        } else {
            Serial.println("no template for content " + idstr + " in " + String(filename));
        }
    } else {
        Serial.println("Failed to open " + String(filename));
    }
}

// The content types, one file each. Included here instead of compiled separately: as one
// compilation unit the firmware is ~12 KB smaller (ArduinoJson code is not duplicated per file).
#include "content/content_draw.inl"
#include "content/content_apinfo.inl"
#include "content/content_buienradar.inl"
#include "content/content_calendar.inl"
#include "content/content_date.inl"
#include "content/content_dayahead.inl"
#include "content/content_image.inl"
#include "content/content_json.inl"
#include "content/content_qr.inl"
#include "content/content_rss.inl"
#include "content/content_tagdata.inl"
#include "content/content_timestamp.inl"
#include "content/content_weather.inl"
