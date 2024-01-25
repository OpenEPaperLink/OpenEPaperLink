#include "webflasher.h"

#ifdef HAS_EXT_FLASHER

#include <ArduinoJson.h>

#include "flasher.h"
#include "ips_display.h"
#include "settings.h"
#include "storage.h"
#include "swd.h"
#include "util.h"
#include "web.h"
#include "zbs_interface.h"

#define FLASHMODE_OFF 0
#define FLASHMODE_AUTO_BACKGROUND 1
#define FLASHMODE_AUTO_FOCUS 2

#define AUTOFLASH_STEP_IDLE 0
#define AUTOFLASH_STEP_CONNECT 1
#define AUTOFLASH_STEP_DISCONNECT 2
#define AUTOFLASH_STEP_WAITDISCONNECT 3
#define AUTOFLASH_STEP_FINISHED 4
#define AUTOFLASH_STEP_ABORT 5
#define AUTOFLASH_STEP_COUNTDOWN 6
#define AUTOFLASH_STEP_COUNTING 8
#define AUTOFLASH_STEP_WRITE 7

#define Seriallog logger

uint8_t webFlashMode = FLASHMODE_AUTO_BACKGROUND;
uint8_t autoFlashStep = AUTOFLASH_STEP_IDLE;

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

#ifdef HAS_TFT
    if (len >= 6 && strncmp(buffer, "\r[", 2) == 0) {
        char content[50];
        strncpy(content, buffer + 2, len - 7);
        content[len - 7] = '\0';
        tft2.fillRect(0, 50, tft2.width(), 40, TFT_BLUE);
        tft2.setCursor(10, 57, 2);
        tft2.setTextColor(TFT_YELLOW);
        tft2.print(String(content));
    }
#endif

    return len;
}

size_t Logger::println(const char* text) {
    size_t len = print(text);
    print('\n');
    return len + 1;
}

Logger logger;
util::Timer tagConnectTimer(seconds(2));
flasher* f;
uint8_t errors = 0;

void infoDisplay(String actionName, uint8_t icon) {
    wsSerial("  " + actionName, "white");
#ifdef HAS_TFT
    tftOverride = true;
    tft2.fillScreen(TFT_NAVY);
    tft2.setCursor(0, 0, 4);
    tft2.setTextColor(TFT_WHITE);
    tft2.print("Progress:");
    tft2.setCursor(18, 30, 2);
    tft2.setTextColor(TFT_WHITE);
    tft2.print(actionName);
#endif
}

bool report(bool result) {
    if (result) {
        wsSerial("<✔", "green");
#ifdef HAS_TFT
        tft2.setTextColor(TFT_GREEN);
        tft2.setCursor(5, 30, 2);
        tft2.print("v");
        tft2.setCursor(6, 30, 2);
        tft2.print("v");
        vTaskDelay(200 / portTICK_PERIOD_MS);
#endif
    } else {
        errors++;
        autoFlashStep = AUTOFLASH_STEP_ABORT;
        wsSerial("<✘", "red");
#ifdef HAS_TFT
        tft2.setTextColor(TFT_RED);
        tft2.setCursor(5, 30, 2);
        tft2.print("x");
        tft2.setCursor(6, 30, 2);
        tft2.print("x");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
#endif
    }
    return result;
}

