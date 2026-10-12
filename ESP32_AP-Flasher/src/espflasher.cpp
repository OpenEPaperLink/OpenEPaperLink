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
#include "espflasher.h"
#include "util.h"

#define LOG(format, ... ) Serial.printf(format,## __VA_ARGS__)

#ifndef FLASHER_DEBUG_PORT
#define FLASHER_DEBUG_PORT 2
#endif

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

/* Where a manifest entry is staged on the local filesystem.

   LittleFS as configured here allows 32 characters per name - measured, not
   assumed: 32 works, 33 fails to open. Upstream's
   firmware_C6_Uart0.json still names OpenEPaperLink_esp32_C6_Uart0.bin, which
   is exactly 33, so downloading it fails with "file open error" and no amount
   of retrying helps. The plain C6 build is 27 and slips through, which is why
   only the Uart0 variant is affected. Our copy is renamed to 32 characters,
   but an access point pointed at the upstream repository still meets the long
   name - which is what this is for.

   The remote name stays untouched - the URL has to match the repository. Only
   the local copy is shortened, and from the front: the tail carries the
   variant and the extension, which is what has to stay distinguishable. */
#define LFS_NAME_MAX_HERE 32

static String stagingPath(const String &remote) {
    if (remote.length() <= LFS_NAME_MAX_HERE) return "/" + remote;
    return "/" + remote.substring(remote.length() - LFS_NAME_MAX_HERE);
}

