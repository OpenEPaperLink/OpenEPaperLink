
#include <Arduino.h>
#include <WiFi.h>
#include <time.h>

#include "contentmanager.h"
#include "flasher.h"
#include "serialap.h"
#include "settings.h"
#include "storage.h"
#include "system.h"
#include "tag_db.h"
#include "tagdata.h"
#include "wifimanager.h"

#ifdef HAS_USB
#include "usbflasher.h"
#endif

#include "language.h"
#include "leds.h"
#include "udp.h"
#include "util.h"
#include "web.h"

util::Timer intervalSysinfo(3000);
util::Timer intervalVars(10000);
util::Timer intervalSaveDB(300000);
util::Timer intervalContentRunner(1000);

SET_LOOP_TASK_STACK_SIZE(16 * 1024);

void pinTest();

void delayedStart(void* parameter) {
    vTaskDelay(30000 / portTICK_PERIOD_MS);
    if (config.runStatus != RUNSTATUS_RUN) {
        wsLog("starting content generation");
        config.runStatus = RUNSTATUS_RUN;
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
    vTaskDelete(NULL);
}

void setup() {
    Serial.begin(115200);
    Serial.print(">\n");
#ifdef YELLOW_IPS_AP
    extern void yellow_ap_display_init(void);
    yellow_ap_display_init();
#endif

    xTaskCreate(ledTask, "ledhandler", 2000, NULL, 2, NULL);
    vTaskDelay(10 / portTICK_PERIOD_MS);

#ifdef HAS_RGB_LED
    // show a nice pattern to indicate the AP is booting / waiting for WiFi setup
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

    // pinTest();
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

    initAPconfig();

    updateLanguageFromConfig();
    updateBrightnessFromConfig();

    init_web();

#ifdef HAS_RGB_LED
    rgbIdle();
#endif
    TagData::loadParsers("/parsers.json");
    loadDB("/current/tagDB.json");
    cleanupCurrent();
    xTaskCreate(APTask, "AP Process", 6000, NULL, 2, NULL);
    vTaskDelay(10 / portTICK_PERIOD_MS);

    config.runStatus = RUNSTATUS_INIT;
    esp_reset_reason_t resetReason = esp_reset_reason();
    if (resetReason == ESP_RST_PANIC) {
        Serial.println("Panic! Pausing content generation for 30 seconds");
        config.runStatus = RUNSTATUS_PAUSE;
    }

    xTaskCreate(initTime, "init time", 5000, NULL, 2, NULL);
    xTaskCreate(delayedStart, "delaystart", 2000, NULL, 2, NULL);

    wsSendSysteminfo();
    util::printHeap();
}

void loop() {
    ws.cleanupClients();
    wm.poll();

    if (intervalSysinfo.doRun()) {
        wsSendSysteminfo();
    }
    if (intervalVars.doRun() && config.runStatus != RUNSTATUS_STOP) {
        checkVars();
    }
    if (intervalSaveDB.doRun() && config.runStatus != RUNSTATUS_STOP) {
        saveDB("/current/tagDB.json");
    }
    if (intervalContentRunner.doRun() && apInfo.state == AP_STATE_ONLINE) {
        contentRunner();
    }

#ifdef YELLOW_IPS_AP
    extern void yellow_ap_display_loop(void);
    yellow_ap_display_loop();
#endif

#ifdef OPENEPAPERLINK_PCB
    time_t tagConnectTimer = 0;
    if (millis() - tagConnectTimer > 1000) {
        tagConnectTimer = millis();
        if (extTagConnected()) {
            flashCountDown(3);

            pinMode(FLASHER_EXT_TEST, OUTPUT);
            digitalWrite(FLASHER_EXT_TEST, LOW);

            doTagFlash();

            vTaskDelay(10000 / portTICK_PERIOD_MS);
            pinMode(FLASHER_EXT_TEST, INPUT);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
#endif

    vTaskDelay(100 / portTICK_PERIOD_MS);
}
