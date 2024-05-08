#ifdef HAS_BLE_WRITER
#include <Arduino.h>

#include "BLEDevice.h"
#include "ble_filter.h"
#include "newproto.h"

#define INTERVAL_BLE_SCANNING_SECONDS 60
#define INTERVAL_HANDLE_PENDING_SECONDS 10
#define BUFFER_MAX_SIZE_COMPRESSING 100000

#define BLE_MAIN_STATE_IDLE 0
#define BLE_MAIN_STATE_PREPARE 1
#define BLE_MAIN_STATE_CONNECT 2
#define BLE_MAIN_STATE_UPLOAD 3

int ble_main_state = BLE_MAIN_STATE_IDLE;
uint32_t last_ble_scan = 0;

#define BLE_UPLOAD_STATE_INIT 0
#define BLE_UPLOAD_STATE_SIZE 1
#define BLE_UPLOAD_STATE_START 2
#define BLE_UPLOAD_STATE_UPLOAD 5
int BLE_upload_state = BLE_UPLOAD_STATE_INIT;

bool BLE_connected = false;
bool BLE_new_notify = false;

static BLEUUID gicServiceUUID((uint16_t)0xfef0);
static BLEUUID gicCtrlUUID((uint16_t)0xfef1);
static BLEUUID gicImgUUID((uint16_t)0xfef2);

BLERemoteCharacteristic* ctrlChar;
BLERemoteCharacteristic* imgChar;
BLEAdvertisedDevice* myDevice;

BLEClient* pClient;

uint8_t BLE_notify_buffer[255] = {0};

uint32_t curr_part = 0;
uint8_t BLE_buff[255];

uint32_t BLE_last_notify = 0;
uint32_t BLE_last_pending_check = 0;
uint8_t BLE_curr_address[8] = {0};

uint32_t compressed_len = 0;
uint8_t* buffer;

static void notifyCallback(
    BLERemoteCharacteristic* pBLERemoteCharacteristic,
    uint8_t* pData,
    size_t length,
    bool isNotify) {
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    for (int i = 0; i < length; i++) {
        Serial.printf("%02X", pData[i]);
        BLE_notify_buffer[1 + i] = pData[i];
    }
    BLE_notify_buffer[0] = length;
    Serial.println();
    BLE_new_notify = true;
}

class MyClientCallback : public BLEClientCallbacks {
    void onConnect(BLEClient* pclient) {
        Serial.println("BLE onConnect");
        BLE_connected = true;
    }

    void onDisconnect(BLEClient* pclient) {
        Serial.println("BLE onDisconnect");
        pclient->disconnect();
        BLE_connected = false;
        ble_main_state = BLE_MAIN_STATE_IDLE;
    }
};

bool BLE_connect(uint8_t addr[8]) {
    uint8_t temp_Address[] = {addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]};
    Serial.printf("BLE Connecting to: %02X:%02X:%02X:%02X:%02X:%02X\r\n", addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
    pClient = BLEDevice::createClient();
    pClient->setClientCallbacks(new MyClientCallback());
    if (!pClient->connect(BLEAddress(temp_Address))) {
        Serial.printf("BLE connection failed\r\n");
        pClient->disconnect();
        return false;
    }
    uint32_t timeStart = millis();
    while (millis() - timeStart <= 5000) {// We wait for a few seconds as otherwise the connection might not be ready!
        delay(100);
    }
    if (!BLE_connected)
        return false;
    Serial.printf("BLE starting to get service\r\n");
    BLERemoteService* pRemoteService = pClient->getService(gicServiceUUID);
    if (pRemoteService == nullptr) {
        Serial.printf("BLE Service failed\r\n");
        pClient->disconnect();
        return false;
    }
    imgChar = pRemoteService->getCharacteristic(gicImgUUID);
    if (imgChar == nullptr) {
        Serial.printf("BLE IMG Char failed\r\n");
        pClient->disconnect();
        return false;
    }
    ctrlChar = pRemoteService->getCharacteristic(gicCtrlUUID);
    if (ctrlChar == nullptr) {
        Serial.printf("BLE ctrl Char failed\r\n");
        pClient->disconnect();
        return false;
    }
    if (ctrlChar->canNotify()) {
        ctrlChar->registerForNotify(notifyCallback);
    } else {
        Serial.printf("BLE Notify failed\r\n");
        pClient->disconnect();
        return false;
    }
    if (pClient->setMTU(255) == false) {
        Serial.printf("BLE MTU failed\r\n");
        pClient->disconnect();
        return false;
    }
    Serial.printf("BLE Connected fully to: %02X:%02X:%02X:%02X:%02X:%02X\r\n", addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
    return true;
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        BLE_filter_add_device(advertisedDevice);
    }
};

void BLE_startScan(uint32_t timeout) {
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setInterval(1349);
    pBLEScan->setWindow(449);
    pBLEScan->setActiveScan(true);
    pBLEScan->start(timeout, false);
}

