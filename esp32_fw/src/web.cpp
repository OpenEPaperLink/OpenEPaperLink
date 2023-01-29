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
#include "newproto.h"
#include "settings.h"

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
    uint32_t ulNotificationValue;
    Serial.print("websocket thread started\n");
    websocketUpdater = xTaskGetCurrentTaskHandle();
    wsMutex = xSemaphoreCreateMutex();
    while (true) {
        ulNotificationValue = ulTaskNotifyTake(pdTRUE, 1000 / portTICK_RATE_MS);
        if (ulNotificationValue == 0) {  // timeout, so every 1s
            ws.cleanupClients();
        } else {
            // if (ws.count())
            //  sendStatus(STATUS_WIFI_ACTIVITY);
            DynamicJsonDocument doc(1500);
            if (ulNotificationValue & 2) {  // WS_SEND_MODE_STATUS) {
                /*  doc["rxActive"] = status.rxActive;
                  doc["txActive"] = status.txActive;
                  doc["freq"] = status.freq;
                  doc["txMode"] = status.currentmode;
                  */
            }
            /*
                JsonArray statusframes = doc.createNestedArray("frames");
                for (uint8_t c = 0; c < STATUSFRAMELISTSIZE; c++) {
                    if (statusframearr[c]) {
                        JsonObject statusframe = statusframes.createNestedObject();
                        statusframe["frame"] = statusframearr[c]->frameno;
                        statusframe["isTX"] = statusframearr[c]->isTX;
                        statusframe["freq"] = statusframearr[c]->freq;
                        statusframe["txSkipped"] = statusframearr[c]->txCancelled;
                        switch (statusframearr[c]->rxtype) {
                            case flexsynctype::SYNC_FLEX_1600:
                                statusframe["rxType"] = "FLEX_1600";
                                break;
                            case flexsynctype::SYNC_FLEX_3200_2:
                                statusframe["rxType"] = "FLEX_3200_2";
                                break;
                            case flexsynctype::SYNC_FLEX_3200_4:
                                statusframe["rxType"] = "FLEX_3200_4";
                                break;
                            case flexsynctype::SYNC_FLEX_6400:
                                statusframe["rxType"] = "FLEX_3200_4";
                                break;
                            default:
                                break;
                        }
                        switch (statusframearr[c]->txformat) {
                            case txframe::FORMAT_FLEX:
                                statusframe["txType"] = "FLEX";
                                break;
                            case txframe::FORMAT_POCSAG:
                                statusframe["txType"] = "POCSAG";
                                break;
                            case txframe::FORMAT_IDLE:
                                statusframe["txType"] = "IDLE";
                                break;
                            case txframe::FORMAT_BLOCKED:
                                statusframe["txType"] = "BLOCKED";
                                break;
                            default:
                                break;
                        }
                    }
                }
            }*/
            size_t len = measureJson(doc);
            xSemaphoreTake(wsMutex, portMAX_DELAY);
            auto buffer = std::make_shared<std::vector<uint8_t>>(len);
            serializeJson(doc, buffer->data(), len);
            // ws.textAll((char*)buffer->data());
            //ws.textAll("ohai");
            xSemaphoreGive(wsMutex);
        }
    }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
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

void wsString(String text) {
    DynamicJsonDocument doc(1500);
    doc["logMsg"] = text;
    xSemaphoreTake(wsMutex, portMAX_DELAY);
    ws.textAll(doc.as<String>());
    xSemaphoreGive(wsMutex);
}

void wsSendTaginfo(uint8_t src[8]) {
    DynamicJsonDocument doc(1500);
    JsonArray tags = doc.createNestedArray("tags");
    JsonObject tag = tags.createNestedObject();
    char buffer[64];
    sprintf(buffer, "%02X%02X%02X%02X%02X%02X\0", src[2], src[3], src[4], src[5], src[6], src[7]);
    tag["mac"] = (String)buffer;
    //tag["buttonstate"] = eadr->adr.buttonState;
    xSemaphoreTake(wsMutex, portMAX_DELAY);
    ws.textAll(doc.as<String>());
    xSemaphoreGive(wsMutex);
}

