// S3 <-> radio wiring self-test (task #32). Design and the meaning of every step:
// docs/design/radio-selftest.md. Only for ESP32-C6/H2 radios (C6_OTA_FLASHING builds).

#include "radioselftest.h"

#include <Arduino.h>
#include <ArduinoJson.h>

#include "newproto.h"
#include "serialap.h"
#include "system.h"
#include "tag_db.h"
#include "web.h"

#ifdef C6_OTA_FLASHING

#if defined(FLASHER_DEBUG_RXD) && !defined(FLASHER_DEBUG_SHARED)
#define SELFTEST_HAS_MONITOR 1
extern volatile bool rxSerialStopTask2;
#endif

static volatile bool selfTestRunning = false;
static String lastResult = "{\"running\":false,\"ok\":false,\"messages\":[],\"observations\":{}}";
static SemaphoreHandle_t resultMutex = nullptr;

RadioPins radioPinsFromConfig() {
    RadioPins p;
    p.protoTx = FLASHER_AP_TXD;
    p.protoRx = FLASHER_AP_RXD;
#ifdef FLASHER_DEBUG_SHARED
    p.dbgTx = -1;
    p.dbgRx = -1;
#else
    // the config names the debug pins from the radio's side
    p.dbgTx = config.flashPinDbgRxd;
    p.dbgRx = config.flashPinDbgTxd;
#endif
    p.reset = config.flashPinReset;
    p.prog = config.flashPinProg;
    return p;
}

// --- helpers ------------------------------------------------------------------------------------

static void say(const String& text) {
    Serial.println("selftest: " + text);
    wsSerial(text);
}

// A pin of -2 stands for a wire that is not connected (development, to simulate faults without
// driving a spare GPIO): the test never drives it and hears nothing on it.
#define SIMULATED_NC -2
static bool present(int8_t pin) { return pin >= 0 || pin == SIMULATED_NC; }
static String gpioName(int8_t pin) { return pin == SIMULATED_NC ? String("GPIO (simulated, not connected)") : "GPIO " + String(pin); }

// Messages name wires the way a builder sees them: by the pin numbers on both boards (the wiki's
// pinout table), not by UART roles. The radio's debug UART (UART0) and PROG pins are fixed per chip.
#ifdef HAS_H2
#define RADIO_NAME "H2"
#define RADIO_DBG_TX 24
#define RADIO_DBG_RX 23
#else
#define RADIO_NAME "C6"
#define RADIO_DBG_TX 16
#define RADIO_DBG_RX 17
#endif
static String s3Pin(int8_t pin) { return "S3 " + gpioName(pin); }
static String radioPin(const char* what, int gpio) { return String("the radio's ") + what + " (" RADIO_NAME " GPIO " + gpio + ")"; }
static String swapped(int8_t a, int8_t b) { return "The wires on " + s3Pin(a) + " and " + gpioName(b) + " are swapped: swap them"; }
static String misplaced(int8_t on, int8_t belongs) { return "The wire on " + s3Pin(on) + " belongs on " + gpioName(belongs) + ": move it"; }

static void releasePin(int8_t pin) {
    if (pin >= 0) pinMode(pin, INPUT);
}

// Boot mode first, then a reset pulse. download = PROG low: the ROM waits for the serial bootloader.
static void resetRadio(const RadioPins& p, bool download) {
    if (p.prog >= 0) {
        pinMode(p.prog, OUTPUT);
        digitalWrite(p.prog, download ? LOW : HIGH);
    }
    if (p.reset < 0) return;
    pinMode(p.reset, OUTPUT);
    digitalWrite(p.reset, LOW);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    digitalWrite(p.reset, HIGH);
}

enum PullResult { PULL_UNKNOWN, PULL_DRIVEN_HIGH, PULL_FLOATING, PULL_DRIVEN_LOW, PULL_ACTIVE };
static const char* pullName(PullResult r) {
    switch (r) {
        case PULL_DRIVEN_HIGH: return "driven high";
        case PULL_FLOATING: return "floating";
        case PULL_DRIVEN_LOW: return "driven low";
        case PULL_ACTIVE: return "toggling";
        default: return "not tested";
    }
}

