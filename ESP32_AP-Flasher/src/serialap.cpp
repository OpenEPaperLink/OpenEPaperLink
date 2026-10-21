#include "serialap.h"

#include <Arduino.h>
#include <HardwareSerial.h>
#include <system.h>
#include <WiFi.h>

#include "commstructs.h"
#include "contentmanager.h"
#include "flasher.h"
#include "leds.h"
#include "newproto.h"
#include "powermgt.h"
#include "settings.h"
#include "storage.h"
#include "web.h"
#include "zbs_interface.h"
#include "wifimanager.h"
#include "radiotransport.h"
#include "radioselftest.h"
#include "../../oepl-radio-v2.h"

#define LOG(format, ...) printf(format, ##__VA_ARGS__)

QueueHandle_t rxCmdQueue;
SemaphoreHandle_t txActive;


#ifndef FLASHER_DEBUG_SHARED
volatile bool rxSerialStopTask2 = false;
#endif

uint8_t channelList[6];
struct espSetChannelPower curChannel = {0, 11, 10};
int16_t radioLedBrightness = -1;  // config.led as last sent with the channel (v2 SET_CONFIG)
int16_t radioLock = -1;           // config.lock, the same


#define AP_ACTIVITY_MAX_INTERVAL 30 * 1000
volatile uint32_t lastAPActivity = 0;
struct APInfoS apInfo;

const char *apTypeName(uint8_t type) {
    switch (type) {
        case ESP32_C6:        return "ESP32-C6";
        case TLSR_ZIGBEE_AP:  return "TLSR Zigbee 802.15.4";  /* 0xF8 */
        case TLSR_GFSK_AP:    return "TLSR GFSK";        /* 0xF9 */
        default:              return "unknown";
    }
}

// Set while rxSerialTask exists; the task clears it as its last act
static TaskHandle_t volatile apReaderTask = nullptr;
static volatile bool apReaderStop = false;
// Set by the debug monitor (rxSerialTask2), handled by APTask: the only task that resets the radio
static volatile bool modemResetRequested = false;

struct rxCmd {
    uint8_t* data;
    uint8_t len;
    uint8_t type;
};


#if (AP_PROCESS_PORT == FLASHER_AP_PORT)
int8_t APpowerPins[] = FLASHER_AP_POWER;
#define AP_RESET_PIN FLASHER_AP_RESET
#define AP_POWER_PIN FLASHER_AP_POWER
#endif
#ifdef HAS_EXT_FLASHER
#if (AP_PROCESS_PORT == FLASHER_EXT_PORT)
int8_t APpowerPins[] = FLASHER_EXT_POWER;
#define AP_RESET_PIN FLASHER_EXT_RESET
#define AP_POWER_PIN FLASHER_EXT_POWER
#endif
#if (AP_PROCESS_PORT == FLASHER_ALTRADIO_PORT)
int8_t APpowerPins[] = FLASHER_ALT_POWER;
#define AP_RESET_PIN FLASHER_ALT_RESET
#define AP_POWER_PIN FLASHER_ALT_POWER
#endif
#endif

void APEnterEarlyReset() {
    pinMode(AP_RESET_PIN, OUTPUT);
    digitalWrite(AP_RESET_PIN, LOW);
}

void setAPstate(bool isOnline, uint8_t state) {
    apInfo.isOnline = isOnline;
    apInfo.state = state;
#ifdef HAS_RGB_LED
    CRGB colorMap[7] = {
        CRGB::Orange,
        CRGB::Green,
        CRGB::Blue,
        CRGB::Yellow,
        CRGB::Aqua,
        CRGB::Red,
        CRGB::YellowGreen};
    rgbIdleColor = colorMap[state];
#ifdef BLE_ONLY
    rgbIdleColor = CRGB::Green;
#endif
    rgbIdlePeriod = (isOnline ? 767 : 255);
    if (isOnline) rgbIdle();
#endif
#ifdef FLASHER_DEBUG_SHARED
    // Flasher shares port with AP comms
    if (state == AP_STATE_FLASHING) stopApReader();
#endif
    wsSendSysteminfo();
}

