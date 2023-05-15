#include "web.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <FS.h>
#include <LittleFS.h>
#include <SPIFFSEditor.h>
#include <WiFi.h>
#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager/tree/feature_asyncwebserver

#include "commstructs.h"
#include "leds.h"
#include "newproto.h"
#include "settings.h"
#include "tag_db.h"
#include "udp.h"

extern uint8_t data_to_send[];

const char *http_username = "admin";
const char *http_password = "admin";
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

SemaphoreHandle_t wsMutex;
TaskHandle_t websocketUpdater;

uint64_t swap64(uint64_t x) {
    uint64_t byte1 = x & 0xff00000000000000;
    uint64_t byte2 = x & 0x00ff000000000000;
    uint64_t byte3 = x & 0x0000ff0000000000;
    uint64_t byte4 = x & 0x000000ff00000000;
    uint64_t byte5 = x & 0x00000000ff000000;
    uint64_t byte6 = x & 0x0000000000ff0000;
    uint64_t byte7 = x & 0x000000000000ff00;
    uint64_t byte8 = x & 0x00000000000000ff;

    return (uint64_t)(byte1 >> 56 | byte2 >> 40 | byte3 >> 24 | byte4 >> 8 |
                      byte5 << 8 | byte6 << 24 | byte7 << 40 | byte8 << 56);
}

void webSocketSendProcess(void *parameter) {
    websocketUpdater = xTaskGetCurrentTaskHandle();
    wsMutex = xSemaphoreCreateMutex();
    while (true) {
        ws.cleanupClients();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
#ifdef HAS_RGB_LED
    shortBlink(CRGB::BlueViolet);
#endif
    switch (type) {
        case WS_EVT_CONNECT:
            // client connected
            ets_printf("ws[%s][%u] connect\n", server->url(), client->id());
            xTaskNotify(websocketUpdater, 2, eSetBits);
            // client->ping();
            break;
        case WS_EVT_DISCONNECT:
            // client disconnected
            ets_printf("ws[%s][%u] disconnect: %u\n", server->url(), client->id());
            break;
        case WS_EVT_ERROR:
            // error was received from the other end
            ets_printf("WS Error received :(\n\n");
            // ets_printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t *)arg), (char *)data);
            break;
        case WS_EVT_PONG:
            // pong message was received (in response to a ping request maybe)
            ets_printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char *)data : "");
            break;
        case WS_EVT_DATA:
            // data packet
            AwsFrameInfo *info = (AwsFrameInfo *)arg;
            if (info->final && info->index == 0 && info->len == len) {
                // the whole message is in a single frame and we got all of it's data
                ets_printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT) ? "text" : "binary", info->len);
                if (info->opcode == WS_TEXT) {
                    data[len] = 0;
                    ets_printf("%s\n", (char *)data);
                } else {
                    for (size_t i = 0; i < info->len; i++) {
                        ets_printf("%02x ", data[i]);
                    }
                    ets_printf("\n");
                }
                if (info->opcode == WS_TEXT)
                    client->text("{\"status\":\"received\"}");
                else
                    client->binary("{\"status\":\"received\"}");
            } else {
                // message is comprised of multiple frames or the frame is split into multiple packets
                if (info->index == 0) {
                    if (info->num == 0)
                        ets_printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
                    ets_printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
                }

                ets_printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT) ? "text" : "binary", info->index, info->index + len);
                if (info->message_opcode == WS_TEXT) {
                    data[len] = 0;
                    ets_printf("%s\n", (char *)data);
                } else {
                    for (size_t i = 0; i < len; i++) {
                        ets_printf("%02x ", data[i]);
                    }
                    ets_printf("\n");
                }

                if ((info->index + len) == info->len) {
                    ets_printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
                    if (info->final) {
                        ets_printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
                        if (info->message_opcode == WS_TEXT)
                            client->text("{\"status\":\"received\"}");
                        else
                            client->binary("{\"status\":\"received\"}");
                    }
                }
            }
            break;
    }
}

void wsLog(String text) {
    StaticJsonDocument<500> doc;
    doc["logMsg"] = text;
    if (wsMutex) xSemaphoreTake(wsMutex, portMAX_DELAY);
    ws.textAll(doc.as<String>());
    if (wsMutex) xSemaphoreGive(wsMutex);
}

void wsErr(String text) {
    StaticJsonDocument<500> doc;
    doc["errMsg"] = text;
    if (wsMutex) xSemaphoreTake(wsMutex, portMAX_DELAY);
    ws.textAll(doc.as<String>());
    if (wsMutex) xSemaphoreGive(wsMutex);
}

void wsSendSysteminfo() {
    DynamicJsonDocument doc(250);
    JsonObject sys = doc.createNestedObject("sys");
    time_t now;
    time(&now);
    sys["currtime"] = now;
    sys["heap"] = ESP.getFreeHeap();
    sys["recordcount"] = tagDB.size();
    sys["dbsize"] = tagDB.size() * sizeof(tagRecord);
    sys["littlefsfree"] = LittleFS.totalBytes() - LittleFS.usedBytes();

    xSemaphoreTake(wsMutex, portMAX_DELAY);
    ws.textAll(doc.as<String>());
    xSemaphoreGive(wsMutex);
}

void wsSendTaginfo(uint8_t mac[6]) {
    String json = "";
    json = tagDBtoJson(mac);

    xSemaphoreTake(wsMutex, portMAX_DELAY);
    ws.textAll(json);
    xSemaphoreGive(wsMutex);
}

