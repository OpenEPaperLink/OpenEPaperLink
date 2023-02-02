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
bool waitingForVersion = true;
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

void SerialRXLoop() {
    if (Serial1.available()) {
        lastchar = Serial1.read();
        Serial.write(lastchar);
        switch (RXState) {
            case ZBS_RX_WAIT_HEADER:
                // shift characters in
                for (uint8_t c = 0; c < 3; c++) {
                    cmdbuffer[c] = cmdbuffer[c + 1];
                }
                cmdbuffer[3] = lastchar;
                if ((strncmp(cmdbuffer, "VER>", 4) == 0) && waitingForVersion) {
                    waitingForVersion = false;
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
                cmdbuffer[charindex] = lastchar;
                charindex++;
                if (charindex == 4) {
                    charindex = 0;
                    version = (uint16_t)strtoul(cmdbuffer, NULL, 16);
                    uint16_t fsversion;  // BREAK here! break;
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
    Serial1.print("VER?");
    while (1) {
        SerialRXLoop();

        if (Serial.available()) {
            Serial1.write(Serial.read());
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
        if (waitingForVersion) {
            if (millis() > 30000) {
                waitingForVersion = false;
                performDeviceFlash();
                // Serial.printf("We've been waiting for communication from the tag, but got nothing. This is expected if this tag hasn't been flashed yet. We'll try to flash it.\n");
                // doAPUpdate();
                //performDeviceFlash();
                // SDAtest();
            }
        }
    }
}
