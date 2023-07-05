#include "ota.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <HTTPClient.h>
#include "storage.h"
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
    AsyncResponseStream* response = request->beginResponseStream("application/json", bufferSize);
    serializeJson(doc, *response);
    request->send(response);
};

void handleCheckFile(AsyncWebServerRequest* request) {
    if (!request->hasParam("path")) {
        request->send(400);
        return;
    }

    String filePath = request->getParam("path")->value();
    File file = contentFS->open(filePath, "r");
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
        http.setConnectTimeout(5000);
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
            wsLog(http.errorToString(httpResponseCode));
        }
        http.end();
    } else {
        request->send(400, "text/plain", "Missing 'url' parameter");
    }
}

void handleLittleFSUpload(AsyncWebServerRequest* request, String filename, size_t index, uint8_t* data, size_t len, bool final) {
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
            request->_tempFile = contentFS->open(path, "w", true);
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
        FirmwareUpdateParams* params = new FirmwareUpdateParams;
        params->url = request->getParam("url", true)->value();
        params->md5 = request->getParam("md5", true)->value();
        params->size = request->getParam("size", true)->value().toInt();
        xTaskCreate(firmwareUpdateTask, "OTAUpdateTask", 6144, params, 10, NULL);

        request->send(200, "text/plain", "In progress");
    } else {
        request->send(400, "Bad request");
    }
}

void firmwareUpdateTask(void* parameter) {
    FirmwareUpdateParams* params = reinterpret_cast<FirmwareUpdateParams*>(parameter);

    if (ESP.getMaxAllocHeap() < 22000) {
        wsLog("Error: Not enough memory left. Restart the esp32 and try updating again.");
        wsLog("[reboot]");
    } else {
        const char* url = params->url.c_str();
        const char* md5 = params->md5.c_str();
        size_t size = params->size;
        updateFirmwareFromURL(url, md5, size);
    }

    delete params;
    vTaskDelete(NULL);
}

bool executeUpdate(const char* expectedMd5, size_t size, Stream &stream, size_t stream_size) {
    if (!Update.begin(size)) {
        wsLog("Failed to begin firmware update");
        wsLog(Update.errorString());
        return false;
    }

    if (expectedMd5) {
        Serial.println("Setting update md5: " + String(expectedMd5));
        if (!Update.setMD5(expectedMd5)) {
            Serial.println("Failed to set firmware md5");
            wsErr("Failed to set firmware md5");
            return false;
        }
    }

    unsigned long progressTimer = millis();
    Update.onProgress([&progressTimer](size_t progress, size_t total) {
        if (millis() - progressTimer > 500 || progress == total) {
            char buffer[50];
            sprintf(buffer, "Progress: %u%% %d %d", progress * 100 / total, progress, total);
            wsLog(String(buffer));
            Serial.println(String(buffer));
            progressTimer = millis();
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
    });

    size_t written = Update.writeStream(stream);
    if (written != stream_size) {
        Serial.println("Error writing firmware:");
        wsLog("Error writing firmware:");
        Serial.println(Update.errorString());
        wsLog(Update.errorString());
        return false;
    }

    if (!Update.end(true)) {
        Serial.println("Error updating firmware:");
        wsLog("Error updating firmware:");
        Serial.println(Update.errorString());
        wsLog(Update.errorString());
        Update.abort();
        return false;
    }

    wsLog("Firmware update successful");
    wsLog("Reboot system now");
    wsLog("[reboot]");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    return true;
}


void updateFirmwareFromURL(const char* url, const char* expectedMd5, size_t size) {
    uint32_t freeStack = uxTaskGetStackHighWaterMark(NULL);
    Serial.printf("Free heap: %d allocatable: %d stack: %d\n", ESP.getFreeHeap(), ESP.getMaxAllocHeap(), freeStack);

    config.runStatus = RUNSTATUS_STOP;
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    // xSemaphoreTake(tagDBOwner, portMAX_DELAY);
    saveDB("/current/tagDB.json");
    // destroyDB();

    HTTPClient httpClient;

    wsLog("start downloading");
    wsLog(url);

    httpClient.begin(url);
    httpClient.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    freeStack = uxTaskGetStackHighWaterMark(NULL);
    Serial.printf("Free heap: %d allocatable: %d stack: %d\n", ESP.getFreeHeap(), ESP.getMaxAllocHeap(), freeStack);
    int httpCode = httpClient.GET();
    freeStack = uxTaskGetStackHighWaterMark(NULL);
    Serial.printf("Free heap: %d allocatable: %d stack: %d\n", ESP.getFreeHeap(), ESP.getMaxAllocHeap(), freeStack);

    if (httpCode == HTTP_CODE_OK) {
        executeUpdate(expectedMd5, size, httpClient.getStream(), httpClient.getSize());
    } else {
        wsLog("Failed to download firmware file (HTTP code " + String(httpCode) + ")");
        wsLog(httpClient.errorToString(httpCode));
    }

    httpClient.end();
    // loadDB("/current/tagDB.json");
    config.runStatus = RUNSTATUS_RUN;
    // xSemaphoreGive(tagDBOwner);
}

void handleRollback(AsyncWebServerRequest* request) {
    if (Update.canRollBack()) {
        bool rollbackSuccess = Update.rollBack();
        if (rollbackSuccess) {
            request->send(200, "Rollback successful");
            wsLog("Rollback successful");
            wsLog("Reboot system now");
            wsLog("[reboot]");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            // ESP.restart();
        } else {
            wsLog("Rollback failed");
            request->send(400, "Rollback failed");
        }
    } else {
        wsLog("Rollback not allowed");
        request->send(400, "Rollback not allowed");
    }
}

void handleUpdateActions(AsyncWebServerRequest* request) {
    wsLog("Performing cleanup");
    File file = contentFS->open("/update_actions.json", "r");
    if (!file) {
        wsLog("No update_actions.json present");
        request->send(200, "No update actions needed");
        return;
    }
    StaticJsonDocument<1000> doc;
    DeserializationError error = deserializeJson(doc, file);
    JsonArray deleteFiles = doc["deletefile"].as<JsonArray>();
    for (const auto& filePath : deleteFiles) {
        if (contentFS->remove(filePath.as<const char*>())) {
            wsLog("deleted file: " + filePath.as<String>());
        }
    }
    file.close();
    wsLog("Cleanup finished");
    request->send(200, "Clean up finished");
    contentFS->remove("/update_actions.json");
}