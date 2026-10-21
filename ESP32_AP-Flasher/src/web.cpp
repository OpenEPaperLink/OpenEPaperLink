#include "web.h"
#include "tlsr_flasher.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <FS.h>
#include <Preferences.h>
#include <WiFi.h>

#include <algorithm>

#include "AsyncJson.h"
#include "LittleFS.h"
#include "SPIFFSEditor.h"
#include "ble_writer.h"
#include "commstructs.h"
#include "language.h"
#include "leds.h"
#include "newproto.h"
#include "od_keys.h"
#include "od_session.h"
#include "pendingupload.h"
#include "radioselftest.h"
#include "ota.h"
#include "serialap.h"
#include "settings.h"
#include "storage.h"
#include "system.h"
#include "tag_db.h"
#include "udp.h"
#include "wifimanager.h"
#include <sys/time.h>

#ifdef HAS_EXT_FLASHER
#include "webflasher.h"
#endif

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// A restart asked for by a web request. The response is only sent once the handler returns, and the work before
// the restart (saving the tag database, a factory reset) shouldn't run in async_tcp: handleRestartRequest() does it
// in the loop task.
enum class RestartRequest : uint8_t { None, Reboot, WifiConfig, FactoryReset };
static volatile RestartRequest restartRequest = RestartRequest::None;

WifiManager wm;

SemaphoreHandle_t wsMutex;
uint32_t lastssidscan = 0;

void wsLog(const String &text) {
    JsonDocument doc;
    doc["logMsg"] = text;
    if (wsMutex) xSemaphoreTake(wsMutex, portMAX_DELAY);
    ws.textAll(doc.as<String>());
    if (wsMutex) xSemaphoreGive(wsMutex);
}

void wsErr(const String &text) {
    JsonDocument doc;
    doc["errMsg"] = text;
    if (wsMutex) xSemaphoreTake(wsMutex, portMAX_DELAY);
    ws.textAll(doc.as<String>());
    if (wsMutex) xSemaphoreGive(wsMutex);
}

// Live upload progress of data being sent to a display.
// Protocol: {"upload":{"src":"<16 hex MAC>","current":N,"total":M}}
// The frontend shows "P% (N/M)" on the tag card; current >= total means done.
void wsSendUploadProgress(const uint8_t *mac, uint16_t current, uint16_t total) {
    if (wsClientCount() == 0) return;  // nobody is listening, skip the work
    char hexmac[17];
    mac2hex(mac, hexmac);
    JsonDocument doc;
    JsonObject up = doc["upload"].to<JsonObject>();
    up["src"] = hexmac;
    up["current"] = current;
    up["total"] = total;
    if (wsMutex) xSemaphoreTake(wsMutex, portMAX_DELAY);
    ws.textAll(doc.as<String>());
    if (wsMutex) xSemaphoreGive(wsMutex);
}

// Sends the real touch positions to all connected websocket clients.
// Protocol: {"touch":{"count":N,"points":[{"id":..,"x":..,"y":..,"size":..}, ...]}}
// count == 0 (points empty) signals "all fingers released".
void wsSendTouch(uint8_t count, const uint16_t *xs, const uint16_t *ys, const uint8_t *ids, const uint8_t *sizes) {
    if (wsClientCount() == 0) return;  // nobody is listening, skip the work
    JsonDocument doc;
    JsonObject touch = doc["touch"].to<JsonObject>();
    touch["count"] = count;
    JsonArray points = touch["points"].to<JsonArray>();
    for (uint8_t i = 0; i < count; i++) {
        JsonObject p = points.add<JsonObject>();
        p["id"] = ids[i];
        p["x"] = xs[i];
        p["y"] = ys[i];
        p["size"] = sizes[i];
    }
    if (wsMutex) xSemaphoreTake(wsMutex, portMAX_DELAY);
    ws.textAll(doc.as<String>());
    if (wsMutex) xSemaphoreGive(wsMutex);
}

size_t dbSize() {
    TagDBLock lock;
    size_t size = tagDB.size() * sizeof(tagRecord);
    for (auto &tag : tagDB) {
        if (tag->data) {
            size += tag->len;
        }
        size += tag->modeConfigJson.length();
    }
    return size;
}

void wsSendSysteminfo() {
    JsonDocument doc;
    JsonObject sys = doc["sys"].to<JsonObject>();
    time_t now;
    time(&now);
    static int tagDBsizeLastRun = 0;
    static size_t tagDBsize = 0;

    sys["currtime"] = now;
    sys["heap"] = ESP.getFreeHeap();
    sys["recordcount"] = tagDBsize;
    sys["dbsize"] = dbSize();

    if (millis() - tagDBsizeLastRun > 30000 || tagDBsizeLastRun == 0) {
        tagDBsize = tagDB.size();
        tagDBsizeLastRun = millis();
    }
    // measuring walks the whole file system: report the last measurement, minus what was reserved since
    sys["littlefsfree"] = Storage.knownFreeSpace();

#if BOARD_HAS_PSRAM
    sys["psfree"] = ESP.getFreePsram();
#endif

    sys["apstate"] = apInfo.state;
    sys["runstate"] = config.runStatus;
    sys["rssi"] = WiFi.RSSI();
    sys["wifistatus"] = WiFi.status();
    sys["wifissid"] = WiFi.SSID();
    sys["uptime"] = esp_timer_get_time() / 1000000;

    static uint8_t day = 0;
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);

    if (day != timeinfo.tm_mday) {
        day = timeinfo.tm_mday;
        char timeBuffer[80];
        strftime(timeBuffer, sizeof(timeBuffer), languageDateFormat[0].c_str(), &timeinfo);
        setVarDB("ap_date", timeBuffer);
    }
    setVarDB("ap_ip", wm.localIP().toString());

#ifdef HAS_SUBGHZ
    String ApChanString = String(apInfo.channel);
    if(apInfo.hasSubGhz) {
       ApChanString += ", SubGhz ";
       if(apInfo.SubGhzChannel == 0) {
          ApChanString += "disabled";
       }
       else {
          ApChanString += String(apInfo.SubGhzChannel);
       }
    }
    setVarDB("ap_ch", ApChanString);
#else
    setVarDB("ap_ch", String(apInfo.channel));
#endif

    // reboot once at night, or when the BLE task hangs: only a reboot gets it going again
    const char* rebootReason = nullptr;
    if (timeinfo.tm_hour == 3 && timeinfo.tm_min == 56 && millis() > 2 * 3600 * 1000 && config.nightlyreboot == 1) {
        rebootReason = "Nightly reboot";
    }
