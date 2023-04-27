#include <Arduino.h>
#include <HardwareSerial.h>
#include <LittleFS.h>

#include "commstructs.h"
#include "flasher.h"
#include "newproto.h"
#include "powermgt.h"
#include "settings.h"
#include "udp.h"
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

QueueHandle_t rxCmdQueue;
SemaphoreHandle_t txActive;
extern UDPcomm udpsync;

#define CMD_REPLY_WAIT 0x00
#define CMD_REPLY_ACK 0x01
#define CMD_REPLY_NOK 0x02
#define CMD_REPLY_NOQ 0x03
volatile uint8_t cmdReplyValue = CMD_REPLY_WAIT;

#define AP_SERIAL_PORT Serial1

bool txStart() {
    while (1) {
        if (xPortInIsrContext()) {
            if (xSemaphoreTakeFromISR(txActive, NULL) == pdTRUE) return true;
        } else {
            if (xSemaphoreTake(txActive, portTICK_PERIOD_MS)) return true;
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
        Serial.println("wait... tx busy");
    }
    return false;
}

void txEnd() {
    if (xPortInIsrContext()) {
        xSemaphoreGiveFromISR(txActive, NULL);
    } else {
        xSemaphoreGive(txActive);
    }
}

bool waitCmdReply() {
    uint32_t val = millis();
    while (millis() < val + 100) {
        switch (cmdReplyValue) {
            case CMD_REPLY_WAIT:
                break;
            case CMD_REPLY_ACK:
                return true;
                break;
            case CMD_REPLY_NOK:
                return false;
                break;
            case CMD_REPLY_NOQ:
                return false;
                break;
        }
        vTaskDelay(1 / portTICK_RATE_MS);
    }
    return false;
}

uint16_t sendBlock(const void* data, const uint16_t len) {
    if (!txStart()) return 0;
    for (uint8_t attempt = 0; attempt < 5; attempt++) {
        cmdReplyValue = CMD_REPLY_WAIT;
        AP_SERIAL_PORT.print(">D>");
        if (waitCmdReply()) goto blksend;
        Serial.printf("block send failed in try %d\n", attempt);
    }
    Serial.print("Failed sending block...\n");
    txEnd();
    return 0;
blksend:
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
        AP_SERIAL_PORT.write(0xAA ^ blockbuffer[c]);
    }

    // send an entire block of data
    uint16_t c;
    for (c = 0; c < len; c++) {
        AP_SERIAL_PORT.write(0xAA ^ ((uint8_t*)data)[c]);
    }

    // fill the rest of the block-length filled with something else (will end up as 0xFF in the buffer)
    for (; c < BLOCK_DATA_SIZE; c++) {
        AP_SERIAL_PORT.write(0x55);
    }

    // dummy bytes in case some bytes were missed, makes sure the AP gets kicked out of data-loading mode
    for (c = 0; c < 32; c++) {
        AP_SERIAL_PORT.write(0xF5);
    }
    delay(10);
    txEnd();
    return bd->checksum;
}

void sendDataAvail(struct pendingData* pending) {
    if (!txStart()) return;
    addCRC(pending, sizeof(struct pendingData));
    for (uint8_t attempt = 0; attempt < 5; attempt++) {
        cmdReplyValue = CMD_REPLY_WAIT;
        AP_SERIAL_PORT.print("SDA>");
        for (uint8_t c = 0; c < sizeof(struct pendingData); c++) {
            AP_SERIAL_PORT.write(((uint8_t*)pending)[c]);
        }
        if (waitCmdReply()) goto sdasend;
        Serial.printf("SDA send failed in try %d\n", attempt);
    }
    Serial.print("SDA failed to send...\n");
    txEnd();
    return;
sdasend:
    txEnd();
}

void sendCancelPending(struct pendingData* pending) {
    if (!txStart()) return;
    addCRC(pending, sizeof(struct pendingData));
    for (uint8_t attempt = 0; attempt < 5; attempt++) {
        cmdReplyValue = CMD_REPLY_WAIT;
        AP_SERIAL_PORT.print("CXD>");
        for (uint8_t c = 0; c < sizeof(struct pendingData); c++) {
            AP_SERIAL_PORT.write(((uint8_t*)pending)[c]);
        }
        if (waitCmdReply()) goto cxdsent;
        AP_SERIAL_PORT.printf("CXD send failed in try %d\n", attempt);
    }
    AP_SERIAL_PORT.print("CXD failed to send...\n");
    txEnd();
    return;
cxdsent:
    txEnd();
}

