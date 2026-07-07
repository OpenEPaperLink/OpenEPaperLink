#ifdef HAS_BLE_WRITER
#include <Arduino.h>
#include <MD5Builder.h>

#include "BLEDevice.h"
#include "ble_filter.h"
#include "newproto.h"
#include "tag_db.h"
#include "web.h"

#define INTERVAL_BLE_SCANNING_SECONDS 60
#define INTERVAL_HANDLE_PENDING_SECONDS 10
#define BUFFER_MAX_SIZE_COMPRESSING 135000

#define BLE_MAIN_STATE_IDLE 0
#define BLE_MAIN_STATE_PREPARE 1
#define BLE_MAIN_STATE_CONNECT 2
#define BLE_MAIN_STATE_UPLOAD 3
#define BLE_MAIN_STATE_ATC_BLE_OEPL_UPLOAD 4
#define BLE_MAIN_STATE_ATC_BLE_OEPL_CMD 5

// expectedNextCheckin sentinel that the web frontend renders as "In deep sleep"
#define BLE_DEEPSLEEP_CHECKIN 3216153600

int ble_main_state = BLE_MAIN_STATE_IDLE;
uint32_t last_ble_scan = 0;
bool BLE_cmd_isDeepsleep = false;

#define BLE_UPLOAD_STATE_INIT 0
#define BLE_UPLOAD_STATE_SIZE 1
#define BLE_UPLOAD_STATE_START 2
#define BLE_UPLOAD_STATE_UPLOAD 5
int BLE_upload_state = BLE_UPLOAD_STATE_INIT;

#define BLE_CMD_ACK_CMD 99
#define BLE_CMD_AVAILDATA 100
#define BLE_CMD_BLK_DATA 101
#define BLE_CMD_ERR_BLKPRT 196
#define BLE_CMD_ACK_BLKPRT 197
#define BLE_CMD_REQ 198
#define BLE_CMD_ACK 199
#define BLE_CMD_ACK_IS_SHOWN 200
#define BLE_CMD_ACK_FW_UPDATED 201

struct AvailDataInfo BLEavaildatainfo = {0};
struct blockRequest BLEblkRequst = {0};

bool BLE_connected = false;
bool BLE_new_notify = false;

static BLEUUID ATC_BLE_OEPL_ServiceUUID((uint16_t)0x1337);
static BLEUUID ATC_BLE_OEPL_CtrlUUID((uint16_t)0x1337);

static BLEUUID gicServiceUUID((uint16_t)0xfef0);
static BLEUUID gicCtrlUUID((uint16_t)0xfef1);
static BLEUUID gicImgUUID((uint16_t)0xfef2);

BLERemoteCharacteristic* ctrlChar;
BLERemoteCharacteristic* imgChar;
BLEAdvertisedDevice* myDevice;
BLEClient* pClient;

uint8_t BLE_notify_buffer[256] = {0};

uint32_t BLE_err_counter = 0;
uint32_t BLE_curr_part = 0;
uint32_t BLE_max_block_parts = 0;
uint8_t BLE_mini_buff[256];

uint32_t BLE_last_notify = 0;
uint32_t BLE_last_pending_check = 0;
uint8_t BLE_curr_address[8] = {0};

// Connect-failure counter for the command path. We cannot reuse BLE_err_counter
// here because BLE_connect() resets it to 0 on every call, so a command aimed at
// an unreachable / already-sleeping display would otherwise retry forever and
// block all other BLE work.
uint32_t BLE_cmd_conn_fails = 0;

uint32_t BLE_compressed_len = 0;
uint8_t* BLE_image_buffer;

// Advertisements discovered during a scan are queued here and processed from
// within BLETask (which has a large stack). The scan callback runs in the
// Bluedroid host task with only a few KB of stack, so doing the full
// processDataReq() there overflows the stack and reboots the ESP32.
#define BLE_DATA_REQ_QUEUE_LEN 64
QueueHandle_t BLE_dataReqQueue = nullptr;

void BLE_enqueue_data_req(struct espAvailDataReq* req) {
    if (BLE_dataReqQueue == nullptr) return;
    // Non-blocking: if the queue is full we simply drop this advertisement,
    // the next scan will pick the tag up again. Never block the BLE host task.
    xQueueSend(BLE_dataReqQueue, req, 0);
}