#ifdef HAS_BLE_WRITER
    else if (BLE_taskHung()) {
        rebootReason = "BLE task hangs, reboot";
    }
#endif
    if (rebootReason != nullptr) {
        logLine(rebootReason);
        wsErr("REBOOTING");
        config.runStatus = RUNSTATUS_STOP;
        ws.enable(false);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        refreshAllPending();
        saveDB("/current/tagDB.json");
        ws.closeAll();
        delay(100);
        ESP.restart();
    }

    static uint32_t tagcounttimer = 0;
    if (millis() - tagcounttimer > 60000 || tagcounttimer == 0) {
        uint32_t timeoutcount = 0, lowbattcount = 0;
        uint32_t tagcount = getTagCount(timeoutcount, lowbattcount);
        sys["lowbattcount"] = lowbattcount;
        sys["timeoutcount"] = timeoutcount;
        char result[40];
        if (timeoutcount > 0) {
            snprintf(result, sizeof(result), "%lu/%lu, %lu timeout", tagcount, tagDB.size(), timeoutcount);
        } else {
            snprintf(result, sizeof(result), "%lu / %lu", tagcount, tagDB.size());
        }
        setVarDB("ap_tagcount", result);
#ifdef HAS_RGB_LED
        if (timeoutcount > 0) {
            if (apInfo.state == AP_STATE_ONLINE && apInfo.isOnline == true) rgbIdleColor = CRGB::DarkBlue;
        } else {
            if (apInfo.state == AP_STATE_ONLINE && apInfo.isOnline == true) rgbIdleColor = CRGB::Green;
        }
#endif
        tagcounttimer = millis();
    }

    xSemaphoreTake(wsMutex, portMAX_DELAY);
    ws.textAll(doc.as<String>());
    xSemaphoreGive(wsMutex);
}

void wsSendTaginfo(const uint8_t *mac, uint8_t syncMode) {
    if (syncMode != SYNC_DELETE) {
        String json = "";
        json = tagDBtoJson(mac);
        xSemaphoreTake(wsMutex, portMAX_DELAY);
        ws.textAll(json);
        xSemaphoreGive(wsMutex);
    }
    if (syncMode > SYNC_NOSYNC) {
        const tagRecord *taginfo = tagRecord::findByMAC(mac);
        if (taginfo != nullptr) {
            if (taginfo->contentMode != 12 || syncMode == SYNC_DELETE) {
                UDPcomm udpsync;
                struct TagInfo taginfoitem;
                memcpy(taginfoitem.mac, taginfo->mac, sizeof(taginfoitem.mac));
                taginfoitem.syncMode = syncMode;
                taginfoitem.contentMode = taginfo->contentMode;
                if (syncMode == SYNC_USERCFG) {
                    strncpy(taginfoitem.alias, taginfo->alias.c_str(), sizeof(taginfoitem.alias) - 1);
                    taginfoitem.alias[sizeof(taginfoitem.alias) - 1] = '\0';
                    taginfoitem.nextupdate = taginfo->nextupdate;
                }
                if (syncMode == SYNC_TAGSTATUS) {
                    taginfoitem.lastseen = taginfo->lastseen;
                    taginfoitem.nextupdate = taginfo->nextupdate;
                    taginfoitem.pendingCount = taginfo->pendingCount;
                    taginfoitem.expectedNextCheckin = taginfo->expectedNextCheckin;
                    taginfoitem.hwType = taginfo->hwType;
                    taginfoitem.wakeupReason = taginfo->wakeupReason;
                    taginfoitem.capabilities = taginfo->capabilities;
                    taginfoitem.pendingIdle = taginfo->pendingIdle;
                    memcpy(taginfoitem.md5, taginfo->md5, sizeof(taginfoitem.md5));
                }
                udpsync.netTaginfo(&taginfoitem);
            }
        }
    }
}

void wsSendAPitem(struct APlist *apitem) {
    JsonDocument doc;
    JsonObject ap = doc["apitem"].to<JsonObject>();

    char version_str[6];
    sprintf(version_str, "%04X", apitem->version);

    ap["ip"] = ((IPAddress)apitem->src).toString();
    ap["alias"] = apitem->alias;
    ap["count"] = apitem->tagCount;
    ap["channel"] = apitem->channelId;
    ap["version"] = version_str;
    ap["udp"] = apitem->udpVersion;

    if (wsMutex) xSemaphoreTake(wsMutex, portMAX_DELAY);
    ws.textAll(doc.as<String>());
    if (wsMutex) xSemaphoreGive(wsMutex);
}

void wsSerial(const String &text) {
    wsSerial(text, String(""));
}

void wsSerial(const String &text, const String &color) {
    JsonDocument doc;
    doc["console"] = text;
    if (!color.isEmpty()) doc["color"] = color;
    Serial.println(text);
    if (wsMutex) xSemaphoreTake(wsMutex, portMAX_DELAY);
    ws.textAll(doc.as<String>());
    if (wsMutex) xSemaphoreGive(wsMutex);
}

uint8_t wsClientCount() {
    return ws.count();
}

// Empty means an open hotspot; WPA2 takes 8 to 63 characters
static bool validHotspotPassword(const String &pw) {
    return pw.length() == 0 || (pw.length() >= 8 && pw.length() <= 63);
}

