#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <time.h>

#include "contentmanager.h"
#include "flasher.h"
#include "makeimage.h"
#include "serialap.h"
#include "settings.h"
#include "tag_db.h"

#ifdef HAS_USB
#include "usbflasher.h"
#endif

#include "leds.h"
#include "udp.h"
#include "web.h"

void timeTask(void* parameter) {
    while (1) {
        time_t now;
        time(&now);
        tm tm;
        if (!getLocalTime(&tm)) {
            Serial.println("Waiting for valid time from NTP-server");
        } else {
            if (now % 5 == 0) {
                wsSendSysteminfo();
            }
            if (now % 300 == 6) saveDB("/current/tagDB.json");

            contentRunner();
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void setup() {
#ifdef OPENEPAPERLINK_MINI_AP_PCB
    // this allows us to view the booting process. After connecting to USB, you have 3 seconds to open a terminal on the COM port
    vTaskDelay(3000 / portTICK_PERIOD_MS);
#endif
    Serial.begin(115200);
    Serial.print(">\n");

    psramInit();

    Serial.println("\n\n##################################");
    Serial.printf("Internal Total heap %d, internal Free Heap %d\n", ESP.getHeapSize(), ESP.getFreeHeap());
    Serial.printf("SPIRam Total heap %d, SPIRam Free Heap %d\n", ESP.getPsramSize(), ESP.getFreePsram());
    Serial.printf("ChipRevision %d, Cpu Freq %d, SDK Version %s\n", ESP.getChipRevision(), ESP.getCpuFreqMHz(), ESP.getSdkVersion());
    Serial.printf("Flash Size %d, Flash Speed %d\n", ESP.getFlashChipSize(), ESP.getFlashChipSpeed());
    Serial.println("##################################\n\n");

    Serial.printf("Total heap: %d\n", ESP.getHeapSize());
    Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
    Serial.printf("Total PSRAM: %d\n", ESP.getPsramSize());
    Serial.printf("Free PSRAM: %d\n\n", ESP.getFreePsram());

    Serial.printf("ESP32 Partition table:\n");
    Serial.printf("| Type | Sub |  Offset  |   Size   |       Label      |\n");
    Serial.printf("| ---- | --- | -------- | -------- | ---------------- |\n");
    esp_partition_iterator_t pi = esp_partition_find(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, NULL);
    if (pi != NULL) {
        do {
            const esp_partition_t* p = esp_partition_get(pi);
            Serial.printf("|  %02x  | %02x  | 0x%06X | 0x%06X | %-16s |\r\n",
                          p->type, p->subtype, p->address, p->size, p->label);
        } while (pi = (esp_partition_next(pi)));
    }

#ifdef HAS_USB
    xTaskCreate(usbFlasherTask, "usbflasher", 10000, NULL, configMAX_PRIORITIES - 10, NULL);
#endif

    xTaskCreate(ledTask, "ledhandler", 5000, NULL, 2, NULL);

    configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", "0.nl.pool.ntp.org", "europe.pool.ntp.org", "time.nist.gov");
    // https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv

#ifdef HAS_RGB_LED
    showColorPattern(CRGB::Aqua, CRGB::Green, CRGB::Blue);
#endif

    initAPconfig();

    init_web();
    init_udp();

#ifdef HAS_RGB_LED
    rgbIdle();
#endif

    loadDB("/current/tagDB.json");

    xTaskCreate(APTask, "AP Process", 10000, NULL, 2, NULL);
    xTaskCreate(webSocketSendProcess, "ws", 5000, NULL, configMAX_PRIORITIES - 10, NULL);
    xTaskCreate(timeTask, "timed tasks", 10000, NULL, 2, NULL);
}

void loop() {
    vTaskDelay(10000 / portTICK_PERIOD_MS);
    // performDeviceFlash();
    while (1) {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}