void webflasher_loop() {

    static int32_t countDownTimer = 0;
    switch (autoFlashStep) {

        case AUTOFLASH_STEP_IDLE: {

            if (webFlashMode != FLASHMODE_OFF && (tagConnectTimer.doRun() || webFlashMode == FLASHMODE_AUTO_FOCUS)) {
                if (extTagConnected()) autoFlashStep = AUTOFLASH_STEP_CONNECT;
            }
            break;
        }

        case AUTOFLASH_STEP_CONNECT: {
            
            wsSerial("-", "clear");
            infoDisplay("Connecting", 0);
            vTaskDelay(2000 / portTICK_PERIOD_MS);

            pinMode(FLASHER_EXT_TEST, OUTPUT);
            digitalWrite(FLASHER_EXT_TEST, LOW);

            f = new flasher();
            errors = 0;

            if (!report(f->connectTag(FLASHER_EXT_PORT))) {

                Seriallog.printf("Sorry, failed to connect to this tag...\n");
                break;

            } else {
                infoDisplay("Get firmware md5", 0);
                report(f->getFirmwareMD5());

                infoDisplay("Find tag", 0);
                if (f->findTagByMD5()) {
                    // this tag currently contains original firmware, found its fingerprint
                    Seriallog.printf("Found original firmware tag (fingerprint %s)\n", f->md5char);

                    infoDisplay("Read info block", 0);
                    report(f->readInfoBlock());
                    if (errors) break;

                    infoDisplay("Get firmware mac", 0);
                    report(f->getFirmwareMac());
                    if (errors) break;

                    infoDisplay("Prepare info block", 0);
                    report(f->prepareInfoBlock());
                    if (errors) break;

                    f->includeInfoBlock = true;
                    autoFlashStep = AUTOFLASH_STEP_COUNTDOWN;
                    break;

                } else if (report(f->getInfoBlockMD5())) {

                    infoDisplay("Find by md5", 0);
                    // did find an infoblock MD5 that looks valid
                    if (report(f->findTagByMD5())) {
                        // did find the md5 in the database

                        infoDisplay("Found tag", 0);
                        Seriallog.printf("Found an already-flashed tag (fingerprint %s)\n", f->md5char);

                        infoDisplay("Get infoblock mac", 0);
                        report(f->getInfoBlockMac());
                        if (errors) break;

                        infoDisplay("Get infoblock type", 0);
                        report(f->getInfoBlockType());
                        if (errors) break;

                        infoDisplay("Read infoblock", 0);
                        report(f->readInfoBlock());
                        if (errors) break;

                        f->includeInfoBlock = false;
                        autoFlashStep = AUTOFLASH_STEP_COUNTDOWN;
                        break;

                    } else {
                        infoDisplay("Unknown fingerprint", 0);
                        report(false);
                        // couldn't find the md5 from the infoblock
                        Seriallog.printf("Found an already-flashed tag, but we couldn't find its fingerprint (%s) in the database\n", f->md5char);
                        break;
                    }
                } else {
                    // We couldn't recognize the tag from its fingerprint...
                    Seriallog.printf("Found a tag but didn't recognize its fingerprint (%s)\n", f->md5char);

                    infoDisplay("Backup firmware", 0);
                    report(f->backupFlash());

                    Seriallog.printf("Saved this MD5 binary to filesystem\n");

                    infoDisplay("Unknown tag", 0);
                    report(false);
                    break;

                }
            }

            f->zbs->reset(false);

            delete f;
            autoFlashStep = AUTOFLASH_STEP_FINISHED;
            break;
        }

        case AUTOFLASH_STEP_COUNTDOWN: {

            infoDisplay("Final Countdown", 0);

            countDownTimer = millis() + 4000;
            autoFlashStep = AUTOFLASH_STEP_COUNTING;
            break;

        }

        case AUTOFLASH_STEP_COUNTING: {

            uint32_t timenow = millis();
            if (timenow >= countDownTimer) {
                autoFlashStep = AUTOFLASH_STEP_WRITE;
                timenow = countDownTimer;
            }

            float timeDifference = countDownTimer - timenow;
            char timeString[10];
            snprintf(timeString, sizeof(timeString), "%.1fs", timeDifference / 1000.0);

#ifdef HAS_TFT
            tftOverride = true;
            tft2.fillRect(0, 50, tft2.width(), 40, TFT_BLUE);
            tft2.setCursor(10, 55, 4);
            tft2.setTextColor(TFT_YELLOW);
            tft2.print(timeString);
#endif
            wsSerial("\r" + String(timeString), "yellow");
            break;
        }

        case AUTOFLASH_STEP_WRITE: {

            if (f->includeInfoBlock) {
                infoDisplay("Write info block", 0);
                report(f->writeInfoBlock());
                if (errors) break;
            }

            infoDisplay("Write flash", 0);
            report(f->writeFlashFromPack("/Tag_FW_Pack.bin", f->tagtype));
            if (errors) break;

            f->zbs->reset(false);
            delete f;

            infoDisplay("Write successful", 0);
            report(true);

#ifdef HAS_TFT
            tftOverride = true;
            tft2.fillRect(0, 50, tft2.width(), 40, TFT_DARKGREEN);
            tft2.setCursor(0, 55, 4);
            tft2.setTextColor(TFT_WHITE);
            tft2.print("OK, Done!");
            vTaskDelay(2000 / portTICK_PERIOD_MS);
#endif
            wsSerial("Flashing succeeded", "green");

            autoFlashStep = AUTOFLASH_STEP_FINISHED;
            break;
        }

        case AUTOFLASH_STEP_ABORT: {

#ifdef HAS_TFT
            tftOverride = true;
            tft2.fillRect(0, 50, tft2.width(), 40, TFT_RED);
            tft2.setCursor(0, 55, 4);
            tft2.setTextColor(TFT_WHITE);
            tft2.print("Flash failed");
            vTaskDelay(2000 / portTICK_PERIOD_MS);
#endif
            wsSerial("Flashing failed", "red");

            f->zbs->reset(false);
            delete f;

            vTaskDelay(100 / portTICK_PERIOD_MS);
            pinMode(FLASHER_EXT_TEST, INPUT_PULLDOWN);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            autoFlashStep = AUTOFLASH_STEP_WAITDISCONNECT;
            break;
        }

        case AUTOFLASH_STEP_FINISHED: {

            Seriallog.printf("You can safely disconnect the tag");

            vTaskDelay(100 / portTICK_PERIOD_MS);
            pinMode(FLASHER_EXT_TEST, INPUT_PULLDOWN);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            autoFlashStep = AUTOFLASH_STEP_WAITDISCONNECT;
            break;
        }

        case AUTOFLASH_STEP_WAITDISCONNECT: {

            if (!extTagConnected()) autoFlashStep = AUTOFLASH_STEP_DISCONNECT;
            vTaskDelay(500 / portTICK_PERIOD_MS);
            break;
        }

        case AUTOFLASH_STEP_DISCONNECT: {

#ifdef HAS_TFT
            tftOverride = true;
            tft2.fillScreen(TFT_NAVY);
            tft2.setCursor(0, 0, 4);
            tft2.setTextColor(TFT_WHITE);
            tft2.print("Flasher mode");
            tft2.setCursor(5, 30, 2);
            tft2.setTextColor(TFT_WHITE);
            tft2.print("Ready to connect a tag");
            if (webFlashMode == FLASHMODE_AUTO_BACKGROUND) tftOverride = false;
#endif
            wsSerial("Ready to connect a tag", "white");
            autoFlashStep = AUTOFLASH_STEP_IDLE;
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            break;
        }
    }
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
#ifdef HAS_TFT            
                tftOverride = true;
                tft2.fillScreen(TFT_NAVY);
                tft2.setCursor(0, 0, 4);
                tft2.setTextColor(TFT_WHITE);
                tft2.print("Flasher mode");
                tft2.setCursor(5, 30, 2);
                tft2.setTextColor(TFT_WHITE);
                tft2.print("Ready to connect a tag");
#endif
                wsSerial("Ready to connect a tag", "white");
                if (webFlashMode == FLASHMODE_AUTO_BACKGROUND) webFlashMode = FLASHMODE_AUTO_FOCUS;
                autoFlashStep = AUTOFLASH_STEP_IDLE;
                break;
            case WEBFLASH_BLUR:
                if (webFlashMode == FLASHMODE_AUTO_FOCUS) webFlashMode = FLASHMODE_AUTO_BACKGROUND;
#ifdef HAS_TFT
                tftOverride = false;
                sendAvail(0xFC);
#endif
                break;
        }
    }

    String jsonResponse;
    serializeJson(response, jsonResponse);
    client->text(jsonResponse);
}

#endif