void init_web() {
    wsMutex = xSemaphoreCreateMutex();
    WiFi.mode(WIFI_STA);
    WiFi.setTxPower(static_cast<wifi_power_t>(config.wifiPower));

    wm.connectToWifi();

    server.addHandler(new SPIFFSEditor(*contentFS));

    server.addHandler(&ws);

    server.on("/reboot", HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "OK Reboot");
        restartRequest = RestartRequest::Reboot;
    });

    server.serveStatic("/current", *contentFS, "/current/").setCacheControl("max-age=604800");
    server.serveStatic("/tagtypes", *contentFS, "/tagtypes/").setCacheControl("max-age=300");

    server.on(
        "/imgupload", HTTP_POST, [](AsyncWebServerRequest *request) {
            if (!request->getResponse()) request->send(200);  // keep the response set by the upload handler
        },
        doImageUpload);
    server.on("/jsonupload", HTTP_POST, doJsonUpload);

    server.on("/get_db", HTTP_GET, [](AsyncWebServerRequest *request) {
        String json = "";
        if (request->hasParam("mac")) {
            String dst = request->getParam("mac")->value();
            uint8_t mac[8];
            if (hex2mac(dst, mac)) {
                json = tagDBtoJson(mac);
            } else {
                json = "{\"error\": \"malformatted parameter\"}";
            }
        } else {
            uint8_t startPos = 0;
            if (request->hasParam("pos")) {
                startPos = atoi(request->getParam("pos")->value().c_str());
            }
            json = tagDBtoJson(nullptr, startPos);
        }
        request->send(200, "application/json", json);
    });

    server.on("/getdata", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (request->hasParam("mac")) {
            String dst = request->getParam("mac")->value();
            uint8_t mac[8];
            if (hex2mac(dst, mac)) {
                tagRecord *taginfo = tagRecord::findByMAC(mac);
                if (taginfo != nullptr) {
                    if (request->hasParam("md5")) {
                        uint8_t md5[8];
                        if (hex2mac(request->getParam("md5")->value(), md5)) {
                            PendingItem *queueItem = getQueueItem(mac, *reinterpret_cast<uint64_t *>(md5));
                            if (queueItem == nullptr) {
                                Serial.println("getQueueItem: no queue item");
                                request->send(404, "text/plain", "File not found");
                                return;
                            }
                            if (!loadQueueItemData(queueItem)) {
                                request->send(404, "text/plain", "File not found");
                                return;
                            }
                            request->send(200, "application/octet-stream", queueItem->data, queueItem->len);
                            return;
                        }
                    } else {
                        // older version without queue
                        if (taginfo->data == nullptr) {
                            fs::File file = contentFS->open(taginfo->filename);
                            if (!file) {
                                request->send(404, "text/plain", "File not found");
                                return;
                            }
                            taginfo->data = getDataForFile(file);
                            file.close();
                        }
                        request->send(200, "application/octet-stream", taginfo->data, taginfo->len);
                        return;
                    }
                }
            }
        }
        request->send(400, "text/plain", "No data available");
    });

    server.on("/save_cfg", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("mac", true)) {
            String dst = request->getParam("mac", true)->value();
            uint8_t mac[8];
            if (hex2mac(dst, mac)) {
                tagRecord *taginfo = tagRecord::findByMAC(mac);
                if (taginfo != nullptr) {
                    if (request->hasParam("contentmode", true)) {
                        uint16_t newContentMode = atoi(request->getParam("contentmode", true)->value().c_str());
                        if (newContentMode != taginfo->contentMode && (newContentMode == 5 || newContentMode == 17 || newContentMode == 18)) {
                            // temporary content, restore after sending
                            pushTagInfo(taginfo);
                        }
                        taginfo->contentMode = newContentMode;
                    }
                    if (request->hasParam("alias", true)) {
                        taginfo->alias = request->getParam("alias", true)->value();
                    }
                    if (request->hasParam("modecfgjson", true)) {
                        taginfo->modeConfigJson = request->getParam("modecfgjson", true)->value();
                    }
                    taginfo->nextupdate = 0;
                    if (request->hasParam("rotate", true)) {
                        taginfo->rotate = atoi(request->getParam("rotate", true)->value().c_str());
                    }
                    if (request->hasParam("lut", true)) {
                        taginfo->lut = atoi(request->getParam("lut", true)->value().c_str());
                    }
                    if (request->hasParam("invert", true)) {
                        taginfo->invert = atoi(request->getParam("invert", true)->value().c_str());
                    }
                    wsSendTaginfo(mac, SYNC_USERCFG);
                    // saveDB("/current/tagDB.json");
                    request->send(200, "text/plain", "Ok, saved");
                } else {
                    request->send(200, "text/plain", "Error while saving: mac not found");
                }
            }
        }
        request->send(200, "text/plain", "Ok, saved");
    });

#ifdef HAS_BLE_WRITER
    // OpenDisplay tag, added by the user with or without an encryption key: GET says whether it
    // is added and has a key (never the key itself); POST sets the key (32 hex digits), adds the
    // tag without one (add=1) or removes either (empty key)
    server.on("/od_key", HTTP_ANY, [](AsyncWebServerRequest *request) {
        const bool post = request->method() == HTTP_POST;
        uint8_t mac[8];
        if (!request->hasParam("mac", post) || !hex2mac(request->getParam("mac", post)->value(), mac) ||
            bleFamily(mac, 0) != BLE_FAMILY_OPENDISPLAY) {
            request->send(400, "text/plain", "Not an OpenDisplay tag");
            return;
        }
        if (!post) {
            const bool set = odKeyHas(mac);
            request->send(200, "application/json", String("{\"set\":") + (set ? "true" : "false") + ",\"added\":" + (set || odKeyAdded(mac) ? "true" : "false") + "}");
            return;
        }
        if (request->hasParam("add", true)) {
            const bool added = odKeyAddWithoutKey(mac);
            odKeyChanged(mac);
            wsSendTaginfo(mac, SYNC_NOSYNC);
            request->send(200, "text/plain", added ? "Tag added without a key" : "Error adding the tag");
            return;
        }
        const String hex = request->hasParam("key", true) ? request->getParam("key", true)->value() : "";
        if (hex.isEmpty()) {
            odKeyClear(mac);
            odForget(mac);
            wsSendTaginfo(mac, SYNC_NOSYNC);  // this AP's view only, the tag may be on another AP
            request->send(200, "text/plain", "Removed: the AP no longer connects to this tag");
            return;
        }
        uint8_t key[16];
        bool valid = hex.length() == 32;
        for (int i = 0; valid && i < 32; i++) valid = isxdigit(hex[i]);
        if (valid) {
            for (int i = 0; i < 16; i++) key[i] = strtoul(hex.substring(i * 2, i * 2 + 2).c_str(), nullptr, 16);
        } else {
            request->send(400, "text/plain", "The key is 32 hex digits");
            return;
        }
        const bool saved = odKeySet(mac, key);
        memset(key, 0, sizeof(key));
        odKeyChanged(mac);
        wsSendTaginfo(mac, SYNC_NOSYNC);
        request->send(200, "text/plain", saved ? "Key saved" : "Error saving the key");
    });
