
#include <Arduino.h>
#include <WiFi.h>
#include <time.h>

#include "flasher.h"
#include "leds.h"
#include "settings.h"
#include "udp.h"
#include "usbflasher.h"
#include "powermgt.h"

void setup() {
    Serial.begin(115200);
    Serial.print(">\n");

    xTaskCreate(ledTask, "ledhandler", 2000, NULL, 2, NULL);
    vTaskDelay(10 / portTICK_PERIOD_MS);

    // this allows us to view the booting process. After the device showing up, you have 3 seconds to open a terminal on the COM port
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    // Specifically for the Mini-version (using an ESP32-S2), use another serial port for debug output. Makes it possible to see core dumps
    Serial0.begin(115200, SERIAL_8N1, 2, 3);
    Serial0.printf("Started debug output...\n");
    Serial0.setDebugOutput(true);

    // pinTest();
    if (!psramInit()) {
        Serial.printf("This build of the AP expects PSRAM, but we couldn't find/init any. Something is terribly wrong here! System halted.");
        while (1) {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    };
    heap_caps_malloc_extmem_enable(64);

    int8_t powerPins2[] = FLASHER_EXT_POWER;
    powerControl(0, (uint8_t*)powerPins2, 4);

    // We'll need to start the 'usbflasher' task for boards with a second (USB) port. This can be used as a 'flasher' interface, using a python script on the host
    pinMode(FLASHER_EXT_RESET, OUTPUT);
    digitalWrite(FLASHER_EXT_RESET, HIGH);

    // pinMode(FLASHER_EXT_TEST, OUTPUT);
    // digitalWrite(FLASHER_EXT_TEST, HIGH);

    xTaskCreate(usbFlasherTask, "usbflasher", 10000, NULL, configMAX_PRIORITIES - 10, NULL);
}

void loop() {
    vTaskDelay(10000 / portTICK_PERIOD_MS);
}