// Does something drive this input? An idle UART TX is high; a line that follows the S3's internal
// pull resistors is not connected to a running radio (or has an external pull, see the design).
static PullResult pullTest(int8_t pin) {
    if (pin == SIMULATED_NC) return PULL_FLOATING;
    if (pin < 0) return PULL_UNKNOWN;
    uint8_t highDown = 0, highUp = 0;
    pinMode(pin, INPUT_PULLDOWN);
    delayMicroseconds(500);
    for (uint8_t i = 0; i < 50; i++) {
        highDown += digitalRead(pin);
        delayMicroseconds(200);
    }
    pinMode(pin, INPUT_PULLUP);
    delayMicroseconds(500);
    for (uint8_t i = 0; i < 50; i++) {
        highUp += digitalRead(pin);
        delayMicroseconds(200);
    }
    pinMode(pin, INPUT);
    if (highDown >= 45 && highUp >= 45) return PULL_DRIVEN_HIGH;
    if (highDown <= 5 && highUp >= 45) return PULL_FLOATING;
    if (highDown <= 5 && highUp <= 5) return PULL_DRIVEN_LOW;
    return PULL_ACTIVE;
}

// Are two S3 pins connected to each other? Drive one, pull the other the opposite way, both
// directions. Needs the radio in reset, so its outputs don't drive the lines. One direction alone
// is not enough: a PC on the S3's USB pins pulls both down with 15k, which also "follows low".
static bool pinsConnected(int8_t a, int8_t b) {
    if (a < 0 || b < 0) return false;
    bool connected = true;
    for (uint8_t dir = 0; dir < 2 && connected; dir++) {
        const int8_t drive = dir ? a : b, sense = dir ? b : a;
        pinMode(drive, OUTPUT);
        digitalWrite(drive, LOW);
        pinMode(sense, INPUT_PULLUP);
        delayMicroseconds(500);
        const bool followsLow = digitalRead(sense) == LOW;
        digitalWrite(drive, HIGH);
        pinMode(sense, INPUT_PULLDOWN);
        delayMicroseconds(500);
        const bool followsHigh = digitalRead(sense) == HIGH;
        connected = followsLow && followsHigh;
    }
    releasePin(a);
    releasePin(b);
    return connected;
}

// Collect what arrives on up to two pins (one UART each) for ms milliseconds.
struct Capture {
    String a, b;
};
static void beginRx(HardwareSerial& port, int8_t rxPin, int8_t txPin, uint32_t baud) {
    port.end();
    if (rxPin < 0 && txPin < 0) return;
    port.setRxBufferSize(2048);
    port.begin(baud, SERIAL_8N1, rxPin, txPin);
}
static void drain(HardwareSerial& port, String& out, size_t max) {
    while (port.available()) {
        char c = (char)port.read();
        if (out.length() < max) out += (c == 0) ? '.' : c;
    }
}
static Capture listen(int8_t pinA, int8_t pinB, const RadioPins& p, bool download, uint32_t ms) {
    Capture cap;
    beginRx(Serial1, pinA, -1, 115200);
    beginRx(Serial2, pinB, -1, 115200);
    resetRadio(p, download);
    uint32_t start = millis();
    while (millis() - start < ms) {
        if (pinA >= 0) drain(Serial1, cap.a, 1500);
        if (pinB >= 0) drain(Serial2, cap.b, 1500);
        vTaskDelay(2 / portTICK_PERIOD_MS);
    }
    Serial1.end();
    Serial2.end();
    releasePin(pinA);
    releasePin(pinB);
    return cap;
}

static bool hasRom(const String& s) { return s.indexOf("ESP-ROM:") >= 0; }
static bool hasFirmware(const String& s) { return s.indexOf("RES>") >= 0 || s.indexOf("RDY>") >= 0; }