// Reset the tag
void APTagReset() {
    Serial.println("Resetting tag");
    uint8_t powerPins = sizeof(APpowerPins);
    if (powerPins > 0 && APpowerPins[0] == -1)
        powerPins = 0;

#ifdef FLASHER_DEBUG_PROG
    pinMode(FLASHER_DEBUG_PROG, OUTPUT);
    digitalWrite(FLASHER_DEBUG_PROG, HIGH);
#endif
    pinMode(AP_RESET_PIN, OUTPUT);
    digitalWrite(AP_RESET_PIN, LOW);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    powerControl(false, (uint8_t*)APpowerPins, powerPins);
    vTaskDelay(300 / portTICK_PERIOD_MS);
    powerControl(true, (uint8_t*)APpowerPins, powerPins);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    digitalWrite(AP_RESET_PIN, HIGH);
    vTaskDelay(100 / portTICK_PERIOD_MS);
}

// The checks on the AP state live here; what goes over the wire is the transport's job
// (radiotransport.h, legacytransport.cpp).
uint16_t sendBlock(const void* data, const uint16_t len, uint64_t ver, uint32_t offset, uint32_t totalSize) {
    if (apInfo.state == AP_STATE_NORADIO) return true;
    if (!apInfo.isOnline) return false;
    return radioTransport->sendBlock(data, len, ver, offset, totalSize);
}
bool cacheBlock(const void* data, const uint16_t len, uint64_t ver, uint32_t offset, uint32_t totalSize) {
    if (!apInfo.isOnline) return false;
    return radioTransport->cacheBlock(data, len, ver, offset, totalSize);
}

// A BLE display never hears the radio. Data the radio holds for one times out there, and that
// timeout dropped the queued image before the BLE task had sent it (#622).
static bool isBleTarget(const uint8_t mac[8]) {
    return tagBleFamily(mac, tagRecord::findByMAC(mac)) != BLE_FAMILY_NONE;
}

bool sendDataAvail(struct pendingData* pending) {
    if (apInfo.state == AP_STATE_NORADIO) return true;
    if (isBleTarget(pending->targetMac)) return false;
    if (!apInfo.isOnline) return false;
    return radioTransport->sendDataAvail(pending);
}
bool sendCancelPending(struct pendingData* pending) {
    if (apInfo.state == AP_STATE_NORADIO) return true;
    if (isBleTarget(pending->targetMac)) return true;
    if (!apInfo.isOnline) return false;
    return radioTransport->cancelPending(pending);
}
bool sendChannelPower(struct espSetChannelPower* scp) {
    if (apInfo.state == AP_STATE_NORADIO) return true;
    if ((apInfo.state != AP_STATE_ONLINE) && (apInfo.state != AP_STATE_COMING_ONLINE)) return false;
    if (!radioTransport->setChannelPower(scp)) return false;
    radioLedBrightness = config.led;
    radioLock = config.lock;
    apInfo.channel = scp->channel;
    apInfo.power = scp->power;
    return true;
}
bool sendPing() {
    if (apInfo.state == AP_STATE_NORADIO) return true;
    if (apInfo.state == AP_STATE_FLASHING) return false;
    Serial.print("ping");
    int t = millis();
    if (radioTransport->ping()) {
        Serial.printf(" ok, %dms\r\n", millis() - t);
        return true;
    }
    Serial.println(" failed");
    return false;
}
bool sendGetInfo() {
    if (apInfo.state == AP_STATE_NORADIO) return true;
    return radioTransport->requestInfo();
}
bool sendHighspeed() {
    if (apInfo.state == AP_STATE_NORADIO) return true;
    return radioTransport->enableFastLink();
}

// add RX'd request from the AP to the processor queue
void addRXQueue(uint8_t* data, uint8_t len, uint8_t type) {
    struct rxCmd* rxcmd = new struct rxCmd;
    rxcmd->data = data;
    rxcmd->len = len;
    rxcmd->type = type;
    BaseType_t queuestatus = xQueueSend(rxCmdQueue, &rxcmd, 0);
    if (queuestatus == pdFALSE) {
        if (data) free(data);
        free(rxcmd);
    }
}