bool sendChannelPower(struct espSetChannelPower* scp) {
    if (!txStart()) return false;
    addCRC(scp, sizeof(struct espSetChannelPower));
    for (uint8_t attempt = 0; attempt < 5; attempt++) {
        cmdReplyValue = CMD_REPLY_WAIT;
        AP_SERIAL_PORT.print("SCP>");
        for (uint8_t c = 0; c < sizeof(struct espSetChannelPower); c++) {
            AP_SERIAL_PORT.write(((uint8_t*)scp)[c]);
        }
        if (waitCmdReply()) goto scpSent;
        AP_SERIAL_PORT.printf("SCP send failed in try %d\n", attempt);
    }
    AP_SERIAL_PORT.print("SCP failed to send...\n");
    txEnd();
    return false;
scpSent:
    txEnd();
    return true;
}

uint64_t waitingForVersion = 0;
uint8_t crashcounter = 0;
uint16_t version;

void Ping() {
    if (!txStart()) return;
    AP_SERIAL_PORT.print("VER?");
    txEnd();
    waitingForVersion = esp_timer_get_time();
}

#define RX_CMD_RQB 0x01
#define RX_CMD_ADR 0x02
#define RX_CMD_XFC 0x03
#define RX_CMD_XTO 0x04

struct rxCmd {
    uint8_t* data;
    uint8_t len;
    uint8_t type;
};

void rxCmdProcessor(void* parameter) {
    rxCmdQueue = xQueueCreate(30, sizeof(struct rxCmd*));
    txActive = xSemaphoreCreateBinary();
    xSemaphoreGive(txActive);
    while (1) {
        struct rxCmd* rxcmd = nullptr;
        BaseType_t q = xQueueReceive(rxCmdQueue, &rxcmd, 10);
        if (q == pdTRUE) {
            switch (rxcmd->type) {
                case RX_CMD_RQB:
                    processBlockRequest((struct espBlockRequest*)rxcmd->data);
                    break;
                case RX_CMD_ADR:
                    processDataReq((struct espAvailDataReq*)rxcmd->data);
                    udpsync.netProcessDataReq((struct espAvailDataReq*)rxcmd->data);
                    break;
                case RX_CMD_XFC:
                    processXferComplete((struct espXferComplete*)rxcmd->data);
                    udpsync.netProcessXferComplete((struct espXferComplete*)rxcmd->data);
                    break;
                case RX_CMD_XTO:
                    processXferTimeout((struct espXferComplete*)rxcmd->data);
                    udpsync.netProcessXferTimeout((struct espXferComplete*)rxcmd->data);
                    break;
            }

            if (rxcmd->data) free(rxcmd->data);
            if (rxcmd) free(rxcmd);
        }
    }
}

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