#endif

    server.on("/tag_cmd", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("mac", true) && request->hasParam("cmd", true)) {
            uint8_t mac[8];
            if (hex2mac(request->getParam("mac", true)->value(), mac)) {
                tagRecord *taginfo = tagRecord::findByMAC(mac);
                if (taginfo != nullptr) {
                    const char *cmdValue = request->getParam("cmd", true)->value().c_str();
                    if (strcmp(cmdValue, "del") == 0) {
                        wsSendTaginfo(mac, SYNC_DELETE);
                        deleteRecord(mac);
                    }
                    if (strcmp(cmdValue, "purge") == 0) {
                        time_t now;
                        time(&now);
                        for (tagRecord *tag : tagDBSnapshot()) {
                            if (tag->expectedNextCheckin == 3216153600 || tag->lastseen < now - 24 * 3600 || now > tag->expectedNextCheckin + 600) {
                                wsSendTaginfo(tag->mac, SYNC_DELETE);
                                deleteRecord(tag->mac);
                            }
                        }
                    }
                    if (strcmp(cmdValue, "clear") == 0) {
                        clearPending(taginfo);
                        dequeueAllItems(mac);
                        taginfo->pendingCount = countQueueItem(mac);
                        wsSendTaginfo(mac, SYNC_TAGSTATUS);
                    }
                    if (strcmp(cmdValue, "refresh") == 0) {
                        updateContent(mac);
                    }
                    if (strcmp(cmdValue, "reboot") == 0) {
                        sendTagCommand(mac, CMD_DO_REBOOT, !taginfo->isExternal);
                    }
                    if (strcmp(cmdValue, "scan") == 0) {
                        sendTagCommand(mac, CMD_DO_SCAN, !taginfo->isExternal);
                    }
                    if (strcmp(cmdValue, "reset") == 0) {
                        sendTagCommand(mac, CMD_DO_RESET_SETTINGS, !taginfo->isExternal);
                    }
                    if (strcmp(cmdValue, "deepsleep") == 0) {
                        sendTagCommand(mac, CMD_DO_DEEPSLEEP, !taginfo->isExternal);
                        taginfo->pendingIdle = 9999;
                        wsSendTaginfo(mac, SYNC_TAGSTATUS);
                    }
                    if (strcmp(cmdValue, "ledflash") == 0) {
                        struct ledFlash flashData = {0};
                        flashData.mode = 1;
                        flashData.flashDuration = 8;
                        flashData.color1 = 0x3C;  // green
                        flashData.color2 = 0xE4;  // red
                        flashData.color3 = 0x03;  // blue
                        flashData.flashCount1 = 3;
                        flashData.flashCount2 = 3;
                        flashData.flashCount3 = 3;
                        flashData.delay1 = 10;
                        flashData.delay2 = 10;
                        flashData.delay3 = 10;
                        flashData.flashSpeed1 = 1;
                        flashData.flashSpeed2 = 5;
                        flashData.flashSpeed3 = 10;
                        flashData.repeats = 2;
                        const uint8_t *payload = reinterpret_cast<const uint8_t *>(&flashData);
                        sendTagCommand(mac, CMD_DO_LEDFLASH, !taginfo->isExternal, payload);
                    }
                    if (strcmp(cmdValue, "ledflash_long") == 0) {
                        struct ledFlash flashData = {0};
                        flashData.mode = 1;
                        flashData.flashDuration = 1;
                        flashData.color1 = 0xE4;  // red
                        flashData.flashCount1 = 3;
                        flashData.flashSpeed1 = 3;
                        flashData.delay1 = 50;
                        flashData.repeats = 60;
                        const uint8_t *payload = reinterpret_cast<const uint8_t *>(&flashData);
                        sendTagCommand(mac, CMD_DO_LEDFLASH, !taginfo->isExternal, payload);
                    }
                    if (strcmp(cmdValue, "ledflash_stop") == 0) {
                        struct ledFlash flashData = {0};
                        flashData.mode = 0;
                        const uint8_t *payload = reinterpret_cast<const uint8_t *>(&flashData);
                        sendTagCommand(mac, CMD_DO_LEDFLASH, !taginfo->isExternal, payload);
                    }
                    request->send(200, "text/plain", "Ok, done");
                } else {
                    request->send(400, "text/plain", "Error: mac not found");
                }
            }
        } else {
            request->send(500, "text/plain", "param error");
        }
    });

    server.on("/led_flash", HTTP_GET, [](AsyncWebServerRequest *request) {
        //  color picker: https://roger-random.github.io/RGB332_color_wheel_three.js/
        //  http GET to /led_flash?mac=000000000000&pattern=000000000000000000000000
        //  see https://github.com/OpenEPaperLink/OpenEPaperLink/wiki/Led-control
        if (request->hasParam("mac")) {
            String dst = request->getParam("mac")->value();
            uint8_t mac[8];
            if (hex2mac(dst, mac)) {
                tagRecord *taginfo = tagRecord::findByMAC(mac);
                if (taginfo != nullptr) {
                    uint8_t payload[12] = {0};
                    if (request->hasParam("pattern")) {
                        if (sscanf(request->getParam("pattern")->value().c_str(), "%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
                                   &payload[0], &payload[1], &payload[2], &payload[3],
                                   &payload[4], &payload[5], &payload[6], &payload[7],
                                   &payload[8], &payload[9], &payload[10], &payload[11]) != 12) {
                            request->send(400, "text/plain", "Error: expects 12 hex bytes in pattern");
                            return;
                        }
                    }
                    sendTagCommand(mac, CMD_DO_LEDFLASH, !taginfo->isExternal, payload);
                    request->send(200, "text/plain", "ok, request transmitted");
                    return;
                }
            }
        }
        request->send(400, "text/plain", "parameters are missing");
    });

    server.on("/get_ap_config", HTTP_GET, [](AsyncWebServerRequest *request) {
        UDPcomm udpsync;
        udpsync.getAPList();
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        String HasC6 = "0";
        String HasH2 = "0";
        String HasTSLR = "0";

        response->print("{");
        /* These are capabilities of the board, not a single choice. A radio
           co-processor is either an H2 or a C6, so those stay exclusive - but
           an SWS wired TLSR is independent of that, and a board that has both
           can flash either. Reporting them separately lets the web interface
           offer whatever the hardware can actually reach. */
#ifdef HAS_H2
        HasH2 = "1";
#elif defined(C6_OTA_FLASHING)
        HasC6 = "1";
#endif
#if defined(FLASHER_AP_SWS) || defined(HAS_TSLR)
        HasTSLR = "1";
#endif
        response->print("\"C6\": \"" + HasC6 + "\", ");
        response->print("\"H2\": \"" + HasH2 + "\", ");
        response->print("\"TLSR\": \"" + HasTSLR + "\", ");
#ifdef SAVE_SPACE
        response->print("\"savespace\": \"1\", ");
#else
        response->print("\"savespace\": \"0\", ");
#endif
#ifdef HAS_EXT_FLASHER
        response->print("\"hasFlasher\": \"1\", ");
#else
        response->print("\"hasFlasher\": \"0\", ");
#endif
#ifdef HAS_BLE_WRITER
        response->print("\"hasBLE\": \"1\", ");
#else
        response->print("\"hasBLE\": \"0\", ");
#endif

#ifdef HAS_SUBGHZ
        response->print("\"hasSubGhz\": \"" + String(apInfo.hasSubGhz) + "\",");
#else
        response->print("\"hasSubGhz\": \"0\", ");
#endif

        {
            Preferences preferences;
            preferences.begin("wifi", true);
            JsonDocument pw;
            pw.set(preferences.getString("appw", ""));
            preferences.end();
            response->print("\"hotspotpw\": ");
            serializeJson(pw, *response);
            response->print(", ");
        }

        response->print("\"apstate\": \"" + String(apInfo.state) + "\"");

        File configFile = contentFS->open("/current/apconfig.json", "r");
        if (configFile) {
            response->print(", ");
            configFile.seek(1);
            const size_t bufferSize = 64;
            uint8_t buffer[bufferSize];
            while (configFile.available()) {
                size_t bytesRead = configFile.read(buffer, bufferSize);
                response->write(buffer, bytesRead);
            }
            configFile.close();
        } else {
            response->print("}");
        }

        request->send(response);
    });

    server.on("/save_apcfg", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("hotspotpw", true)) {
            const String pw = request->getParam("hotspotpw", true)->value();
            if (!validHotspotPassword(pw)) {
                request->send(400, "text/plain", "Hotspot password needs 8 to 63 characters");
                return;
            }
            Preferences preferences;
            preferences.begin("wifi", false);
            preferences.putString("appw", pw);
            preferences.end();
        }
        if (request->hasParam("alias", true)) {
            String aliasValue = request->getParam("alias", true)->value();
            size_t aliasLength = aliasValue.length();
            if (aliasLength > 31) aliasLength = 31;
            aliasValue.toCharArray(config.alias, aliasLength + 1);
            config.alias[aliasLength] = '\0';
        }


        if (request->hasParam("channel", true)) {
            config.channel = static_cast<uint8_t>(request->getParam("channel", true)->value().toInt());
        }
        if (request->hasParam("subghzchannel", true)) {
            config.subghzchannel = static_cast<uint8_t>(request->getParam("subghzchannel", true)->value().toInt());
        }
        if (request->hasParam("led", true)) {
            config.led = static_cast<uint8_t>(request->getParam("led", true)->value().toInt());
            updateBrightnessFromConfig();
        }
        if (request->hasParam("tft", true)) {
            config.tft = static_cast<uint8_t>(request->getParam("tft", true)->value().toInt());
            updateBrightnessFromConfig();
        }
        if (request->hasParam("language", true)) {
            config.language = static_cast<uint8_t>(request->getParam("language", true)->value().toInt());
            updateLanguageFromConfig();
        }
        if (request->hasParam("maxsleep", true)) {
            config.maxsleep = static_cast<uint8_t>(request->getParam("maxsleep", true)->value().toInt());
        }
        if (request->hasParam("stopsleep", true)) {
            config.stopsleep = static_cast<uint8_t>(request->getParam("stopsleep", true)->value().toInt());
        }
        if (request->hasParam("preview", true)) {
            config.preview = static_cast<uint8_t>(request->getParam("preview", true)->value().toInt());
        }
        if (request->hasParam("nightlyreboot", true)) {
            config.nightlyreboot = static_cast<uint8_t>(request->getParam("nightlyreboot", true)->value().toInt());
        }
        if (request->hasParam("lock", true)) {
            config.lock = static_cast<uint8_t>(request->getParam("lock", true)->value().toInt());
        }
        if (request->hasParam("ble", true)) {
            config.ble = static_cast<uint8_t>(request->getParam("ble", true)->value().toInt());
        }
        if (request->hasParam("sleeptime1", true)) {
            config.sleepTime1 = static_cast<uint8_t>(request->getParam("sleeptime1", true)->value().toInt());
            config.sleepTime2 = static_cast<uint8_t>(request->getParam("sleeptime2", true)->value().toInt());
        }
        if (request->hasParam("wifipower", true)) {
            config.wifiPower = static_cast<uint8_t>(request->getParam("wifipower", true)->value().toInt());
            WiFi.setTxPower(static_cast<wifi_power_t>(config.wifiPower));
        }
        if (request->hasParam("timezone", true)) {
            strncpy(config.timeZone, request->getParam("timezone", true)->value().c_str(), sizeof(config.timeZone) - 1);
            config.timeZone[sizeof(config.timeZone) - 1] = '\0';
            setenv("TZ", config.timeZone, 1);
            tzset();
        }
        if (request->hasParam("discovery", true)) {
            config.discovery = static_cast<uint8_t>(request->getParam("discovery", true)->value().toInt());
        }
        if (request->hasParam("showtimestamp", true)) {
            config.showtimestamp = static_cast<uint8_t>(request->getParam("showtimestamp", true)->value().toInt());
        }
        if (request->hasParam("repo", true)) {
            config.repo = request->getParam("repo", true)->value();
        }
        if (request->hasParam("env", true)) {
            config.env = request->getParam("env", true)->value();
        }
        /* Pins used only while flashing a co-processor. Normal operation
           keeps the compile time pinout, so a wrong value here can cost a
           flash attempt but never the running access point. -1 means the
           board has no such pin. */
        {
            struct { const char *name; int8_t *field; } flashPins[] = {
                {"flashpin_sws", &config.flashPinSws},
                {"flashpin_reset", &config.flashPinReset},
                {"flashpin_txd", &config.flashPinTxd},
                {"flashpin_rxd", &config.flashPinRxd},
                {"flashpin_prog", &config.flashPinProg},
                {"flashpin_dbgtxd", &config.flashPinDbgTxd},
                {"flashpin_dbgrxd", &config.flashPinDbgRxd},
            };
            for (auto &p : flashPins) {
                if (!request->hasParam(p.name, true)) continue;
                long v = request->getParam(p.name, true)->value().toInt();
                if (v < -1 || v > 48) continue;   /* outside any ESP32 GPIO */
                *p.field = (int8_t)v;
            }
        }

        saveAPconfig();
        setAPchannel();
        request->send(200, "text/plain", "Ok, saved");
    });

    // Allow external time sync (e.g., from Home Assistant) without Internet
    // Usage: POST /set_time with form field 'epoch' (UNIX time seconds)
    server.on("/set_time", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("epoch", true)) {
            time_t epoch = static_cast<time_t>(request->getParam("epoch", true)->value().toInt());
            if (epoch > 1600000000) { // basic sanity check (~2020-09-13)
                struct timeval tv;
                tv.tv_sec = epoch;
                tv.tv_usec = 0;
                settimeofday(&tv, nullptr);
                logLine("Time set via /set_time");
                wsSendSysteminfo();
                request->send(200, "text/plain", "ok");
                return;
            } else {
                request->send(400, "text/plain", "invalid epoch");
                return;
            }
        }
        request->send(400, "text/plain", "missing 'epoch'");
    });

    server.on("/set_var", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("key", true) && request->hasParam("val", true)) {
            std::string key = request->getParam("key", true)->value().c_str();
            String val = request->getParam("val", true)->value();
            Serial.printf("set key %s value %s\r\n", key.c_str(), val);
            setVarDB(key, val);
            request->send(200, "text/plain", "Ok, saved");
        } else {
            request->send(500, "text/plain", "param error");
        }
    });
    server.on("/set_vars", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("json", true)) {
            JsonDocument jsonDocument;
            DeserializationError error = deserializeJson(jsonDocument, request->getParam("json", true)->value());
            if (error) {
                request->send(400, "text/plain", "Failed to parse JSON");
                return;
            }
            for (JsonPair kv : jsonDocument.as<JsonObject>()) {
                std::string key = kv.key().c_str();
                String val = kv.value().as<String>();
                Serial.printf("set key %s value %s\r\n", key.c_str(), val);
                setVarDB(key, val);
            }
            request->send(200, "text/plain", "JSON uploaded and processed");
        } else {
            request->send(400, "text/plain", "No 'json' parameter found in request");
        }
    });

    // setup

    server.on("/setup", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(*contentFS, "/www/setup.html");
    });

    server.on("/get_wifi_config", HTTP_GET, [](AsyncWebServerRequest *request) {
        Preferences preferences;
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        JsonDocument doc;
        preferences.begin("wifi", false);
        const char *keys[] = {"ssid", "pw", "ip", "mask", "gw", "dns"};
        const size_t numKeys = sizeof(keys) / sizeof(keys[0]);
        for (size_t i = 0; i < numKeys; i++) {
            doc[keys[i]] = preferences.getString(keys[i], "");
        }
        doc["appw"] = preferences.getString("appw", "");
        doc["apfallback"] = preferences.getUChar("apfallback", 1);
        doc["mac"] = WiFi.macAddress();
        serializeJson(doc, *response);
        request->send(response);
    });

    server.on("/get_ssid_list", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        JsonDocument doc;

        doc["scanstatus"] = WiFi.scanComplete();
        JsonArray networks = doc["networks"].to<JsonArray>();
        for (int i = 0; i < (WiFi.scanComplete() > 50 ? 50 : WiFi.scanComplete()); ++i) {
            if (WiFi.SSID(i) != "") {
                JsonObject network = networks.add<JsonObject>();
                network["ssid"] = WiFi.SSID(i);
                network["ch"] = WiFi.channel(i);
                network["rssi"] = WiFi.RSSI(i);
                network["enc"] = WiFi.encryptionType(i);
            }
        }
        if (WiFi.scanComplete() != -1 && (WiFi.scanComplete() == -2 || millis() - lastssidscan > 30000)) {
            WiFi.scanDelete();
            Serial.println("start scanning");
            WiFi.scanNetworks(true, true);
            lastssidscan = millis();
        }

        serializeJson(doc, *response);
        request->send(response);
    });

    AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler("/save_wifi_config", [](AsyncWebServerRequest *request, JsonVariant &json) {
        const JsonObject &jsonObj = json.as<JsonObject>();
        if (jsonObj["appw"].is<String>() && !validHotspotPassword(jsonObj["appw"].as<String>())) {
            request->send(400, "text/plain", "Hotspot password needs 8 to 63 characters");
            return;
        }
        Preferences preferences;
        preferences.begin("wifi", false);
        const char *keys[] = {"ssid", "pw", "ip", "mask", "gw", "dns", "appw"};
        const size_t numKeys = sizeof(keys) / sizeof(keys[0]);
        for (size_t i = 0; i < numKeys; i++) {
            String key = keys[i];
            if (jsonObj[key].is<String>()) {
                preferences.putString(key.c_str(), jsonObj[key].as<String>());
            }
        }
        if (jsonObj["apfallback"].is<int>()) {
            preferences.putUChar("apfallback", jsonObj["apfallback"].as<int>() ? 1 : 0);
        }
        preferences.end();
        Serial.println("config saved");
        request->send(200, "text/plain", "Ok, saved");
        restartRequest = jsonObj["ssid"].as<String>() == "factory" ? RestartRequest::FactoryReset : RestartRequest::WifiConfig;
    });
    server.addHandler(handler);

    // end of setup

    server.on("/backup_db", HTTP_GET, [](AsyncWebServerRequest *request) {
        saveDB("/current/tagDB.json");
        request->send(*contentFS, "/current/tagDB.json", String(), true);
    });
    server.on(
        "/restore_db", HTTP_POST, [](AsyncWebServerRequest *request) {
            if (!request->getResponse()) request->send(200);  // keep the response set by the upload handler
        },
        dotagDBUpload);

    // OTA related calls

    server.on("/sysinfo", HTTP_GET, handleSysinfoRequest);
    // core dump of the last crash, for: espcoredump.py info_corefile -t raw -c coredump.bin firmware.elf
    server.on("/coredump", HTTP_GET, [](AsyncWebServerRequest *request) {
        const size_t size = coreDumpSize();
        if (size == 0) {
            request->send(404, "text/plain", "No core dump");
            return;
        }
        AsyncWebServerResponse *response = request->beginResponse("application/octet-stream", size, [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
            return readCoreDump(buffer, index, maxLen);
        });
        response->addHeader("Content-Disposition", "attachment; filename=\"coredump.bin\"");
        request->send(response);
    });
    server.on("/coredump_erase", HTTP_POST, [](AsyncWebServerRequest *request) {
        const bool erased = eraseCoreDump();
        request->send(erased ? 200 : 500, "text/plain", erased ? "OK" : "Failed");
    });
    server.on("/check_file", HTTP_GET, handleCheckFile);
    server.on("/github_proxy", HTTP_GET, handleGithubProxy);
    server.on("/rollback", HTTP_POST, handleRollback);
    server.on("/update_c6", HTTP_POST, handleUpdateC6);
    /* Radio wiring self-test (task #32). POST starts it (the radio is offline
       for 5-10 s), GET returns the last result. For development, protoTx,
       protoRx, dbgTx, dbgRx, reset and prog (S3-side GPIOs, -2 = not
       connected) replace the configured pins, to simulate wiring faults on a
       correctly wired AP. */
    server.on("/radio_selftest", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "application/json", radioSelfTestResult());
    });
    server.on("/radio_selftest", HTTP_POST, [](AsyncWebServerRequest *request) {
        RadioPins pins = radioPinsFromConfig();
        struct { const char *name; int8_t *pin; } params[] = {
            {"protoTx", &pins.protoTx}, {"protoRx", &pins.protoRx}, {"dbgTx", &pins.dbgTx},
            {"dbgRx", &pins.dbgRx}, {"reset", &pins.reset}, {"prog", &pins.prog},
        };
        for (auto &p : params) {
            if (!request->hasParam(p.name, true)) continue;
            long v = request->getParam(p.name, true)->value().toInt();
            if (v < -2 || v > 48) {  // -2: simulate a wire that is not connected
                request->send(400, "text/plain", String("invalid pin for ") + p.name);
                return;
            }
            *p.pin = (int8_t)v;
        }
        if (startRadioSelfTest(pins)) {
            request->send(202, "text/plain", "started");
        } else {
            request->send(409, "text/plain", "not possible now (test or flashing running, or no ESP radio)");
        }
    });