void BLETask(void* parameter) {
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    Serial.println("BLE task started");
    BLEDevice::init("");
    while (1) {
        switch (ble_main_state) {
            default:
            case BLE_MAIN_STATE_IDLE:
                if (millis() - last_ble_scan > (INTERVAL_BLE_SCANNING_SECONDS * 1000)) {
                    last_ble_scan = millis();
                    Serial.println("Doing the BLE Scan");
                    BLE_startScan(10);  // timeout in seconds, this is blocking but only for this thread!
                }
                if (millis() - BLE_last_pending_check >= (INTERVAL_HANDLE_PENDING_SECONDS * 1000)) {
                    if (BLE_is_image_pending(BLE_curr_address)) {
                        delay(4000);  // We better wait here, since the pending image needs to be created first
                        Serial.println("BLE Image is pending");
                        // Here we create the compressed buffer
                        buffer = (uint8_t*)malloc(BUFFER_MAX_SIZE_COMPRESSING);
                        if (buffer == nullptr) {
                            Serial.println("BLE Could not create buffer!");
                            compressed_len = 0;
                        } else {
                            compressed_len = compress_image(BLE_curr_address, buffer, BUFFER_MAX_SIZE_COMPRESSING);
                            Serial.printf("BLE Compressed Length: %i\r\n", compressed_len);
                            // then we connect to BLE to send the compressed data
                            if (compressed_len && BLE_connect(BLE_curr_address)) {
                                curr_part = 0;
                                memset(BLE_notify_buffer, 0x00, sizeof(BLE_notify_buffer));
                                BLE_upload_state = BLE_UPLOAD_STATE_INIT;
                                ble_main_state = BLE_MAIN_STATE_UPLOAD;
                                BLE_new_notify = true;  // trigger the upload here
                            } else {
                                free(buffer);
                            }
                        }
                        BLE_last_pending_check = millis();
                    }
                }
                break;
            case BLE_MAIN_STATE_UPLOAD: {
                if (BLE_connected && BLE_new_notify) {
                    BLE_new_notify = false;
                    BLE_last_notify = millis();
                    BLE_upload_state = BLE_notify_buffer[1];

                    switch (BLE_upload_state) {
                        default:
                        case BLE_UPLOAD_STATE_INIT:
                            BLE_buff[0] = 1;
                            ctrlChar->writeValue(BLE_buff, 1);
                            break;
                        case BLE_UPLOAD_STATE_SIZE:
                            BLE_buff[0] = 0x02;
                            BLE_buff[1] = compressed_len & 0xff;
                            BLE_buff[2] = (compressed_len >> 8) & 0xff;
                            BLE_buff[3] = (compressed_len >> 16) & 0xff;
                            BLE_buff[4] = (compressed_len >> 24) & 0xff;
                            BLE_buff[5] = 0x00;
                            ctrlChar->writeValue(BLE_buff, 6);
                            break;
                        case BLE_UPLOAD_STATE_START:
                            BLE_buff[0] = 0x03;
                            ctrlChar->writeValue(BLE_buff, 1);
                            break;
                        case BLE_UPLOAD_STATE_UPLOAD:
                            if (BLE_notify_buffer[2] == 0x08) {
                                free(buffer);
                                pClient->disconnect();
                                ble_main_state = BLE_MAIN_STATE_IDLE;
                                BLE_last_pending_check = millis();
                                // Done and the image is refreshing now
                                struct espXferComplete reportStruct;
                                memcpy((uint8_t*)&reportStruct.src, BLE_curr_address, 8);
                                processXferComplete(&reportStruct, true);
                                curr_part = 0;
                            } else {
                                uint32_t req_curr_part = (BLE_notify_buffer[6] << 24) | (BLE_notify_buffer[5] << 24) | (BLE_notify_buffer[4] << 24) | BLE_notify_buffer[3];
                                if (req_curr_part != curr_part) {
                                    Serial.printf("Something went wrong, expected req part: %i but got: %i we better abort here.\r\n", req_curr_part, curr_part);
                                    free(buffer);
                                    pClient->disconnect();
                                    ble_main_state = BLE_MAIN_STATE_IDLE;
                                    BLE_last_pending_check = millis();
                                }
                                uint32_t curr_len = 240;
                                if (compressed_len - (curr_part * 240) < 240)
                                    curr_len = compressed_len - (curr_part * 240);
                                BLE_buff[0] = curr_part & 0xff;
                                BLE_buff[1] = (curr_part >> 8) & 0xff;
                                BLE_buff[2] = (curr_part >> 16) & 0xff;
                                BLE_buff[3] = (curr_part >> 24) & 0xff;
                                memcpy((uint8_t*)&BLE_buff[4], (uint8_t*)&buffer[curr_part * 240], curr_len);
                                imgChar->writeValue(BLE_buff, curr_len + 4);
                                Serial.printf("BLE sending part: %i\r\n", curr_part);
                                curr_part++;
                            }
                            break;
                    }
                } else {
                    if (millis() - BLE_last_notify > 30000) {  // Something odd, better reset connection!
                        Serial.println("BLE err going back to IDLE");
                        free(buffer);
                        pClient->disconnect();
                        ble_main_state = BLE_MAIN_STATE_IDLE;
                        BLE_last_pending_check = millis();
                    }
                }
                break;
            }
        }
        vTaskDelay(15 / portTICK_PERIOD_MS);
    }
}

#endif
