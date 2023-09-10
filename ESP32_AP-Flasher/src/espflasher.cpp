#include <Arduino.h>
#include <esp_loader.h>

#include "esp32_port.h"
#include "esp_littlefs.h"
#include "storage.h"
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

esp_loader_error_t flash_binary(const char *file_path, size_t address) {
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

bool doC6flash() {
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

    if (connect_to_target(230400) == ESP_LOADER_SUCCESS) {
        if (esp_loader_get_target() == ESP32C6_CHIP) {
            wsSerial("Connected to ESP32-C6");
            int maxRetries = 5;
            esp_loader_error_t err;

            for (int retry = 0; retry < maxRetries; retry++) {
                err = flash_binary("/bootloader.bin", 0x0);
                if (err == ESP_LOADER_SUCCESS) break;
                Serial.printf("Flash failed with error %d. Retrying...\n", err);
                delay(1000);
            }
            if (err != ESP_LOADER_SUCCESS) {
                loader_port_esp32_deinit();
                return false;
            }

            for (int retry = 0; retry < maxRetries; retry++) {
                err = flash_binary("/partition-table.bin", 0x8000);
                if (err == ESP_LOADER_SUCCESS) break;
                Serial.printf("Flash failed with error %d. Retrying...\n", err);
                delay(1000);
            }
            if (err != ESP_LOADER_SUCCESS) {
                loader_port_esp32_deinit();
                return false;
            }

            for (int retry = 0; retry < maxRetries; retry++) {
                err = flash_binary("/OpenEPaperLink_esp32_C6.bin", 0x10000);
                if (err == ESP_LOADER_SUCCESS) break;
                Serial.printf("Flash failed with error %d. Retrying...\n", err);
                delay(1000);
            }
            if (err != ESP_LOADER_SUCCESS) {
                loader_port_esp32_deinit();
                return false;
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
    return true;
}