#ifdef FLASHER_AP_SWS
    /* Flash the TLSR over SWS. Defaults to the firmware already on the
       filesystem; "url" fetches a release directory instead. */
    server.on("/flash_tlsr", HTTP_POST, [](AsyncWebServerRequest *request) {
        /* Zigbee is the default: that is what most tags in the field expect.
           The GFSK build is the special case and carries the suffix. The two
           are different radios, so each has its own file. */
        String url = "", apType = "F8";
        if (request->hasParam("url", true)) url = request->getParam("url", true)->value();
        if (request->hasParam("aptype", true)) apType = request->getParam("aptype", true)->value();
        String path = (apType == "F9") ? "/TLSR_AP_FW_GFSK.bin" : "/TLSR_AP_FW.bin";
        if (request->hasParam("file", true)) path = request->getParam("file", true)->value();
        tlsrFlashStart(path, url, apType);
        request->send(200, "text/plain", "started");
    });
#endif
    server.on("/update_actions", HTTP_POST, handleUpdateActions);
    server.on("/update_ota", HTTP_POST, [](AsyncWebServerRequest *request) {
        handleUpdateOTA(request);
    });
    server.on(
        "/littlefs_put", HTTP_POST, [](AsyncWebServerRequest *request) {
            if (!request->getResponse()) request->send(200);  // keep the response set by the upload handler
        },
        handleLittleFSUpload);