void wsSendAPitem(struct APlist* apitem) {
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

void init_web() {
    LittleFS.begin(true);

    if (!LittleFS.exists("/current")) {
        LittleFS.mkdir("/current");
    }
    if (!LittleFS.exists("/temp")) {
        LittleFS.mkdir("/temp");
    }

    WiFi.mode(WIFI_STA);
    WiFiManager wm;
    bool res;
    res = wm.autoConnect("AutoConnectAP");
    if (!res) {
        Serial.println("Failed to connect");
        ESP.restart();
    }
    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());

    server.addHandler(new SPIFFSEditor(LittleFS, http_username, http_password));

    ws.onEvent(onEvent);
    server.addHandler(&ws);

    server.on("/reboot", HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "OK Reboot");
        wsErr("REBOOTING");
        ws.enable(false);
        refreshAllPending();
        saveDB("/current/tagDB.json");
        ws.closeAll();
        delay(100);
        ESP.restart();
    });

    server.serveStatic("/current", LittleFS, "/current/");
    server.serveStatic("/", LittleFS, "/www/").setDefaultFile("index.html");

    server.on(
        "/imgupload", HTTP_POST, [](AsyncWebServerRequest *request) {
            request->send(200);
        },
        doImageUpload);

    server.on("/get_db", HTTP_GET, [](AsyncWebServerRequest *request) {
        String json = "";
        if (request->hasParam("mac")) {
            String dst = request->getParam("mac")->value();
            uint8_t mac[6];
            if (sscanf(dst.c_str(), "%02X%02X%02X%02X%02X%02X", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]) == 6) {
                json = tagDBtoJson(mac);
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

    server.on("/save_cfg", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("mac", true)) {
            String dst = request->getParam("mac", true)->value();
            uint8_t mac[6];
            if (sscanf(dst.c_str(), "%02X%02X%02X%02X%02X%02X", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]) == 6) {
                tagRecord *taginfo = nullptr;
                taginfo = tagRecord::findByMAC(mac);
                if (taginfo != nullptr) {
                    taginfo->alias = request->getParam("alias", true)->value();
                    taginfo->modeConfigJson = request->getParam("modecfgjson", true)->value();
                    taginfo->contentMode = atoi(request->getParam("contentmode", true)->value().c_str());
                    taginfo->nextupdate = 0;
                    // memset(taginfo->md5, 0, 16 * sizeof(uint8_t));
                    // memset(taginfo->md5pending, 0, 16 * sizeof(uint8_t));
                    wsSendTaginfo(mac);
                    saveDB("/current/tagDB.json");
                    request->send(200, "text/plain", "Ok, saved");
                } else {
                    request->send(200, "text/plain", "Error while saving: mac not found");
                }
            }
        }
        request->send(200, "text/plain", "Ok, saved");
    });

    server.on("/delete_cfg", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("mac", true)) {
            String dst = request->getParam("mac", true)->value();
            uint8_t mac[6];
            if (sscanf(dst.c_str(), "%02X%02X%02X%02X%02X%02X", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]) == 6) {
                if (deleteRecord(mac)) {
                    request->send(200, "text/plain", "Ok, deleted");
                } else {
                    request->send(200, "text/plain", "Error while saving: mac not found");
                }
            }
        } else {
            request->send(500, "text/plain", "no mac");
        }
    });

    server.on("/get_ap_list", HTTP_GET, [](AsyncWebServerRequest *request) {
        UDPcomm udpsync;
        udpsync.getAPList();
        File configFile = LittleFS.open("/current/apconfig.json", "r");
        if (!configFile) {
            request->send(500, "text/plain", "Error opening apconfig.json file");
            return;
        }
        request->send(configFile, "application/json");
        configFile.close();
    });

    server.on("/save_apcfg", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("alias", true) && request->hasParam("channel", true)) {
            APconfig["alias"] = request->getParam("alias", true)->value();
            APconfig["channel"] = request->getParam("channel", true)->value();
            APconfig["ledbrightness"] = request->getParam("ledbrightness", true)->value();
            updateBrightnessFromConfig();
            saveAPconfig();
            setAPchannel();
        }
        request->send(200, "text/plain", "Ok, saved");
    });

    server.on("/backup_db", HTTP_GET, [](AsyncWebServerRequest *request) {
        saveDB("/current/tagDB.json");
        File file = LittleFS.open("/current/tagDB.json", "r");
        AsyncWebServerResponse *response = request->beginResponse(file, "tagDB.json", String(), true);
        request->send(response);
        file.close();
    });

    server.onNotFound([](AsyncWebServerRequest *request) {
        if (request->url() == "/" || request->url() == "index.htm") {
            request->send(200, "text/html", "-");
            return;
        }
        request->send(404);
    });

    server.begin();
}

void doImageUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!index) {
        if (request->hasParam("mac", true)) {
            filename = request->getParam("mac", true)->value() + ".jpg";
        } else {
            filename = "unknown.jpg";
        }
        request->_tempFile = LittleFS.open("/" + filename, "w");
    }
    if (len) {
        // stream the incoming chunk to the opened file
        request->_tempFile.write(data, len);
    }
    if (final) {
        request->_tempFile.close();
        if (request->hasParam("mac", true)) {
            String dst = request->getParam("mac", true)->value();
            uint8_t mac[6];
            if (sscanf(dst.c_str(), "%02X%02X%02X%02X%02X%02X", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]) == 6) {
                tagRecord *taginfo = nullptr;
                taginfo = tagRecord::findByMAC(mac);
                if (taginfo != nullptr) {
                    taginfo->modeConfigJson = "{\"filename\":\"" + dst + ".jpg\",\"timetolive\":\"0\"}";
                    taginfo->contentMode = 0;
                    taginfo->nextupdate = 0;
                    wsSendTaginfo(mac);
                    request->send(200, "text/plain", "Ok, saved");
                } else {
                    request->send(200, "text/plain", "Error while saving: mac not found");
                }
            }
        } else {
            request->send(500, "text/plain", "parameters incomplete");
        }
    }
}