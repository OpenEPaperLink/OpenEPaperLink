#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <time.h>

#include "flasher.h"
#include "makeimage.h"
#include "pendingdata.h"
#include "serial.h"
#include "soc/rtc_wdt.h"
#include "tag_db.h"
#include "web.h"

void freeHeapTask(void* parameter) {
    while (1) {
        //Serial.printf("Free heap=%d\n", ESP.getFreeHeap());
        //time_t now;
        tm tm;
        if (!getLocalTime(&tm)) {
            Serial.println("Failed to obtain time");
        }
        wsSendSysteminfo();
        vTaskDelay(30000 / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(115200);
    Serial.print(">\n");
    init_web();

    configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", "europe.pool.ntp.org", "time.nist.gov");
    // https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv

    xTaskCreate(freeHeapTask, "print free heap", 10000, NULL, 2, NULL);
    xTaskCreate(zbsRxTask, "zbsRX Process", 10000, NULL, 2, NULL);
    xTaskCreate(garbageCollection, "pending-data cleanup", 5000, NULL, 1, NULL);
    //xTaskCreate(webSocketSendProcess, "ws", 5000, NULL,configMAX_PRIORITIES-10, NULL);
}

void loop() {
    vTaskDelay(30000 / portTICK_PERIOD_MS);
}