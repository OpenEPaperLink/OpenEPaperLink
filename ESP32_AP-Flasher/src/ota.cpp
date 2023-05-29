#include "ota.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <HTTPClient.h>
#include <LittleFS.h>
#include <MD5Builder.h>
#include <Update.h>

#include "tag_db.h"
#include "web.h"

#ifndef BUILD_ENV_NAME
#define BUILD_ENV_NAME unknown
#endif
#ifndef BUILD_TIME
#define BUILD_TIME 0
#endif
#ifndef BUILD_VERSION
#define BUILD_VERSION custom
#endif
#ifndef SHA
#define SHA 0
#endif

#define STR_IMPL(x) #x
#define STR(x) STR_IMPL(x)

void handleSysinfoRequest(AsyncWebServerRequest* request) {
    StaticJsonDocument<250> doc;
    doc["alias"] = config.alias;
    doc["env"] = STR(BUILD_ENV_NAME);
    doc["buildtime"] = STR(BUILD_TIME);
    doc["buildversion"] = STR(BUILD_VERSION);
    doc["sha"] = STR(SHA);
    doc["psramsize"] = ESP.getPsramSize();
    doc["flashsize"] = ESP.getFlashChipSize();
    doc["rollback"] = Update.canRollBack();

    size_t bufferSize = measureJson(doc) + 1;
    AsyncResponseStream *response = request->beginResponseStream("application/json", bufferSize);
    serializeJson(doc, *response);
    request->send(response);
};

void handleCheckFile(AsyncWebServerRequest *request) {
    if (!request->hasParam("path")) {
        request->send(400);
        return;
    }

    String filePath = request->getParam("path")->value();
    File file = LittleFS.open(filePath, "r");
    if (!file) {
        StaticJsonDocument<64> doc;
        doc["filesize"] = 0;
        doc["md5"] = "";
        String jsonResponse;
        serializeJson(doc, jsonResponse);
        request->send(200, "application/json", jsonResponse);
        return;
    }

    size_t fileSize = file.size();

    MD5Builder md5;
    md5.begin();
    md5.addStream(file, fileSize);
    md5.calculate();
    String md5Hash = md5.toString();

    file.close();

    StaticJsonDocument<128> doc;
    doc["filesize"] = fileSize;
    doc["md5"] = md5Hash;
    String jsonResponse;
    serializeJson(doc, jsonResponse);

    request->send(200, "application/json", jsonResponse);
}

void handleGetExtUrl(AsyncWebServerRequest* request) {
    if (request->hasParam("url")) {
        String url = request->getParam("url")->value();
        HTTPClient http;
        http.begin(url);
        http.setConnectTimeout(4000);
        http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
        int httpResponseCode = http.GET();
        if (httpResponseCode > 0) {
            Serial.println(httpResponseCode);
            String contentType = http.header("Content-Type");
            size_t contentLength = http.getSize();
            if (contentLength > 0) {
                String content = http.getString();
                AsyncWebServerResponse* response = request->beginResponse(200, contentType, content);
                request->send(response);
            } else {
                request->send(500, "text/plain", "no size header");
            }
        } else {
            request->send(httpResponseCode, "text/plain", "Failed to fetch URL");
        }
        http.end();
    } else {
        request->send(400, "text/plain", "Missing 'url' parameter");
    }
}

void handleLittleFSUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    bool error = false;
    if (!index) {
        String path;
        if (!request->hasParam("path", true)) {
            path = "/temp/null.bin";
            final = true;
            error = true;
        } else {
            path = request->getParam("path", true)->value();
            Serial.println("update " + path);
            request->_tempFile = LittleFS.open(path, "w", true);
        }
    }
    if (len) {
        if (!request->_tempFile.write(data, len)) {
            error = true;
            final = true;
        }
    }
    if (final) {
        request->_tempFile.close();
        if (error) {
            request->send(507, "text/plain", "Error. Disk full?");
        } else {
            request->send(200, "text/plain", "Ok, file written");
        }
    }
}

struct FirmwareUpdateParams {
    String url;
    String md5;
    size_t size;
};

void handleUpdateOTA(AsyncWebServerRequest* request) {
    if (request->hasParam("url", true) && request->hasParam("md5", true) && request->hasParam("size", true)) {
        saveDB("/current/tagDB.json");

        FirmwareUpdateParams* params = new FirmwareUpdateParams;
        params->url = request->getParam("url", true)->value();
        params->md5 = request->getParam("md5", true)->value();
        params->size = request->getParam("size", true)->value().toInt();

        xTaskCreatePinnedToCore(firmwareUpdateTask, "OTAUpdateTask", 8192, params, 1, NULL, 1);

        request->send(200, "text/plain", "In progress");
    } else {
        request->send(400, "Bad request");
    }
}

void firmwareUpdateTask(void* parameter) {
    FirmwareUpdateParams* params = reinterpret_cast<FirmwareUpdateParams*>(parameter);

    const char* url = params->url.c_str();
    const char* md5 = params->md5.c_str();
    size_t size = params->size;
    updateFirmware(url, md5, size);

    delete params;
    vTaskDelete(NULL);
}

void updateFirmware(const char* url, const char* expectedMd5, size_t size) {
    HTTPClient httpClient;

    wsSerial("start downloading");
    wsSerial(url);

    httpClient.begin(url);
    httpClient.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    int httpCode = httpClient.GET();

    if (httpCode == HTTP_CODE_OK) {
        if (Update.begin(size)) {
            Update.setMD5(expectedMd5);

            unsigned long progressTimer = millis();
            Update.onProgress([&progressTimer](size_t progress, size_t total) {
                if (millis() - progressTimer > 500 || progress == total) {
                    char buffer[50];
                    sprintf(buffer, "Progress: %u%% %d %d\r\n", progress * 100 / total, progress, total);
                    wsSerial(String(buffer));
                    progressTimer = millis();
                }
            });

            size_t written = Update.writeStream(httpClient.getStream());
            if (written == httpClient.getSize()) {
                if (Update.end(true)) {
                    wsSerial("Firmware update successful");
                    wsSerial("Reboot system now");
                    wsSerial("[reboot]");
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                    ESP.restart();
                } else {
                    wsSerial("Error updating firmware:");
                    wsSerial(Update.errorString());
                }
            } else {
                wsSerial("Error writing firmware data:");
                wsSerial(Update.errorString());
            }
        } else {
            wsSerial("Failed to begin firmware update");
            wsSerial(Update.errorString());
        }
    } else {
        wsSerial("Failed to download firmware file (HTTP code " + String(httpCode) + ")");
    }

    httpClient.end();
}

void handleRollback(AsyncWebServerRequest* request) {
    if (Update.canRollBack()) {
        bool rollbackSuccess = Update.rollBack();
        if (rollbackSuccess) {
            request->send(200, "Rollback successful");
            wsSerial("Rollback successful");
            wsSerial("Restarting system...");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            ESP.restart();
        } else {
            wsSerial("Rollback failed");
            request->send(400, "Rollback failed");
        }
    } else {
        wsSerial("Rollback not allowed");
        request->send(400, "Rollback not allowed");
    }
}