// Emits a websocket log line that starts with the tag MAC (same MAC byte
// order as processBlockRequest). The web frontend blinks the tag card yellow
// ("loading") for every such line, so this gives per-block progress feedback
// during a BLE upload instead of only the single flash at the very end.
static void BLE_reportProgress(const char* what, uint32_t value) {
    char buffer[80];
    sprintf(buffer, "%02X%02X%02X%02X%02X%02X%02X%02X BLE %s %u",
            BLE_curr_address[7], BLE_curr_address[6], BLE_curr_address[5], BLE_curr_address[4],
            BLE_curr_address[3], BLE_curr_address[2], BLE_curr_address[1], BLE_curr_address[0],
            what, value);
    wsLog((String)buffer);
}

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

enum BLE_CONNECTION_TYPE {
    BLE_TYPE_GICISKY = 0,
    BLE_TYPE_ATC_BLE_OEPL
};

bool BLE_connect(uint8_t addr[8], BLE_CONNECTION_TYPE conn_type) {
    BLE_err_counter = 0;
    uint8_t temp_Address[] = {addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]};
    Serial.printf("BLE Connecting to: %02X:%02X:%02X:%02X:%02X:%02X\r\n", addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
    static MyClientCallback clientCallback;
    // Free the client from the previous transfer. BLEDevice::createClient()
    // does 'new BLEClient()' on every call and never reuses or frees them, so
    // without this every connection attempt leaked one client. We only get here
    // from the IDLE state: the previous transfer already disconnected, the
    // GATTC interface was released in ESP_GATTC_DISCONNECT_EVT, and nothing
    // references the old client anymore, so a plain delete is safe here.
    if (pClient != nullptr) {
        delete pClient;
        pClient = nullptr;
    }
    pClient = BLEDevice::createClient();
    pClient->setClientCallbacks(&clientCallback);
    if (!pClient->connect(BLEAddress(temp_Address))) {
        Serial.printf("BLE connection failed\r\n");
        pClient->disconnect();
        return false;
    }
    uint32_t timeStart = millis();
    while (millis() - timeStart <= 5000) {  // We wait for a few seconds as otherwise the connection might not be ready!
        delay(100);
    }
    if (!BLE_connected)
        return false;
    Serial.printf("BLE starting to get service\r\n");
    BLERemoteService* pRemoteService = pClient->getService((conn_type == BLE_TYPE_GICISKY) ? gicServiceUUID : ATC_BLE_OEPL_ServiceUUID);
    if (pRemoteService == nullptr) {
        Serial.printf("BLE Service failed\r\n");
        pClient->disconnect();
        return false;
    }
    if (conn_type == BLE_TYPE_GICISKY) {
        imgChar = pRemoteService->getCharacteristic(gicImgUUID);
        if (imgChar == nullptr) {
            Serial.printf("BLE IMG Char failed\r\n");
            pClient->disconnect();
            return false;
        }
    }
    ctrlChar = pRemoteService->getCharacteristic((conn_type == BLE_TYPE_GICISKY) ? gicCtrlUUID : ATC_BLE_OEPL_CtrlUUID);
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
    static MyAdvertisedDeviceCallbacks advertisedDeviceCallbacks;
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(&advertisedDeviceCallbacks);
    pBLEScan->setInterval(1349);
    pBLEScan->setWindow(449);
    pBLEScan->setActiveScan(true);
    pBLEScan->start(timeout, false);
}