// RDY? on txPin, ACK> on rxPin?
static bool askReady(int8_t rxPin, int8_t txPin, uint32_t baud) {
    if (rxPin < 0 || txPin < 0 || rxPin == txPin) return false;
    bool ok = false;
    beginRx(Serial1, rxPin, txPin, baud);
    for (uint8_t attempt = 0; attempt < 3 && !ok; attempt++) {
        while (Serial1.available()) Serial1.read();
        Serial1.print("RDY?");
        String in;
        uint32_t start = millis();
        while (millis() - start < 200 && !ok) {
            drain(Serial1, in, 200);
            ok = in.indexOf("ACK>") >= 0;
            vTaskDelay(2 / portTICK_PERIOD_MS);
        }
    }
    Serial1.end();
    releasePin(rxPin);
    releasePin(txPin);
    return ok;
}

// The ROM's serial bootloader: SYNC command, SLIP-framed. A ROM in download mode answers with a
// response frame (C0 01 08 ...).
static bool romSync(int8_t rxPin, int8_t txPin) {
    if (rxPin < 0 || txPin < 0 || rxPin == txPin) return false;
    static const uint8_t head[] = {0xC0, 0x00, 0x08, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x12, 0x20};
    bool ok = false;
    beginRx(Serial2, rxPin, txPin, 115200);
    for (uint8_t attempt = 0; attempt < 5 && !ok; attempt++) {
        while (Serial2.available()) Serial2.read();
        Serial2.write(head, sizeof(head));
        for (uint8_t i = 0; i < 32; i++) Serial2.write((uint8_t)0x55);
        Serial2.write((uint8_t)0xC0);
        Serial2.flush();
        uint32_t start = millis();
        uint8_t state = 0;
        while (millis() - start < 100 && !ok) {
            while (Serial2.available() && !ok) {
                uint8_t c = Serial2.read();
                // looking for C0 01 08: frame start, direction "response", command SYNC
                if (state == 2 && c == 0x08) {
                    ok = true;
                } else if (state == 1 && c == 0x01) {
                    state = 2;
                } else {
                    state = (c == 0xC0) ? 1 : 0;
                }
            }
            vTaskDelay(2 / portTICK_PERIOD_MS);
        }
    }
    Serial2.end();
    releasePin(rxPin);
    releasePin(txPin);
    return ok;
}

static String excerpt(const String& s) {
    String e;
    for (size_t i = 0; i < s.length() && e.length() < 120; i++) {
        char c = s[i];
        e += (c >= 32 && c < 127) ? c : ' ';
    }
    e.trim();
    return e;
}

// --- the test -----------------------------------------------------------------------------------

static void stopApSerial() {
    config.runStatus = RUNSTATUS_STOP;
    setAPstate(false, AP_STATE_FLASHING);  // keeps the rest of the AP off the serial port
    stopApReader();
#ifdef SELFTEST_HAS_MONITOR
    rxSerialStopTask2 = true;
#endif
    vTaskDelay(300 / portTICK_PERIOD_MS);
    Serial1.end();
    Serial2.end();
}

static bool restartApSerial() {
    const RadioPins normal = radioPinsFromConfig();
    resetRadio(normal, false);
    releasePin(normal.protoTx);
    releasePin(normal.protoRx);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
#ifdef SELFTEST_HAS_MONITOR
    rxSerialStopTask2 = false;
    xTaskCreate(rxSerialTask2, "rxSerialTask2", 1850, NULL, 2, NULL);
#endif
    apInfo.version = 0;
    setAPstate(false, AP_STATE_OFFLINE);
    if (bringAPOnline(AP_STATE_ONLINE)) {
        config.runStatus = RUNSTATUS_RUN;
        refreshAllPending();
        return true;
    }
    setAPstate(false, AP_STATE_FAILED);
    return false;
}

