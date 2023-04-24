#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <time.h>

#include "contentmanager.h"
#include "flasher.h"
//#include "hal/wdt_hal.h"
#include "makeimage.h"
#include "pendingdata.h"
#include "serial.h"
#include "settings.h"
#include "tag_db.h"

#ifdef HAS_USB
#include "usbflasher.h"
#endif

#include "web.h"
#include "udp.h"
#include "leds.h"

void timeTask(void* parameter) {
    while (1) {
        time_t now;
        time(&now);
        tm tm;
        if (!getLocalTime(&tm)) {
            Serial.println("Waiting for valid time from NTP-server");
        } else {
            if (now % 10 == 0) wsSendSysteminfo();
            if (now % 30 == 3) Ping();
            if (now % 300 == 6) saveDB("/current/tagDB.json");

            contentRunner();
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(115200);
    Serial.print(">\n");

    psramInit();

    Serial.println("\n\n##################################");
    Serial.printf("Internal Total heap %d, internal Free Heap %d\n", ESP.getHeapSize(), ESP.getFreeHeap());
    Serial.printf("SPIRam Total heap %d, SPIRam Free Heap %d\n", ESP.getPsramSize(), ESP.getFreePsram());
    Serial.printf("ChipRevision %d, Cpu Freq %d, SDK Version %s\n", ESP.getChipRevision(), ESP.getCpuFreqMHz(), ESP.getSdkVersion());
    Serial.printf("Flash Size %d, Flash Speed %d\n", ESP.getFlashChipSize(), ESP.getFlashChipSpeed());
    Serial.println("##################################\n\n");

    Serial.println(ESP.getFreeHeap());

    Serial.printf("Total heap: %d", ESP.getHeapSize());
    Serial.printf("Free heap: %d", ESP.getFreeHeap());
    Serial.printf("Total PSRAM: %d", ESP.getPsramSize());
    Serial.printf("Free PSRAM: %d", ESP.getFreePsram());

    #ifdef HAS_USB
    xTaskCreate(usbFlasherTask, "flasher", 10000, NULL, configMAX_PRIORITIES - 10, NULL);
    #endif

    configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", "0.nl.pool.ntp.org", "europe.pool.ntp.org", "time.nist.gov");
    // https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv

    init_web();
    init_udp();
    
    loadDB("/current/tagDB.json");

    xTaskCreate(zbsRxTask, "zbsRX Process", 10000, NULL, 2, NULL);
    xTaskCreate(garbageCollection, "pending-data cleanup", 5000, NULL, 1, NULL);
    xTaskCreate(webSocketSendProcess, "ws", 5000, NULL, configMAX_PRIORITIES - 10, NULL);
    xTaskCreate(timeTask, "timed tasks", 10000, NULL, 2, NULL);
    xTaskCreate(ledTask, "handles leds", 5000, NULL, 10,  NULL);
}

void loop() {
    vTaskDelay(30000 / portTICK_PERIOD_MS);
}