#define BLOCK_DATA_SIZE_BLE 4096
#define BLOCK_PART_DATA_SIZE_BLE 230
uint8_t tempBlockBuffer[BLOCK_DATA_SIZE_BLE + 4];
uint8_t tempPacketBuffer[2 + 3 + BLOCK_PART_DATA_SIZE_BLE];
void ATC_BLE_OEPL_PrepareBlk(uint8_t indexBlockId) {
    if (BLE_image_buffer == nullptr) {
        return;
    }
    uint32_t bufferPosition = (BLOCK_DATA_SIZE_BLE * indexBlockId);
    uint32_t lenNow = BLOCK_DATA_SIZE_BLE;
    uint16_t crcCalc = 0;
    if ((BLE_compressed_len - bufferPosition) < BLOCK_DATA_SIZE_BLE)
        lenNow = (BLE_compressed_len - bufferPosition);
    tempBlockBuffer[0] = lenNow & 0xff;
    tempBlockBuffer[1] = (lenNow >> 8) & 0xff;
    for (uint16_t c = 0; c < lenNow; c++) {
        tempBlockBuffer[4 + c] = BLE_image_buffer[c + bufferPosition];
        crcCalc += tempBlockBuffer[4 + c];
    }
    tempBlockBuffer[2] = crcCalc & 0xff;
    tempBlockBuffer[3] = (crcCalc >> 8) & 0xff;
    BLE_max_block_parts = (4 + lenNow) / BLOCK_PART_DATA_SIZE_BLE;
    if ((4 + lenNow) % BLOCK_PART_DATA_SIZE_BLE)
        BLE_max_block_parts++;
    Serial.println("Preparing block: " + String(indexBlockId) + " BuffPos: " + String(bufferPosition) + " LenNow: " + String(lenNow) + " MaxBLEparts: " + String(BLE_max_block_parts));
    BLE_curr_part = 0;
}

void ATC_BLE_OEPL_SendPart(uint8_t indexBlockId, uint8_t indexPkt) {
    uint8_t crcCalc = indexBlockId + indexPkt;
    for (uint16_t c = 0; c < BLOCK_PART_DATA_SIZE_BLE; c++) {
        tempPacketBuffer[5 + c] = tempBlockBuffer[c + (BLOCK_PART_DATA_SIZE_BLE * indexPkt)];
        crcCalc += tempPacketBuffer[5 + c];
    }
    tempPacketBuffer[0] = 0x00;
    tempPacketBuffer[1] = 0x65;
    tempPacketBuffer[2] = crcCalc;
    tempPacketBuffer[3] = indexBlockId;
    tempPacketBuffer[4] = indexPkt;
    Serial.println("BLE Sending packet Len " + String(sizeof(tempPacketBuffer)));
    ctrlChar->writeValue(tempPacketBuffer, sizeof(tempPacketBuffer), true);
}

// Finish handling a queued command for BLE_curr_address: report the transfer as
// complete (clears the pending item), reset the retry counters, and - for a deep
// sleep - flag the tag as sleeping so the UI shows "In deep sleep" until it is
// seen in a scan again.
static void BLE_finishCommand(bool markSleep) {
    struct espXferComplete reportStruct;
    memcpy((uint8_t*)&reportStruct.src, BLE_curr_address, 8);
    processXferComplete(&reportStruct, true);
    BLE_clear_attempts(BLE_curr_address);
    BLE_err_counter = 0;
    BLE_cmd_conn_fails = 0;
    if (markSleep) {
        tagRecord* taginfo = tagRecord::findByMAC(BLE_curr_address);
        if (taginfo != nullptr) {
            taginfo->expectedNextCheckin = BLE_DEEPSLEEP_CHECKIN;
            wsSendTaginfo(BLE_curr_address, SYNC_TAGSTATUS);
        }
    }
}