static String runTest(const RadioPins& p) {
    JsonDocument doc;
    JsonArray messages = doc["messages"].to<JsonArray>();
    JsonObject obs = doc["observations"].to<JsonObject>();
    bool ok = true;
    auto fail = [&](const String& m) {
        ok = false;
        messages.add(m);
        say(m);
    };

    JsonObject pins = obs["pins"].to<JsonObject>();
    pins["protoTx"] = p.protoTx;
    pins["protoRx"] = p.protoRx;
    pins["dbgTx"] = p.dbgTx;
    pins["dbgRx"] = p.dbgRx;
    pins["reset"] = p.reset;
    pins["prog"] = p.prog;
    const bool sixLine = present(p.dbgTx) && present(p.dbgRx);

    // 0. Is a TX pin connected to its RX pin? Every later step would then misread: the radio's
    // output appears on the S3's TX pin as well, and anything the S3 sends collides with it.
    if (p.reset >= 0) {
        say("checking for lines connected to each other");
        pinMode(p.reset, OUTPUT);
        digitalWrite(p.reset, LOW);  // radio in reset: its pins let go of the lines
        vTaskDelay(20 / portTICK_PERIOD_MS);
        const bool protoShort = pinsConnected(p.protoTx, p.protoRx);
        const bool dbgShort = sixLine && pinsConnected(p.dbgTx, p.dbgRx);
        digitalWrite(p.reset, HIGH);
        obs["protoLinesConnected"] = protoShort;
        obs["dbgLinesConnected"] = dbgShort;
        if (protoShort) fail(s3Pin(p.protoTx) + " and " + gpioName(p.protoRx) + " are connected to each other: look for a short between these two wires");
        if (dbgShort) {
            String m = s3Pin(p.dbgTx) + " and " + gpioName(p.dbgRx) + " are connected to each other: look for a short between these two wires";
#ifdef CONFIG_IDF_TARGET_ESP32S3
            // GPIO19/20 are the S3's USB D-/D+; a USB charger connects D+ and D-
            if ((p.dbgTx == 19 || p.dbgTx == 20) && (p.dbgRx == 19 || p.dbgRx == 20))
                m = "Is a USB charger plugged into the board's \"USB\" port? That disturbs the radio wires on GPIO 19 and 20. "
                    "Plug the charger into the other port (\"UART\" or \"COM\") instead, or power the board from a computer";
#endif
            fail(m);
        }
        if (protoShort || dbgShort) {
            doc["ok"] = false;
            String out;
            serializeJson(doc, out);
            return out;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // let the radio start again before step 1
    }

    // 1. Are the radio's TX lines driven? (radio running, before any reset)
    say("radio wiring test: checking the radio's output lines");
    const PullResult pullProto = pullTest(p.protoRx);
    const PullResult pullDbg = pullTest(p.dbgRx);
    obs["pullProtoRx"] = pullName(pullProto);
    obs["pullDbgRx"] = pullName(pullDbg);

    // 2./3. Where does each output appear after a reset? First on the S3's RX pins, then on its TX
    // pins (only listening, so no two outputs drive each other).
    say("resetting the radio, listening on the S3's receive pins");
    Capture rx = listen(p.protoRx, sixLine ? p.dbgRx : -1, p, false, 1500);
    say("resetting the radio, listening on the S3's transmit pins");
    Capture tx = listen(p.protoTx, sixLine ? p.dbgTx : -1, p, false, 1500);

    struct Seen {
        int8_t pin;
        const String* text;
        const char* role;
    } seen[] = {{p.protoRx, &rx.a, "protoRx"}, {p.dbgRx, &rx.b, "dbgRx"}, {p.protoTx, &tx.a, "protoTx"}, {p.dbgTx, &tx.b, "dbgTx"}};
    int8_t romPin = -1, fwPin = -1;
    JsonObject heard = obs["heard"].to<JsonObject>();
    for (auto& s : seen) {
        if (!present(s.pin)) continue;
        JsonObject h = heard[s.role].to<JsonObject>();
        h["rom"] = hasRom(*s.text);
        h["firmware"] = hasFirmware(*s.text);
        h["bytes"] = s.text->length();
        if (s.text->length()) h["text"] = excerpt(*s.text);
        if (hasRom(*s.text) && romPin < 0) romPin = s.pin;
        if (hasFirmware(*s.text) && fwPin < 0) fwPin = s.pin;
    }

    if (romPin < 0 && fwPin < 0) {
        // Nothing after two resets: is the radio there at all, and does RESET reach it?
        const bool alive = askReady(p.protoRx, p.protoTx, 2000000) || askReady(p.protoRx, p.protoTx, 115200);
        obs["aliveWithoutReset"] = alive;
        if (alive) {
            fail("The radio does not restart: no contact on the wire from " + s3Pin(p.reset) + " to the radio's EN (or RST)");
        } else if (pullProto == PULL_FLOATING && (!sixLine || pullDbg == PULL_FLOATING)) {
            fail("Nothing comes from the radio on " + s3Pin(p.protoRx) + (sixLine ? " or " + gpioName(p.dbgRx) : String("")) +
                 ": check these wires, and whether the radio has power");
        } else {
            fail("No answer from the radio at all: does it have power, and is the OpenEPaperLink radio firmware flashed?");
        }
        doc["ok"] = false;
        String out;
        serializeJson(doc, out);
        return out;
    }

    // Protocol output (RES>/RDY> from the OEPL firmware)
    if (fwPin == p.protoRx) {
        // as it should be
    } else if (fwPin >= 0 && fwPin == p.dbgRx) {
        fail(swapped(p.protoRx, p.dbgRx));
    } else if (fwPin >= 0 && fwPin == p.protoTx) {
        fail(swapped(p.protoTx, p.protoRx));
    } else if (fwPin >= 0) {
        fail(misplaced(fwPin, p.protoRx));
    } else if (pullProto == PULL_FLOATING) {
        fail("Nothing comes from the radio on " + s3Pin(p.protoRx) + ": no contact on this wire");
    } else {
        fail("The radio starts, but sends nothing on " + s3Pin(p.protoRx) + ": is the OpenEPaperLink radio firmware flashed?");
    }

    // ROM output (radio UART0); on shared-UART boards it comes on the protocol line
    if (sixLine) {
        if (romPin == p.dbgRx) {
            // as it should be
        } else if (romPin >= 0 && romPin == p.protoRx) {
            if (fwPin != p.dbgRx) fail(misplaced(p.protoRx, p.dbgRx));
        } else if (romPin >= 0 && romPin == p.dbgTx) {
            fail(swapped(p.dbgTx, p.dbgRx));
        } else if (romPin >= 0) {
            fail(misplaced(romPin, p.dbgRx));
        } else if (pullDbg == PULL_FLOATING) {
            fail("No contact on the wire from " + radioPin("TX", RADIO_DBG_TX) + " to " + s3Pin(p.dbgRx));
        } else {
            // the line is driven, but no boot text came: also the case when the radio's boot messages are off (eFuse)
            fail("Nothing on " + s3Pin(p.dbgRx) + " when the radio restarts: check the wire to " + radioPin("TX", RADIO_DBG_TX));
        }
    }

    // 5. PROG and debug TX through the ROM's serial bootloader, listening where the ROM text came
    int debugTxMessage = -1;  // index of the "debug TX" message, dropped if step 6 finds the TX wires swapped
    if (sixLine && present(p.prog) && romPin >= 0) {
        say("checking PROG and the debug TX wire (bootloader)");
        Capture boot = listen(romPin, -1, p, true, 300);
        const bool download = boot.a.indexOf("DOWNLOAD") >= 0 || boot.a.indexOf("waiting for download") >= 0;
        obs["downloadMode"] = download;
        obs["bootText"] = excerpt(boot.a);
        if (!hasRom(boot.a)) {
            obs["bootloaderSync"] = "not tested";
        } else if (!download) {
            fail("No contact on the wire from " + s3Pin(p.prog) + " to the radio's GPIO 9 (PROG/BOOT)");
        } else {
            const int8_t dbgOut = (romPin == p.dbgTx) ? p.dbgRx : p.dbgTx;
            const bool sync = romSync(romPin, dbgOut);
            obs["bootloaderSync"] = sync;
            if (!sync) {
                debugTxMessage = messages.size();
                fail("No contact on the wire from " + s3Pin(dbgOut) + " to " + radioPin("RX", RADIO_DBG_RX));
            }
        }
        if (p.prog >= 0) digitalWrite(p.prog, HIGH);
    }

    // 6. S3 TX -> radio protocol RX: RDY? must get ACK> (from a freshly booted radio, at 115200)
    if (fwPin >= 0) {
        say("checking the S3 -> radio protocol wire");
        const int8_t protoOut = (fwPin == p.protoTx) ? p.protoRx : p.protoTx;
        resetRadio(p, false);
        vTaskDelay(1500 / portTICK_PERIOD_MS);
        bool ready = askReady(fwPin, protoOut, 115200);
        obs["protocolAck"] = ready;
        if (!ready && sixLine) {
            const int8_t other = (protoOut == p.dbgTx) ? p.dbgRx : p.dbgTx;
            const bool viaOther = other != fwPin && askReady(fwPin, other, 115200);
            obs["protocolAckViaDebugTx"] = viaOther;
            if (viaOther) {
                if (debugTxMessage >= 0) messages.remove(debugTxMessage);  // explained by the swap
                fail(swapped(protoOut, other));
            } else {
                fail("No contact on the wire from " + s3Pin(protoOut) + " to the radio's RX");
            }
        } else if (!ready) {
            fail("No contact on the wire from " + s3Pin(protoOut) + " to the radio's RX");
        }
    }

    if (ok) {
        messages.add("Radio wiring OK");
        say("Radio wiring OK");
    }
    doc["ok"] = ok;
    String out;
    serializeJson(doc, out);
    return out;
}

static void storeResult(const String& json, bool running) {
    if (!resultMutex) resultMutex = xSemaphoreCreateMutex();
    xSemaphoreTake(resultMutex, portMAX_DELAY);
    JsonDocument doc;
    if (deserializeJson(doc, json) != DeserializationError::Ok) doc["messages"].to<JsonArray>().add("result could not be stored");
    doc["running"] = running;
    lastResult = "";
    serializeJson(doc, lastResult);
    xSemaphoreGive(resultMutex);
}

bool radioSelfTest(const RadioPins& p) {
    selfTestRunning = true;
    storeResult("{\"ok\":false,\"messages\":[],\"observations\":{}}", true);
    say("radio wiring test: stopping the AP service");
    stopApSerial();
    String result = runTest(p);
    storeResult(result, false);
    JsonDocument doc;
    deserializeJson(doc, result);
    for (JsonVariant m : doc["messages"].as<JsonArray>()) logLine(String("radio wiring test: ") + m.as<const char*>());
    say("radio wiring test done, bringing the radio online");
    const bool online = restartApSerial();
    say(online ? "radio online" : "radio did not come online");
    selfTestRunning = false;
    return online;
}

static void selfTestTask(void* parameter) {
    RadioPins* p = static_cast<RadioPins*>(parameter);
    radioSelfTest(*p);
    delete p;
    vTaskDelete(NULL);
}

bool startRadioSelfTest(const RadioPins& pins) {
    if (selfTestRunning || apInfo.state == AP_STATE_FLASHING) return false;
    selfTestRunning = true;
    RadioPins* copy = new RadioPins(pins);
    if (xTaskCreate(selfTestTask, "radioSelfTest", 6144, copy, 5, NULL) != pdPASS) {
        delete copy;
        selfTestRunning = false;
        return false;
    }
    return true;
}

String radioSelfTestResult() {
    if (!resultMutex) return lastResult;
    xSemaphoreTake(resultMutex, portMAX_DELAY);
    String copy = lastResult;
    xSemaphoreGive(resultMutex);
    return copy;
}

#else  // no ESP radio: nothing to test

RadioPins radioPinsFromConfig() { return RadioPins{-1, -1, -1, -1, -1, -1}; }
bool radioSelfTest(const RadioPins&) { return false; }
bool startRadioSelfTest(const RadioPins&) { return false; }
String radioSelfTestResult() { return "{\"running\":false,\"ok\":false,\"messages\":[\"no ESP radio on this board\"],\"observations\":{}}"; }

#endif