void SerialRXLoop() {
    static char cmdbuffer[4] = {0};
    static uint8_t* packetp = nullptr;
    static uint8_t pktlen = 0;
    static uint8_t pktindex = 0;
    static uint8_t RXState = ZBS_RX_WAIT_HEADER;
    static char lastchar = 0;
    static uint8_t charindex = 0;

    while (AP_SERIAL_PORT.available()) {
        lastchar = AP_SERIAL_PORT.read();
        switch (RXState) {
            case ZBS_RX_WAIT_HEADER:
                Serial.write(lastchar);
                // shift characters in
                for (uint8_t c = 0; c < 3; c++) {
                    cmdbuffer[c] = cmdbuffer[c + 1];
                }
                cmdbuffer[3] = lastchar;

                if ((strncmp(cmdbuffer, "ACK>", 4) == 0)) cmdReplyValue = CMD_REPLY_ACK;
                if ((strncmp(cmdbuffer, "NOK>", 4) == 0)) cmdReplyValue = CMD_REPLY_NOK;
                if ((strncmp(cmdbuffer, "NOQ>", 4) == 0)) cmdReplyValue = CMD_REPLY_NOQ;

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
                }
                if (strncmp(cmdbuffer, "ADR>", 4) == 0) {
                    RXState = ZBS_RX_WAIT_DATA_REQ;
                    charindex = 0;
                    pktindex = 0;
                    packetp = (uint8_t*)calloc(sizeof(struct espAvailDataReq) + 8, 1);
                    memset(cmdbuffer, 0x00, 4);
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
                    addRXQueue(packetp, pktindex, RX_CMD_RQB);
                    RXState = ZBS_RX_WAIT_HEADER;
                }
                break;
            case ZBS_RX_WAIT_XFERCOMPLETE:
                packetp[pktindex] = lastchar;
                pktindex++;
                if (pktindex == sizeof(struct espXferComplete)) {
                    addRXQueue(packetp, pktindex, RX_CMD_XFC);
                    RXState = ZBS_RX_WAIT_HEADER;
                }
                break;
            case ZBS_RX_WAIT_XFERTIMEOUT:
                packetp[pktindex] = lastchar;
                pktindex++;
                if (pktindex == sizeof(struct espXferComplete)) {
                    addRXQueue(packetp, pktindex, RX_CMD_XTO);
                    RXState = ZBS_RX_WAIT_HEADER;
                }
                break;
            case ZBS_RX_WAIT_DATA_REQ:
                packetp[pktindex] = lastchar;
                pktindex++;
                if (pktindex == sizeof(struct espAvailDataReq)) {
                    addRXQueue(packetp, pktindex, RX_CMD_ADR);
                    RXState = ZBS_RX_WAIT_HEADER;
                }
                break;
            case ZBS_RX_WAIT_VER:
                waitingForVersion = 0;
                crashcounter = 0;
                cmdbuffer[charindex] = lastchar;
                charindex++;
                if (charindex == 4) {
                    charindex = 0;
                    version = (uint16_t)strtoul(cmdbuffer, NULL, 16);
                    RXState = ZBS_RX_WAIT_HEADER;
                }
                break;
        }
    }
}

extern uint8_t* getDataForFile(File* file);

void zbsRxTask(void* parameter) {
    xTaskCreate(rxCmdProcessor, "rxCmdProcessor", 10000, NULL, configMAX_PRIORITIES - 10, NULL);

    AP_SERIAL_PORT.begin(115200, SERIAL_8N1, FLASHER_AP_RXD, FLASHER_AP_TXD);


    pinMode(FLASHER_AP_RESET, OUTPUT);
    digitalWrite(FLASHER_AP_RESET, LOW);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    rampTagPower(FLASHER_AP_POWER, false);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    rampTagPower(FLASHER_AP_POWER, true);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    digitalWrite(FLASHER_AP_RESET, HIGH);
    rampTagPower(FLASHER_AP_POWER, true);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    //}
    bool firstrun = true;

    AP_SERIAL_PORT.print("VER?");
    waitingForVersion = esp_timer_get_time();

    while (1) {
        SerialRXLoop();

        if (Serial.available()) {
            AP_SERIAL_PORT.write(Serial.read());
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);

        if (waitingForVersion) {
            if (esp_timer_get_time() - waitingForVersion > 5000 * 1000ULL) {
                waitingForVersion = 0;
                wsLog("AP doesn't respond... " + String(crashcounter + 1));
                if (++crashcounter >= 4) {
                    crashcounter = 0;
                    if (firstrun) {
                        Serial.println("I wasn't able to connect to a ZBS tag.");
                        Serial.println("If this problem persists, please check wiring and definitions in the settings.h file, and presence of the right firmware");
                        Serial.println("Performing firmware flash in about 10 seconds");
                        vTaskDelay(10000 / portTICK_PERIOD_MS);
                        performDeviceFlash();
                    } else {
                        Serial.println("I wasn't able to connect to a ZBS tag, trying to reboot the tag.");
                        rampTagPower(FLASHER_AP_POWER, false);
                        vTaskDelay(2 / portTICK_PERIOD_MS);
                        rampTagPower(FLASHER_AP_POWER, true);
                        wsErr("The AP tag crashed. Restarting tag, regenerating all pending info.");
                        refreshAllPending();
                    }
                } else {
                    Ping();
                }
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
                if (performDeviceFlash()) {
                    rampTagPower(FLASHER_AP_POWER, true);
                } else {
                    Serial.println("Failed to update version on the AP :(");
                }
            } else if (!fsversion) {
                Serial.println("No ZBS/Zigbee FW binary found on SPIFFS, please upload a zigbeebase000X.bin - format binary to enable flashing");
            }
            firstrun = false;
        }
    }
}