void BLETask(void* parameter) {
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    Serial.println("BLE task started");
    BLE_dataReqQueue = xQueueCreate(BLE_DATA_REQ_QUEUE_LEN, sizeof(struct espAvailDataReq));
    BLEDevice::init("");
    while (1) {
        // Process advertisements collected by the scan callback here, in this
        // task's large stack, instead of inside the tiny BLE host task stack.
        struct espAvailDataReq incomingDataReq;
        while (BLE_dataReqQueue != nullptr && xQueueReceive(BLE_dataReqQueue, &incomingDataReq, 0) == pdTRUE) {
            processDataReq(&incomingDataReq, true);
        }
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
                        Serial.println("BLE Image is pending but we wait a bit");
                        delay(5000);                                                       // We better wait here, since the pending image needs to be created first
                        if (BLE_curr_address[7] == 0x13 && BLE_curr_address[6] == 0x37) {  // This is an ATC BLE OEPL display
                            // A pending item may be a command (deep sleep, reboot, reset, ...)
                            // instead of an image. Handle that first and skip the image upload
                            // path when it is a command.
                            uint8_t cmdFrame[2 + sizeof(struct AvailDataInfo)];
                            uint8_t cmdFrameLen = 0;
                            bool isDeepsleep = false;
                            if (get_ATC_BLE_OEPL_command(BLE_curr_address, cmdFrame, &cmdFrameLen, &isDeepsleep)) {
                                if (cmdFrameLen == 0) {
                                    // No BLE equivalent (e.g. a network scan); just clear it so
                                    // the queue does not get stuck retrying it forever.
                                    Serial.println("BLE command has no BLE equivalent, clearing it");
                                    BLE_finishCommand(false);
                                } else if (BLE_connect(BLE_curr_address, BLE_TYPE_ATC_BLE_OEPL)) {
                                    BLE_cmd_conn_fails = 0;
                                    memset(BLE_notify_buffer, 0x00, sizeof(BLE_notify_buffer));
                                    BLE_new_notify = false;
                                    BLE_cmd_isDeepsleep = isDeepsleep;
                                    ctrlChar->writeValue(cmdFrame, cmdFrameLen, true);  // write-with-response
                                    Serial.printf("BLE command frame sent (%u bytes)\r\n", cmdFrameLen);
                                    if (isDeepsleep) {
                                        // The display shuts its BLE stack down right after ACK-ing a
                                        // deep sleep, so we usually can't read the ACK before it
                                        // disconnects. Having connected and written the command (with
                                        // response) is proof enough of delivery, so finish here -
                                        // otherwise reconnect attempts to the now sleeping display
                                        // would loop and block all other BLE work.
                                        pClient->disconnect();
                                        ble_main_state = BLE_MAIN_STATE_IDLE;
                                        BLE_finishCommand(true);
                                    } else {
                                        BLE_last_notify = millis();
                                        ble_main_state = BLE_MAIN_STATE_ATC_BLE_OEPL_CMD;
                                    }
                                } else {
                                    // Connect failed. Give up after a few tries so a command aimed
                                    // at an unreachable / already-sleeping display cannot block the
                                    // BLE task forever (BLE_connect resets BLE_err_counter, hence the
                                    // separate counter here).
                                    if (++BLE_cmd_conn_fails >= 5) {
                                        Serial.println("BLE command: giving up after repeated connect failures");
                                        BLE_finishCommand(false);
                                    }
                                }
                                BLE_last_pending_check = millis();
                                break;  // handled this pending item, skip the image path
                            }
                            // Here we create the compressed buffer
                            BLE_image_buffer = (uint8_t*)malloc(BUFFER_MAX_SIZE_COMPRESSING);
                            if (BLE_image_buffer == nullptr) {
                                Serial.println("BLE Could not create buffer!");
                                BLE_compressed_len = 0;
                            } else {
                                uint8_t dataType = 0x00;
                                uint8_t dataTypeArgument = 0x00;
                                uint16_t nextCheckin = 0x00;
                                BLE_compressed_len = get_ATC_BLE_OEPL_image(BLE_curr_address, BLE_image_buffer, BUFFER_MAX_SIZE_COMPRESSING, &dataType, &dataTypeArgument, &nextCheckin);
                                Serial.printf("BLE data Length: %i\r\n", BLE_compressed_len);
                                // then we connect to BLE to send the compressed data
                                if (BLE_compressed_len && BLE_connect(BLE_curr_address, BLE_TYPE_ATC_BLE_OEPL)) {
                                    BLE_err_counter = 0;
                                    BLE_curr_part = 0;
                                    memset(BLE_notify_buffer, 0x00, sizeof(BLE_notify_buffer));

                                    uint8_t md5bytes[16];
                                    MD5Builder md5;
                                    md5.begin();
                                    md5.add(BLE_image_buffer, BLE_compressed_len);
                                    md5.calculate();
                                    md5.getBytes(md5bytes);

                                    BLEavaildatainfo.dataType = dataType;
                                    BLEavaildatainfo.dataVer = *((uint64_t*)md5bytes);
                                    BLEavaildatainfo.dataSize = BLE_compressed_len;
                                    BLEavaildatainfo.dataTypeArgument = dataTypeArgument;
                                    BLEavaildatainfo.nextCheckIn = nextCheckin;
                                    BLEavaildatainfo.checksum = 0;
                                    for (uint16_t c = 1; c < sizeof(struct AvailDataInfo); c++) {
                                        BLEavaildatainfo.checksum += (uint8_t)((uint8_t*)&BLEavaildatainfo)[c];
                                    }
                                    BLE_upload_state = BLE_UPLOAD_STATE_INIT;
                                    ble_main_state = BLE_MAIN_STATE_ATC_BLE_OEPL_UPLOAD;
                                    BLE_new_notify = true;  // trigger the upload here
                                } else {
                                    free(BLE_image_buffer);
                                    if (BLE_err_counter++ >= 5) {  // 5 Retries for a BLE Connection
                                        struct espXferComplete reportStruct;
                                        memcpy((uint8_t*)&reportStruct.src, BLE_curr_address, 8);
                                        processXferComplete(&reportStruct, true);
                                        BLE_clear_attempts(BLE_curr_address);
                                    }
                                }
                            }
                        } else {  // This is a Gicisky display
                            // Here we create the compressed buffer
                            BLE_image_buffer = (uint8_t*)malloc(BUFFER_MAX_SIZE_COMPRESSING);
                            if (BLE_image_buffer == nullptr) {
                                Serial.println("BLE Could not create buffer!");
                                BLE_compressed_len = 0;
                            } else {
                                BLE_compressed_len = compress_image(BLE_curr_address, BLE_image_buffer, BUFFER_MAX_SIZE_COMPRESSING);
                                Serial.printf("BLE Compressed Length: %i\r\n", BLE_compressed_len);
                                // then we connect to BLE to send the compressed data
                                if (BLE_compressed_len && BLE_connect(BLE_curr_address, BLE_TYPE_GICISKY)) {
                                    BLE_err_counter = 0;
                                    BLE_curr_part = 0;
                                    memset(BLE_notify_buffer, 0x00, sizeof(BLE_notify_buffer));
                                    BLE_upload_state = BLE_UPLOAD_STATE_INIT;
                                    ble_main_state = BLE_MAIN_STATE_UPLOAD;
                                    BLE_new_notify = true;  // trigger the upload here
                                } else {
                                    free(BLE_image_buffer);
                                    if (BLE_err_counter++ >= 5) {  // 5 Retries for a BLE Connection
                                        struct espXferComplete reportStruct;
                                        memcpy((uint8_t*)&reportStruct.src, BLE_curr_address, 8);
                                        processXferComplete(&reportStruct, true);
                                        BLE_clear_attempts(BLE_curr_address);
                                    }
                                }
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
                            BLE_mini_buff[0] = 1;
                            ctrlChar->writeValue(BLE_mini_buff, 1);
                            break;
                        case BLE_UPLOAD_STATE_SIZE:
                            BLE_mini_buff[0] = 0x02;
                            BLE_mini_buff[1] = BLE_compressed_len & 0xff;
                            BLE_mini_buff[2] = (BLE_compressed_len >> 8) & 0xff;
                            BLE_mini_buff[3] = (BLE_compressed_len >> 16) & 0xff;
                            BLE_mini_buff[4] = (BLE_compressed_len >> 24) & 0xff;
                            BLE_mini_buff[5] = 0x00;
                            ctrlChar->writeValue(BLE_mini_buff, 6);
                            break;
                        case BLE_UPLOAD_STATE_START:
                            BLE_mini_buff[0] = 0x03;
                            ctrlChar->writeValue(BLE_mini_buff, 1);
                            break;
                        case BLE_UPLOAD_STATE_UPLOAD:
                            if (BLE_notify_buffer[2] == 0x08) {
                                free(BLE_image_buffer);
                                pClient->disconnect();
                                ble_main_state = BLE_MAIN_STATE_IDLE;
                                BLE_last_pending_check = millis();
                                // Done and the image is refreshing now
                                struct espXferComplete reportStruct;
                                memcpy((uint8_t*)&reportStruct.src, BLE_curr_address, 8);
                                processXferComplete(&reportStruct, true);
                                BLE_clear_attempts(BLE_curr_address);
                                BLE_err_counter = 0;
                                BLE_curr_part = 0;
                            } else {
                                uint32_t req_curr_part = (BLE_notify_buffer[6] << 24) | (BLE_notify_buffer[5] << 16) | (BLE_notify_buffer[4] << 8) | BLE_notify_buffer[3];
                                if (req_curr_part != BLE_curr_part) {
                                    Serial.printf("Something went wrong, expected req part: %i but got: %i we better abort here.\r\n", req_curr_part, BLE_curr_part);
                                    free(BLE_image_buffer);
                                    pClient->disconnect();
                                    ble_main_state = BLE_MAIN_STATE_IDLE;
                                    BLE_last_pending_check = millis();
                                    break;  // buffer is freed and we left the upload state, do not fall through into a use-after-free
                                }
                                uint32_t curr_len = 240;
                                if (BLE_compressed_len - (BLE_curr_part * 240) < 240)
                                    curr_len = BLE_compressed_len - (BLE_curr_part * 240);
                                BLE_mini_buff[0] = BLE_curr_part & 0xff;
                                BLE_mini_buff[1] = (BLE_curr_part >> 8) & 0xff;
                                BLE_mini_buff[2] = (BLE_curr_part >> 16) & 0xff;
                                BLE_mini_buff[3] = (BLE_curr_part >> 24) & 0xff;
                                memcpy((uint8_t*)&BLE_mini_buff[4], (uint8_t*)&BLE_image_buffer[BLE_curr_part * 240], curr_len);
                                imgChar->writeValue(BLE_mini_buff, curr_len + 4);
                                Serial.printf("BLE sending part: %i\r\n", BLE_curr_part);
                                uint32_t byteOffset = BLE_curr_part * 240;
                                if (byteOffset % BLOCK_DATA_SIZE < 240)  // once per 4096-byte block, same cadence as the 802.15.4 path
                                    BLE_reportProgress("block", byteOffset / BLOCK_DATA_SIZE);
                                uint16_t totalParts = (BLE_compressed_len + 239) / 240;
                                wsSendUploadProgress(BLE_curr_address, BLE_curr_part + 1, totalParts);  // fine-grained part x/total for the tag card
                                BLE_curr_part++;
                            }
                            break;
                    }
                } else {
                    if (millis() - BLE_last_notify > 30000) {  // Something odd, better reset connection!
                        Serial.println("BLE err going back to IDLE");
                        free(BLE_image_buffer);
                        pClient->disconnect();
                        BLE_err_counter = 0;
                        ble_main_state = BLE_MAIN_STATE_IDLE;
                        BLE_last_pending_check = millis();
                    }
                }
                break;
            }
            case BLE_MAIN_STATE_ATC_BLE_OEPL_UPLOAD: {
                if (BLE_connected && BLE_new_notify) {
                    BLE_new_notify = false;
                    BLE_last_notify = millis();
                    switch (BLE_upload_state) {
                        default:
                        case BLE_UPLOAD_STATE_INIT:
                            BLE_mini_buff[0] = 0x00;
                            BLE_mini_buff[1] = 0x64;
                            memcpy((uint8_t*)&BLE_mini_buff[2], &BLEavaildatainfo, sizeof(struct AvailDataInfo));
                            ctrlChar->writeValue(BLE_mini_buff, sizeof(struct AvailDataInfo) + 2);
                            BLE_upload_state = BLE_UPLOAD_STATE_UPLOAD;
                            break;
                        case BLE_UPLOAD_STATE_UPLOAD: {
                            uint8_t notifyLen = BLE_notify_buffer[0];
                            uint16_t notifyCMD = (BLE_notify_buffer[1] << 8) | BLE_notify_buffer[2];
                            Serial.println("BLE CMD " + String(notifyCMD));
                            switch (notifyCMD) {
                                case BLE_CMD_REQ:
                                    if (notifyLen == (sizeof(struct blockRequest) + 2)) {
                                        Serial.println("We got a request for a BLK");
                                        memcpy(&BLEblkRequst, &BLE_notify_buffer[3], sizeof(struct blockRequest));
                                        BLE_curr_part = 0;
                                        ATC_BLE_OEPL_PrepareBlk(BLEblkRequst.blockId);
                                        BLE_reportProgress("block", BLEblkRequst.blockId);
                                        uint16_t totalBlocks = (BLE_compressed_len + BLOCK_DATA_SIZE_BLE - 1) / BLOCK_DATA_SIZE_BLE;
                                        wsSendUploadProgress(BLE_curr_address, BLEblkRequst.blockId + 1, totalBlocks);
                                        ATC_BLE_OEPL_SendPart(BLEblkRequst.blockId, BLE_curr_part);
                                    }
                                    break;
                                case BLE_CMD_ACK_BLKPRT:
                                    BLE_curr_part++;
                                    BLE_err_counter = 0;
                                case BLE_CMD_ERR_BLKPRT:
                                    if (BLE_curr_part <= BLE_max_block_parts && BLE_err_counter++ < 15) {
                                        ATC_BLE_OEPL_SendPart(BLEblkRequst.blockId, BLE_curr_part);
                                        break;
                                    }  // FALLTROUGH!!! We cancel the upload if we land here since we dont have so many parts of a block!
                                case BLE_CMD_ACK:
                                case BLE_CMD_ACK_IS_SHOWN:
                                case BLE_CMD_ACK_FW_UPDATED:
                                    Serial.println("BLE Upload done");
                                    free(BLE_image_buffer);
                                    pClient->disconnect();
                                    ble_main_state = BLE_MAIN_STATE_IDLE;
                                    BLE_last_pending_check = millis();
                                    // Done and the image is refreshing now
                                    struct espXferComplete reportStruct;
                                    memcpy((uint8_t*)&reportStruct.src, BLE_curr_address, 8);
                                    processXferComplete(&reportStruct, true);
                                    BLE_clear_attempts(BLE_curr_address);
                                    BLE_err_counter = 0;
                                    BLE_max_block_parts = 0;
                                    BLE_curr_part = 0;
                                    break;
                            }
                        } break;
                    }
                } else {
                    if (millis() - BLE_last_notify > 30000) {  // Something odd, better reset connection!
                        Serial.println("BLE err going back to IDLE");
                        free(BLE_image_buffer);
                        pClient->disconnect();
                        BLE_err_counter = 0;
                        ble_main_state = BLE_MAIN_STATE_IDLE;
                        BLE_last_pending_check = millis();
                    }
                }
                break;
            }
            case BLE_MAIN_STATE_ATC_BLE_OEPL_CMD: {
                // We sent a single command frame and now wait for the display's
                // acknowledgement notify. Any reply (BLE_CMD_ACK_CMD / BLE_CMD_ACK)
                // means the command was accepted. No image buffer is involved here,
                // so we must never free BLE_image_buffer in this state.
                bool finish = false;
                if (BLE_connected && BLE_new_notify) {
                    BLE_new_notify = false;
                    uint16_t notifyCMD = (BLE_notify_buffer[1] << 8) | BLE_notify_buffer[2];
                    Serial.println("BLE command reply " + String(notifyCMD));
                    finish = true;
                } else if (millis() - BLE_last_notify > 30000) {
                    // A deep-sleeping display stops its BLE stack right after ACK-ing,
                    // so a missed notify is expected there; treat the timeout as done.
                    Serial.println("BLE command timeout, going back to IDLE");
                    finish = true;
                }
                if (finish) {
                    pClient->disconnect();
                    ble_main_state = BLE_MAIN_STATE_IDLE;
                    BLE_last_pending_check = millis();
                    // The display stops advertising after a deep sleep; show it as sleeping
                    // until it is found in a scan again (see processDataReq wake handling).
                    BLE_finishCommand(BLE_cmd_isDeepsleep);
                    BLE_cmd_isDeepsleep = false;
                }
                break;
            }
        }
        vTaskDelay(15 / portTICK_PERIOD_MS);
    }
}

#endif
