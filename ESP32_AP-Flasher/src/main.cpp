
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <time.h>

#include "storage.h"
#include "contentmanager.h"
#include "flasher.h"
#include "makeimage.h"
#include "serialap.h"
#include "settings.h"
#include "system.h"
#include "tag_db.h"

#ifdef HAS_USB
#include "usbflasher.h"
#endif

#include "language.h"
#include "leds.h"
#include "udp.h"
#include "web.h"

void pinTest();

void delayedStart(void* parameter) {
    vTaskDelay(30000 / portTICK_PERIOD_MS);
    Serial.println("Resuming content generation");
    wsLog("resuming content generation");
    config.runStatus = RUNSTATUS_RUN;
    vTaskDelay(10 / portTICK_PERIOD_MS);
    vTaskDelete(NULL);
}

void timeTask(void* parameter) {
    wsSendSysteminfo();
    while (1) {
        time_t now;
        time(&now);

        if (now % 5 == 0 || apInfo.state != AP_STATE_ONLINE || config.runStatus != RUNSTATUS_RUN) wsSendSysteminfo();
        if (now % 300 == 6 && config.runStatus != RUNSTATUS_STOP) saveDB("/current/tagDB.json");
        if (apInfo.state == AP_STATE_ONLINE) contentRunner();

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void setup() {
    // starts the led task/state machine
    xTaskCreate(ledTask, "ledhandler", 2000, NULL, 2, NULL);
    vTaskDelay(10 / portTICK_PERIOD_MS);

    // show a nice pattern to indicate the AP is booting / waiting for WiFi setup
#ifdef HAS_RGB_LED
    showColorPattern(CRGB::Aqua, CRGB::Green, CRGB::Blue);
#endif

#if defined(OPENEPAPERLINK_MINI_AP_PCB) || defined(OPENEPAPERLINK_NANO_AP_PCB)
    APEnterEarlyReset();
    // this allows us to view the booting process. After the device showing up, you have 3 seconds to open a terminal on the COM port
    vTaskDelay(3000 / portTICK_PERIOD_MS);
#ifdef DEBUG_VERSION
    // Specifically for the Mini-version (using an ESP32-S2), use another serial port for debug output. Makes it possible to see core dumps
    Serial0.begin(115200, SERIAL_8N1, 38, 37);
    Serial0.printf("Started debug output...\n");
    Serial0.setDebugOutput(true);
#endif
#endif

    Serial.begin(115200);
    Serial.print(">\n");

    pinTest();
#ifdef BOARD_HAS_PSRAM
    if (!psramInit()) {
        Serial.printf("This build of the AP expects PSRAM, but we couldn't find/init any. Something is terribly wrong here! System halted.");
#ifdef HAS_RGB_LED
        showColorPattern(CRGB::Yellow, CRGB::Red, CRGB::Red);
#endif
        while (1) {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    };
    heap_caps_malloc_extmem_enable(64);
#endif

    Storage.begin();

    /*
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
    */

#ifdef HAS_USB
    // We'll need to start the 'usbflasher' task for boards with a second (USB) port. This can be used as a 'flasher' interface, using a python script on the host
    xTaskCreate(usbFlasherTask, "usbflasher", 10000, NULL, configMAX_PRIORITIES - 10, NULL);
#endif

    configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", "0.nl.pool.ntp.org", "europe.pool.ntp.org", "time.nist.gov");
    // https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv

    initAPconfig();
    updateLanguageFromConfig();
    updateBrightnessFromConfig();

    init_web();
    init_udp();

#ifdef HAS_RGB_LED
    rgbIdle();
#endif
    loadDB("/current/tagDB.json");
    // tagDBOwner = xSemaphoreCreateMutex();
    xTaskCreate(APTask, "AP Process", 6000, NULL, 2, NULL);
    xTaskCreate(webSocketSendProcess, "ws", 2000, NULL, configMAX_PRIORITIES - 10, NULL);
    vTaskDelay(10 / portTICK_PERIOD_MS);

    config.runStatus = RUNSTATUS_INIT;

    xTaskCreate(timeTask, "timed tasks", 12000, NULL, 2, NULL);

    init_time();
    logStartUp();

    esp_reset_reason_t resetReason = esp_reset_reason();
    if (resetReason == ESP_RST_PANIC) {
        Serial.println("Panic! Pausing content generation for 30 seconds");
        config.runStatus = RUNSTATUS_PAUSE;
        xTaskCreate(delayedStart, "delaystart", 2000, NULL, 2, NULL);
    } else {
        config.runStatus = RUNSTATUS_RUN;
    }
}

void loop() {
    vTaskDelay(10000 / portTICK_PERIOD_MS);
    // performDeviceFlash();
    while (1) {
        // pinTest();
        while (1) {
            vTaskDelay(10000 / portTICK_PERIOD_MS);
            // pinTest();
        }
#ifdef OPENEPAPERLINK_PCB
        if (extTagConnected()) {
            flashCountDown(3);

            pinMode(FLASHER_EXT_TEST, OUTPUT);
            digitalWrite(FLASHER_EXT_TEST, LOW);

            doTagFlash();

            vTaskDelay(10000 / portTICK_PERIOD_MS);
            pinMode(FLASHER_EXT_TEST, INPUT);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
#endif
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}