#ifdef HAS_EXT_FLASHER

    // Flasher related calls
    ws.onEvent([](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
        if (type == WS_EVT_DATA) handleWSdata(data, len, client);
    });

#endif

    server.onNotFound([](AsyncWebServerRequest *request) {
        if (request->url() == "/" || request->url() == "index.htm") {
            request->send(200, "text/html", "index.html not found. Did you forget to upload the littlefs partition?");
            return;
        }
        request->send(404);
    });

    server.serveStatic("/", *contentFS, "/www/").setDefaultFile("index.html");

    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "content-type");

    server.begin();
}

/// @brief State of an /imgupload while it is received, in AsyncWebServerRequest::_tempObject
/// @note Allocated with malloc(), because the web server releases _tempObject with free() when a request is aborted
struct ImageUpload {
    uint8_t mac[8];
    char name[48];
    uint8_t *data;  // PSRAM, see pendingupload.h
    size_t size;
    size_t len;
    bool overflow;
};

void handleRestartRequest() {
    switch (restartRequest) {
        case RestartRequest::None:
            return;
        case RestartRequest::Reboot:
            logLine("Reboot request by user");
            wsErr("REBOOTING");
            delay(100);
            ws.enable(false);
            refreshAllPending();
            saveDB("/current/tagDB.json");
            break;
        case RestartRequest::WifiConfig:
            ws.enable(false);
            refreshAllPending();
            saveDB("/current/tagDB.json");
            break;
        case RestartRequest::FactoryReset: {
            ws.enable(false);
            config.runStatus = RUNSTATUS_STOP;
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            Preferences preferences;
            preferences.begin("wifi", false);
            preferences.putString("ssid", "");
            preferences.putString("pw", "");
            preferences.end();
            destroyDB();
            cleanupCurrent();
            contentFS->remove("/AP_FW_Pack.bin");
            contentFS->remove("/OpenEPaperLink_esp32_C6.bin");
            contentFS->remove("/OpenEPaperLink_esp32_C6uart0.bin");
            contentFS->remove("/bootloader.bin");
            contentFS->remove("/partition-table.bin");
            contentFS->remove("/update_actions.json");
            contentFS->remove("/log.txt");
            contentFS->remove("/logold.txt");
            contentFS->remove("/current/tagDB.json");
            contentFS->remove("/current/tagDB.json.bak");
            contentFS->remove("/current/tagDBrestored.json");
            contentFS->remove("/current/apconfig.json");
            delay(100);
            esp_deep_sleep_start();
            break;
        }
    }
    ws.closeAll();
    delay(100);
    ESP.restart();
}

void doImageUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!index) {
        if (config.runStatus != RUNSTATUS_RUN) {
            request->send(409, "text/plain", "Come back later");
            return;
        }
        uint8_t mac[8];
        if (!request->hasParam("mac", true) || !hex2mac(request->getParam("mac", true)->value(), mac)) {
            request->send(400, "text/plain", "parameters incomplete");
            return;
        }
        const size_t size = request->contentLength();
        if (size == 0) {
            request->send(411, "text/plain", "Content-Length required");
            return;
        }
        if (size > IMAGE_UPLOAD_MAX_SIZE) {
            request->send(413, "text/plain", "Image too large");
            return;
        }

        const String name = "/temp/" + request->getParam("mac", true)->value() + "_" + String(millis()) + ".jpg";
        logLine("http imageUpload " + name);

        ImageUpload *upload = static_cast<ImageUpload *>(malloc(sizeof(ImageUpload)));
        uint8_t *buffer = upload != nullptr ? allocUploadBuffer(size) : nullptr;
        if (buffer == nullptr) {
            free(upload);
            AsyncWebServerResponse *response = request->beginResponse(409, "text/plain", "Busy, come back later");
            response->addHeader("Retry-After", "5");
            request->send(response);
            return;
        }
        memcpy(upload->mac, mac, sizeof(mac));
        strlcpy(upload->name, name.c_str(), sizeof(upload->name));
        upload->data = buffer;
        upload->size = size;
        upload->len = 0;
        upload->overflow = false;
        request->_tempObject = upload;
        request->onDisconnect([request]() {
            // aborted before the end: the buffer isn't stored, so free it (the server frees _tempObject)
            const ImageUpload *upload = static_cast<const ImageUpload *>(request->_tempObject);
            if (upload != nullptr) freeUploadBuffer(upload->data, upload->size);
        });
    }

    ImageUpload *upload = static_cast<ImageUpload *>(request->_tempObject);
    if (upload == nullptr) return;

    if (upload->len + len > upload->size) {
        upload->overflow = true;
    } else if (len) {
        memcpy(upload->data + upload->len, data, len);
        upload->len += len;
    }
    if (!final) return;

    request->_tempObject = nullptr;
    const ImageUpload received = *upload;
    free(upload);

    tagRecord *taginfo = received.overflow ? nullptr : tagRecord::findByMAC(received.mac);
    if (taginfo == nullptr) {
        freeUploadBuffer(received.data, received.size);
        request->send(400, "text/plain", received.overflow ? "upload larger than its content length" : "mac not found");
        return;
    }
    const String uploadfilename = received.name;

    uint8_t dither = 1;
    if (request->hasParam("dither", true)) {
        dither = request->getParam("dither", true)->value().toInt();
    }
    if (request->hasParam("alias", true)) {
        taginfo->alias = request->getParam("alias", true)->value();
    }
    if (request->hasParam("rotate", true)) {
        taginfo->rotate = atoi(request->getParam("rotate", true)->value().c_str());
    }
    if (request->hasParam("lut", true)) {
        taginfo->lut = atoi(request->getParam("lut", true)->value().c_str());
    }
    if (request->hasParam("invert", true)) {
        taginfo->invert = atoi(request->getParam("invert", true)->value().c_str());
    }
    uint32_t ttl = 0;
    if (request->hasParam("ttl", true)) {
        ttl = request->getParam("ttl", true)->value().toInt();
    }
    uint8_t preload = 0;
    uint8_t preloadlut = 0;
    uint8_t preloadtype = 0;
    if (request->hasParam("preloadtype", true)) {
        preload = 1;
        preloadtype = request->getParam("preloadtype", true)->value().toInt();
        if (request->hasParam("preloadlut", true)) {
            preloadlut = request->getParam("preloadlut", true)->value().toInt();
        }
    }
    {
        // the stored image and the config that names it change together (see dropStalePendingUploads)
        std::lock_guard<std::recursive_mutex> lock(pendingUploadMutex());
        storePendingUpload(received.mac, uploadfilename, received.data, received.len, received.size);
        taginfo->modeConfigJson = "{\"filename\":\"" + uploadfilename + "\",\"timetolive\":\"" + String(ttl) + "\",\"dither\":\"" + String(dither) + "\",\"delete\":\"1\", \"preload\":\"" + String(preload) + "\", \"preload_lut\":\"" + String(preloadlut) + "\", \"preload_type\":\"" + String(preloadtype) + "\"}";
        if (request->hasParam("contentmode", true)) {
            taginfo->contentMode = request->getParam("contentmode", true)->value().toInt();
        } else {
            taginfo->contentMode = 24;
        }
        taginfo->nextupdate = 0;
    }
    Serial.println("upload finished " + uploadfilename);
    wsSendTaginfo(received.mac, SYNC_USERCFG);
    request->send(200, "text/plain", "Ok, saved");
}

