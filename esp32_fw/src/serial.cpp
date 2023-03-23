#include <Arduino.h>
#include <HardwareSerial.h>
#include <LittleFS.h>

#include "commstructs.h"
#include "flasher.h"
#include "newproto.h"
#include "settings.h"
#include "web.h"
#include "zbs_interface.h"

#define ZBS_RX_WAIT_HEADER 0
#define ZBS_RX_WAIT_PKT_LEN 1
#define ZBS_RX_WAIT_PKT_RX 2
#define ZBS_RX_WAIT_SEP1 3
#define ZBS_RX_WAIT_SEP2 4
#define ZBS_RX_WAIT_VER 6
#define ZBS_RX_BLOCK_REQUEST 7
#define ZBS_RX_WAIT_XFERCOMPLETE 8
#define ZBS_RX_WAIT_DATA_REQ 9
#define ZBS_RX_WAIT_JOINNETWORK 10
#define ZBS_RX_WAIT_XFERTIMEOUT 11

static SemaphoreHandle_t serialWait;

uint8_t restartBlockRequest = 0;

uint16_t sendBlock(const void* data, const uint16_t len) {
    Serial1.print(">D>");
    delay(10);

    uint8_t blockbuffer[sizeof(struct blockData)];
    struct blockData* bd = (struct blockData*)blockbuffer;
    bd->size = len;
    bd->checksum = 0;

    // calculate checksum
    for (uint16_t c = 0; c < len; c++) {
        bd->checksum += ((uint8_t*)data)[c];
    }

    // send blockData header
    for (uint8_t c = 0; c < sizeof(struct blockData); c++) {
        Serial1.write(blockbuffer[c]);
    }

    // send an entire block of data
    uint16_t c;
    for (c = 0; c < len; c++) {
        Serial1.write(((uint8_t*)data)[c]);
    }
    for (; c < BLOCK_DATA_SIZE; c++) {
        Serial1.write(0);
    }

    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
    return bd->checksum;
}

void sendDataAvail(struct pendingData* pending) {
    addCRC(pending, sizeof(struct pendingData));
    Serial1.print("SDA>");
    for (uint8_t c = 0; c < sizeof(struct pendingData); c++) {
        Serial1.write(((uint8_t*)pending)[c]);
    }
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
}

void sendCancelPending(struct pendingData* pending) {
    addCRC(pending, sizeof(struct pendingData));
    Serial1.print("CXD>");
    for (uint8_t c = 0; c < sizeof(struct pendingData); c++) {
        Serial1.write(((uint8_t*)pending)[c]);
    }
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
    Serial1.write(0x00);
}

uint8_t RXState = ZBS_RX_WAIT_HEADER;
char cmdbuffer[4] = {0};
uint8_t* packetp = nullptr;
uint8_t pktlen = 0;
uint8_t pktindex = 0;
char lastchar = 0;
uint8_t charindex = 0;
uint64_t waitingForVersion = 0;
uint16_t version;

