#include "web.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <FS.h>
#include <Preferences.h>
#include <WiFi.h>

#include "AsyncJson.h"
#include "LittleFS.h"
#include "SPIFFSEditor.h"
#include "commstructs.h"
#include "language.h"
#include "leds.h"
#include "newproto.h"
#include "ota.h"
#include "serialap.h"
#include "settings.h"
#include "storage.h"
#include "system.h"
#include "tag_db.h"
#include "udp.h"
#include "wifimanager.h"

extern uint8_t data_to_send[];

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
WifiManager wm;

SemaphoreHandle_t wsMutex;
uint32_t lastssidscan = 0;

void wsLog(const String &text) {
    StaticJsonDocument<250> doc;
    doc["logMsg"] = text;
    if (wsMutex) xSemaphoreTake(wsMutex, portMAX_DELAY);
    ws.textAll(doc.as<String>());
    if (wsMutex) xSemaphoreGive(wsMutex);
}

void wsErr(const String &text) {
    StaticJsonDocument<250> doc;
    doc["errMsg"] = text;
    if (wsMutex) xSemaphoreTake(wsMutex, portMAX_DELAY);
    ws.textAll(doc.as<String>());
    if (wsMutex) xSemaphoreGive(wsMutex);
}

size_t dbSize() {
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
    DynamicJsonDocument doc(250);
    JsonObject sys = doc.createNestedObject("sys");
    time_t now;
    time(&now);
    static int freeSpaceLastRun = 0;
    static size_t tagDBsize = 0;
    static uint64_t freeSpace = Storage.freeSpace();

    sys["currtime"] = now;
    sys["heap"] = ESP.getFreeHeap();
    sys["recordcount"] = tagDBsize;
    sys["dbsize"] = dbSize();

    if (millis() - freeSpaceLastRun > 30000 || freeSpaceLastRun == 0) {
        freeSpace = Storage.freeSpace();
        tagDBsize = tagDB.size();
        freeSpaceLastRun = millis();
    }
    sys["littlefsfree"] = freeSpace;

#if BOARD_HAS_PSRAM
    sys["psfree"] = ESP.getFreePsram();
#endif

    sys["apstate"] = apInfo.state;
    sys["runstate"] = config.runStatus;
    sys["rssi"] = WiFi.RSSI();
    sys["wifistatus"] = WiFi.status();
    sys["wifissid"] = WiFi.SSID();

    static uint8_t day = 0;
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);

    if (day != timeinfo.tm_mday) {
        day = timeinfo.tm_mday;
        char timeBuffer[80];
        strftime(timeBuffer, sizeof(timeBuffer), "%d-%m-%Y", &timeinfo);
        setVarDB("ap_date", timeBuffer);
    }
    setVarDB("ap_ip", WiFi.localIP().toString());
    setVarDB("ap_ch", String(apInfo.channel));

    // reboot once at night
    if (timeinfo.tm_hour == 4 && timeinfo.tm_min == 0 && millis() > 2 * 3600 * 1000) {
        logLine("Nightly reboot");
        wsErr("REBOOTING");
        delay(100);
        ws.enable(false);
        refreshAllPending();
        saveDB("/current/tagDB.json");
        ws.closeAll();
        delay(100);
        ESP.restart();
    }

    static uint32_t tagcounttimer = 0;
    if (millis() - tagcounttimer > 60000 || tagcounttimer == 0) {
        uint32_t timeoutcount = 0;
        uint32_t tagcount = getTagCount(timeoutcount);
        char result[40];
        if (timeoutcount > 0) {
#ifdef HAS_RGB_LED
            if (apInfo.state == AP_STATE_ONLINE && apInfo.isOnline == true) rgbIdleColor = CRGB::DarkBlue;
#endif
            snprintf(result, sizeof(result), "%lu / %lu, %lu timed out", tagcount, tagDB.size(), timeoutcount);
        } else {
#ifdef HAS_RGB_LED
            if (apInfo.state == AP_STATE_ONLINE && apInfo.isOnline == true) rgbIdleColor = CRGB::Green;
#endif
            snprintf(result, sizeof(result), "%lu / %lu", tagcount, tagDB.size());
        }
        setVarDB("ap_tagcount", result);
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
                    taginfoitem.pending = taginfo->pending;
                    taginfoitem.expectedNextCheckin = taginfo->expectedNextCheckin;
                    taginfoitem.hwType = taginfo->hwType;
                    taginfoitem.wakeupReason = taginfo->wakeupReason;
                    taginfoitem.capabilities = taginfo->capabilities;
                    taginfoitem.pendingIdle = taginfo->pendingIdle;
                }
                udpsync.netTaginfo(&taginfoitem);
            }
        }
    }
}