void init_web() {
    LittleFS.begin(true);

    if (!LittleFS.exists("/.exclude.files")) {
		Serial.println("littlefs exclude.files aanmaken");
        File f = LittleFS.open("/.exclude.files", "w");
        f.close();
    }
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

    server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", String(ESP.getFreeHeap()));
    });

    server.on("/reboot", HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "OK Reboot");
        ESP.restart();
    });

    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.htm");

    server.on(
        "/imgupload", HTTP_POST, [](AsyncWebServerRequest *request) {
            request->send(200);
        },
        doImageUpload);

    server.on("/send_image", HTTP_POST, [](AsyncWebServerRequest *request) {
        String filename;
        String dst;
        uint16_t nextCheckin;
        if (request->hasParam("filename", true) && request->hasParam("dst", true)) {
            filename = request->getParam("filename", true)->value();
            dst = request->getParam("dst", true)->value();
            nextCheckin = request->getParam("ttl",true)->value().toInt();
            uint8_t mac_addr[12];  // I expected this to return like 8 values, but if I make the array 8 bytes long, things die.
            mac_addr[0] = 0x00;
            mac_addr[1] = 0x00;
            if (sscanf(dst.c_str(), "%02X%02X%02X%02X%02X%02X",
                       &mac_addr[2],
                       &mac_addr[3],
                       &mac_addr[4],
                       &mac_addr[5],
                       &mac_addr[6],
                       &mac_addr[7]) != 6) {
                request->send(200, "text/plain", "Something went wrong trying to parse the mac address");
            } else {
                *((uint64_t *)mac_addr) = swap64(*((uint64_t *)mac_addr));
                if (prepareDataAvail(&filename, DATATYPE_IMGRAW, mac_addr, nextCheckin)) {
                    request->send(200, "text/plain", "Sending to " + dst);
                } else {
                    request->send(200, "text/plain", "Couldn't find filename :(");
                }
            }
            return;
        }
        request->send(200, "text/plain", "Didn't get the required filename + dst");
        return;
    });

    server.on("/send_fw", HTTP_POST, [](AsyncWebServerRequest *request) {
        String filename;
        String dst;
        if (request->hasParam("filename", true) && request->hasParam("dst", true)) {
            filename = request->getParam("filename", true)->value();
            dst = request->getParam("dst", true)->value();
            uint8_t mac_addr[12];  // I expected this to return like 8 values, but if I make the array 8 bytes long, things die.
            mac_addr[0] = 0x00;
            mac_addr[1] = 0x00;
            if (sscanf(dst.c_str(), "%02X%02X%02X%02X%02X%02X",
                       &mac_addr[2],
                       &mac_addr[3],
                       &mac_addr[4],
                       &mac_addr[5],
                       &mac_addr[6],
                       &mac_addr[7]) != 6) {
                request->send(200, "text/plain", "Something went wrong trying to parse the mac address");
            } else {
                *((uint64_t *)mac_addr) = swap64(*((uint64_t *)mac_addr));
                if (prepareDataAvail(&filename, DATATYPE_UPDATE, mac_addr, 0)) {
                    request->send(200, "text/plain", "Sending FW to " + dst);
                } else {
                    request->send(200, "text/plain", "Couldn't find filename :(");
                }
            }
            return;
        }
        request->send(200, "text/plain", "Didn't get the required filename + dst");
        return;
    });

    server.on("/req_checkin", HTTP_POST, [](AsyncWebServerRequest *request) {
        String filename;
        String dst;
        if (request->hasParam("dst", true)) {
            dst = request->getParam("dst", true)->value();
            uint8_t mac_addr[12];  // I expected this to return like 8 values, but if I make the array 8 bytes long, things die.
            mac_addr[0] = 0x00;
            mac_addr[1] = 0x00;
            if (sscanf(dst.c_str(), "%02X%02X%02X%02X%02X%02X",
                       &mac_addr[2],
                       &mac_addr[3],
                       &mac_addr[4],
                       &mac_addr[5],
                       &mac_addr[6],
                       &mac_addr[7]) != 6) {
                request->send(200, "text/plain", "Something went wrong trying to parse the mac address");
            } else {
                *((uint64_t *)mac_addr) = swap64(*((uint64_t *)mac_addr));
                if (prepareDataAvail(&filename, DATATYPE_NOUPDATE, mac_addr,0)) {
                    request->send(200, "text/plain", "Sending check-in request to " + dst);
                }
            }
            return;
        }
        request->send(200, "text/plain", "Didn't get the required params");
        return;
    });

    server.onNotFound([](AsyncWebServerRequest *request) {
        if (request->url() == "/" || request->url() == "index.htm") {
            request->send(200, "text/html", "-");
            return;
        }
        Serial.printf("NOT_FOUND: ");

        switch (request->method()) {
            case HTTP_GET:
                Serial.printf("GET");
                break;
            case HTTP_POST:
                Serial.printf("POST");
                break;
            case HTTP_DELETE:
                Serial.printf("DELETE");
                break;
            case HTTP_PUT:
                Serial.printf("PUT");
                break;
            case HTTP_PATCH:
                Serial.printf("PATCH");
                break;
            case HTTP_HEAD:
                Serial.printf("HEAD");
                break;
            case HTTP_OPTIONS:
                Serial.printf("OPTIONS");
                break;

            default:
                Serial.printf("UNKNOWN");
                break;
        }
        Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

        if (request->contentLength()) {
            Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
            Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
        }
        for (int i = 0; i < request->headers(); i++) {
            AsyncWebHeader *h = request->getHeader(i);
            Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
        }
        for (int i = 0; i < request->params(); i++) {
            AsyncWebParameter *p = request->getParam(i);
            if (p->isFile()) {
                Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
            } else if (p->isPost()) {
                Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
            } else {
                Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
            }
        }
        request->send(404);
    });

    server.begin();
}

void doImageUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!index) {
        Serial.print((String) "UploadStart: " + filename);
        // open the file on first call and store the file handle in the request object
        request->_tempFile = LittleFS.open("/" + filename, "w");
    }
    if (len) {
        // stream the incoming chunk to the opened file
        request->_tempFile.write(data, len);
    }
    if (final) {
        Serial.print((String) "UploadEnd: " + filename + "," + index + len);
        // close the file handle as the upload is now done
        request->_tempFile.close();
        request->send(200, "text/plain", "File Uploaded !");
        /*
                sscanf() if (request->hasParam("id") && request->hasParam("file")) {
                    id = request->getParam("id")->value().toInt();
                    filename = request->getParam("file")->value();
                }
                */
    }
}