void ShortRXWaitLoop() {
    if (Serial1.available()) {
        lastchar = Serial1.read();
        Serial.write(lastchar);
        // shift characters in
        for (uint8_t c = 0; c < 3; c++) {
            cmdbuffer[c] = cmdbuffer[c + 1];
        }
        cmdbuffer[3] = lastchar;
    }
}
int8_t serialWaitReply() {
    uint32_t start = millis();
    int8_t ret = -1;
    while ((millis() - start) < 100) {
        ShortRXWaitLoop();
        if ((strncmp(cmdbuffer, "ACK>", 4) == 0)) {
            memset(cmdbuffer, 0x00, 4);
            return 1;
        }
        if ((strncmp(cmdbuffer, "NOK>", 4) == 0)) {
            memset(cmdbuffer, 0x00, 4);
            return 0;
        }
        if ((strncmp(cmdbuffer, "NOQ>", 4) == 0)) {
            memset(cmdbuffer, 0x00, 4);
            return 2;
        }
    }
    Serial.println(cmdbuffer);
    return -1;
}
bool sendEventData(struct eventData* ed) {
    addCRC(ed, sizeof(struct eventData));
    uint8_t attempts = 5;
    xSemaphoreTake(serialWait, portMAX_DELAY);
    while (attempts--) {
        uint8_t len = sizeof(struct eventData) + 5;
        uint8_t* sendp = (uint8_t*)ed;
        Serial1.print("U");
        delayMicroseconds(200);

        Serial1.print("E");
        delayMicroseconds(200);

        Serial1.print("D");
        delayMicroseconds(200);

        Serial1.print(">");
        delayMicroseconds(200);

        while (len--) {
            Serial1.write(*(sendp++));
            delayMicroseconds(200);
        }
        if (serialWaitReply() == 1) {
            Serial.println("Event Data Sent");
            xSemaphoreGive(serialWait);
            return true;
        }
        vTaskDelay(3 / portTICK_PERIOD_MS);
    }
    xSemaphoreGive(serialWait);
    Serial.println("Failed to send event data");
    return false;
}
void Ping() {
    Serial1.print("VER?");
    waitingForVersion = esp_timer_get_time();
}
void setEventMode(bool on) {
    uint8_t attempts = 5;
    xSemaphoreTake(serialWait, portMAX_DELAY);
    while (attempts--) {
        if (on) {
            Serial1.print("EEM>");
        } else {
            Serial1.print("SEM>");
        }
        if (serialWaitReply() == 1) {
            xSemaphoreGive(serialWait);
            return;
        }
    }
    xSemaphoreGive(serialWait);
    Serial.println("Failed to set mode...");
}
void SerialRXLoop() {
    if (Serial1.available()) {
        lastchar = Serial1.read();
        // Serial.write(lastchar);
        switch (RXState) {
            case ZBS_RX_WAIT_HEADER:
                Serial.write(lastchar);
                // shift characters in
                for (uint8_t c = 0; c < 3; c++) {
                    cmdbuffer[c] = cmdbuffer[c + 1];
                }
                cmdbuffer[3] = lastchar;
                if ((strncmp(cmdbuffer, "VER>", 4) == 0)) {
                    pktindex = 0;
                    RXState = ZBS_RX_WAIT_VER;
                    charindex = 0;
                    memset(cmdbuffer, 0x00, 4);
                }
                if (strncmp(cmdbuffer, "RQB>", 4) == 0) {
                    RXState = ZBS_RX_BLOCK_REQUEST;
                    charindex = 0;
                    pktindex = 0;
                    packetp = (uint8_t*)calloc(sizeof(struct espBlockRequest) + 8, 1);
                    memset(cmdbuffer, 0x00, 4);
                    restartBlockRequest = 0;
                }
                if (strncmp(cmdbuffer, "ADR>", 4) == 0) {
                    RXState = ZBS_RX_WAIT_DATA_REQ;
                    charindex = 0;
                    pktindex = 0;
                    packetp = (uint8_t*)calloc(sizeof(struct espAvailDataReq) + 8, 1);
                    memset(cmdbuffer, 0x00, 4);
                }
                if (strncmp(cmdbuffer, "BST>", 4) == 0) {
                    Serial.print(">SYNC BURST\n");
                    RXState = ZBS_RX_WAIT_HEADER;
                }
                if (strncmp(cmdbuffer, "XFC>", 4) == 0) {
                    RXState = ZBS_RX_WAIT_XFERCOMPLETE;
                    pktindex = 0;
                    packetp = (uint8_t*)calloc(sizeof(struct espXferComplete) + 8, 1);
                    memset(cmdbuffer, 0x00, 4);
                }
                if (strncmp(cmdbuffer, "XTO>", 4) == 0) {
                    RXState = ZBS_RX_WAIT_XFERTIMEOUT;
                    pktindex = 0;
                    packetp = (uint8_t*)calloc(sizeof(struct espXferComplete) + 8, 1);
                    memset(cmdbuffer, 0x00, 4);
                }
                break;
            case ZBS_RX_BLOCK_REQUEST:
                packetp[pktindex] = lastchar;
                pktindex++;
                if (pktindex == sizeof(struct espBlockRequest)) {
                    processBlockRequest((struct espBlockRequest*)packetp);
                    free(packetp);
                    RXState = ZBS_RX_WAIT_HEADER;
                }
                break;
            case ZBS_RX_WAIT_XFERCOMPLETE:
                packetp[pktindex] = lastchar;
                pktindex++;
                if (pktindex == sizeof(struct espXferComplete)) {
                    struct espXferComplete* xfc = (struct espXferComplete*)packetp;
                    processXferComplete(xfc);
                    free(packetp);
                    RXState = ZBS_RX_WAIT_HEADER;
                }
                break;
            case ZBS_RX_WAIT_XFERTIMEOUT:
                packetp[pktindex] = lastchar;
                pktindex++;
                if (pktindex == sizeof(struct espXferComplete)) {
                    struct espXferComplete* xfc = (struct espXferComplete*)packetp;
                    processXferTimeout(xfc);
                    free(packetp);
                    RXState = ZBS_RX_WAIT_HEADER;
                }
                break;
            case ZBS_RX_WAIT_DATA_REQ:
                packetp[pktindex] = lastchar;
                pktindex++;
                if (pktindex == sizeof(struct espAvailDataReq)) {
                    struct espAvailDataReq* adr = (struct espAvailDataReq*)packetp;
                    processDataReq(adr);
                    free(packetp);
                    RXState = ZBS_RX_WAIT_HEADER;
                }
                break;
            case ZBS_RX_WAIT_VER:
                waitingForVersion = 0;
                cmdbuffer[charindex] = lastchar;
                charindex++;
                if (charindex == 4) {
                    charindex = 0;
                    version = (uint16_t)strtoul(cmdbuffer, NULL, 16);
                    /*
                    uint16_t fsversion;
                    lookupFirmwareFile(fsversion);
                    if ((fsversion) && (version != fsversion)) {
                        Serial.printf("ZBS/Zigbee FW version: %04X, version on SPIFFS: %04X\n", version, fsversion);
                        Serial.printf("Performing flash update in about 30 seconds");
                        vTaskDelay(30000 / portTICK_PERIOD_MS);
                        performDeviceFlash();
                    } else if (!fsversion) {
                        Serial.println("No ZBS/Zigbee FW binary found on SPIFFS, please upload a zigbeebase000X.bin - format binary to enable flashing");
                    } else {
                        Serial.printf("ZBS/Zigbee FW version: %04X\n", version);
                    }
                    */
                    RXState = ZBS_RX_WAIT_HEADER;
                }
                break;
        }
    }
}