// Asynchronous command processor
void rxCmdProcessor(void* parameter) {
    rxCmdQueue = xQueueCreate(30, sizeof(struct rxCmd*));
    txActive = xSemaphoreCreateBinary();
    xSemaphoreGive(txActive);
    while (1) {
        if (apInfo.isOnline) {
            struct rxCmd* rxcmd = nullptr;
            BaseType_t q = xQueueReceive(rxCmdQueue, &rxcmd, 10);
            if (q == pdTRUE) {
                switch (rxcmd->type) {
                    case RX_CMD_RQB:
                        processBlockRequest((struct espBlockRequest*)rxcmd->data);
#ifdef HAS_RGB_LED
                        // shortBlink(CRGB::Blue);
#endif
                        quickBlink(3);
                        break;
                    case RX_CMD_ADR:
                        processDataReq((struct espAvailDataReq*)rxcmd->data, true, IPAddress(0, 0, 0, 0), CHECKIN_VIA_RADIO);
#ifdef HAS_RGB_LED
                        // shortBlink(CRGB::Aqua);
#endif
                        quickBlink(1);
                        break;
                    case RX_CMD_XFC:
                        processXferComplete((struct espXferComplete*)rxcmd->data, true);
#ifdef HAS_RGB_LED
                        // shortBlink(CRGB::Purple);
#endif
                        break;
                    case RX_CMD_XTO:
                        processXferTimeout((struct espXferComplete*)rxcmd->data, true);
                        break;
                    case RX_CMD_RSET:
                        Serial.println("AP did reset, resending pending\r\n");
                        refreshAllPending();
                        if (apInfo.version == 0) {
                            // right after the S3 started, the radio's type is not known yet, and the SCP
                            // payload size depends on it (see sendChannelPower)
                            sendGetInfo();
                            vTaskDelay(200 / portTICK_PERIOD_MS);
                        }
                        sendChannelPower(&curChannel);
                        break;
                    case RX_CMD_TRD:
                        // received tag return data
                        processTagReturnData((struct espTagReturnData*)rxcmd->data, rxcmd->len, true);
                        break;
                    case RX_CMD_PRELOAD:
                        processPreload((struct espBlockRequest*)rxcmd->data);
                        break;
                    case RX_CMD_BLS:
                        processBlockServed((struct v2BlockServed*)rxcmd->data);
                        break;
                }
                if (rxcmd->data) free(rxcmd->data);
                if (rxcmd) free(rxcmd);
            }
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}
void rxSerialTask(void* parameter) {
    LOG("rxSerialTask starting\n");
    while (!apReaderStop) {
        radioTransport->onIdle();
        while (AP_SERIAL_PORT.available()) {
            radioTransport->onByte(AP_SERIAL_PORT.read());
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }

    AP_SERIAL_PORT.end();
    LOG("rxSerialTask stopped\n");
    apReaderTask = nullptr;
    vTaskDelete(NULL);
}

bool apReaderRunning() { return apReaderTask != nullptr; }

// Opens the port and starts the reader, unless it runs already: opening the port again under a
// running reader frees its receive buffer, and a second reader shares the parser state with it.
bool startApReader() {
    if (apReaderTask) return true;
#ifdef HAS_ELECROW_ADV_2_8
    // Set GPIO45 low to connect the wireless interface to the multiplexed pins
    pinMode(45, OUTPUT);
    digitalWrite(45, LOW);
#endif

    // The default 256-byte buffer holds only 1.3 ms of data at 2 Mbaud, while rxSerialTask
    // polls every millisecond and runs below the WiFi tasks. 4 KB gives about 20 ms.
    // Has to be set before begin(); a port that is already open keeps its buffer.
    AP_SERIAL_PORT.setRxBufferSize(4096);
#if (AP_PROCESS_PORT == FLASHER_AP_PORT)
    AP_SERIAL_PORT.begin(115200, SERIAL_8N1, FLASHER_AP_RXD, FLASHER_AP_TXD);
#elif defined(HAS_EXT_FLASHER)
#if (AP_PROCESS_PORT == FLASHER_EXT_PORT)
    AP_SERIAL_PORT.begin(115200, SERIAL_8N1, FLASHER_EXT_RXD, FLASHER_EXT_TXD);
#elif (AP_PROCESS_PORT == FLASHER_ALTRADIO_PORT)
    AP_SERIAL_PORT.begin(115200, SERIAL_8N1, FLASHER_AP_RXD, FLASHER_AP_TXD);
#endif
#endif
    apReaderStop = false;
    // the handle is set before the task can run, so it never sees itself unregistered.
    // 1750 bytes left 616 free in normal use, too little for one ESP_LOG (vprintf) from the UART driver.
    // 4096 since protocol v2: frame decoding and the ACKs it sends run in this task too
    if (xTaskCreate(rxSerialTask, "rxSerialTask", 4096, NULL, 11, (TaskHandle_t*)&apReaderTask) != pdPASS) {
        apReaderTask = nullptr;
        AP_SERIAL_PORT.end();
        return false;
    }
    return true;
}

// Stops the reader and closes the port. False if it did not stop within a second; it then still
// counts as running, so startApReader() won't start a second one.
bool stopApReader() {
    if (!apReaderTask) return true;
    apReaderStop = true;
    const uint32_t start = millis();
    while (apReaderTask && millis() - start < 1000) vTaskDelay(1);
    if (apReaderTask) {
        LOG("rxSerialTask did not stop\n");
        return false;
    }
    return true;
}

#if defined(FLASHER_DEBUG_RXD) && !defined(FLASHER_DEBUG_SHARED)
uint32_t millisDiff(uint32_t m) {
    uint32_t ms = millis();
    if (ms >= m)
        return ms - m;
    else
        return UINT32_MAX - m + ms + 1;
}

void rxSerialTask2(void* parameter) {
    char rxStr[100] = {0};
    int rxStrCount = 0;
    uint32_t modemResetHoldoff = millis();
    char lastchar = 0;
    time_t startTime = millis();
    int charCount = 0;
    Serial2.begin(115200, SERIAL_8N1, FLASHER_DEBUG_TXD, FLASHER_DEBUG_RXD);
    while (rxSerialStopTask2 == false) {
        while (Serial2.available()) {
            lastchar = Serial2.read();
            charCount++;

            // debug info
            Serial.write(lastchar);

            rxStr[rxStrCount] = lastchar;
            if (lastchar == '\n' || lastchar == '\r') {
                if (strncmp(rxStr, "receive buffer full, drop the current frame", 43) == 0 && millisDiff(modemResetHoldoff) > 20000) {
                    modemResetHoldoff = millis();
                    Serial.println("IEEE802.15.4 modem stuck case detected");
                    modemResetRequested = true;  // APTask does the reset
                }
                rxStrCount = 0;
                memset(rxStr, 0, sizeof(rxStr));
            } else if (rxStrCount < sizeof(rxStr) - 2) {
                rxStrCount++;
            } else {
                rxStrCount = 0;
                memset(rxStr, 0, sizeof(rxStr));
            }
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);

        time_t currentTime = millis();
        if (currentTime - startTime >= 1000) {
            if (charCount > 6000) {
                rxSerialStopTask2 = true;
                Serial.println("Serial monitor stopped because of flooding (" + String(charCount) + " characters per second)");
            }
            startTime = currentTime;
            charCount = 0;
        }
    }
    Serial2.end();
    Serial.println("Exiting AP serial monitor");
    vTaskDelete(NULL);
}
#endif

void ShowAPInfo() {
    Serial.printf("\r\n| AP Info - type %02X       |\r\n", apInfo.type);
    Serial.printf("| Ch   |             0x%02X |\r\n", apInfo.channel);
    Serial.printf("| Power|               %02X |\r\n", apInfo.power);
    Serial.printf("| MAC  | %02X%02X%02X%02X%02X%02X%02X%02X |\r\n", apInfo.mac[7], apInfo.mac[6], apInfo.mac[5], apInfo.mac[4], apInfo.mac[3], apInfo.mac[2], apInfo.mac[1], apInfo.mac[0]);
    Serial.printf("| Ver  |           0x%04X |\r\n", apInfo.version);
    Serial.printf("| Link | %16s |\r\n", radioTransport->name());
}

void notifySegmentedFlash() {
    sendAPSegmentedData(apInfo.mac, (String) "Fl     ash", 0x0800, false, true);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
#ifdef POWER_NO_SOFT_POWER
    sendAPSegmentedData(apInfo.mac, (String) "If    done", 0x0800, false, true);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    sendAPSegmentedData(apInfo.mac, (String) "RE    boot", 0x0800, false, true);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
#endif
}
void checkWaitPowerCycle() {
    // check if we should wait for a power cycle. If we do, try to inform the user the best we can, and hang.
#ifdef POWER_NO_SOFT_POWER
    setAPstate(false, AP_STATE_REQUIRED_POWER_CYCLE);
    // If we have no soft power control, we'll now wait until the device is power-cycled
    Serial.printf("Please power-cycle your AP/device\r\n");
#ifdef HAS_RGB_LED
    showColorPattern(CRGB::Aqua, CRGB::Aqua, CRGB::Red);
#endif
    while (1) {
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
#endif
}
void segmentedShowIp() {
    IPAddress IP = wm.localIP();
    char temp[12];
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    sendAPSegmentedData(apInfo.mac, (String) "IP    Addr", 0x0200, true, true);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    sprintf(temp, "%03d IP %03d", IP[0], IP[1]);
    sendAPSegmentedData(apInfo.mac, (String)temp, 0x0200, true, true);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    sprintf(temp, "%03d IP %03d", IP[2], IP[3]);
    sendAPSegmentedData(apInfo.mac, (String)temp, 0x0200, true, true);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
}

// FRM1, then both sides change to V2_BAUD and the S3 sends HELLO (oepl-radio-v2.h)
static bool startFramedLink() {
    if (!radioTransport->enableFraming()) return false;
    AP_SERIAL_PORT.flush();
    vTaskDelay((V2_SWITCH_DELAY_MS + 10) / portTICK_PERIOD_MS);
    AP_SERIAL_PORT.updateBaudRate(V2_BAUD);
    if (!v2StartLink()) return false;
    Serial.printf("protocol v2 at %d baud\r\n", V2_BAUD);
    return true;
}

bool bringAPOnline(uint8_t newState) {
#ifdef BLE_ONLY
    apInfo.state = AP_STATE_NORADIO;
#endif
    if (apInfo.state == AP_STATE_NORADIO) return true;
    if (apInfo.state == AP_STATE_FLASHING) return false;

    if (!apReaderRunning()) {
        if (!startApReader()) return false;
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    setAPstate(false, AP_STATE_OFFLINE);
    // A radio that announces protocol v2 but does not start it is reset and tried again
    for (uint8_t round = 0; round < 3; round++) {
        // Every radio starts with the old protocol at 115200 baud
        radioTransport = legacyRadioTransport();
        AP_SERIAL_PORT.updateBaudRate(115200);
        // try without rebooting, the first time
        uint32_t bootTimeout = millis();
        bool APrdy = (round == 0) && sendPing();
        if (!APrdy) {
            if (apInfo.state == AP_STATE_FLASHING) return false;
            APTagReset();
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            bootTimeout = millis();
            APrdy = false;
            while ((!APrdy) && (millis() - bootTimeout < 10 * 1000) && (apInfo.state != AP_STATE_FLASHING)) {
                APrdy = sendPing();
                vTaskDelay(300 / portTICK_PERIOD_MS);
            }
        }
        if (!APrdy) return false;

        setAPstate(false, AP_STATE_COMING_ONLINE);
        // Info first: sendChannelPower() needs the radio's type and version for the payload size,
        // and CAP> tells whether it speaks protocol v2. The radio sends its tokens after its ACK>, so
        // give the parser a moment.
        apInfo.caps = 0;
        if (!sendGetInfo()) {
            setAPstate(false, AP_STATE_OFFLINE);
            return false;
        }
        vTaskDelay(200 / portTICK_PERIOD_MS);
        if (apInfo.caps & V2_CAP_FRAMING) {
            if (!startFramedLink()) {
                Serial.println("protocol v2 did not start, resetting the radio");
                setAPstate(false, AP_STATE_OFFLINE);
                continue;
            }
            sendChannelPower(&curChannel);
        } else {
            sendChannelPower(&curChannel);
            if (apInfo.type == ESP32_C6) {
                if (sendHighspeed()) {
                    AP_SERIAL_PORT.flush();
                    vTaskDelay(10 / portTICK_PERIOD_MS);
                    AP_SERIAL_PORT.updateBaudRate(2000000);
                    Serial.println("switched to 2000000 baud");
                }
            }
        }

        vTaskDelay(200 / portTICK_PERIOD_MS);
        setAPstate(newState == AP_STATE_ONLINE ? true : false, newState);
        return true;
    }
    radioTransport = legacyRadioTransport();
    return false;
}

bool checkRadio() {
#ifdef BLE_ONLY
    return false;
#endif
#ifndef C6_OTA_FLASHING
    return true;
#endif
    // make a short between FLASHER_AP_TXD and FLASHER_AP_RXD to indicate that no radio is present
    // e.g. for flasher only, or just to use the S3 to generate images for smaller AP's
    pinMode(FLASHER_AP_TXD, OUTPUT);
    pinMode(FLASHER_AP_RXD, INPUT_PULLDOWN);
    digitalWrite(FLASHER_AP_TXD, LOW);
    if (digitalRead(FLASHER_AP_RXD) != LOW) return true;
    digitalWrite(FLASHER_AP_TXD, HIGH);
    if (digitalRead(FLASHER_AP_RXD) != HIGH) return true;
    pinMode(FLASHER_AP_TXD, INPUT_PULLDOWN);
    return false;
}

void APTask(void* parameter) {
    if (!checkRadio()) {
        // no radio
        Serial.println("Working without radio.");
        addFadeMono(config.led);
        setAPstate(true, AP_STATE_NORADIO);
        refreshAllPending();
        vTaskDelete(NULL);
        return;
    }

    xTaskCreate(rxCmdProcessor, "rxCmdProcessor", 6000, NULL, 15, NULL);
#if defined(FLASHER_DEBUG_RXD) && !defined(FLASHER_DEBUG_SHARED)
    xTaskCreate(rxSerialTask2, "rxSerialTask2", 1850, NULL, 2, NULL);
    vTaskDelay(500 / portTICK_PERIOD_MS);
#endif
    bringAPOnline();

#ifndef C6_OTA_FLASHING
    if (checkForcedAPFlash() && FLASHER_AP_MOSI != -1) {
        if (apInfo.type == SOLUM_SEG_UK && apInfo.isOnline) {
            notifySegmentedFlash();
        }
        Serial.printf("We're going to try to perform an 'AP forced flash' in\r\n");
        flashCountDown(10);
        Serial.printf("\r\nPerforming force flash of the AP\r\n");
        setAPstate(false, AP_STATE_FLASHING);
        doForcedAPFlash();
        checkWaitPowerCycle();
        bringAPOnline();
    }
#endif

    if (apInfo.isOnline) {
        // AP works!
        ShowAPInfo();

        if (apInfo.type == SOLUM_SEG_UK) {
            setAPstate(true, AP_STATE_COMING_ONLINE);
            segmentedShowIp();
            showAPSegmentedInfo(apInfo.mac, true);
            setAPstate(true, AP_STATE_ONLINE);
            updateContent(apInfo.mac);
        }

        uint16_t fsversion;
#ifndef C6_OTA_FLASHING
        if (FLASHER_AP_MOSI != -1) {
            fsversion = getAPUpdateVersion(apInfo.type);
            if ((fsversion) && (apInfo.version != fsversion)) {
                Serial.printf("Firmware version on FS: %04X\r\n", fsversion);

                Serial.printf("We're going to try to update the AP's FW in\r\n");
                flashCountDown(30);
                Serial.printf("\r\n");
                notifySegmentedFlash();
                setAPstate(false, AP_STATE_FLASHING);
                if (doAPUpdate(apInfo.type)) {
                    checkWaitPowerCycle();
                    Serial.printf("Flash completed, let's try to boot the AP!\r\n");
                    if (bringAPOnline()) {
                        // AP works
                        ShowAPInfo();
                        setAPchannel();
                    } else {
                        Serial.printf("Failed to bring up the AP after flashing seemed successful... That's not supposed to happen!\r\n");
                        Serial.printf("This can be caused by a bad AP firmware, failed or failing hardware, or the inability to fully power-cycle the AP\r\n");
                        setAPstate(false, AP_STATE_FAILED);
#ifdef HAS_RGB_LED
                        showColorPattern(CRGB::Red, CRGB::Yellow, CRGB::Red);
#endif
                    }
                } else {
                    setAPstate(false, AP_STATE_FAILED);
                    checkWaitPowerCycle();
                    Serial.println("Failed to update version on the AP :(\r\n");
#ifdef HAS_RGB_LED
                    showColorPattern(CRGB::Red, CRGB::Red, CRGB::Red);
#endif
                }
            }
        }
#endif

        refreshAllPending();
    } else {
#ifndef FLASH_TIMEOUT
#define FLASH_TIMEOUT 30
#endif

        if (FLASHER_AP_MOSI == -1) {
            Serial.printf("I wasn't able to connect to the AP radio. Did you flash it?\r\n");
            Serial.printf("The build of this firmware expects an AP tag with TXD/RXD on ESP32 pins %d and %d, does this match with your wiring?\r\n", FLASHER_AP_RXD, FLASHER_AP_TXD);
            bool onlineAfterTest = false;
#ifdef C6_OTA_FLASHING
            // Find out which wire is wrong (result in the log and the web UI); it also tries once more
            if (apInfo.state != AP_STATE_FLASHING) onlineAfterTest = radioSelfTest(radioPinsFromConfig());
#endif
            if (onlineAfterTest) {
                ShowAPInfo();
            } else {
#ifdef HAS_RGB_LED
                showColorPattern(CRGB::Red, CRGB::Yellow, CRGB::Red);
#endif
                if (apInfo.state != AP_STATE_FLASHING)  // In case we are flashing already we do not want to end in a failed AP
                    setAPstate(false, AP_STATE_FAILED);
            }
        } else {
#ifndef C6_OTA_FLASHING
            // AP unavailable, maybe time to flash?
            setAPstate(false, AP_STATE_OFFLINE);

            Serial.printf("I wasn't able to connect to a ZBS (AP) tag.\r\n");
            Serial.printf("This could be the first time this AP is booted and the AP-tag may be unflashed.\r\n");
            Serial.printf("If this tag was previously flashed succesfully but this message still shows up, there's probably something wrong with the serial connections.\r\n");
            Serial.printf("The build of this firmware expects an AP tag with TXD/RXD on ESP32 pins %d and %d, does this match with your wiring?\r\n", FLASHER_AP_RXD, FLASHER_AP_TXD);
            Serial.printf("Performing firmware flash in about %d seconds!\r\n", FLASH_TIMEOUT);
            flashCountDown(FLASH_TIMEOUT);
            if (doAPFlash()) {
                checkWaitPowerCycle();
                if (bringAPOnline()) {
                    // AP works
                    ShowAPInfo();
                    if (apInfo.type == SOLUM_SEG_UK) {
                        segmentedShowIp();
                        showAPSegmentedInfo(apInfo.mac, true);
                    }
                    refreshAllPending();
                } else {
                    Serial.printf("Failed to bring up the AP after successful flashing... That's not supposed to happen!\r\n");
                    Serial.printf("This generally means that the flasher connections (MISO/MOSI/CLK/RESET/CS) are okay,\r\n");
                    Serial.printf("but we can't (yet) talk to the AP over serial lines. Verify the pins mentioned above.\r\n\r\n");

#ifndef POWER_NO_SOFT_POWER
                    Serial.printf("The firmware you're using expects soft power control over the AP tag; if it can't\r\n");
                    Serial.printf("power-cycle the AP-tag using GPIO pin %d, this can cause this very same issue.\r\n", APpowerPins[0]);
#endif

#ifdef HAS_RGB_LED
                    showColorPattern(CRGB::Red, CRGB::Yellow, CRGB::Red);
#endif
                    setAPstate(false, AP_STATE_FAILED);
                }
            } else {
                // failed to flash
#ifdef HAS_RGB_LED
                showColorPattern(CRGB::Red, CRGB::Red, CRGB::Red);
#endif
                setAPstate(false, AP_STATE_FAILED);
                Serial.println("Failed to flash the AP :(");
                Serial.println("Seems like you're running into some issues with the wiring, or (very small chance) the tag itself");
                Serial.println("This ESP32-build expects the following pins connected to the ZBS243:");
                Serial.println("---  ZBS243 based tag              ESP32  ---");
                Serial.printf("       TXD     ----------------     %02d\r\n", FLASHER_AP_RXD);
                Serial.printf("       RXD     ----------------     %02d\r\n", FLASHER_AP_TXD);
                Serial.printf("       CS/SS   ----------------     %02d\r\n", FLASHER_AP_SS);
                Serial.printf("       MOSI    ----------------     %02d\r\n", FLASHER_AP_MOSI);
                Serial.printf("       MISO    ----------------     %02d\r\n", FLASHER_AP_MISO);
                Serial.printf("       CLK     ----------------     %02d\r\n", FLASHER_AP_CLK);
                Serial.printf("       RSET    ----------------     %02d\r\n", FLASHER_AP_RESET);
#ifdef POWER_NO_SOFT_POWER
                Serial.printf("Your firmware is configured without soft power control. This means you'll have to manually power-cycle the tag after flashing.\r\n");
#else
                Serial.printf("       POWER   ----------------     %02d\r\n", APpowerPins[0]);
#endif
                Serial.println("Please verify your wiring and try again!");
            }
#ifdef HAS_SDCARD
            if (SD_CARD_CLK == FLASHER_AP_CLK ||
                SD_CARD_MISO == FLASHER_AP_MISO ||
                SD_CARD_MOSI == FLASHER_AP_MOSI) {
                Serial.println("Reseting in 30 seconds to restore SPI state!\r\n");
                flashCountDown(30);
                ESP.restart();
            }
#endif
#endif
        }
    }

    uint8_t attempts = 0;
    uint32_t lastPingMs = 0;
    while (1) {
        if (modemResetRequested) {
            modemResetRequested = false;
            // not while the port is in other hands (flashing, self-test), they bring the radio back themselves
            if (apInfo.state == AP_STATE_ONLINE || apInfo.state == AP_STATE_FAILED) {
                config.runStatus = RUNSTATUS_STOP;
                Serial.println("IEEE802.15.4 modem stuck, resetting...");
                APTagReset();
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                const bool ok = bringAPOnline();
                if (ok) {
                    attempts = 0;
                } else {
                    // the ping recovery below tries again later
                    setAPstate(false, AP_STATE_FAILED);
                    lastAPActivity = millis();
                }
                config.runStatus = RUNSTATUS_RUN;
                logLine(String("IEEE802.15.4 modem reset ") + (ok ? "ok" : "failed"));
            }
        }
        // Protocol v2 sends a heartbeat every second; the old protocol pings after 30 s of silence
        const uint32_t pingInterval = radioTransport->pingIntervalMs();
        const bool pingDue = radioTransport->heartbeat() ? (millis() - lastPingMs >= pingInterval) : (millis() - lastAPActivity > pingInterval);
        if (((apInfo.state == AP_STATE_ONLINE) || (apInfo.state == AP_STATE_FAILED)) && pingDue) {
            lastPingMs = millis();
            bool reply = sendPing();
            if (!reply) {
                attempts++;
            } else {
                if (apInfo.isOnline == false)
                    setAPstate(true, AP_STATE_ONLINE);
                attempts = 0;
            }
            if (attempts >= radioTransport->maxMissedPings() && apInfo.state != AP_STATE_FLASHING) {
                setAPstate(false, AP_STATE_WAIT_RESET);
                if (!bringAPOnline()) {
                    // tried to reset the AP, but we failed... Maybe the AP-Tag died?
                    setAPstate(false, AP_STATE_FAILED);
#ifdef HAS_RGB_LED
                    showColorPattern(CRGB::Yellow, CRGB::Yellow, CRGB::Red);
#endif
                    lastAPActivity = millis();  // we set this to retrigger a recovery in AP_ACTIVITY_MAX_INTERVAL seconds
                } else {
                    setAPstate(true, AP_STATE_ONLINE);
                    attempts = 0;
                    refreshAllPending();
                }
            }
        }
        if (apInfo.state == AP_STATE_ONLINE) radioTransport->syncAllowList();
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
