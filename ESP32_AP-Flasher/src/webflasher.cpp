#include "webflasher.h"

#include <ArduinoJson.h>

#include "flasher.h"
#include "ips_display.h"
#include "storage.h"
#include "util.h"
#include "web.h"

uint8_t webFlashMode = 0;
bool tagConnected = false;

Logger::Logger() {
}

Logger::~Logger() {
}

size_t Logger::write(uint8_t c) {
    wsSerial(String((char)c));
    return 1;
}

size_t Logger::write(const uint8_t* buffer, size_t size) {
    wsSerial(String((const char*)buffer, size));
    return size;
}

size_t Logger::printf(const char* format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    size_t len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    wsSerial(String(buffer));
    return len;
}

size_t Logger::println(const char* text) {
    size_t len = print(text);
    print('\n');
    return len + 1;
}

Logger logger;
util::Timer tagConnectTimer(seconds(2));

void webflasher_loop() {
    if (tagConnectTimer.doRun() || webFlashMode > 0) {
        if (extTagConnected()) {
            if (!tagConnected) {
                tftOverride = true;
                tft2.fillScreen(TFT_DARKGREEN);
                tft2.setCursor(0, 0, 4);
                tft2.setTextColor(TFT_WHITE);
                tft2.print("Connected");
            }
            tagConnected = true;

            pinMode(FLASHER_EXT_TEST, OUTPUT);
            digitalWrite(FLASHER_EXT_TEST, LOW);

            // doTagFlash();

            vTaskDelay(10000 / portTICK_PERIOD_MS);
            pinMode(FLASHER_EXT_TEST, INPUT);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        } else {
            if (tagConnected) {
                tft2.fillScreen(TFT_RED);
                tft2.setCursor(0, 0, 4);
                tft2.setTextColor(TFT_WHITE);
                tft2.print("Disconnected");
            }
            tagConnected = false;
        }
    }
}

void changeWebflashMode(uint8_t mode) {
    switch (mode) {
        case 0:
            break;
        case 1:
            break;
    }
    webFlashMode = mode;
}

void handleWSdata(uint8_t* data, size_t len, AsyncWebSocketClient* client) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, (const char*)data);
    StaticJsonDocument<250> response;
    response["flashstatus"] = 1;

    if (error) {
        wsSerial("Failed to parse JSON");
        return;
    }

    if (doc.containsKey("flashcmd")) {
        uint16_t flashcmd = doc["flashcmd"].as<int>();
        switch (flashcmd) {
            case WEBFLASH_ENABLE_AUTOFLASH:
                break;
            case WEBFLASH_PINTEST:
                response["ok"] = 1;
                pinTest();
                break;
            case WEBFLASH_FOCUS:
                tftOverride = true;
                tft2.fillScreen(TFT_BLUE);
                tft2.setCursor(0, 0, 4);
                tft2.setTextColor(TFT_WHITE);
                tft2.print("Flasher");
                webFlashMode = 1;
                break;
            case WEBFLASH_BLUR:
                webFlashMode = 0;
                tftOverride = false;
                sendAvail(0xFC);
                break;
        }
    }

    String jsonResponse;
    serializeJson(response, jsonResponse);
    client->text(jsonResponse);
}