void doJsonUpload(AsyncWebServerRequest *request) {
    if (config.runStatus != RUNSTATUS_RUN) {
        request->send(409, "text/plain", "come back later");
        return;
    }
    if (request->hasParam("mac", true) && request->hasParam("json", true)) {
        String dst = request->getParam("mac", true)->value();
        uint8_t mac[8];
        if (hex2mac(dst, mac)) {
            xSemaphoreTake(fsMutex, portMAX_DELAY);
            File file = contentFS->open("/current/" + dst + ".json", "w");
            if (!file) {
                request->send(400, "text/plain", "Failed to create file");
                xSemaphoreGive(fsMutex);
                return;
            }
            file.print(request->getParam("json", true)->value());
            file.close();
            xSemaphoreGive(fsMutex);
            tagRecord *taginfo = tagRecord::findByMAC(mac);
            if (taginfo != nullptr) {
                uint32_t ttl = 0;
                if (request->hasParam("ttl", true)) {
                    ttl = request->getParam("ttl", true)->value().toInt();
                }
                taginfo->modeConfigJson = "{\"filename\":\"/current/" + dst + ".json\",\"interval\":\"" + String(ttl) + "\"}";
                taginfo->contentMode = 19;
                taginfo->nextupdate = 0;
                wsSendTaginfo(mac, SYNC_USERCFG);
                request->send(200, "text/plain", "Ok, saved");
            } else {
                request->send(400, "text/plain", "mac not found in tagDB");
            }
        }
        return;
    }
    request->send(400, "text/plain", "Missing parameters");
}

void dotagDBUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    // fsMutex is only held per chunk: an aborted upload would otherwise never release it
    if (!index) {
        logLine("restore tagDB");
        xSemaphoreTake(fsMutex, portMAX_DELAY);
        request->_tempFile = contentFS->open("/current/tagDBrestored.json", "w");
        xSemaphoreGive(fsMutex);
    }
    if (!request->_tempFile) {
        if (final) request->send(507, "text/plain", "Error writing file");
        return;
    }
    if (len) {
        xSemaphoreTake(fsMutex, portMAX_DELAY);
        request->_tempFile.write(data, len);
        xSemaphoreGive(fsMutex);
    }
    if (final) {
        xSemaphoreTake(fsMutex, portMAX_DELAY);
        request->_tempFile.close();
        xSemaphoreGive(fsMutex);
        destroyDB();
        loadDB("/current/tagDBrestored.json");
        request->send(200, "text/plain", "Ok, restored.");
    }
}
