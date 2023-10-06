#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <HTTPClient.h>
#include <esp_loader.h>

#include "esp32_port.h"
#include "esp_littlefs.h"
#include "storage.h"
#include "tag_db.h"
#include "web.h"

esp_loader_error_t connect_to_target(uint32_t higher_transmission_rate) {
    esp_loader_connect_args_t connect_config = ESP_LOADER_CONNECT_DEFAULT();
    esp_loader_error_t err = esp_loader_connect(&connect_config);
    if (err != ESP_LOADER_SUCCESS) {
        printf("Cannot connect to target. Error: %u\n", err);
        return err;
    }
    printf("Connected to target\n");

    if (higher_transmission_rate && esp_loader_get_target() != ESP8266_CHIP) {
        err = esp_loader_change_transmission_rate(higher_transmission_rate);
        if (err == ESP_LOADER_ERROR_UNSUPPORTED_FUNC) {
            printf("ESP8266 does not support change transmission rate command.");
            return err;
        } else if (err != ESP_LOADER_SUCCESS) {
            printf("Unable to change transmission rate on target.");
            return err;
        } else {
            err = loader_port_change_transmission_rate(higher_transmission_rate);
            if (err != ESP_LOADER_SUCCESS) {
                printf("Unable to change transmission rate.");
                return err;
            }
            printf("Transmission rate changed\n");
        }
    }
    return ESP_LOADER_SUCCESS;
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

esp_loader_error_t flash_binary(String &file_path, size_t address) {
    esp_loader_error_t err;

    wsSerial("Flashing " + String(file_path));

    File file = contentFS->open(file_path, "rb");
    if (!file) {
        wsSerial("Failed to open file");
        return ESP_LOADER_ERROR_FAIL;
    }

    size_t size = file.size();
    static uint8_t payload[1024];
    Serial.println("file size: " + String(size));

    printf("Erasing flash (this may take a while)...\n");
    err = esp_loader_flash_start(address, size, sizeof(payload));
    if (err != ESP_LOADER_SUCCESS) {
        wsSerial("Erasing flash failed");
        file.close();
        return err;
    }
    printf("Start programming\n");

    size_t written = 0;
    size_t sizeleft = size;
    uint32_t t = millis();
    while (sizeleft > 0) {
        size_t to_read = MIN(sizeleft, sizeof(payload));
        size_t bytes_read = file.readBytes(reinterpret_cast<char *>(payload), to_read);

        if (bytes_read != to_read) {
            wsSerial("Failed to read file.");
            file.close();
            return ESP_LOADER_ERROR_FAIL;
        }

        err = esp_loader_flash_write(payload, to_read);
        if (err != ESP_LOADER_SUCCESS) {
            wsSerial("Packet could not be written!");
            file.close();
            return err;
        }

        sizeleft -= to_read;
        written += to_read;

        if (millis() - t > 250) {
            uint32_t progress = written * 100 / size;
            // printf("\rProgress: %d%%", progress);
            wsSerial("Progress: " + String(progress) + "%");
            fflush(stdout);
            t = millis();
        }
    };
    wsSerial("Progress: 100%");

    file.close();
    printf("\nFinished programming\n");

#if MD5_ENABLED
    err = esp_loader_flash_verify();
    if (err == ESP_LOADER_ERROR_UNSUPPORTED_FUNC) {
        printf("ESP8266 does not support flash verify command.");
        return err;
    } else if (err != ESP_LOADER_SUCCESS) {
        wsSerial("MD5 does not match.");
        return err;
    }
    wsSerial("Flash verified");
#endif

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    return ESP_LOADER_SUCCESS;
}

bool downloadAndWriteBinary(String &filename, const char *url) {
    HTTPClient binaryHttp;
    Serial.println(url);
    binaryHttp.begin(url);
    binaryHttp.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    int binaryResponseCode = binaryHttp.GET();
    Serial.println(binaryResponseCode);
    if (binaryResponseCode == HTTP_CODE_OK) {
        int contentLength = binaryHttp.getSize();
        Serial.println(contentLength);
        xSemaphoreTake(fsMutex, portMAX_DELAY);
        File file = contentFS->open(filename, "wb");
        if (file) {
            wsSerial("downloading " + String(filename));
            WiFiClient *stream = binaryHttp.getStreamPtr();
            uint8_t buffer[1024];
            size_t totalBytesRead = 0;
            time_t timeOut = millis() + 5000;
            // while (stream->available()) {
            while (millis() < timeOut) {
                size_t bytesRead = stream->readBytes(buffer, sizeof(buffer));
                file.write(buffer, bytesRead);
                totalBytesRead += bytesRead;
                if (totalBytesRead == contentLength) break;
            }
            file.close();
            xSemaphoreGive(fsMutex);
            binaryHttp.end();

            file = contentFS->open(filename, "r");
            if (file) {
                if (totalBytesRead == contentLength || (contentLength == 0 && file.size() > 0)) {
                    file.close();
                    return true;
                }
                wsSerial("Download failed, " + String(file.size()) + " bytes");
                file.close();
            }
        } else {
            xSemaphoreGive(fsMutex);
            wsSerial("file open error " + String(filename));
        }
    } else {
        wsSerial("http error " + String(binaryResponseCode) + " fetching " + String(url));
    }
    binaryHttp.end();
    return false;
}

bool doC6flash(uint8_t doDownload) {
    const String githubUrl = "https://raw.githubusercontent.com/" + config.repo + "/master/binaries/ESP32-C6/firmware.json";
    HTTPClient http;
    Serial.println(githubUrl);
    http.begin(githubUrl);
    http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    int httpResponseCode = http.GET();
    Serial.println(httpResponseCode);
    if (httpResponseCode == HTTP_CODE_OK) {
        DynamicJsonDocument jsonDoc(1024);
        DeserializationError jsonError = deserializeJson(jsonDoc, http.getStream());
        http.end();

        if (!jsonError) {
            if (doDownload) {
                JsonArray jsonArray = jsonDoc.as<JsonArray>();
                for (JsonObject obj : jsonArray) {
                    String filename = "/" + obj["filename"].as<String>();
                    // String binaryUrl = "https://raw.githubusercontent.com/" + config.repo + "/master/binaries/ESP32-C6" + String(filename);
                    String binaryUrl = "http://www.openepaperlink.eu/binaries/ESP32-C6" + String(filename);
                    for (int retry = 0; retry < 10; retry++) {
                        if (downloadAndWriteBinary(filename, binaryUrl.c_str())) {
                            break;
                        }
                        wsSerial("Retry " + String(retry));
                        if (retry < 9) {
                            delay(1000);
                        } else {
                            return false;
                        }
                    }
                }
            }
        } else {
            wsSerial("json error fetching " + String(githubUrl));
            return false;
        }

        const loader_esp32_config_t config = {
            .baud_rate = 115200,
            .uart_port = 2,
            .uart_rx_pin = FLASHER_DEBUG_TXD,
            .uart_tx_pin = FLASHER_DEBUG_RXD,
            .reset_trigger_pin = FLASHER_AP_RESET,
            .gpio0_trigger_pin = FLASHER_DEBUG_PROG,
        };

        if (loader_port_esp32_init(&config) != ESP_LOADER_SUCCESS) {
            wsSerial("Serial initialization failed");
            loader_port_esp32_deinit();
            return false;
        }

        if (connect_to_target(115200) == ESP_LOADER_SUCCESS) {
            if (esp_loader_get_target() == ESP32C6_CHIP) {
                wsSerial("Connected to ESP32-C6");
                int maxRetries = 5;
                esp_loader_error_t err;

                JsonArray jsonArray = jsonDoc.as<JsonArray>();
                for (JsonObject obj : jsonArray) {
                    String filename = "/" + obj["filename"].as<String>();
                    const char *addressStr = obj["address"];
                    uint32_t address = strtoul(addressStr, NULL, 16);

                    for (int retry = 0; retry < maxRetries; retry++) {
                        err = flash_binary(filename, address);
                        if (err == ESP_LOADER_SUCCESS) break;
                        Serial.printf("Flash failed with error %d. Retrying...\n", err);
                        delay(1000);
                    }
                    if (err != ESP_LOADER_SUCCESS) {
                        loader_port_esp32_deinit();
                        return false;
                    }
                }

                Serial.println("Done!");
            } else {
                wsSerial("Connected to wrong ESP32 type");
                loader_port_esp32_deinit();
                return false;
            }
        } else {
            wsSerial("Connection failed");
            loader_port_esp32_deinit();
            return false;
        }
        loader_port_esp32_deinit();
    } else {
        wsSerial("http error " + String(httpResponseCode) + " fetching " + String(githubUrl));
        return false;
    }
    return true;
}