void wsSendAPitem(struct APlist *apitem) {
    DynamicJsonDocument doc(250);
    JsonObject ap = doc.createNestedObject("apitem");

    char version_str[6];
    sprintf(version_str, "%04X", apitem->version);

    ap["ip"] = ((IPAddress)apitem->src).toString();
    ap["alias"] = apitem->alias;
    ap["count"] = apitem->tagCount;
    ap["channel"] = apitem->channelId;
    ap["version"] = version_str;

    if (wsMutex) xSemaphoreTake(wsMutex, portMAX_DELAY);
    ws.textAll(doc.as<String>());
    if (wsMutex) xSemaphoreGive(wsMutex);
}

void wsSerial(const String &text) {
    StaticJsonDocument<250> doc;
    doc["console"] = text;
    Serial.println(text);
    if (wsMutex) xSemaphoreTake(wsMutex, portMAX_DELAY);
    ws.textAll(doc.as<String>());
    if (wsMutex) xSemaphoreGive(wsMutex);
}

uint8_t wsClientCount() {
    return ws.count();
}

void init_web() {
    wsMutex = xSemaphoreCreateMutex();
    Storage.begin();
    WiFi.mode(WIFI_STA);
    WiFi.setTxPower(static_cast<wifi_power_t>(config.wifiPower));

    wm.connectToWifi();

    server.addHandler(new SPIFFSEditor(*contentFS));

    server.addHandler(&ws);

    server.on("/reboot", HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "OK Reboot");
        logLine("Reboot request by user");
        wsErr("REBOOTING");
        delay(100);
        ws.enable(false);
        refreshAllPending();
        saveDB("/current/tagDB.json");
        ws.closeAll();
        delay(100);
        ESP.restart();
    });

    server.serveStatic("/current", *contentFS, "/current/").setCacheControl("max-age=604800");
    server.serveStatic("/tagtypes", *contentFS, "/tagtypes/").setCacheControl("max-age=604800");
    server.serveStatic("/", *contentFS, "/www/").setDefaultFile("index.html");

    server.on(
        "/imgupload", HTTP_POST, [](AsyncWebServerRequest *request) {
            request->send(200);
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
                const tagRecord *taginfo = tagRecord::findByMAC(mac);
                if (taginfo != nullptr) {
                    if (taginfo->pending == true) {
                        request->send_P(200, "application/octet-stream", taginfo->data, taginfo->len);
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
                    taginfo->alias = request->getParam("alias", true)->value();
                    taginfo->modeConfigJson = request->getParam("modecfgjson", true)->value();
                    taginfo->contentMode = atoi(request->getParam("contentmode", true)->value().c_str());
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
                    // memset(taginfo->md5, 0, 16 * sizeof(uint8_t));
                    // memset(taginfo->md5pending, 0, 16 * sizeof(uint8_t));
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
                    if (strcmp(cmdValue, "clear") == 0) {
                        clearPending(taginfo);
                        memcpy(taginfo->md5pending, taginfo->md5, sizeof(taginfo->md5pending));
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
        //  http GET to /led_flash?mac=000000000000&pattern=1,3/0x1C,4,5/0xE0,3,1/0x4F,5,10/5/0,0,0/0
        //  http://192.168.178.198/led_flash?mac=00007E1F250CB29C&pattern=1,1/0x1C,1,15/0xE0,1,15/0x4F,1,15/1/0,0,0/0
        //  (mode,flashDuration/color1,flashCount1,flashSpeed1/color2,flashCount2,flashSpeed2/color3,flashCount3,flashSpeed3/repeats/delay1,delay2,delay3/spare)
        if (request->hasParam("mac")) {
            String dst = request->getParam("mac")->value();
            uint8_t mac[8];
            if (hex2mac(dst, mac)) {
                tagRecord *taginfo = tagRecord::findByMAC(mac);
                if (taginfo != nullptr) {
                    if (request->hasParam("pattern")) {
                        String pattern = request->getParam("pattern")->value();
                        struct ledFlash flashData;

                        int values[18];
                        int numValues = sscanf(
                            pattern.c_str(),
                            "%i,%i/%i,%i,%i/%i,%i,%i/%i,%i,%i/%i/%i,%i,%i/%i",
                            &values[16], &values[1], &values[2], &values[3], &values[4], &values[5],&values[6], &values[7], &values[8], &values[9], &values[10], &values[11], &values[12], &values[13], &values[14], &values[15]);

                        if (numValues != 16) {
                            request->send(400, "text/plain", "Error: wrong number of inputs in pattern");
                            return;
                        } else {
                            flashData.mode = values[16] & 0x0F;
                            flashData.flashDuration = values[1] & 0x0F;
                            flashData.color1 = values[2];
                            flashData.flashCount1 = values[3] & 0x0F;
                            flashData.flashSpeed1 = values[4] & 0x0F;
                            flashData.color2 = values[5];
                            flashData.flashCount2 = values[6] & 0x0F;
                            flashData.flashSpeed2 = values[7] & 0x0F;
                            flashData.color3 = values[8];
                            flashData.flashCount3 = values[9] & 0x0F;
                            flashData.flashSpeed3 = values[10] & 0x0F;
                            flashData.repeats = values[11];
                            flashData.delay1 = values[12];
                            flashData.delay2 = values[13];
                            flashData.delay3 = values[14];
                            flashData.spare = values[15];


                            const uint8_t *payload = reinterpret_cast<const uint8_t *>(&flashData);
                            sendTagCommand(mac, CMD_DO_LEDFLASH, !taginfo->isExternal, payload);
                            request->send(200, "text/plain", "ok, request transmitted");
                            return;
                        }
                    }
                }
            }
        }
        request->send(400, "text/plain", "parameters are missing");
    });

    server.on("/get_ap_config", HTTP_GET, [](AsyncWebServerRequest *request) {
        UDPcomm udpsync;
        udpsync.getAPList();
        File configFile = contentFS->open("/current/apconfig.json", "r");
        if (!configFile) {
            request->send(500, "text/plain", "Error opening apconfig.json file");
            return;
        }
        request->send(configFile, "application/json");
        configFile.close();
    });

    server.on("/save_apcfg", HTTP_POST, [](AsyncWebServerRequest *request) {
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
        if (request->hasParam("led", true)) {
            config.led = static_cast<int16_t>(request->getParam("led", true)->value().toInt());
            updateBrightnessFromConfig();
        }
        if (request->hasParam("tft", true)) {
            config.tft = static_cast<int16_t>(request->getParam("tft", true)->value().toInt());
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
        if (request->hasParam("lock", true)) {
            config.lock = static_cast<uint8_t>(request->getParam("lock", true)->value().toInt());
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
        if (request->hasParam("repo", true)) {
            config.repo = request->getParam("repo", true)->value();
        }
        if (request->hasParam("env", true)) {
            config.env = request->getParam("env", true)->value();
        }
        saveAPconfig();
        setAPchannel();
        request->send(200, "text/plain", "Ok, saved");
    });

    server.on("/set_var", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("key", true) && request->hasParam("val", true)) {
            std::string key = request->getParam("key", true)->value().c_str();
            String val = request->getParam("val", true)->value();
            Serial.printf("set key %s value %s\n", key.c_str(), val);
            setVarDB(key, val);
            request->send(200, "text/plain", "Ok, saved");
        } else {
            request->send(500, "text/plain", "param error");
        }
    });
    server.on("/set_vars", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("json", true)) {
            DynamicJsonDocument jsonDocument(2048);
            DeserializationError error = deserializeJson(jsonDocument, request->getParam("json", true)->value());
            if (error) {
                request->send(400, "text/plain", "Failed to parse JSON");
                return;
            }
            for (JsonPair kv : jsonDocument.as<JsonObject>()) {
                std::string key = kv.key().c_str();
                String val = kv.value().as<String>();
                Serial.printf("set key %s value %s\n", key.c_str(), val);
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
        StaticJsonDocument<250> doc;
        preferences.begin("wifi", false);
        const char *keys[] = {"ssid", "pw", "ip", "mask", "gw", "dns"};
        const size_t numKeys = sizeof(keys) / sizeof(keys[0]);
        for (size_t i = 0; i < numKeys; i++) {
            doc[keys[i]] = preferences.getString(keys[i], "");
        }
        serializeJson(doc, *response);
        request->send(response);
    });

    server.on("/get_ssid_list", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        DynamicJsonDocument doc(5000);

        doc["scanstatus"] = WiFi.scanComplete();
        JsonArray networks = doc.createNestedArray("networks");
        for (int i = 0; i < (WiFi.scanComplete() > 50 ? 50 : WiFi.scanComplete()); ++i) {
            if (WiFi.SSID(i) != "") {
                JsonObject network = networks.createNestedObject();
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
        Preferences preferences;
        preferences.begin("wifi", false);
        const char *keys[] = {"ssid", "pw", "ip", "mask", "gw", "dns"};
        const size_t numKeys = sizeof(keys) / sizeof(keys[0]);
        for (size_t i = 0; i < numKeys; i++) {
            String key = keys[i];
            if (jsonObj.containsKey(key)) {
                preferences.putString(key.c_str(), jsonObj[key].as<String>());
            }
        }
        preferences.end();
        Serial.println("config saved");
        request->send(200, "text/plain", "Ok, saved");

        ws.enable(false);

        if (jsonObj["ssid"].as<String>() == "factory") {
            preferences.begin("wifi", false);
            preferences.putString("ssid", "");
            preferences.putString("pw", "");
            preferences.end();
            contentFS->remove("/AP_FW_Pack.bin");
            contentFS->remove("/OpenEPaperLink_esp32_C6.bin");
            contentFS->remove("/bootloader.bin");
            contentFS->remove("/partition-table.bin");
            contentFS->remove("/update_actions.json");
            contentFS->remove("/log.txt");
            contentFS->remove("/current/tagDB.json");
            delay(100);
            ESP.restart();
        } else {
            refreshAllPending();
            saveDB("/current/tagDB.json");
        }

        ws.closeAll();
        delay(100);
        ESP.restart();
    });
    server.addHandler(handler);

    // end of setup

    server.on("/backup_db", HTTP_GET, [](AsyncWebServerRequest *request) {
        saveDB("/current/tagDB.json");
        request->send(*contentFS, "/current/tagDB.json", String(), true);
    });
    server.on(
        "/restore_db", HTTP_POST, [](AsyncWebServerRequest *request) {
            request->send(200);
        },
        dotagDBUpload);

    server.on("/sysinfo", HTTP_GET, handleSysinfoRequest);
    server.on("/check_file", HTTP_GET, handleCheckFile);
    server.on("/rollback", HTTP_POST, handleRollback);
    server.on("/update_c6", HTTP_POST, handleUpdateC6);
    server.on("/update_actions", HTTP_POST, handleUpdateActions);
    server.on("/update_ota", HTTP_POST, [](AsyncWebServerRequest *request) {
        handleUpdateOTA(request);
    });
    server.on(
        "/littlefs_put", HTTP_POST, [](AsyncWebServerRequest *request) {
            request->send(200);
        },
        handleLittleFSUpload);

    server.onNotFound([](AsyncWebServerRequest *request) {
        if (request->url() == "/" || request->url() == "index.htm") {
            request->send(200, "text/html", "index.html not found. Did you forget to upload the littlefs partition?");
            return;
        }
        request->send(404);
    });

    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "content-type");

    server.begin();
}

void doImageUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    static bool imageUploadBusy = false;
    if (!index) {
        if (config.runStatus != RUNSTATUS_RUN || imageUploadBusy) {
            request->send(409, "text/plain", "Come back later");
            return;
        }
        if (request->hasParam("mac", true)) {
            filename = request->getParam("mac", true)->value() + ".jpg";
        } else {
            filename = "unknown.jpg";
        }
        imageUploadBusy = true;
        logLine("http imageUpload " + filename);
        xSemaphoreTake(fsMutex, portMAX_DELAY);
        request->_tempFile = contentFS->open("/" + filename, "w");
    }
    if (len) {
        request->_tempFile.write(data, len);
    }
    if (final) {
        request->_tempFile.close();
        xSemaphoreGive(fsMutex);
        if (request->hasParam("mac", true)) {
            String dst = request->getParam("mac", true)->value();
            uint8_t mac[8];
            if (hex2mac(dst, mac)) {
                tagRecord *taginfo = tagRecord::findByMAC(mac);
                if (taginfo != nullptr) {
                    bool dither = true;
                    if (request->hasParam("dither", true)) {
                        if (request->getParam("dither", true)->value() == "0") dither = false;
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
                    taginfo->modeConfigJson = "{\"filename\":\"" + dst + ".jpg\",\"timetolive\":\"" + String(ttl) + "\",\"dither\":\"" + String(dither) + "\",\"delete\":\"1\", \"preload\":\"" + String(preload) + "\", \"preload_lut\":\"" + String(preloadlut) + "\", \"preload_type\":\"" + String(preloadtype) + "\"}";
                    taginfo->contentMode = 0;
                    taginfo->nextupdate = 0;
                    wsSendTaginfo(mac, SYNC_USERCFG);
                    request->send(200, "text/plain", "Ok, saved");
                } else {
                    request->send(400, "text/plain", "mac not found");
                }
            }
        } else {
            request->send(400, "text/plain", "parameters incomplete");
        }
        imageUploadBusy = false;
    }
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
            File file = LittleFS.open("/current/" + dst + ".json", "w");
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
    if (!index) {
        logLine("restore tagDB");
        xSemaphoreTake(fsMutex, portMAX_DELAY);
        request->_tempFile = contentFS->open("/current/tagDBrestored.json", "w");
    }
    if (len) {
        request->_tempFile.write(data, len);
    }
    if (final) {
        request->_tempFile.close();
        xSemaphoreGive(fsMutex);
        destroyDB();
        loadDB("/current/tagDBrestored.json");
        request->send(200, "text/plain", "Ok, restored.");
    }
}