bool downloadAndWriteBinary(String &filename, const char *url) {
    HTTPClient binaryHttp;
    bool Ret = false;
    bool bHaveFsMutex = false;

    LOG("downloadAndWriteBinary: url %s\n",url);
    binaryHttp.begin(url);
    binaryHttp.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    do {
        int binaryResponseCode = binaryHttp.GET();
        if(binaryResponseCode != HTTP_CODE_OK) {
            wsSerial("http error " + String(binaryResponseCode) + " fetching " + String(url));
            break;
        }
        int contentLength = binaryHttp.getSize();
        LOG("contentLength %d\r\n",contentLength);
        if(contentLength < 0) {
            wsSerial("Couldn't get contentLength");
            break;
        }
        xSemaphoreTake(fsMutex, portMAX_DELAY);
        bHaveFsMutex = true;
        File file = contentFS->open(filename, "wb");
        if(!file) {
            wsSerial("file open error " + String(filename));
            break;
        }
        wsSerial("downloading " + String(filename));
        WiFiClient *stream = binaryHttp.getStreamPtr();
        uint8_t buffer[1024];
        size_t totalBytesRead = 0;
     // timeout if we don't average at least 1k bytes/second
        unsigned long timeOut = millis() + contentLength;
        while(stream->connected() && totalBytesRead < contentLength) {
            size_t bytesRead;
            size_t bytesToRead;
            if(stream->available()) {
                bytesToRead = min(sizeof(buffer), (size_t) stream->available());
                bytesRead = stream->readBytes(buffer, bytesToRead);
                if(bytesRead == 0 || millis() > timeOut) {
                    wsSerial("Download time out");
                    break;
                }
                file.write(buffer, bytesRead);
                totalBytesRead += bytesRead;
                vTaskDelay(1 / portTICK_PERIOD_MS);
            } else {
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
        }
        file.close();

        // A server may close the connection right after the last byte (HTTP/1.0 style), so judge
        // by the number of bytes, not by whether the connection is still open.
        if(totalBytesRead < (size_t)contentLength) {
           wsSerial("Connection dropped during transfer");
           break;
        }
        file = contentFS->open(filename, "r");
        if(!file) {
            wsSerial("file open error " + String(filename));
            break;
        }
        if(file.size() == contentLength) {
            Ret = true;
        } else {
            wsSerial("Download failed, " + String(file.size()) + " bytes");
        }
        file.close();
    } while(false);
    binaryHttp.setReuse(false);
    binaryHttp.end();
    if(bHaveFsMutex) {
        xSemaphoreGive(fsMutex);
    }

    return Ret;
}

bool FlashC6_H2(const char *RepoUrl) {
    String JsonFilename = "/firmware_" OTA_FW_JSON ".json" ;
    bool Ret = false;
    bool bLoaderInit = false;
    bool bDownload = strlen(RepoUrl) > 0;
    int retry;
    JsonDocument jsonDoc;

    LOG("%s#%d: ",__FUNCTION__,__LINE__); util::printHeap();

    do {
        if(bDownload) {
           String FileUrl = RepoUrl + JsonFilename;
            if(!downloadAndWriteBinary(JsonFilename, FileUrl.c_str())) {
               LOG("%s#%d: ",__FUNCTION__,__LINE__); util::printHeap();
                break;
            }
        }

        File readfile = contentFS->open(JsonFilename, "r");
        if(!readfile) {
            wsSerial("load " + JsonFilename + ": Failed to open file");
            return true;
        }
        DeserializationError jsonError = deserializeJson(jsonDoc, readfile);

        if(jsonError) {
            wsSerial(String("json error parsing") + JsonFilename);
            break;
        }

        if(!bDownload) {
           Ret = true;
           break;
        }

        JsonArray jsonArray = jsonDoc.as<JsonArray>();
        for(JsonObject obj : jsonArray) {
            String remote = obj["filename"].as<String>();
            String remotePath = "/" + remote;          /* unveraendert fuer die URL */
            String filename = stagingPath(remote);     /* ggf. gekuerzt, nur lokal */
            String binaryUrl = RepoUrl + remotePath;

            /* The manifest carries a version per file and nothing used to look
               at it. Printing it is the only way to tell from the log which
               build actually went onto the chip. */
            String ver = obj["version"].as<String>();
            wsSerial("  " + remote + (ver.length() ? " (version " + ver + ")" : ""));

            for(retry = 0; retry < 10; retry++) {
                if(downloadAndWriteBinary(filename, binaryUrl.c_str())) {
                    break;
                }
                wsSerial("Retry " + String(retry));
                if(retry < 9) {
                    delay(1000);
                }
            }
            if(retry == 10) {
                break;
            }
        }
        if(retry < 10) {
           Ret = true;
        }
    } while(false);

    if(Ret == true) do {
       Ret = false;
        /* Read the pins out first. The local below is called "config" as
           well and would shadow the global one inside its own initialiser -
           which compiles to nonsense rather than an error in some contexts. */
        const int8_t pinRx = ::config.flashPinDbgTxd;
        const int8_t pinTx = ::config.flashPinDbgRxd;
        const int8_t pinRst = ::config.flashPinReset;
        const int8_t pinProg = ::config.flashPinProg;
        const loader_esp32_config_t config = {
            .baud_rate = 115200,
            .uart_port = FLASHER_DEBUG_PORT,
            .uart_rx_pin = (gpio_num_t)pinRx,
            .uart_tx_pin = (gpio_num_t)pinTx,
            .reset_trigger_pin = (gpio_num_t)pinRst,
            .gpio0_trigger_pin = (gpio_num_t)pinProg,
        };

        bLoaderInit = true;
        if(loader_port_esp32_init(&config) != ESP_LOADER_SUCCESS) {
            wsSerial("Serial initialization failed");
            break;
        }

        if(connect_to_target(115200) != ESP_LOADER_SUCCESS) {
            wsSerial("Connection to the " SHORT_CHIP_NAME " failed");
            break;
        }

#ifdef ESP_CHIP_TYPE
        if(esp_loader_get_target() != ESP_CHIP_TYPE) {
            wsSerial("Connected to wrong ESP32 type");
            break;
        }
#else
        /* Boards whose radio co-processor is not an ESP (a TLSR825x, written
           over SWS) define no chip type - there is nothing to compare. */
#endif
        wsSerial("Connected to ESP32-" SHORT_CHIP_NAME);
        int maxRetries = 5;
        esp_loader_error_t err;

        JsonArray jsonArray = jsonDoc.as<JsonArray>();
        for(JsonObject obj : jsonArray) {
            String filename = stagingPath(obj["filename"].as<String>());
            const char *addressStr = obj["address"];
            uint32_t address = strtoul(addressStr, NULL, 16);

            for(int retry = 0; retry < maxRetries; retry++) {
                err = flash_binary(filename, address);
                if(err == ESP_LOADER_SUCCESS) {
                   Ret = true;
                   break;
                }
                Serial.printf("Flash failed with error %d. Retrying...\n", err);
                delay(1000);
            }
            if(err != ESP_LOADER_SUCCESS) {
                break;
            }
        }
        Serial.println("Done!");
    } while(false);

    if(bLoaderInit) {
        loader_port_esp32_deinit();
    }

    LOG("%s#%d: ",__FUNCTION__,__LINE__); util::printHeap();
    return Ret;
}