extern uint8_t* getDataForFile(File* file);
void zbsRxTask(void* parameter) {
    Serial1.begin(230400, SERIAL_8N1, RXD1, TXD1);

    simplePowerOn();
    bool firstrun = true;

    Serial1.print("VER?");
    waitingForVersion = esp_timer_get_time();
    serialWait = xSemaphoreCreateMutex();
    while (1) {
        xSemaphoreTake(serialWait, portMAX_DELAY);
        SerialRXLoop();

        if (Serial.available()) {
            Serial1.write(Serial.read());
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);

        if (waitingForVersion) {
            if (esp_timer_get_time() - waitingForVersion > 10000 * 1000ULL) {
                waitingForVersion = 0;
                // performDeviceFlash();
                Serial.println("I wasn't able to connect to a ZBS tag, trying to reboot the tag.");
                Serial.println("If this problem persists, please check wiring and definitions in the settings.h file, and presence of the right firmware");
                simplePowerOn();
                wsErr("The AP tag crashed. Restarting tag, regenerating all pending info.");
                refreshAllPending();
            }
        }

        if (version && firstrun) {
            Serial.printf("ZBS/Zigbee FW version: %04X\n", version);
            uint16_t fsversion;
            lookupFirmwareFile(fsversion);
            if ((fsversion) && (version != fsversion)) {
                Serial.printf("Firmware version on LittleFS: %04X\n", fsversion);
                Serial.printf("Performing flash update in about 30 seconds");
                vTaskDelay(30000 / portTICK_PERIOD_MS);
                performDeviceFlash();
            } else if (!fsversion) {
                Serial.println("No ZBS/Zigbee FW binary found on SPIFFS, please upload a zigbeebase000X.bin - format binary to enable flashing");
            }
            firstrun = false;
        }
        xSemaphoreGive(serialWait);
    }
}
